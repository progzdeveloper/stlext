// Copyright (c) 2018, Michael Polukarov (Russia).
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
#include <memory>
#include <set>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <functional>
#include <utility>

#include "../platform/common.h"

_STDX_BEGIN



template<
class _Key,
        class _Priority,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
        class priority_set
{
public:
    typedef _Key   key_type;
    typedef _Priority priority_type;

    typedef key_type value_type;

    typedef _Hasher hasher;
    typedef _Comp   key_equal;

    typedef _Alloc allocator_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef size_t size_type;


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
        const _Priority priority;

        // default constructor
        // construct node with priority 1
        // node key-value pair is constructing using defalt constructor
        node() : priority() {
        }


        /*template<class... _Args>
        node(const _Priority& p, _Args&&... args) :
            key(std::forward<_Args>(args)...), priority(p) {
        }*/

        // constructor
        // construct node with specified key and value and priority
        node(const key_type& k, const _Priority& p = _Priority()) :
            key(k), priority(p) {
        }

        friend inline bool operator< (const node& lhs, const node& rhs) {
            return (lhs.priority < rhs.priority);
        }
    };
    typedef node node_type;


    // type of node allocator
    typedef typename _Alloc::template rebind<node>::other node_allocator;
    // type of node list
    typedef std::multiset<node, std::less<node>, node_allocator> nodelist;


    typedef std::pair<const key_type*, typename nodelist::iterator> pair_type;
    // type of nodemap allocator
    typedef typename _Alloc::template rebind<pair_type>::other nodemap_allocator;
    // type of node mapping
    typedef std::unordered_map<key_type*, typename nodelist::iterator, key_hash, key_comp, nodemap_allocator> nodemap;


    static inline key_type* key_pointer(const key_type& key) {
        return const_cast<key_type*>(std::addressof(key));
    }

    static inline key_type* key_pointer(const node_type& v) {
        return const_cast<key_type*>(std::addressof(v.key));
    }

public:

    typedef typename nodelist::iterator iterator;
    typedef typename nodelist::const_iterator const_iterator;
    typedef typename nodelist::reverse_iterator reverse_iterator;
    typedef typename nodelist::const_reverse_iterator const_reverse_iterator;

    priority_set() {
    }

    ~priority_set() {
    }

    bool empty() const {
        return __m_nodes.empty();
    }

    size_type size() const {
        return __m_nodes.size();
    }
    size_type max_size() const {
        return __m_nodes.max_size();
    }

    size_type bucket_count() const {
        return __m_lookup.bucket_count();
    }

    size_type count(const _Key& key) const {
        return __m_lookup.count(key);
    }

    void reserve(size_type size) {
        __m_lookup.reserve(size);
    }

    void clear() {
        __m_nodes.clear();
        __m_lookup.clear();
    }


    iterator begin() {
        return __m_nodes.begin();
    }
    const_iterator begin() const {
        return __m_nodes.begin();
    }

    iterator end() {
        return __m_nodes.end();
    }
    const_iterator end() const {
        return __m_nodes.end();
    }

    reverse_iterator rbegin() {
        return __m_nodes.rbegin();
    }
    const_reverse_iterator rbegin() const {
        return __m_nodes.rbegin();
    }

    reverse_iterator rend() {
        return __m_nodes.rend();
    }
    const_reverse_iterator rend() const {
        return __m_nodes.rend();
    }

    const node_type& front() const {
        return __m_nodes.front();
    }

    const node_type& back() const {
        return __m_nodes.back();
    }

    node_type& front() {
        return __m_nodes.front();
    }

    node_type& back() {
        return __m_nodes.back();
    }

    const_iterator find(const _Key& key) {
        auto mapIt = __m_lookup.find(key_pointer(key));
        return (mapIt == __m_lookup.end() ? __m_nodes.end() : mapIt->second);
    }

    void pop_back() {
        __m_lookup.erase(key_pointer(*std::prev(__m_nodes.end())));
        __m_nodes.erase(std::prev(__m_nodes.end()));
    }

    void pop_front() {
        __m_lookup.erase(key_pointer(*__m_nodes.begin()));
        __m_nodes.erase(__m_nodes.begin());
    }

    iterator insert(const _Key& key, const _Priority& priority)
    {
        iterator it;

        auto mapIt = __m_lookup.find(key_pointer(key));
        if (mapIt == __m_lookup.end())
        {
            // insert node to set
            auto it = __m_nodes.emplace(key, priority);
            // update lookup
            __m_lookup[key_pointer(*it)] = it;
        }
        else
        {
            it = mapIt->second;
            if (it->priority == priority)
                return it;

            // copy node
            node n = *it;
            // assign new priority
            *(const_cast<_Priority*>(&n.priority)) = priority;

            // erase node O(1)
            __m_nodes.erase(it);
            // insert node (O(logN))
            auto it = __m_nodes.emplace(n);
            // insert into lookup
            __m_lookup[key_pointer(*it)] = it;
        }

        return it;
    }

    iterator update(const _Key& key, const priority_type priority)
    {
        auto mapIt = __m_lookup.find(key_pointer(key));
        if (mapIt == __m_lookup.end())
            return __m_nodes.end();

        auto mit = mapIt->second;
        if (mit->priority == priority)
            return mit;

        // copy node
        node n = *mit;
        // assign new priority
        *(const_cast<_Priority*>(&n.priority)) = priority;

        // erase node O(1)
        __m_nodes.erase(mit);
        // insert node (O(logN))
        auto it = __m_nodes.emplace(n);
        // insert into lookup
        __m_lookup[key_pointer(*it)] = it;

        return it;
    }

    size_type erase(const _Key& key)
    {
        auto it = __m_lookup.find(key_pointer(key));
        if (it == __m_lookup.end()) {
            return 0;
        }
        else {
            auto nodeIt = it->second;
            __m_lookup.erase(it);
            __m_nodes.erase(nodeIt);
            return 1;
        }
    }

