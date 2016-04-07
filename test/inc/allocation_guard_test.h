#include <cxxtest/TestSuite.h>

#include <flex/allocation_guard.h>
#include <string>

using namespace flex;
struct guarded_class: public guarded_object
{
  char data[256];
};

class allocation_guard_test: public CxxTest::TestSuite
{
public:

  void setUp()
  {
    errno = 0;
  }

  void tearDown()
  {
    TS_ASSERT(!errno);
  }

  void test_enable_and_disable(void)
  {
    allocation_guard::enable();
    TS_ASSERT(allocation_guard::is_enabled());
    allocation_guard::disable();
    TS_ASSERT(!allocation_guard::is_enabled());
  }

  void test_new_operator(void)
  {
    guarded_class* ptr;

    allocation_guard::enable();
    ptr = new guarded_class;
    TS_ASSERT(errno);
    errno=0;
    allocation_guard::disable();
    delete ptr;

    ptr = new guarded_class;
    TS_ASSERT(!errno);
    //Ensure the object is allocated by writing to it and not getting a seg fault.
    for (int i = 0; i < 256; ++i)
    {
      ptr->data[i] = (char) i;
    }
    delete ptr;
  }

  void test_new_array_operator(void)
  {
    guarded_class* ptr;

    allocation_guard::enable();
    ptr = new guarded_class[2];
    errno=0;
    allocation_guard::disable();
    delete[] ptr;

    ptr = new guarded_class[2];
    TS_ASSERT(!errno);
    delete[] ptr;
  }
};
