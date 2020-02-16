// Copyright (c) 2016-2018, Michael Polukarov (Russia).
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// - Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer listed
//   in this license in the documentation and/or other materials
//   provided with the distribution.
//
// - Neither the name of the copyright holders nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include <cstdint>
#include <cstring>
#include <iterator>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <stdexcept>

#include "../platform/common.h"

#ifdef __EXPERIMENTAL_OPTIMIZATIONS
#ifdef _MSC_VER
#include <intrin.h>
#ifdef _M_X64
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanForward)
#else
#pragma intrinsic(_BitScanForward)
#endif
#pragma intrinsic(_mm_prefetch)
#define _PREFETCH_T0(adr)  _mm_prefetch(reinterpret_cast<const char*>(adr), _MM_HINT_T0);
#define _PREFETCH_NTA(adr) _mm_prefetch(reinterpret_cast<const char*>(adr), _MM_HINT_NTA);
#else
#include <x86intrin.h>
#define _PREFETCH_T0(adr)  _mm_prefetch(reinterpret_cast<const char*>(adr), _MM_HINT_T0);
#define _PREFETCH_NTA(adr) _mm_prefetch(reinterpret_cast<const char*>(adr), _MM_HINT_NTA);
#endif
#else
#define _PREFETCH_T0(adr)
#define _PREFETCH_NTA(adr)
#endif



_STDX_BEGIN

namespace detail
{
	
	template<
		typename _Key,
		typename _Hasher,
		typename _Keycomp,
		size_t _Size
	>
	struct packed_traits
	{
		static const size_t max_capacity = _Size;

		typedef _Key key_type;
		typedef _Key value_type;
		typedef _Hasher  hash_function;
		typedef _Keycomp key_compare;

	protected:
		static inline const key_type& key_of(const value_type& val) {
			return (val);
		}

		inline bool compare(const value_type& lhs, const value_type& rhs) const {
			return comparer(key_of(lhs), key_of(rhs));
		}

		inline size_t hash_value(const key_type& key) const {
			return (hasher(key) % max_capacity);
		}

		hash_function hasher;  // TODO: can be space optimized using EBO
		key_compare comparer;  // TODO: can be space optimized using EBO
	}; // end struct coalesced_traits<>

	template<
		typename _Key,
		typename _Value,
		typename _Hasher,
		typename _Keycomp,
		size_t _Size
	>
	struct packed_traits<std::pair<const _Key, _Value>, _Hasher, _Keycomp, _Size>
	{
		static constexpr size_t max_capacity = _Size;

		typedef _Key key_type;
		typedef typename std::pair<const _Key, _Value> value_type;
		typedef _Hasher hash_function;
		typedef _Keycomp key_compare;

	protected:
		static inline const key_type& key_of(const value_type& val) {
			return (val.first);
		}

		inline bool compare(const value_type& lhs, const value_type& rhs) const {
			return comparer(key_of(lhs), key_of(rhs));
		}

		inline bool compare(const key_type& lhs, const key_type& rhs) const {
			return comparer(lhs, rhs);
		}

		inline size_t hash_value(const key_type& key) const {
			return (hasher(key) % max_capacity);
		}

		hash_function hasher;  // TODO: can be space optimized using EBO
		key_compare comparer;  // TODO: can be space optimized using EBO
	}; // end struct coalesced_traits<>




