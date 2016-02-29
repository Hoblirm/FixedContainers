#ifndef FLEX_LIST_ITERATOR_H
#define FLEX_LIST_ITERATOR_H

#include <iterator>

namespace flex
{

   struct list_node_base
   {
      list_node_base* mPrev;
      list_node_base* mNext;

      static void swap(list_node_base& a, list_node_base & b);
   };

   inline void list_node_base::swap(list_node_base& a, list_node_base& b)
   {
      //Swap the internal contents of the nodes.
      std::swap(a,b);

      /*
       * The external nodes that pointed to the swapped nodes must be updated.
       * We also must check for the special case in which the node pointed to itself.
       */
      if (a.mNext == &b)
         a.mNext = a.mPrev = &a;
      else
         a.mNext->mPrev = a.mPrev->mNext = &a;

      if (b.mNext == &a)
         b.mNext = b.mPrev = &b;
      else
         b.mNext->mPrev = b.mPrev->mNext = &b;
   }

   template<class T> struct list_node : public list_node_base
   {
      T mValue;
   };

   template<typename T, typename Pointer = T*, typename Reference = T&>
           struct list_iterator
   {
      typedef list_iterator<T, Pointer, Reference> this_type;
      typedef list_iterator<T, T*, T&> iterator;
      typedef list_node<T> node_type;

      /*
       * The 5 typedefs below are required by the std library to properly identify an iterator.
       */
      typedef T value_type;
      typedef std::ptrdiff_t difference_type;
      typedef Pointer pointer;
      typedef Reference reference;
      typedef std::bidirectional_iterator_tag iterator_category;

   public:
      node_type* mNode;

   public:
      list_iterator();
      list_iterator(const list_node_base * pNode);
      list_iterator(const iterator & x);

      reference operator*() const;
      pointer operator->() const;

      this_type& operator++();
      this_type operator++(int);

      this_type& operator--();
      this_type operator--(int);
   };

   template<typename T, typename Pointer, typename Reference>
   inline list_iterator<T, Pointer, Reference>::list_iterator() :
   mNode()
   {

   }

   template<typename T, typename Pointer, typename Reference>
   inline list_iterator<T, Pointer, Reference>::list_iterator(const list_node_base* pNode) :
   mNode(static_cast<node_type*> ((list_node<T>*)const_cast<list_node_base*> (pNode)))
   {

   }

   template<typename T, typename Pointer, typename Reference>
   inline list_iterator<T, Pointer, Reference>::list_iterator(const iterator& x) :
   mNode(const_cast<node_type*> (x.mNode))
   {

   }

   template<typename T, typename Pointer, typename Reference>
   inline typename list_iterator<T, Pointer, Reference>::reference list_iterator<T, Pointer, Reference>::operator*() const
   {
      return mNode->mValue;
   }

   template<typename T, typename Pointer, typename Reference>
   inline typename list_iterator<T, Pointer, Reference>::pointer list_iterator<T, Pointer, Reference>::operator->() const
   {
      return &mNode->mValue;
   }

   template<typename T, typename Pointer, typename Reference>
   inline typename list_iterator<T, Pointer, Reference>::this_type&
   list_iterator<T, Pointer, Reference>::operator++()
   {
      mNode = static_cast<node_type*> (mNode->mNext);
      return *this;
   }

   template<typename T, typename Pointer, typename Reference>
   inline typename list_iterator<T, Pointer, Reference>::this_type list_iterator<T, Pointer, Reference>::operator++(int)
   {
      this_type temp(*this);
      mNode = static_cast<node_type*> (mNode->mNext);
      return temp;
   }

   template<typename T, typename Pointer, typename Reference>
   inline typename list_iterator<T, Pointer, Reference>::this_type&
   list_iterator<T, Pointer, Reference>::operator--()
   {
      mNode = static_cast<node_type*> (mNode->mPrev);
      return *this;
   }

   template<typename T, typename Pointer, typename Reference>
   inline typename list_iterator<T, Pointer, Reference>::this_type list_iterator<T, Pointer, Reference>::operator--(int)
   {
      this_type temp(*this);
      mNode = static_cast<node_type*> (mNode->mPrev);
      return temp;
   }

   // Extra template parameters were put in to support comparisons between const and non-const iterators.

   template<typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
   inline bool operator==(const list_iterator<T, PointerA, ReferenceA>& a, const list_iterator<T, PointerB, ReferenceB>& b)
   {
      return a.mNode == b.mNode;
   }

   template<typename T, typename PointerA, typename ReferenceA, typename PointerB, typename ReferenceB>
   inline bool operator!=(const list_iterator<T, PointerA, ReferenceA>& a, const list_iterator<T, PointerB, ReferenceB>& b)
   {
      return a.mNode != b.mNode;
   }

} //namespace flex

#endif /* FLEX_LIST_ITERATOR_H */
