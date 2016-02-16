#ifndef FLEX_RING_ITERATOR_H
#define FLEX_RING_ITERATOR_H

#include <iterator>

namespace flex
{

  template<class T> struct ring_iterator_base: public std::iterator<std::bidirectional_iterator_tag, T>
  {
    typedef std::iterator<std::bidirectional_iterator_tag, T> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef const T* const_pointer;
    typedef typename base_type::reference reference;
    typedef const T& const_reference;
    typedef typename base_type::iterator_category iterator_category;

    pointer mPtr;
    pointer mLeftBound;
    pointer mRightBound;

    ring_iterator_base();
    ring_iterator_base(T* ptr, T* left_bound, T* right_bound);

    bool operator==(ring_iterator_base<T> obj) const;
    bool operator!=(ring_iterator_base<T> obj) const;
  };

  template<class T> struct ring_const_iterator: public ring_iterator_base<T>
  {
    typedef ring_iterator_base<T> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::iterator_category iterator_category;

    using base_type::mPtr;
    using base_type::mLeftBound;
    using base_type::mRightBound;

    ring_const_iterator();
    ring_const_iterator(T* ptr, T* left_bound, T* right_bound);

    ring_const_iterator<T>& operator++();
    ring_const_iterator<T> operator++(int);
    ring_const_iterator<T>& operator--();
    ring_const_iterator<T> operator--(int);
    ring_const_iterator<T> operator+(difference_type n) const;
    ring_const_iterator<T> operator-(difference_type n) const;
    difference_type operator-(const ring_const_iterator<T>& begin) const;
    ring_const_iterator<T>& operator+=(difference_type n);
    ring_const_iterator<T>& operator-=(difference_type n);

    const_reference operator*() const;
    const_pointer operator->() const;
    const_reference operator[](difference_type n) const;
  };

  template<class T> struct ring_iterator: public ring_iterator_base<T>
  {
    typedef ring_iterator_base<T> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::iterator_category iterator_category;

    using base_type::mPtr;
    using base_type::mLeftBound;
    using base_type::mRightBound;

    ring_iterator();
    ring_iterator(T* ptr, T* left_bound, T* right_bound);
    ring_iterator<T>& operator++();
    ring_iterator<T> operator++(int);
    ring_iterator<T>& operator--();
    ring_iterator<T> operator--(int);
    ring_iterator<T> operator+(difference_type n) const;
    ring_iterator<T> operator-(difference_type n) const;
    difference_type operator-(const ring_iterator<T>& begin) const;
    ring_iterator<T>& operator+=(difference_type n);
    ring_iterator<T>& operator-=(difference_type n);
    reference operator[](difference_type n);

    const_reference operator*() const;
    const_pointer operator->() const;
    const_reference operator[](difference_type n) const;
    T& operator*();
    T* operator->();
    operator ring_const_iterator<T>&() const;
  };

  template<class T> struct ring_const_reverse_iterator: public ring_const_iterator<T>
  {
    typedef ring_const_iterator<T> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::iterator_category iterator_category;

    using base_type::mPtr;
    using base_type::mLeftBound;
    using base_type::mRightBound;

    ring_const_reverse_iterator();
    ring_const_reverse_iterator(T* ptr, T* left_bound, T* right_bound);
    ring_const_reverse_iterator<T>& operator++();
    ring_const_reverse_iterator<T> operator++(int);
    ring_const_reverse_iterator<T>& operator--();
    ring_const_reverse_iterator<T> operator--(int);
    ring_const_reverse_iterator<T> operator+(difference_type n) const;
    ring_const_reverse_iterator<T> operator-(difference_type n) const;
    difference_type operator-(const ring_const_reverse_iterator<T>& begin) const;
    ring_const_reverse_iterator<T>& operator+=(difference_type n);
    ring_const_reverse_iterator<T>& operator-=(difference_type n);
  };

  template<class T> struct ring_reverse_iterator: public ring_iterator<T>
  {
    typedef ring_iterator<T> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::iterator_category iterator_category;

    using base_type::mPtr;
    using base_type::mLeftBound;
    using base_type::mRightBound;

    ring_reverse_iterator();
    ring_reverse_iterator(T* ptr, T* left_bound, T* right_bound);
    ring_reverse_iterator<T>& operator++();
    ring_reverse_iterator<T> operator++(int);
    ring_reverse_iterator<T>& operator--();
    ring_reverse_iterator<T> operator--(int);
    ring_reverse_iterator<T> operator+(difference_type n) const;
    ring_reverse_iterator<T> operator-(difference_type n) const;
    difference_type operator-(const ring_reverse_iterator<T>& begin) const;
    ring_reverse_iterator<T>& operator+=(difference_type n);
    ring_reverse_iterator<T>& operator-=(difference_type n);
    operator const ring_const_reverse_iterator<T>&() const;
  };

