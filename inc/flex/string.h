///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements a basic_string class, much like the C++ std::basic_string.
// The primary distinctions between basic_string and std::basic_string are:
//    - basic_string has a few extension functions that allow for increased performance.
//    - basic_string has a few extension functions that make use easier,
//      such as member tolower/toupper functions.
//    - basic_string supports debug memory naming natively.
//    - basic_string is easier to read, debug, and visualize.
//    - basic_string internally manually expands basic functions such as begin(),
//      size(), etc. in order to improve debug performance and optimizer success.
//    - basic_string is savvy to an environment that doesn't have exception handling,
//      as is sometimes the case with console or embedded environments.
//    - basic_string has less deeply nested function calls and allows the user to
//      enable forced inlining in debug builds in order to reduce bloat.
//    - basic_string doesn't use char traits. As a result, EASTL assumes that
//      strings will hold characters and not exotic things like widgets. At the
//      very least, basic_string assumes that the value_type is a POD.
//    - basic_string data is guaranteed to be contiguous.
//    - basic_string data is guaranteed to be 0-terminated, and the c_str() function
//      is guaranteed to return the same pointer as the data() which is guaranteed
//      to be the same value as &string[0].
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Copy on Write (cow)
//
// This string implementation does not do copy on write (cow). This is by design,
// as cow penalizes 95% of string uses for the benefit of only 5% of the uses
// (these percentages are qualitative, not quantitative). The primary benefit of
// cow is that it allows for the sharing of string data between two string objects.
// Thus if you say this:
//    string a("hello");
//    string b(a);
// the "hello" will be shared between a and b. If you then say this:
//    a = "world";
// then a will release its reference to "hello" and leave b with the only reference
// to it. Normally this functionality is accomplished via reference counting and
// with atomic operations or mutexes.
//
// The C++ standard does not say anything about basic_string and cow. However,
// for a basic_string implementation to be standards-conforming, a number of
// issues arise which dictate some things about how one would have to implement
// a cow string. The discussion of these issues will not be rehashed here, as you
// can read the references below for better detail than can be provided in the
// space we have here. However, we can say that the C++ standard is sensible and
// that anything we try to do here to allow for an efficient cow implementation
// would result in a generally unacceptable string interface.
//
// The disadvantages of cow strings are:
//    - A reference count needs to exist with the string, which increases string memory usage.
//    - With thread safety, atomic operations and mutex locks are expensive, especially
//      on weaker memory systems such as console gaming platforms.
//    - All non-const string accessor functions need to do a sharing check the the
//      first such check needs to detach the string. Similarly, all string assignments
//      need to do a sharing check as well. If you access the string before doing an
//      assignment, the assignment doesn't result in a shared string, because the string
//      has already been detached.
//    - String sharing doesn't happen the large majority of the time. In some cases,
//      the total sum of the reference count memory can exceed any memory savings
//      gained by the strings that share representations.
//
// The addition of a string_cow class is under consideration for this library.
// There are conceivably some systems which have string usage patterns which would
// benefit from cow sharing. Such functionality is best saved for a separate string
// implementation so that the other string uses aren't penalized.
//
// References:
//    This is a good starting HTML reference on the topic:
//       http://www.gotw.ca/publications/optimizations.htm
//    Here is a Usenet discussion on the topic:
//       http://groups-beta.google.com/group/comp.lang.c++.moderated/browse_thread/thread/3dc6af5198d0bf7/886c8642cb06e03d
//
///////////////////////////////////////////////////////////////////////////////

#ifndef FLEX_STRING_H
#define FLEX_STRING_H

#include <algorithm>
#include <iterator>
#include <flex/allocator.h>

#include <stddef.h>             // size_t, ptrdiff_t, etc.
#include <stdlib.h>             // malloc, free.
#include <ctype.h>              // toupper, etc.
#include <wchar.h>

#include <stdint.h>
#include <uchar.h>
typedef char char8_t;

#include <string.h> // strlen, etc.

///////////////////////////////////////////////////////////////////////////////
// EASTL_STRING_INITIAL_CAPACITY
//
// As of this writing, this must be > 0. Note that an initially empty string
// has a capacity of zero (it allocates no memory).
//
const size_t EASTL_STRING_INITIAL_CAPACITY = 8;
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

namespace flex
{

/// EASTL_BASIC_STRING_DEFAULT_NAME
///
/// Defines a default container name in the absence of a user-provided name.
///
#ifndef EASTL_BASIC_STRING_DEFAULT_NAME
#define EASTL_BASIC_STRING_DEFAULT_NAME EASTL_DEFAULT_NAME_PREFIX " basic_string" // Unless the user overrides something, this is "EASTL basic_string".
#endif

/// EASTL_BASIC_STRING_DEFAULT_ALLOCATOR
///
#ifndef EASTL_BASIC_STRING_DEFAULT_ALLOCATOR
#define EASTL_BASIC_STRING_DEFAULT_ALLOCATOR allocator_type(EASTL_BASIC_STRING_DEFAULT_NAME)
#endif

///////////////////////////////////////////////////////////////////////////////
/// basic_string
///
/// Implements a templated string class, somewhat like C++ std::basic_string.
///
/// Notes:
///     As of this writing, an insert of a string into itself necessarily
///     triggers a reallocation, even if there is enough capacity in self
///     to handle the increase in size. This is due to the slightly tricky
///     nature of the operation of modifying one's self with one's self,
///     and thus the source and destination are being modified during the
///     operation. It might be useful to rectify this to the extent possible.
///
///     Our usage of noexcept specifiers is a little different from the
///     requirements specified by std::basic_string in C++11. This is because
///     our allocators are instances and not types and thus can be non-equal
///     and result in exceptions during assignments that theoretically can't
///     occur with std containers.
///
  template<typename T, typename Allocator = flex::allocator<T> >
  class basic_string: public guarded_object
  {
  public:
    typedef basic_string<T, Allocator> this_type;
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator; // Maintainer note: We want to leave iterator defined as T* -- at least in release builds -- as this gives some algorithms an advantage that optimizers cannot get around.
    typedef const T* const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef size_t size_type; // See config.h for the definition of eastl_size_t, which defaults to uint32_t.
    typedef ptrdiff_t difference_type;
    typedef Allocator allocator_type;

    static const size_type npos = (size_type) -1; /// 'npos' means non-valid position or simply non-position.
    static const size_type kMaxSize = (size_type) -2; /// -1 is reserved for 'npos'. It also happens to be slightly beneficial that kMaxSize is a value less than -1, as it helps us deal with potential integer wraparound issues.

    /// sEmptyString
    ///
    /// Declares a shared terminating 0 representation for scalar strings that are empty.
    ///
    union EmptyString
    {
      uint32_t mUint32;
      char mEmpty8[1];
      unsigned char mEmptyU8[1];
      signed char mEmptyS8[1];
      char16_t mEmpty16[1];
      char32_t mEmpty32[1];
    };

    static EmptyString sEmptyString;

    inline static const signed char* GetEmptyString(signed char)
    {
      return sEmptyString.mEmptyS8;
    }
    inline static const unsigned char* GetEmptyString(unsigned char)
    {
      return sEmptyString.mEmptyU8;
    }
    inline static const char* GetEmptyString(char)
    {
      return sEmptyString.mEmpty8;
    }
    inline static const char16_t* GetEmptyString(char16_t)
    {
      return sEmptyString.mEmpty16;
    }
    inline static const char32_t* GetEmptyString(char32_t)
    {
      return sEmptyString.mEmpty32;
    }

  public:
    // CtorDoNotInitialize exists so that we can create a constructor that allocates but doesn't
    // initialize and also doesn't collide with any other constructor declaration.
    struct CtorDoNotInitialize
    {
    };

  protected:
    value_type* mpBegin; // Begin of string.
    value_type* mpEnd; // End of string. *mpEnd is always '0', as we 0-terminate our string. mpEnd is always < mpCapacity.
    value_type* mpCapacity; // End of allocated space, including the space needed to store the trailing '0' char. mpCapacity is always at least mpEnd + 1. To consider: rename this to mpAllocEnd, thus avoiding confusion with the public capacity() function.
    allocator_type mAllocator; // To do: Use base class optimization to make this go away.

  public:
    // Constructor, destructor
    basic_string();
    explicit basic_string(const allocator_type& allocator);
    basic_string(const this_type& x, size_type position, size_type n = npos);
    basic_string(const value_type* p, size_type n, const allocator_type& allocator = allocator_type());
    basic_string(const value_type* p, const allocator_type& allocator = allocator_type());
    basic_string(size_type n, value_type c, const allocator_type& allocator = allocator_type());
    basic_string(const this_type& x);
    template<class InputIterator> basic_string(InputIterator pBegin, InputIterator pEnd,
        const allocator_type& allocator = allocator_type());
    basic_string(CtorDoNotInitialize, size_type n, const allocator_type& allocator = allocator_type());
    basic_string(std::initializer_list<value_type> init, const allocator_type& allocator = allocator_type());

#if FLEX_HAS_CXX11
    basic_string(this_type&& x);
    basic_string(this_type&& x, const allocator_type& allocator);
#endif

    ~basic_string();

    // Allocator
    const allocator_type& get_allocator() const FLEX_NOEXCEPT;
    allocator_type& get_allocator() FLEX_NOEXCEPT;
    void set_allocator(const allocator_type& allocator);

    // Operator =
    this_type& operator=(const this_type& x);
    this_type& operator=(const value_type* p);
    this_type& operator=(value_type c);
    this_type& operator=(std::initializer_list<value_type> ilist);

#if FLEX_HAS_CXX11
    this_type& operator=(this_type&& x);
#endif

    void swap(this_type& x);

    // Assignment operations
    this_type& assign(const this_type& x);
    this_type& assign(const this_type& x, size_type position, size_type n);
    this_type& assign(const value_type* p, size_type n);
    this_type& assign(const value_type* p);
    this_type& assign(size_type n, value_type c);
    this_type& assign(const value_type* pBegin, const value_type* pEnd);
#if FLEX_HAS_CXX11
    this_type& assign(this_type&& x);
#endif
    this_type& assign(std::initializer_list<value_type>);

    template<typename OtherCharType>
    this_type& assign_convert(const OtherCharType* p);

    template<typename OtherCharType>
    this_type& assign_convert(const OtherCharType* p, size_type n);

    template<typename OtherStringType>
    this_type& assign_convert(const OtherStringType& x);

    // Iterators.
    iterator begin() FLEX_NOEXCEPT; // Expanded in source code as: mpBegin
    const_iterator begin() const FLEX_NOEXCEPT; // Expanded in source code as: mpBegin
    const_iterator cbegin() const FLEX_NOEXCEPT;

    iterator end() FLEX_NOEXCEPT; // Expanded in source code as: mpEnd
    const_iterator end() const FLEX_NOEXCEPT; // Expanded in source code as: mpEnd
    const_iterator cend() const FLEX_NOEXCEPT;

    reverse_iterator rbegin() FLEX_NOEXCEPT;
    const_reverse_iterator rbegin() const FLEX_NOEXCEPT;
    const_reverse_iterator crbegin() const FLEX_NOEXCEPT;

    reverse_iterator rend() FLEX_NOEXCEPT;
    const_reverse_iterator rend() const FLEX_NOEXCEPT;
    const_reverse_iterator crend() const FLEX_NOEXCEPT;

