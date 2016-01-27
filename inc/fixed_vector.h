#ifndef FIXED_VECTOR_H
#define FIXED_VECTOR_H

#include <fixed_vector_base.h>

template<class T, size_t N = 0> class fixed_vector: public fixed_vector_base<T>
{
public:
  fixed_vector();
  fixed_vector(size_t size, const T& val = T());
  fixed_vector(const T* first, const T* last);
  fixed_vector(const fixed_vector<T, 0> & obj);
  fixed_vector<T, N>& operator=(const fixed_vector<T, N>& obj);
  fixed_vector<T, N>& operator=(const fixed_vector<T, 0>& obj);
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
  for (const T* it = first; it != last; ++it)
  {
    this->mAryPtr[this->mSize] = *it;
    ++this->mSize;
  }
}

template<class T, size_t N> fixed_vector<T, N>::fixed_vector(const fixed_vector<T, 0> & obj) :
    fixed_vector_base<T>(N, mAry)
{
  *this = obj;
}

template<class T, size_t N> fixed_vector<T, N>& fixed_vector<T, N>::operator=(const fixed_vector<T, N>& obj)
{
  this->mSize = obj.size();
  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }
  return *this;
}

template<class T, size_t N> fixed_vector<T, N>& fixed_vector<T, N>::operator=(const fixed_vector<T, 0>& obj)
{
  fixed_vector_base<T>::operator=(obj);
  return *this;
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
  fixed_vector<T, 0>& operator=(const fixed_vector<T, 0>& obj);

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
  for (const T* it = first; it != last; ++it)
  {
    this->mAryPtr[this->mSize] = *it;
    ++this->mSize;
  }
}

template<class T> fixed_vector<T, 0>::fixed_vector(const fixed_vector<T, 0> & obj) :
    fixed_vector_base<T>(obj.capacity())
{
  allocate();
  *this = obj;
}

template<class T> fixed_vector<T, 0>::~fixed_vector()
{
  delete[] this->mAryPtr;
}

template<class T> fixed_vector<T, 0>& fixed_vector<T, 0>::operator=(const fixed_vector<T, 0>& obj)
{
  fixed_vector_base<T>::operator=(obj);
  return *this;
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

#endif /* FIXED_VECTOR_H */