	template<class _Traits>
	class packed_hashtbl :
		public _Traits
	{
		// class for basic free list logic
		class freelist
		{
		public:
			// construct with empty list
			freelist() : __m_head(nullptr) {
			}

			// push onto free list
			inline void push(void *__ptr) {
				reinterpret_cast<node*>(__ptr)->__m_next = __m_head;
				__m_head = reinterpret_cast<node*>(__ptr);
			}

			// pop node from free list
			inline void *pop() {
				void *__ptr = __m_head;
				if (__ptr != nullptr) { // relink
					__m_head = __m_head->__m_next;
				}
				return (__ptr);
			}

		private:
			struct node { // list node
				node *__m_next;
			};
			node *__m_head; // head node
		};

	public:
		typedef packed_hashtbl<_Traits> this_type;
		typedef _Traits traits_type;

		// non-standart
		static const size_t max_capacity = traits_type::max_capacity;

		typedef typename traits_type::key_type key_type;
		typedef typename traits_type::value_type value_type;
		typedef typename traits_type::hash_function hash_function;
		typedef typename traits_type::key_compare key_compare;

		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;

		typedef size_t size_type;

	private:
		struct node
		{   // warning: do NOT change the order of variabes!
			node* next;
			value_type value;
			node() : next(nullptr), value(value_type()) {}
			node(const_reference val) : next(nullptr), value(val) {}

#ifdef _HAS_CPP11X
			template<typename... _Args>
			node(_Args&&... args) :
				next(nullptr), value(std::forward(args)...){ 
			}
#endif
		};
		static constexpr uintptr_t expired_node = uintptr_t(-1);

		template<bool _Constant>
		struct node_iterator_base
		{
			typedef std::forward_iterator_tag iterator_category;
			typedef std::ptrdiff_t            difference_type;
			typedef typename std::conditional<_Constant,
				typename this_type::const_reference,
				typename this_type::reference
			>::type reference;
			typedef typename std::conditional<_Constant,
				typename this_type::const_pointer,
				typename this_type::pointer
			>::type pointer;
			typedef typename this_type::value_type value_type;

			inline reference operator*() const { return (__m_node->value); }
			inline pointer operator->() const { return std::addressof(__m_node->value); }

			friend inline bool operator== (const node_iterator_base& x, const node_iterator_base& y) {
				return (x.__m_node == y.__m_node);
			}

			friend inline bool operator!= (const node_iterator_base& x, const node_iterator_base& y) {
				return (x.__m_node != y.__m_node);
			}
		protected:
			node_iterator_base(node* e) :
				__m_node(e) {
			}
			inline void __next_node() { __m_node = __m_node->next; }
			node* __m_node;
		};

		template<bool _Constant>
		struct hashtable_iterator_base :
			public node_iterator_base<_Constant>
		{
		protected:
			hashtable_iterator_base(node* e, node** bucket) :
				node_iterator_base<_Constant>(e), __m_bucket(bucket) {
			}

			inline void __increment()
			{
				this->__next_node();
				if (!this->__m_node) {
					__next_bucket();
				}
			}

			inline void __next_bucket()
			{
				++__m_bucket;
				while (!*__m_bucket) { // This loop requires the bucket array to have a non-null sentinel.
					++__m_bucket;
				}
				this->__m_node = *__m_bucket;
			}
			node** __m_bucket;
		};

	public:
		class node_iterator :
			public node_iterator_base<false>
		{
			friend this_type;
			typedef node_iterator_base<false> base_type;

			node_iterator(node* e) :
				base_type(e) {}
		public:
			node_iterator() : base_type(nullptr) {}
			node_iterator(const node_iterator& _Other) :
				base_type(_Other.__m_node) {
			}
			inline node_iterator& operator++() {
				this->__next_node(); return (*this);
			}
			inline node_iterator operator++(int) {
				node_iterator tmp(*this);
				this->__next_node(); 
				return (tmp);
			}
		};

		class const_node_iterator :
			public node_iterator_base<true>
		{
			friend this_type;
			typedef node_iterator_base<true> base_type;
			const_node_iterator(node* e) :
				base_type(e) {}
		public:
			const_node_iterator() : base_type(nullptr) {}
			const_node_iterator(const node_iterator& _Other) :
				base_type(_Other.__m_node) {
			}
			inline const_node_iterator& operator++() {
				this->__next_node(); return (*this);
			}
			inline const_node_iterator operator++(int) {
				const_node_iterator tmp(*this);
				this->__next_node(); 
				return (tmp);
			}
		};
		typedef node_iterator local_iterator;
		typedef const_node_iterator const_local_iterator;


		class hashtable_iterator :
			public hashtable_iterator_base<false>
		{
			friend this_type;
			typedef hashtable_iterator_base<false> base_type;
			hashtable_iterator(node* e, node** buckets) :
				base_type(e, buckets) {}
			hashtable_iterator(node** buckets) :
				base_type(*buckets, buckets) {}
		public:
			hashtable_iterator() : base_type(nullptr, nullptr) {}
			hashtable_iterator(const hashtable_iterator& _Other) :
				base_type(_Other.__m_node, _Other.__m_bucket) {
			}
			inline hashtable_iterator& operator++() {
				this->__increment(); return (*this);
			}
			inline hashtable_iterator operator++(int) {
				hashtable_iterator tmp(*this);
				this->__increment(); 
				return (tmp);
			}
		};

		class const_hashtable_iterator :
			public hashtable_iterator_base<true>
		{
			friend this_type;
			typedef hashtable_iterator_base<true> base_type;
			const_hashtable_iterator(node* e, node** buckets) :
				base_type(e, buckets) {}
			const_hashtable_iterator(node** buckets) :
				base_type(*buckets, buckets) {}
		public:
			const_hashtable_iterator() : base_type(nullptr, nullptr) {}
			const_hashtable_iterator(const const_hashtable_iterator& _Other) :
				base_type(_Other.__m_node, _Other.__m_bucket) {
			}
			inline const_hashtable_iterator& operator++() {
				this->__increment(); return (*this);
			}
			inline const_hashtable_iterator operator++(int) {
				const_hashtable_iterator tmp(*this);
				this->__increment(); 
				return (tmp);
			}
		};
		typedef hashtable_iterator iterator;
		typedef const_hashtable_iterator const_iterator;

		packed_hashtbl() {
			__construct();
		}

		packed_hashtbl(const packed_hashtbl& other) {
			__construct();
			insert(other.begin(), other.end());
		}

		packed_hashtbl& operator= (const packed_hashtbl& other) {
			if (std::addressof(other) != this) {
				clear();
				insert(other.begin(), other.end());
			}
			return (*this);
		}

		template<class _It>
		packed_hashtbl(_It __first, _It __last) {
			__construct();
			insert(__first, __last);
		}

		~packed_hashtbl()
		{
			for (size_t i = 0; i < max_capacity; i++) {
				__destroy_node(std::addressof(__m_slots[i]));
			}
		}

		inline local_iterator begin(size_t __bucket) {
			return local_iterator(__m_buckets[__bucket]);
		}

		inline const_local_iterator begin(size_t __bucket) const {
			return const_local_iterator(__m_buckets[__bucket]);
		}

		inline const_local_iterator cbegin(size_t __bucket) const {
			return const_local_iterator(__m_buckets[__bucket]);
		}

		inline local_iterator end(size_t) {
			return local_iterator();
		}

		inline const_local_iterator end(size_t) const {
			return const_local_iterator();
		}

		inline const_local_iterator cend(size_t) const {
			return const_local_iterator();
		}


		inline iterator begin() {
			iterator it(const_cast<node**>(__m_buckets));
			if (!it.__m_node) it.__next_bucket();
			return it;
		}

		inline const_iterator begin() const {
			const_iterator it(const_cast<node**>(__m_buckets));
			if (!it.__m_node) it.__next_bucket();
			return it;
		}

		inline const_iterator cbegin() const {
			const_iterator it(const_cast<node**>(__m_buckets));
			if (!it.__m_node) it.__next_bucket();
			return it;
		}

		inline iterator end() {
			return __make_iterator(reinterpret_cast<const node*>(expired_node), max_capacity);
		}

		inline const_iterator end() const {
			return __make_iterator(reinterpret_cast<const node*>(expired_node), max_capacity);
		}

		inline const_iterator cend() const {
			return __make_iterator(reinterpret_cast<const node*>(expired_node), max_capacity);
		}

		inline size_t count(const key_type& key) {
			return static_cast<size_t>(__lookup(__m_buckets[traits_type::hash_value(key)], key) != nullptr);
		}

		inline size_t bucket(const key_type& __key) const {
			return (traits_type::hash_value(__key));
		}

		inline size_t bucket_size(size_t __i) const {
			return std::distance(begin(__i), end(__i));
		}

		inline size_type max_bucket_count() const { return max_capacity; }
		inline size_t bucket_count() const { return max_capacity; }

		inline size_t capacity() const { return max_capacity; }
		inline size_t max_size() const { return max_capacity; }
		inline size_t size() const { return __m_size; }

		float load_factor() const {
			return ((float)__m_size / (float)max_capacity);
		}

		inline bool empty() const { return (__m_size == 0); }
		inline bool full() const { return (__m_size == max_capacity); }

		inline void clear()
		{
			__m_size = 0;
			__m_buckets[max_capacity] = reinterpret_cast<node*>(expired_node);
			std::memset(__m_buckets, 0, sizeof(node*) * max_capacity);
			for (node* it = __m_slots, *end = (__m_slots + max_capacity); it != end; ++it) {
				__m_freeslots.push(it);
			}
		}


		const_iterator find(const key_type& key) const
		{
			size_t hash_code = traits_type::hash_value(key);
			const node* e = __lookup(__m_buckets[hash_code], key);
			return (e != nullptr ? __make_iterator(e, hash_code) : end());
		}

		iterator find(const key_type& key)
		{
			size_t hash_code = traits_type::hash_value(key);
			const node* e = __lookup(__m_buckets[hash_code], key);
			return (e != nullptr ? __make_iterator(e, hash_code) : end());
		}

		std::pair<iterator, iterator> equal_range(const key_type& __key) {
			iterator it = find(__key);
			return (it != end() ? std::make_pair(it, ++it) : std::make_pair(end(), end()));
		}

		std::pair<const_iterator, const_iterator> equal_range(const key_type& __key) const {
			const_iterator it = find(__key);
			return (it != end() ? std::make_pair(it, ++it) : std::make_pair(end(), end()));
		}

		template<class _It>
		void insert(_It __first, _It __last) {
			for (; __first != __last; ++__first) {
				this_type::insert(*__first);
			}
		}

		std::pair<iterator, bool> insert(const_reference __val)
		{
			if (full()) throw std::overflow_error("coalesced hash overflow");
			size_t hash_code = traits_type::hash_value(traits_type::key_of(__val));
			node* entry = __insert_node(__val, hash_code);
			return std::make_pair(__make_iterator(entry, hash_code), entry != nullptr);
		}

#ifdef _HAS_CPP11X
#if 0
		template<typename... _Args>
		std::pair<iterator, bool> emplace(_Args&&... __args) {
			// not implemented
		}
#endif
#endif

		size_type erase(const key_type& __key) {
			return __erase_node(__key);
		}

		iterator erase(const_iterator __pos) {
			return iterator(__erase_node(__pos.__m_node, __pos.__m_bucket), __pos.__m_bucket);
		}

		iterator erase(const_iterator __first, const_iterator __last) {
			iterator result;
			for (; __first != __last; ++__first) {
				result = erase(__first);
			}
			return result;
		}

	protected:
		inline iterator __make_iterator(const node* __e, size_t __i) {
			return iterator(const_cast<node*>(__e), const_cast<node**>(__m_buckets + __i));
		}

		inline const_iterator __make_iterator(const node* __e, size_t __i) const {
			return const_iterator(const_cast<node*>(__e), const_cast<node**>(__m_buckets + __i));
		}

		inline void __construct()
		{
			__m_size = 0;
			__m_buckets[max_capacity] = reinterpret_cast<node*>(expired_node);
			std::memset(__m_buckets, 0, sizeof(node*) * max_capacity);
			for (node* it = __m_slots, *end = (__m_slots + max_capacity); it != end; ++it) {
				__m_freeslots.push(it);
			}
		}

		// allocate and initialize a new node using value reference
		inline node* __create_node(const_reference __val) {
			++__m_size;
			// pop pointer from free list and allocate node in place
			return ::new(__m_freeslots.pop()) node(__val);
		}

#ifdef _HAS_CPP11X
		// allocate and initialize a new node using perfect forwarding
		template<typename... _Args>
		inline node* __create_node(_Args&&... args) {
			++__m_size;
			// pop pointer from free list and allocate node in place
			return ::new(__m_freeslots.pop()) node(std::forward(args)...);
		}
#endif

		// mark node as deleted and deallocate
		inline void __destroy_node(node* entry) {
            entry->next = nullptr;
            //entry->value.~value_type(); // call value dtor
			__m_freeslots.push(entry); // push pointer to free list
			--__m_size;
		}

		// detach node from neighboring nodes
		inline void __detach_node(node* prev, node* entry) {
			if (prev) { prev->next = entry->next; }
		}


		inline node* __insert_node(const_reference __val, size_t __hash_code)
		{
			_PREFETCH_T0(__m_buckets); // ###
			node* entry = __m_buckets[__hash_code];
			if (!entry) {
				entry = __create_node(__val);
				__m_buckets[__hash_code] = entry;
				return entry;
			}

			if (traits_type::compare(__val, entry->value)) {
				return nullptr;
			}

			while (entry->next) {
				if (traits_type::compare(__val, entry->next->value)) {
					return nullptr;
				}
				entry = entry->next;
			}

			entry->next = __create_node(__val);
			return entry->next;
		}


		// find node by key and hash_code
		inline const node* __lookup(const node* entry, const key_type& key) const
		{
			//_PREFETCH_T0(__m_buckets); // ###
			while (entry) {
				if (traits_type::compare(key, traits_type::key_of(entry->value))) {
					break;
				}
				entry = entry->next;
			}
			return entry;
		}

		// erase node by key
		inline size_type __erase_node(const key_type& key)
		{
			size_t hash_code = traits_type::hash_value(key);
			_PREFETCH_T0(__m_buckets); // ###
			node** head = std::addressof(__m_buckets[hash_code]);
			node* entry = *head;
			node* prev = nullptr;
			while (entry) {
				if (traits_type::compare(key, traits_type::key_of(entry->value))) {
					break;
				}
				prev = entry;
				entry = entry->next;
			}

			if (!entry) // entry was not found
				return 0;

			if (entry == *head) { // reset bucket pointer
				*head = entry->next;
			}
			__detach_node(prev, entry); // detach node from neighboring nodes
			__destroy_node(entry); // destroy node (if any)
			return 1;
		}

		// erase node at position
		inline node* __erase_node(node* pos, node** head)
		{
			node* next = nullptr;
			node* prev = *head;
			if (pos == prev) {
				*head = pos->next;
				__detach_node(nullptr, pos); // ???
				__destroy_node(pos);
				return *head;
			}

			while (prev->next != pos) {
				prev = prev->next;
			}

			next = pos->next;
			__detach_node(prev, pos);
			__destroy_node(pos);
			return next;
		}


	private:
		node      __m_slots[max_capacity]; // array of slots
		node*     __m_buckets[max_capacity + 1]; // array of buckets
		freelist  __m_freeslots; // free list of slots
		size_t    __m_size; // number of elements

	}; // end class packed_hashtbl<>

} // end namespace detail





