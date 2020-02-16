// Copyright (c) 2014-2016, Michael Polukarov (Russia).
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

#define LE 1

_STDX_BEGIN

// compute log2(x) for unsigned integer
static inline size_t log2i(size_t x)
{
	if (x == 0)
		return 0;
	x -= 1;
	union {
		unsigned i[2];
		double dbl;
	};
	
	dbl = (double)x + 0.5;
	return ( 32 - (1054 - (i[LE] >> 20)) );
}

inline int round(double x)
{ 
	return (x >= 0.0 ? int(x + 0.5) : int(x - double(int(x - 1)) + 0.5) + int(x - 1));
}

inline int round(float x)
{ 
	return (x >= 0.0f ? int(x + 0.5f) : int(x - float(int(x - 1)) + 0.5f) + int(x - 1));
}


template<typename _Numeric>
inline _Numeric signum(_Numeric x) {
	return ((x > _Numeric(0)) ? _Numeric(1) : _Numeric(0));
}

_STDX_END