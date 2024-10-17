dnl
dnl  This file is part of Bakefile (http://www.bakefile.org)
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
dnl  Compiler detection macros by David Elliott and Vadim Zeitlin
dnl


dnl ===========================================================================
dnl Macros to detect different C/C++ compilers
dnl ===========================================================================

dnl Based on autoconf _AC_LANG_COMPILER_GNU
dnl _AC_BAKEFILE_LANG_COMPILER(NAME, LANG, SYMBOL, IF-YES, IF-NO)
AC_DEFUN([_AC_BAKEFILE_LANG_COMPILER],
[
    AC_LANG_PUSH($2)
    AC_CACHE_CHECK(
        [whether we are using the $1 $2 compiler],
        [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3],
        [AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
            [],
            [
             #ifndef $3
                choke me
             #endif
            ])],
            [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3=yes],
            [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3=no]
         )
        ]
    )
    if test "x$bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3" = "xyes"; then
        :; $4
    else
        :; $5
    fi
    AC_LANG_POP($2)
])

dnl More specific version of the above macro checking whether the compiler
dnl version is at least the given one (assumes that we do use this compiler)
dnl
dnl _AC_BAKEFILE_LANG_COMPILER_LATER_THAN(NAME, LANG, SYMBOL, VER, VERMSG, IF-YES, IF-NO)
AC_DEFUN([_AC_BAKEFILE_LANG_COMPILER_LATER_THAN],
[
    AC_LANG_PUSH($2)
    AC_CACHE_CHECK(
        [whether we are using $1 $2 compiler v$5 or later],
        [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3[]_lt_[]$4],
        [AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
            [],
            [
             #ifndef $3 || $3 < $4
                choke me
             #endif
            ])],
            [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3[]_lt_[]$4=yes],
            [bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3[]_lt_[]$4=no]
         )
        ]
    )
    if test "x$bakefile_cv_[]_AC_LANG_ABBREV[]_compiler_[]$3[]_lt_[]$4" = "xyes"; then
        :; $6
    else
        :; $7
    fi
    AC_LANG_POP($2)
])

dnl IBM xlC compiler defines __xlC__ for both C and C++
AC_DEFUN([AC_BAKEFILE_PROG_XLCC],
[
    _AC_BAKEFILE_LANG_COMPILER([IBM xlC], C, __xlC__, XLCC=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_XLCXX],
[
    _AC_BAKEFILE_LANG_COMPILER([IBM xlC], C++, __xlC__, XLCXX=yes)
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

dnl Intel compiler command line options changed in incompatible ways sometimes
dnl before v8 (-KPIC was replaced with gcc-compatible -fPIC) and again in v10
dnl (-create-pch deprecated in favour of -pch-create) so we need to test for
dnl its exact version too
AC_DEFUN([AC_BAKEFILE_PROG_INTELCC_8],
[
    _AC_BAKEFILE_LANG_COMPILER_LATER_THAN(Intel, C, __INTEL_COMPILER, 800, 8, INTELCC8=yes)
])
AC_DEFUN([AC_BAKEFILE_PROG_INTELCXX_8],
[
    _AC_BAKEFILE_LANG_COMPILER_LATER_THAN(Intel, C++, __INTEL_COMPILER, 800, 8, INTELCXX8=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_INTELCC_10],
[
    _AC_BAKEFILE_LANG_COMPILER_LATER_THAN(Intel, C, __INTEL_COMPILER, 1000, 10, INTELCC10=yes)
])

AC_DEFUN([AC_BAKEFILE_PROG_INTELCXX_10],
[
    _AC_BAKEFILE_LANG_COMPILER_LATER_THAN(Intel, C++, __INTEL_COMPILER, 1000, 10, INTELCXX10=yes)
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
dnl Macros to do all of the compiler detections as one macro
dnl ===========================================================================

dnl check for different proprietary compilers depending on target platform
dnl _AC_BAKEFILE_PROG_COMPILER(LANG)
AC_DEFUN([_AC_BAKEFILE_PROG_COMPILER],
[
    AC_REQUIRE([AC_PROG_$1])

    dnl Intel compiler can be used under several different OS and even
    dnl different architectures (x86, amd64 and Itanium) so it's easier to just
    dnl always test for it
    AC_BAKEFILE_PROG_INTEL$1

    dnl If we use Intel compiler we also need to know its version
    if test "$INTEL$1" = "yes"; then
        AC_BAKEFILE_PROG_INTEL$1_8
        AC_BAKEFILE_PROG_INTEL$1_10
    fi

    dnl if we're using gcc, we can't be using any of incompatible compilers
    if test "x$G$1" != "xyes"; then
        dnl most of these compilers are only used under well-defined OS so
        dnl don't waste time checking for them on other ones
        case `uname -s` in
            AIX*)
                AC_BAKEFILE_PROG_XL$1
                ;;

            Darwin)
                AC_BAKEFILE_PROG_XL$1
                ;;

            IRIX*)
                AC_BAKEFILE_PROG_SGI$1
                ;;

            Linux*)
                dnl Sun CC is now available under Linux too, test for it unless
                dnl we already found that we were using a different compiler
                if test "$INTEL$1" != "yes"; then
                    AC_BAKEFILE_PROG_SUN$1
                fi
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

