# This file is part of Autoconf.                       -*- Autoconf -*-
# Programming languages support.
# Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#
# As a special exception, the Free Software Foundation gives unlimited
# permission to copy, distribute and modify the configure scripts that
# are the output of Autoconf.  You need not follow the terms of the GNU
# General Public License when using or distributing such scripts, even
# though portions of the text of Autoconf appear in them.  The GNU
# General Public License (GPL) does govern all other use of the material
# that constitutes the Autoconf program.
#
# Certain portions of the Autoconf source text are designed to be copied
# (in certain cases, depending on the input) into the output of
# Autoconf.  We call these the "data" portions.  The rest of the Autoconf
# source text consists of comments plus executable code that decides which
# of the data portions to output in any given case.  We call these
# comments and executable code the "non-data" portions.  Autoconf never
# copies any of the non-data portions into its output.
#
# This special exception to the GPL applies to versions of Autoconf
# released by the Free Software Foundation.  When you make and
# distribute a modified version of Autoconf, you may extend this special
# exception to the GPL to apply to your modified version as well, *unless*
# your modified version has the potential to copy into its output some
# of the text that was the non-data portion of the version that you started
# with.  (In other words, unless your change moves or copies text from
# the non-data portions to the data portions.)  If your modification has
# such potential, you must delete any notice of this special exception
# to the GPL from your modified version.
#
# Written by David MacKenzie, with help from
# Franc,ois Pinard, Karl Berry, Richard Pixley, Ian Lance Taylor,
# Roland McGrath, Noah Friedman, david d zuhn, and many others.


# -------------------- #
# 1b. The C language.  #
# -------------------- #


# AC_LANG(C)
# ----------
# CFLAGS is not in ac_cpp because -g, -O, etc. are not valid cpp options.
m4_define([AC_LANG(C)],
[ac_ext=c
ac_cpp='$CPP $CPPFLAGS'
ac_compile='$CC -c $CFLAGS $CPPFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
ac_link='$CC -o conftest$ac_exeext $CFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS >&AS_MESSAGE_LOG_FD'
ac_compiler_gnu=$ac_cv_c_compiler_gnu
])


# AC_LANG_C
# ---------
AU_DEFUN([AC_LANG_C], [AC_LANG(C)])


# _AC_LANG_ABBREV(C)
# ------------------
m4_define([_AC_LANG_ABBREV(C)], [c])


# _AC_LANG_PREFIX(C)
# ------------------
m4_define([_AC_LANG_PREFIX(C)], [C])


# ---------------------- #
# 1c. The C++ language.  #
# ---------------------- #


# AC_LANG(C++)
# ------------
# CXXFLAGS is not in ac_cpp because -g, -O, etc. are not valid cpp options.
m4_define([AC_LANG(C++)],
[ac_ext=cc
ac_cpp='$CXXCPP $CPPFLAGS'
ac_compile='$CXX -c $CXXFLAGS $CPPFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
ac_link='$CXX -o conftest$ac_exeext $CXXFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS >&AS_MESSAGE_LOG_FD'
ac_compiler_gnu=$ac_cv_cxx_compiler_gnu
])


# AC_LANG_CPLUSPLUS
# -----------------
AU_DEFUN([AC_LANG_CPLUSPLUS], [AC_LANG(C++)])


# _AC_LANG_ABBREV(C++)
# --------------------
m4_define([_AC_LANG_ABBREV(C++)], [cxx])


# _AC_LANG_PREFIX(C++)
# --------------------
m4_define([_AC_LANG_PREFIX(C++)], [CXX])



## ---------------------- ##
## 2.Producing programs.  ##
## ---------------------- ##


# --------------- #
# 2b. C sources.  #
# --------------- #

# AC_LANG_SOURCE(C)(BODY)
# -----------------------
# This sometimes fails to find confdefs.h, for some reason.
# We can't use '#line $LINENO "configure"' here, since
# Sun c89 (Sun WorkShop 6 update 2 C 5.3 Patch 111679-08 2002/05/09)
# rejects $LINENO greater than 32767, and some configure scripts
# are longer than 32767 lines.
m4_define([AC_LANG_SOURCE(C)],
[/* confdefs.h.  */
_ACEOF
cat confdefs.h >>conftest.$ac_ext
cat >>conftest.$ac_ext <<_ACEOF
/* end confdefs.h.  */
$1])


# AC_LANG_PROGRAM(C)([PROLOGUE], [BODY])
# --------------------------------------
m4_define([AC_LANG_PROGRAM(C)],
[$1
m4_ifdef([_AC_LANG_PROGRAM_C_F77_HOOKS], [_AC_LANG_PROGRAM_C_F77_HOOKS])[]dnl
m4_ifdef([_AC_LANG_PROGRAM_C_FC_HOOKS], [_AC_LANG_PROGRAM_C_FC_HOOKS])[]dnl
int
main ()
{
dnl Do *not* indent the following line: there may be CPP directives.
dnl Don't move the `;' right after for the same reason.
$2
  ;
  return 0;
}])


# AC_LANG_CALL(C)(PROLOGUE, FUNCTION)
# -----------------------------------
# Avoid conflicting decl of main.
m4_define([AC_LANG_CALL(C)],
[AC_LANG_PROGRAM([$1
m4_if([$2], [main], ,
[/* Override any gcc2 internal prototype to avoid an error.  */
#ifdef __cplusplus
extern "C"
#endif
/* We use char because int might match the return type of a gcc2
   builtin and then its argument prototype would still apply.  */
char $2 ();])], [$2 ();])])


