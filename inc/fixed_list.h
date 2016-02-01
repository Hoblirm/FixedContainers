#ifndef FIXED_LIST_H
#define FIXED_LIST_H

#include <fixed_list_base.h>

template<class T, size_t N = 0> class fixed_list: public fixed_list_base<T,std::allocator<fixed_list_node<T> > >
{
public:
  fixed_list();
  fixed_list(size_t size, const T& val = T());
  fixed_list(const T* first, const T* last);
  fixed_list(const fixed_list<T, 0> & obj);
  fixed_list<T, N>& operator=(const fixed_list<T, N>& obj);
  fixed_list<T, N>& operator=(const fixed_list<T, 0>& obj);
  operator const fixed_list<T,0>&() const;
  operator fixed_list<T,0>&();
private:
  fixed_pool<fixed_list_node<T>,N> mPool;
};

template<class T, size_t N> fixed_list<T, N>::fixed_list() :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >(&mPool)
{
}

template<class T, size_t N> fixed_list<T, N>::fixed_list(size_t size, const T& val) :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >(&mPool)
{
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(size,val);
}

template<class T, size_t N> fixed_list<T, N>::fixed_list(const T* first, const T* last) :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >(&mPool)
{
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(first,last);
}

template<class T, size_t N> fixed_list<T, N>::fixed_list(const fixed_list<T, 0> & obj) :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >(&mPool)
{
  *this = obj;
}

template<class T, size_t N> fixed_list<T, N>& fixed_list<T, N>::operator=(const fixed_list<T, N>& obj)
{
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(obj.begin(),obj.end());
  return *this;
}

template<class T, size_t N> fixed_list<T, N>& fixed_list<T, N>::operator=(const fixed_list<T, 0>& obj)
{
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(obj.begin(),obj.end());
  return *this;
}

template<class T, size_t N> fixed_list<T, N>::operator const fixed_list<T, 0>&() const
{
  return *((fixed_list<T, 0>*) this);
}

template<class T, size_t N> fixed_list<T, N>::operator fixed_list<T, 0>&()
{
  return *((fixed_list<T, 0>*) this);
}

template<class T> class fixed_list<T, 0> : public fixed_list_base<T,std::allocator<fixed_list_node<T> > >
{
public:
  fixed_list();
  fixed_list(size_t size, const T& val = T());
  fixed_list(const T* first, const T* last);
  fixed_list(const fixed_list<T, 0> & obj);
  ~fixed_list();
  fixed_list<T, 0>& operator=(const fixed_list<T, 0>& obj);

private:
  //void allocate();
};

template<class T> fixed_list<T, 0>::fixed_list() :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >()
{
  //allocate();
}

template<class T> fixed_list<T, 0>::fixed_list(size_t size, const T& val) :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >()
{
  //allocate();
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(size,val);
}

template<class T> fixed_list<T, 0>::fixed_list(const T* first, const T* last) :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >()
{
  //allocate();
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(first,last);
}

template<class T> fixed_list<T, 0>::fixed_list(const fixed_list<T, 0> & obj) :
    fixed_list_base<T,std::allocator<fixed_list_node<T> > >()
{
  //allocate();
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(obj.cbegin(),obj.cend());
}

template<class T> fixed_list<T, 0>::~fixed_list()
{
   //TODO:
  //delete[] this->mAryPtr;
}

template<class T> fixed_list<T, 0>& fixed_list<T, 0>::operator=(const fixed_list<T, 0>& obj)
{
  fixed_list_base<T,std::allocator<fixed_list_node<T> > >::assign(obj.cbegin(),obj.cend());
  return *this;
}

/*
template<class T> void fixed_list<T, 0>::allocate()
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
