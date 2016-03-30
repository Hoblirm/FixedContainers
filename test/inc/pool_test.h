#include <cxxtest/TestSuite.h>

#include "flex/pool.h"
#include "flex/vector.h"

using namespace flex;

class pool_test: public CxxTest::TestSuite
{
public:

  struct obj
  {
    static const int DEFAULT_VAL = 1;
    static const int INIT_KEY = 858599509;

    obj() :
        val(DEFAULT_VAL), init(INIT_KEY)
    {
    }

    obj(int i) :
        val(i), init(INIT_KEY)
    {
    }

    ~obj()
    {
      init = 0;
    }

    obj& operator=(const obj& o)
    {
      val = o.val;
      return *this;
    }

    operator int() const
    {
      return val;
    }

    int val;
    int init;
  };

  bool is_container_valid(pool<obj>& c)
  {
    bool is_valid = true;
    flex::fixed_vector<void*, 128> v;
    while (!c.empty())
    {
      void* ptr = c.allocate();
      if (((obj*) ptr)->init == obj::INIT_KEY)
      {
        is_valid = false;
      }
      v.push_back(ptr);
    }
    while (!v.empty())
    {
      c.deallocate(v.back());
      v.pop_back();
    }
    return is_valid;
  }

  void test_default_constructor()
  {
    /*
     * Case1: Ensure it doesn't allocate memory.
     */
    flex::allocation_guard::enable();
    pool<obj> a;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    flex::allocation_guard::disable();
  }

  void test_fill_constructor()
  {
    /*
     * Case1: Ensure it reserves memory.
     */
    pool<obj> a(16);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);

    /*
     * Case2: Ensure it throws if allocation guard is enabled.
     */
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(pool<obj> b(16), std::runtime_error)
    flex::allocation_guard::disable();
  }

  void test_destructor()
  {
    /*
     * Case1: Ensure it works on empty container.
     */
    pool<obj> a;
    a.~pool();
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Ensure it cleans a populated container
     */
    pool<obj> b(16);
    b.~pool();
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 0);
  }

  void test_allocate()
  {
    /*
     * Case1: Ensure throw keeps container in valid state:
     */
    pool<obj> a;
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(a.allocate(), std::runtime_error)
    flex::allocation_guard::disable();
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Ensure it works on empty container.
     */
    obj* ptr = (obj*) a.allocate();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    new ((void*) ptr) obj(7);
    TS_ASSERT_EQUALS(ptr->val, 7);
    a.deallocate((void*) ptr);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 1);

    /*
     * Case2: Ensure it works on populated container
     */
    pool<obj> b(16);
    flex::allocation_guard::enable();
    ptr = (obj*) b.allocate();
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 15);
    new ((void*) ptr) obj(7);
    TS_ASSERT_EQUALS(ptr->val, 7);
    b.deallocate((void*) ptr);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 16);
    flex::allocation_guard::disable();
  }

  void test_deallocate()
  {

  }

  void test_construct()
  {
    /*
     * Case1: Ensure it works on empty container.
     */
    pool<obj> a;
    obj* ptr = a.construct();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(ptr->val, obj::DEFAULT_VAL);
    a.destruct(ptr);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 1);

    /*
     * Case2: Ensure it works on populated container
     */
    pool<obj> b(16);
    flex::allocation_guard::enable();
    ptr = b.construct();
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 15);
    TS_ASSERT_EQUALS(ptr->val, obj::DEFAULT_VAL);
    b.destruct(ptr);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 16);
    flex::allocation_guard::disable();
  }

  void test_construct_val()
  {
    /*
     * Case1: Ensure it works on empty container.
     */
    pool<obj> a;
    obj* ptr = a.construct(7);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(ptr->val, 7);
    a.destruct(ptr);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 1);

    /*
     * Case2: Ensure it works on populated container
     */
    pool<obj> b(16);
    flex::allocation_guard::enable();
    ptr = b.construct(7);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 15);
    TS_ASSERT_EQUALS(ptr->val, 7);
    b.destruct(ptr);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 16);
    flex::allocation_guard::disable();
  }

  void test_assignment_operator()
  {
    /*
     * Case1: Ensure assignment operator is no-op
     */
    pool<obj> a;
    pool<obj> b(16);
    a = b;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
  }

  void test_reserve()
  {
    /*
     * Case1: Ensure throw keeps container in valid state:
     */
    pool<obj> a;
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(a.reserve(16), std::runtime_error)
    flex::allocation_guard::disable();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);

    /*
     * Case2: Ensure it allocates to an empty container
     */
    a.reserve(16);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);

    /*
     * Case3: Ensure it allocates to a populated container
     *
     */
    a.reserve(8);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 24);
  }

};
