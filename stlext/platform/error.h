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

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "version.h"  // library version stuff
#include "compiler.h" // compiler detection

#ifndef _DISABLE_EXCEPTIONS
#include <stdexcept>
#endif
#ifdef STDX_CMPLR_MSVC
#include <crtdbg.h>
#endif


#ifndef _STDX_ERROR_MESSAGE_MAX_BUFFER_SIZE
#define _STDX_ERROR_MESSAGE_MAX_BUFFER_SIZE 4096 // maximum size of error message
#endif



#ifdef _STDX_DEBUG

#define _STDX_EFATAL -1
#define _STDX_EWARN  1
#define _STDX_EDEBUG 0


#ifdef _STDX_DISABLE_EXCEPTIONS
#define __stdx_except(_Exception, _Message, ...) ::stdx::detail::print_message(_STDX_EFATAL, __FILE__, __LINE__, _Message, ##__VA_ARGS__)
#else
#define __stdx_except(_Exception, _Message, ...) ::stdx::detail::raise_exception<_Exception>(__FILE__, __LINE__, _Message, ##__VA_ARGS__)
#endif

#define __stdx_assert(_Expr, _Exception)                 if (!(_Expr)) __stdx_except(_Exception, "assertion: '%s' failed", ##_Expr)
#define __stdx_assertx(_Expr, _Exception, _Message, ...) if (!(_Expr)) __stdx_except(_Exception, "assertion: '%s' failed with message: " _Message, #_Expr, ##__VA_ARGS__)
#define __stdx_trace(_Fmt, ...) ::stdx::detail::print_message(_STDX_EDEBUG, __FILE__, __LINE__, _Fmt, ##__VA_ARGS__)
#define __stdx_warn(_Fmt, ...)  ::stdx::detail::print_message(_STDX_EWARN,  __FILE__, __LINE__, _Fmt, ##__VA_ARGS__)
#define __stdx_fatal(_Fmt, ...) ::stdx::detail::print_message(_STDX_EFATAL, __FILE__, __LINE__, _Fmt, ##__VA_ARGS__)


_STDX_BEGIN

namespace detail 
{
#ifdef STDX_CMPLR_MSVC
#pragma warning(push)
#pragma warning(disable:4996)
#define snprintf _snprintf
#endif
	inline void format_message(int mode, const char* file, size_t line, char* buffer, size_t bufsize, const char* format, va_list ap)
	{
		size_t nbytes = 0;

		switch (mode)
		{
		case _STDX_EFATAL:
			nbytes = snprintf(buffer, bufsize, "%s(%zd) fatal:   ", file, line); break;
		case _STDX_EWARN:
			nbytes = snprintf(buffer, bufsize, "%s(%zd) warning: ", file, line); break;
		default:
			nbytes = snprintf(buffer, bufsize, "%s(%zd) debug:   ", file, line);
		}

		if (format)
			nbytes += vsnprintf(buffer + nbytes, bufsize - nbytes, format, ap);

		if (nbytes < bufsize-1) {
			*(buffer + nbytes) = '\n';
			*(buffer + nbytes + 1) = '\0';
		}
	}
#ifdef STDX_CMPLR_MSVC
#pragma warning(pop)
#undef snprintf
#endif


	inline void print_message(int mode, const char* file, size_t line, const char* format, ...)
	{
		char buffer[_STDX_ERROR_MESSAGE_MAX_BUFFER_SIZE];

		va_list ap;
		va_start(ap, format);
		format_message(mode, file, line, buffer, _STDX_ERROR_MESSAGE_MAX_BUFFER_SIZE, format, ap);
		va_end(ap);

		if (mode == _STDX_EFATAL) {
#if defined(STDX_CMPLR_MSVC) && defined(_STDX_DEBUG)
			int reportMode = _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
			_CrtSetReportMode(_CRT_ERROR, reportMode);

			int ret = _CrtDbgReport(_CRT_ERROR, file, (int)line, file, buffer);
			if ((ret == 0) && (reportMode & _CRTDBG_MODE_WNDW))
				return; // ignore
			else if (ret == 1)
				_CrtDbgBreak();
#else
			fputs(buffer, stderr);
            abort();
#endif
		}
#if defined(STDX_CMPLR_MSVC) && defined(_STDX_DEBUG)
		_CrtDbgReport(_CRT_WARN, NULL, 0, file, buffer);
#else
		fputs(buffer, stderr);
#endif
	}

	template<typename _Exception>
	inline void raise_exception(const char* file, size_t line, const char* format, ...)
	{
		char buffer[_STDX_ERROR_MESSAGE_MAX_BUFFER_SIZE];

		va_list ap;
		va_start(ap, format);
		format_message(_STDX_EFATAL, file, line, buffer, _STDX_ERROR_MESSAGE_MAX_BUFFER_SIZE, format, ap);
		va_end(ap);

		throw _Exception(buffer);
	}

} // end namespace detail

_STDX_END

#else

#define __stdx_except(_Exception, _Message, ...)         ((void)0)
#define __stdx_assert(_Expr, _Exception)                 ((void)0)
#define __stdx_assertx(_Expr, _Exception, _Message, ...) ((void)0)
#define __stdx_trace(_Fmt, ...)                          ((void)0)
#define __stdx_warn(_Fmt, ...)                           ((void)0)
#define __stdx_fatal(_Fmt, ...)                          ((void)0)

#endif

