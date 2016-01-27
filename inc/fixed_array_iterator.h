#ifndef FIXED_ARRAY_ITERATOR_H
#define FIXED_ARRAY_ITERATOR_H

#include <cstdlib>

//TODO: Need to add +, -, +=, -=, []
template<class T> struct fixed_array_reverse_iterator_base
{
   T* mPtr;

   fixed_array_reverse_iterator_base<T > (T * p) :
   mPtr(p)
   {
   }

   bool operator==(fixed_array_reverse_iterator_base<T> obj) const
   {
      return (mPtr == obj.mPtr);
   }

   bool operator!=(fixed_array_reverse_iterator_base<T> obj) const
   {
      return (mPtr != obj.mPtr);
   }

   bool operator<(fixed_array_reverse_iterator_base<T> obj) const
   {
      return (mPtr > obj.mPtr);
   }

   bool operator<=(fixed_array_reverse_iterator_base<T> obj) const
   {
      return (mPtr >= obj.mPtr);
   }

   bool operator>(fixed_array_reverse_iterator_base<T> obj) const
   {
      return (mPtr < obj.mPtr);
   }

   bool operator>=(fixed_array_reverse_iterator_base<T> obj) const
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

template<class T> struct fixed_array_const_reverse_iterator : public fixed_array_reverse_iterator_base<T>
{

   fixed_array_const_reverse_iterator<T > (T * p) : fixed_array_reverse_iterator_base<T>(p)
   {
   }

   fixed_array_const_reverse_iterator<T>& operator++()
   {
      --(this->mPtr);
      return *this;
   }

   fixed_array_const_reverse_iterator<T>& operator--()
   {
      ++(this->mPtr);
      return *this;
   }

};

template<class T> struct fixed_array_reverse_iterator : public fixed_array_reverse_iterator_base<T>
{

   fixed_array_reverse_iterator<T > (T * p) : fixed_array_reverse_iterator_base<T>(p)
   {
   }

   fixed_array_reverse_iterator<T>& operator++()
   {
      --(this->mPtr);
      return *this;
   }

   fixed_array_reverse_iterator<T>& operator--()
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
#endif /* FIXED_ARRAY_ITERATOR_H */
