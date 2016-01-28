#ifndef FIXED_LIST_BASE_H
#define FIXED_LIST_BASE_H

#include <fixed_pool.h>
#include <fixed_list_iterator.h>

template<class T, class Pool> class fixed_list_base : public allocation_guard
{
public:
   typedef T value_type;
   typedef T& reference;
   typedef fixed_list_node<T> node;
   typedef const T& const_reference;
   typedef fixed_list_iterator<T> iterator;
   typedef fixed_list_const_iterator<T> const_iterator;
   typedef fixed_list_reverse_iterator<T> reverse_iterator;
   typedef fixed_list_const_reverse_iterator<T> const_reverse_iterator;

   void assign(size_t size, const_reference val);
   void assign(const_iterator first, const_iterator last);
   void assign(const T* first, const T* last);

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

   reference front();
   const_reference front() const;

   size_t max_size() const;

   fixed_list_base<T, Pool>& operator=(const fixed_list_base<T, Pool>& obj);

   void pop_back();
   void push_back(const_reference val);

   reverse_iterator rbegin();
   const_reverse_iterator rbegin() const;
   reverse_iterator rend();
   const_reverse_iterator rend() const;

   size_t size() const;
   void swap(fixed_list_base<T, Pool>& obj);

protected:
   fixed_list_base();
   fixed_list_base(size_t size);

   node* mHead;
   node* mTail;

private:
   void push_back_no_throw(const_reference val);
protected:
   Pool mPool;
   //**Note: The mPool member must be defined last!  This template class is different between specialized versions of fixed_list.
   // Since the size varies, it is critical that this variable is defined at the end to allow casting between the specialized and non-specialized lists.
};

template<class T, class Pool> void fixed_list_base<T, Pool>::assign(size_t size, fixed_list_base<T, Pool>::const_reference val)
{
   if (size > mPool.outstanding())
   {
      throw std::runtime_error("fixed_list: assign() fill range exceeds capacity");
   }

   if (mPool.outstanding() < size)
   {
      for (typename fixed_list_base<T, Pool>::iterator it = begin(); it != end(); ++it)
      {
         *it = val;
      }
      while (mPool.outstanding() < size)
      {
         //Capacity check done above. Safe to do the faster no_throw method.
         push_back_no_throw(val);
      }
   }
   else
   {
      typename fixed_list_base<T, Pool>::iterator it = begin();
      for (int i = 0; i < size; ++i)
      {
         *(it++) = val;
      }
      while (mPool.outstanding() > size)
      {
         pop_back();
      }
   }
}

template<class T, class Pool> void fixed_list_base<T, Pool>::assign(fixed_list_base<T, Pool>::const_iterator first,
fixed_list_base::const_iterator last)
{

   typename fixed_list_base<T, Pool>::iterator lit = begin();
   typename fixed_list_base<T, Pool>::const_iterator rit = first;
   size_t rsize = 0;
   while ((lit != end()) && (rit != last))
   {
      *(lit++) = *(rit++);
   }
   while (mPool.outstanding() > rsize)
   {
      pop_back();
   }
   while (rit != last)
   {
      push_back(*(rit++));
   }
}

