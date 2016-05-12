#include <cxxtest/TestSuite.h>

#include <flex/fixed_string.h>

class fixed_string_test: public CxxTest::TestSuite
{
  typedef flex::fixed_string<16> str;

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

  bool is_container_valid(const str& c)
  {
    if (c.size() != strlen(c.c_str()))
    {
      printf("Error: Expected (c.size() == strlen(c.c_str())), found (%zu != %zu)\n", c.size(), strlen(c.c_str()));
      return false;
    }

    return c.validate();
  }

  void test_default_constructor()
  {
    /*
     * Case1: Normal conditions
     */
    str a;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
  }

  void test_from_c_string_constructor()
  {
    /*
     * Case1: Empty c string
     */
    str a("");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);

    /*
     * Case2: Populated c string
     */
    str b("01234567");
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 8);
    TS_ASSERT_EQUALS(b.c_str(), "01234567");
  }

  void test_copy_constructor()
  {
    /*
     * Case1: Empty string
     */
    str a;
    str b(a);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 0);

    /*
     * Case2: Populated string
     */
    str c("01234567");
    str d(c);
    TS_ASSERT_EQUALS(d.size(), 8);
    TS_ASSERT_EQUALS(d.c_str(), "01234567");
  }

  void test_substring_constructor()
  {
    /*
     * Case1: Empty string
     */
    str a;
    str b(a, 0, 0);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 0);

    /*
     * Case2: Populated string
     */
    str c("01234567");
    str d(c, 2, 4);
    TS_ASSERT_EQUALS(d.size(), 4);
    TS_ASSERT_EQUALS(d.c_str(), "2345");

    /*
     * Case3: Out of range substring
     */
    str e(c, 2, 32);
    TS_ASSERT_EQUALS(e.size(), 6);
    TS_ASSERT_EQUALS(e.c_str(), "234567");
  }

  void test_from_buffer_constructor()
  {
    /*
     * Case1: Empty string
     */
    str a;
    str b(a.c_str(), 0);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 0);

    /*
     * Case2: Populated string
     */
    str c("01234567");
    str d(c.c_str(), 4);
    TS_ASSERT_EQUALS(d.size(), 4);
    TS_ASSERT_EQUALS(d.c_str(), "0123");
  }

  void test_fill_constructor()
  {
    /*
     * Case1: Empty string
     */
    str a(0, '8');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);

    /*
     * Case2: Populated string
     */
    str b(8, '8');
    TS_ASSERT_EQUALS(b.size(), 8);
    TS_ASSERT_EQUALS(b.c_str(), "88888888");

    /*
     * Case3: Size exceeds capacity
     */
    TS_ASSERT(!errno);
    str c(17, '1');
    TS_ASSERT(errno);
    errno = 0;
    TS_ASSERT_EQUALS(c.size(), 17);
    TS_ASSERT_EQUALS(c.c_str(), "11111111111111111");
  }

  void test_range_constructor()
  {
    /*
     * Case1: Empty string
     */
    str a;
    str b(a.begin(), a.end());
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 0);

    /*
     * Case2: Populated string
     */
    str c("01234567");
    str d(c.begin(), c.end());
    TS_ASSERT(is_container_valid(d));
    TS_ASSERT_EQUALS(d.size(), 8);
    TS_ASSERT_EQUALS(d.c_str(), "01234567");
  }

  void test_initializer_constructor()
  {
    /*
     * Case1: Populated c string
     */
    str b( { '0', '1', '2', '3', '4', '5', '6', '7' });
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 8);
    TS_ASSERT_EQUALS(b.c_str(), "01234567");
  }

  void test_append_string()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a.append(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_append_substring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("01234567");
    a.append(b, 4, 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_append_cstring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.append("4567");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_append_buffer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a.append(b.c_str(), 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_append_fill()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.append(4, '4');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234444");
  }

  void test_append_range()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a.append(b.begin(), b.end());
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_append_initializer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.append( { '4', '5', '6', '7' });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_assign_string()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a.assign(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_assign_substring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("01234567");
    a.assign(b, 4, 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_assign_cstring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.assign("4567");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_assign_buffer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a.assign(b.c_str(), 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_assign_fill()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.assign(4, '4');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4444");

    /*
     * Case2: Size exceeds capacity
     */
    str b("0123");
    TS_ASSERT(!errno);
    b.assign(17, '1');
    TS_ASSERT(errno);
    errno = 0;
    TS_ASSERT_EQUALS(b.size(), 17);
    TS_ASSERT_EQUALS(b.c_str(), "11111111111111111");
  }

  void test_assign_range()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a.assign(b.begin(), b.end());
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_assign_initializer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.assign( { '4', '5', '6', '7' });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_assign_move()
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    const char* b_ptr = b.c_str();
    allocation_guard::enable();
    a.assign(std::move(b));
    allocation_guard::disable();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    //Ensure pointers aren't actually moved, as this is invalid for
    //fixed containers.
    TS_ASSERT_DIFFERS((void*)a.c_str(), (void*)b_ptr);
    TS_ASSERT_EQUALS((void*)b.c_str(), (void*)b_ptr);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
#endif
  }

  void test_at()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    TS_ASSERT_EQUALS(a.at(0), '0');
    TS_ASSERT_EQUALS(a.at(1), '1');
    TS_ASSERT_EQUALS(a.at(2), '2');
    TS_ASSERT_EQUALS(a.at(3), '3');

    /*
     * Case2: Out-of-range
     */
    TS_ASSERT_THROWS(a.at(-1), std::out_of_range);
    TS_ASSERT_THROWS(a.at(4), std::out_of_range);
  }

  void test_at_const()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    TS_ASSERT_EQUALS(a.at(0), '0');
    TS_ASSERT_EQUALS(a.at(1), '1');
    TS_ASSERT_EQUALS(a.at(2), '2');
    TS_ASSERT_EQUALS(a.at(3), '3');

    /*
     * Case2: Out-of-range
     */
    TS_ASSERT_THROWS(a.at(-1), std::out_of_range);
    TS_ASSERT_THROWS(a.at(4), std::out_of_range);
  }

  void test_back()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    TS_ASSERT_EQUALS(a.back(), '3');
  }

  void test_back_const()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    TS_ASSERT_EQUALS(a.back(), '3');
  }

  void test_begin()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str::iterator it = a.begin();
    TS_ASSERT_EQUALS(*(it++), '0');
    TS_ASSERT_EQUALS(*(it++), '1');
    TS_ASSERT_EQUALS(*(it++), '2');
    TS_ASSERT_EQUALS(*(it++), '3');
  }

  void test_begin_const()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    str::const_iterator it = a.begin();
    TS_ASSERT_EQUALS(*(it++), '0');
    TS_ASSERT_EQUALS(*(it++), '1');
    TS_ASSERT_EQUALS(*(it++), '2');
    TS_ASSERT_EQUALS(*(it++), '3');
  }

  void test_capacity()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    TS_ASSERT_EQUALS(a.capacity(), 16);
    a.append("4");
    TS_ASSERT_EQUALS(a.capacity(), 16);
    a.assign("123");
    TS_ASSERT_EQUALS(a.capacity(), 16);
  }

  void test_cbegin()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    str::const_iterator it = a.cbegin();
    TS_ASSERT_EQUALS(*(it++), '0');
    TS_ASSERT_EQUALS(*(it++), '1');
    TS_ASSERT_EQUALS(*(it++), '2');
    TS_ASSERT_EQUALS(*(it++), '3');
  }

  void test_cend()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    str::const_iterator it = a.cend();
    TS_ASSERT_EQUALS(*(--it), '3');
    TS_ASSERT_EQUALS(*(--it), '2');
    TS_ASSERT_EQUALS(*(--it), '1');
    TS_ASSERT_EQUALS(*(--it), '0');
  }

  void test_clear()
  {
    /*
     * Case1: Empty string
     */
    str a;
    a.clear();
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Normal conditions
     */
    str b("0123");
    b.clear();
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 0);
  }

  void test_compare_string()
  {
    const str a("0123");
    const str b("4567");

    /*
     * Case1: Less than
     */
    TS_ASSERT_LESS_THAN(a.compare(b), 0);

    /*
     * Case2: Equal
     */
    TS_ASSERT_EQUALS(a.compare(a), 0);

    /*
     * Case3: Greater than
     */
    TS_ASSERT_LESS_THAN(0, b.compare(a));
  }

  void test_compare_cstring()
  {
    const str a("0123");
    const str b("4567");

    /*
     * Case1: Less than
     */
    TS_ASSERT_LESS_THAN(a.compare("4567"), 0);

    /*
     * Case2: Equal
     */
    TS_ASSERT_EQUALS(a.compare("0123"), 0);

    /*
     * Case3: Greater than
     */
    TS_ASSERT_LESS_THAN(0, b.compare("0123"));
  }

  void test_compare_substring_to_string()
  {
    const str a("0123");
    const str b("4567");

    /*
     * Case1: Less than
     */
    TS_ASSERT_LESS_THAN(a.compare(0, 4, b), 0);

    /*
     * Case2: Equal
     */
    TS_ASSERT_EQUALS(a.compare(0, 4, a), 0);

    /*
     * Case3: Greater than
     */
    TS_ASSERT_LESS_THAN(0, b.compare(0, 4, a));
  }

  void test_compare_substring_to_substring()
  {
    const str a("0123");
    const str b("4567");

    /*
     * Case1: Less than
     */
    TS_ASSERT_LESS_THAN(a.compare(0, 4, b, 0, 4), 0);

    /*
     * Case2: Equal
     */
    TS_ASSERT_EQUALS(a.compare(0, 4, a, 0, 4), 0);

    /*
     * Case3: Greater than
     */
    TS_ASSERT_LESS_THAN(0, b.compare(0, 4, a, 0, 4));
  }

  void test_compare_substring_to_cstring()
  {
    const str a("0123");
    const str b("4567");

    /*
     * Case1: Less than
     */
    TS_ASSERT_LESS_THAN(a.compare(0, 4, "4567"), 0);

    /*
     * Case2: Equal
     */
    TS_ASSERT_EQUALS(a.compare(0, 4, "0123"), 0);

    /*
     * Case3: Greater than
     */
    TS_ASSERT_LESS_THAN(0, b.compare(0, 4, "0123"));
  }

  void test_compare_substring_to_buffer()
  {
    const str a("0123");
    const str b("4567");

    /*
     * Case1: Less than
     */
    TS_ASSERT_LESS_THAN(a.compare(0, 4, b.c_str(), 4), 0);

    /*
     * Case2: Equal
     */
    TS_ASSERT_EQUALS(a.compare(0, 4, a.c_str(), 4), 0);

    /*
     * Case3: Greater than
     */
    TS_ASSERT_LESS_THAN(0, b.compare(0, 4, a.c_str(), 4));
  }

  void test_copy()
  {
    const str a("0123");

    /*
     * Case1: Normal conditions
     */
    char b[5];
    int i = a.copy(b, 4, 0);
    b[4] = '\0';
    TS_ASSERT_EQUALS(i, 4);
    TS_ASSERT_EQUALS(b, "0123");

    /*
     * Case2: String is shorter than copy length
     */
    char c[5];
    i = a.copy(c, 8, 0);
    c[4] = '\0';
    TS_ASSERT_EQUALS(i, 4);
    TS_ASSERT_EQUALS(c, "0123");
  }

  void test_crbegin()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    str::const_reverse_iterator it = a.crbegin();
    TS_ASSERT_EQUALS(*(it++), '3');
    TS_ASSERT_EQUALS(*(it++), '2');
    TS_ASSERT_EQUALS(*(it++), '1');
    TS_ASSERT_EQUALS(*(it++), '0');
  }

  void test_crend()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    str::const_reverse_iterator it = a.crend();
    TS_ASSERT_EQUALS(*(--it), '0');
    TS_ASSERT_EQUALS(*(--it), '1');
    TS_ASSERT_EQUALS(*(--it), '2');
    TS_ASSERT_EQUALS(*(--it), '3');
  }

  void test_cstr()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    TS_ASSERT_EQUALS(a.c_str(), "0123");
  }

  void test_data()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    TS_ASSERT_EQUALS(a.data(), "0123");
  }

  void test_empty()
  {
    /*
     * Case1: Empty
     */
    const str a;
    TS_ASSERT(a.empty());

    /*
     * Case2: Populated
     */
    const str b("0123");
    TS_ASSERT(!b.empty());
  }

  void test_end()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str::const_iterator it = a.end();
    TS_ASSERT_EQUALS(*(--it), '3');
    TS_ASSERT_EQUALS(*(--it), '2');
    TS_ASSERT_EQUALS(*(--it), '1');
    TS_ASSERT_EQUALS(*(--it), '0');
  }

  void test_erase_sequence()
  {
    /*
     * Case1: Erase at begin
     */
    str a("01234567");
    a.erase(0, 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");

    /*
     * Case2: Erase in middle
     */
    str b("01234567");
    b.erase(2, 4);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 4);
    TS_ASSERT_EQUALS(b.c_str(), "0167");

    /*
     * Case3: Erase at end
     */
    str c("01234567");
    c.erase(4, 4);
    TS_ASSERT(is_container_valid(c));
    TS_ASSERT_EQUALS(c.size(), 4);
    TS_ASSERT_EQUALS(c.c_str(), "0123");
  }

  void test_erase_character()
  {
    /*
     * Case1: Erase at begin
     */
    str a("01234567");
    a.erase(a.begin());
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 7);
    TS_ASSERT_EQUALS(a.c_str(), "1234567");

    /*
     * Case2: Erase in middle
     */
    str b("01234567");
    b.erase(b.begin() + 4);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 7);
    TS_ASSERT_EQUALS(b.c_str(), "0123567");

    /*
     * Case3: Erase at end
     */
    str c("01234567");
    c.erase(c.end() - 1);
    TS_ASSERT(is_container_valid(c));
    TS_ASSERT_EQUALS(c.size(), 7);
    TS_ASSERT_EQUALS(c.c_str(), "0123456");
  }

  void test_erase_range()
  {
    /*
     * Case1: Erase at begin
     */
    str a("01234567");
    a.erase(a.begin(), a.begin() + 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");

    /*
     * Case2: Erase in middle
     */
    str b("01234567");
    b.erase(b.begin() + 2, b.begin() + 6);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 4);
    TS_ASSERT_EQUALS(b.c_str(), "0167");

    /*
     * Case3: Erase at end
     */
    str c("01234567");
    c.erase(c.begin() + 4, c.end());
    TS_ASSERT(is_container_valid(c));
    TS_ASSERT_EQUALS(c.size(), 4);
    TS_ASSERT_EQUALS(c.c_str(), "0123");
  }

  void test_find_string()
  {
    str a("01234567");
    /*
     * Case1: String found
     */
    TS_ASSERT_EQUALS(a.find(str("2345")), 2);

    /*
     * Case2: String not found
     */
    TS_ASSERT_EQUALS(a.find(str("8")), str::npos);
  }

  void test_find_cstring()
  {
    str a("01234567");
    /*
     * Case1: CString found
     */
    TS_ASSERT_EQUALS(a.find("2345"), 2);

    /*
     * Case2: CString not found
     */
    TS_ASSERT_EQUALS(a.find("8"), str::npos);
  }

  void test_find_buffer()
  {
    str a("01234567");

    /*
     * Case1: Buffer found
     */
    char buf[5] = "2345";
    TS_ASSERT_EQUALS(a.find(buf, 0, 4), 2);

    /*
     * Case2: Buffer not found
     */
    char buf2[2] = "8";
    TS_ASSERT_EQUALS(a.find(buf2, 0, 1), str::npos);
  }

  void test_find_character()
  {
    str a("01234567");
    /*
     * Case1: Character found
     */
    TS_ASSERT_EQUALS(a.find('2'), 2);

    /*
     * Case2: Character not found
     */
    TS_ASSERT_EQUALS(a.find('8'), str::npos);
  }

  void test_find_first_not_of_string()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_first_not_of(str("0123")), 4);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_first_not_of(str("01234567")), str::npos);
  }

  void test_find_first_not_of_cstring()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_first_not_of("0123"), 4);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_first_not_of("01234567"), str::npos);
  }

  void test_find_first_not_of_buffer()
  {
    str a("01234567");

    /*
     * Case1: Index found
     */
    char buf[5] = "0123";
    TS_ASSERT_EQUALS(a.find_first_not_of(buf, 0, 4), 4);

    /*
     * Case2: Index not found
     */
    char buf2[9] = "01234567";
    TS_ASSERT_EQUALS(a.find_first_not_of(buf2, 0, 8), str::npos);
  }

  void test_find_first_not_of_character()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_first_not_of('0'), 1);

    /*
     * Case2: Index not found
     */
    str b("88888888");
    TS_ASSERT_EQUALS(b.find_first_not_of('8'), str::npos);
  }

  void test_find_first_of_string()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_first_of(str("2345")), 2);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_first_of(str("89AB")), str::npos);
  }

  void test_find_first_of_cstring()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_first_of("2345"), 2);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_first_of("89AB"), str::npos);
  }

  void test_find_first_of_buffer()
  {
    str a("01234567");

    /*
     * Case1: Index found
     */
    char buf[5] = "2345";
    TS_ASSERT_EQUALS(a.find_first_of(buf, 0, 4), 2);

    /*
     * Case2: Index not found
     */
    char buf2[5] = "89AB";
    TS_ASSERT_EQUALS(a.find_first_of(buf2, 0, 4), str::npos);
  }

  void test_find_first_of_character()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_first_of('2'), 2);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_first_of('8'), str::npos);
  }

  void test_find_last_not_of_string()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_last_not_of(str("4567")), 3);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_last_not_of(str("01234567")), str::npos);
  }

  void test_find_last_not_of_cstring()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_last_not_of("4567"), 3);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_last_not_of("01234567"), str::npos);
  }

  void test_find_last_not_of_buffer()
  {
    str a("01234567");

    /*
     * Case1: Index found
     */
    char buf[5] = "0123";
    TS_ASSERT_EQUALS(a.find_last_not_of(buf, 7, 4), 7);

    /*
     * Case2: Index not found
     */
    char buf2[9] = "01234567";
    TS_ASSERT_EQUALS(a.find_last_not_of(buf2, 0, 8), str::npos);
  }

  void test_find_last_not_of_character()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_last_not_of('7'), 6);

    /*
     * Case2: Index not found
     */
    str b("88888888");
    TS_ASSERT_EQUALS(b.find_last_not_of('8'), str::npos);
  }

  void test_find_last_of_string()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_last_of(str("2345")), 5);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_last_of(str("89AB")), str::npos);
  }

  void test_find_last_of_cstring()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_last_of("2345"), 5);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_last_of("89AB"), str::npos);
  }

  void test_find_last_of_buffer()
  {
    str a("01234567");

    /*
     * Case1: Index found
     */
    char buf[5] = "2345";
    TS_ASSERT_EQUALS(a.find_last_of(buf, 7, 4), 5);

    /*
     * Case2: Index not found
     */
    char buf2[5] = "89AB";
    TS_ASSERT_EQUALS(a.find_last_of(buf2, 0, 4), str::npos);
  }

  void test_find_last_of_character()
  {
    str a("01234567");
    /*
     * Case1: Index found
     */
    TS_ASSERT_EQUALS(a.find_last_of('2'), 2);

    /*
     * Case2: Index not found
     */
    TS_ASSERT_EQUALS(a.find_last_of('8'), str::npos);
  }

  void test_front()
  {
    /*
     * Case1: Normal condition
     */
    str a("0123");
    TS_ASSERT_EQUALS(a.front(), '0');
  }

  void test_front_const()
  {
    /*
     * Case1: Normal condition
     */
    const str a("0123");
    TS_ASSERT_EQUALS(a.front(), '0');
  }

  void test_get_allocator()
  {
    /*
     * Case1: Normal condition
     */
    str a;
    str b;
    TS_ASSERT_EQUALS(a.get_allocator(), b.get_allocator());
  }

  void test_insert_string()
  {
    str a("25");

    /*
     * Case1: At beginning
     */
    str b("01");
    a.insert(0, b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0125");

    /*
     * Case2: At end
     */
    str c("67");
    a.insert(4, c);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012567");

    /*
     * Case3: In middle
     */
    str d("34");
    a.insert(3, d);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_insert_substring()
  {
    str a("25");
    str b("01234567");

    /*
     * Case1: At beginning
     */
    a.insert(0, b, 0, 2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0125");

    /*
     * Case2: At end
     */
    a.insert(4, b, 6, 2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012567");

    /*
     * Case3: In middle
     */
    a.insert(3, b, 3, 2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_insert_cstring()
  {
    str a("25");

    /*
     * Case1: At beginning
     */
    a.insert(0, "01");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0125");

    /*
     * Case2: At end
     */
    a.insert(4, "67");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012567");

    /*
     * Case3: In middle
     */
    a.insert(3, "34");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_insert_buffer()
  {
    str a("25");
    char buf[9] = "01234567";

    /*
     * Case1: At beginning
     */
    a.insert(0, buf, 2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0125");

    /*
     * Case2: At end
     */
    a.insert(4, buf + 6, 2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012567");

    /*
     * Case3: In middle
     */
    a.insert(3, buf + 3, 2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_insert_fill()
  {
    str a("25");

    /*
     * Case1: At beginning
     */
    a.insert((size_t) 0, 2, '0');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0025");

    /*
     * Case2: At end
     */
    a.insert((size_t) 4, 2, '6');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "002566");

    /*
     * Case3: In middle
     */
    a.insert((size_t) 3, 2, '3');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "00233566");
  }

  void test_insert_character()
  {
    str a("25");

    /*
     * Case1: At beginning
     */
    a.insert(a.begin(), '0');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 3);
    TS_ASSERT_EQUALS(a.c_str(), "025");

    /*
     * Case2: At end
     */
    a.insert(a.end(), '6');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0256");

    /*
     * Case3: In middle
     */
    a.insert(a.begin() + 2, '3');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 5);
    TS_ASSERT_EQUALS(a.c_str(), "02356");
  }

  void test_insert_range()
  {
    str a("25");

    /*
     * Case1: At beginning
     */
    str b("01");
    a.insert(a.begin(), b.begin(), b.end());
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0125");

    /*
     * Case2: At end
     */
    str c("67");
    a.insert(a.end(), c.begin(), c.end());
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012567");

    /*
     * Case3: In middle
     */
    str d("34");
    a.insert(a.begin() + 3, d.begin(), d.end());
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_insert_initializer()
  {
    str a("25");

    /*
     * Case1: At beginning
     */
    a.insert(a.begin(), { '0', '1' });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "0125");

    /*
     * Case2: At end
     */
    a.insert(a.begin() + 4, { '6', '7' });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012567");

    /*
     * Case3: In middle
     */
    a.insert(a.begin() + 3, { '3', '4' });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_length()
  {
    /*
     * Case1: Empty
     */
    str a;
    TS_ASSERT_EQUALS(a.length(), 0);

    /*
     * Case1: Populated
     */
    str b("0123");
    TS_ASSERT_EQUALS(b.length(), 4);
  }

  void test_max_size()
  {
    /*
     * Case1: Normal condition
     */
    str a;
    TS_ASSERT_EQUALS(a.max_size(), str::npos - 1);
  }

  void test_operator_plus_set_string()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a += b;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_operator_plus_set_cstring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a += "4567";
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_operator_plus_set_char()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a += '4';
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 5);
    TS_ASSERT_EQUALS(a.c_str(), "01234");
  }

  void test_operator_plus_set_initializer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a +=
    { '4','5','6','7'};
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");
  }

  void test_operator_assign_string()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    a = b;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_operator_assign_cstring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a = "4567";
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_operator_assign_char()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a = '4';
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 1);
    TS_ASSERT_EQUALS(a.c_str(), "4");
  }

  void test_operator_assign_initializer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a =
    { '4','5','6','7'};
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
  }

  void test_operator_assign_move()
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str b("4567");
    const char* ptr = b.c_str();
    a = std::move(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_DIFFERS((void*)a.c_str(), (void*)ptr);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
#endif
  }

  void test_operator_reference()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    TS_ASSERT_EQUALS(a[0], '0');
    TS_ASSERT_EQUALS(a[1], '1');
    TS_ASSERT_EQUALS(a[2], '2');
    TS_ASSERT_EQUALS(a[3], '3');
  }

  void test_operator_reference_const()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    TS_ASSERT_EQUALS(a[0], '0');
    TS_ASSERT_EQUALS(a[1], '1');
    TS_ASSERT_EQUALS(a[2], '2');
    TS_ASSERT_EQUALS(a[3], '3');
  }

  void test_pop_back()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.pop_back();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 3);
    TS_ASSERT_EQUALS(a.c_str(), "012");
  }

  void test_push_back()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    a.push_back('4');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 5);
    TS_ASSERT_EQUALS(a.c_str(), "01234");
  }

  void test_rbegin()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str::reverse_iterator it = a.rbegin();
    TS_ASSERT_EQUALS(*(it++), '3');
    TS_ASSERT_EQUALS(*(it++), '2');
    TS_ASSERT_EQUALS(*(it++), '1');
    TS_ASSERT_EQUALS(*(it++), '0');
  }

  void test_rbegin_const()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    str::const_reverse_iterator it = a.rbegin();
    TS_ASSERT_EQUALS(*(it++), '3');
    TS_ASSERT_EQUALS(*(it++), '2');
    TS_ASSERT_EQUALS(*(it++), '1');
    TS_ASSERT_EQUALS(*(it++), '0');
  }

  void test_rend()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0123");
    str::reverse_iterator it = a.rend();
    TS_ASSERT_EQUALS(*(--it), '0');
    TS_ASSERT_EQUALS(*(--it), '1');
    TS_ASSERT_EQUALS(*(--it), '2');
    TS_ASSERT_EQUALS(*(--it), '3');
  }

  void test_rend_const()
  {
    /*
     * Case1: Normal conditions
     */
    const str a("0123");
    str::const_reverse_iterator it = a.rend();
    TS_ASSERT_EQUALS(*(--it), '0');
    TS_ASSERT_EQUALS(*(--it), '1');
    TS_ASSERT_EQUALS(*(--it), '2');
    TS_ASSERT_EQUALS(*(--it), '3');
  }

  void test_replace_string()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");
    str b("1234");

    a.replace(1, 2, b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_string_iterators()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");
    str b("1234");

    a.replace(a.begin() + 1, a.begin() + 3, b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_substring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("018945");
    str b("1234");

    a.replace(2, 2, b, 1, 2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_cstring()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");

    a.replace(1, 2, "1234");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_cstring_iterators()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");

    a.replace(a.begin() + 1, a.begin() + 3, "1234");
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_buffer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");
    char buf[5] = "1234";

    a.replace(1, 2, buf, 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_buffer_iterators()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");
    char buf[5] = "1234";

    a.replace(a.begin() + 1, a.begin() + 3, buf, 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_fill()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");

    a.replace(1, 2, 4, '1');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "011115");
  }

  void test_replace_fill_iterators()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");

    a.replace(a.begin() + 1, a.begin() + 3, 4, '1');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "011115");
  }

  void test_replace_range()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");
    str b("1234");

    a.replace(a.begin() + 1, a.begin() + 3, b.begin(), b.end());
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_replace_initializer()
  {
    /*
     * Case1: Normal conditions
     */
    str a("0895");

    a.replace(a.begin() + 1, a.begin() + 3, { '1', '2', '3', '4' });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 6);
    TS_ASSERT_EQUALS(a.c_str(), "012345");
  }

  void test_resize()
  {
    /*
     * Case1: Size is increased
     */
    str a;
    a.resize(8);
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.capacity(), 16);

    /*
     * Case1: Size is decreased.
     */
    a.resize(4);
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.capacity(), 16);
  }

  void test_resize_char()
  {
    /*
     * Case1: Size is increased
     */
    str a;
    a.resize(8, '0');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.capacity(), 16);

    /*
     * Case1: Size is decreased.
     */
    a.resize(4, '0');
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.capacity(), 16);
  }

  void test_rfind_string()
  {
    str a("01234567");
    /*
     * Case1: String found
     */
    TS_ASSERT_EQUALS(a.rfind(str("2345")), 2);

    /*
     * Case2: String not found
     */
    TS_ASSERT_EQUALS(a.rfind(str("8")), str::npos);
  }

  void test_rfind_cstring()
  {
    str a("01234567");
    /*
     * Case1: CString found
     */
    TS_ASSERT_EQUALS(a.rfind("2345"), 2);

    /*
     * Case2: CString not found
     */
    TS_ASSERT_EQUALS(a.rfind("8"), str::npos);
  }

  void test_rfind_buffer()
  {
    str a("01234567");

    /*
     * Case1: Buffer found
     */
    char buf[5] = "2345";
    TS_ASSERT_EQUALS(a.rfind(buf, 7, 4), 2);

    /*
     * Case2: Buffer not found
     */
    char buf2[2] = "8";
    TS_ASSERT_EQUALS(a.rfind(buf2, 7, 1), str::npos);
  }

  void test_rfind_character()
  {
    str a("01234567");
    /*
     * Case1: Character found
     */
    TS_ASSERT_EQUALS(a.rfind('2'), 2);

    /*
     * Case2: Character not found
     */
    TS_ASSERT_EQUALS(a.rfind('8'), str::npos);
  }

  void test_size()
  {
    /*
     * Case1: Empty
     */
    str a;
    TS_ASSERT_EQUALS(a.size(), 0);

    /*
     * Case1: Populated
     */
    str b("0123");
    TS_ASSERT_EQUALS(b.size(), 4);
  }

  void test_substr()
  {
    /*
     * Case1: Normal condition
     */
    str a("01234567");
    str b = a.substr(2, 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 4);
    TS_ASSERT_EQUALS(b.c_str(), "2345");
  }

  void test_swap()
  {
    str a;
    str b("4567");
    str c("0123");

    /*
     * Case1: Empty strings
     */
    a.swap(c);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(c));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(c.size(), 0);
    TS_ASSERT_EQUALS(a.c_str(), "0123");

    /*
     * Case2: Normal condition
     */
    a.swap(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(b.size(), 4);
    TS_ASSERT_EQUALS(a.c_str(), "4567");
    TS_ASSERT_EQUALS(b.c_str(), "0123");
  }

  /*
   * Non-member overloads
   */

  void test_operator_plus_string()
  {
    /*
     * Case1: Neither are rvalues
     */
    str a("0123");
    str b("4567");
    a = a + b;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");

#ifdef FLEX_HAS_CXX11
    /*
     * Case2: Left is rvalue
     */
    str c("0123");
    str d("4567");
    const char* c_ptr = c.c_str();
    d = std::move(c) + d;
    TS_ASSERT(is_container_valid(d));
    TS_ASSERT_EQUALS(d.size(),8);
    //We want to ensure the pointer is not actually moved, as this would be invalid
    //for a fixed container.
    TS_ASSERT_EQUALS((void*)c.c_str(),(void*)c_ptr)
    TS_ASSERT_DIFFERS((void*)d.c_str(),(void*)c_ptr)
    TS_ASSERT_EQUALS(d.c_str(),"01234567");

    /*
     * Case3: Right is rvalue
     */
    str e("0123");
    str f("4567");
    e = e + std::move(f);
    TS_ASSERT(is_container_valid(e));
    TS_ASSERT_EQUALS(e.size(),8);
    TS_ASSERT_EQUALS(e.c_str(),"01234567");

    /*
     * Case4: Both are rvalues
     */
    str g("0123");
    str h("4567");
    const char* g_ptr = g.c_str();
    str i = std::move(g) + std::move(h);
    TS_ASSERT(is_container_valid(i));
    TS_ASSERT_EQUALS(i.size(),8);
    //We want to ensure the pointer is not actually moved, as this would be invalid
    //for a fixed container.
    TS_ASSERT_EQUALS((void*)g.c_str(),(void*)g_ptr)
    TS_ASSERT_DIFFERS((void*)i.c_str(),(void*)g_ptr)
    TS_ASSERT_EQUALS(i.c_str(),"01234567");
#endif
  }

  void test_operator_plus_cstring()
  {
    /*
     * Case1: Right is cstring
     */
    str a("0123");
    a = a + "4567";
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.c_str(), "01234567");

    /*
     * Case2: Left is cstring
     */
    str b("4567");
    b = "0123" + b;
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 8);
    TS_ASSERT_EQUALS(b.c_str(), "01234567");

