#include <cxxtest/TestSuite.h>

#include <fixed_list.h>

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
    it = ++first.begin();
    
    TS_ASSERT_THROWS(second.assign(it, first.end()), std::runtime_error);
    second.assign(it, first.end()); // the 5 central values of first

    int myints[] = { 1776, 7, 4 };
    third.assign(myints, myints + 3);   // assigning from array.

    TS_ASSERT_EQUALS(first.size(), 7);
    it = first.begin();
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);

    TS_ASSERT_EQUALS(second.size(), 5);
    it = second.begin();
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);
    TS_ASSERT_EQUALS(*(it++), 100);

    TS_ASSERT_EQUALS(third.size(), 3);
    it = third.begin();
    TS_ASSERT_EQUALS(*(it++), 1776);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 4);
  }

  void test_back(void)
  {
    unsigned size = 3;
    fixed_list<int> a(size);
    
    int i=0;
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       *it = i++;
    }
    
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_back_const(void)
  {
    unsigned size = 3;
    int ary[3] = {0,1,2};
    const fixed_list<int> a(size,ary,ary+3);  
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_begin_and_end(void)
  {
    unsigned size = 3;

    fixed_list<int> a(size);
    int i=0;
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       *it = i++;
    }

    i = 0;
    for (fixed_list<int>::iterator it = a.begin(); it != a.end(); ++it)
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

    int ary[3] = {0,1,2};
    const fixed_list<int> a(size,ary,ary+3);  

    int i = 0;
    for (fixed_list<int>::const_iterator it = a.begin(); it != a.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_cbegin_and_cend(void)
  {
    unsigned size = 3;

    int ary[3] = {0,1,2};
    const fixed_list<int> a(size,ary,ary+3);  

    int i = 0;
    for (fixed_list<int>::const_iterator it = a.cbegin(); it != a.cend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_crbegin_and_crend(void)
  {
    unsigned size = 3;

    int ary[3] = {0,1,2};
    const fixed_list<int> a(size,ary,ary+3);  

    int i = size - 1;
    for (fixed_list<int>::const_reverse_iterator it = a.crbegin(); it != a.crend(); ++it)
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
    fixed_list<int> a(size);
    int i=size;
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       *it = i--;
    }
    TS_ASSERT_EQUALS(a.front(), size);
  }

  void test_front_const(void)
  {
    unsigned size = 3;
    int ary[3] = {3,2,1};
    const fixed_list<int> a(size,ary,ary+3);  
    TS_ASSERT_EQUALS(a.front(), size);
  }

  void test_max_size(void)
  {
    unsigned size = 3;
    fixed_list<int> a(size);
    TS_ASSERT_EQUALS(a.max_size(), size);
    fixed_list<int,3> b;
    TS_ASSERT_EQUALS(b.max_size(), size);
  }

  void test_assignment_operator(void)
  {
    fixed_list<int, 8> foo(3);

    fixed_list<int>::iterator it=foo.begin();
    *(it++) = 1;
    *(it++)  = 5;
    *(it++)  = 17;

    fixed_list<int, 8> bar(5, 2);

    bar = foo;
    TS_ASSERT_EQUALS(bar.size(), 3);
    it = bar.begin();
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 17);

    //Ensure assignments on bar doens't impact foo.
    bar.assign(0, 3);
    TS_ASSERT_EQUALS(foo.size(), 3);
     it = foo.begin();
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 17);

    //We are setting foo to an empty array.  Size is zero, but the capacity is still eight.  Behavior-wise
    //it doesn't matter what the contents are since the size is zero.  However, we want to ensure that the
    //assignment operator doesn't perform extra work by resetting these values.  This happens if no assignment
    //operator is defined and a default one is used.
    foo = fixed_list<int, 8>();
    TS_ASSERT_EQUALS(foo.size(), 0);
    it = foo.begin();
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 17);

    //Same thing as above, but we want to ensure that the cast operator allows assignment of lists
    //with different capacities.
    foo = fixed_list<int, 16>(1, 19);
    TS_ASSERT_EQUALS(foo.size(), 1);
    it = foo.begin();
    TS_ASSERT_EQUALS(*(it++), 19);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 17);

    fixed_list<int, 16> larger(16);
    TS_ASSERT_THROWS(foo = larger, std::runtime_error);

    foo.assign(3, 0);
    it = foo.begin();
    *(it++) = 1;
    *(it++) = 5;
   *(it++) = 17;

    fixed_list<int> bar2(8, 5, 2);
    TS_ASSERT_THROWS(bar2 = larger, std::runtime_error);

    bar2 = foo;
    TS_ASSERT_EQUALS(bar2.size(), 3);
    it = bar2.begin();
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 17);

    //Ensure assignments on bar doens't impact foo.
    bar2.assign(0, 3);
    TS_ASSERT_EQUALS(foo.size(), 3);
    it = foo.begin();
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 17);
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

    fixed_list<int> a(size);
    int i=0;
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       *it = i++;
    }

    i = size - 1;
    for (fixed_list<int>::reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
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

    int ary[3] = {0,1,2};
    const fixed_list<int> a(size,ary,ary+3);  

    int i = size - 1;
    for (fixed_list<int>::const_reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_size(void)
  {
    unsigned size = 3;
    const fixed_list<int> a(size);
    TS_ASSERT_EQUALS(a.size(), size);
  }

  void test_swap(void)
  {
    fixed_list<int> foo(16,3, 100);   // three ints with a value of 100
    fixed_list<int> bar(8, 5, 200);   // five ints with a value of 200

    foo.swap(bar);
    TS_ASSERT_EQUALS(foo.size(), 5);
    
    for (fixed_list<int>::iterator it=foo.begin(); it != foo.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 200);
    }

    TS_ASSERT_EQUALS(bar.size(), 3);
    for (fixed_list<int>::iterator it=bar.begin(); it != bar.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);
    bar.swap(foo);
    TS_ASSERT_EQUALS(foo.size(), 6);
    for (fixed_list<int>::iterator it=foo.begin(); it != foo.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    TS_ASSERT_EQUALS(bar.size(), 5);
    for (fixed_list<int>::iterator it=bar.begin(); it != bar.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 200);
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
    int i=0;
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       *it = i++;
    }

    fixed_list<int, 3> b(a);
    fixed_list<int>::iterator bit = b.begin();
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       TS_ASSERT_EQUALS(*bit, *it);
    }
  }

  void assignment_method(fixed_list<int>& x)
  {
    int i=0;
    for (fixed_list<int>::iterator it=x.begin();it!=x.end();++it)
    {
       *it = i++;
    }
  }

  void read_method(const fixed_list<int>& x)
  {
     int i=0;
    for (fixed_list<int>::const_iterator it=x.begin();it!=x.end();++it)
    {
       TS_ASSERT_EQUALS(*it, i);
    }
  }

  void copy_method(fixed_list<int> x)
  {
    int i=0;
    for (fixed_list<int>::iterator it=x.begin();it!=x.end();++it)
    {
       TS_ASSERT_EQUALS(*it, i);
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
    TS_ASSERT_THROWS(fixed_list<int> a3(3), std::runtime_error);

    allocation_guard::disable();
    fixed_list<int> a3(3);
    TS_ASSERT_EQUALS(a3.size(), 3);
  }

  void test_specialized_copy_constructor(void)
  {
    unsigned size = 3;

    fixed_list<int> a(size);
    int i=0;
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       *it = i;
    }

    allocation_guard::enable();
    TS_ASSERT_THROWS(fixed_list<int> b(a), std::runtime_error);

    allocation_guard::disable();
    fixed_list<int> b(a);
    fixed_list<int>::iterator bit=b.begin();
    for (fixed_list<int>::iterator it=a.begin();it!=a.end();++it)
    {
       TS_ASSERT_EQUALS(*bit, *it);
    }
  }

  void test_relational_operators(void)
  {
    int aAry[5] = { 10, 20, 30, 40, 50 };
    int bAry[5] = { 10, 20, 30, 40, 50 };
    int cAry[5] = { 50, 40, 30, 20, 10 };
    fixed_list<int, 5> a;
    fixed_list<int, 5> b;
    fixed_list<int, 5> c;
    
    a.assign(aAry,aAry+5);
    a.assign(bAry,bAry+5);
    a.assign(cAry,cAry+5);

    TS_ASSERT((a == b));
    TS_ASSERT(b != c);
    TS_ASSERT(b < c);
    TS_ASSERT(c > b);
    TS_ASSERT(a <= b);
    TS_ASSERT(a >= b);
  }

};
