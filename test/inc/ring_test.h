#include <cxxtest/TestSuite.h>

#include <flex/ring.h>
#include <flex/allocator_debug.h>

typedef flex::ring<int, flex::allocator_debug<int> > ring_int;

class ring_test: public CxxTest::TestSuite
{
public:

  void setUp()
  {
    flex::allocator_debug<int>::clear();
  }

  void tearDown()
  {
    //This ensures that all objects constructed by the container have their destructors called.
    TS_ASSERT(flex::allocator_debug<int>::mConstructedPointers.empty());

    //This ensures that all memory allocated by the container is properly freed.
    TS_ASSERT(flex::allocator_debug<int>::mAllocatedPointers.empty());
  }

  void test_at(void)
  {
    unsigned size = 3;
    ring_int a(size);
    TS_ASSERT_THROWS(a.at(-1), std::out_of_range);
    for (int i = 0; i < size; i++)
    {
      a.at(i) = i;
      TS_ASSERT_EQUALS(a.at(i), a[i]);
    }
    TS_ASSERT_THROWS(a.at(size), std::out_of_range);
  }

  void test_at_const(void)
  {
    unsigned size = 3;
    const ring_int a(size, 7);
    TS_ASSERT_THROWS(const int& v = a.at(-1), std::out_of_range);
    for (int i = 0; i < size; i++)
    {
      const int& v = a.at(i);
      TS_ASSERT_EQUALS(v, a[i]);
    }
    TS_ASSERT_THROWS(const int& v = a.at(size), std::out_of_range);
  }

