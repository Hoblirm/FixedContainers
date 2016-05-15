#include <cxxtest/TestSuite.h>

#include <flex/hash_map.h>

using namespace flex;

class hash_map_test: public CxxTest::TestSuite
{

public:

  void setUp()
  {

  }

  void tearDown()
  {
  }

  void test_default_constructor(void)
  {
    flex::hash_map<int,int> a;
  }

}
;
