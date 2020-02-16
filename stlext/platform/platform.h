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
#include <climits>

// Check the platform bits in a byte
#if (CHAR_BIT != 8) 
#pragma message("Warning: Current platforn is NOT 8-bit platform")
#endif


/*
The operating system, must be one of: (STDX_OS_x)

DARWIN   - Any Darwin system
MAC      - OS X and iOS
OSX      - OS X
IOS      - iOS
MSDOS    - MS-DOS and Windows
OS2      - OS/2
OS2EMX   - XFree86 on OS/2 (not PM)
WIN32    - Win32 (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
WINCE    - WinCE (Windows CE 5.0)
WINRT    - WinRT (Windows 8 Runtime)
CYGWIN   - Cygwin
SOLARIS  - Sun Solaris
HPUX     - HP-UX
ULTRIX   - DEC Ultrix
LINUX    - Linux [has variants]
FREEBSD  - FreeBSD [has variants]
NETBSD   - NetBSD
OPENBSD  - OpenBSD
BSDI     - BSD/OS
IRIX     - SGI Irix
OSF      - HP Tru64 UNIX
SCO      - SCO OpenServer 5
UNIXWARE - UnixWare 7, Open UNIX 8
AIX      - AIX
HURD     - GNU Hurd
DGUX     - DG/UX
RELIANT  - Reliant UNIX
DYNIX    - DYNIX/ptx
QNX      - QNX [has variants]
QNX6     - QNX RTP 6.1
LYNX     - LynxOS
BSD4     - Any BSD 4.4 system
UNIX     - Any UNIX BSD/SYSV system
ANDROID  - Android platform

The following operating systems have variants:
LINUX    - both STDX_OS_LINUX and STDX_OS_ANDROID are defined when building for Android
- only STDX_OS_LINUX is defined if building for other Linux systems
QNX      - both STDX_OS_QNX and STDX_OS_BLACKBERRY are defined when building for Blackberry 10
- only STDX_OS_QNX is defined if building for other QNX targets
FREEBSD  - STDX_OS_FREEBSD is defined only when building for FreeBSD with a BSD userland
- STDX_OS_FREEBSD_KERNEL is always defined on FreeBSD, even if the userland is from GNU
*/

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define STDX_OS_DARWIN
#  define STDX_OS_BSD4
#  ifdef __LP64__
#    define STDX_OS_DARWIN64
#  else
#    define STDX_OS_DARWIN32
#  endif
#elif defined(ANDROID)
#  define STDX_OS_ANDROID
#  define STDX_OS_LINUX
#elif defined(__CYGWIN__)
#  define STDX_OS_CYGWIN
#elif !defined(SAG_COM) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define STDX_OS_WIN32
#  define STDX_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINCE) || defined(_WIN32_WCE)
#    define STDX_OS_WINCE
#  elif defined(WINAPI_FAMILY)
#    if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
#      define STDX_OS_WINPHONE
#      define STDX_OS_WINRT
#    elif WINAPI_FAMILY==WINAPI_FAMILY_APP
#      define STDX_OS_WINRT
#    else
#      define STDX_OS_WIN32
#    endif
#  else
#    define STDX_OS_WIN32
#  endif
#elif defined(__sun) || defined(sun)
#  define STDX_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define STDX_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define STDX_OS_ULTRIX
#elif defined(sinix)
#  define STDX_OS_RELIANT
#elif defined(__native_client__)
#  define STDX_OS_NACL
#elif defined(__linux__) || defined(__linux)
#  define STDX_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#  ifndef __FreeBSD_kernel__
#    define STDX_OS_FREEBSD
#  endif
#  define STDX_OS_FREEBSD_KERNEL
#  define STDX_OS_BSD4
#elif defined(__NetBSD__)
#  define STDX_OS_NETBSD
#  define STDX_OS_BSD4
#elif defined(__OpenBSD__)
#  define STDX_OS_OPENBSD
#  define STDX_OS_BSD4
#elif defined(__bsdi__)
#  define STDX_OS_BSDI
#  define STDX_OS_BSD4
#elif defined(__sgi)
#  define STDX_OS_IRIX
#elif defined(__osf__)
#  define STDX_OS_OSF
#elif defined(_AIX)
#  define STDX_OS_AIX
#elif defined(__Lynx__)
#  define STDX_OS_LYNX
#elif defined(__GNU__)
#  define STDX_OS_HURD
#elif defined(__DGUX__)
#  define STDX_OS_DGUX
#elif defined(__QNXNTO__)
#  define STDX_OS_QNX
#elif defined(_SEQUENT_)
#  define STDX_OS_DYNIX
#elif defined(_SCO_DS) /* SCO OpenServer 5 + GCC */
#  define STDX_OS_SCO
#elif defined(__USLC__) /* all SCO platforms + UDK or OUDK */
#  define STDX_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define STDX_OS_UNIXWARE
#elif defined(__INTEGRITY)
#  define STDX_OS_INTEGRITY
#elif defined(VXWORKS) /* there is no "real" VxWorks define - this has to be set in the mkspec! */
#  define STDX_OS_VXWORKS
#elif defined(__MAKEDEPEND__)
#else
#  error "stlext has not been ported to this OS"
#endif

