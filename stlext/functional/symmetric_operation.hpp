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

#include "../platform/common.h"

_STDX_BEGIN

template<class _Op>
struct symmetric_operation;

template<class T>
struct symmetric_operation< std::less<T> > {
    typedef std::greater<T> type;
};

template<class T>
struct symmetric_operation< std::greater<T> > {
    typedef std::less<T> type;
};

template<class T>
struct symmetric_operation< std::equal_to<T> > {
    typedef std::not_equal_to<T> type;
};

template<class T>
struct symmetric_operation< std::not_equal_to<T> > {
    typedef std::equal_to<T> type;
};


template<class T>
struct symmetric_operation< std::plus<T> > {
    typedef std::minus<T> type;
};

template<class T>
struct symmetric_operation< std::minus<T> > {
    typedef std::plus<T> type;
};

template<class T>
struct symmetric_operation< std::multiplies<T> > {
    typedef std::divides<T> type;
};

template<class T>
struct symmetric_operation< std::divides<T> > {
    typedef std::multiplies<T> type;
};




template<class _Op1, class _Op2>
struct is_symmetric : std::false_type {};



template<class T1, class T2>
struct is_symmetric< std::less<T1>, std::greater<T2> > : std::true_type {};

template<class T1, class T2>
struct is_symmetric< std::greater<T2>, std::less<T1> > : std::true_type {};



template<class T1, class T2>
struct is_symmetric< std::equal_to<T1>, std::not_equal_to<T2> > : std::true_type {};

template<class T1, class T2>
struct is_symmetric< std::not_equal_to<T2>, std::equal_to<T1> > : std::true_type {};



template<class T1, class T2>
struct is_symmetric< std::plus<T1>, std::minus<T2> > : std::true_type {};

template<class T1, class T2>
struct is_symmetric< std::minus<T2>, std::plus<T1> > : std::true_type {};



template<class T1, class T2>
struct is_symmetric< std::multiplies<T1>, std::divides<T2> > : std::true_type {};

template<class T1, class T2>
struct is_symmetric< std::divides<T2>, std::multiplies<T1> > : std::true_type {};

_STDX_END
