#include <cxxtest/TestSuite.h>

#include <fixed_vector.h>

class fixed_vector_test: public CxxTest::TestSuite
{
public:

  void test_assign(void)
  {
    fixed_vector<int, 7> first;
    fixed_vector<int, 5> second;
    fixed_vector<int> third;

    TS_ASSERT_THROWS(first.assign(8, 100), std::runtime_error);
    first.assign(7, 100);             // 7 ints with a value of 100

    fixed_vector<int, 7>::iterator it;
    it = first.begin() + 1;

    TS_ASSERT_THROWS(second.assign(it, first.end()), std::runtime_error);
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
    fixed_vector<int, 17> a;
    TS_ASSERT_EQUALS(a.capacity(), 17);
    fixed_vector<int> b;
    TS_ASSERT_EQUALS(b.capacity(), 0);
  }

  void test_clear(void)
  {
    fixed_vector<int, 8> myvector;
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
    fixed_vector<int, 16> myvector;
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
    fixed_vector<int, 16> myvector;

    // set some values (from 1 to 10)
    for (int i = 1; i <= 10; i++)
      myvector.push_back(i);
    TS_ASSERT_EQUALS(myvector.size(), 10);

    // erase the 6th element
    fixed_vector<int>::iterator it = myvector.erase(myvector.begin() + 5);
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
    fixed_vector<int,1> emptyvec;
    TS_ASSERT_THROWS(emptyvec.insert(emptyvec.begin(), 2, 2), std::runtime_error);
    fixed_vector<int, 16> myvector(3, 100);
    fixed_vector<int, 16>::iterator it;

    it = myvector.begin();
    it = myvector.insert(it, 200);
    TS_ASSERT_EQUALS(myvector.size(), 4);
    TS_ASSERT_EQUALS(myvector[0], 200);
    TS_ASSERT_EQUALS(myvector[1], 100);
    TS_ASSERT_EQUALS(myvector[2], 100);
    TS_ASSERT_EQUALS(myvector[3], 100);

    TS_ASSERT_THROWS(myvector.insert(it, 13, 300), std::runtime_error);
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
    fixed_vector<int, 16> anothervector(2, 400);
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
    TS_ASSERT_THROWS(myvector.insert(myvector.begin(), myarray, myarray + 9), std::runtime_error);
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
    fixed_vector<int, 17> a;
    TS_ASSERT_EQUALS(a.max_size(), 17);
    fixed_vector<int> b;
    TS_ASSERT_EQUALS(b.max_size(), 0);
  }

  void test_assignment_operator(void)
  {
    fixed_vector<int, 8> foo(3);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    fixed_vector<int, 8> bar(5, 2);

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
    foo = fixed_vector<int, 8>();
    TS_ASSERT_EQUALS(foo.size(), 0);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //Same thing as above, but we want to ensure that the cast operator allows assignment of vectors
    //with different capacities.
    foo = fixed_vector<int, 16>(1, 19);
    TS_ASSERT_EQUALS(foo.size(), 1);
    TS_ASSERT_EQUALS(foo[0], 19);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    fixed_vector<int, 16> larger(16);
    TS_ASSERT_THROWS(foo = larger, std::runtime_error);

    foo.assign(3, 0);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    fixed_vector<int> bar2(5, 2);
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
    fixed_vector<int, 8> myvector;
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
    fixed_vector<int, 16> myvector;

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
    fixed_vector<int, 16> foo(3, 100);   // three ints with a value of 100
    fixed_vector<int> bar(5, 200);   // five ints with a value of 200

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
    TS_ASSERT_THROWS(bar.swap(foo), std::runtime_error);
  }

