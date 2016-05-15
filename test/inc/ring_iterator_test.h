#include <cxxtest/TestSuite.h>

#include <flex/internal/ring_iterator.h>

using namespace flex;

class ring_iterator_test : public CxxTest::TestSuite
{
   int INT_DATA[128] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 39535304, 2113617954, -262399995, -1776526244,
      2007130159, -751355444, -1850306681, 1670328314, 174975647, 1520325186, 752193990, 1141698902, 414986917,
      -1084506988, -1274438196, -407784340, -1476797751, 952482371, 1659351065, -1840296979, 1174260466, -830555035,
      1187249412, -1439716735, -606656096, 1968778085, -468774603, -741213671, -1792595459, -1043591241, -399781674,
      1441797965, -539577554, -1712941906, 893437261, 1243708130, -276655685, 169167272, 1548266128, 2134938409,
      -165983522, 65335344, 777222631, -1975346548, 1736737965, -1297235370, -1778585082, -445115751, 77287795,
      -904742465, 1566979049, -1276550055, -1523151595, -1877472326, -1965521838, 309774311, 285638537, 1694499811,
      395062486, -599472639, -562348494, 622523556, 1991792880, 1485225099, -26143183, 1213635789, -1867261885,
      1401932595, 1643956672, 1152265615, -206296253, -1341812088, -928119996, 1335888378, -2127839732, -805081880,
      -461979923, 258594093, 1322814281, -1856950276, 763906168, -110775798, 29138078, -728231554, -1738124420,
      -1130024844, 2112808498, -2147190929, -46681067, -1746560845, -1931350352, -2121713887, -2077836858, -68560373,
      542144249, -964249373, 672765407, 1240222082, -170251308, 573136605, 522427348, -1842488270, -803442179, 1214800559,
      -439290856, -850489475, -371113959, -528653948, -1466750983, -299654597, -1095361209, 912904732};

   struct object
   {
      static const int INIT_VAL = 1;

      object() : val(INIT_VAL)
      {
      }

      object(int i) :
      val(i)
      {
      }

      int val;
   };

