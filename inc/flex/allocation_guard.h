#ifndef FLEX_ALLOCATION_GUARD_H
#define FLEX_ALLOCATION_GUARD_H

#include <cstdlib>
#include <exception>
#include <stdexcept>

namespace flex
{

  class allocation_guard
  {
  public:
    static void enable();
    static void disable();
    static bool is_enabled();

    static void* operator new(std::size_t sz)
    {
      if (allocation_guard::is_enabled())
      {
        throw std::runtime_error("allocation_guard: new operator called for child class.");
      }
      else
      {
        return ::operator new(sz);
      }
    }

    static void* operator new[](std::size_t sz)
    {
      if (allocation_guard::is_enabled())
      {
        throw std::runtime_error("allocation_guard: new[] operator called for child class.");
      }
      else
      {
        return ::operator new(sz);
      }
    }
  protected:
    allocation_guard();
  private:
    static bool sEnabled;
  };

  bool allocation_guard::sEnabled = false;

  allocation_guard::allocation_guard()
  {
  }

  void allocation_guard::enable()
  {
    sEnabled = true;
  }

  void allocation_guard::disable()
  {
    sEnabled = false;
  }

  bool allocation_guard::is_enabled()
  {
    return sEnabled;
  }
}

#endif /* FLEX_ALLOCATION_GUARD_H */
