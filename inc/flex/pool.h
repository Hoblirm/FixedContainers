#ifndef FLEX_POOL_H
#define FLEX_POOL_H

#include <flex/allocation_guard.h>

namespace flex
{

  struct Link
  {
    Link* mNext;
  };

  template<size_t N>
  struct PoolNode
  {
    char data[N];
  };

  template<class T/*, class Alloc = flex::allocator<PoolNode<(sizeof(T) < sizeof(Link)) ? sizeof(Link) : sizeof(T)>  >*/> class pool: guarded_object
  {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;

    pool();
    explicit pool(size_type n);
    ~pool();

    void* allocate();
    pointer construct();
    pointer construct(const value_type& val);
    void destruct(pointer ptr);
    void deallocate(void* ptr);
    bool empty() const;
    pool& operator=(const pool&);
    void reserve(size_type n);
    size_type size() const;

  protected:
    Link* mHead;
    size_t mObjectSize;
    bool mFixed;
  };

  template<class T>
  inline pool<T>::pool() :
      mHead(NULL), mObjectSize((sizeof(T) < sizeof(Link)) ? sizeof(Link) : sizeof(T)), mFixed(false)
  {
  }

  template<class T>
  inline pool<T>::pool(size_t n) :
      mHead(NULL), mObjectSize((sizeof(T) < sizeof(Link)) ? sizeof(Link) : sizeof(T)), mFixed(false)
  {
    reserve(n);
  }

  template<class T>
  inline pool<T>::~pool()
  {
    if (!mFixed)
    {
      //Only want to retrieve pointers when mHead is set.  Otherwise the pool will internally
      //allocate new objects.
      while (mHead)
      {
        //Allocate retrieves an available pointer in the pool which we will then delete.
        void* ptr = allocate();
        ::operator delete(ptr);
      }
    }
  }

  template<class T>
  inline void* pool<T>::allocate()
  {
    if (mHead)
    {
      Link* ptr = mHead;
      mHead = ptr->mNext;
      return ptr;
    }
    else
    {
      if (FLEX_UNLIKELY(mFixed || sAllocationGuardEnabled))
      {
        throw std::runtime_error("pool: performed runtime allocation");
      }
      return ::operator new(mObjectSize);
    }
  }

  template<class T>
  inline typename pool<T>::pointer pool<T>::construct()
  {
    void* ptr = allocate();
    new (ptr) value_type();
    return (pointer) ptr;
  }

  template<class T>
  inline typename pool<T>::pointer pool<T>::construct(const value_type& val)
  {
    void* ptr = allocate();
    new (ptr) value_type(val);
    return (pointer) ptr;
  }

  template<class T>
  inline void pool<T>::deallocate(void* ptr)
  {
    ((Link*) ptr)->mNext = mHead;
    mHead = ((Link*) ptr);
  }

  template<class T>
  inline void pool<T>::destruct(pointer ptr)
  {
    ptr->~value_type();
    deallocate(ptr);
  }

  template<class T>
  inline bool pool<T>::empty() const
  {
    return (mHead == NULL);
  }

  template<class T>
  inline typename pool<T>::size_type pool<T>::size() const
  {
    size_type n = 0;
    for (Link* it = mHead; it; it = it->mNext)
    {
      ++n;
    }
    return n;
  }

  template<class T>
  inline pool<T>& pool<T>::operator=(const pool&)
  {
    return *this;
  }

  template<class T>
  inline void pool<T>::reserve(size_type n)
  {
    for (; n; --n)
    {
      if (FLEX_UNLIKELY(mFixed || sAllocationGuardEnabled))
      {
        throw std::runtime_error("pool: performed runtime allocation");
      }

      //The pool's deallocate method simply adds a new pointer to the pool. Despite the
      //name, it doesn't perform an actual deallocation.  In this method, it is treated like
      //a push_front() method.  We simply allocate a new object, and push the pointer to the pool.
      deallocate(::operator new(mObjectSize));
    }
  }

} //namespace flex
#endif /* FLEX_POOL_H */
