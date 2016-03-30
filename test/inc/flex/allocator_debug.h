#ifndef FLEX_ALLOCATOR_DEBUG_H
#define FLEX_ALLOCATOR_DEBUG_H

#include <flex/allocator.h>
#include <map>
#include <set>

namespace flex
{
  template<class T> class allocator_debug: public allocator<T>
  {
  public:
    // type definitions
    typedef allocator<T> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::const_pointer const_pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::difference_type difference_type;

    pointer allocate(size_type num, typename std::allocator<void>::const_pointer hint = 0)
    {
      pointer p = allocator<T>::allocate(num, hint);
      if (NULL != p)
        mAllocatedPointers.insert(std::pair<void*, size_type>((void*)p, num));
      return p;
    }

    // initialize elements of allocated storage p with value value
    void construct(pointer p, const T& value)
    {
      mConstructedPointers.insert(p);
      allocator<T>::construct(p, value);
    }

    // destroy elements of initialized storage p
    void destroy(pointer p)
    {
      typename std::set<void*>::iterator it = mConstructedPointers.find(p);
      if (it == mConstructedPointers.end())
      {
        throw std::runtime_error("flex::allocator_debug.destroy() - invalid pointer");
      }
      else
      {
        mConstructedPointers.erase(it);
      }
      allocator<T>::destroy(p);
    }

    // deallocate storage p of deleted elements
    void deallocate(pointer p, size_type num)
    {
      deallocate((void*)p,num);
    }

    inline void deallocate(void* p, size_type num)
    {
      if (NULL != p)
      {
        typename std::map<void*, size_type>::iterator it = mAllocatedPointers.find(p);
        if (it == mAllocatedPointers.end())
        {
          throw std::runtime_error("flex::allocator_debug.deallocate() - invalid pointer");
        }
        else
        {
          if (it->second != num)
          {
            throw std::runtime_error("flex::allocator_debug.deallocate() - incorrect size deallocated");
          }
          else
          {
            mAllocatedPointers.erase(it);
          }
        }
      }
      allocator<T>::deallocate(p, num);
    }
    
    static void clear()
    {
      mAllocatedPointers.clear();
      mConstructedPointers.clear();
    }

    static std::map<void*, size_type> mAllocatedPointers;
    static std::set<void*> mConstructedPointers;
  };

  template<class T> std::map<void*, typename allocator_debug<T>::size_type> allocator_debug<T>::mAllocatedPointers;
  template<class T> std::set<void*> allocator_debug<T>::mConstructedPointers;

}

#endif /* FLEX_ALLOCATOR_DEBUG_H */
