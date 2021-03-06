#include <cxxtest/TestSuite.h>

#include "flex/list.h"
#include "flex/debug/allocator.h"
#include "flex/debug/obj.h"

class list_test: public CxxTest::TestSuite
{

  typedef flex::debug::obj obj;
  typedef flex::list<obj, flex::debug::allocator<flex::list<obj>::node_type> > list_obj;

  const obj OBJ_DATA[128] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 39535304, 2113617954, -262399995,
      -1776526244, 2007130159, -751355444, -1850306681, 1670328314, 174975647, 1520325186, 752193990, 1141698902,
      414986917, -1084506988, -1274438196, -407784340, -1476797751, 952482371, 1659351065, -1840296979, 1174260466,
      -830555035, 1187249412, -1439716735, -606656096, 1968778085, -468774603, -741213671, -1792595459, -1043591241,
      -399781674, 1441797965, -539577554, -1712941906, 893437261, 1243708130, -276655685, 169167272, 1548266128,
      2134938409, -165983522, 65335344, 777222631, -1975346548, 1736737965, -1297235370, -1778585082, -445115751,
      77287795, -904742465, 1566979049, -1276550055, -1523151595, -1877472326, -1965521838, 309774311, 285638537,
      1694499811, 395062486, -599472639, -562348494, 622523556, 1991792880, 1485225099, -26143183, 1213635789,
      -1867261885, 1401932595, 1643956672, 1152265615, -206296253, -1341812088, -928119996, 1335888378, -2127839732,
      -805081880, -461979923, 258594093, 1322814281, -1856950276, 763906168, -110775798, 29138078, -728231554,
      -1738124420, -1130024844, 2112808498, -2147190929, -46681067, -1746560845, -1931350352, -2121713887, -2077836858,
      -68560373, 542144249, -964249373, 672765407, 1240222082, -170251308, 573136605, 522427348, -1842488270,
      -803442179, 1214800559, -439290856, -850489475, -371113959, -528653948, -1466750983, -299654597, -1095361209,
      912904732 };

  const size_t SIZE_COUNT = 10;
  const size_t SIZES[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 128 };

