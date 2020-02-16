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
#include <functional>
#include <unordered_map>
#include <tuple>

#include "../platform/common.h"
#include "../tuple/tuple_utils.hpp"

#include "function_traits.hpp"

_STDX_BEGIN

/*!
\brief template class memorized<>  provides caching function call results.

\tparam _Fn type of function
\tparam _CacheMap type of cache map

See \ref memorized< _Ret(_Args...), _CacheMap > specialization for full documentation
*/
template<
	typename _Fn,
	class _CacheMap = std::unordered_map<
		typename function_traits<_Fn>::args_type,
		typename function_traits<_Fn>::result_type,
		tuple_hash< typename function_traits<_Fn>::tuple_type >
	>
>
class memorized {};



/*!
\brief template class memorized<_Ret(_Args...), _CacheMap> provides caching function object

\tparam _Fn type of function _Ret(_Args...)
\tparam _CacheMap type of cache map (std::unordered_map<> by default)

The technique explored by this class often called 'memorization'.
Class memorized<> use internal cache map to store arguments and
corresponding results of target function invokation. If actual
arguments found in cache the corresponding result is returned
immediately, bypassing the actual function invokation.

This strategy increases performance for long running functions when
probability that they will be invoking with same arguments are high.

Users of memorized<> class can setup the results cache map by
specifying _CacheMap template parameter. By default
std::unordered_map<> is used as internal result cache map.

*/
template<
	typename _CacheMap,
	typename _Ret, 
	typename... _Args
>
class memorized < _Ret(_Args...), _CacheMap >
{
	static_assert(!std::is_same<_Ret, void>::value, "void return type cannot be memorized");
public:
	/*! type of function */
	typedef std::function<_Ret(_Args...)> function_type;
	/*! type of arguments tuple */
	typedef std::tuple<_Args...> args_type;
	/*! type of result */
	typedef _Ret result_type;
	/*! type of cache mapping */
	typedef _CacheMap cache_map;

	/*!
	\brief Default constructor
	*/
	memorized() {}

	/*!
	\brief Parametrized constructor
	\tparam _Fx type of function
	\tparam _Alloc type of allocator
	\param target target function
	\param al allocator
	*/
	template<class _Fx, class _Alloc>
	memorized(_Fx target, const _Alloc& al) :
		__m_target(target, al) {
	}

	/*!
	\brief Parametrized constructor
	\tparam _Fx type of function
	\param __target const reference to target function
	\param __cache  const reference to results cache
	*/
	template<class _Fx>
	memorized(const _Fx& target, const _CacheMap& cache = _CacheMap()) :
		__m_target(target), _cache(cache) {
	}

	/*!
	\brief Parametrized constructor
	\tparam _Fx type of function
	\param __target rvalue reference to target function
	\param __cache  rvalue reference to results cache
	*/
	template<typename _Fx>
	memorized(_Fx&& target, _CacheMap&& cache = _CacheMap()) :
		__m_target(target), __m_cache(cache) {
	}

	/*!
	\brief Parametrized constructor
	\param __target const reference to target std::function object
	\param __cache  const reference to results cache
	*/
	memorized(const function_type& target, const _CacheMap& cache = _CacheMap()) :
		__m_target(target), __m_cache(cache) {
	}

	/*!
	\brief Parametrized constructor
	\param __target rvalue reference to target std::function object
	\param __cache  rvalue reference to results cache
	*/
	memorized(function_type&& target, _CacheMap&& cache = _CacheMap()) :
		__m_target(target), __m_cache(cache) {
	}

	/*!
	\brief Parametrized constructor
	\tparam _Fx type of function
	\param __target reference wrapper of target function 
	\param __cache  const reference to results cache
	*/
	template<class _Fx>
	memorized(std::reference_wrapper<_Fx> target, const _CacheMap& cache = _CacheMap()) :
		__m_target(target), __m_cache(cache) {
	}

	/*!
	\brief Parametrized constructor
	\tparam _Fx type of function
	\tparam _Alloc type of allocator
	\param __target reference wrapper of target function
	\param __al  const reference to allocator
	*/
	template<class _Fx, class _Alloc>
	memorized(std::reference_wrapper<_Fx> target, const _Alloc& al) :
		__m_target(target, al) {
	}

	/*!
	\brief Copy constructor 
	\param __other const reference to memorized object to copy from
	*/
	memorized(const memorized& other) :
		__m_target(other.target),
		__m_cache(other.__m_cache) {
	}

	/*!
	\brief Move constructor
	\param __other rvalue reference to memorized object to move from
	*/
	memorized(memorized&& other) :
		__m_target(std::move(other.target)),
		__m_cache(std::move(other.__m_cache)) {
	}

