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
#include "version.h"
#include "platform.h"
#include "compiler.h"
#include "arch.h"
#include "error.h"


#ifndef _MAX_STACK_SIZE
#define _MAX_STACK_SIZE (1 << 19)
#endif



#ifndef __stack_alloc
#define __stack_alloc                   \
private:                                \
    void* operator new(size_t)          \
    void* operator new(size_t,void*);   \
    void* operator new[](size_t);       \
    void* operator new[](size_t,void*); \
    void operator delete(void*);        \
    void operator delete[](void*);
#endif


#ifndef __disable_copy
#ifndef _NO_CPP11X 
#define __disable_copy(_Class)                    \
	_Class(const _Class&) = delete;               \
	_Class& operator= (const _Class&) = delete;
#else // No C++11
#define __disable_copy(_Class)			\
private:								\
	_Class(const _Class&);				\
	_Class& operator= (const _Class&);
#endif // _NO_CPP11X
#endif // __disable_copy


// C++11x features emulation
#ifndef _NO_CPP11X 
#include <cstdint>

#else // emulating C++11x features

// emulate stdint.h header
#define INT8_C(x)    (x)
#define INT16_C(x)   (x)
#define INT32_C(x)   (x)
#define INT64_C(x)   (x ## LL)

#define UINT8_C(x)   (x)
#define UINT16_C(x)  (x)
#define UINT32_C(x)  (x ## U)
#define UINT64_C(x)  (x ## ULL)

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

// emulate static_assert
#define __STATIC_ASSERT_CONCAT(A, B) __STATIC_ASSERT_CONCAT_IMPL(A, B)
#define __STATIC_ASSERT_CONCAT_IMPL(A, B) A ## B
#define static_assert(_Expr, _Message) typedef char __STATIC_ASSERT_CONCAT(__static_assert_, __LINE__)[ (!!(_Expr)) ]

#endif




