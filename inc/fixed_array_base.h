#ifndef FIXED_ARRAY_BASE_H
#define FIXED_ARRAY_BASE_H

#include <allocator.h>
#include <fixed_array_iterator.h>

template<class T, class Alloc = flex::allocator<T> > class fixed_array_base: public allocation_guard
{
public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef fixed_array_reverse_iterator<T> reverse_iterator;
  typedef fixed_array_const_reverse_iterator<T> const_reverse_iterator;
  typedef Alloc allocator_type;

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

  fixed_array_base<T,Alloc>& operator=(const fixed_array_base<T,Alloc>& obj);
  reference operator[](size_t n);
  const_reference operator[](size_t n) const;

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;

  size_t size() const;
  void swap(fixed_array_base<T,Alloc>& obj);

protected:
  fixed_array_base(size_t size);
  fixed_array_base(size_t size, T* ptr);

  size_t mSize;
  T* mAryPtr;
};

template<class T, class Alloc> T& fixed_array_base<T,Alloc>::at(size_t n)
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

template<class T, class Alloc> const T& fixed_array_base<T,Alloc>::at(size_t n) const
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

template<class T, class Alloc> T& fixed_array_base<T,Alloc>::back()
{
  return mAryPtr[mSize - 1];
}

template<class T, class Alloc> const T& fixed_array_base<T,Alloc>::back() const
{
  return mAryPtr[mSize - 1];
}

template<class T, class Alloc> T* fixed_array_base<T,Alloc>::begin()
{
  return mAryPtr;
}

template<class T, class Alloc> const T* fixed_array_base<T,Alloc>::begin() const
{
  return mAryPtr;
}

template<class T, class Alloc> const T* fixed_array_base<T,Alloc>::cbegin() const
{
  return mAryPtr;
}

template<class T, class Alloc> const T* fixed_array_base<T,Alloc>::cend() const
{
  return &mAryPtr[mSize];
}

template<class T, class Alloc> fixed_array_const_reverse_iterator<T> fixed_array_base<T,Alloc>::crbegin() const
{
  return fixed_array_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T, class Alloc> fixed_array_const_reverse_iterator<T> fixed_array_base<T,Alloc>::crend() const
{
  return fixed_array_const_reverse_iterator<T>(&mAryPtr[-1]);
}

template<class T, class Alloc> T* fixed_array_base<T,Alloc>::data()
{
  return mAryPtr;
}

template<class T, class Alloc> const T* fixed_array_base<T,Alloc>::data() const
{
  return mAryPtr;
}

template<class T, class Alloc> bool fixed_array_base<T,Alloc>::empty() const
{
  return (0 == mSize);
}

template<class T, class Alloc> T* fixed_array_base<T,Alloc>::end()
{
  return &mAryPtr[mSize];
}

template<class T, class Alloc> const T* fixed_array_base<T,Alloc>::end() const
{
  return &mAryPtr[mSize];
}

template<class T, class Alloc> void fixed_array_base<T,Alloc>::fill(const T& v)
{
  for (iterator it = begin(); it < end(); ++it)
  {
    *it = v;
  }
}

template<class T, class Alloc> T& fixed_array_base<T,Alloc>::front()
{
  return mAryPtr[0];
}

template<class T, class Alloc> const T& fixed_array_base<T,Alloc>::front() const
{
  return mAryPtr[0];
}

template<class T, class Alloc> size_t fixed_array_base<T,Alloc>::max_size() const
{
  return mSize;
}

template<class T, class Alloc> fixed_array_base<T,Alloc>& fixed_array_base<T,Alloc>::operator=(const fixed_array_base<T,Alloc>& obj)
{
  if (obj.size() != mSize)
  {
    throw std::runtime_error("fixed_array: assignment operator's parameter size doesn't match");
  }

  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }

  return *this;
}

template<class T, class Alloc> T& fixed_array_base<T,Alloc>::operator[](size_t n)
{
  return mAryPtr[n];
}

template<class T, class Alloc> const T& fixed_array_base<T,Alloc>::operator[](size_t n) const
{
  return mAryPtr[n];
}

template<class T, class Alloc> fixed_array_reverse_iterator<T> fixed_array_base<T,Alloc>::rbegin()
{
  return fixed_array_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T, class Alloc> fixed_array_const_reverse_iterator<T> fixed_array_base<T,Alloc>::rbegin() const
{
  return fixed_array_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T, class Alloc> fixed_array_reverse_iterator<T> fixed_array_base<T,Alloc>::rend()
{
  return fixed_array_reverse_iterator<T>(&mAryPtr[-1]);
}

template<class T, class Alloc> fixed_array_const_reverse_iterator<T> fixed_array_base<T,Alloc>::rend() const
{
  return fixed_array_const_reverse_iterator<T>(&mAryPtr[-1]);
}

template<class T, class Alloc> size_t fixed_array_base<T,Alloc>::size() const
{
  return mSize;
}

template<class T, class Alloc> void fixed_array_base<T,Alloc>::swap(fixed_array_base<T,Alloc>& obj)
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

template<class T, class Alloc> fixed_array_base<T,Alloc>::fixed_array_base(size_t size) :
    mSize(size)
{
}

template<class T, class Alloc> fixed_array_base<T,Alloc>::fixed_array_base(size_t size, T* ptr) :
    mSize(size), mAryPtr(ptr)
{
}

template<class T, class Alloc>
bool operator==(const fixed_array_base<T,Alloc>& lhs, const fixed_array_base<T,Alloc>& rhs)
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

template<class T, class Alloc>
bool operator<(const fixed_array_base<T,Alloc>& lhs, const fixed_array_base<T,Alloc>& rhs)
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

template<class T, class Alloc>
bool operator!=(const fixed_array_base<T,Alloc>& lhs, const fixed_array_base<T,Alloc>& rhs)
{
  return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator>(const fixed_array_base<T,Alloc>& lhs, const fixed_array_base<T,Alloc>& rhs)
{
  return rhs < lhs;
}

template<class T, class Alloc>
bool operator<=(const fixed_array_base<T,Alloc>& lhs, const fixed_array_base<T,Alloc>& rhs)
{
  return !(rhs < lhs);
}

template<class T, class Alloc>
bool operator>=(const fixed_array_base<T,Alloc>& lhs, const fixed_array_base<T,Alloc>& rhs)
{
  return !(lhs < rhs);
}

#endif /* FIXED_ARRAY_BASE_H */
