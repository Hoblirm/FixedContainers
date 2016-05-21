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
        printf("Error: Expected (!(hash[%d].move_only && hash[%d].was_copied)\n", it->first, it->first);
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

  void test_begin_and_end(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    hash_map::iterator it = a.begin();
    TS_ASSERT_EQUALS((it++)->second, 10);
    TS_ASSERT_EQUALS((it++)->second, 11);
    TS_ASSERT_EQUALS((it++)->second, 12);
    TS_ASSERT_EQUALS(it, a.end());
  }

  void test_begin_and_end_const(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    hash_map::const_iterator it = a.begin();
    TS_ASSERT_EQUALS((it++)->second, 10);
    TS_ASSERT_EQUALS((it++)->second, 11);
    TS_ASSERT_EQUALS((it++)->second, 12);
    TS_ASSERT_EQUALS(it, a.end());
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
    hash_map a( { { 0, 10 }, { 1, 11 }, { 2, 12 } });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 3);
  }

  void test_count(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });
    TS_ASSERT_EQUALS(a.count(-1), 0);
    TS_ASSERT_EQUALS(a.count(0), 1);
    TS_ASSERT_EQUALS(a.count(1), 1);
    TS_ASSERT_EQUALS(a.count(3), 1);
    TS_ASSERT_EQUALS(a.count(4), 0);
  }

  void test_emplace(void)
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Successful inserts
     */
    hash_map a;
    std::pair<int, obj> pairs[4] =
    {
      { 0, 10},
      { 1, 11},
      { 3, 13},
      { 2, 12}};
    std::pair<hash_map::iterator, bool> rt;
    for (int i = 0; i < 4; ++i)
    {
      clear_copy_flags(a);
      rt = a.emplace(pairs[i].first,std::move(pairs[i].second));
      mark_move_only(a);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT(rt.second);
      TS_ASSERT_EQUALS(rt.first, a.find(pairs[i].first));
      TS_ASSERT_EQUALS(a.size(), i + 1);
      TS_ASSERT_EQUALS(a[pairs[i].first], pairs[i].second);
    }

    /*
     * Case2: Insert failure
     */
    rt = a.emplace(0, 100);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(!rt.second);
    TS_ASSERT_EQUALS(rt.first, a.find(0));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[0], 10);
#endif
  }

  void test_emplace_hint(void)
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Successful inserts
     */
    hash_map a;
    std::pair<int, obj> pairs[4] =
    {
      { 0, 10},
      { 1, 11},
      { 3, 13},
      { 2, 12}};
    hash_map::iterator it;
    for (int i = 0; i < 4; ++i)
    {
      clear_copy_flags(a);
      it = a.emplace_hint(a.begin(),pairs[i].first,std::move(pairs[i].second));
      mark_move_only(a);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(it, a.find(pairs[i].first));
      TS_ASSERT_EQUALS(a.size(), i + 1);
      TS_ASSERT_EQUALS(a[pairs[i].first], pairs[i].second);
    }

    /*
     * Case2: Insert failure
     */
    it = a.emplace_hint(a.begin(), 0, 100);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(it, a.find(0));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[0], 10);
