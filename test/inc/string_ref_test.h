#include <cxxtest/TestSuite.h>

#include <flex/fixed_string.h>
#include <flex/string_ref.h>

class string_ref_test: public CxxTest::TestSuite
{
  typedef flex::fixed_string<16> str;
  typedef flex::string_ref str_ref;

public:

  void setUp()
  {
    flex::allocation_guard::enable();
    errno = 0;
  }

  void tearDown()
  {
    TS_ASSERT(!errno);
    flex::allocation_guard::disable();
  }

  bool is_container_valid(const str_ref& c)
  {
    return c.validate();
  }

  void test_default_constructor()
  {
    /*
     * Case1: Normal condition
     */
    str_ref r1;
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));
  }

  void test_string_constructor()
  {
    /*
     * Case1: Empty string
     */
    str s1;
    str_ref r1(s1);
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str s2("0123");
    str_ref r2(s2);
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_substring_constructor()
  {
    /*
     * Case1: Empty string
     */
    str s1;
    str_ref r1(s1, 0, 0);
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str s2("01234567");
    str_ref r2(s2, 2, 4);
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '2');
    TS_ASSERT_EQUALS(r2[1], '3');
    TS_ASSERT_EQUALS(r2[2], '4');
    TS_ASSERT_EQUALS(r2[3], '5');

    /*
     * Case3: Populated string with npos
     */
    str s3("01234567");
    str_ref r3(s3, 4, str_ref::npos);
    TS_ASSERT_EQUALS(r3.size(), 4);
    TS_ASSERT(is_container_valid(r3));
    TS_ASSERT_EQUALS(r3[0], '4');
    TS_ASSERT_EQUALS(r3[1], '5');
    TS_ASSERT_EQUALS(r3[2], '6');
    TS_ASSERT_EQUALS(r3[3], '7');
  }

  void test_buffer_constructor()
  {
    /*
     * Case1: Empty string
     */
    str_ref r1("", (size_t) 0);
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str_ref r2("01234567", (size_t) 4);
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_cstring_constructor()
  {
    /*
     * Case1: Empty string
     */
    str_ref r1("");
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str_ref r2("0123");
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_range_constructor()
  {
    /*
     * Case1: Empty string
     */
    str s1;
    str_ref r1(s1.begin(), s1.end());
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str s2("0123");
    str_ref r2(s2.begin(), s2.end());
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_assign_string()
  {
    /*
     * Case1: Empty string
     */
    str s1;
    str_ref r1;
    r1.assign(s1);
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str s2("0123");
    str_ref r2;
    r2.assign(s2);
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_assign_substring()
  {
    /*
     * Case1: Empty string
     */
    str s1;
    str_ref r1;
    r1.assign(s1, 0, 0);
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str s2("01234567");
    str_ref r2;
    r2.assign(s2, 2, 4);
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '2');
    TS_ASSERT_EQUALS(r2[1], '3');
    TS_ASSERT_EQUALS(r2[2], '4');
    TS_ASSERT_EQUALS(r2[3], '5');

    /*
     * Case3: Populated string with npos
     */
    str s3("01234567");
    str_ref r3;
    r3.assign(s3, 4, str_ref::npos);
    TS_ASSERT_EQUALS(r3.size(), 4);
    TS_ASSERT(is_container_valid(r3));
    TS_ASSERT_EQUALS(r3[0], '4');
    TS_ASSERT_EQUALS(r3[1], '5');
    TS_ASSERT_EQUALS(r3[2], '6');
    TS_ASSERT_EQUALS(r3[3], '7');
  }

  void test_assign_buffer()
  {
    /*
     * Case1: Empty string
     */
    str_ref r1;
    r1.assign("", (size_t) 0);
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str_ref r2;
    r2.assign("01234567", (size_t) 4);
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_assign_cstring()
  {
    /*
     * Case1: Empty string
     */
    str_ref r1;
    r1.assign("");
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str_ref r2;
    r2.assign("0123");
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_assign_range()
  {
    /*
     * Case1: Empty string
     */
    str s1;
    str_ref r1;
    r1.assign(s1.begin(), s1.end());
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str s2("0123");
    str_ref r2;
    r2.assign(s2.begin(), s2.end());
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_operator_assignment_string()
  {
    /*
     * Case1: Empty string
     */
    str s1;
    str_ref r1;
    r1 = s1;
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str s2("0123");
    str_ref r2;
    r2 = s2;
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

  void test_operator_assignment_cstring()
  {
    /*
     * Case1: Empty string
     */
    str_ref r1;
    r1 = "";
    TS_ASSERT_EQUALS(r1.size(), 0);
    TS_ASSERT(is_container_valid(r1));

    /*
     * Case2: Populated string
     */
    str_ref r2;
    r2 = "0123";
    TS_ASSERT_EQUALS(r2.size(), 4);
    TS_ASSERT(is_container_valid(r2));
    TS_ASSERT_EQUALS(r2[0], '0');
    TS_ASSERT_EQUALS(r2[1], '1');
    TS_ASSERT_EQUALS(r2[2], '2');
    TS_ASSERT_EQUALS(r2[3], '3');
  }

}
;
