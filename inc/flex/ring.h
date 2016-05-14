#ifndef FLEX_RING_H
#define FLEX_RING_H

#include <flex/ring_iterator.h>
#include <flex/allocator.h>
#include <flex/initializer_list.h>

#include <algorithm>
#include <memory>

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

    //Since a ring iterator contains a left and right-bound pointer, these values are duplicated between mBegin and mEnd.
    //These two iterators could be replaced by four unique pointers, but it would make the code a bit more messy and would
    //require additional iterator construction within the functions.
    iterator mBegin;
    iterator mEnd;
    allocator_type mAllocator;
    bool mFixed;

    ring_base();
    ring_base(size_type n);
    ring_base(pointer new_begin, pointer new_end, pointer right_bound);
    ~ring_base();

    pointer Allocate(size_type n);
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
    using base_type::Allocate;
    using base_type::DestroyAndDeallocate;

    ring();
    explicit ring(size_type size, const value_type& val = value_type());
    ring(int size, const value_type& val);
    template<typename InputIterator> ring(InputIterator first, InputIterator last);
    ring(const ring<T, Alloc> & obj);
    ring(std::initializer_list<value_type> il);
#ifdef FLEX_HAS_CXX11
    ring(ring<T, Alloc> && obj);
#endif
    void assign(size_type size, const value_type& val);
    void assign(int size, const value_type& val);
    template<typename InputIterator> void assign(InputIterator first, InputIterator last);
    void assign(std::initializer_list<value_type> il);

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
#ifdef FLEX_HAS_CXX11
    template<class...Args> iterator emplace(iterator position, Args&&... args);
    template<class...Args> void emplace_back(Args&&... args);
    template<class...Args> void emplace_front(Args&&... args);
#endif
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
#ifdef FLEX_HAS_CXX11
    iterator insert(iterator position, value_type&& val);
#endif
    void insert(iterator position, size_type n, const value_type& val);
    void insert(iterator position, int n, const value_type& val);
    template<typename InputIterator> void insert(iterator position, InputIterator first, InputIterator last);
    size_type max_size() const;
    ring<T, Alloc>& operator=(const ring<T, Alloc>& obj);
    ring<T, Alloc>& operator=(std::initializer_list<value_type> il);
#ifdef FLEX_HAS_CXX11
    ring<T, Alloc>& operator=(ring<T, Alloc>&& obj);
#endif
    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    void pop_back();
    void pop_front();
    void push_back(const value_type& val);
    void push_front(const value_type& val);
#ifdef FLEX_HAS_CXX11
    void push_back(value_type&& val);
    void push_front(value_type&& val);
#endif
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    void reserve(size_type n);
    void resize(size_type n, const value_type& val = value_type());
    void shrink_to_fit();
    size_type size() const;
    void swap(ring<T, Alloc>& obj);
#ifdef FLEX_HAS_CXX11
    void swap(ring<T, Alloc>&& obj);
