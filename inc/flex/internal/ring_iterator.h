#ifndef FLEX_INTERNAL_RING_ITERATOR_H
#define FLEX_INTERNAL_RING_ITERATOR_H

#include <iterator>

namespace flex
{

  template<class T, class Pointer = T*, class Reference = T&> struct ring_iterator
  {
    typedef ring_iterator<T, Pointer, Reference> this_type;
    typedef ring_iterator<T, T*, T&> iterator;

    /*
     * The 5 typedefs below are required by the std library to properly identify an iterator.
     */
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef std::random_access_iterator_tag iterator_category; //Although this does not contain all methods of a random_access_iterator, we assign
                                                               //it this tag to ensure the operator-() method is called by std::distance().
    typedef size_t size_type;

    pointer mPtr;
    pointer mLeftBound;
    pointer mRightBound;

    ring_iterator();
    ring_iterator(T* ptr, T* left_bound, T* right_bound);
    ring_iterator(T* ptr, size_type n);
    ring_iterator(const iterator& x);

    this_type& operator++();
    this_type operator++(int);
    this_type& operator--();
    this_type operator--(int);
    this_type operator+(difference_type n) const;
    this_type operator-(difference_type n) const;
    difference_type operator-(const this_type& begin) const;
    this_type& operator+=(difference_type n);
    this_type& operator-=(difference_type n);

    reference operator*() const;
    pointer operator->() const;
    reference operator[](difference_type n) const;
  };

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>::ring_iterator() :
      mPtr(NULL), mLeftBound(NULL), mRightBound(NULL)
  {
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>::ring_iterator(T* ptr, T* left_bound, T* right_bound) :
      mPtr(ptr), mLeftBound(left_bound), mRightBound(right_bound)
  {
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>::ring_iterator(T* ptr, size_type n) :
      mPtr(ptr), mLeftBound(ptr), mRightBound(ptr + n)
  {
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>::ring_iterator(const iterator& x) :
      mPtr(x.mPtr), mLeftBound(x.mLeftBound), mRightBound(x.mRightBound)
  {

  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>& ring_iterator<T, Pointer, Reference>::operator++()
  {
    if (mPtr == mRightBound)
    {
      mPtr = mLeftBound;
    }
    else
    {
      ++mPtr;
    }
    return *this;
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference> ring_iterator<T, Pointer, Reference>::operator++(int)
  {
    ring_iterator<T, Pointer, Reference> tmp(mPtr, mLeftBound, mRightBound);
    operator++();
    return tmp;
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>& ring_iterator<T, Pointer, Reference>::operator--()
  {
    if (mPtr == mLeftBound)
    {
      mPtr = mRightBound;
    }
    else
    {
      --mPtr;
    }
    return *this;
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference> ring_iterator<T, Pointer, Reference>::operator--(int)
  {
    ring_iterator<T, Pointer, Reference> tmp(mPtr, mLeftBound, mRightBound);
    operator--();
    return tmp;
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference> ring_iterator<T, Pointer, Reference>::operator+(difference_type n) const
  {
    ring_iterator<T, Pointer, Reference> tmp(mPtr, mLeftBound, mRightBound);
    tmp += n;
    return tmp;
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference> ring_iterator<T, Pointer, Reference>::operator-(difference_type n) const
  {
    ring_iterator<T, Pointer, Reference> tmp(mPtr, mLeftBound, mRightBound);
    tmp -= n;
    return tmp;
  }

  template<class T, class Pointer, class Reference>
  inline typename ring_iterator<T, Pointer, Reference>::difference_type ring_iterator<T, Pointer, Reference>::operator-(
      const this_type& begin) const
  {
    difference_type val = mPtr - begin.mPtr;
    //Since this is a circular iterator, the result is different depending on which iterator is considered "begin" and which
    //is considered "end".  Typically "end" is subtracted by "begin" to get a positive distance.  Therefore, this method
    //assumes that "this" iterator is the "end" and the parameter is "begin".  If subtracting the pointers yields a negative
    //result, we must add the size, (mRightBound-mLeftBound) + 1, to wrap-around and get the "circular" distance between
    //"begin" and "end".
    if (val < 0)
    {
      val += (mRightBound - mLeftBound) + 1;
    }
    return val;
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>& ring_iterator<T, Pointer, Reference>::operator+=(difference_type n)
  {
    mPtr += n;
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (mPtr > mRightBound)
    {
      mPtr -= (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T, class Pointer, class Reference>
  inline ring_iterator<T, Pointer, Reference>& ring_iterator<T, Pointer, Reference>::operator-=(difference_type n)
  {
    mPtr -= n;
    //If the iterators extends out the left bound, we add the size aka (right-left+1) to wrap back around.
    if (mPtr < mLeftBound)
    {
      mPtr += (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T, class Pointer, class Reference>
  inline typename ring_iterator<T, Pointer, Reference>::reference ring_iterator<T, Pointer, Reference>::operator*() const
  {
    return *mPtr;
  }

  template<class T, class Pointer, class Reference>
  inline typename ring_iterator<T, Pointer, Reference>::pointer ring_iterator<T, Pointer, Reference>::operator->() const
  {
    return mPtr;
  }

  template<class T, class Pointer, class Reference>
  inline typename ring_iterator<T, Pointer, Reference>::reference ring_iterator<T, Pointer, Reference>::operator[](
      difference_type n) const
  {
    pointer tmp = (mPtr + n);
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (tmp > mRightBound)
    {
      tmp -= (mRightBound - mLeftBound + 1);
    }
    return *tmp;
  }

  // Extra template parameters were put in to support comparisons between const and non-const iterators.
  template<typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
  inline bool operator==(const ring_iterator<T, PointerA, ReferenceA>& a,
      const ring_iterator<T, PointerB, ReferenceB>& b)
  {
    return a.mPtr == b.mPtr;
  }

  template<typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
  inline bool operator!=(const ring_iterator<T, PointerA, ReferenceA>& a,
      const ring_iterator<T, PointerB, ReferenceB>& b)
  {
    return a.mPtr != b.mPtr;
  }
}    //namespace flex

#endif /* FLEX_INTERNAL_RING_ITERATOR_H */
