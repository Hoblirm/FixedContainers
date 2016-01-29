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
  Node* mNodePtr;

  fixed_forward_list_iterator_base<T, Node>(Node* p) :
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

   fixed_forward_list_iterator<T, Node>() :
      fixed_forward_list_iterator_base<T, Node>(NULL)
  {
  }
    
  fixed_forward_list_iterator<T, Node>(Node* p) :
      fixed_forward_list_iterator_base<T, Node>(p)
  {
  }

  operator fixed_forward_list_const_iterator<T>&() const
  {
	  return *((fixed_forward_list_const_iterator<T>*)this);
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
    this->mNodePtr = this->mNodePtr->next;
    return *this;
  }

   fixed_list_const_iterator<T> operator++(T) 
  {
   fixed_list_const_iterator<T> tmp(this->mNodePtr);
   operator++(); 
   return tmp;
}
   
  fixed_list_const_iterator<T>& operator--()
  {
    this->mNodePtr = this->mNodePtr->prev;
    return *this;
  }

   fixed_list_const_iterator<T> operator--(T) 
  {
   fixed_list_const_iterator<T> tmp(this->mNodePtr);
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


  operator fixed_list_const_iterator<T>&() const
  {
     return *((fixed_list_const_iterator<T>*)this);
  }
  
  fixed_list_iterator<T>& operator++()
  {
    this->mNodePtr = this->mNodePtr->next;
    return *this;
  }

  fixed_list_iterator<T> operator++(T) 
  {
   fixed_list_iterator<T> tmp(this->mNodePtr);
   operator++(); 
   return tmp;
}
  
  fixed_list_iterator<T>& operator--()
  {
    this->mNodePtr = this->mNodePtr->prev;
    return *this;
  }

  fixed_list_iterator<T> operator--(T) 
  {
   fixed_list_iterator<T> tmp(this->mNodePtr);
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
    this->mNodePtr = this->mNodePtr->prev;
    return *this;
  }

  fixed_list_const_reverse_iterator<T> operator++(T) 
  {
   fixed_list_const_reverse_iterator<T> tmp(this->mNodePtr);
   operator++(); 
   return tmp;
}
  
  fixed_list_const_reverse_iterator<T>& operator--()
  {
    this->mNodePtr = this->mNodePtr->next;
    return *this;
  }

  fixed_list_const_reverse_iterator<T> operator--(T) 
  {
   fixed_list_const_reverse_iterator<T> tmp(this->mNodePtr);
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
	  return *((fixed_list_const_reverse_iterator<T>*)this);
  }
  
  fixed_list_reverse_iterator<T>& operator++()
  {
    this->mNodePtr = this->mNodePtr->prev;
    return *this;
  }
  
fixed_list_reverse_iterator<T> operator++(T) 
{
   fixed_list_reverse_iterator<T> tmp(this->mNodePtr);
   operator++(); 
   return tmp;
}
  
  fixed_list_reverse_iterator<T>& operator--()
  {
    this->mNodePtr = this->mNodePtr->next;
    return *this;
  }
  
  fixed_list_reverse_iterator<T> operator--(T) 
  {
   fixed_list_reverse_iterator<T> tmp(this->mNodePtr);
   operator--(); 
   return tmp;
}
};

#endif /* FIXED_LIST_ITERATOR_H */
