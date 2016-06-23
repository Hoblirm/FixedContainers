/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a hash_map and hash_multimap which use a fixed size 
// memory pool for its buckets and nodes. 
///////////////////////////////////////////////////////////////////////////////


#ifndef FLEX_FIXED_HASH_MAP_H
#define FLEX_FIXED_HASH_MAP_H


#include <flex/hash_map.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
#pragma once // Some compilers (e.g. VC++) benefit significantly from using this. We've measured 3-4% build speed improvements in apps as a result.
#endif



namespace flex
{

   /// fixed_hash_map
   ///
   /// Implements a hash_map with a fixed block of memory identified by the nodeCount and bucketCount
   /// template parameters. 
   ///
   /// Template parameters:
   ///     Key                    The key type for the map. This is a map of Key to T (value).
   ///     T                      The value type for the map.
   ///     nodeCount              The max number of objects to contain. This value must be >= 1.
   ///     bucketCount            The number of buckets to use. This value must be >= 2.
   ///     Hash                   hash_set hash function. See hash_set.
   ///     Predicate              hash_set equality testing function. See hash_set.
   ///

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount = nodeCount + 1,
           typename Hash = std::hash<Key>, typename Predicate = std::equal_to<Key>, typename Allocator = flex::allocator<char>, bool bCacheHashCode = false >
           class fixed_hash_map : public hash_map<Key,
           T,
           Hash,
           Predicate,
           Allocator,
           bCacheHashCode>
   {
   public:
      typedef Allocator fixed_allocator_type;
      typedef hash_map<Key, T, Hash, Predicate, fixed_allocator_type, bCacheHashCode> base_type;
      typedef fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode> this_type;
      typedef typename base_type::value_type value_type;
      typedef typename base_type::node_type node_type;
      typedef typename base_type::size_type size_type;

      enum
      {
         kMaxSize = nodeCount
      };

      using base_type::mAllocator;
      using base_type::mNodePool;
      using base_type::mOverflow;
      using base_type::clear;
      
   protected:
      node_type* mBucketBuffer[bucketCount + 1]; // '+1' because the hash table needs a null terminating bucket.

#ifdef FLEX_HAS_CXX11
      typename std::aligned_storage<sizeof (node_type), alignof(node_type)>::type mNodeBuffer[nodeCount];
#else

      union
      {
         char mNodeBuffer[nodeCount * sizeof (node_type)];
         long double dummy;
      };
#endif

   public:

      explicit fixed_hash_map(const Hash& hashFunction = Hash(),
              const Predicate& predicate = Predicate());

      template <typename InputIterator>
      fixed_hash_map(InputIterator first, InputIterator last,
              const Hash& hashFunction = Hash(),
              const Predicate& predicate = Predicate());

      fixed_hash_map(const this_type& x);
#if FLEX_HAS_CXX11
      fixed_hash_map(this_type && x);
#endif
      fixed_hash_map(std::initializer_list<value_type> ilist);

      ~fixed_hash_map();

      this_type& operator=(const this_type& x);
      this_type& operator=(std::initializer_list<value_type> ilist);
#if FLEX_HAS_CXX11
      this_type& operator=(this_type && x);
#endif

      void swap(this_type& x);

      void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.

      size_type max_size() const;

   }; // fixed_hash_map



   /// fixed_hash_multimap
   ///
   /// Implements a hash_multimap with a fixed block of memory identified by the nodeCount and bucketCount
   /// template parameters. 
   ///
   /// Template parameters:
   ///     Key                    The key type for the map. This is a map of Key to T (value).
   ///     T                      The value type for the map.
   ///     nodeCount              The max number of objects to contain. This value must be >= 1.
   ///     bucketCount            The number of buckets to use. This value must be >= 2.
   ///     Hash                   hash_set hash function. See hash_set.
   ///     Predicate              hash_set equality testing function. See hash_set.
   ///

   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount = nodeCount + 1,
   //           typename Hash = std::hash<Key>, typename Predicate = std::equal_to<Key>, typename Allocator = flex::allocator<char>, bool bCacheHashCode = false>
   //           class fixed_hash_multimap : public hash_multimap<Key,
   //           T,
   //           Hash,
   //           Predicate,
   //           Allocator,
   //           bCacheHashCode>
   //   {
   //   public:
   //      typedef Allocator fixed_allocator_type
   //      typedef hash_multimap<Key, T, Hash, Predicate, fixed_allocator_type, bCacheHashCode> base_type;
   //      typedef fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode> this_type;
   //      typedef typename base_type::value_type value_type;
   //      typedef typename base_type::node_type node_type;
   //      typedef typename base_type::size_type size_type;
   //
   //      enum
   //      {
   //         kMaxSize = nodeCount
   //      };
   //
   //      using base_type::mAllocator;
   //
   //   protected:
   //      node_type* mBucketBuffer[bucketCount + 1]; // '+1' because the hash table needs a null terminating bucket.
   //      char mNodeBuffer[fixed_allocator_type::kBufferSize]; // kBufferSize will take into account alignment requirements.
   //
   //   public:
   //
   //      explicit fixed_hash_multimap(const Hash& hashFunction = Hash(),
   //              const Predicate& predicate = Predicate());
   //
   //      template <typename InputIterator>
   //      fixed_hash_multimap(InputIterator first, InputIterator last,
   //              const Hash& hashFunction = Hash(),
   //              const Predicate& predicate = Predicate());
   //
   //      fixed_hash_multimap(const this_type& x);
   //#if FLEX_HAS_CXX11
   //      fixed_hash_multimap(this_type && x);
   //#endif
   //      fixed_hash_multimap(std::initializer_list<value_type> ilist);
   //
   //      this_type& operator=(const this_type& x);
   //      this_type& operator=(std::initializer_list<value_type> ilist);
   //#if FLEX_HAS_CXX11
   //      this_type& operator=(this_type && x);
   //#endif
   //
   //      void swap(this_type& x);
   //
   //      void reset_lose_memory(); // This is a unilateral reset to an initially empty state. No destructors are called, no deallocation occurs.
   //
   //      size_type max_size() const;
   //
   //   }; // fixed_hash_multimap