public:

   void test_default_constructor(void)
   {
      ring_iterator<int> a;
      TS_ASSERT(a.mPtr == NULL);
      TS_ASSERT(a.mLeftBound == NULL);
      TS_ASSERT(a.mRightBound == NULL);
   }

   void test_assignment_constructor(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);
      TS_ASSERT_EQUALS(a.mPtr, INT_DATA + 2);
      TS_ASSERT_EQUALS(a.mLeftBound, INT_DATA);
      TS_ASSERT_EQUALS(a.mRightBound, INT_DATA + 8);
   }

   void test_equality_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);
      ring_iterator<int, const int*, const int&> b(a);
      a==b;
      //TS_ASSERT(a == b);
      //a.mPtr += 1;
      //TS_ASSERT(a != b);
   }

   void test_dereference_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);
      TS_ASSERT_EQUALS(*a, INT_DATA[2]);
      TS_ASSERT_EQUALS(a[0], INT_DATA[2]);
      TS_ASSERT_EQUALS(a[1], INT_DATA[3]);
      TS_ASSERT_EQUALS(a[2], INT_DATA[4]);
      TS_ASSERT_EQUALS(a[3], INT_DATA[5]);
      TS_ASSERT_EQUALS(a[4], INT_DATA[6]);
      TS_ASSERT_EQUALS(a[5], INT_DATA[7]);
      TS_ASSERT_EQUALS(a[6], INT_DATA[8]);
      TS_ASSERT_EQUALS(a[7], INT_DATA[0]);
      TS_ASSERT_EQUALS(a[8], INT_DATA[1]);

      const int obj_val = 7;
      object obj(obj_val);
      ring_iterator<object> b(&obj, &obj, &obj);
      TS_ASSERT_EQUALS(b->val, obj_val);
      TS_ASSERT_EQUALS(b[0].val, obj_val);
   }

   void test_increment_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);

      TS_ASSERT_EQUALS(*(++a), INT_DATA[3]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[4]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[5]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[6]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[7]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[8]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[1]);
      TS_ASSERT_EQUALS(*(++a), INT_DATA[2]);

      TS_ASSERT_EQUALS(*(a++), INT_DATA[2]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[3]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[4]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[5]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[6]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[7]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[8]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(a++), INT_DATA[1]);
      TS_ASSERT_EQUALS(*a, INT_DATA[2]);
   }

   void test_decrement_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);

      TS_ASSERT_EQUALS(*(--a), INT_DATA[1]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[8]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[7]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[6]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[5]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[4]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[3]);
      TS_ASSERT_EQUALS(*(--a), INT_DATA[2]);

      TS_ASSERT_EQUALS(*(a--), INT_DATA[2]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[1]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[8]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[7]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[6]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[5]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[4]);
      TS_ASSERT_EQUALS(*(a--), INT_DATA[3]);
      TS_ASSERT_EQUALS(*a, INT_DATA[2]);
   }

   void test_plus_assign_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);

      TS_ASSERT_EQUALS(*(a += 1), INT_DATA[3]);
      TS_ASSERT_EQUALS(*(a += 2), INT_DATA[5]);
      TS_ASSERT_EQUALS(*(a += 2), INT_DATA[7]);
      TS_ASSERT_EQUALS(*(a += 2), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(a += 2), INT_DATA[2]);
   }

   void test_plus_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);

      TS_ASSERT_EQUALS(*(a + 1), INT_DATA[3]);
      TS_ASSERT_EQUALS(*(a + 3), INT_DATA[5]);
      TS_ASSERT_EQUALS(*(a + 5), INT_DATA[7]);
      TS_ASSERT_EQUALS(*(a + 7), INT_DATA[0]);
      TS_ASSERT_EQUALS(*(a + 9), INT_DATA[2]);
   }

   void test_minus_assign_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);

      TS_ASSERT_EQUALS(*(a -= 1), INT_DATA[1]);
      TS_ASSERT_EQUALS(*(a -= 2), INT_DATA[8]);
      TS_ASSERT_EQUALS(*(a -= 2), INT_DATA[6]);
      TS_ASSERT_EQUALS(*(a -= 2), INT_DATA[4]);
      TS_ASSERT_EQUALS(*(a -= 2), INT_DATA[2]);
   }

   void test_minus_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);

      TS_ASSERT_EQUALS(*(a - 1), INT_DATA[1]);
      TS_ASSERT_EQUALS(*(a - 3), INT_DATA[8]);
      TS_ASSERT_EQUALS(*(a - 5), INT_DATA[6]);
      TS_ASSERT_EQUALS(*(a - 7), INT_DATA[4]);
      TS_ASSERT_EQUALS(*(a - 9), INT_DATA[2]);
   }

   void test_minus_iterator_operator(void)
   {
      ring_iterator<int> a(INT_DATA + 2, INT_DATA, INT_DATA + 8);

      TS_ASSERT_EQUALS(0, a - ring_iterator<int>(INT_DATA + 2, NULL, NULL));
      TS_ASSERT_EQUALS(8, a - ring_iterator<int>(INT_DATA + 3, NULL, NULL));
      TS_ASSERT_EQUALS(7, a - ring_iterator<int>(INT_DATA + 4, NULL, NULL));
      TS_ASSERT_EQUALS(6, a - ring_iterator<int>(INT_DATA + 5, NULL, NULL));
      TS_ASSERT_EQUALS(5, a - ring_iterator<int>(INT_DATA + 6, NULL, NULL));
      TS_ASSERT_EQUALS(4, a - ring_iterator<int>(INT_DATA + 7, NULL, NULL));
      TS_ASSERT_EQUALS(3, a - ring_iterator<int>(INT_DATA + 8, NULL, NULL));
      TS_ASSERT_EQUALS(2, a - ring_iterator<int>(INT_DATA + 0, NULL, NULL));
      TS_ASSERT_EQUALS(1, a - ring_iterator<int>(INT_DATA + 1, NULL, NULL));
   }

}
;
