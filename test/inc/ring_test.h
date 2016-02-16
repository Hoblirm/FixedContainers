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

  void test_default_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(ring_int a(1), std::runtime_error);

    flex::allocation_guard::disable();
    ring_int a;
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 0);
  }

  void test_default_fill_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(ring_int a(2), std::runtime_error);

    flex::allocation_guard::disable();
    ring_int a(2);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
  }

  void test_fill_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(ring_int a(2, 7), std::runtime_error);

    flex::allocation_guard::disable();
    ring_int a(2, 7);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], 7);
    TS_ASSERT_EQUALS(a[1], 7);
  }

  void test_range_constructor(void)
  {
    ring_int first(4, 100);
    ring_int second(first.begin(), first.end());  // iterating through first
    TS_ASSERT_EQUALS(second.size(), 4);
    for (ring_int::iterator it = second.begin(); it != second.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    // the iterator constructor can also be used to construct from arrays:
    int myints[] = { 16, 2, 77, 29 };
    ring_int third(myints, myints + sizeof(myints) / sizeof(int));
    TS_ASSERT_EQUALS(third.size(), 4);
    TS_ASSERT_EQUALS(third[0], 16);
    TS_ASSERT_EQUALS(third[1], 2);
    TS_ASSERT_EQUALS(third[2], 77);
    TS_ASSERT_EQUALS(third[3], 29);
  }

  void test_copy_constructor(void)
  {
    ring_int a(3, 7);

    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(ring_int b(a), std::runtime_error);

    flex::allocation_guard::disable();
    ring_int b(a);
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b[i], a[i]);
    }
  }

  void test_assign(void)
  {
    ring_int first;
    ring_int second;
    ring_int third;

    //TS_ASSERT_THROWS(first.assign(8, 100), std::runtime_error);
    first.assign(7, 100);             // 7 ints with a value of 100
    ring_int::iterator it;
    it = first.begin() + 1;

    //TS_ASSERT_THROWS(second.assign(it, first.end()), std::runtime_error);
    second.assign(it, first.end() - 1); // the 5 central values of first

    int myints[] = { 1776, 7, 4 };
    third.assign(myints, myints + 3);   // assigning from array.

    TS_ASSERT_EQUALS(first.size(), 7);
    TS_ASSERT_EQUALS(first[0], 100);
    TS_ASSERT_EQUALS(first[1], 100);
    TS_ASSERT_EQUALS(first[2], 100);
    TS_ASSERT_EQUALS(first[3], 100);
    TS_ASSERT_EQUALS(first[4], 100);
    TS_ASSERT_EQUALS(first[5], 100);
    TS_ASSERT_EQUALS(first[6], 100);

    TS_ASSERT_EQUALS(second.size(), 5);
    TS_ASSERT_EQUALS(second[0], 100);
    TS_ASSERT_EQUALS(second[1], 100);
    TS_ASSERT_EQUALS(second[2], 100);
    TS_ASSERT_EQUALS(second[3], 100);
    TS_ASSERT_EQUALS(second[4], 100);

    TS_ASSERT_EQUALS(third.size(), 3);
    TS_ASSERT_EQUALS(third[0], 1776);
    TS_ASSERT_EQUALS(third[1], 7);
    TS_ASSERT_EQUALS(third[2], 4);
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

  void test_cbegin_and_cend(void)
  {
    unsigned size = 3;

    const ring_int a(size, 7);

    int i = 0;
    for (ring_int::const_iterator it = a.cbegin(); it != a.cend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 7);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_crbegin_and_crend(void)
  {
    unsigned size = 3;

    const ring_int a(size, 7);

    int i = size - 1;
    for (ring_int::const_reverse_iterator it = a.crbegin(); it != a.crend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 7);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_empty(void)
  {
    unsigned size = 3;
    ring_int a(size);
    TS_ASSERT_EQUALS(a.empty(), false);
    ring_int b;
    TS_ASSERT_EQUALS(b.empty(), true);
  }

  void test_erase()
  {
    ring_int myring;

    // set some values (from 1 to 10)
    for (int i = 1; i <= 10; i++)
      myring.push_back(i);
    TS_ASSERT_EQUALS(myring.size(), 10);

    // erase the 6th element
    ring_int::iterator it = myring.erase(myring.begin() + 5);
    TS_ASSERT_EQUALS(*it, 7);
    TS_ASSERT_EQUALS(myring.size(), 9);

    // erase the first 3 elements:
    it = myring.erase(myring.begin(), myring.begin() + 3);
    TS_ASSERT_EQUALS(*it, 4);
    TS_ASSERT_EQUALS(myring.size(), 6);

    TS_ASSERT_EQUALS(myring[0], 4);
    TS_ASSERT_EQUALS(myring[1], 5);
    TS_ASSERT_EQUALS(myring[2], 7);
    TS_ASSERT_EQUALS(myring[3], 8);
    TS_ASSERT_EQUALS(myring[4], 9);
    TS_ASSERT_EQUALS(myring[5], 10);
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

  void test_insert(void)
  {
    ring_int myring(3, 100);
    ring_int::iterator it;

    it = myring.begin();
    it = myring.insert(it, 200);
    TS_ASSERT_EQUALS(myring.size(), 4);
    TS_ASSERT_EQUALS(myring[0], 200);
    TS_ASSERT_EQUALS(myring[1], 100);
    TS_ASSERT_EQUALS(myring[2], 100);
    TS_ASSERT_EQUALS(myring[3], 100);

    myring.insert(it, 2, 300);
    TS_ASSERT_EQUALS(myring.size(), 6);
    TS_ASSERT_EQUALS(myring[0], 300);
    TS_ASSERT_EQUALS(myring[1], 300);
    TS_ASSERT_EQUALS(myring[2], 200);
    TS_ASSERT_EQUALS(myring[3], 100);
    TS_ASSERT_EQUALS(myring[4], 100);
    TS_ASSERT_EQUALS(myring[5], 100);

    // "it" no longer valid, get a new one:
    it = myring.begin();
    ring_int anotherring(2, 400);
    myring.insert(it + 2, anotherring.begin(), anotherring.end());
    TS_ASSERT_EQUALS(myring.size(), 8);
    TS_ASSERT_EQUALS(myring[0], 300);
    TS_ASSERT_EQUALS(myring[1], 300);
    TS_ASSERT_EQUALS(myring[2], 400);
    TS_ASSERT_EQUALS(myring[3], 400);
    TS_ASSERT_EQUALS(myring[4], 200);
    TS_ASSERT_EQUALS(myring[5], 100);
    TS_ASSERT_EQUALS(myring[6], 100);
    TS_ASSERT_EQUALS(myring[7], 100);

    int myarray[] = { 501, 502, 503, 12, 13, 14, 15, 16, 17 };
    myring.insert(myring.begin(), myarray, myarray + 3);
    TS_ASSERT_EQUALS(myring.size(), 11);
    TS_ASSERT_EQUALS(myring[0], 501);
    TS_ASSERT_EQUALS(myring[1], 502);
    TS_ASSERT_EQUALS(myring[2], 503);
    TS_ASSERT_EQUALS(myring[3], 300);
    TS_ASSERT_EQUALS(myring[4], 300);
    TS_ASSERT_EQUALS(myring[5], 400);
    TS_ASSERT_EQUALS(myring[6], 400);
    TS_ASSERT_EQUALS(myring[7], 200);
    TS_ASSERT_EQUALS(myring[8], 100);
    TS_ASSERT_EQUALS(myring[9], 100);
    TS_ASSERT_EQUALS(myring[10], 100);
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

  void test_rbegin_and_rend(void)
  {
    unsigned size = 3;

    ring_int a(size);
    for (int i = 0; i < size; i++)
    {
      a[i] = i;
    }

    int i = size - 1;
    for (ring_int::reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      *it = 0; //Ensure it is not const.
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_rbegin_and_rend_const(void)
  {
    unsigned size = 3;

    const ring_int a(size, 7);

    int i = size - 1;
    for (ring_int::const_reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 7);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_reserve(void)
  {
    ring_int bar;
    bar.reserve(100);
    TS_ASSERT_EQUALS(bar.capacity(), 100);
    for (int i = 0; i < 100; ++i)
    {
      bar.push_back(i);
    }

    TS_ASSERT_EQUALS(bar.capacity(), 100);
  }

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

  void test_assignment_operator(void)
  {
    ring_int foo(3);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    ring_int bar(5, 2);

    bar = foo;
    TS_ASSERT_EQUALS(bar.size(), 3);
    TS_ASSERT_EQUALS(bar[0], 1);
    TS_ASSERT_EQUALS(bar[1], 5);
    TS_ASSERT_EQUALS(bar[2], 17);

    //Ensure assignments on bar doens't impact foo.
    bar.assign(0, 3);
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //We are setting foo to an empty array.  Size is zero, but the capacity is still eight.  Behavior-wise
    //it doesn't matter what the contents are since the size is zero.  However, we want to ensure that the
    //assignment operator doesn't perform extra work by resetting these values.  This happens if no assignment
    //operator is defined and a default one is used.
    foo = ring_int();
    TS_ASSERT_EQUALS(foo.size(), 0);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //Same thing as above, but we want to ensure that the cast operator allows assignment of ring_inttors
    //with different capacities.
    foo = ring_int(1, 19);
    TS_ASSERT_EQUALS(foo.size(), 1);
    TS_ASSERT_EQUALS(foo[0], 19);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    foo.assign(3, 0);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    ring_int bar2(5, 2);
    bar2 = foo;
    TS_ASSERT_EQUALS(bar2.size(), 3);
    TS_ASSERT_EQUALS(bar2[0], 1);
    TS_ASSERT_EQUALS(bar2[1], 5);
    TS_ASSERT_EQUALS(bar2[2], 17);

    //Ensure assignments on bar doens't impact foo.
    bar2.assign(0, 3);
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);
  }

  void test_push_back_and_pop_back(void)
  {
    ring_int myring;
    int size = 0;
    TS_ASSERT_EQUALS(myring.size(), size);

    int sum(0);
    myring.push_back(100);
    ++size;
    TS_ASSERT_EQUALS(myring.size(), size);
    myring.push_back(200);
    ++size;
    TS_ASSERT_EQUALS(myring.size(), size);
    myring.push_back(300);
    ++size;
    TS_ASSERT_EQUALS(myring.size(), size);

    while (!myring.empty())
    {
      sum += myring.back();
      myring.pop_back();
      --size;
      TS_ASSERT_EQUALS(myring.size(), size);
    }
    TS_ASSERT_EQUALS(sum, 600);
  }

  void test_pop_front(void)
  {
    ring_int myring;
    myring.push_back(100);
    myring.push_back(200);
    myring.push_back(300);

    myring.pop_front();
    TS_ASSERT_EQUALS(myring.size(), 2);
    ring_int::iterator it = myring.begin();
    TS_ASSERT_EQUALS(*(it++), 200);
    TS_ASSERT_EQUALS(*(it++), 300);

    myring.pop_front();
    TS_ASSERT_EQUALS(myring.size(), 1);
    it = myring.begin();
    TS_ASSERT_EQUALS(*(it++), 300);

    myring.pop_front();
    TS_ASSERT_EQUALS(myring.size(), 0);
  }

  void test_push_front(void)
  {
    ring_int myring(2, 100); // two ints with a value of 100
    myring.push_front(200);
    myring.push_front(300);

    ring_int::iterator it = myring.begin();

    TS_ASSERT_EQUALS(*(it++), 300);
    TS_ASSERT_EQUALS(*(it++), 200);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
  }

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