# AC_LANG_FUNC_LINK_TRY(C)(FUNCTION)
# ----------------------------------
# Don't include <ctype.h> because on OSF/1 3.0 it includes
# <sys/types.h> which includes <sys/select.h> which contains a
# prototype for select.  Similarly for bzero.
#
# This test used to merely assign f=$1 in main(), but that was
# optimized away by HP unbundled cc A.05.36 for ia64 under +O3,
# presumably on the basis that there's no need to do that store if the
# program is about to exit.  Conversely, the AIX linker optimizes an
# unused external declaration that initializes f=$1.  So this test
# program has both an external initialization of f, and a use of f in
# main that affects the exit status.
#
m4_define([AC_LANG_FUNC_LINK_TRY(C)],
[AC_LANG_PROGRAM(
[/* Define $1 to an innocuous variant, in case <limits.h> declares $1.
   For example, HP-UX 11i <limits.h> declares gettimeofday.  */
#define $1 innocuous_$1

/* System header to define __stub macros and hopefully few prototypes,
    which can conflict with char $1 (); below.
    Prefer <limits.h> to <assert.h> if __STDC__ is defined, since
    <limits.h> exists even on freestanding compilers.  */

#ifdef __STDC__
# include <limits.h>
#else
# include <assert.h>
#endif

#undef $1

/* Override any gcc2 internal prototype to avoid an error.  */
#ifdef __cplusplus
extern "C"
{
#endif
/* We use char because int might match the return type of a gcc2
   builtin and then its argument prototype would still apply.  */
char $1 ();
/* The GNU C library defines this for functions which it implements
    to always fail with ENOSYS.  Some functions are actually named
    something starting with __ and the normal name is an alias.  */
#if defined (__stub_$1) || defined (__stub___$1)
choke me
#else
char (*f) () = $1;
#endif
#ifdef __cplusplus
}
#endif
], [return f != $1;])])


# AC_LANG_BOOL_COMPILE_TRY(C)(PROLOGUE, EXPRESSION)
# -------------------------------------------------
# Be sure to use this array to avoid `unused' warnings, which are even
# errors with `-W error'.
m4_define([AC_LANG_BOOL_COMPILE_TRY(C)],
[AC_LANG_PROGRAM([$1], [static int test_array @<:@1 - 2 * !($2)@:>@;
test_array @<:@0@:>@ = 0
])])


# AC_LANG_INT_SAVE(C)(PROLOGUE, EXPRESSION)
# -----------------------------------------
# We need `stdio.h' to open a `FILE' and `stdlib.h' for `exit'.
# But we include them only after the EXPRESSION has been evaluated.
m4_define([AC_LANG_INT_SAVE(C)],
[AC_LANG_PROGRAM([$1
long longval () { return $2; }
unsigned long ulongval () { return $2; }
@%:@include <stdio.h>
@%:@include <stdlib.h>],
[
  FILE *f = fopen ("conftest.val", "w");
  if (! f)
    exit (1);
  if (($2) < 0)
    {
      long i = longval ();
      if (i != ($2))
	exit (1);
      fprintf (f, "%ld\n", i);
    }
  else
    {
      unsigned long i = ulongval ();
      if (i != ($2))
	exit (1);
      fprintf (f, "%lu\n", i);
    }
  exit (ferror (f) || fclose (f) != 0);
])])


# ----------------- #
# 2c. C++ sources.  #
# ----------------- #

# AC_LANG_SOURCE(C++)(BODY)
# -------------------------
m4_copy([AC_LANG_SOURCE(C)], [AC_LANG_SOURCE(C++)])


# AC_LANG_PROGRAM(C++)([PROLOGUE], [BODY])
# ----------------------------------------
m4_copy([AC_LANG_PROGRAM(C)], [AC_LANG_PROGRAM(C++)])


# AC_LANG_CALL(C++)(PROLOGUE, FUNCTION)
# -------------------------------------
m4_copy([AC_LANG_CALL(C)], [AC_LANG_CALL(C++)])


# AC_LANG_FUNC_LINK_TRY(C++)(FUNCTION)
# ------------------------------------
m4_copy([AC_LANG_FUNC_LINK_TRY(C)], [AC_LANG_FUNC_LINK_TRY(C++)])


# AC_LANG_BOOL_COMPILE_TRY(C++)(PROLOGUE, EXPRESSION)
# ---------------------------------------------------
m4_copy([AC_LANG_BOOL_COMPILE_TRY(C)], [AC_LANG_BOOL_COMPILE_TRY(C++)])


# AC_LANG_INT_SAVE(C++)(PROLOGUE, EXPRESSION)
# -------------------------------------------
m4_copy([AC_LANG_INT_SAVE(C)], [AC_LANG_INT_SAVE(C++)])



## -------------------------------------------- ##
## 3. Looking for Compilers and Preprocessors.  ##
## -------------------------------------------- ##

# -------------------- #
# 3b. The C compiler.  #
# -------------------- #


# _AC_ARG_VAR_CPPFLAGS
# --------------------
# Document and register CPPFLAGS, which is used by
# AC_PROG_{CC, CPP, CXX, CXXCPP}.
AC_DEFUN([_AC_ARG_VAR_CPPFLAGS],
[AC_ARG_VAR([CPPFLAGS],
	    [C/C++ preprocessor flags, e.g. -I<include dir> if you have
	     headers in a nonstandard directory <include dir>])])


# _AC_ARG_VAR_LDFLAGS
# -------------------
# Document and register LDFLAGS, which is used by
# AC_PROG_{CC, CXX, F77}.
AC_DEFUN([_AC_ARG_VAR_LDFLAGS],
[AC_ARG_VAR([LDFLAGS],
	    [linker flags, e.g. -L<lib dir> if you have libraries in a
	     nonstandard directory <lib dir>])])



# AC_LANG_PREPROC(C)
# -------------------
# Find the C preprocessor.  Must be AC_DEFUN'd to be AC_REQUIRE'able.
AC_DEFUN([AC_LANG_PREPROC(C)],
[AC_REQUIRE([AC_PROG_CPP])])