  void test_default_constructor(void)
  {
    allocation_guard::enable();
    fixed_vector<int, 3> a;
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 3);
    allocation_guard::disable();
  }

  void invalid_fill_constructor(void)
  {
    fixed_vector<int, 3> a(4);
  }

  void test_default_fill_constructor(void)
  {
    allocation_guard::enable();
    fixed_vector<int, 3> a(2);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 3);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);

    TS_ASSERT_THROWS(invalid_fill_constructor(), std::runtime_error);
    allocation_guard::disable();
  }

  void test_fill_constructor(void)
  {
    allocation_guard::enable();
    fixed_vector<int, 3> a(2, 7);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 3);
    TS_ASSERT_EQUALS(a[0], 7);
    TS_ASSERT_EQUALS(a[1], 7);
    allocation_guard::disable();
  }

  void test_range_constructor(void)
  {
    fixed_vector<int, 4> first(4, 100);
    fixed_vector<int, 4> second(first.begin(), first.end());  // iterating through first
    TS_ASSERT_EQUALS(second.size(), 4);
    for (fixed_vector<int>::iterator it = second.begin(); it != second.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    // the iterator constructor can also be used to construct from arrays:
    int myints[] = { 16, 2, 77, 29 };
    fixed_vector<int, 4> third(myints, myints + sizeof(myints) / sizeof(int));
    TS_ASSERT_EQUALS(third.size(), 4);
    TS_ASSERT_EQUALS(third[0], 16);
    TS_ASSERT_EQUALS(third[1], 2);
    TS_ASSERT_EQUALS(third[2], 77);
    TS_ASSERT_EQUALS(third[3], 29);
  }

  void test_copy_constructor(void)
  {
    fixed_vector<int, 3> a(3, 0);
    for (int i = 0; i < a.size(); i++)
    {
      a.data()[i] = i;
    }

    fixed_vector<int, 3> b(a);
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
    }
  }

  void assignment_method(fixed_vector<int>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      x[i] = i;
    }
  }

  void read_method(const fixed_vector<int>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], i);
    }
  }

  void copy_method(fixed_vector<int> x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], i);
    }
  }

  void test_cast_operator(void)
  {
    allocation_guard::enable();
    fixed_vector<int, 8> a(8,0);
    assignment_method(a);
    read_method(a);
    TS_ASSERT_THROWS(copy_method(a), std::runtime_error);

    allocation_guard::disable();
    copy_method(a);
  }

  void test_specialized_default_constructor(void)
  {
    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_vector<int> a(1), std::runtime_error);

    allocation_guard::disable();
    fixed_vector<int> a;
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 0);
  }

  void test_specialized_default_fill_constructor(void)
  {
    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_vector<int> a(2), std::runtime_error);

    allocation_guard::disable();
    fixed_vector<int> a(2);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);
  }

  void test_specialized_fill_constructor(void)
  {
    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_vector<int> a(2, 7), std::runtime_error);

    allocation_guard::disable();
    fixed_vector<int> a(2, 7);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], 7);
    TS_ASSERT_EQUALS(a[1], 7);
  }

  void test_specialized_range_constructor(void)
  {
    fixed_vector<int> first(4, 100);
    fixed_vector<int> second(first.begin(), first.end());  // iterating through first
    TS_ASSERT_EQUALS(second.size(), 4);
    for (fixed_vector<int>::iterator it = second.begin(); it != second.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    // the iterator constructor can also be used to construct from arrays:
    int myints[] = { 16, 2, 77, 29 };
    fixed_vector<int> third(myints, myints + sizeof(myints) / sizeof(int));
    TS_ASSERT_EQUALS(third.size(), 4);
    TS_ASSERT_EQUALS(third[0], 16);
    TS_ASSERT_EQUALS(third[1], 2);
    TS_ASSERT_EQUALS(third[2], 77);
    TS_ASSERT_EQUALS(third[3], 29);
  }

  void test_specialized_copy_constructor(void)
  {
    fixed_vector<int> a(3, 0);
    for (int i = 0; i < a.size(); i++)
    {
      a.data()[i] = i;
    }

    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_vector<int> b(a), std::runtime_error);

    allocation_guard::disable();
    fixed_vector<int> b(a);
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
    }
  }

  void test_relational_operators(void)
  {
    fixed_vector<int, 8> foo(3, 100);   // three ints with a value of 100
    fixed_vector<int, 8> bar(2, 200);   // two ints with a value of 200

    TS_ASSERT(!(foo == bar));
    TS_ASSERT(foo != bar);
    TS_ASSERT(foo < bar);
    TS_ASSERT(!(foo > bar));
    TS_ASSERT(foo <= bar);
    TS_ASSERT(!(foo >= bar));
  }
};
