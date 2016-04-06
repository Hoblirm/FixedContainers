#ifndef FLEX_CONFIG_H
#define FLEX_CONFIG_H

#ifndef FLEX_LIKELY

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define FLEX_LIKELY(x)   __builtin_expect(!!(x), true)
#define FLEX_UNLIKELY(x) __builtin_expect(!!(x), false)
#else
#define FLEX_LIKELY(x)   (x)
#define FLEX_UNLIKELY(x) (x)
#endif

#endif//FLEX_LIKELY

//Trivial macro to determine C++11 support.  A more fine-grain solution may be needed for specific compilers.
#if (__cplusplus >= 201103L)
#define FLEX_HAS_CXX11 true
#endif

#ifdef FLEX_HAS_CXX11
#include <type_traits>
#define FLEX_NOEXCEPT noexcept
#else
#define FLEX_NOEXCEPT
#endif

namespace flex
{

#ifdef FLEX_HAS_CXX11
  template<typename InputIterator>
  inline void destruct_range_impl(InputIterator first, InputIterator last, std::true_type)
  {
    return;
  }

  template<typename InputIterator>
  inline void destruct_range_impl(InputIterator first, InputIterator last, std::false_type)
  {
    typedef typename std::iterator_traits<InputIterator>::value_type value_t;
    while (first != last)
    {
      first->~value_t();
      ++first;
    }
  }
#endif

  template<typename InputIterator>
  inline void destruct_range(InputIterator first, InputIterator last)
  {
    typedef typename std::iterator_traits<InputIterator>::value_type value_t;
#ifdef FLEX_HAS_CXX11
    destruct_range_impl(first, last, typename std::is_trivially_destructible< value_t >::type());
#else
    while (first != last)
    {
      first->~value_t();
      ++first;
    }
#endif
  }

} //namespace flex

#ifdef FLEX_HAS_CXX11
#include <initializer_list>
#else
namespace std
{
  // See the C++11 Standard, section 18.9.
  template<class E>
  class initializer_list
  {
  public:
    typedef E value_type;
    typedef const E& reference;
    typedef const E& const_reference;
    typedef size_t size_type;
    typedef const E* iterator;      // Must be const, as initializer_list (and its mpArray) is an immutable temp object.
    typedef const E* const_iterator;

  private:
    iterator mpArray;
    size_type mArraySize;

    // This constructor is private, but the C++ compiler has the ability to call it, as per the C++11 Standard.
    initializer_list(const_iterator pArray, size_type arraySize) :
        mpArray(pArray), mArraySize(arraySize)
    {
    }

  public:
    initializer_list() FLEX_NOEXCEPT
    : mpArray(NULL), mArraySize(0)
    {}

    size_type size() const FLEX_NOEXCEPT
    { return mArraySize;}
    const_iterator begin() const FLEX_NOEXCEPT
    { return mpArray;}     // Must be const_iterator, as initializer_list (and its mpArray) is an immutable temp object.
    const_iterator end() const FLEX_NOEXCEPT
    { return mpArray + mArraySize;}
  };

  template<class T>
  const T* begin(std::initializer_list<T> ilist) FLEX_NOEXCEPT
  {
    return ilist.begin();
  }

  template<class T>
  const T* end(std::initializer_list<T> ilist) FLEX_NOEXCEPT
  {
    return ilist.end();
  }
}
#endif

#endif //FLEX_CONFIG_H
