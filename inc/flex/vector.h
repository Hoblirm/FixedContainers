#ifndef FLEX_VECTOR_H
#define FLEX_VECTOR_H

#include <flex/allocator.h>

#include <algorithm>
#include <memory>
#include <iterator>

namespace flex
{

  template<class T, class Alloc = allocator<T> >
  class vector_base: public guarded_object
  {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef std::reverse_iterator<T*> reverse_iterator;
    typedef std::reverse_iterator<const T*> const_reverse_iterator;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef Alloc allocator_type;

  protected:
    iterator mBegin;
    iterator mEnd;
    pointer mCapacity;
    Alloc mAllocator;
    bool mFixed;

    vector_base();
    vector_base(size_type n);
    vector_base(pointer new_begin, pointer new_end, size_type capacity);
    ~vector_base();

    pointer Allocate(size_type n);
    void DestroyAndDeallocate();
    size_type GetNewCapacity(size_type min);
  };

  template<class T, class Alloc = allocator<T> >
  class vector: public vector_base<T, Alloc>
  {
    typedef vector_base<T, Alloc> base_type;
    typedef vector<T, Alloc> this_type;

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
    using base_type::mCapacity;
    using base_type::mFixed;
    using base_type::Allocate;
    using base_type::DestroyAndDeallocate;
    using base_type::GetNewCapacity;

    vector();
    explicit vector(size_type size, const value_type& val = value_type());
    vector(int size, const value_type& val);
    template<typename InputIterator> vector(InputIterator first, InputIterator last);
    vector(const vector<T, Alloc> & obj);
#ifdef FLEX_HAS_CXX11
    vector(vector<T, Alloc>&& x);
#endif
    vector(std::initializer_list<value_type> il);

    void assign(size_type size, const T& val);
    void assign(int size, const T& val);
    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last);
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
    template<class...Args> iterator emplace(iterator position, Args&&... val);
    template<class...Args> void emplace_back(Args&&... val);
#endif
    bool empty() const;
    iterator end();
    const_iterator end() const;
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    bool fixed() const;
    reference front();
    const_reference front() const;
    allocator_type get_allocator() const;
    iterator insert(iterator position, const value_type& val);
#ifdef FLEX_HAS_CXX11
    iterator insert(iterator position, value_type&& val);
#endif
    void insert(iterator position, size_type n, const value_type& val);
    void insert(iterator position, int n, const value_type& val);
    template<typename InputIterator> void insert(iterator position, InputIterator first, InputIterator last);
    size_type max_size() const;
    vector<T, Alloc>& operator=(const vector<T, Alloc>& obj);
#ifdef FLEX_HAS_CXX11
    vector<T, Alloc>& operator=(vector<T, Alloc>&& x);
#endif
    vector<T, Alloc>& operator=(std::initializer_list<value_type> il);
    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    void pop_back();
    void push_back(const value_type& val);
#ifdef FLEX_HAS_CXX11
    void push_back(value_type&& val);
#endif
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    void reserve(size_type n);
    void resize(size_type n, const value_type& val = value_type());
    void shrink_to_fit();
    size_type size() const;
    void swap(vector<T, Alloc>& obj);
#ifdef FLEX_HAS_CXX11
    void swap(vector<T, Alloc>&& obj);
#endif

