#ifndef FLEX_ARRAY_H
#define FLEX_ARRAY_H

#include <algorithm>
#include <iterator>

#include <flex/allocator.h>

namespace flex
{
  template<class T, class Alloc = allocator<T> > class array_base: public allocation_guard
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

    explicit array_base(size_type n);

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

    array_base<T, Alloc>& operator=(const array_base<T, Alloc>& obj);
    reference operator[](size_t n);
    const_reference operator[](size_t n) const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

    size_t size() const;

  protected:
    array_base();
    array_base(T* ptr);
    array_base(T* ptr,size_type size);

    T* AllocateAndConstruct(size_t n);

    Alloc mAllocator;
    pointer mBegin;
    pointer mEnd;
  };

  template<class T, class Alloc>
  inline array_base<T, Alloc>::array_base(size_type n)
  {
    mBegin = mAllocator.allocate(n);
    mEnd = mBegin + n;
    for (T* it = mBegin; it != mEnd; ++it)
    {
      mAllocator.construct(it, value_type());
    }
  }

  template<class T, class Alloc> T& array_base<T, Alloc>::at(size_t n)
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

  template<class T, class Alloc> const T& array_base<T, Alloc>::at(size_t n) const
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

  template<class T, class Alloc> T& array_base<T, Alloc>::back()
  {
    return *(mEnd - 1);
  }

  template<class T, class Alloc> const T& array_base<T, Alloc>::back() const
  {
    return *(mEnd - 1);
  }

  template<class T, class Alloc> T* array_base<T, Alloc>::begin()
  {
    return mBegin;
  }

  template<class T, class Alloc> const T* array_base<T, Alloc>::begin() const
  {
    return mBegin;
  }

  template<class T, class Alloc> const T* array_base<T, Alloc>::cbegin() const
  {
    return mBegin;
  }

  template<class T, class Alloc> const T* array_base<T, Alloc>::cend() const
  {
    return mEnd;
  }

  template<class T, class Alloc> typename array_base<T, Alloc>::const_reverse_iterator array_base<T, Alloc>::crbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T, class Alloc> typename array_base<T, Alloc>::const_reverse_iterator array_base<T, Alloc>::crend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T, class Alloc> T* array_base<T, Alloc>::data()
  {
    return mBegin;
  }

  template<class T, class Alloc> const T* array_base<T, Alloc>::data() const
  {
    return mBegin;
  }

  template<class T, class Alloc> bool array_base<T, Alloc>::empty() const
  {
    return (mBegin == mEnd);
  }

  template<class T, class Alloc> T* array_base<T, Alloc>::end()
  {
    return mEnd;
  }

  template<class T, class Alloc> const T* array_base<T, Alloc>::end() const
  {
    return mEnd;
  }

  template<class T, class Alloc> void array_base<T, Alloc>::fill(const T& v)
  {
    std::fill(mBegin, mEnd, v);
  }

  template<class T, class Alloc> T& array_base<T, Alloc>::front()
  {
    return *mBegin;
  }

  template<class T, class Alloc> const T& array_base<T, Alloc>::front() const
  {
    return *mBegin;
  }

  template<class T, class Alloc> size_t array_base<T, Alloc>::max_size() const
  {
    return mEnd - mBegin;
  }

  template<class T, class Alloc> array_base<T, Alloc>& array_base<T, Alloc>::operator=(const array_base<T, Alloc>& obj)
  {
    if (obj.size() != size())
    {
      throw std::runtime_error("array: assignment operator's parameter size doesn't match");
    }
    std::copy(obj.begin(), obj.end(), mBegin);
    return *this;
  }

  template<class T, class Alloc> T& array_base<T, Alloc>::operator[](size_t n)
  {
    return mBegin[n];
  }

  template<class T, class Alloc> const T& array_base<T, Alloc>::operator[](size_t n) const
  {
    return mBegin[n];
  }

  template<class T, class Alloc> typename array_base<T, Alloc>::reverse_iterator array_base<T, Alloc>::rbegin()
  {
    return reverse_iterator(mEnd);
  }

  template<class T, class Alloc> typename array_base<T, Alloc>::const_reverse_iterator array_base<T, Alloc>::rbegin() const
  {
    return const_reverse_iterator(mEnd);
  }

  template<class T, class Alloc> typename array_base<T, Alloc>::reverse_iterator array_base<T, Alloc>::rend()
  {
    return reverse_iterator(mBegin);
  }

  template<class T, class Alloc> typename array_base<T, Alloc>::const_reverse_iterator array_base<T, Alloc>::rend() const
  {
    return const_reverse_iterator(mBegin);
  }

  template<class T, class Alloc> size_t array_base<T, Alloc>::size() const
  {
    return mEnd - mBegin;
  }

  template<class T, class Alloc> array_base<T, Alloc>::array_base() :
      mBegin(NULL), mEnd(NULL)
  {
  }

  /*
   * This is the protected constructor used by vectors.  The vector sets the
   * size internally, so this constructor doesn't need to set it.
   */
  template<class T, class Alloc> array_base<T, Alloc>::array_base(T* ptr) :
      mBegin(ptr)
  {
  }

  template<class T, class Alloc> array_base<T, Alloc>::array_base(T* ptr,size_type size) :
      mBegin(ptr), mEnd(ptr + size)
  {
  }

  template<class T, class Alloc> T* array_base<T, Alloc>::AllocateAndConstruct(size_t n)
  {
    iterator new_begin = mAllocator.allocate(n);
    for (T* it = new_begin; it != (new_begin + n); ++it)
    {
      mAllocator.construct(it, value_type());
    }
    return new_begin;
  }

  template<class T, class Alloc>
  bool operator==(const array_base<T, Alloc>& lhs, const array_base<T, Alloc>& rhs)
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
  bool operator<(const array_base<T, Alloc>& lhs, const array_base<T, Alloc>& rhs)
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
  bool operator!=(const array_base<T, Alloc>& lhs, const array_base<T, Alloc>& rhs)
  {
    return !(lhs == rhs);
  }

  template<class T, class Alloc>
  bool operator>(const array_base<T, Alloc>& lhs, const array_base<T, Alloc>& rhs)
  {
    return rhs < lhs;
  }

  template<class T, class Alloc>
  bool operator<=(const array_base<T, Alloc>& lhs, const array_base<T, Alloc>& rhs)
  {
    return !(rhs < lhs);
  }

  template<class T, class Alloc>
  bool operator>=(const array_base<T, Alloc>& lhs, const array_base<T, Alloc>& rhs)
  {
    return !(lhs < rhs);
  }

  template<class T, size_t N = 0, class Alloc = allocator<T> > class array: public array_base<T, Alloc>
  {
  public:
    using array_base<T, Alloc>::mBegin;
    using array_base<T, Alloc>::mEnd;

    array();
    array(const array<T, N, Alloc> & obj);
    array<T, N, Alloc>& operator=(const array<T, N, Alloc>& obj);
    size_t size() const;
    void swap(array<T, N, Alloc>& obj);
  private:
    T mAry[N];
  };

  template<class T, size_t N, class Alloc> array<T, N, Alloc>::array() :
      array_base<T, Alloc>(mAry,N)
  {
  }

  template<class T, size_t N, class Alloc> array<T, N, Alloc>::array(const array<T, N, Alloc> & obj) :
      array_base<T, Alloc>(mAry,N)
  {
    std::copy(obj.begin(), obj.end(), mBegin);
  }

  template<class T, size_t N, class Alloc> array<T, N, Alloc>& array<T, N, Alloc>::operator=(
      const array<T, N, Alloc> & obj)
  {
    std::copy(obj.begin(), obj.end(), mBegin);
    return *this;
  }

  template<class T, size_t N, class Alloc> size_t array<T, N, Alloc>::size() const
  {
    return N;
  }

  template<class T, size_t N, class Alloc> void array<T, N, Alloc>::swap(array<T, N, Alloc>& obj)
  {
    std::swap_ranges(mBegin, mEnd, obj.begin());
  }

} //namespace flex

#endif /* FLEX_ARRAY_H */
