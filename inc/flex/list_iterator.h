#ifndef FLEX_LIST_ITERATOR_H
#define FLEX_LIST_ITERATOR_H

#include <cstdlib>

namespace flex
{

  template<class T>
  struct list_node
  {
    T val;
    list_node* next;
    list_node* prev;
  };

  template<class T>
  struct fixed_forward_list_node
  {
    T val;
    fixed_forward_list_node* next;
  };

  template<class T, class Node> struct fixed_forward_list_iterator_base
  {
    Node* mNodePtr;

    fixed_forward_list_iterator_base(Node* p) :
        mNodePtr(p)
    {
    }

    bool operator==(fixed_forward_list_iterator_base<T, Node> obj) const
    {
      return (mNodePtr == obj.mNodePtr);
    }

    bool operator!=(fixed_forward_list_iterator_base<T, Node> obj) const
    {
      return (mNodePtr != obj.mNodePtr);
    }

    const T& operator*() const
    {
      return mNodePtr->val;
    }

    const T* operator->() const
    {
      return &(mNodePtr->val);
    }

  };

  template<class T, class Node = fixed_forward_list_node<T> > struct fixed_forward_list_const_iterator: public fixed_forward_list_iterator_base<
      T, Node>
  {

    fixed_forward_list_const_iterator() :
        fixed_forward_list_iterator_base<T, Node>(NULL)
    {
    }

    fixed_forward_list_const_iterator(Node* p) :
        fixed_forward_list_iterator_base<T, Node>(p)
    {
    }

    fixed_forward_list_const_iterator<T, Node>& operator++()
    {
      this->mNodePtr = this->mNodePtr->next;
      return *this;
    }

    fixed_forward_list_const_iterator<T> operator++(T)
    {
      fixed_forward_list_const_iterator<T> tmp(this->mNodePtr);
      operator++();
      return tmp;
    }

  };

  template<class T, class Node = fixed_forward_list_node<T> > struct fixed_forward_list_iterator: public fixed_forward_list_iterator_base<
      T, Node>
  {

    fixed_forward_list_iterator() :
        fixed_forward_list_iterator_base<T, Node>(NULL)
    {
    }

    fixed_forward_list_iterator(Node* p) :
        fixed_forward_list_iterator_base<T, Node>(p)
    {
    }

    operator fixed_forward_list_const_iterator<T>&() const
    {
      return *((fixed_forward_list_const_iterator<T>*) this);
    }

    fixed_forward_list_iterator<T, Node>& operator++()
    {
      this->mNodePtr = this->mNodePtr->next;
      return *this;
    }

    fixed_forward_list_iterator<T> operator++(T)
    {
      fixed_forward_list_iterator<T> tmp(this->mNodePtr);
      operator++();
      return tmp;
    }

    T& operator*()
    {
      return (this->mNodePtr)->val;
    }

    T* operator->()
    {
      return &((this->mNodePtr)->val);
    }
  };

  template<class T> struct list_const_iterator: public fixed_forward_list_const_iterator<T, list_node<T> >
  {

    list_const_iterator() :
        fixed_forward_list_const_iterator<T, list_node<T> >(NULL)
    {
    }

    list_const_iterator(list_node<T>* p) :
        fixed_forward_list_const_iterator<T, list_node<T> >(p)
    {
    }

    list_const_iterator<T>& operator++()
    {
      this->mNodePtr = this->mNodePtr->next;
      return *this;
    }

    list_const_iterator<T> operator++(T)
    {
      list_const_iterator<T> tmp(this->mNodePtr);
      operator++();
      return tmp;
    }

    list_const_iterator<T>& operator--()
    {
      this->mNodePtr = this->mNodePtr->prev;
      return *this;
    }

    list_const_iterator<T> operator--(T)
    {
      list_const_iterator<T> tmp(this->mNodePtr);
      operator--();
      return tmp;
    }

  };

  template<class T> struct list_iterator: public fixed_forward_list_iterator<T, list_node<T> >
  {

    list_iterator() :
        fixed_forward_list_iterator<T, list_node<T> >(NULL)
    {
    }

    list_iterator(list_node<T>* p) :
        fixed_forward_list_iterator<T, list_node<T> >(p)
    {
    }

    operator list_const_iterator<T>&() const
    {
      return *((list_const_iterator<T>*) this);
    }

    list_iterator<T>& operator++()
    {
      this->mNodePtr = this->mNodePtr->next;
      return *this;
    }

    list_iterator<T> operator++(T)
    {
      list_iterator<T> tmp(this->mNodePtr);
      operator++();
      return tmp;
    }

    list_iterator<T>& operator--()
    {
      this->mNodePtr = this->mNodePtr->prev;
      return *this;
    }

    list_iterator<T> operator--(T)
    {
      list_iterator<T> tmp(this->mNodePtr);
      operator--();
      return tmp;
    }
  };

  template<class T> struct list_const_reverse_iterator: public fixed_forward_list_const_iterator<T, list_node<T> >
  {

    list_const_reverse_iterator() :
        fixed_forward_list_const_iterator<T, list_node<T> >(NULL)
    {
    }

    list_const_reverse_iterator(list_node<T>* p) :
        fixed_forward_list_const_iterator<T, list_node<T> >(p)
    {
    }

    list_const_reverse_iterator<T>& operator++()
    {
      this->mNodePtr = this->mNodePtr->prev;
      return *this;
    }

    list_const_reverse_iterator<T> operator++(T)
    {
      list_const_reverse_iterator<T> tmp(this->mNodePtr);
      operator++();
      return tmp;
    }

    list_const_reverse_iterator<T>& operator--()
    {
      this->mNodePtr = this->mNodePtr->next;
      return *this;
    }

    list_const_reverse_iterator<T> operator--(T)
    {
      list_const_reverse_iterator<T> tmp(this->mNodePtr);
      operator--();
      return tmp;
    }

  };

  template<class T> struct list_reverse_iterator: public fixed_forward_list_iterator<T, list_node<T> >
  {
    list_reverse_iterator() :
        fixed_forward_list_iterator<T, list_node<T> >(NULL)
    {
    }

    list_reverse_iterator(list_node<T>* p) :
        fixed_forward_list_iterator<T, list_node<T> >(p)
    {
    }

    operator const list_const_reverse_iterator<T>&() const
    {
      return *((list_const_reverse_iterator<T>*) this);
    }

    list_reverse_iterator<T>& operator++()
    {
      this->mNodePtr = this->mNodePtr->prev;
      return *this;
    }

    list_reverse_iterator<T> operator++(T)
    {
      list_reverse_iterator<T> tmp(this->mNodePtr);
      operator++();
      return tmp;
    }

    list_reverse_iterator<T>& operator--()
    {
      this->mNodePtr = this->mNodePtr->next;
      return *this;
    }

    list_reverse_iterator<T> operator--(T)
    {
      list_reverse_iterator<T> tmp(this->mNodePtr);
      operator--();
      return tmp;
    }
  };

} //namespace flex

#endif /* FLEX_LIST_ITERATOR_H */