public:

  void setUp()
  {
    flex::debug::allocator<flex::list<obj>::node_type>::clear();
    errno = 0;
  }

  void tearDown()
  {
    TS_ASSERT(!errno);

    //This ensures that all objs constructed by the container have their destructors called.
    TS_ASSERT(flex::debug::allocator<flex::list<obj>::node_type>::mConstructedPointers.empty());

    //This ensures that all memory allocated by the container is properly freed.
    TS_ASSERT(flex::debug::allocator<flex::list<obj>::node_type>::mAllocatedPointers.empty());
  }

  void mark_move_only(list_obj& c)
  {
#ifdef FLEX_HAS_CXX11
    for (list_obj::iterator it = c.begin(); it != c.end(); ++it)
    {
      it->move_only = true;
    }
#endif
  }

  void clear_copy_flags(list_obj& c)
  {
    for (list_obj::iterator it = c.begin(); it != c.end(); ++it)
    {
      it->was_copied = false;
    }
  }

  bool is_container_valid(list_obj& list)
  {
    //This checks to ensure the list is valid.  This checks three main attributes for validity.
    //1. Ensures prev & next pointers on all nodes agree with each other.
    //2. Ensures that nodes are properly initialized and destructed.  Remember that unused list
    //   nodes are kept in a pool.  These unused nodes should be uninitialized.
    //3. Ensures the cached size variable is in sync with the number of list nodes.

    //We will loop through all the list nodes ensuring that the previous iterator matches that
    //of the current node's prev_pointer.  All nodes in the list should be initialized.
    typename list_obj::const_iterator prev = --list.begin();
    typename list_obj::const_iterator it = list.begin();
    size_t n = 0;
    while (it != list.end())
    {
      if (prev.mNode != it.mNode->mPrev)
      {
        printf("Error: Expected (prev.mNode == it.mNode->mPrev) when n=%zu, found (%p != %p)\n", n, prev.mNode,
            it.mNode->mPrev);
        return false;
      }

      if (it->init != obj::INIT_KEY)
      {
        printf("Error: Expected (it->init == obj::INIT_KEY), found (%d == %d)\n", it->init, obj::INIT_KEY);
        return false;
      }

      if (it->move_only && it->was_copied)
      {
        printf("Error: Expected (!(it->move_only && it->was_copied)) when n=%zu", n);
        return false;
      }

      ++it;
      ++prev;
      ++n;
    }
    if (prev.mNode != it.mNode->mPrev)
    {
      printf("Error: Expected (prev.mNode == it.mNode->mPrev) when n=%zu, found (%p != %p)\n", prev.mNode,
          it.mNode->mPrev);
      return false;
    }

    //If the list is not full, we will check the node pool to ensure all unused nodes are uninitialized.
    if (!list.full())
    {
      //The node pool is private, so we implement a hack here to get the node pool pointer.  We get the
      //iterator of a newly added node, and then remove it.  Doing this should have the iterator pointing
      //to the top of the node pool.
      list.push_front(0);
      list_obj::iterator pool_it = list.begin();
      list.pop_front();

      //Now we can use the iterator to loop through the pool.  Ensure all pool entries are uninitialized.
      while (pool_it.mNode != NULL)
      {
        if (pool_it->init == obj::INIT_KEY)
        {
          printf("Error: Expected (pool_it->mValue != object::INIT_KEY), found (%d == %d)\n", pool_it->init,
              obj::INIT_KEY);
          return false;
        }
        ++pool_it;
      }
    }

    if (n != list.size())
    {
      printf("Error: Expected (n == list.size()), found (%zu != %zu)\n", n, list.size());
      return false;
    }

    return true;
  }

  void test_default_constructor(void)
  {
    /*
     * Case1: Verify default constructor works and doesn't allocate space.
     */
    flex::allocation_guard::enable();
    list_obj a;
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 0);
    flex::allocation_guard::disable();
  }

  void test_default_fill_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Verify fill constructor allocates memory.
       */
      if (SIZES[s] > 0)
      {
        flex::allocation_guard::enable();
        list_obj a(SIZES[s]);
        TS_ASSERT(errno);
        errno = 0;
        flex::allocation_guard::disable();
      }

      /*
       * Case2: Verify fill constructor with primitive elements.
       */
      list_obj a(SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, obj::DEFAULT_VAL);
      }
      TS_ASSERT(is_container_valid(a));

    } //for: SIZE_COUNT
  }

  void test_fill_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const obj fill_val = OBJ_DATA[SIZES[s] - 1];

      /*
       * Case1: Verify fill constructor assigns value parameter for primitives.
       */
      list_obj a(SIZES[s], fill_val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, fill_val);
      }
      TS_ASSERT(is_container_valid(a));

    } //for: SIZE_COUNT
  }

  void test_range_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Verify range constructor with pointer parameters
       */
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      int i = 0;
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++i;
      }
      TS_ASSERT(is_container_valid(a));

      /*
       * Case2: Verify range constructor with iterator parameters
       */
      list_obj b(a.begin(), a.end());
      TS_ASSERT_EQUALS(b.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      i = 0;
      for (list_obj::iterator it = b.begin(); it != b.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++i;
      }
      TS_ASSERT(is_container_valid(b));

    } //for: SIZE_COUNT
  }

  void test_copy_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Verify copy constructor allocates memory
       */
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      if (a.size() > 0)
      {
        flex::allocation_guard::enable();
        list_obj b(a);
        TS_ASSERT(errno);
        errno = 0;
        flex::allocation_guard::disable();
      }
      TS_ASSERT(is_container_valid(a));

      /*
       * Case2: Verify copy constructor with primitive elements.
       */
      list_obj b(a);
      TS_ASSERT_EQUALS(b.size(), a.size());
      TS_ASSERT_EQUALS(b.size(), b.capacity());
      list_obj::iterator ait = a.begin();
      for (list_obj::iterator it = b.begin(); it != b.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, *ait);
        ++ait;
      }
      TS_ASSERT(is_container_valid(b));

    } //for: SIZE_COUNT
  }

  void test_move_constructor()
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Normal condition.
     */
    list_obj a =
    { 0, 1, 2, 3};
    clear_copy_flags(a);
    list_obj b(std::move(a));
    mark_move_only(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(a.size(),0);
    TS_ASSERT_EQUALS(b.size(),4);
    list_obj::iterator it = b.begin();
    TS_ASSERT_EQUALS(*it++, 0);
    TS_ASSERT_EQUALS(*it++, 1);
    TS_ASSERT_EQUALS(*it++, 2);
    TS_ASSERT_EQUALS(*it++, 3);

    /*
     * Case1: Fixed parameter.
     */
    flex::fixed_list<obj,16,list_obj::allocator_type> c =
    { 0, 1, 2, 3};
    clear_copy_flags(c);
    list_obj d(std::move(c));
    mark_move_only(d);
    TS_ASSERT(is_container_valid(c));
    TS_ASSERT(is_container_valid(d));
    TS_ASSERT_EQUALS(c.size(),0);
    TS_ASSERT_EQUALS(d.size(),4);
    it = d.begin();
    TS_ASSERT_EQUALS(*it++, 0);
    TS_ASSERT_EQUALS(*it++, 1);
    TS_ASSERT_EQUALS(*it++, 2);
    TS_ASSERT_EQUALS(*it++, 3);
#endif
  }

  void test_initializer_constructor()
  {
    /*
     * Case1: Normal condition
     */
    list_obj a( { 0, 1, 2, 3 });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    list_obj::iterator it = a.begin();
    TS_ASSERT_EQUALS(*it++, 0);
    TS_ASSERT_EQUALS(*it++, 1);
    TS_ASSERT_EQUALS(*it++, 2);
    TS_ASSERT_EQUALS(*it++, 3);
  }

  void test_assign_fill(void)
  {
    /*
     * Case1: Verify assign can increase size.
     */
    list_obj a;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const obj fill_val = OBJ_DATA[SIZES[s] - 1];
      a.assign(SIZES[s], fill_val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, fill_val);
      }
      TS_ASSERT(is_container_valid(a));
    }

    /*
     * Case2: Verify assign can decrease size.
     */
    for (int s = SIZE_COUNT - 1; s != -1; --s)
    {
      const obj fill_val = OBJ_DATA[SIZES[s] - 1];
      a.assign(SIZES[s], fill_val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, fill_val);
      }
      TS_ASSERT(is_container_valid(a));
    }
  }

  void test_assign_iterator()
  {
    list_obj a;

    /*
     * Case1: Verify assign can increase size.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_obj tmp(OBJ_DATA, OBJ_DATA + SIZES[s]);
      a.assign(tmp.begin(), tmp.end());
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      list_obj::iterator tmp_it = tmp.begin();
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, *tmp_it);
        ++tmp_it;
      }
      TS_ASSERT(is_container_valid(a));
    }

    /*
     * Case2: Verify assign can decrease size.
     */
    for (int s = SIZE_COUNT - 1; s != -1; --s)
    {
      list_obj tmp(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t prev_capacity = a.capacity();
      a.assign(tmp.begin(), tmp.end());
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      list_obj::iterator tmp_it = tmp.begin();
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, *tmp_it);
        ++tmp_it;
      }
      TS_ASSERT(is_container_valid(a));
    }
  }

  void test_assign_initializer()
  {
    /*
     * Case1: Normal condition
     */
    list_obj a;
    a.assign( { 0, 1, 2, 3 });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    list_obj::iterator it = a.begin();
    TS_ASSERT_EQUALS(*it++, 0);
    TS_ASSERT_EQUALS(*it++, 1);
    TS_ASSERT_EQUALS(*it++, 2);
    TS_ASSERT_EQUALS(*it++, 3);
  }

  void test_back(void)
  {
    //Start s at 1, as back() isn't supported on empty container.
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.back(), OBJ_DATA[SIZES[s] - 1]);
    } //for: SIZE_COUNT
  }

  void test_back_const(void)
  {
    //Start s at 1, as back() isn't supported on empty container.
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      const list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.back(), OBJ_DATA[SIZES[s] - 1]);
    } //for: SIZE_COUNT
  }

  void test_begin_and_end(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        *it = 0; //Ensure it is not const.
        ++i;
      }
      TS_ASSERT_EQUALS(i, a.size());
    } //for: SIZE_COUNT
  }

  void test_begin_and_end_const(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (list_obj::const_iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++i;
      }
      TS_ASSERT_EQUALS(i, a.size());
    } //for: SIZE_COUNT
  }

  void test_cbegin_and_cend(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (list_obj::const_iterator it = a.cbegin(); it != a.cend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++i;
      }
      TS_ASSERT_EQUALS(i, a.size());
    } //for: SIZE_COUNT
  }

  void test_crbegin_and_crend(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (list_obj::const_reverse_iterator it = a.crbegin(); it != a.crend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + a.size() - 1]);
        --i;
      }
      TS_ASSERT_EQUALS(i, -a.size());
    } //for: SIZE_COUNT
  }

  void test_capacity(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_obj a(SIZES[s]);
      TS_ASSERT_EQUALS(a.capacity(), SIZES[s]);
      a.clear();
      TS_ASSERT_EQUALS(a.capacity(), SIZES[s]);
    } //for: SIZE_COUNT
  }

  void test_clear(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      a.clear();
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT(a.empty());
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_emplace(void)
  {
#ifdef FLEX_HAS_CXX11
    list_obj a;
    list_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const int val = 19;

      /*
       * Case1: Test insert at end
       */
      it = a.emplace(a.end(), val, true);
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);

      /*
       * Case2: Test insert at begin
       */
      it = a.emplace(a.begin(), val, true);
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a.front(), val);
      it = ++a.begin();
      for (int i = 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      it = a.emplace(it, val,true);
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a.front(), val);
      it = ++a.begin();
      for (int i = 1; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      ++it;
      for (int i = mid_index + 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 2]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
#endif
  }

  void test_emplace_back(void)
  {
#ifdef FLEX_HAS_CXX11
    list_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        a.emplace_back(OBJ_DATA[i].val,true);
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[i]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
      TS_ASSERT(is_container_valid(a));
    }