# _AC_PROG_PREPROC_WORKS_IFELSE(IF-WORKS, IF-NOT)
# -----------------------------------------------
# Check if $ac_cpp is a working preprocessor that can flag absent
# includes either by the exit status or by warnings.
# Set ac_cpp_err to a non-empty value if the preprocessor failed.
# This macro is for all languages, not only C.
AC_DEFUN([_AC_PROG_PREPROC_WORKS_IFELSE],
[ac_preproc_ok=false
for ac_[]_AC_LANG_ABBREV[]_preproc_warn_flag in '' yes
do
  # Use a header file that comes with gcc, so configuring glibc
  # with a fresh cross-compiler works.
  # Prefer <limits.h> to <assert.h> if __STDC__ is defined, since
  # <limits.h> exists even on freestanding compilers.
  # On the NeXT, cc -E runs the code through the compiler's parser,
  # not just through cpp. "Syntax error" is here to catch this case.
  _AC_PREPROC_IFELSE([AC_LANG_SOURCE([[@%:@ifdef __STDC__
@%:@ include <limits.h>
@%:@else
@%:@ include <assert.h>
@%:@endif
		     Syntax error]])],
		     [],
		     [# Broken: fails on valid input.
continue])

  # OK, works on sane cases.  Now check whether non-existent headers
  # can be detected and how.
  _AC_PREPROC_IFELSE([AC_LANG_SOURCE([[@%:@include <ac_nonexistent.h>]])],
		     [# Broken: success on invalid input.
continue],
		     [# Passes both tests.
ac_preproc_ok=:
break])

done
# Because of `break', _AC_PREPROC_IFELSE's cleaning code was skipped.
rm -f conftest.err conftest.$ac_ext
AS_IF([$ac_preproc_ok], [$1], [$2])])# _AC_PROG_PREPROC_WORKS_IFELSE


# AC_PROG_CPP
# -----------
# Find a working C preprocessor.
# We shouldn't have to require AC_PROG_CC, but this is due to the concurrency
# between the AC_LANG_COMPILER_REQUIRE family and that of AC_PROG_CC.
AN_MAKEVAR([CPP], [AC_PROG_CPP])
AN_PROGRAM([cpp], [AC_PROG_CPP])
AC_DEFUN([AC_PROG_CPP],
[AC_REQUIRE([AC_PROG_CC])dnl
AC_ARG_VAR([CPP],      [C preprocessor])dnl
_AC_ARG_VAR_CPPFLAGS()dnl
AC_LANG_PUSH(C)dnl
AC_MSG_CHECKING([how to run the C preprocessor])
# On Suns, sometimes $CPP names a directory.
if test -n "$CPP" && test -d "$CPP"; then
  CPP=
fi
if test -z "$CPP"; then
  AC_CACHE_VAL([ac_cv_prog_CPP],
  [dnl
    # Double quotes because CPP needs to be expanded
    for CPP in "$CC -E" "$CC -E -traditional-cpp" "/lib/cpp"
    do
      _AC_PROG_PREPROC_WORKS_IFELSE([break])
    done
    ac_cv_prog_CPP=$CPP
  ])dnl
  CPP=$ac_cv_prog_CPP
else
  ac_cv_prog_CPP=$CPP
fi
AC_MSG_RESULT([$CPP])
_AC_PROG_PREPROC_WORKS_IFELSE([],
		[AC_MSG_FAILURE([C preprocessor "$CPP" fails sanity check])])
AC_SUBST(CPP)dnl
AC_LANG_POP(C)dnl
])# AC_PROG_CPP

# AC_PROG_CPP_WERROR
# ------------------
# Treat warnings from the preprocessor as errors.
AC_DEFUN([AC_PROG_CPP_WERROR],
[AC_REQUIRE([AC_PROG_CPP])dnl
ac_c_preproc_warn_flag=yes])# AC_PROG_CPP_WERROR

# AC_LANG_COMPILER(C)
# -------------------
# Find the C compiler.  Must be AC_DEFUN'd to be AC_REQUIRE'able.
AC_DEFUN([AC_LANG_COMPILER(C)],
[AC_REQUIRE([AC_PROG_CC])])


# ac_cv_prog_gcc
# --------------
# We used to name the cache variable this way.
AU_DEFUN([ac_cv_prog_gcc],
[ac_cv_c_compiler_gnu])


# AC_PROG_CC([COMPILER ...])
# --------------------------
# COMPILER ... is a space separated list of C compilers to search for.
# This just gives the user an opportunity to specify an alternative
# search list for the C compiler.
AN_MAKEVAR([CC],  [AC_PROG_CC])
AN_PROGRAM([cc],  [AC_PROG_CC])
AN_PROGRAM([gcc], [AC_PROG_CC])
AC_DEFUN([AC_PROG_CC],
[AC_LANG_PUSH(C)dnl
AC_ARG_VAR([CC],     [C compiler command])dnl
AC_ARG_VAR([CFLAGS], [C compiler flags])dnl
_AC_ARG_VAR_LDFLAGS()dnl
_AC_ARG_VAR_CPPFLAGS()dnl
m4_ifval([$1],
      [AC_CHECK_TOOLS(CC, [$1])],
[AC_CHECK_TOOL(CC, gcc)
if test -z "$CC"; then
  AC_CHECK_TOOL(CC, cc)
fi
if test -z "$CC"; then
  AC_CHECK_PROG(CC, cc, cc, , , /usr/ucb/cc)
fi
if test -z "$CC"; then
  AC_CHECK_TOOLS(CC, cl)
fi
])

test -z "$CC" && AC_MSG_FAILURE([no acceptable C compiler found in \$PATH])

# Provide some information about the compiler.
echo "$as_me:$LINENO:" \
     "checking for _AC_LANG compiler version" >&AS_MESSAGE_LOG_FD
ac_compiler=`set X $ac_compile; echo $[2]`
_AC_EVAL([$ac_compiler --version </dev/null >&AS_MESSAGE_LOG_FD])
_AC_EVAL([$ac_compiler -v </dev/null >&AS_MESSAGE_LOG_FD])
_AC_EVAL([$ac_compiler -V </dev/null >&AS_MESSAGE_LOG_FD])

m4_expand_once([_AC_COMPILER_EXEEXT])[]dnl
m4_expand_once([_AC_COMPILER_OBJEXT])[]dnl
_AC_LANG_COMPILER_GNU
GCC=`test $ac_compiler_gnu = yes && echo yes`
_AC_PROG_CC_G
_AC_PROG_CC_STDC
# Some people use a C++ compiler to compile C.  Since we use `exit',
# in C++ we need to declare it.  In case someone uses the same compiler
# for both compiling C and C++ we need to have the C++ compiler decide
# the declaration of exit, since it's the most demanding environment.
_AC_COMPILE_IFELSE([@%:@ifndef __cplusplus
  choke me
@%:@endif],
		   [_AC_PROG_CXX_EXIT_DECLARATION])
AC_LANG_POP(C)dnl
])# AC_PROG_CC


