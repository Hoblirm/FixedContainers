#ifndef FLEX_ARRAY_H
#define FLEX_ARRAY_H

#include <algorithm>

#include <flex/allocator.h>
#include <flex/array_iterator.h>

namespace flex
{
  template<class T> class array_base: public allocation_guard
  {
  public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef array_reverse_iterator<T> reverse_iterator;
    typedef array_const_reverse_iterator<T> const_reverse_iterator;

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

    value_type* data();
    const value_type* data() const;

    bool empty() const;

    iterator end();
    const_iterator end() const;

    void fill(const T& v);

    reference front();
    const_reference front() const;

    size_t max_size() const;

    array_base<T>& operator=(const array_base<T>& obj);
    reference operator[](size_t n);
    const_reference operator[](size_t n) const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

    size_t size() const;

  protected:
    explicit array_base(size_t size);
    array_base(size_t size, T* ptr);

    size_t mSize;
    T* mAryPtr;
  };

  template<class T> T& array_base<T>::at(size_t n)
  {
    if (n < mSize)
    {
      return mAryPtr[n];
    }
    else
    {
      throw std::out_of_range("Fixed container called at() with out-of-bounds index.");
    }
  }

  template<class T> const T& array_base<T>::at(size_t n) const
  {
    if (n < mSize)
    {
      return mAryPtr[n];
    }
    else
    {
      throw std::out_of_range("Fixed container called at() with out-of-bounds index.");
    }
  }

  template<class T> T& array_base<T>::back()
  {
    return mAryPtr[mSize - 1];
  }

  template<class T> const T& array_base<T>::back() const
  {
    return mAryPtr[mSize - 1];
  }

  template<class T> T* array_base<T>::begin()
  {
    return mAryPtr;
  }

  template<class T> const T* array_base<T>::begin() const
  {
    return mAryPtr;
  }

  template<class T> const T* array_base<T>::cbegin() const
  {
    return mAryPtr;
  }

  template<class T> const T* array_base<T>::cend() const
  {
    return &mAryPtr[mSize];
  }

  template<class T> array_const_reverse_iterator<T> array_base<T>::crbegin() const
  {
    return array_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
  }

  template<class T> array_const_reverse_iterator<T> array_base<T>::crend() const
  {
    return array_const_reverse_iterator<T>(&mAryPtr[-1]);
  }

  template<class T> T* array_base<T>::data()
  {
    return mAryPtr;
  }

  template<class T> const T* array_base<T>::data() const
  {
    return mAryPtr;
  }

  template<class T> bool array_base<T>::empty() const
  {
    return (0 == mSize);
  }

  template<class T> T* array_base<T>::end()
  {
    return &mAryPtr[mSize];
  }

  template<class T> const T* array_base<T>::end() const
  {
    return &mAryPtr[mSize];
  }

  template<class T> void array_base<T>::fill(const T& v)
  {
    std::fill(mAryPtr, mAryPtr + mSize, v);
  }

  template<class T> T& array_base<T>::front()
  {
    return mAryPtr[0];
  }

  template<class T> const T& array_base<T>::front() const
  {
    return mAryPtr[0];
  }

  template<class T> size_t array_base<T>::max_size() const
  {
    return mSize;
  }

  template<class T> array_base<T>& array_base<T>::operator=(const array_base<T>& obj)
  {
    if (obj.size() != mSize)
    {
      throw std::runtime_error("array: assignment operator's parameter size doesn't match");
    }
    std::copy(obj.begin(), obj.end(), mAryPtr);
    return *this;
  }

  template<class T> T& array_base<T>::operator[](size_t n)
  {
    return mAryPtr[n];
  }

  template<class T> const T& array_base<T>::operator[](size_t n) const
  {
    return mAryPtr[n];
  }

  template<class T> array_reverse_iterator<T> array_base<T>::rbegin()
  {
    return array_reverse_iterator<T>(&mAryPtr[mSize - 1]);
  }

  template<class T> array_const_reverse_iterator<T> array_base<T>::rbegin() const
  {
    return array_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
  }

  template<class T> array_reverse_iterator<T> array_base<T>::rend()
  {
    return array_reverse_iterator<T>(&mAryPtr[-1]);
  }

  template<class T> array_const_reverse_iterator<T> array_base<T>::rend() const
  {
    return array_const_reverse_iterator<T>(&mAryPtr[-1]);
  }

  template<class T> size_t array_base<T>::size() const
  {
    return mSize;
  }

  template<class T> array_base<T>::array_base(size_t size) :
      mSize(size), mAryPtr(NULL)
  {
  }

  template<class T> array_base<T>::array_base(size_t size, T* ptr) :
      mSize(size), mAryPtr(ptr)
  {
  }

  template<class T>
  bool operator==(const array_base<T>& lhs, const array_base<T>& rhs)
  {
    if (lhs.size() != rhs.size())
    {
      return false;
    }
    else
    {
      for (int i = 0; i < lhs.size(); ++i)
      {
        if (lhs[i] != rhs[i])
        {
          return false;
        }
      }
      return true;
    }
  }

  template<class T>
  bool operator<(const array_base<T>& lhs, const array_base<T>& rhs)
  {
    if (lhs.size() < rhs.size())
    {
      for (int i = 0; i < lhs.size(); ++i)
      {
        if (lhs[i] < rhs[i])
        {
          return true;
        }
        else if (lhs[i] > rhs[i])
        {
          return false;
        }
      }
      return true;
    }
    else
    {
      for (int i = 0; i < rhs.size(); ++i)
      {
        if (lhs[i] < rhs[i])
        {
          return true;
        }
        else if (lhs[i] > rhs[i])
        {
          return false;
        }
      }
      return false;
    }
  }

  template<class T>
  bool operator!=(const array_base<T>& lhs, const array_base<T>& rhs)
  {
    return !(lhs == rhs);
  }

  template<class T>
  bool operator>(const array_base<T>& lhs, const array_base<T>& rhs)
  {
    return rhs < lhs;
  }

  template<class T>
  bool operator<=(const array_base<T>& lhs, const array_base<T>& rhs)
  {
    return !(rhs < lhs);
  }

  template<class T>
  bool operator>=(const array_base<T>& lhs, const array_base<T>& rhs)
  {
    return !(lhs < rhs);
  }

  template<class T, size_t N = 0> class array: public array_base<T>
  {
  public:
    using array_base<T>::mAryPtr;
    using array_base<T>::mSize;

    array();
    array(const array<T, N> & obj);
    array<T, N>& operator=(const array<T, N>& obj);
    size_t size() const;
    void swap(array<T, N>& obj);
  private:
    T mAry[N];
  };

  template<class T, size_t N> array<T, N>::array() :
      array_base<T>(N, mAry)
  {
  }

  template<class T, size_t N> array<T, N>::array(const array<T, N> & obj) :
      array_base<T>(obj.size(), mAry)
  {
    std::copy(obj.begin(),obj.end(),mAryPtr);
  }

  template<class T, size_t N> array<T, N>& array<T, N>::operator=(const array<T, N> & obj)
  {
    std::copy(obj.begin(),obj.end(),mAryPtr);
    return *this;
  }

  template<class T, size_t N> size_t array<T, N>::size() const
  {
    return N;
  }

  template<class T, size_t N> void array<T, N>::swap(array<T, N>& obj)
  {
    std::swap_ranges(mAryPtr,mAryPtr+mSize,obj.begin());
  }

} //namespace flex

#endif /* FLEX_ARRAY_H */
