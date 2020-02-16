// Copyright (c) 2016, Michael Polukarov (Russia).
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
#include <list>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <functional>

#include "../platform/common.h"


_STDX_BEGIN

template<
class _Key,
        class _Value,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
        struct cachemap_traits
{
    typedef _Key   key_type;
    typedef _Value mapped_type;
    typedef std::pair<const _Key, _Value> value_type;

    typedef _Hasher hasher;
    typedef _Comp   key_equal;

    typedef _Alloc allocator_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef size_t size_type;

protected:
    // indirect key hashing
    struct key_hash : _Hasher {
        inline size_t operator()(const key_type* key) const {
            return _Hasher::operator()(*key);
        }
    };

    // indirect key comparing
    struct key_comp : _Comp {
        inline bool operator()(const key_type* lhs, const key_type* rhs) const {
            return _Comp::operator()(*lhs, *rhs);
        }
    };

    struct node : public value_type
    {
        size_t cost;

        // default constructor
        // construct node with null prev and next pointers and cost 1
        // node key-value pair is constructing using defalt constructor
        node() : cost(1) {
        }

        // constructor
        // construct node with null prev and next pointers
        // and specified key and value
        node(const key_type& key, const mapped_type& val = mapped_type(), size_t acost = 1) :
            value_type(key, val), cost(acost) {
        }

        // constructor
        // construct node with null prev and next pointers
        // and specified key and value
        node(const_reference val, size_t acost = 1) :
            value_type(val), cost(acost) {
        }

    };
    typedef node node_type;

    typedef std::allocator_traits<allocator_type> alloc_traits;

    // type of node allocator
    typedef typename alloc_traits::template rebind_alloc<node>::other node_allocator;
    // type of node list
    typedef std::list<node, node_allocator> nodelist;


    typedef std::pair<key_type*, typename nodelist::iterator> pair_type;
    // type of nodemap allocator
    typedef typename alloc_traits::template rebind_alloc<pair_type>::other nodemap_allocator;
    // type of node mapping
    typedef std::unordered_map<key_type*, typename nodelist::iterator, key_hash, key_comp, nodemap_allocator> nodemap;


    static inline key_type* key_pointer(const value_type& v) {
        return const_cast<key_type*>(std::addressof(v.first));
    }

    static inline key_type& project_key(const value_type& v) {
        return const_cast<key_type&>(v.first);
    }

    static inline mapped_type& project_val(const value_type& v) {
        return const_cast<mapped_type&>(v.second);
    }

    static inline const value_type& project(const node_type& v) {
        return (value_type&)(v);
    }

    static inline value_type construct(const key_type& key) {
        return std::make_pair(key, mapped_type());
    }

    inline typename nodelist::const_iterator victim() {
        return (--__m_list.end());
    }

    virtual ~cachemap_traits() {}

protected:
    nodelist __m_list;
    nodemap  __m_lookup;
};




template<
        class _Key,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
struct cacheset_traits
{
    typedef _Key   key_type;
    typedef _Key   value_type;
    typedef _Key   mapped_type;

    typedef _Hasher hasher;
    typedef _Comp   key_equal;

    typedef _Alloc allocator_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef size_t size_type;

protected:
    // indirect key hashing
    struct key_hash : _Hasher {
        inline size_t operator()(const key_type* key) const {
            return _Hasher::operator()(*key);
        }
    };

    // indirect key comparing
    struct key_comp : _Comp {
        inline bool operator()(const key_type* lhs, const key_type* rhs) const {
            return _Comp::operator()(*lhs, *rhs);
        }
    };

    struct node
    {
        key_type key;
        size_t cost;

        // default constructor
        // construct node with null prev and next pointers and cost 1
        // node key-value pair is constructing using defalt constructor
        node() : cost(1) {
        }

        // constructor
        // construct node with null prev and next pointers
        // and specified key and value
        node(const key_type& v, size_t acost = 1) :
            key(v), cost(acost) {
        }

    };
    typedef node node_type;

    typedef std::allocator_traits<allocator_type> alloc_traits;

    // type of node allocator
    typedef typename alloc_traits::template rebind_alloc<node> node_allocator;
    // type of node list
    typedef std::list<node, node_allocator> nodelist;


