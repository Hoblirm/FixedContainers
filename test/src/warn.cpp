#include <fixed_array.h>
#include <fixed_vector.h>
#include <fixed_list.h>

int main(int argc, char** argv)
{

fixed_array<int, 8> a;
fixed_array<int> b(8);
fixed_vector<int, 8> c;
fixed_vector<int> d(8);
fixed_list<int, 8> e;
fixed_list<int> f(8);
  return 0;
}
