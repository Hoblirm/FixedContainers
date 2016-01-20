#ifndef FIXED_STRING_H
#define FIXED_STRING_H

#include <fixed_array.h>

template<size_t N = 0>
class fixed_string: public fixed_array<char, N>
{
public:
  operator const fixed_string<0>&() const;
  operator fixed_string<0>&();
};

template<size_t N> fixed_string<N>::operator const fixed_string<0>&() const
{
  return *((fixed_string<0>*) this);
}

template<size_t N> fixed_string<N>::operator fixed_string<0>&()
{
  return *((fixed_string<0>*) this);
}

template<>
class fixed_string<0> : public fixed_array<char, 0>
{
public:
  fixed_string(size_t capacity);
};

fixed_string<0>::fixed_string(size_t capacity) :
    fixed_array(capacity)
{
}

#endif /* FIXED_STRING_H */
