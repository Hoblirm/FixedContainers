#ifndef FLEX_RING_H
#define FLEX_RING_H

#include <algorithm>

#include <flex/allocator.h>
#include <flex/array_iterator.h>

namespace flex
{
  template<class T, class Alloc = allocator<T> > class ring: public allocation_guard
  {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* iterator;
    typedef const T* const_iterator;
//    typedef array_reverse_iterator<T, Alloc> reverse_iterator;
//    typedef array_const_reverse_iterator<T, Alloc> const_reverse_iterator;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef Alloc allocator_type;

    ring();
//    explicit ring(size_type size, const value_type& val = value_type());

    reference at(size_t n);
    const_reference at(size_t n) const;

    reference back();
    const_reference back() const;

    iterator begin();
    const_iterator begin() const;
//    const_iterator cbegin() const;
//    const_iterator cend() const;
//    const_reverse_iterator crbegin() const;
//    const_reverse_iterator crend() const;
//

    bool empty() const;

    iterator end();
    const_iterator end() const;

    reference front();
    const_reference front() const;

    size_t max_size() const;

    ring<T, Alloc>& operator=(const ring<T, Alloc>& obj);
    reference operator[](size_t n);
    const_reference operator[](size_t n) const;

    void pop_back();
    void pop_front();
    void push_back(const value_type& val);
    void push_front(const value_type& val);

//    reverse_iterator rbegin();
//    const_reverse_iterator rbegin() const;
//    reverse_iterator rend();
//    const_reverse_iterator rend() const;

    size_t size() const;

  protected:
    ring(size_t size, T* ptr);

    T* mLeftBound;
    T* mRightBound;
    T* mBegin;
    T* mEnd;
  };

  template<class T, class Alloc> T& ring<T, Alloc>::at(size_t n)
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

  template<class T, class Alloc> const T& ring<T, Alloc>::at(size_t n) const
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

  template<class T, class Alloc> T& ring<T, Alloc>::back()
  {
    return mAryPtr[mSize - 1];
  }

  template<class T, class Alloc> const T& ring<T, Alloc>::back() const
  {
    return mAryPtr[mSize - 1];
  }

  template<class T, class Alloc> T* ring<T, Alloc>::begin()
  {
    return mAryPtr;
  }

  template<class T, class Alloc> const T* ring<T, Alloc>::begin() const
  {
    return mAryPtr;
  }

//  template<class T, class Alloc> const T* ring<T, Alloc>::cbegin() const
//  {
//    return mAryPtr;
//  }
//
//  template<class T, class Alloc> const T* ring<T, Alloc>::cend() const
//  {
//    return &mAryPtr[mSize];
//  }

//  template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::crbegin() const
//  {
//    return array_const_reverse_iterator<T, Alloc>(&mAryPtr[mSize - 1]);
//  }
//
//  template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::crend() const
//  {
//    return array_const_reverse_iterator<T, Alloc>(&mAryPtr[-1]);
//  }

  template<class T, class Alloc> bool ring<T, Alloc>::empty() const
  {
    return (0 == mSize);
  }

  template<class T, class Alloc> T* ring<T, Alloc>::end()
  {
    return &mAryPtr[mSize];
  }

  template<class T, class Alloc> const T* ring<T, Alloc>::end() const
  {
    return &mAryPtr[mSize];
  }

  template<class T, class Alloc> T& ring<T, Alloc>::front()
  {
    return mAryPtr[0];
  }

  template<class T, class Alloc> const T& ring<T, Alloc>::front() const
  {
    return mAryPtr[0];
  }

  template<class T, class Alloc> size_t ring<T, Alloc>::max_size() const
  {
    return mSize;
  }

  template<class T, class Alloc> ring<T, Alloc>& ring<T, Alloc>::operator=(const ring<T, Alloc>& obj)
  {
    if (obj.size() != mSize)
    {
      throw std::runtime_error("array: assignment operator's parameter size doesn't match");
    }
    std::copy(obj.begin(), obj.end(), mAryPtr);
    return *this;
  }

  template<class T, class Alloc> T& ring<T, Alloc>::operator[](size_t n)
  {
    return mAryPtr[n];
  }

  template<class T, class Alloc> const T& ring<T, Alloc>::operator[](size_t n) const
  {
    return mAryPtr[n];
  }

  template<class T, class Alloc> void ring<T, Alloc>::pop_back()
    {
      //--mSize;
    }

    template<class T, class Alloc> void ring<T, Alloc>::pop_front()
    {
      //--mSize;
    }

    template<class T, class Alloc> void ring<T, Alloc>::push_back(const T& val)
    {
      /*
       if (mSize >= mCapacity)
       {
       if (mFixed)
       {
       throw std::runtime_error("flex::ring - push_back() caused size to exceed capacity");
       }
       else
       {
       //Allocate memory with sufficient capacity.
       size_t new_capacity = GetNewCapacity(mSize + 1);
       T* new_begin = DoAllocateAndConstruct(new_capacity);

       //Copy all values.
       T* new_end = std::copy(mAryPtr, mAryPtr + mSize, new_begin);
       *new_end = val;

       //Deallocate and reassign.
       DoDestroyAndDeallocate();
       mAryPtr = new_begin;
       mCapacity = new_capacity;
       }
       }
       else
       {
       mAryPtr[mSize] = val;
       }
       ++mSize;*/
    }

    template<class T, class Alloc> void ring<T, Alloc>::push_front(const T& val)
    {

    }

    template<class T, size_t N> size_t array<T, N>::size() const
    {
      return N;
    }
  /*
  template<class T, class Alloc> array_reverse_iterator<T, Alloc> ring<T, Alloc>::rbegin()
  {
    return array_reverse_iterator<T, Alloc>(&mAryPtr[mSize - 1]);
  }

  template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::rbegin() const
  {
    return array_const_reverse_iterator<T, Alloc>(&mAryPtr[mSize - 1]);
  }

  template<class T, class Alloc> array_reverse_iterator<T, Alloc> ring<T, Alloc>::rend()
  {
    return array_reverse_iterator<T, Alloc>(&mAryPtr[-1]);
  }

  template<class T, class Alloc> array_const_reverse_iterator<T, Alloc> ring<T, Alloc>::rend() const
  {
    return array_const_reverse_iterator<T, Alloc>(&mAryPtr[-1]);
  }
*/
  template<class T, class Alloc> size_t ring<T, Alloc>::size() const
  {
    return mSize;
  }

  template<class T, class Alloc> ring<T, Alloc>::ring(size_t size, T* ptr)
  {
  }

  template<class T, class Alloc>
  bool operator==(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
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
  bool operator<(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
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
  bool operator!=(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return !(lhs == rhs);
  }

  template<class T, class Alloc>
  bool operator>(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return rhs < lhs;
  }

  template<class T, class Alloc>
  bool operator<=(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return !(rhs < lhs);
  }

  template<class T, class Alloc>
  bool operator>=(const ring<T, Alloc>& lhs, const ring<T, Alloc>& rhs)
  {
    return !(lhs < rhs);
  }


} //namespace flex

#endif /* FLEX_RING_H */
