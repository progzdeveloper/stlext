// Copyright (c) 2021, Michael Polukarov (Russia).
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
#include <string>
#include <string_view>
#include <memory>
#include <array>
#include <tuple>
#include <type_traits>

#include "../platform/common.h"

_STDX_BEGIN

namespace detail
{
    template<class _String>
    constexpr size_t length(const _String& s) { return std::size(s); }

    constexpr size_t length(char) { return 1; }
    constexpr size_t length(wchar_t) { return 1; }
    constexpr size_t length(int16_t) { return 6; }
    constexpr size_t length(uint16_t) { return 5; }
    constexpr size_t length(int32_t) { return 33; }
    constexpr size_t length(uint32_t) { return 32; }
    constexpr size_t length(int64_t) { return 65; }
    constexpr size_t length(uint64_t) { return 64; }
    constexpr size_t length(float) { return 32; }
    constexpr size_t length(double) { return 32; }
}

template<class... _Strings>
class string_builder
{
public:
    using Tuple = std::tuple<_Strings...>;

    constexpr string_builder() {}

    constexpr string_builder(_Strings... s) : strings_(std::forward<_Strings>(s)...) {}

    template<class _Tuple>
    constexpr string_builder(_Tuple&& s) : strings_(std::forward<_Tuple>(s)) {}

    template<class _String>
    constexpr auto operator%(_String&& s)
    {
        return string_builder<_Strings..., _String>( std::tuple_cat(strings_, std::forward_as_tuple(s)) );
    }

    template<class _String>
    constexpr _String to_string() const
    {
        return this->concatenate<_String>();
    }

    template<class _String>
    constexpr operator _String () const
    {
        return this->to_string<_String>();
    }

protected:
    template <class _String>
    constexpr auto concatenate() const
    {
        return this->concatenate_impl<_String>(std::index_sequence_for<_Strings...>{});
    }

    template <class _String, std::size_t... Idx>
    constexpr auto concatenate_impl(std::index_sequence<Idx...>) const
    {
        const auto n = (detail::length(std::get<Idx>(strings_)) + ... + 0);

        _String result;
        result.reserve(n);

        append(result, std::get<Idx>(strings_)...);
        return result;
    }

    template<class _String, class... Args>
    static void append(_String& buf, Args&&... args)
    {
        (do_append(buf, std::forward<Args>(args)), ...);
    }

    template<class _String, class _Type>
    static void do_append(_String& out, _Type t)
    {
        (out += t);
    }

    template<class _String>
    static void do_append(_String& out, int t)
    {
        (out += std::to_string(t));
    }

    template<class _String>
    static void do_append(_String& out, double t)
    {
        (out += std::to_string(t));
    }

    std::ostream& output(std::ostream& out) const
    {
        return output_impl(out, std::index_sequence_for<_Strings...>{});
    }

    template <std::size_t... Idx>
    std::ostream& output_impl(std::ostream& out, std::index_sequence<Idx...>) const
    {
        return append(out, std::get<Idx>(strings_)...);
    }

    template<class _Char, class _Traits, class... Args>
    static std::basic_ostream<_Char, _Traits>& append(std::basic_ostream<_Char, _Traits>& out, Args&&... args)
    {
        return (put(out, std::forward<Args>(args)), ...);
    }

    template<class _Char, class _Traits, class _Type>
    static std::basic_ostream<_Char, _Traits>& put(std::basic_ostream<_Char, _Traits>& out, _Type t)
    {
        return (out << t);
    }

    Tuple strings_;

public:
    friend inline std::ostream& operator<<(std::ostream& out, const string_builder& sb)
    {
        return sb.output(out);
    }
};


// std::string <> char
template<class _Char, class _Traits, class _Alloc>
auto operator%(const std::basic_string<_Char, _Traits, _Alloc>& s, _Char c)
{
    using string_type = std::basic_string<_Char, _Traits, _Alloc>;
    return string_builder<const string_type&, char>(s, c);
}

template<class _Char, class _Traits, class _Alloc>
auto operator%(_Char c, const std::basic_string<_Char, _Traits, _Alloc>& s)
{
    using string_type = std::basic_string<_Char, _Traits, _Alloc>;
    return string_builder<char, const string_type&>(c, s);
}

// std::string_view <> char
template<class _Char, class _Traits>
auto operator%(const std::basic_string_view<_Char, _Traits>& s, _Char c)
{
    using string_type = std::basic_string_view<_Char, _Traits>;
    return string_builder<const string_type&, char>(s, c);
}

template<class _Char, class _Traits>
auto operator%(_Char c, const std::basic_string_view<_Char, _Traits>& s)
{
    using string_type = std::basic_string_view<_Char, _Traits>;
    return string_builder<char, const string_type&>(c, s);
}


// std::string <> T
// Note: the enable_if<> resolve ambiguity on operator% overload for
// basic_string / basic_string_view and vice versa

template<class _Char, class _Traits, class _Alloc, class _Type>
typename std::enable_if<
    !std::is_same<_Type, std::basic_string_view<_Char, _Traits>>::value,
    string_builder<const std::basic_string<_Char, _Traits, _Alloc>&, _Type>
>::type
operator%(const std::basic_string<_Char, _Traits, _Alloc>& s, const _Type& c)
{
    using string_type = std::basic_string<_Char, _Traits, _Alloc>;
    return string_builder<const string_type&, _Type>(s, c);
}

template<class _Type, class _Char, class _Traits, class _Alloc>
typename std::enable_if<
    !std::is_same<_Type, std::basic_string_view<_Char, _Traits>>::value,
    string_builder<_Type, const std::basic_string<_Char, _Traits, _Alloc>&>
>::type
operator%(const _Type& c, const std::basic_string<_Char, _Traits, _Alloc>& s)
{
    using string_type = std::basic_string<_Char, _Traits, _Alloc>;
    return string_builder<_Type, const string_type&>(c, s);
}

// std::string_view <> T

template<class _Char, class _Traits, class _Type>
auto operator%(const std::basic_string_view<_Char, _Traits>& s, const _Type& c)
{
    using string_type = std::basic_string_view<_Char, _Traits>;
    return string_builder<const string_type&, _Type>(s, c);
}

template<class _Type, class _Char, class _Traits>
auto operator%(const _Type& c, const std::basic_string_view<_Char, _Traits>& s)
{
    using string_type = std::basic_string_view<_Char, _Traits>;
    return string_builder<_Type, const string_type&>(c, s);
}


template<class... _Args>
constexpr auto make_string(_Args&&... args)
{
    return string_builder<_Args...>(std::forward<_Args>(args)...);
}

_STDX_END
