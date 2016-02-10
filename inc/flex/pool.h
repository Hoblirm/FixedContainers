#ifndef FLEX_POOL_H
#define FLEX_POOL_H

#include <flex/allocation_guard.h>

namespace flex
{

  template<class T> class pool: allocation_guard
  {
  public:
    explicit pool(size_t capacity);
    ~pool();
    T* allocate();
    void deallocate(T* ptr);

    size_t outstanding() const;
    size_t available() const;
    size_t max_size() const;

  protected:
    pool(size_t capacity, T* contentPtr, T** ptrPtr);
    T* allocate_no_throw();

    size_t mIndex;
    size_t mCapacity;
    bool mFixed;

    T* mContentList;
    T** mPtrList;
  };

  template<class T> pool<T>::pool(size_t capacity) :
      mIndex(0), mCapacity(capacity), mFixed(false)
  {
    if (allocation_guard::is_enabled())
    {
      throw std::runtime_error("allocation_guard: pool performed allocation.");
    }
    else
    {
      mContentList = new T[capacity];
      mPtrList = new T*[capacity];
      for (size_t i = 0; i < mCapacity; i++)
      {
        mPtrList[i] = &mContentList[i];
      }
    }
  }

  template<class T> pool<T>::pool(size_t capacity, T* contentPtr, T** ptrPtr) :
      mIndex(0), mCapacity(capacity), mFixed(true), mContentList(contentPtr), mPtrList(ptrPtr)
  {
  }

  template<class T> pool<T>::~pool()
  {
    if (!mFixed)
    {
      if (mContentList)
      {
        delete[] mContentList;
      }
      if (mPtrList)
      {
        delete[] mPtrList;
      }
    }
  }

  template<class T> T* pool<T>::allocate()
  {
    if (mIndex == mCapacity)
    {
      throw std::runtime_error("fixed_pool: allocate() exceeded capacity.");
    }
    return mPtrList[mIndex++];
  }

  template<class T> T* pool<T>::allocate_no_throw()
  {
    return mPtrList[mIndex++];
  }

  template<class T> void pool<T>::deallocate(T* ptr)
  {
    mPtrList[--mIndex] = ptr;
  }

  template<class T> size_t pool<T>::available() const
  {
    return mCapacity - mIndex;
  }

  template<class T> size_t pool<T>::outstanding() const
  {
    return mIndex;
  }

  template<class T> size_t pool<T>::max_size() const
  {
    return mCapacity;
  }

}  //namespace flex
#endif /* FLEX_POOL_H */
