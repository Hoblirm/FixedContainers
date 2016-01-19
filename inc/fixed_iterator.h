#ifndef FIXED_ITERATOR_H
#define FIXED_ITERATOR_H

#include <cstdlib>

template<class T> struct fixed_reverse_iterator
{
  T* ptr;

  fixed_reverse_iterator<T>(T* p) :
      ptr(p)
  {
  }

  fixed_reverse_iterator<T>& operator++()
  {
    --ptr;
    return *this;
  }

  fixed_reverse_iterator<T>& operator--()
  {
    ++ptr;
    return *this;
  }

  bool operator==(fixed_reverse_iterator<T> obj) const
  {
    return (ptr == obj.ptr);
  }

  bool operator!=(fixed_reverse_iterator<T> obj) const
  {
    return (ptr != obj.ptr);
  }

  bool operator<(fixed_reverse_iterator<T> obj) const
  {
    return (ptr > obj.ptr);
  }

  bool operator<=(fixed_reverse_iterator<T> obj) const
  {
    return (ptr >= obj.ptr);
  }

  bool operator>(fixed_reverse_iterator<T> obj) const
  {
    return (ptr < obj.ptr);
  }

  bool operator>=(fixed_reverse_iterator<T> obj) const
  {
    return (ptr <= obj.ptr);
  }

  const T& operator*() const
  {
    return *ptr;
  }

  const T* operator->() const
  {
    return ptr;
  }

  T& operator*()
  {
    return *ptr;
  }

  T* operator->()
  {
    return ptr;
  }

};


#endif /* FIXED_ITERATOR_H */
