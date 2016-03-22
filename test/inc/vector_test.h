#include <cxxtest/TestSuite.h>

#include <flex/vector.h>
#include <flex/allocator_debug.h>

class vector_test: public CxxTest::TestSuite
{

  struct object
  {
    static const int DEFAULT_VAL = 1;
    static const int INIT_KEY = 858599509;

    object() :
        val(DEFAULT_VAL), init(INIT_KEY)
    {
    }

    object(int i) :
        val(i), init(INIT_KEY)
    {
    }

    ~object()
    {
      val = init = 0;
    }

    object& operator=(const object& o)
    {
      val = o.val;
      return *this;
    }

    operator int() const
    {
      return val;
    }

    int val;
    int init;
  };

  typedef flex::vector<object/*, flex::allocator_debug<object>*/> vec;

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

public:

  void setUp()
  {
    flex::allocator_debug<int>::clear();
  }

  void tearDown()
  {
    //This ensures that all objects constructed by the container have their destructors called.
    TS_ASSERT(flex::allocator_debug<int>::mConstructedPointers.empty());

    //This ensures that all memory allocated by the container is properly freed.
    TS_ASSERT(flex::allocator_debug<int>::mAllocatedPointers.empty());
  }

  bool is_container_valid(const vec& v)
  {
    for (int i = 0; i < v.size(); ++i)
    {
      if (v[i].init != object::INIT_KEY)
      {
        printf("NOT INIT\n");
        return false;
      }
    }
    for (int i = v.size(); i < v.capacity(); ++i)
    {
      if (v[i].init == object::INIT_KEY)
      {
        printf("UNINIT = INIT\n");
        return false;
      }
    }
    return true;
  }

  void test_assign(void)
  {
    vec first;
    vec second;
    vec third;

    //TS_ASSERT_THROWS(first.assign(8, 100), std::runtime_error);
    first.assign(7, object(100));             // 7 ints with a value of 100
    TS_ASSERT(is_container_valid(first));
    vec::iterator it;
    it = first.begin() + 1;

    //TS_ASSERT_THROWS(second.assign(it, first.end()), std::runtime_error);
    second.assign(it, first.end() - 1); // the 5 central values of first
    TS_ASSERT(is_container_valid(second));

    int myints[] = { 1776, 7, 4 };
    third.assign(myints, myints + 3);   // assigning from vector.
    TS_ASSERT(is_container_valid(third));

    TS_ASSERT_EQUALS(first.size(), 7);
    TS_ASSERT_EQUALS(first[0], 100);
    TS_ASSERT_EQUALS(first[1], 100);
    TS_ASSERT_EQUALS(first[2], 100);
    TS_ASSERT_EQUALS(first[3], 100);
    TS_ASSERT_EQUALS(first[4], 100);
    TS_ASSERT_EQUALS(first[5], 100);
    TS_ASSERT_EQUALS(first[6], 100);

    TS_ASSERT_EQUALS(second.size(), 5);
    TS_ASSERT_EQUALS(second[0], 100);
    TS_ASSERT_EQUALS(second[1], 100);
    TS_ASSERT_EQUALS(second[2], 100);
    TS_ASSERT_EQUALS(second[3], 100);
    TS_ASSERT_EQUALS(second[4], 100);

    TS_ASSERT_EQUALS(third.size(), 3);
    TS_ASSERT_EQUALS(third[0], 1776);
    TS_ASSERT_EQUALS(third[1], 7);
    TS_ASSERT_EQUALS(third[2], 4);
  }