   ///////////////////////////////////////////////////////////////////////
   // fixed_hash_map
   ///////////////////////////////////////////////////////////////////////

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   fixed_hash_map(const Hash& hashFunction,
           const Predicate& predicate)
   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction,
   predicate, fixed_allocator_type(), mBucketBuffer, (node_type*) mNodeBuffer, ((node_type*) mNodeBuffer) + nodeCount)
   {
      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.
   }

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   template <typename InputIterator>
   fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   fixed_hash_map(InputIterator first, InputIterator last,
           const Hash& hashFunction,
           const Predicate& predicate)
   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction,
   predicate, fixed_allocator_type(), mBucketBuffer, (node_type*) mNodeBuffer, ((node_type*) mNodeBuffer) + nodeCount)
   {
      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

      base_type::insert(first, last);
   }

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   fixed_hash_map(const this_type& x)
   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(),
   x.equal_function(), fixed_allocator_type(), mBucketBuffer, (node_type*) mNodeBuffer, ((node_type*) mNodeBuffer) + nodeCount)
   {
      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

      base_type::insert(x.begin(), x.end());
   }


#if FLEX_HAS_CXX11

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   fixed_hash_map(this_type && x)
   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(),
   x.equal_function(), fixed_allocator_type(), mBucketBuffer, (node_type*) mNodeBuffer, ((node_type*) mNodeBuffer) + nodeCount)
   {
      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

      base_type::insert(x.begin(), x.end());
   }
#endif

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   fixed_hash_map(std::initializer_list<value_type> ilist)
   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), Hash(),
   Predicate(), fixed_allocator_type(), mBucketBuffer, (node_type*) mNodeBuffer, ((node_type*) mNodeBuffer) + nodeCount)
   {
      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.

      base_type::insert(ilist.begin(), ilist.end());
   }

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   ~fixed_hash_map()
   {
#ifndef FLEX_RELEASE
      if (FLEX_UNLIKELY(mOverflow))
      {
         //If the fixed list overflowed, we want to clean out the nodes that were allocated.
         clear(); //Perform a clear which moves everything to the node pool
         while (mNodePool != NULL)
         {
            node_type* next = static_cast<node_type*> (mNodePool->mpNext);
            //A node was allocated if it is outside the range of buffer.  Remember mNodeBuffer + nodeCount
            //denotes the end() iterator.  Therefore it is possible that an allocated node could be
            //equal to it.
            if ((mNodePool < (node_type*) mNodeBuffer) || (mNodePool >= ((node_type*) mNodeBuffer) + nodeCount))
            {
               mAllocator.deallocate((char*) mNodePool, sizeof (node_type));
            }
            mNodePool = next;
         }
      }
#endif
   }

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
           inline typename fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::this_type&
           fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::operator=(const this_type& x)
   {
      base_type::operator=(x);
      return *this;
   }


#if FLEX_HAS_CXX11

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
           inline typename fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::this_type&
           fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::operator=(this_type && x)
   {
      base_type::operator=(x);
      return *this;
   }
