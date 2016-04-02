#include <cxxtest/TestSuite.h>

#include <flex/string.h>
#include <flex/allocator_debug.h>

#include <string.h>
#include <string>

namespace eastl
{
  EASTL_API EmptyString gEmptyString = { 0 };
}

//typedef std::basic_string<char, std::char_traits<char>, flex::allocator_debug<char> > str;
typedef eastl::basic_string<char, flex::allocator_debug<char> > str;

class string_test: public CxxTest::TestSuite
{
public:

  void setUp()
  {
    flex::allocator_debug<char>::clear();
  }

  void tearDown()
  {
    //This ensures that all objs constructed by the container have their destructors called.
    TS_ASSERT(flex::allocator_debug<char>::mConstructedPointers.empty());

    //This ensures that all memory allocated by the container is properly freed.
    TS_ASSERT(flex::allocator_debug<char>::mAllocatedPointers.empty());
  }

  bool is_container_valid(const str& c)
  {
    if (c.size() != strlen(c.c_str()))
    {
      printf("Error: Expected (c.size() == strlen(c.c_str())), found (%zu != %zu)\n", c.size(), strlen(c.c_str()));
      return false;
    }

    //TODO: Consider adding string validate method in here.
    return true;
  }

  void test_default_constructor()
  {
    /*
     * Case1: Normal conditions
     */
    flex::allocation_guard::enable();
    str a;
    flex::allocation_guard::disable();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
  }

  void test_from_c_string_constructor()
  {
    /*
     * Case1: Empty c string
     */
    flex::allocation_guard::enable();
    str a("");
    flex::allocation_guard::disable();
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);

    /*
     * Case2: Populated c string
     */
    str b("01234567");
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 8);

    /*
     * Case3: Allocation exception
     */
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(str("01234567"), std::runtime_error);
    flex::allocation_guard::disable();
  }
};
