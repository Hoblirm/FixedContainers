#ifndef FIXED_ARRAY_H 
#define FIXED_ARRAY_H

#include <fixed_array_base.h>

template<class T, size_t N = 0> class fixed_array: public fixed_array_base<T>
{
public:
  fixed_array();
  fixed_array(const fixed_array<T, N> & obj);
  fixed_array<T, N>& operator=(const fixed_array<T, N>& obj);
  fixed_array<T, N>& operator=(const fixed_array<T, 0>& obj);
  operator const fixed_array<T,0>&() const;
  operator fixed_array<T,0>&();
private:
  T mAry[N];
};

template<class T, size_t N> fixed_array<T, N>::fixed_array() :
    fixed_array_base<T>(N, mAry)
{
}

template<class T, size_t N> fixed_array<T, N>::fixed_array(const fixed_array<T, N> & obj) :
    fixed_array_base<T>(obj.size(), mAry)
{
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T, size_t N> fixed_array<T, N>& fixed_array<T, N>::operator=(const fixed_array<T, N> & obj)
{
  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }
  return *this;
}

template<class T, size_t N> fixed_array<T, N>& fixed_array<T, N>::operator=(const fixed_array<T, 0> & obj)
{
  fixed_array_base<T>::operator=(obj);
  return *this;
}

template<class T, size_t N> fixed_array<T, N>::operator const fixed_array<T,0>&() const
{
  return *((fixed_array<T, 0>*) this);
}

template<class T, size_t N> fixed_array<T, N>::operator fixed_array<T,0>&()
{
  return *((fixed_array<T, 0>*) this);
}


template<class T> class fixed_array<T, 0> : public fixed_array_base<T>
{
public:
  fixed_array(size_t size);
  fixed_array(const fixed_array<T> & obj);
  ~fixed_array();
  fixed_array<T, 0>& operator=(const fixed_array<T, 0> & obj);
private:
  void allocate();
};

template<class T> fixed_array<T, 0>::fixed_array(size_t size) :
    fixed_array_base<T>(size)
{
  allocate();
}

template<class T> fixed_array<T, 0>::fixed_array(const fixed_array<T> & obj) :
    fixed_array_base<T>(obj.size())
{
  allocate();
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T> fixed_array<T, 0>::~fixed_array()
{
  delete[] this->mAryPtr;
}

template<class T> fixed_array<T, 0>& fixed_array<T, 0>::operator=(const fixed_array<T, 0> & obj)
{
  fixed_array_base<T>::operator=(obj);
  return *this;
}

template<class T> void fixed_array<T, 0>::allocate()
{
  if (allocation_guard::is_enabled())
  {
    throw std::runtime_error("allocation_guard: fixed_array performed allocation.");
  }
  else
  {
    this->mAryPtr = new T[this->mSize];
  }
}

#endif /* FIXED_ARRAY_H */
