#include <cxxtest/TestSuite.h>

#include <flex/fixed_vector.h>

struct obj
{
  static const int DEFAULT_VAL = 1;
  static const int INIT_KEY = 858599509;

  obj() :
      val(DEFAULT_VAL), init(INIT_KEY)
  {
  }

  obj(int i) :
      val(i), init(INIT_KEY)
  {
  }

  ~obj()
  {
    init = 0;
  }

  obj& operator=(const obj& o)
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
    -68560373, 542144249, -964249373, 672765407, 1240222082, -170251308, 573136605, 522427348, -1842488270, -803442179,
    1214800559, -439290856, -850489475, -371113959, -528653948, -1466750983, -299654597, -1095361209, 912904732 };

class fixed_vector_test: public CxxTest::TestSuite
{
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

  bool is_container_valid(const flex::vector<obj>& c)
  {
    for (int i = 0; i < c.size(); ++i)
    {
      if (c[i].init != obj::INIT_KEY)
      {
        printf("Error: Expected (c[%d] == object::INIT_KEY), found (%d != %d)\n", i, c[i].init, obj::INIT_KEY);
        return false;
      }
    }
    for (int i = c.size(); i < c.capacity(); ++i)
    {
      if (c[i].init == obj::INIT_KEY)
      {
        printf("Error: Expected (c[%d] != object::INIT_KEY), found (%d == %d)\n", i, c[i].init, obj::INIT_KEY);
        return false;
      }
    }
    return true;
  }

  void test_assign(void)
  {
    fixed_vector<obj, 7> first;
    fixed_vector<obj, 5> second;
    vector<obj> third;

    TS_ASSERT_THROWS(first.assign(8, (obj )100), std::out_of_range);
    first.assign(7, (obj) 100);             // 7 ints with a value of 100
    TS_ASSERT(is_container_valid(first));

    fixed_vector<obj, 7>::iterator it;
    it = first.begin() + 1;

    TS_ASSERT_THROWS(second.assign(it, first.end()), std::out_of_range);
    second.assign(it, first.end() - 1); // the 5 central values of first
    TS_ASSERT(is_container_valid(second));

    int myints[] = { 1776, 7, 4 };
    flex::allocation_guard::disable();
    third.assign(myints, myints + 3);   // assigning from array.
    flex::allocation_guard::enable();
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

  void test_capacity(void)
  {
    fixed_vector<obj, 17> a;
    TS_ASSERT_EQUALS(a.capacity(), 17);
    vector<obj> b;
    TS_ASSERT_EQUALS(b.capacity(), 0);
  }

  void test_clear(void)
  {
    fixed_vector<obj, 8> myvector;
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
    fixed_vector<obj, 16> myvector;
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
    fixed_vector<obj, 16> myvector;

    // set some values (from 1 to 10)
    for (int i = 1; i <= 10; i++)
      myvector.push_back(i);
    TS_ASSERT_EQUALS(myvector.size(), 10);

    // erase the 6th element
    vector<obj>::iterator it = myvector.erase(myvector.begin() + 5);
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

  void test_insert(void)
  {
    fixed_vector<obj, 1> emptyvec;
    TS_ASSERT_THROWS(emptyvec.insert(emptyvec.begin(), 2, (obj )2), std::out_of_range);
    fixed_vector<obj, 16> myvector(3, (obj) 100);
    TS_ASSERT(is_container_valid(myvector));
    fixed_vector<obj, 16>::iterator it;

    it = myvector.begin();
    it = myvector.insert(it, 200);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector.size(), 4);
    TS_ASSERT_EQUALS(myvector[0], 200);
    TS_ASSERT_EQUALS(myvector[1], 100);
    TS_ASSERT_EQUALS(myvector[2], 100);
    TS_ASSERT_EQUALS(myvector[3], 100);

    TS_ASSERT_THROWS(myvector.insert(it, 13, (obj )300), std::out_of_range);
    myvector.insert(it, 2, (obj) 300);
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
    fixed_vector<obj, 16> anothervector(2, (obj) 400);
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

    int myarray[] = { 501, 502, 503, 12, 13, 14, 15, 16, 17 };
    TS_ASSERT_THROWS(myvector.insert(myvector.begin(), myarray, myarray + 9), std::out_of_range);
    myvector.insert(myvector.begin(), myarray, myarray + 3);
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
    fixed_vector<obj, 17> a;
    TS_ASSERT_EQUALS(a.max_size(), 17);
  }

  void test_assignment_operator(void)
  {
    fixed_vector<obj, 8> foo(3);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    fixed_vector<obj, 8> bar(5, (obj) 2);

    bar = foo;
    TS_ASSERT(is_container_valid(bar));
    TS_ASSERT_EQUALS(bar.size(), 3);
    TS_ASSERT_EQUALS(bar[0], 1);
    TS_ASSERT_EQUALS(bar[1], 5);
    TS_ASSERT_EQUALS(bar[2], 17);

    //Ensure assignments on bar doens't impact foo.
    bar.assign(0, (obj) 3);
    TS_ASSERT(is_container_valid(bar));
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //We are setting foo to an empty array.  Size is zero, but the capacity is still eight.  Behavior-wise
    //it doesn't matter what the contents are since the size is zero.  However, we want to ensure that the
    //assignment operator doesn't perform extra work by resetting these values.  This happens if no assignment
    //operator is defined and a default one is used.
    foo = fixed_vector<obj, 8>();
    TS_ASSERT(is_container_valid(foo));
    TS_ASSERT_EQUALS(foo.size(), 0);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    //Same thing as above, but we want to ensure that the cast operator allows assignment of vectors
    //with different capacities.
    foo = fixed_vector<obj, 16>(1, (obj) 19);
    TS_ASSERT(is_container_valid(foo));
    TS_ASSERT_EQUALS(foo.size(), 1);
    TS_ASSERT_EQUALS(foo[0], 19);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);

    fixed_vector<obj, 16> larger(16);
    TS_ASSERT_THROWS(foo = larger, std::out_of_range);

    foo.assign(3, (obj) 0);
    foo[0] = 1;
    foo[1] = 5;
    foo[2] = 17;

    flex::allocation_guard::disable();
    vector<obj> bar2(5, (obj) 2);
    bar2 = foo;
    TS_ASSERT(is_container_valid(bar2));
    flex::allocation_guard::enable();
    TS_ASSERT_EQUALS(bar2.size(), 3);
    TS_ASSERT_EQUALS(bar2[0], 1);
    TS_ASSERT_EQUALS(bar2[1], 5);
    TS_ASSERT_EQUALS(bar2[2], 17);

    //Ensure assignments on bar doens't impact foo.
    bar2.assign(0, (obj) 3);
    TS_ASSERT(is_container_valid(bar2));
    TS_ASSERT_EQUALS(foo.size(), 3);
    TS_ASSERT_EQUALS(foo[0], 1);
    TS_ASSERT_EQUALS(foo[1], 5);
    TS_ASSERT_EQUALS(foo[2], 17);
  }

