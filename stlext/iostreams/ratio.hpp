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
#include <string>
#include <ratio>
#include <limits>

#include "../compability/string_view"

#include "../platform/common.h"

_STDX_BEGIN

/// Add binary prefixes (IEC 60027-2 A.2 and ISO/IEC 80000).
typedef std::ratio<(1ULL << 10), 1> kibi;
typedef std::ratio<(1ULL << 20), 1> mebi;
typedef std::ratio<(1ULL << 30), 1> gibi;
typedef std::ratio<(1ULL << 40), 1> tebi;
typedef std::ratio<(1ULL << 50), 1> pebi;
typedef std::ratio<(1ULL << 60), 1> exbi;


template <class _Ratio, class _Char>
struct ratio_traits
{
    static const std::basic_string<_Char>& symbol() {
        return prefix();
    }

    static const std::basic_string<_Char>& prefix() {
        static std::basic_string<_Char> str;
        if (str.empty()) {
            str.reserve(2 * (std::numeric_limits<intmax_t>::digits10 + 2) + 3);
            str += _Char('[');
            str += std::to_string(_Ratio::num);
            str += _Char('/');
            str += std::to_string(_Ratio::den);
            str += _Char(']');
            str.shrink_to_fit();
        }
        return str;
    }
};




template <>
struct ratio_traits<std::ratio<1, 1>, char>
{
    static const std::string_view symbol() { return std::string_view(); }
    static const std::string_view prefix() { return std::string_view(); }
};




// atto

template <>
struct ratio_traits<std::atto, char>
{
    static const std::string_view symbol() { return std::string_view("a", 1); }
    static const std::string_view prefix() { return std::string_view("atto", 4); }
};

template <>
struct ratio_traits<std::atto, char16_t>
{
    static const std::u16string_view symbol() { return std::u16string_view(u"a", 1); }
    static const std::u16string_view prefix() { return std::u16string_view(u"atto", 4); }
};

template <>
struct ratio_traits<std::atto, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"a", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"atto", 4); }
};

template <>
struct ratio_traits<std::atto, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"a", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"atto", 4); }
};




// femto

template <>
struct ratio_traits<std::femto, char>
{
    static std::string_view symbol() { return std::string_view("f", 1); }
    static std::string_view prefix() { return std::string_view("femto", 5); }
};


template <>
struct ratio_traits<std::femto, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"f", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"femto", 5); }
};

template <>
struct ratio_traits<std::femto, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"f", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"femto", 5); }
};


template <>
struct ratio_traits<std::femto, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"f", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"femto", 5); }
};




// pico

template <>
struct ratio_traits<std::pico, char>
{
    static std::string_view symbol() { return std::string_view("p", 1); }
    static std::string_view prefix() { return std::string_view("pico", 4); }
};


template <>
struct ratio_traits<std::pico, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"p", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"pico", 4); }
};

template <>
struct ratio_traits<std::pico, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"p", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"pico", 4); }
};


template <>
struct ratio_traits<std::pico, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"p", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"pico", 4); }
};




// nano

template <>
struct ratio_traits<std::nano, char>
{
    static std::string_view symbol() { return std::string_view("n", 1); }
    static std::string_view prefix() { return std::string_view("nano", 4); }
};

template <>
struct ratio_traits<std::nano, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"n", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"nano", 4); }
};

template <>
struct ratio_traits<std::nano, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"n", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"nano", 4); }
};


template <>
struct ratio_traits<std::nano, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"n", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"nano", 4); }
};


// micro

template <>
struct ratio_traits<std::micro, char>
{
    static std::string_view symbol() { return std::string_view("\xC2\xB5", 2); } // \xC2\xB5
    static std::string_view prefix() { return std::string_view("micro", 5); }
};

template <>
struct ratio_traits<std::micro, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"\xB5", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"micro", 5); }
};

template <>
struct ratio_traits<std::micro, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"\xB5", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"micro", 5); }
};


template <>
struct ratio_traits<std::micro, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"\xB5", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"micro", 5); }
};




// milli

template <>
struct ratio_traits<std::milli, char>
{
    static std::string_view symbol() { return std::string_view("m", 1); }
    static std::string_view prefix() { return std::string_view("milli", 5); }
};


template <>
struct ratio_traits<std::milli, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"m", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"milli", 5); }
};

template <>
struct ratio_traits<std::milli, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"m", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"milli", 5); }
};


template <>
struct ratio_traits<std::milli, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"m", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"milli", 5); }
};




// centi

template <>
struct ratio_traits<std::centi, char>
{
    static std::string_view symbol() { return std::string_view("c", 1); }
    static std::string_view prefix() { return std::string_view("centi", 5); }
};



