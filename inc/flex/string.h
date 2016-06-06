///////////////////////////////////////////////////////////////////////////////
// This file is based on eastl::string from the Electronic Arts EASTL library.
// The main changes between this version and eastl::string include:
// - eastl library methods were replaced with std methods
// - asserts and throws were adapted to the FLEX model
// -
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
//    - basic_string has less deeply nested function calls and allows the user to
//      enable forced inlining in debug builds in order to reduce bloat.
//    - basic_string doesn't use char traits.
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

#include <flex/allocator.h>
#include <flex/initializer_list.h>

#include <algorithm>
#include <iterator>

#include <stddef.h>             // size_t, ptrdiff_t, etc.
#include <stdlib.h>             // malloc, free.
#include <ctype.h>              // toupper, etc.
#include <wchar.h>
#include <stdint.h>
#include <string.h> // strlen, etc.

typedef char char8_t;
#ifdef FLEX_HAS_CXX11
typedef <uchar.h>
#else
typedef uint16_t char16_t;
typedef uint32_t char32_t;
#endif


///////////////////////////////////////////////////////////////////////////////
// FLEX_STRING_INITIAL_CAPACITY
//
// As of this writing, this must be > 0. Note that an initially empty string
// has a capacity of zero (it allocates no memory).
//
const size_t FLEX_STRING_INITIAL_CAPACITY = 8;
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

namespace flex
{

  /// gEmptyString
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

  static EmptyString gEmptyString = { 0 };

  inline static const signed char* GetEmptyString(signed char)
  {
    return gEmptyString.mEmptyS8;
  }
  inline static const unsigned char* GetEmptyString(unsigned char)
  {
    return gEmptyString.mEmptyU8;
  }
  inline static const char* GetEmptyString(char)
  {
    return gEmptyString.mEmpty8;
  }
  inline static const char16_t* GetEmptyString(char16_t)
  {
    return gEmptyString.mEmpty16;
  }
  inline static const char32_t* GetEmptyString(char32_t)
  {
    return gEmptyString.mEmpty32;
  }

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
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Allocator allocator_type;

    static const size_type npos = (size_type) -1; /// 'npos' means non-valid position or simply non-position.
    static const size_type kMaxSize = (size_type) -2; /// -1 is reserved for 'npos'. It also happens to be slightly beneficial that kMaxSize is a value less than -1, as it helps us deal with potential integer wraparound issues.

  public:
    // CtorDoNotInitialize exists so that we can create a constructor that allocates but doesn't
    // initialize and also doesn't collide with any other constructor declaration.
    struct CtorDoNotInitialize
    {
    };

  protected:
    value_type* mBegin; // Begin of string.
    value_type* mEnd; // End of string. *mEnd is always '0', as we 0-terminate our string. mEnd is always < mCapacity.
    value_type* mCapacity; // End of allocated space, including the space needed to store the trailing '0' char. mCapacity is always at least mEnd + 1. To consider: rename this to mpAllocEnd, thus avoiding confusion with the public capacity() function.
    allocator_type mAllocator; // To do: Use base class optimization to make this go away.
    bool mFixed;

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

    // Iterators.
    iterator begin() FLEX_NOEXCEPT; // Expanded in source code as: mBegin
    const_iterator begin() const FLEX_NOEXCEPT; // Expanded in source code as: mBegin
    const_iterator cbegin() const FLEX_NOEXCEPT;

    iterator end() FLEX_NOEXCEPT; // Expanded in source code as: mEnd
    const_iterator end() const FLEX_NOEXCEPT; // Expanded in source code as: mEnd
    const_iterator cend() const FLEX_NOEXCEPT;

    reverse_iterator rbegin() FLEX_NOEXCEPT;
    const_reverse_iterator rbegin() const FLEX_NOEXCEPT;
    const_reverse_iterator crbegin() const FLEX_NOEXCEPT;

    reverse_iterator rend() FLEX_NOEXCEPT;
    const_reverse_iterator rend() const FLEX_NOEXCEPT;
    const_reverse_iterator crend() const FLEX_NOEXCEPT;

    // Size-related functionality
    bool empty() const FLEX_NOEXCEPT; // Expanded in source code as: (mBegin == mEnd) or (mBegin != mEnd)
    size_type size() const FLEX_NOEXCEPT; // Expanded in source code as: (size_type)(mEnd - mBegin)
    size_type length() const FLEX_NOEXCEPT; // Expanded in source code as: (size_type)(mEnd - mBegin)
    size_type max_size() const FLEX_NOEXCEPT; // Expanded in source code as: kMaxSize
    size_type capacity() const FLEX_NOEXCEPT; // Expanded in source code as: (size_type)((mCapacity - mBegin) - 1). Thus thus returns the max strlen the container can currently hold without resizing.
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
    this_type& operator+=(std::initializer_list<value_type> x);
    this_type& operator+=(const value_type* p);
    this_type& operator+=(value_type c);

    this_type& append(const this_type& x);
    this_type& append(const this_type& x, size_type position, size_type n);
    this_type& append(const value_type* p, size_type n);
    this_type& append(const value_type* p);
    this_type& append(size_type n, value_type c);
    this_type& append(const value_type* pBegin, const value_type* pEnd);
    this_type& append(std::initializer_list<value_type> x);

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

    //Replacement operations
    this_type& replace(size_type position, size_type n, const this_type& x);
    this_type& replace(size_type pos1, size_type n1, const this_type& x, size_type pos2, size_type n2);
    this_type& replace(size_type position, size_type n1, const value_type* p, size_type n2);
    this_type& replace(size_type position, size_type n1, const value_type* p);
    this_type& replace(size_type position, size_type n1, size_type n2, value_type c);
    this_type& replace(const_iterator first, const_iterator last, const this_type& x);
    this_type& replace(const_iterator first, const_iterator last, std::initializer_list<value_type> x);
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

    bool validate() const FLEX_NOEXCEPT;

