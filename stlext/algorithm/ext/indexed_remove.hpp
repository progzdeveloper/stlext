// Copyright (c) 2020, Michael Polukarov (Russia).
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

#include "../../platform/common.h"

#include "../../iterator/enumeration.hpp"

_STDX_BEGIN

template<class _FwdIt, class _IndexIt>
_FwdIt remove_indices(_FwdIt first, _FwdIt last, _IndexIt first_idx, _IndexIt last_idx)
{
    typedef typename std::iterator_traits<_IndexIt>::value_type index_type;
    typedef enumerable_iterator<_FwdIt, index_type> iterator;

    if (first_idx == last_idx)
        return last; // nothing to remove

    if (first == last)
        return last; // nothing to remove

    iterator it(first, 0);
    for (; it != last; ++it) {
        // any more to remove?
        if (it->first == *first_idx) {
            ++first_idx;
            break;
        }
    }

    if (it == last)
        return it->second;

    for(auto i = it; ++i != last; ) {
        if (first_idx != last_idx && i->first == *first_idx) {
            ++first_idx;
        } else {
            *(it->second) = std::move(*(i->second));
            ++it;
        }
    }
    return it->second;
}


_STDX_END

