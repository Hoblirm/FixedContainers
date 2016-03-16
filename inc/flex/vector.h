#ifndef FLEX_VECTOR_H
#define FLEX_VECTOR_H

#include <flex/array.h>

namespace flex
{
  template<class T, class Alloc = allocator<T> >
  class vector: public array_base<T, Alloc>
  {
  public:
    typedef array_base<T, Alloc> base_type;
    
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
    
    typedef Alloc allocator_type;
    
    using base_type::mBegin;
    using base_type::mEnd;
    using base_type::mAllocator;
    using base_type::size;
    using base_type::AllocateAndConstruct;

    vector();
    explicit vector(size_type size, const value_type& val = value_type());
    vector(const T* first, const T* last);
    vector(const vector<T, Alloc> & obj);
    ~vector();

    void assign(size_type size, const T& val);
    //TODO: Get template to work with vector assign() to use multiple iterators.
    void assign(const T* first, const T* last);

    size_type capacity() const;
    void clear();
    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);
    bool fixed() const;
    allocator_type get_allocator() const;
    iterator insert(iterator position, const value_type& val);
    void insert(iterator position, size_type n, const value_type& val);
    //TODO: Get template to work with vector insert() to use multiple iterators.
    void insert(iterator position, const_iterator first, const_iterator last);
    size_type max_size() const;
    vector<T, Alloc>& operator=(const vector<T, Alloc>& obj);
    void pop_back();
    void push_back(const value_type& val);
    void resize(size_type n, const value_type& val = value_type());
    void swap(vector<T, Alloc>& obj);

  protected:
    pointer mCapacity;
    bool mFixed;

    vector(size_type capacity, pointer ptr);

