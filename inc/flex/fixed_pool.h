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

    fixed_pool();

    fixed_pool<T, N, Alloc>& operator=(const fixed_pool<T, N, Alloc>& obj);

  private:
#ifdef FLEX_HAS_CXX11
    typename std::aligned_storage<sizeof(T), alignof(T)>::type mBuffer[N];
#else
    union
    {
      char mBuffer[N * sizeof(node_type)];
      uint64_t dummy;
    };
#endif
  };

  template<class T, size_t N, class Alloc>
  inline fixed_pool<T, N, Alloc>::fixed_pool() :
      pool<T, Alloc>((node_type*) mBuffer, (node_type*) mBuffer + N)
  {
  }

  template<class T, size_t N, class Alloc>
  inline fixed_pool<T, N, Alloc>& fixed_pool<T, N, Alloc>::operator=(const fixed_pool<T, N, Alloc>& obj)
  {
    return *this;
  }

} //namespace flex

#endif /* FLEX_FIXED_POOL_H */