  protected:
    basic_string(pointer new_begin, pointer new_end, size_type capacity);
    basic_string(pointer new_begin, size_type size, bool fixed);

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

    // Replacements for STL template functions.
    static const value_type* CharTypeStringFindEnd(const value_type* pBegin, const value_type* pEnd, value_type c);
    static const value_type* CharTypeStringRFind(const value_type* pRBegin, const value_type* pREnd,
        const value_type c);
    static const value_type* CharTypeStringRSearch(const value_type* p1Begin, const value_type* p1End,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringRFindFirstOf(const value_type* p1RBegin, const value_type* p1REnd,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringFindFirstNotOf(const value_type* p1Begin, const value_type* p1End,
        const value_type* p2Begin, const value_type* p2End);
    static const value_type* CharTypeStringRFindFirstNotOf(const value_type* p1RBegin, const value_type* p1REnd,
        const value_type* p2Begin, const value_type* p2End);

  };
// basic_string

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
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(), mFixed(false)
  {
    AllocateSelf();
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const allocator_type& allocator) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    AllocateSelf();
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const this_type& x) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(x.mAllocator), mFixed(false)
  {
    RangeInitialize(x.mBegin, x.mEnd);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(const this_type& x, size_type position, size_type n) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(x.mAllocator), mFixed(false)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(x.mEnd - x.mBegin),
        "flex::basic_string - substring constructor has invalid position"); // 21.4.2 p4

    RangeInitialize(x.mBegin + position, x.mBegin + position + std::min(n, (size_type) (x.mEnd - x.mBegin) - position));
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const value_type* p, size_type n, const allocator_type& allocator) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    RangeInitialize(p, p + n);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(const value_type* p, const allocator_type& allocator) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    RangeInitialize(p);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(size_type n, value_type c, const allocator_type& allocator) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    SizeInitialize(n, c);
  }

  template<typename T, typename Allocator>
  template<class InputIterator>
  inline basic_string<T, Allocator>::basic_string(InputIterator pBegin, InputIterator pEnd,
      const allocator_type& allocator) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    RangeInitialize(pBegin, pEnd);
  }

// CtorDoNotInitialize exists so that we can create a version that allocates but doesn't
// initialize but also doesn't collide with any other constructor declaration.
  template<typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(CtorDoNotInitialize /*unused*/, size_type n, const allocator_type& allocator) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    // Note that we do not call SizeInitialize here.
    AllocateSelf(n + 1); // '+1' so that we have room for the terminating 0.
    *mEnd = 0;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(std::initializer_list<value_type> init, const allocator_type& allocator) :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    RangeInitialize(init.begin(), init.end());
  }

#if FLEX_HAS_CXX11
  template <typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(this_type&& x) :
  mBegin(NULL),mEnd(NULL), mCapacity(NULL), mFixed(false)
  {
    if(!mFixed && !x.mFixed) // If we can borrow from x...
    {
      mBegin = x.mBegin; // It's OK if x.mBegin is NULL.
      mEnd = x.mEnd;
      mCapacity = x.mCapacity;
      x.AllocateSelf();
    }
    else if(x.mBegin)
    {
      RangeInitialize(x.mBegin, x.mEnd);
      // Let x destruct its own items.
    }
  }

  template <typename T, typename Allocator>
  basic_string<T, Allocator>::basic_string(this_type&& x, const allocator_type& allocator) :
  mBegin(NULL), mEnd(NULL), mCapacity(NULL), mAllocator(allocator), mFixed(false)
  {
    if(!mFixed && !x.mFixed) // If we can borrow from x...
    {
      mBegin = x.mBegin; // It's OK if x.mBegin is NULL.
      mEnd = x.mEnd;
      mCapacity = x.mCapacity;
      x.AllocateSelf();
    }
    else if(x.mBegin)
    {
      RangeInitialize(x.mBegin, x.mEnd);
      // Let x destruct its own items.
    }
  }
