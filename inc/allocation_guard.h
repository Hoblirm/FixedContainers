#ifndef ALLOCATION_GUARD_H
#define ALLOCATION_GUARD_H

#include <cstdlib>
#include <exception>

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

private:
  static bool sEnabled;
};

bool allocation_guard::sEnabled = false;

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

#endif /* ALLOCATION_GUARD_H */
