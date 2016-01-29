#ifndef FIXED_POOL_H
#define FIXED_POOL_H

#include <allocation_guard.h>

template<class T> class fixed_pool_base: allocation_guard
{
public:
  T* allocate();
  T* allocate_no_throw();
  void deallocate(T* ptr);

  size_t outstanding() const;
  size_t available() const;
  size_t max_size() const;
  
protected:
  fixed_pool_base(size_t capacity);
  fixed_pool_base(size_t capacity, T* contentPtr, T** ptrPtr);

  size_t mIndex;
  size_t mCapacity;

  T* mContentList;
  T** mPtrList;
};

template<class T> fixed_pool_base<T>::fixed_pool_base(size_t capacity) :
    mIndex(0), mCapacity(capacity)
{
}

template<class T> fixed_pool_base<T>::fixed_pool_base(size_t capacity, T* contentPtr, T** ptrPtr) :
    mIndex(0), mCapacity(capacity), mContentList(contentPtr), mPtrList(ptrPtr)
{
}

template<class T> T* fixed_pool_base<T>::allocate()
{
  if (mIndex == mCapacity)
  {
    throw std::runtime_error("fixed_pool: allocate() exceeded capacity.");
  }
  return mPtrList[mIndex++];
}

template<class T> T* fixed_pool_base<T>::allocate_no_throw()
{
  return mPtrList[mIndex++];
}

template<class T> void fixed_pool_base<T>::deallocate(T* ptr)
{
  mPtrList[--mIndex] = ptr;
}

template<class T> size_t fixed_pool_base<T>::available() const
{
  return mCapacity - mIndex;
}

template<class T> size_t fixed_pool_base<T>::outstanding() const
{
  return mIndex;
}

template<class T> size_t fixed_pool_base<T>::max_size() const
{
  return mCapacity;
}

template<class T, size_t N = 0> class fixed_pool: public fixed_pool_base<T>
{
public:
  fixed_pool();
  operator const fixed_pool<T,0>&() const;
  operator fixed_pool<T,0>&();
private:
   //TODO: bring back or remove.
  //fixed_pool(const fixed_pool<T, N> & obj);
  fixed_pool<T, N>& operator=(const fixed_pool<T, N>& obj);
  T mContentAry[N];
  T* mPtrAry[N];
};

template<class T, size_t N> fixed_pool<T, N>::fixed_pool() :
    fixed_pool_base<T>(N, mContentAry, mPtrAry)
{
  for (size_t i = 0; i < this->mCapacity; i++)
  {
    mPtrAry[i] = &mContentAry[i];
  }
}

template<class T, size_t N> fixed_pool<T, N>::operator const fixed_pool<T,0>&() const
{
  return *((fixed_pool<T, 0>*) this);
}

template<class T, size_t N> fixed_pool<T, N>::operator fixed_pool<T,0>&()
{
  return *((fixed_pool<T, 0>*) this);
}

template<class T> class fixed_pool<T, 0> : public fixed_pool_base<T>
{
public:
  fixed_pool(size_t size);
  ~fixed_pool();
private:
   //TODO: bring back or remove.
  //fixed_pool(const fixed_pool<T> & obj);
  fixed_pool<T, 0>& operator=(const fixed_pool<T, 0> & obj);
  void allocate_pool();
};

template<class T> fixed_pool<T, 0>::fixed_pool(size_t size) :
    fixed_pool_base<T>(size)
{
  allocate_pool();
  for (size_t i = 0; i < this->mCapacity; i++)
  {
    this->mPtrList[i] = &(this->mContentList[i]);
  }
}

template<class T> fixed_pool<T, 0>::~fixed_pool()
{
  delete[] this->mPtrList;
  delete[] this->mContentList;
}

template<class T> void fixed_pool<T, 0>::allocate_pool()
{
  if (allocation_guard::is_enabled())
  {
    throw std::runtime_error("allocation_guard: fixed_pool performed allocation.");
  }
  else
  {
    this->mContentList = new T[this->mCapacity];
    this->mPtrList = new T*[this->mCapacity];
  }
}

#endif /* FIXED_POOL_H */
