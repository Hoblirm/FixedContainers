#include <cxxtest/TestSuite.h>

#include <flex/array.h>

using namespace flex;
template<class T, size_t N = 0>
class array_mock: public array<T, N>
{
public:

  size_t Size()
  {
    return this->mSize;
  }

  T* Array() const
  {
    return this->mAryPtr;
  }

};

class array_test: public CxxTest::TestSuite
{
public:

  void test_at(void)
  {
    unsigned size = 3;
    array_mock<int, 3> a;
    TS_ASSERT_THROWS(a.at(-1), std::out_of_range);
    for (int i = 0; i < size; i++)
    {
      a.at(i) = i;
      TS_ASSERT_EQUALS(a.at(i), a.Array()[i]);
    }
    TS_ASSERT_THROWS(a.at(size), std::out_of_range);
  }

  void test_at_const(void)
  {
    unsigned size = 3;
    const array_mock<int, 3> a;
    TS_ASSERT_THROWS(const int& v = a.at(-1), std::out_of_range);
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
      const int& v = a.at(i);
      TS_ASSERT_EQUALS(v, a.Array()[i]);
    }
    TS_ASSERT_THROWS(const int& v = a.at(size), std::out_of_range);
  }

  void test_back(void)
  {
    unsigned size = 3;
    array_mock<int, 3> a;
    a.Array()[size - 1] = size - 1;
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_back_const(void)
  {
    unsigned size = 3;
    const array_mock<int, 3> a;
    a.Array()[size - 1] = size - 1;
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_begin_and_end(void)
  {
    unsigned size = 3;

    array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = 0;
    for (array_mock<int, 3>::iterator it = a.begin(); it < a.end(); ++it)
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

    const array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = 0;
    for (array_mock<int, 3>::const_iterator it = a.begin(); it != a.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_cbegin_and_cend(void)
  {
    unsigned size = 3;

    const array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = 0;
    for (array_mock<int, 3>::const_iterator it = a.cbegin(); it < a.cend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_crbegin_and_crend(void)
  {
    unsigned size = 3;

    const array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = size - 1;
    for (array_mock<int, 3>::const_reverse_iterator it = a.crbegin(); it < a.crend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_data(void)
  {
    unsigned size = 3;
    array_mock<int, 3> a;
    TS_ASSERT_EQUALS(a.data(), a.Array());
  }

  void test_data_const(void)
  {
    unsigned size = 3;
    const array_mock<int, 3> a;
    TS_ASSERT_EQUALS(a.data(), a.Array());
  }

  void test_empty(void)
  {
    unsigned size = 3;
    array_mock<int,3> a;
    TS_ASSERT_EQUALS(a.empty(), false);
    array_mock<int,0> b;
    TS_ASSERT_EQUALS(b.empty(), true);
  }

  void test_fill(void)
  {
    unsigned size = 3;
    array_mock<int, 3> a;
    a.fill(5);
    for (int i = 0; i < size; i++)
    {
      TS_ASSERT_EQUALS(a.Array()[i], 5);
    }
  }

  void test_front(void)
  {
    unsigned size = 3;
    array_mock<int, 3> a;
    a.Array()[0] = size;
    TS_ASSERT_EQUALS(a.front(), size);
  }

  void test_front_const(void)
  {
    unsigned size = 3;
    const array_mock<int, 3> a;
    a.Array()[0] = size;
    TS_ASSERT_EQUALS(a.front(), size);
  }

  void test_max_size(void)
  {
    unsigned size = 3;
    const array_mock<int, 3> a;
    TS_ASSERT_EQUALS(a.max_size(), size);
  }

  void test_ary_operator(void)
  {
    unsigned size = 3;
    array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a[i] = i;
      TS_ASSERT_EQUALS(a[i], a.Array()[i]);
    }
  }

  void test_ary_operator_const(void)
  {
    unsigned size = 3;
    const array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
      const int& v = a[i];
      TS_ASSERT_EQUALS(v, a.Array()[i]);
    }
  }

  void test_rbegin_and_rend(void)
  {
    unsigned size = 3;

    array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = size - 1;
    for (array_mock<int, 3>::reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
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

    const array_mock<int, 3> a;
    for (int i = 0; i < size; i++)
    {
      a.Array()[i] = i;
    }

    int i = size - 1;
    for (array_mock<int, 3>::const_reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_size(void)
  {
    unsigned size = 3;
    const array_mock<int, 3> a;
    TS_ASSERT_EQUALS(a.size(), size);
  }

  void test_swap(void)
  {
    unsigned size = 5;

    array_mock<int, 5> first;
    first.Array()[0] = 10;
    first.Array()[1] = 20;
    first.Array()[2] = 30;
    first.Array()[3] = 40;
    first.Array()[4] = 50;

    array_mock<int, 5> second;
    second.Array()[0] = 11;
    second.Array()[1] = 22;
    second.Array()[2] = 33;
    second.Array()[3] = 44;
    second.Array()[4] = 55;

    first.swap(second);

    TS_ASSERT_EQUALS(first.Array()[0], 11);
    TS_ASSERT_EQUALS(first.Array()[1], 22);
    TS_ASSERT_EQUALS(first.Array()[2], 33);
    TS_ASSERT_EQUALS(first.Array()[3], 44);
    TS_ASSERT_EQUALS(first.Array()[4], 55);

    TS_ASSERT_EQUALS(second.Array()[0], 10);
    TS_ASSERT_EQUALS(second.Array()[1], 20);
    TS_ASSERT_EQUALS(second.Array()[2], 30);
    TS_ASSERT_EQUALS(second.Array()[3], 40);
    TS_ASSERT_EQUALS(second.Array()[4], 50);
  }

  void test_constructor(void)
  {
    allocation_guard::enable();
    array<int, 3> a;
    TS_ASSERT_EQUALS(a.size(), 3);
    allocation_guard::disable();
  }

  void test_copy_constructor(void)
  {
    array<int, 3> a;
    for (int i = 0; i < a.size(); i++)
    {
      a.data()[i] = i;
    }

    array<int, 3> b(a);
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
    }
  }

  void test_assignment_operator(void)
  {
    //#1 Test default assignment.
    array<int, 3> foo;
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    array<int, 3> bar;
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

    array<int, 3> bar2;
    bar2.fill(2);

    //#2 Test assignment with specialized parameter.
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
    array<int, 3> foo2;
    foo2[0] = 1;
    foo2[1] = 5;
    foo2[2] = 17;

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

  void test_relational_operators(void)
  {
    int aAry[5] = { 10, 20, 30, 40, 50 };
    int bAry[5] = { 10, 20, 30, 40, 50 };
    int cAry[5] = { 50, 40, 30, 20, 10 };
    array<int, 5> a;
    array<int, 5> b;
    array<int, 5> c;
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
