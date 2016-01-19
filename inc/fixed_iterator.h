#ifndef FIXED_ITERATOR_H
#define FIXED_ITERATOR_H

#include <cstdlib>

template<class T> struct fixed_reverse_iterator_base
{
   T* mPtr;

   fixed_reverse_iterator_base<T > (T * p) :
   mPtr(p)
   {
   }

   bool operator==(fixed_reverse_iterator_base<T> obj) const
   {
      return (mPtr == obj.mPtr);
   }

   bool operator!=(fixed_reverse_iterator_base<T> obj) const
   {
      return (mPtr != obj.mPtr);
   }

   bool operator<(fixed_reverse_iterator_base<T> obj) const
   {
      return (mPtr > obj.mPtr);
   }

   bool operator<=(fixed_reverse_iterator_base<T> obj) const
   {
      return (mPtr >= obj.mPtr);
   }

   bool operator>(fixed_reverse_iterator_base<T> obj) const
   {
      return (mPtr < obj.mPtr);
   }

   bool operator>=(fixed_reverse_iterator_base<T> obj) const
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

template<class T> struct fixed_const_reverse_iterator : public fixed_reverse_iterator_base<T>
{

   fixed_const_reverse_iterator<T > (T * p) : fixed_reverse_iterator_base<T>(p)
   {
   }

   fixed_const_reverse_iterator<T>& operator++()
   {
      --(this->mPtr);
      return *this;
   }

   fixed_const_reverse_iterator<T>& operator--()
   {
      ++(this->mPtr);
      return *this;
   }

};

template<class T> struct fixed_reverse_iterator : public fixed_reverse_iterator_base<T>
{

   fixed_reverse_iterator<T > (T * p) : fixed_reverse_iterator_base<T>(p)
   {
   }

   fixed_reverse_iterator<T>& operator++()
   {
      --(this->mPtr);
      return *this;
   }

   fixed_reverse_iterator<T>& operator--()
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
#endif /* FIXED_ITERATOR_H */