  protected:
    vector(pointer new_begin, pointer new_end, size_type capacity);

  };

  /*
   * vector_base
   */
  template<class T, class Alloc>
  inline vector_base<T, Alloc>::vector_base() :
      mBegin(NULL), mEnd(NULL), mCapacity(NULL), mFixed(false)

  {
  }

  template<class T, class Alloc>
  inline vector_base<T, Alloc>::vector_base(size_type n) :
      mBegin(Allocate(n)), mEnd(mBegin + n), mCapacity(mEnd), mFixed(false)

  {
  }

  template<class T, class Alloc>
  inline vector_base<T, Alloc>::vector_base(pointer new_begin, pointer new_end, size_type capacity) :
      mBegin(new_begin), mEnd(new_end), mCapacity(mBegin + capacity), mFixed(true)

  {
#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(mEnd > mCapacity))
    {
      size_type n = new_end - new_begin;
      mFixed = false;
      mBegin = Allocate(n);
      mEnd = mCapacity = mBegin + n;
      flex::error_msg("flex::fixed_vector - constructor() size exceeds capacity");
    }
#endif
  }

  template<class T, class Alloc>
  inline vector_base<T, Alloc>::~vector_base()
  {
    flex::destruct_range(mBegin, mEnd);
    if (!mFixed)
    {
      mAllocator.deallocate(mBegin, mCapacity - mBegin);
    }
  }

  template<class T, class Alloc>
  inline typename vector_base<T, Alloc>::pointer vector_base<T, Alloc>::Allocate(size_type n)
  {
    return mAllocator.allocate(n);
  }

  template<class T, class Alloc>
  inline void vector_base<T, Alloc>::DestroyAndDeallocate()
  {
    flex::destruct_range(mBegin, mEnd);

#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(mFixed))
    {
      mFixed = false;
      flex::error_msg("flex::fixed_vector - capacity exceeded");
    }
    else
#endif
    mAllocator.deallocate(mBegin, mCapacity - mBegin);
  }

  template<class T, class Alloc>
  inline typename vector_base<T, Alloc>::size_type vector_base<T, Alloc>::GetNewCapacity(size_type min_size)
  {
    // This needs to return a value of at least currentCapacity and at least 1.
    size_type new_capacity = ((mCapacity - mBegin) > 0) ? (2 * (mCapacity - mBegin)) : 1;

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

  /*
   * vector
   */
  template<class T, class Alloc>
  inline vector<T, Alloc>::vector() :
      base_type()
  {
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(size_type size, const value_type& val) :
      base_type(size)
  {
    std::uninitialized_fill(mBegin, mEnd, val);
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(int size, const value_type& val) :
      base_type(size)
  {
    std::uninitialized_fill(mBegin, mEnd, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline vector<T, Alloc>::vector(InputIterator first, InputIterator last) :
      base_type(std::distance(first, last))
  {
    std::uninitialized_copy(first, last, mBegin);
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(const vector<T, Alloc> & obj) :
      base_type(obj.size())
  {
    std::uninitialized_copy(obj.mBegin, obj.mEnd, mBegin);
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(vector<T, Alloc> && obj) :
  base_type()
  {
    swap(std::move(obj));
  }
#endif

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(std::initializer_list<value_type> il) :
      base_type(il.size())
  {
    std::uninitialized_copy(il.begin(), il.end(), mBegin);
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::assign(size_type n, const T& val)
  {
    if (n > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(n);
      T* new_begin = Allocate(new_capacity);

      //Copy all values.
      std::uninitialized_fill_n(new_begin, n, val);

      DestroyAndDeallocate();

      mBegin = new_begin;
      mEnd = mBegin + n;
      mCapacity = mBegin + new_capacity;
    }
    else
    {
      if (n < size())
      {
        const iterator new_end = mBegin + n;
        std::fill_n(mBegin, n, val);
        flex::destruct_range(new_end, mEnd);
        mEnd = new_end;
      }
      else
      {
        const iterator new_end = mBegin + n;
        std::fill(mBegin, mEnd, val);
        std::uninitialized_fill(mEnd, new_end, val);
        mEnd = new_end;
      }
    }
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::assign(int size, const T& val)
  {
    assign((size_type) size, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline void vector<T, Alloc>::assign(InputIterator first, InputIterator last)
  {
    size_type n = std::distance(first, last);
    if (n > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(n);
      T* new_begin = Allocate(new_capacity);

      //Copy all values.
      pointer new_end = std::uninitialized_copy(first, last, new_begin);

      DestroyAndDeallocate();

      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
    }
    else
    {
      if (n < size())
      {
        const iterator new_end = std::copy(first, last, mBegin);
        flex::destruct_range(new_end, mEnd);
        mEnd = new_end;
      }
      else
      {
        InputIterator it(first);
        std::advance(it, size());
        std::copy(first, it, mBegin);
        mEnd = std::uninitialized_copy(it, last, mEnd);
      }
    }
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::assign(std::initializer_list<value_type> il)
  {
    assign(il.begin(), il.end());
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::reference vector<T, Alloc>::at(size_type n)
  {
    FLEX_THROW_OUT_OF_RANGE_IF(n >= size(), "flex::vector.at() - index out-of-bounds");
    return mBegin[n];
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reference vector<T, Alloc>::at(size_type n) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(n >= size(), "flex::vector.at() - index out-of-bounds");
    return mBegin[n];
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::reference vector<T, Alloc>::back()
  {
    return *(mEnd - 1);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reference vector<T, Alloc>::back() const
  {
    return *(mEnd - 1);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::begin()
  {
    return mBegin;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::begin() const
  {
    return mBegin;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::cbegin() const
  {
    return mBegin;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::cend() const
  {
    return mEnd;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::crbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::crend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::size_type vector<T, Alloc>::capacity() const
  {
    return mCapacity - mBegin;
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::clear()
  {
    flex::destruct_range(mBegin, mEnd);
    mEnd = mBegin;
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  template<class... Args>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::emplace(iterator position, Args&&... args)
  {
    if (mEnd == mCapacity)
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      T* new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Copy the inserted parameter val.
      T* new_position = new_end;
      new ((void*) new_end) value_type(std::forward<Args>(args)...);

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), ++new_end);

      DestroyAndDeallocate();

      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
      return new_position;
    }
    else
    {
      if (position == mEnd)
      {
        //End of container case must be checked, as in this case position gets constructed.
        ::new ((void*) position) value_type(std::forward<Args>(args)...);
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
        new ((void*) mEnd) value_type(FLEX_MOVE(*(mEnd - 1)));
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd - 1), mEnd);
        *position = std::move(tmp);
      }
      ++mEnd;
      return position;
    }
  }

  template<class T, class Alloc>
  template<class... Args>
  inline void vector<T, Alloc>::emplace_back(Args&&... args)
  {
    if (mEnd == mCapacity)
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      T* new_begin = Allocate(new_capacity);

      //Copy all values.
      T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), new_begin);
      new ((void*) new_end) value_type(std::forward<Args>(args)...);
      ++new_end;

      //Deallocate and reassign.
      DestroyAndDeallocate();
      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
    }
    else
    {
      new ((void*) mEnd) value_type(std::forward<Args>(args)...);
      ++mEnd;
    }
  }
#endif

  template<class T, class Alloc>
  inline bool vector<T, Alloc>::empty() const
  {
    return (mBegin == mEnd);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::end()
  {
    return mEnd;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::end() const
  {
    return mEnd;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
  {
    //This copy will simply shift everything after position over to the left by one.
    //This will effectively overwrite position, erasing it from the container.
    FLEX_COPY_OR_MOVE(position + 1, mEnd, position);
    (--mEnd)->~value_type();
    return position;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
  {
    if (first != last)
    {
      //Move all the elements after the erased range to the front of the range.  This
      //will overwrite the erased elements, and the size will be set accordingly.
      FLEX_COPY_OR_MOVE(last, mEnd, first);
      iterator new_end = mEnd - (last - first);
      flex::destruct_range(new_end, mEnd);
      mEnd = new_end;
    }
    return first;
  }

  template<class T, class Alloc>
  inline bool vector<T, Alloc>::fixed() const
  {
    return mFixed;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::reference vector<T, Alloc>::front()
  {
    return *mBegin;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reference vector<T, Alloc>::front() const
  {
    return *mBegin;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::allocator_type vector<T, Alloc>::get_allocator() const
  {
    return mAllocator;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const value_type& val)
  {
    if (mEnd == mCapacity)
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      T* new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Copy the inserted parameter val.
      T* new_position = new_end;
      new ((void*) new_end) value_type(val);

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), ++new_end);

      DestroyAndDeallocate();

      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
      return new_position;
    }
    else
    {
      if (position == mEnd)
      {
        //End of container case must be checked, as in this case position gets constructed.
        ::new ((void*) position) value_type(val);
      }
      else
      {
        // Handle the case in which val is a reference within the container.
        const_iterator it = &val;
        if ((it >= position) && (it < mEnd))
          ++it;

        //This copy backwards will shift all the elements after position to the right
        //by one space.  This is valid since the capacity check above ensures we have
        //at least one spot available after the end.
        new ((void*) mEnd) value_type(FLEX_MOVE(*(mEnd - 1)));
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd - 1), mEnd);
        *position = *it;
      }
      ++mEnd;
      return position;
    }
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, value_type&& val)
  {
    if (mEnd == mCapacity)
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      T* new_begin = Allocate(new_capacity);

      //Copy all values to the left of position.
      T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

      //Copy the inserted parameter val.
      T* new_position = new_end;
      new ((void*) new_end) value_type(FLEX_MOVE(val));

      //Copy all values that come after position.
      new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), ++new_end);

      DestroyAndDeallocate();

      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
      return new_position;
    }
    else
    {
      if (position == mEnd)
      {
        //End of container case must be checked, as in this case position gets constructed.
        ::new ((void*) position) value_type(FLEX_MOVE(val));
      }
      else
      {
        // Handle the case in which val is a reference within the container.
        const_iterator it = &val;
        if ((it >= position) && (it < mEnd))
        ++it;

        //This copy backwards will shift all the elements after position to the right
        //by one space.  This is valid since the capacity check above ensures we have
        //at least one spot available after the end.
        new ((void*) mEnd) value_type(FLEX_MOVE(*(mEnd - 1)));
        std::copy_backward(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd - 1), mEnd);
        *position = FLEX_MOVE(*it);
      }
      ++mEnd;
      return position;
    }
  }
#endif

  template<class T, class Alloc>
  inline void vector<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
  {

    if (n > 0)
    {
      if ((mEnd + n) > mCapacity)
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + n;
        size_type new_capacity = GetNewCapacity(new_size);
        T* new_begin = Allocate(new_capacity);
        //Copy all values to the left of position.
        T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

        //Fill the parameter val.
        std::uninitialized_fill_n(new_end, n, val);

        //Copy all values that come after position.
        new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), new_end + n);

        DestroyAndDeallocate();

        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
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

  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::insert(iterator position, int n, const value_type& val)
  {
    insert(position, (size_type) n, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline void vector<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
  {
    if (first != last)
    {
      size_type n = std::distance(first, last);
      if ((mEnd + n) > mCapacity)
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + n;
        size_type new_capacity = GetNewCapacity(new_size);
        T* new_begin = Allocate(new_capacity);

        //Copy all values to the left of position.
        T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(position), new_begin);

        //Copy the inserted parameter val.
        new_end = std::uninitialized_copy(first, last, new_end);

        //Copy all values that come after position.
        new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), new_end);

        //Deallocate and reassign
        DestroyAndDeallocate();
        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
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
          mEnd = std::uninitialized_copy(FLEX_MOVE_ITERATOR(position), FLEX_MOVE_ITERATOR(mEnd), mEnd + n - rhs_n);
          std::copy(first, it, position);
        }
      }
    }
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::size_type vector<T, Alloc>::max_size() const
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
  inline vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline vector<T, Alloc>& vector<T, Alloc>::operator=(vector<T, Alloc>&& obj)
  {
    swap(std::move(obj));
    return *this;
  }
#endif

  template<class T, class Alloc>
  inline vector<T, Alloc>& vector<T, Alloc>::operator=(std::initializer_list<value_type> il)
  {
    assign(il.begin(), il.end());
    return *this;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::reference vector<T, Alloc>::operator[](size_type n)
  {
    return mBegin[n];
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reference vector<T, Alloc>::operator[](size_type n) const
  {
    return mBegin[n];
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::pop_back()
  {
    (--mEnd)->~value_type();
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::push_back(const value_type& val)
  {
    if (mEnd == mCapacity)
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      T* new_begin = Allocate(new_capacity);

      //Copy all values.
      T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), new_begin);
      new ((void*) new_end) value_type(val);
      ++new_end;

      //Deallocate and reassign.
      DestroyAndDeallocate();
      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
    }
    else
    {
      new ((void*) mEnd) value_type(val);
      ++mEnd;
    }
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  inline void vector<T, Alloc>::push_back(value_type&& val)
  {
    if (mEnd == mCapacity)
    {
      //Allocate memory with sufficient capacity.
      size_type new_size = size() + 1;
      size_type new_capacity = GetNewCapacity(new_size);
      T* new_begin = Allocate(new_capacity);

      //Copy all values.
      T* new_end = std::uninitialized_copy(FLEX_MOVE_ITERATOR(mBegin), FLEX_MOVE_ITERATOR(mEnd), new_begin);
      new ((void*) new_end) value_type(std::move(val));
      ++new_end;

      //Deallocate and reassign.
      DestroyAndDeallocate();
      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
    }
    else
    {
      new ((void*) mEnd) value_type(std::move(val));
      ++mEnd;
    }
  }
#endif

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rbegin()
  {
    return reverse_iterator(mEnd);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::rbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::reverse_iterator vector<T, Alloc>::rend()
  {
    return reverse_iterator(mBegin);
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::const_reverse_iterator vector<T, Alloc>::rend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::reserve(size_type n)
  {
    if (n > capacity())
    {
      //Allocate memory with sufficient capacity.
      size_type new_capacity = GetNewCapacity(n);
      pointer new_begin = Allocate(new_capacity);

      //Copy all current values.
      pointer new_end = std::uninitialized_copy(mBegin, mEnd, new_begin);

      //Deallocate and reassign.
      DestroyAndDeallocate();
      mBegin = new_begin;
      mEnd = new_end;
      mCapacity = mBegin + new_capacity;
    }
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::resize(size_type n, const value_type& val)
  {
    if (n < size())
    {
      size_type diff = size() - n;
      iterator new_end = mBegin + n;
      flex::destruct_range(new_end, mEnd);
      mEnd = new_end;
    }
    else if (n > size())
    {
      insert(mEnd, n - size(), val);
    }
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::shrink_to_fit()
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
        DestroyAndDeallocate();
        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
      }
    }
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::size_type vector<T, Alloc>::size() const
  {
    return mEnd - mBegin;
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::swap(vector<T, Alloc>& obj)
  {
    if ((!mFixed) && (!obj.fixed()))
    {
      std::swap(mBegin, obj.mBegin);
      std::swap(mEnd, obj.mEnd);
      std::swap(mCapacity, obj.mCapacity);
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
  inline void vector<T, Alloc>::swap(vector<T, Alloc>&& obj)
  {
    if ((!mFixed) && (!obj.fixed()))
    {
      std::swap(mBegin, obj.mBegin);
      std::swap(mEnd, obj.mEnd);
      std::swap(mCapacity, obj.mCapacity);
    }
    else
    {
      assign(std::make_move_iterator(obj.begin()), std::make_move_iterator(obj.end()));
      obj.clear();
    }
  }
#endif

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(pointer new_begin, pointer new_end, size_type capacity) :
      base_type(new_begin, new_end, capacity)
  {
  }

  template<class T, class Alloc>
  inline bool operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
  {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

  template<class T, class Alloc>
  inline bool operator<(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
  {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  template<class T, class Alloc>
  inline bool operator!=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
  {
    return !(lhs == rhs);
  }

  template<class T, class Alloc>
  inline bool operator>(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
  {
    return rhs < lhs;
  }

  template<class T, class Alloc>
  inline bool operator<=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
  {
    return !(rhs < lhs);
  }

  template<class T, class Alloc>
  inline bool operator>=(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs)
  {
    return !(lhs < rhs);
  }

} //namespace flex

#endif /* FLEX_VECTOR_H */
