#ifndef FIXED_LIST_ITERATOR_H
#define FIXED_LIST_ITERATOR_H

#include <cstdlib>

template<class T>
struct fixed_list_node
{
  T val;
  fixed_list_node* next;
  fixed_list_node* prev;
};

template<class T>
struct fixed_forward_list_node
{
  T val;
  fixed_forward_list_node* next;
};

template<class T, class Node> struct fixed_forward_list_iterator_base
{
  Node* mPtr;

  fixed_forward_list_iterator_base<T, Node>(Node* p) :
      mPtr(p)
  {
  }

  bool operator==(fixed_forward_list_iterator_base<T, Node> obj) const
  {
    return (mPtr == obj.mPtr);
  }

  bool operator!=(fixed_forward_list_iterator_base<T, Node> obj) const
  {
    return (mPtr != obj.mPtr);
  }

  const T& operator*() const
  {
    return mPtr->val;
  }

  const T* operator->() const
  {
    return &(mPtr->val);
  }

};

template<class T, class Node = fixed_forward_list_node<T> > struct fixed_forward_list_const_iterator: public fixed_forward_list_iterator_base<
    T, Node>
{

  fixed_forward_list_const_iterator<T, Node>() :
      fixed_forward_list_iterator_base<T, Node>(NULL)
  {
  }
   
  fixed_forward_list_const_iterator<T, Node>(Node* p) :
      fixed_forward_list_iterator_base<T, Node>(p)
  {
  }

  fixed_forward_list_const_iterator<T, Node>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }
  
   fixed_forward_list_const_iterator<T> operator++(T) 
  {
   fixed_forward_list_const_iterator<T> tmp(this->mPtr); 
   operator++(); 
   return tmp;
}
   
};

template<class T, class Node = fixed_forward_list_node<T> > struct fixed_forward_list_iterator: public fixed_forward_list_iterator_base<
    T, Node>
{

   fixed_forward_list_iterator<T, Node>() :
      fixed_forward_list_iterator_base<T, Node>(NULL)
  {
  }
    
  fixed_forward_list_iterator<T, Node>(Node* p) :
      fixed_forward_list_iterator_base<T, Node>(p)
  {
  }

  operator const fixed_forward_list_const_iterator<T>&() const
  {
     return *this;
  }
  
  fixed_forward_list_iterator<T, Node>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

   fixed_forward_list_iterator<T> operator++(T) 
  {
   fixed_forward_list_iterator<T> tmp(this->mPtr); 
   operator++(); 
   return tmp;
}
   
  T& operator*()
  {
    return (this->mPtr)->val;
  }

  T* operator->()
  {
    return &((this->mPtr)->val);
  }
};

template<class T> struct fixed_list_const_iterator: public fixed_forward_list_const_iterator<T, fixed_list_node<T> >
{

   fixed_list_const_iterator<T>() :
      fixed_forward_list_const_iterator<T, fixed_list_node<T> >(NULL)
  {
  }
   
  fixed_list_const_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_const_iterator<T, fixed_list_node<T> >(p)
  {
  }

  fixed_list_const_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

   fixed_list_const_iterator<T> operator++(T) 
  {
   fixed_list_const_iterator<T> tmp(this->mPtr); 
   operator++(); 
   return tmp;
}
   
  fixed_list_const_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }

   fixed_list_const_iterator<T> operator--(T) 
  {
   fixed_list_const_iterator<T> tmp(this->mPtr); 
   operator--(); 
   return tmp;
}
   
};

template<class T> struct fixed_list_iterator: public fixed_forward_list_iterator<T, fixed_list_node<T> >
{

   fixed_list_iterator<T>() :
      fixed_forward_list_iterator<T, fixed_list_node<T> >(NULL)
  {
  }
   
  fixed_list_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_iterator<T, fixed_list_node<T> >(p)
  {
  }

  operator const fixed_list_const_iterator<T>&() const
  {
     return *this;
  }
  
  fixed_list_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

  fixed_list_iterator<T> operator++(T) 
  {
   fixed_list_iterator<T> tmp(this->mPtr); 
   operator++(); 
   return tmp;
}
  
  fixed_list_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }

  fixed_list_iterator<T> operator--(T) 
  {
   fixed_list_iterator<T> tmp(this->mPtr); 
   operator--(); 
   return tmp;
}
};

template<class T> struct fixed_list_const_reverse_iterator: public fixed_forward_list_const_iterator<T,
    fixed_list_node<T> >
{

   fixed_list_const_reverse_iterator<T>() :
      fixed_forward_list_const_iterator<T, fixed_list_node<T> >(NULL)
  {
  }
   
  fixed_list_const_reverse_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_const_iterator<T, fixed_list_node<T> >(p)
  {
  }

  fixed_list_const_reverse_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }

  fixed_list_const_reverse_iterator<T> operator++(T) 
  {
   fixed_list_const_reverse_iterator<T> tmp(this->mPtr); 
   operator++(); 
   return tmp;
}
  
  fixed_list_const_reverse_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }

  fixed_list_const_reverse_iterator<T> operator--(T) 
  {
   fixed_list_const_reverse_iterator<T> tmp(this->mPtr); 
   operator--(); 
   return tmp;
}
  
};

template<class T> struct fixed_list_reverse_iterator: public fixed_forward_list_iterator<T, fixed_list_node<T> >
{
   fixed_list_reverse_iterator<T>() :
      fixed_forward_list_iterator<T, fixed_list_node<T> >(NULL)
  {
  }
   
  fixed_list_reverse_iterator<T>(fixed_list_node<T>* p) :
      fixed_forward_list_iterator<T, fixed_list_node<T> >(p)
  {
  }

  operator const fixed_list_const_reverse_iterator<T>&() const
  {
     return *this;
  }
  
  fixed_list_reverse_iterator<T>& operator++()
  {
    this->mPtr = this->mPtr->prev;
    return *this;
  }
  
fixed_list_reverse_iterator<T> operator++(T) 
{
   fixed_list_reverse_iterator<T> tmp(this->mPtr); 
   operator++(); 
   return tmp;
}
  
  fixed_list_reverse_iterator<T>& operator--()
  {
    this->mPtr = this->mPtr->next;
    return *this;
  }
  
  fixed_list_reverse_iterator<T> operator--(T) 
  {
   fixed_list_reverse_iterator<T> tmp(this->mPtr); 
   operator--(); 
   return tmp;
}
};

#endif /* FIXED_LIST_ITERATOR_H */