    typedef std::pair<key_type*, typename nodelist::iterator> pair_type;
    // type of nodemap allocator
    typedef typename alloc_traits::template rebind_alloc<pair_type> nodemap_allocator;
    // type of node mapping
    typedef std::unordered_map<key_type*, typename nodelist::iterator, key_hash, key_comp, nodemap_allocator> nodemap;


    static inline key_type* key_pointer(const node_type& v) {
        return const_cast<key_type*>(std::addressof(v.key));
    }

    static inline key_type* key_pointer(const value_type& v) {
        return const_cast<key_type*>(std::addressof(v));
    }

    static inline key_type& project_key(const value_type& v) {
        return const_cast<key_type&>(v);
    }

    static inline key_type& project_val(const value_type& v) {
        return const_cast<key_type&>(v);
    }

    static inline key_type& project_val(const node_type& v) {
        return const_cast<key_type&>(v.key);
    }

    static inline value_type& project(const node_type& v) {
        return const_cast<value_type&>(v.key);
    }

    static inline value_type construct(const key_type& key) {
        return value_type(key);
    }

    inline typename nodelist::const_iterator victim() const {
        return (--(this->__m_list.end()));
    }

    virtual ~cacheset_traits() {}

protected:
    nodelist __m_list;
    nodemap  __m_lookup;
};






template<
        class _Traits
        >
class lru_traits :
        public _Traits
{
protected:
    typedef _Traits traits_type;
    typedef typename traits_type::nodelist nodelist;
    typedef typename traits_type::key_type key_type;
    typedef typename traits_type::mapped_type mapped_type;
    typedef typename traits_type::value_type value_type;

    typedef typename traits_type::hasher hasher;
    typedef typename traits_type::key_equal key_equal;

    typedef typename traits_type::allocator_type allocator_type;

    typedef typename traits_type::reference reference;
    typedef typename traits_type::const_reference const_reference;

    typedef typename traits_type::pointer pointer;
    typedef typename traits_type::const_pointer const_pointer;

    inline void reorder(typename nodelist::iterator it) {
        this->__m_list.splice(this->__m_list.begin(), this->__m_list, it);
    }

    inline typename nodelist::const_iterator top() const {
        return this->__m_list.cbegin();
    }
};


template<
        class _Traits
        >
class mru_traits :
        public _Traits
{
protected:
    typedef _Traits traits_type;
    typedef typename traits_type::nodelist nodelist;
    typedef typename traits_type::key_type key_type;
    typedef typename traits_type::mapped_type mapped_type;
    typedef typename traits_type::value_type value_type;

    typedef typename traits_type::hasher hasher;
    typedef typename traits_type::key_equal key_equal;

    typedef typename traits_type::allocator_type allocator_type;

    typedef typename traits_type::reference reference;
    typedef typename traits_type::const_reference const_reference;

    typedef typename traits_type::pointer pointer;
    typedef typename traits_type::const_pointer const_pointer;

    inline void reorder(typename nodelist::iterator it) {
        this->__m_list.splice(this->__m_list.end(), this->__m_list, it);
    }

    inline typename nodelist::const_iterator top() const {
        return this->__m_list.cend();
    }
};



template<class _Traits>
class basic_cache : public _Traits
{
    typedef _Traits traits_type;

    typedef typename traits_type::node_type node_type;
    typedef typename traits_type::nodelist  nodelist;
    typedef typename traits_type::nodemap   nodemap;

    typedef basic_cache<_Traits> __this_type;
public:
    typedef typename traits_type::key_type key_type;
    typedef typename traits_type::mapped_type mapped_type;
    typedef typename traits_type::value_type value_type;
    typedef typename traits_type::hasher hasher;
    typedef typename traits_type::key_equal key_equal;

    typedef typename traits_type::allocator_type allocator_type;

    typedef typename traits_type::reference reference;
    typedef typename traits_type::const_reference const_reference;

    typedef typename traits_type::pointer pointer;
    typedef typename traits_type::const_pointer const_pointer;

public:
    class __node_iterator
    {
        typedef typename nodelist::const_iterator __list_iter;
        friend __this_type;

        __node_iterator(__list_iter other) :
            __m_it(other) {
        }

    public:
        typedef typename __list_iter::iterator_category iterator_category;
        typedef typename traits_type::value_type value_type;
        typedef size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;

        __node_iterator(const __node_iterator& other) :
            __m_it(other.__m_it) {
        }

        /*__node_iterator& operator= (__list_iter other) {
                        __m_it = other;
                        return (*this);
        }*/

