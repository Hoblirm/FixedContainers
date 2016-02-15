#ifndef FLEX_RING_H
#define FLEX_RING_H

#include <algorithm>

#include <flex/allocator.h>
#include <flex/ring_iterator.h>

namespace flex
{
  template<class T, class Alloc = allocator<T> > class ring: public allocation_guard
  {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef ring_iterator<T> iterator;
    typedef ring_const_iterator<T> const_iterator;
    typedef ring_reverse_iterator<T> reverse_iterator;
    typedef ring_const_reverse_iterator<T> const_reverse_iterator;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef Alloc allocator_type;

    ring();
    explicit ring(size_type size, const value_type& val = value_type());
    ~ring();

    reference at(size_t n);
    const_reference at(size_t n) const;

    reference back();
    const_reference back() const;

    iterator begin();
    const_iterator begin() const;
//    const_iterator cbegin() const;
//    const_iterator cend() const;
//    const_reverse_iterator crbegin() const;
//    const_reverse_iterator crend() const;
//

    size_type capacity() const;

    bool empty() const;

    iterator end();
    const_iterator end() const;

    reference front();
    const_reference front() const;

    size_t max_size() const;

    ring<T, Alloc>& operator=(const ring<T, Alloc>& obj);
    reference operator[](size_t n);
    const_reference operator[](size_t n) const;

    void pop_back();
    void pop_front();
    void push_back(const value_type& val);
    void push_front(const value_type& val);

//    reverse_iterator rbegin();
//    const_reverse_iterator rbegin() const;
//    reverse_iterator rend();
//    const_reverse_iterator rend() const;

    size_t size() const;

  protected:
    ring(size_t size, T* ptr);

    allocator_type mAllocator;

    //Since a ring iterator contains a left and right-bound pointer, these values are duplicated between mBegin and mEnd.
    //These two iterators could be replaced by four unique pointers, but it would make the code a bit more messy and would
    //require additional iterator construction within the runtime methods.
    iterator mBegin;
    iterator mEnd;

    bool mFixed;

  private:
    size_t GetNewCapacity(size_type min);
    pointer DoAllocateAndConstruct(size_type capacity);
    void DoDestroyAndDeallocate();
  };

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring() :
      mBegin(NULL, NULL, NULL), mEnd(NULL, NULL, NULL), mFixed(false)
  {
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(size_type capacity, const value_type& val) :
      mFixed(false)
  {
    mBegin.mPtr = DoAllocateAndConstruct(capacity);
    mBegin.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mBegin.mPtr + capacity;

    mEnd.mPtr = mBegin.mRightBound;
    mEnd.mLeftBound = mBegin.mLeftBound;
    mEnd.mRightBound = mBegin.mRightBound;

    //Using the raw mPtr is a bit more efficient, as we know the circular iterator logic is not needed.
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::~ring()
  {
    if (!mFixed && (NULL != mBegin.mPtr))
    {
      DoDestroyAndDeallocate();
    }
  }

  template<class T, class Alloc>
  inline T& ring<T, Alloc>::at(size_t n)
  {
    if (n < size())
    {
      return operator[](n);
    }
    else
    {
      throw std::out_of_range("flex::ring.at() - index out-of-bounds");
    }
  }

  template<class T, class Alloc>
  inline const T& ring<T, Alloc>::at(size_t n) const
  {
    if (n < size())
    {
      return operator[](n);
    }
    else
    {
      throw std::out_of_range("flex::ring.at() - index out-of-bounds");
    }
  }

  template<class T, class Alloc>
  inline T& ring<T, Alloc>::back()
  {
    return *(mEnd - 1); //subtract 1 since mEnd points to one after the last element
  }

  template<class T, class Alloc>
  inline const T& ring<T, Alloc>::back() const
  {
    return *(mEnd - 1); //subtract 1 since mEnd points to one after the last element
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::begin()
  {
    return mBegin;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_iterator ring<T, Alloc>::begin() const
  {
    return mBegin;
  }

//  template<class T, class Alloc> const T* ring<T, Alloc>::cbegin() const
//  {
//    return mAryPtr;
//  }
//
//  template<class T, class Alloc> const T* ring<T, Alloc>::cend() const
//  {
//    return &mAryPtr[mSize];
//  }

//  template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::crbegin() const
//  {
//    return array_const_reverse_iterator<T, Alloc>(&mAryPtr[mSize - 1]);
//  }
//
//  template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::crend() const
//  {
//    return array_const_reverse_iterator<T, Alloc>(&mAryPtr[-1]);
//  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::capacity() const
  {
    return (mBegin.mRightBound - mBegin.mLeftBound);
  }

  template<class T, class Alloc>
  inline bool ring<T, Alloc>::empty() const
  {
    return (0 == size());
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::end()
  {
    return mEnd;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_iterator ring<T, Alloc>::end() const
  {
    return mEnd;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::reference ring<T, Alloc>::front()
  {
    return *mBegin;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reference ring<T, Alloc>::front() const
  {
    return *((const_iterator) mBegin);
  }

  template<class T, class Alloc>
  inline size_t ring<T, Alloc>::max_size() const
  {
    return mAllocator.max_size();
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>& ring<T, Alloc>::operator=(const ring<T, Alloc>& obj)
  {
    //TODO: Consider changing to a method call to assign()
    if (obj.size() > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.operator=() - parameter size exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_capacity = GetNewCapacity(obj.size());
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy object values to newly allocated space.
        std::copy(obj.begin(), obj.end(), new_begin);

        //Deallocate and reassign pointers to newly allocated space.
        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + obj.size();
      }
    }
    else
    {
      std::copy(obj.begin(), obj.end(), mBegin);
      mEnd.mPtr = (mBegin + obj.size()).mPtr; //Slightly more efficient than "mEnd = mBegin + obj.size()";
    }
    return *this;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::reference ring<T, Alloc>::operator[](size_t n)
  {
    return mBegin[n];
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reference ring<T, Alloc>::operator[](size_t n) const
  {
    return ((const_iterator) mBegin)[n];
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::pop_back()
  {
    --mEnd;
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::pop_front()
  {
    ++mBegin;
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::push_back(const T& val)
  {
    if (size() == capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.push_back() - size exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + 1;
        size_type new_capacity = GetNewCapacity(new_size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values.
        pointer new_end = std::copy(mBegin, mEnd, new_begin);
        *new_end = val;

        //Deallocate and reassign.
        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + new_size;
      }
    }
    else
    {
      *mEnd = val;
      ++mEnd;
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::push_front(const T& val)
  {
    if (size() == capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.push_front() - size exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + 1;
        size_type new_capacity = GetNewCapacity(new_size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values.
        *new_begin = val;
        std::copy(mBegin, mEnd, (new_begin + 1));

        //Deallocate and reassign.
        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + new_size;
      }
    }
    else
    {
      --mBegin;
      *mBegin = val;
    }
  }

  /*
   template<class T, class Alloc> array_reverse_iterator<T, Alloc> ring<T, Alloc>::rbegin()
   {
   return array_reverse_iterator<T, Alloc>(&mAryPtr[mSize - 1]);
   }

   template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::rbegin() const
   {
   return array_const_reverse_iterator<T, Alloc>(&mAryPtr[mSize - 1]);
   }

   template<class T, class Alloc> array_reverse_iterator<T, Alloc> ring<T, Alloc>::rend()
   {
   return array_reverse_iterator<T, Alloc>(&mAryPtr[-1]);
   }

   template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::rend() const
   {
   return array_const_reverse_iterator<T, Alloc>(&mAryPtr[-1]);
   }
   */
  template<class T, class Alloc>
  inline size_t ring<T, Alloc>::size() const
  {
    return (mEnd - mBegin);
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::GetNewCapacity(size_t min_size)
  {
    // This needs to return a value of at least currentCapacity and at least 1.
    size_t new_capacity = (capacity() > 0) ? (2 * capacity()) : 1;

    // If we are still less than the min_size, just set to the min_size.
    if (new_capacity < min_size)
    {
      return min_size;
    }
    else
    {
      return new_capacity;
    }
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::pointer ring<T, Alloc>::DoAllocateAndConstruct(size_t capacity)
  {
    pointer new_begin;
    //The size allocated is 1 more than the capacity.  This is do to the fact that we don't want begin() to equal end().
    //Therefore there will always be one allocated element that is unused.
    new_begin = mAllocator.allocate(capacity + 1);

    for (pointer it = new_begin; it != (new_begin + capacity + 1); ++it)
    {
      mAllocator.construct(it, value_type());
    }
    return new_begin;
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::DoDestroyAndDeallocate()
  {
    for (pointer it = mBegin.mLeftBound; it != (mBegin.mRightBound + 1); ++it)
    {
      mAllocator.destroy(it);
    }
    //Once again, the allocated size is one more than capacity.  Increment capacity by one when deallocating.
    mAllocator.deallocate(mBegin.mLeftBound, capacity() + 1);
  }

  template<class T, class Alloc>
  inline bool operator==(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    if (lhs.size() != rhs.size())
    {
      return false;
    }
    else
    {
      typename ring<T, Alloc>::const_iterator lit = lhs.begin();
      typename ring<T, Alloc>::const_iterator rit = rhs.begin();
      while (lit != lhs.end())
      {
        if (*lit != *rit)
        {
          return false;
        }
        ++lit;
        ++rit;
      }
      return true;
    }
  }

  template<class T, class Alloc>
  inline bool operator<(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    if (lhs.size() < rhs.size())
    {
      typename ring<T, Alloc>::const_iterator lit = lhs.begin();
      typename ring<T, Alloc>::const_iterator rit = rhs.begin();
      while (lit != lhs.end())
      {
        if (*lit < *rit)
        {
          return true;
        }
        else if (*lit > *rit)
        {
          return false;
        }
        ++lit;
        ++rit;
      }
      return true;
    }
    else
    {
      typename ring<T, Alloc>::const_iterator lit = lhs.begin();
      typename ring<T, Alloc>::const_iterator rit = rhs.begin();
      while (rit != rhs.end())
      {
        if (*lit < *rit)
        {
          return true;
        }
        else if (*lit > *rit)
        {
          return false;
        }
        ++lit;
        ++rit;
      }
      return false;
    }
  }

  template<class T, class Alloc>
  inline bool operator!=(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return !(lhs == rhs);
  }

  template<class T, class Alloc>
  inline bool operator>(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return rhs < lhs;
  }

  template<class T, class Alloc>
  inline bool operator<=(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return !(rhs < lhs);
  }

  template<class T, class Alloc>
  inline bool operator>=(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return !(lhs < rhs);
  }

} //namespace flex

#endif /* FLEX_RING_H */
