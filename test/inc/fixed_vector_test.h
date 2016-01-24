#include <cxxtest/TestSuite.h>

#include <fixed_vector.h>

class fixed_vector_test: public CxxTest::TestSuite
{
public:

  void test_assign(void)
  {
    fixed_vector<int, 7> first;
    fixed_vector<int, 5> second;
    fixed_vector<int> third(3);

    TS_ASSERT_THROWS(first.assign(8, 100), std::out_of_range);
    first.assign(7, 100);             // 7 ints with a value of 100

    fixed_vector<int, 7>::iterator it;
    it = first.begin() + 1;

    TS_ASSERT_THROWS(second.assign(it, first.end()), std::out_of_range);
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
    fixed_vector<int> b(19);
    TS_ASSERT_EQUALS(b.capacity(), 19);
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

    TS_ASSERT_THROWS(invalid_fill_constructor(), std::out_of_range);
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
    fixed_vector<int, 8> a;
    assignment_method(a);
    read_method(a);
    TS_ASSERT_THROWS(copy_method(a), std::runtime_error);

    allocation_guard::disable();
    copy_method(a);
  }

  void test_specialized_default_constructor(void)
  {
    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_vector<int> a(3), std::runtime_error);

    allocation_guard::disable();
    fixed_vector<int> a(3);
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 3);
  }

  void test_specialized_default_fill_constructor(void)
  {
    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_vector<int> a(3, 2), std::runtime_error);

    allocation_guard::disable();
    fixed_vector<int> a(3, 2);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 3);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 0);

    TS_ASSERT_THROWS(fixed_vector<int> b(3,4), std::out_of_range);
  }

  void test_specialized_fill_constructor(void)
  {
    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_vector<int> a(3, 2, 7), std::runtime_error);

    allocation_guard::disable();
    fixed_vector<int> a(3, 2, 7);
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 3);
    TS_ASSERT_EQUALS(a[0], 7);
    TS_ASSERT_EQUALS(a[1], 7);
  }

  void test_specialized_range_constructor(void)
  {
    fixed_vector<int> first(4, 4, 100);
    fixed_vector<int> second(4, first.begin(), first.end());  // iterating through first
    TS_ASSERT_EQUALS(second.size(), 4);
    for (fixed_vector<int>::iterator it = second.begin(); it != second.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    // the iterator constructor can also be used to construct from arrays:
    int myints[] = { 16, 2, 77, 29 };
    fixed_vector<int> third(4, myints, myints + sizeof(myints) / sizeof(int));
    TS_ASSERT_EQUALS(third.size(), 4);
    TS_ASSERT_EQUALS(third[0], 16);
    TS_ASSERT_EQUALS(third[1], 2);
    TS_ASSERT_EQUALS(third[2], 77);
    TS_ASSERT_EQUALS(third[3], 29);
  }

  void test_specialized_copy_constructor(void)
  {
    fixed_vector<int> a(3, 3, 0);
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

};
