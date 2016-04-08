#ifndef FLEX_ALLOCATION_GUARD_H
#define FLEX_ALLOCATION_GUARD_H

#include <flex/config.h>

namespace flex
{

  struct guarded_object
  {
    inline static void* operator new(std::size_t sz)
    {
      FLEX_ERROR_MSG_IF(sAllocationGuardEnabled, "guarded_object: new operator called for child class");
      return ::operator new(sz);
    }

    inline static void* operator new[](std::size_t sz)
    {
      FLEX_ERROR_MSG_IF(sAllocationGuardEnabled, "guarded_object: new[] operator called for child class");
      return ::operator new(sz);

    }
  protected:
    static bool sAllocationGuardEnabled;
  };

  bool guarded_object::sAllocationGuardEnabled = false;

  class allocation_guard: public guarded_object
  {
  public:

    inline static void enable()
    {
      sAllocationGuardEnabled = true;
    }

    inline static void disable()
    {
      sAllocationGuardEnabled = false;
    }

    inline static bool is_enabled()
    {
      return sAllocationGuardEnabled;
    }

  private:
    allocation_guard()
    {
    }

  };
}

#endif /* FLEX_ALLOCATION_GUARD_H */
