#ifndef FIXED_VECTOR_BASE_H
#define FIXED_VECTOR_BASE_H

#include <fixed_array.h>

template<class T, class Alloc = flex::allocator<T> >
class fixed_vector_base: public fixed_array_base<T, Alloc>
{
public:

  using fixed_array_base<T, Alloc>::mAryPtr;
  using fixed_array_base<T, Alloc>::mSize;
  using fixed_array_base<T, Alloc>::begin;
  using fixed_array_base<T, Alloc>::end;

  void assign(size_t size, const T& val);
  //TODO: Get template to work with fixed_vector assign() to use multiple iterators.
  void assign(const T* first, const T* last);

  size_t capacity() const;
  void clear();
  bool empty();
  T* erase(T* position);
  T* erase(T* first, T* last);
  bool fixed();
  T* insert(T* position, const T& val);
  void insert(T* position, size_t n, const T& val);
  //TODO: Get template to work with fixed_vector insert() to use multiple iterators.
  void insert(T* position, const T* first, const T* last);
  size_t max_size() const;
  fixed_vector_base<T, Alloc>& operator=(const fixed_vector_base<T, Alloc>& obj);
  void pop_back();
  void push_back(const T& val);
  void resize(size_t n, const T& val = T());
  void swap(fixed_vector_base<T, Alloc>& obj);

protected:
  Alloc mAllocator;
  size_t mCapacity;
  bool mFixed;

  fixed_vector_base(size_t capacity) :
      fixed_array_base<T, Alloc>(0), mCapacity(capacity), mFixed(false)
  {
    this->mAryPtr = NULL;
  }

  fixed_vector_base(size_t capacity, size_t size) :
      fixed_array_base<T, Alloc>(size), mCapacity(capacity), mFixed(false)
  {
    if (size > capacity)
    {
      throw std::runtime_error("fixed_vector: constructor's parameter size exceeds capacity");
    }
  }

  fixed_vector_base(size_t capacity, T* ptr) :
      fixed_array_base<T, Alloc>(0, ptr), mCapacity(capacity), mFixed(true)
  {
  }

  fixed_vector_base(size_t capacity, size_t size, T* ptr) :
      fixed_array_base<T, Alloc>(size, ptr), mCapacity(capacity), mFixed(true)
  {
    if (size > capacity)
    {
      throw std::runtime_error("fixed_vector: constructor's parameter size exceeds capacity");
    }
  }

private:
  void set_size(size_t size);
  size_t GetNewCapacity(size_t min);
  T* DoAllocateAndConstruct(size_t size);
  void DoDestroyAndDeallocate();
};

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::assign(size_t size, const T& val)
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
      T* new_begin = DoAllocateAndConstruct(new_capacity);

      //Copy all values.
      std::fill_n(new_begin, size, val);

      DoDestroyAndDeallocate();

      this->mAryPtr = new_begin;
      mCapacity = new_capacity;
    }
  }
  else
  {
    std::fill_n(begin(), size, val);
  }
  this->mSize = size;
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::assign(const T* first, const T* last)
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
      T* new_begin = DoAllocateAndConstruct(new_capacity);

      //Copy all values.
      std::copy(first, last, new_begin);

      DoDestroyAndDeallocate();

      this->mAryPtr = new_begin;
      mCapacity = new_capacity;
    }
  }
  else
  {
    std::copy(first, last, this->mAryPtr);
  }
  this->mSize = size;
}

