dnl
dnl  This file is part of Bakefile (http://bakefile.sourceforge.net)
dnl
dnl  Copyright (C) 2003-2007 Vaclav Slavik, David Elliott and others
dnl
dnl  Permission is hereby granted, free of charge, to any person obtaining a
dnl  copy of this software and associated documentation files (the "Software"),
dnl  to deal in the Software without restriction, including without limitation
dnl  the rights to use, copy, modify, merge, publish, distribute, sublicense,
dnl  and/or sell copies of the Software, and to permit persons to whom the
dnl  Software is furnished to do so, subject to the following conditions:
dnl
dnl  The above copyright notice and this permission notice shall be included in
dnl  all copies or substantial portions of the Software.
dnl
dnl  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
dnl  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
dnl  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
dnl  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
dnl  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
dnl  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
dnl  DEALINGS IN THE SOFTWARE.
dnl
dnl  $Id$
dnl
dnl  Compiler detection macros by David Elliott
dnl


dnl ===========================================================================
dnl Macros to detect non-GNU compilers (MetroWerks, XLC)
dnl ===========================================================================

dnl Based on autoconf _AC_LANG_COMPILER_GNU
AC_DEFUN([_AC_BAKEFILE_LANG_COMPILER_MWERKS],
[AC_CACHE_CHECK([whether we are using the Metrowerks _AC_LANG compiler],
    [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_mwerks],
    [AC_TRY_COMPILE([],[#ifndef __MWERKS__
       choke me
#endif
],
        [bakefile_compiler_mwerks=yes],
        [bakefile_compiler_mwerks=no])
    bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_mwerks=$bakefile_compiler_mwerks
    ])
])

dnl Loosely based on autoconf AC_PROG_CC
dnl TODO: Maybe this should wrap the call to AC_PROG_CC and be used instead.
AC_DEFUN([AC_BAKEFILE_PROG_MWCC],
[AC_LANG_PUSH(C)
_AC_BAKEFILE_LANG_COMPILER_MWERKS
MWCC=`test $bakefile_cv_c_compiler_mwerks = yes && echo yes`
AC_LANG_POP(C)
])

dnl Loosely based on autoconf AC_PROG_CXX
dnl TODO: Maybe this should wrap the call to AC_PROG_CXX and be used instead.
AC_DEFUN([AC_BAKEFILE_PROG_MWCXX],
[AC_LANG_PUSH(C++)
_AC_BAKEFILE_LANG_COMPILER_MWERKS
MWCXX=`test $bakefile_cv_cxx_compiler_mwerks = yes && echo yes`
AC_LANG_POP(C++)
])

dnl Based on autoconf _AC_LANG_COMPILER_GNU
AC_DEFUN([_AC_BAKEFILE_LANG_COMPILER_XLC],
[AC_CACHE_CHECK([whether we are using the IBM xlC _AC_LANG compiler],
    [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_xlc],
    [AC_TRY_COMPILE([],[#ifndef __xlC__
       choke me
#endif
],
        [bakefile_compiler_xlc=yes],
        [bakefile_compiler_xlc=no])
    bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_xlc=$bakefile_compiler_xlc
    ])
])

dnl Loosely based on autoconf AC_PROG_CC
AC_DEFUN([AC_BAKEFILE_PROG_XLCC],
[AC_LANG_PUSH(C)
_AC_BAKEFILE_LANG_COMPILER_XLC
XLCC=`test $bakefile_cv_c_compiler_xlc = yes && echo yes`
AC_LANG_POP(C)
])

dnl Loosely based on autoconf AC_PROG_CXX
AC_DEFUN([AC_BAKEFILE_PROG_XLCXX],
[AC_LANG_PUSH(C++)
_AC_BAKEFILE_LANG_COMPILER_XLC
XLCXX=`test $bakefile_cv_cxx_compiler_xlc = yes && echo yes`
AC_LANG_POP(C++)
])

dnl Based on autoconf _AC_LANG_COMPILER_GNU
dnl _AC_BAKEFILE_LANG_COMPILER(NAME, LANG, SYMBOL, IF-YES, IF-NO)
AC_DEFUN([_AC_BAKEFILE_LANG_COMPILER],
[
    AC_LANG_PUSH($2)
    AC_CACHE_CHECK(
        [whether we are using the $1 $2 compiler],
        [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3],
        [AC_TRY_COMPILE(
            [],
            [
             #ifndef $3
                choke me
             #endif
            ],
            [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3=yes],
            [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3=no]
         )
        ]
    )
    AC_LANG_POP($2)
    if test "x$bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3" = "xyes"; then
        :; $4
    else
        :; $5
    fi
])

dnl recent versions of SGI mipsPro compiler define _SGI_COMPILER_VERSION
dnl
dnl NB: old versions define _COMPILER_VERSION but this could probably be
dnl     defined by other compilers too so don't test for it to be safe
AC_DEFUN([AC_BAKEFILE_PROG_SGICC],
[
    _AC_BAKEFILE_LANG_COMPILER(SGI, C, _SGI_COMPILER_VERSION, SGICC=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_SGICXX],
[
    _AC_BAKEFILE_LANG_COMPILER(SGI, C++, _SGI_COMPILER_VERSION, SGICXX=yes)
])

dnl Sun compiler defines __SUNPRO_C/__SUNPRO_CC
AC_DEFUN([AC_BAKEFILE_PROG_SUNCC],
[
    _AC_BAKEFILE_LANG_COMPILER(Sun, C, __SUNPRO_C, SUNCC=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_SUNCXX],
[
    _AC_BAKEFILE_LANG_COMPILER(Sun, C++, __SUNPRO_CC, SUNCXX=yes)
])

dnl Intel icc compiler defines __INTEL_COMPILER for both C and C++
AC_DEFUN([AC_BAKEFILE_PROG_INTELCC],
[
    _AC_BAKEFILE_LANG_COMPILER(Intel, C, __INTEL_COMPILER, INTELCC=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_INTELCXX],
[
    _AC_BAKEFILE_LANG_COMPILER(Intel, C++, __INTEL_COMPILER, INTELCXX=yes)
])

dnl HP-UX aCC: see http://docs.hp.com/en/6162/preprocess.htm#macropredef
AC_DEFUN([AC_BAKEFILE_PROG_HPCC],
[
    _AC_BAKEFILE_LANG_COMPILER(HP, C, __HP_cc, HPCC=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_HPCXX],
[
    _AC_BAKEFILE_LANG_COMPILER(HP, C++, __HP_aCC, HPCXX=yes)
])

dnl Tru64 cc and cxx
AC_DEFUN([AC_BAKEFILE_PROG_COMPAQCC],
[
    _AC_BAKEFILE_LANG_COMPILER(Compaq, C, __DECC, COMPAQCC=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_COMPAQCXX],
[
    _AC_BAKEFILE_LANG_COMPILER(Compaq, C++, __DECCXX, COMPAQCXX=yes)
])

dnl ===========================================================================
dnl macros to detect specialty compiler options
dnl ===========================================================================

dnl Figure out if we need to pass -ext o to compiler (MetroWerks)
AC_DEFUN([AC_BAKEFILE_METROWERKS_EXTO],
[AC_CACHE_CHECK([if the _AC_LANG compiler requires -ext o], bakefile_cv_[]_AC_LANG_ABBREV[]_exto,
dnl First create an empty conf test
[AC_LANG_CONFTEST([AC_LANG_PROGRAM()])
dnl Now remove .o and .c.o or .cc.o
rm -f conftest.$ac_objext conftest.$ac_ext.o
dnl Now compile the test
AS_IF([AC_TRY_EVAL(ac_compile)],
dnl If the test succeeded look for conftest.c.o or conftest.cc.o
[for ac_file in `(ls conftest.* 2>/dev/null)`; do
    case $ac_file in
        conftest.$ac_ext.o)
            bakefile_cv_[]_AC_LANG_ABBREV[]_exto="-ext o"
            ;;
        *)
            ;;
    esac
done],
[AC_MSG_FAILURE([cannot figure out if compiler needs -ext o: cannot compile])
]) dnl AS_IF

rm -f conftest.$ac_ext.o conftest.$ac_objext conftest.$ac_ext
]) dnl AC_CACHE_CHECK

if test "x$bakefile_cv_[]_AC_LANG_ABBREV[]_exto" '!=' "x"; then
    if test "[]_AC_LANG_ABBREV[]" = "c"; then
        CFLAGS="$bakefile_cv_[]_AC_LANG_ABBREV[]_exto $CFLAGS"
    fi
    if test "[]_AC_LANG_ABBREV[]" = "cxx"; then
        CXXFLAGS="$bakefile_cv_[]_AC_LANG_ABBREV[]_exto $CXXFLAGS"
    fi
fi
]) dnl AC_DEFUN


dnl ===========================================================================
dnl Macros to do all of the compiler detections as one macro
dnl ===========================================================================

dnl check for different proprietary compilers depending on target platform
dnl _AC_BAKEFILE_PROG_COMPILER(LANG)
AC_DEFUN([_AC_BAKEFILE_PROG_COMPILER],
[
    AC_PROG_$1
    AC_BAKEFILE_PROG_INTEL$1
    dnl if we're using gcc, we can't be using any of incompatible compilers
    if test "x$G$1" != "xyes"; then
        if test "x$1" = "xC"; then
            AC_BAKEFILE_METROWERKS_EXTO
            if test "x$bakefile_cv_c_exto" '!=' "x"; then
                unset ac_cv_prog_cc_g
                _AC_PROG_CC_G
            fi
        fi

        dnl most of these compilers are only used under well-defined OS so
        dnl don't waste time checking for them on other ones
        case `uname -s` in
            AIX*)
                AC_BAKEFILE_PROG_XL$1
                ;;

            Darwin)
                AC_BAKEFILE_PROG_MW$1
                AC_BAKEFILE_PROG_XL$1
                ;;

            IRIX*)
                AC_BAKEFILE_PROG_SGI$1
                ;;

            HP-UX*)
                AC_BAKEFILE_PROG_HP$1
                ;;

            OSF1)
                AC_BAKEFILE_PROG_COMPAQ$1
                ;;

            SunOS)
                AC_BAKEFILE_PROG_SUN$1
                ;;
        esac
    fi
])

AC_DEFUN([AC_BAKEFILE_PROG_CC],
[
    _AC_BAKEFILE_PROG_COMPILER(CC)
])

AC_DEFUN([AC_BAKEFILE_PROG_CXX],
[
    _AC_BAKEFILE_PROG_COMPILER(CXX)
])

