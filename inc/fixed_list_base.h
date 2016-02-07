#ifndef FIXED_LIST_BASE_H
#define FIXED_LIST_BASE_H

#include <fixed_pool.h>
#include <fixed_list_iterator.h>
#include <allocator.h>

template<class T, class Alloc = flex::allocator<fixed_list_node<T> > > class fixed_list_base: public fixed_pool_base<
    fixed_list_node<T> >
{
public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef fixed_list_node<T> node;
  typedef fixed_list_iterator<T> iterator;
  typedef fixed_list_const_iterator<T> const_iterator;
  typedef fixed_list_reverse_iterator<T> reverse_iterator;
  typedef fixed_list_const_reverse_iterator<T> const_reverse_iterator;
  typedef Alloc allocator_type;

  void assign(size_t size, const_reference val);
  //TODO: Get a template working with fixed_list assign() to use multiple iterators.
  void assign(const_iterator first, const_iterator last);
  void assign(const T* first, const T* last);

  reference back();
  const_reference back() const;

  iterator begin();
  const_iterator begin() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  bool empty() const;

  iterator end();
  const_iterator end() const;

  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);

  bool fixed() const;

  reference front();
  allocator_type get_allocator() const;
  const_reference front() const;

  iterator insert(iterator position, const_reference val);
  void insert(iterator position, size_t n, const_reference val);
  //TODO: Get template to work with fixed_list insert() to use multiple iterators.
  void insert(iterator position, const_iterator first, const_iterator last);
  void insert(iterator position, const T* first, const T* last);

  size_t max_size() const;

  fixed_list_base<T, Alloc>& operator=(const fixed_list_base<T, Alloc>& obj);

  void pop_back();
  void pop_front();
  void push_back(const_reference val);
  void push_front(const_reference val);

  reverse_iterator rbegin();
  const_reverse_iterator rbegin() const;
  reverse_iterator rend();
  const_reverse_iterator rend() const;

  size_t size() const;
  void swap(fixed_list_base<T, Alloc>& obj);

protected:
  fixed_list_base();
  fixed_list_base(size_t capacity, fixed_list_node<T>* contentPtr, fixed_list_node<T>** ptrPtr);

  node* mHead;
  node* mTail;
  Alloc mAllocator;

private:
  void push_back_no_throw(const_reference val);
  node* DoAllocateAndConstruct();
  void DoDestroyAndDeallocate(node* ptr);
};

