#ifndef FLEX_LIST_H
#define FLEX_LIST_H

#include <flex/allocation_guard.h>
#include <flex/list_iterator.h>
#include <flex/allocator.h>

namespace flex
{

   template<class T, class Alloc = allocator<list_node<T> > > class list : public allocation_guard
   {
   public:
      typedef T value_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef list_node<T> node_type;
      typedef list_node_base base_node_type;
      typedef list_iterator<T, T*, T&> iterator;
      typedef list_iterator<T, const T*, const T&> const_iterator;
      typedef std::reverse_iterator<iterator> reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
      typedef size_t size_type;
      typedef std::ptrdiff_t difference_type;
      typedef Alloc allocator_type;

      list();
      explicit list(size_t size, const T& val = T());
      //TODO: Need a constructor that supports list iterators.
      list(const T* first, const T* last);
      list(const list<T, Alloc> & obj);
      ~list();

      void assign(size_t size, const_reference val);
      //TODO: Get a template working with fixed_list assign() to use multiple iterators.
      void assign(const_iterator first, const_iterator last);
      void assign(const_pointer first, const_pointer last);

      reference back();
      const_reference back() const;

      iterator begin();
      const_iterator begin() const;
      const_iterator cbegin() const;
      const_iterator cend() const;
      const_reverse_iterator crbegin() const;
      const_reverse_iterator crend() const;

      bool empty() const;

      iterator end();
      const_iterator end() const;

      iterator erase(iterator position);
      iterator erase(iterator first, iterator last);

      bool fixed() const;

      reference front();
      allocator_type get_allocator() const;
      const_reference front() const;

      iterator insert(iterator position, const_reference val);
      void insert(iterator position, size_t n, const_reference val);
      //TODO: Get template to work with fixed_list insert() to use multiple iterators.
      void insert(iterator position, const_iterator first, const_iterator last);
      void insert(iterator position, const T* first, const T* last);

      size_type max_size() const;

      list<T, Alloc>& operator=(const list<T, Alloc>& obj);

      void pop_back();
      void pop_front();
      void push_back(const_reference val);
      void push_front(const_reference val);

      reverse_iterator rbegin();
      const_reverse_iterator rbegin() const;
      reverse_iterator rend();
      const_reverse_iterator rend() const;

      size_t size() const;
      void swap(list<T, Alloc>& obj);

   protected:
      list(size_t capacity, list_node<T>* contentPtr);

      base_node_type mAnchor;
      size_type mSize;
      bool mFixed;
      node_type* mNodePool;
      Alloc mAllocator;

   private:
      void push_back_no_throw(const_reference val);
      node_type* DoRetrieveNode();
      void DoPushToNodePool(node_type* ptr);
      void DoDestroyAndDeallocateNode(node_type* ptr);
   };

   template<class T, class Alloc> list<T, Alloc>::list() :
   mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
   {
      mAnchor.mNext = mAnchor.mPrev = &mAnchor;
   }

   template<class T, class Alloc> list<T, Alloc>::list(size_t size, const T& val) :
   mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
   {
      mAnchor.mNext = mAnchor.mPrev = &mAnchor;
      list<T, Alloc>::assign(size, val);
   }

   template<class T, class Alloc> list<T, Alloc>::list(const T* first, const T* last) :
   mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
   {
      mAnchor.mNext = mAnchor.mPrev = &mAnchor;
      list<T, Alloc>::assign(first, last);
   }

   template<class T, class Alloc> list<T, Alloc>::list(const list<T, Alloc> & obj) :
   mAnchor(), mSize(0), mFixed(false), mNodePool(NULL)
   {
      mAnchor.mNext = mAnchor.mPrev = &mAnchor;
      list<T, Alloc>::assign(obj.cbegin(), obj.cend());
   }

   template<class T, class Alloc> list<T, Alloc>::~list()
   {
      if (!mFixed)
      {
         for (iterator it = begin(); it != end(); ++it)
         {
            DoDestroyAndDeallocateNode(it.mNode);
         }
         while (mNodePool != NULL)
         {
            node_type* next = static_cast<node_type*> (mNodePool->mNext);
            DoDestroyAndDeallocateNode(mNodePool);
            mNodePool = next;
         }
      }
   }