#ifdef FLEX_HAS_CXX11
    /*
     * Case3: Left is rvalue
     */
    str c("0123");
    const char* c_ptr = c.c_str();
    str d = std::move(c) + "4567";
    TS_ASSERT(is_container_valid(d));
    //We want to ensure the pointer is not actually moved, as this would be invalid
    //for a fixed container.
    TS_ASSERT_EQUALS((void*)c.c_str(),(void*)c_ptr)
    TS_ASSERT_DIFFERS((void*)d.c_str(),(void*)c_ptr)
    TS_ASSERT_EQUALS(d.size(), 8);
    TS_ASSERT_EQUALS(d.c_str(), "01234567");

    /*
     * Case4: Right is rvalue
     */
    str e("4567");
    const char* e_ptr = e.c_str();
    str f = "0123" + std::move(e);
    TS_ASSERT(is_container_valid(f));
    //We want to ensure the pointer is not actually moved, as this would be invalid
    //for a fixed container.
    TS_ASSERT_EQUALS((void*)e.c_str(),(void*)e_ptr)
    TS_ASSERT_DIFFERS((void*)f.c_str(),(void*)e_ptr)
    TS_ASSERT_EQUALS(f.size(), 8);
    TS_ASSERT_EQUALS(f.c_str(), "01234567");