# _AC_PROG_CC_G
# -------------
# Check whether -g works, even if CFLAGS is set, in case the package
# plays around with CFLAGS (such as to build both debugging and normal
# versions of a library), tasteless as that idea is.
m4_define([_AC_PROG_CC_G],
[ac_test_CFLAGS=${CFLAGS+set}
ac_save_CFLAGS=$CFLAGS
CFLAGS="$CFLAGS -g"
AC_CACHE_CHECK(whether $CC accepts -g, ac_cv_prog_cc_g,
	       [_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()], [ac_cv_prog_cc_g=yes],
							[ac_cv_prog_cc_g=no])])
if test "$ac_test_CFLAGS" = set; then
  CFLAGS=$ac_save_CFLAGS
elif test $ac_cv_prog_cc_g = yes; then
  if test "$GCC" = yes; then
    CFLAGS="-g -O2"
  else
    CFLAGS="-g"
  fi
else
  if test "$GCC" = yes; then
    CFLAGS="-O2"
  else
    CFLAGS=
  fi
fi[]dnl
])# _AC_PROG_CC_G


# AC_PROG_GCC_TRADITIONAL
# -----------------------
AN_FUNCTION([ioctl],   [AC_PROG_GCC_TRADITIONAL])
AC_DEFUN([AC_PROG_GCC_TRADITIONAL],
[if test $ac_cv_c_compiler_gnu = yes; then
    AC_CACHE_CHECK(whether $CC needs -traditional,
      ac_cv_prog_gcc_traditional,
[  ac_pattern="Autoconf.*'x'"
  AC_EGREP_CPP($ac_pattern, [#include <sgtty.h>
Autoconf TIOCGETP],
  ac_cv_prog_gcc_traditional=yes, ac_cv_prog_gcc_traditional=no)

  if test $ac_cv_prog_gcc_traditional = no; then
    AC_EGREP_CPP($ac_pattern, [#include <termio.h>
Autoconf TCGETA],
    ac_cv_prog_gcc_traditional=yes)
  fi])
  if test $ac_cv_prog_gcc_traditional = yes; then
    CC="$CC -traditional"
  fi
fi
])# AC_PROG_GCC_TRADITIONAL