   template<class T, class Alloc> void list<T, Alloc>::assign(size_t rsize, list<T, Alloc>::const_reference val)
   {
      if (rsize > max_size())
      {
         throw std::runtime_error("fixed_list: assign() fill range exceeds capacity");
      }

      if (size() < rsize)
      {
         for (iterator it = begin(); it != end(); ++it)
         {
            *it = val;
         }
         while (size() < rsize)
         {
            //Capacity check done above. Safe to do the faster no_throw method.
            push_back_no_throw(val);
         }
      }
      else
      {
         iterator it = begin();
         for (int i = 0; i < rsize; ++i)
         {
            *(it++) = val;
         }
         while (size() > rsize)
         {
            pop_back();
         }
      }
   }

   template<class T, class Alloc> void list<T, Alloc>::assign(list<T, Alloc>::const_iterator first,
   list::const_iterator last)
   {
      typename list<T, Alloc>::iterator lit = begin();
      typename list<T, Alloc>::const_iterator rit = first;
      size_t rsize = 0;
      while ((lit != end()) && (rit != last))
      {
         *(lit++) = *(rit++);
         ++rsize;
      }
      while (size() > rsize)
      {
         pop_back();
      }
      while (rit != last)
      {
         push_back(*(rit++));
      }
   }

   template<class T, class Alloc> void list<T, Alloc>::assign(const T* first, const T* last)
   {
      typename list<T, Alloc>::iterator lit = begin();
      const T* rit = first;
      size_t rsize = 0;
      while ((lit != end()) && (rit != last))
      {
         *(lit++) = *(rit++);
         ++rsize;
      }
      while (size() > rsize)
      {
         pop_back();
      }
      while (rit != last)
      {
         push_back(*(rit++));
      }
   }