#endif
  }

  void test_operator_plus_char()
  {
    /*
     * Case1: Right is cstring
     */
    str a("0123");
    a = a + '4';
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 5);
    TS_ASSERT_EQUALS(a.c_str(), "01234");

    /*
     * Case2: Left is cstring
     */
    str b("4567");
    b = '3' + b;
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 5);
    TS_ASSERT_EQUALS(b.c_str(), "34567");

#ifdef FLEX_HAS_CXX11
    /*
     * Case3: Left is rvalue
     */
    str c("0123");
    const char* c_ptr = c.c_str();
    str d = std::move(c) + '4';
    TS_ASSERT(is_container_valid(d));
    //We want to ensure the pointer is not actually moved, as this would be invalid
    //for a fixed container.
    TS_ASSERT_EQUALS((void*)c.c_str(),(void*)c_ptr)
    TS_ASSERT_DIFFERS((void*)d.c_str(),(void*)c_ptr)
    TS_ASSERT_EQUALS(d.size(), 5);
    TS_ASSERT_EQUALS(d.c_str(), "01234");

    /*
     * Case4: Right is rvalue
     */
    str e("4567");
    str f = '3' + std::move(e);
    TS_ASSERT(is_container_valid(f));
    TS_ASSERT_EQUALS(f.size(), 5);
    TS_ASSERT_EQUALS(f.c_str(), "34567");