  void test_push_back_and_pop_back(void)
  {
    fixed_vector<obj, 8> myvector;
    int size = 0;
    TS_ASSERT_EQUALS(myvector.size(), size);

    int sum(0);
    myvector.push_back(100);
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);
    myvector.push_back(200);
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);
    myvector.push_back(300);
    ++size;
    TS_ASSERT_EQUALS(myvector.size(), size);
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

  void test_resize(void)
  {
    fixed_vector<obj, 16> myvector;

    for (int i = 1; i < 10; i++)
      myvector.push_back(i);

    myvector.resize(5);
    TS_ASSERT(is_container_valid(myvector));
    TS_ASSERT_EQUALS(myvector[0], 1);
    TS_ASSERT_EQUALS(myvector[1], 2);
    TS_ASSERT_EQUALS(myvector[2], 3);
    TS_ASSERT_EQUALS(myvector[3], 4);
    TS_ASSERT_EQUALS(myvector[4], 5);

    myvector.resize(8, (obj) 100);
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
    TS_ASSERT_EQUALS(myvector[8], obj::DEFAULT_VAL);
    TS_ASSERT_EQUALS(myvector[9], obj::DEFAULT_VAL);
    TS_ASSERT_EQUALS(myvector[10], obj::DEFAULT_VAL);
    TS_ASSERT_EQUALS(myvector[11], obj::DEFAULT_VAL);
  }

  void test_swap(void)
  {
    fixed_vector<obj, 16> foo(3, (obj) 100);   // three ints with a value of 100
    flex::allocation_guard::disable();
    vector<obj> bar(5, (obj) 200);   // five ints with a value of 200
    flex::allocation_guard::enable();
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

    flex::allocation_guard::disable();
    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);
    flex::allocation_guard::enable();
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

    //Verify that swap will properly cause non-fixed containers to reallocate as necessary.
    TS_ASSERT_EQUALS(bar.capacity(), 10);
    flex::allocation_guard::disable();
    bar.swap(foo);
    flex::allocation_guard::enable();
    TS_ASSERT_EQUALS(bar.capacity(), 20);
    TS_ASSERT(is_container_valid(foo));
    TS_ASSERT(is_container_valid(bar));

    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);
    bar.push_back(100);

    TS_ASSERT_THROWS(bar.swap(foo), std::out_of_range);
  }

  void test_default_constructor(void)
  {
    fixed_vector<obj, 3> a;
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 0);
    TS_ASSERT_EQUALS(a.capacity(), 3);
  }

  void invalid_fill_constructor(void)
  {
    fixed_vector<obj, 3> a(4);
    TS_ASSERT(is_container_valid(a));
  }

  void test_default_fill_constructor(void)
  {
    fixed_vector<obj, 3> a(2);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 3);
    TS_ASSERT_EQUALS(a[0], obj::DEFAULT_VAL);
    TS_ASSERT_EQUALS(a[1], obj::DEFAULT_VAL);

    TS_ASSERT_THROWS(invalid_fill_constructor(), std::out_of_range);
  }

  void test_fill_constructor(void)
  {
    fixed_vector<obj, 3> a(2, (obj) 7);
    TS_ASSERT(is_container_valid(a));
    TS_ASSERT_EQUALS(a.size(), 2);
    TS_ASSERT_EQUALS(a.capacity(), 3);
    TS_ASSERT_EQUALS(a[0], 7);
    TS_ASSERT_EQUALS(a[1], 7);
  }

  void test_range_constructor(void)
  {
    fixed_vector<obj, 4> first(4, (obj) 100);
    TS_ASSERT(is_container_valid(first));
    fixed_vector<obj, 4> second(first.begin(), first.end());  // iterating through first
    TS_ASSERT(is_container_valid(second));
    TS_ASSERT_EQUALS(second.size(), 4);
    for (vector<obj>::iterator it = second.begin(); it != second.end(); ++it)
    {
      TS_ASSERT_EQUALS(*it, 100);
    }

    // the iterator constructor can also be used to construct from arrays:
    int myints[] = { 16, 2, 77, 29 };
    fixed_vector<obj, 4> third(myints, myints + sizeof(myints) / sizeof(int));
    TS_ASSERT(is_container_valid(third));
    TS_ASSERT_EQUALS(third.size(), 4);
    TS_ASSERT_EQUALS(third[0], 16);
    TS_ASSERT_EQUALS(third[1], 2);
    TS_ASSERT_EQUALS(third[2], 77);
    TS_ASSERT_EQUALS(third[3], 29);
  }

  void test_copy_constructor(void)
  {
    fixed_vector<obj, 3> a(3, (obj) 0);
    for (int i = 0; i < a.size(); i++)
    {
      a[i] = i;
    }

    fixed_vector<obj, 3> b(a);
    TS_ASSERT(is_container_valid(b));
    for (int i = 0; i < b.size(); i++)
    {
      TS_ASSERT_EQUALS(b[i], a[i]);
    }
  }

  void test_move_constructor()
  {
    printf("X");
  }

  void test_initializer_constructor()
  {
    printf("X");
  }

  void assignment_method(vector<obj>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      x[i] = i;
    }
  }

  void read_method(const vector<obj>& x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], i);
    }
  }

  void copy_method(vector<obj> x)
  {
    for (int i = 0; i < x.size(); i++)
    {
      TS_ASSERT_EQUALS(x[i], i);
    }
  }

  void test_cast_operator(void)
  {
    fixed_vector<obj, 8> a(8, (obj) 0);
    assignment_method(a);
    read_method(a);

    TS_ASSERT(!errno);
    copy_method(a);
    TS_ASSERT(errno);
    errno = 0;

    allocation_guard::disable();
    copy_method(a);
  }

  void test_relational_operators(void)
  {
    fixed_vector<obj, 8> foo(3, (obj) 100);   // three ints with a value of 100
    fixed_vector<obj, 8> bar(2, (obj) 200);   // two ints with a value of 200

    TS_ASSERT(!(foo == bar));
    TS_ASSERT(foo != bar);
    TS_ASSERT(foo < bar);
    TS_ASSERT(!(foo > bar));
    TS_ASSERT(foo <= bar);
    TS_ASSERT(!(foo >= bar));
  }
};