#endif

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
           inline typename fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::this_type&
           fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::operator=(std::initializer_list<value_type> ilist)
   {
      base_type::clear();
      base_type::insert(ilist.begin(), ilist.end());
      return *this;
   }

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline void fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   swap(this_type& x)
   {
      base_type::swap(x);
   }

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline void fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   reset_lose_memory()
   {
      base_type::reset_lose_memory();
      base_type::get_allocator().reset(mNodeBuffer);
   }

   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   inline typename fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::size_type
   fixed_hash_map<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::max_size() const
   {
      return kMaxSize;
   }

   ///////////////////////////////////////////////////////////////////////
   // global operators
   ///////////////////////////////////////////////////////////////////////

   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode>
   //   inline void swap(fixed_hash_map<Key, T, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& a,
   //           fixed_hash_map<Key, T, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& b)
   //   {
   //      a.swap(b);
   //   }




   ///////////////////////////////////////////////////////////////////////
   // fixed_hash_multimap
   ///////////////////////////////////////////////////////////////////////

   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   inline fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   //   fixed_hash_multimap(const Hash& hashFunction,
   //           const Predicate& predicate)
   //   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction,
   //   predicate, fixed_allocator_type())
   //   {
   //      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
   //      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.
   //
   //      mAllocator.reset(mNodeBuffer);
   //   }
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   template <typename InputIterator>
   //   fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   //   fixed_hash_multimap(InputIterator first, InputIterator last,
   //           const Hash& hashFunction,
   //           const Predicate& predicate)
   //   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), hashFunction,
   //   predicate, fixed_allocator_type())
   //   {
   //      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
   //      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.
   //
   //      mAllocator.reset(mNodeBuffer);
   //      base_type::insert(first, last);
   //   }
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   inline fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   //   fixed_hash_multimap(const this_type& x)
   //   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(),
   //   x.equal_function(), fixed_allocator_type())
   //   {
   //      mAllocator.copy_overflow_allocator(x.mAllocator);
   //
   //      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
   //      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.
   //
   //      mAllocator.reset(mNodeBuffer);
   //      base_type::insert(x.begin(), x.end());
   //   }
   //
   //
   //#if FLEX_HAS_CXX11
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   inline fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   //   fixed_hash_multimap(this_type && x)
   //   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), x.hash_function(),
   //   x.equal_function(), fixed_allocator_type())
   //   {
   //      // This implementation is the same as above. If we could rely on using C++11 delegating constructor support then we could just call that here.
   //      mAllocator.copy_overflow_allocator(x.mAllocator);
   //
   //      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
   //      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.
   //
   //      mAllocator.reset(mNodeBuffer);
   //      base_type::insert(x.begin(), x.end());
   //   }
   //#endif
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   inline fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   //   fixed_hash_multimap(std::initializer_list<value_type> ilist)
   //   : base_type(prime_rehash_policy::GetPrevBucketCountOnly(bucketCount), Hash(),
   //   Predicate(), fixed_allocator_type())
   //   {
   //      FLEX_ASSERT((nodeCount >= 1) && (bucketCount >= 2));
   //      base_type::set_max_load_factor(10000.f); // Set it so that we will never resize.
   //
   //      mAllocator.reset(mNodeBuffer);
   //      base_type::insert(ilist.begin(), ilist.end());
   //   }
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //           inline typename fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::this_type&
   //           fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::operator=(const this_type& x)
   //   {
   //      base_type::operator=(x);
   //      return *this;
   //   }
   //
   //
   //#if FLEX_HAS_CXX11
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //           inline typename fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::this_type&
   //           fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::operator=(this_type && x)
   //   {
   //      base_type::operator=(x);
   //      return *this;
   //   }
   //#endif
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //           inline typename fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::this_type&
   //           fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::operator=(std::initializer_list<value_type> ilist)
   //   {
   //      base_type::clear();
   //      base_type::insert(ilist.begin(), ilist.end());
   //      return *this;
   //   }
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   inline void fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   //   swap(this_type& x)
   //   {
   //      base:type::swap(x);
   //   }
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   inline void fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::
   //   reset_lose_memory()
   //   {
   //      base_type::reset_lose_memory();
   //      base_type::get_allocator().reset(mNodeBuffer);
   //   }
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, typename Hash, typename Predicate, typename Allocator, bool bCacheHashCode>
   //   inline typename fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::size_type
   //   fixed_hash_multimap<Key, T, nodeCount, bucketCount, Hash, Predicate, Allocator, bCacheHashCode>::max_size() const
   //   {
   //      return kMaxSize;
   //   }
   //
   //   ///////////////////////////////////////////////////////////////////////
   //   // global operators
   //   ///////////////////////////////////////////////////////////////////////
   //
   //   template <typename Key, typename T, size_t nodeCount, size_t bucketCount, bool bEnableOverflow, typename Hash, typename Predicate, bool bCacheHashCode>
   //   inline void swap(fixed_hash_multimap<Key, T, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& a,
   //           fixed_hash_multimap<Key, T, nodeCount, bucketCount, bEnableOverflow, Hash, Predicate, bCacheHashCode>& b)
   //   {
   //     a.swap(b);
   //   }


} // namespace flex

#endif // Header include guard