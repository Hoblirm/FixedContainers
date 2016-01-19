#include <cxxtest/TestSuite.h>

#include <allocation_guard.h>
#include <string>

class guarded_class: public allocation_guard
{
};

class allocation_guard_test: public CxxTest::TestSuite
{
public:

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
    TS_ASSERT_THROWS(ptr = new guarded_class, std::runtime_error);

    allocation_guard::disable();
    TS_ASSERT_THROWS_NOTHING(ptr = new guarded_class);
    delete ptr;
  }

  void test_new_array_operator(void)
  {
    guarded_class* ptr;

    allocation_guard::enable();
    TS_ASSERT_THROWS(ptr = new guarded_class[2], std::runtime_error);

    allocation_guard::disable();
    TS_ASSERT_THROWS_NOTHING(ptr = new guarded_class[2]);
    delete[] ptr;
  }
};