template <>
struct ratio_traits<std::centi, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"c", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"centi", 5); }
};

template <>
struct ratio_traits<std::centi, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"c", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"centi", 5); }
};

template <>
struct ratio_traits<std::centi, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"c", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"centi", 5); }
};




// deci

template <>
struct ratio_traits<std::deci, char>
{
    static std::string_view symbol() { return std::string_view("d", 1); }
    static std::string_view prefix() { return std::string_view("deci", 4); }
};


template <>
struct ratio_traits<std::deci, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"d", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"deci", 4); }
};

template <>
struct ratio_traits<std::deci, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"d", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"deci", 4); }
};


template <>
struct ratio_traits<std::deci, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"d", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"deci", 4); }
};




// unit

// deca

template <>
struct ratio_traits<std::deca, char>
{
    static std::string_view symbol() { return std::string_view("da", 2); }
    static std::string_view prefix() { return std::string_view("deca", 4); }
};

template <>
struct ratio_traits<std::deca, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"da", 2); }
    static std::u16string_view prefix() { return std::u16string_view(u"deca", 4); }
};

template <>
struct ratio_traits<std::deca, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"da", 2); }
    static std::u32string_view prefix() { return std::u32string_view(U"deca", 4); }
};

template <>
struct ratio_traits<std::deca, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"da", 2); }
    static std::wstring_view prefix() { return std::wstring_view(L"deca", 4); }
};





// hecto

template <>
struct ratio_traits<std::hecto, char>
{
    static std::string_view symbol() { return std::string_view("h", 1); }
    static std::string_view prefix() { return std::string_view("hecto", 5); }
};

template <>
struct ratio_traits<std::hecto, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"h", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"hecto", 5); }
};

template <>
struct ratio_traits<std::hecto, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"h", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"hecto", 5); }
};

template <>
struct ratio_traits<std::hecto, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"h", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"hecto", 5); }
};





// kilo

template <>
struct ratio_traits<std::kilo, char>
{
    static std::string_view symbol() { return std::string_view("k", 1); }
    static std::string_view prefix() { return std::string_view("kilo", 4); }
};

template <>
struct ratio_traits<std::kilo, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"k", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"kilo", 4); }
};

template <>
struct ratio_traits<std::kilo, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"k", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"kilo", 4); }
};


template <>
struct ratio_traits<std::kilo, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"k", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"kilo", 4); }
};





// mega

template <>
struct ratio_traits<std::mega, char>
{
    static std::string_view symbol() { return std::string_view("M", 1); }
    static std::string_view prefix() { return std::string_view("mega", 4); }
};

template <>
struct ratio_traits<std::mega, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"M", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"mega", 4); }
};

template <>
struct ratio_traits<std::mega, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"M", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"mega", 4); }
};

template <>
struct ratio_traits<std::mega, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"M", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"mega", 4); }
};




// giga

template <>
struct ratio_traits<std::giga, char>
{
    static std::string_view symbol() { return std::string_view("G", 1); }
    static std::string_view prefix() { return std::string_view("giga", 4); }
};

template <>
struct ratio_traits<std::giga, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"G", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"giga", 4); }
};

template <>
struct ratio_traits<std::giga, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"G", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"giga", 4); }
};

template <>
struct ratio_traits<std::giga, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"G", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"giga", 4); }
};




// tera

template <>
struct ratio_traits<std::tera, char>
{
    static std::string_view symbol() { return std::string_view("T", 1); }
    static std::string_view prefix() { return std::string_view("tera", 4); }
};

template <>
struct ratio_traits<std::tera, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"T", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"tera", 4); }
};

template <>
struct ratio_traits<std::tera, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"T", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"tera", 4); }
};

template <>
struct ratio_traits<std::tera, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"T", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"tera", 4); }
};




// peta

template <>
struct ratio_traits<std::peta, char>
{
    static std::string_view symbol() { return std::string_view("P", 1); }
    static std::string_view prefix() { return std::string_view("peta", 4); }
};

template <>
struct ratio_traits<std::peta, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"P", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"peta", 4); }
};

template <>
struct ratio_traits<std::peta, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"P", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"peta", 4); }
};

template <>
struct ratio_traits<std::peta, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"P", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"peta", 4); }
};





// exa

template <>
struct ratio_traits<std::exa, char>
{
    static std::string_view symbol() { return std::string_view("E", 1); }
    static std::string_view prefix() { return std::string_view("exa", 3); }
};

template <>
struct ratio_traits<std::exa, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"E", 1); }
    static std::u16string_view prefix() { return std::u16string_view(u"exa", 3); }
};

