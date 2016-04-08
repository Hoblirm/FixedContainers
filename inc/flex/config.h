#ifndef FLEX_CONFIG_H
#define FLEX_CONFIG_H

#include <cstdlib>
#include <cerrno> //Used errno in error_msg()
#include <stdexcept> //For exceptions
#include <stdio.h>//For printf used in assert/throw methods.

#ifdef FLEX_TEST
#define FLEX_DEBUG
#endif

/*
 * FLEX_HAS_CXX11
 */
#if (__cplusplus >= 201103L)
#define FLEX_HAS_CXX11 true
#endif

/*
 * FLEX_NOEXCEPT
 */
#ifdef FLEX_HAS_CXX11
#include <type_traits>
#define FLEX_NOEXCEPT noexcept
#else
#define FLEX_NOEXCEPT
#endif

/*
 * FLEX_LIKELY
 */
#ifndef FLEX_LIKELY
#if defined(__GNUC__) && (__GNUC__ >= 3)
#define FLEX_LIKELY(x)   __builtin_expect(!!(x), true)
#define FLEX_UNLIKELY(x) __builtin_expect(!!(x), false)
#else
#define FLEX_LIKELY(x)   (x)
#define FLEX_UNLIKELY(x) (x)
#endif

#endif//FLEX_LIKELY

/*
 * flex::destruct_range
 */
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

namespace flex
{
  inline void error_msg(const char* msg, int error_code = -1)
  {
    //errno is used mainly for testing purposes.  May want to consider
    //alternative solutions if error handling is to be done at runtime.
    errno = error_code;
#ifndef FLEX_TEST
    printf("%s", msg);
    printf("\n");
#endif
  }

  inline void assert_failure(const char* expression)
  {
    const int msg_size = 128;
    char msg[msg_size];
    snprintf(msg, msg_size, "FLEX_ASSERT(%s) failed!", expression);
    flex::error_msg(msg);
  }

  inline void throw_bad_alloc()
  {
    throw std::bad_alloc();
  }

  inline void throw_invalid_argument(const char* msg)
  {
    throw std::invalid_argument(msg);
  }

  inline void throw_length_error(const char* msg)
  {
    throw std::length_error(msg);
  }

  inline void throw_out_of_range(const char* msg)
  {
    throw std::out_of_range(msg);
  }

  inline void throw_overflow_error(const char* msg)
  {
    throw std::overflow_error(msg);
  }
}

/*
 * FLEX_ASSERT
 */
#ifndef FLEX_ASSERT
#ifdef FLEX_DEBUG
#define FLEX_ASSERT(expression) \
        do { \
        	if (FLEX_UNLIKELY(!(expression))) \
        	{ \
        		flex::assert_failure(#expression); \
        	} \
        } while (0)
#else
#define FLEX_ASSERT(expression)
#endif
#endif

/*
 * FLEX_ERROR_MSG_IF
 */
#ifndef FLEX_ERROR_MSG_IF
#ifndef FLEX_RELEASE
#define FLEX_ERROR_MSG_IF(expression,msg) \
        do { \
             if (FLEX_UNLIKELY(expression)) \
             { \
                flex::error_msg(msg); \
             } \
        } while (0)
#else
#define FLEX_ERROR_MSG_IF(expression,msg)
#endif
#endif

/*
 * FLEX_THROW_BAD_ALLOC_IF
 */
#ifndef FLEX_THROW_BAD_ALLOC_IF
#ifndef FLEX_RELEASE
#define FLEX_THROW_BAD_ALLOC_IF(expression) \
        do { \
             if (FLEX_UNLIKELY(expression)) \
             { \
                flex::throw_bad_alloc(); \
             } \
        } while (0)
#else
#define FLEX_THROW_BAD_ALLOC_IF(expression)
#endif
#endif

/*
 * FLEX_THROW_INVALID_ARGUMENT_IF
 */
#ifndef FLEX_THROW_INVALID_ARGUMENT_IF
#ifndef FLEX_RELEASE
#define FLEX_THROW_INVALID_ARGUMENT_IF(expression,msg) \
        do { \
             if (FLEX_UNLIKELY(expression)) \
             { \
                flex::throw_invalid_argument(msg); \
             } \
        } while (0)
#else
#define FLEX_THROW_INVALID_ARGUMENT_IF(expression,msg)
#endif
#endif

/*
 * FLEX_THROW_LENGTH_ERROR_IF
 */
#ifndef FLEX_THROW_LENGTH_ERROR_IF
#ifndef FLEX_RELEASE
#define FLEX_THROW_LENGTH_ERROR_IF(expression,msg) \
        do { \
             if (FLEX_UNLIKELY(expression)) \
             { \
                flex::throw_length_error(msg); \
             } \
        } while (0)
#else
#define FLEX_THROW_LENGTH_ERROR_IF(expression,msg)
#endif
#endif

/*
 * FLEX_THROW_OUT_OF_RANGE_IF
 */
#ifndef FLEX_THROW_OUT_OF_RANGE_IF
#ifndef FLEX_RELEASE
#define FLEX_THROW_OUT_OF_RANGE_IF(expression,msg) \
        do { \
             if (FLEX_UNLIKELY(expression)) \
             { \
                flex::throw_out_of_range(msg); \
             } \
        } while (0)
#else
#define FLEX_THROW_OUT_OF_RANGE_IF(expression,msg)
#endif
#endif

/*
 * FLEX_THROW_OVERFLOW_ERROR_IF
 */
#ifndef FLEX_THROW_OVERFLOW_ERROR_IF
#ifndef FLEX_RELEASE
#define FLEX_THROW_OVERFLOW_ERROR_IF(expression,msg) \
        do { \
             if (FLEX_UNLIKELY(expression)) \
             { \
                flex::throw_overflow_error(msg); \
             } \
        } while (0)
#else
#define FLEX_THROW_OVERFLOW_ERROR_IF(expression,msg)
#endif
#endif

/*
 * std::initializer_list
 */
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
    typedef const E* iterator;    // Must be const, as initializer_list (and its mpArray) is an immutable temp object.
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
    { return mpArray;}   // Must be const_iterator, as initializer_list (and its mpArray) is an immutable temp object.
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
