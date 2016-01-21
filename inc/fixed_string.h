#ifndef FIXED_STRING_H
#define FIXED_STRING_H

#include <fixed_array.h>

class fixed_string_base: public fixed_array_base<char>
{
public:
  fixed_string_base(size_t capacity) :
      fixed_array_base(capacity)
  {

  }

  fixed_string_base(size_t capacity, char* ptr) :
      fixed_array_base(capacity, ptr)
  {

  }
};

template<size_t N = 0> class fixed_string: public fixed_string_base
{
public:
  fixed_string();
  fixed_string(const fixed_string<N> & obj);
  operator const fixed_string<0>&() const;
  operator fixed_string<0>&();
private:
  char mAry[N];
};

template<size_t N> fixed_string<N>::fixed_string() :
    fixed_string_base(N, mAry)
{
}

template<size_t N> fixed_string<N>::fixed_string(const fixed_string<N> & obj) :
    fixed_string_base(obj.size(), mAry)
{
  for (int i = 0; i < this->mCapacity; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

template<size_t N> fixed_string<N>::operator const fixed_string<0>&() const
{
  return *((fixed_string<0>*) this);
}

template<size_t N> fixed_string<N>::operator fixed_string<0>&()
{
  return *((fixed_string<0>*) this);
}

template<> class fixed_string<0> : public fixed_string_base
{
public:
  fixed_string(size_t capacity);
  fixed_string(const fixed_string<> & obj);
  ~fixed_string();

private:
  void allocate();
};

fixed_string<0>::fixed_string(size_t capacity) :
    fixed_string_base(capacity)
{
  allocate();
}

fixed_string<0>::fixed_string(const fixed_string<> & obj) :
    fixed_string_base(obj.size())
{
  allocate();
  for (int i = 0; i < this->mCapacity; i++)
  {
    this->mAryPtr[i] = obj[i];
  }
}

fixed_string<0>::~fixed_string()
{
  delete[] this->mAryPtr;
}

void fixed_string<0>::allocate()
{
  if (allocation_guard::is_enabled())
  {
    throw std::runtime_error("allocation_guard: fixed_string performed allocation.");
  }
  else
  {
    this->mAryPtr = new char[this->mCapacity]();
  }
}

#endif /* FIXED_STRING_H */
