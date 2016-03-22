#ifndef FLEX_FIXED_VECTOR_H
#define FLEX_FIXED_VECTOR_H

#include <flex/vector.h>

namespace flex
{

   //TODO: Put this in the proper place.  Maybe fixed pool.
   template<size_t N>
   struct aligned_buffer
   {
      char mBuffer[N];
   };
   
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

    fixed_vector();
    explicit fixed_vector(size_type size, const value_type& val = value_type());
    fixed_vector(int size, const value_type& val);
    template<typename InputIterator> fixed_vector(InputIterator first, InputIterator last);
    fixed_vector(const fixed_vector<T, N, Alloc> & obj);
    fixed_vector(const vector<T, Alloc> & obj);

    fixed_vector<T, N, Alloc>& operator=(const fixed_vector<T, N, Alloc>& obj);
    fixed_vector<T, N, Alloc>& operator=(const vector<T, Alloc>& obj);
    size_type max_size() const;

  private:
    aligned_buffer<N * sizeof(T)> mBuffer;
  };

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector() :
      vector<T, Alloc>((pointer)(&mBuffer), (pointer)(&mBuffer), N)
  {
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(size_type size, const value_type& val) :
      vector<T, Alloc>((pointer)(&mBuffer), (pointer)(&mBuffer) + size, N)
  {
    std::fill(mBegin, mEnd, val);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(int size, const value_type& val) :
      vector<T, Alloc>((pointer)(&mBuffer), (pointer)(&mBuffer) + size, N)
  {
    std::fill(mBegin, mEnd, val);
  }

  template<class T, size_t N, class Alloc>
  template<typename InputIterator>
  inline fixed_vector<T, N, Alloc>::fixed_vector(InputIterator first, InputIterator last) :
      vector<T, Alloc>((pointer)(&mBuffer), (pointer)(&mBuffer) + std::distance(first, last), N)
  {
    std::copy(first, last, mBegin);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(const fixed_vector<T, N, Alloc> & obj) :
      vector<T, Alloc>((pointer)(&mBuffer), (pointer)(&mBuffer) + std::distance(obj.mBegin, obj.mEnd), N)
  {
    std::copy(obj.mBegin, obj.mEnd, mBegin);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>::fixed_vector(const vector<T, Alloc> & obj) :
      vector<T, Alloc>((pointer)(&mBuffer), (pointer)(&mBuffer) + std::distance(obj.mBegin, obj.mEnd), N)
  {
    std::copy(obj.mBegin, obj.mEnd, mBegin);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(const fixed_vector<T, N, Alloc>& obj)
  {
    //A bit more efficient than assign() as no capacity check is needed.
    std::copy(obj.mBegin, obj.mEnd, mBegin);
    mEnd = mBegin + obj.size();
    return *this;
  }

  template<class T, size_t N, class Alloc>
  inline fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(const vector<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<class T, size_t N, class Alloc>
  inline typename fixed_vector<T, N, Alloc>::size_type fixed_vector<T, N, Alloc>::max_size() const
  {
    return N;
  }

} //namespace flex

#endif /* FLEX_FIXED_VECTOR_H */
