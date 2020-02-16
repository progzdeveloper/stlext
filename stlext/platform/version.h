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
/*
STDX_VERSION is (major << 16) + (minor << 8) + build.
*/
#define STDX_VERSION ((STDX_VERSION_MAJOR << 16) + (STDX_VERSION_MINOR << 8) + STDX_VERSION_BUILD)
/*
can be used like #if (STDX_VERSION >= STDX_VERSION_CHECK(4, 4, 0))
*/
#define STDX_VERSION_CHECK(major, minor, build) ((major<<16)|(minor<<8)|(build))

#ifndef __STDX_VERSION_DEFINED
#define STDX_VERSION_STR   "1.0.0"
#define STDX_VERSION_MAJOR 1
#define STDX_VERSION_MINOR 0
#define STDX_VERSION_BUILD 0
#define __STDX_VERSION_DEFINED 1
#endif

#ifndef _STDX_BEGIN
#define _STDX_BEGIN namespace stdx {
#endif

#ifndef _STDX_END
#define _STDX_END   }
#endif


