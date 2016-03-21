#ifndef FLEX_FIXED_RING_H
#define FLEX_FIXED_RING_H

#include <flex/ring.h>

namespace flex
{

  template<class T, size_t N, class Alloc = allocator<T> > class fixed_ring: public ring<T, Alloc>
  {
  public:
    typedef ring<T, Alloc> base_type;

    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::difference_type difference_type;

    using base_type::mBegin;
    using base_type::mEnd;
    using base_type::assign;

    fixed_ring();
    explicit fixed_ring(size_type size, const value_type& val = value_type());
    fixed_ring(int size, const value_type& val);
    template<typename InputIterator> fixed_ring(InputIterator first, InputIterator last);
    fixed_ring(const fixed_ring<T, N, Alloc> & obj);
    fixed_ring(const ring<T, Alloc> & obj);

    fixed_ring<T, N, Alloc>& operator=(const fixed_ring<T, N, Alloc>& obj);
    fixed_ring<T, N, Alloc>& operator=(const ring<T, Alloc>& obj);

  private:
    T mBuffer[N + 1]; //Need to add one as a full ring buffer has one element that is unused.
  };

  template<class T, size_t N, class Alloc>
  inline fixed_ring<T, N, Alloc>::fixed_ring() :
      ring<T, Alloc>(mBuffer, mBuffer, mBuffer + N)
  {
  }

  template<class T, size_t N, class Alloc>
  inline fixed_ring<T, N, Alloc>::fixed_ring(size_type size, const value_type& val) :
      ring<T, Alloc>(mBuffer, mBuffer + size, mBuffer + N)
  {
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_ring<T, N, Alloc>::fixed_ring(int size, const value_type& val) :
      ring<T, Alloc>(mBuffer, mBuffer + size, mBuffer + N)
  {
    std::fill(mBegin.mPtr, mEnd.mPtr, val);
  }

  template<class T, size_t N, class Alloc>
  template<typename InputIterator>
  inline fixed_ring<T, N, Alloc>::fixed_ring(InputIterator first, InputIterator last) :
      ring<T, Alloc>(mBuffer, mBuffer + std::distance(first, last), mBuffer + N)
  {
    std::copy(first, last, mBegin.mPtr);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_ring<T, N, Alloc>::fixed_ring(const fixed_ring<T, N, Alloc> & obj) :
      ring<T, Alloc>(mBuffer, mBuffer + std::distance(obj.mBegin, obj.mEnd), mBuffer + N)
  {
    std::copy(obj.mBegin, obj.mEnd, mBegin.mPtr);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_ring<T, N, Alloc>::fixed_ring(const ring<T, Alloc> & obj) :
      ring<T, Alloc>(mBuffer, mBuffer + std::distance(obj.mBegin, obj.mEnd), mBuffer + N)
  {
    std::copy(obj.mBegin, obj.mEnd, mBegin.mPtr);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_ring<T, N, Alloc>& fixed_ring<T, N, Alloc>::operator=(const fixed_ring<T, N, Alloc>& obj)
  {
    //A bit more efficient than assign() as no capacity check is needed.
    std::copy(obj.mBegin, obj.mEnd, mBegin);
    mEnd = mBegin + obj.size();
    return *this;
  }

  template<class T, size_t N, class Alloc>
  inline fixed_ring<T, N, Alloc>& fixed_ring<T, N, Alloc>::operator=(const ring<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

} //namespace flex

#endif /* FLEX_FIXED_RING_H */
