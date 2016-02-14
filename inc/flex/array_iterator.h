#ifndef FLEX_ARRAY_ITERATOR_H
#define FLEX_ARRAY_ITERATOR_H

#include <cstdlib>

namespace flex
{
//TODO: Need to add +, -, +=, -=, [], posfix++.--, and casting() to const
  template<class T> struct array_reverse_iterator_base
  {
    T* mPtr;

    array_reverse_iterator_base(T * p) :
        mPtr(p)
    {
    }

    bool operator==(array_reverse_iterator_base<T> obj) const
    {
      return (mPtr == obj.mPtr);
    }

    bool operator!=(array_reverse_iterator_base<T> obj) const
    {
      return (mPtr != obj.mPtr);
    }

    bool operator<(array_reverse_iterator_base<T> obj) const
    {
      return (mPtr > obj.mPtr);
    }

    bool operator<=(array_reverse_iterator_base<T> obj) const
    {
      return (mPtr >= obj.mPtr);
    }

    bool operator>(array_reverse_iterator_base<T> obj) const
    {
      return (mPtr < obj.mPtr);
    }

    bool operator>=(array_reverse_iterator_base<T> obj) const
    {
      return (mPtr <= obj.mPtr);
    }

    const T& operator*() const
    {
      return *mPtr;
    }

    const T* operator->() const
    {
      return mPtr;
    }

  };

  template<class T> struct array_const_reverse_iterator: public array_reverse_iterator_base<T>
  {

    array_const_reverse_iterator(T * p) :
        array_reverse_iterator_base<T>(p)
    {
    }

    array_const_reverse_iterator<T>& operator++()
    {
      --(this->mPtr);
      return *this;
    }

    array_const_reverse_iterator<T>& operator--()
    {
      ++(this->mPtr);
      return *this;
    }

  };

  template<class T> struct array_reverse_iterator: public array_reverse_iterator_base<T>
  {

    array_reverse_iterator(T * p) :
        array_reverse_iterator_base<T>(p)
    {
    }

    array_reverse_iterator<T>& operator++()
    {
      --(this->mPtr);
      return *this;
    }

    array_reverse_iterator<T>& operator--()
    {
      ++(this->mPtr);
      return *this;
    }

    T& operator*()
    {
      return *(this->mPtr);
    }

    T* operator->()
    {
      return this->mPtr;
    }
  };

} //namespace flex
#endif /* FLEX_ARRAY_ITERATOR_H */