        inline __node_iterator& operator= (const __node_iterator& other)
        {
            if (std::addressof(other) == this) // self assignment
                return (*this);

            __m_it = other.__m_it;
            return (*this);
        }

        inline size_t cost() const { return (__m_it->cost); }

        inline __node_iterator& operator++() {
            __next();
            return *this;
        }

        inline __node_iterator operator++(int) {
            __node_iterator tmp = *this;
            __next();
            return tmp;
        }

        inline __node_iterator& operator--() {
            __prev();
            return (*this);
        }

        inline __node_iterator operator--(int) {
            __node_iterator tmp = *this;
            __prev();
            return tmp;
        }

        inline const value_type& operator*() const { return _Traits::project(*__m_it); }
        inline const value_type* operator->() const { return std::addressof(_Traits::project(*__m_it)); }

        friend bool operator == (const __node_iterator& x, const __node_iterator& y) { return (x.__m_it == y.__m_it); }
        friend bool operator != (const __node_iterator& x, const __node_iterator& y) { return (x.__m_it != y.__m_it); }

    private:
        inline void __next() { ++__m_it; }
        inline void __prev() { --__m_it; }

    private:
        __list_iter __m_it;
    };
    typedef __node_iterator const_iterator;


    basic_cache(size_t max_cost = 128) :
        __m_maxcost(max_cost), __m_cost(0)
    {
        this->__m_lookup.reserve(max_cost);
        reset_hitref();

    }

    // clear cache and release all resources
    virtual ~basic_cache() {
    }

    // insert new element with key, value and specified cost (1 by default)
    // side-effect: order of elements will change, some elements may or may not be evicted
    const_iterator insert(const_reference val, size_t cost = 1)
    {
        __stdx_assert(cost > 0, std::invalid_argument, "incorrect cost value");

        if (cost > __m_maxcost) {
            clear();
            return end();
        }

        // increase number of cache references
        cache_ref();

        // special case: cost is equal to max cost
        // this optimization save CPU ticks upon trimming
        // the cache until it fits max cost
        if (cost == __m_maxcost) {
            // search for key
            auto it = this->__m_lookup.find(_Traits::key_pointer(val));
            if (it != this->__m_lookup.end()) {
                cache_hit(); // we've got the cache-hit
                // update node (node is move to front as a side-effect)
                __update_node(it->second, val, cost);
                // erase all except this node
                this->__m_list.erase(std::next(this->__m_list.begin()), this->__m_list.end());
                // clear lookup
                this->__m_lookup.clear();
                // emplace key pointer and value pointer into lookup mapping
                this->__m_lookup.emplace(_Traits::key_pointer(*this->__m_list.begin()), this->__m_list.begin());
            } else { // we've got the cache-miss
                // clear lookup
                this->__m_lookup.clear();
                // clear nodes
                this->__m_list.clear();
                // insert new one
                __insert_node(val, cost);
            }
            this->__m_cost = cost;  // reset total cost
            return begin();
        }

        // trim cache until it fits max cost
        while (overflow(cost)) {
            pop();
        }

        // search for key
        auto it = this->__m_lookup.find(_Traits::key_pointer(val));
        if (it != this->__m_lookup.end()) {
            cache_hit(); // we've got the cache-hit
            __m_cost -= it->second->cost; // reset total cost
            __update_node(it->second, val, cost); // update node
        }
        else {  // we've got the cache-miss
            __insert_node(val, cost);
        }
        __m_cost += cost; // update total cost
        return begin();
    }


    // erase element from cache
    inline const_iterator erase(const_iterator where)
    {
        if (where == end()) { return where; }
        // decrease cost
        this->__m_cost -= where.cost();
        // erase element from lookup
        this->__m_lookup.erase(_Traits::key_pointer(*where));
        // erase element from list
        return const_iterator(this->__m_list.erase(where.__m_it));
    }

    // erase element with specified key from the cache
    // return number of erased elements
    size_t erase(const key_type& key)
    {
        auto it = this->__m_lookup.find(_Traits::key_pointer(key));
        if (it != this->__m_lookup.end()) {
            this->__m_cost -= it->second->cost;   // decrease cost
            this->__m_list.erase(it->second);     // erase element from list
            this->__m_lookup.erase(it);           // erase element from lookup
            return 1;
        }
        return 0;
    }

