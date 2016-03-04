#ifndef FLEX_LIST_H
#define FLEX_LIST_H

#include <flex/allocation_guard.h>
#include <flex/list_iterator.h>
#include <flex/allocator.h>

namespace flex
{

  template<class T, class Alloc = allocator<list_node<T> > > class list: public allocation_guard
  {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef list_node<T> node_type;
    typedef list_node_base base_node_type;
    typedef list_iterator<T, T*, T&> iterator;
    typedef list_iterator<T, const T*, const T&> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef Alloc allocator_type;

    list();
    explicit list(size_t size, const T& val = T());
    list(const_iterator first, const_iterator last);
    list(const T* first, const T* last);
    list(const list<T, Alloc> & obj);
    ~list();

    void assign(size_t size, const_reference val);
    //TODO: Get a template working with fixed_list assign() to use multiple iterators.
    void assign(const_iterator first, const_iterator last);
    void assign(const_pointer first, const_pointer last);

    reference back();
    const_reference back() const;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    size_type capacity();
    void clear();
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
    void insert(iterator position, size_type n, const_reference val);
    //TODO: Get template to work with fixed_list insert() to use multiple iterators.
    void insert(iterator position, const_iterator first, const_iterator last);
    void insert(iterator position, const T* first, const T* last);

    size_type max_size() const;

    list<T, Alloc>& operator=(const list<T, Alloc>& obj);

    void pop_back();
    void pop_front();
    void push_back(const_reference val);
    void push_front(const_reference val);

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

    void reserve(size_type n);
    void resize(size_type n, const value_type& val = value_type());
    void shrink_to_fit();
    size_t size() const;
    void swap(list<T, Alloc>& obj);

  protected:
    list(size_t capacity, list_node<T>* contentPtr);

    base_node_type mAnchor;
    size_type mSize;
    bool mFixed;
    node_type* mNodePool;
    Alloc mAllocator;

  private:
    node_type* RetrieveNode();
    size_type GetNodePoolSize();
    void PushToNodePool(node_type* ptr);
    void PurgeNodePool();
    void DestroyAndDeallocateNode(node_type* ptr);
  };

