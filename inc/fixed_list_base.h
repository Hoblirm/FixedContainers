#ifndef FIXED_LIST_BASE_H
#define FIXED_LIST_BASE_H

#include <fixed_pool.h>
#include <fixed_list_iterator.h>

template<class T> class fixed_list_base: public allocation_guard
{
public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef fixed_list_iterator<T> iterator;
  typedef fixed_list_const_iterator<T> const_iterator;
  typedef fixed_list_reverse_iterator<T> reverse_iterator;
  typedef fixed_list_const_reverse_iterator<T> const_reverse_iterator;

  reference back();
  const_reference back() const;

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  bool empty() const;

  iterator end();
  const_iterator end() const;

  reference front();
  const_reference front() const;

  size_t max_size() const;

  fixed_list_base<T>& operator=(const fixed_list_base<T>& obj);
  reference operator[](size_t n);
  const_reference operator[](size_t n) const;

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;

  size_t size() const;
  void swap(fixed_list_base<T>& obj);

protected:
  fixed_list_base(size_t size);
  fixed_list_base(size_t size, T* ptr);


  size_t mSize;
  T* mAryPtr;
};


template<class T> T& fixed_list_base<T>::back()
{
  return mAryPtr[mSize - 1];
}

template<class T> const T& fixed_list_base<T>::back() const
{
  return mAryPtr[mSize - 1];
}

template<class T> fixed_list_const_reverse_iterator<T> fixed_list_base<T>::crbegin() const
{
  return fixed_list_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T> fixed_list_const_reverse_iterator<T> fixed_list_base<T>::crend() const
{
  return fixed_list_const_reverse_iterator<T>(NULL);
}

template<class T> bool fixed_list_base<T>::empty() const
{
  return (0 == mSize);
}

template<class T> T& fixed_list_base<T>::front()
{
  return mAryPtr[0];
}

template<class T> const T& fixed_list_base<T>::front() const
{
  return mAryPtr[0];
}

template<class T> size_t fixed_list_base<T>::max_size() const
{
  return mSize;
}

template<class T> fixed_list_base<T>& fixed_list_base<T>::operator=(const fixed_list_base<T>& obj)
{
  if (obj.size() != mSize)
  {
    throw std::runtime_error("fixed_list: assignment operator's parameter size doesn't match");
  }

  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }

  return *this;
}

template<class T> T& fixed_list_base<T>::operator[](size_t n)
{
  return mAryPtr[n];
}

template<class T> const T& fixed_list_base<T>::operator[](size_t n) const
{
  return mAryPtr[n];
}

template<class T> fixed_list_reverse_iterator<T> fixed_list_base<T>::rbegin()
{
  return fixed_list_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T> fixed_list_const_reverse_iterator<T> fixed_list_base<T>::rbegin() const
{
  return fixed_list_const_reverse_iterator<T>(&mAryPtr[mSize - 1]);
}

template<class T> fixed_list_reverse_iterator<T> fixed_list_base<T>::rend()
{
  return fixed_list_reverse_iterator<T>(NULL);
}

template<class T> fixed_list_const_reverse_iterator<T> fixed_list_base<T>::rend() const
{
  return fixed_list_const_reverse_iterator<T>(NULL);
}

template<class T> size_t fixed_list_base<T>::size() const
{
  return mSize;
}

template<class T> void fixed_list_base<T>::swap(fixed_list_base<T>& obj)
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

template<class T> fixed_list_base<T>::fixed_list_base(size_t size) :
    mSize(size)
{
}

template<class T> fixed_list_base<T>::fixed_list_base(size_t size, T* ptr) :
    mSize(size), mAryPtr(ptr)
{
}

template<class T>
bool operator==(const fixed_list_base<T>& lhs, const fixed_list_base<T>& rhs)
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
bool operator<(const fixed_list_base<T>& lhs, const fixed_list_base<T>& rhs)
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
bool operator!=(const fixed_list_base<T>& lhs, const fixed_list_base<T>& rhs)
{
  return !(lhs == rhs);
}

template<class T>
bool operator>(const fixed_list_base<T>& lhs, const fixed_list_base<T>& rhs)
{
  return rhs < lhs;
}

template<class T>
bool operator<=(const fixed_list_base<T>& lhs, const fixed_list_base<T>& rhs)
{
  return !(rhs < lhs);
}

template<class T>
bool operator>=(const fixed_list_base<T>& lhs, const fixed_list_base<T>& rhs)
{
  return !(lhs < rhs);
}

#endif /* FIXED_LIST_BASE_H */
