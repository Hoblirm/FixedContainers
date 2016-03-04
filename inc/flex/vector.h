#ifndef FLEX_VECTOR_H
#define FLEX_VECTOR_H

#include <flex/array.h>

namespace flex
{
  template<class T, class Alloc = allocator<T> >
  class vector: public array_base<T>
  {
  public:

    typedef array_base<T> base_type;
    typedef typename base_type::iterator iterator;
    typedef Alloc allocator_type;

    using array_base<T>::mAryPtr;
    using array_base<T>::mSize;

    vector();
    explicit vector(size_t size, const T& val = T());
    vector(const T* first, const T* last);
    vector(const vector<T, Alloc> & obj);
    ~vector();

    void assign(size_t size, const T& val);
    //TODO: Get template to work with vector assign() to use multiple iterators.
    void assign(const T* first, const T* last);

    size_t capacity() const;
    void clear();
    bool empty();
    T* erase(T* position);
    T* erase(T* first, T* last);
    bool fixed() const;
    allocator_type get_allocator() const;
    T* insert(T* position, const T& val);
    void insert(T* position, size_t n, const T& val);
    //TODO: Get template to work with vector insert() to use multiple iterators.
    void insert(T* position, const T* first, const T* last);
    size_t max_size() const;
    vector<T, Alloc>& operator=(const vector<T, Alloc>& obj);
    void pop_back();
    void push_back(const T& val);
    void resize(size_t n, const T& val = T());
    void swap(vector<T, Alloc>& obj);

  protected:
    Alloc mAllocator;
    size_t mCapacity;
    bool mFixed;

    vector(size_t capacity, T* ptr);
    vector(size_t capacity, size_t size, T* ptr);

  private:
    size_t GetNewCapacity(size_t min);
    T* AllocateAndConstruct(size_t capacity);
    void DestroyAndDeallocate();
  };

  template<class T, class Alloc> vector<T, Alloc>::vector() :
      array_base<T>(0), mCapacity(0), mFixed(false)
  {
  }

  template<class T, class Alloc> vector<T, Alloc>::vector(size_t size, const T& val) :
      array_base<T>(size), mCapacity(size), mFixed(false)
  {
    mAryPtr = AllocateAndConstruct(mCapacity);
    array_base<T>::fill(val);
  }

  template<class T, class Alloc> vector<T, Alloc>::vector(const T* first, const T* last) :
      array_base<T>(last - first), mCapacity(last - first), mFixed(false)
  {
    mAryPtr = AllocateAndConstruct(mCapacity);
    std::copy(first, last, mAryPtr);
  }

  template<class T, class Alloc> vector<T, Alloc>::vector(const vector<T, Alloc> & obj) :
      array_base<T>(obj.size()), mCapacity(obj.capacity()), mFixed(false)
  {
    mAryPtr = AllocateAndConstruct(mCapacity);
    std::copy(obj.begin(), obj.end(), mAryPtr);
  }

  template<class T, class Alloc> vector<T, Alloc>::~vector()
  {
    if (!mFixed && (NULL != mAryPtr))
    {
      DestroyAndDeallocate();
    }
  }

  template<class T, class Alloc> void vector<T, Alloc>::assign(size_t size, const T& val)
  {
    if (size > mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - assign() fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_t new_capacity = GetNewCapacity(size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values.
        std::fill_n(new_begin, size, val);

        DestroyAndDeallocate();

        mAryPtr = new_begin;
        mCapacity = new_capacity;
      }
    }
    else
    {
      std::fill_n(mAryPtr, size, val);
    }
    mSize = size;
  }

