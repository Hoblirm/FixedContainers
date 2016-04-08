#ifndef FLEX_ALLOCATOR_H
#define FLEX_ALLOCATOR_H

#include <limits>//For std::numeric_limits used in max_size()

#include <flex/allocation_guard.h>

namespace flex
{
  template<class T> class allocator: public guarded_object
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
    inline allocator()
    {
    }

    inline allocator(const allocator&)
    {
    }

    template<class U>
    inline allocator(const allocator<U>&)
    {
    }

    inline ~allocator()
    {
    }

    // return maximum number of elements that can be allocated
    inline size_type max_size() const
    {
      return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    // allocate but don't initialize num elements of type T
    //TODO: May want to consider changing this method to use void*.  This is most accurate
    //since it returns uninitialized data. For the flex::pool class, it actually wants a
    //void* returned.  Since this method casts to pointer, flex::pool must cast back to
    //void*.  Would be cleanest to change this method and update other containers.
    inline pointer allocate(size_type num, typename std::allocator<void>::const_pointer = 0)
    {
      FLEX_ERROR_MSG_IF(sAllocationGuardEnabled, "flex_allocator: performed allocation when guard was enabled");
      return reinterpret_cast<pointer>(::operator new(num * sizeof(T)));
    }

    // initialize elements of allocated storage p with value value
    inline void construct(pointer p, const T& value)
    {
      // initialize memory with placement new
      new ((void*) p) T(value);
    }

    // destroy elements of initialized storage p
    inline void destroy(pointer p)
    {
      // destroy objects by calling their destructor
      p->~T();
    }

    // deallocate storage p of deleted elements
    //TODO: May want to consider removing this method in favor of void*.  It prevents
    //a double cast in cases where void* is being deallocated.
    inline void deallocate(pointer p, size_type num)
    {
      ::operator delete((void*) p);
    }

    inline void deallocate(void* p, size_type num)
    {
      ::operator delete(p);
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