  void test_back(void)
  {
    unsigned size = 3;
    ring_int a(size);
    a[size - 1] = size - 1;
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_back_const(void)
  {
    unsigned size = 3;
    const ring_int a(3, 7);
    TS_ASSERT_EQUALS(a.back(), 7);
  }

  void test_begin_and_end(void)
  {
    unsigned size = 3;

    ring_int a(size);
    for (int i = 0; i < size; i++)
    {
      a[i] = i;
    }

    int i = 0;
    for (ring_int::iterator it = a.begin(); it != a.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      *it = 0; //Ensure it is not const.
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_begin_and_end_const(void)
  {
    unsigned size = 3;

    const ring_int a(3, 7);

    int i = 0;
    for (ring_int::const_iterator it = a.begin(); it != a.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 7);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

//  void test_cbegin_and_cend(void)
//  {
//    unsigned size = 3;
//
//    const ring_int a;
//    for (int i = 0; i < size; i++)
//    {
//      a[i] = i;
//    }
//
//    int i = 0;
//    for (ring_int::const_iterator it = a.cbegin(); it < a.cend(); ++it)
//    {
//      TS_ASSERT_EQUALS(*it, i);
//      ++i;
//    }
//    TS_ASSERT_EQUALS(i, size);
//  }
//
//  void test_crbegin_and_crend(void)
//  {
//    unsigned size = 3;
//
//    const ring_int a;
//    for (int i = 0; i < size; i++)
//    {
//      a[i] = i;
//    }
//
//    int i = size - 1;
//    for (ring_int::const_reverse_iterator it = a.crbegin(); it < a.crend(); ++it)
//    {
//      TS_ASSERT_EQUALS(*it, i);
//      --i;
//    }
//    TS_ASSERT_EQUALS(i, -1);
//  }

  void test_empty(void)
  {
    unsigned size = 3;
    ring_int a(size);
    TS_ASSERT_EQUALS(a.empty(), false);
    ring_int b;
    TS_ASSERT_EQUALS(b.empty(), true);
  }

  void test_front(void)
  {
    unsigned size = 3;
    ring_int a(size);
    a[0] = size;
    TS_ASSERT_EQUALS(a.front(), size);
  }

  void test_front_const(void)
  {
    unsigned size = 3;
    const ring_int a(3, 7);
    TS_ASSERT_EQUALS(a.front(), 7);
  }

  void test_max_size(void)
  {
    const ring_int a;
    //  TS_ASSERT_EQUALS(a.max_size(), a.get_allocator().max_size());
  }

  void test_ary_operator(void)
  {
    unsigned size = 3;
    ring_int a(size);
    for (int i = 0; i < size; i++)
    {
      a[i] = i;
      TS_ASSERT_EQUALS(a[i], a[i]);
    }
  }

  void test_ary_operator_const(void)
  {
    unsigned size = 3;
    const ring_int a(size, 7);
    for (int i = 0; i < size; i++)
    {
      const int& v = a[i];
      TS_ASSERT_EQUALS(v, a[i]);
    }
  }

//  void test_rbegin_and_rend(void)
//  {
//    unsigned size = 3;
//
//    ring_int a;
//    for (int i = 0; i < size; i++)
//    {
//      a[i] = i;
//    }
//
//    int i = size - 1;
//    for (ring_int::reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
//    {
//      TS_ASSERT_EQUALS(*it, i);
//      *it = 0; //Ensure it is not const.
//      --i;
//    }
//    TS_ASSERT_EQUALS(i, -1);
//  }
//
//  void test_rbegin_and_rend_const(void)
//  {
//    unsigned size = 3;
//
//    const ring_int a;
//    for (int i = 0; i < size; i++)
//    {
//      a[i] = i;
//    }
//
//    int i = size - 1;
//    for (ring_int::const_reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
//    {
//      TS_ASSERT_EQUALS(*it, i);
//      --i;
//    }
//    TS_ASSERT_EQUALS(i, -1);
//  }

  void test_size(void)
  {
    unsigned size = 3;
    const ring_int a(3);
    TS_ASSERT_EQUALS(a.size(), size);
  }

//  void test_swap(void)
//  {
//    unsigned size = 5;
//
//    ring_int first;
//    first[0] = 10;
//    first[1] = 20;
//    first[2] = 30;
//    first[3] = 40;
//    first[4] = 50;
//
//    ring_int second;
//    second[0] = 11;
//    second[1] = 22;
//    second[2] = 33;
//    second[3] = 44;
//    second[4] = 55;
//
//    first.swap(second);
//
//    TS_ASSERT_EQUALS(first[0], 11);
//    TS_ASSERT_EQUALS(first[1], 22);
//    TS_ASSERT_EQUALS(first[2], 33);
//    TS_ASSERT_EQUALS(first[3], 44);
//    TS_ASSERT_EQUALS(first[4], 55);
//
//    TS_ASSERT_EQUALS(second[0], 10);
//    TS_ASSERT_EQUALS(second[1], 20);
//    TS_ASSERT_EQUALS(second[2], 30);
//    TS_ASSERT_EQUALS(second[3], 40);
//    TS_ASSERT_EQUALS(second[4], 50);
//  }

  void test_constructor(void)
  {
    flex::allocation_guard::enable();
    ring_int a;
    TS_ASSERT_EQUALS(a.size(), 0);
    flex::allocation_guard::disable();
  }

//  void test_copy_constructor(void)
//  {
//    ring_int a;
//    for (int i = 0; i < a.size(); i++)
//    {
//      a.data()[i] = i;
//    }
//
//    ring_int b(a);
//    for (int i = 0; i < b.size(); i++)
//    {
//      TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
//    }
//  }

//  void test_assignment_operator(void)
//  {
//    //#1 Test default assignment.
//    ring_int foo;
//    foo[0] = 1;
//    foo[1] = 5;
//    foo[2] = 17;
//
//    ring_int bar;
//    bar.fill(2);
//
//    bar = foo;
//    TS_ASSERT_EQUALS(bar.size(), 3);
//    TS_ASSERT_EQUALS(bar[0], 1);
//    TS_ASSERT_EQUALS(bar[1], 5);
//    TS_ASSERT_EQUALS(bar[2], 17);
//
//    //Ensure that assignments to bar doesn't impact the value of foo.
//    bar.fill(0);
//    TS_ASSERT_EQUALS(foo.size(), 3);
//    TS_ASSERT_EQUALS(foo[0], 1);
//    TS_ASSERT_EQUALS(foo[1], 5);
//    TS_ASSERT_EQUALS(foo[2], 17);
//
//    ring_int bar2;
//    bar2.fill(2);
//
//    //#2 Test assignment with specialized parameter.
//    bar2 = foo;
//    TS_ASSERT_EQUALS(bar2.size(), 3);
//    TS_ASSERT_EQUALS(bar2[0], 1);
//    TS_ASSERT_EQUALS(bar2[1], 5);
//    TS_ASSERT_EQUALS(bar2[2], 17);
//
//    //Ensure that assignments to bar2 doesn't impact the value of foo.
//    bar2.fill(0);
//    TS_ASSERT_EQUALS(foo.size(), 3);
//    TS_ASSERT_EQUALS(foo[0], 1);
//    TS_ASSERT_EQUALS(foo[1], 5);
//    TS_ASSERT_EQUALS(foo[2], 17);
//
//    //#3 Test specialized assignment.
//    ring_int foo2;
//    foo2[0] = 1;
//    foo2[1] = 5;
//    foo2[2] = 17;
//
//    bar2.fill(2);
//
//    bar2 = foo;
//    TS_ASSERT_EQUALS(bar2.size(), 3);
//    TS_ASSERT_EQUALS(bar2[0], 1);
//    TS_ASSERT_EQUALS(bar2[1], 5);
//    TS_ASSERT_EQUALS(bar2[2], 17);
//
//    //Ensure that assignments to bar2 doesn't impact the value of foo.
//    bar2.fill(0);
//    TS_ASSERT_EQUALS(foo2.size(), 3);
//    TS_ASSERT_EQUALS(foo2[0], 1);
//    TS_ASSERT_EQUALS(foo2[1], 5);
//    TS_ASSERT_EQUALS(foo2[2], 17);
//  }

  void test_relational_operators(void)
  {
    int aAry[5] = { 10, 20, 30, 40, 50 };
    int bAry[5] = { 10, 20, 30, 40, 50 };
    int cAry[5] = { 50, 40, 30, 20, 10 };
    ring_int a(5);
    ring_int b(5);
    ring_int c(5);
    for (int i = 0; i < 5; ++i)
    {
      a[i] = b[i] = (i * 10) + 10;
      c[i] = 50 - (i * 10);
    }

    TS_ASSERT((a == b));
    TS_ASSERT(b != c);
    TS_ASSERT(b < c);
    TS_ASSERT(c > b);
    TS_ASSERT(a <= b);
    TS_ASSERT(a >= b);
  }

};