    // Size-related functionality
    bool empty() const FLEX_NOEXCEPT; // Expanded in source code as: (mpBegin == mpEnd) or (mpBegin != mpEnd)
    size_type size() const FLEX_NOEXCEPT; // Expanded in source code as: (size_type)(mpEnd - mpBegin)
    size_type length() const FLEX_NOEXCEPT; // Expanded in source code as: (size_type)(mpEnd - mpBegin)
    size_type max_size() const FLEX_NOEXCEPT; // Expanded in source code as: kMaxSize
    size_type capacity() const FLEX_NOEXCEPT; // Expanded in source code as: (size_type)((mpCapacity - mpBegin) - 1). Thus thus returns the max strlen the container can currently hold without resizing.
    void resize(size_type n, value_type c);
    void resize(size_type n);
    void reserve(size_type = 0);
    void set_capacity(size_type n = npos); // Revises the capacity to the user-specified value. Resizes the container to match the capacity if the requested capacity n is less than the current size. If n == npos then the capacity is reallocated (if necessary) such that capacity == size.
    void shrink_to_fit();

    // Raw access
    const value_type* data() const FLEX_NOEXCEPT;
    const value_type* c_str() const FLEX_NOEXCEPT;

    // Element access
    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    reference at(size_type n);
    const_reference at(size_type n) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // Append operations
    this_type& operator+=(const this_type& x);
    this_type& operator+=(const value_type* p);
    this_type& operator+=(value_type c);

    this_type& append(const this_type& x);
    this_type& append(const this_type& x, size_type position, size_type n);
    this_type& append(const value_type* p, size_type n);
    this_type& append(const value_type* p);
    this_type& append(size_type n, value_type c);
    this_type& append(const value_type* pBegin, const value_type* pEnd);

    template<typename OtherCharType>
    this_type& append_convert(const OtherCharType* p);

    template<typename OtherCharType>
    this_type& append_convert(const OtherCharType* p, size_type n);

    template<typename OtherStringType>
    this_type& append_convert(const OtherStringType& x);

    void push_back(value_type c);
    void pop_back();

    // Insertion operations
    this_type& insert(size_type position, const this_type& x);
    this_type& insert(size_type position, const this_type& x, size_type beg, size_type n);
    this_type& insert(size_type position, const value_type* p, size_type n);
    this_type& insert(size_type position, const value_type* p);
    this_type& insert(size_type position, size_type n, value_type c);
    iterator insert(const_iterator p, value_type c);
    iterator insert(const_iterator p, size_type n, value_type c);
    iterator insert(const_iterator p, const value_type* pBegin, const value_type* pEnd);
    iterator insert(const_iterator p, std::initializer_list<value_type>);

    // Erase operations
    this_type& erase(size_type position = 0, size_type n = npos);
    iterator erase(const_iterator p);
    iterator erase(const_iterator pBegin, const_iterator pEnd);
    reverse_iterator erase(reverse_iterator position);
    reverse_iterator erase(reverse_iterator first, reverse_iterator last);
    void clear() FLEX_NOEXCEPT;
    void reset_lose_memory() FLEX_NOEXCEPT; // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

    //Replacement operations
    this_type& replace(size_type position, size_type n, const this_type& x);
    this_type& replace(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2);
    this_type& replace(size_type position, size_type n1, const value_type* p, size_type n2);
    this_type& replace(size_type position, size_type n1, const value_type* p);
    this_type& replace(size_type position, size_type n1, size_type n2, value_type c);
    this_type& replace(const_iterator first, const_iterator last, const this_type& x);
    this_type& replace(const_iterator first, const_iterator last, const value_type* p, size_type n);
    this_type& replace(const_iterator first, const_iterator last, const value_type* p);
    this_type& replace(const_iterator first, const_iterator last, size_type n, value_type c);
    this_type& replace(const_iterator first, const_iterator last, const value_type* pBegin, const value_type* pEnd);
    size_type copy(value_type* p, size_type n, size_type position = 0) const;

    // Find operations
    size_type find(const this_type& x, size_type position = 0) const FLEX_NOEXCEPT;
    size_type find(const value_type* p, size_type position = 0) const;
    size_type find(const value_type* p, size_type position, size_type n) const;
    size_type find(value_type c, size_type position = 0) const FLEX_NOEXCEPT;

    // Reverse find operations
    size_type rfind(const this_type& x, size_type position = npos) const FLEX_NOEXCEPT;
    size_type rfind(const value_type* p, size_type position = npos) const;
    size_type rfind(const value_type* p, size_type position, size_type n) const;
    size_type rfind(value_type c, size_type position = npos) const FLEX_NOEXCEPT;

    // Find first-of operations
    size_type find_first_of(const this_type& x, size_type position = 0) const FLEX_NOEXCEPT;
    size_type find_first_of(const value_type* p, size_type position = 0) const;
    size_type find_first_of(const value_type* p, size_type position, size_type n) const;
    size_type find_first_of(value_type c, size_type position = 0) const FLEX_NOEXCEPT;

    // Find last-of operations
    size_type find_last_of(const this_type& x, size_type position = npos) const FLEX_NOEXCEPT;
    size_type find_last_of(const value_type* p, size_type position = npos) const;
    size_type find_last_of(const value_type* p, size_type position, size_type n) const;
    size_type find_last_of(value_type c, size_type position = npos) const FLEX_NOEXCEPT;

    // Find first not-of operations
    size_type find_first_not_of(const this_type& x, size_type position = 0) const FLEX_NOEXCEPT;
    size_type find_first_not_of(const value_type* p, size_type position = 0) const;
    size_type find_first_not_of(const value_type* p, size_type position, size_type n) const;
    size_type find_first_not_of(value_type c, size_type position = 0) const FLEX_NOEXCEPT;

    // Find last not-of operations
    size_type find_last_not_of(const this_type& x, size_type position = npos) const FLEX_NOEXCEPT;
    size_type find_last_not_of(const value_type* p, size_type position = npos) const;
    size_type find_last_not_of(const value_type* p, size_type position, size_type n) const;
    size_type find_last_not_of(value_type c, size_type position = npos) const FLEX_NOEXCEPT;

    // Substring functionality
    this_type substr(size_type position = 0, size_type n = npos) const;

    // Comparison operations
    int compare(const this_type& x) const FLEX_NOEXCEPT;
    int compare(size_type pos1, size_type n1, const this_type& x) const;
    int compare(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2) const;
    int compare(const value_type* p) const;
    int compare(size_type pos1, size_type n1, const value_type* p) const;
    int compare(size_type pos1, size_type n1, const value_type* p, size_type n2) const;
    static int compare(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2,
        const value_type* pEnd2);

    // Misc functionality, not part of C++ this_type.
    void make_lower();
    void make_upper();
    void ltrim();
    void rtrim();
    void trim();
    this_type left(size_type n) const;
    this_type right(size_type n) const;

    bool validate() const FLEX_NOEXCEPT;

  protected:
    // Helper functions for initialization/insertion operations.
    value_type* DoAllocate(size_type n);
    void DoFree(value_type* p, size_type n);
    size_type GetNewCapacity(size_type currentCapacity);
    void AllocateSelf();
    void AllocateSelf(size_type n);
    void DeallocateSelf();
    iterator InsertInternal(const_iterator p, value_type c);
    template<class InputIterator> void RangeInitialize(InputIterator pBegin, InputIterator pEnd);
    void RangeInitialize(const value_type* pBegin);
    void SizeInitialize(size_type n, value_type c);

    void ThrowLengthException() const;
    void ThrowRangeException() const;
    void ThrowInvalidArgumentException() const;

