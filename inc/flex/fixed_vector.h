#ifndef FLEX_FIXED_VECTOR_H
#define FLEX_FIXED_VECTOR_H

#include <flex/vector.h>

namespace flex
{

  template<class T, size_t N, class Alloc = allocator<T> > class fixed_vector: public vector<T, Alloc>
  {
  public:
      typedef vector<T, Alloc> base_type;
    
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
    using base_type::capacity;

    fixed_vector();
    explicit fixed_vector(size_type size, const value_type& val = value_type());
    fixed_vector(const T* first, const T* last);
    fixed_vector(const fixed_vector<T, N, Alloc> & obj);
    fixed_vector(const vector<T, Alloc> & obj);
    fixed_vector<T, N, Alloc>& operator=(const fixed_vector<T, N, Alloc>& obj);
    fixed_vector<T, N, Alloc>& operator=(const vector<T, Alloc>& obj);
    size_type max_size() const;
  private:
    T mAry[N];
  };

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector() :
      vector<T, Alloc>(N, mAry)
  {
    mEnd = mBegin;
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(size_type size, const value_type& val) :
      vector<T, Alloc>(N, mAry)
  {
   assign(size, val);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(const T* first, const T* last) :
      vector<T, Alloc>(N, mAry)
  {
    assign(first, last);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(const fixed_vector<T, N, Alloc> & obj) :
      vector<T, Alloc>(N, mAry)
  {
     std::copy(obj.mBegin, obj.mEnd, mBegin);
    mEnd = mBegin + obj.size();
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(const vector<T, Alloc> & obj) :
      vector<T, Alloc>(N, mAry)
  {
    assign(obj.mBegin, obj.mEnd);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(
      const fixed_vector<T, N, Alloc>& obj)
  {
    std::copy(obj.mBegin, obj.mEnd, mBegin);
    mEnd = mBegin + obj.size();
    return *this;
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(
      const vector<T, Alloc>& obj)
  {
    assign(obj.mBegin,obj.mEnd);
    return *this;
  }

  template<class T, size_t N, class Alloc>
  inline typename fixed_vector<T, N, Alloc>::size_type fixed_vector<T, N, Alloc>::max_size() const
  {
    return capacity();
  }

} //namespace flex

#endif /* FLEX_FIXED_VECTOR_H */
