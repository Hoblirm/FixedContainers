#ifndef FLEX_RING_H
#define FLEX_RING_H

#include <algorithm>

#include <flex/allocator.h>
#include <flex/ring_iterator.h>

namespace flex
{
  template<class T, class Alloc = allocator<T> >
  class ring_base: public guarded_object
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

  protected:
    allocator_type mAllocator;
    bool mFixed;

    //Since a ring iterator contains a left and right-bound pointer, these values are duplicated between mBegin and mEnd.
    //These two iterators could be replaced by four unique pointers, but it would make the code a bit more messy and would
    //require additional iterator construction within the functions.
    iterator mBegin;
    iterator mEnd;

    ring_base();
    ring_base(size_type n);
    ring_base(pointer new_begin, pointer new_end, pointer right_bound);
    ~ring_base();

    pointer AllocateAndConstruct(size_type n);
    void DestroyAndDeallocate();
  };

  template<class T, class Alloc = allocator<T> > class ring: public ring_base<T, Alloc>
  {
    typedef ring_base<T, Alloc> base_type;
    typedef ring<T, Alloc> this_type;

  public:
    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::allocator_type allocator_type;

    using base_type::mAllocator;
    using base_type::mBegin;
    using base_type::mEnd;
    using base_type::mFixed;
    using base_type::AllocateAndConstruct;
    using base_type::DestroyAndDeallocate;

    ring();
    explicit ring(size_type size, const value_type& val = value_type());
    ring(int size, const value_type& val);
    template<typename InputIterator> ring(InputIterator first, InputIterator last);
    ring(const ring<T, Alloc> & obj);

    void assign(size_type size, const value_type& val);
    void assign(int size, const value_type& val);
    template<typename InputIterator> void assign(InputIterator first, InputIterator last);

    reference at(size_type n);
    const_reference at(size_type n) const;
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
    size_type max_size() const;
    ring<T, Alloc>& operator=(const ring<T, Alloc>& obj);
    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    void pop_back();
    void pop_front();
    void push_back(const value_type& val);
    void push_front(const value_type& val);
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    void reserve(size_type n);
    void resize(size_type n, const value_type& val = value_type());
    void shrink_to_fit();
    size_type size() const;
    void swap(ring<T, Alloc>& obj);

  protected:
    ring(pointer new_begin, pointer new_end, pointer right_bound);

    size_type GetNewCapacity(size_type min);
    void DeallocateAndReassign(pointer new_begin, pointer new_end, size_type new_capacity);
  };

  /*
   * ring_base
   */
  template<class T, class Alloc>
  inline ring_base<T, Alloc>::ring_base() :
      mFixed(false), mBegin(NULL, NULL, NULL), mEnd(NULL, NULL, NULL)

  {
  }

  template<class T, class Alloc>
  inline ring_base<T, Alloc>::ring_base(size_type n) :
      mFixed(false), mBegin(AllocateAndConstruct(n), n), mEnd(mBegin.mRightBound, mBegin.mPtr, mBegin.mRightBound)

  {
  }

  template<class T, class Alloc>
  inline ring_base<T, Alloc>::ring_base(pointer new_begin, pointer new_end, pointer right_bound) :
      mFixed(true), mBegin(new_begin, new_begin, right_bound), mEnd(new_end, new_begin, right_bound)

  {
    if (FLEX_UNLIKELY(new_end > right_bound))
    {
      throw std::runtime_error("flex::fixed_ring - constructor() size exceeds capacity");
    }
  }

  template<class T, class Alloc>
  inline ring_base<T, Alloc>::~ring_base()
  {
    if (!mFixed && (NULL != mBegin.mPtr))
    {
      DestroyAndDeallocate();
    }
  }

  template<class T, class Alloc>
  inline typename ring_base<T, Alloc>::pointer ring_base<T, Alloc>::AllocateAndConstruct(size_type capacity)
  {
    if (FLEX_UNLIKELY(mFixed))
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
  inline void ring_base<T, Alloc>::DestroyAndDeallocate()
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
    mAllocator.deallocate(mBegin.mLeftBound, (mBegin.mRightBound - mBegin.mLeftBound) + 1);
  }

  /*
   * ring
   */
  template<class T, class Alloc>
  inline ring<T, Alloc>::ring() :
      base_type()
  {
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(size_type capacity, const value_type& val) :
      base_type(capacity)
  {
    //Using mPtr is a bit more efficient, as we know the the newly allocated data doesn't wrap.
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(int capacity, const value_type& val) :
      base_type(capacity)
  {
    //Using mPtr is a bit more efficient, as we know the the newly allocated data doesn't wrap.
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline ring<T, Alloc>::ring(InputIterator first, InputIterator last) :
      base_type(std::distance(first, last))
  {
    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::copy(first, last, mBegin.mPtr);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(const ring<T, Alloc> & obj) :
      base_type(obj.size())
  {
    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::copy(obj.begin(), obj.end(), mBegin.mPtr);
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
  inline typename ring<T, Alloc>::reference ring<T, Alloc>::at(size_type n)
  {
    if (FLEX_LIKELY(n < size()))
    {
      return operator[](n);
    }
    else
    {
      throw std::out_of_range("flex::ring.at() - index out-of-bounds");
    }
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reference ring<T, Alloc>::at(size_type n) const
  {
    if (FLEX_LIKELY(n < size()))
    {
      return operator[](n);
    }
    else
    {
      throw std::out_of_range("flex::ring.at() - index out-of-bounds");
    }
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::reference ring<T, Alloc>::back()
  {
    return *(mEnd - 1); //subtract 1 since mEnd points to one after the last element
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reference ring<T, Alloc>::back() const
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
  inline void ring<T, Alloc>::resize(size_type n, const value_type& val)
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
    return (mBegin.mPtr == mEnd.mPtr);
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
    //Increment is performed first as it allows a much faster capacity check. The
    //drawback is that the iterator needs to be reverted if reallocation occurs.
    const iterator prev_end = mEnd;
    ++mEnd;

    if (mEnd.mPtr == mBegin.mPtr)
    {
      //Capacity has been exceeded. Put container back in a valid
      //state and reallocate.
      mEnd.mPtr = prev_end.mPtr;

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
      //by one space. 
      std::copy_backward(position, prev_end, mEnd);

      *position = val;

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
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::max_size() const
  {
    if (mFixed)
    {
      return capacity();
    }
    else
    {
      return mAllocator.max_size();
    }
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>& ring<T, Alloc>::operator=(const ring<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::reference ring<T, Alloc>::operator[](size_type n)
  {
    return mBegin[n];
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reference ring<T, Alloc>::operator[](size_type n) const
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
    //Increment is performed first as it allows a much faster capacity check. The
    //drawback is that the iterator needs to be reverted if reallocation occurs.
    const pointer prev_end = mEnd.mPtr;
    ++mEnd;

    if (mEnd.mPtr == mBegin.mPtr)
    {
      //Capacity has been exceeded. Put container back in a valid
      //state and reallocate.
      mEnd.mPtr = prev_end;

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
      *prev_end = val;
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::push_front(const value_type& val)
  {
    //Decrement is performed first as it allows a much faster capacity check. The
    //drawback is that the iterator needs to be incremented if reallocation occurs.
    --mBegin;

    if (mBegin.mPtr == mEnd.mPtr)
    {
      //Capacity has been exceeded. Put container back in a valid
      //state and reallocate.
      ++mBegin;

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
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::size() const
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
        iterator it = std::swap_ranges(mBegin, mEnd, obj.mBegin);
        insert(mEnd, it, obj.mEnd);
        obj.erase(it, obj.mEnd);
      }
      else
      {
        iterator it = std::swap_ranges(obj.mBegin, obj.mEnd, mBegin);
        obj.insert(obj.mEnd, it, mEnd);
        erase(it, mEnd);
      }
    }
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(pointer new_begin, pointer new_end, pointer right_bound) :
      base_type(new_begin, new_end, right_bound)

  {
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::GetNewCapacity(size_type min_size)
  {
    // This needs to return a value of at least currentCapacity and at least 1.
    size_type new_capacity = (capacity() > 0) ? (2 * capacity()) : 1;

    // If we are still less than the min_size, just return the min_size.
    return (new_capacity < min_size) ? min_size : new_capacity;
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
