///////////////////////////////////////////////////////////////////////////////
// This file is based on eastl::fixed_substring from the Electronic Arts EASTL library.
//////////////////////////////////////////////////////////////////////////////

#ifndef FLEX_STRING_REF_H
#define FLEX_STRING_REF_H

#include <flex/string.h>

namespace flex
{

  /// basic_string_ref
  ///
  /// Implements a string which is a reference to a segment of characters.
  /// This class is efficient because it allocates no memory and copies no
  /// memory during construction and assignment, but rather refers directly
  /// to the segment of chracters. A common use of this is to have a
  /// basic_string_ref efficiently refer to a substring within another string.
  ///
  /// You cannot directly resize a basic_string_ref (e.g. via resize, insert,
  /// append, erase), but you can assign a different substring to it.
  /// You can modify the characters within a substring in place.
  /// As of this writing, in the name of being lean and simple it is the
  /// user's responsibility to not call unsupported resizing functions
  /// such as those listed above. A detailed listing of the functions which
  /// are not supported is given below in the class declaration.
  ///
  /// The c_str function doesn't act as one might hope, as it simply
  /// returns the pointer to the beginning of the string segment and the
  /// 0-terminator may be beyond the end of the segment. If you want to
  /// always be able to use c_str as expected, use the fixed string solution
  /// we describe below.
  ///
  /// Another use of basic_string_ref is to provide C++ string-like functionality
  /// with a C character array. This allows you to work on a C character array
  /// as if it were a C++ string as opposed using the C string API. Thus you
  /// can do this:
  ///
  ///    void DoSomethingForUser(char* timeStr, size_t timeStrCapacity)
  ///    {
  ///        basic_string_ref tmp(timeStr, timeStrCapacity);
  ///        tmp  = "hello ";
  ///        tmp += "world";
  ///    }
  ///
  /// Note that this class constructs and assigns from const string pointers
  /// and const string objects, yet this class does not declare its member
  /// data as const. This is a concession in order to allow this implementation
  /// to be simple and lean. It is the user's responsibility to make sure
  /// that strings that should not or can not be modified are either not
  /// used by basic_string_ref or are not modified by basic_string_ref.
  ///
  /// A more flexible alternative to basic_string_ref is fixed_string.
  /// fixed_string has none of the functional limitations that basic_string_ref
  /// has and like basic_string_ref it doesn't allocate memory. However,
  /// fixed_string makes a *copy* of the source string and uses local
  /// memory to store that copy. Also, fixed_string objects on the stack
  /// are going to have a limit as to their maximum size.
  ///
  /// Notes:
  ///     As of this writing, the string class necessarily reallocates when
  ///     an insert of self is done into self. As a result, the basic_string_ref
  ///     class doesn't support inserting self into self.
  ///
  /// Example usage:
  ///     basic_string<char>    str("hello world");
  ///     basic_string_ref<char> sub(str, 2, 5);      // sub == "llo w"
  ///
  template<typename T>
  class basic_string_ref: public basic_string<T>
  {
  public:
    typedef basic_string<T> base_type;
    typedef basic_string_ref<T> this_type;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    using base_type::npos;
    using base_type::mFixed;
    using base_type::mBegin;
    using base_type::mEnd;
    using base_type::mCapacity;

  public:
    basic_string_ref() :
        base_type()
    {
      mFixed = true;
    }

    basic_string_ref(const base_type& x) :
        base_type(const_cast<value_type*>(x.data()), x.size(), true)
    {
    }

    // We gain no benefit from having an rvalue move constructor or assignment operator,
    // as this class is a const class.

    basic_string_ref(const base_type& x, size_type position, size_type n = base_type::npos) :
        base_type(const_cast<value_type*>(x.data()) + position, (n > (x.size() - position)) ? (x.size() - position) : n,
            true)
    {
    }

    basic_string_ref(const value_type* p, size_type n) :
        base_type(const_cast<value_type*>(p), n, true)
    {
    }

    basic_string_ref(const value_type* p) :
        base_type(const_cast<value_type*>(p), CharStrlen(p), true)
    {
    }

    basic_string_ref(const value_type* pBegin, const value_type* pEnd) :
        base_type(const_cast<value_type*>(pBegin), pEnd - pBegin, true)
    {
    }

    this_type& operator=(const base_type& x)
    {
      assign(x);
      return *this;
    }

    this_type& operator=(const value_type* p)
    {
      assign(p);
      return *this;
    }

