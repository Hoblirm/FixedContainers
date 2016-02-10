#ifndef FLEX_FIXED_STRING_H
#define FLEX_FIXED_STRING_H

#include <flex/string.h>

namespace flex
{
  template<size_t N> class fixed_string: public string
  {
  public:
    fixed_string();
    fixed_string(const fixed_string<N> & obj);
    fixed_string(const string & obj);
  private:
    char mAry[N];
  };

  template<size_t N> fixed_string<N>::fixed_string() :
      string(N, mAry)
  {
  }

  template<size_t N> fixed_string<N>::fixed_string(const fixed_string<N> & obj) :
      string(obj.capacity(), obj.size(), mAry)
  {
    for (int i = 0; i < this->mSize; i++)
    {
      this->mAryPtr[i] = obj[i];
    }
  }

  template<size_t N> fixed_string<N>::fixed_string(const string & obj)  :
      string(obj.capacity(), obj.size(), mAry)
  {
    for (int i = 0; i < this->mSize; i++)
    {
      this->mAryPtr[i] = obj[i];
    }
  }
} //namespace flex
#endif /* FLEX_FIXED_STRING_H */
