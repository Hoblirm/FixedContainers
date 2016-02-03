#ifndef FIXED_LIST_H
#define FIXED_LIST_H

#include <fixed_list_base.h>

template<class T, size_t N = 0, class Alloc = flex::allocator<fixed_list_node<T> > > class fixed_list: public fixed_list_base<
    T, Alloc>
{
public:
  fixed_list();
  fixed_list(size_t size, const T& val = T());
  fixed_list(const T* first, const T* last);
  fixed_list(const fixed_list<T, 0, Alloc> & obj);
  fixed_list<T, N, Alloc>& operator=(const fixed_list<T, N, Alloc>& obj);
  fixed_list<T, N, Alloc>& operator=(const fixed_list<T, 0, Alloc>& obj);
  operator const fixed_list<T,0,Alloc>&() const;
  operator fixed_list<T,0,Alloc>&();
private:
  fixed_list_node<T> mContentAry[N];
  fixed_list_node<T>* mPtrAry[N];
};

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list() :
    fixed_list_base<T, Alloc>(N, mContentAry, mPtrAry)
{
}

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list(size_t size, const T& val) :
    fixed_list_base<T, Alloc>(N, mContentAry, mPtrAry)
{
  fixed_list_base<T, Alloc>::assign(size, val);
}

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list(const T* first, const T* last) :
    fixed_list_base<T, Alloc>(N, mContentAry, mPtrAry)
{
  fixed_list_base<T, Alloc>::assign(first, last);
}

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list(const fixed_list<T, 0, Alloc> & obj) :
    fixed_list_base<T, Alloc>(N, mContentAry, mPtrAry)
{
  *this = obj;
}

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(
    const fixed_list<T, N, Alloc>& obj)
{
  fixed_list_base<T, Alloc>::assign(obj.begin(), obj.end());
  return *this;
}

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(
    const fixed_list<T, 0, Alloc>& obj)
{
  fixed_list_base<T, Alloc>::assign(obj.begin(), obj.end());
  return *this;
}

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::operator const fixed_list<T,0,Alloc>&() const
{
  return *((fixed_list<T, 0, Alloc>*) this);
}

template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::operator fixed_list<T,0,Alloc>&()
{
  return *((fixed_list<T, 0, Alloc>*) this);
}

template<class T, class Alloc> class fixed_list<T, 0, Alloc> : public fixed_list_base<T, Alloc>
{
public:
  fixed_list();
  fixed_list(size_t size, const T& val = T());
  fixed_list(const T* first, const T* last);
  fixed_list(const fixed_list<T, 0, Alloc> & obj);
  ~fixed_list();
  fixed_list<T, 0, Alloc>& operator=(const fixed_list<T, 0, Alloc>& obj);

private:
  //void allocate();
};

template<class T, class Alloc> fixed_list<T, 0, Alloc>::fixed_list() :
    fixed_list_base<T, Alloc>()
{
  //allocate();
}

template<class T, class Alloc> fixed_list<T, 0, Alloc>::fixed_list(size_t size, const T& val) :
    fixed_list_base<T, Alloc>()
{
  //allocate();
  fixed_list_base<T, Alloc>::assign(size, val);
}

template<class T, class Alloc> fixed_list<T, 0, Alloc>::fixed_list(const T* first, const T* last) :
    fixed_list_base<T, Alloc>()
{
  //allocate();
  fixed_list_base<T, Alloc>::assign(first, last);
}

template<class T, class Alloc> fixed_list<T, 0, Alloc>::fixed_list(const fixed_list<T, 0, Alloc> & obj) :
    fixed_list_base<T, Alloc>()
{
  //allocate();
  fixed_list_base<T, Alloc>::assign(obj.cbegin(), obj.cend());
}

template<class T, class Alloc> fixed_list<T, 0, Alloc>::~fixed_list()
{
  //TODO:
  //delete[] this->mAryPtr;
}

template<class T, class Alloc> fixed_list<T, 0, Alloc>& fixed_list<T, 0, Alloc>::operator=(
    const fixed_list<T, 0, Alloc>& obj)
{
  fixed_list_base<T, Alloc>::assign(obj.cbegin(), obj.cend());
  return *this;
}

/*
 template<class T, class Alloc> void fixed_list<T,0,Alloc>::allocate()
 {
 if (allocation_guard::is_enabled())
 {
 throw std::runtime_error("allocation_guard: fixed_list performed allocation.");
 }
 else
 {
 this->mAryPtr = new T[this->mCapacity];
 }
 }
 */

#endif /* FIXED_LIST_H */