  /*
   * ring_iterator_base
   */
  template<class T>
  inline ring_iterator_base<T>::ring_iterator_base() :
      mPtr(NULL), mLeftBound(NULL), mRightBound(NULL)
  {
  }

  template<class T>
  inline ring_iterator_base<T>::ring_iterator_base(T* ptr, T* left_bound, T* right_bound) :
      mPtr(ptr), mLeftBound(left_bound), mRightBound(right_bound)
  {
  }

  template<class T>
  inline bool ring_iterator_base<T>::operator==(ring_iterator_base<T> obj) const
  {
    return (mPtr == obj.mPtr);
  }

  template<class T>
  inline bool ring_iterator_base<T>::operator!=(ring_iterator_base<T> obj) const
  {
    return (mPtr != obj.mPtr);
  }

  /*
   * ring_const_iterator
   */
  template<class T>
  inline ring_const_iterator<T>::ring_const_iterator() :
      ring_iterator_base<T>(NULL, NULL, NULL)
  {
  }

  template<class T>
  inline ring_const_iterator<T>::ring_const_iterator(T* ptr, T* left_bound, T* right_bound) :
      ring_iterator_base<T>(ptr, left_bound, right_bound)
  {
  }

  template<class T>
  inline ring_const_iterator<T>& ring_const_iterator<T>::operator++()
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

  template<class T>
  inline ring_const_iterator<T> ring_const_iterator<T>::operator++(int)
  {
    ring_const_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator++();
    return tmp;
  }

  template<class T>
  inline ring_const_iterator<T>& ring_const_iterator<T>::operator--()
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

  template<class T>
  inline ring_const_iterator<T> ring_const_iterator<T>::operator--(int)
  {
    ring_const_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator--();
    return tmp;
  }

  template<class T>
  inline ring_const_iterator<T> ring_const_iterator<T>::operator+(difference_type n) const
  {
    ring_const_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp += n;
    return tmp;
  }

  template<class T>
  inline ring_const_iterator<T> ring_const_iterator<T>::operator-(difference_type n) const
  {
    ring_const_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp -= n;
    return tmp;
  }

  template<class T>
  inline typename ring_const_iterator<T>::difference_type ring_const_iterator<T>::operator-(
      const ring_const_iterator<T>& begin) const
  {
    difference_type val = mPtr - begin.mPtr;
    //Since this is a circular iterator, the result is different depending on which iterator is considered "begin" and which
    //is considered "end".  Typically "end" is subtracted by "begin" to get a positive distance.  Therefore, this method
    //assumes that "this" iterator is the "end" and the parameter is "begin".  If subtracting the pointers yields a negative
    //result, we must add the size, (mRightBound-mLeftBound) + 1, to get the "circular" distance between "begin" and "end".
    if (val < 0)
    {
      val += (mRightBound - mLeftBound) + 1;
    }
    return val;
  }