template<class T, class Alloc> void fixed_list_base<T, Alloc>::assign(size_t rsize,
    fixed_list_base<T, Alloc>::const_reference val)
{
  if (rsize > max_size())
  {
    throw std::runtime_error("fixed_list: assign() fill range exceeds capacity");
  }

  if (size() < rsize)
  {
    for (iterator it = begin(); it != end(); ++it)
    {
      *it = val;
    }
    while (size() < rsize)
    {
      //Capacity check done above. Safe to do the faster no_throw method.
      push_back_no_throw(val);
    }
  }
  else
  {
    iterator it = begin();
    for (int i = 0; i < rsize; ++i)
    {
      *(it++) = val;
    }
    while (size() > rsize)
    {
      pop_back();
    }
  }
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::assign(fixed_list_base<T, Alloc>::const_iterator first,
    fixed_list_base::const_iterator last)
{
  typename fixed_list_base<T, Alloc>::iterator lit = begin();
  typename fixed_list_base<T, Alloc>::const_iterator rit = first;
  size_t rsize = 0;
  while ((lit != end()) && (rit != last))
  {
    *(lit++) = *(rit++);
    ++rsize;
  }
  while (size() > rsize)
  {
    pop_back();
  }
  while (rit != last)
  {
    push_back(*(rit++));
  }
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::assign(const T* first, const T* last)
{
  typename fixed_list_base<T, Alloc>::iterator lit = begin();
  const T* rit = first;
  size_t rsize = 0;
  while ((lit != end()) && (rit != last))
  {
    *(lit++) = *(rit++);
    ++rsize;
  }
  while (size() > rsize)
  {
    pop_back();
  }
  while (rit != last)
  {
    push_back(*(rit++));
  }
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::reference fixed_list_base<T, Alloc>::back()
{
  return mTail->val;
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_reference fixed_list_base<T, Alloc>::back() const
{
  return mTail->val;
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::iterator fixed_list_base<T, Alloc>::begin()
{
  return typename fixed_list_base<T, Alloc>::iterator(mHead);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_iterator fixed_list_base<T, Alloc>::begin() const
{

  return typename fixed_list_base<T, Alloc>::const_iterator(mHead);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_iterator fixed_list_base<T, Alloc>::cbegin() const
{
  return typename fixed_list_base<T, Alloc>::const_iterator(mHead);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_iterator fixed_list_base<T, Alloc>::cend() const
{
  return typename fixed_list_base<T, Alloc>::const_iterator(NULL);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_reverse_iterator fixed_list_base<T, Alloc>::crbegin() const
{
  return fixed_list_const_reverse_iterator<T>(mTail);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_reverse_iterator fixed_list_base<T, Alloc>::crend() const
{
  return fixed_list_const_reverse_iterator<T>(NULL);
}

template<class T, class Alloc> bool fixed_list_base<T, Alloc>::empty() const
{
  return (0 == size());
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::iterator fixed_list_base<T, Alloc>::end()
{
  return typename fixed_list_base<T, Alloc>::iterator(NULL);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_iterator fixed_list_base<T, Alloc>::end() const
{
  return typename fixed_list_base<T, Alloc>::const_iterator(NULL);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::iterator fixed_list_base<T, Alloc>::erase(
    typename fixed_list_base<T, Alloc>::iterator position)
{
  node* lhs = position.mNodePtr->prev;
  node* rhs = position.mNodePtr->next;

  if (NULL != lhs)
  {
    lhs->next = rhs;
  }
  else
  {
    mHead = rhs; //head is being erased; must update
  }

  if (NULL != rhs)
  {
    rhs->prev = lhs;
  }
  else
  {
    mTail = lhs; //tail is being erased; must update
  }

  if (fixed())
  {
    fixed_pool_base<fixed_list_node<T> >::deallocate(position.mNodePtr);
  }
  else
  {
    DoDestroyAndDeallocate(position.mNodePtr);
    --this->mIndex;
  }

  return iterator(rhs);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::iterator fixed_list_base<T, Alloc>::erase(
    typename fixed_list_base<T, Alloc>::iterator first, typename fixed_list_base<T, Alloc>::iterator last)
{
  node* lhs = first.mNodePtr->prev;
  node* rhs = last.mNodePtr;

  if (NULL != lhs)
  {
    lhs->next = rhs;
  }
  else
  {
    mHead = rhs; //head is being erased; must update
  }

  if (NULL != rhs)
  {
    rhs->prev = lhs;
  }
  else
  {
    mTail = lhs; //tail is being erased; must update
  }

  if (fixed())
  {
    for (iterator it = first; it != last; ++it)
    {
      fixed_pool_base<fixed_list_node<T> >::deallocate(it.mNodePtr);
    }
  }
  else
  {
    for (iterator it = first; it != last; ++it)
    {
      DoDestroyAndDeallocate(it.mNodePtr);
      --this->mIndex;
    }
  }

  return last;
}

template<class T, class Alloc> bool fixed_list_base<T, Alloc>::fixed() const
{
  return (NULL != this->mContentList);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::allocator_type fixed_list_base<T, Alloc>::get_allocator() const
{
  return mAllocator;
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::reference fixed_list_base<T, Alloc>::front()
{
  return mHead->val;
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_reference fixed_list_base<T, Alloc>::front() const
{
  return mHead->val;
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::iterator fixed_list_base<T, Alloc>::insert(
    typename fixed_list_base<T, Alloc>::iterator position, typename fixed_list_base<T, Alloc>::const_reference val)
{
  if (size() == max_size())
  {
    throw std::runtime_error("fixed_list: insert() called when size was at capacity");
  }

  if (NULL == position.mNodePtr)
  {
    //List is empty, or we are inserting at the end of the list.
    push_back_no_throw(val);
  }
  else
  {
    node* lhs = position.mNodePtr->prev;
    node* nd;
    if (fixed())
    {
      nd = fixed_pool_base<fixed_list_node<T> >::allocate_no_throw();
    }
    else
    {
      nd = DoAllocateAndConstruct();
      ++this->mIndex;
    }

    nd->val = val;
    nd->next = position.mNodePtr;
    position.mNodePtr->prev = nd;

    if (NULL != lhs)
    {
      nd->prev = lhs;
      lhs->next = nd;
    }
    else
    {
      nd->prev = NULL;
      mHead = nd;
      //list cannot be empty due to position NULL check; therefore tail doesn't need to be examined
    }
    position.mNodePtr = nd;
  }

  return position;
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::insert(fixed_list_base<T, Alloc>::iterator position,
    size_t n, fixed_list_base<T, Alloc>::const_reference val)
{
  size_t rsize = size() + n;
  if (rsize > max_size())
  {
    throw std::runtime_error("fixed_list: insert() fill range exceeds capacity");
  }

  if (NULL == position.mNodePtr)
  {
    //List is empty, or we are inserting at the end of the list.
    while (size() < rsize)
    {
      push_back_no_throw(val);
    }
  }
  else
  {
    node* lhs = position.mNodePtr->prev;
    node* nd = NULL;
    for (size_t i = 0; i < n; ++i)
    {
      if (fixed())
      {
        nd = fixed_pool_base<fixed_list_node<T> >::allocate_no_throw();
      }
      else
      {
        nd = DoAllocateAndConstruct();
        ++this->mIndex;
      }
      nd->val = val;
      nd->next = position.mNodePtr;
      position.mNodePtr->prev = nd;
      position.mNodePtr = nd;
    }

    if (NULL != lhs)
    {
      nd->prev = lhs;
      lhs->next = nd;
    }
    else
    {
      nd->prev = NULL;
      mHead = nd;
      //list cannot be empty due to position NULL check; therefore tail doesn't need to be examined
    }
  }
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::insert(fixed_list_base<T, Alloc>::iterator position,
    fixed_list_base<T, Alloc>::const_iterator first, fixed_list_base<T, Alloc>::const_iterator last)
{
  if (NULL == position.mNodePtr)
  {
    //List is empty, or we are inserting at the end of the list.
    for (const_iterator it = first; it != last; ++it)
    {
      push_back(*it);
    }
  }
  else
  {
    node* lhs = position.mNodePtr->prev;
    node* nd = NULL;
    const_iterator it = first;

    if (NULL == lhs)
    {
      if (fixed())
      {
        nd = fixed_pool_base<fixed_list_node<T> >::allocate();
      }
      else
      {
        nd = DoAllocateAndConstruct();
        ++this->mIndex;
      }
      nd->val = *it;
      nd->prev = NULL;
      mHead = lhs = nd;
      ++it;
    }

    while (it != last)
    {
      if (fixed())
      {
        nd = fixed_pool_base<fixed_list_node<T> >::allocate();
      }
      else
      {
        nd = DoAllocateAndConstruct();
        ++this->mIndex;
      }
      nd->val = *it;
      nd->prev = lhs;
      lhs->next = nd;
      lhs = nd;
      ++it;
    }

    nd->next = position.mNodePtr;
    position.mNodePtr->prev = nd;
  }
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::insert(fixed_list_base<T, Alloc>::iterator position,
    const T* first, const T* last)
{
  if (NULL == position.mNodePtr)
  {
    //List is empty, or we are inserting at the end of the list.
    for (const T* it = first; it != last; ++it)
    {
      push_back(*it);
    }
  }
  else
  {
    node* lhs = position.mNodePtr->prev;
    node* nd = NULL;
    const T* it = first;

    if (NULL == lhs)
    {
      if (fixed())
      {
        nd = fixed_pool_base<fixed_list_node<T> >::allocate();
      }
      else
      {
        nd = DoAllocateAndConstruct();
        ++this->mIndex;
      }
      nd->val = *it;
      nd->prev = NULL;
      mHead = lhs = nd;
      ++it;
    }

    while (it != last)
    {
      if (fixed())
      {
        nd = fixed_pool_base<fixed_list_node<T> >::allocate();
      }
      else
      {
        nd = DoAllocateAndConstruct();
        ++this->mIndex;
      }
      nd->val = *it;
      nd->prev = lhs;
      lhs->next = nd;
      lhs = nd;
      ++it;
    }

    nd->next = position.mNodePtr;
    position.mNodePtr->prev = nd;
  }
}

template<class T, class Alloc> size_t fixed_list_base<T, Alloc>::max_size() const
{
  return this->mCapacity;
}

template<class T, class Alloc> fixed_list_base<T, Alloc>& fixed_list_base<T, Alloc>::operator=(
    const fixed_list_base<T, Alloc>& obj)
{
  assign(obj.begin(), obj.end());
  return *this;
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::pop_back()
{
  node* prev = mTail->prev;
  if (NULL != prev)
  {
    prev->next = NULL;
  }
  else
  {
    mHead = NULL;
  }
  if (fixed())
  {
    fixed_pool_base<fixed_list_node<T> >::deallocate(mTail);
  }
  else
  {
    DoDestroyAndDeallocate(mTail);
    --this->mIndex;
  }
  mTail = prev;
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::pop_front()
{
  node* next = mHead->next;
  if (NULL != next)
  {
    next->prev = NULL;
  }
  else
  {
    mTail = NULL;
  }
  if (fixed())
  {
    fixed_pool_base<fixed_list_node<T> >::deallocate(mHead);
  }
  else
  {
    DoDestroyAndDeallocate(mHead);
    --this->mIndex;
  }
  mHead = next;
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::push_back(const T& val)
{
  if (size() == max_size())
  {
    throw std::runtime_error("fixed_list: push_back() caused size to exceed capacity");
  }

  push_back_no_throw(val);
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::push_back_no_throw(const T& val)
{
  node* nd;
  if (fixed())
  {
    nd = fixed_pool_base<fixed_list_node<T> >::allocate_no_throw();
  }
  else
  {
    nd = DoAllocateAndConstruct();
    ++this->mIndex;
  }
  nd->prev = mTail;
  nd->val = val;
  nd->next = NULL;
  if (NULL != mTail)
  {
    mTail->next = nd;
  }
  else
  {
    mHead = nd;
  }
  mTail = nd;
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::push_front(const T& val)
{
  if (size() == max_size())
  {
    throw std::runtime_error("fixed_list: push_front() caused size to exceed capacity");
  }

  node* nd;
  if (fixed())
  {
    nd = fixed_pool_base<fixed_list_node<T> >::allocate_no_throw();
  }
  else
  {
    nd = DoAllocateAndConstruct();
    ++this->mIndex;
  }
  nd->next = mHead;
  nd->val = val;
  nd->prev = NULL;
  if (NULL != mHead)
  {
    mHead->prev = nd;
  }
  else
  {
    mTail = nd;
  }
  mHead = nd;
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::reverse_iterator fixed_list_base<T, Alloc>::rbegin()
{
  return fixed_list_reverse_iterator<T>(mTail);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_reverse_iterator fixed_list_base<T, Alloc>::rbegin() const
{
  return fixed_list_const_reverse_iterator<T>(mTail);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::reverse_iterator fixed_list_base<T, Alloc>::rend()
{
  return fixed_list_reverse_iterator<T>(NULL);
}

template<class T, class Alloc> typename fixed_list_base<T, Alloc>::const_reverse_iterator fixed_list_base<T, Alloc>::rend() const
{
  return fixed_list_const_reverse_iterator<T>(NULL);
}

template<class T, class Alloc> size_t fixed_list_base<T, Alloc>::size() const
{
  return this->mIndex;
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::swap(fixed_list_base<T, Alloc>& obj)
{
  if ((obj.size() > max_size()) || (size() > obj.max_size()))
  {
    throw std::runtime_error("fixed_list: swap() parameters' size exceed capacity");
  }

  size_t lsize = size();
  size_t rsize = obj.size();
  if (lsize < rsize)
  {
    T tmp;
    typename fixed_list_base<T, Alloc>::iterator lit = begin();
    typename fixed_list_base<T, Alloc>::iterator rit = obj.begin();
    while (lit != end())
    {
      tmp = *lit;
      *lit = *rit;
      *rit = tmp;
      ++lit;
      ++rit;
    }

    typename fixed_list_base<T, Alloc>::reverse_iterator rrit = obj.rbegin();
    while (size() < rsize)
    {
      //Capacity check done at the top of method. Safe to do the faster no_throw method.
      push_back_no_throw(*(rrit++));
      obj.pop_back();
    }
    /*while (mIndex < rsize)
     {
     push_back(*(rit++));
     }
     while (obj.size() > lsize)
     {
     obj.pop_back();
     }*/
  }
  else
  {
    T tmp;
    typename fixed_list_base<T, Alloc>::iterator lit = begin();
    typename fixed_list_base<T, Alloc>::iterator rit = obj.begin();
    while (rit != obj.end())
    {
      tmp = *lit;
      *lit = *rit;
      *rit = tmp;
      ++lit;
      ++rit;
    }

    typename fixed_list_base<T, Alloc>::reverse_iterator rlit = rbegin();
    while (obj.size() < lsize)
    {
      //Capacity check done at the top of method. Safe to do the faster no_throw method.
      obj.push_back_no_throw(*(rlit++));
      pop_back();
    }
    /*while (obj.size() < lsize)
     {
     obj.push_back(*(rlit++));
     }
     while (size() > rsize)
     {
     pop_back();
     }*/
  }

}

template<class T, class Alloc> fixed_list_base<T, Alloc>::fixed_list_base() :
    fixed_pool_base<fixed_list_node<T> >(0), mHead(NULL), mTail(NULL)
{
  Alloc a;
  this->mCapacity = a.max_size();
  this->mContentList = NULL;
  this->mPtrList = NULL;
}

template<class T, class Alloc> fixed_list_base<T, Alloc>::fixed_list_base(size_t capacity,
    fixed_list_node<T>* contentPtr, fixed_list_node<T>** ptrPtr) :
    fixed_pool_base<fixed_list_node<T> >(capacity, contentPtr, ptrPtr), mHead(NULL), mTail(NULL)
{
  for (size_t i = 0; i < this->mCapacity; i++)
  {
    this->mPtrList[i] = &this->mContentList[i];
  }
}

template<class T, class Alloc> fixed_list_node<T>* fixed_list_base<T, Alloc>::DoAllocateAndConstruct()
{
  fixed_list_node<T>* ptr = mAllocator.allocate(1);
  mAllocator.construct(ptr, fixed_list_node<T>());
  return ptr;
}

template<class T, class Alloc> void fixed_list_base<T, Alloc>::DoDestroyAndDeallocate(fixed_list_node<T>* ptr)
{
  mAllocator.destroy(ptr);
  mAllocator.deallocate(ptr, 1);
}

template<class T, class Alloc>
bool operator==(const fixed_list_base<T, Alloc>& lhs, const fixed_list_base<T, Alloc>& rhs)
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }
  else
  {
    typename fixed_list_base<T, Alloc>::const_iterator lit = lhs.begin();
    typename fixed_list_base<T, Alloc>::const_iterator rit = rhs.begin();
    while (lit != lhs.end())
    {
      if (*(lit++) != *(rit++))
      {
        return false;
      }
    }
    return true;
  }
}

template<class T, class Alloc>
bool operator<(const fixed_list_base<T, Alloc>& lhs, const fixed_list_base<T, Alloc>& rhs)
{
  if (lhs.size() < rhs.size())
  {
    typename fixed_list_base<T, Alloc>::const_iterator lit = lhs.begin();
    typename fixed_list_base<T, Alloc>::const_iterator rit = rhs.begin();
    while (lit != lhs.end())
    {
      if (*lit < *rit)
      {
        return true;
      }
      else if (*lit > *rit)
      {
        return false;
      }
      ++lit;
      ++rit;
    }
    return true;
  }
  else
  {
    typename fixed_list_base<T, Alloc>::const_iterator lit = lhs.begin();
    typename fixed_list_base<T, Alloc>::const_iterator rit = rhs.begin();
    while (rit != rhs.end())
    {
      if (*lit < *rit)
      {
        return true;
      }
      else if (*lit > *rit)
      {
        return false;
      }
      ++lit;
      ++rit;
    }
    return false;
  }
}

template<class T, class Alloc>
bool operator!=(const fixed_list_base<T, Alloc>& lhs, const fixed_list_base<T, Alloc>& rhs)
{
  return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator>(const fixed_list_base<T, Alloc>& lhs, const fixed_list_base<T, Alloc>& rhs)
{
  return rhs < lhs;
}

template<class T, class Alloc>
bool operator<=(const fixed_list_base<T, Alloc>& lhs, const fixed_list_base<T, Alloc>& rhs)
{
  return !(rhs < lhs);
}

template<class T, class Alloc>
bool operator>=(const fixed_list_base<T, Alloc>& lhs, const fixed_list_base<T, Alloc>& rhs)
{
  return !(lhs < rhs);
}

#endif /* FIXED_LIST_BASE_H */