  private:
    size_type GetNewCapacity(size_type min);
    void DestroyAndDeallocate();
  };

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector() :
    array_base<T, Alloc>(), mCapacity(NULL), mFixed(false)
  {
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(size_type size, const value_type& val) :
      array_base<T, Alloc>(size), mCapacity(mEnd), mFixed(false)
  {
    array_base<T, Alloc>::fill(val);
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(const T* first, const T* last) :
      array_base<T, Alloc>(last - first), mCapacity(mEnd), mFixed(false)
  {
    std::copy(first, last, mBegin);
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(const vector<T, Alloc> & obj) :
      array_base<T, Alloc>(obj.size()), mCapacity(mEnd), mFixed(false)
  {
    std::copy(obj.begin(), obj.end(), mBegin);
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::~vector()
  {
    if (!mFixed && (NULL != mBegin))
    {
      DestroyAndDeallocate();
    }
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::assign(size_type size, const T& val)
  {
    if (size > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - assign() fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_capacity = GetNewCapacity(size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values.
        std::fill_n(new_begin, size, val);

        DestroyAndDeallocate();

        mBegin = new_begin;
        mCapacity = mBegin + new_capacity;
      }
    }
    else
    {
      std::fill_n(mBegin, size, val);
    }
    mEnd = mBegin + size;
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::assign(const T* first, const T* last)
  {
    size_type size = last - first;
    if (size > capacity())
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - assign() iterator range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_capacity = GetNewCapacity(size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values.
        pointer new_end = std::copy(first, last, new_begin);

        DestroyAndDeallocate();

        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
      }
    }
    else
    {
      std::copy(first, last, mBegin);
      mEnd = mBegin + size;
    }
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::size_type vector<T, Alloc>::capacity() const
  {
    return mCapacity - mBegin;
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::clear()
  {
    mEnd = mBegin;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
  {
    //This copy will simply shift everything after position over to the left by one.
    //This will effectively overwrite position, erasing it from the container.
    std::copy(position + 1, mEnd, position);
    --mEnd;
    return position;
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
  {
    //Move all the elements after the erased range to the front of the range.  This
    //will overwrite the erased elements, and the size will be set accordingly.
    std::copy(last, mEnd, first);
    mEnd -= (last - first);
    return first;
  }

  template<class T, class Alloc>
  inline bool vector<T, Alloc>::fixed() const
  {
    return mFixed;
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
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - insert() called when size was at capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + 1;
        size_type new_capacity = GetNewCapacity(new_size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        T* new_end = std::copy(mBegin, position, new_begin);

        //Copy the inserted parameter val.
        T* new_position = new_end;
        *new_end = val;

        //Copy all values that come after position.
        new_end = std::copy(position, mEnd, ++new_end);

        DestroyAndDeallocate();

        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
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

  template<class T, class Alloc>
  inline void vector<T, Alloc>::insert(iterator position, size_type n, const value_type& val)
  {
    if ((mEnd + n) > mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector -: insert() fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + n;
        size_type new_capacity = GetNewCapacity(new_size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        T* new_end = std::copy(mBegin, position, new_begin);

        //Fill the parameter val.
        std::fill_n(new_end, n, val);

        //Copy all values that come after position.
        new_end = std::copy(position, mEnd, new_end + n);

        DestroyAndDeallocate();

        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
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

  template<class T, class Alloc>
  inline void vector<T, Alloc>::insert(iterator position, const_iterator first, const_iterator last)
  {
    size_type n = last - first;
    if ((mEnd + n) > mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - insert() fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + n;
        size_type new_capacity = GetNewCapacity(new_size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        T* new_end = std::copy(mBegin, position, new_begin);

        //Copy the inserted parameter val.
        new_end = std::copy(first, last, new_end);

        //Copy all values that come after position.
        new_end = std::copy(position, mEnd, new_end);

        //Deallocate and reassign
        DestroyAndDeallocate();
        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
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
  inline typename vector<T, Alloc>::size_type vector<T, Alloc>::max_size() const
  {
    return mAllocator.max_size();
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::pop_back()
  {
    --mEnd;
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::push_back(const value_type& val)
  {
    if (mEnd == mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - push_back() caused size to exceed capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_type new_size = size() + 1;
        size_type new_capacity = GetNewCapacity(new_size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values.
        T* new_end = std::copy(mBegin, mEnd, new_begin);
        *new_end = val;
        ++new_end;

        //Deallocate and reassign.
        DestroyAndDeallocate();
        mBegin = new_begin;
        mEnd = new_end;
        mCapacity = mBegin + new_capacity;
      }
    }
    else
    {
      *mEnd = val;
      ++mEnd;
    }
  }

  template<class T, class Alloc>
  inline void vector<T, Alloc>::resize(size_type n, const value_type& val)
  {
    if (n < size())
    {
      mEnd = mBegin + n;
    }
    else if (n > size())
    {
      insert(mEnd, n - size(), val);
    }
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
      if ((obj.size() > capacity()) || (size() > obj.capacity()))
      {
        throw std::runtime_error("flex::vector - swap() parameters' size exceed capacity");
      }

      if (size() < obj.size())
      {
        iterator it = std::swap_ranges(mBegin, mEnd, obj.mBegin);
        std::copy(it, obj.mEnd, mEnd);
      }
      else
      {
        iterator it = std::swap_ranges(obj.mBegin, obj.mEnd, mBegin);
        std::copy(it, mEnd, obj.mEnd);
      }
      size_type tmp_size = size();
      mEnd = mBegin + obj.size();
      obj.mEnd = obj.mBegin + tmp_size;
    }
  }

  template<class T, class Alloc>
  inline vector<T, Alloc>::vector(size_type capacity, pointer ptr) :
      array_base<T, Alloc>(ptr), mCapacity(ptr+capacity), mFixed(true)
  {
  }

  template<class T, class Alloc>
  inline typename vector<T, Alloc>::size_type vector<T, Alloc>::GetNewCapacity(size_type min_size)
  {
    // This needs to return a value of at least currentCapacity and at least 1.
    size_type new_capacity = (capacity() > 0) ? (2 * capacity()) : 1;

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
  inline void vector<T, Alloc>::DestroyAndDeallocate()
  {
    for (T* it = mBegin; it != mCapacity; ++it)
    {
      mAllocator.destroy(it);
    }
    mAllocator.deallocate(mBegin, capacity());
  }

}    //namespace flex

#endif /* FLEX_VECTOR_H */
