#include <cxxtest/TestSuite.h>

#include <flex/list.h>
#include <flex/allocator_debug.h>

class list_test: public CxxTest::TestSuite
{

  struct object
  {
    static const int INIT_VAL = 1;

    object() :
        val(INIT_VAL)
    {
    }

    object(int i) :
        val(i)
    {
    }

    int val;
  };

  typedef flex::list<int, flex::allocator_debug<flex::list<int>::node_type> > list_int;
  typedef flex::list<object, flex::allocator_debug<flex::list<object>::node_type> > list_obj;

  const int INT_DATA[128] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 39535304, 2113617954, -262399995,
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

  const object OBJ_DATA[128] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 39535304, 2113617954, -262399995,
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
    flex::allocator_debug<flex::list<int>::node_type>::clear();
    flex::allocator_debug<flex::list<object>::node_type>::clear();
  }

  void tearDown()
  {
    //This ensures that all objects constructed by the container have their destructors called.
    TS_ASSERT(flex::allocator_debug<flex::list<int>::node_type>::mConstructedPointers.empty());
    TS_ASSERT(flex::allocator_debug<flex::list<object>::node_type>::mConstructedPointers.empty());

    //This ensures that all memory allocated by the container is properly freed.
    TS_ASSERT(flex::allocator_debug<flex::list<int>::node_type>::mAllocatedPointers.empty());
    TS_ASSERT(flex::allocator_debug<flex::list<object>::node_type>::mAllocatedPointers.empty());
  }

  template<class List>
  void assert_list_validity(const List& list)
  {
    //This checks to ensure the list is valid.  The main purpose is to ensure that
    //all next and prev node pointers are in sync.  This is used in list modifier
    //tests such as assign(), insert(), erase() etc, to ensure the list is kept in
    //a valid state.
    typename List::const_iterator prev = --list.begin();
    typename List::const_iterator it = list.begin();
    size_t n = 0;
    while (it != list.end())
    {
      TS_ASSERT_EQUALS(prev.mNode, it.mNode->mPrev);
      ++it;
      ++prev;
      ++n;
    }
    TS_ASSERT_EQUALS(prev.mNode, it.mNode->mPrev);
    TS_ASSERT_EQUALS(n, list.size());
  }

  void test_default_constructor(void)
  {
    /*
     * Case1: Verify default constructor works and doesn't allocate space.
     */
    flex::allocation_guard::enable();
    list_int a;
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
        TS_ASSERT_THROWS(list_int a(SIZES[s]), std::runtime_error);
        flex::allocation_guard::disable();
      }

      /*
       * Case2: Verify fill constructor with primitive elements.
       */
      list_int a(SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, 0);
      }
      assert_list_validity(a);

      /*
       * Case3: Verify fill constructor with object elements.
       */
      list_obj b(SIZES[s]);
      TS_ASSERT_EQUALS(b.size(), SIZES[s]);
      TS_ASSERT_EQUALS(b.size(), b.capacity());
      for (list_obj::iterator it = b.begin(); it != b.end(); ++it)
      {
        TS_ASSERT_EQUALS((*it).val, object::INIT_VAL);
      }
      assert_list_validity(b);

    } //for: SIZE_COUNT
  }

  void test_fill_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const int fill_val = INT_DATA[SIZES[s] - 1];

      /*
       * Case1: Verify fill constructor assigns value parameter for primitives.
       */
      list_int a(SIZES[s], fill_val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, fill_val);
      }
      assert_list_validity(a);

      /*
       * Case1: Verify fill constructor assigns value parameter for objects.
       */
      list_obj b(SIZES[s], object(fill_val));
      TS_ASSERT_EQUALS(b.size(), SIZES[s]);
      TS_ASSERT_EQUALS(b.size(), b.capacity());
      for (list_obj::iterator it = b.begin(); it != b.end(); ++it)
      {
        TS_ASSERT_EQUALS((*it).val, fill_val);
      }
      assert_list_validity(b);

    } //for: SIZE_COUNT
  }

  void test_range_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Verify range constructor with pointer parameters
       */
      list_int a(INT_DATA, INT_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      int i = 0;
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++i;
      }
      assert_list_validity(a);

      /*
       * Case2: Verify range constructor with iterator parameters
       */
      list_int b(a.begin(), a.end());
      TS_ASSERT_EQUALS(b.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      i = 0;
      for (list_int::iterator it = b.begin(); it != b.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++i;
      }
      assert_list_validity(b);

      /*
       * Case3: Verify range constructor for object elements.
       */
      list_obj c(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(c.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      i = 0;
      for (list_obj::iterator it = c.begin(); it != c.end(); ++it)
      {
        TS_ASSERT_EQUALS((*it).val, OBJ_DATA[i].val);
        ++i;
      }
      assert_list_validity(c);
    } //for: SIZE_COUNT
  }

  void test_copy_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Verify copy constructor allocates memory
       */
      list_int a(INT_DATA, INT_DATA + SIZES[s]);
      if (a.size() > 0)
      {
        flex::allocation_guard::enable();
        TS_ASSERT_THROWS(list_int b(a), std::runtime_error);
        flex::allocation_guard::disable();
      }
      assert_list_validity(a);

      /*
       * Case2: Verify copy constructor with primitive elements.
       */
      list_int b(a);
      TS_ASSERT_EQUALS(b.size(), a.size());
      TS_ASSERT_EQUALS(b.size(), b.capacity());
      list_int::iterator ait = a.begin();
      for (list_int::iterator it = b.begin(); it != b.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, *ait);
        ++ait;
      }
      assert_list_validity(b);

      /*
       * Case3: Verify copy constructor with object elements.
       */
      list_obj c(OBJ_DATA, OBJ_DATA + SIZES[s]);
      list_obj d(c);
      TS_ASSERT_EQUALS(d.size(), c.size());
      TS_ASSERT_EQUALS(d.size(), d.capacity());
      list_obj::iterator cit = c.begin();
      for (list_obj::iterator it = d.begin(); it != d.end(); ++it)
      {
        TS_ASSERT_EQUALS((*it).val, (*cit).val);
        ++cit;
      }
      assert_list_validity(d);

    } //for: SIZE_COUNT
  }

  void test_assign_fill(void)
  {
    /*
     * Case1: Verify assign can increase size.
     */
    list_int a;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const int fill_val = INT_DATA[SIZES[s] - 1];
      a.assign(SIZES[s], fill_val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, fill_val);
      }
      assert_list_validity(a);
    }

    /*
     * Case2: Verify assign can decrease size.
     */
    for (int s = SIZE_COUNT - 1; s != -1; --s)
    {
      const int fill_val = INT_DATA[SIZES[s] - 1];
      a.assign(SIZES[s], fill_val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, fill_val);
      }
      assert_list_validity(a);
    }
  }

  void test_assign_iterator()
  {
    list_int a;

    /*
     * Case1: Verify assign can increase size.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_int tmp(INT_DATA, INT_DATA + SIZES[s]);
      a.assign(tmp.begin(), tmp.end());
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      list_int::iterator tmp_it = tmp.begin();
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, *tmp_it);
        ++tmp_it;
      }
      assert_list_validity(a);
    }

    /*
     * Case2: Verify assign can decrease size.
     */
    for (int s = SIZE_COUNT - 1; s != -1; --s)
    {
      list_int tmp(INT_DATA, INT_DATA + SIZES[s]);
      size_t prev_capacity = a.capacity();
      a.assign(tmp.begin(), tmp.end());
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      list_int::iterator tmp_it = tmp.begin();
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, *tmp_it);
        ++tmp_it;
      }
      assert_list_validity(a);
    }
  }

  void test_assign_pointer()
  {
    list_int a;

    /*
     * Case1: Verify assign can increase size.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), a.capacity());
      int i = 0;
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++i;
      }
      assert_list_validity(a);
    }

    /*
     * Case2: Verify assign can decrease size.
     */
    for (int s = SIZE_COUNT - 1; s != -1; --s)
    {
      size_t prev_capacity = a.capacity();
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      int i = 0;
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++i;
      }
      assert_list_validity(a);
    }
  }

  void test_back(void)
  {
    //Start s at 1, as back() isn't supported on empty container.
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      list_int a(INT_DATA, INT_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.back(), INT_DATA[SIZES[s] - 1]);
    } //for: SIZE_COUNT
  }

  void test_back_const(void)
  {
    //Start s at 1, as back() isn't supported on empty container.
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      const list_int a(INT_DATA, INT_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.back(), INT_DATA[SIZES[s] - 1]);
    } //for: SIZE_COUNT
  }

  void test_begin_and_end(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_int a(INT_DATA, INT_DATA + SIZES[s]);

      int i = 0;
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
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
      const list_int a(INT_DATA, INT_DATA + SIZES[s]);

      int i = 0;
      for (list_int::const_iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++i;
      }
      TS_ASSERT_EQUALS(i, a.size());
    } //for: SIZE_COUNT
  }

  void test_cbegin_and_cend(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const list_int a(INT_DATA, INT_DATA + SIZES[s]);

      int i = 0;
      for (list_int::const_iterator it = a.cbegin(); it != a.cend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++i;
      }
      TS_ASSERT_EQUALS(i, a.size());
    } //for: SIZE_COUNT
  }

  void test_crbegin_and_crend(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const list_int a(INT_DATA, INT_DATA + SIZES[s]);

      int i = 0;
      for (list_int::const_reverse_iterator it = a.crbegin(); it != a.crend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + a.size() - 1]);
        --i;
      }
      TS_ASSERT_EQUALS(i, -a.size());
    } //for: SIZE_COUNT
  }

  void test_capacity(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_int a(SIZES[s]);
      TS_ASSERT_EQUALS(a.capacity(), SIZES[s]);
      a.clear();
      TS_ASSERT_EQUALS(a.capacity(), SIZES[s]);
    } //for: SIZE_COUNT
  }

  void test_clear(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_int a(INT_DATA, INT_DATA + SIZES[s]);
      a.clear();
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT(a.empty());
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_empty(void)
  {
    /*
     * Case 1: Verify empty on init.
     */
    list_int a;
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
    list_int a(1);
    list_int::iterator it;
    list_int::iterator erase_it;
    it = a.erase(a.begin());
    TS_ASSERT_EQUALS(it, a.begin());
    TS_ASSERT_EQUALS(a.size(), 0);

    //Start s at 3, as the below tests expect a size of at least 3.
    for (unsigned s = 3; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();

      /*
       * Case2: Test erase at end
       */
      it = a.erase(--a.end());
      --current_size;
      TS_ASSERT_EQUALS(it, a.end());
      TS_ASSERT_EQUALS(a.size(), current_size);
      int i = 0;
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
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
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + 1]);
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
        TS_ASSERT_EQUALS(*it, INT_DATA[i + 1]);
        ++it;
      }
      for (int i = mid_index; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + 2]);
        ++it;
      }
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_erase_range()
  {
    /*
     * Case 1: Test erase on size of 1.
     */
    list_int a(1);
    list_int::iterator it;
    list_int::iterator erase_it;
    it = a.erase(a.begin(), a.end());
    TS_ASSERT_EQUALS(it, a.end());
    TS_ASSERT_EQUALS(a.size(), 0);

    //Start s at 7, as the below tests expect a size of at least 7.
    for (unsigned s = 7; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
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
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
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
      for (list_int::iterator it = a.begin(); it != a.end(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + 2]);
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
      list_int::iterator tmp_it = erase_it;
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
      list_int::iterator it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + 2]);
        ++it;
      }
      for (int i = mid_index; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + 4]);
        ++it;
      }
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_front(void)
  {
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      list_int a(INT_DATA, INT_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.front(), INT_DATA[0]);
    } //for: SIZE_COUNT
  }

  void test_front_const(void)
  {
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      const list_int a(INT_DATA, INT_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.front(), INT_DATA[0]);
    } //for: SIZE_COUNT
  }

  void test_insert_position(void)
  {
    list_int a;
    list_int::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();
      const int val = 19;

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
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
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
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 1]);
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
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      ++it;
      for (int i = mid_index + 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 2]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, val);
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_insert_fill(void)
  {
    list_int a;
    list_int::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();
      const int val = 19;

      /*
       * Case1: Test insert at end
       */
      a.insert(a.end(), 2, val);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
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
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 2]);
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
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(*(it++), val);
      TS_ASSERT_EQUALS(*(it++), val);
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_insert_pointers(void)
  {
    list_int a;
    list_int::iterator it;

    //Start s at 3, as the below tests expect a size of at least 3.
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();

      /*
       * Case1: Test insert at end
       */
      a.insert(a.end(), INT_DATA, INT_DATA + 2);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      for (int i = 0; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);

      /*
       * Case2: Test insert at begin
       */
      a.insert(a.begin(), INT_DATA, INT_DATA + 2);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      for (int i = 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.begin();
      for (int i = 0; i < mid_index; ++i)
      {
        ++it;
      }
      a.insert(it, INT_DATA, INT_DATA + 2);
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      for (int i = 2; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_insert_iterators(void)
  {
    list_int a;
    list_int b(INT_DATA, INT_DATA + 2);
    list_int::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
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
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);

      /*
       * Case2: Test insert at begin
       */
      a.insert(a.begin(), b.begin(), b.end());
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      it = a.begin();
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      for (int i = 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);

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
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      for (int i = 2; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(*(it++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(it++), INT_DATA[1]);
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_max_size(void)
  {
    const list_int a;
    TS_ASSERT_EQUALS(a.max_size(), a.get_allocator().max_size());
  }

  void test_merge(void)
  {
    int a_data[8] = { 0, 2, 4, 5, 8, 9, 11, 13 };
    int b_data[8] = { 1, 3, 6, 7, 10, 12, 14, 15 };
    list_int a(a_data, a_data + 8);
    list_int b(b_data, b_data + 8);

    a.merge(b);
    TS_ASSERT_EQUALS(a.size(), 16);
    TS_ASSERT_EQUALS(b.size(), 0);
    list_int::iterator it = a.begin();
    for (int i = 0; i < a.size(); ++i)
    {
      TS_ASSERT_EQUALS(*it, INT_DATA[i]);
      ++it;
    }
    assert_list_validity(a);
    assert_list_validity(b);
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
    int a_data[8] = { 13, 11, 9, 8, 5, 4, 2, 0 };
    int b_data[8] = { 15, 14, 12, 10, 7, 6, 3, 1 };
    list_int a(a_data, a_data + 8);
    list_int b(b_data, b_data + 8);

    a.merge(b, reverse_merge_functor());
    TS_ASSERT_EQUALS(a.size(), 16);
    TS_ASSERT_EQUALS(b.size(), 0);
    list_int::iterator it = a.begin();
    for (int i = 15; i >= 0; --i)
    {
      TS_ASSERT_EQUALS(*it, INT_DATA[i]);
      ++it;
    }
    assert_list_validity(a);
    assert_list_validity(b);
  }

  void test_rbegin_and_rend(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_int a(INT_DATA, INT_DATA + SIZES[s]);

      int i = 0;
      for (list_int::reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + a.size() - 1]);
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
      const list_int a(INT_DATA, INT_DATA + SIZES[s]);

      int i = 0;
      for (list_int::const_reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i + a.size() - 1]);
        --i;
      }
      TS_ASSERT_EQUALS(i, -a.size());
    } //for: SIZE_COUNT
  }

  void test_remove(void)
  {
    int data[16] = { 0, 0, 2, 3, 11, 5, 6, 7, 11, 9, 10, 11, 12, 13, 14, 15 };
    list_int a(data, data + 16);

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
    list_int::iterator it = a.begin();
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
    assert_list_validity(a);
  }

  struct is_zero_functor
  {
    bool operator()(int val)
    {
      return (val == 0);
    }
  };

  struct is_negative_functor
  {
    bool operator()(int val)
    {
      return (val < 0);
    }
  };

  struct is_over_thirty
  {
    bool operator()(int val)
    {
      return (val > 30);
    }
  };

  void test_remove_predicate(void)
  {
    int data[16] = { 0, 0, 2, 3, -11, 5, 6, 7, -11, 9, 10, -11, 12, 13, 14, 35 };
    list_int a(data, data + 16);

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
    list_int::iterator it = a.begin();
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
    assert_list_validity(a);
  }

  void test_reserve(void)
  {
    list_int bar;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      bar.reserve(SIZES[s]);
      TS_ASSERT_EQUALS(bar.size(), 0);
      TS_ASSERT_LESS_THAN_EQUALS(SIZES[s], bar.capacity());
    }
  }

  void test_resize(void)
  {
    list_int a;
    const int val = 19;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case 1: Verify resize can decrease size.
       */
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t new_size = SIZES[s] / 2;
      a.resize(new_size);
      TS_ASSERT_EQUALS(a.size(), new_size);
      TS_ASSERT(a == list_int(INT_DATA, INT_DATA + new_size));
      assert_list_validity(a);

      /*
       * Case 2: Verify resize can increase size.
       */
      a.resize(SIZES[s], val);
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      list_int::iterator it = a.begin();
      for (int i = 0; i < new_size; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), INT_DATA[i]);
      }
      for (int i = new_size; i < SIZES[s]; ++i)
      {
        TS_ASSERT_EQUALS(*(it++), val);
      }
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_reverse(void)
  {
    list_int a;
    list_int::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case 1: Normal conditions
       */
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      a.reverse();
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      it = a.begin();
      for (int i = SIZES[s] - 1; i >= 0; --i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_shrink_to_fit(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      list_int a(INT_DATA, INT_DATA + SIZES[s]);
      a.shrink_to_fit();
      TS_ASSERT_EQUALS(a.size(), a.capacity());

      a.assign(INT_DATA, INT_DATA + (SIZES[s] / 2));
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
    const list_int a(3);
    TS_ASSERT_EQUALS(a.size(), size);
  }

  void test_splice_list(void)
  {
    list_int a;
    list_int b;
    list_int::iterator it;

    /*
     * Case1: Test splice on empty list
     */
    a.splice(a.begin(), b);
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(b.size(), 0);
    assert_list_validity(a);
    assert_list_validity(b);

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();
      const int val = 19;

      /*
       * Case 2: Test splice at end
       */
      b.assign(INT_DATA, INT_DATA + 4);
      a.splice(a.end(), b);
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 0);
      assert_list_validity(b);
      it = a.begin();
      for (int i = 0; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }

      /*
       * Case3: Test splice at begin
       */
      list_int c(INT_DATA + 4, INT_DATA + 8);
      a.splice(a.begin(), c);
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(c.size(), 0);
      assert_list_validity(c);
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = 4; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 4]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }

      /*
       * Case4: Test splice in middle
       */
      list_int d(INT_DATA + 8, INT_DATA + 16);
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
      assert_list_validity(d);
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = 4; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 4]);
        ++it;
      }
      for (int i = 8; i < 16; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = mid_index + 8; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 12]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_splice_element(void)
  {
    list_int a;
    list_int b;
    list_int::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      b.assign(INT_DATA, INT_DATA + 16);
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();
      const int val = 19;

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
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, INT_DATA[0]);

      /*
       * Case2: Test splice at begin
       */
      a.splice(a.begin(), b, --b.end());
      ++current_size;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 14);
      TS_ASSERT_EQUALS(a.front(), INT_DATA[15]);
      it = ++a.begin();
      for (int i = 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, INT_DATA[0]);

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
      TS_ASSERT_EQUALS(a.front(), INT_DATA[15]);
      it = ++a.begin();
      for (int i = 1; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 1]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, INT_DATA[1]);
      ++it;
      for (int i = mid_index + 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 2]);
        ++it;
      }
      TS_ASSERT_EQUALS(*it, INT_DATA[0]);
      assert_list_validity(a);

      it = b.begin();
      for (int i = 2; i < 15; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      assert_list_validity(b);
    } //for: SIZE_COUNT
  }

  void test_splice_range(void)
  {
    list_int a;
    list_int b;
    list_int::iterator it;

    /*
     * Case1: Test splice on empty list
     */
    a.splice(a.begin(), b, b.begin(), b.end());
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(b.size(), 0);
    assert_list_validity(a);
    assert_list_validity(b);

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();
      const int val = 19;

      /*
       * Case 2: Test splice at end
       */
      b.assign(INT_DATA, INT_DATA + 4);
      a.splice(a.end(), b, b.begin(), b.end());
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(b.size(), 0);
      assert_list_validity(b);
      it = a.begin();
      for (int i = 0; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }

      /*
       * Case3: Test splice at begin
       */
      list_int c(INT_DATA + 4, INT_DATA + 8);
      a.splice(a.begin(), c, c.begin(), c.end());
      current_size += 4;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(c.size(), 0);
      assert_list_validity(c);
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = 4; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 4]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }

      /*
       * Case4: Test splice in middle
       */
      list_int d(INT_DATA + 8, INT_DATA + 16);
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
      assert_list_validity(d);
      it = a.begin();
      for (int i = 4; i < 8; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = 4; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 4]);
        ++it;
      }
      for (int i = 8; i < 16; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      for (int i = mid_index + 8; i < a.size() - 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i - 12]);
        ++it;
      }
      for (int i = 0; i < 4; ++i)
      {
        TS_ASSERT_EQUALS(*it, INT_DATA[i]);
        ++it;
      }
      assert_list_validity(a);
    } //for: SIZE_COUNT
  }

  void test_swap(void)
  {
    list_int a;
    list_int b;

    /*
     * Case 1: Test on empty container
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      list_int tmp;
      a.swap(tmp);
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT_EQUALS(tmp.size(), SIZES[s]);
      TS_ASSERT(tmp == list_int(INT_DATA, INT_DATA + SIZES[s]));
      assert_list_validity(a);
    }

    /*
     * Case 2: Test on two populated containers
     */
    a.assign(INT_DATA, INT_DATA + 9);
    b.assign(INT_DATA + 9, INT_DATA + 16);

    a.swap(b);
    TS_ASSERT_EQUALS(a.size(), 7);
    TS_ASSERT_EQUALS(b.size(), 9);
    TS_ASSERT(a == list_int(INT_DATA + 9, INT_DATA + 16));
    TS_ASSERT(b == list_int(INT_DATA, INT_DATA + 9));
    assert_list_validity(a);
    assert_list_validity(b);
  }

  void test_assignment_operator(void)
  {
    //Light-weight test, as this simply calls the assign() method.
    list_int a;
    list_int tmp(INT_DATA, INT_DATA + 8);
    list_int::iterator it;
    list_int::iterator tmp_it;
    a = tmp;
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_EQUALS(a.size(), a.capacity());
    it = a.begin();
    tmp_it = tmp.begin();
    for (int i = 0; i < a.size(); i++)
    {
      TS_ASSERT_EQUALS(*(it++), *(tmp_it++));
    }
    assert_list_validity(a);
  }

  void test_pop_back(void)
  {
    list_int a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.back(), INT_DATA[current_size - 1]);
        a.pop_back();
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
      assert_list_validity(a);
    }

  }

  void test_pop_front(void)
  {
    list_int a;

    /*
     * Case 1: Normal conditions
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.front(), INT_DATA[SIZES[s] - current_size]);
        a.pop_front();
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
      assert_list_validity(a);
    }
  }

  void test_push_back(void)
  {
    list_int a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        a.push_back(INT_DATA[i]);
        TS_ASSERT_EQUALS(a.back(), INT_DATA[i]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_int(INT_DATA, INT_DATA + SIZES[s]));
      assert_list_validity(a);
    }

  }

  void test_push_front(void)
  {
    list_int a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign((int*) NULL, (int*) NULL);
      for (int i = 0; i < SIZES[s]; ++i)
      {
        const unsigned data_index = SIZES[s] - 1 - i;
        a.push_front(INT_DATA[data_index]);
        TS_ASSERT_EQUALS(a.front(), INT_DATA[data_index]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == list_int(INT_DATA, INT_DATA + SIZES[s]));
      assert_list_validity(a);
    }
  }

  void test_equality_operator(void)
  {
    /*
     * Case1: Test size of 0.
     */
    list_int a;
    list_int b;
    TS_ASSERT((a == b));

    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      /*
       * Case2: Test containers that are equal
       */
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      b.assign(INT_DATA, INT_DATA + SIZES[s]);
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
    list_int a;
    list_int b;
    list_int::iterator ait;
    list_int::iterator bit;
    TS_ASSERT(!(a < b));
    TS_ASSERT(!(b < a));

    for (unsigned s = 3; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Test containers that are equal
       */
      a.assign(INT_DATA, INT_DATA + SIZES[s]);
      b.assign(INT_DATA, INT_DATA + SIZES[s]);
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
    list_int a(5);
    list_int b(5);
    list_int c(5);

    list_int::iterator ait = a.begin();
    list_int::iterator bit = b.begin();
    list_int::iterator cit = c.begin();
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
