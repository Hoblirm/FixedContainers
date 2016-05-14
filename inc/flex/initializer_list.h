#ifndef FLEX_INITIALIZER_LIST_H
#define FLEX_INITIALIZER_LIST_H

#ifdef FLEX_HAS_CXX11
#include <initializer_list>
#else

#include <flex/config.h>

namespace std
{
  // See the C++11 Standard, section 18.9.
  template<class E>
  class initializer_list
  {
  public:
    typedef E value_type;
    typedef const E& reference;
    typedef const E& const_reference;
    typedef size_t size_type;
    typedef const E* iterator;    // Must be const, as initializer_list (and its mpArray) is an immutable temp object.
    typedef const E* const_iterator;

  private:
    iterator mpArray;
    size_type mArraySize;

    // This constructor is private, but the C++ compiler has the ability to call it, as per the C++11 Standard.
    initializer_list(const_iterator pArray, size_type arraySize) :
        mpArray(pArray), mArraySize(arraySize)
    {
    }

  public:
    initializer_list() FLEX_NOEXCEPT
    : mpArray(NULL), mArraySize(0)
    {}

    size_type size() const FLEX_NOEXCEPT
    { return mArraySize;}
    const_iterator begin() const FLEX_NOEXCEPT
    { return mpArray;}   // Must be const_iterator, as initializer_list (and its mpArray) is an immutable temp object.
    const_iterator end() const FLEX_NOEXCEPT
    { return mpArray + mArraySize;}
  };

template<class T>
const T* begin(std::initializer_list<T> ilist) FLEX_NOEXCEPT
{
  return ilist.begin();
}

template<class T>
const T* end(std::initializer_list<T> ilist) FLEX_NOEXCEPT
{
  return ilist.end();
}
}
#endif//FLEX_HAS_CXX11

#endif //FLEX_INITIALIZER_LIST_H