    this_type& assign(const base_type& x)
    {
      // By design, we need to cast away const-ness here.
      mBegin = const_cast<value_type*>(x.data());
      mEnd = mBegin + x.size();
      mCapacity = mEnd;
      return *this;
    }

    this_type& assign(const base_type& x, size_type position, size_type n)
    {
      if (n > (x.size() - position))      //Need to handle case when n == ::npos
      {
        n = x.size() - position;
      }

      // By design, we need to cast away const-ness here.
      mBegin = const_cast<value_type*>(x.data()) + position;
      mEnd = mBegin + n;
      mCapacity = mEnd;
      return *this;
    }

    this_type& assign(const value_type* p, size_type n)
    {
      // By design, we need to cast away const-ness here.
      mBegin = const_cast<value_type*>(p);
      mEnd = mBegin + n;
      mCapacity = mEnd;
      return *this;
    }

    this_type& assign(const value_type* p)
    {
      // By design, we need to cast away const-ness here.
      mBegin = const_cast<value_type*>(p);
      mEnd = mBegin + CharStrlen(p);
      mCapacity = mEnd;
      return *this;
    }

    this_type& assign(const value_type* pBegin, const value_type* pEnd)
    {
      // By design, we need to cast away const-ness here.
      mBegin = const_cast<value_type*>(pBegin);
      mEnd = const_cast<value_type*>(pEnd);
      mCapacity = mEnd;
      return *this;
    }

    // Partially supported functionality
    //
    // When using basic_string_ref on a character sequence that is within another
    // string, the following functions may do one of two things:
    //     1 Attempt to reallocate
    //     2 Write a 0 char at the end of the basic_string_ref
    //
    // Item #1 will result in a crash, due to the attempt by the underlying
    // string class to free the substring memory. Item #2 will result in a 0
    // char being written to the character array. Item #2 may or may not be
    // a problem, depending on how you use basic_string_ref. Thus the following
    // functions cannot be used safely.

#if FLEX_HAS_CXX11
    this_type& operator=(value_type c) = delete;
    void resize(size_type n, value_type c) = delete;
    void resize(size_type n) = delete;
    void reserve(size_type = 0) = delete;
    void set_capacity(size_type n) = delete;
    void clear() = delete;
    this_type& operator+=(const base_type& x) = delete;
    this_type& operator+=(const value_type* p) = delete;
    this_type& operator+=(value_type c) = delete;
    this_type& append(const base_type& x) = delete;
    this_type& append(const base_type& x, size_type position, size_type n) = delete;
    this_type& append(const value_type* p, size_type n) = delete;
    this_type& append(const value_type* p) = delete;
    this_type& append(size_type n) = delete;
    this_type& append(size_type n, value_type c) = delete;
    this_type& append(const value_type* pBegin, const value_type* pEnd) = delete;
    void push_back(value_type c) = delete;
    void pop_back() = delete;
    this_type& assign(size_type n, value_type c) = delete;
    this_type& insert(size_type position, const base_type& x) = delete;
    this_type& insert(size_type position, const base_type& x, size_type beg, size_type n) = delete;
    this_type& insert(size_type position, const value_type* p, size_type n) = delete;
    this_type& insert(size_type position, const value_type* p) = delete;
    this_type& insert(size_type position, size_type n, value_type c) = delete;
    iterator insert(const_iterator p, value_type c) = delete;
    void insert(const_iterator p, size_type n, value_type c) = delete;
    void insert(const_iterator p, const value_type* pBegin, const value_type* pEnd) = delete;
    this_type& erase(size_type position = 0, size_type n = npos) = delete;
    iterator erase(const_iterator p) = delete;
    iterator erase(const_iterator pBegin, const_iterator pEnd) = delete;
    void swap(base_type& x) = delete;
#endif

  };
// basic_string_ref

  /// string / wstring
  typedef basic_string_ref<char> string_ref;
  typedef basic_string_ref<wchar_t> wstring_ref;

  /// string8 / string16 / string32
  typedef basic_string_ref<char8_t> string8_ref;
  typedef basic_string_ref<char16_t> string16_ref;
  typedef basic_string_ref<char32_t> string32_ref;

  // C++11 string types
  typedef basic_string_ref<char8_t> u8string_ref; // Actually not a C++11 type, but added for consistency.
  typedef basic_string_ref<char16_t> u16string_ref;
  typedef basic_string_ref<char32_t> u32string_ref;

} // namespace flex

#endif // Header include guard