  void test_at(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 0, 0 };
    vector<int> a(myints, myints + size);
    TS_ASSERT_THROWS(a.at(-1), std::out_of_range);
    for (int i = 0; i < size; i++)
    {
      a.at(i) = i;
      TS_ASSERT_EQUALS(a.at(i), a[i]);
    }
    TS_ASSERT_THROWS(a.at(size), std::out_of_range);
  }

  void test_at_const(void)
  {
    size_t size = 3;
    const vector<int> a(size, 7);
    TS_ASSERT_THROWS(const int& v = a.at(-1), std::out_of_range);
    for (int i = 0; i < size; i++)
    {
      const int& v = a.at(i);
      TS_ASSERT_EQUALS(v, 7);
    }
    TS_ASSERT_THROWS(const int& v = a.at(size), std::out_of_range);
  }

  void test_back(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    vector<int> a(myints, myints + size);
    a[size - 1] = size - 1;
    TS_ASSERT_EQUALS(a.back(), size - 1);
  }

  void test_back_const(void)
  {
    size_t size = 3;
    const vector<int> a(size, 7);
    TS_ASSERT_EQUALS(a.back(), 7);
  }

  void test_begin_and_end(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    vector<int> a(myints, myints + size);
    for (int i = 0; i < size; i++)
    {
      a[i] = i;
    }

    int i = 0;
    for (vector<int>::iterator it = a.begin(); it < a.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      *it = 0; //Ensure it is not const.
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_begin_and_end_const(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    const vector<int> a(myints, myints + size);

    int i = 0;
    for (vector<int>::const_iterator it = a.begin(); it != a.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_cbegin_and_cend(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    const vector<int> a(myints, myints + size);

    int i = 0;
    for (vector<int>::const_iterator it = a.cbegin(); it < a.cend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      ++i;
    }
    TS_ASSERT_EQUALS(i, size);
  }

  void test_crbegin_and_crend(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    const vector<int> a(myints, myints + size);

    int i = size - 1;
    for (vector<int>::const_reverse_iterator it = a.crbegin(); it < a.crend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_capacity(void)
  {
    vec a;
    TS_ASSERT_EQUALS(a.capacity(), 0);
  }

  void test_clear(void)
  {
    vec myvector;
    myvector.push_back(100);
    myvector.push_back(200);
    myvector.push_back(300);
    TS_ASSERT_EQUALS(myvector.size(), 3);
    myvector.clear();
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector.size(), 0);
    myvector.push_back(1101);
    myvector.push_back(2202);
    TS_ASSERT_EQUALS(myvector.size(), 2);
    TS_ASSERT_EQUALS(myvector[0], 1101);
    TS_ASSERT_EQUALS(myvector[1], 2202);
  }

  void test_empty(void)
  {
    vec myvector;
    int sum(0);

    TS_ASSERT_EQUALS(myvector.empty(), true);
    for (int i = 1; i <= 10; i++)
      myvector.push_back(i);

    TS_ASSERT_EQUALS(myvector.empty(), false);
    while (!myvector.empty())
    {
      sum += myvector.back();
      myvector.pop_back();
    }

    TS_ASSERT_EQUALS(myvector.empty(), true);
    TS_ASSERT_EQUALS(sum, 55);
  }

  void test_erase()
  {
    vec myvector;

    // set some values (from 1 to 10)
    for (int i = 1; i <= 10; i++)
      myvector.push_back(i);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector.size(), 10);

    // erase the 6th element
    vec::iterator it = myvector.erase(myvector.begin() + 5);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(*it, 7);
    TS_ASSERT_EQUALS(myvector.size(), 9);

    // erase the first 3 elements:
    it = myvector.erase(myvector.begin(), myvector.begin() + 3);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(*it, 4);
    TS_ASSERT_EQUALS(myvector.size(), 6);

    TS_ASSERT_EQUALS(myvector[0], 4);
    TS_ASSERT_EQUALS(myvector[1], 5);
    TS_ASSERT_EQUALS(myvector[2], 7);
    TS_ASSERT_EQUALS(myvector[3], 8);
    TS_ASSERT_EQUALS(myvector[4], 9);
    TS_ASSERT_EQUALS(myvector[5], 10);
  }

  void test_front(void)
  {
    const size_t size = 3;
    int myints[size] = { 2, 1, 0 };
    vector<int> a(myints, myints + size);
    TS_ASSERT_EQUALS(a.front(), 2);
  }

  void test_front_const(void)
  {
    const size_t size = 3;
    int myints[size] = { 2, 1, 0 };
    const vector<int> a(myints, myints + size);
    TS_ASSERT_EQUALS(a.front(), 2);
  }

  void test_insert(void)
  {
    vec myvector(3, object(100));
    vec::iterator it;

    it = myvector.begin();
    it = myvector.insert(it, 200);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector.size(), 4);
    TS_ASSERT_EQUALS(myvector[0], 200);
    TS_ASSERT_EQUALS(myvector[1], 100);
    TS_ASSERT_EQUALS(myvector[2], 100);
    TS_ASSERT_EQUALS(myvector[3], 100);

    myvector.insert(it, 2, object(300));
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector.size(), 6);
    TS_ASSERT_EQUALS(myvector[0], 300);
    TS_ASSERT_EQUALS(myvector[1], 300);
    TS_ASSERT_EQUALS(myvector[2], 200);
    TS_ASSERT_EQUALS(myvector[3], 100);
    TS_ASSERT_EQUALS(myvector[4], 100);
    TS_ASSERT_EQUALS(myvector[5], 100);

    // "it" no longer valid, get a new one:
    it = myvector.begin();
    vec anothervector(2, object(400));
    myvector.insert(it + 2, anothervector.begin(), anothervector.end());
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector.size(), 8);
    TS_ASSERT_EQUALS(myvector[0], 300);
    TS_ASSERT_EQUALS(myvector[1], 300);
    TS_ASSERT_EQUALS(myvector[2], 400);
    TS_ASSERT_EQUALS(myvector[3], 400);
    TS_ASSERT_EQUALS(myvector[4], 200);
    TS_ASSERT_EQUALS(myvector[5], 100);
    TS_ASSERT_EQUALS(myvector[6], 100);
    TS_ASSERT_EQUALS(myvector[7], 100);

    int myints[] = { 501, 502, 503, 12, 13, 14, 15, 16, 17 };
    myvector.insert(myvector.begin(), myints, myints + 3);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector.size(), 11);
    TS_ASSERT_EQUALS(myvector[0], 501);
    TS_ASSERT_EQUALS(myvector[1], 502);
    TS_ASSERT_EQUALS(myvector[2], 503);
    TS_ASSERT_EQUALS(myvector[3], 300);
    TS_ASSERT_EQUALS(myvector[4], 300);
    TS_ASSERT_EQUALS(myvector[5], 400);
    TS_ASSERT_EQUALS(myvector[6], 400);
    TS_ASSERT_EQUALS(myvector[7], 200);
    TS_ASSERT_EQUALS(myvector[8], 100);
    TS_ASSERT_EQUALS(myvector[9], 100);
    TS_ASSERT_EQUALS(myvector[10], 100);
  }

  void test_max_size(void)
  {
    vec a;
    TS_ASSERT_EQUALS(a.max_size(), a.get_allocator().max_size());
  }

  void test_assignment_operator(void)
  {
    vec foo(3);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    vec bar(5, object(2));

    bar = foo;
    TS_ASSERT(is_container_valid(bar));
    TS_ASSERT_EQUALS(bar.size(), 3);
    TS_ASSERT_EQUALS(bar[0], 1);
    TS_ASSERT_EQUALS(bar[1], 5);
    TS_ASSERT_EQUALS(bar[2], 17);

    //Ensure assignments on bar doens't impact foo.
    bar.assign(0, object(3));
    TS_ASSERT(is_container_valid(bar));
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //We are setting foo to an empty vector.  Size is zero, but the capacity is still eight.  Behavior-wise
    //it doesn't matter what the contents are since the size is zero.  However, we want to ensure that the
    //assignment operator doesn't perform extra work by resetting these values.  This happens if no assignment
    //operator is defined and a default one is used.
    foo = vec();
    TS_ASSERT(is_container_valid(foo));
    TS_ASSERT_EQUALS(foo.size(), 0);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //Same thing as above, but we want to ensure that the cast operator allows assignment of vectors
    //with different capacities.
    foo = vec(1, object(19));
    TS_ASSERT(is_container_valid(foo));
    TS_ASSERT_EQUALS(foo.size(), 1);
    TS_ASSERT_EQUALS(foo[0], 19);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    foo.assign(3, object(0));
    TS_ASSERT(is_container_valid(foo));
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    vec bar2(5, object(2));
    bar2 = foo;
    TS_ASSERT(is_container_valid(bar2));
    TS_ASSERT_EQUALS(bar2.size(), 3);
    TS_ASSERT_EQUALS(bar2[0], 1);
    TS_ASSERT_EQUALS(bar2[1], 5);
    TS_ASSERT_EQUALS(bar2[2], 17);

    //Ensure assignments on bar doens't impact foo.
    bar2.assign(0, object(3));
    TS_ASSERT(is_container_valid(bar2));
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);
  }

  void test_ary_operator(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    vector<int> a(myints, myints + size);
    for (int i = 0; i < size; i++)
    {
      TS_ASSERT_EQUALS(i, a[i]);
    }
  }

  void test_ary_operator_const(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    const vector<int> a(myints, myints + size);

    for (int i = 0; i < size; i++)
    {
      const int& v = a[i];
      TS_ASSERT_EQUALS(v, a[i]);
    }
  }

  void test_push_back_and_pop_back(void)
  {
    vec myvector;
    int size = 0;
    TS_ASSERT_EQUALS(myvector.size(), size);
    TS_ASSERT(is_container_valid(myvector));
    int sum(0);
    myvector.push_back(100);
    TS_ASSERT(is_container_valid(myvector));
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);
    myvector.push_back(200);
    TS_ASSERT(is_container_valid(myvector));
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);
    myvector.push_back(300);
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);

    for (int i=0;i<4;++i)
    {
      printf("%d %d\n",myvector[i].val,myvector[i].init);
    }
    TS_ASSERT(is_container_valid(myvector));

    while (!myvector.empty())
    {
      sum += myvector.back();
      myvector.pop_back();
      TS_ASSERT(is_container_valid(myvector));
      --size;
      TS_ASSERT_EQUALS(myvector.size(), size);
    }
    TS_ASSERT_EQUALS(sum, 600);
  }

  void test_rbegin_and_rend(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    vector<int> a(myints, myints + size);

    int i = size - 1;
    for (vector<int>::reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      *it = 0; //Ensure it is not const.
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_rbegin_and_rend_const(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    const vector<int> a(myints, myints + size);

    int i = size - 1;
    for (vector<int>::const_reverse_iterator it = a.rbegin(); it < a.rend(); ++it)
    {
      TS_ASSERT_EQUALS(*it, i);
      --i;
    }
    TS_ASSERT_EQUALS(i, -1);
  }

  void test_reserve(void)
  {
    vec bar;
    bar.reserve(128);
    TS_ASSERT(is_container_valid(bar));
    TS_ASSERT_EQUALS(bar.size(), 0);
    TS_ASSERT_LESS_THAN_EQUALS(128, bar.capacity());
  }

  void test_resize(void)
  {
    vec myvector;

    for (int i = 1; i < 10; i++)
      myvector.push_back(i);

    myvector.resize(5);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector[0], 1);
    TS_ASSERT_EQUALS(myvector[1], 2);
    TS_ASSERT_EQUALS(myvector[2], 3);
    TS_ASSERT_EQUALS(myvector[3], 4);
    TS_ASSERT_EQUALS(myvector[4], 5);

    myvector.resize(8, 100);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector[0], 1);
    TS_ASSERT_EQUALS(myvector[1], 2);
    TS_ASSERT_EQUALS(myvector[2], 3);
    TS_ASSERT_EQUALS(myvector[3], 4);
    TS_ASSERT_EQUALS(myvector[4], 5);
    TS_ASSERT_EQUALS(myvector[5], 100);
    TS_ASSERT_EQUALS(myvector[6], 100);
    TS_ASSERT_EQUALS(myvector[7], 100);

    myvector.resize(12);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector[0], 1);
    TS_ASSERT_EQUALS(myvector[1], 2);
    TS_ASSERT_EQUALS(myvector[2], 3);
    TS_ASSERT_EQUALS(myvector[3], 4);
    TS_ASSERT_EQUALS(myvector[4], 5);
    TS_ASSERT_EQUALS(myvector[5], 100);
    TS_ASSERT_EQUALS(myvector[6], 100);
    TS_ASSERT_EQUALS(myvector[7], 100);
    TS_ASSERT_EQUALS(myvector[8], object::DEFAULT_VAL);
    TS_ASSERT_EQUALS(myvector[9], object::DEFAULT_VAL);
    TS_ASSERT_EQUALS(myvector[10], object::DEFAULT_VAL);
    TS_ASSERT_EQUALS(myvector[11], object::DEFAULT_VAL);
  }

  void test_size(void)
  {
    const size_t size = 3;
    int myints[size] = { 0, 1, 2 };
    vector<int> a(myints, myints + size);
    TS_ASSERT_EQUALS(a.size(), size);
  }

  void test_swap(void)
  {
    vec foo(3, object(100));   // three ints with a value of 100
    vec bar(5, object(200));   // five ints with a value of 200

    foo.swap(bar);
    TS_ASSERT(is_container_valid(foo));
    TS_ASSERT(is_container_valid(bar));

    TS_ASSERT_EQUALS(foo.size(), 5);
    for (int i = 0; i < foo.size(); ++i)
    {
      TS_ASSERT_EQUALS(foo[i], 200);
    }

    TS_ASSERT_EQUALS(bar.size(), 3);
    for (int i = 0; i < bar.size(); ++i)
    {
      TS_ASSERT_EQUALS(bar[i], 100);
    }

    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);
    bar.swap(foo);
    TS_ASSERT(is_container_valid(foo));
    TS_ASSERT(is_container_valid(bar));
    TS_ASSERT_EQUALS(foo.size(), 6);
    for (int i = 0; i < foo.size(); ++i)
    {
      TS_ASSERT_EQUALS(foo[i], 100);
    }

    TS_ASSERT_EQUALS(bar.size(), 5);
    for (int i = 0; i < bar.size(); ++i)
    {
      TS_ASSERT_EQUALS(bar[i], 200);
    }

    foo.push_back(100);
    foo.push_back(100);
    foo.push_back(100);
    foo.push_back(100);
    foo.push_back(100);
  }

  void test_default_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec a(1), std::runtime_error);
    flex::allocation_guard::disable();

    vec a;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 0);
  }

  void test_default_fill_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec a(2), std::runtime_error);
    flex::allocation_guard::disable();

    vec a(2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], object::DEFAULT_VAL);
    TS_ASSERT_EQUALS(a[1], object::DEFAULT_VAL);
  }

  void test_fill_constructor(void)
  {
    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec a(2, object(7)), std::runtime_error);
    flex::allocation_guard::disable();

    vec a(2, object(7));
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 2);
    TS_ASSERT_EQUALS(a[0], 7);
    TS_ASSERT_EQUALS(a[1], 7);
  }

  void test_range_constructor(void)
  {
    vec first(4, object(100));
    TS_ASSERT(is_container_valid(first));

    vec second(first.begin(), first.end());  // iterating through first
    TS_ASSERT(is_container_valid(second));
    TS_ASSERT_EQUALS(second.size(), 4);
    for (vec::iterator it = second.begin(); it != second.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    // the iterator constructor can also be used to construct from vectors:
    int myints[] = { 16, 2, 77, 29 };
    vec third(myints, myints + sizeof(myints) / sizeof(int));
    TS_ASSERT(is_container_valid(third));
    TS_ASSERT_EQUALS(third.size(), 4);
    TS_ASSERT_EQUALS(third[0], 16);
    TS_ASSERT_EQUALS(third[1], 2);
    TS_ASSERT_EQUALS(third[2], 77);
    TS_ASSERT_EQUALS(third[3], 29);
  }

  void test_copy_constructor(void)
  {
    vec a(3, object(0));
    for (int i = 0; i < a.size(); i++)
    {
      a[i] = i;
    }

    flex::allocation_guard::enable();
    TS_ASSERT_THROWS(vec b(a), std::runtime_error);
    flex::allocation_guard::disable();

    vec b(a);
    TS_ASSERT(is_container_valid(b));
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b[i], a[i]);
    }
  }

  void test_relational_operators(void)
  {

    vec foo(3, object(100));   // three ints with a value of 100
    vec bar(2, object(200));   // two ints with a value of 200

    TS_ASSERT(!(foo == bar));
    TS_ASSERT(foo != bar);
    TS_ASSERT(foo < bar);
    TS_ASSERT(!(foo > bar));
    TS_ASSERT(foo <= bar);
    TS_ASSERT(!(foo >= bar));
  }
};
