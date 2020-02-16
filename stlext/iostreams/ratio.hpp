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
	static std::basic_string<_Char> symbol() {
		return prefix(); 
	}

	static std::basic_string<_Char> prefix() {
		std::basic_ostringstream<_Char> os;
		os << _Char('[') << _Ratio::num << _Char('/')
			<< _Ratio::den << _Char(']');
		return os.str();
	}
};




template <>
struct ratio_traits<std::ratio<1, 1>, char>
{
	static std::string symbol() { return std::string(); }
	static std::string prefix() { return std::string(); }
};




// atto

template <>
struct ratio_traits<std::atto, char>
{
	static std::string symbol() { return std::string(1, 'a'); }
	static std::string prefix() { return std::string("atto"); }
};

template <>
struct ratio_traits<std::atto, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'a'); }
	static std::u16string prefix() { return std::u16string(u"atto"); }
};

template <>
struct ratio_traits<std::atto, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'a'); }
	static std::u32string prefix() { return std::u32string(U"atto"); }
};

template <>
struct ratio_traits<std::atto, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'a'); }
	static std::wstring prefix() { return std::wstring(L"atto"); }
};




// femto

template <>
struct ratio_traits<std::femto, char>
{
	static std::string symbol() { return std::string(1, 'f'); }
	static std::string prefix() { return std::string("femto"); }
};


template <>
struct ratio_traits<std::femto, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'f'); }
	static std::u16string prefix() { return std::u16string(u"femto"); }
};

template <>
struct ratio_traits<std::femto, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'f'); }
	static std::u32string prefix() { return std::u32string(U"femto"); }
};


template <>
struct ratio_traits<std::femto, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'f'); }
	static std::wstring prefix() { return std::wstring(L"femto"); }
};




// pico

template <>
struct ratio_traits<std::pico, char>
{
	static std::string symbol() { return std::string(1, 'p'); }
	static std::string prefix() { return std::string("pico"); }
};


template <>
struct ratio_traits<std::pico, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'p'); }
	static std::u16string prefix() { return std::u16string(u"pico"); }
};

template <>
struct ratio_traits<std::pico, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'p'); }
	static std::u32string prefix() { return std::u32string(U"pico"); }
};


template <>
struct ratio_traits<std::pico, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'p'); }
	static std::wstring prefix() { return std::wstring(L"pico"); }
};




// nano

template <>
struct ratio_traits<std::nano, char>
{
	static std::string symbol() { return std::string(1, 'n'); }
	static std::string prefix() { return std::string("nano"); }
};

template <>
struct ratio_traits<std::nano, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'n'); }
	static std::u16string prefix() { return std::u16string(u"nano"); }
};

template <>
struct ratio_traits<std::nano, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'n'); }
	static std::u32string prefix() { return std::u32string(U"nano"); }
};


template <>
struct ratio_traits<std::nano, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'n'); }
	static std::wstring prefix() { return std::wstring(L"nano"); }
};


// micro

template <>
struct ratio_traits<std::micro, char>
{
    static std::string symbol() { return std::string("\xC2\xB5"); } // \xC2\xB5
	static std::string prefix() { return std::string("micro"); }
};

template <>
struct ratio_traits<std::micro, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'\xB5'); }
	static std::u16string prefix() { return std::u16string(u"micro"); }
};

template <>
struct ratio_traits<std::micro, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'\xB5'); }
	static std::u32string prefix() { return std::u32string(U"micro"); }
};


template <>
struct ratio_traits<std::micro, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'\xB5'); }
	static std::wstring prefix() { return std::wstring(L"micro"); }
};




// milli

template <>
struct ratio_traits<std::milli, char>
{
	static std::string symbol() { return std::string(1, 'm'); }
	static std::string prefix() { return std::string("milli"); }
};


template <>
struct ratio_traits<std::milli, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'm'); }
	static std::u16string prefix() { return std::u16string(u"milli"); }
};

