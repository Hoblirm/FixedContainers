#ifndef FLEX_LIST_H
#define FLEX_LIST_H

#include <flex/list_iterator.h>
#include <flex/allocator.h>

namespace flex
{

  template<class T, class Alloc = allocator<list_node<T> > > class list: public guarded_object
  {
  public:
    typedef list<T, Alloc> this_type;

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
    list(int size, const T& val);
    template<typename InputIterator> list(InputIterator first, InputIterator last);
    list(const list<T, Alloc> & obj);
    ~list();

    void assign(size_t size, const_reference val);
    void assign(int size, const_reference val);
    template<typename InputIterator> void assign(InputIterator first, InputIterator last);

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
    bool full() const;
    iterator insert(iterator position, const_reference val);
    void insert(iterator position, size_type n, const_reference val);
    void insert(iterator position, int n, const_reference val);
    template<typename InputIterator> void insert(iterator position, InputIterator first, InputIterator last);

    size_type max_size() const;
    void merge(this_type& x);
    template<typename Compare> void merge(this_type& x, Compare comp);

    list<T, Alloc>& operator=(const list<T, Alloc>& obj);

    void pop_back();
    void pop_front();
    void push_back(const_reference val);
    void push_front(const_reference val);

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

    void remove(const T& x);
    template<typename Predicate> void remove_if(Predicate pred);

    void reserve(size_type n);
    void resize(size_type n, const value_type& val = value_type());

    void reverse(void);

    void shrink_to_fit();
    size_t size() const;

    void sort();
    template<typename Compare> void sort(Compare comp);

    void splice(iterator position, this_type& x);
    void splice(iterator position, this_type& x, iterator i);
    void splice(iterator position, this_type& x, iterator first, iterator last);

    void swap(list<T, Alloc>& obj);
    void unique();
    template<typename BinaryPredicate> void unique(BinaryPredicate binary_pred);

  protected:

    list(node_type* first, node_type* last);

    /*
     * Internal methods used by the public sort() method.
     */
    iterator merge(iterator lhs_first, iterator rhs_first, iterator rhs_last);
    template<typename Compare> iterator merge(iterator lhs_first, iterator rhs_first, iterator rhs_last, Compare comp);
    template<typename Compare> iterator sort(iterator first, iterator last, size_type n, Compare comp);
    void splice(iterator position, iterator first, iterator last);

    node_type* AllocateNode();
    node_type* RetrieveNode(const value_type& val);
    size_type GetNodePoolSize();
    void FillNodePool(size_type n);
    void PushToNodePool(node_type* ptr);
    void PushRangeToNodePool(iterator first, iterator last);
    void PurgeNodePool();

    Alloc mAllocator;
    bool mFixed;

    /*
     * The anchor node contains the head and tail pointers for the list.  It is type base_node_type, and doesn't take up
     * any space for a T value.  An anchor node provides various advantages over using a simple head and tail pointer.
     * It simplifies logic when the list is empty, and when modifications are around list end-points.  The anchor node
     * also allows the implementation of an end() iterator that can be decremented.
     */
    base_node_type mAnchor;
    size_type mSize;
    node_type* mNodePool;

  private:

  };