	/*!
	\brief Assignment operator overload 
	\param __other const reference to memorized object to copy from
	\return reference to modified memorized object
	*/
	memorized& operator=(const memorized& other) 
	{	// gently escape self-assignment
		if (std::addressof(other) == this) { return *this; }
		__m_target = other.__m_target;
		__m_cache = other.__m_cache;
		return (*this);
	}

	/*!
	\brief Assignment operator overload
	\param __other rvalue reference to memorized object to move from
	\return reference to modified memorized object
	*/
	memorized& operator=(memorized&& other) 
	{	// gently escape self-assignment
		if (std::addressof(other) == this) { return *this; }
		__m_target = other.__m_target;
		__m_cache = other.__m_cache;
		return (*this);
	}

	/*!
	\brief Assignment operator overload
	\param __target rvalue reference to std::function object to move from
	\return reference to modified memorized object
	*/
	memorized& operator=(function_type&& target) 
	{	// gently escape self-assignment
		if (std::addressof(target) == std::addressof(__m_target) { return *this; }
		__m_target = target;
		__m_cache.clear();
		return (*this);
	}

	/*!
	\brief Assignment operator overload
	\param __target const reference to std::function object to copy from
	\return reference to modified memorized object
	*/
	memorized& operator=(const function_type& target)
	{	// gently escape self-assignment
		if (std::addressof(target) == std::addressof(__m_target) { return *this; }
		__m_target = target;
		__m_cache.clear();
		return (*this);
	}

	/*!
	\brief Assignment operator overload
	\param __target rvalue reference to function to move from
	\return reference to modified memorized object
	*/
	template<class _Fx>
	memorized& operator=(_Fx&& target) 
	{	// gently escape self-assignment
		if (std::addressof(target) == std::addressof(__m_target.target<_Fx>()) { 
			return *this; 
		}
		__m_target = target;
		__m_cache.clear();
		return (*this);
	}

	/*!
	\brief Assign from function
	\tparam _Fx type of function
	\tparam _Alloc type of allocator
	\param __target rvalue reference to target function
	\param __al const reference to allocator
	*/
	template<class _Fx, class _Alloc>
	inline void assign(_Fx&& target, const _Alloc& al) {
		// gently escape self-assignment
		if (std::addressof(target) == std::addressof(__m_target.target<_Fx>()) { 
			return; 
		}
		__m_target.assign(target, al);
		__m_cache.clear();
	}

	/*!
	\brief Call operator overload
	\param __args arguments of function
	\return result of function call operation
	\note function call results is cached and if current arguments
	was found in cache function result returns immediately without
	actual target function invokation
	*/
	inline result_type operator()(_Args&&... args)
	{
		using namespace std;
		if (!__m_target) // no target
			throw bad_function_call();

		auto&& args = tie(args); // tie args
		auto it = __m_cache.find(args); // search for args tuple
		if (it != __m_cache.end()) { 
			return it->second; // return cached result
		}

		// call function and cache result
		__m_cache.emplace(move(args_pack), __m_target(forward<_Args>(__args)...));
		return result;
	}

	/*!
	\brief Call operator overload (constant version)
	\param __args arguments of function
	\return result of function call operation
	\note function call results is cached and if current arguments
	was found in cache function result returns immediately without
	actual target function invokation
	*/
	inline result_type operator()(_Args&&... __args) const
	{
		using namespace std;
		if (!__m_target) // no target
			throw bad_function_call();

		auto&& args = forward_as_tuple(__args);  // tie args
		auto it = __m_cache.find(args); // search for args tuple
		if (it != __m_cache.end()) {
			return it->second; // return cached result
		}

		// call function and cache result
		__m_cache.emplace(move(args_pack), __m_target(forward<_Args>(__args)...));
		return result;
	}

	
	/*!
	\brief Cleanup cache
	Clean internal result cache 
	*/
	inline void clear() {
		__m_cache.clear();
	}

	/*!
	\brief Extract results cache
	\return const reference to results cache
	*/
	const cache_map& cache() const {
		return __m_cache;
	}

	/*!
	\brief explicit bool operator overload
	*/
	explicit operator bool() const {
		return (__m_target.operator bool());
	}

	/*!
	\brief type_info of target function
	\return type_info of target function
	*/
	const std::type_info& target_type() const {
		return __m_target.target_type();
	}

	/*!
	\brief Extract non-const raw pointer to target function
	\tparam _Fty2 type of function
	\return raw pointer to target function
	*/
	template<class _Fty2>
	_Fty2 *target() {
		return (__m_target.target<_Fty2>());
	}

	/*!
	\brief Extract const raw pointer to target function
	\tparam _Fty2 type of function
	\return raw pointer to target function
	*/
	template<class _Fty2>
	const _Fty2 *target() const {
		return (__m_target.target<_Fty2>());
	}

private:
	mutable cache_map __m_cache; // !< results cache
	function_type __m_target;    // !< target function
};


_STDX_END
