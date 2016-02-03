#ifndef FIXED_ARRAY_H 
#define FIXED_ARRAY_H

#include <fixed_array_base.h>

template<class T, size_t N = 0, class Alloc = flex::allocator<T> > class fixed_array: public fixed_array_base<T, Alloc>
{
public:
  fixed_array();
  fixed_array(const fixed_array<T, N, Alloc> & obj);
  fixed_array<T, N, Alloc>& operator=(const fixed_array<T, N, Alloc>& obj);
  fixed_array<T, N, Alloc>& operator=(const fixed_array<T, 0, Alloc>& obj);
  operator const fixed_array<T,0>&() const;
  operator fixed_array<T,0>&();
  size_t size() const;
private:
  T mAry[N];
};

template<class T, size_t N, class Alloc> fixed_array<T, N, Alloc>::fixed_array() :
    fixed_array_base<T>(N, mAry)
{
}

template<class T, size_t N, class Alloc> fixed_array<T, N, Alloc>::fixed_array(const fixed_array<T, N, Alloc> & obj) :
    fixed_array_base<T>(obj.size(), mAry)
{
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T, size_t N, class Alloc> fixed_array<T, N, Alloc>& fixed_array<T, N, Alloc>::operator=(
    const fixed_array<T, N, Alloc> & obj)
{
  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }
  return *this;
}

template<class T, size_t N, class Alloc> fixed_array<T, N, Alloc>& fixed_array<T, N, Alloc>::operator=(
    const fixed_array<T, 0, Alloc> & obj)
{
  fixed_array_base<T>::operator=(obj);
  return *this;
}

template<class T, size_t N, class Alloc> fixed_array<T, N, Alloc>::operator const fixed_array<T,0>&() const
{
  return *((fixed_array<T, 0, Alloc>*) this);
}

template<class T, size_t N, class Alloc> fixed_array<T, N, Alloc>::operator fixed_array<T,0>&()
{
  return *((fixed_array<T, 0, Alloc>*) this);
}

template<class T, size_t N, class Alloc> size_t fixed_array<T, N, Alloc>::size() const
{
  return N;
}

//TODO:  Decide what to do with the specialized Array class, as we probably don't want it to be expandable.  Remove it?
template<class T, class Alloc> class fixed_array<T, 0, Alloc> : public fixed_array_base<T, Alloc>
{
public:
  fixed_array(size_t size);
  fixed_array(const fixed_array<T> & obj);
  ~fixed_array();
  fixed_array<T, 0, Alloc>& operator=(const fixed_array<T, 0, Alloc> & obj);
private:
  void allocate();
};

template<class T, class Alloc> fixed_array<T, 0, Alloc>::fixed_array(size_t size) :
    fixed_array_base<T>(size)
{
  allocate();
}

template<class T, class Alloc> fixed_array<T, 0, Alloc>::fixed_array(const fixed_array<T> & obj) :
    fixed_array_base<T>(obj.size())
{
  allocate();
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T, class Alloc> fixed_array<T, 0, Alloc>::~fixed_array()
{
  Alloc a;
  a.deallocate(this->mAryPtr, this->mSize);
}

template<class T, class Alloc> fixed_array<T, 0, Alloc>& fixed_array<T, 0, Alloc>::operator=(
    const fixed_array<T, 0, Alloc> & obj)
{
  fixed_array_base<T>::operator=(obj);
  return *this;
}

template<class T, class Alloc> void fixed_array<T, 0, Alloc>::allocate()
{
  Alloc a;
  this->mAryPtr = a.allocate(this->mSize);
}

#endif /* FIXED_ARRAY_H */
