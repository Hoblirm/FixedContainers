#include <cxxtest/TestSuite.h>

#include <fixed_string.h>

class fixed_string_test: public CxxTest::TestSuite
{
public:

  void test_constructor(void)
    {
      allocation_guard::enable();
      fixed_string<3> a;
      TS_ASSERT_EQUALS(a.size(), 3);
      allocation_guard::disable();
    }

    void test_copy_constructor(void)
    {
      fixed_string<3> a;
      for (int i = 0; i < a.size(); i++)
      {
        a.data()[i] = i;
      }

      fixed_string<3> b(a);
      for (int i = 0; i < b.size(); i++)
      {
        TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
      }
    }

    void assignment_method(fixed_string<>& x)
    {
      for (int i = 0; i < x.size(); i++)
      {
        x[i] = i;
      }
    }

    void read_method(const fixed_string<>& x)
    {
      for (int i = 0; i < x.size(); i++)
      {
        TS_ASSERT_EQUALS(x[i], i);
      }
    }

    void copy_method(fixed_string<> x)
    {
      for (int i = 0; i < x.size(); i++)
      {
        TS_ASSERT_EQUALS(x[i], i);
      }
    }

    void test_cast_operator(void)
    {
      allocation_guard::enable();
      fixed_string<8> a;
      assignment_method(a);
      read_method(a);
      TS_ASSERT_THROWS(copy_method(a), std::runtime_error);

      allocation_guard::disable();
      copy_method(a);
    }

    void test_specialized_constructor(void)
    {
      allocation_guard::enable();
      TS_ASSERT_THROWS(fixed_string<> a3(3), std::runtime_error);

      allocation_guard::disable();
      fixed_string<> a3(3);
      TS_ASSERT_EQUALS(a3.size(), 3);
    }

    void test_specialized_copy_constructor(void)
    {
      unsigned size = 3;

      fixed_string<> a(size);
      for (int i = 0; i < size; i++)
      {
        a.data()[i] = i;
      }

      allocation_guard::enable();
      TS_ASSERT_THROWS(fixed_string<> b(a), std::runtime_error);

      allocation_guard::disable();
      fixed_string<> b(a);
      for (int i = 0; i < size; i++)
      {
        TS_ASSERT_EQUALS(b.data()[i], a.data()[i]);
      }
    }

};