#if defined(STDX_OS_WIN32) || defined(STDX_OS_WIN64) || defined(STDX_OS_WINCE) || defined(STDX_OS_WINRT)
#  define STDX_OS_WIN
#endif

#if defined(STDX_OS_DARWIN)
#  define STDX_OS_MAC
#  if defined(STDX_OS_DARWIN64)
#     define STDX_OS_MAC64
#  elif defined(STDX_OS_DARWIN32)
#     define STDX_OS_MAC32
#  endif
#  include <TargetConditionals.h>
#  if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#     define STDX_OS_IOS
#  elif defined(TARGET_OS_MAC) && TARGET_OS_MAC
#     define STDX_OS_OSX
#     define STDX_OS_MACX // compatibility synonym
#  endif
#endif

#if defined(STDX_OS_WIN)
#  undef STDX_OS_UNIX
#elif !defined(STDX_OS_UNIX)
#  define STDX_OS_UNIX
#endif

#ifdef STDX_OS_DARWIN
#  include <Availability.h>
#  include <AvailabilityMacros.h>
#
#  ifdef STDX_OS_OSX
#    if !defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED < __MAC_10_6
#       undef __MAC_OS_X_VERSION_MIN_REQUIRED
#       define __MAC_OS_X_VERSION_MIN_REQUIRED __MAC_10_6
#    endif
#    if !defined(MAC_OS_X_VERSION_MIN_REQUIRED) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_6
#       undef MAC_OS_X_VERSION_MIN_REQUIRED
#       define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_6
#    endif
#  endif
#
#  // Numerical checks are preferred to named checks, but to be safe
#  // we define the missing version names in case stlext uses them.
#
#  if !defined(__MAC_10_7)
#       define __MAC_10_7 1070
#  endif
#  if !defined(__MAC_10_8)
#       define __MAC_10_8 1080
#  endif
#  if !defined(__MAC_10_9)
#       define __MAC_10_9 1090
#  endif
#  if !defined(__MAC_10_10)
#       define __MAC_10_10 101000
#  endif
#  if !defined(MAC_OS_X_VERSION_10_7)
#       define MAC_OS_X_VERSION_10_7 1070
#  endif
#  if !defined(MAC_OS_X_VERSION_10_8)
#       define MAC_OS_X_VERSION_10_8 1080
#  endif
#  if !defined(MAC_OS_X_VERSION_10_9)
#       define MAC_OS_X_VERSION_10_9 1090
#  endif
#  if !defined(MAC_OS_X_VERSION_10_10)
#       define MAC_OS_X_VERSION_10_10 101000
#  endif
#
#  if !defined(__IPHONE_4_3)
#       define __IPHONE_4_3 40300
#  endif
#  if !defined(__IPHONE_5_0)
#       define __IPHONE_5_0 50000
#  endif
#  if !defined(__IPHONE_5_1)
#       define __IPHONE_5_1 50100
#  endif
#  if !defined(__IPHONE_6_0)
#       define __IPHONE_6_0 60000
#  endif
#  if !defined(__IPHONE_6_1)
#       define __IPHONE_6_1 60100
#  endif
#  if !defined(__IPHONE_7_0)
#       define __IPHONE_7_0 70000
#  endif
#  if !defined(__IPHONE_7_1)
#       define __IPHONE_7_1 70100
#  endif
#  if !defined(__IPHONE_8_0)
#       define __IPHONE_8_0 80000
#  endif
#endif

#ifdef __LSB_VERSION__
#  if __LSB_VERSION__ < 40
#    error "This version of the Linux Standard Base is unsupported"
#  endif
#ifndef STDX_LINUXBASE
#  define STDX_LINUXBASE
#endif
#endif


