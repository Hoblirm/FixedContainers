#ifndef FLEX_FIXED_POOL_H
#define FLEX_FIXED_POOL_H

#include <flex/pool.h>

namespace flex
{

  template<class T, size_t N, class Alloc = flex::allocator<pool_node<FLEX_POOL_NODE_SIZE(T)> > > class fixed_pool: public pool<
      T, Alloc>
  {
  public:
    typedef pool<T, Alloc> base_type;

    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::node_type node_type;
    typedef typename base_type::size_type size_type;

    using base_type::mHead;
    using base_type::mAllocator;
    using base_type::mOverflow;
    using base_type::allocate;

    fixed_pool();
    ~fixed_pool();

    fixed_pool<T, N, Alloc>& operator=(const fixed_pool<T, N, Alloc>& obj);

  private:
#ifdef FLEX_HAS_CXX11
    typename std::aligned_storage<sizeof(T), alignof(T)>::type mBuffer[N];
#else
    union
    {
      char mBuffer[N * sizeof(node_type)];
      long double dummy;
    };
#endif
  };

  template<class T, size_t N, class Alloc>
  inline fixed_pool<T, N, Alloc>::fixed_pool() :
      pool<T, Alloc>((node_type*) mBuffer, (node_type*) mBuffer + N)
  {
  }

  template<class T, size_t N, class Alloc>
  inline fixed_pool<T, N, Alloc>::~fixed_pool()
  {
#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(mOverflow))
    {
      //Only want to retrieve pointers when mHead is set (aka !empty()).
      //Otherwise the pool will internally allocate new objects.
      while (mHead)
      {
        //Do not confuse the mAllocator allocate/deallocate methods with the pool methods.  The pool's
        //allocate() method is called to remove a pointer from the pool.  This is then deleted
        //by mAllocator.  Think of the allocate() method as pool.pop_front().
        node_type* ptr = (node_type*)allocate();

        //A node was allocated if it is outside the range of buffer.  Remember mBuffer + N
        //denotes the end() iterator.  Therefore it is possible that an allocated node could be
        //equal to it.
        if ((ptr < (node_type*) mBuffer) || (ptr >= ((node_type*) mBuffer) + N))
        {
          mAllocator.deallocate(ptr, 1);
        }
      }
    }
#endif
  }

  template<class T, size_t N, class Alloc>
  inline fixed_pool<T, N, Alloc>& fixed_pool<T, N, Alloc>::operator=(const fixed_pool<T, N, Alloc>& obj)
  {
    return *this;
  }

} //namespace flex

#endif /* FLEX_FIXED_POOL_H */