template<
	class _Key,
	size_t _Size,
	class _Hasher = std::hash<_Key>,
	class _Comparer = std::equal_to<_Key>
>
class packed_hashset :
	public detail::packed_hashtbl<
	detail::packed_traits<
	_Key, _Hasher, _Comparer, _Size
	>
	>
{
	typedef  detail::packed_hashtbl<
		detail::packed_traits<
		_Key, _Hasher, _Comparer, _Size
		>
	> base_type;

public:
	typedef typename base_type::key_type key_type;
	typedef typename base_type::value_type value_type;
	typedef typename base_type::hash_function hash_function;
	typedef typename base_type::key_compare key_compare;

	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::const_pointer const_pointer;

	typedef typename base_type::size_type size_type;
	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;

	packed_hashset() : base_type() {}
	packed_hashset(const packed_hashset& other) : base_type(other) {}
	packed_hashset& operator=(const packed_hashset& other) {
		return base_type::operator=(other);
	}
};


template<
	class _Key,
	class _Value,
	size_t _Size,
	class _Hasher = std::hash<_Key>,
	class _Comparer = std::equal_to<_Key>
>
class packed_hashmap :
	public detail::packed_hashtbl<
	detail::packed_traits<
	std::pair<const _Key, _Value>,
	_Hasher, _Comparer, _Size
	>
	>
{
	typedef detail::packed_hashtbl<
		detail::packed_traits<
		std::pair<const _Key, _Value>,
		_Hasher, _Comparer, _Size
		>
	> base_type;

public:
	typedef typename base_type::key_type key_type;
	typedef typename base_type::value_type value_type;
	typedef typename base_type::hash_function hash_function;
	typedef typename base_type::key_compare key_compare;

	typedef typename base_type::reference reference;
	typedef typename base_type::const_reference const_reference;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::const_pointer const_pointer;

	typedef typename base_type::size_type size_type;
	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;

	packed_hashmap() : base_type() {}
	packed_hashmap(const packed_hashmap& other) : base_type(other) {}
	packed_hashmap& operator=(const packed_hashmap& other) {
		return base_type::operator=(other);
	}
};



_STDX_END

