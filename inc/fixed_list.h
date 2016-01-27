#ifndef FIXED_LIST_H
#define FIXED_LIST_H

#include <fixed_list_base.h>

template<class T, size_t N = 0> class fixed_list: public fixed_list_base<T>
{
public:
  fixed_list();
  fixed_list(const fixed_list<T, N> & obj);
  fixed_list<T, N>& operator=(const fixed_list<T, N>& obj);
  fixed_list<T, N>& operator=(const fixed_list<T, 0>& obj);
  operator const fixed_list<T,0>&() const;
  operator fixed_list<T,0>&();
private:
  T mAry[N];
};

template<class T, size_t N> fixed_list<T, N>::fixed_list() :
    fixed_list_base<T>(N, mAry)
{
}

template<class T, size_t N> fixed_list<T, N>::fixed_list(const fixed_list<T, N> & obj) :
    fixed_list_base<T>(obj.size(), mAry)
{
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T, size_t N> fixed_list<T, N>& fixed_list<T, N>::operator=(const fixed_list<T, N> & obj)
{
  for (int i = 0; i < obj.size(); i++)
  {
    this->mAryPtr[i] = obj[i];
  }
  return *this;
}

template<class T, size_t N> fixed_list<T, N>& fixed_list<T, N>::operator=(const fixed_list<T, 0> & obj)
{
  fixed_list_base<T>::operator=(obj);
  return *this;
}

template<class T, size_t N> fixed_list<T, N>::operator const fixed_list<T,0>&() const
{
  return *((fixed_list<T, 0>*) this);
}

template<class T, size_t N> fixed_list<T, N>::operator fixed_list<T,0>&()
{
  return *((fixed_list<T, 0>*) this);
}


template<class T> class fixed_list<T, 0> : public fixed_list_base<T>
{
public:
  fixed_list(size_t size);
  fixed_list(const fixed_list<T> & obj);
  ~fixed_list();
  fixed_list<T, 0>& operator=(const fixed_list<T, 0> & obj);
private:
  void allocate();
};

template<class T> fixed_list<T, 0>::fixed_list(size_t size) :
    fixed_list_base<T>(size)
{
  allocate();
}

template<class T> fixed_list<T, 0>::fixed_list(const fixed_list<T> & obj) :
    fixed_list_base<T>(obj.size())
{
  allocate();
  for (int i = 0; i < this->mSize; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<class T> fixed_list<T, 0>::~fixed_list()
{
  delete[] this->mAryPtr;
}

template<class T> fixed_list<T, 0>& fixed_list<T, 0>::operator=(const fixed_list<T, 0> & obj)
{
  fixed_list_base<T>::operator=(obj);
  return *this;
}

template<class T> void fixed_list<T, 0>::allocate()
{
  if (allocation_guard::is_enabled())
  {
    throw std::runtime_error("allocation_guard: fixed_list performed allocation.");
  }
  else
  {
    this->mAryPtr = new T[this->mSize];
  }
}

#endif /* FIXED_LIST_H */
