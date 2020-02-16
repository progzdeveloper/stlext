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
#include <array>
#include <bitset>
#include <functional>

#include "../platform/common.h"

#include "packed_hashtbl.hpp"


_STDX_BEGIN

template<
	typename _Key,
	typename _Value,
	size_t _Size,
	typename _Hasher = std::hash<_Key>,
	typename _Deleter = empty_deleter<_Value> // ???
>
class fixed_lru_cache
{
	static_assert(_Size >= 4, "size of lru cache too small");

	typedef fixed_lru_cache this_type;
public:
	typedef _Key   key_type;
	typedef _Value mapped_type;
	typedef _Hasher  hasher;
	typedef _Deleter deleter;


private:
	struct node
	{
		node() :
			pval(nullptr), pkey(nullptr),
			prev(nullptr), next(nullptr), cost(1) {
		}

		node(const mapped_type* val, size_t c = 1) :
			pval(val), pkey(nullptr),
			prev(nullptr), next(nullptr), cost(c) {
		}

		const mapped_type* pval;
		const key_type*    pkey;
		node*  prev;
		node*  next;
		size_t cost;
	};

	typedef packed_hashmap<key_type, node, _Size, hasher> nodemap;

public:
	// constant non-mutable iterator (mostly for debugging purposes)
	class _ConstIterator
	{
		friend class this_type;

		_ConstIterator(const node* n) :
			curr(n) {
		}
	public:
		typedef std::pair<const _Key, _Value> value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef value_type value_type;

		typedef std::bidirectional_iterator_tag iterator_category;

		_ConstIterator() : 
			curr(nullptr) {
		}

		_ConstIterator(const _ConstIterator& other) :
			curr(other.curr) {
		}

		inline _ConstIterator& operator= (const _ConstIterator& other)
		{
			if (std::addressof(other) == this) // self assignment
				return (*this);

			curr = other.curr;
			return (*this);
		}

		inline _ConstIterator& operator++() {
			next();
			return *this;
		}
		inline _ConstIterator operator++(int) {
			_ConstIterator tmp(*this);
			next();
			return tmp;
		}

		inline _ConstIterator& operator--() {
			prev();
			return (*this);
		}

		inline _ConstIterator operator--(int) {
			_ConstIterator tmp = *this;
			prev();
			return tmp;
		}

		inline size_t cost() const { return (curr != nullptr ? curr->cost : 0); }
		inline const key_type& key() const { return (*(curr->pkey)); }
		inline const mapped_type& value() const { return (*(curr->pval)); }

		inline const mapped_type& operator*() const { return value(); }
		inline const mapped_type* operator->() const { return (curr->pval); }

		friend bool operator == (const _ConstIterator& x, const _ConstIterator& y) { return (x.curr == y.curr); }
		friend bool operator != (const _ConstIterator& x, const _ConstIterator& y) { return (x.curr != y.curr); }

	private:
		inline void next() { curr = curr->next; }
		inline void prev() { curr = curr->prev; }
		const node* curr;
	};
	typedef _ConstIterator const_iterator;


	fixed_lru_cache(size_t _MaxCost = 128) :
		_maxcost(_MaxCost),
		_cost(0)
	{
		reinit();
	}

	fixed_lru_cache(const deleter& _Deleter, const hasher& _Hash, size_t _MaxCost = 128) :
		_lookup(_Hash), _deleter(_Deleter),
		_maxcost(_MaxCost), _cost(0)
	{
		reinit();
	}

	~fixed_lru_cache() {
		clear();
	}

	// return iterator pointing to begin of cache
	inline const_iterator begin() const { return _ConstIterator(_head); }

	// return iterator pointing to end of cache
	inline const_iterator end() const { return _ConstIterator(nullptr); }

	// insert new element with key, value and specified cost (1 by default)
	bool insert(const key_type& key, const mapped_type* value, size_t cost = 1)
	{
		typedef typename nodemap::iterator nodemap_iterator;

#ifdef _DEBUG
		++mrefcnt;
#endif

		// validate input arguments
		if ((value == nullptr) || (cost > _maxcost))
			return false;

		// search for key
		nodemap_iterator it = _lookup.find(key);
		// fetch pointer to entry
		node* entry = (it != _lookup.end() ? &(it->second) : nullptr);

		// trim cache until it fits max cost

		// since removing nodes from hash table
		// can invalidate node pointers 
		// we use some tricks (see pop_back() for details)

		// while expression is also tricky:
		// _cost value is modified by pop_back()
		// so (_cost + cost) really holds in a temporal 
		// variable witch than compared with _maxcost
		while ((_cost + cost) > _maxcost) {
			entry = pop_back(entry); // pop node and reassign entry pointer
		}

		if (entry) {
			_cost -= entry->cost;
			entry->cost = cost;
			entry->pval = value;
			move_front(entry);
#ifdef _DEBUG
			++mhitcnt;
#endif
		}
		else {
			it = _lookup.insert(std::make_pair(key, node(value, cost)));
			entry = &(it->second);
			entry->pkey = &(it->first);
			push_front(entry);
		}
		_cost += cost;

		return true;
	}

