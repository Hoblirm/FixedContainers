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
    typedef ring_iterator<T, T*, T&> iterator;
    typedef ring_iterator<T, const T*, const T&> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef Alloc allocator_type;

    ring();
    explicit ring(size_type size, const value_type& val = value_type());
    ring(int size, const value_type& val);
    template<typename InputIterator> ring(InputIterator first, InputIterator last);
    ring(const ring<T, Alloc> & obj);
    ~ring();

    void assign(size_type size, const value_type& val);
    void assign(int size, const value_type& val);
    template<typename InputIterator> void assign(InputIterator first, InputIterator last);

    reference at(size_t n);
    const_reference at(size_t n) const;
    reference back();
    const_reference back() const;
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;
    size_type capacity() const;
    void clear();
    bool empty() const;
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    iterator end();
    const_iterator end() const;
    bool fixed() const;
    reference front();
    const_reference front() const;
    bool full() const;
    allocator_type get_allocator() const;
    iterator insert(iterator position, const value_type& val);
    void insert(iterator position, size_type n, const value_type& val);
    void insert(iterator position, int n, const value_type& val);
    template<typename InputIterator> void insert(iterator position, InputIterator first, InputIterator last);
    size_t max_size() const;
    ring<T, Alloc>& operator=(const ring<T, Alloc>& obj);
    reference operator[](size_t n);
    const_reference operator[](size_t n) const;
    void pop_back();
    void pop_front();
    void push_back(const value_type& val);
    void push_front(const value_type& val);
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    void reserve(size_type n);
    void resize(size_t n, const value_type& val = value_type());
    void shrink_to_fit();
    size_t size() const;
    void swap(ring<T, Alloc>& obj);

  protected:
    ring(size_t size, T* ptr);

    allocator_type mAllocator;

    //Since a ring iterator contains a left and right-bound pointer, these values are duplicated between mBegin and mEnd.
    //These two iterators could be replaced by four unique pointers, but it would make the code a bit more messy and would
    //require additional iterator construction within the functions.
    iterator mBegin;
    iterator mEnd;

    bool mFixed;

  private:
    size_t GetNewCapacity(size_type min);
    pointer AllocateAndConstruct(size_type capacity);
    void DestroyAndDeallocate();
    void DeallocateAndReassign(pointer new_begin, pointer new_end, size_type new_capacity);
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
    mBegin.mPtr = AllocateAndConstruct(capacity);
    mEnd.mPtr = mBegin.mPtr + capacity;

    mBegin.mLeftBound = mEnd.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mEnd.mRightBound = mEnd.mPtr;

    //Using mPtr is a bit more efficient, as we know the the newly allocated data doesn't wrap.
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(int capacity, const value_type& val) :
      mFixed(false)
  {
    mBegin.mPtr = AllocateAndConstruct(capacity);
    mEnd.mPtr = mBegin.mPtr + capacity;

    mBegin.mLeftBound = mEnd.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mEnd.mRightBound = mEnd.mPtr;

    //Using mPtr is a bit more efficient, as we know the the newly allocated data doesn't wrap.
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline ring<T, Alloc>::ring(InputIterator first, InputIterator last) :
      mFixed(false)
  {
    size_type new_size = std::distance(first, last);

    mBegin.mPtr = AllocateAndConstruct(new_size);
    mEnd.mPtr = mBegin.mPtr + new_size;

    mBegin.mLeftBound = mEnd.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mEnd.mRightBound = mEnd.mPtr;

    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::copy(first, last, mBegin.mPtr);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(const ring<T, Alloc> & obj) :
      mFixed(false)
  {
    size_type new_size = obj.size();
    mBegin.mPtr = AllocateAndConstruct(new_size);
    mEnd.mPtr = mBegin.mPtr + new_size;

    mBegin.mLeftBound = mEnd.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mEnd.mRightBound = mEnd.mPtr;

    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::copy(obj.begin(), obj.end(), mBegin.mPtr);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::~ring()
  {
    if (!mFixed && (NULL != mBegin.mPtr))
    {
      DestroyAndDeallocate();
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::assign(size_type new_size, const value_type& val)
  {
    if (new_size > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(new_size);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values.
      std::fill_n(new_begin, new_size, val);

      DeallocateAndReassign(new_begin, new_begin + new_size, new_capacity);
    }
    else
    {
      std::fill_n(mBegin, new_size, val);
      mEnd.mPtr = (mBegin + new_size).mPtr;    //Slightly more efficient than "mEnd = mBegin + size";
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::assign(int new_size, const value_type& val)
  {
    assign((size_type) new_size, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline void ring<T, Alloc>::assign(InputIterator first, InputIterator last)
  {
    size_type new_size = std::distance(first, last);
    if (new_size > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(new_size);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values.
      pointer new_end = std::copy(first, last, new_begin);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      std::copy(first, last, mBegin);
      mEnd.mPtr = (mBegin + new_size).mPtr;    //Slightly more efficient than "mEnd = mBegin + size";
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

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_iterator ring<T, Alloc>::cbegin() const
  {
    return mBegin;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_iterator ring<T, Alloc>::cend() const
  {
    return mEnd;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::crbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::crend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::resize(size_t n, const value_type& val)
  {
    if (n < size())
    {
      mEnd.mPtr = (mBegin + n).mPtr;
    }
    else if (n > size())
    {
      insert(mEnd, n - size(), val);
    }
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::capacity() const
  {
    return (mBegin.mRightBound - mBegin.mLeftBound);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::clear()
  {
    resize(0);
  }

  template<class T, class Alloc>
  inline bool ring<T, Alloc>::empty() const
  {
    return (0 == size());
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::erase(iterator position)
  {
    //This copy will simply shift everything after position over to the left by one.
    //This will effectively overwrite position, erasing it from the container.
    std::copy(position + 1, mEnd, position);
    --mEnd;
    return position;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::erase(iterator first, iterator last)
  {
    //Move all the elements after the erased range to the front of the range.  This
    //will overwrite the erased elements, and the size will be set accordingly.
    std::copy(last, mEnd, first);
    mEnd -= (last - first);
    return first;
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
    return *mBegin;
  }

  template<class T, class Alloc>
  inline bool ring<T, Alloc>::fixed() const
  {
    return mFixed;
  }

  template<class T, class Alloc>
  inline bool ring<T, Alloc>::full() const
  {
    return mBegin.mPtr == (mEnd + 1).mPtr;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::allocator_type ring<T, Alloc>::get_allocator() const
  {
    return mAllocator;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::insert(iterator position, const value_type& val)
  {
    if (full())
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_t new_capacity = GetNewCapacity(new_size);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::copy(mBegin, position, new_begin);

      //Copy the inserted parameter val.
      iterator new_position(new_end, new_begin, new_begin + new_capacity);
      *new_end = val;

      //Copy all values that come after position.
      new_end = std::copy(position, mEnd, ++new_end);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
      return new_position;
    }
    else
    {
      //This copy backwards will shift all the elements after position to the right
      //by one space.  This is valid since the capacity check above ensures we have
      //at least one spot available after the end.
      std::copy_backward(position, mEnd, mEnd + 1);

      *position = val;
      ++mEnd;

      return position;
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
  {
    if ((size() + n) > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + n;
      size_type new_capacity = GetNewCapacity(new_size);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::copy(mBegin, position, new_begin);

      //Fill the parameter val.
      std::fill_n(new_end, n, val);

      //Copy all values that come after position.
      new_end = std::copy(position, mEnd, new_end + n);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      //Slide everything to the right 'n' spaces to make room for the new elements.
      std::copy_backward(position, mEnd, mEnd + n);

      //Insert the new elements into the available space.
      std::fill_n(position, n, val);

      mEnd += n;
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::insert(iterator position, int n, const value_type& val)
  {
    insert(position, (size_type) n, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline void ring<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
  {
    size_type n = std::distance(first, last);
    if ((size() + n) > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + n;
      size_type new_capacity = GetNewCapacity(new_size);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::copy(mBegin, position, new_begin);

      //Copy the inserted range.
      new_end = std::copy(first, last, new_end);

      //Copy all values that come after position.
      new_end = std::copy(position, mEnd, new_end);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      //Slide everything to the right 'n' spaces to make room for the new elements.
      std::copy_backward(position, mEnd, mEnd + n);

      //Insert the new elements into the available space.
      std::copy(first, last, position);

      mEnd += n;
    }
  }

  template<class T, class Alloc>
  inline size_t ring<T, Alloc>::max_size() const
  {
    return mAllocator.max_size();
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>& ring<T, Alloc>::operator=(const ring<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
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
    return mBegin[n];
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
  inline void ring<T, Alloc>::push_back(const value_type& val)
  {
    if (full())
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values.
      pointer new_end = std::copy(mBegin, mEnd, new_begin);
      *new_end = val;
      ++new_end;

      DeallocateAndReassign(new_begin, new_end, new_capacity);
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
    if (full())
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values.
      *new_begin = val;
      pointer new_end = std::copy(mBegin, mEnd, (new_begin + 1));

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      --mBegin;
      *mBegin = val;
    }
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::reverse_iterator ring<T, Alloc>::rbegin()
  {
    return reverse_iterator(mEnd);
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::rbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::reverse_iterator ring<T, Alloc>::rend()
  {
    return reverse_iterator(mBegin);
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::rend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::reserve(size_type n)
  {
    if (n > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(n);
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all current values.
      pointer new_end = std::copy(mBegin, mEnd, new_begin);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::shrink_to_fit()
  {
    if (capacity() > size())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = size();
      pointer new_begin = AllocateAndConstruct(new_capacity);

      //Copy all values.
      pointer new_end = std::copy(mBegin, mEnd, new_begin);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
  }

  template<class T, class Alloc>
  inline size_t ring<T, Alloc>::size() const
  {
    return (mEnd - mBegin);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::swap(ring<T, Alloc>& obj)
  {
    if ((!mFixed) && (!obj.fixed()))
    {
      std::swap(mBegin, obj.mBegin);
      std::swap(mEnd, obj.mEnd);
    }
    else
    {
      if (size() < obj.size())
      {
        iterator it = std::swap_ranges(mBegin, mEnd, obj.begin());
        insert(mEnd, it, obj.end());
        obj.erase(it, obj.end());
      }
      else
      {
        iterator it = std::swap_ranges(obj.begin(), obj.end(), mBegin);
        obj.insert(obj.end(), it, mEnd);
        erase(it, mEnd);
      }
    }
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::GetNewCapacity(size_t min_size)
  {
    // This needs to return a value of at least currentCapacity and at least 1.
    size_t new_capacity = (capacity() > 0) ? (2 * capacity()) : 1;

    // If we are still less than the min_size, just return the min_size.
    return (new_capacity < min_size) ? min_size : new_capacity;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::pointer ring<T, Alloc>::AllocateAndConstruct(size_t capacity)
  {
    if (mFixed)
    {
      throw std::runtime_error("flex::fixed_ring - allocation performed");
      mFixed = false;
    }

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
  inline void ring<T, Alloc>::DestroyAndDeallocate()
  {
    for (pointer it = mBegin.mLeftBound; it != mBegin.mRightBound; ++it)
    {
      mAllocator.destroy(it);
    }

    //Due to the circular nature of the container, mRightBound points to the last element in the allocated space.  This
    //is different than a typical end iterator which points to a place one beyond the last element.  Therefore, the above
    //loop misses the last space.  We don't want to use (mRightBound+1) in the above loop because it doesn't work when
    //both mLeftBound and mRightBound are NULL.  The best option is to simply handle the last element after the loop.
    if (mBegin.mRightBound != NULL)
    {
      mAllocator.destroy(mBegin.mRightBound);
    }

    //Once again, the allocated size is one more than capacity.  Increment capacity by one when deallocating.
    mAllocator.deallocate(mBegin.mLeftBound, capacity() + 1);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::DeallocateAndReassign(pointer new_begin, pointer new_end, size_type new_capacity)
  {
    DestroyAndDeallocate();

    mBegin.mPtr = new_begin;
    mEnd.mPtr = new_end;

    mBegin.mLeftBound = mEnd.mLeftBound = new_begin;
    mBegin.mRightBound = mEnd.mRightBound = new_begin + new_capacity;
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