# AC_PROG_CC_C_O
# --------------
AC_DEFUN([AC_PROG_CC_C_O],
[AC_REQUIRE([AC_PROG_CC])dnl
if test "x$CC" != xcc; then
  AC_MSG_CHECKING([whether $CC and cc understand -c and -o together])
else
  AC_MSG_CHECKING([whether cc understands -c and -o together])
fi
set dummy $CC; ac_cc=`echo $[2] |
		      sed 's/[[^a-zA-Z0-9_]]/_/g;s/^[[0-9]]/_/'`
AC_CACHE_VAL(ac_cv_prog_cc_${ac_cc}_c_o,
[AC_LANG_CONFTEST([AC_LANG_PROGRAM([])])
# Make sure it works both with $CC and with simple cc.
# We do the test twice because some compilers refuse to overwrite an
# existing .o file with -o, though they will create one.
ac_try='$CC -c conftest.$ac_ext -o conftest.$ac_objext >&AS_MESSAGE_LOG_FD'
if AC_TRY_EVAL(ac_try) &&
   test -f conftest.$ac_objext && AC_TRY_EVAL(ac_try);
then
  eval ac_cv_prog_cc_${ac_cc}_c_o=yes
  if test "x$CC" != xcc; then
    # Test first that cc exists at all.
    if AC_TRY_COMMAND(cc -c conftest.$ac_ext >&AS_MESSAGE_LOG_FD); then
      ac_try='cc -c conftest.$ac_ext -o conftest.$ac_objext >&AS_MESSAGE_LOG_FD'
      if AC_TRY_EVAL(ac_try) &&
	 test -f conftest.$ac_objext && AC_TRY_EVAL(ac_try);
      then
	# cc works too.
	:
      else
	# cc exists but doesn't like -o.
	eval ac_cv_prog_cc_${ac_cc}_c_o=no
      fi
    fi
  fi
else
  eval ac_cv_prog_cc_${ac_cc}_c_o=no
fi
rm -f conftest*
])dnl
if eval "test \"`echo '$ac_cv_prog_cc_'${ac_cc}_c_o`\" = yes"; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  AC_DEFINE(NO_MINUS_C_MINUS_O, 1,
	   [Define to 1 if your C compiler doesn't accept -c and -o together.])
fi
])# AC_PROG_CC_C_O


# ---------------------- #
# 3c. The C++ compiler.  #
# ---------------------- #


# AC_LANG_PREPROC(C++)
# ---------------------
# Find the C++ preprocessor.  Must be AC_DEFUN'd to be AC_REQUIRE'able.
AC_DEFUN([AC_LANG_PREPROC(C++)],
[AC_REQUIRE([AC_PROG_CXXCPP])])


# AC_PROG_CXXCPP
# --------------
# Find a working C++ preprocessor.
# We shouldn't have to require AC_PROG_CC, but this is due to the concurrency
# between the AC_LANG_COMPILER_REQUIRE family and that of AC_PROG_CXX.
AC_DEFUN([AC_PROG_CXXCPP],
[AC_REQUIRE([AC_PROG_CXX])dnl
AC_ARG_VAR([CXXCPP],   [C++ preprocessor])dnl
_AC_ARG_VAR_CPPFLAGS()dnl
AC_LANG_PUSH(C++)dnl
AC_MSG_CHECKING([how to run the C++ preprocessor])
if test -z "$CXXCPP"; then
  AC_CACHE_VAL(ac_cv_prog_CXXCPP,
  [dnl
    # Double quotes because CXXCPP needs to be expanded
    for CXXCPP in "$CXX -E" "/lib/cpp"
    do
      _AC_PROG_PREPROC_WORKS_IFELSE([break])
    done
    ac_cv_prog_CXXCPP=$CXXCPP
  ])dnl
  CXXCPP=$ac_cv_prog_CXXCPP
else
  ac_cv_prog_CXXCPP=$CXXCPP
fi
AC_MSG_RESULT([$CXXCPP])
_AC_PROG_PREPROC_WORKS_IFELSE([],
	  [AC_MSG_FAILURE([C++ preprocessor "$CXXCPP" fails sanity check])])
AC_SUBST(CXXCPP)dnl
AC_LANG_POP(C++)dnl
])# AC_PROG_CXXCPP


# AC_LANG_COMPILER(C++)
# ---------------------
# Find the C++ compiler.  Must be AC_DEFUN'd to be AC_REQUIRE'able.
AC_DEFUN([AC_LANG_COMPILER(C++)],
[AC_REQUIRE([AC_PROG_CXX])])


# ac_cv_prog_gxx
# --------------
# We used to name the cache variable this way.
AU_DEFUN([ac_cv_prog_gxx],
[ac_cv_cxx_compiler_gnu])


# AC_PROG_CXX([LIST-OF-COMPILERS])
# --------------------------------
# LIST-OF-COMPILERS is a space separated list of C++ compilers to search
# for (if not specified, a default list is used).  This just gives the
# user an opportunity to specify an alternative search list for the C++
# compiler.
# aCC	HP-UX C++ compiler much better than `CC', so test before.
# FCC   Fujitsu C++ compiler
# KCC	KAI C++ compiler
# RCC	Rational C++
# xlC_r	AIX C Set++ (with support for reentrant code)
# xlC	AIX C Set++
AN_MAKEVAR([CXX],  [AC_PROG_CXX])
AN_PROGRAM([CC],   [AC_PROG_CXX])
AN_PROGRAM([c++],  [AC_PROG_CXX])
AN_PROGRAM([g++],  [AC_PROG_CXX])
AC_DEFUN([AC_PROG_CXX],
[AC_LANG_PUSH(C++)dnl
AC_ARG_VAR([CXX],      [C++ compiler command])dnl
AC_ARG_VAR([CXXFLAGS], [C++ compiler flags])dnl
_AC_ARG_VAR_LDFLAGS()dnl
_AC_ARG_VAR_CPPFLAGS()dnl
AC_CHECK_TOOLS(CXX,
	       [$CCC m4_default([$1],
			  [g++ c++ gpp aCC CC cxx cc++ cl FCC KCC RCC xlC_r xlC])],
	       g++)

# Provide some information about the compiler.
echo "$as_me:$LINENO:" \
     "checking for _AC_LANG compiler version" >&AS_MESSAGE_LOG_FD
ac_compiler=`set X $ac_compile; echo $[2]`
_AC_EVAL([$ac_compiler --version </dev/null >&AS_MESSAGE_LOG_FD])
_AC_EVAL([$ac_compiler -v </dev/null >&AS_MESSAGE_LOG_FD])
_AC_EVAL([$ac_compiler -V </dev/null >&AS_MESSAGE_LOG_FD])

m4_expand_once([_AC_COMPILER_EXEEXT])[]dnl
m4_expand_once([_AC_COMPILER_OBJEXT])[]dnl
_AC_LANG_COMPILER_GNU
GXX=`test $ac_compiler_gnu = yes && echo yes`
_AC_PROG_CXX_G
_AC_PROG_CXX_EXIT_DECLARATION
AC_LANG_POP(C++)dnl
])# AC_PROG_CXX


# _AC_PROG_CXX_G
# --------------
# Check whether -g works, even if CXXFLAGS is set, in case the package
# plays around with CXXFLAGS (such as to build both debugging and
# normal versions of a library), tasteless as that idea is.
m4_define([_AC_PROG_CXX_G],
[ac_test_CXXFLAGS=${CXXFLAGS+set}
ac_save_CXXFLAGS=$CXXFLAGS
CXXFLAGS="-g"
AC_CACHE_CHECK(whether $CXX accepts -g, ac_cv_prog_cxx_g,
	       [_AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
				   [ac_cv_prog_cxx_g=yes],
				   [ac_cv_prog_cxx_g=no])])
if test "$ac_test_CXXFLAGS" = set; then
  CXXFLAGS=$ac_save_CXXFLAGS
elif test $ac_cv_prog_cxx_g = yes; then
  if test "$GXX" = yes; then
    CXXFLAGS="$CXXFLAGS -g -O2"
  else
    CXXFLAGS="$CXXFLAGS -g"
  fi
else
  if test "$GXX" = yes; then
    CXXFLAGS="-O2"
  else
    CXXFLAGS=
  fi
fi[]dnl
])# _AC_PROG_CXX_G


# _AC_PROG_CXX_EXIT_DECLARATION
# -----------------------------
# If <stdlib.h> doesn't already provide a valid prototype for exit,
# determine the appropriate prototype and put it in confdefs.h.
# This macro is run only when testing a C++ compiler, but it generates
# a prototype that is also appropriate for C compilers in order to
# support a mixed C/C++ configuration environment.
# We don't need to worry about this for C, since we include <stdlib.h>
# if it is available, and that method works for all C compilers.
m4_define([_AC_PROG_CXX_EXIT_DECLARATION],
[for ac_declaration in \
   '' \
   'extern "C" void std::exit (int) throw (); using std::exit;' \
   'extern "C" void std::exit (int); using std::exit;' \
   'extern "C" void exit (int) throw ();' \
   'extern "C" void exit (int);' \
   'void exit (int);'
do
  _AC_COMPILE_IFELSE([AC_LANG_PROGRAM([$ac_declaration
@%:@include <stdlib.h>],
				      [exit (42);])],
		     [],
		     [continue])
  _AC_COMPILE_IFELSE([AC_LANG_PROGRAM([$ac_declaration],
				      [exit (42);])],
		     [break])
done
rm -f conftest*
if test -n "$ac_declaration"; then
  echo '#ifdef __cplusplus' >>confdefs.h
  echo $ac_declaration      >>confdefs.h
  echo '#endif'             >>confdefs.h
fi
])# _AC_PROG_CXX_EXIT_DECLARATION






## ------------------------------- ##
## 4. Compilers' characteristics.  ##
## ------------------------------- ##


# -------------------------------- #
# 4b. C compiler characteristics.  #
# -------------------------------- #

# _AC_PROG_CC_STDC
# ----------------
# If the C compiler in not in ANSI C mode by default, try to add an
# option to output variable @code{CC} to make it so.  This macro tries
# various options that select ANSI C on some system or another.  It
# considers the compiler to be in ANSI C mode if it handles function
# prototypes correctly.
AC_DEFUN([_AC_PROG_CC_STDC],
[AC_MSG_CHECKING([for $CC option to accept ANSI C])
AC_CACHE_VAL(ac_cv_prog_cc_stdc,
[ac_cv_prog_cc_stdc=no
ac_save_CC=$CC
AC_LANG_CONFTEST([AC_LANG_PROGRAM(
[[#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
/* Most of the following tests are stolen from RCS 5.7's src/conf.sh.  */
struct buf { int x; };
FILE * (*rcsopen) (struct buf *, struct stat *, int);
static char *e (p, i)
     char **p;
     int i;
{
  return p[i];
}
static char *f (char * (*g) (char **, int), char **p, ...)
{
  char *s;
  va_list v;
  va_start (v,p);
  s = g (p, va_arg (v,int));
  va_end (v);
  return s;
}

/* OSF 4.0 Compaq cc is some sort of almost-ANSI by default.  It has
   function prototypes and stuff, but not '\xHH' hex character constants.
   These don't provoke an error unfortunately, instead are silently treated
   as 'x'.  The following induces an error, until -std1 is added to get
   proper ANSI mode.  Curiously '\x00'!='x' always comes out true, for an
   array size at least.  It's necessary to write '\x00'==0 to get something
   that's true only with -std1.  */
int osf4_cc_array ['\x00' == 0 ? 1 : -1];

int test (int i, double x);
struct s1 {int (*f) (int a);};
struct s2 {int (*f) (double a);};
int pairnames (int, char **, FILE *(*)(struct buf *, struct stat *, int), int, int);
int argc;
char **argv;]],
[[return f (e, argv, 0) != argv[0]  ||  f (e, argv, 1) != argv[1];]])])
# Don't try gcc -ansi; that turns off useful extensions and
# breaks some systems' header files.
# AIX			-qlanglvl=ansi
# Ultrix and OSF/1	-std1
# HP-UX 10.20 and later	-Ae
# HP-UX older versions	-Aa -D_HPUX_SOURCE
# SVR4			-Xc -D__EXTENSIONS__
for ac_arg in "" -qlanglvl=ansi -std1 -Ae "-Aa -D_HPUX_SOURCE" "-Xc -D__EXTENSIONS__"
do
  CC="$ac_save_CC $ac_arg"
  _AC_COMPILE_IFELSE([],
		     [ac_cv_prog_cc_stdc=$ac_arg
break])
done
rm -f conftest.$ac_ext conftest.$ac_objext
CC=$ac_save_CC
])
case "x$ac_cv_prog_cc_stdc" in
  x|xno)
    AC_MSG_RESULT([none needed]) ;;
  *)
    AC_MSG_RESULT([$ac_cv_prog_cc_stdc])
    CC="$CC $ac_cv_prog_cc_stdc" ;;
esac
])# _AC_PROG_CC_STDC