template<class T, class Alloc> size_t fixed_vector_base<T, Alloc>::capacity() const
{
  return mCapacity;
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::clear()
{
  this->mSize = 0;
}

template<class T, class Alloc> bool fixed_vector_base<T, Alloc>::empty()
{
  return (this->mSize == 0);
}

template<class T, class Alloc> T* fixed_vector_base<T, Alloc>::erase(T* position)
{
  for (T* it = position; it != (fixed_array_base<T, Alloc>::end() - 1); ++it)
  {
    *it = *(it + 1);
  }
  --this->mSize;
  return position;
}

template<class T, class Alloc> T* fixed_vector_base<T, Alloc>::erase(T* first, T* last)
{
  T* leftIt = first;
  for (T* rightIt = last; rightIt != fixed_array_base<T, Alloc>::end(); ++rightIt)
  {
    *leftIt = *rightIt;
    ++leftIt;
  }
  this->mSize -= (last - first);
  return first;
}

template<class T, class Alloc> bool fixed_vector_base<T, Alloc>::fixed()
{
  return mFixed;
}

template<class T, class Alloc> T* fixed_vector_base<T, Alloc>::insert(T* position, const T& val)
{
  if (this->mSize >= mCapacity)
  {
    if (mFixed)
    {
      throw std::runtime_error("flex::vector - insert() called when size was at capacity");
    }
    else
    {
      //Allocate memory with sufficient capacity.
      size_t new_capacity = GetNewCapacity(this->mSize + 1);
      T* new_begin = DoAllocateAndConstruct(new_capacity);

      //Copy all values to the left of position.
      T* new_end = std::copy(this->mAryPtr, position, new_begin);

      //Copy the inserted parameter val.
      T* new_position = new_end;
      *new_end = val;

      //Copy all values that come after position.
      new_end = std::copy(position, end(), ++new_end);

      DoDestroyAndDeallocate();

      this->mAryPtr = new_begin;
      ++this->mSize;
      mCapacity = new_capacity;
      return new_position;
    }
  }
  else
  {
    //TODO: Put in copy_backward
    //std::copy_backward(position, end(),end()+1);
    for (T* it = fixed_array_base<T, Alloc>::end(); it != position; --it)
    {
      *it = *(it - 1);
    }

    *position = val;
    ++this->mSize;

    return position;
  }
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::insert(T* position, size_t n, const T& val)
{
  if ((this->mSize + n) > mCapacity)
  {
    if (mFixed)
    {
      throw std::runtime_error("flex::vector -: insert() fill range exceeds capacity");
    }
    else
    {
      //Allocate memory with sufficient capacity.
      size_t new_capacity = GetNewCapacity(this->mSize + n);
      T* new_begin = DoAllocateAndConstruct(new_capacity);

      //Copy all values to the left of position.
      T* new_end = std::copy(this->mAryPtr, position, new_begin);

      //Fill the parameter val.
      std::fill_n(new_end, n, val);

      //Copy all values that come after position.
      new_end = std::copy(position, end(), new_end + n);

      DoDestroyAndDeallocate();

      this->mAryPtr = new_begin;
      mCapacity = new_capacity;
    }
  }
  else
  {
    //Slide everything to the right 'n' spaces to make room for the new elements.
    for (T* it = (fixed_array_base<T, Alloc>::end() + (n - 1)); it != (position + (n - 1)); --it)
    {
      *it = *(it - n);
    }

    //Insert the new elements into the remaining space.
    for (T* it = position; it != (position + n); ++it)
    {
      *it = val;
    }
  }
  this->mSize += n;
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::insert(T* position, const T* first, const T* last)
{
  size_t n = last - first;
  if ((this->mSize + n) > mCapacity)
  {
    if (mFixed)
    {
      throw std::runtime_error("flex::vector - insert() fill range exceeds capacity");
    }
    else
    {
      //Allocate memory with sufficient capacity.
      size_t new_capacity = GetNewCapacity(this->mSize + n);
      T* new_begin = DoAllocateAndConstruct(new_capacity);

      //Copy all values to the left of position.
      T* new_end = std::copy(this->mAryPtr, position, new_begin);

      //Copy the inserted parameter val.
      new_end = std::copy(first, last, new_end);

      //Copy all values that come after position.
      new_end = std::copy(position, end(), new_end);

      DoDestroyAndDeallocate();

      this->mAryPtr = new_begin;
      mCapacity = new_capacity;
    }
  }

  //TODO: replace with copy_backward
//Slide everything to the right 'n' spaces to make room for the new elements.
  for (T* it = (fixed_array_base<T, Alloc>::end() + (n - 1)); it != (position + (n - 1)); --it)
  {
    *it = *(it - n);
  }

//Insert the new elements into the remaining space.
  for (T* it = position; it != (position + n); ++it)
  {
    *it = *(first++);
  }

  this->mSize += n;
}

template<class T, class Alloc> size_t fixed_vector_base<T, Alloc>::max_size() const
{
  return mCapacity;
}

template<class T, class Alloc> fixed_vector_base<T, Alloc>& fixed_vector_base<T, Alloc>::operator=(
    const fixed_vector_base<T, Alloc>& obj)
{
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
      T* new_begin = DoAllocateAndConstruct(new_capacity);

      //Copy all values.
      std::copy(obj.begin(), obj.end(), new_begin);

      DoDestroyAndDeallocate();

      this->mAryPtr = new_begin;
      mCapacity = new_capacity;
    }
  }
  else
  {
    std::copy(obj.begin(), obj.end(), this->mAryPtr);
  }
  this->mSize = obj.size();
  return *this;
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::pop_back()
{
  --this->mSize;
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::push_back(const T& val)
{
  if (this->mSize >= mCapacity)
  {
    if (mFixed)
    {
      throw std::runtime_error("flex::vector - push_back() caused size to exceed capacity");
    }
    else
    {
      //Allocate memory with sufficient capacity.
      size_t new_capacity = GetNewCapacity(this->mSize + 1);
      T* new_begin = DoAllocateAndConstruct(new_capacity);

      //Copy all values.
      T* new_end = std::copy(begin(), end(), new_begin);
      *new_end = val;

      DoDestroyAndDeallocate();

      this->mAryPtr = new_begin;
      mCapacity = new_capacity;
    }
  }
  else
  {
    this->mAryPtr[this->mSize] = val;
  }
  ++this->mSize;
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::resize(size_t n, const T& val)
{
  if (n < this->mSize)
  {
    this->mSize = n;
  }
  else if (n > this->mSize)
  {
    insert(fixed_array_base<T, Alloc>::end(), n - this->mSize, val);
  }
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::swap(fixed_vector_base<T, Alloc>& obj)
{
  if ((!mFixed) && (!obj.fixed()))
  {
    typename fixed_array_base<T, Alloc>::iterator tmp_begin = this->mAryPtr;
    size_t tmp_size = this->mSize;
    size_t tmp_capacity = mCapacity;

    this->mAryPtr = obj.mAryPtr;
    this->mSize = obj.mSize;
    mCapacity = obj.mCapacity;

    obj.mAryPtr = tmp_begin;
    obj.mSize = tmp_size;
    obj.mCapacity = tmp_capacity;
  }
  else
  {
    if ((obj.size() > mCapacity) || (this->mSize > obj.capacity()))
    {
      throw std::runtime_error("flex::vector - swap() parameters' size exceed capacity");
    }

    if (this->mSize < obj.size())
    {
      T tmp;
      for (int i = 0; i < this->mSize; i++)
      {
        tmp = this->mAryPtr[i];
        this->mAryPtr[i] = obj[i];
        obj[i] = tmp;
      }

      for (int i = this->mSize; i < obj.size(); i++)
      {
        this->mAryPtr[i] = obj[i];
      }
    }
    else
    {
      T tmp;
      for (int i = 0; i < obj.size(); i++)
      {
        tmp = this->mAryPtr[i];
        this->mAryPtr[i] = obj[i];
        obj[i] = tmp;
      }

      for (int i = obj.size(); i < this->mSize; i++)
      {
        obj[i] = this->mAryPtr[i];
      }
    }

    size_t tmp_size = this->mSize;
    this->mSize = obj.size();
    obj.set_size(tmp_size);
  }
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::set_size(size_t size)
{
  this->mSize = size;
}

template<class T, class Alloc> size_t fixed_vector_base<T, Alloc>::GetNewCapacity(size_t min_size)
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

template<class T, class Alloc> T* fixed_vector_base<T, Alloc>::DoAllocateAndConstruct(size_t size)
{
  typename fixed_array_base<T, Alloc>::iterator new_begin = mAllocator.allocate(size);
  typename fixed_array_base<T, Alloc>::iterator first = new_begin;
  typename fixed_array_base<T, Alloc>::iterator last = first + size;
  for (; first != last; ++first)
  {
    mAllocator.construct(first, typename fixed_array_base<T, Alloc>::value_type());
  }
  return new_begin;
}

template<class T, class Alloc> void fixed_vector_base<T, Alloc>::DoDestroyAndDeallocate()
{
  for (T* it = begin(); it != end(); ++it)
  {
    mAllocator.destroy(it);
  }
  mAllocator.deallocate(this->mAryPtr, mCapacity);
}

#endif /* FIXED_VECTOR_BASE_H */