#endif

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
      mBegin(NULL, NULL, NULL), mEnd(NULL, NULL, NULL), mFixed(false)

  {
  }

  template<class T, class Alloc>
  inline ring_base<T, Alloc>::ring_base(size_type n) :
      mBegin(Allocate(n), n), mEnd(mBegin.mRightBound, mBegin.mPtr, mBegin.mRightBound), mFixed(false)

  {
  }

  template<class T, class Alloc>
  inline ring_base<T, Alloc>::ring_base(pointer new_begin, pointer new_end, pointer right_bound) :
      mBegin(new_begin, new_begin, right_bound), mEnd(new_end, new_begin, right_bound), mFixed(true)

  {
#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(new_end > right_bound))
    {
      size_type n = new_end - new_begin;
      mFixed = false;
      new_begin = Allocate(n);
      new_end = new_begin + n;

      mBegin.mPtr = mBegin.mLeftBound = mEnd.mLeftBound = new_begin;
      mEnd.mPtr = mBegin.mRightBound = mEnd.mRightBound = new_end;

      flex::error_msg("flex::fixed_ring - constructor() size exceeds capacity");
    }
#endif
  }

  template<class T, class Alloc>
  inline ring_base<T, Alloc>::~ring_base()
  {
    flex::destruct_range(mBegin, mEnd);
    if (!mFixed)
    {
      //Once again, the allocated size is one more than capacity.  Increment capacity by one when deallocating.
      mAllocator.deallocate(mBegin.mLeftBound, (mBegin.mRightBound - mBegin.mLeftBound) + 1);
    }
  }

  template<class T, class Alloc>
  inline typename ring_base<T, Alloc>::pointer ring_base<T, Alloc>::Allocate(size_type capacity)
  {
    //The size allocated is 1 more than the capacity.  This is due to the fact that we don't want begin() to equal end().
    //Therefore there will always be one allocated element that is unused.
    return mAllocator.allocate(capacity + 1);
  }

  template<class T, class Alloc>
  inline void ring_base<T, Alloc>::DestroyAndDeallocate()
  {
    flex::destruct_range(mBegin, mEnd);

#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(mFixed))
    {
      mFixed = false;
      flex::error_msg("flex::fixed_ring - capacity exceeded");
    }
    else
#endif

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
    std::uninitialized_fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(int capacity, const value_type& val) :
      base_type(capacity)
  {
    //Using mPtr is a bit more efficient, as we know the the newly allocated data doesn't wrap.
    std::uninitialized_fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline ring<T, Alloc>::ring(InputIterator first, InputIterator last) :
      base_type(std::distance(first, last))
  {
    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::uninitialized_copy(first, last, mBegin.mPtr);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(const ring<T, Alloc> & obj) :
      base_type(obj.size())
  {
    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::uninitialized_copy(obj.begin(), obj.end(), mBegin.mPtr);
  }

  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(std::initializer_list<value_type> il) :
      base_type(il.size())
  {
    //Using the mBegin.mPtr is a bit more efficient, as we know the newly allocated data doesn't wrap.
    std::uninitialized_copy(il.begin(), il.end(), mBegin.mPtr);
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline ring<T, Alloc>::ring(ring<T, Alloc> && obj) :
  base_type()
  {
    swap(std::move(obj));
  }
#endif

  template<class T, class Alloc>
  inline void ring<T, Alloc>::assign(size_type n, const value_type& val)
  {
    if (n > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(n);
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      std::uninitialized_fill_n(new_begin, n, val);

      DeallocateAndReassign(new_begin, new_begin + n, new_capacity);
    }
    else
    {
      if (n < size())
      {
        const iterator new_end = mBegin + n;
        std::fill_n(mBegin, n, val);
        flex::destruct_range(new_end, mEnd);
        mEnd.mPtr = new_end.mPtr;
      }
      else
      {
        const iterator new_end = mBegin + n;
        std::fill(mBegin, mEnd, val);
        std::uninitialized_fill(mEnd, new_end, val);
        mEnd.mPtr = new_end.mPtr;
      }

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
    size_type n = std::distance(first, last);
    if (n > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(n);
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      pointer new_end = std::uninitialized_copy(first, last, new_begin);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      if (n < size())
      {
        const iterator new_end = std::copy(first, last, mBegin);
        flex::destruct_range(new_end, mEnd);
        mEnd.mPtr = new_end.mPtr;
      }
      else
      {
        InputIterator it(first);
        std::advance(it, size());
        std::copy(first, it, mBegin);
        mEnd.mPtr = std::uninitialized_copy(it, last, mEnd).mPtr;
      }
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::assign(std::initializer_list<value_type> il)
  {
    assign(il.begin(), il.end());
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::reference ring<T, Alloc>::at(size_type n)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(n >= size(), "flex::ring.at() - index out-of-bounds");
    return operator[](n);
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::const_reference ring<T, Alloc>::at(size_type n) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(n >= size(), "flex::ring.at() - index out-of-bounds");
    return operator[](n);
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
  inline typename ring<T, Alloc>::size_type ring<T, Alloc>::capacity() const
  {
    return (mBegin.mRightBound - mBegin.mLeftBound);
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::clear()
  {
    flex::destruct_range(mBegin, mEnd);
    mEnd.mPtr = mBegin.mPtr;
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  template<class... Args>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::emplace(iterator position, Args&&... args)
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Copy the inserted parameter val.
      iterator new_position(new_end, new_begin, new_begin + new_capacity);
      new ((void*) new_end) T(std::forward<Args>(args)...);

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), ++new_end);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
      return new_position;
    }
    else
    {
      if (position == prev_end)
      {
        //End of container case must be checked, as in this case position gets constructed.
        new ((void*) position.mPtr) value_type(std::forward<Args>(args)...);
      }
      else
      {
        //If we are inserting into the middle of the list, we are going to perform an assignment
        //instead of a construct, aka placement new.  Since we are doing an assignment, we are
        //forced to make a temporary object.  It makes the most sense to build the temporary object
        //now, as the arguments may contain references that may get invalidated by the copy_backward()
        //performed below.
        value_type tmp = value_type(std::forward<Args>(args)...);

        //This copy backwards will shift all the elements after position to the right
        //by one space.  This is valid since the capacity check above ensures we have
        //at least one spot available after the end.
        iterator back = prev_end;
        --back;
        new ((void*) prev_end.mPtr) T(FLEX_MOVE(*back));
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(back), prev_end);
        *position = std::move(tmp);
      }
      return position;
    }
  }

  template<class T, class Alloc>
  template<class... Args>
  inline void ring<T, Alloc>::emplace_back(Args&&... args)
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), new_begin);
      new ((void*) new_end) T(std::forward<Args>(args)...);
      ++new_end;

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      new ((void*) prev_end) T(std::forward<Args>(args)...);
    }
  }

  template<class T, class Alloc>
  template<class... Args>
  inline void ring<T, Alloc>::emplace_front(Args&&... args)
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      new ((void*) new_begin) T(std::forward<Args>(args)...);
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), (new_begin + 1));

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      new ((void*) mBegin.mPtr) T(std::forward<Args>(args)...);
    }
  }

