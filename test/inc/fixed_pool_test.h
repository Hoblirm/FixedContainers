//#include <cxxtest/TestSuite.h>
//
//#include "flex/fixed_pool.h"
//#include <string>
//
//using namespace flex;
//
//class Foo
//{
//public:
//  int id;
//  char* name;
//
//  Foo(void) :
//      id(0)
//  {
//    name = new char[16];
//  }
//  ~Foo(void)
//  {
//    delete[] name;
//  }
//  void reset()
//  {
//    id = 0;
//    name[0] = '\0';
//  }
//};
//
//class fixed_pool_test: public CxxTest::TestSuite
//{
//public:
//
//  // It should return the max_size of the pool
//  void test_max_size(void)
//  {
//    allocation_guard::disable();
//    pool<Foo> pool(10);
//    TS_ASSERT_EQUALS(pool.max_size(), 10);
//  }
//
//  // It should return the available objects in the pool.
//  void test_available(void)
//  {
//    pool<Foo> pool(10);
//    Foo* ptrList[10];
//    for (int i = 10; i > 0; i--)
//    {
//      TS_ASSERT_EQUALS(pool.available(), i);
//      ptrList[i - 1] = pool.allocate();
//    }
//
//    for (int i = 0; i < 10; i++)
//    {
//      TS_ASSERT_EQUALS(pool.available(), i);
//      pool.deallocate(ptrList[i]);
//    }
//  }
//
//  // It should return the number of objects allocated by the pool.
//   void test_outstanding(void)
//   {
//     pool<Foo> pool(10);
//     Foo* ptrList[10];
//     for (int i = 0; i < 10; i++)
//     {
//       TS_ASSERT_EQUALS(pool.outstanding(), i);
//       ptrList[i] = pool.allocate();
//     }
//
//     for (int i = 10; i > 0; i--)
//     {
//       TS_ASSERT_EQUALS(pool.outstanding(), i);
//       pool.deallocate(ptrList[i-1]);
//     }
//   }
//
//  // It should be able to allocate objects in order.
//  void test_allocate_ordered(void)
//  {
//    pool<Foo> pool(3);
//
//    Foo* a = pool.allocate();
//    Foo* b = pool.allocate();
//    pool.deallocate(b);
//    pool.deallocate(a);
//
//    a = pool.allocate();
//    b = pool.allocate();
//
//    a->id = 1;
//    b->id = 2;
//
//    TS_ASSERT_EQUALS(a->id, 1);
//    TS_ASSERT_EQUALS(b->id, 2);
//  }
//
//  // It should be able to allocate objects out of order.
//  void test_allocate_unordered(void)
//  {
//    fixed_pool<Foo, 3> pool;
//
//    Foo* a = pool.allocate();
//    Foo* b = pool.allocate();
//    pool.deallocate(a);
//    pool.deallocate(b);
//
//    a = pool.allocate();
//    b = pool.allocate();
//
//    a->id = 1;
//    b->id = 2;
//
//    TS_ASSERT_EQUALS(a->id, 1);
//    TS_ASSERT_EQUALS(b->id, 2);
//  }
//
//  // It should throw an exception if max_size is exceeded.
//  void test_allocate_exception(void)
//  {
//    bool exception_caught = false;
//    fixed_pool<Foo, 1> pool;
//    try
//    {
//      pool.allocate();
//      pool.allocate();
//    }
//    catch (std::exception& e)
//    {
//      exception_caught = true;
//    }
//    TS_ASSERT(exception_caught);
//
//  }
//
//  void modify_method(pool<Foo>& x)
//  {
//    for (int i = 0; i < x.max_size(); i++)
//    {
//      x.allocate();
//    }
//  }
//
//  void read_method(const pool<Foo>& x)
//  {
//    TS_ASSERT_EQUALS(x.available(), 0);
//  }
//
//  void test_cast_operator(void)
//  {
//    fixed_pool<Foo, 8> a;
//    modify_method(a);
//    read_method(a);
//  }
//
//  void test_constructor(void)
//  {
//    allocation_guard::enable();
//    fixed_pool<Foo, 8> p;
//    TS_ASSERT_EQUALS(p.available(), 8);
//    TS_ASSERT_THROWS(pool<Foo> p2(8), std::runtime_error);
//    allocation_guard::disable();
//    flex::pool<Foo> p2(8);
//    TS_ASSERT_EQUALS(p2.available(), 8);
//  }
//
//};