#endif
  }

  void test_empty(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });
    TS_ASSERT(!a.empty());
    a.clear();
    TS_ASSERT(a.empty());
  }

  void test_equal_range(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });

    std::pair<hash_map::iterator, hash_map::iterator> p;
    p = a.equal_range(3);
    TS_ASSERT_EQUALS(p.first, a.find(3));
    TS_ASSERT_EQUALS(p.second, ++p.first);
  }

  void test_equal_range_const(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });

    std::pair<hash_map::const_iterator, hash_map::const_iterator> p;
    p = a.equal_range(3);
    TS_ASSERT_EQUALS(p.first, a.find(3));
    TS_ASSERT_EQUALS(p.second, ++p.first);
  }

  void test_erase_position(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });
    hash_map::iterator it = a.find(3);
    hash_map::iterator first = it++;
    TS_ASSERT_EQUALS(it, a.erase(first));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a[0], 10);
    TS_ASSERT_EQUALS(a[1], 11);
  }

  void test_erase_key(void)
  {
    /*
     * Case 1: Key contained in map
     */
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });
    TS_ASSERT_EQUALS(1, a.erase(3));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a[0], 10);
    TS_ASSERT_EQUALS(a[1], 11);

    /*
     * Case2: Key not contained in map
     */
    TS_ASSERT_EQUALS(0, a.erase(2));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a[0], 10);
    TS_ASSERT_EQUALS(a[1], 11);
  }

  void test_erase_range(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });
    hash_map::iterator last = a.find(0);
    hash_map::iterator first = last++;
    ++last;
    TS_ASSERT_EQUALS(last, a.erase(first, last));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 1);
    TS_ASSERT_EQUALS(a[3], 13);
  }

  void test_find(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });

    /*
     * Case1: Key contained in map
     */
    hash_map::iterator it = a.find(3);
    TS_ASSERT_EQUALS(it->second, 13);

    /*
     * Case2: Key not contained in map
     */
    it = a.find(2);
    TS_ASSERT_EQUALS(it, a.end());
  }

  void test_find_const(void)
  {
    hash_map a( { { 0, 10 }, { 1, 11 }, { 3, 13 } });

    /*
     * Case1: Key contained in map
     */
    hash_map::const_iterator it = a.find(3);
    TS_ASSERT_EQUALS(it->second, 13);

    /*
     * Case2: Key not contained in map
     */
    it = a.find(2);
    TS_ASSERT_EQUALS(it, a.end());
  }

  void test_get_allocator(void)
  {
    hash_map a;
    hash_map::allocator_type alloc = a.get_allocator();
  }

  void test_hash_function(void)
  {
    hash_map a;
    hash_map::hasher h = a.hash_function();
  }

  void test_insert(void)
  {
    /*
     * Case1: Successful inserts
     */
    hash_map a;
    std::pair<int, obj> pairs[4] = { { 0, 10 }, { 1, 11 }, { 3, 13 }, { 2, 12 } };
    std::pair<hash_map::iterator, bool> rt;
    for (int i = 0; i < 4; ++i)
    {
      rt = a.insert(pairs[i]);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT(rt.second);
      TS_ASSERT_EQUALS(rt.first, a.find(pairs[i].first));
      TS_ASSERT_EQUALS(a.size(), i + 1);
      TS_ASSERT_EQUALS(a[pairs[i].first], pairs[i].second);
    }

    /*
     * Case2: Insert failure
     */
    rt = a.insert(std::pair<int, obj>(0, 100));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(!rt.second);
    TS_ASSERT_EQUALS(rt.first, a.find(0));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[0], 10);
  }

  void test_insert_move(void)
  {
#if FLEX_HAS_CXX11
    /*
     * Case1: Successful inserts
     */
    hash_map a;
    std::pair<int, obj> pairs[4] =
    {
      { 0, 10},
      { 1, 11},
      { 3, 13},
      { 2, 12}};
    std::pair<hash_map::iterator, bool> rt;
    for (int i = 0; i < 4; ++i)
    {
      clear_copy_flags(a);
      rt = a.insert(std::move(pairs[i]));
      mark_move_only(a);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT(rt.second);
      TS_ASSERT_EQUALS(rt.first, a.find(pairs[i].first));
      TS_ASSERT_EQUALS(a.size(), i + 1);
      TS_ASSERT_EQUALS(a[pairs[i].first], pairs[i].second);
    }

    /*
     * Case2: Insert failure
     */
    rt = a.insert(std::pair<int, obj>(0, 100));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(!rt.second);
    TS_ASSERT_EQUALS(rt.first, a.find(0));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[0], 10);
