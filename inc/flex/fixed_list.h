#ifndef FLEX_FIXED_LIST_H
#define FLEX_FIXED_LIST_H

#include <flex/list.h>

namespace flex
{

  template<class T, size_t N = 0, class Alloc = allocator<list_node<T> > > class fixed_list: public list<T, Alloc>
  {
  public:
    fixed_list();
    fixed_list(size_t size, const T& val = T());
    fixed_list(const T* first, const T* last);
    fixed_list(const fixed_list<T, 0, Alloc> & obj);
    fixed_list<T, N, Alloc>& operator=(const fixed_list<T, N, Alloc>& obj);
    fixed_list<T, N, Alloc>& operator=(const list<T, Alloc>& obj);
    operator const fixed_list<T,0,Alloc>&() const;
    operator fixed_list<T,0,Alloc>&();
  private:
    list_node<T> mContentAry[N];
    list_node<T>* mPtrAry[N];
  };

  template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list() :
      list<T, Alloc>(N, mContentAry, mPtrAry)
  {
  }

  template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list(size_t size, const T& val) :
      list<T, Alloc>(N, mContentAry, mPtrAry)
  {
    list<T, Alloc>::assign(size, val);
  }

  template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list(const T* first, const T* last) :
      list<T, Alloc>(N, mContentAry, mPtrAry)
  {
    list<T, Alloc>::assign(first, last);
  }

  template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>::fixed_list(const fixed_list<T, 0, Alloc> & obj) :
      list<T, Alloc>(N, mContentAry, mPtrAry)
  {
    *this = obj;
  }

  template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(
      const fixed_list<T, N, Alloc>& obj)
  {
    list<T, Alloc>::assign(obj.begin(), obj.end());
    return *this;
  }

  template<class T, size_t N, class Alloc> fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(
      const list<T, Alloc>& obj)
  {
    list<T, Alloc>::assign(obj.begin(), obj.end());
    return *this;
  }

} //namespace flex
#endif /* FLEX_FIXED_LIST_H */
