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
Alpha family, no revisions or variants

Alpha is bi-endian, use endianness auto-detection implemented below.
*/
// #elif defined(__alpha__) || defined(_M_ALPHA)
// #  define STDX_PROCESSOR_ALPHA

/*
ARM family, known revisions: V5, V6, V7, V8

ARM is bi-endian, detect using __ARMEL__ or __ARMEB__, falling back to
auto-detection implemented below.
*/
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM) || defined(__aarch64__)
#  define STDX_PROCESSOR_ARM
#  if defined(__aarch64__)
#    define STDX_PROCESSOR_ARM_64
#  else
#    define STDX_PROCESSOR_ARM_32
#  endif
#  if defined(__ARM64_ARCH_8__)
#    define STDX_PROCESSOR_ARM_V8
#    define STDX_PROCESSOR_ARM_V7
#    define STDX_PROCESSOR_ARM_V6
#    define STDX_PROCESSOR_ARM_V5
#  elif defined(__ARM_ARCH_7__) \
      || defined(__ARM_ARCH_7A__) \
      || defined(__ARM_ARCH_7R__) \
      || defined(__ARM_ARCH_7M__) \
      || defined(__ARM_ARCH_7S__) \
      || defined(_ARM_ARCH_7) \
      || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7) \
      || (defined(_M_ARM) && _M_ARM-0 >= 7)
#    define STDX_PROCESSOR_ARM_V7
#    define STDX_PROCESSOR_ARM_V6
#    define STDX_PROCESSOR_ARM_V5
#  elif defined(__ARM_ARCH_6__) \
      || defined(__ARM_ARCH_6J__) \
      || defined(__ARM_ARCH_6T2__) \
      || defined(__ARM_ARCH_6Z__) \
      || defined(__ARM_ARCH_6K__) \
      || defined(__ARM_ARCH_6ZK__) \
      || defined(__ARM_ARCH_6M__) \
      || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6) \
      || (defined(_M_ARM) && _M_ARM-0 >= 6)
#    define STDX_PROCESSOR_ARM_V6
#    define STDX_PROCESSOR_ARM_V5
#  elif defined(__ARM_ARCH_5TEJ__) \
        || defined(__ARM_ARCH_5TE__) \
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5) \
        || (defined(_M_ARM) && _M_ARM-0 >= 5)
#    define STDX_PROCESSOR_ARM_V5
#  endif


/*
AVR32 family, no revisions or variants

AVR32 is big-endian.
*/
// #elif defined(__avr32__)
// #  define STDX_PROCESSOR_AVR32


/*
Blackfin family, no revisions or variants

Blackfin is little-endian.
*/
// #elif defined(__bfin__)
// #  define STDX_PROCESSOR_BLACKFIN


/*
X86 family, known variants: 32- and 64-bit

X86 is little-endian.
*/
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
#  define STDX_PROCESSOR_X86_32
#  define STDX_PROCESSOR_WORDSIZE   4

/*
* We define STDX_PROCESSOR_X86 == 6 for anything above a equivalent or better
* than a Pentium Pro (the processor whose architecture was called P6) or an
* Athlon.
*
* All processors since the Pentium III and the Athlon 4 have SSE support, so
* we use that to detect. That leaves the original Athlon, Pentium Pro and
* Pentium II.
*/

#  if defined(_M_IX86)
#    define STDX_PROCESSOR_X86     (_M_IX86/100)
#  elif defined(__i686__) || defined(__athlon__) || defined(__SSE__)
#    define STDX_PROCESSOR_X86     6
#  elif defined(__i586__) || defined(__k6__)
#    define STDX_PROCESSOR_X86     5
#  elif defined(__i486__)
#    define STDX_PROCESSOR_X86     4
#  else
#    define STDX_PROCESSOR_X86     3
#  endif

#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
#  define STDX_PROCESSOR_X86       6
#  define STDX_PROCESSOR_X86_64    
#  define STDX_PROCESSOR_WORDSIZE   8

/*
Itanium (IA-64) family, no revisions or variants

Itanium is bi-endian, use endianness auto-detection implemented below.
*/
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#  define STDX_PROCESSOR_IA64
#  define STDX_PROCESSOR_WORDSIZE   8

/*
MIPS family, known revisions: I, II, III, IV, 32, 64

MIPS is bi-endian, use endianness auto-detection implemented below.
*/
#elif defined(__mips) || defined(__mips__) || defined(_M_MRX000)
#  define STDX_PROCESSOR_MIPS
#  if defined(_MIPS_ARCH_MIPS1) || (defined(__mips) && __mips - 0 >= 1)
#    define STDX_PROCESSOR_MIPS_I
#  endif
#  if defined(_MIPS_ARCH_MIPS2) || (defined(__mips) && __mips - 0 >= 2)
#    define STDX_PROCESSOR_MIPS_II
#  endif
#  if defined(_MIPS_ARCH_MIPS32) || defined(__mips32)
#    define STDX_PROCESSOR_MIPS_32
#  endif
#  if defined(_MIPS_ARCH_MIPS3) || (defined(__mips) && __mips - 0 >= 3)
#    define STDX_PROCESSOR_MIPS_III
#  endif
#  if defined(_MIPS_ARCH_MIPS4) || (defined(__mips) && __mips - 0 >= 4)
#    define STDX_PROCESSOR_MIPS_IV
#  endif
#  if defined(_MIPS_ARCH_MIPS5) || (defined(__mips) && __mips - 0 >= 5)
#    define STDX_PROCESSOR_MIPS_V
#  endif
#  if defined(_MIPS_ARCH_MIPS64) || defined(__mips64)
#    define STDX_PROCESSOR_MIPS_64
#  endif

/*
Power family, known variants: 32- and 64-bit

There are many more known variants/revisions that we do not handle/detect.
See http://en.wikipedia.org/wiki/Power_Architecture
and http://en.wikipedia.org/wiki/File:PowerISA-evolution.svg

Power is bi-endian, use endianness auto-detection implemented below.
*/
#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \
      || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC)  \
      || defined(_M_MPPC) || defined(_M_PPC)
#  define STDX_PROCESSOR_POWER
#  if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
#    define STDX_PROCESSOR_POWER_64
#  else
#    define STDX_PROCESSOR_POWER_32
#  endif

/*
S390 family, known variant: S390X (64-bit)

S390 is big-endian.
*/
#elif defined(__s390__)
#  define STDX_PROCESSOR_S390
#  if defined(__s390x__)
#    define STDX_PROCESSOR_S390_X
#  endif

/*
SuperH family, optional revision: SH-4A

SuperH is bi-endian, use endianness auto-detection implemented below.
*/
// #elif defined(__sh__)
// #  define STDX_PROCESSOR_SH
// #  if defined(__sh4a__)
// #    define STDX_PROCESSOR_SH_4A
// #  endif

/*
SPARC family, optional revision: V9

SPARC is big-endian only prior to V9, while V9 is bi-endian with big-endian
as the default byte order. Assume all SPARC systems are big-endian.
*/
#elif defined(__sparc__)
#  define STDX_PROCESSOR_SPARC
#  if defined(__sparc_v9__)
#    define STDX_PROCESSOR_SPARC_V9
#  endif
#  if defined(__sparc64__)
#    define STDX_PROCESSOR_SPARC_64
#  endif

#endif


