#ifndef FLEX_POOL_H
#define FLEX_POOL_H

#include <flex/allocator.h>

namespace flex
{

  //The method in which the pool organizes its memory is a bit of an optimization hack.  Objects that
  //are stored in the pool are uninitialized.  Therefore, it is valid to use this uninitialized data
  //for other purposes.  The pool container takes advantage of this by storing a linked list of pointers
  //over-top of the uninitialized objects.  This linked list keeps track of which objects are available
  //to be retrieved from the pool.  Each uninitialized object has a single pointer stored on top of it
  //that points to the next available object.  The last object stores a NULL pointer.  This implementation
  //saves memory as no pointers need to be allocated, and caching performance may be a bit better (more so
  //for fixed_pool) as the pointers are stored in the same location as the objects.  The pool_link
  //struct is the data type used by the pool to create this linked list.
  struct pool_link
  {
    pool_link* mNext;
  };

  //Since the pool_link is stored over-top of the objects, the node size cannot be smaller than pool_link.
#define FLEX_POOL_NODE_SIZE(a) (sizeof(a) < sizeof(pool_link)) ? sizeof(pool_link) : sizeof(a)

  //Define a generic object that has the same size as a pool object node.  This is only needed
  //so we have an object to pass to the allocator.
  template<size_t N>
  struct pool_node
  {
    char data[N];
  };

  template<class T, class Alloc = flex::allocator<pool_node<FLEX_POOL_NODE_SIZE(T)> > > class pool: guarded_object
  {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef pool_node<FLEX_POOL_NODE_SIZE(T)> node_type;
    typedef size_t size_type;

    pool();
    explicit pool(size_type n);
    ~pool();

    void* allocate();
#ifdef FLEX_HAS_CXX11
    template<class...Args> pointer construct(Args&&... val);
#else
    pointer construct();
    pointer construct(const value_type& val);
#endif
    void destruct(pointer ptr);
    void deallocate(void* ptr);
    bool empty() const;
    pool& operator=(const pool&);
    void reserve(size_type n);
    size_type size() const;

  protected:
    pool_link* mHead;
    Alloc mAllocator;
    bool mFixed;
    bool mOverflow;

    pool(node_type* first, node_type* last);

    void* AllocateNewObject();
  };

  template<class T, class Alloc>
  inline pool<T, Alloc>::pool() :
      mHead(NULL), mFixed(false), mOverflow(false)
  {
  }

  template<class T, class Alloc>
  inline pool<T, Alloc>::pool(size_type n) :
      mHead(NULL), mFixed(false), mOverflow(false)
  {
    reserve(n);
  }

  template<class T, class Alloc>
  inline pool<T, Alloc>::~pool()
  {
    if (!mFixed)
    {
      //Only want to retrieve pointers when mHead is set (aka !empty()).
      //Otherwise the pool will internally allocate new objects.
      while (mHead)
      {
        //Do not confuse the mAllocator allocate/deallocate methods with the pool methods.  The pool's
        //allocate() method is called to remove a pointer from the pool.  This is then deleted
        //by mAllocator.  Think of the allocate() method as pool.pop_front().
        mAllocator.deallocate((node_type*)allocate(), 1);
      }
    }
  }

  template<class T, class Alloc>
  inline void* pool<T, Alloc>::allocate()
  {
    //Retrieve a pointer from the pool.  mHead points to the first available location.
    if (mHead)
    {
      //Remember, a link to the next object is stored over-top of the uninitialized object
      //that we are about to return.  Set head to the new link, and return the object pointer.
      pool_link* ptr = mHead;
      mHead = mHead->mNext;
      return ptr;
    }
    else
    {
      //Head is NULL which means the list is empty.  Attempt to allocate a new object.
      return AllocateNewObject();
    }
  }

#ifdef FLEX_HAS_CXX11
  template<class T, class Alloc>
  template<class... Args>
  inline typename pool<T, Alloc>::pointer pool<T, Alloc>::construct(Args&&... args)
  {
    void* ptr = allocate();
    new (ptr) value_type(std::forward<Args>(args)...);
    return (pointer) ptr;
  }
#else
  template<class T, class Alloc>
  inline typename pool<T, Alloc>::pointer pool<T, Alloc>::construct()
  {
    void* ptr = allocate();
    new (ptr) value_type();
    return (pointer) ptr;
  }

  template<class T, class Alloc>
  inline typename pool<T, Alloc>::pointer pool<T, Alloc>::construct(const value_type& val)
  {
    void* ptr = allocate();
    new (ptr) value_type(val);
    return (pointer) ptr;
  }
#endif

  template<class T, class Alloc>
  inline void pool<T, Alloc>::deallocate(void* ptr)
  {
    //Return the object pointer to the pool.  The object data should already be uninitialized.
    //Write the pool's front node pointer, aka mHead, over top of the object.
    ((pool_link*) ptr)->mNext = mHead;

    //Now set the head of the pool to the returned object pointer.
    mHead = ((pool_link*) ptr);
  }

  template<class T, class Alloc>
  inline void pool<T, Alloc>::destruct(pointer ptr)
  {
    ptr->~value_type();
    deallocate(ptr);
  }

  template<class T, class Alloc>
  inline bool pool<T, Alloc>::empty() const
  {
    return (mHead == NULL);
  }

  template<class T, class Alloc>
  inline typename pool<T, Alloc>::size_type pool<T, Alloc>::size() const
  {
    size_type n = 0;
    for (pool_link* it = mHead; it; it = it->mNext)
    {
      ++n;
    }
    return n;
  }

  template<class T, class Alloc>
  inline pool<T, Alloc>& pool<T, Alloc>::operator=(const pool&)
  {
    return *this;
  }

  template<class T, class Alloc>
  inline void pool<T, Alloc>::reserve(size_type n)
  {
    for (; n; --n)
    {
      //The pool's deallocate method simply adds a new pointer to the pool. Despite the
      //name, it doesn't perform an actual deallocation.  Think of the deallocate method
      //as pool.push_front() for the purpose it serves here.
      deallocate(AllocateNewObject());
    }
  }

  template<class T, class Alloc>
  inline pool<T, Alloc>::pool(node_type* first, node_type* last) :
      mHead(NULL), mFixed(true), mOverflow(false)
  {
    for (node_type* it = first; it != last; ++it)
    {
      //Similar to the reserve method, deallocate() is working like pool.push_front().
      deallocate((void*) it);
    }
  }

  template<class T, class Alloc>
  inline void* pool<T, Alloc>::AllocateNewObject()
  {
#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(mFixed))
    {
      if (!mOverflow)
      {
        mOverflow = true;
        flex::error_msg("fixed_pool: exceeded capacity");
      }
    }
#endif

    return mAllocator.allocate(1);
  }

} //namespace flex
#endif /* FLEX_POOL_H */
