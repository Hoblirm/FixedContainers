#ifndef FLEX_INTERNAL_ALGORITHM_H
#define FLEX_INTERNAL_ALGORITHM_H

#include <algorithm>

#ifndef FLEX_HAS_CXX11
namespace std
{

  /// is_permutation
  ///
  template<typename ForwardIterator1, typename ForwardIterator2>
  bool is_permutation(ForwardIterator1 first1, ForwardIterator1 last1, ForwardIterator2 first2)
  {
    typedef typename std::iterator_traits<ForwardIterator1>::difference_type difference_type;

    // Skip past any equivalent initial elements.
    while ((first1 != last1) && (*first1 == *first2))
    {
      ++first1;
      ++first2;
    }

    if (first1 != last1)
    {
      const difference_type first1Size = std::distance(first1, last1);
      ForwardIterator2 last2 = first2;
      std::advance(last2, first1Size);

      for (ForwardIterator1 i = first1; i != last1; ++i)
      {
        if (i == std::find(first1, i, *i))
        {
          const difference_type c = std::count(first2, last2, *i);

          if ((c == 0) || (c != std::count(i, last1, *i)))
            return false;
        }
      }
    }

    return true;
  }

}  //namespace std

#endif //FLEX_HAS_CXX11

#endif //FLEX_INTERNAL_ALGORITHM_H