  template<class T, class Alloc>
  inline list<T, Alloc>::list() :
      mFixed(false), mSize(0), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(size_type size, const T& val) :
      mFixed(false), mSize(0), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    insert(begin(), size, val);
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(int size, const T& val) :
      mFixed(false), mSize(0), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    insert(begin(), (size_type) size, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline list<T, Alloc>::list(InputIterator first, InputIterator last) :
      mFixed(false), mSize(0), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    insert(begin(), first, last);
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(const list<T, Alloc> & obj) :
      mFixed(false), mSize(0), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    insert(begin(), obj.cbegin(), obj.cend());
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::~list()
  {
    flex::destruct_range(begin(), end());

    if (!mFixed)
    {
      for (iterator it = begin(); it != end(); ++it)
      {
        mAllocator.deallocate(it.mNode, 1);
      }

      PurgeNodePool();
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
  inline void list<T, Alloc>::assign(int n, const_reference val)
  {
    //The purpose of this method is to prevent ambiguity issues in the case where
    //both n and val are type int.  Without this method, the compiler will incorrectly
    //use the templated insert() method assuming n and val are type InputIterator.
    assign((size_type) n, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline void list<T, Alloc>::assign(InputIterator first, InputIterator last)
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
    return (0 == mSize);
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
    if (first != last)
    {
      PushRangeToNodePool(first, last);

      //The erased range is [first,last).  Reassign the pointers of nodes (first-1) and last
      //to point to each other.
      first.mNode->mPrev->mNext = last.mNode;
      last.mNode->mPrev = first.mNode->mPrev;
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
  inline bool list<T, Alloc>::full() const
  {
    return (NULL == mNodePool);
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator position, const_reference val)
  {
    node_type* lhs = static_cast<node_type*>(position.mNode->mPrev);

    node_type* new_node = RetrieveNode(val);
    ++mSize;

    new_node->mPrev = position.mNode->mPrev;
    new_node->mPrev->mNext = new_node;
    new_node->mNext = position.mNode;
    position.mNode->mPrev = new_node;

    return iterator(new_node);
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::insert(iterator position, size_type n, const_reference val)
  {
    node_type* lhs = static_cast<node_type*>(position.mNode->mPrev);

    const size_type new_size = mSize + n;

    //Iterate through the node pool and insert nodes until the pool is
    //empty, or n nodes have been inserted.
    if (mNodePool != NULL)
    {
      lhs->mNext = mNodePool;
      for (; ((n > 0) && (mNodePool != NULL)); --n)
      {
        node_type* new_node = mNodePool;
        new ((void*) &new_node->mValue) value_type(val);
        mNodePool = static_cast<node_type*>(mNodePool->mNext);
        new_node->mPrev = lhs;

        //Note: lhs->mNext does not need to be assigned in this loop
        //as all pool nodes are linked by lhs->next.  This would change
        //if the implementation of the node pool was altered.
        lhs = new_node;
      }
    }

    //If n is not zero, the node pool was exhausted.  Continue inserting
    //nodes by performing memory allocations.
    for (; n > 0; --n)
    {
      //The list is currently in an invalid state, so we must catch the
      //allocation if it throws.
      node_type* new_node;

#ifndef FLEX_RELEASE
      try
      {
#endif
        new_node = AllocateNode();
#ifndef FLEX_RELEASE
      }
      catch (...)
      {
        //Put the list back in a valid state.
        lhs->mNext = position.mNode;
        position.mNode->mPrev = lhs;
        throw;
      }
#endif
      new ((void*) &new_node->mValue) value_type(val);
      new_node->mPrev = lhs;

      lhs->mNext = new_node;
      lhs = new_node;
    }
    lhs->mNext = position.mNode;
    position.mNode->mPrev = lhs;
    mSize = new_size;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::insert(iterator position, int n, const_reference val)
  {
    //The purpose of this method is to prevent ambiguity issues in the case where
    //both n and val are type int.  Without this method, the compiler will incorrectly
    //use the templated insert() method assuming n and val are type InputIterator.
    insert(position, (size_type) n, val);
  }

  template<class T, class Alloc>
  template<typename InputIterator>
  inline void list<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
  {
    node_type* lhs = static_cast<node_type*>(position.mNode->mPrev);

    //Iterate through the node pool and insert nodes until the pool is
    //empty, or n nodes have been inserted.  Prior to starting the loop
    //we assign lhs->mNext to the first node in the pool, as it doesn't
    //get assigned in the loop.  If it turns out the pool is empty,
    //lhs->mNext will be reassigned in the second loop.
    lhs->mNext = mNodePool;
    for (; ((first != last) && (mNodePool != NULL)); ++first)
    {
      node_type* new_node = mNodePool;
      new ((void*) &new_node->mValue) value_type(*first);
      mNodePool = static_cast<node_type*>(mNodePool->mNext);
      new_node->mPrev = lhs;

      ++mSize;

      new_node->mPrev = lhs;

      //Note: lhs->mNext does not need to be assigned in this loop
      //as all pool nodes are linked by lhs->next.  This would change
      //if the implementation of the node pool was altered.
      lhs = new_node;
    }

    //If n is not zero, the node pool was exhausted.  Continue inserting
    //nodes by performing memory allocations.
    for (; first != last; ++first)
    {
      //The list is currently in an invalid state, so we must catch the
      //allocation if it throws.
      node_type* new_node;
#ifndef FLEX_RELEASE
      try
      {
#endif
        new_node = AllocateNode();
#ifndef FLEX_RELEASE
      }
      catch (...)
      {
        //Put the list back in a valid state.
        lhs->mNext = position.mNode;
        position.mNode->mPrev = lhs;
        throw;
      }
#endif
      new ((void*) &new_node->mValue) value_type(*first);
      ++mSize;

      new_node->mPrev = lhs;

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
  inline void list<T, Alloc>::merge(this_type& x)
  {
    if (this != &x)
    {
      iterator lhs_first(begin());
      iterator rhs_first(x.begin());
      const iterator lhs_last(end());
      const iterator rhs_last(x.end());

      while ((lhs_first != lhs_last) && (rhs_first != rhs_last))
      {
        if (*rhs_first < *lhs_first)
        {
          iterator splice_begin = rhs_first;
          ++rhs_first;
          while ((*rhs_first < *lhs_first) && (rhs_first != rhs_last))
          {
            ++rhs_first;
          }
          splice(lhs_first, x, splice_begin, rhs_first);
        }
        ++lhs_first;
      }

      if (rhs_first != rhs_last)
        splice(lhs_last, x, rhs_first, rhs_last);
    }
  }

  template<class T, class Alloc>
  template<typename Compare>
  inline void list<T, Alloc>::merge(this_type& x, Compare comp)
  {
    if (this != &x)
    {
      iterator lhs_first(begin());
      iterator rhs_first(x.begin());
      const iterator lhs_last(end());
      const iterator rhs_last(x.end());

      while ((lhs_first != lhs_last) && (rhs_first != rhs_last))
      {
        if (comp(*rhs_first, *lhs_first))
        {
          iterator splice_begin = rhs_first;
          ++rhs_first;
          while ((comp(*rhs_first, *lhs_first)) && (rhs_first != rhs_last))
          {
            ++rhs_first;
          }
          splice(lhs_first, x, splice_begin, rhs_first);
        }
        ++lhs_first;
      }

      if (rhs_first != rhs_last)
        splice(lhs_last, x, rhs_first, rhs_last);
    }
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::merge(iterator lhs_first, iterator rhs_first,
      iterator rhs_last)
  {
    iterator front;
    if (*rhs_first < *lhs_first)
    {
      front = rhs_first;
    }
    else
    {
      front = lhs_first;
    }

    while ((lhs_first != rhs_first) && (rhs_first != rhs_last))
    {
      if (*rhs_first < *lhs_first)
      {
        iterator splice_begin = rhs_first;
        ++rhs_first;
        while ((*rhs_first < *lhs_first) && (rhs_first != rhs_last))
        {
          ++rhs_first;
        }
        splice(lhs_first, splice_begin, rhs_first);
      }
      ++lhs_first;
    }

    return front;
  }

  template<class T, class Alloc>
  template<typename Compare>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::merge(iterator lhs_first, iterator rhs_first,
      iterator rhs_last, Compare comp)
  {
    iterator front;
    if (*rhs_first < *lhs_first)
    {
      front = rhs_first;
    }
    else
    {
      front = lhs_first;
    }

    while ((lhs_first != rhs_first) && (rhs_first != rhs_last))
    {
      if (comp(*rhs_first, *lhs_first))
      {
        iterator splice_begin = rhs_first;
        ++rhs_first;
        while (comp(*rhs_first, *lhs_first) && (rhs_first != rhs_last))
        {
          ++rhs_first;
        }
        splice(lhs_first, splice_begin, rhs_first);
      }
      ++lhs_first;
    }

    return front;
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
    node_type* new_node = RetrieveNode(val);
    ++mSize;

    new_node->mPrev = mAnchor.mPrev;
    new_node->mNext = &mAnchor;

    mAnchor.mPrev->mNext = new_node;
    mAnchor.mPrev = new_node;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::push_front(const T& val)
  {
    node_type* new_node = RetrieveNode(val);
    ++mSize;

    new_node->mNext = mAnchor.mNext;
    new_node->mPrev = &mAnchor;

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

  template<typename T, typename Alloc>
  inline void list<T, Alloc>::remove(const value_type& value)
  {
    iterator it((base_node_type*) mAnchor.mNext);

    while (it.mNode != &mAnchor)
    {
      if (*it == value)
      {
        ++it;
        erase((base_node_type*) it.mNode->mPrev);
      }
      else
      {
        ++it;
      }
    }
  }

  template<typename T, typename Alloc>
  template<typename Predicate>
  inline void list<T, Alloc>::remove_if(Predicate pred)
  {
    iterator it((base_node_type*) mAnchor.mNext);

    while (it.mNode != &mAnchor)
    {
      if (pred(*it))
      {
        ++it;
        erase((base_node_type*) it.mNode->mPrev);
      }
      else
      {
        ++it;
      }
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::reserve(size_type n)
  {
    size_type current_capacity = capacity();
    if (n > current_capacity)
    {
      FillNodePool(n - current_capacity);
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
  inline void list<T, Alloc>::reverse()
  {
    base_node_type* node_ptr = &mAnchor;
    do
    {
      base_node_type * const tmp = node_ptr->mNext;
      node_ptr->mNext = node_ptr->mPrev;
      node_ptr->mPrev = tmp;
      node_ptr = node_ptr->mPrev;
    }
    while (node_ptr != &mAnchor);
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::shrink_to_fit()
  {
    if (FLEX_LIKELY(!mFixed))
    {
      PurgeNodePool();
    }
  }

  template<class T, class Alloc>
  inline size_t list<T, Alloc>::size() const
  {
    return mSize;
  }

  template<typename T, typename Alloc>
  inline void list<T, Alloc>::sort()
  {
    std::less<value_type> comp;
    sort(begin(), end(), size(), comp);
  }

  template<typename T, typename Alloc>
  template<typename Compare>
  inline void list<T, Alloc>::sort(Compare comp)
  {
    sort(begin(), end(), size(), comp);
  }

  template<typename T, typename Alloc>
  template<typename Compare>
  inline typename list<T, Alloc>::iterator list<T, Alloc>::sort(iterator first, iterator last, size_type n,
      Compare comp)
  {
    switch (n)
    {
    case 0:
    case 1:
      return first;
    case 2: //Optimize when size is 2 (Not required)
      if (comp(*--last, *first))
      {
        last.mNode->remove();
        last.mNode->insert(first.mNode);
        return last;
      }
      return first;
    case 3: //Optimize when size is 3 (Not required)
    {
      iterator mid = first;
      ++mid;
      --last;
      //Unroll every combination of three elements.
      if (comp(*mid, *first))
      {
        if (comp(*last, *mid))
        {
          //3 2 1
          list_node_base::swap_not_empty(*first.mNode, *last.mNode);
          return last;
        }
        else if (comp(*last, *first))
        {
          //3 1 2
          first.mNode->remove();
          first.mNode->insert(last.mNode->mNext);
          return mid;
        }
        else
        {
          //2 1 3
          mid.mNode->remove();
          mid.mNode->insert(first.mNode);
          return mid;
        }
      }
      else if (comp(*last, *first))
      {
        //2 3 1
        last.mNode->remove();
        last.mNode->insert(first.mNode);
        return last;
      }
      else if (comp(*last, *mid))
      {
        //1 3 2
        last.mNode->remove();
        last.mNode->insert(mid.mNode);
        return first;
      }
      // 1 2 3
      return first;
    } //end of case 3
    default: //merge sort for all larger sizes
    {
      size_type new_size = n / 2;
      iterator mid(first);
      std::advance(mid, new_size);

      iterator lhs_front = sort(first, mid, new_size, comp);
      iterator rhs_front = sort(mid, last, n - new_size, comp);

      return merge(lhs_front, rhs_front, last, comp);
    } //end of default
    } //end of switch
  }

  template<typename T, typename Alloc>
  inline void list<T, Alloc>::splice(iterator position, this_type& x)
  {
    if (x.mSize)
    {
      if ((!mFixed) && (!x.fixed()))
      {
        ((base_node_type*) position.mNode)->splice((base_node_type*) x.mAnchor.mNext, (base_node_type*) &x.mAnchor);
        mSize += x.mSize;
        x.mSize = 0;
      }
      else
      {

        insert(position, x.begin(), x.end());
        x.clear();
      }
    }
  }

  template<typename T, typename Alloc>
  inline void list<T, Alloc>::splice(iterator position, list& x, iterator i)
  {
    if ((!mFixed) && (!x.fixed()))
    {
      iterator last(i);
      ++last;
      if ((position != i) && (position != last))
      {
        ((base_node_type*) position.mNode)->splice((base_node_type*) i.mNode, (base_node_type*) last.mNode);

        ++mSize;
        --x.mSize;
      }
    }
    else
    {

      insert(position, *i);
      x.erase(i);
    }
  }

  template<typename T, typename Alloc>
  inline void list<T, Alloc>::splice(iterator position, this_type& x, iterator first, iterator last)
  {
    if ((!mFixed) && (!x.fixed()))
    {
      const size_type n = (size_type) std::distance(first, last);

      if (n)
      {
        ((base_node_type*) position.mNode)->splice((base_node_type*) first.mNode, (base_node_type*) last.mNode);
        mSize += n;
        x.mSize -= n;
      }
    }
    else
    {

      insert(position, first, last);
      x.erase(first, last);
    }
  }

  template<typename T, typename Alloc>
  inline void list<T, Alloc>::splice(iterator position, iterator first, iterator last)
  {
    if (first != last)
    {
      ((base_node_type*) position.mNode)->splice((base_node_type*) first.mNode, (base_node_type*) last.mNode);
    }
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

  template<typename T, typename Alloc>
  inline void list<T, Alloc>::unique()
  {
    /*
     * Based on the standard, we shall delete all but the first element of each consecutive group.  We will iterate
     * backwards, as it is simpler to remove elements behind the iterator.  Although it shouldn't matter which unique
     * elements are deleted, it is best to follow the standard just in case a dependency exists on the behavior.
     */
    node_type* begin_ptr = static_cast<node_type*>(mAnchor.mPrev);
    node_type* end_ptr = static_cast<node_type*>(&mAnchor);

    while (begin_ptr != end_ptr)
    {
      const iterator prev(begin_ptr);
      begin_ptr = static_cast<node_type*>(begin_ptr->mPrev);
      if (begin_ptr->mValue == *prev)
      {

        erase(prev);
      }
    }
  }

  template<typename T, typename Alloc>
  template<typename BinaryPredicate>
  inline void list<T, Alloc>::unique(BinaryPredicate binary_pred)
  {
    /*
     * Based on the standard, we shall delete all but the first element of each consecutive group.  We will iterate
     * backwards, as it is simpler to remove elements behind the iterator.  Although it shouldn't matter which unique
     * elements are deleted, it is best to follow the standard just in case a dependency exists on the behavior.
     */
    node_type* begin_ptr = static_cast<node_type*>(mAnchor.mPrev);
    node_type* end_ptr = static_cast<node_type*>(&mAnchor);

    while (begin_ptr != end_ptr)
    {
      const iterator prev(begin_ptr);
      begin_ptr = static_cast<node_type*>(begin_ptr->mPrev);
      if (binary_pred(begin_ptr->mValue, *prev))
      {

        erase(prev);
      }
    }
  }

  template<class T, class Alloc>
  inline list<T, Alloc>::list(node_type* first, node_type* last) :
      mFixed(true), mSize(0), mNodePool(NULL)
  {
    mAnchor.mNext = mAnchor.mPrev = &mAnchor;
    if (first != last)
    {
      mNodePool = first;
      node_type* prev = first;
      ++first;
      for (; first != last; ++first)
      {
        prev->mNext = first;
        prev = first;
      }
      prev->mNext = NULL;
    }
  }

  template<class T, class Alloc>
  inline typename list<T, Alloc>::node_type* list<T, Alloc>::AllocateNode()
  {
    FLEX_THROW_OUT_OF_RANGE_IF(mFixed, "flex::fixed_list - exceeded capacity");
    return mAllocator.allocate(1);
  }

  template<class T, class Alloc>
  inline list_node<T>* list<T, Alloc>::RetrieveNode(const value_type& val)
  {
    node_type* ptr;
    if (NULL == mNodePool)
    {
      ptr = AllocateNode();
      new ((void*) &ptr->mValue) value_type(val);
    }
    else
    {
      ptr = mNodePool;
      new ((void*) &ptr->mValue) value_type(val);
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
  inline void list<T, Alloc>::FillNodePool(size_type n)
  {
    for (; n; --n)
    {
      node_type* node_ptr = mAllocator.allocate(1);
      node_ptr->mNext = mNodePool;
      mNodePool = node_ptr;
    }
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::PushToNodePool(list_node<T>* ptr)
  {
    ptr->mValue.~value_type();
    ptr->mNext = mNodePool;
    mNodePool = ptr;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::PushRangeToNodePool(iterator first, iterator last)
  {
    //It is worth mentioning that this routine will put the list in an invalid
    //state if first==last.  It is expected that this method is only called in
    //a context in which first and last have been checked to not be equal.  The
    //check is omitted in this function as it is most efficient for the check
    //to be done at a higher level.
    iterator it = first;
    for (; it != last; ++it)
    {
      it.mNode->mValue.~value_type();
      --mSize;
    }
    last.mNode->mPrev->mNext = mNodePool;
    mNodePool = first.mNode;
  }

  template<class T, class Alloc>
  inline void list<T, Alloc>::PurgeNodePool(void)
  {
    while (mNodePool != NULL)
    {
      node_type* next = static_cast<node_type*>(mNodePool->mNext);
      mAllocator.deallocate(mNodePool, 1);
      mNodePool = next;
    }
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
