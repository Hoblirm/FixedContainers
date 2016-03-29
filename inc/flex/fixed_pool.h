//#ifndef FLEX_FIXED_POOL_H
//#define FLEX_FIXED_POOL_H
//
//#include <flex/pool.h>
//
//namespace flex
//{
//
//  template<class T, size_t N = 0> class fixed_pool: public pool<T>
//  {
//  public:
//    fixed_pool();
//  private:
//    T mContentAry[N];
//    T* mPtrAry[N];
//  };
//
//  template<class T, size_t N> fixed_pool<T, N>::fixed_pool() :
//      pool<T>(N, mContentAry, mPtrAry)
//  {
//    for (size_t i = 0; i < this->mCapacity; i++)
//    {
//      mPtrAry[i] = &mContentAry[i];
//    }
//  }
//
//}  //namespace flex
//
//#endif /* FLEX_FIXED_POOL_H */
