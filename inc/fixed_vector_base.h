#ifndef FIXED_VECTOR_BASE_H
#define FIXED_VECTOR_BASE_H

#include <fixed_array.h>

template<class T, class Alloc = flex::allocator<T> >
class fixed_vector_base: public fixed_array_base<T,Alloc>
{
public:

  void assign(size_t size, const T& val);
  //TODO: Get template to work with fixed_vector assign() to use multiple iterators.
  void assign(const T* first, const T* last);

  size_t capacity() const;
  void clear();
  bool empty();
  T* erase(T* position);
  T* erase(T* first, T* last);
  T* insert(T* position, const T& val);
  void insert(T* position, size_t n, const T& val);
  //TODO: Get template to work with fixed_vector insert() to use multiple iterators.
  void insert(T* position, const T* first, const T* last);
  size_t max_size() const;
  fixed_vector_base<T,Alloc>& operator=(const fixed_vector_base<T,Alloc>& obj);
  void pop_back();
  void push_back(const T& val);
  void resize(size_t n, const T& val = T());
  void swap(fixed_vector_base<T,Alloc>& obj);

protected:
  size_t mCapacity;

  fixed_vector_base(size_t capacity) :
     fixed_array_base<T,Alloc>(0), mCapacity(capacity)
  {
  }

  fixed_vector_base(size_t capacity, T* ptr) :
      fixed_array_base<T,Alloc>(0, ptr), mCapacity(capacity)
  {
  }

  fixed_vector_base(size_t capacity, size_t size) :
      fixed_array_base<T,Alloc>(size), mCapacity(capacity)
  {
    if (size > capacity)
    {
      throw std::runtime_error("fixed_vector: constructor's parameter size exceeds capacity");
    }
  }

  fixed_vector_base(size_t capacity, size_t size, T* ptr) :
      fixed_array_base<T,Alloc>(size, ptr), mCapacity(capacity)
  {
    if (size > capacity)
    {
      throw std::runtime_error("fixed_vector: constructor's parameter size exceeds capacity");
    }
  }

private:
  void set_size(size_t size);
};

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::assign(size_t size, const T& val)
{
  if (size > mCapacity)
  {
    throw std::runtime_error("fixed_vector: assign() fill range exceeds capacity");
  }
  this->mSize = size;
  fixed_array_base<T,Alloc>::fill(val);
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::assign(const T* first, const T* last)
{
  this->mSize = 0;
  for (const T* it = first; it != last; ++it)
  {
    if (this->mSize >= mCapacity)
    {
      throw std::runtime_error("fixed_vector: assign() iterator range exceeds capacity");
    }
    this->mAryPtr[this->mSize] = *it;
    ++this->mSize;
  }
}

template<class T, class Alloc> size_t fixed_vector_base<T,Alloc>::capacity() const
{
  return mCapacity;
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::clear()
{
  this->mSize = 0;
}

template<class T, class Alloc> bool fixed_vector_base<T,Alloc>::empty()
{
  return (this->mSize == 0);
}

template<class T, class Alloc> T* fixed_vector_base<T,Alloc>::erase(T* position)
{
  for (T* it = position; it != (fixed_array_base<T,Alloc>::end() - 1); ++it)
  {
    *it = *(it + 1);
  }
  --this->mSize;
  return position;
}

template<class T, class Alloc> T* fixed_vector_base<T,Alloc>::erase(T* first, T* last)
{
  T* leftIt = first;
  for (T* rightIt = last; rightIt != fixed_array_base<T,Alloc>::end(); ++rightIt)
  {
    *leftIt = *rightIt;
    ++leftIt;
  }
  this->mSize -= (last - first);
  return first;
}

template<class T, class Alloc> T* fixed_vector_base<T,Alloc>::insert(T* position, const T& val)
{
  if (this->mSize >= mCapacity)
  {
    throw std::runtime_error("fixed_vector: insert() called when size was at capacity");
  }

  for (T* it = fixed_array_base<T,Alloc>::end(); it != position; --it)
  {
    *it = *(it - 1);
  }

  *position = val;
  ++this->mSize;

  return position;
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::insert(T* position, size_t n, const T& val)
{
  if ((this->mSize + n) > mCapacity)
  {
    throw std::runtime_error("fixed_vector: insert() fill range exceeds capacity");
  }

  //Slide everything to the right 'n' spaces to make room for the new elements.
  for (T* it = (fixed_array_base<T,Alloc>::end() + (n - 1)); it != (position + (n - 1)); --it)
  {
    *it = *(it - n);
  }

  //Insert the new elements into the remaining space.
  for (T* it = position; it != (position + n); ++it)
  {
    *it = val;
  }

  this->mSize += n;
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::insert(T* position, const T* first, const T* last)
{
  size_t n = last - first;
  if ((this->mSize + n) > mCapacity)
  {
    throw std::runtime_error("fixed_vector: insert() fill range exceeds capacity");
  }

  //Slide everything to the right 'n' spaces to make room for the new elements.
  for (T* it = (fixed_array_base<T,Alloc>::end() + (n - 1)); it != (position + (n - 1)); --it)
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

template<class T, class Alloc> size_t fixed_vector_base<T,Alloc>::max_size() const
{
  return mCapacity;
}

template<class T, class Alloc> fixed_vector_base<T,Alloc>& fixed_vector_base<T,Alloc>::operator=(const fixed_vector_base<T,Alloc>& obj)
{
  if (obj.size() > mCapacity)
  {
    throw std::runtime_error("fixed_vector: assignment operator's parameter size exceeds capacity");
  }

  this->mSize = obj.size();
  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }

  return *this;
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::pop_back()
{
  --this->mSize;
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::push_back(const T& val)
{
  if (this->mSize >= mCapacity)
  {
    throw std::runtime_error("fixed_vector: push_back() caused size to exceed capacity");
  }
  this->mAryPtr[this->mSize] = val;
  ++this->mSize;
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::resize(size_t n, const T& val)
{
  if (n < this->mSize)
  {
    this->mSize = n;
  }
  else if (n > this->mSize)
  {
    insert(fixed_array_base<T,Alloc>::end(), n - this->mSize, val);
  }
}

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::swap(fixed_vector_base<T,Alloc>& obj)
{
  if ((obj.size() > mCapacity) || (this->mSize > obj.capacity()))
  {
    throw std::runtime_error("fixed_vector: swap() parameters' size exceed capacity");
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

template<class T, class Alloc> void fixed_vector_base<T,Alloc>::set_size(size_t size)
{
  this->mSize = size;
}

#endif /* FIXED_VECTOR_BASE_H */