# AC_PROG_CC_STDC
# ---------------
# Has been merged into AC_PROG_CC.
AU_DEFUN([AC_PROG_CC_STDC], [])


# AC_C_BACKSLASH_A
# ----------------
AC_DEFUN([AC_C_BACKSLASH_A],
[
  AC_CACHE_CHECK([whether backslash-a works in strings], ac_cv_c_backslash_a,
   [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],
     [[
#if '\a' == 'a'
      syntax error;
#endif
      char buf['\a' == 'a' ? -1 : 1];
      buf[0] = '\a';
      return buf[0] != "\a"[0];
     ]])],
     [ac_cv_c_backslash_a=yes],
     [ac_cv_c_backslash_a=no])])
  if test $ac_cv_c_backslash_a = yes; then
    AC_DEFINE(HAVE_C_BACKSLASH_A, 1,
      [Define if backslash-a works in C strings.])
  fi
])


# AC_C_CROSS
# ----------
# Has been merged into AC_PROG_CC.
AU_DEFUN([AC_C_CROSS], [])


# AC_C_CHAR_UNSIGNED
# ------------------
AC_DEFUN([AC_C_CHAR_UNSIGNED],
[AH_VERBATIM([__CHAR_UNSIGNED__],
[/* Define to 1 if type `char' is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
# undef __CHAR_UNSIGNED__
#endif])dnl
AC_CACHE_CHECK(whether char is unsigned, ac_cv_c_char_unsigned,
[AC_COMPILE_IFELSE([AC_LANG_BOOL_COMPILE_TRY([AC_INCLUDES_DEFAULT([])],
					     [((char) -1) < 0])],
		   ac_cv_c_char_unsigned=no, ac_cv_c_char_unsigned=yes)])
if test $ac_cv_c_char_unsigned = yes && test "$GCC" != yes; then
  AC_DEFINE(__CHAR_UNSIGNED__)
fi
])# AC_C_CHAR_UNSIGNED


# AC_C_LONG_DOUBLE
# ----------------
AC_DEFUN([AC_C_LONG_DOUBLE],
[AC_CACHE_CHECK(
   [for working long double with more range or precision than double],
   [ac_cv_c_long_double],
   [AC_COMPILE_IFELSE(
      [AC_LANG_BOOL_COMPILE_TRY(
	 [#include <float.h>
	  long double foo = 0.0;],
	 [/* Using '|' rather than '||' catches a GCC 2.95.2 x86 bug.  */
	  (DBL_MAX < LDBL_MAX) | (LDBL_EPSILON < DBL_EPSILON)
	  | (DBL_MAX_EXP < LDBL_MAX_EXP) | (DBL_MANT_DIG < LDBL_MANT_DIG)])],
      ac_cv_c_long_double=yes,
      ac_cv_c_long_double=no)])