template<class T, class Pool> void fixed_list_base<T, Pool>::assign(const T* first, const T* last)
{
   typename fixed_list_base<T, Pool>::iterator lit = begin();
   const T* rit = first;
   size_t rsize = 0;
   while ((lit != end()) && (rit != last))
   {
      *(lit++) = *(rit++);
   }
   while (mPool.outstanding() > rsize)
   {
      pop_back();
   }
   while (rit != last)
   {
      push_back(*(rit++));
   }
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::reference fixed_list_base<T, Pool>::back()
{
   return mTail->val;
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_reference fixed_list_base<T, Pool>::back() const
{
   return mTail->val;
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::iterator fixed_list_base<T, Pool>::begin() 
{
   return typename fixed_list_base<T, Pool>::iterator (mHead);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_iterator fixed_list_base<T, Pool>::begin() const
{

   return typename fixed_list_base<T, Pool>::const_iterator (mHead);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_iterator fixed_list_base<T, Pool>::cbegin() const
{
   return typename fixed_list_base<T, Pool>::const_iterator (mHead);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_iterator fixed_list_base<T, Pool>::cend() const
{
   return typename fixed_list_base<T, Pool>::const_iterator (NULL);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_reverse_iterator fixed_list_base<T, Pool>::crbegin() const
{
   return fixed_list_const_reverse_iterator < T > (mTail);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_reverse_iterator fixed_list_base<T, Pool>::crend() const
{
   return fixed_list_const_reverse_iterator < T > (NULL);
}

template<class T, class Pool> bool fixed_list_base<T, Pool>::empty() const
{
   return (0 == mPool.outstanding());
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::iterator fixed_list_base<T, Pool>::end()
{
   return typename fixed_list_base<T, Pool>::iterator (NULL);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_iterator fixed_list_base<T, Pool>::end() const
{
   return typename fixed_list_base<T, Pool>::const_iterator (NULL);
}
   
template<class T, class Pool> typename fixed_list_base<T, Pool>::reference fixed_list_base<T, Pool>::front()
{
   return mHead->val;
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_reference fixed_list_base<T, Pool>::front() const
{
   return mHead->val;
}

template<class T, class Pool> size_t fixed_list_base<T, Pool>::max_size() const
{
   return mPool.max_size();
}

template<class T, class Pool> fixed_list_base<T, Pool>& fixed_list_base<T, Pool>::operator=(const fixed_list_base<T, Pool>& obj)
{
   if (obj.size() > mPool.max_size())
   {
      throw std::runtime_error("fixed_list: assignment operator's parameter size exceeds capacity");
   }

   typename fixed_list_base<T, Pool>::iterator lit = begin();
   typename fixed_list_base<T, Pool>::iterator rit = obj.begin();

   while ((lit != end()) && (rit != obj.end()))
   {
      *(lit++) = *(rit++);
   }
   while (mPool.outstanding() > obj.size())
   {
      pop_back();
   }
   while (rit != obj.end())
   {
      //Capacity was check above. We can use the quicker no_throw method.
      push_back_no_throw(*(rit++));
   }

   return *this;
}

template<class T, class Pool> void fixed_list_base<T, Pool>::pop_back()
{
   node* prev = mTail->prev;
   if (NULL != prev)
   {
      prev->next = NULL;
   }
   mPool.deallocate(mTail);
   mTail = prev;
}

template<class T, class Pool> void fixed_list_base<T, Pool>::push_back(const T& val)
{
   if (mPool.outstanding() == mPool.max_size())
   {
      throw std::runtime_error("fixed_list: push_back() caused size to exceed capacity");
   }

   push_back_no_throw(val);
}

template<class T, class Pool> void fixed_list_base<T, Pool>::push_back_no_throw(const T& val)
{
   node* next = mPool.allocate_no_throw();
   next->prev = mTail;
   next->val = val;
   next->next = NULL;
   if (NULL != mTail)
   {
      mTail->next = next;
   }
   mTail = next;
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::reverse_iterator fixed_list_base<T, Pool>::rbegin()
{
   return fixed_list_reverse_iterator < T > (mTail);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_reverse_iterator fixed_list_base<T, Pool>::rbegin() const
{
   return fixed_list_const_reverse_iterator < T > (mTail);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::reverse_iterator fixed_list_base<T, Pool>::rend()
{
   return fixed_list_reverse_iterator < T > (NULL);
}

template<class T, class Pool> typename fixed_list_base<T, Pool>::const_reverse_iterator fixed_list_base<T, Pool>::rend() const
{
   return fixed_list_const_reverse_iterator < T > (NULL);
}

template<class T, class Pool> size_t fixed_list_base<T, Pool>::size() const
{
   return mPool.outstanding();
}

template<class T, class Pool> void fixed_list_base<T, Pool>::swap(fixed_list_base<T, Pool>& obj)
{
   if ((obj.size() > mPool.max_size()) || (mPool.outstanding() > obj.max_size()))
   {
      throw std::runtime_error("fixed_list: swap() parameters' size exceed capacity");
   }

   size_t lsize = mPool.outstanding();
   size_t rsize = obj.size();
   if (lsize < rsize)
   {
      T tmp;
      typename fixed_list_base<T, Pool>::iterator lit = begin();
      typename fixed_list_base<T, Pool>::iterator rit = obj.begin();
      while (lit != end())
      {
         tmp = *lit;
         *lit = *rit;
         *rit = tmp;
         ++lit;
         ++rit;
      }

      typename fixed_list_base<T, Pool>::reverse_iterator rrit = obj.rbegin();
      while (mPool.outstanding() < rsize)
      {
         //Capacity check done at the top of method. Safe to do the faster no_throw method.
         push_back_no_throw(*(rrit++));
         obj.pop_back();
      }
      /*while (mPool.outstanding < rsize)
       {
       mPool.push_back(*(rit++));
       }
       while (obj.size() > lsize)
       {
       obj.pop_back();
       }*/
   }
   else
   {
      T tmp;
      typename fixed_list_base<T, Pool>::iterator lit = begin();
      typename fixed_list_base<T, Pool>::iterator rit = obj.begin();
      while (rit != obj.end())
      {
         tmp = *lit;
         *lit = *rit;
         *rit = tmp;
         ++lit;
         ++rit;
      }

      typename fixed_list_base<T, Pool>::reverse_iterator rlit = rbegin();
      while (obj.size() < lsize)
      {
         //Capacity check done at the top of method. Safe to do the faster no_throw method.
         obj.push_back_no_throw(*(rlit++));
         pop_back();
      }
      /*while (obj.size() < lsize)
       {
       obj.push_back(*(rlit++));
       }
       while (mPool.outstanding() > rsize)
       {
       pop_back();
       }*/
   }

}

template<class T, class Pool> fixed_list_base<T, Pool>::fixed_list_base() :
mHead(NULL), mTail(NULL), mPool()
{
}

template<class T, class Pool> fixed_list_base<T, Pool>::fixed_list_base(size_t size) :
mHead(NULL), mTail(NULL), mPool(size)
{
}

template<class T, class Pool>
bool operator==(const fixed_list_base<T, Pool>& lhs, const fixed_list_base<T, Pool>& rhs)
{
   if (lhs.size() != rhs.size())
   {
      return false;
   }
   else
   {
      typename fixed_list_base<T, Pool>::const_iterator lit = lhs.begin();
      typename fixed_list_base<T, Pool>::const_iterator rit = rhs.begin();
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

template<class T, class Pool>
bool operator<(const fixed_list_base<T, Pool>& lhs, const fixed_list_base<T, Pool>& rhs)
{
   if (lhs.size() < rhs.size())
   {
      typename fixed_list_base<T, Pool>::const_iterator lit = lhs.begin();
      typename fixed_list_base<T, Pool>::const_iterator rit = rhs.begin();
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
      typename fixed_list_base<T, Pool>::const_iterator lit = lhs.begin();
      typename fixed_list_base<T, Pool>::const_iterator rit = rhs.begin();
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

template<class T, class Pool>
bool operator!=(const fixed_list_base<T, Pool>& lhs, const fixed_list_base<T, Pool>& rhs)
{
   return !(lhs == rhs);
}

template<class T, class Pool>
bool operator>(const fixed_list_base<T, Pool>& lhs, const fixed_list_base<T, Pool>& rhs)
{
   return rhs < lhs;
}

template<class T, class Pool>
bool operator<=(const fixed_list_base<T, Pool>& lhs, const fixed_list_base<T, Pool>& rhs)
{
   return !(rhs < lhs);
}

template<class T, class Pool>
bool operator>=(const fixed_list_base<T, Pool>& lhs, const fixed_list_base<T, Pool>& rhs)
{
   return !(lhs < rhs);
}

#endif /* FIXED_LIST_BASE_H */
