#include <cxxtest/TestSuite.h>

#include "object_pool.h"
#include <string>

using namespace std;

class Foo {
  public:
  int id;
  char* name;

  Foo(void) { name = new char[16];}
  ~Foo(void){ delete[] name;}
  void reset() {
    id = 0;
    name[0] = '\0';
  }
};

class object_pool_test : public CxxTest::TestSuite {
  public:

    // It should return the capacity of the pool
    void testCapacity(void) {
      object_pool<Foo> pool(10);
      TS_ASSERT_EQUALS(pool.capacity(),10);
    }

    // It should return the available objects in the pool.
    void testAvailable(void) {
       object_pool<Foo> pool(10);
       Foo* ptrList[10];
       for (int i=10;i>0;i--) {
          TS_ASSERT_EQUALS(pool.available(),i);
          ptrList[i-1] = pool.allocate();
       }
       
       for (int i=0;i<10;i++) {
          TS_ASSERT_EQUALS(pool.available(),i);
          pool.release(ptrList[i]);
       }
    }

    // It should be able to allocate objects in order.
    void testOrderAllocate(void) {
      object_pool<Foo> pool(3);

      Foo* a = pool.allocate();
      Foo* b = pool.allocate();
      pool.release(b);
      pool.release(a);

      a = pool.allocate();
      b = pool.allocate();

      a->id = 1;
      b->id = 2;

      TS_ASSERT_EQUALS(a->id,1);
      TS_ASSERT_EQUALS(b->id,2);
    }

    // It should be able to allocate objects out of order.
    void testUnorderedAllocate(void){
      object_pool<Foo> pool(3);

      Foo* a = pool.allocate();
      Foo* b = pool.allocate();
      pool.release(a);
      pool.release(b);

      a = pool.allocate();
      b = pool.allocate();

      a->id = 1;
      b->id = 2;

      TS_ASSERT_EQUALS(a->id,1);
      TS_ASSERT_EQUALS(b->id,2);
    }

    // It should reset an object after it is released.
    void testResetOnRelease(void){
      object_pool<Foo> pool(1);
      Foo* a = pool.allocate();
      a->id = 1337;
      pool.release(a);
      a = pool.allocate();
      TS_ASSERT_DIFFERS(1337,a->id);
    }

    // It should throw an exception if capacity is exceeded.
    void testAllocateException(void){
      bool exception_caught = false;
      object_pool<Foo> pool(1);
      try{
        pool.allocate();
        pool.allocate();
      }
      catch(exception& e) {
        exception_caught = true;
      }
      TS_ASSERT(exception_caught);

    }

};