#endif

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
    FLEX_COPY_OR_MOVE(position + 1, mEnd, position);
    (--mEnd)->~T();
    return position;
  }

  template<class T, class Alloc>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::erase(iterator first, iterator last)
  {
    if (first != last)
    {
      //Move all the elements after the erased range to the front of the range.  This
      //will overwrite the erased elements, and the size will be set accordingly.
      FLEX_COPY_OR_MOVE(last, mEnd, first);

      iterator new_end = mEnd - (last - first);
      flex::destruct_range(new_end, mEnd);
      mEnd.mPtr = new_end.mPtr;
    }
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Copy the inserted parameter val.
      iterator new_position(new_end, new_begin, new_begin + new_capacity);
      new ((void*) new_end) T(val);

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), ++new_end);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
      return new_position;
    }
    else
    {
      if (position == prev_end)
      {
        //End of container case must be checked, as in this case position gets constructed.
        new ((void*) position.mPtr) value_type(val);
      }
      else
      {
        const value_type* valPtr = &val;

        // Handle the case in which val is a reference within the container.
        if ((valPtr >= mBegin.mLeftBound) && (valPtr <= mBegin.mRightBound))
        {
          //Container looks like: [ *** End *** Begin *** Position ***]
          if (position.mPtr > mEnd.mPtr)
          {
            //If val is greater than position, or less than mEnd it must be incremented.
            if ((valPtr >= position.mPtr) || (valPtr < mEnd.mPtr))
            {
              if (valPtr == mBegin.mRightBound)
              {
                valPtr = mBegin.mLeftBound;
              }
              else
              {
                ++valPtr;
              }
            }
          }
          else
          {
            //Possible container formats : [ *** Begin *** Position *** End ***]
            //                             [ *** Position *** End *** Begin ***]

            //If val is between position and mEnd, then it must be incremented.
            if ((valPtr >= position.mPtr) && (valPtr < mEnd.mPtr))
              ++valPtr;
          }
        }

        //This copy backwards will shift all the elements after position to the right
        //by one space.  This is valid since the capacity check above ensures we have
        //at least one spot available after the end.
        iterator back = prev_end;
        --back;
        new ((void*) prev_end.mPtr) T(FLEX_MOVE(*back));
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(back), prev_end);
        *position = *valPtr;
      }
      return position;
    }
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline typename ring<T, Alloc>::iterator ring<T, Alloc>::insert(iterator position, value_type&& val)
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Copy the inserted parameter val.
      iterator new_position(new_end, new_begin, new_begin + new_capacity);
      new ((void*) new_end) T(FLEX_MOVE(val));

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), ++new_end);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
      return new_position;
    }
    else
    {
      if (position == prev_end)
      {
        //End of container case must be checked, as in this case position gets constructed.
        new ((void*) position.mPtr) value_type(FLEX_MOVE(val));
      }
      else
      {
        const value_type* valPtr = &val;

        // Handle the case in which val is a reference within the container.
        if ((valPtr >= mBegin.mLeftBound) && (valPtr <= mBegin.mRightBound))
        {
          //Container looks like: [ *** End *** Begin *** Position ***]
          if (position.mPtr > mEnd.mPtr)
          {
            //If val is greater than position, or less than mEnd it must be incremented.
            if ((valPtr >= position.mPtr) || (valPtr < mEnd.mPtr))
            {
              if (valPtr == mBegin.mRightBound)
              {
                valPtr = mBegin.mLeftBound;
              }
              else
              {
                ++valPtr;
              }
            }
          }
          else
          {
            //Possible container formats : [ *** Begin *** Position *** End ***]
            //                             [ *** Position *** End *** Begin ***]

            //If val is between position and mEnd, then it must be incremented.
            if ((valPtr >= position.mPtr) && (valPtr < mEnd.mPtr))
            ++valPtr;
          }
        }

        //This copy backwards will shift all the elements after position to the right
        //by one space.  This is valid since the capacity check above ensures we have
        //at least one spot available after the end.
        iterator back = prev_end;
        --back;
        new ((void*) prev_end.mPtr) T(FLEX_MOVE(*back));
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(back), prev_end);
        *position = FLEX_MOVE(*valPtr);
      }
      return position;
    }
  }
