#include <cxxtest/TestSuite.h>

#include "flex/hash_map.h"
#include "flex/debug/allocator.h"
#include "flex/debug/obj.h"

#include <climits>

using namespace flex;

class hash_map_test: public CxxTest::TestSuite
{

  typedef flex::debug::obj obj;
  typedef flex::hash_map<int, obj, std::hash<int>, std::equal_to<int>, flex::debug::allocator<char> > hash_map;

public:

  void setUp()
  {
    flex::debug::allocator<char>::clear();
    errno = 0;
  }

  void tearDown()
  {
    TS_ASSERT(!errno);

    //This ensures that all objs constructed by the container have their destructors called.
    TS_ASSERT(flex::debug::allocator<char>::mConstructedPointers.empty());

    //This ensures that all memory allocated by the container is properly freed.
    TS_ASSERT(flex::debug::allocator<char>::mAllocatedPointers.empty());
  }

  void mark_move_only(hash_map& c)
  {
#ifdef FLEX_HAS_CXX11
    for (hash_map::iterator it = c.begin(); it != c.end(); ++it)
    {
      it->second.move_only = true;
    }
#endif
  }

  void clear_copy_flags(hash_map& c)
  {
    for (hash_map::iterator it = c.begin(); it != c.end(); ++it)
    {
      it->second.was_copied = false;
    }
  }

  bool is_container_valid(hash_map& c)
  {
    size_t size = 0;
    int max_key = INT_MIN;
    for (hash_map::iterator it = c.begin(); it != c.end(); ++it)
    {
      if (it->second.init != obj::INIT_KEY)
      {
        printf("Error: Expected (hash[%d] == obj::INIT_KEY), found (%d != %d)\n", it->first, it->second.init,
            obj::INIT_KEY);
        return false;
      }
      if (it->second.move_only && it->second.was_copied)
      {
        printf("Error: Expected (!(hash[%d].move_only && hash[%d].was_copied)", it->first, it->first);
        return false;
      }
      if (it->first > max_key)
      {
        max_key = it->first;
      }
      ++size;
    }

    if (size != c.size())
    {
      printf("Error: Expected (size == hash.size()), found (%zu != %zu)\n", size, c.size());
      return false;
    }

    if (false)
    {
      //The node pool is private, so we implement a hack here to get the node pool pointer.  We get the
      //iterator of a newly added node, and then remove it.  Doing this should have the iterator pointing
      //to the top of the node pool.
      int unique_key = ++max_key;
      c.insert(std::pair<int, obj>(unique_key, 1));
      hash_map::iterator pool_it = c.find(unique_key);
      c.erase(unique_key);

      //Now we can use the iterator to loop through the pool.  Ensure all pool entries are uninitialized.
//      while (pool_it.mNode != NULL)
//      {
//        if (pool_it->init == obj::INIT_KEY)
//        {
//          printf("Error: Expected (pool_it->mValue != object::INIT_KEY), found (%d == %d)\n", pool_it->init,
//              obj::INIT_KEY);
//          return false;
//        }
//        ++pool_it;
//      }

    }
    return true;
  }

  void test_constructor_empty(void)
  {
    /*
     * Case1: Default Container
     */
    hash_map a;
    TS_ASSERT(is_container_valid(a));
  }

  void test_constructor_initializer_list(void)
  {
    /*
     * Case1: Default Case
     */
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 3);

    TS_ASSERT_EQUALS(a[0], 10);
    TS_ASSERT_EQUALS(a[1], 11);
    TS_ASSERT_EQUALS(a[2], 12);
  }

  void test_constructor_range(void)
  {
    /*
     * Case1: Default Case
     */
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    hash_map b(a.begin(), a.end());
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 3);

    TS_ASSERT_EQUALS(b[0], 10);
    TS_ASSERT_EQUALS(b[1], 11);
    TS_ASSERT_EQUALS(b[2], 12);
  }

  void test_constructor_copy(void)
  {
    /*
     * Case1: Default Case
     */
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    hash_map b(a);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 3);

    TS_ASSERT_EQUALS(b[0], 10);
    TS_ASSERT_EQUALS(b[1], 11);
    TS_ASSERT_EQUALS(b[2], 12);
  }

  void test_constructor_move(void)
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Default Case
     */
    hash_map a(
        {
          { 0, 10},
          { 1, 11},
          { 2, 12}});
    clear_copy_flags(a);
    hash_map b(std::move(a));
    mark_move_only(b);
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(b.size(), 3);

    TS_ASSERT_EQUALS(b[0], 10);
    TS_ASSERT_EQUALS(b[1], 11);
    TS_ASSERT_EQUALS(b[2], 12);
#endif
  }

  void test_at(void)
  {
    /*
     * Currently not supported.
     */
//    /*
//     * Case1: Default Case
//     */
//    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
//    TS_ASSERT_EQUALS(a.at(0), 10);
//    TS_ASSERT_EQUALS(a.at(1), 11);
//    TS_ASSERT_EQUALS(a.at(2), 12);
//
//    a.at(0) = 20;
//    a.at(1) = 21;
//    a.at(2) = 22;
//    for (int i = 0; i < 3; ++i)
//    {
//      const obj val = a.at(i);
//      TS_ASSERT_EQUALS(val, 20 + i);
//    }
  }

  void test_begin(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    hash_map::iterator it = a.begin();
    TS_ASSERT_EQUALS((it++)->second, 10);
    TS_ASSERT_EQUALS((it++)->second, 11);
    TS_ASSERT_EQUALS((it++)->second, 12);
    TS_ASSERT_EQUALS(it, a.end());
  }

  void test_begin_const(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    hash_map::const_iterator it = a.begin();
    TS_ASSERT_EQUALS((it++)->second, 10);
    TS_ASSERT_EQUALS((it++)->second, 11);
    TS_ASSERT_EQUALS((it++)->second, 12);
    TS_ASSERT_EQUALS(it, a.cend());
  }

  void test_bucket(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    TS_ASSERT_EQUALS(a.bucket(0), 0);
    TS_ASSERT_EQUALS(a.bucket(1), 1);
    TS_ASSERT_EQUALS(a.bucket(2), 2);
  }

  void test_bucket_count(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    TS_ASSERT_EQUALS(a.bucket_count(), 3);
  }

  void test_bucket_size(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 12 } });
    TS_ASSERT_EQUALS(a.bucket_size(0), 2);
    TS_ASSERT_EQUALS(a.bucket_size(1), 1);
    TS_ASSERT_EQUALS(a.bucket_size(2), 0);
  }

  void test_cbegin(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    hash_map::const_iterator it = a.cbegin();
    TS_ASSERT_EQUALS((it++)->second, 10);
    TS_ASSERT_EQUALS((it++)->second, 11);
    TS_ASSERT_EQUALS((it++)->second, 12);
    TS_ASSERT_EQUALS(it, a.cend());
  }

  void test_clear(void)
  {

  }

}
;