    // evict element
    inline void pop() {
        this->erase(this->victim());
    }

    // check whatever key is in cache
    // side-effects: none
    inline bool contains(const key_type& key) const {
        return (this->__m_lookup.find(_Traits::key_pointer(key)) != this->__m_lookup.end());
    }

    // access to the element with specified key
    // side-effects: none
    inline const_iterator find(const key_type& key) const
    {
        auto it = this->__m_lookup.find(_Traits::key_pointer(key));
        return (it != this->__m_lookup.end() ? const_iterator(it->second) : end());
    }

    // access to the element with specified key
    // side-effects: if the element is found it will be reordered
    inline const_iterator touch(const key_type& key) const
    {
        cache_ref();
        auto it = this->__m_lookup.find(const_cast<key_type*>(std::addressof(key))); // TODO: replace it with _Traits::key_pointer() ???
        if (it != this->__m_lookup.end())
        {
            cache_hit();
            if (it->second != this->__m_list.begin())
                const_cast<__this_type*>(this)->reorder(it->second);
            return const_iterator(it->second);
        }
        return end();
    }

    // clear cache and destroy all elements
    inline void clear()
    {
        this->__m_cost = 0;
        this->__m_lookup.clear();
        this->__m_list.clear();
        reset_hitref();
    }

    // return iterator pointing to begin of cache
    inline const_iterator begin() const { return const_iterator(this->__m_list.begin()); }

    // return iterator pointing to end of cache
    inline const_iterator end() const { return const_iterator(this->__m_list.end()); }

    // return reference to front element
    inline const_reference front() const { return traits_type::project(this->__m_list.front()); }

    // return reference to back element
    inline const_reference back() const { return traits_type::project(this->__m_list.back()); }

    // check whatever overflow occur with inserting
    // item with specified cost
    inline bool overflow(size_t cost = 1) const {
        return ((cost + __m_cost) > __m_maxcost);
    }

    // check whatever overflow occur with updating cost
    // of item pointed by iterator it to specified cost
    inline bool overflow(const_iterator it, size_t cost) const {
        return ((cost + (__m_cost - it->cost())) > __m_maxcost);
    }

    // retrieve the total number of items with default
    // cost that can be added  to the cache before it becomes full.
    inline size_t available() const {
        return (__m_maxcost - __m_cost);
    }

    // retrieve the total number of items with specified cost
    // that can be added to the cache before it becomes full.
    // \warning the result is undefined if cost is equal to 0
    inline size_t available(size_t cost) const {
        __stdx_assert(cost > 0, std::invalid_argument, "incorrect cost value");
        return ((__m_maxcost - __m_cost) / cost);
    }

    // return maximum possible number of elements with default
    // cost in the cache
    inline size_t capacity() const {
        return __m_maxcost;
    }

    // return maximum possible number of elements with specified
    // cost in the cache
    // \warning the result is undefined if cost is equal to 0
    inline size_t capacity(size_t cost) const {
        __stdx_assert(cost > 0, std::invalid_argument, "incorrect cost value");
        return (__m_maxcost / cost);
    }

    // return maximum possible cost of all elements in the cache
    inline size_t max_cost() const { return __m_maxcost; }

    // return current cost of all elements in the cache
    inline size_t cost() const { return __m_cost; }

    // return number of elements that cache currently holds
    inline size_t size() const { return this->__m_lookup.size(); }

    // return max possible number of elements that cache can hold
    inline size_t max_size() const { return __m_maxcost; }

    // return true if cache is empty otherwise return false
    inline bool empty() const { return this->__m_lookup.empty(); }

    // return true if cache is strictly full (i.e. current cost is
    // equal max_cost) otherwise return false
    inline bool full() const { return (__m_cost == __m_maxcost); }

    inline hasher hash_function() const
    {	// return hasher object
        return hasher();
    }

    inline key_equal key_eq() const
    {	// return equality comparator object
        return key_equal();
    }

private:
    inline void __insert_node(const_reference val, size_t cost)
    {
        // push entry to top
        auto it = this->__m_list.emplace(this->top(), val, cost);
        // emplace key pointer and value pointer into lookup mapping
        this->__m_lookup.emplace(_Traits::key_pointer(*it), it);
    }

