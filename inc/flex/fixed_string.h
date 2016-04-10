#ifndef FLEX_FIXED_STRING_H
#define FLEX_FIXED_STRING_H

#include <flex/string.h>

namespace flex
{

  template<size_t N, class Alloc = flex::allocator<char> > class fixed_string: public basic_string<char, Alloc>
  {
  public:
    typedef basic_string<char, Alloc> base_type;
    typedef fixed_string<N, Alloc> this_type;

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
    using base_type::mCapacity;
    using base_type::assign;
    using base_type::npos;

    fixed_string();
    fixed_string(const this_type& x);
    fixed_string(const base_type& obj);
    fixed_string(const this_type& x, size_type position, size_type n = npos);
    fixed_string(const value_type* p);
    fixed_string(const value_type* p, size_type n);
    fixed_string(size_type n, value_type c);
    template<class InputIterator> fixed_string(InputIterator pBegin, InputIterator pEnd);
    fixed_string(std::initializer_list<value_type> init);

    this_type& operator=(const this_type& obj);
    this_type& operator=(const base_type& obj);
    this_type& operator=(std::initializer_list<value_type> obj);
    this_type& operator=(const value_type* p);
    this_type& operator=(const value_type c);

    this_type substr(size_type position = 0, size_type n = npos) const;

  private:
#ifdef FLEX_HAS_CXX11
    typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type mBuffer[N+1];
#else
    union
    {
      char mBuffer[(N + 1) * sizeof(value_type)];
      long double dummy;
    };
#endif
  };

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string() :
      base_type((pointer) mBuffer, (pointer) mBuffer, N + 1)
  {
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string(const this_type& x) :
      base_type((pointer) mBuffer, (pointer) mBuffer + x.size(), N + 1)
  {
    memcpy(mBegin, x.mBegin, x.size() * sizeof(value_type));
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string(const base_type& x) :
      base_type((pointer) mBuffer, (pointer) mBuffer + x.size(), N + 1)
  {
    memcpy(mBegin, x.begin(), x.size() * sizeof(value_type));
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string(const this_type& x, size_type position, size_type n) :
      base_type((pointer) mBuffer, (pointer) mBuffer + ((n > (x.size() - position)) ? (x.size() - position) : n), N + 1)
  {
    memcpy(mBegin, x.mBegin + position, (mEnd - mBegin) * sizeof(value_type));
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string(const value_type* p) :
      base_type((pointer) mBuffer, (pointer) mBuffer + strlen(p), N + 1)
  {
    memcpy(mBegin, p, (mEnd - mBegin) * sizeof(value_type));
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string(const value_type* p, size_type n) :
      base_type((pointer) mBuffer, (pointer) mBuffer + n, N + 1)
  {
    memcpy(mBegin, p, n * sizeof(value_type));
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string(size_type n, value_type c) :
      base_type((pointer) mBuffer, (pointer) mBuffer + n, N + 1)
  {
    if (n)
    {
      memset(mBegin, c, n);
    }
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  template<class InputIterator>
  inline fixed_string<N, Alloc>::fixed_string(InputIterator pBegin, InputIterator pEnd) :
      base_type((pointer) mBuffer, (pointer) mBuffer + std::distance(pBegin, pEnd), N + 1)
  {
    memcpy(mBegin, pBegin, (mEnd - mBegin) * sizeof(value_type));
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>::fixed_string(std::initializer_list<value_type> init) :
      base_type((pointer) mBuffer, (pointer) mBuffer + init.size(), N + 1)
  {
    memcpy(mBegin, init.begin(), init.size() * sizeof(value_type));
    *mEnd = 0;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>& fixed_string<N, Alloc>::operator=(const this_type& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>& fixed_string<N, Alloc>::operator=(const base_type& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>& fixed_string<N, Alloc>::operator=(std::initializer_list<value_type> obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>& fixed_string<N, Alloc>::operator=(const value_type* p)
  {
    assign(p);
    return *this;
  }

  template<size_t N, class Alloc>
  inline fixed_string<N, Alloc>& fixed_string<N, Alloc>::operator=(const value_type c)
  {
    assign((size_type) 1, c);
    return *this;
  }

  template<size_t N, typename Alloc>
  inline fixed_string<N, Alloc> fixed_string<N, Alloc>::substr(size_type position, size_type n) const
  {
    FLEX_THROW_OUT_OF_RANGE_IF(position > (size_type )(mEnd - mBegin), "fixed_string -- out of range");
    return this_type(mBegin + position, mBegin + position + std::min(n, (size_type) (mEnd - mBegin) - position));
  }

} //namespace flex

#endif /* FLEX_FIXED_STRING_H */
