#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stdexcept>

template<class T> class object_pool
{

public:
  object_pool(int capacity);
  ~object_pool();

  T* allocate();
  void release(T* ptr);

  int available();
  int capacity();

private:
  T* mContentList;
  T** mPtrList;

  int mIndex;
  int mCapacity;
};

template<class T> object_pool<T>::object_pool(int capacity)
{
  mCapacity = capacity;
  mIndex = 0;

  mContentList = new T[mCapacity]();
  mPtrList = new T*[mCapacity]();
  for (int i = 0; i < mCapacity; i++)
  {
    mPtrList[i] = &mContentList[i];
  }
}

template<class T> object_pool<T>::~object_pool()
{
  delete[] mPtrList;
  delete[] mContentList;
}

template<class T> T* object_pool<T>::allocate()
{
  if (mIndex == mCapacity)
  {
    throw std::runtime_error("Object pool at capacity and failed allocation.");
  }
  return mPtrList[mIndex++];
}

template<class T> void object_pool<T>::release(T* ptr)
{
  ptr->reset();
  mPtrList[--mIndex] = ptr;
}

template<class T> int object_pool<T>::available()
{
  return mCapacity - mIndex;
}

template<class T> int object_pool<T>::capacity()
{
  return mCapacity;
}

#endif /* OBJECT_POOL_H */
