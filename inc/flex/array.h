#ifndef FLEX_ARRAY_H
#define FLEX_ARRAY_H

#include <algorithm>
#include <iterator>

#include <flex/allocator.h>

namespace flex
{
  template<class T> class array_base: public allocation_guard
  {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator;
    typedef const T* const_iterator;
    typedef std::reverse_iterator<T*> reverse_iterator;
    typedef std::reverse_iterator<const T*> const_reverse_iterator;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;

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
    explicit array_base();
    array_base(size_t size, T* ptr);

    pointer mBegin;
    pointer mEnd;
  };

  template<class T> T& array_base<T>::at(size_t n)
  {
    if (n < size())
    {
      return mBegin[n];
    }
    else
    {
      throw std::out_of_range("Fixed container called at() with out-of-bounds index.");
    }
  }

  template<class T> const T& array_base<T>::at(size_t n) const
  {
    if (n < size())
    {
      return mBegin[n];
    }
    else
    {
      throw std::out_of_range("Fixed container called at() with out-of-bounds index.");
    }
  }

  template<class T> T& array_base<T>::back()
  {
    return *(mEnd - 1);
  }

  template<class T> const T& array_base<T>::back() const
  {
    return *(mEnd - 1);
  }

  template<class T> T* array_base<T>::begin()
  {
    return mBegin;
  }

  template<class T> const T* array_base<T>::begin() const
  {
    return mBegin;
  }

  template<class T> const T* array_base<T>::cbegin() const
  {
    return mBegin;
  }

  template<class T> const T* array_base<T>::cend() const
  {
    return mEnd;
  }

  template<class T> typename array_base<T>::const_reverse_iterator array_base<T>::crbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T> typename array_base<T>::const_reverse_iterator array_base<T>::crend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T> T* array_base<T>::data()
  {
    return mBegin;
  }

  template<class T> const T* array_base<T>::data() const
  {
    return mBegin;
  }

  template<class T> bool array_base<T>::empty() const
  {
    return (mBegin == mEnd);
  }

  template<class T> T* array_base<T>::end()
  {
    return mEnd;
  }

  template<class T> const T* array_base<T>::end() const
  {
    return mEnd;
  }

  template<class T> void array_base<T>::fill(const T& v)
  {
    std::fill(mBegin, mEnd, v);
  }

  template<class T> T& array_base<T>::front()
  {
    return *mBegin;
  }

  template<class T> const T& array_base<T>::front() const
  {
    return *mBegin;
  }

  template<class T> size_t array_base<T>::max_size() const
  {
    return mEnd - mBegin;
  }

  template<class T> array_base<T>& array_base<T>::operator=(const array_base<T>& obj)
  {
    if (obj.size() != size())
    {
      throw std::runtime_error("array: assignment operator's parameter size doesn't match");
    }
    std::copy(obj.begin(), obj.end(), mBegin);
    return *this;
  }

  template<class T> T& array_base<T>::operator[](size_t n)
  {
    return mBegin[n];
  }

  template<class T> const T& array_base<T>::operator[](size_t n) const
  {
    return mBegin[n];
  }

  template<class T> typename array_base<T>::reverse_iterator array_base<T>::rbegin()
  {
    return reverse_iterator(mEnd);
  }

  template<class T> typename array_base<T>::const_reverse_iterator array_base<T>::rbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T> typename array_base<T>::reverse_iterator array_base<T>::rend()
  {
    return reverse_iterator(mBegin);
  }

  template<class T> typename array_base<T>::const_reverse_iterator array_base<T>::rend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T> size_t array_base<T>::size() const
  {
    return mEnd - mBegin;
  }

  template<class T> array_base<T>::array_base() :
      mBegin(NULL), mEnd(NULL)
  {
  }

  template<class T> array_base<T>::array_base(size_t size, T* ptr) :
      mBegin(ptr), mEnd(mBegin + size)
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
    using array_base<T>::mBegin;
    using array_base<T>::mEnd;

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
    std::copy(obj.begin(), obj.end(), mBegin);
  }

  template<class T, size_t N> array<T, N>& array<T, N>::operator=(const array<T, N> & obj)
  {
    std::copy(obj.begin(), obj.end(), mBegin);
    return *this;
  }

  template<class T, size_t N> size_t array<T, N>::size() const
  {
    return N;
  }

  template<class T, size_t N> void array<T, N>::swap(array<T, N>& obj)
  {
    std::swap_ranges(mBegin, mEnd, obj.begin());
  }

} //namespace flex

#endif /* FLEX_ARRAY_H */
