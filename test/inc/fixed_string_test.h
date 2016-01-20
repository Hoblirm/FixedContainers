#include <cxxtest/TestSuite.h>

#include <fixed_string.h>

class fixed_string_test: public CxxTest::TestSuite
{
public:

  void test_specialized_constructor(void)
  {
    typedef fixed_string<> fixed_str;
    fixed_str str(10);
    fixed_string<10> str2;
  }

  void assignment_method(fixed_string<>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      x[i] = ('A' + (char)i);
    }
  }

  void read_method(const fixed_string<>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], ('A' + (char)i));
    }
  }

  void copy_method(fixed_string<> x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], ('A' + (char)i));
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

};
