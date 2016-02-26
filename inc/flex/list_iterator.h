//#ifndef FLEX_LIST_ITERATOR_H
//#define FLEX_LIST_ITERATOR_H
//
//#include <iterator>
//
//namespace flex
//{
//
//   struct list_node_base
//   {
//      list_node_base* prev;
//      list_node_base* next;
//   };
//
//   template <class T> list_node : public list_node_base
//   {
//      T val;
//   };
//   
//   template<class T> struct list_iterator_base : public std::iterator<std::bidirectional_iterator_tag, T>
//   {
//      typedef std::iterator<std::bidirectional_iterator_tag, T> base_type;
//      typedef typename base_type::value_type value_type;
//      typedef typename base_type::difference_type difference_type;
//      typedef typename base_type::pointer pointer;
//      typedef const T* const_pointer;
//      typedef typename base_type::reference reference;
//      typedef const T& const_reference;
//      typedef typename base_type::iterator_category iterator_category;
//
//      pointer mPtr;
//      pointer mLeftBound;
//      pointer mRightBound;
//
//      list_iterator_base();
//      list_iterator_base(T* ptr, T* left_bound, T * right_bound);
//
//      bool operator==(list_iterator_base<T> obj) const;
//      bool operator!=(list_iterator_base<T> obj) const;
//
//      const_reference operator*() const;
//      const_pointer operator->() const;
//   };
//
//   template<class T> struct list_const_iterator : public list_iterator_base<T>
//   {
//      typedef list_iterator_base<T> base_type;
//      typedef typename base_type::value_type value_type;
//      typedef typename base_type::difference_type difference_type;
//      typedef typename base_type::pointer pointer;
//      typedef typename base_type::const_pointer const_pointer;
//      typedef typename base_type::reference reference;
//      typedef typename base_type::const_reference const_reference;
//      typedef typename base_type::iterator_category iterator_category;
//
//      using base_type::mPtr;
//      using base_type::mLeftBound;
//      using base_type::mRightBound;
//
//      list_const_iterator();
//      list_const_iterator(T* ptr, T* left_bound, T * right_bound);
//
//      list_const_iterator<T>& operator++();
//      list_const_iterator<T> operator++(int);
//      list_const_iterator<T>& operator--();
//      list_const_iterator<T> operator--(int);
//   };
//
//   template<class T> struct list_iterator : public list_const_iterator<T>
//   {
//      typedef list_const_iterator<T> base_type;
//      typedef typename base_type::value_type value_type;
//      typedef typename base_type::difference_type difference_type;
//      typedef typename base_type::pointer pointer;
//      typedef typename base_type::const_pointer const_pointer;
//      typedef typename base_type::reference reference;
//      typedef typename base_type::const_reference const_reference;
//      typedef typename base_type::iterator_category iterator_category;
//
//      using base_type::mPtr;
//      using base_type::mLeftBound;
//      using base_type::mRightBound;
//
//      list_iterator();
//      list_iterator(T* ptr, T* left_bound, T * right_bound);
//      list_iterator<T>& operator++();
//      list_iterator<T> operator++(int);
//      list_iterator<T>& operator--();
//      list_iterator<T> operator--(int);
//
//      const_reference operator*() const;
//      const_pointer operator->() const;
//      reference operator*();
//      pointer operator->();
//   };
//
//   template<class T> struct list_const_reverse_iterator : public list_iterator_base<T>
//   {
//      typedef list_iterator_base<T> base_type;
//      typedef typename base_type::value_type value_type;
//      typedef typename base_type::difference_type difference_type;
//      typedef typename base_type::pointer pointer;
//      typedef typename base_type::const_pointer const_pointer;
//      typedef typename base_type::reference reference;
//      typedef typename base_type::const_reference const_reference;
//      typedef typename base_type::iterator_category iterator_category;
//
//      using base_type::mPtr;
//      using base_type::mLeftBound;
//      using base_type::mRightBound;
//
//      list_const_reverse_iterator();
//      list_const_reverse_iterator(T* ptr, T* left_bound, T * right_bound);
//      list_const_reverse_iterator<T>& operator++();
//      list_const_reverse_iterator<T> operator++(int);
//      list_const_reverse_iterator<T>& operator--();
//      list_const_reverse_iterator<T> operator--(int);
//   };
//
//   template<class T> struct list_reverse_iterator : public list_const_reverse_iterator<T>
//   {
//      typedef list_const_reverse_iterator<T> base_type;
//      typedef typename base_type::value_type value_type;
//      typedef typename base_type::difference_type difference_type;
//      typedef typename base_type::pointer pointer;
//      typedef typename base_type::const_pointer const_pointer;
//      typedef typename base_type::reference reference;
//      typedef typename base_type::const_reference const_reference;
//      typedef typename base_type::iterator_category iterator_category;
//
//      using base_type::mPtr;
//      using base_type::mLeftBound;
//      using base_type::mRightBound;
//
//      list_reverse_iterator();
//      list_reverse_iterator(T* ptr, T* left_bound, T * right_bound);
//      list_reverse_iterator<T>& operator++();
//      list_reverse_iterator<T> operator++(int);
//      list_reverse_iterator<T>& operator--();
//      list_reverse_iterator<T> operator--(int);
//
//      const_reference operator*() const;
//      const_pointer operator->() const;
//      reference operator*();
//      pointer operator->();
//   };
//
//   /*
//    * list_iterator_base
//    */
//   template<class T>
//   inline list_iterator_base<T>::list_iterator_base() :
//   mPtr(NULL), mLeftBound(NULL), mRightBound(NULL)
//   {
//   }
//
//   template<class T>
//   inline list_iterator_base<T>::list_iterator_base(T* ptr, T* left_bound, T* right_bound) :
//   mPtr(ptr), mLeftBound(left_bound), mRightBound(right_bound)
//   {
//   }
//
//   template<class T>
//   inline bool list_iterator_base<T>::operator==(list_iterator_base<T> obj) const
//   {
//      return (mPtr == obj.mPtr);
//   }
//
//   template<class T>
//   inline bool list_iterator_base<T>::operator!=(list_iterator_base<T> obj) const
//   {
//      return (mPtr != obj.mPtr);
//   }
//
//   template<class T>
//   inline typename list_iterator_base<T>::const_reference list_iterator_base<T>::operator*() const
//   {
//      return *mPtr;
//   }
//
//   template<class T>
//   inline typename list_iterator_base<T>::const_pointer list_iterator_base<T>::operator->() const
//   {
//      return mPtr;
//   }
//
//   /*
//    * list_const_iterator
//    */
//   template<class T>
//   inline list_const_iterator<T>::list_const_iterator() :
//   base_type(NULL, NULL, NULL)
//   {
//   }
//
//   template<class T>
//   inline list_const_iterator<T>::list_const_iterator(T* ptr, T* left_bound, T* right_bound) :
//   base_type(ptr, left_bound, right_bound)
//   {
//   }
//
//   template<class T>
//   inline list_const_iterator<T>& list_const_iterator<T>::operator++()
//   {
//      if (mPtr == mRightBound)
//      {
//         mPtr = mLeftBound;
//      }
//      else
//      {
//         ++mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_const_iterator<T> list_const_iterator<T>::operator++(int)
//   {
//      list_const_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator++();
//      return tmp;
//   }
//
//   template<class T>
//   inline list_const_iterator<T>& list_const_iterator<T>::operator--()
//   {
//      if (mPtr == mLeftBound)
//      {
//         mPtr = mRightBound;
//      }
//      else
//      {
//         --mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_const_iterator<T> list_const_iterator<T>::operator--(int)
//   {
//      list_const_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator--();
//      return tmp;
//   }
//
//   /*
//    * list_iterator
//    */
//
//   template<class T>
//   inline list_iterator<T>::list_iterator() :
//   base_type(NULL, NULL, NULL)
//   {
//   }
//
//   template<class T>
//   inline list_iterator<T>::list_iterator(T* ptr, T* left_bound, T* right_bound) :
//   base_type(ptr, left_bound, right_bound)
//   {
//   }
//
//   template<class T>
//   inline list_iterator<T>& list_iterator<T>::operator++()
//   {
//      if (mPtr == mRightBound)
//      {
//         mPtr = mLeftBound;
//      }
//      else
//      {
//         ++mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_iterator<T> list_iterator<T>::operator++(int)
//   {
//      list_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator++();
//      return tmp;
//   }
//
//   template<class T>
//   inline list_iterator<T>& list_iterator<T>::operator--()
//   {
//      if (mPtr == mLeftBound)
//      {
//         mPtr = mRightBound;
//      }
//      else
//      {
//         --mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_iterator<T> list_iterator<T>::operator--(int)
//   {
//      list_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator--();
//      return tmp;
//   }
//
//   template<class T>
//   inline typename list_iterator<T>::reference list_iterator<T>::operator*()
//   {
//      return *mPtr;
//   }
//
//   template<class T>
//   inline typename list_iterator<T>::pointer list_iterator<T>::operator->()
//   {
//      return mPtr;
//   }
//
//   template<class T>
//   inline typename list_iterator<T>::const_reference list_iterator<T>::operator*() const
//   {
//      return *mPtr;
//   }
//
//   template<class T>
//   inline typename list_iterator<T>::const_pointer list_iterator<T>::operator->() const
//   {
//      return mPtr;
//   }
//
//   /*
//    * list_const_reverse_iterator
//    */
//   template<class T>
//   inline list_const_reverse_iterator<T>::list_const_reverse_iterator() :
//   base_type(NULL, NULL, NULL)
//   {
//   }
//
//   template<class T>
//   inline list_const_reverse_iterator<T>::list_const_reverse_iterator(T* ptr, T* left_bound, T* right_bound) :
//   base_type(ptr, left_bound, right_bound)
//   {
//   }
//
//   template<class T>
//   inline list_const_reverse_iterator<T>& list_const_reverse_iterator<T>::operator++()
//   {
//      if (mPtr == mLeftBound)
//      {
//         mPtr = mRightBound;
//      }
//      else
//      {
//         --mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_const_reverse_iterator<T> list_const_reverse_iterator<T>::operator++(int)
//   {
//      list_const_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator++();
//      return tmp;
//   }
//
//   template<class T>
//   inline list_const_reverse_iterator<T>& list_const_reverse_iterator<T>::operator--()
//   {
//      if (mPtr == mRightBound)
//      {
//         mPtr = mLeftBound;
//      }
//      else
//      {
//         ++mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_const_reverse_iterator<T> list_const_reverse_iterator<T>::operator--(int)
//   {
//      list_const_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator--();
//      return tmp;
//   }
//
//   /*
//    * list_reverse_iterator
//    */
//   template<class T>
//   inline list_reverse_iterator<T>::list_reverse_iterator() :
//   base_type(NULL, NULL, NULL)
//   {
//   }
//
//   template<class T>
//   inline list_reverse_iterator<T>::list_reverse_iterator(T* ptr, T* left_bound, T* right_bound) :
//   base_type(ptr, left_bound, right_bound)
//   {
//   }
//
//   template<class T>
//   inline list_reverse_iterator<T>& list_reverse_iterator<T>::operator++()
//   {
//      if (mPtr == mLeftBound)
//      {
//         mPtr = mRightBound;
//      }
//      else
//      {
//         --mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_reverse_iterator<T> list_reverse_iterator<T>::operator++(int)
//   {
//      list_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator++();
//      return tmp;
//   }
//
//   template<class T>
//   inline list_reverse_iterator<T>& list_reverse_iterator<T>::operator--()
//   {
//      if (mPtr == mRightBound)
//      {
//         mPtr = mLeftBound;
//      }
//      else
//      {
//         ++mPtr;
//      }
//      return *this;
//   }
//
//   template<class T>
//   inline list_reverse_iterator<T> list_reverse_iterator<T>::operator--(int)
//   {
//      list_reverse_iterator<T> tmp(mPtr, mLeftBound, mRightBound);
//      operator--();
//      return tmp;
//   }
//
//   template<class T>
//   inline typename list_reverse_iterator<T>::reference list_reverse_iterator<T>::operator*()
//   {
//      return *mPtr;
//   }
//
//   template<class T>
//   inline typename list_reverse_iterator<T>::pointer list_reverse_iterator<T>::operator->()
//   {
//      return mPtr;
//   }
//
//   template<class T>
//   inline typename list_reverse_iterator<T>::const_reference list_reverse_iterator<T>::operator*() const
//   {
//      return *mPtr;
//   }
//
//   template<class T>
//   inline typename list_reverse_iterator<T>::const_pointer list_reverse_iterator<T>::operator->() const
//   {
//      return mPtr;
//   }
//
//} //namespace flex
//
//#endif /* FLEX_LIST_ITERATOR_H */