  template<class T, class Alloc>
  inline list<T, Alloc>::list() :
      mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(size_t size, const T& val) :
      mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    list<T, Alloc>::assign(size, val);
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(const_iterator first, const_iterator last) :
      mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    list<T, Alloc>::assign(first, last);
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(const T* first, const T* last) :
      mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    list<T, Alloc>::assign(first, last);
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(const list<T, Alloc> & obj) :
      mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    list<T, Alloc>::assign(obj.cbegin(), obj.cend());
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::~list()
  {
    if (!mFixed)
    {
      PurgeNodePool();

      for (iterator it = begin(); it != end(); ++it)
      {
        DestroyAndDeallocateNode(it.mNode);
      }
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::assign(size_t n, const_reference val)
  {
    node_type* node_ptr = static_cast<node_type*>(mAnchor.mNext);
    for (; (n > 0) && (node_ptr != &mAnchor); --n)
    {
      node_ptr->mValue = val;
      node_ptr = static_cast<node_type*>(node_ptr->mNext);
    }

    if (n)
    {
      insert(iterator(node_ptr), n, val);
    }
    else
    {
      erase(iterator(node_ptr), iterator(&mAnchor));
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::assign(const_iterator first, const_iterator last)
  {
    node_type* node_ptr = static_cast<node_type*>(mAnchor.mNext);
    for (; (first != last) && (node_ptr != &mAnchor); ++first)
    {
      node_ptr->mValue = *first;
      node_ptr = static_cast<node_type*>(node_ptr->mNext);
    }

    if (first != last)
    {
      insert(iterator(node_ptr), first, last);
    }
    else
    {
      erase(iterator(node_ptr), iterator(&mAnchor));
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::assign(const T* first, const T* last)
  {
    node_type* node_ptr = static_cast<node_type*>(mAnchor.mNext);
    for (; (first != last) && (node_ptr != &mAnchor); ++first)
    {
      node_ptr->mValue = *first;
      node_ptr = static_cast<node_type*>(node_ptr->mNext);
    }

    if (first != last)
    {
      insert(iterator(node_ptr), first, last);
    }
    else
    {
      erase(iterator(node_ptr), iterator(&mAnchor));
    }
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::reference list<T, Alloc>::back()
  {
    return static_cast<node_type*>(mAnchor.mPrev)->mValue;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_reference list<T, Alloc>::back() const
  {
    return static_cast<node_type*>(mAnchor.mPrev)->mValue;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::begin()
  {
    return iterator(mAnchor.mNext);
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_iterator list<T, Alloc>::begin() const
  {
    return typename list<T, Alloc>::const_iterator(mAnchor.mNext);
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const
  {
    return typename list<T, Alloc>::const_iterator(mAnchor.mNext);
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const
  {
    return const_iterator(&mAnchor);
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin() const
  {
    return const_reverse_iterator(end());
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const
  {
    return const_reverse_iterator(begin());
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::size_type list<T, Alloc>::capacity()
  {
    return mSize + GetNodePoolSize();
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::clear()
  {
    erase(begin(), end());
  }

  template<class T, class Alloc>
  inline bool list<T, Alloc>::empty() const
  {
    return (0 == size());
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::end()
  {
    return iterator(&mAnchor);
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_iterator list<T, Alloc>::end() const
  {
    return const_iterator(&mAnchor);
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator position)
  {
    //Reassign the pointers of nodes (position--) and (position++) to point to each other.
    position.mNode->mPrev->mNext = position.mNode->mNext;
    position.mNode->mNext->mPrev = position.mNode->mPrev;

    //The list no longer points to position.  It is now safe to erase it.
    //Pushing the node to the pool invalidates the iterator.  Therefore, we must store
    //a temp pointer and increment the iterator BEFORE adding the node to the pool.
    node_type* ptr = position.mNode;
    ++position;

    PushToNodePool(ptr);
    --mSize;

    return position;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last)
  {
    //The erased range is [first,last).  Reassign the pointers of nodes (first-1) and last
    //to point to each other.
    first.mNode->mPrev->mNext = last.mNode;
    last.mNode->mPrev = first.mNode->mPrev;

    //The list no longer points to the range, now it is safe to erase it.
    for (iterator it = first; it != last;)
    {
      //Pushing the node to the pool invalidates the iterator.  Therefore, we must store
      //a temp pointer and increment the iterator BEFORE adding the node to the pool.
      node_type* ptr = it.mNode;
      ++it;
      PushToNodePool(ptr);
      --mSize;
    }

    return last;
  }

  template<class T, class Alloc>
  inline bool list<T, Alloc>::fixed() const
  {
    return mFixed;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::allocator_type list<T, Alloc>::get_allocator() const
  {
    return mAllocator;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::reference list<T, Alloc>::front()
  {
    return static_cast<node_type*>(mAnchor.mNext)->mValue;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_reference list<T, Alloc>::front() const
  {
    return static_cast<node_type*>(mAnchor.mNext)->mValue;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator position, const_reference val)
  {
    node_type* lhs = static_cast<node_type*>(position.mNode->mPrev);

    node_type* new_node = RetrieveNode();
    ++mSize;

    new_node->mPrev = position.mNode->mPrev;
    new_node->mPrev->mNext = new_node;
    new_node->mValue = val;
    new_node->mNext = position.mNode;
    position.mNode->mPrev = new_node;

    return iterator(new_node);
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::insert(iterator position, size_type n, const_reference val)
  {
    node_type* lhs = static_cast<node_type*>(position.mNode->mPrev);
    for (; n > 0; --n)
    {
      node_type* new_node = RetrieveNode();
      ++mSize;

      new_node->mPrev = lhs;
      new_node->mValue = val;

      lhs->mNext = new_node;
      lhs = new_node;
    }
    lhs->mNext = position.mNode;
    position.mNode->mPrev = lhs;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::insert(iterator position, const_iterator first, const_iterator last)
  {
    node_type* lhs = static_cast<node_type*>(position.mNode->mPrev);
    for (; first != last; ++first)
    {
      node_type* new_node = RetrieveNode();
      ++mSize;

      new_node->mPrev = lhs;
      new_node->mValue = *first;

      lhs->mNext = new_node;
      lhs = new_node;
    }
    lhs->mNext = position.mNode;
    position.mNode->mPrev = lhs;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::insert(iterator position, const T* first, const T* last)
  {
    node_type* lhs = static_cast<node_type*>(position.mNode->mPrev);
    for (; first != last; ++first)
    {
      node_type* new_node = RetrieveNode();
      ++mSize;

      new_node->mPrev = lhs;
      new_node->mValue = *first;

      lhs->mNext = new_node;
      lhs = new_node;
    }
    lhs->mNext = position.mNode;
    position.mNode->mPrev = lhs;
  }

  template<class T, class Alloc>
  inline size_t list<T, Alloc>::max_size() const
  {
    return mAllocator.max_size();
  }

  template<class T, class Alloc>
  inline list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::pop_back()
  {
    node_type* back_node = static_cast<node_type*>(mAnchor.mPrev);
    mAnchor.mPrev = back_node->mPrev;
    mAnchor.mPrev->mNext = &mAnchor;
    PushToNodePool(back_node);
    --mSize;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::pop_front()
  {
    node_type* front_node = static_cast<node_type*>(mAnchor.mNext);
    mAnchor.mNext = front_node->mNext;
    mAnchor.mNext->mPrev = &mAnchor;
    PushToNodePool(front_node);
    --mSize;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::push_back(const T& val)
  {
    node_type* new_node = RetrieveNode();
    ++mSize;

    new_node->mPrev = mAnchor.mPrev;
    new_node->mNext = &mAnchor;
    new_node->mValue = val;

    mAnchor.mPrev->mNext = new_node;
    mAnchor.mPrev = new_node;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::push_front(const T& val)
  {
    node_type* new_node = RetrieveNode();
    ++mSize;

    new_node->mNext = mAnchor.mNext;
    new_node->mPrev = &mAnchor;
    new_node->mValue = val;

    mAnchor.mNext->mPrev = new_node;
    mAnchor.mNext = new_node;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin()
  {
    return reverse_iterator(end());
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::rbegin() const
  {
    return const_reverse_iterator(end());
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend()
  {
    return reverse_iterator(begin());
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::rend() const
  {
    return const_reverse_iterator(begin());
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::reserve(size_type n)
  {
    size_type current_capacity = capacity();
    if (n > current_capacity)
    {
      while (current_capacity != n)
      {
        node_type* node_ptr = mAllocator.allocate(1);
        mAllocator.construct(node_ptr, node_type());
        PushToNodePool(node_ptr);
        ++current_capacity;
      }
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::resize(size_type n, const value_type& val)
  {
    if (n < mSize)
    {
      iterator it(mAnchor.mNext);
      for (; n > 0; --n)
      {
        ++it;
      }
      erase(it, end());
    }
    else if (n > mSize)
    {
      insert(end(), n - mSize, val);
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::shrink_to_fit()
  {
    PurgeNodePool();
  }

  template<class T, class Alloc>
  inline size_t list<T, Alloc>::size() const
  {
    return mSize;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::swap(list<T, Alloc>& obj)
  {
    if ((!mFixed) && (!obj.fixed()))
    {
      base_node_type::swap(mAnchor, obj.mAnchor);
      std::swap(mSize, obj.mSize);
      std::swap(mNodePool, obj.mNodePool);
    }
    else
    {
      iterator lhs_it = begin();
      iterator lhs_end = end();
      iterator rhs_it = obj.begin();
      iterator rhs_end = obj.end();
      while ((lhs_it != lhs_end) && (rhs_it != rhs_end))
      {
        std::swap(*lhs_it, *rhs_it);
        ++lhs_it;
        ++rhs_it;
      }
      if (lhs_it != lhs_end)
      {
        obj.insert(rhs_it, lhs_it, lhs_end);
        erase(lhs_it, lhs_end);
      }
      else
      {
        insert(lhs_it, rhs_it, rhs_end);
        obj.erase(rhs_it, rhs_end);
      }
    }
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(size_t capacity, list_node<T>* contentPtr) :
      mAnchor(), mSize(0), mNodePool(NULL), mFixed(true)
  {

  }

  template<class T, class Alloc>
  inline list_node<T>* list<T, Alloc>::RetrieveNode()
  {
    node_type* ptr;
    if (NULL == mNodePool)
    {
      if (mFixed)
      {
        throw std::runtime_error("flex::fixed_list - exceeded capacity");
      }
      else
      {
        ptr = mAllocator.allocate(1);
        mAllocator.construct(ptr, node_type());
      }
    }
    else
    {
      ptr = mNodePool;
      mNodePool = static_cast<node_type*>(mNodePool->mNext);
    }
    return ptr;
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::size_type list<T, Alloc>::GetNodePoolSize()
  {
    size_type n = 0;
    node_type* node_ptr = mNodePool;
    while (node_ptr != NULL)
    {
      node_ptr = static_cast<node_type*>(node_ptr->mNext);
      ++n;
    }
    return n;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::PushToNodePool(list_node<T>* ptr)
  {
    ptr->mNext = mNodePool;
    mNodePool = ptr;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::PurgeNodePool(void)
  {
    while (mNodePool != NULL)
    {
      node_type* next = static_cast<node_type*>(mNodePool->mNext);
      DestroyAndDeallocateNode(mNodePool);
      mNodePool = next;
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::DestroyAndDeallocateNode(list_node<T>* ptr)
  {
    mAllocator.destroy(ptr);
    mAllocator.deallocate(ptr, 1);
  }

  template<class T, class Alloc>
  bool operator==(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
  {
    if (lhs.size() != rhs.size())
    {
      return false;
    }
    else
    {
      typename list<T, Alloc>::const_iterator lit = lhs.begin();
      typename list<T, Alloc>::const_iterator rit = rhs.begin();
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
  bool operator<(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
  {
    if (lhs.size() < rhs.size())
    {
      typename list<T, Alloc>::const_iterator lit = lhs.begin();
      typename list<T, Alloc>::const_iterator rit = rhs.begin();
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
      typename list<T, Alloc>::const_iterator lit = lhs.begin();
      typename list<T, Alloc>::const_iterator rit = rhs.begin();
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
  bool operator!=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
  {
    return !(lhs == rhs);
  }

  template<class T, class Alloc>
  bool operator>(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
  {
    return rhs < lhs;
  }

  template<class T, class Alloc>
  bool operator<=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
  {
    return !(rhs < lhs);
  }

  template<class T, class Alloc>
  bool operator>=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
  {
    return !(lhs < rhs);
  }

} //namespace flex
#endif /* FLEX_LIST_H */
