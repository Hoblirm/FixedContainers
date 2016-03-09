#ifndef FLEX_FIXED_VECTOR_H
#define FLEX_FIXED_VECTOR_H

#include <flex/vector.h>

namespace flex
{

  template<class T, size_t N, class Alloc = allocator<T> > class fixed_vector: public vector<T, Alloc>
  {
  public:
    using vector<T, Alloc>::mBegin;
    using vector<T, Alloc>::mEnd;
    using vector<T, Alloc>::mCapacity;
    using vector<T, Alloc>::capacity;

    fixed_vector();
    explicit fixed_vector(size_t size, const T& val = T());
    fixed_vector(const T* first, const T* last);
    fixed_vector(const fixed_vector<T, N, Alloc> & obj);
    fixed_vector(const vector<T, Alloc> & obj);
    fixed_vector<T, N, Alloc>& operator=(const fixed_vector<T, N, Alloc>& obj);
    fixed_vector<T, N, Alloc>& operator=(const vector<T, Alloc>& obj);
    size_t max_size() const;
  private:
    T mAry[N];
  };

  template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector() :
      vector<T, Alloc>(N, 0, mAry)
  {
  }

  template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector(size_t size, const T& val) :
      vector<T, Alloc>(N, mAry)
  {
    //TODO: Write test cases when capacity and size don't match for constructor
    vector<T, Alloc>::assign(size, val);
  }

  template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector(const T* first, const T* last) :
      vector<T, Alloc>(N, mAry)
  {
    //TODO: Write test cases when capacity and size don't match for constructor
    vector<T, Alloc>::assign(first, last);
  }

  template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector(
      const fixed_vector<T, N, Alloc> & obj) :
      vector<T, Alloc>(N, mAry)
  {
    //TODO: Write test cases when capacity and size don't match for constructor
    vector<T, Alloc>::assign(obj.begin(), obj.end());
  }

  template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector(const vector<T, Alloc> & obj) :
      vector<T, Alloc>(N, mAry)
  {
    //TODO: Write test cases when capacity and size don't match for constructor
    vector<T, Alloc>::assign(obj.begin(), obj.end());
  }

  template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(
      const fixed_vector<T, N, Alloc>& obj)
  {
    std::copy(obj.begin(), obj.end(), mBegin);
    mEnd = mBegin + obj.size();
    return *this;
  }

  template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(
      const vector<T, Alloc>& obj)
  {
    vector<T, Alloc>::operator=(obj);
    return *this;
  }

  template<class T, size_t N, class Alloc> size_t fixed_vector<T, N, Alloc>::max_size() const
  {
    return capacity();
  }

} //namespace flex

#endif /* FLEX_FIXED_VECTOR_H */
