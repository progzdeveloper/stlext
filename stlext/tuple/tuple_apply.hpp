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
#include "../platform/common.h"

#include "../compability/integer_sequence"
#include "../compability/functional"


_STDX_BEGIN

namespace detail {

	/// APPLY FUNCTION IMPLEMENTATION DETAILS

#if (defined(STDX_CMPLR_MSVC) && STDX_CMPLR_MSVC <= 1800) || \
    (defined(STDX_CMPLR_GNU) && (STDX_CMPLR_GNU < 500)) || \
    (defined(STDX_CMPLR_INTEL) && STDX_CMPLR_INTEL <= 1800)


    template<class _Fn, class _Tuple, std::size_t... _Idx>
    inline constexpr auto apply_impl(_Fn&& target, _Tuple&& args, stdx::index_sequence<_Idx...>)
            -> decltype(stdx::invoke(std::forward<_Fn>(target), std::get<_Idx>(std::forward<_Tuple>(args))...))
    {
        using std::get;
        using std::forward;
        return stdx::invoke(forward<_Fn>(target), get<_Idx>(forward<_Tuple>(args))...);
    }

    template<class _Fn, class _Tuple, std::size_t... _Idx>
    inline constexpr auto apply_impl(_Fn target, const _Tuple& args, stdx::index_sequence<_Idx...>)
            -> decltype(stdx::invoke(target, std::get<_Idx>(args)...))
    {
        using std::get;
        return stdx::invoke(target, get<_Idx>(args)...);
    }

#else

    template<typename _Fn, typename _Tuple, std::size_t... _Idx>
    inline auto apply_impl(_Fn&& target, _Tuple&& args, std::index_sequence < _Idx... >)
            -> decltype(stdx::invoke(std::forward<_Fn>(target), std::get<_Idx>(std::forward<_Tuple>(args))...))
    {
        using namespace std;
        return stdx::invoke(forward<_Fn>(target), get<_Idx>(forward<_Tuple>(args))...);
    }

#endif

} // end namespace detail


/// APPLY TUPLE AS FUNCTION ARGS

/// C++17 proposal
#if (defined(STDX_CMPLR_MSVC) && STDX_CMPLR_MSVC <= 1800) || \
    (defined(STDX_CMPLR_GNU) && (STDX_CMPLR_GNU < 500)) || \
    (defined(STDX_CMPLR_INTEL) && STDX_CMPLR_INTEL <= 1800)

template<typename _Fn, typename... _Args>
inline constexpr auto apply(_Fn&& target, std::tuple<_Args...>&& args)
    -> decltype(detail::apply_impl(std::forward<_Fn>(target), std::forward< std::tuple<_Args...> >(args), stdx::index_sequence_for<_Args...>{}))
{
    using namespace std;
    typedef std::tuple<_Args...> tuple_t;
    using _Indices = stdx::make_index_sequence<tuple_size<tuple_t>::value>;
    return detail::apply_impl(forward<_Fn>(target), forward<tuple_t>(args), _Indices{});
}


template<class _Fn, class... _Args>
inline constexpr auto apply(_Fn target, const std::tuple<_Args...>& args)
    -> decltype(detail::apply_impl(std::forward<_Fn>(target), args, stdx::index_sequence_for<_Args...>{}))
{
    using namespace std;
    typedef std::tuple<_Args...> tuple_t;
    using _Indices = stdx::make_index_sequence<tuple_size<tuple_t>::value>;
    return detail::apply_impl(target, args, _Indices{});
}


#else

template<typename _Fn, typename... _Args>
inline auto apply(_Fn&& target, std::tuple<_Args...>&& args)
    -> decltype(detail::apply_impl(std::forward<_Fn>(target), std::forward< std::tuple<_Args...> >(args), std::index_sequence_for<_Args...>{}))
{
    using namespace std;
    typedef std::tuple<_Args...> tuple_t;
    using _Indices = make_index_sequence<tuple_size<tuple_t>::value>;
    return detail::apply_impl(forward<_Fn>(target), forward<tuple_t>(args), _Indices{});
}


template<typename _Fn, typename... _Args>
inline auto apply(_Fn&& target, const std::tuple<_Args...>& args)
    -> decltype(detail::apply_impl(std::forward<_Fn>(target), args, std::index_sequence_for<_Args...>{}))
{
    using namespace std;
    typedef std::tuple<_Args...> tuple_t;
    using _Indices = make_index_sequence<tuple_size<tuple_t>::value>;
    return detail::apply_impl(target, args, _Indices{});
}

#endif

_STDX_END


