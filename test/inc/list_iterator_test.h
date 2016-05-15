#include <cxxtest/TestSuite.h>

#include <flex/internal/list_iterator.h>

using namespace flex;

class list_iterator_test: public CxxTest::TestSuite
{
  int INT_DATA[128] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 39535304, 2113617954, -262399995,
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

  list_node<int> INT_NODES[128];
  list_node<object> OBJ_NODES[128];

public:

  void setUp()
  {
    /*
     * Construct nodes which contain a raw integer from the
     * information contained in INT_DATA.
     */
    INT_NODES[0].mValue = INT_DATA[0];
    INT_NODES[0].mNext = INT_NODES + 1;
    INT_NODES[0].mPrev = NULL;
    for (size_t i = 1; i < 127; ++i)
    {
      INT_NODES[i].mValue = INT_DATA[i];
      INT_NODES[i].mNext = INT_NODES + i + 1;
      INT_NODES[i].mPrev = INT_NODES + i - 1;
    }
    INT_NODES[127].mValue = INT_DATA[127];
    INT_NODES[127].mNext = NULL;
    INT_NODES[127].mPrev = INT_NODES + 126;

    /*
     * Construct nodes which contain an object type from the
     * information contained in INT_DATA.
     */
    OBJ_NODES[0].mValue.val = INT_DATA[0];
    OBJ_NODES[0].mNext = OBJ_NODES + 1;
    OBJ_NODES[0].mPrev = NULL;
    for (size_t i = 1; i < 127; ++i)
    {
      OBJ_NODES[i].mValue.val = INT_DATA[i];
      OBJ_NODES[i].mNext = OBJ_NODES + i + 1;
      OBJ_NODES[i].mPrev = OBJ_NODES + i - 1;
    }
    OBJ_NODES[127].mValue.val = INT_DATA[127];
    OBJ_NODES[127].mNext = NULL;
    OBJ_NODES[127].mPrev = OBJ_NODES + 126;
  }

  void tearDown()
  {
  }

  void test_default_constructor(void)
  {
    list_iterator<int> a;
    TS_ASSERT(a.mNode == NULL);
  }

  void test_assignment_constructor(void)
  {
    list_iterator<int> a(&INT_NODES[1]);
    TS_ASSERT_EQUALS(a.mNode, &INT_NODES[1]);
    TS_ASSERT_EQUALS(a.mNode->mPrev, &INT_NODES[0]);
    TS_ASSERT_EQUALS(a.mNode->mNext, &INT_NODES[2]);
  }

  void test_equality_operator(void)
  {
    list_iterator<int> a(&INT_NODES[1]);
    list_iterator<int, const int*, const int&> b(a);
    TS_ASSERT(a == b);
    ++a;
    TS_ASSERT(a != b);
  }

  void test_dereference_operator(void)
  {
    for (int i = 0; i < 128; ++i)
    {
      list_iterator<int> a(&INT_NODES[i]);
      list_iterator<object> b(&OBJ_NODES[i]);
      TS_ASSERT_EQUALS(*a, INT_DATA[i]);
      TS_ASSERT_EQUALS(b->val, INT_DATA[i]);
      TS_ASSERT_EQUALS((*b).val, INT_DATA[i]);
    }
  }

  void test_increment_operator(void)
  {
    list_iterator<int> a(INT_NODES);
    list_iterator<object> b(OBJ_NODES);
    for (int i = 0; i < 64; ++i)
    {

      TS_ASSERT_EQUALS(*(a++), INT_DATA[i]);
      TS_ASSERT_EQUALS(b->val, INT_DATA[i]);
      TS_ASSERT_EQUALS((*(b++)).val, INT_DATA[i]);
    }

    TS_ASSERT_EQUALS(*a, INT_DATA[64]);
    TS_ASSERT_EQUALS(b->val, INT_DATA[64]);
    TS_ASSERT_EQUALS((*b).val, INT_DATA[64]);

    for (int i = 65; i < 128; ++i)
    {
      TS_ASSERT_EQUALS(*(++a), INT_DATA[i]);
      TS_ASSERT_EQUALS((++b)->val, INT_DATA[i]);
      TS_ASSERT_EQUALS((*b).val, INT_DATA[i]);
    }
  }

  void test_decrement_operator(void)
  {
    list_iterator<int> a(&INT_NODES[127]);
    list_iterator<object> b(&OBJ_NODES[127]);
    for (int i = 127; i > 64; --i)
    {

      TS_ASSERT_EQUALS(*(a--), INT_DATA[i]);
      TS_ASSERT_EQUALS(b->val, INT_DATA[i]);
      TS_ASSERT_EQUALS((*(b--)).val, INT_DATA[i]);
    }

    TS_ASSERT_EQUALS(*a, INT_DATA[64]);
    TS_ASSERT_EQUALS(b->val, INT_DATA[64]);
    TS_ASSERT_EQUALS((*b).val, INT_DATA[64]);

    for (int i = 63; i > -1; --i)
    {
      TS_ASSERT_EQUALS(*(--a), INT_DATA[i]);
      TS_ASSERT_EQUALS((--b)->val, INT_DATA[i]);
      TS_ASSERT_EQUALS((*b).val, INT_DATA[i]);
    }
  }
}
;