template <>
struct ratio_traits<std::milli, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'm'); }
	static std::u32string prefix() { return std::u32string(U"milli"); }
};


template <>
struct ratio_traits<std::milli, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'm'); }
	static std::wstring prefix() { return std::wstring(L"milli"); }
};




// centi

template <>
struct ratio_traits<std::centi, char>
{
	static std::string symbol() { return std::string(1, 'c'); }
	static std::string prefix() { return std::string("centi"); }
};



template <>
struct ratio_traits<std::centi, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'c'); }
	static std::u16string prefix() { return std::u16string(u"centi"); }
};

template <>
struct ratio_traits<std::centi, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'c'); }
	static std::u32string prefix() { return std::u32string(U"centi"); }
};

template <>
struct ratio_traits<std::centi, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'c'); }
	static std::wstring prefix() { return std::wstring(L"centi"); }
};




// deci

template <>
struct ratio_traits<std::deci, char>
{
	static std::string symbol() { return std::string(1, 'd'); }
	static std::string prefix() { return std::string("deci"); }
};


template <>
struct ratio_traits<std::deci, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'd'); }
	static std::u16string prefix() { return std::u16string(u"deci"); }
};

template <>
struct ratio_traits<std::deci, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'd'); }
	static std::u32string prefix() { return std::u32string(U"deci"); }
};


template <>
struct ratio_traits<std::deci, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'd'); }
	static std::wstring prefix() { return std::wstring(L"deci"); }
};




// unit

// deca

template <>
struct ratio_traits<std::deca, char>
{
	static std::string symbol() { return std::string("da"); }
	static std::string prefix() { return std::string("deca"); }
};

template <>
struct ratio_traits<std::deca, char16_t>
{
	static std::u16string symbol() { return std::u16string(u"da"); }
	static std::u16string prefix() { return std::u16string(u"deca"); }
};

template <>
struct ratio_traits<std::deca, char32_t>
{
	static std::u32string symbol() { return std::u32string(U"da"); }
	static std::u32string prefix() { return std::u32string(U"deca"); }
};

template <>
struct ratio_traits<std::deca, wchar_t>
{
	static std::wstring symbol() { return std::wstring(L"da"); }
	static std::wstring prefix() { return std::wstring(L"deca"); }
};





// hecto

template <>
struct ratio_traits<std::hecto, char>
{
	static std::string symbol() { return std::string(1, 'h'); }
	static std::string prefix() { return std::string("hecto"); }
};

template <>
struct ratio_traits<std::hecto, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'h'); }
	static std::u16string prefix() { return std::u16string(u"hecto"); }
};

template <>
struct ratio_traits<std::hecto, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'h'); }
	static std::u32string prefix() { return std::u32string(U"hecto"); }
};

template <>
struct ratio_traits<std::hecto, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'h'); }
	static std::wstring prefix() { return std::wstring(L"hecto"); }
};





// kilo

template <>
struct ratio_traits<std::kilo, char>
{
	static std::string symbol() { return std::string(1, 'k'); }
	static std::string prefix() { return std::string("kilo"); }
};

template <>
struct ratio_traits<std::kilo, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'k'); }
	static std::u16string prefix() { return std::u16string(u"kilo"); }
};

template <>
struct ratio_traits<std::kilo, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'k'); }
	static std::u32string prefix() { return std::u32string(U"kilo"); }
};


template <>
struct ratio_traits<std::kilo, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'k'); }
	static std::wstring prefix() { return std::wstring(L"kilo"); }
};





// mega

template <>
struct ratio_traits<std::mega, char>
{
	static std::string symbol() { return std::string(1, 'M'); }
	static std::string prefix() { return std::string("mega"); }
};

template <>
struct ratio_traits<std::mega, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'M'); }
	static std::u16string prefix() { return std::u16string(u"mega"); }
};