  template<class T, class Alloc> void vector<T, Alloc>::assign(const T* first, const T* last)
  {
    size_t size = last - first;
    if (size > mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - assign() iterator range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_t new_capacity = GetNewCapacity(size);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values.
        std::copy(first, last, new_begin);

        DestroyAndDeallocate();

        mAryPtr = new_begin;
        mCapacity = new_capacity;
      }
    }
    else
    {
      std::copy(first, last, mAryPtr);
    }
    mSize = size;
  }

  template<class T, class Alloc> size_t vector<T, Alloc>::capacity() const
  {
    return mCapacity;
  }

  template<class T, class Alloc> void vector<T, Alloc>::clear()
  {
    mSize = 0;
  }

  template<class T, class Alloc> bool vector<T, Alloc>::empty()
  {
    return (mSize == 0);
  }

  template<class T, class Alloc> T* vector<T, Alloc>::erase(T* position)
  {
    //This copy will simply shift everything after position over to the left by one.
    //This will effectively overwrite position, erasing it from the container.
    std::copy(position + 1, (mAryPtr + mSize), position);
    --mSize;
    return position;
  }

  template<class T, class Alloc> T* vector<T, Alloc>::erase(T* first, T* last)
  {
    //Move all the elements after the erased range to the front of the range.  This
    //will overwrite the erased elements, and the size will be set accordingly.
    std::copy(last, (mAryPtr + mSize), first);
    mSize -= (last - first);
    return first;
  }

  template<class T, class Alloc> bool vector<T, Alloc>::fixed() const
  {
    return mFixed;
  }

  template<class T, class Alloc> typename vector<T, Alloc>::allocator_type vector<T, Alloc>::get_allocator() const
  {
    return mAllocator;
  }

  template<class T, class Alloc> T* vector<T, Alloc>::insert(T* position, const T& val)
  {
    if (mSize >= mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - insert() called when size was at capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_t new_capacity = GetNewCapacity(mSize + 1);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        T* new_end = std::copy(mAryPtr, position, new_begin);

        //Copy the inserted parameter val.
        T* new_position = new_end;
        *new_end = val;

        //Copy all values that come after position.
        new_end = std::copy(position, (mAryPtr + mSize), ++new_end);

        DestroyAndDeallocate();

        mAryPtr = new_begin;
        ++mSize;
        mCapacity = new_capacity;
        return new_position;
      }
    }
    else
    {
      //This copy backwards will shift all the elements after position to the right
      //by one space.  This is valid since the capacity check above ensures we have
      //at least one spot available after the end.
      std::copy_backward(position, (mAryPtr + mSize), (mAryPtr + mSize) + 1);

      *position = val;
      ++mSize;

      return position;
    }
  }

  template<class T, class Alloc> void vector<T, Alloc>::insert(T* position, size_t n, const T& val)
  {
    if ((mSize + n) > mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector -: insert() fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_t new_capacity = GetNewCapacity(mSize + n);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        T* new_end = std::copy(mAryPtr, position, new_begin);

        //Fill the parameter val.
        std::fill_n(new_end, n, val);

        //Copy all values that come after position.
        new_end = std::copy(position, (mAryPtr + mSize), new_end + n);

        DestroyAndDeallocate();

        mAryPtr = new_begin;
        mCapacity = new_capacity;
      }
    }
    else
    {
      //Slide everything to the right 'n' spaces to make room for the new elements.
      std::copy_backward(position, (mAryPtr + mSize), (mAryPtr + mSize) + n);

      //Insert the new elements into the available space.
      std::fill_n(position, n, val);
    }
    mSize += n;
  }

  template<class T, class Alloc> void vector<T, Alloc>::insert(T* position, const T* first, const T* last)
  {
    size_t n = last - first;
    if ((mSize + n) > mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - insert() fill range exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_t new_capacity = GetNewCapacity(mSize + n);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values to the left of position.
        T* new_end = std::copy(mAryPtr, position, new_begin);

        //Copy the inserted parameter val.
        new_end = std::copy(first, last, new_end);

        //Copy all values that come after position.
        new_end = std::copy(position, (mAryPtr + mSize), new_end);

        //Deallocate and reassign
        DestroyAndDeallocate();
        mAryPtr = new_begin;
        mCapacity = new_capacity;
      }
    }
    else
    {
      //Slide everything to the right 'n' spaces to make room for the new elements.
      std::copy_backward(position, (mAryPtr + mSize), (mAryPtr + mSize) + n);

      //Insert the new elements into the available space.
      std::copy(first, last, position);
    }
    mSize += n;
  }

  template<class T, class Alloc> size_t vector<T, Alloc>::max_size() const
  {
    return mAllocator.max_size();
  }

  template<class T, class Alloc> vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& obj)
  {
    //TODO: Consider making method a call to assign().
    if (obj.size() > mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - assignment operator's parameter size exceeds capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_t new_capacity = GetNewCapacity(obj.size());
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values.
        std::copy(obj.begin(), obj.end(), new_begin);

        //Deallocate and reassign.
        DestroyAndDeallocate();
        mAryPtr = new_begin;
        mCapacity = new_capacity;
      }
    }
    else
    {
      std::copy(obj.begin(), obj.end(), mAryPtr);
    }
    mSize = obj.size();
    return *this;
  }

  template<class T, class Alloc> void vector<T, Alloc>::pop_back()
  {
    --mSize;
  }

  template<class T, class Alloc> void vector<T, Alloc>::push_back(const T& val)
  {
    if (mSize >= mCapacity)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::vector - push_back() caused size to exceed capacity");
      }
      else
      {
        //Allocate memory with sufficient capacity.
        size_t new_capacity = GetNewCapacity(mSize + 1);
        T* new_begin = AllocateAndConstruct(new_capacity);

        //Copy all values.
        T* new_end = std::copy(mAryPtr, mAryPtr + mSize, new_begin);
        *new_end = val;

        //Deallocate and reassign.
        DestroyAndDeallocate();
        mAryPtr = new_begin;
        mCapacity = new_capacity;
      }
    }
    else
    {
      mAryPtr[mSize] = val;
    }
    ++mSize;
  }

  template<class T, class Alloc> void vector<T, Alloc>::resize(size_t n, const T& val)
  {
    if (n < mSize)
    {
      mSize = n;
    }
    else if (n > mSize)
    {
      insert(array_base<T>::end(), n - mSize, val);
    }
  }

  template<class T, class Alloc> void vector<T, Alloc>::swap(vector<T, Alloc>& obj)
  {
    if ((!mFixed) && (!obj.fixed()))
    {
      std::swap(mAryPtr, obj.mAryPtr);
      std::swap(mSize, obj.mSize);
      std::swap(mCapacity, obj.mCapacity);
    }
    else
    {
      if ((obj.size() > mCapacity) || (mSize > obj.capacity()))
      {
        throw std::runtime_error("flex::vector - swap() parameters' size exceed capacity");
      }

      if (mSize < obj.size())
      {
        typename array_base<T>::iterator it = std::swap_ranges(mAryPtr, (mAryPtr + mSize), obj.begin());
        std::copy(it, obj.end(), mAryPtr + mSize);
      }
      else
      {
        typename array_base<T>::iterator it = std::swap_ranges(obj.begin(), obj.end(), mAryPtr);
        std::copy(it, (mAryPtr + mSize), obj.end());
      }
      std::swap(mSize, obj.mSize);
    }
  }

  template<class T, class Alloc> vector<T, Alloc>::vector(size_t capacity, T* ptr) :
      array_base<T>(0, ptr), mCapacity(capacity), mFixed(true)
  {
  }

  template<class T, class Alloc> vector<T, Alloc>::vector(size_t capacity, size_t size, T* ptr) :
      array_base<T>(size, ptr), mCapacity(capacity), mFixed(true)
  {
    if (size > capacity)
    {
      throw std::runtime_error("vector: constructor's parameter size exceeds capacity");
    }
  }

  template<class T, class Alloc> size_t vector<T, Alloc>::GetNewCapacity(size_t min_size)
  {
    // This needs to return a value of at least currentCapacity and at least 1.
    size_t new_capacity = (mCapacity > 0) ? (2 * mCapacity) : 1;

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

  template<class T, class Alloc> T* vector<T, Alloc>::AllocateAndConstruct(size_t capacity)
  {
    typename array_base<T>::iterator new_begin = mAllocator.allocate(capacity);
    for (T* it = new_begin; it != (new_begin + capacity); ++it)
    {
      mAllocator.construct(it, typename array_base<T>::value_type());
    }
    return new_begin;
  }

  template<class T, class Alloc> void vector<T, Alloc>::DestroyAndDeallocate()
  {
    for (T* it = mAryPtr; it != (mAryPtr + mCapacity); ++it)
    {
      mAllocator.destroy(it);
    }
    mAllocator.deallocate(mAryPtr, mCapacity);
  }

}    //namespace flex

#endif /* FLEX_VECTOR_H */
