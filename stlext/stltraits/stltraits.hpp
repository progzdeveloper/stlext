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
#include <type_traits>
#include "stlfwd.hpp"
#include "stdxfwd.hpp"
#include "../platform/common.h"

_STDX_BEGIN


// general case
template<typename T>
struct is_container : std::false_type {};

// plain C-like array
template<typename T, size_t N>
struct is_container<T[N]> : std::true_type{};

template<typename T, size_t N>
struct is_container<T(&)[N]> : std::true_type{};


// vector-like containers
template<typename T> 
struct is_container< std::valarray<T> > : std::true_type {};

template<typename T, size_t N> 
struct is_container< std::array<T, N> > : std::true_type{};
template<typename T, typename A> 
struct is_container< std::vector<T, A> > : std::true_type{};
template<typename T, typename A> 
struct is_container< std::deque<T, A> > : std::true_type{};
template<typename T, typename A> 
struct is_container< std::list<T, A> > : std::true_type{};
template<typename T, typename A> 
struct is_container< std::forward_list<T, A> > : std::true_type{};

template<typename E, typename T, typename A> 
struct is_container< std::basic_string<E, T, A> > : std::true_type{};

// associative containers
template<typename K, typename C, typename A> 
struct is_container< std::set<K, C, A> > : std::true_type{};
template<typename K, typename C, typename A> 
struct is_container< std::multiset<K, C, A> > : std::true_type{};

template<typename K, typename T, typename C, typename A> 
struct is_container< std::map<K, T, C, A> > : std::true_type{};
template<typename K, typename T, typename C, typename A> 
struct is_container< std::multimap<K, T, C, A> > : std::true_type{};

template<typename K, typename H, typename E, typename A> 
struct is_container< std::unordered_set<K, H, E, A> > : std::true_type{};
template<typename K, typename H, typename E, typename A> 
struct is_container< std::unordered_multiset<K, H, E, A> > : std::true_type{};

template<typename K, typename T, typename H, typename E, typename A> 
struct is_container< std::unordered_map<K, T, H, E, A> > : std::true_type{};
template<typename K, typename T, typename H, typename E, typename A> 
struct is_container< std::unordered_multimap<K, T, H, E, A> > : std::true_type{};



template<typename T>
struct is_pair : std::false_type {};

template<typename T1, typename T2>
struct is_pair< std::pair<T1, T2> > : std::true_type{};


template<typename T>
struct is_tuple : std::false_type {};

template<typename... _Args>
struct is_tuple< std::tuple<_Args...> > : std::true_type{};


/// STDX iter_range specialization
template<typename _It>
struct is_container< iter_range<_It> > : std::true_type{};





template<typename T>
struct is_continious : std::false_type {};

// vector types
template<typename T, size_t N>
struct is_continious<T[N]> : std::true_type{};
template<typename T>
struct is_continious< std::valarray<T> > : std::true_type{};
template<typename T, size_t N>
struct is_continious< std::array<T, N> > : std::true_type{};
template<typename T, typename A>
struct is_continious< std::vector<T, A> > : std::true_type{};
template<typename E, typename T, typename A>
struct is_continious< std::basic_string<E, T, A> > : std::true_type{};


template<typename T>
struct is_segregated : std::false_type {};

// list types
template<typename T, typename A>
struct is_segregated< std::deque<T, A> > : std::true_type{};
template<typename T, typename A>
struct is_segregated< std::list<T, A> > : std::true_type{};
template<typename T, typename A>
struct is_segregated< std::forward_list<T, A> > : std::true_type{};

// accotiative types
template<typename K, typename C, typename A>
struct is_segregated< std::set<K, C, A> > : std::true_type{};
template<typename K, typename C, typename A>
struct is_segregated< std::multiset<K, C, A> > : std::true_type{};

template<typename K, typename T, typename C, typename A>
struct is_segregated< std::map<K, T, C, A> > : std::true_type{};
template<typename K, typename T, typename C, typename A>
struct is_segregated< std::multimap<K, T, C, A> > : std::true_type{};

template<typename K, typename H, typename E, typename A>
struct is_segregated< std::unordered_set<K, H, E, A> > : std::true_type{};
template<typename K, typename H, typename E, typename A>
struct is_segregated< std::unordered_multiset<K, H, E, A> > : std::true_type{};

template<typename K, typename T, typename H, typename E, typename A>
struct is_segregated< std::unordered_map<K, T, H, E, A> > : std::true_type{};
template<typename K, typename T, typename H, typename E, typename A>
struct is_segregated< std::unordered_multimap<K, T, H, E, A> > : std::true_type{};


// accotiative types
template<typename T>
struct is_associative : std::false_type{};

template<typename K, typename C, typename A>
struct is_associative< std::set<K, C, A> > : std::true_type{};
template<typename K, typename C, typename A>
struct is_associative< std::multiset<K, C, A> > : std::true_type{};

template<typename K, typename T, typename C, typename A>
struct is_associative< std::map<K, T, C, A> > : std::true_type{};
template<typename K, typename T, typename C, typename A>
struct is_associative< std::multimap<K, T, C, A> > : std::true_type{};

template<typename K, typename H, typename E, typename A>
struct is_associative< std::unordered_set<K, H, E, A> > : std::true_type{};
template<typename K, typename H, typename E, typename A>
struct is_associative< std::unordered_multiset<K, H, E, A> > : std::true_type{};

template<typename K, typename T, typename H, typename E, typename A>
struct is_associative< std::unordered_map<K, T, H, E, A> > : std::true_type{};
template<typename K, typename T, typename H, typename E, typename A>
struct is_associative< std::unordered_multimap<K, T, H, E, A> > : std::true_type{};


_STDX_END