template <>
struct ratio_traits<std::mega, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'M'); }
	static std::u32string prefix() { return std::u32string(U"mega"); }
};

template <>
struct ratio_traits<std::mega, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'M'); }
	static std::wstring prefix() { return std::wstring(L"mega"); }
};




// giga

template <>
struct ratio_traits<std::giga, char>
{
	static std::string symbol() { return std::string(1, 'G'); }
	static std::string prefix() { return std::string("giga"); }
};

template <>
struct ratio_traits<std::giga, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'G'); }
	static std::u16string prefix() { return std::u16string(u"giga"); }
};

template <>
struct ratio_traits<std::giga, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'G'); }
	static std::u32string prefix() { return std::u32string(U"giga"); }
};

template <>
struct ratio_traits<std::giga, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'G'); }
	static std::wstring prefix() { return std::wstring(L"giga"); }
};




// tera

template <>
struct ratio_traits<std::tera, char>
{
	static std::string symbol() { return std::string(1, 'T'); }
	static std::string prefix() { return std::string("tera"); }
};

template <>
struct ratio_traits<std::tera, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'T'); }
	static std::u16string prefix() { return std::u16string(u"tera"); }
};

template <>
struct ratio_traits<std::tera, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'T'); }
	static std::u32string prefix() { return std::u32string(U"tera"); }
};

template <>
struct ratio_traits<std::tera, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'T'); }
	static std::wstring prefix() { return std::wstring(L"tera"); }
};




// peta

template <>
struct ratio_traits<std::peta, char>
{
	static std::string symbol() { return std::string(1, 'P'); }
	static std::string prefix() { return std::string("peta"); }
};

template <>
struct ratio_traits<std::peta, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'P'); }
	static std::u16string prefix() { return std::u16string(u"peta"); }
};

template <>
struct ratio_traits<std::peta, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'P'); }
	static std::u32string prefix() { return std::u32string(U"peta"); }
};

template <>
struct ratio_traits<std::peta, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'P'); }
	static std::wstring prefix() { return std::wstring(L"peta"); }
};





// exa

template <>
struct ratio_traits<std::exa, char>
{
	static std::string symbol() { return std::string(1, 'E'); }
	static std::string prefix() { return std::string("exa"); }
};

template <>
struct ratio_traits<std::exa, char16_t>
{
	static std::u16string symbol() { return std::u16string(1, u'E'); }
	static std::u16string prefix() { return std::u16string(u"exa"); }
};

template <>
struct ratio_traits<std::exa, char32_t>
{
	static std::u32string symbol() { return std::u32string(1, U'E'); }
	static std::u32string prefix() { return std::u32string(U"exa"); }
};

template <>
struct ratio_traits<std::exa, wchar_t>
{
	static std::wstring symbol() { return std::wstring(1, L'E'); }
	static std::wstring prefix() { return std::wstring(L"exa"); }
};




// kibi

template <>
struct ratio_traits<kibi, char>
{
	static std::string symbol() { return std::string("Ki"); }
	static std::string prefix() { return std::string("kibi"); }
};

template <>
struct ratio_traits<kibi, char16_t>
{
	static std::u16string symbol() { return std::u16string(u"Ki"); }
	static std::u16string prefix() { return std::u16string(u"kibi"); }
};

template <>
struct ratio_traits<kibi, char32_t>
{
	static std::u32string symbol() { return std::u32string(U"Ki"); }
	static std::u32string prefix() { return std::u32string(U"kibi"); }
};

template <>
struct ratio_traits<kibi, wchar_t>
{
	static std::wstring symbol() { return std::wstring(L"Ki"); }
	static std::wstring prefix() { return std::wstring(L"kibi"); }
};




// mebi

template <>
struct ratio_traits<mebi, char>
{
	static std::string symbol() { return std::string("Mi"); }
	static std::string prefix() { return std::string("mebi"); }
};

