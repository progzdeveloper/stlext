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

#ifndef __cplusplus
#error "Unable to compile with non-C++ compiler"
#endif

// Symantec / Digital Mars C++ Compilers
#if defined(__DMC__) || defined(__SC__) 
#  define STDX_CMPLR_SYMANTEC

// Microsoft Visual C++ Compiler
#elif defined(_MSC_VER) 
#define STDX_CMPLR_MSVC _MSC_VER 
#  if _MSC_VER < 1700
#    error "Compiler doesn't have C++11 support"
#  endif

// Intel C++ Compiler
#  if defined(__INTEL_COMPILER) 
#    define STDX_CMPLR_INTEL __INTEL_COMPILER
#  endif

// Borlad C++ Compiler
#elif defined(__BORLANDC__) || defined(__TURBOC__) 
#  define STDX_CMPLR_BORLAND __BORLANDC__
#  if __BORLANDC__ < 0x502
#    error "Compiler doesn't have C++11 support"
#  endif

// Watcom C++ Compiler
#elif defined(__WATCOMC__) 
#  define STDX_CMPLR_WATCOM

/* ARM Realview Compiler Suite
RVCT Compiler also defines __EDG__ and __GNUC__ (if --gnu flag is given),
so check for it before that */
#elif defined(__ARMCC__) || defined(__CC_ARM)
#define STDX_CMPLR_RVCT

// GNU C++ Compilers
#elif defined(__GNUC__)
#  define STDX_CMPLR_GNU          (__GNUC__ * 100 + __GNUC_MINOR__)
#  if defined(__MINGW32__)
#    define STDX_CMPLR_MINGW
#  endif
#  if defined(__INTEL_COMPILER)
/* Intel C++ also masquerades as GCC */
#    define STDX_CMPLR_INTEL      (__INTEL_COMPILER)
#    ifdef __clang__
/* Intel C++ masquerades as Clang masquerading as GCC */
#      define STDX_CMPLR_CLANG    305
#    endif
#  elif defined(__clang__)
/* Clang also masquerades as GCC */
#    if defined(__apple_build_version__)
#      /* http://en.wikipedia.org/wiki/Xcode#Toolchain_Versions */
#      if __apple_build_version__ >= 6000051
#        define STDX_CMPLR_CLANG 305
#      elif __apple_build_version__ >= 5030038
#        define STDX_CMPLR_CLANG 304
#      elif __apple_build_version__ >= 5000275
#        define STDX_CMPLR_CLANG 303
#      elif __apple_build_version__ >= 4250024
#        define STDX_CMPLR_CLANG 302
#      elif __apple_build_version__ >= 3180045
#        define STDX_CMPLR_CLANG 301
#      elif __apple_build_version__ >= 2111001
#        define STDX_CMPLR_CLANG 300
#      else
#        error "Unknown Apple Clang version"
#      endif
#    else
#      define STDX_CMPLR_CLANG ((__clang_major__ * 100) + __clang_minor__)
#    endif
#  endif

/* IBM Compiler versions are a bit messy. There are actually two products:
the C product, and the C++ product. The C++ Compiler is always packaged
with the latest version of the C Compiler. Version numbers do not always
match. This little table (I'm not sure it's accurate) should be helpful:

C++ product                C product

C Set 3.1                  C Compiler 3.0
...                        ...
C++ Compiler 3.6.6         C Compiler 4.3
...                        ...
Visual Age C++ 4.0         ...
...                        ...
Visual Age C++ 5.0         C Compiler 5.0
...                        ...
Visual Age C++ 6.0         C Compiler 6.0

Now:
__xlC__    is the version of the C Compiler in hexadecimal notation
is only an approximation of the C++ Compiler version
__IBMCPP__ is the version of the C++ Compiler in decimal notation
but it is not defined on older Compiler like C Set 3.1 */
#elif defined(__xlC__)
#  define STDX_CMPLR_XLC
#  if __xlC__ < 0x400
#    error "Compiler not supported"
#  endif

/* The Portland Group C++ Compilers is based on EDG and does define __EDG__
but the C Compiler does not */
#elif defined(__PGI)
#  define STDX_CMPLR_PGI
#  if defined(__EDG__)
#    define STDX_CMPLR_EDG
#  endif

/* Compilers with EDG front end are similar. To detect them we test:
__EDG documented by SGI, observed on MIPSpro 7.3.1.1 and KAI C++ 4.0b
__EDG__ documented in EDG online docs, observed on Compaq C++ V6.3-002
and PGI C++ 5.2-4 */
#elif (defined(__EDG) || defined(__EDG__))
#  define STDX_CMPLR_EDG
/* From the EDG documentation (does not seem to apply to Compaq C++):
_BOOL
Defined in C++ mode when bool is a keyword. The name of this
predefined macro is specified by a configuration flag. _BOOL
is the default.
__BOOL_DEFINED
Defined in Microsoft C++ mode when bool is a keyword. */
#  if !defined(_BOOL) && !defined(__BOOL_DEFINED)
#    error "Compiler not supported"
#  endif

/* The Comeau Compiler is based on EDG and does define __EDG__ */
#  if defined(__COMO__)
#    define STDX_CMPLR_COMEAU

