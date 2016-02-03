#ifndef FLEX_ALLOCATOR_H
#define FLEX_ALLOCATOR_H

#include <limits>
#include <allocation_guard.h>

namespace flex
{
  template<class T> class allocator: public allocation_guard
  {
  public:
    // type definitions
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    // rebind allocator to type U
    template<class U>
    struct rebind
    {
      typedef allocator<U> other;
    };

    // return address of values
    pointer address(reference value) const
    {
      return &value;
    }
    const_pointer address(const_reference value) const
    {
      return &value;
    }

    /* constructors and destructor
     * - nothing to do because the allocator has no state
     */
    allocator()
    {
    }

    allocator(const allocator&)
    {
    }

    template<class U>
    allocator(const allocator<U>&)
    {
    }

    ~allocator()
    {
    }

    // return maximum number of elements that can be allocated
    size_type max_size() const
    {
      return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    // allocate but don't initialize num elements of type T
    pointer allocate(size_type num, typename std::allocator<void>::const_pointer = 0)
    {
      if (allocation_guard::is_enabled())
      {
        throw std::runtime_error("allocation_guard: allocator performed runtime allocation");
      }
      return reinterpret_cast<pointer>(::operator new(num * sizeof(T)));
    }

    // initialize elements of allocated storage p with value value
    void construct(pointer p, const T& value)
    {
      // initialize memory with placement new
      new ((void*) p) T(value);
    }

    // destroy elements of initialized storage p
    void destroy(pointer p)
    {
      // destroy objects by calling their destructor
      p->~T();
    }

    // deallocate storage p of deleted elements
    void deallocate(pointer p, size_type num)
    {
      ::operator delete((void*) p);
    }
  };

  // return that all specializations of this allocator are interchangeable
  template<class T1, class T2>
  bool operator==(const allocator<T1>&, const allocator<T2>&)
  {
    return true;
  }

  template<class T1, class T2>
  bool operator!=(const allocator<T1>&, const allocator<T2>&)
  {
    return false;
  }
}

#endif /* FLEX_ALLOCATOR_H */