#endif
  }

  void test_insert_hint(void)
  {
    /*
     * Case1: Successful inserts
     */
    hash_map a;
    std::pair<int, obj> pairs[4] = { { 0, 10 }, { 1, 11 }, { 3, 13 }, { 2, 12 } };
    hash_map::iterator it;
    for (int i = 0; i < 4; ++i)
    {
      it = a.insert(a.begin(), pairs[i]);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(it, a.find(pairs[i].first));
      TS_ASSERT_EQUALS(a.size(), i + 1);
      TS_ASSERT_EQUALS(a[pairs[i].first], pairs[i].second);
    }

    /*
     * Case2: Insert failure
     */
    it = a.insert(a.begin(), std::pair<int, obj>(0, 100));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(it, a.find(0));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[0], 10);
  }

  void test_insert_hint_move(void)
  {
#if FLEX_HAS_CXX11
    /*
     * Case1: Successful inserts
     */
    hash_map a;
    std::pair<int, obj> pairs[4] =
    {
      { 0, 10},
      { 1, 11},
      { 3, 13},
      { 2, 12}};
    hash_map::iterator it;
    for (int i = 0; i < 4; ++i)
    {
      clear_copy_flags(a);
      it = a.insert(a.begin(),std::move(pairs[i]));
      mark_move_only(a);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(it, a.find(pairs[i].first));
      TS_ASSERT_EQUALS(a.size(), i + 1);
      TS_ASSERT_EQUALS(a[pairs[i].first], pairs[i].second);
    }

    /*
     * Case2: Insert failure
     */
    it = a.insert(a.begin(), std::pair<int, obj>(0, 100));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(it, a.find(0));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[0], 10);
#endif
  }

  void test_insert_range(void)
  {
    std::pair<int, obj> pairs[5] = { { 0, 10 }, { 1, 11 }, { 3, 13 }, { 2, 12 }, { 5, 15 } };
    hash_map a;

    /*
     * Case1: Insert into empty map
     */
    a.insert(pairs, pairs + 3);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 3);
    TS_ASSERT_EQUALS(a[0], 10);
    TS_ASSERT_EQUALS(a[1], 11);
    TS_ASSERT_EQUALS(a[3], 13);

    /*
     * Case2: Insert into map with available capacity
     */
    a.insert(pairs + 3, pairs + 4);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[2], 12);

    /*
     * Case3: Insert into map at capacity.
     */
    a.insert(pairs + 4, pairs + 5);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 5);
    TS_ASSERT_EQUALS(a[5], 15);
  }

  void test_insert_initializer_list(void)
  {
    hash_map a;

    /*
     * Case1: Insert into empty map
     */
    a.insert( { { 0, 10 }, { 1, 11 }, { 3, 13 } });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 3);
    TS_ASSERT_EQUALS(a[0], 10);
    TS_ASSERT_EQUALS(a[1], 11);
    TS_ASSERT_EQUALS(a[3], 13);

    /*
     * Case2: Insert into map with available capacity
     */
    a.insert( { 2, 12 });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[2], 12);

    /*
     * Case3: Insert into map at capacity.
     */
    a.insert( { 5, 15 });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 5);
    TS_ASSERT_EQUALS(a[5], 15);
  }

  void test_key_eq(void)
  {
    hash_map a;
    hash_map::key_equal key_eq = a.key_eq();
  }

  void test_load_factor(void)
  {
    /*
     * Case1: Empty map
     */
    hash_map a;
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.bucket_count(), 1);
    TS_ASSERT_EQUALS(a.load_factor(), 0.0f);
    TS_ASSERT_EQUALS(a.get_max_load_factor(), 1.0f);

    /*
     * Case2: Populated map
     */
    a.insert( { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 } });
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.bucket_count(), 5);
    TS_ASSERT_EQUALS(a.load_factor(), 0.8f);
    TS_ASSERT_EQUALS(a.get_max_load_factor(), 1.0f);

    /*
     * Case3: Modified max_load_factor
     */
    a.set_max_load_factor(0.5f);
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a.bucket_count(), 11);
    TS_ASSERT_EQUALS(a.load_factor(), 4.0f / 11.0f);
    TS_ASSERT_EQUALS(a.get_max_load_factor(), 0.5f);
  }

  void test_operator_dereference(void)
  {
    /*
     * Case1: Insert multiple values.
     */
    hash_map a;
    std::pair<int, obj> pairs[4] = { { 0, 10 }, { 1, 11 }, { 3, 13 }, { 2, 12 } };
    obj o;
    for (int i = 0; i < 4; ++i)
    {
      o = a[pairs[i].first];
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), i + 1);
      TS_ASSERT_EQUALS(o.val, obj::DEFAULT_VAL);
      a[pairs[i].first] = pairs[i].second;
      TS_ASSERT_EQUALS(a[pairs[i].first], pairs[i].second);
    }

  }
}
;
