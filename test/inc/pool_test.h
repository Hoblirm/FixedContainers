#include <cxxtest/TestSuite.h>

#include "flex/pool.h"
#include "flex/fixed_vector.h"

using namespace flex;

class pool_test: public CxxTest::TestSuite
{
public:

  struct obj
  {
    static const int DEFAULT_VAL = 1;
    static const int INIT_KEY = 858599509;

    obj() :
        link_ptr(NULL), val(DEFAULT_VAL), init(INIT_KEY)
    {
    }

    obj(int i) :
        link_ptr(NULL), val(i), init(INIT_KEY)
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

    //The pool uses the first eight bytes of each object to link all its available memory with pointers.
    //We create a placeholder here so the remaining data is not overwritten.  This allows us to read the object
    //after calling the pool's deallocate method.  Reading objects after they are returned to the pool is
    //useful for testing the pool's validity.
    void* link_ptr;

    int val;
    int init;
  };

  typedef flex::pool<obj, flex::allocator_debug<flex::pool<obj>::node_type> > pool_obj;

  void setUp()
  {
    flex::allocator_debug<flex::pool<obj>::node_type>::clear();
    errno = 0;
  }

  void tearDown()
  {
    TS_ASSERT(!errno);

    //This ensures that all objs constructed by the container have their destructors called.
    TS_ASSERT(flex::allocator_debug<flex::pool<obj>::node_type>::mConstructedPointers.empty());

    //This ensures that all memory allocated by the container is properly freed.
    TS_ASSERT(flex::allocator_debug<flex::pool<obj>::node_type>::mAllocatedPointers.empty());
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
     * Case1: Ensure it doesn't allocate memory.
     */
    flex::allocation_guard::enable();
    pool_obj a;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    flex::allocation_guard::disable();
  }

  void test_fill_constructor()
  {
    /*
     * Case1: Ensure it reserves memory.
     */
    pool_obj a(16);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);

    /*
     * Case2: Ensure it throws if allocation guard is enabled.
     */
    flex::allocation_guard::enable();
    pool_obj b(16);
    TS_ASSERT(errno);
    errno = 0;
    flex::allocation_guard::disable();
  }

  void test_destructor()
  {
    /*
     * Case1: Ensure it works on empty container.
     */
    pool_obj a;
    a.~pool();
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Ensure it cleans a populated container
     */
    pool_obj b(16);
    b.~pool();
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 0);
  }

  void test_allocate()
  {
    /*
     * Case1: Ensure throw keeps container in valid state:
     */
    pool_obj a;
    flex::allocation_guard::enable();
    obj* ptr = (obj*)a.allocate();
    TS_ASSERT(errno);
    errno = 0;
    flex::allocation_guard::disable();
    a.deallocate((void*) ptr);
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Ensure it works on empty container.
     */
    ptr = (obj*) a.allocate();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    new ((void*) ptr) obj(7);
    TS_ASSERT_EQUALS(ptr->val, 7);
    ptr->~obj();
    a.deallocate((void*) ptr);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 1);

    /*
     * Case2: Ensure it works on populated container
     */
    pool_obj b(16);
    flex::allocation_guard::enable();
    ptr = (obj*) b.allocate();
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 15);
    new ((void*) ptr) obj(7);
    TS_ASSERT_EQUALS(ptr->val, 7);
    ptr->~obj();
    b.deallocate((void*) ptr);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 16);
    flex::allocation_guard::disable();
  }

  void test_deallocate()
  {
    /*
     * Case1: It maintains order for multiple allocates/deallocates.
     */
    fixed_vector<void*, 16> v;
    pool_obj a(8);

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
     * Case1: Ensure it works on empty container.
     */
    pool_obj a;
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
    pool_obj b(16);
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
    pool_obj a;
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
    pool_obj b(16);
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
    pool_obj a;
    pool_obj b(16);
    a = b;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
  }

  void test_reserve()
  {
    /*
     * Case1: Ensure throw keeps container in valid state:
     */
    pool_obj a;
    flex::allocation_guard::enable();
    a.reserve(16);
    TS_ASSERT(errno);
    errno = 0;
    flex::allocation_guard::disable();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 16);

    /*
     * Case2: Ensure it allocates to an empty container
     */
    a.reserve(16);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 32);

    /*
     * Case3: Ensure it allocates to a populated container
     *
     */
    a.reserve(8);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 40);
  }

};