template <>
struct ratio_traits<mebi, char16_t>
{
	static std::u16string symbol() { return std::u16string(u"Mi"); }
	static std::u16string prefix() { return std::u16string(u"mebi"); }
};

template <>
struct ratio_traits<mebi, char32_t>
{
	static std::u32string symbol() { return std::u32string(U"Mi"); }
	static std::u32string prefix() { return std::u32string(U"mebi"); }
};


template <>
struct ratio_traits<mebi, wchar_t>
{
	static std::wstring symbol() { return std::wstring(L"Mi"); }
	static std::wstring prefix() { return std::wstring(L"mebi"); }
};





// gibi

template <>
struct ratio_traits<gibi, char>
{
	static std::string symbol() { return std::string("Gi"); }
	static std::string prefix() { return std::string("gibi"); }
};

template <>
struct ratio_traits<gibi, char16_t>
{
	static std::u16string symbol() { return std::u16string(u"Gi"); }
	static std::u16string prefix() { return std::u16string(u"gibi"); }
};

template <>
struct ratio_traits<gibi, char32_t>
{
	static std::u32string symbol() { return std::u32string(U"Gi"); }
	static std::u32string prefix() { return std::u32string(U"gibi"); }
};

template <>
struct ratio_traits<gibi, wchar_t>
{
	static std::wstring symbol() { return std::wstring(L"Gi"); }
	static std::wstring prefix() { return std::wstring(L"gibi"); }
};




// tebi

template <>
struct ratio_traits<tebi, char>
{
	static std::string symbol() { return std::string("Ti"); }
	static std::string prefix() { return std::string("tebi"); }
};

template <>
struct ratio_traits<tebi, char16_t>
{
	static std::u16string symbol() { return std::u16string(u"Ti"); }
	static std::u16string prefix() { return std::u16string(u"tebi"); }
};

template <>
struct ratio_traits<tebi, char32_t>
{
	static std::u32string symbol() { return std::u32string(U"Ti"); }
	static std::u32string prefix() { return std::u32string(U"tebi"); }
};

template <>
struct ratio_traits<tebi, wchar_t>
{
	static std::wstring symbol() { return std::wstring(L"Ti"); }
	static std::wstring prefix() { return std::wstring(L"tebi"); }
};





// pebi

template <>
struct ratio_traits<pebi, char>
{
	static std::string symbol() { return std::string("Pi"); }
	static std::string prefix() { return std::string("pebi"); }
};


template <>
struct ratio_traits<pebi, char16_t>
{
	static std::u16string symbol() { return std::u16string(u"Pi"); }
	static std::u16string prefix() { return std::u16string(u"pebi"); }
};

template <>
struct ratio_traits<pebi, char32_t>
{
	static std::u32string symbol() { return std::u32string(U"Pi"); }
	static std::u32string prefix() { return std::u32string(U"pebi"); }
};

template <>
struct ratio_traits<pebi, wchar_t>
{
	static std::wstring symbol() { return std::wstring(L"Pi"); }
	static std::wstring prefix() { return std::wstring(L"pebi"); }
};

template <>
struct ratio_traits<exbi, char>
{
	static std::string symbol() { return std::string("Ei"); }
	static std::string prefix() { return std::string("exbi"); }
};

template <>
struct ratio_traits<exbi, char16_t>
{
	static std::u16string symbol() { return std::u16string(u"Ei"); }
	static std::u16string prefix() { return std::u16string(u"exbi"); }
};

template <>
struct ratio_traits<exbi, char32_t>
{
	static std::u32string symbol() { return std::u32string(U"Ei"); }
	static std::u32string prefix() { return std::u32string(U"exbi"); }
};


template <>
struct ratio_traits<exbi, wchar_t>
{
	static std::wstring symbol() { return std::wstring(L"Ei"); }
	static std::wstring prefix() { return std::wstring(L"exbi"); }
};



_STDX_END

