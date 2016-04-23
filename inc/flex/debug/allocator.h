#ifndef FLEX_DEBUG_ALLOCATOR_H
#define FLEX_DEBUG_ALLOCATOR_H

#include <flex/allocator.h>
#include <map>
#include <set>

namespace flex
{
  namespace debug
  {
    template<class T> class allocator: public flex::allocator<T>
    {
    public:
      // type definitions
      typedef flex::allocator<T> base_type;
      typedef typename base_type::value_type value_type;
      typedef typename base_type::pointer pointer;
      typedef typename base_type::const_pointer const_pointer;
      typedef typename base_type::reference reference;
      typedef typename base_type::const_reference const_reference;
      typedef typename base_type::size_type size_type;
      typedef typename base_type::difference_type difference_type;

      inline allocator()
      {
      }

      template<class U>
      inline allocator(const allocator<U>&)
      {
      }

      pointer allocate(size_type num, typename std::allocator<void>::const_pointer hint = 0)
      {
        pointer p = base_type::allocate(num, hint);
        if (NULL != p)
          mAllocatedPointers.insert(std::pair<void*, size_type>((void*) p, num));
        return p;
      }

      // initialize elements of allocated storage p with value value
      void construct(pointer p, const T& value)
      {
        mConstructedPointers.insert(p);
        base_type::construct(p, value);
      }

      // destroy elements of initialized storage p
      void destroy(pointer p)
      {
        typename std::set<void*>::iterator it = mConstructedPointers.find(p);
        if (it == mConstructedPointers.end())
        {
          throw std::runtime_error("flex::debug::allocator.destroy() - invalid pointer");
        }
        else
        {
          mConstructedPointers.erase(it);
        }
        base_type::destroy(p);
      }

      // deallocate storage p of deleted elements
      void deallocate(pointer p, size_type num)
      {
        deallocate((void*) p, num);
      }

      inline void deallocate(void* p, size_type num)
      {
        if (NULL != p)
        {
          typename std::map<void*, size_type>::iterator it = mAllocatedPointers.find(p);
          if (it == mAllocatedPointers.end())
          {
            throw std::runtime_error("flex::debug::allocator.deallocate() - invalid pointer");
          }
          else
          {
            if (it->second != num)
            {
              throw std::runtime_error("flex::debug::allocator.deallocate() - incorrect size deallocated");
            }
            else
            {
              mAllocatedPointers.erase(it);
            }
          }
        }
        base_type::deallocate(p, num);
      }

      static void clear()
      {
        mAllocatedPointers.clear();
        mConstructedPointers.clear();
      }

      static std::map<void*, size_type> mAllocatedPointers;
      static std::set<void*> mConstructedPointers;
    };

    template<class T> std::map<void*, typename allocator<T>::size_type> allocator<T>::mAllocatedPointers;
    template<class T> std::set<void*> allocator<T>::mConstructedPointers;
  } //namespace debug
} //namespace flex

#endif /* FLEX_DEBUG_ALLOCATOR_H */