   template<class T, class Alloc> typename list<T, Alloc>::reference list<T, Alloc>::back()
   {
      return static_cast<node_type*> (mAnchor.mPrev)->mValue;
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_reference list<T, Alloc>::back() const
   {
      return static_cast<node_type*> (mAnchor.mPrev)->mValue;
   }

   template<class T, class Alloc> typename list<T, Alloc>::iterator list<T, Alloc>::begin()
   {
      return iterator(mAnchor.mNext);
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_iterator list<T, Alloc>::begin() const
   {

      return typename list<T, Alloc>::const_iterator(mAnchor.mNext);
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const
   {
      return typename list<T, Alloc>::const_iterator(mAnchor.mNext);
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const
   {
      return const_iterator(&mAnchor);
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin() const
   {
      return const_reverse_iterator(end());
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const
   {
      return const_reverse_iterator(begin());
   }

   template<class T, class Alloc> bool list<T, Alloc>::empty() const
   {
      return (0 == size());
   }

   template<class T, class Alloc> typename list<T, Alloc>::iterator list<T, Alloc>::end()
   {
      return iterator(&mAnchor);
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_iterator list<T, Alloc>::end() const
   {
      return const_iterator(&mAnchor);
   }

   template<class T, class Alloc> typename list<T, Alloc>::iterator list<T, Alloc>::erase(
   typename list<T, Alloc>::iterator position)
   {

      node_type* lhs = static_cast<node_type*> (position.mNode->mPrev);
      node_type* rhs = static_cast<node_type*> (position.mNode->mNext);

      if (&mAnchor != lhs)
      {
         lhs->mNext = rhs;
      }
      else
      {
         mAnchor.mNext = rhs; //head is being erased; must update
      }

      if (&mAnchor != rhs)
      {
         rhs->mPrev = lhs;
      }
      else
      {
         mAnchor.mPrev = lhs; //tail is being erased; must update
      }

      DoPushToNodePool(position.mNode);
      --mSize;

      return iterator(rhs);
   }

   template<class T, class Alloc> typename list<T, Alloc>::iterator list<T, Alloc>::erase(
   typename list<T, Alloc>::iterator first, typename list<T, Alloc>::iterator last)
   {
      node_type* lhs = static_cast<node_type*> (first.mNode->mPrev);
      node_type* rhs = last.mNode;

      if (&mAnchor != lhs)
      {
         lhs->mNext = rhs;
      }
      else
      {
         mAnchor.mNext = rhs; //head is being erased; must update
      }

      if (&mAnchor != rhs)
      {
         rhs->mPrev = lhs;
      }
      else
      {
         mAnchor.mPrev = lhs; //tail is being erased; must update
      }

      for (iterator it = first; it != last;)
      {
         //Since the iterator become invalidated after the destroy, we want to
         //increment it first and destroy the previous value.
         node_type* ptr = it.mNode;
         ++it;
         DoPushToNodePool(ptr);
         --mSize;
      }

      return last;
   }

   template<class T, class Alloc> bool list<T, Alloc>::fixed() const
   {
      return mFixed;
   }

   template<class T, class Alloc> typename list<T, Alloc>::allocator_type list<T, Alloc>::get_allocator() const
   {
      return mAllocator;
   }

   template<class T, class Alloc> typename list<T, Alloc>::reference list<T, Alloc>::front()
   {
      return static_cast<node_type*> (mAnchor.mNext)->mValue;
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_reference list<T, Alloc>::front() const
   {
      return static_cast<node_type*> (mAnchor.mNext)->mValue;
   }

   template<class T, class Alloc> typename list<T, Alloc>::iterator list<T, Alloc>::insert(
   typename list<T, Alloc>::iterator position, typename list<T, Alloc>::const_reference val)
   {
      if (size() == max_size())
      {
         throw std::runtime_error("fixed_list: insert() called when size was at capacity");
      }

      if (&mAnchor == position.mNode)
      {
         //List is empty, or we are inserting at the end of the list.
         push_back_no_throw(val);
      }
      else
      {
         node_type* lhs = static_cast<node_type*> (position.mNode->mPrev);
         node_type* nd = DoRetrieveNode();
         ++mSize;

         nd->mValue = val;
         nd->mNext = position.mNode;
         position.mNode->mPrev = nd;

         if (&mAnchor != lhs)
         {
            nd->mPrev = lhs;
            lhs->mNext = nd;
         }
         else
         {
            nd->mPrev = &mAnchor;
            mAnchor.mNext = nd;
            //list cannot be empty due to position NULL check; therefore tail doesn't need to be examined
         }
      }

      return iterator(position.mNode->mPrev);
   }

   template<class T, class Alloc> void list<T, Alloc>::insert(list<T, Alloc>::iterator position, size_t n,
   list<T, Alloc>::const_reference val)
   {
      size_t rsize = size() + n;
      if (rsize > max_size())
      {
         throw std::runtime_error("fixed_list: insert() fill range exceeds capacity");
      }

      if (&mAnchor == position.mNode)
      {
         //List is empty, or we are inserting at the end of the list.
         while (size() < rsize)
         {
            push_back_no_throw(val);
         }
      }
      else
      {
         node_type* lhs = static_cast<node_type*> (position.mNode->mPrev);
         node_type* nd = NULL;
         for (size_t i = 0; i < n; ++i)
         {
            nd = DoRetrieveNode();
            ++mSize;

            nd->mValue = val;
            nd->mNext = position.mNode;
            position.mNode->mPrev = nd;
            position.mNode = nd;
         }

         if (&mAnchor != lhs)
         {
            nd->mPrev = lhs;
            lhs->mNext = nd;
         }
         else
         {
            nd->mPrev = &mAnchor;
            mAnchor.mNext = nd;
            //list cannot be empty due to position NULL check; therefore tail doesn't need to be examined
         }
      }
   }

   template<class T, class Alloc> void list<T, Alloc>::insert(list<T, Alloc>::iterator position,
   list<T, Alloc>::const_iterator first, list<T, Alloc>::const_iterator last)
   {
      if (&mAnchor == position.mNode)
      {
         //List is empty, or we are inserting at the end of the list.
         for (const_iterator it = first; it != last; ++it)
         {
            push_back(*it);
         }
      }
      else
      {
         node_type* lhs = static_cast<node_type*> (position.mNode->mPrev);
         node_type* nd = NULL;
         const_iterator it = first;

         if (&mAnchor == lhs)
         {
            nd = DoRetrieveNode();
            ++mSize;

            nd->mValue = *it;
            nd->mPrev = &mAnchor;
            mAnchor.mNext = lhs = nd;
            ++it;
         }

         while (it != last)
         {
            nd = DoRetrieveNode();
            ++mSize;

            nd->mValue = *it;
            nd->mPrev = lhs;
            lhs->mNext = nd;
            lhs = nd;
            ++it;
         }

         nd->mNext = position.mNode;
         position.mNode->mPrev = nd;
      }
   }

   template<class T, class Alloc> void list<T, Alloc>::insert(list<T, Alloc>::iterator position, const T* first,
   const T* last)
   {
      if (&mAnchor == position.mNode)
      {
         //List is empty, or we are inserting at the end of the list.
         for (const T* it = first; it != last; ++it)
         {
            push_back(*it);
         }
      }
      else
      {
         node_type* lhs = static_cast<node_type*> (position.mNode->mPrev);
         node_type* nd = NULL;
         const T* it = first;

         if (&mAnchor == lhs)
         {
            nd = DoRetrieveNode();
            ++mSize;

            nd->mValue = *it;
            nd->mPrev = &mAnchor;
            mAnchor.mNext = lhs = nd;
            ++it;
         }

         while (it != last)
         {
            nd = DoRetrieveNode();
            ++mSize;

            nd->mValue = *it;
            nd->mPrev = lhs;
            lhs->mNext = nd;
            lhs = nd;
            ++it;
         }

         nd->mNext = position.mNode;
         position.mNode->mPrev = nd;
      }
   }

   template<class T, class Alloc> size_t list<T, Alloc>::max_size() const
   {
      return mAllocator.max_size();
   }

   template<class T, class Alloc> list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& obj)
   {
      assign(obj.begin(), obj.end());
      return *this;
   }

   template<class T, class Alloc> void list<T, Alloc>::pop_back()
   {
      node_type* prev = static_cast<node_type*> (mAnchor.mPrev->mPrev);
      if (&mAnchor != prev)
      {
         prev->mNext = &mAnchor;
      }
      else
      {
         mAnchor.mNext = &mAnchor;
      }

      DoPushToNodePool(static_cast<node_type*> (mAnchor.mPrev));
      --mSize;

      mAnchor.mPrev = prev;
   }

   template<class T, class Alloc> void list<T, Alloc>::pop_front()
   {
      node_type* next = static_cast<node_type*> (mAnchor.mNext->mNext);
      if (&mAnchor != next)
      {
         next->mPrev = &mAnchor;
      }
      else
      {
         mAnchor.mPrev = &mAnchor;
      }

      DoPushToNodePool(static_cast<node_type*> (mAnchor.mNext));
      --mSize;

      mAnchor.mNext = next;
   }

   template<class T, class Alloc> void list<T, Alloc>::push_back(const T& val)
   {
      if (size() == max_size())
      {
         throw std::runtime_error("fixed_list: push_back() caused size to exceed capacity");
      }

      push_back_no_throw(val);
   }

   template<class T, class Alloc> void list<T, Alloc>::push_back_no_throw(const T& val)
   {
      node_type* nd = DoRetrieveNode();
      ++mSize;

      nd->mPrev = mAnchor.mPrev;
      nd->mValue = val;
      nd->mNext = &mAnchor;
      if (&mAnchor != mAnchor.mPrev)
      {
         mAnchor.mPrev->mNext = nd;
      }
      else
      {
         mAnchor.mNext = nd;
      }
      mAnchor.mPrev = nd;
   }

   template<class T, class Alloc> void list<T, Alloc>::push_front(const T& val)
   {
      if (size() == max_size())
      {
         throw std::runtime_error("fixed_list: push_front() caused size to exceed capacity");
      }

      node_type* nd = DoRetrieveNode();
      ++mSize;

      nd->mNext = mAnchor.mNext;
      nd->mValue = val;
      nd->mPrev = &mAnchor;
      if (&mAnchor != mAnchor.mNext)
      {
         mAnchor.mNext->mPrev = nd;
      }
      else
      {
         mAnchor.mPrev = nd;
      }
      mAnchor.mNext = nd;
   }

   template<class T, class Alloc> typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin()
   {
      return reverse_iterator(end());
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::rbegin() const
   {
      return const_reverse_iterator(end());
   }

   template<class T, class Alloc> typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend()
   {
      return reverse_iterator(begin());
   }

   template<class T, class Alloc> typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::rend() const
   {
      return const_reverse_iterator(begin());
   }

   template<class T, class Alloc> size_t list<T, Alloc>::size() const
   {
      return mSize;
   }

   template<class T, class Alloc> void list<T, Alloc>::swap(list<T, Alloc>& obj)
   {
      if ((!mFixed) && (!obj.fixed()))
      {
         base_node_type::swap(mAnchor, obj.mAnchor);
         std::swap(mSize, obj.mSize);
         std::swap(mNodePool, obj.mNodePool);
      }
      else
      {
         if ((obj.size() > max_size()) || (size() > obj.max_size()))
         {
            throw std::runtime_error("fixed_list: swap() parameters' size exceed capacity");
         }

         size_t lsize = size();
         size_t rsize = obj.size();
         if (lsize < rsize)
         {
            T tmp;

            typename list<T, Alloc>::iterator lit = begin();
            typename list<T, Alloc>::iterator rit = obj.begin();
            while (lit != end())
            {
               tmp = *lit;
               *lit = *rit;
               *rit = tmp;
               ++lit;
               ++rit;
            }

            while (size() < rsize)
            {
               //Capacity check done at the top of method. Safe to do the faster no_throw method.
               push_back_no_throw(*(rit++));
            }

            while (obj.size() > lsize)
            {
               obj.pop_back();
            }
         }
         else
         {
            T tmp;
            typename list<T, Alloc>::iterator lit = begin();
            typename list<T, Alloc>::iterator rit = obj.begin();
            while (rit != obj.end())
            {
               tmp = *lit;
               *lit = *rit;
               *rit = tmp;
               ++lit;
               ++rit;
            }

            while (obj.size() < lsize)
            {
               //Capacity check done at the top of method. Safe to do the faster no_throw method.
               obj.push_back_no_throw(*(lit++));
            }

            while (size() > rsize)
            {
               pop_back();
            }
         }
      }
   }

   template<class T, class Alloc> list<T, Alloc>::list(size_t capacity, list_node<T>* contentPtr) :
   mAnchor(), mNodePool(NULL), mFixed(true)
   {

   }

   template<class T, class Alloc> list_node<T>* list<T, Alloc>::DoRetrieveNode()
   {
      node_type* ptr;
      if (NULL == mNodePool)
      {
         if (mFixed)
         {
            throw std::runtime_error("flex::fixed_list - exceeded capacity");
         }
         else
         {
            ptr = mAllocator.allocate(1);
            mAllocator.construct(ptr, node_type());
         }
      }
      else
      {
         ptr = mNodePool;
         mNodePool = static_cast<node_type*> (mNodePool->mNext);
      }
      return ptr;
   }

   template<class T, class Alloc>
   inline void list<T, Alloc>::DoPushToNodePool(list_node<T>* ptr)
   {
      ptr->mNext = mNodePool;
      mNodePool = ptr;
   }

   template<class T, class Alloc> void list<T, Alloc>::DoDestroyAndDeallocateNode(list_node<T>* ptr)
   {
      mAllocator.destroy(ptr);
      mAllocator.deallocate(ptr, 1);
   }

   template<class T, class Alloc>
   bool operator==(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
   {
      if (lhs.size() != rhs.size())
      {
         return false;
      }
      else
      {
         typename list<T, Alloc>::const_iterator lit = lhs.begin();
         typename list<T, Alloc>::const_iterator rit = rhs.begin();
         while (lit != lhs.end())
         {
            if (*(lit++) != *(rit++))
            {
               return false;
            }
         }
         return true;
      }
   }

   template<class T, class Alloc>
   bool operator<(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
   {
      if (lhs.size() < rhs.size())
      {
         typename list<T, Alloc>::const_iterator lit = lhs.begin();
         typename list<T, Alloc>::const_iterator rit = rhs.begin();
         while (lit != lhs.end())
         {
            if (*lit < *rit)
            {
               return true;
            }
            else if (*lit > *rit)
            {
               return false;
            }
            ++lit;
            ++rit;
         }
         return true;
      }
      else
      {
         typename list<T, Alloc>::const_iterator lit = lhs.begin();
         typename list<T, Alloc>::const_iterator rit = rhs.begin();
         while (rit != rhs.end())
         {
            if (*lit < *rit)
            {
               return true;
            }
            else if (*lit > *rit)
            {
               return false;
            }
            ++lit;
            ++rit;
         }
         return false;
      }
   }

   template<class T, class Alloc>
   bool operator!=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
   {
      return !(lhs == rhs);
   }

   template<class T, class Alloc>
   bool operator>(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
   {
      return rhs < lhs;
   }

   template<class T, class Alloc>
   bool operator<=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
   {
      return !(rhs < lhs);
   }

   template<class T, class Alloc>
   bool operator>=(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs)
   {
      return !(lhs < rhs);
   }

} //namespace flex
#endif /* FLEX_LIST_H */
