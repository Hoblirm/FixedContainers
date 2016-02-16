#include <cxxtest/TestSuite.h>

#include <flex/vector.h>
#include <flex/allocator_debug.h>

typedef flex::vector<int, flex::allocator_debug<int> > vec;

class vector_test: public CxxTest::TestSuite
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

  void test_assign(void)
  {
    vec first;
    vec second;
    vec third;

    //TS_ASSERT_THROWS(first.assign(8, 100), std::runtime_error);
    first.assign(7, 100);             // 7 ints with a value of 100
    vec::iterator it;
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

  void test_capacity(void)
  {
    vec a;
    TS_ASSERT_EQUALS(a.capacity(), 0);
  }

  void test_clear(void)
  {
    vec myvector;
    myvector.push_back(100);
    myvector.push_back(200);
    myvector.push_back(300);
    TS_ASSERT_EQUALS(myvector.size(), 3);
    myvector.clear();
    TS_ASSERT_EQUALS(myvector.size(), 0);
    myvector.push_back(1101);
    myvector.push_back(2202);
    TS_ASSERT_EQUALS(myvector.size(), 2);
    TS_ASSERT_EQUALS(myvector[0], 1101);
    TS_ASSERT_EQUALS(myvector[1], 2202);
  }

  void test_empty(void)
  {
    vec myvector;
    int sum(0);

    TS_ASSERT_EQUALS(myvector.empty(), true);
    for (int i = 1; i <= 10; i++)
      myvector.push_back(i);

    TS_ASSERT_EQUALS(myvector.empty(), false);
    while (!myvector.empty())
    {
      sum += myvector.back();
      myvector.pop_back();
    }

    TS_ASSERT_EQUALS(myvector.empty(), true);
    TS_ASSERT_EQUALS(sum, 55);
  }

  void test_erase()
  {
    vec myvector;

    // set some values (from 1 to 10)
    for (int i = 1; i <= 10; i++)
      myvector.push_back(i);
    TS_ASSERT_EQUALS(myvector.size(), 10);

    // erase the 6th element
    vec::iterator it = myvector.erase(myvector.begin() + 5);
    TS_ASSERT_EQUALS(*it, 7);
    TS_ASSERT_EQUALS(myvector.size(), 9);

    // erase the first 3 elements:
    it = myvector.erase(myvector.begin(), myvector.begin() + 3);
    TS_ASSERT_EQUALS(*it, 4);
    TS_ASSERT_EQUALS(myvector.size(), 6);

    TS_ASSERT_EQUALS(myvector[0], 4);
    TS_ASSERT_EQUALS(myvector[1], 5);
    TS_ASSERT_EQUALS(myvector[2], 7);
    TS_ASSERT_EQUALS(myvector[3], 8);
    TS_ASSERT_EQUALS(myvector[4], 9);
    TS_ASSERT_EQUALS(myvector[5], 10);
  }

  void test_insert(void)
  {
    vec myvector(3, 100);
    vec::iterator it;

    it = myvector.begin();
    it = myvector.insert(it, 200);
    TS_ASSERT_EQUALS(myvector.size(), 4);
    TS_ASSERT_EQUALS(myvector[0], 200);
    TS_ASSERT_EQUALS(myvector[1], 100);
    TS_ASSERT_EQUALS(myvector[2], 100);
    TS_ASSERT_EQUALS(myvector[3], 100);

    myvector.insert(it, 2, 300);
    TS_ASSERT_EQUALS(myvector.size(), 6);
    TS_ASSERT_EQUALS(myvector[0], 300);
    TS_ASSERT_EQUALS(myvector[1], 300);
    TS_ASSERT_EQUALS(myvector[2], 200);
    TS_ASSERT_EQUALS(myvector[3], 100);
    TS_ASSERT_EQUALS(myvector[4], 100);
    TS_ASSERT_EQUALS(myvector[5], 100);

    // "it" no longer valid, get a new one:
    it = myvector.begin();
    vec anothervector(2, 400);
    myvector.insert(it + 2, anothervector.begin(), anothervector.end());
    TS_ASSERT_EQUALS(myvector.size(), 8);
    TS_ASSERT_EQUALS(myvector[0], 300);
    TS_ASSERT_EQUALS(myvector[1], 300);
    TS_ASSERT_EQUALS(myvector[2], 400);
    TS_ASSERT_EQUALS(myvector[3], 400);
    TS_ASSERT_EQUALS(myvector[4], 200);
    TS_ASSERT_EQUALS(myvector[5], 100);
    TS_ASSERT_EQUALS(myvector[6], 100);
    TS_ASSERT_EQUALS(myvector[7], 100);

    int myarray[] = { 501, 502, 503, 12, 13, 14, 15, 16, 17 };
    myvector.insert(myvector.begin(), myarray, myarray + 3);
    TS_ASSERT_EQUALS(myvector.size(), 11);
    TS_ASSERT_EQUALS(myvector[0], 501);
    TS_ASSERT_EQUALS(myvector[1], 502);
    TS_ASSERT_EQUALS(myvector[2], 503);
    TS_ASSERT_EQUALS(myvector[3], 300);
    TS_ASSERT_EQUALS(myvector[4], 300);
    TS_ASSERT_EQUALS(myvector[5], 400);
    TS_ASSERT_EQUALS(myvector[6], 400);
    TS_ASSERT_EQUALS(myvector[7], 200);
    TS_ASSERT_EQUALS(myvector[8], 100);
    TS_ASSERT_EQUALS(myvector[9], 100);
    TS_ASSERT_EQUALS(myvector[10], 100);
  }

  void test_max_size(void)
  {
    vec a;
    TS_ASSERT_EQUALS(a.max_size(), a.get_allocator().max_size());
  }

  void test_assignment_operator(void)
  {
    vec foo(3);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    vec bar(5, 2);

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
    foo = vec();
    TS_ASSERT_EQUALS(foo.size(), 0);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //Same thing as above, but we want to ensure that the cast operator allows assignment of vectors
    //with different capacities.
    foo = vec(1, 19);
    TS_ASSERT_EQUALS(foo.size(), 1);
    TS_ASSERT_EQUALS(foo[0], 19);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    foo.assign(3, 0);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    vec bar2(5, 2);
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
    vec myvector;
    int size = 0;
    TS_ASSERT_EQUALS(myvector.size(), size);

    int sum(0);
    myvector.push_back(100);
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);
    myvector.push_back(200);
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);
    myvector.push_back(300);
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);

    while (!myvector.empty())
    {
      sum += myvector.back();
      myvector.pop_back();
      --size;
      TS_ASSERT_EQUALS(myvector.size(), size);
    }
    TS_ASSERT_EQUALS(sum, 600);
  }

  void test_resize(void)
  {
    vec myvector;

    for (int i = 1; i < 10; i++)
      myvector.push_back(i);

    myvector.resize(5);
    TS_ASSERT_EQUALS(myvector[0], 1);
    TS_ASSERT_EQUALS(myvector[1], 2);
    TS_ASSERT_EQUALS(myvector[2], 3);
    TS_ASSERT_EQUALS(myvector[3], 4);
    TS_ASSERT_EQUALS(myvector[4], 5);

    myvector.resize(8, 100);
    TS_ASSERT_EQUALS(myvector[0], 1);
    TS_ASSERT_EQUALS(myvector[1], 2);
    TS_ASSERT_EQUALS(myvector[2], 3);
    TS_ASSERT_EQUALS(myvector[3], 4);
    TS_ASSERT_EQUALS(myvector[4], 5);
    TS_ASSERT_EQUALS(myvector[5], 100);
    TS_ASSERT_EQUALS(myvector[6], 100);
    TS_ASSERT_EQUALS(myvector[7], 100);

    myvector.resize(12);
    TS_ASSERT_EQUALS(myvector[0], 1);
    TS_ASSERT_EQUALS(myvector[1], 2);
    TS_ASSERT_EQUALS(myvector[2], 3);
    TS_ASSERT_EQUALS(myvector[3], 4);
    TS_ASSERT_EQUALS(myvector[4], 5);
    TS_ASSERT_EQUALS(myvector[5], 100);
    TS_ASSERT_EQUALS(myvector[6], 100);
    TS_ASSERT_EQUALS(myvector[7], 100);
    TS_ASSERT_EQUALS(myvector[8], 0);
    TS_ASSERT_EQUALS(myvector[9], 0);
    TS_ASSERT_EQUALS(myvector[10], 0);
    TS_ASSERT_EQUALS(myvector[11], 0);
  }

  void test_swap(void)
  {
    vec foo(3, 100);   // three ints with a value of 100
    vec bar(5, 200);   // five ints with a value of 200

    foo.swap(bar);
    TS_ASSERT_EQUALS(foo.size(), 5);
    for (int i = 0; i < foo.size(); ++i)
    {
      TS_ASSERT_EQUALS(foo[i], 200);
    }

    TS_ASSERT_EQUALS(bar.size(), 3);
    for (int i = 0; i < bar.size(); ++i)
    {
      TS_ASSERT_EQUALS(bar[i], 100);
    }

    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);
    bar.swap(foo);
    TS_ASSERT_EQUALS(foo.size(), 6);
    for (int i = 0; i < foo.size(); ++i)
    {
      TS_ASSERT_EQUALS(foo[i], 100);
    }

    TS_ASSERT_EQUALS(bar.size(), 5);
    for (int i = 0; i < bar.size(); ++i)
    {
      TS_ASSERT_EQUALS(bar[i], 200);
    }

    foo.push_back(100);
    foo.push_back(100);
    foo.push_back(100);
    foo.push_back(100);
    foo.push_back(100);
  }

  void test_default_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec a(1), std::runtime_error);

    flex::allocation_guard::disable();
    vec a;
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 0);
  }

  void test_default_fill_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec a(2), std::runtime_error);

    flex::allocation_guard::disable();
    vec a(2);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
  }

  void test_fill_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec a(2, 7), std::runtime_error);

    flex::allocation_guard::disable();
    vec a(2, 7);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], 7);
    TS_ASSERT_EQUALS(a[1], 7);
  }

  void test_range_constructor(void)
  {
    vec first(4, 100);
    vec second(first.begin(), first.end());  // iterating through first
    TS_ASSERT_EQUALS(second.size(), 4);
    for (vec::iterator it = second.begin(); it != second.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    // the iterator constructor can also be used to construct from arrays:
    int myints[] = { 16, 2, 77, 29 };
    vec third(myints, myints + sizeof(myints) / sizeof(int));
    TS_ASSERT_EQUALS(third.size(), 4);
    TS_ASSERT_EQUALS(third[0], 16);
    TS_ASSERT_EQUALS(third[1], 2);
    TS_ASSERT_EQUALS(third[2], 77);
    TS_ASSERT_EQUALS(third[3], 29);
  }

  void test_copy_constructor(void)
  {
    vec a(3, 0);
    for (int i = 0; i < a.size(); i++)
    {
      a.data()[i] = i;
    }

    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec b(a), std::runtime_error);

    flex::allocation_guard::disable();
    vec b(a);
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
    }
  }

  void test_relational_operators(void)
  {
    vec foo(3, 100);   // three ints with a value of 100
    vec bar(2, 200);   // two ints with a value of 200

    TS_ASSERT(!(foo == bar));
    TS_ASSERT(foo != bar);
    TS_ASSERT(foo < bar);
    TS_ASSERT(!(foo > bar));
    TS_ASSERT(foo <= bar);
    TS_ASSERT(!(foo >= bar));
  }
};
