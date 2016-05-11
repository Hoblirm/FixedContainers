#include <cxxtest/TestSuite.h>

#include "flex/fixed_ring.h"
#include "flex/debug/allocator.h"
#include "flex/debug/obj.h"

class fixed_ring_test: public CxxTest::TestSuite
{

  typedef flex::debug::obj obj;
  typedef flex::fixed_ring<flex::debug::obj, 128, flex::debug::allocator<flex::debug::obj> > ring_obj;

  const flex::debug::obj OBJ_DATA[128] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 39535304, 2113617954,
      -262399995, -1776526244, 2007130159, -751355444, -1850306681, 1670328314, 174975647, 1520325186, 752193990,
      1141698902, 414986917, -1084506988, -1274438196, -407784340, -1476797751, 952482371, 1659351065, -1840296979,
      1174260466, -830555035, 1187249412, -1439716735, -606656096, 1968778085, -468774603, -741213671, -1792595459,
      -1043591241, -399781674, 1441797965, -539577554, -1712941906, 893437261, 1243708130, -276655685, 169167272,
      1548266128, 2134938409, -165983522, 65335344, 777222631, -1975346548, 1736737965, -1297235370, -1778585082,
      -445115751, 77287795, -904742465, 1566979049, -1276550055, -1523151595, -1877472326, -1965521838, 309774311,
      285638537, 1694499811, 395062486, -599472639, -562348494, 622523556, 1991792880, 1485225099, -26143183,
      1213635789, -1867261885, 1401932595, 1643956672, 1152265615, -206296253, -1341812088, -928119996, 1335888378,
      -2127839732, -805081880, -461979923, 258594093, 1322814281, -1856950276, 763906168, -110775798, 29138078,
      -728231554, -1738124420, -1130024844, 2112808498, -2147190929, -46681067, -1746560845, -1931350352, -2121713887,
      -2077836858, -68560373, 542144249, -964249373, 672765407, 1240222082, -170251308, 573136605, 522427348,
      -1842488270, -803442179, 1214800559, -439290856, -850489475, -371113959, -528653948, -1466750983, -299654597,
      -1095361209, 912904732 };

  const size_t SIZE_COUNT = 10;
  const size_t SIZES[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 128 };

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

  void mark_move_only(flex::ring<flex::debug::obj, flex::debug::allocator<flex::debug::obj> >& c)
  {
#ifdef FLEX_HAS_CXX11
    for (int i = 0; i < c.size(); ++i)
    {
      c[i].move_only = true;
    }
#endif
  }

  void clear_copy_flags(flex::ring<flex::debug::obj, flex::debug::allocator<flex::debug::obj> >& c)
  {
    for (int i = 0; i < c.size(); ++i)
    {
      c[i].was_copied = false;
    }
  }

  bool is_container_valid(const flex::ring<flex::debug::obj, flex::debug::allocator<flex::debug::obj> >& c)
  {
    for (int i = 0; i < c.size(); ++i)
    {
      if (c[i].init != flex::debug::obj::INIT_KEY)
      {
        printf("Error: Expected (c[%d] == flex::debug::obj::INIT_KEY), found (%d != %d)\n", i, c[i].init,
            flex::debug::obj::INIT_KEY);
        return false;
      }
      if (c[i].move_only && c[i].was_copied)
      {
        printf("Error: Expected (!(c[%d].move_only && c[%d].was_copied))", i, i);
        return false;
      }
    }
    for (int i = c.size(); i < c.capacity(); ++i)
    {
      if (c[i].init == flex::debug::obj::INIT_KEY)
      {
        printf("Error: Expected (c[%d] != flex::debug::obj::INIT_KEY), found (%d == %d)\n", i, c[i].init,
            flex::debug::obj::INIT_KEY);
        return false;
      }
    }
    return true;
  }

  void test_default_constructor(void)
  {
    /*
     * Case1: Verify default constructor works
     */
    ring_obj a;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 128);
  }

  void test_default_fill_constructor(void)
  {
    /*
     * Case1: Size exceeds capacity
     */
    TS_ASSERT(!errno);
    ring_obj a(256);
    TS_ASSERT(errno);
    errno = 0;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 256);

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case2: Verify fill constructor with primitive elements.
       */
      ring_obj b(SIZES[s]);
      TS_ASSERT(is_container_valid(b));
      TS_ASSERT_EQUALS(b.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(b.size(), b.capacity());
      for (int i = 0; i < b.size(); ++i)
      {
        TS_ASSERT_EQUALS(b[i], flex::debug::obj::DEFAULT_VAL);
      }

    } //for: SIZE_COUNT
  }

  void test_fill_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const flex::debug::obj fill_val = OBJ_DATA[SIZES[s] - 1];

      /*
       * Case1: Verify fill constructor assigns value parameter for primitives.
       */
      ring_obj a(SIZES[s], fill_val);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      TS_ASSERT_EQUALS(a.capacity(), 128);
      for (int i = 0; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], fill_val);
      }

    } //for: SIZE_COUNT
  }

  void test_range_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Verify range constructor with pointer parameters
       */
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      TS_ASSERT_EQUALS(a.capacity(), 128);
      for (int i = 0; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i]);
      }

      /*
       * Case2: Verify range constructor with iterator parameters
       */
      ring_obj b(a.begin(), a.end());
      TS_ASSERT(is_container_valid(b));
      TS_ASSERT_EQUALS(b.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(b.size(), b.capacity());
      TS_ASSERT_EQUALS(b.capacity(), 128);
      for (int i = 0; i < b.size(); ++i)
      {
        TS_ASSERT_EQUALS(b[i], OBJ_DATA[i]);
      }

    } //for: SIZE_COUNT
  }

  void test_copy_constructor(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case1: Verify copy constructor with primitive elements.
       */
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      ring_obj b(a);
      TS_ASSERT(is_container_valid(b));
      TS_ASSERT_EQUALS(b.size(), a.size());
      TS_ASSERT_LESS_THAN_EQUALS(b.size(), b.capacity());
      TS_ASSERT_EQUALS(b.capacity(), 128);
      for (int i = 0; i < b.size(); i++)
      {
        TS_ASSERT_EQUALS(b[i], a[i]);
      }

    } //for: SIZE_COUNT
  }

  void test_move_constructor()
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Normal condition.
     */
    ring_obj a =
    { 0, 1, 2, 3};
    clear_copy_flags(a);
    ring_obj b(std::move(a));
    mark_move_only(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(a.size(),0);
    TS_ASSERT_EQUALS(b.size(),4);
    TS_ASSERT_EQUALS(b[0],0);
    TS_ASSERT_EQUALS(b[1],1);
    TS_ASSERT_EQUALS(b[2],2);
    TS_ASSERT_EQUALS(b[3],3);
#endif
  }

  void test_initializer_constructor()
  {
    /*
     * Case1: Normal condition
     */
    ring_obj a( { 0, 1, 2, 3 });
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 4);
    TS_ASSERT_EQUALS(a[0], 0);
    TS_ASSERT_EQUALS(a[1], 1);
    TS_ASSERT_EQUALS(a[2], 2);
    TS_ASSERT_EQUALS(a[3], 3);
  }

  void test_assign_fill(void)
  {
    /*
     * Case1: Size exceeds capacity
     */
    ring_obj a;
    TS_ASSERT(!errno);
    a.assign(256, OBJ_DATA[0]);
    TS_ASSERT(errno);
    errno = 0;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 256);

    /*
     * Case2: Verify assign can increase size.
     */

    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const flex::debug::obj fill_val = OBJ_DATA[SIZES[s] - 1];
      a.assign(SIZES[s], fill_val);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      TS_ASSERT_EQUALS(a.capacity(), 256);
      for (int i = 0; i < a.size(); i++)
      {
        TS_ASSERT_EQUALS(a[i], fill_val);
      }
    }

    /*
     * Case2: Verify assign can decrease size.
     */
    for (int s = SIZE_COUNT - 1; s != -1; --s)
    {
      const flex::debug::obj fill_val = OBJ_DATA[SIZES[s] - 1];
      const size_t prev_capacity = a.capacity();
      a.assign(SIZES[s], fill_val);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      TS_ASSERT_EQUALS(a.capacity(), 256);
      for (int i = 0; i < a.size(); i++)
      {
        TS_ASSERT_EQUALS(a[i], fill_val);
      }
    }
  }

  void test_assign_iterator()
  {
    ring_obj a;

    /*
     * Case1: Verify assign can increase size.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      ring_obj tmp(OBJ_DATA, OBJ_DATA + SIZES[s]);
      a.assign(tmp.begin(), tmp.end());
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      TS_ASSERT_EQUALS(a.capacity(), 128);
      for (int i = 0; i < a.size(); i++)
      {
        TS_ASSERT_EQUALS(a[i], tmp[i]);
      }
    }

    /*
     * Case2: Verify assign can decrease size.
     */
    for (int s = SIZE_COUNT - 1; s != -1; --s)
    {
      ring_obj tmp(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t prev_capacity = a.capacity();
      a.assign(tmp.begin(), tmp.end());
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
      TS_ASSERT_EQUALS(a.capacity(), 128);
      for (int i = 0; i < a.size(); i++)
      {
        TS_ASSERT_EQUALS(a[i], tmp[i]);
      }
    }
  }

  void test_at(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      /*
       * Case1: Verify out_of_range error at both boundaries.
       */
      TS_ASSERT_THROWS(a.at(-1), std::out_of_range);
      TS_ASSERT_THROWS(a.at(SIZES[s]), std::out_of_range);

      /*
       * Case2: Verify at() read.
       */
      for (int i = 0; i < SIZES[s]; i++)
      {
        TS_ASSERT_EQUALS(a.at(i), OBJ_DATA[i]);
      }

      /*
       * Case3: Verify at() write.
       */
      for (int i = 0; i < SIZES[s]; i++)
      {
        const flex::debug::obj val = -i;
        a.at(i) = val;
        TS_ASSERT_EQUALS(a.at(i), val);
      }
    } //for: SIZE_COUNT
  }

  void test_at_const(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      const ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      /*
       * Case1: Verify out_of_range error at both boundaries.
       */
      TS_ASSERT_THROWS(a.at(-1), std::out_of_range);
      TS_ASSERT_THROWS(a.at(SIZES[s]), std::out_of_range);

      /*
       * Case2: Verify at() read.
       */
      for (int i = 0; i < SIZES[s]; i++)
      {
        TS_ASSERT_EQUALS(a.at(i), OBJ_DATA[i]);
      }

    } //for: SIZE_COUNT
  }

  void test_back(void)
  {
    //Start s at 1, as back() isn't supported on empty container.
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.back(), OBJ_DATA[SIZES[s] - 1]);
    } //for: SIZE_COUNT
  }

  void test_back_const(void)
  {
    //Start s at 1, as back() isn't supported on empty container.
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      const ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.back(), OBJ_DATA[SIZES[s] - 1]);
    } //for: SIZE_COUNT
  }

  void test_begin_and_end(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (ring_obj::iterator it = a.begin(); it != a.end(); ++it)
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
      const ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (ring_obj::const_iterator it = a.begin(); it != a.end(); ++it)
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
      const ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (ring_obj::const_iterator it = a.cbegin(); it != a.cend(); ++it)
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
      const ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (ring_obj::const_reverse_iterator it = a.crbegin(); it != a.crend(); ++it)
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
      ring_obj a(SIZES[s]);
      TS_ASSERT_EQUALS(a.capacity(), 128);
    } //for: SIZE_COUNT
  }

  void test_clear(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      a.clear();
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT(a.empty());
    } //for: SIZE_COUNT
  }

  void test_empty(void)
  {
    /*
     * Case 1: Verify empty on init.
     */
    ring_obj a;
    TS_ASSERT_EQUALS(a.empty(), true);

    /*
     * Case 2: Verify empty on modify
     */
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      a.assign(SIZES[s], 0);
      TS_ASSERT_EQUALS(a.empty(), false);
      a.clear();
      TS_ASSERT_EQUALS(a.empty(), true);
    } //for: SIZE_COUNT
  }

  void test_erase_position()
  {
    /*
     * Case 1: Test erase on size of 1.
     */
    ring_obj a(1);
    ring_obj::iterator it;
    it = a.erase(a.begin());
    TS_ASSERT(is_container_valid(a));
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
      it = a.erase(a.end() - 1);
      TS_ASSERT(is_container_valid(a));
      --current_size;
      TS_ASSERT_EQUALS(it, a.end());
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i]);
      }

      /*
       * Case3: Test erase at begin
       */
      it = a.erase(a.begin());
      TS_ASSERT(is_container_valid(a));
      --current_size;
      TS_ASSERT_EQUALS(it, a.begin());
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i + 1]);
      }

      /*
       * Case4: Test erase in middle
       */
      int mid_index = current_size / 2;
      it = a.erase(a.begin() + mid_index);
      TS_ASSERT(is_container_valid(a));
      --current_size;
      TS_ASSERT_EQUALS(it, a.begin() + mid_index);
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i + 1]);
      }
      for (int i = mid_index; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i + 2]);
      }
    } //for: SIZE_COUNT
  }

  void test_erase_range()
  {
    /*
     * Case 1: Test erase on size of 1.
     */
    ring_obj a(1);
    ring_obj::iterator it;
    it = a.erase(a.begin(), a.end());
    TS_ASSERT(is_container_valid(a));
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
      it = a.erase(a.end() - 2, a.end());
      TS_ASSERT(is_container_valid(a));
      current_size -= 2;
      TS_ASSERT_EQUALS(it, a.end());
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i]);
      }

      /*
       * Case3: Test erase at begin
       */
      it = a.erase(a.begin(), a.begin() + 2);
      TS_ASSERT(is_container_valid(a));
      current_size -= 2;
      TS_ASSERT_EQUALS(it, a.begin());
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i + 2]);
      }

      /*
       * Case4: Test erase in middle
       */
      int mid_index = current_size / 2;
      it = a.erase(a.begin() + mid_index, a.begin() + mid_index + 2);
      TS_ASSERT(is_container_valid(a));
      current_size -= 2;
      TS_ASSERT_EQUALS(it, a.begin() + mid_index);
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i + 2]);
      }
      for (int i = mid_index; i < a.size(); ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i + 4]);
      }
    } //for: SIZE_COUNT
  }

  void test_front(void)
  {
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.front(), OBJ_DATA[0]);
    } //for: SIZE_COUNT
  }
  void test_front_const(void)
  {
    for (unsigned s = 1; s < SIZE_COUNT; ++s)
    {
      const ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);
      TS_ASSERT_EQUALS(a.front(), OBJ_DATA[0]);
    } //for: SIZE_COUNT
  }

  void test_full(void)
  {
    /*
     * Case1: Normal conditions
     */
    ring_obj b(OBJ_DATA, OBJ_DATA + 128);
    TS_ASSERT(b.full());
    b.pop_back();
    TS_ASSERT(!b.full());
    b.push_front(0);
    TS_ASSERT(b.full());
  }

  void test_insert_position(void)
  {
    ring_obj a;
    ring_obj::iterator it;

    for (unsigned s = 0; s < SIZE_COUNT - 1; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const flex::debug::obj val = 19;

      /*
       * Case1: Test insert at end
       */
      it = a.insert(a.end(), val);
      TS_ASSERT(is_container_valid(a));
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 1], val);

      /*
       * Case2: Test insert at begin
       */
      it = a.insert(a.begin(), val);
      TS_ASSERT(is_container_valid(a));
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a[0], val);
      for (int i = 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 1]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 1], val);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      it = a.insert(a.begin() + mid_index, val);
      TS_ASSERT(is_container_valid(a));
      ++current_size;
      TS_ASSERT_EQUALS(*it, val);
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a[0], val);
      for (int i = 1; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 1]);
      }
      TS_ASSERT_EQUALS(a[mid_index], val);
      for (int i = mid_index + 1; i < a.size() - 1; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 1], val);
    } //for: SIZE_COUNT
  }

  void test_insert_fill(void)
  {
    ring_obj a;

    for (unsigned s = 0; s < SIZE_COUNT - 1; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      const flex::debug::obj val = 19;

      /*
       * Case1: Test insert at end
       */
      a.insert(a.end(), 2, val);
      TS_ASSERT(is_container_valid(a));
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 1], val);
      TS_ASSERT_EQUALS(a[a.size() - 2], val);

      /*
       * Case2: Test insert at begin
       */
      a.insert(a.begin(), 2, val);
      TS_ASSERT(is_container_valid(a));
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a[0], val);
      TS_ASSERT_EQUALS(a[1], val);
      for (int i = 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 1], val);
      TS_ASSERT_EQUALS(a[a.size() - 2], val);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      a.insert(a.begin() + mid_index, 2, val);
      TS_ASSERT(is_container_valid(a));
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a[0], val);
      TS_ASSERT_EQUALS(a[1], val);
      for (int i = 2; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(a[mid_index], val);
      TS_ASSERT_EQUALS(a[mid_index + 1], val);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 1], val);
      TS_ASSERT_EQUALS(a[a.size() - 2], val);
    } //for: SIZE_COUNT
  }

  void test_insert_iterators(void)
  {
    ring_obj a;
    ring_obj b(OBJ_DATA, OBJ_DATA + 2);

    //Start s at 3, as the below tests expect a size of at least 3.
    for (unsigned s = 0; s < SIZE_COUNT - 1; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();

      /*
       * Case1: Test insert at end
       */
      a.insert(a.end(), b.begin(), b.end());
      TS_ASSERT(is_container_valid(a));
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      for (int i = 0; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 2], OBJ_DATA[0]);
      TS_ASSERT_EQUALS(a[a.size() - 1], OBJ_DATA[1]);

      /*
       * Case2: Test insert at begin
       */
      a.insert(a.begin(), b.begin(), b.end());
      TS_ASSERT(is_container_valid(a));
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a[0], OBJ_DATA[0]);
      TS_ASSERT_EQUALS(a[1], OBJ_DATA[1]);
      for (int i = 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 2], OBJ_DATA[0]);
      TS_ASSERT_EQUALS(a[a.size() - 1], OBJ_DATA[1]);

      /*
       * Case3: Test insert in middle
       */
      int mid_index = current_size / 2;
      a.insert(a.begin() + mid_index, b.begin(), b.end());
      TS_ASSERT(is_container_valid(a));
      current_size += 2;
      TS_ASSERT_EQUALS(a.size(), current_size);
      TS_ASSERT_EQUALS(a[0], OBJ_DATA[0]);
      TS_ASSERT_EQUALS(a[1], OBJ_DATA[1]);
      for (int i = 2; i < mid_index; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 2]);
      }
      TS_ASSERT_EQUALS(a[mid_index], OBJ_DATA[0]);
      TS_ASSERT_EQUALS(a[mid_index + 1], OBJ_DATA[1]);
      for (int i = mid_index + 2; i < a.size() - 2; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i - 4]);
      }
      TS_ASSERT_EQUALS(a[a.size() - 2], OBJ_DATA[0]);
      TS_ASSERT_EQUALS(a[a.size() - 1], OBJ_DATA[1]);
    } //for: SIZE_COUNT
  }

  void test_max_size(void)
  {
    const ring_obj a;
    TS_ASSERT_EQUALS(a.max_size(), a.capacity());
  }

  void test_ary_operator(void)
  {
    unsigned size = 3;
    ring_obj a(size);
    for (int i = 0; i < size; i++)
    {
      a[i] = i;
      TS_ASSERT_EQUALS(a[i], i);
    }
  }

  void test_ary_operator_const(void)
  {
    size_t size = 3;
    const ring_obj a(size, 7);
    for (int i = 0; i < size; i++)
    {
      const int& v = a[i];
      TS_ASSERT_EQUALS(v, a[i]);
    }
  }

  void test_rbegin_and_rend(void)
  {
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (ring_obj::reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
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
      const ring_obj a(OBJ_DATA, OBJ_DATA + SIZES[s]);

      int i = 0;
      for (ring_obj::const_reverse_iterator it = a.rbegin(); it != a.rend(); ++it)
      {
        TS_ASSERT_EQUALS(*it, OBJ_DATA[i + a.size() - 1]);
        --i;
      }
      TS_ASSERT_EQUALS(i, -a.size());
    } //for: SIZE_COUNT
  }

  void test_reserve(void)
  {
    ring_obj a;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.reserve(SIZES[s]);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT_LESS_THAN_EQUALS(SIZES[s], a.capacity());
    }
  }

  void test_resize(void)
  {
    ring_obj a;
    const flex::debug::obj val = 19;
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      /*
       * Case 1: Verify resize can decrease size.
       */
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t new_size = SIZES[s] / 2;
      a.resize(new_size);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), new_size);
      TS_ASSERT(a == ring_obj(OBJ_DATA, OBJ_DATA + new_size));

      /*
       * Case 2: Verify resize can increase size.
       */
      a.resize(SIZES[s], val);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT_EQUALS(a.size(), SIZES[s]);
      for (int i = 0; i < new_size; ++i)
      {
        TS_ASSERT_EQUALS(a[i], OBJ_DATA[i]);
      }
      for (int i = new_size; i < SIZES[s]; ++i)
      {
        TS_ASSERT_EQUALS(a[i], val);
      }

    } //for: SIZE_COUNT
  }

  void test_size(void)
  {
    unsigned size = 3;
    const ring_obj a(3);
    TS_ASSERT_EQUALS(a.size(), size);
  }

  void test_swap(void)
  {
    ring_obj a;
    ring_obj b;

    /*
     * Case 1: Test on empty container
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);

      ring_obj tmp;
      a.swap(tmp);
      TS_ASSERT(is_container_valid(a));
      TS_ASSERT(is_container_valid(tmp));
      TS_ASSERT_EQUALS(a.size(), 0);
      TS_ASSERT_EQUALS(tmp.size(), SIZES[s]);
      TS_ASSERT(tmp == ring_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
    }

    /*
     * Case 2: Test on two populated containers
     */
    a.assign(OBJ_DATA, OBJ_DATA + 9);
    b.assign(OBJ_DATA + 9, OBJ_DATA + 16);

    a.swap(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(a.size(), 7);
    TS_ASSERT_EQUALS(b.size(), 9);
    TS_ASSERT(a == ring_obj(OBJ_DATA + 9, OBJ_DATA + 16));
    TS_ASSERT(b == ring_obj(OBJ_DATA, OBJ_DATA + 9));
  }

  void test_assignment_operator(void)
  {
    //Light-weight test, as this simply calls the assign() method.
    ring_obj a;
    ring_obj tmp(OBJ_DATA, OBJ_DATA + 8);
    a = tmp;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 8);
    TS_ASSERT_LESS_THAN_EQUALS(a.size(), a.capacity());
    TS_ASSERT_EQUALS(a.capacity(), 128);
    for (int i = 0; i < a.size(); i++)
    {
      TS_ASSERT_EQUALS(a[i], tmp[i]);
    }
  }

  void test_assignment_operator_move()
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Normal condition.
     */
    ring_obj a =
    { 0, 1, 2, 3};
    clear_copy_flags(a);
    ring_obj b;
    b = std::move(a);
    mark_move_only(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(a.size(),0);
    TS_ASSERT_EQUALS(b.size(),4);
    TS_ASSERT_EQUALS(b[0],0);
    TS_ASSERT_EQUALS(b[1],1);
    TS_ASSERT_EQUALS(b[2],2);
    TS_ASSERT_EQUALS(b[3],3);
#endif
  }

  void test_assignment_operator_move_base()
  {
#ifdef FLEX_HAS_CXX11
    /*
     * Case1: Normal condition.
     */
    allocation_guard::disable();
    flex::ring<flex::debug::obj,flex::debug::allocator<flex::debug::obj> > a =
    { 0, 1, 2, 3};
    allocation_guard::enable();
    clear_copy_flags(a);
    ring_obj b;
    b = std::move(a);
    mark_move_only(b);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT(is_container_valid(b));
    TS_ASSERT_EQUALS(a.size(),0);
    TS_ASSERT_EQUALS(b.size(),4);
    TS_ASSERT_EQUALS(b[0],0);
    TS_ASSERT_EQUALS(b[1],1);
    TS_ASSERT_EQUALS(b[2],2);
    TS_ASSERT_EQUALS(b[3],3);
#endif
  }

  void test_pop_back(void)
  {
    ring_obj a;

    /*
     * Case 1: Container doesn't wrap
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[current_size - 1]);
        a.pop_back();
        TS_ASSERT(is_container_valid(a));
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
    }

    /*
     * Case 2: Container wraps
     */
    for (unsigned s = 2; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA + 2, OBJ_DATA + SIZES[s]);
      a.push_front(OBJ_DATA[1]);
      a.push_front(OBJ_DATA[0]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[current_size - 1]);
        a.pop_back();
        TS_ASSERT(is_container_valid(a));
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
    }
  }

  void test_pop_front(void)
  {
    ring_obj a;

    /*
     * Case 1: Container doesn't wrap
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA, OBJ_DATA + SIZES[s]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.front(), OBJ_DATA[SIZES[s] - current_size]);
        a.pop_front();
        TS_ASSERT(is_container_valid(a));
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
    }

    /*
     * Case 2: Container wraps
     */
    for (unsigned s = 2; s < SIZE_COUNT; ++s)
    {
      a.assign(OBJ_DATA + 2, OBJ_DATA + SIZES[s]);
      a.push_front(OBJ_DATA[1]);
      a.push_front(OBJ_DATA[0]);
      size_t current_size = a.size();
      while (!a.empty())
      {
        TS_ASSERT_EQUALS(a.front(), OBJ_DATA[SIZES[s] - current_size]);
        a.pop_front();
        TS_ASSERT(is_container_valid(a));
        --current_size;
        TS_ASSERT_EQUALS(a.size(), current_size);
      }
    }
  }

  void test_push_back(void)
  {
    ring_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.clear();
      for (int i = 0; i < SIZES[s]; ++i)
      {
        a.push_back(OBJ_DATA[i]);
        TS_ASSERT(is_container_valid(a));
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[i]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == ring_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
    }

    /*
     * Case 2: Container wraps.
     */
    for (unsigned s = 2; s < SIZE_COUNT; ++s)
    {
      a.clear();
      a.push_front(OBJ_DATA[1]);
      a.push_front(OBJ_DATA[0]);
      for (int i = 2; i < SIZES[s]; ++i)
      {
        a.push_back(OBJ_DATA[i]);
        TS_ASSERT(is_container_valid(a));
        TS_ASSERT_EQUALS(a.back(), OBJ_DATA[i]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == ring_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
    }
  }

  void test_push_front(void)
  {
    ring_obj a;

    /*
     * Case 1: Normal condition.
     */
    for (unsigned s = 0; s < SIZE_COUNT; ++s)
    {
      a.clear();
      for (int i = 0; i < SIZES[s]; ++i)
      {
        const unsigned data_index = SIZES[s] - 1 - i;
        a.push_front(OBJ_DATA[data_index]);
        TS_ASSERT(is_container_valid(a));
        TS_ASSERT_EQUALS(a.front(), OBJ_DATA[data_index]);
        TS_ASSERT_EQUALS(a.size(), i + 1);
      }
      TS_ASSERT(a == ring_obj(OBJ_DATA, OBJ_DATA + SIZES[s]));
    }
  }

  void test_equality_operator(void)
  {
    /*
     * Case1: Test size of 0.
     */
    ring_obj a;
    ring_obj b;
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
      a.clear();
      TS_ASSERT(!(a == b));
    }
  }

  void test_less_than_operator(void)
  {
    /*
     * Case1: Test size of 0.
     */
    ring_obj a;
    ring_obj b;
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

      b[0] = a[0] - 1;
      TS_ASSERT(b < a);
      TS_ASSERT(!(a < b));

      /*
       * Case3: Test on empty containers
       */
      b.clear();
      TS_ASSERT(b < a);
      TS_ASSERT(!(a < b));
    }
  }

  void test_relational_operators(void)
  {
    int aAry[5] = { 10, 20, 30, 40, 50 };
    int bAry[5] = { 10, 20, 30, 40, 50 };
    int cAry[5] = { 50, 40, 30, 20, 10 };
    ring_obj a(5);
    ring_obj b(5);
    ring_obj c(5);
    for (int i = 0; i < 5; ++i)
    {
      a[i] = b[i] = (i * 10) + 10;
      c[i] = 50 - (i * 10);
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