#endif
  }

  void test_relational_operators()
  {
    str a("0123");
    str b("4567");
    str c("0123");

    /*
     * Case1: == operator
     */
    TS_ASSERT(a == c);
    TS_ASSERT(a == "0123");
    TS_ASSERT("0123" == c);
    TS_ASSERT(!(a == b));
    TS_ASSERT(!("0123" == b));
    TS_ASSERT(!(a == "4567"));

    /*
     * Case2: != operator
     */
    TS_ASSERT(a != b);
    TS_ASSERT("0123" != b);
    TS_ASSERT(a != "4567");
    TS_ASSERT(!(a != c));
    TS_ASSERT(!(a != "0123"));
    TS_ASSERT(!("0123" != c));

    /*
     * Case3: < operator
     */
    TS_ASSERT(a < b);
    TS_ASSERT("0123" < b);
    TS_ASSERT(a < "4567");
    TS_ASSERT(!(b < a));
    TS_ASSERT(!(b < "0123"));
    TS_ASSERT(!("4567" < a));
    TS_ASSERT(!(c < a));
    TS_ASSERT(!(c < "0123"));
    TS_ASSERT(!("0123" < a));

    /*
     * Case4: <= operator
     */
    TS_ASSERT(a <= b);
    TS_ASSERT("0123" <= b);
    TS_ASSERT(a <= "4567");
    TS_ASSERT(!(b <= a));
    TS_ASSERT(!(b <= "0123"));
    TS_ASSERT(!("4567" <= a));
    TS_ASSERT(c <= a);
    TS_ASSERT(c <= "0123");
    TS_ASSERT("0123" <= a);

    /*
     * Case5: > operator
     */
    TS_ASSERT(b > a);
    TS_ASSERT(b > "0123");
    TS_ASSERT("4567" > a);
    TS_ASSERT(!(a > b));
    TS_ASSERT(!("0123" > b));
    TS_ASSERT(!(a > "4567"));
    TS_ASSERT(!(c > a));
    TS_ASSERT(!(c > "0123"));
    TS_ASSERT(!("0123" > a));

    /*
     * Case6: >= operator
     */
    TS_ASSERT(b >= a);
    TS_ASSERT(b >= "0123");
    TS_ASSERT("4567" >= a);
    TS_ASSERT(!(a >= b));
    TS_ASSERT(!("0123" >= b));
    TS_ASSERT(!(a >= "4567"));
    TS_ASSERT(c >= a);
    TS_ASSERT(c >= "0123");
    TS_ASSERT("0123" >= a);
  }

  /*
   * Non-standard member methods
   */
  void test_make_lower()
  {
    /*
     * Case1: Normal condition
     */
    str a("AbCdEfGh");
    a.make_lower();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.c_str(), "abcdefgh");
  }

  void test_make_upper()
  {
    /*
     * Case1: Normal condition
     */
    str a("AbCdEfGh");
    a.make_upper();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.c_str(), "ABCDEFGH");
  }

  void test_ltrim()
  {
    /*
     * Case1: Normal condition
     */
    str a("  2345  ");
    a.ltrim();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.c_str(), "2345  ");
  }

  void test_rtrim()
  {
    /*
     * Case1: Normal condition
     */
    str a("  2345  ");
    a.rtrim();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.c_str(), "  2345");
  }

  void test_trim()
  {
    /*
     * Case1: Normal condition
     */
    str a("  2345  ");
    a.trim();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.c_str(), "2345");
  }

}
;
