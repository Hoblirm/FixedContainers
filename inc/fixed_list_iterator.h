#ifndef FIXED_LIST_ITERATOR_H
#define FIXED_LIST_ITERATOR_H

#include <cstdlib>

template<class T>
struct fixed_list_node
{
  fixed_list_node* prev;
  fixed_list_node* next;
  T data;
};

template<class T> struct fixed_forward_list_iterator_base
{
  fixed_list_node<T>* mPtr;

  fixed_forward_list_iterator_base<T>(fixed_list_node<T>* p) :
      mPtr(p)
  {
  }

  bool operator==(fixed_forward_list_iterator_base<T> obj) const
  {
    return (mPtr == obj.mPtr);
  }

  bool operator!=(fixed_forward_list_iterator_base<T> obj) const
  {
    return (mPtr != obj.mPtr);
  }

  const T& operator*() const
  {
    return mPtr->data;
  }

  const T* operator->() const
  {
    return &(mPtr->data);
  }

};

template<class T> struct fixed_forward_list_const_iterator: public fixed_forward_list_iterator_base<T>
{

  fixed_forward_list_const_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_iterator_base<T>(p)
  {
  }

  fixed_forward_list_const_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }
};

template<class T> struct fixed_forward_list_iterator: public fixed_forward_list_iterator_base<T>
{

  fixed_forward_list_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_iterator_base<T>(p)
  {
  }

  fixed_forward_list_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

  T& operator*()
  {
    return *(this->mPtr);
  }

  T* operator->()
  {
    return &((this->mPtr)->data);
  }
};

template<class T> struct fixed_list_const_iterator: public fixed_forward_list_const_iterator<T>
{

  fixed_list_const_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_const_iterator<T>(p)
  {
  }

  fixed_list_const_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

  fixed_list_const_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }

};

template<class T> struct fixed_list_iterator: public fixed_forward_list_iterator<T>
{

  fixed_list_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_iterator<T>(p)
  {
  }

  fixed_list_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

  fixed_list_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }

};

template<class T> struct fixed_list_const_reverse_iterator: public fixed_forward_list_const_iterator<T>
{

  fixed_list_const_reverse_iterator<T>(fixed_list_node<T>* p) :
    fixed_forward_list_const_iterator<T>(p)
  {
  }

  fixed_list_const_reverse_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }

  fixed_list_const_reverse_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

};

template<class T> struct fixed_list_reverse_iterator: public fixed_forward_list_iterator<T>
{
  fixed_list_reverse_iterator<T>(fixed_list_node<T>* p) :
    fixed_forward_list_iterator<T>(p)
  {
  }

  fixed_list_reverse_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }

  fixed_list_reverse_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }
};

#endif /* FIXED_LIST_ITERATOR_H */
