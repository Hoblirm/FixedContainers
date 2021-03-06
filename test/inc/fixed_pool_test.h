#include <cxxtest/TestSuite.h>

#include "flex/fixed_pool.h"
#include "flex/fixed_vector.h"
#include "flex/debug/obj.h"

using namespace flex;

class fixed_pool_test: public CxxTest::TestSuite
{
public:

  typedef flex::debug::obj obj;
  typedef flex::fixed_pool<obj, 16> pool_obj;

  void setUp()
  {
    flex::allocation_guard::enable();
    errno = 0;
  }

  void tearDown()
  {
    TS_ASSERT(!errno);
    flex::allocation_guard::disable();
  }

  bool is_container_valid(pool_obj& c)
  {
    bool is_valid = true;
    flex::fixed_vector<void*, 128> v;
    size_t n = 0;
    while (!c.empty())
    {
      void* ptr = c.allocate();
      if (((obj*) ptr)->init == obj::INIT_KEY)
      {
        printf("Error: Expected (((obj*) ptr)->init == obj::INIT_KEY) when n=%zu,found (%d == %d)\n", n,
            ((obj*) ptr)->init, obj::INIT_KEY);
        is_valid = false;
      }
      v.push_back(ptr);
      ++n;
    }
    while (!v.empty())
    {
      c.deallocate(v.back());
      v.pop_back();
    }
    return is_valid;
  }

  void test_pool_node_size()
  {
    /*
     * Case1: Object size is larger than link.
     */
    TS_ASSERT_EQUALS(FLEX_POOL_NODE_SIZE(obj), sizeof(obj));

    /*
     * Case2: Object size is smaller than link.
     */
    TS_ASSERT_EQUALS(FLEX_POOL_NODE_SIZE(char), sizeof(pool_link));
  }

  void test_default_constructor()
  {
    /*
     * Case1: Default case.
     */
    pool_obj a;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);
  }

  void test_destructor()
  {
    /*
     * Case1: Ensure it does nothing for fixed container
     */
    pool_obj a;
    a.~pool();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);
  }

  void test_allocate()
  {
    /*
     * Case1: Ensure it works on populated container
     */
    pool_obj a;
    obj* ptr = (obj*) a.allocate();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 15);
    new ((void*) ptr) obj(7);
    TS_ASSERT_EQUALS(ptr->val, 7);
    ptr->~obj();
    a.deallocate((void*) ptr);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);

    /*
     * Case2: Ensure throw keeps container in valid state:
     */
    flex::fixed_vector<void*, 16> v;
    for (int i = 0; i < 16; ++i)
    {
      v.push_back(a.allocate());
    }
    TS_ASSERT(!errno);
    v.push_back(a.allocate());
    TS_ASSERT(errno);
    errno = 0;

    TS_ASSERT(is_container_valid(a));

    //Clean up to prevent memory leak.
    for (int i = 0; i < 17; ++i)
    {
      a.deallocate(v.back());
      v.pop_back();
    }
  }

  void test_deallocate()
  {
    /*
     * Case1: It maintains order for multiple allocates/deallocates.
     */
    fixed_vector<void*, 16> v;
    pool_obj a;

    for (int i = 0; i < 16; ++i)
    {
      v.push_back(a.allocate());
    }
    TS_ASSERT(is_container_valid(a));

    for (int i = 15; i >= 0; --i)
    {
      a.deallocate(v[i]);
    }
    TS_ASSERT(is_container_valid(a));

    for (int i = 0; i < 16; ++i)
    {
      TS_ASSERT_EQUALS(v[i], a.allocate());
    }
    TS_ASSERT(is_container_valid(a));

    //Just for the sake of preventing memory leaks.
    for (int i = 15; i >= 0; --i)
    {
      a.deallocate(v[i]);
    }
  }

  void test_construct()
  {
    /*
     * Case1: Ensure it works on populated container
     */
    pool_obj a;
    obj* ptr = a.construct();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 15);
    TS_ASSERT_EQUALS(ptr->val, obj::DEFAULT_VAL);
    a.destruct(ptr);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);
  }

  void test_construct_val()
  {
    /*
     * Case1: Ensure it works on populated container
     */
    pool_obj a;
    obj* ptr = a.construct(7);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 15);
    TS_ASSERT_EQUALS(ptr->val, 7);
    a.destruct(ptr);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);
  }

  void test_assignment_operator()
  {
    /*
     * Case1: Ensure assignment operator is no-op
     */
    pool_obj a;
    pool_obj b;
    a = b;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);
  }

  void test_reserve()
  {
    /*
     * Case1: Ensure reserves works in overflow condition
     */
    pool_obj a;
    TS_ASSERT(!errno);
    a.reserve(32);
    TS_ASSERT(errno);
    errno = 0;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 48);
  }

};