template <>
struct ratio_traits<std::exa, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"E", 1); }
    static std::u32string_view prefix() { return std::u32string_view(U"exa", 3); }
};

template <>
struct ratio_traits<std::exa, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"E", 1); }
    static std::wstring_view prefix() { return std::wstring_view(L"exa", 3); }
};




// kibi

template <>
struct ratio_traits<kibi, char>
{
    static std::string_view symbol() { return std::string_view("Ki", 2); }
    static std::string_view prefix() { return std::string_view("kibi", 4); }
};

template <>
struct ratio_traits<kibi, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"Ki", 2); }
    static std::u16string_view prefix() { return std::u16string_view(u"kibi", 4); }
};

template <>
struct ratio_traits<kibi, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"Ki", 2); }
    static std::u32string_view prefix() { return std::u32string_view(U"kibi", 4); }
};

template <>
struct ratio_traits<kibi, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"Ki", 2); }
    static std::wstring_view prefix() { return std::wstring_view(L"kibi", 4); }
};




// mebi

template <>
struct ratio_traits<mebi, char>
{
    static std::string_view symbol() { return std::string_view("Mi", 2); }
    static std::string_view prefix() { return std::string_view("mebi", 4); }
};

template <>
struct ratio_traits<mebi, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"Mi", 2); }
    static std::u16string_view prefix() { return std::u16string_view(u"mebi", 4); }
};

template <>
struct ratio_traits<mebi, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"Mi", 2); }
    static std::u32string_view prefix() { return std::u32string_view(U"mebi", 4); }
};


template <>
struct ratio_traits<mebi, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"Mi", 2); }
    static std::wstring_view prefix() { return std::wstring_view(L"mebi", 4); }
};





// gibi

template <>
struct ratio_traits<gibi, char>
{
    static std::string_view symbol() { return std::string_view("Gi", 2); }
    static std::string_view prefix() { return std::string_view("gibi", 4); }
};

template <>
struct ratio_traits<gibi, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"Gi", 2); }
    static std::u16string_view prefix() { return std::u16string_view(u"gibi", 4); }
};

template <>
struct ratio_traits<gibi, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"Gi", 2); }
    static std::u32string_view prefix() { return std::u32string_view(U"gibi", 4); }
};

template <>
struct ratio_traits<gibi, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"Gi", 2); }
    static std::wstring_view prefix() { return std::wstring_view(L"gibi", 4); }
};




// tebi

template <>
struct ratio_traits<tebi, char>
{
    static std::string_view symbol() { return std::string_view("Ti", 2); }
    static std::string_view prefix() { return std::string_view("tebi", 4); }
};

template <>
struct ratio_traits<tebi, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"Ti", 2); }
    static std::u16string_view prefix() { return std::u16string_view(u"tebi", 4); }
};

template <>
struct ratio_traits<tebi, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"Ti", 2); }
    static std::u32string_view prefix() { return std::u32string_view(U"tebi", 4); }
};

template <>
struct ratio_traits<tebi, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"Ti", 2); }
    static std::wstring_view prefix() { return std::wstring_view(L"tebi", 4); }
};





// pebi

template <>
struct ratio_traits<pebi, char>
{
    static std::string_view symbol() { return std::string_view("Pi", 2); }
    static std::string_view prefix() { return std::string_view("pebi", 4); }
};


template <>
struct ratio_traits<pebi, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"Pi", 2); }
    static std::u16string_view prefix() { return std::u16string_view(u"pebi", 4); }
};

template <>
struct ratio_traits<pebi, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"Pi", 2); }
    static std::u32string_view prefix() { return std::u32string_view(U"pebi", 4); }
};

template <>
struct ratio_traits<pebi, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"Pi", 2); }
    static std::wstring_view prefix() { return std::wstring_view(L"pebi", 4); }
};

template <>
struct ratio_traits<exbi, char>
{
    static std::string_view symbol() { return std::string_view("Ei", 2); }
    static std::string_view prefix() { return std::string_view("exbi", 4); }
};

template <>
struct ratio_traits<exbi, char16_t>
{
    static std::u16string_view symbol() { return std::u16string_view(u"Ei", 2); }
    static std::u16string_view prefix() { return std::u16string_view(u"exbi", 4); }
};

template <>
struct ratio_traits<exbi, char32_t>
{
    static std::u32string_view symbol() { return std::u32string_view(U"Ei", 2); }
    static std::u32string_view prefix() { return std::u32string_view(U"exbi", 4); }
};


template <>
struct ratio_traits<exbi, wchar_t>
{
    static std::wstring_view symbol() { return std::wstring_view(L"Ei", 2); }
    static std::wstring_view prefix() { return std::wstring_view(L"exbi", 4); }
};



_STDX_END