    // Replacements for STL template functions.
    static const value_type* CharTypeStringFindEnd(const value_type* pBegin, const value_type* pEnd, value_type c);
    static const value_type* CharTypeStringRFind(const value_type* pRBegin, const value_type* pREnd,
        const value_type c);
    static const value_type* CharTypeStringSearch(const value_type* p1Begin, const value_type* p1End,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringRSearch(const value_type* p1Begin, const value_type* p1End,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringFindFirstOf(const value_type* p1Begin, const value_type* p1End,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringRFindFirstOf(const value_type* p1RBegin, const value_type* p1REnd,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringFindFirstNotOf(const value_type* p1Begin, const value_type* p1End,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringRFindFirstNotOf(const value_type* p1RBegin, const value_type* p1REnd,
        const value_type* p2Begin, const value_type* p2End);

  };
// basic_string

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::EmptyString basic_string<T, Allocator>::sEmptyString = { 0 };

///////////////////////////////////////////////////////////////////////////////
// 'char traits' functionality
//
  inline char8_t CharToLower(char8_t c)
  {
    return (char8_t) tolower((uint8_t) c);
  }

  inline char16_t CharToLower(char16_t c)
  {
    if ((unsigned) c <= 0xff)
      return (char16_t) tolower((uint8_t) c);
    return c;
  }

  inline char32_t CharToLower(char32_t c)
  {
    if ((unsigned) c <= 0xff)
      return (char32_t) tolower((uint8_t) c);
    return c;
  }

  inline char8_t CharToUpper(char8_t c)
  {
    return (char8_t) toupper((uint8_t) c);
  }

  inline char16_t CharToUpper(char16_t c)
  {
    if ((unsigned) c <= 0xff)
      return (char16_t) toupper((uint8_t) c);
    return c;
  }

  inline char32_t CharToUpper(char32_t c)
  {
    if ((unsigned) c <= 0xff)
      return (char32_t) toupper((uint8_t) c);
    return c;
  }

#ifdef FLEX_HAS_CXX11
  template<typename T>
  int Compare(const T* p1, const T* p2, size_t n)
  {
    for (; n > 0; ++p1, ++p2, --n)
    {
      if (*p1 != *p2)
      return (static_cast<typename std::make_unsigned<T>::type>(*p1)
          < static_cast<typename std::make_unsigned<T>::type>(*p2)) ? -1 : 1;
    }
    return 0;
  }
#endif

  inline int Compare(const char8_t* p1, const char8_t* p2, size_t n)
  {
    return memcmp(p1, p2, n);
  }

  inline const char8_t* Find(const char8_t* p, char8_t c, size_t n)
  {
    return (const char8_t*) memchr(p, c, n);
  }

  inline const char16_t* Find(const char16_t* p, char16_t c, size_t n)
  {
    for (; n > 0; --n, ++p)
    {
      if (*p == c)
        return p;
    }

    return NULL;
  }

  inline const char32_t* Find(const char32_t* p, char32_t c, size_t n)
  {
    for (; n > 0; --n, ++p)
    {
      if (*p == c)
        return p;
    }

    return NULL;
  }

  inline size_t CharStrlen(const char8_t* p)
  {
#ifdef _MSC_VER // VC++ can implement an instrinsic here.
    return strlen(p);
#else
    const char8_t* pCurrent = p;
    while (*pCurrent)
      ++pCurrent;
    return (size_t) (pCurrent - p);
#endif
  }

  inline size_t CharStrlen(const char16_t* p)
  {
    const char16_t* pCurrent = p;
    while (*pCurrent)
      ++pCurrent;
    return (size_t) (pCurrent - p);
  }

  inline size_t CharStrlen(const char32_t* p)
  {
    const char32_t* pCurrent = p;
    while (*pCurrent)
      ++pCurrent;
    return (size_t) (pCurrent - p);
  }

  template<typename T>
  inline T* CharStringUninitializedCopy(const T* pSource, const T* pSourceEnd, T* pDestination)
  {
    memmove(pDestination, pSource, (size_t) (pSourceEnd - pSource) * sizeof(T));
    return pDestination + (pSourceEnd - pSource);
  }

  inline char8_t* CharStringUninitializedFillN(char8_t* pDestination, size_t n, const char8_t c)
  {
    if (n) // Some compilers (e.g. GCC 4.3+) generate a warning (which can't be disabled) if you call memset with a size of 0.
      memset(pDestination, (uint8_t) c, (size_t) n);
    return pDestination + n;
  }

  inline char16_t* CharStringUninitializedFillN(char16_t* pDestination, size_t n, const char16_t c)
  {
    char16_t* pDest16 = pDestination;
    const char16_t* const pEnd = pDestination + n;
    while (pDest16 < pEnd)
      *pDest16++ = c;
    return pDestination + n;
  }

  inline char32_t* CharStringUninitializedFillN(char32_t* pDestination, size_t n, const char32_t c)
  {
    char32_t* pDest32 = pDestination;
    const char32_t* const pEnd = pDestination + n;
    while (pDest32 < pEnd)
      *pDest32++ = c;
    return pDestination + n;
  }

  inline char8_t* CharTypeAssignN(char8_t* pDestination, size_t n, char8_t c)
  {
    if (n) // Some compilers (e.g. GCC 4.3+) generate a warning (which can't be disabled) if you call memset with a size of 0.
      return (char8_t*) memset(pDestination, c, (size_t) n);
    return pDestination;
  }

  inline char16_t* CharTypeAssignN(char16_t* pDestination, size_t n, char16_t c)
  {
    char16_t* pDest16 = pDestination;
    const char16_t* const pEnd = pDestination + n;
    while (pDest16 < pEnd)
      *pDest16++ = c;
    return pDestination;
  }

  inline char32_t* CharTypeAssignN(char32_t* pDestination, size_t n, char32_t c)
  {
    char32_t* pDest32 = pDestination;
    const char32_t* const pEnd = pDestination + n;
    while (pDest32 < pEnd)
      *pDest32++ = c;
    return pDestination;
  }

///////////////////////////////////////////////////////////////////////////////
// basic_string
///////////////////////////////////////////////////////////////////////////////

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string() :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator()
  {
    AllocateSelf();
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const allocator_type& allocator) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(allocator)
  {
    AllocateSelf();
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const this_type& x) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(x.mAllocator)
  {
    RangeInitialize(x.mpBegin, x.mpEnd);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(const this_type& x, size_type position, size_type n) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(x.mAllocator)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(x.mpEnd - x.mpBegin))) // 21.4.2 p4
    {
      ThrowRangeException();
      AllocateSelf();
    }
    else
    RangeInitialize(x.mpBegin + position, x.mpBegin + position + std::min(n, (size_type)(x.mpEnd - x.mpBegin) - position));
#else
    RangeInitialize(x.mpBegin + position,
        x.mpBegin + position + std::min(n, (size_type) (x.mpEnd - x.mpBegin) - position));
#endif
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const value_type* p, size_type n, const allocator_type& allocator) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(allocator)
  {
    RangeInitialize(p, p + n);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const value_type* p, const allocator_type& allocator) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(allocator)
  {
    RangeInitialize(p);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(size_type n, value_type c, const allocator_type& allocator) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(allocator)
  {
    SizeInitialize(n, c);
  }

  template<typename T, typename Allocator>
  template<class InputIterator>
  inline basic_string<T, Allocator>::basic_string(InputIterator pBegin, InputIterator pEnd,
      const allocator_type& allocator) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(allocator)
  {
    RangeInitialize(pBegin, pEnd);
  }

// CtorDoNotInitialize exists so that we can create a version that allocates but doesn't
// initialize but also doesn't collide with any other constructor declaration.
  template<typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(CtorDoNotInitialize /*unused*/, size_type n, const allocator_type& allocator) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(allocator)
  {
    // Note that we do not call SizeInitialize here.
    AllocateSelf(n + 1); // '+1' so that we have room for the terminating 0.
    *mpEnd = 0;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(std::initializer_list<value_type> init, const allocator_type& allocator) :
      mpBegin(NULL), mpEnd(NULL), mpCapacity(NULL), mAllocator(allocator)
  {
    RangeInitialize(init.begin(), init.end());
  }

#if FLEX_HAS_CXX11
  template <typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(this_type&& x)
  : mpBegin(x.mpBegin),
  mpEnd(x.mpEnd),
  mpCapacity(x.mpCapacity),
  mAllocator(x.mAllocator)
  {
    x.AllocateSelf();
  }

  template <typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(this_type&& x, const allocator_type& allocator)
  : mpBegin(NULL),
  mpEnd(NULL),
  mpCapacity(NULL),
  mAllocator(allocator)
  {
    if(mAllocator == x.mAllocator) // If we can borrow from x...
    {
      mpBegin = x.mpBegin; // It's OK if x.mpBegin is NULL.
      mpEnd = x.mpEnd;
      mpCapacity = x.mpCapacity;
      x.AllocateSelf();
    }
    else if(x.mpBegin)
    {
      RangeInitialize(x.mpBegin, x.mpEnd);
      // Let x destruct its own items.
    }
  }
#endif

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::~basic_string()
  {
    DeallocateSelf();
  }

  template<typename T, typename Allocator>
  inline const typename basic_string<T, Allocator>::allocator_type&
  basic_string<T, Allocator>::get_allocator() const FLEX_NOEXCEPT
  {
    return mAllocator;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::allocator_type&
  basic_string<T, Allocator>::get_allocator() FLEX_NOEXCEPT
  {
    return mAllocator;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::set_allocator(const allocator_type& allocator)
  {
    mAllocator = allocator;
  }

  template<typename T, typename Allocator>
  inline const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::data() const FLEX_NOEXCEPT
  {
    return mpBegin;
  }

  template<typename T, typename Allocator>
  inline const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::c_str() const FLEX_NOEXCEPT
  {
    return mpBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::begin() FLEX_NOEXCEPT
  {
    return mpBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::end() FLEX_NOEXCEPT
  {
    return mpEnd;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::begin() const FLEX_NOEXCEPT
  {
    return mpBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::cbegin() const FLEX_NOEXCEPT
  {
    return mpBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::end() const FLEX_NOEXCEPT
  {
    return mpEnd;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::cend() const FLEX_NOEXCEPT
  {
    return mpEnd;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reverse_iterator basic_string<T, Allocator>::rbegin() FLEX_NOEXCEPT
  {
    return reverse_iterator(mpEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reverse_iterator basic_string<T, Allocator>::rend() FLEX_NOEXCEPT
  {
    return reverse_iterator(mpBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::rbegin() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mpEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::crbegin() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mpEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::rend() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mpBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::crend() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mpBegin);
  }

  template<typename T, typename Allocator>
  inline bool basic_string<T, Allocator>::empty() const FLEX_NOEXCEPT
  {
    return (mpBegin == mpEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::size() const FLEX_NOEXCEPT
  {
    return (size_type) (mpEnd - mpBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::length() const FLEX_NOEXCEPT
  {
    return (size_type) (mpEnd - mpBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::max_size() const FLEX_NOEXCEPT
  {
    return kMaxSize;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::capacity() const FLEX_NOEXCEPT
  {
    return (size_type) ((mpCapacity - mpBegin) - 1); // '-1' because we pretend that we didn't allocate memory for the terminating 0.
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::operator[](size_type n) const
  {
#if EASTL_ASSERT_ENABLED // We allow the user to reference the trailing 0 char without asserting. Perhaps we shouldn't.
    if(FLEX_UNLIKELY(n > (static_cast<size_type>(mpEnd - mpBegin))))
    printf("basic_string::operator[] -- out of range");
#endif

    return mpBegin[n]; // Sometimes done as *(mpBegin + n)
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::operator[](size_type n)
  {
#if EASTL_ASSERT_ENABLED // We allow the user to reference the trailing 0 char without asserting. Perhaps we shouldn't.
    if(FLEX_UNLIKELY(n > (static_cast<size_type>(mpEnd - mpBegin))))
    printf("basic_string::operator[] -- out of range");
#endif

    return mpBegin[n]; // Sometimes done as *(mpBegin + n)
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(const this_type& x)
  {
    if (&x != this)
    {
      assign(x.mpBegin, x.mpEnd);
    }
    return *this;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(const value_type* p)
  {
    return assign(p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(value_type c)
  {
    return assign((size_type) 1, c);
  }

#if FLEX_HAS_CXX11
  template <typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(this_type&& x)
  {
    return assign(std::move(x));
  }
#endif

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(
      std::initializer_list<value_type> ilist)
  {
    return assign(ilist.begin(), ilist.end());
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::resize(size_type n, value_type c)
  {
    const size_type s = (size_type) (mpEnd - mpBegin);

    if (n < s)
      erase(mpBegin + n, mpEnd);
    else if (n > s)
      append(n - s, c);
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::resize(size_type n)
  {
    const size_type s = (size_type) (mpEnd - mpBegin);

    if (n < s)
      erase(mpBegin + n, mpEnd);
    else if (n > s)
    {
      append(n - s, value_type());
    }
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::reserve(size_type n)
  {
#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY(n > kMaxSize))
    ThrowLengthException();
#endif

    // The C++ standard for basic_string doesn't specify if we should or shouldn't
    // downsize the container. The standard is overly vague in its description of reserve:
    //    The member function reserve() is a directive that informs a
    //    basic_string object of a planned change in size, so that it
    //    can manage the storage allocation accordingly.
    // We will act like the vector container and preserve the contents of
    // the container and only reallocate if increasing the size. The user
    // can use the set_capacity function to reduce the capacity.

    n = std::max(n, (size_type) (mpEnd - mpBegin)); // Calculate the new capacity, which needs to be >= container size.

    if (n >= (size_type) (mpCapacity - mpBegin)) // If there is something to do... // We use >= because mpCapacity accounts for the trailing zero.
      set_capacity(n);
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::set_capacity(size_type n)
  {
    if (n == npos) // If the user wants to set the capacity to equal the current size... // '-1' because we pretend that we didn't allocate memory for the terminating 0.
      n = (size_type) (mpEnd - mpBegin);
    else if (n < (size_type) (mpEnd - mpBegin))
      mpEnd = mpBegin + n;

    if (n != (size_type) ((mpCapacity - mpBegin) - 1)) // If there is any capacity change...
    {
      if (n)
      {
        pointer pNewBegin = DoAllocate(n + 1); // We need the + 1 to accomodate the trailing 0.
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mpBegin, mpEnd, pNewBegin);
        *pNewEnd = 0;

        DeallocateSelf();
        mpBegin = pNewBegin;
        mpEnd = pNewEnd;
        mpCapacity = pNewBegin + (n + 1);
      }
      else
      {
        DeallocateSelf();
        AllocateSelf();
      }
    }
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::shrink_to_fit()
  {
    size_type n = size();
    if (n != capacity()) // If there is any capacity change...
    {
      if (n)
      {
        pointer pNewBegin = DoAllocate(n + 1); // We need the + 1 to accomodate the trailing 0.
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mpBegin, mpEnd, pNewBegin);
        *pNewEnd = 0;

        DeallocateSelf();
        mpBegin = pNewBegin;
        mpEnd = pNewEnd;
        mpCapacity = pNewBegin + (n + 1);
      }
      else
      {
        DeallocateSelf();
        AllocateSelf();
      }
    }
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::clear() FLEX_NOEXCEPT
  {
    if (mpBegin != mpEnd)
    {
      *mpBegin = value_type(0);
      mpEnd = mpBegin;
    }
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::reset_lose_memory() FLEX_NOEXCEPT
  {
    // The reset function is a special extension function which unilaterally
    // resets the container to an empty state without freeing the memory of
    // the contained objects. This is useful for very quickly tearing down a
    // container built into scratch memory.
    AllocateSelf();
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::at(size_type n) const
  {
    if (FLEX_UNLIKELY(n >= (size_type ) (mpEnd - mpBegin)))
    {
      throw std::out_of_range("flex::string.at() - index out-of-bounds");
    }
    else
    {
      return mpBegin[n];
    }
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::at(size_type n)
  {
    if (FLEX_UNLIKELY(n >= (size_type ) (mpEnd - mpBegin)))
    {
      throw std::out_of_range("flex::string.at() - index out-of-bounds");
    }
    else
    {
      return mpBegin[n];
    }
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::front()
  {
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
    // We allow the user to reference the trailing 0 char without asserting.
#elif EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(mpEnd <= mpBegin)) // We assert if the user references the trailing 0 char.
    printf("basic_string::front -- empty string");
#endif

    return *mpBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::front() const
  {
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
    // We allow the user to reference the trailing 0 char without asserting.
#elif EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(mpEnd <= mpBegin)) // We assert if the user references the trailing 0 char.
    printf("basic_string::front -- empty string");
#endif

    return *mpBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::back()
  {
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
    // We allow the user to reference the trailing 0 char without asserting.
#elif EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(mpEnd <= mpBegin)) // We assert if the user references the trailing 0 char.
    printf("basic_string::back -- empty string");
#endif

    return *(mpEnd - 1);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::back() const
  {
#if EASTL_EMPTY_REFERENCE_ASSERT_ENABLED
    // We allow the user to reference the trailing 0 char without asserting.
#elif EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(mpEnd <= mpBegin)) // We assert if the user references the trailing 0 char.
    printf("basic_string::back -- empty string");
#endif

    return *(mpEnd - 1);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(const this_type& x)
  {
    return append(x);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(const value_type* p)
  {
    return append(p);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(value_type c)
  {
    push_back(c);
    return *this;
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const this_type& x)
  {
    return append(x.mpBegin, x.mpEnd);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const this_type& x, size_type position,
      size_type n)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(x.mpEnd - x.mpBegin))) // position must be < x.mpEnd, but position + n may be > mpEnd.
    ThrowRangeException();
#endif

    return append(x.mpBegin + position,
        x.mpBegin + position + std::min(n, (size_type) (x.mpEnd - x.mpBegin) - position));
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const value_type* p, size_type n)
  {
    return append(p, p + n);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const value_type* p)
  {
    return append(p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  template<typename OtherCharType>
  basic_string<T, Allocator>& basic_string<T, Allocator>::append_convert(const OtherCharType* pOther)
  {
    return append_convert(pOther, (size_type) CharStrlen(pOther));
  }

  template<typename T, typename Allocator>
  template<typename OtherStringType>
  basic_string<T, Allocator>& basic_string<T, Allocator>::append_convert(const OtherStringType& x)
  {
    return append_convert(x, x.length());
  }

  template<typename T, typename Allocator>
  template<typename OtherCharType>
  basic_string<T, Allocator>& basic_string<T, Allocator>::append_convert(const OtherCharType* pOther, size_type n)
  {
    // Question: What do we do in the case that we have an illegally encoded source string?
    // This can happen with UTF8 strings. Do we throw an exception or do we ignore the input?
    // One argument is that it's not a string class' job to handle the security aspects of a
    // program and the higher level application code should be verifying UTF8 string validity,
    // and thus we should do the friendly thing and ignore the invalid characters as opposed
    // to making the user of this function handle exceptions that are easily forgotten.

    const size_t kBufferSize = 512;
    value_type selfBuffer[kBufferSize]; // This assumes that value_type is one of char8_t, char16_t, char32_t, or wchar_t. Or more importantly, a type with a trivial constructor and destructor.
    value_type* const selfBufferEnd = selfBuffer + kBufferSize;
    const OtherCharType* pOtherEnd = pOther + n;

    while (pOther != pOtherEnd)
    {
      value_type* pSelfBufferCurrent = selfBuffer;
      DecodePart(pOther, pOtherEnd, pSelfBufferCurrent, selfBufferEnd); // Write pOther to pSelfBuffer, converting encoding as we go. We currently ignore the return value, as we don't yet have a plan for handling encoding errors.
      append(selfBuffer, pSelfBufferCurrent);
    }

    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::append(size_type n, value_type c)
  {
    const size_type s = (size_type) (mpEnd - mpBegin);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((n > kMaxSize) || (s > (kMaxSize - n))))
    ThrowLengthException();
#endif

    const size_type nCapacity = (size_type) ((mpCapacity - mpBegin) - 1);

    if ((s + n) > nCapacity)
      reserve(std::max((size_type) GetNewCapacity(nCapacity), (size_type) (s + n)));

    if (n > 0)
    {
      CharStringUninitializedFillN(mpEnd + 1, n - 1, c);
      *mpEnd = c;
      mpEnd += n;
      *mpEnd = 0;
    }

    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::append(const value_type* pBegin, const value_type* pEnd)
  {
    if (pBegin != pEnd)
    {
      const size_type nOldSize = (size_type) (mpEnd - mpBegin);
      const size_type n = (size_type) (pEnd - pBegin);

#if EASTL_STRING_OPT_LENGTH_ERRORS
      if(FLEX_UNLIKELY(((size_t)n > kMaxSize) || (nOldSize > (kMaxSize - n))))
      ThrowLengthException();
#endif

      const size_type nCapacity = (size_type) ((mpCapacity - mpBegin) - 1);

      if ((nOldSize + n) > nCapacity)
      {
        const size_type nLength = std::max((size_type) GetNewCapacity(nCapacity), (size_type) (nOldSize + n)) + 1; // + 1 to accomodate the trailing 0.

        pointer pNewBegin = DoAllocate(nLength);
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mpBegin, mpEnd, pNewBegin);
        pNewEnd = CharStringUninitializedCopy(pBegin, pEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mpBegin = pNewBegin;
        mpEnd = pNewEnd;
        mpCapacity = pNewBegin + nLength;
      }
      else
      {
        const value_type* pTemp = pBegin;
        ++pTemp;
        CharStringUninitializedCopy(pTemp, pEnd, mpEnd + 1);
        mpEnd[n] = 0;
        *mpEnd = *pBegin;
        mpEnd += n;
      }
    }

    return *this;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::push_back(value_type c)
  {
    if ((mpEnd + 1) == mpCapacity) // If we are out of space... (note that we test for + 1 because we have a trailing 0)
      reserve(std::max(GetNewCapacity((size_type) ((mpCapacity - mpBegin) - 1)), (size_type) (mpEnd - mpBegin) + 1));
    *mpEnd++ = c;
    *mpEnd = 0;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::pop_back()
  {
#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(mpEnd <= mpBegin))
    printf("basic_string::pop_back -- empty string");
#endif

    mpEnd[-1] = value_type(0);
    --mpEnd;
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const this_type& x)
  {
    // The C++11 Standard 21.4.6.3 p6 specifies that assign from this_type assigns contents only and not the allocator.
    return assign(x.mpBegin, x.mpEnd);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const this_type& x, size_type position,
      size_type n)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(x.mpEnd - x.mpBegin)))
    ThrowRangeException();
#endif

    // The C++11 Standard 21.4.6.3 p6 specifies that assign from this_type assigns contents only and not the allocator.
    return assign(x.mpBegin + position,
        x.mpBegin + position + std::min(n, (size_type) (x.mpEnd - x.mpBegin) - position));
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const value_type* p, size_type n)
  {
    return assign(p, p + n);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const value_type* p)
  {
    return assign(p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::assign(size_type n, value_type c)
  {
    if (n <= (size_type) (mpEnd - mpBegin))
    {
      CharTypeAssignN(mpBegin, n, c);
      erase(mpBegin + n, mpEnd);
    }
    else
    {
      CharTypeAssignN(mpBegin, (size_type) (mpEnd - mpBegin), c);
      append(n - (size_type) (mpEnd - mpBegin), c);
    }
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const value_type* pBegin, const value_type* pEnd)
  {
    const ptrdiff_t n = pEnd - pBegin;
    if (static_cast<size_type>(n) <= (size_type) (mpEnd - mpBegin))
    {
      memmove(mpBegin, pBegin, (size_t) n * sizeof(value_type));
      erase(mpBegin + n, mpEnd);
    }
    else
    {
      memmove(mpBegin, pBegin, (size_t) (mpEnd - mpBegin) * sizeof(value_type));
      append(pBegin + (size_type) (mpEnd - mpBegin), pEnd);
    }
    return *this;
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(std::initializer_list<value_type> ilist)
  {
    return assign(ilist.begin(), ilist.end());
  }

#if FLEX_HAS_CXX11
  template <typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(this_type&& x)
  {
    if(mAllocator == x.mAllocator)
    {
      std::swap(mpBegin, x.mpBegin);
      std::swap(mpEnd, x.mpEnd);
      std::swap(mpCapacity, x.mpCapacity);
    }
    else
    assign(x.mpBegin, x.mpEnd);

    return *this;
  }
#endif

  template<typename T, typename Allocator>
  template<typename OtherCharType>
  basic_string<T, Allocator>& basic_string<T, Allocator>::assign_convert(const OtherCharType* p)
  {
    clear();
    append_convert(p);
    return *this;
  }

  template<typename T, typename Allocator>
  template<typename OtherCharType>
  basic_string<T, Allocator>& basic_string<T, Allocator>::assign_convert(const OtherCharType* p, size_type n)
  {
    clear();
    append_convert(p, n);
    return *this;
  }

  template<typename T, typename Allocator>
  template<typename OtherStringType>
  basic_string<T, Allocator>& basic_string<T, Allocator>::assign_convert(const OtherStringType& x)
  {
    clear();
    append_convert(x.data(), x.length());
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const this_type& x)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((size_type)(mpEnd - mpBegin) > (kMaxSize - (size_type)(x.mpEnd - x.mpBegin))))
    ThrowLengthException();
#endif

    insert(mpBegin + position, x.mpBegin, x.mpEnd);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const this_type& x, size_type beg,
      size_type n)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY((position > (size_type)(mpEnd - mpBegin)) || (beg > (size_type)(x.mpEnd - x.mpBegin))))
    ThrowRangeException();
#endif

    size_type nLength = std::min(n, (size_type) (x.mpEnd - x.mpBegin) - beg);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((size_type)(mpEnd - mpBegin) > (kMaxSize - nLength)))
    ThrowLengthException();
#endif

    insert(mpBegin + position, x.mpBegin + beg, x.mpBegin + beg + nLength);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const value_type* p, size_type n)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((size_type)(mpEnd - mpBegin) > (kMaxSize - n)))
    ThrowLengthException();
#endif

    insert(mpBegin + position, p, p + n);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const value_type* p)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    size_type nLength = (size_type) CharStrlen(p);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((size_type)(mpEnd - mpBegin) > (kMaxSize - nLength)))
    ThrowLengthException();
#endif

    insert(mpBegin + position, p, p + nLength);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, size_type n, value_type c)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((size_type)(mpEnd - mpBegin) > (kMaxSize - n)))
    ThrowLengthException();
#endif

    insert(mpBegin + position, n, c);
    return *this;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::insert(const_iterator p,
      value_type c)
  {
    if (p == mpEnd)
    {
      push_back(c);
      return mpEnd - 1;
    }
    return InsertInternal(p, c);
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::insert(const_iterator p, size_type n,
      value_type c)
  {
    const ptrdiff_t nPosition = (p - mpBegin); // Save this because we might reallocate.

#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY((p < mpBegin) || (p > mpEnd)))
    printf("basic_string::insert -- invalid position");
#endif

    if (n) // If there is anything to insert...
    {
      if (size_type(mpCapacity - mpEnd) >= (n + 1)) // If we have enough capacity...
      {
        const size_type nElementsAfter = (size_type) (mpEnd - p);
        iterator pOldEnd = mpEnd;

        if (nElementsAfter >= n) // If there's enough space for the new chars between the insert position and the end...
        {
          CharStringUninitializedCopy((mpEnd - n) + 1, mpEnd + 1, mpEnd + 1);
          mpEnd += n;
          memmove(const_cast<value_type*>(p) + n, p, (size_t) ((nElementsAfter - n) + 1) * sizeof(value_type));
          CharTypeAssignN(const_cast<value_type*>(p), n, c);
        }
        else
        {
          CharStringUninitializedFillN(mpEnd + 1, n - nElementsAfter - 1, c);
          mpEnd += n - nElementsAfter;

          CharStringUninitializedCopy(p, pOldEnd + 1, mpEnd);
          mpEnd += nElementsAfter;

          CharTypeAssignN(const_cast<value_type*>(p), nElementsAfter + 1, c);
        }
      }
      else
      {
        const size_type nOldSize = (size_type) (mpEnd - mpBegin);
        const size_type nOldCap = (size_type) ((mpCapacity - mpBegin) - 1);
        const size_type nLength = std::max((size_type) GetNewCapacity(nOldCap), (size_type) (nOldSize + n)) + 1; // + 1 to accomodate the trailing 0.

        iterator pNewBegin = DoAllocate(nLength);
        iterator pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mpBegin, p, pNewBegin);
        pNewEnd = CharStringUninitializedFillN(pNewEnd, n, c);
        pNewEnd = CharStringUninitializedCopy(p, mpEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mpBegin = pNewBegin;
        mpEnd = pNewEnd;
        mpCapacity = pNewBegin + nLength;
      }
    }

    return mpBegin + nPosition;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::insert(const_iterator p,
      const value_type* pBegin, const value_type* pEnd)
  {
    const ptrdiff_t nPosition = (p - mpBegin); // Save this because we might reallocate.

#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY((p < mpBegin) || (p > mpEnd)))
    printf("basic_string::insert -- invalid position");
#endif

    const size_type n = (size_type) (pEnd - pBegin);

    if (n)
    {
      const bool bCapacityIsSufficient = ((mpCapacity - mpEnd) >= (difference_type) (n + 1));
      const bool bSourceIsFromSelf = ((pEnd >= mpBegin) && (pBegin <= mpEnd));

      // If bSourceIsFromSelf is true, then we reallocate. This is because we are
      // inserting ourself into ourself and thus both the source and destination
      // be modified, making it rather tricky to attempt to do in place. The simplest
      // resolution is to reallocate. To consider: there may be a way to implement this
      // whereby we don't need to reallocate or can often avoid reallocating.
      if (bCapacityIsSufficient && !bSourceIsFromSelf)
      {
        const ptrdiff_t nElementsAfter = (mpEnd - p);
        iterator pOldEnd = mpEnd;

        if (nElementsAfter >= (ptrdiff_t) n) // If the newly inserted characters entirely fit within the size of the original string...
        {
          memmove(mpEnd + 1, mpEnd - n + 1, (size_t) n * sizeof(value_type));
          mpEnd += n;
          memmove(const_cast<value_type*>(p) + n, p, (size_t) ((nElementsAfter - n) + 1) * sizeof(value_type));
          memmove(const_cast<value_type*>(p), pBegin, (size_t) (pEnd - pBegin) * sizeof(value_type));
        }
        else
        {
          const value_type* const pMid = pBegin + (nElementsAfter + 1);

          memmove(mpEnd + 1, pMid, (size_t) (pEnd - pMid) * sizeof(value_type));
          mpEnd += n - nElementsAfter;

          memmove(mpEnd, p, (size_t) (pOldEnd - p + 1) * sizeof(value_type));
          mpEnd += nElementsAfter;

          memmove(const_cast<value_type*>(p), pBegin, (size_t) (pMid - pBegin) * sizeof(value_type));
        }
      }
      else // Else we need to reallocate to implement this.
      {
        const size_type nOldSize = (size_type) (mpEnd - mpBegin);
        const size_type nOldCap = (size_type) ((mpCapacity - mpBegin) - 1);
        size_type nLength;

        if (bCapacityIsSufficient) // If bCapacityIsSufficient is true, then bSourceIsFromSelf must be false.
          nLength = nOldSize + n + 1; // + 1 to accomodate the trailing 0.
        else
          nLength = std::max((size_type) GetNewCapacity(nOldCap), (size_type) (nOldSize + n)) + 1; // + 1 to accomodate the trailing 0.

        pointer pNewBegin = DoAllocate(nLength);
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mpBegin, p, pNewBegin);
        pNewEnd = CharStringUninitializedCopy(pBegin, pEnd, pNewEnd);
        pNewEnd = CharStringUninitializedCopy(p, mpEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mpBegin = pNewBegin;
        mpEnd = pNewEnd;
        mpCapacity = pNewBegin + nLength;
      }
    }

    return mpBegin + nPosition;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::insert(const_iterator p,
      std::initializer_list<value_type> ilist)
  {
    return insert(p, ilist.begin(), ilist.end());
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::erase(size_type position, size_type n)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    printf("basic_string::erase -- invalid position");
#endif

    erase(mpBegin + position, mpBegin + position + std::min(n, (size_type) (mpEnd - mpBegin) - position));
    return *this;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::erase(const_iterator p)
  {
#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY((p < mpBegin) || (p >= mpEnd)))
    printf("basic_string::erase -- invalid position");
#endif

    memmove(const_cast<value_type*>(p), p + 1, (size_t) (mpEnd - p) * sizeof(value_type));
    --mpEnd;
    return const_cast<value_type*>(p);
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::erase(const_iterator pBegin,
      const_iterator pEnd)
  {
#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY((pBegin < mpBegin) || (pBegin > mpEnd) || (pEnd < mpBegin) || (pEnd > mpEnd) || (pEnd < pBegin)))
    printf("basic_string::erase -- invalid position");
#endif

    if (pBegin != pEnd)
    {
      memmove(const_cast<value_type*>(pBegin), pEnd, (size_t) ((mpEnd - pEnd) + 1) * sizeof(value_type));
      const iterator pNewEnd = (mpEnd - (pEnd - pBegin));
      mpEnd = pNewEnd;
    }
    return const_cast<value_type*>(pBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reverse_iterator basic_string<T, Allocator>::erase(
      reverse_iterator position)
  {
    return reverse_iterator(erase((++position).base()));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::reverse_iterator basic_string<T, Allocator>::erase(reverse_iterator first,
      reverse_iterator last)
  {
    return reverse_iterator(erase((++last).base(), (++first).base()));
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n, const this_type& x)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    const size_type nLength = std::min(n, (size_type) (mpEnd - mpBegin) - position);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY(((size_type)(mpEnd - mpBegin) - nLength) >= (kMaxSize - (size_type)(x.mpEnd - x.mpBegin))))
    ThrowLengthException();
#endif

    return replace(mpBegin + position, mpBegin + position + nLength, x.mpBegin, x.mpEnd);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type pos1, size_type n1, const this_type& x,
      size_type pos2, size_type n2)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY((pos1 > (size_type)(mpEnd - mpBegin)) || (pos2 > (size_type)(x.mpEnd - x.mpBegin))))
    ThrowRangeException();
#endif

    const size_type nLength1 = std::min(n1, (size_type) (mpEnd - mpBegin) - pos1);
    const size_type nLength2 = std::min(n2, (size_type) (x.mpEnd - x.mpBegin) - pos2);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY(((size_type)(mpEnd - mpBegin) - nLength1) >= (kMaxSize - nLength2)))
    ThrowLengthException();
#endif

    return replace(mpBegin + pos1, mpBegin + pos1 + nLength1, x.mpBegin + pos2, x.mpBegin + pos2 + nLength2);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, const value_type* p,
      size_type n2)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    const size_type nLength = std::min(n1, (size_type) (mpEnd - mpBegin) - position);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((n2 > kMaxSize) || (((size_type)(mpEnd - mpBegin) - nLength) >= (kMaxSize - n2))))
    ThrowLengthException();
#endif

    return replace(mpBegin + position, mpBegin + position + nLength, p, p + n2);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, const value_type* p)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    const size_type nLength = std::min(n1, (size_type) (mpEnd - mpBegin) - position);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    const size_type n2 = (size_type)CharStrlen(p);
    if(FLEX_UNLIKELY((n2 > kMaxSize) || (((size_type)(mpEnd - mpBegin) - nLength) >= (kMaxSize - n2))))
    ThrowLengthException();
#endif

    return replace(mpBegin + position, mpBegin + position + nLength, p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, size_type n2,
      value_type c)
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    const size_type nLength = std::min(n1, (size_type) (mpEnd - mpBegin) - position);

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY((n2 > kMaxSize) || ((size_type)(mpEnd - mpBegin) - nLength) >= (kMaxSize - n2)))
    ThrowLengthException();
#endif

    return replace(mpBegin + position, mpBegin + position + nLength, n2, c);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd,
      const this_type& x)
  {
    return replace(pBegin, pEnd, x.mpBegin, x.mpEnd);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd,
      const value_type* p, size_type n)
  {
    return replace(pBegin, pEnd, p, p + n);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd,
      const value_type* p)
  {
    return replace(pBegin, pEnd, p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd,
      size_type n, value_type c)
  {
#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY((pBegin < mpBegin) || (pBegin > mpEnd) || (pEnd < mpBegin) || (pEnd > mpEnd) || (pEnd < pBegin)))
    printf("basic_string::replace -- invalid position");
#endif

    const size_type nLength = static_cast<size_type>(pEnd - pBegin);

    if (nLength >= n)
    {
      CharTypeAssignN(const_cast<value_type*>(pBegin), n, c);
      erase(pBegin + n, pEnd);
    }
    else
    {
      CharTypeAssignN(const_cast<value_type*>(pBegin), nLength, c);
      insert(pEnd, n - nLength, c);
    }
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin1, const_iterator pEnd1,
      const value_type* pBegin2, const value_type* pEnd2)
  {
#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY((pBegin1 < mpBegin) || (pBegin1 > mpEnd) || (pEnd1 < mpBegin) || (pEnd1 > mpEnd) || (pEnd1 < pBegin1)))
    printf("basic_string::replace -- invalid position");
#endif

    const size_type nLength1 = (size_type) (pEnd1 - pBegin1);
    const size_type nLength2 = (size_type) (pEnd2 - pBegin2);

    if (nLength1 >= nLength2) // If we have a non-expanding operation...
    {
      if ((pBegin2 > pEnd1) || (pEnd2 <= pBegin1)) // If we have a non-overlapping operation...
        memcpy(const_cast<value_type*>(pBegin1), pBegin2, (size_t) (pEnd2 - pBegin2) * sizeof(value_type));
      else
        memmove(const_cast<value_type*>(pBegin1), pBegin2, (size_t) (pEnd2 - pBegin2) * sizeof(value_type));
      erase(pBegin1 + nLength2, pEnd1);
    }
    else // Else we are expanding.
    {
      if ((pBegin2 > pEnd1) || (pEnd2 <= pBegin1)) // If we have a non-overlapping operation...
      {
        const value_type* const pMid2 = pBegin2 + nLength1;

        if ((pEnd2 <= pBegin1) || (pBegin2 > pEnd1))
          memcpy(const_cast<value_type*>(pBegin1), pBegin2, (size_t) (pMid2 - pBegin2) * sizeof(value_type));
        else
          memmove(const_cast<value_type*>(pBegin1), pBegin2, (size_t) (pMid2 - pBegin2) * sizeof(value_type));
        insert(pEnd1, pMid2, pEnd2);
      }
      else // else we have an overlapping operation.
      {
        // I can't think of any easy way of doing this without allocating temporary memory.
        const size_type nOldSize = (size_type) (mpEnd - mpBegin);
        const size_type nOldCap = (size_type) ((mpCapacity - mpBegin) - 1);
        const size_type nNewCapacity = std::max((size_type) GetNewCapacity(nOldCap),
            (size_type) (nOldSize + (nLength2 - nLength1))) + 1; // + 1 to accomodate the trailing 0.

        pointer pNewBegin = DoAllocate(nNewCapacity);
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mpBegin, pBegin1, pNewBegin);
        pNewEnd = CharStringUninitializedCopy(pBegin2, pEnd2, pNewEnd);
        pNewEnd = CharStringUninitializedCopy(pEnd1, mpEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mpBegin = pNewBegin;
        mpEnd = pNewEnd;
        mpCapacity = pNewBegin + nNewCapacity;
      }
    }
    return *this;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::copy(value_type* p, size_type n,
      size_type position) const
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    // It is not clear from the C++ standard if 'p' destination pointer is allowed to
    // refer to memory from within the string itself. We assume so and use memmove
    // instead of memcpy until we find otherwise.
    const size_type nLength = std::min(n, (size_type) (mpEnd - mpBegin) - position);
    memmove(p, mpBegin + position, (size_t) nLength * sizeof(value_type));
    return nLength;
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::swap(this_type& x)
  {
    if (mAllocator == x.mAllocator) // If allocators are equivalent...
    {
      // We leave mAllocator as-is.
      std::swap(mpBegin, x.mpBegin);
      std::swap(mpEnd, x.mpEnd);
      std::swap(mpCapacity, x.mpCapacity);
    }
    else // else swap the contents.
    {
      const this_type temp(*this); // Can't call std::swap because that would
      *this = x; // itself call this member swap function.
      x = temp;
    }
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return find(x.mpBegin, position, (size_type) (x.mpEnd - x.mpBegin));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find(const value_type* p,
      size_type position) const
  {
    return find(p, position, (size_type) CharStrlen(p));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find(const value_type* p,
      size_type position, size_type n) const
  {
    // It is not clear what the requirements are for position, but since the C++ standard
    // appears to be silent it is assumed for now that position can be any value.
    //#if EASTL_ASSERT_ENABLED
    //    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    //        printf("basic_string::find -- invalid position");
    //#endif

    if (FLEX_LIKELY(((npos - n) >= position) && (position + n) <= (size_type ) (mpEnd - mpBegin))) // If the range is valid...
    {
      const value_type* const pTemp = std::search(mpBegin + position, mpEnd, p, p + n);

      if ((pTemp != mpEnd) || (n == 0))
        return (size_type) (pTemp - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    // It is not clear what the requirements are for position, but since the C++ standard
    // appears to be silent it is assumed for now that position can be any value.
    //#if EASTL_ASSERT_ENABLED
    //    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    //        printf("basic_string::find -- invalid position");
    //#endif

    if (FLEX_LIKELY(position < (size_type )(mpEnd - mpBegin))) // If the position is valid...
    {
      const const_iterator pResult = std::find(mpBegin + position, mpEnd, c);

      if (pResult != mpEnd)
        return (size_type) (pResult - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::rfind(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return rfind(x.mpBegin, position, (size_type) (x.mpEnd - x.mpBegin));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::rfind(const value_type* p,
      size_type position) const
  {
    return rfind(p, position, (size_type) CharStrlen(p));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::rfind(const value_type* p,
      size_type position, size_type n) const
  {
    // Disabled because it's not clear what values are valid for position.
    // It is documented that npos is a valid value, though. We return npos and
    // don't crash if postion is any invalid value.
    //#if EASTL_ASSERT_ENABLED
    //    if(FLEX_UNLIKELY((position != npos) && (position > (size_type)(mpEnd - mpBegin))))
    //        printf("basic_string::rfind -- invalid position");
    //#endif

    // Note that a search for a zero length string starting at position = end() returns end() and not npos.
    // Note by Paul Pedriana: I am not sure how this should behave in the case of n == 0 and position > size.
    // The standard seems to suggest that rfind doesn't act exactly the same as find in that input position
    // can be > size and the return value can still be other than npos. Thus, if n == 0 then you can
    // never return npos, unlike the case with find.
    const size_type nLength = (size_type) (mpEnd - mpBegin);

    if (FLEX_LIKELY(n <= nLength))
    {
      if (FLEX_LIKELY(n))
      {
        const const_iterator pEnd = mpBegin + std::min(nLength - n, position) + n;
        const const_iterator pResult = CharTypeStringRSearch(mpBegin, pEnd, p, p + n);

        if (pResult != pEnd)
          return (size_type) (pResult - mpBegin);
      }
      else
        return std::min(nLength, position);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::rfind(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    // If n is zero or position is >= size, we return npos.
    const size_type nLength = (size_type) (mpEnd - mpBegin);

    if (FLEX_LIKELY(nLength))
    {
      const value_type* const pEnd = mpBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFind(pEnd, mpBegin, c);

      if (pResult != mpBegin)
        return (size_type) ((pResult - 1) - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_of(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return find_first_of(x.mpBegin, position, (size_type) (x.mpEnd - x.mpBegin));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_of(const value_type* p,
      size_type position) const
  {
    return find_first_of(p, position, (size_type) CharStrlen(p));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_of(const value_type* p,
      size_type position, size_type n) const
  {
// If position is >= size, we return npos.
    if (FLEX_LIKELY((position < (size_type ) (mpEnd - mpBegin))))
    {
      const value_type* const pBegin = mpBegin + position;
      const const_iterator pResult = CharTypeStringFindFirstOf(pBegin, mpEnd, p, p + n);

      if (pResult != mpEnd)
        return (size_type) (pResult - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_of(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    return find(c, position);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_of(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return find_last_of(x.mpBegin, position, (size_type) (x.mpEnd - x.mpBegin));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_of(const value_type* p,
      size_type position) const
  {
    return find_last_of(p, position, (size_type) CharStrlen(p));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_of(const value_type* p,
      size_type position, size_type n) const
  {
// If n is zero or position is >= size, we return npos.
    const size_type nLength = (size_type) (mpEnd - mpBegin);

    if (FLEX_LIKELY(nLength))
    {
      const value_type* const pEnd = mpBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFindFirstOf(pEnd, mpBegin, p, p + n);

      if (pResult != mpBegin)
        return (size_type) ((pResult - 1) - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_of(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    return rfind(c, position);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_not_of(
      const this_type& x, size_type position) const FLEX_NOEXCEPT
  {
    return find_first_not_of(x.mpBegin, position, (size_type) (x.mpEnd - x.mpBegin));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_not_of(
      const value_type* p, size_type position) const
  {
    return find_first_not_of(p, position, (size_type) CharStrlen(p));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_not_of(const value_type* p,
      size_type position, size_type n) const
  {
    if (FLEX_LIKELY(position <= (size_type ) (mpEnd - mpBegin)))
    {
      const const_iterator pResult = CharTypeStringFindFirstNotOf(mpBegin + position, mpEnd, p, p + n);

      if (pResult != mpEnd)
        return (size_type) (pResult - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_not_of(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    if (FLEX_LIKELY(position <= (size_type )(mpEnd - mpBegin)))
    {
// Todo: Possibly make a specialized version of CharTypeStringFindFirstNotOf(pBegin, pEnd, c).
      const const_iterator pResult = CharTypeStringFindFirstNotOf(mpBegin + position, mpEnd, &c, &c + 1);

      if (pResult != mpEnd)
        return (size_type) (pResult - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_not_of(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return find_last_not_of(x.mpBegin, position, (size_type) (x.mpEnd - x.mpBegin));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_not_of(
      const value_type* p, size_type position) const
  {
    return find_last_not_of(p, position, (size_type) CharStrlen(p));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_not_of(const value_type* p,
      size_type position, size_type n) const
  {
    const size_type nLength = (size_type) (mpEnd - mpBegin);

    if (FLEX_LIKELY(nLength))
    {
      const value_type* const pEnd = mpBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, mpBegin, p, p + n);

      if (pResult != mpBegin)
        return (size_type) ((pResult - 1) - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_not_of(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    const size_type nLength = (size_type) (mpEnd - mpBegin);

    if (FLEX_LIKELY(nLength))
    {
      // Todo: Possibly make a specialized version of CharTypeStringRFindFirstNotOf(pBegin, pEnd, c).
      const value_type* const pEnd = mpBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, mpBegin, &c, &c + 1);

      if (pResult != mpBegin)
        return (size_type) ((pResult - 1) - mpBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator> basic_string<T, Allocator>::substr(size_type position, size_type n) const
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#elif EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(position > (size_type)(mpEnd - mpBegin)))
    printf("basic_string::substr -- invalid position");
#endif

    return basic_string(mpBegin + position, mpBegin + position + std::min(n, (size_type) (mpEnd - mpBegin) - position),
        mAllocator);
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(const this_type& x) const FLEX_NOEXCEPT
  {
    return compare(mpBegin, mpEnd, x.mpBegin, x.mpEnd);
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const this_type& x) const
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(pos1 > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    return compare(mpBegin + pos1, mpBegin + pos1 + std::min(n1, (size_type) (mpEnd - mpBegin) - pos1), x.mpBegin,
        x.mpEnd);
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const this_type& x, size_type pos2,
      size_type n2) const
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY((pos1 > (size_type)(mpEnd - mpBegin)) || (pos2 > (size_type)(x.mpEnd - x.mpBegin))))
    ThrowRangeException();
#endif

    return compare(mpBegin + pos1, mpBegin + pos1 + std::min(n1, (size_type) (mpEnd - mpBegin) - pos1),
        x.mpBegin + pos2, x.mpBegin + pos2 + std::min(n2, (size_type) (x.mpEnd - x.mpBegin) - pos2));
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(const value_type* p) const
  {
    return compare(mpBegin, mpEnd, p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const value_type* p) const
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(pos1 > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    return compare(mpBegin + pos1, mpBegin + pos1 + std::min(n1, (size_type) (mpEnd - mpBegin) - pos1), p,
        p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const value_type* p, size_type n2) const
  {
#if EASTL_STRING_OPT_RANGE_ERRORS
    if(FLEX_UNLIKELY(pos1 > (size_type)(mpEnd - mpBegin)))
    ThrowRangeException();
#endif

    return compare(mpBegin + pos1, mpBegin + pos1 + std::min(n1, (size_type) (mpEnd - mpBegin) - pos1), p, p + n2);
  }

// make_lower
// This is a very simple ASCII-only case conversion function
// Anything more complicated should use a more powerful separate library.
  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::make_lower()
  {
    for (pointer p = mpBegin; p < mpEnd; ++p)
      *p = (value_type) CharToLower(*p);
  }

// make_upper
// This is a very simple ASCII-only case conversion function
// Anything more complicated should use a more powerful separate library.
  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::make_upper()
  {
    for (pointer p = mpBegin; p < mpEnd; ++p)
      *p = (value_type) CharToUpper(*p);
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::ltrim()
  {
    const value_type array[] = { ' ', '\t', 0 }; // This is a pretty simplistic view of whitespace.
    erase(0, find_first_not_of(array));
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::rtrim()
  {
    const value_type array[] = { ' ', '\t', 0 }; // This is a pretty simplistic view of whitespace.
    erase(find_last_not_of(array) + 1);
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::trim()
  {
    ltrim();
    rtrim();
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator> basic_string<T, Allocator>::left(size_type n) const
  {
    const size_type nLength = length();
    if (n < nLength)
      return substr(0, n);
    return *this;
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator> basic_string<T, Allocator>::right(size_type n) const
  {
    const size_type nLength = length();
    if (n < nLength)
      return substr(nLength - n, n);
    return *this;
  }

  template<typename T, typename Allocator>
  int basic_string<T, Allocator>::compare(const value_type* pBegin1, const value_type* pEnd1, const value_type* pBegin2,
      const value_type* pEnd2)
  {
    const ptrdiff_t n1 = pEnd1 - pBegin1;
    const ptrdiff_t n2 = pEnd2 - pBegin2;
    const ptrdiff_t nMin = std::min(n1, n2);
    const int cmp = Compare(pBegin1, pBegin2, (size_t) nMin);

    return (cmp != 0 ? cmp : (n1 < n2 ? -1 : (n1 > n2 ? 1 : 0)));
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::InsertInternal(const_iterator p,
      value_type c)
  {
    iterator pNewPosition = const_cast<value_type*>(p);

    if ((mpEnd + 1) < mpCapacity)
    {
      *(mpEnd + 1) = 0;
      memmove(const_cast<value_type*>(p) + 1, p, (size_t) (mpEnd - p) * sizeof(value_type));
      *pNewPosition = c;
      ++mpEnd;
    }
    else
    {
      const size_type nOldSize = (size_type) (mpEnd - mpBegin);
      const size_type nOldCap = (size_type) ((mpCapacity - mpBegin) - 1);
      const size_type nLength = std::max((size_type) GetNewCapacity(nOldCap), (size_type) (nOldSize + 1)) + 1; // The second + 1 is to accomodate the trailing 0.

      iterator pNewBegin = DoAllocate(nLength);
      iterator pNewEnd = pNewBegin;

      pNewPosition = CharStringUninitializedCopy(mpBegin, p, pNewBegin);
      *pNewPosition = c;

      pNewEnd = pNewPosition + 1;
      pNewEnd = CharStringUninitializedCopy(p, mpEnd, pNewEnd);
      *pNewEnd = 0;

      DeallocateSelf();
      mpBegin = pNewBegin;
      mpEnd = pNewEnd;
      mpCapacity = pNewBegin + nLength;
    }
    return pNewPosition;
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::SizeInitialize(size_type n, value_type c)
  {
    AllocateSelf((size_type) (n + 1)); // '+1' so that we have room for the terminating 0.

    mpEnd = CharStringUninitializedFillN(mpBegin, n, c);
    *mpEnd = 0;
  }

  template<typename T, typename Allocator>
  template<class InputIterator>
  void basic_string<T, Allocator>::RangeInitialize(InputIterator pBegin, InputIterator pEnd)
  {
#if EASTL_STRING_OPT_ARGUMENT_ERRORS
    if(FLEX_UNLIKELY(!pBegin && (pEnd < pBegin))) // 21.4.2 p7
    ThrowInvalidArgumentException();
#endif

    const size_type n = (size_type) (pEnd - pBegin);

    AllocateSelf((size_type) (n + 1)); // '+1' so that we have room for the terminating 0.

    mpEnd = CharStringUninitializedCopy(pBegin, pEnd, mpBegin);
    *mpEnd = 0;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::RangeInitialize(const value_type* pBegin)
  {
#if EASTL_STRING_OPT_ARGUMENT_ERRORS
    if(FLEX_UNLIKELY(!pBegin))
    ThrowInvalidArgumentException();
#endif

    RangeInitialize(pBegin, pBegin + CharStrlen(pBegin));
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::DoAllocate(size_type n)
  {
    return (value_type*) mAllocator.allocate(n);
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::DoFree(value_type* p, size_type n)
  {
    if (p)
      mAllocator.deallocate(p, n);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::GetNewCapacity(
      size_type currentCapacity) // This needs to return a value of at least currentCapacity and at least 1.
  {
    return (currentCapacity > EASTL_STRING_INITIAL_CAPACITY) ? (2 * currentCapacity) : EASTL_STRING_INITIAL_CAPACITY;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::AllocateSelf()
  {
    //EASTL_ASSERT(sEmptyString.mUint32 == 0);
    mpBegin = const_cast<value_type*>(GetEmptyString(value_type())); // In const_cast-int this, we promise not to modify it.
    mpEnd = mpBegin;
    mpCapacity = mpBegin + 1; // When we are using sEmptyString, mpCapacity is always mpEnd + 1. This is an important distinguising characteristic.
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::AllocateSelf(size_type n)
  {
#if EASTL_ASSERT_ENABLED
    if(FLEX_UNLIKELY(n >= 0x40000000))
    printf("basic_string::AllocateSelf -- improbably large request.");
#endif

#if EASTL_STRING_OPT_LENGTH_ERRORS
    if(FLEX_UNLIKELY(n > kMaxSize))
    ThrowLengthException();
#endif

    if (n > 1)
    {
      mpBegin = DoAllocate(n);
      mpEnd = mpBegin;
      mpCapacity = mpBegin + n;
    }
    else
      AllocateSelf();
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::DeallocateSelf()
  {
    // Note that we compare mpCapacity to mpEnd instead of comparing
    // mpBegin to &sEmptyString. This is important because we may have
    // a case whereby one library passes a string to another library to
    // deallocate and the two libraries have independent versions of sEmptyString.
    if ((mpCapacity - mpBegin) > 1) // If we are not using sEmptyString as our memory...
      DoFree(mpBegin, (size_type) (mpCapacity - mpBegin));
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::ThrowLengthException() const
  {
#if EASTL_EXCEPTIONS_ENABLED
    throw std::length_error("basic_string -- length_error");
#elif EASTL_ASSERT_ENABLED
    printf("basic_string -- length_error");
#endif
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::ThrowRangeException() const
  {
#if EASTL_EXCEPTIONS_ENABLED
    throw std::out_of_range("basic_string -- out of range");
#elif EASTL_ASSERT_ENABLED
    printf("basic_string -- out of range");
#endif
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::ThrowInvalidArgumentException() const
  {
#if EASTL_EXCEPTIONS_ENABLED
    throw std::invalid_argument("basic_string -- invalid argument");
#elif EASTL_ASSERT_ENABLED
    printf("basic_string -- invalid argument");
#endif
  }

// CharTypeStringFindEnd
// Specialized char version of STL find() from back function.
// Not the same as RFind because search range is specified as forward iterators.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringFindEnd(const value_type* pBegin, const value_type* pEnd, value_type c)
  {
    const value_type* pTemp = pEnd;
    while (--pTemp >= pBegin)
    {
      if (*pTemp == c)
        return pTemp;
    }

    return pEnd;
  }

// CharTypeStringRFind
// Specialized value_type version of STL find() function in reverse.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringRFind(const value_type* pRBegin, const value_type* pREnd,
      const value_type c)
  {
    while (pRBegin > pREnd)
    {
      if (*(pRBegin - 1) == c)
        return pRBegin;
      --pRBegin;
    }
    return pREnd;
  }

// CharTypeStringSearch
// Specialized value_type version of STL search() function.
// Purpose: find p2 within p1. Return p1End if not found or if either string is zero length.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringSearch(const value_type* p1Begin, const value_type* p1End,
      const value_type* p2Begin, const value_type* p2End)
  {
    // Test for zero length strings, in which case we have a match or a failure,
    // but the return value is the same either way.
    if ((p1Begin == p1End) || (p2Begin == p2End))
      return p1Begin;

    // Test for a pattern of length 1.
    if ((p2Begin + 1) == p2End)
      return std::find(p1Begin, p1End, *p2Begin);

    // General case.
    const value_type* pTemp;
    const value_type* pTemp1 = (p2Begin + 1);
    const value_type* pCurrent = p1Begin;

    while (p1Begin != p1End)
    {
      p1Begin = std::find(p1Begin, p1End, *p2Begin);
      if (p1Begin == p1End)
        return p1End;

      pTemp = pTemp1;
      pCurrent = p1Begin;
      if (++pCurrent == p1End)
        return p1End;

      while (*pCurrent == *pTemp)
      {
        if (++pTemp == p2End)
          return p1Begin;
        if (++pCurrent == p1End)
          return p1End;
      }

      ++p1Begin;
    }

    return p1Begin;
  }

// CharTypeStringRSearch
// Specialized value_type version of STL find_end() function (which really is a reverse search function).
// Purpose: find last instance of p2 within p1. Return p1End if not found or if either string is zero length.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringRSearch(const value_type* p1Begin, const value_type* p1End,
      const value_type* p2Begin, const value_type* p2End)
  {
    // Test for zero length strings, in which case we have a match or a failure,
    // but the return value is the same either way.
    if ((p1Begin == p1End) || (p2Begin == p2End))
      return p1Begin;

    // Test for a pattern of length 1.
    if ((p2Begin + 1) == p2End)
      return CharTypeStringFindEnd(p1Begin, p1End, *p2Begin);

    // Test for search string length being longer than string length.
    if ((p2End - p2Begin) > (p1End - p1Begin))
      return p1End;

    // General case.
    const value_type* pSearchEnd = (p1End - (p2End - p2Begin) + 1);
    const value_type* pCurrent1;
    const value_type* pCurrent2;

    while (pSearchEnd != p1Begin)
    {
      // Search for the last occurrence of *p2Begin.
      pCurrent1 = CharTypeStringFindEnd(p1Begin, pSearchEnd, *p2Begin);
      if (pCurrent1 == pSearchEnd) // If the first char of p2 wasn't found,
        return p1End; // then we immediately have failure.

      // In this case, *pTemp == *p2Begin. So compare the rest.
      pCurrent2 = p2Begin;
      while (*pCurrent1++ == *pCurrent2++)
      {
        if (pCurrent2 == p2End)
          return (pCurrent1 - (p2End - p2Begin));
      }

      // A smarter algorithm might know to subtract more than just one,
      // but in most cases it won't make much difference anyway.
      --pSearchEnd;
    }

    return p1End;
  }

// CharTypeStringFindFirstOf
// Specialized value_type version of STL find_first_of() function.
// This function is much like the C runtime strtok function, except the strings aren't null-terminated.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringFindFirstOf(const value_type* p1Begin, const value_type* p1End,
      const value_type* p2Begin, const value_type* p2End)
  {
    for (; p1Begin != p1End; ++p1Begin)
    {
      for (const value_type* pTemp = p2Begin; pTemp != p2End; ++pTemp)
      {
        if (*p1Begin == *pTemp)
          return p1Begin;
      }
    }
    return p1End;
  }

// CharTypeStringRFindFirstOf
// Specialized value_type version of STL find_first_of() function in reverse.
// This function is much like the C runtime strtok function, except the strings aren't null-terminated.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringRFindFirstOf(const value_type* p1RBegin, const value_type* p1REnd,
      const value_type* p2Begin, const value_type* p2End)
  {
    for (; p1RBegin != p1REnd; --p1RBegin)
    {
      for (const value_type* pTemp = p2Begin; pTemp != p2End; ++pTemp)
      {
        if (*(p1RBegin - 1) == *pTemp)
          return p1RBegin;
      }
    }
    return p1REnd;
  }

// CharTypeStringFindFirstNotOf
// Specialized value_type version of STL find_first_not_of() function.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringFindFirstNotOf(const value_type* p1Begin, const value_type* p1End,
      const value_type* p2Begin, const value_type* p2End)
  {
    for (; p1Begin != p1End; ++p1Begin)
    {
      const value_type* pTemp;
      for (pTemp = p2Begin; pTemp != p2End; ++pTemp)
      {
        if (*p1Begin == *pTemp)
          break;
      }
      if (pTemp == p2End)
        return p1Begin;
    }
    return p1End;
  }

// CharTypeStringRFindFirstNotOf
// Specialized value_type version of STL find_first_not_of() function in reverse.
  template<typename T, typename Allocator>
  const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::CharTypeStringRFindFirstNotOf(const value_type* p1RBegin, const value_type* p1REnd,
      const value_type* p2Begin, const value_type* p2End)
  {
    for (; p1RBegin != p1REnd; --p1RBegin)
    {
      const value_type* pTemp;
      for (pTemp = p2Begin; pTemp != p2End; ++pTemp)
      {
        if (*(p1RBegin - 1) == *pTemp)
          break;
      }
      if (pTemp == p2End)
        return p1RBegin;
    }
    return p1REnd;
  }

// iterator operators
  template<typename T, typename Allocator>
  inline bool operator==(const typename basic_string<T, Allocator>::reverse_iterator& r1,
      const typename basic_string<T, Allocator>::reverse_iterator& r2)
  {
    return r1.mpCurrent == r2.mpCurrent;
  }

  template<typename T, typename Allocator>
  inline bool operator!=(const typename basic_string<T, Allocator>::reverse_iterator& r1,
      const typename basic_string<T, Allocator>::reverse_iterator& r2)
  {
    return r1.mpCurrent != r2.mpCurrent;
  }

// Operator +
  template<typename T, typename Allocator>
  basic_string<T, Allocator> operator+(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
  {
    typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
    CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
    basic_string<T, Allocator> result(cDNI, a.size() + b.size(),
        const_cast<basic_string<T, Allocator>&>(a).get_allocator()); // Note that we choose to assign a's allocator.
    result.append(a);
    result.append(b);
    return result;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator> operator+(const typename basic_string<T, Allocator>::value_type* p,
      const basic_string<T, Allocator>& b)
  {
    typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
    CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
    const typename basic_string<T, Allocator>::size_type n =
        (typename basic_string<T, Allocator>::size_type) CharStrlen(p);
    basic_string<T, Allocator> result(cDNI, n + b.size(), const_cast<basic_string<T, Allocator>&>(b).get_allocator());
    result.append(p, p + n);
    result.append(b);
    return result;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator> operator+(typename basic_string<T, Allocator>::value_type c,
      const basic_string<T, Allocator>& b)
  {
    typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
    CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
    basic_string<T, Allocator> result(cDNI, 1 + b.size(), const_cast<basic_string<T, Allocator>&>(b).get_allocator());
    result.push_back(c);
    result.append(b);
    return result;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator> operator+(const basic_string<T, Allocator>& a,
      const typename basic_string<T, Allocator>::value_type* p)
  {
    typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
    CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
    const typename basic_string<T, Allocator>::size_type n =
        (typename basic_string<T, Allocator>::size_type) CharStrlen(p);
    basic_string<T, Allocator> result(cDNI, a.size() + n, const_cast<basic_string<T, Allocator>&>(a).get_allocator());
    result.append(a);
    result.append(p, p + n);
    return result;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator> operator+(const basic_string<T, Allocator>& a,
      typename basic_string<T, Allocator>::value_type c)
  {
    typedef typename basic_string<T, Allocator>::CtorDoNotInitialize CtorDoNotInitialize;
    CtorDoNotInitialize cDNI; // GCC 2.x forces us to declare a named temporary like this.
    basic_string<T, Allocator> result(cDNI, a.size() + 1, const_cast<basic_string<T, Allocator>&>(a).get_allocator());
    result.append(a);
    result.push_back(c);
    return result;
  }

#if FLEX_HAS_CXX11
  template <typename T, typename Allocator>
  basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, basic_string<T, Allocator>&& b)
  {
    a.append(b); // Using an rvalue by name results in it becoming an lvalue.
    return a;
  }

  template <typename T, typename Allocator>
  basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, const basic_string<T, Allocator>& b)
  {
    a.append(b);
    return a;
  }

  template <typename T, typename Allocator>
  basic_string<T, Allocator> operator+(const typename basic_string<T, Allocator>::value_type* p, basic_string<T, Allocator>&& b)
  {
    b.insert(0, p);
    return b;
  }

  template <typename T, typename Allocator>
  basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, const typename basic_string<T, Allocator>::value_type* p)
  {
    a.append(p);
    return a;
  }

  template <typename T, typename Allocator>
  basic_string<T, Allocator> operator+(basic_string<T, Allocator>&& a, typename basic_string<T, Allocator>::value_type c)
  {
    a.push_back(c);
    return a;
  }
#endif

  template<typename T, typename Allocator>
  inline bool basic_string<T, Allocator>::validate() const FLEX_NOEXCEPT
  {
    if ((mpBegin == NULL) || (mpEnd == NULL))
      return false;
    if (mpEnd < mpBegin)
      return false;
    if (mpCapacity < mpEnd)
      return false;
    return true;
  }

///////////////////////////////////////////////////////////////////////
// global operators
///////////////////////////////////////////////////////////////////////

// Operator== and operator!=
  template<typename T, typename Allocator>
  inline bool operator==(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
  {
    return ((a.size() == b.size())
        && (memcmp(a.data(), b.data(), (size_t) a.size() * sizeof(typename basic_string<T, Allocator>::value_type)) == 0));
  }

  template<typename T, typename Allocator>
  inline bool operator==(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
  {
    typedef typename basic_string<T, Allocator>::size_type size_type;
    const size_type n = (size_type) CharStrlen(p);
    return ((n == b.size()) && (memcmp(p, b.data(), (size_t) n * sizeof(*p)) == 0));
  }

  template<typename T, typename Allocator>
  inline bool operator==(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
  {
    typedef typename basic_string<T, Allocator>::size_type size_type;
    const size_type n = (size_type) CharStrlen(p);
    return ((a.size() == n) && (memcmp(a.data(), p, (size_t) n * sizeof(*p)) == 0));
  }

  template<typename T, typename Allocator>
  inline bool operator!=(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
  {
    return !(a == b);
  }

  template<typename T, typename Allocator>
  inline bool operator!=(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
  {
    return !(p == b);
  }

  template<typename T, typename Allocator>
  inline bool operator!=(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
  {
    return !(a == p);
  }

// Operator< (and also >, <=, and >=).
  template<typename T, typename Allocator>
  inline bool operator<(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
  {
    return basic_string<T, Allocator>::compare(a.begin(), a.end(), b.begin(), b.end()) < 0;
  }

  template<typename T, typename Allocator>
  inline bool operator<(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
  {
    typedef typename basic_string<T, Allocator>::size_type size_type;
    const size_type n = (size_type) CharStrlen(p);
    return basic_string<T, Allocator>::compare(p, p + n, b.begin(), b.end()) < 0;
  }

  template<typename T, typename Allocator>
  inline bool operator<(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
  {
    typedef typename basic_string<T, Allocator>::size_type size_type;
    const size_type n = (size_type) CharStrlen(p);
    return basic_string<T, Allocator>::compare(a.begin(), a.end(), p, p + n) < 0;
  }

  template<typename T, typename Allocator>
  inline bool operator>(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
  {
    return b < a;
  }

  template<typename T, typename Allocator>
  inline bool operator>(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
  {
    return b < p;
  }

  template<typename T, typename Allocator>
  inline bool operator>(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
  {
    return p < a;
  }

  template<typename T, typename Allocator>
  inline bool operator<=(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
  {
    return !(b < a);
  }

  template<typename T, typename Allocator>
  inline bool operator<=(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
  {
    return !(b < p);
  }

  template<typename T, typename Allocator>
  inline bool operator<=(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
  {
    return !(p < a);
  }

  template<typename T, typename Allocator>
  inline bool operator>=(const basic_string<T, Allocator>& a, const basic_string<T, Allocator>& b)
  {
    return !(a < b);
  }

  template<typename T, typename Allocator>
  inline bool operator>=(const typename basic_string<T, Allocator>::value_type* p, const basic_string<T, Allocator>& b)
  {
    return !(p < b);
  }

  template<typename T, typename Allocator>
  inline bool operator>=(const basic_string<T, Allocator>& a, const typename basic_string<T, Allocator>::value_type* p)
  {
    return !(a < p);
  }

  template<typename T, typename Allocator>
  inline void swap(basic_string<T, Allocator>& a, basic_string<T, Allocator>& b)
  {
    a.swap(b);
  }

/// string / wstring
  typedef basic_string<char> string;
  typedef basic_string<wchar_t> wstring;

/// string8 / string16 / string32
  typedef basic_string<char8_t> string8;
  typedef basic_string<char16_t> string16;
  typedef basic_string<char32_t> string32;

// C++11 string types
  typedef basic_string<char8_t> u8string; // Actually not a C++11 type, but added for consistency.
  typedef basic_string<char16_t> u16string;
  typedef basic_string<char32_t> u32string;

/// hash<string>
///
/// We provide EASTL hash function objects for use in hash table containers.
///
/// Example usage:
///    #include <EASTL/hash_set.h>
///    hash_set<string> stringHashSet;
///
  template<typename T> struct hash;

  template<>
  struct hash<string>
  {
    size_t operator()(const string& x) const
    {
      const unsigned char* p = (const unsigned char*) x.c_str(); // To consider: limit p to at most 256 chars.
      unsigned int c, result = 2166136261U; // We implement an FNV-like string hash.
      while ((c = *p++) != 0) // Using '!=' disables compiler warnings.
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

  template<>
  struct hash<string16>
  {
    size_t operator()(const string16& x) const
    {
      const char16_t* p = x.c_str();
      unsigned int c, result = 2166136261U;
      while ((c = *p++) != 0)
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

  template<>
  struct hash<string32>
  {
    size_t operator()(const string32& x) const
    {
      const char32_t* p = x.c_str();
      unsigned int c, result = 2166136261U;
      while ((c = (unsigned int) *p++) != 0)
        result = (result * 16777619) ^ c;
      return (size_t) result;
    }
  };

}
// namespace eastl

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // Header include guard