private:
    nodelist __m_nodes;
    nodemap __m_lookup;
};









template<
        class _Key,
        class _Value,
        class _Priority,
        class _Hasher = std::hash<_Key>,
        class _Comp = std::equal_to<_Key>,
        class _Alloc = std::allocator<char>
        >
class priority_map
{
public:
    typedef _Key   key_type;
    typedef _Value mapped_type;
    typedef _Priority priority_type;

    typedef std::pair<const _Key, _Value> value_type;

    typedef _Hasher hasher;
    typedef _Comp   key_equal;

    typedef _Alloc allocator_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef size_t size_type;


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

    struct node :
            public value_type
    {
        const _Priority priority;

        // default constructor
        // construct node with priority 1
        // node key-value pair is constructing using defalt constructor
        node() : priority(0) {
        }

        // constructor
        // construct node with specified key and value and priority
        node(const key_type& key, const mapped_type& val, const _Priority& p = 0) :
            value_type(key, val), priority(p) {
        }

        template<class... _Args>
        node(const _Priority& p, const key_type& key, _Args&&... args) :
            value_type(key, std::forward<_Args>(args)...), priority(p) {
        }

        // constructor
        // construct node
        node(const_reference val, size_t p = 0) :
            value_type(val), priority(p) {
        }

        friend inline bool operator< (const node& lhs, const node& rhs) {
            return (lhs.priority < rhs.priority);
        }
    };
    typedef node node_type;


    // type of node allocator
    typedef typename _Alloc::template rebind<node>::other node_allocator;
    // type of node list
    typedef std::multiset<node, std::less<node>, node_allocator> nodelist;


    typedef std::pair<const key_type*, typename nodelist::iterator> pair_type;
    // type of nodemap allocator
    typedef typename _Alloc::template rebind<pair_type>::other nodemap_allocator;
    // type of node mapping
    typedef std::unordered_map<key_type*, typename nodelist::iterator, key_hash, key_comp, nodemap_allocator> nodemap;


    static inline key_type* key_pointer(const key_type& key) {
        return const_cast<key_type*>(std::addressof(key));
    }

    static inline key_type* key_pointer(const node_type& v) {
        return const_cast<key_type*>(std::addressof(v.first));
    }

    static inline key_type* key_pointer(const value_type& v) {
        return const_cast<key_type*>(std::addressof(v.first));
    }

public:

    typedef typename nodelist::iterator iterator;
    typedef typename nodelist::const_iterator const_iterator;
    typedef typename nodelist::reverse_iterator reverse_iterator;
    typedef typename nodelist::const_reverse_iterator const_reverse_iterator;

    priority_map() {
    }

    ~priority_map() {
    }

    bool empty() const {
        return __m_nodes.empty();
    }

    size_type size() const {
        return __m_nodes.size();
    }
    size_type max_size() const {
        return __m_nodes.max_size();
    }

    size_type bucket_count() const {
        return __m_lookup.bucket_count();
    }

