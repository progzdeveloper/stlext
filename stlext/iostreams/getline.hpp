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
#include <iostream>
#include <streambuf>

#include "../platform/common.h"

_STDX_BEGIN

template<
class _Elem,
    class _Traits,
    class _Alloc,
    class _Predicate
>
inline std::basic_istream<_Elem, _Traits>& getline(std::basic_istream<_Elem, _Traits>& stream,
                                                   std::basic_string<_Elem, _Traits, _Alloc>& str,
                                                   const _Predicate& pr)
{   // get characters into string, discard delimiters

    using namespace std;
    typedef basic_istream<_Elem, _Traits> stream_type;

    ios_base::iostate state = ios_base::goodbit;
    bool changed = false;

    const typename stream_type::sentry guard(stream, true);

    if (guard)
    {	// state okay, extract characters
        try {
            str.erase(); // clear string

            typename _Traits::int_type _Meta = stream.rdbuf()->sgetc();
            for(; ; _Meta = stream.rdbuf()->snextc())
            {	// find first non-escaped character of EOF
                if (_Traits::eq_int_type(_Traits::eof(), _Meta))
                {	// end of file, quit
                    state |= ios_base::eofbit;
                    state |= ios_base::failbit;
                    break;
                }
                if (!pr(_Meta))
                    break;
            }

            if (state & ios_base::eofbit) { // end-of-stream
                stream.setstate(state);
                return (stream); // bail out
            }

            for (; ; _Meta = stream.rdbuf()->snextc())
            {
                if (_Traits::eq_int_type(_Traits::eof(), _Meta))
                {	// end of file, quit
                    state |= ios_base::eofbit;
                    break;
                }
                else if (pr(_Meta))
                {	// got a delimiter, discard it and quit
                    changed = true;
                    stream.rdbuf()->sbumpc();
                    break;
                }
                else if (str.max_size() <= str.size())
                {	// string too large, quit
                    state |= ios_base::failbit;
                    break;
                }
                else
                {	// got a character, add it to string
                    str += _Traits::to_char_type(_Meta);
                    changed = true;
                }
            }
        }
        catch (...) {
            stream.setstate(ios_base::badbit);
        }
    }

    if (!changed)
        state |= ios_base::failbit;
    stream.setstate(state);
    return (stream);
}

_STDX_END