/* The `using' keyword was introduced to avoid KAI C++ warnings
but it's now causing KAI C++ errors instead. The standard is
unclear about the use of this keyword, and in practice every
Compiler is using its own set of rules. Forget it. */
#  elif defined(__KCC)
#    define STDX_CMPLR_KAI

/* Using the `using' keyword avoids Intel C++ for Linux warnings */
#  elif defined(__INTEL_COMPILER)
#    define STDX_CMPLR_INTEL      (__INTEL_COMPILER)

/* Uses CFront, make sure to read the manual how to tweak templates. */
#  elif defined(__ghs)
#    define STDX_CMPLR_GHS

#  elif defined(__DCC__)
#    define STDX_CMPLR_DIAB
#    if !defined(__bool)
#      error "Compiler not supported"
#    endif

/* The UnixWare 7 UDK Compiler is based on EDG and does define __EDG__ */
#  elif defined(__USLC__) && defined(__SCO_VERSION__)
#    define STDX_CMPLR_USLC

/* Never tested! */
#  elif defined(CENTERLINE_CLPP) || defined(OBJECTCENTER)
#    define STDX_CMPLR_OC

/* CDS++ defines __EDG__ although this is not documented in the Reliant
documentation. It also follows conventions like _BOOL and this documented */
#  elif defined(sinix)
#    define STDX_CMPLR_CDS

/* The MIPSpro Compiler defines __EDG */
#  elif defined(__sgi)
#    define STDX_CMPLR_MIPS _COMPILER_VERSION
#    if defined(_COMPILER_VERSION) && (_COMPILER_VERSION >= 740)
#      pragma set woff 3624,3625,3649 /* turn off some harmless warnings */
#    endif
#  endif

/* VxWorks' DIAB toolchain has an additional EDG type C++ Compiler
(see __DCC__ above). This one is for C mode files (__EDG is not defined) */
#elif defined(_DIAB_TOOL)
#  define STDX_CMPLR_DIAB

/* Never tested! */
#elif defined(__HIGHC__)
#  define STDX_CMPLR_HIGHC

#elif defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#  define STDX_CMPLR_SUN
/* 5.0 Compiler or better
'bool' is enabled by default but can be disabled using -features=nobool
in which case _BOOL is not defined
this is the default in 4.2 compatibility mode triggered by -compat=4 */
#  if __SUNPRO_CC >= 0x500
#    if !defined(_BOOL)
#      error "Compiler not supported"
#    endif
/* 4.2 Compiler or older */
#  else
#    error "Compiler not supported"
#  endif

/* CDS++ does not seem to define __EDG__ or __EDG according to Reliant
documentation but nevertheless uses EDG conventions like _BOOL */
#elif defined(sinix)
#  define STDX_CMPLR_EDG
#  define STDX_CMPLR_CDS
#  if !defined(_BOOL)
#    error "Compiler not supported"
#  endif
// Unknown Compiler
#else
#  error "STDX has not been tested with this compiler..."
#endif



#if defined(STDX_CMPLR_MSVC) // Microsift Visual C++
#define __NO_VTABLE     __declspec(novtable)
#define __VECTORCALL    __vectorcall
#define __FASTCALL      __fastcall
#define __STDCALL       __stdcall
#define __CDECLCALL     __cdecl

#define __INLINE        __inline
#define __FORCE_INLINE  __forceinline
#define __STATIC_INLINE static inline
#define __RESTRICT      __restrict
#if STDX_CMPLR_MSVC <= 1800
#define __ALIGNAS(_Size)  __declspec(align(_Size))
#define __CONSTEXPR       const
#define __THREADLOCAL     
#else
#define __ALIGNAS(_Size)  alignas(_Size)
#define __CONSTEXPR       constexpr
#define __THREADLOCAL     thread_local
#endif

#elif defined(STDX_CMPLR_GNU) || defined(STDX_CMPLR_MINGW) // GNU Compiler Collection C++
#define __NO_VTABLE     
#define __VECTORCALL    __attribute__((vectorcall))
#define __FASTCALL      __attribute__((fastcall))
#define __STDCALL       __attribute__((stdcall))
#define __CDECLCALL     __attribute__((cdecl))

#define __INLINE          inline
#define __FORCE_INLINE    __inline__ __attribute__((always_inline))
#define __STATIC_INLINE   static inline
#define __RESTRICT        __restrict__
#define __CONSTEXPR       constexpr
//#define __ALIGNAS(_Size)  alignas(_Size)
#define __ALIGNAS(_Size) __attribute__((aligned(_Size)))
#define __THREADLOCAL     thread_local

#else // Unknown Compiler
#define __NO_VTABLE     
#define __VECTORCALL    
#define __FASTCALL      
#define __STDCALL       
#define __CDECLCALL     

#define __INLINE        inline
#define __FORCE_INLINE  inline
#define __STATIC_INLINE static inline
#define __RESTRICT      restrict
#define __CONSTEXPR     constexpr
#define __ALIGNAS(_Size)
#endif

// SSE/AVX Alignment
#ifndef _XMM_ALIGN
#define _XMM_ALIGN 16
#endif