#endif
  }

  void test_emplace_front(void)
  {
#ifdef FLEX_HAS_CXX11
    list_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        const unsigned data_index = SIZES[s] - 1 - i;
        a.emplace_front(OBJ_DATA[data_index].val,true);
        TS_ASSERT_EQUALS(a.front(), OBJ_DATA[data_index]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
      TS_ASSERT(is_container_valid(a));
    }
#endif
  }

  void test_empty(void)
  {
    /*
     * Case 1: Verify empty on init.
     */
    list_obj a;
    TS_ASSERT_EQUALS(a.empty(), true);

    /*
     * Case 2: Verify empty on modify
     */
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      a.assign(SIZES[s], 0);
      TS_ASSERT_EQUALS(a.empty(), false);
      a.assign((int*) NULL, (int*) NULL);
      TS_ASSERT_EQUALS(a.empty(), true);
    } //for: SIZE_COUNT
  }

  void test_erase_position()
  {
    /*
     * Case 1: Test erase on size of 1.
     */
    list_obj a(1);
    list_obj::iterator it;
    list_obj::iterator erase_it;
    it = a.erase(a.begin());
    TS_ASSERT_EQUALS(it, a.begin());
    TS_ASSERT_EQUALS(a.size(), 0);

    //Start s at 3, as the below tests expect a size of at least 3.
    for (unsigned s = 3; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();

      /*
       * Case2: Test erase at end
       */
      it = a.erase(--a.end());
      --current_size;
      TS_ASSERT_EQUALS(it, a.end());
      TS_ASSERT_EQUALS(a.size(), current_size);
      int i = 0;
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++i;
      }

      /*
       * Case3: Test erase at begin
       */
      it = a.erase(a.begin());
      --current_size;
      TS_ASSERT_EQUALS(it, a.begin());
      TS_ASSERT_EQUALS(a.size(), current_size);
      i = 0;
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + 1]);
        ++i;
      }

      /*
       * Case4: Test erase in middle
       */
      int mid_index = current_size / 2;
      erase_it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++erase_it;
      }
      it = a.erase(erase_it);
      erase_it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++erase_it;
      }
      --current_size;
      TS_ASSERT_EQUALS(it, erase_it);
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + 1]);
        ++it;
      }
      for (int i = mid_index; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + 2]);
        ++it;
      }
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_erase_range()
  {
    /*
     * Case 1: Test erase on size of 1.
     */
    list_obj a(1);
    list_obj::iterator it;
    list_obj::iterator erase_it;
    it = a.erase(a.begin(), a.end());
    TS_ASSERT_EQUALS(it, a.end());
    TS_ASSERT_EQUALS(a.size(), 0);

    //Start s at 7, as the below tests expect a size of at least 7.
    for (unsigned s = 7; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();

      /*
       * Case2: Test erase at end
       */
      erase_it = a.end();
      for (int i = 0; i < 2; ++i)
      {
        --erase_it;
      }
      it = a.erase(erase_it, a.end());
      current_size -= 2;
      TS_ASSERT_EQUALS(it, a.end());
      TS_ASSERT_EQUALS(a.size(), current_size);
      int i = 0;
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++i;
      }

      /*
       * Case3: Test erase at begin
       */
      erase_it = a.begin();
      for (int i = 0; i < 2; ++i)
      {
        ++erase_it;
      }
      it = a.erase(a.begin(), erase_it);
      current_size -= 2;
      TS_ASSERT_EQUALS(it, a.begin());
      TS_ASSERT_EQUALS(a.size(), current_size);
      i = 0;
      for (list_obj::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + 2]);
        ++i;
      }

      /*
       * Case4: Test erase in middle
       */
      int mid_index = current_size / 2;
      erase_it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++erase_it;
      }
      list_obj::iterator tmp_it = erase_it;
      for (int i = 0; i < 2; ++i)
      {
        ++tmp_it;
      }
      it = a.erase(erase_it, tmp_it);
      current_size -= 2;
      erase_it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++erase_it;
      }
      TS_ASSERT_EQUALS(it, erase_it);
      TS_ASSERT_EQUALS(a.size(), current_size);
      list_obj::iterator it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + 2]);
        ++it;
      }
      for (int i = mid_index; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + 4]);
        ++it;
      }
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_front(void)
  {
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.front(), OBJ_DATA[0]);
    } //for: SIZE_COUNT
  }

  void test_front_const(void)
  {
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      const list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.front(), OBJ_DATA[0]);
    } //for: SIZE_COUNT
  }

  void test_full(void)
  {
    /*
     * Case1: Uninitialized container list
     */
    list_obj a;
    //full() should return true whenever size is equal to capacity; even if they are both zero.
    //This is necessary as it is used to determine if a reallocation will occur.
    TS_ASSERT(a.full());

    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      list_obj b(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT(b.full());
      b.pop_back();
      TS_ASSERT(!b.full());
      b.push_front(0);
      TS_ASSERT(b.full());
    } //for: SIZE_COUNT
  }

  void test_insert_position(void)
  {
    list_obj a;
    list_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const obj val = 19;

      /*
       * Case1: Test insert at end
       */
      it = a.insert(a.end(), val);
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);

      /*
       * Case2: Test insert at begin
       */
      it = a.insert(a.begin(), val);
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a.front(), val);
      it = ++a.begin();
      for (int i = 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      it = a.insert(it, val);
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a.front(), val);
      it = ++a.begin();
      for (int i = 1; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      ++it;
      for (int i = mid_index + 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 2]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_insert_move(void)
  {
#ifdef FLEX_HAS_CXX11
    list_obj a;
    list_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      clear_copy_flags(a);
      size_t current_size = a.size();
      obj val = 19;

      /*
       * Case1: Test insert at end
       */
      it = a.insert(a.end(), std::move(val));
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);

      /*
       * Case2: Test insert at begin
       */
      it = a.insert(a.begin(), std::move(val));
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a.front(), val);
      it = ++a.begin();
      for (int i = 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      it = a.insert(it, std::move(val));
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a.front(), val);
      it = ++a.begin();
      for (int i = 1; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      ++it;
      for (int i = mid_index + 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 2]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      mark_move_only(a);
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
#endif
  }

  void test_insert_fill(void)
  {
    list_obj a;
    list_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const obj val = 19;

      /*
       * Case1: Test insert at end
       */
      a.insert(a.end(), 2, val);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);

      /*
       * Case2: Test insert at begin
       */
      a.insert(a.begin(), 2, val);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);
      for (int i = 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      a.insert(it, 2, val);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);
      for (int i = 2; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_insert_pointers(void)
  {
    list_obj a;
    list_obj::iterator it;

    //Start s at 3, as the below tests expect a size of at least 3.
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();

      /*
       * Case1: Test insert at end
       */
      a.insert(a.end(), OBJ_DATA, OBJ_DATA + 2);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);

      /*
       * Case2: Test insert at begin
       */
      a.insert(a.begin(), OBJ_DATA, OBJ_DATA + 2);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      for (int i = 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      a.insert(it, OBJ_DATA, OBJ_DATA + 2);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      for (int i = 2; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_insert_iterators(void)
  {
    list_obj a;
    list_obj b(OBJ_DATA, OBJ_DATA + 2);
    list_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();

      /*
       * Case1: Test insert at end
       */
      a.insert(a.end(), b.begin(), b.end());
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);

      /*
       * Case2: Test insert at begin
       */
      a.insert(a.begin(), b.begin(), b.end());
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      for (int i = 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      a.insert(it, b.begin(), b.end());
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      for (int i = 2; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[1]);
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_max_size(void)
  {
    const list_obj a;
    TS_ASSERT_EQUALS(a.max_size(), a.get_allocator().max_size());
  }

  void test_merge(void)
  {
    int a_data[8] = { 0, 2, 4, 5, 8, 9, 11, 13 };
    int b_data[8] = { 1, 3, 6, 7, 10, 12, 14, 15 };
    list_obj a(a_data, a_data + 8);
    list_obj b(b_data, b_data + 8);

    a.merge(b);
    TS_ASSERT_EQUALS(a.size(), 16);
    TS_ASSERT_EQUALS(b.size(), 0);
    list_obj::iterator it = a.begin();
    for (int i = 0; i < a.size(); ++i)
    {
      TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
      ++it;
    }
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
  }

  struct reverse_merge_functor
  {
    bool operator()(int first, int second)
    {
      return (int(first) > int(second));
    }
  };

  void test_merge_compare(void)
  {
    int a_data[8] = { 13, 11, 9, 8, 5, 4, 3, 1 };
    int b_data[8] = { 15, 14, 12, 10, 7, 6, 2, 0 };
    list_obj a(a_data, a_data + 8);
    list_obj b(b_data, b_data + 8);

    a.merge(b, reverse_merge_functor());
    TS_ASSERT_EQUALS(a.size(), 16);
    TS_ASSERT_EQUALS(b.size(), 0);
    list_obj::iterator it = a.begin();
    for (int i = 15; i >= 0; --i)
    {
      TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
      ++it;
    }
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
  }

  void test_rbegin_and_rend(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (list_obj::reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + a.size() - 1]);
        *it = 0; //verify not const
        --i;
      }
      TS_ASSERT_EQUALS(i, -a.size());
    } //for: SIZE_COUNT
  }

  void test_rbegin_and_rend_const(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (list_obj::const_reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + a.size() - 1]);
        --i;
      }
      TS_ASSERT_EQUALS(i, -a.size());
    } //for: SIZE_COUNT
  }

  void test_remove(void)
  {
    int data[16] = { 0, 0, 2, 3, 11, 5, 6, 7, 11, 9, 10, 11, 12, 13, 14, 15 };
    list_obj a(data, data + 16);

    /*
     * Case1: Attempt to remove entry not in list.
     */
    a.remove(16);
    TS_ASSERT_EQUALS(a.size(), 16);

    /*
     * Case2: Remove entry that is at the end of list
     */
    a.remove(15);
    TS_ASSERT_EQUALS(a.size(), 15);

    /*
     * Case3: Remove entry that is at the front of list
     */
    a.remove(0);
    TS_ASSERT_EQUALS(a.size(), 13);

    /*
     * Case4: Remove entry that is duplicated
     */
    a.remove(11);
    TS_ASSERT_EQUALS(a.size(), 10);
    list_obj::iterator it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT_EQUALS(*(it++), 3);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 6);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 9);
    TS_ASSERT_EQUALS(*(it++), 10);
    TS_ASSERT_EQUALS(*(it++), 12);
    TS_ASSERT_EQUALS(*(it++), 13);
    TS_ASSERT_EQUALS(*(it++), 14);
    TS_ASSERT(is_container_valid(a));
  }

  struct is_zero_functor
  {
    bool operator()(obj val)
    {
      return (val == 0);
    }
  };

  struct is_negative_functor
  {
    bool operator()(obj val)
    {
      return (val < 0);
    }
  };

  struct is_over_thirty
  {
    bool operator()(obj val)
    {
      return (val > 30);
    }
  };

  void test_remove_predicate(void)
  {
    int data[16] = { 0, 0, 2, 3, -11, 5, 6, 7, -11, 9, 10, -11, 12, 13, 14, 35 };
    list_obj a(data, data + 16);

    /*
     * Case1: Remove entry that is at the end of list
     */
    a.remove_if(is_over_thirty());
    TS_ASSERT_EQUALS(a.size(), 15);

    /*
     * Case2: Remove entry that is at the front of list
     */
    a.remove_if(is_zero_functor());
    TS_ASSERT_EQUALS(a.size(), 13);

    /*
     * Case3: Remove entry that is duplicated
     */
    a.remove_if(is_negative_functor());
    TS_ASSERT_EQUALS(a.size(), 10);
    list_obj::iterator it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT_EQUALS(*(it++), 3);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 6);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 9);
    TS_ASSERT_EQUALS(*(it++), 10);
    TS_ASSERT_EQUALS(*(it++), 12);
    TS_ASSERT_EQUALS(*(it++), 13);
    TS_ASSERT_EQUALS(*(it++), 14);
    TS_ASSERT(is_container_valid(a));
  }

  void test_reserve(void)
  {
    list_obj a;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.reserve(SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT_LESS_THAN_EQUALS(SIZES[s], a.capacity());
    }
  }

  void test_resize(void)
  {
    list_obj a;
    const obj val = 19;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case 1: Verify resize can decrease size.
       */
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t new_size = SIZES[s] / 2;
      a.resize(new_size);
      TS_ASSERT_EQUALS(a.size(), new_size);
      TS_ASSERT(a == list_obj(OBJ_DATA, OBJ_DATA + new_size));
      TS_ASSERT(is_container_valid(a));

      /*
       * Case 2: Verify resize can increase size.
       */
      a.resize(SIZES[s], val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      list_obj::iterator it = a.begin();
      for (int i = 0; i < new_size; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i]);
      }
      for (int i = new_size; i < SIZES[s]; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), val);
      }
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_reverse(void)
  {
    list_obj a;
    list_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case 1: Normal conditions
       */
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      a.reverse();
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      it = a.begin();
      for (int i = SIZES[s] - 1; i >= 0; --i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_shrink_to_fit(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      a.shrink_to_fit();
      TS_ASSERT_EQUALS(a.size(), a.capacity());

      a.assign(OBJ_DATA, OBJ_DATA + (SIZES[s] / 2));
      a.shrink_to_fit();
      TS_ASSERT_EQUALS(a.size(), a.capacity());

      a.clear();
      a.shrink_to_fit();
      TS_ASSERT_EQUALS(a.size(), a.capacity());
    } //for: SIZE_COUNT
  }

  void test_size(void)
  {
    unsigned size = 3;
    const list_obj a(3);
    TS_ASSERT_EQUALS(a.size(), size);
  }

  void test_sort(void)
  {
    list_obj a;
    list_obj::iterator it;

    /*
     * Case1: Sort empty list
     */
    a.sort();
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Sort all combinations of 3 unique elements.
     */
    int data_a[3] = { 0, 1, 2 };
    a.assign(data_a, data_a + 3);
    a.sort();
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_b[3] = { 0, 2, 1 };
    a.assign(data_b, data_b + 3);
    a.sort();
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_c[3] = { 1, 0, 2 };
    a.assign(data_c, data_c + 3);
    a.sort();
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_d[3] = { 1, 2, 0 };
    a.assign(data_d, data_d + 3);
    a.sort();
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_e[3] = { 2, 1, 0 };
    a.assign(data_e, data_e + 3);
    a.sort();
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_f[3] = { 2, 0, 1 };
    a.assign(data_f, data_f + 3);
    a.sort();
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    /*
     * Case3: Sort an array with duplicates.
     */
    int data_g[16] = { 13, 3, 7, 15, 2, 1, 14, 0, 11, 7, 4, 5, 11, 10, 7, 8 };
    a.assign(data_g, data_g + 16);
    a.sort();
    TS_ASSERT_EQUALS(a.size(), 16);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT_EQUALS(*(it++), 3);
    TS_ASSERT_EQUALS(*(it++), 4);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 8);
    TS_ASSERT_EQUALS(*(it++), 10);
    TS_ASSERT_EQUALS(*(it++), 11);
    TS_ASSERT_EQUALS(*(it++), 11);
    TS_ASSERT_EQUALS(*(it++), 13);
    TS_ASSERT_EQUALS(*(it++), 14);
    TS_ASSERT_EQUALS(*(it++), 15);
    TS_ASSERT(is_container_valid(a));
  }

  struct is_less_than
  {
    bool operator()(int a, int b)
    {
      return (a < b);
    }
  };

  void test_sort_compare(void)
  {
    list_obj a;
    list_obj::iterator it;

    /*
     * Case1: Sort empty list
     */
    a.sort(is_less_than());
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Sort all combinations of 3 unique elements.
     */
    int data_a[3] = { 0, 1, 2 };
    a.assign(data_a, data_a + 3);
    a.sort(is_less_than());
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_b[3] = { 0, 2, 1 };
    a.assign(data_b, data_b + 3);
    a.sort(is_less_than());
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_c[3] = { 1, 0, 2 };
    a.assign(data_c, data_c + 3);
    a.sort(is_less_than());
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_d[3] = { 1, 2, 0 };
    a.assign(data_d, data_d + 3);
    a.sort(is_less_than());
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_e[3] = { 2, 1, 0 };
    a.assign(data_e, data_e + 3);
    a.sort(is_less_than());
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    int data_f[3] = { 2, 0, 1 };
    a.assign(data_f, data_f + 3);
    a.sort(is_less_than());
    TS_ASSERT_EQUALS(a.size(), 3);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT(is_container_valid(a));

    /*
     * Case3: Sort an array with duplicates.
     */
    int data_g[16] = { 13, 3, 7, 15, 2, 1, 14, 0, 11, 7, 4, 5, 11, 10, 7, 8 };
    a.assign(data_g, data_g + 16);
    a.sort(is_less_than());
    TS_ASSERT_EQUALS(a.size(), 16);
    it = a.begin();
    TS_ASSERT_EQUALS(*(it++), 0);
    TS_ASSERT_EQUALS(*(it++), 1);
    TS_ASSERT_EQUALS(*(it++), 2);
    TS_ASSERT_EQUALS(*(it++), 3);
    TS_ASSERT_EQUALS(*(it++), 4);
    TS_ASSERT_EQUALS(*(it++), 5);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 7);
    TS_ASSERT_EQUALS(*(it++), 8);
    TS_ASSERT_EQUALS(*(it++), 10);
    TS_ASSERT_EQUALS(*(it++), 11);
    TS_ASSERT_EQUALS(*(it++), 11);
    TS_ASSERT_EQUALS(*(it++), 13);
    TS_ASSERT_EQUALS(*(it++), 14);
    TS_ASSERT_EQUALS(*(it++), 15);
    TS_ASSERT(is_container_valid(a));
  }

  void test_splice_list(void)
  {
    list_obj a;
    list_obj b;
    list_obj::iterator it;

    /*
     * Case1: Test splice on empty list
     */
    a.splice(a.begin(), b);
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(b.size(), 0);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const obj val = 19;

      /*
       * Case 2: Test splice at end
       */
      b.assign(OBJ_DATA, OBJ_DATA + 4);
      a.splice(a.end(), b);
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 0);
      TS_ASSERT(is_container_valid(b));
      it = a.begin();
      for (int i = 0; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }

      /*
       * Case3: Test splice at begin
       */
      list_obj c(OBJ_DATA + 4, OBJ_DATA + 8);
      a.splice(a.begin(), c);
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(c.size(), 0);
      TS_ASSERT(is_container_valid(c));
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = 4; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 4]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }

      /*
       * Case4: Test splice in middle
       */
      list_obj d(OBJ_DATA + 8, OBJ_DATA + 16);
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      a.splice(it, d);
      current_size += 8;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(d.size(), 0);
      TS_ASSERT(is_container_valid(d));
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = 4; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 4]);
        ++it;
      }
      for (int i = 8; i < 16; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = mid_index + 8; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 12]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_splice_element(void)
  {
    list_obj a;
    list_obj b;
    list_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      b.assign(OBJ_DATA, OBJ_DATA + 16);
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const obj val = 19;

      /*
       * Case1: Test splice at end
       */
      a.splice(a.end(), b, b.begin());
      ++current_size;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 15);
      it = a.begin();
      for (int i = 0; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, OBJ_DATA[0]);

      /*
       * Case2: Test splice at begin
       */
      a.splice(a.begin(), b, --b.end());
      ++current_size;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 14);
      TS_ASSERT_EQUALS(a.front(), OBJ_DATA[15]);
      it = ++a.begin();
      for (int i = 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, OBJ_DATA[0]);

      /*
       * Case3: Test splice in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      a.splice(it, b, b.begin());
      ++current_size;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 13);
      TS_ASSERT_EQUALS(a.front(), OBJ_DATA[15]);
      it = ++a.begin();
      for (int i = 1; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, OBJ_DATA[1]);
      ++it;
      for (int i = mid_index + 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 2]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, OBJ_DATA[0]);
      TS_ASSERT(is_container_valid(a));

      it = b.begin();
      for (int i = 2; i < 15; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT(is_container_valid(b));
    } //for: SIZE_COUNT
  }

  void test_splice_range(void)
  {
    list_obj a;
    list_obj b;
    list_obj::iterator it;

    /*
     * Case1: Test splice on empty list
     */
    a.splice(a.begin(), b, b.begin(), b.end());
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(b.size(), 0);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const obj val = 19;

      /*
       * Case 2: Test splice at end
       */
      b.assign(OBJ_DATA, OBJ_DATA + 4);
      a.splice(a.end(), b, b.begin(), b.end());
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 0);
      TS_ASSERT(is_container_valid(b));
      it = a.begin();
      for (int i = 0; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }

      /*
       * Case3: Test splice at begin
       */
      list_obj c(OBJ_DATA + 4, OBJ_DATA + 8);
      a.splice(a.begin(), c, c.begin(), c.end());
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(c.size(), 0);
      TS_ASSERT(is_container_valid(c));
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = 4; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 4]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }

      /*
       * Case4: Test splice in middle
       */
      list_obj d(OBJ_DATA + 8, OBJ_DATA + 16);
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      a.splice(it, d, d.begin(), d.end());
      current_size += 8;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(d.size(), 0);
      TS_ASSERT(is_container_valid(d));
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = 4; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 4]);
        ++it;
      }
      for (int i = 8; i < 16; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      for (int i = mid_index + 8; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i - 12]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i]);
        ++it;
      }
      TS_ASSERT(is_container_valid(a));
    } //for: SIZE_COUNT
  }

  void test_swap(void)
  {
    list_obj a;
    list_obj b;

    /*
     * Case 1: Test on empty container
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      list_obj tmp;
      a.swap(tmp);
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT_EQUALS(tmp.size(), SIZES[s]);
      TS_ASSERT(tmp == list_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
      TS_ASSERT(is_container_valid(a));
    }

    /*
     * Case 2: Test on two populated containers
     */
    a.assign(OBJ_DATA, OBJ_DATA + 9);
    b.assign(OBJ_DATA + 9, OBJ_DATA + 16);

    a.swap(b);
    TS_ASSERT_EQUALS(a.size(), 7);
    TS_ASSERT_EQUALS(b.size(), 9);
    TS_ASSERT(a == list_obj(OBJ_DATA + 9, OBJ_DATA + 16));
    TS_ASSERT(b == list_obj(OBJ_DATA, OBJ_DATA + 9));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
  }

  void test_unique(void)
  {
    list_obj a;

    /*
     * Case1: Test on empty list
     */
    a.unique();
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Normal conditions.
     */
    int data[25] = { 0, 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 8, 8, 9, 10, 11, 11, 11, 11, 12, 13, 14, 15, 15, 15 };
    a.assign(data, data + 25);
    TS_ASSERT_EQUALS(a.size(), 25);
    a.unique();
    TS_ASSERT_EQUALS(a.size(), 16);
    list_obj::iterator it = a.begin();
    for (int i = 0; i < 16; ++i)
    {
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i]);
    }
    TS_ASSERT(is_container_valid(a));
  }

  struct is_equal
  {
    bool operator()(int first, int second)
    {
      return (first == second);
    }
  };

  void test_unique_predicate(void)
  {
    list_obj a;

    /*
     * Case1: Test on empty list
     */
    a.unique(is_equal());
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT(is_container_valid(a));

    /*
     * Case2: Normal conditions.
     */
    int data[25] = { 0, 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 8, 8, 9, 10, 11, 11, 11, 11, 12, 13, 14, 15, 15, 15 };
    a.assign(data, data + 25);
    TS_ASSERT_EQUALS(a.size(), 25);
    a.unique(is_equal());
    TS_ASSERT_EQUALS(a.size(), 16);
    list_obj::iterator it = a.begin();
    for (int i = 0; i < 16; ++i)
    {
      TS_ASSERT_EQUALS(*(it++), OBJ_DATA[i]);
    }
    TS_ASSERT(is_container_valid(a));
  }

  void test_assignment_operator(void)
  {
    //Light-weight test, as this simply calls the assign() method.
    list_obj a;
    list_obj tmp(OBJ_DATA, OBJ_DATA + 8);
    list_obj::iterator it;
    list_obj::iterator tmp_it;
    a = tmp;
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.size(), a.capacity());
    it = a.begin();
    tmp_it = tmp.begin();
    for (int i = 0; i < a.size(); ++i)
    {
      TS_ASSERT_EQUALS(*(it++), *(tmp_it++));
    }
    TS_ASSERT(is_container_valid(a));
  }

  void test_assignment_operator_move()
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Normal condition.
     */
    list_obj a =
    { 0, 1, 2, 3};
    clear_copy_flags(a);
    list_obj b;
    b = std::move(a);
    mark_move_only(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(a.size(),0);
    TS_ASSERT_EQUALS(b.size(),4);
    list_obj::iterator it = b.begin();
    TS_ASSERT_EQUALS(*it++, 0);
    TS_ASSERT_EQUALS(*it++, 1);
    TS_ASSERT_EQUALS(*it++, 2);
    TS_ASSERT_EQUALS(*it++, 3);
#endif

  }

  void test_assignment_operator_initializer()
  {
    /*
     * Case1: Normal condition
     */
    list_obj a;
    a =
    { 0, 1, 2, 3};
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    list_obj::iterator it = a.begin();
    TS_ASSERT_EQUALS(*it++, 0);
    TS_ASSERT_EQUALS(*it++, 1);
    TS_ASSERT_EQUALS(*it++, 2);
    TS_ASSERT_EQUALS(*it++, 3);
  }

  void test_pop_back(void)
  {
    list_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[current_size - 1]);
        a.pop_back();
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
      TS_ASSERT(is_container_valid(a));
    }

  }

  void test_pop_front(void)
  {
    list_obj a;

    /*
     * Case 1: Normal conditions
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.front(), OBJ_DATA[SIZES[s] - current_size]);
        a.pop_front();
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
      TS_ASSERT(is_container_valid(a));
    }
  }

  void test_push_back(void)
  {
    list_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        a.push_back(OBJ_DATA[i]);
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[i]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
      TS_ASSERT(is_container_valid(a));
    }
  }

  void test_push_back_move(void)
  {
#ifdef FLEX_HAS_CXX11
    list_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        obj tmp = OBJ_DATA[i];
        tmp.was_copied = false;
        a.push_back(std::move(tmp));
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[i]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
      mark_move_only(a);
      TS_ASSERT(is_container_valid(a));
    }
#endif
  }

  void test_push_front(void)
  {
    list_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        const unsigned data_index = SIZES[s] - 1 - i;
        a.push_front(OBJ_DATA[data_index]);
        TS_ASSERT_EQUALS(a.front(), OBJ_DATA[data_index]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
      TS_ASSERT(is_container_valid(a));
    }
  }

  void test_push_front_move(void)
  {
#ifdef FLEX_HAS_CXX11
    list_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        const unsigned data_index = SIZES[s] - 1 - i;
        obj tmp = OBJ_DATA[data_index];
        tmp.was_copied = false;
        a.push_front(std::move(tmp));
        TS_ASSERT_EQUALS(a.front(), OBJ_DATA[data_index]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
      mark_move_only(a);
      TS_ASSERT(is_container_valid(a));
    }
#endif
  }

  void test_equality_operator(void)
  {
    /*
     * Case1: Test size of 0.
     */
    list_obj a;
    list_obj b;
    TS_ASSERT((a == b));

    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      /*
       * Case2: Test containers that are equal
       */
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      b.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT((a == b));

      /*
       * Case3: Test containers that are different sizes
       */
      a.pop_back();
      TS_ASSERT(!(a == b));

      /*
       * Case4: Test on empty container
       */
      a.assign((int*) NULL, (int*) NULL);
      TS_ASSERT(!(a == b));
    }
  }

  void test_less_than_operator(void)
  {
    /*
     * Case1: Test size of 0.
     */
    list_obj a;
    list_obj b;
    list_obj::iterator ait;
    list_obj::iterator bit;
    TS_ASSERT(!(a < b));
    TS_ASSERT(!(b < a));

    for (unsigned s = 3; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Test containers that are equal
       */
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      b.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT(!(a < b));
      TS_ASSERT(!(b < a));

      /*
       * Case2: Test containers that are not equal
       */
      a.pop_back();
      TS_ASSERT(a < b);
      TS_ASSERT(!(b < a));

      b.pop_back();
      b.pop_front();
      TS_ASSERT(a < b);
      TS_ASSERT(!(b < a));

      ait = a.begin();
      bit = b.begin();
      *bit = *ait - 1;
      TS_ASSERT(b < a);
      TS_ASSERT(!(a < b));

      /*
       * Case3: Test on empty containers
       */
      b.assign((int*) NULL, (int*) NULL);
      TS_ASSERT(b < a);
      TS_ASSERT(!(a < b));
    }
  }

  void test_relational_operators(void)
  {
    int aAry[5] = { 10, 20, 30, 40, 50 };
    int bAry[5] = { 10, 20, 30, 40, 50 };
    int cAry[5] = { 50, 40, 30, 20, 10 };
    list_obj a(5);
    list_obj b(5);
    list_obj c(5);

    list_obj::iterator ait = a.begin();
    list_obj::iterator bit = b.begin();
    list_obj::iterator cit = c.begin();
    for (int i = 0; i < 5; ++i)
    {
      *(ait++) = *(bit++) = (i * 10) + 10;
      *(cit++) = 50 - (i * 10);
    }

    TS_ASSERT((a == b));
    TS_ASSERT(b != c);
    TS_ASSERT(b < c);
    TS_ASSERT(c > b);
    TS_ASSERT(a <= b);
    TS_ASSERT(a >= b);
  }

}
;