if test $ac_cv_c_long_double = yes; then
  AC_DEFINE(HAVE_LONG_DOUBLE, 1,
	    [Define to 1 if long double works and has more range or precision than double.])
fi
])# AC_C_LONG_DOUBLE


# AC_C_BIGENDIAN ([ACTION-IF-TRUE], [ACTION-IF-FALSE], [ACTION-IF-UNKNOWN])
# -------------------------------------------------------------------------
AC_DEFUN([AC_C_BIGENDIAN],
[AC_CACHE_CHECK(whether byte ordering is bigendian, ac_cv_c_bigendian,
[# See if sys/param.h defines the BYTE_ORDER macro.
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <sys/types.h>
#include <sys/param.h>
],
[#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
 bogus endian macros
#endif
])],
[# It does; now see whether it defined to BIG_ENDIAN or not.
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <sys/types.h>
#include <sys/param.h>
], [#if BYTE_ORDER != BIG_ENDIAN
 not big endian
#endif
])], [ac_cv_c_bigendian=yes], [ac_cv_c_bigendian=no])],
[# It does not; compile a test program.
AC_RUN_IFELSE(
[AC_LANG_SOURCE([[int
main ()
{
  /* Are we little or big endian?  From Harbison&Steele.  */
  union
  {
    long l;
    char c[sizeof (long)];
  } u;
  u.l = 1;
  exit (u.c[sizeof (long) - 1] == 1);
}]])],
	      [ac_cv_c_bigendian=no],
	      [ac_cv_c_bigendian=yes],
[# try to guess the endianness by grepping values into an object file
  ac_cv_c_bigendian=unknown
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[[short ascii_mm[] = { 0x4249, 0x4765, 0x6E44, 0x6961, 0x6E53, 0x7953, 0 };
short ascii_ii[] = { 0x694C, 0x5454, 0x656C, 0x6E45, 0x6944, 0x6E61, 0 };
void _ascii () { char *s = (char *) ascii_mm; s = (char *) ascii_ii; }
short ebcdic_ii[] = { 0x89D3, 0xE3E3, 0x8593, 0x95C5, 0x89C4, 0x9581, 0 };
short ebcdic_mm[] = { 0xC2C9, 0xC785, 0x95C4, 0x8981, 0x95E2, 0xA8E2, 0 };
void _ebcdic () { char *s = (char *) ebcdic_mm; s = (char *) ebcdic_ii; }]],
[[ _ascii (); _ebcdic (); ]])],
[if grep BIGenDianSyS conftest.$ac_objext >/dev/null ; then
  ac_cv_c_bigendian=yes
fi
if grep LiTTleEnDian conftest.$ac_objext >/dev/null ; then
  if test "$ac_cv_c_bigendian" = unknown; then
    ac_cv_c_bigendian=no
  else
    # finding both strings is unlikely to happen, but who knows?
    ac_cv_c_bigendian=unknown
  fi
fi])])])])
case $ac_cv_c_bigendian in
  yes)
    m4_default([$1],
      [AC_DEFINE([WORDS_BIGENDIAN], 1,
	[Define to 1 if your processor stores words with the most significant
	 byte first (like Motorola and SPARC, unlike Intel and VAX).])]) ;;
  no)
    $2 ;;
  *)
    m4_default([$3],
      [AC_MSG_ERROR([unknown endianness
presetting ac_cv_c_bigendian=no (or yes) will help])]) ;;
esac
])# AC_C_BIGENDIAN


# AC_C_INLINE
# -----------
# Do nothing if the compiler accepts the inline keyword.
# Otherwise define inline to __inline__ or __inline if one of those work,
# otherwise define inline to be empty.
#
# HP C version B.11.11.04 doesn't allow a typedef as the return value for an
# inline function, only builtin types.
#
AN_IDENTIFIER([inline], [AC_C_INLINE])
AC_DEFUN([AC_C_INLINE],
[AC_CACHE_CHECK([for inline], ac_cv_c_inline,
[ac_cv_c_inline=no
for ac_kw in inline __inline__ __inline; do
  AC_COMPILE_IFELSE([AC_LANG_SOURCE(
[#ifndef __cplusplus
typedef int foo_t;
static $ac_kw foo_t static_foo () {return 0; }
$ac_kw foo_t foo () {return 0; }
#endif
])],
		    [ac_cv_c_inline=$ac_kw; break])
done
])
AH_VERBATIM([inline],
[/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif])
case $ac_cv_c_inline in
  inline | yes) ;;
  *)
    case $ac_cv_c_inline in
      no) ac_val=;;
      *) ac_val=$ac_cv_c_inline;;
    esac
    cat >>confdefs.h <<_ACEOF
#ifndef __cplusplus
#define inline $ac_val
#endif
_ACEOF
    ;;
esac
])# AC_C_INLINE


