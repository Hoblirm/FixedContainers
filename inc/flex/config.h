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

#endif

#ifdef FLEX_HAS_CXX11
#include <type_traits>
#endif

namespace flex
{

  template<typename InputIterator>
  void destruct_range(InputIterator first, InputIterator last)
  {

#ifdef FLEX_HAS_CXX11
    typedef typename std::iterator_traits<InputIterator>::value_type value_t;
    if (std::is_trivially_destructible < value_t > ::value)
    {
      return;
    }
#endif

    while (first != last)
    {
      first->~value_t();
      ++first;
    }
  }
} //namespace flex

#endif //FLEX_CONFIG_H
