#ifndef FIXED_ARRAY_H 
#define FIXED_ARRAY_H

#include <allocation_guard.h>
#include <fixed_iterator.h>

template<class T> class fixed_array_base: public allocation_guard
{
public:
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef fixed_reverse_iterator<T> reverse_iterator;
  typedef fixed_const_reverse_iterator<T> const_reverse_iterator;
  typedef T& reference;
  typedef const T& const_reference;

  reference at(size_t n);
  const_reference at(size_t n) const;

  reference back();
  const_reference back() const;

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  T* data();
  const T* data() const;

  bool empty() const;

  iterator end();
  const_iterator end() const;

  void fill(const T& v);

  reference front();
  const_reference front() const;

  size_t max_size() const;

  //fixed_array_base<T>& operator=(const fixed_array_base<T>& obj);

  reference operator[](size_t n);
  const_reference operator[](size_t n) const;

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;

  size_t size() const;
  void swap(fixed_array_base<T>& obj);

protected:
  fixed_array_base(size_t size);
  fixed_array_base(size_t size, T* ptr);

  T* mAryPtr;
  size_t mSize;
};

template<class T> T& fixed_array_base<T>::at(size_t n)
{
  if (n < mSize)
  {
    return mAryPtr[n];
  }
  else
  {
    throw std::out_of_range("Fixed container caught out-of-bounds exception.");
  }
}

template<class T> const T& fixed_array_base<T>::at(size_t n) const
{
  if (n < mSize)
  {
    return mAryPtr[n];
  }
  else
  {
    throw std::out_of_range("Fixed container caught out-of-bounds exception.");
  }
}

template<class T> T& fixed_array_base<T>::back()
{
  return mAryPtr[mSize - 1];
}

template<class T> const T& fixed_array_base<T>::back() const
{
  return mAryPtr[mSize - 1];
}

template<class T> T* fixed_array_base<T>::begin()
{
  return mAryPtr;
}

template<class T> const T* fixed_array_base<T>::begin() const
{
  return mAryPtr;
}

template<class T> const T* fixed_array_base<T>::cbegin() const
{
  return mAryPtr;
}

template<class T> const T* fixed_array_base<T>::cend() const
{
  return &mAryPtr[mSize];
}

template<class T> fixed_const_reverse_iterator<T> fixed_array_base<T>::crbegin() const
{
  return fixed_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T> fixed_const_reverse_iterator<T> fixed_array_base<T>::crend() const
{
  return fixed_const_reverse_iterator<T>(&mAryPtr[-1]);
}

template<class T> T* fixed_array_base<T>::data()
{
  return mAryPtr;
}

template<class T> const T* fixed_array_base<T>::data() const
{
  return mAryPtr;
}

template<class T> bool fixed_array_base<T>::empty() const
{
  return (0 == mSize);
}

template<class T> T* fixed_array_base<T>::end()
{
  return &mAryPtr[mSize];
}

template<class T> const T* fixed_array_base<T>::end() const
{
  return &mAryPtr[mSize];
}

template<class T> void fixed_array_base<T>::fill(const T& v)
{
  for (iterator it = begin(); it < end(); ++it)
  {
    *it = v;
  }
}

template<class T> T& fixed_array_base<T>::front()
{
  return mAryPtr[0];
}

template<class T> const T& fixed_array_base<T>::front() const
{
  return mAryPtr[0];
}

template<class T> size_t fixed_array_base<T>::max_size() const
{
  return mSize;
}

/*
 template<class T> fixed_array_base<T>& fixed_array_base<T>::operator=(const fixed_array_base<T>& obj)
 {
 if (mSize != obj.size())
 {
 if (mAryPtr)
 delete[] mAryPtr;
 allocate(obj.size());
 }

 for (int i = 0; i < mSize; i++)
 {
 mAryPtr[i] = obj[i];
 }

 return *this;
 }
 */
template<class T> T& fixed_array_base<T>::operator[](size_t n)
{
  return mAryPtr[n];
}

template<class T> const T& fixed_array_base<T>::operator[](size_t n) const
{
  return mAryPtr[n];
}

template<class T> fixed_reverse_iterator<T> fixed_array_base<T>::rbegin()
{
  return fixed_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T> fixed_const_reverse_iterator<T> fixed_array_base<T>::rbegin() const
{
  return fixed_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T> fixed_reverse_iterator<T> fixed_array_base<T>::rend()
{
  return fixed_reverse_iterator<T>(&mAryPtr[-1]);
}

template<class T> fixed_const_reverse_iterator<T> fixed_array_base<T>::rend() const
{
  return fixed_const_reverse_iterator<T>(&mAryPtr[-1]);
}

template<class T> size_t fixed_array_base<T>::size() const
{
  return mSize;
}

template<class T> void fixed_array_base<T>::swap(fixed_array_base<T>& obj)
{
  if (mSize == obj.size())
  {
    T tmp;
    for (int i = 0; i < mSize; i++)
    {
      tmp = mAryPtr[i];
      mAryPtr[i] = obj[i];
      obj[i] = tmp;
    }
  }
}

template<class T> fixed_array_base<T>::fixed_array_base(size_t size) :
    mSize(size)
{
}

template<class T> fixed_array_base<T>::fixed_array_base(size_t size, T* ptr) :
    mSize(size), mAryPtr(ptr)
{
}

template<class T, size_t N = 0> class fixed_array: public fixed_array_base<T>
{
public:
  fixed_array();
  fixed_array(const fixed_array<T, N> & obj);
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
