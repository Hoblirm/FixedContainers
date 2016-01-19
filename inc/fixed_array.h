#ifndef FIXED_ARRAY_H 
#define FIXED_ARRAY_H

#include <allocation_guard.h>
#include <fixed_iterator.h>

//template <class T, size_t N=0> class fixed_array;

template<class T/*, size_t N=0*/> class fixed_array: public allocation_guard
{

public:
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef fixed_reverse_iterator<T> reverse_iterator;
  typedef T& reference;
  typedef const T& const_reference;

  fixed_array(size_t capacity);
  fixed_array(const fixed_array<T>& obj);

  ~fixed_array();

  reference at(size_t n);
  const_reference at(size_t n) const;

  reference back();
  const_reference back() const;

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  T* data();
  const T* data() const;

  bool empty() const;

  iterator end();
  const_iterator end() const;

  void fill(const T& v);

  reference front();
  const_reference front() const;

  size_t max_size() const;

  //fixed_array<T>& operator=(const fixed_array<T>& obj);

  reference operator[](size_t n);
  const_reference operator[](size_t n) const;

  reverse_iterator rbegin();
  reverse_iterator rend();

  size_t size() const;
  void swap(fixed_array<T>& obj);

protected:
  T* mAryPtr;
  size_t mCapacity;

private:
  void allocate();
  //Since we what all template versions of the class to be ordered in memory in the same fashion
  //we want the array to be defined last, as it has a variable size between template classes.
  /*T mAry[N];*/
};

template<class T> fixed_array<T>::fixed_array(size_t capacity) :
    mCapacity(capacity)
{
  allocate();
}

template<class T> fixed_array<T>::fixed_array(const fixed_array<T>& obj) :
    mCapacity(obj.size())
{
  allocate();
  for (int i = 0; i < mCapacity; i++)
  {
    mAryPtr[i] = obj[i];
  }
}

template<class T> fixed_array<T>::~fixed_array()
{
  delete[] mAryPtr;
}

template<class T> T& fixed_array<T>::at(size_t n)
{
  if (n < mCapacity)
  {
    return mAryPtr[n];
  }
  else
  {
    throw std::out_of_range("Fixed container caught out-of-bounds exception.");
  }
}

template<class T> const T& fixed_array<T>::at(size_t n) const
{
  if (n < mCapacity)
  {
    return mAryPtr[n];
  }
  else
  {
    throw std::out_of_range("Fixed container caught out-of-bounds exception.");
  }
}

template<class T> T& fixed_array<T>::back()
{
  return mAryPtr[mCapacity - 1];
}

template<class T> const T& fixed_array<T>::back() const
{
  return mAryPtr[mCapacity - 1];
}

template<class T> T* fixed_array<T>::begin()
{
  return mAryPtr;
}

template<class T> const T* fixed_array<T>::begin() const
{
  return mAryPtr;
}

template<class T> const T* fixed_array<T>::cbegin() const
{
  return mAryPtr;
}

template<class T> const T* fixed_array<T>::cend() const
{
  return &mAryPtr[mCapacity];
}

template<class T> T* fixed_array<T>::data()
{
  return mAryPtr;
}

template<class T> const T* fixed_array<T>::data() const
{
  return mAryPtr;
}

template<class T> bool fixed_array<T>::empty() const
{
  return (0 == mCapacity);
}

template<class T> T* fixed_array<T>::end()
{
  return &mAryPtr[mCapacity];
}

template<class T> const T* fixed_array<T>::end() const
{
  return &mAryPtr[mCapacity];
}

template<class T> void fixed_array<T>::fill(const T& v)
{
  for (iterator it = begin(); it < end(); ++it)
  {
    *it = v;
  }
}

template<class T> T& fixed_array<T>::front()
{
  return mAryPtr[0];
}

template<class T> const T& fixed_array<T>::front() const
{
  return mAryPtr[0];
}

template<class T> size_t fixed_array<T>::max_size() const
{
  return mCapacity;
}

/*
 template<class T> fixed_array<T>& fixed_array<T>::operator=(const fixed_array<T>& obj)
 {
 if (mCapacity != obj.size())
 {
 if (mAryPtr)
 delete[] mAryPtr;
 allocate(obj.size());
 }

 for (int i = 0; i < mCapacity; i++)
 {
 mAryPtr[i] = obj[i];
 }

 return *this;
 }
 */
template<class T> T& fixed_array<T>::operator[](size_t n)
{
  return mAryPtr[n];
}

template<class T> const T& fixed_array<T>::operator[](size_t n) const
{
  return mAryPtr[n];
}

template<class T> fixed_reverse_iterator<T> fixed_array<T>::rbegin()
{
  return fixed_reverse_iterator<T>(&mAryPtr[mCapacity-1]);
}

template<class T> fixed_reverse_iterator<T> fixed_array<T>::rend()
{
  return fixed_reverse_iterator<T>(&mAryPtr[-1]);
}

template<class T> size_t fixed_array<T>::size() const
{
  return mCapacity;
}

template<class T> void fixed_array<T>::swap(fixed_array<T>& obj)
{
  if (mCapacity == obj.size())
  {
    T tmp;
    for (int i = 0; i < mCapacity; i++)
    {
      tmp = mAryPtr[i];
      mAryPtr[i] = obj[i];
      obj[i] = tmp;
    }
  }
}

template<class T> void fixed_array<T>::allocate()
{
  if (allocation_guard::is_enabled())
  {
    throw std::runtime_error("allocation_guard: fixed_array performed allocation.");
  }
  else
  {
    mAryPtr = new T[mCapacity]();
  }
}


#endif /* FIXED_ARRAY_H */