#endif

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::~basic_string()
  {
    if (!mFixed)
    {
      DeallocateSelf();
    }
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
    return mBegin;
  }

  template<typename T, typename Allocator>
  inline const typename basic_string<T, Allocator>::value_type*
  basic_string<T, Allocator>::c_str() const FLEX_NOEXCEPT
  {
    return mBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::begin() FLEX_NOEXCEPT
  {
    return mBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::end() FLEX_NOEXCEPT
  {
    return mEnd;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::begin() const FLEX_NOEXCEPT
  {
    return mBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::cbegin() const FLEX_NOEXCEPT
  {
    return mBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::end() const FLEX_NOEXCEPT
  {
    return mEnd;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_iterator basic_string<T, Allocator>::cend() const FLEX_NOEXCEPT
  {
    return mEnd;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reverse_iterator basic_string<T, Allocator>::rbegin() FLEX_NOEXCEPT
  {
    return reverse_iterator(mEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reverse_iterator basic_string<T, Allocator>::rend() FLEX_NOEXCEPT
  {
    return reverse_iterator(mBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::rbegin() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::crbegin() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::rend() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reverse_iterator basic_string<T, Allocator>::crend() const FLEX_NOEXCEPT
  {
    return const_reverse_iterator(mBegin);
  }

  template<typename T, typename Allocator>
  inline bool basic_string<T, Allocator>::empty() const FLEX_NOEXCEPT
  {
    return (mBegin == mEnd);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::size() const FLEX_NOEXCEPT
  {
    return (size_type) (mEnd - mBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::length() const FLEX_NOEXCEPT
  {
    return (size_type) (mEnd - mBegin);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::max_size() const FLEX_NOEXCEPT
  {
    return kMaxSize;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::capacity() const FLEX_NOEXCEPT
  {
    return (size_type) ((mCapacity - mBegin) - 1); // '-1' because we pretend that we didn't allocate memory for the terminating 0.
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::operator[](size_type n) const
  {
    // We allow the user to reference the trailing 0 char without asserting. Perhaps we shouldn't.
    FLEX_ASSERT(n <= (static_cast<size_type>(mEnd - mBegin)));
    return mBegin[n]; // Sometimes done as *(mBegin + n)
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::operator[](size_type n)
  {
    // We allow the user to reference the trailing 0 char without asserting. Perhaps we shouldn't.
    FLEX_ASSERT(n <= (static_cast<size_type>(mEnd - mBegin)));
    return mBegin[n]; // Sometimes done as *(mBegin + n)
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::this_type& basic_string<T, Allocator>::operator=(const this_type& x)
  {
    if (&x != this)
    {
      assign(x.mBegin, x.mEnd);
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
    const size_type s = (size_type) (mEnd - mBegin);

    if (n < s)
      erase(mBegin + n, mEnd);
    else if (n > s)
      append(n - s, c);
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::resize(size_type n)
  {
    const size_type s = (size_type) (mEnd - mBegin);

    if (n < s)
      erase(mBegin + n, mEnd);
    else if (n > s)
    {
      append(n - s, value_type());
    }
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::reserve(size_type n)
  {
    FLEX_THROW_LENGTH_ERROR_IF(n > kMaxSize, "flex::basic_string - reserve() invalid length");

    // The C++ standard for basic_string doesn't specify if we should or shouldn't
    // downsize the container. The standard is overly vague in its description of reserve:
    //    The member function reserve() is a directive that informs a
    //    basic_string object of a planned change in size, so that it
    //    can manage the storage allocation accordingly.
    // We will act like the vector container and preserve the contents of
    // the container and only reallocate if increasing the size. The user
    // can use the set_capacity function to reduce the capacity.

    n = std::max(n, (size_type) (mEnd - mBegin)); // Calculate the new capacity, which needs to be >= container size.

    if (n >= (size_type) (mCapacity - mBegin)) // If there is something to do... // We use >= because mCapacity accounts for the trailing zero.
      set_capacity(n);
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::set_capacity(size_type n)
  {
    if (n == npos) // If the user wants to set the capacity to equal the current size... // '-1' because we pretend that we didn't allocate memory for the terminating 0.
      n = (size_type) (mEnd - mBegin);
    else if (n < (size_type) (mEnd - mBegin))
      mEnd = mBegin + n;

    if (n != (size_type) ((mCapacity - mBegin) - 1)) // If there is any capacity change...
    {
      if (n)
      {
        pointer pNewBegin = DoAllocate(n + 1); // We need the + 1 to accomodate the trailing 0.
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mBegin, mEnd, pNewBegin);
        *pNewEnd = 0;

        DeallocateSelf();
        mBegin = pNewBegin;
        mEnd = pNewEnd;
        mCapacity = pNewBegin + (n + 1);
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
    if (!mFixed)
    {
      size_type n = size();
      if (n != capacity()) // If there is any capacity change...
      {
        if (n)
        {
          pointer pNewBegin = DoAllocate(n + 1); // We need the + 1 to accomodate the trailing 0.
          pointer pNewEnd = pNewBegin;

          pNewEnd = CharStringUninitializedCopy(mBegin, mEnd, pNewBegin);
          *pNewEnd = 0;

          DeallocateSelf();
          mBegin = pNewBegin;
          mEnd = pNewEnd;
          mCapacity = pNewBegin + (n + 1);
        }
        else
        {
          DeallocateSelf();
          AllocateSelf();
        }
      }
    }
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::clear() FLEX_NOEXCEPT
  {
    if (mBegin != mEnd)
    {
      *mBegin = value_type(0);
      mEnd = mBegin;
    }
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::at(size_type n) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(n >= (size_type ) (mEnd - mBegin), "flex::string.at() - index out-of-bounds");
    return mBegin[n];
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::at(size_type n)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(n >= (size_type ) (mEnd - mBegin), "flex::string.at() - index out-of-bounds");
    return mBegin[n];
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::front()
  {
    FLEX_ASSERT(mBegin < mEnd);
    return *mBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::front() const
  {
    FLEX_ASSERT(mBegin < mEnd);
    return *mBegin;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::reference basic_string<T, Allocator>::back()
  {
    FLEX_ASSERT(mBegin < mEnd);
    return *(mEnd - 1);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::const_reference basic_string<T, Allocator>::back() const
  {
    FLEX_ASSERT(mBegin < mEnd);
    return *(mEnd - 1);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(const this_type& x)
  {
    return append(x);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::operator+=(std::initializer_list<value_type> x)
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
    return append(x.mBegin, x.mEnd);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(std::initializer_list<value_type> x)
  {
    return append(x.begin(), x.end());
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::append(const this_type& x, size_type position,
      size_type n)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(x.mEnd - x.mBegin), "basic_string -- out of range");
    return append(x.mBegin + position, x.mBegin + position + std::min(n, (size_type) (x.mEnd - x.mBegin) - position));
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
  basic_string<T, Allocator>& basic_string<T, Allocator>::append(size_type n, value_type c)
  {
    const size_type s = (size_type) (mEnd - mBegin);

    FLEX_THROW_LENGTH_ERROR_IF((n > kMaxSize) || (s > (kMaxSize - n)), "basic_string -- length_error");

    const size_type nCapacity = (size_type) ((mCapacity - mBegin) - 1);

    if ((s + n) > nCapacity)
      reserve(std::max((size_type) GetNewCapacity(nCapacity), (size_type) (s + n)));

    if (n > 0)
    {
      CharStringUninitializedFillN(mEnd + 1, n - 1, c);
      *mEnd = c;
      mEnd += n;
      *mEnd = 0;
    }

    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::append(const value_type* pBegin, const value_type* pEnd)
  {
    if (pBegin != pEnd)
    {
      const size_type nOldSize = (size_type) (mEnd - mBegin);
      const size_type n = (size_type) (pEnd - pBegin);

      FLEX_THROW_LENGTH_ERROR_IF(((size_t )n > kMaxSize) || (nOldSize > (kMaxSize - n)),
          "basic_string -- length_error");

      const size_type nCapacity = (size_type) ((mCapacity - mBegin) - 1);

      if ((nOldSize + n) > nCapacity)
      {
        const size_type nLength = std::max((size_type) GetNewCapacity(nCapacity), (size_type) (nOldSize + n)) + 1; // + 1 to accomodate the trailing 0.

        pointer pNewBegin = DoAllocate(nLength);
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mBegin, mEnd, pNewBegin);
        pNewEnd = CharStringUninitializedCopy(pBegin, pEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mBegin = pNewBegin;
        mEnd = pNewEnd;
        mCapacity = pNewBegin + nLength;
      }
      else
      {
        const value_type* pTemp = pBegin;
        ++pTemp;
        CharStringUninitializedCopy(pTemp, pEnd, mEnd + 1);
        mEnd[n] = 0;
        *mEnd = *pBegin;
        mEnd += n;
      }
    }

    return *this;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::push_back(value_type c)
  {
    if ((mEnd + 1) == mCapacity) // If we are out of space... (note that we test for + 1 because we have a trailing 0)
      reserve(std::max(GetNewCapacity((size_type) ((mCapacity - mBegin) - 1)), (size_type) (mEnd - mBegin) + 1));
    *mEnd++ = c;
    *mEnd = 0;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::pop_back()
  {
    FLEX_ASSERT(mBegin < mEnd);
    mEnd[-1] = value_type(0);
    --mEnd;
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const this_type& x)
  {
    // The C++11 Standard 21.4.6.3 p6 specifies that assign from this_type assigns contents only and not the allocator.
    return assign(x.mBegin, x.mEnd);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const this_type& x, size_type position,
      size_type n)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(x.mEnd - x.mBegin), "basic_string -- out of range");
    // The C++11 Standard 21.4.6.3 p6 specifies that assign from this_type assigns contents only and not the allocator.
    return assign(x.mBegin + position, x.mBegin + position + std::min(n, (size_type) (x.mEnd - x.mBegin) - position));
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
    if (n <= (size_type) (mEnd - mBegin))
    {
      CharTypeAssignN(mBegin, n, c);
      erase(mBegin + n, mEnd);
    }
    else
    {
      CharTypeAssignN(mBegin, (size_type) (mEnd - mBegin), c);
      append(n - (size_type) (mEnd - mBegin), c);
    }
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::assign(const value_type* pBegin, const value_type* pEnd)
  {
    const ptrdiff_t n = pEnd - pBegin;
    if (static_cast<size_type>(n) <= (size_type) (mEnd - mBegin))
    {
      memmove(mBegin, pBegin, (size_t) n * sizeof(value_type));
      erase(mBegin + n, mEnd);
    }
    else
    {
      memmove(mBegin, pBegin, (size_t) (mEnd - mBegin) * sizeof(value_type));
      append(pBegin + (size_type) (mEnd - mBegin), pEnd);
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
    if(!mFixed && !x.mFixed)
    {
      std::swap(mBegin, x.mBegin);
      std::swap(mEnd, x.mEnd);
      std::swap(mCapacity, x.mCapacity);
    }
    else
    assign(x.mBegin, x.mEnd);

    return *this;
  }
#endif

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const this_type& x)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    FLEX_THROW_LENGTH_ERROR_IF((size_type )(mEnd - mBegin) > (kMaxSize - (size_type )(x.mEnd - x.mBegin)),
        "basic_string -- length_error");
    insert(mBegin + position, x.mBegin, x.mEnd);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const this_type& x, size_type beg,
      size_type n)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    size_type nLength = std::min(n, (size_type) (x.mEnd - x.mBegin) - beg);
    FLEX_THROW_LENGTH_ERROR_IF((size_type )(mEnd - mBegin) > (kMaxSize - nLength), "basic_string -- length_error");
    insert(mBegin + position, x.mBegin + beg, x.mBegin + beg + nLength);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const value_type* p, size_type n)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    FLEX_THROW_LENGTH_ERROR_IF((size_type )(mEnd - mBegin) > (kMaxSize - n), "basic_string -- length_error");
    insert(mBegin + position, p, p + n);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, const value_type* p)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    size_type nLength = (size_type) CharStrlen(p);
    FLEX_THROW_LENGTH_ERROR_IF((size_type )(mEnd - mBegin) > (kMaxSize - nLength), "basic_string -- length_error");
    insert(mBegin + position, p, p + nLength);
    return *this;
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::insert(size_type position, size_type n, value_type c)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    FLEX_THROW_LENGTH_ERROR_IF((size_type )(mEnd - mBegin) > (kMaxSize - n), "basic_string -- length_error");
    insert(mBegin + position, n, c);
    return *this;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::insert(const_iterator p,
      value_type c)
  {
    if (p == mEnd)
    {
      push_back(c);
      return mEnd - 1;
    }
    return InsertInternal(p, c);
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::insert(const_iterator p, size_type n,
      value_type c)
  {
    const ptrdiff_t nPosition = (p - mBegin); // Save this because we might reallocate.
    FLEX_ASSERT((mBegin <= p) && (p <= mEnd));

    if (n) // If there is anything to insert...
    {
      if (size_type(mCapacity - mEnd) >= (n + 1)) // If we have enough capacity...
      {
        const size_type nElementsAfter = (size_type) (mEnd - p);
        iterator pOldEnd = mEnd;

        if (nElementsAfter >= n) // If there's enough space for the new chars between the insert position and the end...
        {
          CharStringUninitializedCopy((mEnd - n) + 1, mEnd + 1, mEnd + 1);
          mEnd += n;
          memmove(const_cast<value_type*>(p) + n, p, (size_t) ((nElementsAfter - n) + 1) * sizeof(value_type));
          CharTypeAssignN(const_cast<value_type*>(p), n, c);
        }
        else
        {
          CharStringUninitializedFillN(mEnd + 1, n - nElementsAfter - 1, c);
          mEnd += n - nElementsAfter;

          CharStringUninitializedCopy(p, pOldEnd + 1, mEnd);
          mEnd += nElementsAfter;

          CharTypeAssignN(const_cast<value_type*>(p), nElementsAfter + 1, c);
        }
      }
      else
      {
        const size_type nOldSize = (size_type) (mEnd - mBegin);
        const size_type nOldCap = (size_type) ((mCapacity - mBegin) - 1);
        const size_type nLength = std::max((size_type) GetNewCapacity(nOldCap), (size_type) (nOldSize + n)) + 1; // + 1 to accomodate the trailing 0.

        iterator pNewBegin = DoAllocate(nLength);
        iterator pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mBegin, p, pNewBegin);
        pNewEnd = CharStringUninitializedFillN(pNewEnd, n, c);
        pNewEnd = CharStringUninitializedCopy(p, mEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mBegin = pNewBegin;
        mEnd = pNewEnd;
        mCapacity = pNewBegin + nLength;
      }
    }

    return mBegin + nPosition;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::insert(const_iterator p,
      const value_type* pBegin, const value_type* pEnd)
  {
    const ptrdiff_t nPosition = (p - mBegin); // Save this because we might reallocate.
    FLEX_ASSERT((mBegin <= p) || (p <= mEnd));

    const size_type n = (size_type) (pEnd - pBegin);

    if (n)
    {
      const bool bCapacityIsSufficient = ((mCapacity - mEnd) >= (difference_type) (n + 1));
      const bool bSourceIsFromSelf = ((pEnd >= mBegin) && (pBegin <= mEnd));

      // If bSourceIsFromSelf is true, then we reallocate. This is because we are
      // inserting ourself into ourself and thus both the source and destination
      // be modified, making it rather tricky to attempt to do in place. The simplest
      // resolution is to reallocate. To consider: there may be a way to implement this
      // whereby we don't need to reallocate or can often avoid reallocating.
      if (bCapacityIsSufficient && !bSourceIsFromSelf)
      {
        const ptrdiff_t nElementsAfter = (mEnd - p);
        iterator pOldEnd = mEnd;

        if (nElementsAfter >= (ptrdiff_t) n) // If the newly inserted characters entirely fit within the size of the original string...
        {
          memmove(mEnd + 1, mEnd - n + 1, (size_t) n * sizeof(value_type));
          mEnd += n;
          memmove(const_cast<value_type*>(p) + n, p, (size_t) ((nElementsAfter - n) + 1) * sizeof(value_type));
          memmove(const_cast<value_type*>(p), pBegin, (size_t) (pEnd - pBegin) * sizeof(value_type));
        }
        else
        {
          const value_type* const pMid = pBegin + (nElementsAfter + 1);

          memmove(mEnd + 1, pMid, (size_t) (pEnd - pMid) * sizeof(value_type));
          mEnd += n - nElementsAfter;

          memmove(mEnd, p, (size_t) (pOldEnd - p + 1) * sizeof(value_type));
          mEnd += nElementsAfter;

          memmove(const_cast<value_type*>(p), pBegin, (size_t) (pMid - pBegin) * sizeof(value_type));
        }
      }
      else // Else we need to reallocate to implement this.
      {
        const size_type nOldSize = (size_type) (mEnd - mBegin);
        const size_type nOldCap = (size_type) ((mCapacity - mBegin) - 1);
        size_type nLength;

        if (bCapacityIsSufficient) // If bCapacityIsSufficient is true, then bSourceIsFromSelf must be false.
          nLength = nOldSize + n + 1; // + 1 to accomodate the trailing 0.
        else
          nLength = std::max((size_type) GetNewCapacity(nOldCap), (size_type) (nOldSize + n)) + 1; // + 1 to accomodate the trailing 0.

        pointer pNewBegin = DoAllocate(nLength);
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mBegin, p, pNewBegin);
        pNewEnd = CharStringUninitializedCopy(pBegin, pEnd, pNewEnd);
        pNewEnd = CharStringUninitializedCopy(p, mEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mBegin = pNewBegin;
        mEnd = pNewEnd;
        mCapacity = pNewBegin + nLength;
      }
    }

    return mBegin + nPosition;
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
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");FLEX_ASSERT(position <= (size_type)(mEnd - mBegin));
    erase(mBegin + position, mBegin + position + std::min(n, (size_type) (mEnd - mBegin) - position));
    return *this;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::erase(const_iterator p)
  {
    FLEX_ASSERT((mBegin <= p) && (p < mEnd));
    memmove(const_cast<value_type*>(p), p + 1, (size_t) (mEnd - p) * sizeof(value_type));
    --mEnd;
    return const_cast<value_type*>(p);
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::iterator basic_string<T, Allocator>::erase(const_iterator pBegin,
      const_iterator pEnd)
  {
    FLEX_ASSERT((pBegin >= mBegin) && (pBegin <= mEnd) && (pEnd >= mBegin) && (pEnd <= mEnd) && (pEnd >= pBegin));
    if (pBegin != pEnd)
    {
      memmove(const_cast<value_type*>(pBegin), pEnd, (size_t) ((mEnd - pEnd) + 1) * sizeof(value_type));
      const iterator pNewEnd = (mEnd - (pEnd - pBegin));
      mEnd = pNewEnd;
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
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    const size_type nLength = std::min(n, (size_type) (mEnd - mBegin) - position);
    FLEX_THROW_LENGTH_ERROR_IF(((size_type )(mEnd - mBegin) - nLength) >= (kMaxSize - (size_type )(x.mEnd - x.mBegin)),
        "basic_string -- length_error");
    return replace(mBegin + position, mBegin + position + nLength, x.mBegin, x.mEnd);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type pos1, size_type n1, const this_type& x,
      size_type pos2, size_type n2)
  {
    FLEX_THROW_OUT_OF_RANGE_IF((pos1 > (size_type )(mEnd - mBegin)) || (pos2 > (size_type )(x.mEnd - x.mBegin)),
        "basic_string -- out of range");
    const size_type nLength1 = std::min(n1, (size_type) (mEnd - mBegin) - pos1);
    const size_type nLength2 = std::min(n2, (size_type) (x.mEnd - x.mBegin) - pos2);
    FLEX_THROW_LENGTH_ERROR_IF(((size_type )(mEnd - mBegin) - nLength1) >= (kMaxSize - nLength2),
        "basic_string -- length_error");
    return replace(mBegin + pos1, mBegin + pos1 + nLength1, x.mBegin + pos2, x.mBegin + pos2 + nLength2);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, const value_type* p,
      size_type n2)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    const size_type nLength = std::min(n1, (size_type) (mEnd - mBegin) - position);
    FLEX_THROW_LENGTH_ERROR_IF((n2 > kMaxSize) || (((size_type )(mEnd - mBegin) - nLength) >= (kMaxSize - n2)),
        "basic_string -- length_error");
    return replace(mBegin + position, mBegin + position + nLength, p, p + n2);
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, const value_type* p)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    const size_type nLength = std::min(n1, (size_type) (mEnd - mBegin) - position);

#ifndef FLEX_RELEASE
    const size_type n2 = (size_type) CharStrlen(p);
    if (FLEX_UNLIKELY((n2 > kMaxSize) || (((size_type )(mEnd - mBegin) - nLength) >= (kMaxSize - n2))))
      flex::throw_length_error("basic_string -- length_error");
#endif

    return replace(mBegin + position, mBegin + position + nLength, p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  basic_string<T, Allocator>& basic_string<T, Allocator>::replace(size_type position, size_type n1, size_type n2,
      value_type c)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    const size_type nLength = std::min(n1, (size_type) (mEnd - mBegin) - position);
    FLEX_THROW_LENGTH_ERROR_IF((n2 > kMaxSize) || ((size_type )(mEnd - mBegin) - nLength) >= (kMaxSize - n2),
        "basic_string -- length_error");
    return replace(mBegin + position, mBegin + position + nLength, n2, c);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd,
      const this_type& x)
  {
    return replace(pBegin, pEnd, x.mBegin, x.mEnd);
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>& basic_string<T, Allocator>::replace(const_iterator pBegin, const_iterator pEnd,
      std::initializer_list<value_type> x)
  {
    return replace(pBegin, pEnd, x.begin(), x.end());
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
    FLEX_ASSERT((pBegin >= mBegin) && (pBegin <= mEnd) && (pEnd >= mBegin) && (pEnd <= mEnd) && (pEnd >= pBegin));

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
    FLEX_ASSERT((pBegin1 >= mBegin) && (pBegin1 <= mEnd) && (pEnd1 >= mBegin) && (pEnd1 <= mEnd) && (pEnd1 >= pBegin1));
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
        const size_type nOldSize = (size_type) (mEnd - mBegin);
        const size_type nOldCap = (size_type) ((mCapacity - mBegin) - 1);
        const size_type nNewCapacity = std::max((size_type) GetNewCapacity(nOldCap),
            (size_type) (nOldSize + (nLength2 - nLength1))) + 1; // + 1 to accomodate the trailing 0.

        pointer pNewBegin = DoAllocate(nNewCapacity);
        pointer pNewEnd = pNewBegin;

        pNewEnd = CharStringUninitializedCopy(mBegin, pBegin1, pNewBegin);
        pNewEnd = CharStringUninitializedCopy(pBegin2, pEnd2, pNewEnd);
        pNewEnd = CharStringUninitializedCopy(pEnd1, mEnd, pNewEnd);
        *pNewEnd = 0;

        DeallocateSelf();
        mBegin = pNewBegin;
        mEnd = pNewEnd;
        mCapacity = pNewBegin + nNewCapacity;
      }
    }
    return *this;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::copy(value_type* p, size_type n,
      size_type position) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");

    // It is not clear from the C++ standard if 'p' destination pointer is allowed to
    // refer to memory from within the string itself. We assume so and use memmove
    // instead of memcpy until we find otherwise.
    const size_type nLength = std::min(n, (size_type) (mEnd - mBegin) - position);
    memmove(p, mBegin + position, (size_t) nLength * sizeof(value_type));
    return nLength;
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::swap(this_type& x)
  {
    if (!mFixed && !x.mFixed) // If allocators are equivalent...
    {
      // We leave mAllocator as-is.
      std::swap(mBegin, x.mBegin);
      std::swap(mEnd, x.mEnd);
      std::swap(mCapacity, x.mCapacity);
    }
    else // else swap the contents.
    {
      if (size() < x.size())
      {
        iterator it = std::swap_ranges(mBegin, mEnd, x.mBegin);
        append(it, x.mEnd);
        x.erase(it, x.mEnd);
      }
      else
      {
        iterator it = std::swap_ranges(x.mBegin, x.mEnd, mBegin);
        x.append(it, mEnd);
        erase(it, mEnd);
      }
    }
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return find(x.mBegin, position, (size_type) (x.mEnd - x.mBegin));
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
    if (FLEX_LIKELY(((npos - n) >= position) && (position + n) <= (size_type ) (mEnd - mBegin))) // If the range is valid...
    {
      const value_type* const pTemp = std::search(mBegin + position, mEnd, p, p + n);

      if ((pTemp != mEnd) || (n == 0))
        return (size_type) (pTemp - mBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    if (FLEX_LIKELY(position < (size_type )(mEnd - mBegin))) // If the position is valid...
    {
      const const_iterator pResult = std::find(mBegin + position, mEnd, c);

      if (pResult != mEnd)
        return (size_type) (pResult - mBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::rfind(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return rfind(x.mBegin, position, (size_type) (x.mEnd - x.mBegin));
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
    // Note that a search for a zero length string starting at position = end() returns end() and not npos.
    // Note by Paul Pedriana: I am not sure how this should behave in the case of n == 0 and position > size.
    // The standard seems to suggest that rfind doesn't act exactly the same as find in that input position
    // can be > size and the return value can still be other than npos. Thus, if n == 0 then you can
    // never return npos, unlike the case with find.
    const size_type nLength = (size_type) (mEnd - mBegin);

    if (FLEX_LIKELY(n <= nLength))
    {
      if (FLEX_LIKELY(n))
      {
        const const_iterator pEnd = mBegin + std::min(nLength - n, position) + n;
        const const_iterator pResult = CharTypeStringRSearch(mBegin, pEnd, p, p + n);

        if (pResult != pEnd)
          return (size_type) (pResult - mBegin);
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
    const size_type nLength = (size_type) (mEnd - mBegin);

    if (FLEX_LIKELY(nLength))
    {
      const value_type* const pEnd = mBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFind(pEnd, mBegin, c);

      if (pResult != mBegin)
        return (size_type) ((pResult - 1) - mBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_of(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return find_first_of(x.mBegin, position, (size_type) (x.mEnd - x.mBegin));
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
    if (FLEX_LIKELY((position < (size_type ) (mEnd - mBegin))))
    {
      const iterator pBegin = mBegin + position;
      const iterator pResult = std::find_first_of(pBegin, mEnd, p, p + n);

      if (pResult != mEnd)
        return (size_type) (pResult - mBegin);
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
    return find_last_of(x.mBegin, position, (size_type) (x.mEnd - x.mBegin));
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
    const size_type nLength = (size_type) (mEnd - mBegin);

    if (FLEX_LIKELY(nLength))
    {
      const value_type* const pEnd = mBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFindFirstOf(pEnd, mBegin, p, p + n);

      if (pResult != mBegin)
        return (size_type) ((pResult - 1) - mBegin);
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
    return find_first_not_of(x.mBegin, position, (size_type) (x.mEnd - x.mBegin));
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
    if (FLEX_LIKELY(position <= (size_type ) (mEnd - mBegin)))
    {
      const const_iterator pResult = CharTypeStringFindFirstNotOf(mBegin + position, mEnd, p, p + n);

      if (pResult != mEnd)
        return (size_type) (pResult - mBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_first_not_of(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    if (FLEX_LIKELY(position <= (size_type )(mEnd - mBegin)))
    {
// Todo: Possibly make a specialized version of CharTypeStringFindFirstNotOf(pBegin, pEnd, c).
      const const_iterator pResult = CharTypeStringFindFirstNotOf(mBegin + position, mEnd, &c, &c + 1);

      if (pResult != mEnd)
        return (size_type) (pResult - mBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_not_of(const this_type& x,
      size_type position) const FLEX_NOEXCEPT
  {
    return find_last_not_of(x.mBegin, position, (size_type) (x.mEnd - x.mBegin));
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
    const size_type nLength = (size_type) (mEnd - mBegin);

    if (FLEX_LIKELY(nLength))
    {
      const value_type* const pEnd = mBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, mBegin, p, p + n);

      if (pResult != mBegin)
        return (size_type) ((pResult - 1) - mBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::find_last_not_of(value_type c,
      size_type position) const FLEX_NOEXCEPT
  {
    const size_type nLength = (size_type) (mEnd - mBegin);

    if (FLEX_LIKELY(nLength))
    {
      // Todo: Possibly make a specialized version of CharTypeStringRFindFirstNotOf(pBegin, pEnd, c).
      const value_type* const pEnd = mBegin + std::min(nLength - 1, position) + 1;
      const value_type* const pResult = CharTypeStringRFindFirstNotOf(pEnd, mBegin, &c, &c + 1);

      if (pResult != mBegin)
        return (size_type) ((pResult - 1) - mBegin);
    }
    return npos;
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator> basic_string<T, Allocator>::substr(size_type position, size_type n) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    return basic_string(mBegin + position, mBegin + position + std::min(n, (size_type) (mEnd - mBegin) - position),
        mAllocator);
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(const this_type& x) const FLEX_NOEXCEPT
  {
    return compare(mBegin, mEnd, x.mBegin, x.mEnd);
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const this_type& x) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(pos1 > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    return compare(mBegin + pos1, mBegin + pos1 + std::min(n1, (size_type) (mEnd - mBegin) - pos1), x.mBegin, x.mEnd);
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const this_type& x, size_type pos2,
      size_type n2) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF((pos1 > (size_type )(mEnd - mBegin)) || (pos2 > (size_type )(x.mEnd - x.mBegin)),
        "basic_string -- out of range");
    return compare(mBegin + pos1, mBegin + pos1 + std::min(n1, (size_type) (mEnd - mBegin) - pos1), x.mBegin + pos2,
        x.mBegin + pos2 + std::min(n2, (size_type) (x.mEnd - x.mBegin) - pos2));
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(const value_type* p) const
  {
    return compare(mBegin, mEnd, p, p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const value_type* p) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(pos1 > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    return compare(mBegin + pos1, mBegin + pos1 + std::min(n1, (size_type) (mEnd - mBegin) - pos1), p,
        p + CharStrlen(p));
  }

  template<typename T, typename Allocator>
  inline int basic_string<T, Allocator>::compare(size_type pos1, size_type n1, const value_type* p, size_type n2) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(pos1 > (size_type )(mEnd - mBegin), "basic_string -- out of range");
    return compare(mBegin + pos1, mBegin + pos1 + std::min(n1, (size_type) (mEnd - mBegin) - pos1), p, p + n2);
  }

// make_lower
// This is a very simple ASCII-only case conversion function
// Anything more complicated should use a more powerful separate library.
  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::make_lower()
  {
    for (pointer p = mBegin; p < mEnd; ++p)
      *p = (value_type) CharToLower(*p);
  }

// make_upper
// This is a very simple ASCII-only case conversion function
// Anything more complicated should use a more powerful separate library.
  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::make_upper()
  {
    for (pointer p = mBegin; p < mEnd; ++p)
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

    if ((mEnd + 1) < mCapacity)
    {
      *(mEnd + 1) = 0;
      memmove(const_cast<value_type*>(p) + 1, p, (size_t) (mEnd - p) * sizeof(value_type));
      *pNewPosition = c;
      ++mEnd;
    }
    else
    {
      const size_type nOldSize = (size_type) (mEnd - mBegin);
      const size_type nOldCap = (size_type) ((mCapacity - mBegin) - 1);
      const size_type nLength = std::max((size_type) GetNewCapacity(nOldCap), (size_type) (nOldSize + 1)) + 1; // The second + 1 is to accomodate the trailing 0.

      iterator pNewBegin = DoAllocate(nLength);
      iterator pNewEnd = pNewBegin;

      pNewPosition = CharStringUninitializedCopy(mBegin, p, pNewBegin);
      *pNewPosition = c;

      pNewEnd = pNewPosition + 1;
      pNewEnd = CharStringUninitializedCopy(p, mEnd, pNewEnd);
      *pNewEnd = 0;

      DeallocateSelf();
      mBegin = pNewBegin;
      mEnd = pNewEnd;
      mCapacity = pNewBegin + nLength;
    }
    return pNewPosition;
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::SizeInitialize(size_type n, value_type c)
  {
    AllocateSelf((size_type) (n + 1)); // '+1' so that we have room for the terminating 0.

    mEnd = CharStringUninitializedFillN(mBegin, n, c);
    *mEnd = 0;
  }

  template<typename T, typename Allocator>
  template<class InputIterator>
  void basic_string<T, Allocator>::RangeInitialize(InputIterator pBegin, InputIterator pEnd)
  {
    FLEX_THROW_INVALID_ARGUMENT_IF(!pBegin && (pEnd < pBegin), "basic_string -- invalid argument"); // 21.4.2 p7

    const size_type n = (size_type) (pEnd - pBegin);

    AllocateSelf((size_type) (n + 1)); // '+1' so that we have room for the terminating 0.

    mEnd = CharStringUninitializedCopy(pBegin, pEnd, mBegin);
    *mEnd = 0;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::RangeInitialize(const value_type* pBegin)
  {
    FLEX_THROW_INVALID_ARGUMENT_IF(!pBegin, "basic_string -- invalid argument");
    RangeInitialize(pBegin, pBegin + CharStrlen(pBegin));
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(pointer new_begin, pointer new_end, size_type capacity) :
      mBegin(new_begin), mEnd(new_end), mCapacity(mBegin + capacity), mFixed(true)
  {
#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY((mEnd + 1) > mCapacity))
    {
      size_type n = new_end - new_begin;
      mFixed = false;
      mBegin = DoAllocate(n + 1); //We need +1 to account for null terminator
      mEnd = mCapacity = mBegin + n;

      flex::error_msg("flex::fixed_string - constructor() size exceeds capacity");
    }
#endif
  }

  template<typename T, typename Allocator>
  inline basic_string<T, Allocator>::basic_string(pointer new_begin, size_type n, bool fixed) :
      mBegin(new_begin), mEnd(new_begin + n), mCapacity(mEnd), mFixed(fixed)
  {

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
#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(mFixed))
    {
      mFixed = false;
      flex::error_msg("flex::fixed_string - capacity exceeded");
    }
    else
#endif
    if (p)
      mAllocator.deallocate(p, n);
  }

  template<typename T, typename Allocator>
  inline typename basic_string<T, Allocator>::size_type basic_string<T, Allocator>::GetNewCapacity(
      size_type currentCapacity) // This needs to return a value of at least currentCapacity and at least 1.
  {
    return (currentCapacity > FLEX_STRING_INITIAL_CAPACITY) ? (2 * currentCapacity) : FLEX_STRING_INITIAL_CAPACITY;
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::AllocateSelf()
  {
    FLEX_ASSERT(gEmptyString.mUint32 == 0);
    mBegin = const_cast<value_type*>(GetEmptyString(value_type())); // In const_cast-int this, we promise not to modify it.
    mEnd = mBegin;
    mCapacity = mBegin + 1; // When we are using gEmptyString, mCapacity is always mEnd + 1. This is an important distinguising characteristic.
  }

  template<typename T, typename Allocator>
  void basic_string<T, Allocator>::AllocateSelf(size_type n)
  {
    FLEX_ASSERT(n < 0x40000000);
    FLEX_THROW_LENGTH_ERROR_IF(n > kMaxSize, "basic_string -- length_error");

    if (n > 1)
    {
      mBegin = DoAllocate(n);
      mEnd = mBegin;
      mCapacity = mBegin + n;
    }
    else
      AllocateSelf();
  }

  template<typename T, typename Allocator>
  inline void basic_string<T, Allocator>::DeallocateSelf()
  {
    // Note that we compare mCapacity to mEnd instead of comparing
    // mBegin to &gEmptyString. This is important because we may have
    // a case whereby one library passes a string to another library to
    // deallocate and the two libraries have independent versions of gEmptyString.
    if ((mCapacity - mBegin) > 1) // If we are not using gEmptyString as our memory...
      DoFree(mBegin, (size_type) (mCapacity - mBegin));
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
    if ((mBegin == NULL) || (mEnd == NULL))
      return false;
    if (mEnd < mBegin)
      return false;
    if (mCapacity < mEnd)
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

  // hash<string>
  //
  // Defined hash functors that can be used in hashed containers.  These hash functions currently use a FNV hash.
  template<typename T> struct hash;

  template<>
  struct hash<string>
  {
    size_t operator()(const string& x) const
    {
      const unsigned char* p = (const unsigned char*) x.c_str(); // To consider: limit p to at most 256 chars.
      unsigned int c, result = 2166136261U; // We implement an FNV-like string hash.
      while ((c = *p++) != 0) // Using '!=' disables compiler warnings.
        result = (result ^ c) * 16777619;
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
        result = (result ^ c) * 16777619;
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
        result = (result ^ c) * 16777619;
      return (size_t) result;
    }
  };

}
// namespace flex

#endif // Header include guard