  template<class T>
  inline ring_const_iterator<T>& ring_const_iterator<T>::operator+=(difference_type n)
  {
    mPtr += n;
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (mPtr > mRightBound)
    {
      mPtr -= (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T>
  inline ring_const_iterator<T>& ring_const_iterator<T>::operator-=(difference_type n)
  {
    mPtr -= n;
    //If the iterators extends out the left bound, we add the size aka (right-left+1) to wrap back around.
    if (mPtr < mLeftBound)
    {
      mPtr += (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T>
  inline typename ring_const_iterator<T>::const_reference ring_const_iterator<T>::operator*() const
  {
    return *mPtr;
  }

  template<class T>
  inline typename ring_const_iterator<T>::const_pointer ring_const_iterator<T>::operator->() const
  {
    return mPtr;
  }

  template<class T>
  inline typename ring_const_iterator<T>::const_reference ring_const_iterator<T>::operator[](difference_type n) const
  {
    pointer tmp = (mPtr + n);
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (tmp > mRightBound)
    {
      tmp -= (mRightBound - mLeftBound + 1);
    }
    return *tmp;
  }

  /*
   * ring_iterator
   */

  template<class T>
  inline ring_iterator<T>::ring_iterator() :
      ring_iterator_base<T>(NULL, NULL, NULL)
  {
  }

  template<class T>
  inline ring_iterator<T>::ring_iterator(T* ptr, T* left_bound, T* right_bound) :
      ring_iterator_base<T>(ptr, left_bound, right_bound)
  {
  }

  template<class T>
  inline ring_iterator<T>& ring_iterator<T>::operator++()
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

  template<class T>
  inline ring_iterator<T> ring_iterator<T>::operator++(int)
  {
    ring_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator++();
    return tmp;
  }

  template<class T>
  inline ring_iterator<T>& ring_iterator<T>::operator--()
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

  template<class T>
  inline ring_iterator<T> ring_iterator<T>::operator--(int)
  {
    ring_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator--();
    return tmp;
  }

  template<class T>
  inline T& ring_iterator<T>::operator*()
  {
    return *mPtr;
  }

  template<class T>
  inline T* ring_iterator<T>::operator->()
  {
    return mPtr;
  }

  template<class T>
  inline ring_iterator<T>::operator ring_const_iterator<T>&() const
  {
    return *((ring_const_iterator<T>*) this);
  }

  template<class T>
  inline ring_iterator<T> ring_iterator<T>::operator+(difference_type n) const
  {
    ring_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp += n;
    return tmp;
  }

  template<class T>
  inline ring_iterator<T> ring_iterator<T>::operator-(difference_type n) const
  {
    ring_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp -= n;
    return tmp;
  }

  template<class T>
  inline typename ring_iterator<T>::difference_type ring_iterator<T>::operator-(const ring_iterator<T>& begin) const
  {
    difference_type val = mPtr - begin.mPtr;
    //Since this is a circular iterator, the result is different depending on which iterator is considered "begin" and which
    //is considered "end".  Typically "end" is subtracted by "begin" to get a positive distance.  Therefore, this method
    //assumes that "this" iterator is the "end" and the parameter is "begin".  If subtracting the pointers yields a negative
    //result, we must add the size, (mRightBound-mLeftBound) + 1, to get the "circular" distance between "begin" and "end".
    if (val < 0)
    {
      val += (mRightBound - mLeftBound) + 1;
    }
    return val;
  }

  template<class T>
  inline ring_iterator<T>& ring_iterator<T>::operator+=(difference_type n)
  {
    mPtr += n;
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (mPtr > mRightBound)
    {
      mPtr -= (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T>
  inline ring_iterator<T>& ring_iterator<T>::operator-=(difference_type n)
  {
    mPtr -= n;
    //If the iterators extends out the left bound, we add the size aka (right-left+1) to wrap back around.
    if (mPtr < mLeftBound)
    {
      mPtr += (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T>
  inline typename ring_iterator<T>::reference ring_iterator<T>::operator[](difference_type n)
  {
    pointer tmp = (mPtr + n);
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (tmp > mRightBound)
    {
      tmp -= (mRightBound - mLeftBound + 1);
    }
    return *tmp;
  }

  template<class T>
  inline typename ring_iterator<T>::const_reference ring_iterator<T>::operator*() const
  {
    return *mPtr;
  }

  template<class T>
  inline typename ring_iterator<T>::const_pointer ring_iterator<T>::operator->() const
  {
    return mPtr;
  }

  template<class T>
  inline typename ring_iterator<T>::const_reference ring_iterator<T>::operator[](difference_type n) const
  {
    pointer tmp = (mPtr + n);
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (tmp > mRightBound)
    {
      tmp -= (mRightBound - mLeftBound + 1);
    }
    return *tmp;
  }

  /*
   * ring_const_reverse_iterator
   */
  template<class T>
  inline ring_const_reverse_iterator<T>::ring_const_reverse_iterator() :
      ring_const_iterator<T>(NULL, NULL, NULL)
  {
  }

  template<class T>
  inline ring_const_reverse_iterator<T>::ring_const_reverse_iterator(T* ptr, T* left_bound, T* right_bound) :
      ring_const_iterator<T>(ptr, left_bound, right_bound)
  {
  }

  template<class T>
  inline ring_const_reverse_iterator<T>& ring_const_reverse_iterator<T>::operator++()
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

  template<class T>
  inline ring_const_reverse_iterator<T> ring_const_reverse_iterator<T>::operator++(int)
  {
    ring_const_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator++();
    return tmp;
  }

  template<class T>
  inline ring_const_reverse_iterator<T>& ring_const_reverse_iterator<T>::operator--()
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

  template<class T>
  inline ring_const_reverse_iterator<T> ring_const_reverse_iterator<T>::operator--(int)
  {
    ring_const_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator--();
    return tmp;
  }

  template<class T>
  inline ring_const_reverse_iterator<T> ring_const_reverse_iterator<T>::operator+(difference_type n) const
  {
    ring_const_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp += n;
    return tmp;
  }

  template<class T>
  inline ring_const_reverse_iterator<T> ring_const_reverse_iterator<T>::operator-(difference_type n) const
  {
    ring_const_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp -= n;
    return tmp;
  }

  template<class T>
  inline typename ring_const_reverse_iterator<T>::difference_type ring_const_reverse_iterator<T>::operator-(
      const ring_const_reverse_iterator<T>& begin) const
  {
    //Since this is a reverse iterator, we subtract "this" pointer from the "begin" pointer.
    difference_type val = begin.mPtr - mPtr;

    //Since this is a circular iterator, the result is different depending on which iterator is considered "begin" and which
    //is considered "end".  Typically "begin" is subtracted by "end" to get a positive distance.  Therefore, this method
    //assumes that "this" iterator is the "end" and the parameter is "begin".  If subtracting the pointers yields a negative
    //result, we must add the size, (mRightBound-mLeftBound) + 1, to get the "circular" distance between "begin" and "end".
    if (val < 0)
    {
      val += (mRightBound - mLeftBound) + 1;
    }
    return val;
  }

  template<class T>
  inline ring_const_reverse_iterator<T>& ring_const_reverse_iterator<T>::operator+=(difference_type n)
  {
    mPtr -= n;
    //If the iterators extends out the left bound, we add the size aka (right-left+1) to wrap back around.
    if (mPtr < mLeftBound)
    {
      mPtr += (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T>
  inline ring_const_reverse_iterator<T>& ring_const_reverse_iterator<T>::operator-=(difference_type n)
  {
    mPtr += n;
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (mPtr > mRightBound)
    {
      mPtr -= (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  /*
   * ring_reverse_iterator
   */
  template<class T>
  inline ring_reverse_iterator<T>::ring_reverse_iterator() :
      ring_iterator<T>(NULL, NULL, NULL)
  {
  }

  template<class T>
  inline ring_reverse_iterator<T>::ring_reverse_iterator(T* ptr, T* left_bound, T* right_bound) :
      ring_iterator<T>(ptr, left_bound, right_bound)
  {
  }

  template<class T>
  inline ring_reverse_iterator<T>& ring_reverse_iterator<T>::operator++()
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

  template<class T>
  inline ring_reverse_iterator<T> ring_reverse_iterator<T>::operator++(int)
  {
    ring_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator++();
    return tmp;
  }

  template<class T>
  inline ring_reverse_iterator<T>& ring_reverse_iterator<T>::operator--()
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

  template<class T>
  inline ring_reverse_iterator<T> ring_reverse_iterator<T>::operator--(int)
  {
    ring_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    operator--();
    return tmp;
  }

  template<class T>
  inline ring_reverse_iterator<T> ring_reverse_iterator<T>::operator+(difference_type n) const
  {
    ring_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp += n;
    return tmp;
  }

  template<class T>
  inline ring_reverse_iterator<T> ring_reverse_iterator<T>::operator-(difference_type n) const
  {
    ring_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
    tmp -= n;
    return tmp;
  }

  template<class T>
  inline typename ring_reverse_iterator<T>::difference_type ring_reverse_iterator<T>::operator-(
      const ring_reverse_iterator<T>& begin) const
  {
    //Since this is a reverse iterator, we subtract "this" pointer from the "begin" pointer.
    difference_type val = begin.mPtr - mPtr;

    //Since this is a circular iterator, the result is different depending on which iterator is considered "begin" and which
    //is considered "end".  Typically "begin" is subtracted by "end" to get a positive distance.  Therefore, this method
    //assumes that "this" iterator is the "end" and the parameter is "begin".  If subtracting the pointers yields a negative
    //result, we must add the size, (mRightBound-mLeftBound) + 1, to get the "circular" distance between "begin" and "end".
    if (val < 0)
    {
      val += (mRightBound - mLeftBound) + 1;
    }
    return val;
  }

  template<class T>
  inline ring_reverse_iterator<T>& ring_reverse_iterator<T>::operator+=(difference_type n)
  {
    mPtr -= n;
    //If the iterators extends out the left bound, we add the size aka (right-left+1) to wrap back around.
    if (mPtr < mLeftBound)
    {
      mPtr += (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T>
  inline ring_reverse_iterator<T>& ring_reverse_iterator<T>::operator-=(difference_type n)
  {
    mPtr += n;
    //If the iterators extends out the right bound, we subtract the size aka (right-left+1) to wrap back around.
    if (mPtr > mRightBound)
    {
      mPtr -= (mRightBound - mLeftBound + 1);
    }
    return *this;
  }

  template<class T>
  inline ring_reverse_iterator<T>::operator const ring_const_reverse_iterator<T>&() const
  {
    return *((ring_const_reverse_iterator<T>*) this);
  }
} //namespace flex

#endif /* FLEX_RING_ITERATOR_H */
