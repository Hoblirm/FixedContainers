#ifndef fixed_vector_H
#define fixed_vector_H

#include <fixed_array.h>

template<class T>
class fixed_vector_base: public fixed_array_base<T>
{
public:

  void assign(size_t size, int val);
  void assign(const T* first,const T* last);

  size_t capacity() const;
  void clear();
  bool empty();
  T* erase(T* position);
  T* erase(T* first, T* last);
  void pop_back();
  void push_back(const T& val);

protected:
  size_t mCapacity;

  fixed_vector_base(size_t capacity) :
      mCapacity(capacity), fixed_array_base<T>(0)
  {
  }

  fixed_vector_base(size_t capacity, T* ptr) :
      mCapacity(capacity), fixed_array_base<T>(0, ptr)
  {
  }

  fixed_vector_base(size_t capacity, size_t size) :
      mCapacity(capacity), fixed_array_base<T>(size)
  {
    if (size > capacity)
    {
      throw std::out_of_range("fixed_vector: constructor called with size greater than capacity");
    }
  }

  fixed_vector_base(size_t capacity, size_t size, T* ptr) :
      mCapacity(capacity), fixed_array_base<T>(size, ptr)
  {
    if (size > capacity)
    {
      throw std::out_of_range("fixed_vector: constructor called with size greater than capacity");
    }
  }
};

template<class T> void fixed_vector_base<T>::assign(size_t size, int val)
{
  if (size > mCapacity)
  {
    throw std::out_of_range("fixed_vector: assign called with size greater than capacity");
  }
  this->mSize = size;
  fixed_array_base<T>::fill(val);
}

template<class T> void fixed_vector_base<T>::assign(const T* first, const T* last)
{
  this->mSize = 0;
  for (const T* it = first; it != last; ++it)
  {
    this->mAryPtr[this->mSize] = *it;
    ++this->mSize;
    if (this->mSize > mCapacity)
    {
      throw std::out_of_range("fixed_vector: assign called with iterator range greater than capacity");
    }
  }
}

template<class T> size_t fixed_vector_base<T>::capacity() const
{
  return mCapacity;
}

template<class T> void fixed_vector_base<T>::clear()
{
  this->mSize = 0;
}

template<class T> bool fixed_vector_base<T>::empty()
{
  return (this->mSize == 0);
}

template<class T> T* fixed_vector_base<T>::erase(T* position)
{
  for (T* it = position; it != (fixed_array_base<T>::end() - 1); ++it)
  {
    *it = *(it + 1);
  }
  --this->mSize;
  return position;
}

template<class T> T* fixed_vector_base<T>::erase(T* first, T* last)
{
  T* leftIt = first;
  for (T* rightIt = last; rightIt != (fixed_array_base<T>::end()); ++rightIt)
  {
    *leftIt = *rightIt;
    ++leftIt;
  }
  this->mSize -= (last-first);
  return first;
}

template<class T> void fixed_vector_base<T>::pop_back()
{
  --this->mSize;
}

template<class T> void fixed_vector_base<T>::push_back(const T& val)
{
  if (this->mSize >= mCapacity)
  {
    throw std::out_of_range("fixed_vector: push_back caused size to exceed capacity");
  }
  this->mAryPtr[this->mSize] = val;
  ++this->mSize;
}

template<class T, size_t N = 0> class fixed_vector: public fixed_vector_base<T>
{
public:
  fixed_vector();
  fixed_vector(size_t size, const T& val = T());
  fixed_vector(const T* first, const T* last);
  fixed_vector(const fixed_vector<T, N> & obj);
  operator const fixed_vector<T,0>&() const;
  operator fixed_vector<T,0>&();
private:
  T mAry[N];
};

template<class T, size_t N> fixed_vector<T, N>::fixed_vector() :
    fixed_vector_base<T>(N, mAry)
{
}

template<class T, size_t N> fixed_vector<T, N>::fixed_vector(size_t size, const T& val) :
    fixed_vector_base<T>(N, size, mAry)
{
  fixed_array_base<T>::fill(val);
}

template<class T, size_t N> fixed_vector<T, N>::fixed_vector(const T* first, const T* last) :
    fixed_vector_base<T>(N, mAry)
{
  this->mSize = 0;
  for (const T* it = first; it!=last;++it)
  {
    this->mAryPtr[this->mSize] = *it;
    ++this->mSize;
  }
}

template<class T, size_t N> fixed_vector<T, N>::fixed_vector(const fixed_vector<T, N> & obj) :
    fixed_vector_base<T>(N, mAry)
{
  this->mSize = obj.size();
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T, size_t N> fixed_vector<T, N>::operator const fixed_vector<T, 0>&() const
{
  return *((fixed_vector<T, 0>*) this);
}

template<class T, size_t N> fixed_vector<T, N>::operator fixed_vector<T, 0>&()
{
  return *((fixed_vector<T, 0>*) this);
}

template<class T> class fixed_vector<T, 0> : public fixed_vector_base<T>
{
public:
  fixed_vector(size_t capacity);
  fixed_vector(size_t capacity, size_t size, const T& val = T());
  fixed_vector(size_t capacity, const T* first, const T* last);
  fixed_vector(const fixed_vector<T, 0> & obj);
  ~fixed_vector();

private:
  void allocate();
};

template<class T> fixed_vector<T, 0>::fixed_vector(size_t capacity) :
    fixed_vector_base<T>(capacity)
{
  allocate();
}

template<class T> fixed_vector<T, 0>::fixed_vector(size_t capacity, size_t size, const T& val) :
    fixed_vector_base<T>(capacity, size)
{
  allocate();
  fixed_array_base<T>::fill(val);
}

template<class T> fixed_vector<T, 0>::fixed_vector(size_t capacity, const T* first, const T* last) :
    fixed_vector_base<T>(capacity)
{
  allocate();
  this->mSize = 0;
  for (const T* it = first; it!=last;++it)
  {
    this->mAryPtr[this->mSize] = *it;
    ++this->mSize;
  }
}

template<class T> fixed_vector<T, 0>::fixed_vector(const fixed_vector<T, 0> & obj) :
    fixed_vector_base<T>(obj.capacity())
{
  allocate();
  this->mSize = obj.size();
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T> fixed_vector<T, 0>::~fixed_vector()
{
  delete[] this->mAryPtr;
}

template<class T> void fixed_vector<T, 0>::allocate()
{
  if (allocation_guard::is_enabled())
  {
    throw std::runtime_error("allocation_guard: fixed_vector performed allocation.");
  }
  else
  {
    this->mAryPtr = new T[this->mCapacity];
  }
}

#endif /* fixed_vector_H */