    inline void __update_node(typename nodelist::iterator it, const_reference val, size_t cost)
    {
        // reset entry cost
        it->cost = cost;
        // reset value
        _Traits::project_val(*it) = _Traits::project_val(val);
        // reorder list
        const_cast<__this_type*>(this)->reorder(it);
    }

#ifdef _DEBUG
    // return total number of cache references
    inline size_t ref_count() const { return __m_refcnt; }
    // return number of cache-hits
    inline size_t hit_count() const { return __m_hitcnt; }
    // return number of cache misses
    inline size_t miss_count() const { return (__m_refcnt - __m_hitcnt); }
    // return cache hit rate
    inline double hit_rate() const { return (ref_count() != 0 ? ((double)hit_count() / (double)ref_count()) : 0); }
#else
    inline size_t ref_count() const { return 0; }
    inline size_t hit_count() const { return 0; }
    inline size_t miss_count() const { return 0; }
    inline double hit_rate() const { return 0; }
#endif

private:
#ifdef _DEBUG
    // reset cache reference counter
    inline void reset_hitref() { __m_refcnt = __m_hitcnt = 0; }
    // increase cache reference counter
    inline void cache_ref() const { ++__m_refcnt; }
    // increase cache-hit counter
    inline void cache_hit() const { ++__m_hitcnt; }
#else
    inline void reset_hitref() { }
    inline void cache_ref() const { }
    inline void cache_hit() const { }
#endif


private:
#ifdef _DEBUG
    mutable size_t __m_refcnt; // cache reference counter
    mutable size_t __m_hitcnt; // cache-hit counter
#endif
    size_t __m_maxcost;
    size_t __m_cost;
};




template<
        class _Traits
        >
class cachemap : public basic_cache<_Traits>
{
    typedef basic_cache<_Traits> __base_type;
public:
    typedef typename __base_type::key_type key_type;
    typedef typename __base_type::mapped_type mapped_type;
    typedef typename __base_type::value_type value_type;
    typedef typename __base_type::allocator_type allocator_type;

    typedef typename __base_type::reference reference;
    typedef typename __base_type::const_reference const_reference;

    typedef typename __base_type::pointer pointer;
    typedef typename __base_type::const_pointer const_pointer;

    typedef typename __base_type::const_iterator const_iterator;

    cachemap(size_t maxcost = 128) : __base_type(maxcost) {}

    const_iterator emplace(const key_type& key, const mapped_type& val, size_t cost = 1) {
        return insert({ key, val }, cost);
    }

    // access to the element with specified key
    // side-effects: if the element is found it will be reordered
    mapped_type& operator[](const key_type& key)
    {
        auto it = touch(key);
        if (it != this->end()) {
            return _Traits::project_val(*it);
        }
        return _Traits::project_val(*emplace(key, mapped_type()));
    }

    // access to the element with specified key
    // side-effects: if the element is found it will be reordered
    const mapped_type& operator[](const key_type& key) const
    {
        auto it = touch(key);
        if (it != this->end()) {
            return _Traits::project_val(*it);
        }
        return _Traits::project_val(*emplace(key, mapped_type()));
    }
};




template<
        class _Traits
        >
class cacheset : public basic_cache < _Traits >
{
    typedef basic_cache<_Traits> __base_type;
public:
    typedef typename __base_type::key_type key_type;
    typedef typename __base_type::mapped_type mapped_type;
    typedef typename __base_type::value_type value_type;
    typedef typename __base_type::allocator_type allocator_type;

    typedef typename __base_type::reference reference;
    typedef typename __base_type::const_reference const_reference;

    typedef typename __base_type::pointer pointer;
    typedef typename __base_type::const_pointer const_pointer;

    typedef typename __base_type::const_iterator const_iterator;

    cacheset(size_t maxcost = 128) : __base_type(maxcost) {}


};


template<
        class _Key,
        class _Value,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
using lru_cachemap = cachemap < lru_traits< cachemap_traits<_Key, _Value, _Hasher, _Comp, _Alloc> > >;


template<
        class _Key,
        class _Value,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
using mru_cachemap = cachemap < mru_traits < cachemap_traits<_Key, _Value, _Hasher, _Comp, _Alloc> > >;


template<
        class _Key,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
using lru_cacheset = cacheset < lru_traits< cacheset_traits<_Key, _Hasher, _Comp, _Alloc> > >;


template<
        class _Key,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
using mru_cacheset = cacheset < mru_traits < cacheset_traits<_Key, _Hasher, _Comp, _Alloc> > >;


_STDX_END