#endif

  template<class T, class Alloc>
  inline void ring<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
  {
    if ((size() + n) > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + n;
      size_type new_capacity = GetNewCapacity(new_size);
      pointer new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Fill the parameter val.
      std::uninitialized_fill_n(new_end, n, val);

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), new_end + n);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      const size_type rhs_n = static_cast<size_type>(mEnd - position);

      if (n < rhs_n)
      {
        std::uninitialized_copy(FLEX_MOVE_ITERATOR(mEnd - n), FLEX_MOVE_ITERATOR(mEnd), mEnd);
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd - n), mEnd);
        std::fill(position, position + n, val);
      }
      else
      {
        //Shift existing values to the right.
        std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), mEnd + n - rhs_n);

        //Fill in the values that go into uninitialized data.
        std::uninitialized_fill_n(mEnd, n - rhs_n, val);

        //Insert values in initialized data.
        std::fill(position, mEnd, val);
      }
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Copy the inserted range.
      new_end = std::uninitialized_copy(first, last, new_end);

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), new_end);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      const size_type rhs_n = static_cast<size_type>(mEnd - position);

      if (n < rhs_n)
      {
        std::uninitialized_copy(FLEX_MOVE_ITERATOR(mEnd - n), FLEX_MOVE_ITERATOR(mEnd), mEnd);
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd - n), mEnd);
        std::copy(first, last, position);
        mEnd += n;
      }
      else
      {
        InputIterator it = first;
        std::advance(it, rhs_n);
        std::uninitialized_copy(it, last, mEnd);
        mEnd.mPtr =
            std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), mEnd + n - rhs_n).mPtr;
        std::copy(first, it, position);
      }
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
  inline ring<T, Alloc>& ring<T, Alloc>::operator=(std::initializer_list<value_type> il)
  {
    assign(il.begin(), il.end());
    return *this;
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline ring<T, Alloc>& ring<T, Alloc>::operator=(ring<T, Alloc>&& obj)
  {
    swap(std::move(obj));
    return *this;
  }
#endif

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
    (--mEnd)->~T();
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::pop_front()
  {
    mBegin->~T();
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), new_begin);
      new ((void*) new_end) T(val);
      ++new_end;

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      new ((void*) prev_end) T(val);
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      new ((void*) new_begin) T(val);
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), (new_begin + 1));

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      new ((void*) mBegin.mPtr) T(val);
    }
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline void ring<T, Alloc>::push_back(value_type&& val)
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), new_begin);
      new ((void*) new_end) T(std::move(val));
      ++new_end;

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      new ((void*) prev_end) T(std::move(val));
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::push_front(value_type&& val)
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
      pointer new_begin = Allocate(new_capacity);

      //Copy all values.
      new ((void*) new_begin) T(std::move(val));
      pointer new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), (new_begin + 1));

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
    else
    {
      new ((void*) mBegin.mPtr) T(std::move(val));
    }
  }
#endif

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
      pointer new_begin = Allocate(new_capacity);

      //Copy all current values.
      pointer new_end = std::uninitialized_copy(mBegin, mEnd, new_begin);

      DeallocateAndReassign(new_begin, new_end, new_capacity);
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::resize(size_type n, const value_type& val)
  {
    if (n < size())
    {
      const iterator new_end = (mBegin + n);
      flex::destruct_range(new_end, mEnd);
      mEnd.mPtr = new_end.mPtr;
    }
    else if (n > size())
    {
      insert(mEnd, n - size(), val);
    }
  }

  template<class T, class Alloc>
  inline void ring<T, Alloc>::shrink_to_fit()
  {
    if (!mFixed)
    {
      size_type n = size();
      if (capacity() > n)
      {
        pointer new_begin, new_end;
        size_type new_capacity;
        if (n)
        {
          //Allocate memory with sufficient capacity.
          new_capacity = size();
          new_begin = Allocate(new_capacity);

          //Copy all values.
          new_end = std::uninitialized_copy(mBegin, mEnd, new_begin);
        }
        else
        {
          new_begin = new_end = NULL;
          new_capacity = 0;
        }
        DeallocateAndReassign(new_begin, new_end, new_capacity);
      }
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

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline void ring<T, Alloc>::swap(ring<T, Alloc>&& obj)
  {
    if ((!mFixed) && (!obj.fixed()))
    {
      std::swap(mBegin, obj.mBegin);
      std::swap(mEnd, obj.mEnd);
    }
    else
    {
      assign(std::make_move_iterator(obj.begin()), std::make_move_iterator(obj.end()));
      obj.clear();
    }
  }
#endif

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