    size_type count(const _Key& key) const {
        return __m_lookup.count(key);
    }

    void reserve(size_type size) {
        __m_lookup.reserve(size);
    }

    void clear() {
        __m_nodes.clear();
        __m_lookup.clear();
    }


    iterator begin() {
        return __m_nodes.begin();
    }
    const_iterator begin() const {
        return __m_nodes.begin();
    }

    iterator end() {
        return __m_nodes.end();
    }
    const_iterator end() const {
        return __m_nodes.end();
    }

    reverse_iterator rbegin() {
        return __m_nodes.rbegin();
    }
    const_reverse_iterator rbegin() const {
        return __m_nodes.rbegin();
    }

    reverse_iterator rend() {
        return __m_nodes.rend();
    }
    const_reverse_iterator rend() const {
        return __m_nodes.rend();
    }


    const node_type& front() const {
        return __m_nodes.front();
    }

    const node_type& back() const {
        return __m_nodes.back();
    }

    node_type& front() {
        return __m_nodes.front();
    }

    node_type& back() {
        return __m_nodes.back();
    }

    const_iterator find(const _Key& key) {
        auto mapIt = __m_lookup.find(key_pointer(key));
        return (mapIt == __m_lookup.end() ? __m_nodes.end() : mapIt->second);
    }

    void pop_back() {
        __m_lookup.erase(key_pointer(std::prev(__m_nodes.end())->first));
        __m_nodes.erase(std::prev(__m_nodes.end()));
    }

    void pop_front() {
        __m_lookup.erase(key_pointer(__m_nodes.begin()->first));
        __m_nodes.erase(__m_nodes.begin());
    }

    template<class... _Args>
    iterator emplace(const _Key& key, const _Priority& priority, _Args&&... args)
    {
        iterator it;

        auto mapIt = __m_lookup.find(key_pointer(key));
        if (mapIt == __m_lookup.end())
        {
            // insert node to set
            auto result = __m_nodes.emplace(priority, key, std::forward<_Args>(args)...);
            it = result.first;
            // update lookup
            __m_lookup[key_pointer(*it)] = it;
        }
        else
        {
            it = mapIt->second;
            if (it->priority == priority)
                return it;

            // copy node
            node n = *it;
            // assign new priority
            *(const_cast<_Priority*>(&n.priority)) = priority;

            // erase node O(1)
            __m_nodes.erase(it);
            // insert node (O(logN))
            auto result = __m_nodes.emplace(n);
            __m_lookup[key_pointer(*result.first)] = result.first;
        }

        return it;
    }


    iterator insert(const _Key& key, const _Value& value, const _Priority& priority = _Priority())
    {
        iterator it;

        auto mapIt = __m_lookup.find(key_pointer(key));
        if (mapIt == __m_lookup.end())
        {
            // insert node to set
            auto result = __m_nodes.emplace(key, value, priority);
            it = result.first;
            // update lookup
            __m_lookup[key_pointer(*it)] = it;
        }
        else
        {
            it = mapIt->second;
            if (it->priority == priority)
                return it;

            // copy node
            node n = *it;
            // assign new priority
            *(const_cast<_Priority*>(&n.priority)) = priority;

            // erase node O(1)
            __m_nodes.erase(it);
            // insert node (O(logN))
            auto result = __m_nodes.emplace(n);
            it = result.first;
            // insert into lookup
            __m_lookup[key_pointer(*it)] = it;
        }

        return it;
    }

    iterator update(const _Key& key, const priority_type priority)
    {
        auto mapIt = __m_lookup.find(key_pointer(key));
        if (mapIt == __m_lookup.end())
            return __m_nodes.end();

        auto it = mapIt->second;
        if (it->priority == priority)
            return it;

        // copy node
        node n = *it;
        // assign new priority
        *(const_cast<_Priority*>(&n.priority)) = priority;

        // erase node O(1)
        __m_nodes.erase(it);
        // insert node (O(logN))
        auto result = __m_nodes.emplace(n);
        it = result.first;
        // insert into lookup
        __m_lookup[key_pointer(*it)] = it;

        return it;
    }

    size_type erase(const _Key& key)
    {
        auto it = __m_lookup.find(key_pointer(key));
        if (it == __m_lookup.end()) {
            return 0;
        }
        else {
            auto nodeIt = it->second;
            __m_lookup.erase(it);
            __m_nodes.erase(nodeIt);
            return 1;
        }
    }

private:
    nodelist __m_nodes;
    nodemap __m_lookup;
};


_STDX_END