	// erase element with specified key from the cache
	// return number of erased elements (1 in most of cases)
	inline size_t erase(const key_type& key)
	{
		typename nodemap::iterator it = _lookup.find(key);
		if (it != _lookup.end())
		{
			node* entry = &(it->second); // fetch node pointer
			detach(entry); // detach list node
			destroy(entry->pval); // destroy value
			_cost -= entry->cost; // decrease cost
			// erase element from lookup
			return _lookup.erase(key);
		}
		return 0;
	}

	// erase element with specified key from the cache
	// using specified destroyer
	template<typename _Destroyer>
	inline const _Destroyer& erase(const key_type& key, const _Destroyer& destroyer)
	{
		typename nodemap::iterator it = _lookup.find(key);
		if (it != _lookup.end())
		{
			node* entry = &(it->second);  // fetch node pointer
			detach(entry); // detach list node
			// call destroyer on value pointer
			destroyer(const_cast<mapped_type*>(entry->pval));
			_cost -= entry->cost; // decrease cost
			// erase element from lookup
			_lookup.erase(key);
		}
		return destroyer; // return destroyer
	}

	// access to the element with specified key
	// side-effects: the element will be moved to front of the cache
	inline const mapped_type* object(const key_type& key) const
	{
#ifdef _DEBUG
		++mrefcnt;
#endif
		typename nodemap::const_iterator it = _lookup.find(key);
		if (it != _lookup.end()) {
#ifdef _DEBUG
			++mhitcnt;
#endif
			const node* entry = (&(it->second));
			const_cast<this_type*>(this)->move_front(const_cast<node*>(entry));
			return (entry->pval);
		}
		return (nullptr);
	}

	// access to the element with specified key
	// side-effects: the element will be moved to front of the cache
	inline const mapped_type* operator[](const key_type& key) const {
		return object(key);
	}

	// clear cache anddestroy all elements
	void clear()
	{
		typedef typename nodemap::reference reference;

		std::for_each(_lookup.begin(), _lookup.end(), [this](reference item) {
			destroy(item.second.pval);
		});
		_lookup.clear();

		reinit();
	}

	// return maximum cost of all elements in the cache
	inline size_t max_cost() const { return _maxcost; }

	// return current cost of all elements in the cache
	inline size_t cost() const { return _cost; }

	// return number of elements that cache currently holds
	inline size_t size() const { return _lookup.size(); }

	// return max number of elements that chache can hold 
	inline size_t max_size() const { return _lookup.get_allocator().max_size(); }

	// return true if cache is empty otherwise return false
	inline bool empty() const { return _lookup.empty(); }

#ifdef _DEBUG
	inline size_t hit_count() const { return mhitcht; }

	inline size_t miss_count() const { return (mrefcnt - mhitcnt); }

	inline size_t ref_count() const { return mrefcnt; }

	inline double hit_rate() const { return ((double)ref_count() / (double)hit_count()); }
#endif

private:
	// detach entry from neibourhood nodes
	inline void detach(node* entry)
	{
		if (!entry)
			return;

		if (entry->prev) entry->prev->next = entry->next;
		if (entry->next) entry->next->prev = entry->prev;
		if (_tail == entry) _tail = entry->prev;
		if (_head == entry) _head = entry->next;
	}

	// move entry to front of the list
	inline void move_front(node* entry)
	{
		detach(entry);
		push_front(entry);
	}

	// insert entry to front of the list
	inline void push_front(node* entry)
	{
		if (!entry)
			return;

		if (_head)
			_head->prev = entry;

		entry->prev = nullptr;
		entry->next = _head;
		_head = entry;

		if (!_tail)
			_tail = _head;
	}

	// pop node from list tail and from hash table
	// note: removing node from hash table 
	// can invalidate pointer hint
	// in this pop_back() return null pointer
	// to indicate that hint pointer is no longer actual
	// otherwise it returns same as hint pointer
	node* pop_back(node* hint)
	{
		if (_tail == nullptr) // assume this never can happen
			return nullptr;

		// initializing a indicator pointer:
		// if hint and tail addressed to the same memory
		// block than indicator pointer is null pointer
		// otherwise ret is a hint pointer
		node* ret = (hint == _tail ? nullptr : hint);

		node* entry = _tail;

		detach(_tail); // detach tail
		destroy(entry->pval); // delete node value
		_cost -= entry->cost; // decrease cost value

		_lookup.erase(*entry->pkey); // erase node from lookup

		return ret; // return indicator pointer
	}

	// reinit list (initialize head and tail pointers to null)
	inline void reinit() {
		_head = _tail = nullptr;
	}

	inline void destroy(const mapped_type* ptr) {
		// cast to non-cost pointer to make deleter works
		_deleter(const_cast<mapped_type*>(ptr));
	}

private:
	node *_head, *_tail;
	nodemap _lookup;
	deleter _deleter;
	size_t _maxcost;
	size_t _cost;

#ifdef _DEBUG
	size_t mrefcnt;
	size_t mhitcnt;
#endif
};


_STDX_END

