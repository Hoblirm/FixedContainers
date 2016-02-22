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
    //TODO: Get template to work with range constructor to use multiple iterator types
    ring(const_iterator first, const_iterator last);
    ring(const_pointer first, const_pointer last);
    ring(const ring<T, Alloc> & obj);
    ~ring();

    void assign(size_t size, const value_type& val);
    //TODO: Get template to work with ring assign() to use multiple iterators.
    void assign(const_iterator first, const_iterator last);
    void assign(const_pointer first, const_pointer last);

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
    allocator_type get_allocator() const;

    iterator insert(iterator position, const value_type& val);
    //TODO: Get template to work with ring insert() to use multiple iterators.
    void insert(iterator position, size_t n, const value_type& val);
    void insert(iterator position, const_iterator first, const_iterator last);
    void insert(iterator position, const_pointer first, const_pointer last);

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
    size_t size() const;

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

    //Using mPtr is a bit more efficient, as we know the the newly allocated data doesn't wrap.
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(const_iterator first, const_iterator last) :
      mFixed(false)
  {
    size_type new_size = last - first;
    mBegin.mPtr = DoAllocateAndConstruct(new_size);
    mBegin.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mBegin.mPtr + new_size;

    mEnd.mPtr = mBegin.mRightBound;
    mEnd.mLeftBound = mBegin.mLeftBound;
    mEnd.mRightBound = mBegin.mRightBound;

    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::copy(first, last, mBegin.mPtr);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(const_pointer first, const_pointer last) :
      mFixed(false)
  {
    size_type new_size = last - first;
    mBegin.mPtr = DoAllocateAndConstruct(new_size);
    mBegin.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mBegin.mPtr + new_size;

    mEnd.mPtr = mBegin.mRightBound;
    mEnd.mLeftBound = mBegin.mLeftBound;
    mEnd.mRightBound = mBegin.mRightBound;

    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::copy(first, last, mBegin.mPtr);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(const ring<T, Alloc> & obj) :
      mFixed(false)
  {
    size_type new_size = obj.size();
    mBegin.mPtr = DoAllocateAndConstruct(new_size);
    mBegin.mLeftBound = mBegin.mPtr;
    mBegin.mRightBound = mBegin.mPtr + new_size;

    mEnd.mPtr = mBegin.mRightBound;
    mEnd.mLeftBound = mBegin.mLeftBound;
    mEnd.mRightBound = mBegin.mRightBound;

    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::copy(obj.begin(), obj.end(), mBegin.mPtr);
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
  inline void ring<T, Alloc>::assign(size_type size, const value_type& val)
  {
    if (size > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.assign() - fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_capacity = GetNewCapacity(size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values.
        std::fill_n(new_begin, size, val);

        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + size;
      }
    }
    else
    {
      std::fill_n(mBegin, size, val);
      mEnd.mPtr = (mBegin + size).mPtr;    //Slightly more efficient than "mEnd = mBegin + size";
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::assign(const_iterator first, const_iterator last)
  {
    size_type size = last - first;
    if (size > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.assign() - iterator range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_capacity = GetNewCapacity(size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values.
        std::copy(first, last, new_begin);

        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + size;
      }
    }
    else
    {
      std::copy(first, last, mBegin);
      mEnd.mPtr = (mBegin + size).mPtr;    //Slightly more efficient than "mEnd = mBegin + size";
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::assign(const_pointer first, const_pointer last)
  {
    size_type size = last - first;
    if (size > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.assign() - pointer range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_capacity = GetNewCapacity(size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values.
        std::copy(first, last, new_begin);

        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + size;
      }
    }
    else
    {
      std::copy(first, last, mBegin);
      mEnd.mPtr = (mBegin + size).mPtr;    //Slightly more efficient than "mEnd = mBegin + size";
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

  template<class T, class Alloc> typename ring<T, Alloc>::const_iterator ring<T, Alloc>::cbegin() const
  {
    return mBegin;
  }

  template<class T, class Alloc> typename ring<T, Alloc>::const_iterator ring<T, Alloc>::cend() const
  {
    return mEnd;
  }

  template<class T, class Alloc> typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::crbegin() const
  {
    return const_reverse_iterator((mEnd - 1).mPtr, mEnd.mLeftBound, mEnd.mRightBound);
  }

  template<class T, class Alloc> typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::crend() const
  {
    return const_reverse_iterator((mBegin - 1).mPtr, mBegin.mLeftBound, mBegin.mRightBound);
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::capacity() const
  {
    return (mBegin.mRightBound - mBegin.mLeftBound);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::clear()
  {
    assign((pointer)NULL,(pointer)NULL);
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

  template<class T, class Alloc> bool ring<T, Alloc>::fixed() const
  {
    return mFixed;
  }

  template<class T, class Alloc> typename ring<T, Alloc>::allocator_type ring<T, Alloc>::get_allocator() const
  {
    return mAllocator;
  }

  template<class T, class Alloc> typename ring<T, Alloc>::iterator ring<T, Alloc>::insert(iterator position,
      const value_type& val)
  {
    if (size() == capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.insert() - capacity exceeded");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + 1;
        size_t new_capacity = GetNewCapacity(new_size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        pointer new_end = std::copy(mBegin, position, new_begin);

        //Copy the inserted parameter val.
        iterator new_position;
        new_position.mPtr = new_end;
        new_position.mLeftBound = new_begin;
        new_position.mRightBound = new_begin + new_capacity;
        *new_end = val;

        //Copy all values that come after position.
        new_end = std::copy(position, mEnd, ++new_end);

        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_position.mRightBound;
        mEnd = mBegin + new_size;
        return new_position;
      }
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

  template<class T, class Alloc> void ring<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
  {
    if ((size() + n) > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.insert() - fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + n;
        size_type new_capacity = GetNewCapacity(new_size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        pointer new_end = std::copy(mBegin, position, new_begin);

        //Fill the parameter val.
        std::fill_n(new_end, n, val);

        //Copy all values that come after position.
        new_end = std::copy(position, mEnd, new_end + n);

        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + new_size;
      }
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

  template<class T, class Alloc> void ring<T, Alloc>::insert(iterator position, const_iterator first,
      const_iterator last)
  {
    size_type n = (last - first);
    if ((size() + n) > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.insert() - fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + n;
        size_type new_capacity = GetNewCapacity(new_size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        pointer new_end = std::copy(mBegin, position, new_begin);

        //Copy the inserted range.
        new_end = std::copy(first, last, new_end);

        //Copy all values that come after position.
        new_end = std::copy(position, mEnd, new_end);

        //Deallocate and reassign
        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + new_size;
      }
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

  template<class T, class Alloc> void ring<T, Alloc>::insert(iterator position, const_pointer first, const_pointer last)
  {
    size_type n = (last - first);
    if ((size() + n) > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.insert() - fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + n;
        size_type new_capacity = GetNewCapacity(new_size);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        pointer new_end = std::copy(mBegin, position, new_begin);

        //Copy the inserted range.
        new_end = std::copy(first, last, new_end);

        //Copy all values that come after position.
        new_end = std::copy(position, mEnd, new_end);

        //Deallocate and reassign
        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + new_size;
      }
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

  template<class T, class Alloc> typename ring<T, Alloc>::reverse_iterator ring<T, Alloc>::rbegin()
  {
    return reverse_iterator((mEnd - 1).mPtr, mEnd.mLeftBound, mEnd.mRightBound);
  }

  template<class T, class Alloc> typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::rbegin() const
  {
    return const_reverse_iterator((mEnd - 1).mPtr, mEnd.mLeftBound, mEnd.mRightBound);
  }

  template<class T, class Alloc> typename ring<T, Alloc>::reverse_iterator ring<T, Alloc>::rend()
  {
    return reverse_iterator((mBegin - 1).mPtr, mBegin.mLeftBound, mBegin.mRightBound);
  }

  template<class T, class Alloc> typename ring<T, Alloc>::const_reverse_iterator ring<T, Alloc>::rend() const
  {
    return const_reverse_iterator((mBegin - 1).mPtr, mBegin.mLeftBound, mBegin.mRightBound);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::reserve(size_type n)
  {
    if (n > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::ring.reserve() - exceeded capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size();
        size_type new_capacity = GetNewCapacity(n);
        pointer new_begin = DoAllocateAndConstruct(new_capacity);

        //Copy all current values.
        std::copy(mBegin, mEnd, new_begin);

        DoDestroyAndDeallocate();
        mBegin.mPtr = new_begin;
        mBegin.mLeftBound = new_begin;
        mBegin.mRightBound = new_begin + new_capacity;
        mEnd = mBegin + new_size;
      }
    }
  }

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

//MY PRECIOUS!!!
//hmdhdmmmNmdhmmmmmmdmmmmNNNdmmNNNNmNmmmNmmNmmdmmmmysddhymmmdyysyhhhhhhyyyyyssyyyyhhhossdNNddNNNNNNNmdmmmmmmmmmmmddddddmmm
//mmddNmmNNNmmdmmmNmmmmmmmddmmmNNNmNddmNNNmmhddmddhdhddddmmdhysssssssyyysoo+++++++o+++oyyhymNNNNNNNNmmmmddmmddmmmmmmmdmmmm
//mhhdNNmNmddmmmmNNNmmmhdmmmmNNmmNNNmmdNmNNmhhddmdmmmdhddhyssssssssssooo+oo++++++oo+/////+oydmNNNNNNmddmddmmdddmmddddddddd
//mdddmdmNmdmmmhmmmmmmmmmmmdNNmmmmNNmmmNNNNdddddhhdddhyysssssyyyysso++osssoo++++o++++++//:::/ohmmNNNmdhdmmmdddddddmmmmmdhh
//mdmdmmmmmdmmmdmmmmNmmhmmmmNNNNNNNNmmmdmNmddddddyhhhyssssssssooo+ooossoooooooo+++///+++/////::+shddmmdmmmmdddmmddddddmmdd
//dmmdmdNdmhhmmmmdddmmhmNmmmmmmmmmdmddddmmddhddhhhhyyssyyysoo++oooooooooo++++o+++++////+++////::::+shhdddddddddddddddddddh
//mmmddmmmdhmmdNNdmmdyydmdNNNmmmmNmmmdmmmdddddhhhyyyyyyhysooo+ooooooooooo+++++++++++++////+///:::--:/oyhdhddddddddddmmmmmd
//mmmddmdhmmmdmmmddmmmmmmdmdmmmmdmNdmNmmNmhhddhyyyyyyyyyssoooooosooooo++++++++++++++++//////+//:------/yhhddddddddmmmmmmmm
//mmdmmmmmmNNNmmmddmNNmmmmmmmmNmmmmdmNNmmdhddhyyyyyyyysssooooossosoo++++++++++++++++++///++////::----.-+yddddhhdddddmmmmmm
//mddmmNmmmNmhdmmmmNNmddmmmmmNmmdmddmNmmmdddhyyyyyysyyssssooossssoooo++++oo++++++++++++/////++/:::::-..-/yhdddhhdmmmmmmmmm
//mdmmmmmmmmmmmmmdmmNdddmmmmdmmmdmmmmmdmmddhyyyyyysyssssssosssooooo++++++++oo++++o++++++/+++++//::::-..--ohdmmmmmmNNNmmmmm
//hdmdyyddddmddddhddddmmmmmmmmmmmmmmNddhdddhhhhhysyssssssossssoooooo+o+++ooooooooo++++o++++++o++/::--...-:ydmmmmNNNNNmmmmm
//dmmhhhmmddmmddddddhhdddhhddmdddddddhdddddhhhhssssssooossooooooooooooooooooooooooooooooooooo+o++///:-..-:+hhddmmmmmmmmmmm
//dddmddmNddmmddddmmmddhddmhhydhhddddddddddhhhyssssssosssoo++ooooo+++oooosossossoooooooooooooooo+++//:-.--:ssyhhhhhhdddhyy
//hhhmmmmmhhddhhdddhdddddddhhhhhmmmmddhhddhhhhyyyyssssssooooooooooooooooooooooossoooooooooooo+oo+++++/-----+yyhyssssssssoo
//dmhydmdmdddhhhhdhhhmdddhyyyssyyhhdmhhhdddhhyyyyssoyyssoo++oo+oossssssosssoooooooooooooooo+o+oo++////:----:ssyssossyyyyhh
//mmmddddmmhddhddhshhyyddhsshhyysssyhdddmmdhhyyyssssysso+oooooooooooooossssoossssooooooooooooooo++//+/::-.--ossyhyyhddddmm
//mmmmdmmhdddhdddddhyhhmdysyhhhhyyyssyhddmmdhyyysssyooooooooooosooooooooosossoossooooooooo++++oo++////:-----+yyhhyhdmNNNNN
//mmdmmdmhhdhdhdmdddyhyhhhdhsyhmmdyyssoyyddhhhyyssyooo++++++++oossoooooooooooooosooooooo+++++o+++//////:----:hhdddmmNNmmNN
//mmdmdhddddhdhdddhhhdhyhdNhsyhdmmmdyysssyysyyysssooooo+oso+++++oossssosssoooooosooooooo+++++++++///////:----yddmmmNNNNNNN
//mhmmmmddmddddddhdddhhhhhmhyyhdmmmNmdyyyyssyyyssoooooosssssoooo+ooossssssossssssooooooooooo+++++++++++/:----ydddhyyhdNNNN
//mddmmNdhdhhhhhdddmmhdmdmddhdmmmmmmmNmhyhssyyssoossssyyyyhhyyyssssssoossssosssssooosssoooooooooo++o++++/:---sdhs+//:/mNNN
//hdmmmNmhhdddmhdddmddmNmmmmdhdmmmmmmNNmhyssssyssssssyyyyyyhhhhhyyyssysssssssssssssssyssooooooooooo++++++/:-.oso+++//ymmmm
//dmmmmmhddhhdmdmdmmdddmmmmmmyyhdmmmmNNNdyssosssssssyyhhhddddddddhhhyyyyyyyyyyyyyssyyysssssssssssssssossso+:-:++o+oo/osssy
//mmmNNmmddmmmdmNNmddhhmmmmNdsyyyyhdmNNhsssssosysssyhhhdhsoshdmNNmmddddddddddddhhyhhhhhhhhhyyyyyyyyyyyyssoo+:-/ossss/:yyys
//mdmNmddmmNmmdmmmmmddddddmdmyyoooosyhhsyssssooyssyhddhs++yhdddmmNNNmmmmmmmmmmmddhhddddddddddhhhhhhhhhhhyso/:-ymdhyo/osyys
//dmNMNdddmmdmmmdddmmddddmdhddhysoooo+shysooooossyhhmdhsoshshmNmhmmNNNNNmNmmmmmmddhhdmmmmmmdddmmmmmmdsoshys+-:dddyooyhyyss
//mmmNmmddmmmmmmhddmNmmmmmmmmmdhdhyoo+oshyssssossyyyyhhhssysyhdddddmNNMNNNNNmmmddhhhdmmmmmmNmNNmNNmdhd/:/sy+:.yhs+oddddhhd
//NmmdhddddmmddmdhmmddmmmmddmdhhhdmdhyssyyysssyyhyyhyyyyyyhhhddmmmmNNNNNNNNNmmdhhhyyhmmmNNNNmNNNNNmhhho::/yo/.oyoommdmmmmN
//mmmdmdddhddmNmmmdmmdddmddmmmhdhdmmdddddyyyyhysyyyyhdddhhhhhhhhdddmmNNNNNNNmhso++ooymNNNNNNmmNNmdhhys++/+s:/--s/hmmmmmmmN
//mmNmmhdmdddmmmmhdmdddymmdddhmmmddmddhddhhhhyssossyyhddmmmmmmmmmmmmmddddhhysooooo+osdNNNNNNNmddhhhhyyssso+/:--+smmmmmmmmm
//mmmmmdmmdmmddmmmmhdhhhmdmdddddhhmNddhhddmdysssssyyyyhhdmmmmmmmmmmdhyyyhyyssosoooo+osyhmmNNNmmddhhhhhhyys+:::/ymmmmmmmmdd
//mmmmNNmdmddmmmmdhddmmdmmmmmydhyhddhdddmdddhyyyyyyyyyyhhddddddddmmmddddhyssoossooo+osyyhddddmmmmmmmmdhyso/--/dNNmmmmdmmdd
//NNNNNNmmNmdmmddmdmmmmmmmdddhdhdddhdddhhysydhyhhhhhhhhhhhhdddddmmmmmmmdysosoooooooo+shddhhyyyyyhhhhhyyso+/-:hNNNNmmmmmddd
//mmNNmmmmddmmmmdmmNmmdmdddddddhhhhdyhhhhyyhdddmmmdddddddhhdddddmmmmdhhddhsooooossoooosdmmddhyyyyyyhhhyso+/-hNNNNNmmmmmmdd
//mmmNNNmmmmmmmmmNNddhhhdddmmdddddyyhyyhhyyysdmmmmmmmmddddddddmmmmdyysyydmdhysyyssssssyhmmmmdddhhhhhyyso+/:omNNmmmmmmmmmmm
//NNNNmNNmNmmmmmmNmddhhhhhhmmmdhysyyhhyysssssymmmmmmmmmmdmmmmmmmdhysssssyhdmmdhhhhdhddhyydmmmmddddhhyyoo++smNmmmmmmmmmmmmd
//NNNNNNNNNmdmmNmmmhddddhhhdhhyyssssssyssoossydmmmmmmmmmmmmmmmmdyssssssssyydmmmmmmddhhhyyyydmmmmddhhysosyhdmmmmmmmdddddddd
//mmNNNNNNmmmmdmmmdhddhdhyysssssssssssssooydmmmmmmmmmmmmmmmmmddhyssssssyyyyyhdmmdddhhhyyyyssymmmdhysshhhdddmdddmmddhhyyhdd
//mNNNNNNmdddhdmmddhhyyssssoooossoossossoydmmdmmmmmmmmmmdmmmddhhyyyysssyyyyyyhddddhhhhysssssoymdhsoyddhdddmdddhhhhyyyssyhh
//mNNmNmmmmmmhdmdyssssssosoooooosossssoooydmdddmmNmmmmmmddmmmmmddddhhyyyyyyyyhhddhhhyhysssyyoohyo+ydhdhhyhhhyyyysssyyssyhh
//NmmmmmmmmNmddhssoooooooooooooosooosooosydddddmmmmmmmmmdddmmmmNNNNNmmmmmmdmmmmmmmmdddhhyhhdyss++yddhhhhyyhhyyyyssyhhyyyyy
//NNmdmmddNmdysooooooooooooooooosoooosoossdddmmmmmmmNNmmmhhhhyhhhdmmNNNNNNNNNNNNNNNNmddmhhddyo///oyhhhhhhhdhhhdddddmmdhhhh
//mNNmNmmmNmyooooooossoosssssssssoossssssshddmmmmmmmmNmmmhyyyyyyhhhhdddNNNNNNNmNmmNddhdhosyso//sooooyhdddmmmdddddmmmmmmmmm
//mmNmmmmNhsooooooossoossssosssssysyyyssyyyddmmmmmmmmmmmmmhysyyyyhhhhyhhhddddmddmmmmdhysoooo/+hdhyys+sddmNNNmmmdddmmmmmmdm
//mmNmdmNy+oooooooooooosooooosssyyyyyyhhhhhdmmmmmmmmmmmmmmdhyyyyyyyhhhddddhhhhhddddddys++oo/odmmddhys++hNNNNNNNmddmmmmmmmm
//NNNmmds+ooooooooooooooosossssyyhyhhhhhhddmmmNmmmmmmmmmmmmddhhyyyyyhdddmmmddddddddhhyo++o+sddmmmddhyo//dNNNNNNNNmmmmmmmNm
//NNNmh+ooooooooooooooooosssssyyyhyyhhhhddmdmmmNNNNmmmmmmmmmmmdhyyyyhhddddddddddddhhys++oshdmmmmmmddhy+-+NNNNNNNNNNNNNNNNN
//NmNhoosssoooooooooosssssyyyyyyyhhyhdhhddddmmmmmNNNNmmmmmmmmmmmdhhhhhyyhhhhdddhhhhyyooyhdmmmmmmmmdddhs/:mNNNNNNNNNNNNNNNN
//NmhssssssoooooooossssyyyyyyyyhhhhhhddhhddmmdddmmmNNNNmmmmmmmmmmmdddhhhdddddddddhyysydmmmmmmmmmmmdddhy+/mNNNNNNNNNNNNNNNN
//Ndyssooooooossssssssssyyyyhhhddddddddhhhddhhddmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmdhyhdmmmmmmmdmmmmdddddho+mNNNNNNNNNNNNNNNM
//NhysoooooooossssssssssyyyhdddddddddddhhhhyyhhddmmmmmmmmmmmmmmmmmmmmmmmmmmmmdddddmmmmmmmmddddmmmmmmmdhosNNNNMNNNNNMNNMMMM
//dyysoooooooossssssssyyyhhmmmmddmmmddddhhhyyyyyhdmmmmmmmmmmNNNmmmmmmmmmmmmddddmmmmmmmmmmmddmmmmmmmmmdysmMMNMMMMMMMMMMMMMM
//hyssoooooosssssssssyyyhdmmmmmmmmmmmddddddhhhyyyhddddmmmNmmmmmmddddmmmmmddddmmmmmmmmmmmmmmmmmmmmmmmmdymMMMNMMMMNNNNNNNMMM
//hysssosoossssssssssyhhdmmmmmmmmmmmmmmmdmdddhhhyyyhddmmmmmmmmmdddddddddddmmmmmmmmmmmmmmmmmmmmmmmmmddhmMMMMMNNNNNNNNNNNNNM
//hysysoooossssssssyyhhdmmmmmmmmmmmmmmmmmmmdddhhhhhhhdmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmdddmmmdymNNNNMMNNNNNNMMMNNNNM
//hyssssssssssssssyyyhdmmNmmmmmmmmmmmmmmmmmmmmmmmmmddmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmdddddmmdyhNNNNNNNNNMMMMMMMMMNNN
//hyyyssssssssssssyyhdmmNNmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmdmmmmmmmmmmmmdddddmmh+oo++++ooshdNNMMMMNNNNN
//dhyysssssyyyyyyyhhdmmNNmmmmmmmmmmmmmmmmmmmddmmmmmmmmmmmmmmmmmmmmmmmmmmmmmdhhdmmmmmmmmmmmmmdddmmmy/yssooosssso++dNMNNNNNN
//dhyyyysssyyyyyyhhdmmmNNNmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmhydmNmmmmmmmmmmmmmddmmmds/hddddddddddhy+sNNNNNNN
