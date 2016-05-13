#ifndef FLEX_FIXED_LIST_H
#define FLEX_FIXED_LIST_H

#include <flex/list.h>

namespace flex
{

  template<class T, size_t N, class Alloc = allocator<list_node<T> > > class fixed_list: public list<T, Alloc>
  {
  public:
    typedef list<T, Alloc> base_type;

    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::node_type node_type;
    typedef typename base_type::base_node_type base_node_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::difference_type difference_type;

    using base_type::mAnchor;
    using base_type::mNodePool;
    using base_type::mAllocator;
    using base_type::mOverflow;
    using base_type::assign;
    using base_type::begin;
    using base_type::clear;
    using base_type::end;
    using base_type::insert;

    fixed_list();
    explicit fixed_list(size_type size, const value_type& val = value_type());
    fixed_list(int size, const value_type& val);
    template<typename InputIterator> fixed_list(InputIterator first, InputIterator last);
    fixed_list(const fixed_list<T, N, Alloc> & obj);
    fixed_list(const list<T, Alloc> & obj);
    fixed_list(std::initializer_list<value_type> il);
#ifdef FLEX_HAS_CXX11
    fixed_list(fixed_list<T, N, Alloc> && obj);
    fixed_list(list<T, Alloc> && obj);
#endif
    ~fixed_list();

    fixed_list<T, N, Alloc>& operator=(const fixed_list<T, N, Alloc>& obj);
    fixed_list<T, N, Alloc>& operator=(const list<T, Alloc>& obj);
    fixed_list<T, N, Alloc>& operator=(std::initializer_list<value_type> il);
#ifdef FLEX_HAS_CXX11
    fixed_list<T, N, Alloc>& operator=(fixed_list<T, N, Alloc>&& obj);
    fixed_list<T, N, Alloc>& operator=(list<T, Alloc>&& obj);
#endif

  private:
#ifdef FLEX_HAS_CXX11
    typename std::aligned_storage<sizeof(node_type), alignof(node_type)>::type mBuffer[N];
#else
    union
    {
      char mBuffer[N * sizeof(node_type)];
      long double dummy;
    };
#endif
  };

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list() :
      list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list(size_type size, const value_type& val) :
      list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(), size, val);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list(int size, const value_type& val) :
      list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(), (size_type) size, val);
  }

  template<class T, size_t N, class Alloc>
  template<typename InputIterator>
  inline fixed_list<T, N, Alloc>::fixed_list(InputIterator first, InputIterator last) :
      list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(), first, last);
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list(const fixed_list<T, N, Alloc> & obj) :
      list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(), obj.cbegin(), obj.cend());
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list(const list<T, Alloc> & obj) :
      list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(), obj.cbegin(), obj.cend());
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list(std::initializer_list<value_type> il) :
      list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(), il.begin(), il.end());
  }

#ifdef FLEX_HAS_CXX11
  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list(fixed_list<T, N, Alloc> && obj) :
  list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(),std::make_move_iterator(obj.begin()), std::make_move_iterator(obj.end()));
    obj.clear();
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::fixed_list(list<T, Alloc> && obj) :
  list<T, Alloc>((node_type*) mBuffer, ((node_type*) mBuffer) + N)
  {
    insert(begin(),std::make_move_iterator(obj.begin()), std::make_move_iterator(obj.end()));
    obj.clear();
  }
#endif

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>::~fixed_list()
  {
#ifndef FLEX_RELEASE
    if (FLEX_UNLIKELY(mOverflow))
    {
      //If the fixed list overflowed, we want to clean out the nodes that were allocated.
      clear(); //Perform a clear which moves everything to the node pool
      while (mNodePool != NULL)
      {
        node_type* next = static_cast<node_type*>(mNodePool->mNext);
        //A node was allocated if it is outside the range of buffer.  Remember mBuffer + N
        //denotes the end() iterator.  Therefore it is possible that an allocated node could be
        //equal to it.
        if ((mNodePool < (node_type*) mBuffer) || (mNodePool >= ((node_type*) mBuffer) + N))
        {
          mAllocator.deallocate(mNodePool, 1);
        }
        mNodePool = next;
      }
    }
#endif
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(const fixed_list<T, N, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(const list<T, Alloc>& obj)
  {
    assign(obj.begin(), obj.end());
    return *this;
  }

  template<class T, size_t N, class Alloc>
  inline fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(std::initializer_list<value_type> il)
  {
    assign(il.begin(), il.end());
    return *this;
  }

#ifdef FLEX_HAS_CXX11
template<class T, size_t N, class Alloc>
inline fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(fixed_list<T, N, Alloc>&& obj)
{
  assign(std::make_move_iterator(obj.begin()), std::make_move_iterator(obj.end()));
  obj.clear();
  return *this;
}

template<class T, size_t N, class Alloc>
inline fixed_list<T, N, Alloc>& fixed_list<T, N, Alloc>::operator=(list<T, Alloc>&& obj)
{
  assign(std::make_move_iterator(obj.begin()), std::make_move_iterator(obj.end()));
  obj.clear();
  return *this;
}
#endif

}
 //namespace flex

#endif /* FLEX_FIXED_LIST_H */
