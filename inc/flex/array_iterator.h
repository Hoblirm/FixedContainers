#ifndef FLEX_ARRAY_ITERATOR_H
#define FLEX_ARRAY_ITERATOR_H

#include <cstdlib>

namespace flex
{
//TODO: Need to add +, -, +=, -=, [], posfix++.--, and casting() to const, also need to inherit from std::iterator
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

  };

  template<class T> struct array_const_reverse_iterator: public array_reverse_iterator_base<T>
  {
    typedef array_reverse_iterator_base<T> base_type;
    using base_type::mPtr;

    array_const_reverse_iterator(T * p) :
        array_reverse_iterator_base<T>(p)
    {
    }

    array_const_reverse_iterator<T>& operator++()
    {
      --mPtr;
      return *this;
    }

    array_const_reverse_iterator<T>& operator--()
    {
      ++mPtr;
      return *this;
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

  template<class T> struct array_reverse_iterator: public array_reverse_iterator_base<T>
  {
    typedef array_reverse_iterator_base<T> base_type;
    using base_type::mPtr;

    array_reverse_iterator(T * p) :
        array_reverse_iterator_base<T>(p)
    {
    }

    array_reverse_iterator<T>& operator++()
    {
      --mPtr;
      return *this;
    }

    array_reverse_iterator<T>& operator--()
    {
      ++mPtr;
      return *this;
    }

    const T& operator*() const
    {
      return *mPtr;
    }

    const T* operator->() const
    {
      return mPtr;
    }

    T& operator*()
    {
      return *mPtr;
    }

    T* operator->()
    {
      return mPtr;
    }
  };

} //namespace flex
#endif /* FLEX_ARRAY_ITERATOR_H */
