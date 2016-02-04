#ifndef FIXED_VECTOR_H
#define FIXED_VECTOR_H

#include <fixed_vector_base.h>

template<class T, size_t N = 0, class Alloc = flex::allocator<T> > class fixed_vector: public fixed_vector_base<T, Alloc>
{
public:
  fixed_vector();
  fixed_vector(size_t size, const T& val = T());
  fixed_vector(const T* first, const T* last);
  fixed_vector(const fixed_vector<T, 0, Alloc> & obj);
  fixed_vector<T, N, Alloc>& operator=(const fixed_vector<T, N, Alloc>& obj);
  fixed_vector<T, N, Alloc>& operator=(const fixed_vector<T, 0, Alloc>& obj);
  operator const fixed_vector<T,0,Alloc>&() const;
  operator fixed_vector<T,0,Alloc>&();
private:
  T mAry[N];
};

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector() :
    fixed_vector_base<T, Alloc>(N, mAry)
{
}

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector(size_t size, const T& val) :
    fixed_vector_base<T, Alloc>(N, size, mAry)
{
  fixed_array_base<T>::fill(val);
}

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector(const T* first, const T* last) :
    fixed_vector_base<T, Alloc>(N, mAry)
{
  this->mSize = 0;
  for (const T* it = first; it != last; ++it)
  {
    this->mAryPtr[this->mSize] = *it;
    ++this->mSize;
  }
}

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::fixed_vector(const fixed_vector<T, 0, Alloc> & obj) :
    fixed_vector_base<T, Alloc>(N, mAry)
{
  *this = obj;
}

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(
    const fixed_vector<T, N, Alloc>& obj)
{
  this->mSize = obj.size();
  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }
  return *this;
}

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>& fixed_vector<T, N, Alloc>::operator=(
    const fixed_vector<T, 0, Alloc>& obj)
{
  fixed_vector_base<T, Alloc>::operator=(obj);
  return *this;
}

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::operator const fixed_vector<T, 0, Alloc>&() const
{
  return *((fixed_vector<T, 0, Alloc>*) this);
}

template<class T, size_t N, class Alloc> fixed_vector<T, N, Alloc>::operator fixed_vector<T, 0, Alloc>&()
{
  return *((fixed_vector<T, 0, Alloc>*) this);
}

template<class T, class Alloc> class fixed_vector<T, 0, Alloc> : public fixed_vector_base<T, Alloc>
{
public:
  fixed_vector();
  fixed_vector(size_t size, const T& val = T());
  fixed_vector(const T* first, const T* last);
  fixed_vector(const fixed_vector<T, 0, Alloc> & obj);
  ~fixed_vector();
  fixed_vector<T, 0, Alloc>& operator=(const fixed_vector<T, 0, Alloc>& obj);

private:
  void allocate();
};

template<class T, class Alloc> fixed_vector<T, 0, Alloc>::fixed_vector() :
    fixed_vector_base<T, Alloc>(0)
{
}

template<class T, class Alloc> fixed_vector<T, 0, Alloc>::fixed_vector(size_t size, const T& val) :
    fixed_vector_base<T, Alloc>(size,size)
{
  allocate();
  fixed_array_base<T>::fill(val);
}

template<class T, class Alloc> fixed_vector<T, 0, Alloc>::fixed_vector(const T* first, const T* last) :
    fixed_vector_base<T, Alloc>(last-first,last-first)
{
  allocate();
  T* lit = this->mAryPtr;
  for (const T* it = first; it != last; ++it)
  {
    *lit++ = *it;
  }
}

template<class T, class Alloc> fixed_vector<T, 0, Alloc>::fixed_vector(const fixed_vector<T, 0, Alloc> & obj) :
    fixed_vector_base<T, Alloc>(obj.capacity())
{
  allocate();
  *this = obj;
}

template<class T, class Alloc> fixed_vector<T, 0, Alloc>::~fixed_vector()
{
  if (!this->mFixed && (NULL != this->mAryPtr))
  {
    Alloc alloc;
    alloc.deallocate(this->mAryPtr, this->mCapacity);
  }
}

template<class T, class Alloc> fixed_vector<T, 0, Alloc>& fixed_vector<T, 0, Alloc>::operator=(
    const fixed_vector<T, 0, Alloc>& obj)
{
  fixed_vector_base<T, Alloc>::operator=(obj);
  return *this;
}

template<class T, class Alloc> void fixed_vector<T, 0, Alloc>::allocate()
{
  Alloc alloc;
  this->mAryPtr = alloc.allocate(this->mCapacity);
}

#endif /* FIXED_VECTOR_H */