# AC_C_CONST
# ----------
AN_IDENTIFIER([const],  [AC_C_CONST])
AC_DEFUN([AC_C_CONST],
[AC_CACHE_CHECK([for an ANSI C-conforming const], ac_cv_c_const,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],
[[/* FIXME: Include the comments suggested by Paul. */
#ifndef __cplusplus
  /* Ultrix mips cc rejects this.  */
  typedef int charset[2];
  const charset x;
  /* SunOS 4.1.1 cc rejects this.  */
  char const *const *ccp;
  char **p;
  /* NEC SVR4.0.2 mips cc rejects this.  */
  struct point {int x, y;};
  static struct point const zero = {0,0};
  /* AIX XL C 1.02.0.0 rejects this.
     It does not let you subtract one const X* pointer from another in
     an arm of an if-expression whose if-part is not a constant
     expression */
  const char *g = "string";
  ccp = &g + (g ? g-g : 0);
  /* HPUX 7.0 cc rejects these. */
  ++ccp;
  p = (char**) ccp;
  ccp = (char const *const *) p;
  { /* SCO 3.2v4 cc rejects this.  */
    char *t;
    char const *s = 0 ? (char *) 0 : (char const *) 0;

    *t++ = 0;
  }
  { /* Someone thinks the Sun supposedly-ANSI compiler will reject this.  */
    int x[] = {25, 17};
    const int *foo = &x[0];
    ++foo;
  }
  { /* Sun SC1.0 ANSI compiler rejects this -- but not the above. */
    typedef const int *iptr;
    iptr p = 0;
    ++p;
  }
  { /* AIX XL C 1.02.0.0 rejects this saying
       "k.c", line 2.27: 1506-025 (S) Operand must be a modifiable lvalue. */
    struct s { int j; const int *ap[3]; };
    struct s *b; b->j = 5;
  }
  { /* ULTRIX-32 V3.1 (Rev 9) vcc rejects this */
    const int foo = 10;
  }
#endif
]])],
		   [ac_cv_c_const=yes],
		   [ac_cv_c_const=no])])
if test $ac_cv_c_const = no; then
  AC_DEFINE(const,,
	    [Define to empty if `const' does not conform to ANSI C.])
fi
])# AC_C_CONST


# AC_C_RESTRICT
# -------------
# based on acx_restrict.m4, from the GNU Autoconf Macro Archive at:
# http://www.gnu.org/software/ac-archive/htmldoc/acx_restrict.html
#
# Determine whether the C/C++ compiler supports the "restrict" keyword
# introduced in ANSI C99, or an equivalent.  Do nothing if the compiler
# accepts it.  Otherwise, if the compiler supports an equivalent,
# define "restrict" to be that.  Here are some variants:
# - GCC supports both __restrict and __restrict__
# - older DEC Alpha C compilers support only __restrict
# - _Restrict is the only spelling accepted by Sun WorkShop 6 update 2 C
# Otherwise, define "restrict" to be empty.
AN_IDENTIFIER([restrict], [AC_C_RESTRICT])
AC_DEFUN([AC_C_RESTRICT],
[AC_CACHE_CHECK([for C/C++ restrict keyword], ac_cv_c_restrict,
  [ac_cv_c_restrict=no
   # Try the official restrict keyword, then gcc's __restrict, and
   # the less common variants.
   for ac_kw in restrict __restrict __restrict__ _Restrict; do
     AC_COMPILE_IFELSE([AC_LANG_SOURCE(
      [float * $ac_kw x;])],
      [ac_cv_c_restrict=$ac_kw; break])
   done
  ])
 case $ac_cv_c_restrict in
   restrict) ;;
   no) AC_DEFINE(restrict,,
	[Define to equivalent of C99 restrict keyword, or to nothing if this
	is not supported.  Do not define if restrict is supported directly.]) ;;
   *)  AC_DEFINE_UNQUOTED(restrict, $ac_cv_c_restrict) ;;
 esac
])# AC_C_RESTRICT


# AC_C_VOLATILE
# -------------
# Note that, unlike const, #defining volatile to be the empty string can
# actually turn a correct program into an incorrect one, since removing
# uses of volatile actually grants the compiler permission to perform
# optimizations that could break the user's code.  So, do not #define
# volatile away unless it is really necessary to allow the user's code
# to compile cleanly.  Benign compiler failures should be tolerated.
AN_IDENTIFIER([volatile], [AC_C_VOLATILE])
AC_DEFUN([AC_C_VOLATILE],
[AC_CACHE_CHECK([for working volatile], ac_cv_c_volatile,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [
volatile int x;
int * volatile y;])],
		   [ac_cv_c_volatile=yes],
		   [ac_cv_c_volatile=no])])
if test $ac_cv_c_volatile = no; then
  AC_DEFINE(volatile,,
	    [Define to empty if the keyword `volatile' does not work.
	     Warning: valid code using `volatile' can become incorrect
	     without.  Disable with care.])
fi
])# AC_C_VOLATILE


# AC_C_STRINGIZE
# --------------
# Checks if `#' can be used to glue strings together at the CPP level.
# Defines HAVE_STRINGIZE if positive.
AC_DEFUN([AC_C_STRINGIZE],
[AC_CACHE_CHECK([for preprocessor stringizing operator],
		[ac_cv_c_stringize],
[AC_EGREP_CPP([@%:@teststring],
	      [@%:@define x(y) #y

char *s = x(teststring);],
	      [ac_cv_c_stringize=no],
	      [ac_cv_c_stringize=yes])])
if test $ac_cv_c_stringize = yes; then
  AC_DEFINE(HAVE_STRINGIZE, 1,
	    [Define to 1 if cpp supports the ANSI @%:@ stringizing operator.])
fi
])# AC_C_STRINGIZE


# AC_C_PROTOTYPES
# ---------------
# Check if the C compiler supports prototypes, included if it needs
# options.
AC_DEFUN([AC_C_PROTOTYPES],
[AC_REQUIRE([AC_PROG_CC])dnl
AC_MSG_CHECKING([for function prototypes])
if test "$ac_cv_prog_cc_stdc" != no; then
  AC_MSG_RESULT([yes])
  AC_DEFINE(PROTOTYPES, 1,
	    [Define to 1 if the C compiler supports function prototypes.])
  AC_DEFINE(__PROTOTYPES, 1,
	    [Define like PROTOTYPES; this can be used by system headers.])
else
  AC_MSG_RESULT([no])
fi
])# AC_C_PROTOTYPES
