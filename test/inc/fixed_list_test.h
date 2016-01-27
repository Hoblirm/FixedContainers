#include <cxxtest/TestSuite.h>

#include <fixed_list.h>

template<class T>
class fixed_list_mock: public fixed_list<T>
{
public:

  fixed_list_mock(size_t Size) :
      fixed_list<T>(Size)
  {
  }

  size_t Size()
  {
    return this->mSize;
  }

  T* Array() const
  {
    return this->mAryPtr;
  }

};

class fixed_list_test: public CxxTest::TestSuite
{
public:

  void test_assign(void)
  {
    fixed_list<int, 7> first;
    fixed_list<int, 5> second;
    fixed_list<int> third(3);

    TS_ASSERT_THROWS(first.assign(8, 100), std::runtime_error);
    first.assign(7, 100);             // 7 ints with a value of 100

    fixed_list<int, 7>::iterator it;
    it = ++first.begin();

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

  void test_back(void)
  {
    unsigned size = 3;
    fixed_list_mock<int> a(size);
    a.Array()[size - 1] = size - 1;
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_back_const(void)
  {
    unsigned size = 3;
    const fixed_list_mock<int> a(size);
    a.Array()[size - 1] = size - 1;
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_begin_and_end(void)
  {
    unsigned size = 3;

    fixed_list_mock<int> a(size);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = 0;
    for (fixed_list_mock<int>::iterator it = a.begin(); it < a.end(); ++it)
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

    const fixed_list_mock<int> a(size);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = 0;
    for (fixed_list_mock<int>::const_iterator it = a.begin(); it != a.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_cbegin_and_cend(void)
  {
    unsigned size = 3;

    const fixed_list_mock<int> a(size);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = 0;
    for (fixed_list_mock<int>::const_iterator it = a.cbegin(); it < a.cend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_crbegin_and_crend(void)
  {
    unsigned size = 3;

    const fixed_list_mock<int> a(size);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = size - 1;
    for (fixed_list_mock<int>::const_reverse_iterator it = a.crbegin(); it < a.crend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_empty(void)
  {
    fixed_list<int, 16> mylist;
    int sum(0);

    TS_ASSERT_EQUALS(mylist.empty(), true);
    for (int i = 1; i <= 10; i++)
      mylist.push_back(i);

    TS_ASSERT_EQUALS(mylist.empty(), false);
    while (!mylist.empty())
    {
      sum += mylist.back();
      mylist.pop_back();
    }

    TS_ASSERT_EQUALS(mylist.empty(), true);
    TS_ASSERT_EQUALS(sum, 55);
  }

  void test_front(void)
  {
    unsigned size = 3;
    fixed_list_mock<int> a(size);
    a.Array()[0] = size;
    TS_ASSERT_EQUALS(a.front(), size);
  }

  void test_front_const(void)
  {
    unsigned size = 3;
    const fixed_list_mock<int> a(size);
    a.Array()[0] = size;
    TS_ASSERT_EQUALS(a.front(), size);
  }

  void test_max_size(void)
  {
    unsigned size = 3;
    const fixed_list_mock<int> a(size);
    TS_ASSERT_EQUALS(a.max_size(), size);
  }

  void test_assignment_operator(void)
  {
    fixed_list<int, 8> foo(3);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    fixed_list<int, 8> bar(5, 2);

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
    foo = fixed_list<int, 8>();
    TS_ASSERT_EQUALS(foo.size(), 0);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //Same thing as above, but we want to ensure that the cast operator allows assignment of lists
    //with different capacities.
    foo = fixed_list<int, 16>(1, 19);
    TS_ASSERT_EQUALS(foo.size(), 1);
    TS_ASSERT_EQUALS(foo[0], 19);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    fixed_list<int, 16> larger(16);
    TS_ASSERT_THROWS(foo = larger, std::runtime_error);

    foo.assign(3, 0);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    fixed_list<int> bar2(8, 5, 2);
    TS_ASSERT_THROWS(bar2 = larger, std::runtime_error);

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
    fixed_list<int, 8> mylist;
    int size = 0;
    TS_ASSERT_EQUALS(mylist.size(), size);

    int sum(0);
    mylist.push_back(100);
    ++size;
    TS_ASSERT_EQUALS(mylist.size(), size);
    mylist.push_back(200);
    ++size;
    TS_ASSERT_EQUALS(mylist.size(), size);
    mylist.push_back(300);
    ++size;
    TS_ASSERT_EQUALS(mylist.size(), size);

    while (!mylist.empty())
    {
      sum += mylist.back();
      mylist.pop_back();
      --size;
      TS_ASSERT_EQUALS(mylist.size(), size);
    }
    TS_ASSERT_EQUALS(sum, 600);
  }

  void test_rbegin_and_rend(void)
  {
    unsigned size = 3;

    fixed_list_mock<int> a(size);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = size - 1;
    for (fixed_list_mock<int>::reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
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

    const fixed_list_mock<int> a(size);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = size - 1;
    for (fixed_list_mock<int>::const_reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_size(void)
  {
    unsigned size = 3;
    const fixed_list_mock<int> a(size);
    TS_ASSERT_EQUALS(a.size(), size);
  }

  void test_swap(void)
  {
    fixed_list<int, 16> foo(3, 100);   // three ints with a value of 100
    fixed_list<int> bar(8, 5, 200);   // five ints with a value of 200

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
    TS_ASSERT_THROWS(bar.swap(foo), std::runtime_error);
  }

  void test_constructor(void)
  {
    allocation_guard::enable();
    fixed_list<int, 3> a;
    TS_ASSERT_EQUALS(a.size(), 3);
    allocation_guard::disable();
  }

  void test_copy_constructor(void)
  {
    fixed_list<int, 3> a;
    for (int i = 0; i < a.size(); i++)
    {
      a.data()[i] = i;
    }

    fixed_list<int, 3> b(a);
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
    }
  }

  void test_assignment_operator(void)
  {
    //#1 Test default assignment.
    fixed_list<int, 3> foo;
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    fixed_list<int, 3> bar;
    bar.fill(2);

    bar = foo;
    TS_ASSERT_EQUALS(bar.size(), 3);
    TS_ASSERT_EQUALS(bar[0], 1);
    TS_ASSERT_EQUALS(bar[1], 5);
    TS_ASSERT_EQUALS(bar[2], 17);

    //Ensure that assignments to bar doesn't impact the value of foo.
    bar.fill(0);
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    fixed_list<int> bar2(3);
    bar2.fill(2);

    //#2 Test assignment with specialized parameter.
    fixed_list<int> larger(4);
    TS_ASSERT_THROWS(larger = foo, std::runtime_error);

    bar2 = foo;
    TS_ASSERT_EQUALS(bar2.size(), 3);
    TS_ASSERT_EQUALS(bar2[0], 1);
    TS_ASSERT_EQUALS(bar2[1], 5);
    TS_ASSERT_EQUALS(bar2[2], 17);

    //Ensure that assignments to bar2 doesn't impact the value of foo.
    bar2.fill(0);
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //#3 Test specialized assignment.
    fixed_list<int> foo2(3);
    foo2[0] = 1;
    foo2[1] = 5;
    foo2[2] = 17;

    TS_ASSERT_THROWS(larger = foo2, std::runtime_error);

    bar2.fill(2);

    bar2 = foo;
    TS_ASSERT_EQUALS(bar2.size(), 3);
    TS_ASSERT_EQUALS(bar2[0], 1);
    TS_ASSERT_EQUALS(bar2[1], 5);
    TS_ASSERT_EQUALS(bar2[2], 17);

    //Ensure that assignments to bar2 doesn't impact the value of foo.
    bar2.fill(0);
    TS_ASSERT_EQUALS(foo2.size(), 3);
    TS_ASSERT_EQUALS(foo2[0], 1);
    TS_ASSERT_EQUALS(foo2[1], 5);
    TS_ASSERT_EQUALS(foo2[2], 17);
  }

  void assignment_method(fixed_list<int>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      x[i] = i;
    }
  }

  void read_method(const fixed_list<int>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], i);
    }
  }

  void copy_method(fixed_list<int> x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], i);
    }
  }

  void test_cast_operator(void)
  {
    allocation_guard::enable();
    fixed_list<int, 8> a;
    assignment_method(a);
    read_method(a);
    TS_ASSERT_THROWS(copy_method(a), std::runtime_error);

    allocation_guard::disable();
    copy_method(a);
  }

  void test_specialized_constructor(void)
  {
    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_list_mock<int> a3(3), std::runtime_error);

    allocation_guard::disable();
    fixed_list_mock<int> a3(3);
    TS_ASSERT_EQUALS(a3.Size(), 3);
  }

  void test_specialized_copy_constructor(void)
  {
    unsigned size = 3;

    fixed_list_mock<int> a(size);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_list_mock<int> b(a), std::runtime_error);

    allocation_guard::disable();
    fixed_list_mock<int> b(a);
    for (int i = 0; i < size; i++)
    {
      TS_ASSERT_EQUALS(b.Array()[i], a.Array()[i]);
    }
  }

  void test_relational_operators(void)
  {
    int aAry[5] = { 10, 20, 30, 40, 50 };
    int bAry[5] = { 10, 20, 30, 40, 50 };
    int cAry[5] = { 50, 40, 30, 20, 10 };
    fixed_list<int, 5> a;
    fixed_list<int, 5> b;
    fixed_list<int> c(5);
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
