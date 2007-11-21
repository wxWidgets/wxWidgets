# This file is part of Autoconf.                       -*- Autoconf -*-
# Parameterized macros.
# Copyright (C) 1992, 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2001,
# 2002, 2003, Free Software Foundation, Inc.

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


## ---------------- ##
## The diversions.  ##
## ---------------- ##


# We heavily use m4's diversions both for the initializations and for
# required macros (see AC_REQUIRE), because in both cases we have to
# issue high in `configure' something which is discovered late.
#
# KILL is only used to suppress output.
#
# The layers of `configure'.  We let m4 undivert them by itself, when
# it reaches the end of `configure.ac'.
#
# - BINSH
#   #! /bin/sh
# - HEADER-REVISION
#   Sent by AC_REVISION
# - HEADER-COMMENT
#   Purpose of the script.
# - HEADER-COPYRIGHT
#   Copyright notice(s)
# - M4SH-INIT
#   Initialization of bottom layers.
#
# - DEFAULTS
#   early initializations (defaults)
# - PARSE_ARGS
#   initialization code, option handling loop.
#
# - HELP_BEGIN
#   Handling `configure --help'.
# - HELP_CANON
#   Help msg for AC_CANONICAL_*
# - HELP_ENABLE
#   Help msg from AC_ARG_ENABLE.
# - HELP_WITH
#   Help msg from AC_ARG_WITH.
# - HELP_VAR
#   Help msg from AC_ARG_VAR.
# - HELP_VAR_END
#   A small paragraph on the use of the variables.
# - HELP_END
#   Tail of the handling of --help.
#
# - VERSION_BEGIN
#   Head of the handling of --version.
# - VERSION_FSF
#   FSF copyright notice for --version.
# - VERSION_USER
#   User copyright notice for --version.
# - VERSION_END
#   Tail of the handling of --version.
#
# - INIT_PREPARE
#   Tail of initialization code.
#
# - BODY
#   the tests and output code
#


# _m4_divert(DIVERSION-NAME)
# --------------------------
# Convert a diversion name into its number.  Otherwise, return
# DIVERSION-NAME which is supposed to be an actual diversion number.
# Of course it would be nicer to use m4_case here, instead of zillions
# of little macros, but it then takes twice longer to run `autoconf'!
#
# From M4sugar:
#    -1. KILL
# 10000. GROW
#
# From M4sh:
#    0. BINSH
#    1. HEADER-REVISION
#    2. HEADER-COMMENT
#    3. HEADER-COPYRIGHT
#    4. M4SH-INIT
# 1000. BODY
m4_define([_m4_divert(DEFAULTS)],        10)
m4_define([_m4_divert(PARSE_ARGS)],      20)

m4_define([_m4_divert(HELP_BEGIN)],     100)
m4_define([_m4_divert(HELP_CANON)],     101)
m4_define([_m4_divert(HELP_ENABLE)],    102)
m4_define([_m4_divert(HELP_WITH)],      103)
m4_define([_m4_divert(HELP_VAR)],       104)
m4_define([_m4_divert(HELP_VAR_END)],   105)
m4_define([_m4_divert(HELP_END)],       106)

m4_define([_m4_divert(VERSION_BEGIN)],  200)
m4_define([_m4_divert(VERSION_FSF)],    201)
m4_define([_m4_divert(VERSION_USER)],   202)
m4_define([_m4_divert(VERSION_END)],    203)

m4_define([_m4_divert(INIT_PREPARE)],   300)



# AC_DIVERT_PUSH(DIVERSION-NAME)
# AC_DIVERT_POP
# ------------------------------
m4_copy([m4_divert_push],[AC_DIVERT_PUSH])
m4_copy([m4_divert_pop], [AC_DIVERT_POP])



## ------------------------------------ ##
## Defining/requiring Autoconf macros.  ##
## ------------------------------------ ##


# AC_DEFUN(NAME, EXPANSION)
# AC_DEFUN_ONCE(NAME, EXPANSION)
# AC_BEFORE(THIS-MACRO-NAME, CALLED-MACRO-NAME)
# AC_REQUIRE(STRING)
# AC_PROVIDE(MACRO-NAME)
# AC_PROVIDE_IFELSE(MACRO-NAME, IF-PROVIDED, IF-NOT-PROVIDED)
# -----------------------------------------------------------
m4_copy([m4_defun],       [AC_DEFUN])
m4_copy([m4_defun_once],  [AC_DEFUN_ONCE])
m4_copy([m4_before],      [AC_BEFORE])
m4_copy([m4_require],     [AC_REQUIRE])
m4_copy([m4_provide],     [AC_PROVIDE])
m4_copy([m4_provide_if],  [AC_PROVIDE_IFELSE])


# AC_OBSOLETE(THIS-MACRO-NAME, [SUGGESTION])
# ------------------------------------------
m4_define([AC_OBSOLETE],
[AC_DIAGNOSE([obsolete], [$1 is obsolete$2])])



## ----------------------------- ##
## Implementing Autoconf loops.  ##
## ----------------------------- ##


# AC_FOREACH(VARIABLE, LIST, EXPRESSION)
# --------------------------------------
#
# Compute EXPRESSION assigning to VARIABLE each value of the LIST.
# LIST is a /bin/sh list, i.e., it has the form ` item_1 item_2
# ... item_n ': white spaces are separators, and leading and trailing
# spaces are meaningless.
#
# This macro is robust to active symbols:
#    AC_FOREACH([Var], [ active
#    b	act\
#    ive  ], [-Var-])end
#    => -active--b--active-end
m4_define([AC_FOREACH],
[m4_foreach([$1], m4_split(m4_normalize([$2])), [$3])])




## ----------------------------------- ##
## Helping macros to display strings.  ##
## ----------------------------------- ##


# AU::AC_HELP_STRING(LHS, RHS, [COLUMN])
# --------------------------------------
AU_ALIAS([AC_HELP_STRING], [AS_HELP_STRING])



## ---------------------------------------------- ##
## Information on the package being Autoconf'ed.  ##
## ---------------------------------------------- ##


# It is suggested that the macros in this section appear before
# AC_INIT in `configure.ac'.  Nevertheless, this is just stylistic,
# and from the implementation point of, AC_INIT *must* be expanded
# beforehand: it puts data in diversions which must appear before the
# data provided by the macros of this section.

# The solution is to require AC_INIT in each of these macros.  AC_INIT
# has the needed magic so that it can't be expanded twice.



# _AC_INIT_PACKAGE(PACKAGE-NAME, VERSION, BUG-REPORT, [TARNAME])
# --------------------------------------------------------------
m4_define([_AC_INIT_PACKAGE],
[AS_LITERAL_IF([$1], [], [m4_warn([syntax], [AC_INIT: not a literal: $1])])
AS_LITERAL_IF([$2], [],  [m4_warn([syntax], [AC_INIT: not a literal: $2])])
AS_LITERAL_IF([$3], [],  [m4_warn([syntax], [AC_INIT: not a literal: $3])])
m4_ifndef([AC_PACKAGE_NAME],
	  [m4_define([AC_PACKAGE_NAME],     [$1])])
m4_ifndef([AC_PACKAGE_TARNAME],
	  [m4_define([AC_PACKAGE_TARNAME],
		     m4_default([$4],
				[m4_bpatsubst(m4_tolower(m4_bpatsubst([[[$1]]],
								     [GNU ])),
				 [[^_abcdefghijklmnopqrstuvwxyz0123456789]],
				 [-])]))])
m4_ifndef([AC_PACKAGE_VERSION],
	  [m4_define([AC_PACKAGE_VERSION],   [$2])])
m4_ifndef([AC_PACKAGE_STRING],
	  [m4_define([AC_PACKAGE_STRING],    [$1 $2])])
m4_ifndef([AC_PACKAGE_BUGREPORT],
	  [m4_define([AC_PACKAGE_BUGREPORT], [$3])])
])


# AC_COPYRIGHT(TEXT, [VERSION-DIVERSION = VERSION_USER])
# ------------------------------------------------------
# Append Copyright information in the top of `configure'.  TEXT is
# evaluated once, hence TEXT can use macros.  Note that we do not
# prepend `# ' but `@%:@ ', since m4 does not evaluate the comments.
# Had we used `# ', the Copyright sent in the beginning of `configure'
# would have not been evaluated.  Another solution, a bit fragile,
# would have be to use m4_quote to force an evaluation:
#
#     m4_bpatsubst(m4_quote($1), [^], [# ])
m4_define([AC_COPYRIGHT],
[m4_divert_text([HEADER-COPYRIGHT],
[m4_bpatsubst([
$1], [^], [@%:@ ])])dnl
m4_divert_text(m4_default([$2], [VERSION_USER]),
[
$1])dnl
])# AC_COPYRIGHT


# AC_REVISION(REVISION-INFO)
# --------------------------
# The second quote in the translit is just to cope with font-lock-mode
# which sees the opening of a string.
m4_define([AC_REVISION],
[m4_divert_text([HEADER-REVISION],
		[@%:@ From __file__ m4_translit([$1], [$""]).])dnl
])




## ---------------------------------------- ##
## Requirements over the Autoconf version.  ##
## ---------------------------------------- ##


# AU::AC_PREREQ(VERSION)
# ----------------------
# Update this `AC_PREREQ' statement to require the current version of
# Autoconf.  But fail if ever this autoupdate is too old.
#
# Note that `m4_defn([m4_PACKAGE_VERSION])' below are expanded before
# calling `AU_DEFUN', i.e., it is hard coded.  Otherwise it would be
# quite complex for autoupdate to import the value of
# `m4_PACKAGE_VERSION'.  We could `AU_DEFUN' `m4_PACKAGE_VERSION', but
# this would replace all its occurrences with the current version of
# Autoconf, which is certainly not what the user intended.
AU_DEFUN([AC_PREREQ],
[m4_version_prereq([$1])[]dnl
[AC_PREREQ(]]m4_defn([m4_PACKAGE_VERSION])[[)]])


# AC_PREREQ(VERSION)
# ------------------
# Complain and exit if the Autoconf version is less than VERSION.
m4_copy([m4_version_prereq], [AC_PREREQ])






## ---------------- ##
## Initialization.  ##
## ---------------- ##


# All the following macros are used by AC_INIT.  Ideally, they should
# be presented in the order in which they are output.  Please, help us
# sorting it, or at least, don't augment the entropy.


# _AC_INIT_NOTICE
# ---------------
m4_define([_AC_INIT_NOTICE],
[m4_divert_text([HEADER-COMMENT],
[@%:@ Guess values for system-dependent variables and create Makefiles.
@%:@ Generated by m4_PACKAGE_STRING[]dnl
m4_ifset([AC_PACKAGE_STRING], [ for AC_PACKAGE_STRING]).])

m4_ifset([AC_PACKAGE_BUGREPORT],
	 [m4_divert_text([HEADER-COMMENT],
			 [@%:@
@%:@ Report bugs to <AC_PACKAGE_BUGREPORT>.])])
])


# _AC_INIT_COPYRIGHT
# ------------------
# We dump to VERSION_FSF to make sure we are inserted before the
# user copyrights, and after the setup of the --version handling.
m4_define([_AC_INIT_COPYRIGHT],
[AC_COPYRIGHT(
[Copyright (C) 2003 Free Software Foundation, Inc.
This configure script is free software; the Free Software Foundation
gives unlimited permission to copy, distribute and modify it.],
	      [VERSION_FSF])dnl
])


# File Descriptors
# ----------------
# Set up the file descriptors used by `configure'.
# File descriptor usage:
# 0 standard input
# 1 file creation
# 2 errors and warnings
# AS_MESSAGE_LOG_FD compiler messages saved in config.log
# AS_MESSAGE_FD checking for... messages and results

m4_define([AS_MESSAGE_FD], 6)
# That's how they used to be named.
AU_ALIAS([AC_FD_CC],  [AS_MESSAGE_LOG_FD])
AU_ALIAS([AC_FD_MSG], [AS_MESSAGE_FD])


# _AC_INIT_DEFAULTS
# -----------------
# Values which defaults can be set from `configure.ac'.
# `/bin/machine' is used in `glibcbug'.  The others are used in config.*
m4_define([_AC_INIT_DEFAULTS],
[m4_divert_push([DEFAULTS])dnl

# Name of the host.
# hostname on some systems (SVR3.2, Linux) returns a bogus exit status,
# so uname gets run too.
ac_hostname=`(hostname || uname -n) 2>/dev/null | sed 1q`

exec AS_MESSAGE_FD>&1

#
# Initializations.
#
ac_default_prefix=/usr/local
ac_config_libobj_dir=.
cross_compiling=no
subdirs=
MFLAGS=
MAKEFLAGS=
AC_SUBST([SHELL], [${CONFIG_SHELL-/bin/sh}])dnl
AC_SUBST([PATH_SEPARATOR])dnl

# Maximum number of lines to put in a shell here document.
# This variable seems obsolete.  It should probably be removed, and
# only ac_max_sed_lines should be used.
: ${ac_max_here_lines=38}

# Identity of this package.
AC_SUBST([PACKAGE_NAME],
	 [m4_ifdef([AC_PACKAGE_NAME],      ['AC_PACKAGE_NAME'])])dnl
AC_SUBST([PACKAGE_TARNAME],
	 [m4_ifdef([AC_PACKAGE_TARNAME],   ['AC_PACKAGE_TARNAME'])])dnl
AC_SUBST([PACKAGE_VERSION],
	 [m4_ifdef([AC_PACKAGE_VERSION],   ['AC_PACKAGE_VERSION'])])dnl
AC_SUBST([PACKAGE_STRING],
	 [m4_ifdef([AC_PACKAGE_STRING],    ['AC_PACKAGE_STRING'])])dnl
AC_SUBST([PACKAGE_BUGREPORT],
	 [m4_ifdef([AC_PACKAGE_BUGREPORT], ['AC_PACKAGE_BUGREPORT'])])dnl

m4_divert_pop([DEFAULTS])dnl
m4_wrap([m4_divert_text([DEFAULTS],
[ac_subst_vars='m4_ifdef([_AC_SUBST_VARS],  [m4_defn([_AC_SUBST_VARS])])'
ac_subst_files='m4_ifdef([_AC_SUBST_FILES], [m4_defn([_AC_SUBST_FILES])])'])])dnl
])# _AC_INIT_DEFAULTS


# AC_PREFIX_DEFAULT(PREFIX)
# -------------------------
AC_DEFUN([AC_PREFIX_DEFAULT],
[m4_divert_text([DEFAULTS], [ac_default_prefix=$1])])


# AC_PREFIX_PROGRAM(PROGRAM)
# --------------------------
# Guess the value for the `prefix' variable by looking for
# the argument program along PATH and taking its parent.
# Example: if the argument is `gcc' and we find /usr/local/gnu/bin/gcc,
# set `prefix' to /usr/local/gnu.
# This comes too late to find a site file based on the prefix,
# and it might use a cached value for the path.
# No big loss, I think, since most configures don't use this macro anyway.
AC_DEFUN([AC_PREFIX_PROGRAM],
[if test "x$prefix" = xNONE; then
dnl We reimplement AC_MSG_CHECKING (mostly) to avoid the ... in the middle.
  _AS_ECHO_N([checking for prefix by ])
  AC_PATH_PROG(ac_prefix_program, [$1])
  if test -n "$ac_prefix_program"; then
    prefix=`AS_DIRNAME(["$ac_prefix_program"])`
    prefix=`AS_DIRNAME(["$prefix"])`
  fi
fi
])# AC_PREFIX_PROGRAM


# AC_CONFIG_SRCDIR([UNIQUE-FILE-IN-SOURCE-DIR])
# ---------------------------------------------
# UNIQUE-FILE-IN-SOURCE-DIR is a filename unique to this package,
# relative to the directory that configure is in, which we can look
# for to find out if srcdir is correct.
AC_DEFUN([AC_CONFIG_SRCDIR],
[m4_divert_text([DEFAULTS], [ac_unique_file="$1"])])


# _AC_INIT_SRCDIR
# ---------------
# Compute `srcdir' based on `$ac_unique_file'.
m4_define([_AC_INIT_SRCDIR],
[m4_divert_push([PARSE_ARGS])dnl

# Find the source files, if location was not specified.
if test -z "$srcdir"; then
  ac_srcdir_defaulted=yes
  # Try the directory containing this script, then its parent.
  ac_confdir=`AS_DIRNAME(["$[0]"])`
  srcdir=$ac_confdir
  if test ! -r $srcdir/$ac_unique_file; then
    srcdir=..
  fi
else
  ac_srcdir_defaulted=no
fi
if test ! -r $srcdir/$ac_unique_file; then
  if test "$ac_srcdir_defaulted" = yes; then
    AC_MSG_ERROR([cannot find sources ($ac_unique_file) in $ac_confdir or ..])
  else
    AC_MSG_ERROR([cannot find sources ($ac_unique_file) in $srcdir])
  fi
fi
(cd $srcdir && test -r ./$ac_unique_file) 2>/dev/null ||
  AC_MSG_ERROR([sources are in $srcdir, but `cd $srcdir' does not work])
dnl Double slashes in pathnames in object file debugging info
dnl mess up M-x gdb in Emacs.
srcdir=`echo "$srcdir" | sed 's%\([[^\\/]]\)[[\\/]]*$%\1%'`
m4_divert_pop([PARSE_ARGS])dnl
])# _AC_INIT_SRCDIR


# _AC_INIT_PARSE_ARGS
# -------------------
m4_define([_AC_INIT_PARSE_ARGS],
[m4_divert_push([PARSE_ARGS])dnl

# Initialize some variables set by options.
ac_init_help=
ac_init_version=false
# The variables have the same names as the options, with
# dashes changed to underlines.
cache_file=/dev/null
AC_SUBST(exec_prefix, NONE)dnl
no_create=
no_recursion=
AC_SUBST(prefix, NONE)dnl
program_prefix=NONE
program_suffix=NONE
AC_SUBST(program_transform_name, [s,x,x,])dnl
silent=
site=
srcdir=
verbose=
x_includes=NONE
x_libraries=NONE

# Installation directory options.
# These are left unexpanded so users can "make install exec_prefix=/foo"
# and all the variables that are supposed to be based on exec_prefix
# by default will actually change.
# Use braces instead of parens because sh, perl, etc. also accept them.
AC_SUBST([bindir],         ['${exec_prefix}/bin'])dnl
AC_SUBST([sbindir],        ['${exec_prefix}/sbin'])dnl
AC_SUBST([libexecdir],     ['${exec_prefix}/libexec'])dnl
AC_SUBST([datadir],        ['${prefix}/share'])dnl
AC_SUBST([sysconfdir],     ['${prefix}/etc'])dnl
AC_SUBST([sharedstatedir], ['${prefix}/com'])dnl
AC_SUBST([localstatedir],  ['${prefix}/var'])dnl
AC_SUBST([libdir],         ['${exec_prefix}/lib'])dnl
AC_SUBST([includedir],     ['${prefix}/include'])dnl
AC_SUBST([oldincludedir],  ['/usr/include'])dnl
AC_SUBST([infodir],        ['${prefix}/info'])dnl
AC_SUBST([mandir],         ['${prefix}/man'])dnl

ac_prev=
for ac_option
do
  # If the previous option needs an argument, assign it.
  if test -n "$ac_prev"; then
    eval "$ac_prev=\$ac_option"
    ac_prev=
    continue
  fi

  ac_optarg=`expr "x$ac_option" : 'x[[^=]]*=\(.*\)'`

  # Accept the important Cygnus configure options, so we can diagnose typos.

  case $ac_option in

  -bindir | --bindir | --bindi | --bind | --bin | --bi)
    ac_prev=bindir ;;
  -bindir=* | --bindir=* | --bindi=* | --bind=* | --bin=* | --bi=*)
    bindir=$ac_optarg ;;

  -build | --build | --buil | --bui | --bu)
    ac_prev=build_alias ;;
  -build=* | --build=* | --buil=* | --bui=* | --bu=*)
    build_alias=$ac_optarg ;;

  -cache-file | --cache-file | --cache-fil | --cache-fi \
  | --cache-f | --cache- | --cache | --cach | --cac | --ca | --c)
    ac_prev=cache_file ;;
  -cache-file=* | --cache-file=* | --cache-fil=* | --cache-fi=* \
  | --cache-f=* | --cache-=* | --cache=* | --cach=* | --cac=* | --ca=* | --c=*)
    cache_file=$ac_optarg ;;

  --config-cache | -C)
    cache_file=config.cache ;;

  -datadir | --datadir | --datadi | --datad | --data | --dat | --da)
    ac_prev=datadir ;;
  -datadir=* | --datadir=* | --datadi=* | --datad=* | --data=* | --dat=* \
  | --da=*)
    datadir=$ac_optarg ;;

  -disable-* | --disable-*)
    ac_feature=`expr "x$ac_option" : 'x-*disable-\(.*\)'`
    # Reject names that are not valid shell variable names.
    expr "x$ac_feature" : "[.*[^-_$as_cr_alnum]]" >/dev/null &&
      AC_MSG_ERROR([invalid feature name: $ac_feature])
    ac_feature=`echo $ac_feature | sed 's/-/_/g'`
    eval "enable_$ac_feature=no" ;;

  -enable-* | --enable-*)
    ac_feature=`expr "x$ac_option" : 'x-*enable-\([[^=]]*\)'`
    # Reject names that are not valid shell variable names.
    expr "x$ac_feature" : "[.*[^-_$as_cr_alnum]]" >/dev/null &&
      AC_MSG_ERROR([invalid feature name: $ac_feature])
    ac_feature=`echo $ac_feature | sed 's/-/_/g'`
    case $ac_option in
      *=*) ac_optarg=`echo "$ac_optarg" | sed "s/'/'\\\\\\\\''/g"`;;
      *) ac_optarg=yes ;;
    esac
    eval "enable_$ac_feature='$ac_optarg'" ;;

  -exec-prefix | --exec_prefix | --exec-prefix | --exec-prefi \
  | --exec-pref | --exec-pre | --exec-pr | --exec-p | --exec- \
  | --exec | --exe | --ex)
    ac_prev=exec_prefix ;;
  -exec-prefix=* | --exec_prefix=* | --exec-prefix=* | --exec-prefi=* \
  | --exec-pref=* | --exec-pre=* | --exec-pr=* | --exec-p=* | --exec-=* \
  | --exec=* | --exe=* | --ex=*)
    exec_prefix=$ac_optarg ;;

  -gas | --gas | --ga | --g)
    # Obsolete; use --with-gas.
    with_gas=yes ;;

  -help | --help | --hel | --he | -h)
    ac_init_help=long ;;
  -help=r* | --help=r* | --hel=r* | --he=r* | -hr*)
    ac_init_help=recursive ;;
  -help=s* | --help=s* | --hel=s* | --he=s* | -hs*)
    ac_init_help=short ;;

  -host | --host | --hos | --ho)
    ac_prev=host_alias ;;
  -host=* | --host=* | --hos=* | --ho=*)
    host_alias=$ac_optarg ;;

  -includedir | --includedir | --includedi | --included | --include \
  | --includ | --inclu | --incl | --inc)
    ac_prev=includedir ;;
  -includedir=* | --includedir=* | --includedi=* | --included=* | --include=* \
  | --includ=* | --inclu=* | --incl=* | --inc=*)
    includedir=$ac_optarg ;;

  -infodir | --infodir | --infodi | --infod | --info | --inf)
    ac_prev=infodir ;;
  -infodir=* | --infodir=* | --infodi=* | --infod=* | --info=* | --inf=*)
    infodir=$ac_optarg ;;

  -libdir | --libdir | --libdi | --libd)
    ac_prev=libdir ;;
  -libdir=* | --libdir=* | --libdi=* | --libd=*)
    libdir=$ac_optarg ;;

  -libexecdir | --libexecdir | --libexecdi | --libexecd | --libexec \
  | --libexe | --libex | --libe)
    ac_prev=libexecdir ;;
  -libexecdir=* | --libexecdir=* | --libexecdi=* | --libexecd=* | --libexec=* \
  | --libexe=* | --libex=* | --libe=*)
    libexecdir=$ac_optarg ;;

  -localstatedir | --localstatedir | --localstatedi | --localstated \
  | --localstate | --localstat | --localsta | --localst \
  | --locals | --local | --loca | --loc | --lo)
    ac_prev=localstatedir ;;
  -localstatedir=* | --localstatedir=* | --localstatedi=* | --localstated=* \
  | --localstate=* | --localstat=* | --localsta=* | --localst=* \
  | --locals=* | --local=* | --loca=* | --loc=* | --lo=*)
    localstatedir=$ac_optarg ;;

  -mandir | --mandir | --mandi | --mand | --man | --ma | --m)
    ac_prev=mandir ;;
  -mandir=* | --mandir=* | --mandi=* | --mand=* | --man=* | --ma=* | --m=*)
    mandir=$ac_optarg ;;

  -nfp | --nfp | --nf)
    # Obsolete; use --without-fp.
    with_fp=no ;;

  -no-create | --no-create | --no-creat | --no-crea | --no-cre \
  | --no-cr | --no-c | -n)
    no_create=yes ;;

  -no-recursion | --no-recursion | --no-recursio | --no-recursi \
  | --no-recurs | --no-recur | --no-recu | --no-rec | --no-re | --no-r)
    no_recursion=yes ;;

  -oldincludedir | --oldincludedir | --oldincludedi | --oldincluded \
  | --oldinclude | --oldinclud | --oldinclu | --oldincl | --oldinc \
  | --oldin | --oldi | --old | --ol | --o)
    ac_prev=oldincludedir ;;
  -oldincludedir=* | --oldincludedir=* | --oldincludedi=* | --oldincluded=* \
  | --oldinclude=* | --oldinclud=* | --oldinclu=* | --oldincl=* | --oldinc=* \
  | --oldin=* | --oldi=* | --old=* | --ol=* | --o=*)
    oldincludedir=$ac_optarg ;;

  -prefix | --prefix | --prefi | --pref | --pre | --pr | --p)
    ac_prev=prefix ;;
  -prefix=* | --prefix=* | --prefi=* | --pref=* | --pre=* | --pr=* | --p=*)
    prefix=$ac_optarg ;;

  -program-prefix | --program-prefix | --program-prefi | --program-pref \
  | --program-pre | --program-pr | --program-p)
    ac_prev=program_prefix ;;
  -program-prefix=* | --program-prefix=* | --program-prefi=* \
  | --program-pref=* | --program-pre=* | --program-pr=* | --program-p=*)
    program_prefix=$ac_optarg ;;

  -program-suffix | --program-suffix | --program-suffi | --program-suff \
  | --program-suf | --program-su | --program-s)
    ac_prev=program_suffix ;;
  -program-suffix=* | --program-suffix=* | --program-suffi=* \
  | --program-suff=* | --program-suf=* | --program-su=* | --program-s=*)
    program_suffix=$ac_optarg ;;

  -program-transform-name | --program-transform-name \
  | --program-transform-nam | --program-transform-na \
  | --program-transform-n | --program-transform- \
  | --program-transform | --program-transfor \
  | --program-transfo | --program-transf \
  | --program-trans | --program-tran \
  | --progr-tra | --program-tr | --program-t)
    ac_prev=program_transform_name ;;
  -program-transform-name=* | --program-transform-name=* \
  | --program-transform-nam=* | --program-transform-na=* \
  | --program-transform-n=* | --program-transform-=* \
  | --program-transform=* | --program-transfor=* \
  | --program-transfo=* | --program-transf=* \
  | --program-trans=* | --program-tran=* \
  | --progr-tra=* | --program-tr=* | --program-t=*)
    program_transform_name=$ac_optarg ;;

  -q | -quiet | --quiet | --quie | --qui | --qu | --q \
  | -silent | --silent | --silen | --sile | --sil)
    silent=yes ;;

  -sbindir | --sbindir | --sbindi | --sbind | --sbin | --sbi | --sb)
    ac_prev=sbindir ;;
  -sbindir=* | --sbindir=* | --sbindi=* | --sbind=* | --sbin=* \
  | --sbi=* | --sb=*)
    sbindir=$ac_optarg ;;

  -sharedstatedir | --sharedstatedir | --sharedstatedi \
  | --sharedstated | --sharedstate | --sharedstat | --sharedsta \
  | --sharedst | --shareds | --shared | --share | --shar \
  | --sha | --sh)
    ac_prev=sharedstatedir ;;
  -sharedstatedir=* | --sharedstatedir=* | --sharedstatedi=* \
  | --sharedstated=* | --sharedstate=* | --sharedstat=* | --sharedsta=* \
  | --sharedst=* | --shareds=* | --shared=* | --share=* | --shar=* \
  | --sha=* | --sh=*)
    sharedstatedir=$ac_optarg ;;

  -site | --site | --sit)
    ac_prev=site ;;
  -site=* | --site=* | --sit=*)
    site=$ac_optarg ;;

  -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
    ac_prev=srcdir ;;
  -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
    srcdir=$ac_optarg ;;

  -sysconfdir | --sysconfdir | --sysconfdi | --sysconfd | --sysconf \
  | --syscon | --sysco | --sysc | --sys | --sy)
    ac_prev=sysconfdir ;;
  -sysconfdir=* | --sysconfdir=* | --sysconfdi=* | --sysconfd=* | --sysconf=* \
  | --syscon=* | --sysco=* | --sysc=* | --sys=* | --sy=*)
    sysconfdir=$ac_optarg ;;

  -target | --target | --targe | --targ | --tar | --ta | --t)
    ac_prev=target_alias ;;
  -target=* | --target=* | --targe=* | --targ=* | --tar=* | --ta=* | --t=*)
    target_alias=$ac_optarg ;;

  -v | -verbose | --verbose | --verbos | --verbo | --verb)
    verbose=yes ;;

  -version | --version | --versio | --versi | --vers | -V)
    ac_init_version=: ;;

  -with-* | --with-*)
    ac_package=`expr "x$ac_option" : 'x-*with-\([[^=]]*\)'`
    # Reject names that are not valid shell variable names.
    expr "x$ac_package" : "[.*[^-_$as_cr_alnum]]" >/dev/null &&
      AC_MSG_ERROR([invalid package name: $ac_package])
    ac_package=`echo $ac_package| sed 's/-/_/g'`
    case $ac_option in
      *=*) ac_optarg=`echo "$ac_optarg" | sed "s/'/'\\\\\\\\''/g"`;;
      *) ac_optarg=yes ;;
    esac
    eval "with_$ac_package='$ac_optarg'" ;;

  -without-* | --without-*)
    ac_package=`expr "x$ac_option" : 'x-*without-\(.*\)'`
    # Reject names that are not valid shell variable names.
    expr "x$ac_package" : "[.*[^-_$as_cr_alnum]]" >/dev/null &&
      AC_MSG_ERROR([invalid package name: $ac_package])
    ac_package=`echo $ac_package | sed 's/-/_/g'`
    eval "with_$ac_package=no" ;;

  --x)
    # Obsolete; use --with-x.
    with_x=yes ;;

  -x-includes | --x-includes | --x-include | --x-includ | --x-inclu \
  | --x-incl | --x-inc | --x-in | --x-i)
    ac_prev=x_includes ;;
  -x-includes=* | --x-includes=* | --x-include=* | --x-includ=* | --x-inclu=* \
  | --x-incl=* | --x-inc=* | --x-in=* | --x-i=*)
    x_includes=$ac_optarg ;;

  -x-libraries | --x-libraries | --x-librarie | --x-librari \
  | --x-librar | --x-libra | --x-libr | --x-lib | --x-li | --x-l)
    ac_prev=x_libraries ;;
  -x-libraries=* | --x-libraries=* | --x-librarie=* | --x-librari=* \
  | --x-librar=* | --x-libra=* | --x-libr=* | --x-lib=* | --x-li=* | --x-l=*)
    x_libraries=$ac_optarg ;;

  -*) AC_MSG_ERROR([unrecognized option: $ac_option
Try `$[0] --help' for more information.])
    ;;

  *=*)
    ac_envvar=`expr "x$ac_option" : 'x\([[^=]]*\)='`
    # Reject names that are not valid shell variable names.
    expr "x$ac_envvar" : "[.*[^_$as_cr_alnum]]" >/dev/null &&
      AC_MSG_ERROR([invalid variable name: $ac_envvar])
    ac_optarg=`echo "$ac_optarg" | sed "s/'/'\\\\\\\\''/g"`
    eval "$ac_envvar='$ac_optarg'"
    export $ac_envvar ;;

  *)
    # FIXME: should be removed in autoconf 3.0.
    AC_MSG_WARN([you should use --build, --host, --target])
    expr "x$ac_option" : "[.*[^-._$as_cr_alnum]]" >/dev/null &&
      AC_MSG_WARN([invalid host type: $ac_option])
    : ${build_alias=$ac_option} ${host_alias=$ac_option} ${target_alias=$ac_option}
    ;;

  esac
done

if test -n "$ac_prev"; then
  ac_option=--`echo $ac_prev | sed 's/_/-/g'`
  AC_MSG_ERROR([missing argument to $ac_option])
fi

# Be sure to have absolute paths.
for ac_var in exec_prefix prefix
do
  eval ac_val=$`echo $ac_var`
  case $ac_val in
    [[\\/$]]* | ?:[[\\/]]* | NONE | '' ) ;;
    *)  AC_MSG_ERROR([expected an absolute directory name for --$ac_var: $ac_val]);;
  esac
done

# Be sure to have absolute paths.
for ac_var in bindir sbindir libexecdir datadir sysconfdir sharedstatedir \
	      localstatedir libdir includedir oldincludedir infodir mandir
do
  eval ac_val=$`echo $ac_var`
  case $ac_val in
    [[\\/$]]* | ?:[[\\/]]* ) ;;
    *)  AC_MSG_ERROR([expected an absolute directory name for --$ac_var: $ac_val]);;
  esac
done

# There might be people who depend on the old broken behavior: `$host'
# used to hold the argument of --host etc.
# FIXME: To remove some day.
build=$build_alias
host=$host_alias
target=$target_alias

# FIXME: To remove some day.
if test "x$host_alias" != x; then
  if test "x$build_alias" = x; then
    cross_compiling=maybe
    AC_MSG_WARN([If you wanted to set the --build type, don't use --host.
    If a cross compiler is detected then cross compile mode will be used.])
  elif test "x$build_alias" != "x$host_alias"; then
    cross_compiling=yes
  fi
fi

ac_tool_prefix=
test -n "$host_alias" && ac_tool_prefix=$host_alias-

test "$silent" = yes && exec AS_MESSAGE_FD>/dev/null

m4_divert_pop([PARSE_ARGS])dnl
])# _AC_INIT_PARSE_ARGS


# _AC_INIT_HELP
# -------------
# Handle the `configure --help' message.
m4_define([_AC_INIT_HELP],
[m4_divert_push([HELP_BEGIN])dnl

#
# Report the --help message.
#
if test "$ac_init_help" = "long"; then
  # Omit some internal or obsolete options to make the list less imposing.
  # This message is too long to be a string in the A/UX 3.1 sh.
  cat <<_ACEOF
\`configure' configures m4_ifset([AC_PACKAGE_STRING],
			[AC_PACKAGE_STRING],
			[this package]) to adapt to many kinds of systems.

Usage: $[0] [[OPTION]]... [[VAR=VALUE]]...

[To assign environment variables (e.g., CC, CFLAGS...), specify them as
VAR=VALUE.  See below for descriptions of some of the useful variables.

Defaults for the options are specified in brackets.

Configuration:
  -h, --help              display this help and exit
      --help=short        display options specific to this package
      --help=recursive    display the short help of all the included packages
  -V, --version           display version information and exit
  -q, --quiet, --silent   do not print \`checking...' messages
      --cache-file=FILE   cache test results in FILE [disabled]
  -C, --config-cache      alias for \`--cache-file=config.cache'
  -n, --no-create         do not create output files
      --srcdir=DIR        find the sources in DIR [configure dir or \`..']

_ACEOF

  cat <<_ACEOF
Installation directories:
  --prefix=PREFIX         install architecture-independent files in PREFIX
			  [$ac_default_prefix]
  --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX
			  [PREFIX]

By default, \`make install' will install all the files in
\`$ac_default_prefix/bin', \`$ac_default_prefix/lib' etc.  You can specify
an installation prefix other than \`$ac_default_prefix' using \`--prefix',
for instance \`--prefix=\$HOME'.

For better control, use the options below.

Fine tuning of the installation directories:
  --bindir=DIR           user executables [EPREFIX/bin]
  --sbindir=DIR          system admin executables [EPREFIX/sbin]
  --libexecdir=DIR       program executables [EPREFIX/libexec]
  --datadir=DIR          read-only architecture-independent data [PREFIX/share]
  --sysconfdir=DIR       read-only single-machine data [PREFIX/etc]
  --sharedstatedir=DIR   modifiable architecture-independent data [PREFIX/com]
  --localstatedir=DIR    modifiable single-machine data [PREFIX/var]
  --libdir=DIR           object code libraries [EPREFIX/lib]
  --includedir=DIR       C header files [PREFIX/include]
  --oldincludedir=DIR    C header files for non-gcc [/usr/include]
  --infodir=DIR          info documentation [PREFIX/info]
  --mandir=DIR           man documentation [PREFIX/man]
_ACEOF

  cat <<\_ACEOF]
m4_divert_pop([HELP_BEGIN])dnl
dnl The order of the diversions here is
dnl - HELP_BEGIN
dnl   which may be extended by extra generic options such as with X or
dnl   AC_ARG_PROGRAM.  Displayed only in long --help.
dnl
dnl - HELP_CANON
dnl   Support for cross compilation (--build, --host and --target).
dnl   Display only in long --help.
dnl
dnl - HELP_ENABLE
dnl   which starts with the trailer of the HELP_BEGIN, HELP_CANON section,
dnl   then implements the header of the non generic options.
dnl
dnl - HELP_WITH
dnl
dnl - HELP_VAR
dnl
dnl - HELP_VAR_END
dnl
dnl - HELP_END
dnl   initialized below, in which we dump the trailer (handling of the
dnl   recursion for instance).
m4_divert_push([HELP_ENABLE])dnl
_ACEOF
fi

if test -n "$ac_init_help"; then
m4_ifset([AC_PACKAGE_STRING],
[  case $ac_init_help in
     short | recursive ) echo "Configuration of AC_PACKAGE_STRING:";;
   esac])
  cat <<\_ACEOF
m4_divert_pop([HELP_ENABLE])dnl
m4_divert_push([HELP_END])dnl
m4_ifset([AC_PACKAGE_BUGREPORT], [
Report bugs to <AC_PACKAGE_BUGREPORT>.])
_ACEOF
fi

if test "$ac_init_help" = "recursive"; then
  # If there are subdirs, report their specific --help.
  ac_popdir=`pwd`
  for ac_dir in : $ac_subdirs_all; do test "x$ac_dir" = x: && continue
    test -d $ac_dir || continue
    _AC_SRCPATHS(["$ac_dir"])
    cd $ac_dir
    # Check for guested configure; otherwise get Cygnus style configure.
    if test -f $ac_srcdir/configure.gnu; then
      echo
      $SHELL $ac_srcdir/configure.gnu  --help=recursive
    elif test -f $ac_srcdir/configure; then
      echo
      $SHELL $ac_srcdir/configure  --help=recursive
    elif test -f $ac_srcdir/configure.ac ||
	   test -f $ac_srcdir/configure.in; then
      echo
      $ac_configure --help
    else
      AC_MSG_WARN([no configuration information is in $ac_dir])
    fi
    cd "$ac_popdir"
  done
fi

test -n "$ac_init_help" && exit 0
m4_divert_pop([HELP_END])dnl
])# _AC_INIT_HELP


# _AC_INIT_VERSION
# ----------------
# Handle the `configure --version' message.
m4_define([_AC_INIT_VERSION],
[m4_divert_text([VERSION_BEGIN],
[if $ac_init_version; then
  cat <<\_ACEOF])dnl
m4_ifset([AC_PACKAGE_STRING],
	 [m4_divert_text([VERSION_BEGIN],
			 [dnl
m4_ifset([AC_PACKAGE_NAME], [AC_PACKAGE_NAME ])configure[]dnl
m4_ifset([AC_PACKAGE_VERSION], [ AC_PACKAGE_VERSION])
generated by m4_PACKAGE_STRING])])
m4_divert_text([VERSION_END],
[_ACEOF
  exit 0
fi])dnl
])# _AC_INIT_VERSION


# _AC_INIT_CONFIG_LOG
# -------------------
# Initialize the config.log file descriptor and write header to it.
m4_define([_AC_INIT_CONFIG_LOG],
[m4_divert_text([INIT_PREPARE],
[m4_define([AS_MESSAGE_LOG_FD], 5)dnl
exec AS_MESSAGE_LOG_FD>config.log
cat >&AS_MESSAGE_LOG_FD <<_ACEOF
This file contains any messages produced by compilers while
running configure, to aid debugging if configure makes a mistake.

It was created by m4_ifset([AC_PACKAGE_NAME], [AC_PACKAGE_NAME ])dnl
$as_me[]m4_ifset([AC_PACKAGE_VERSION], [ AC_PACKAGE_VERSION]), which was
generated by m4_PACKAGE_STRING.  Invocation command line was

  $ $[0] $[@]

_ACEOF
AS_UNAME >&AS_MESSAGE_LOG_FD

cat >&AS_MESSAGE_LOG_FD <<_ACEOF


m4_text_box([Core tests.])

_ACEOF
])])# _AC_INIT_CONFIG_LOG


# _AC_INIT_PREPARE
# ----------------
# Called by AC_INIT to build the preamble of the `configure' scripts.
# 1. Trap and clean up various tmp files.
# 2. Set up the fd and output files
# 3. Remember the options given to `configure' for `config.status --recheck'.
# 4. Ensure a correct environment
# 5. Required macros (cache, default AC_SUBST etc.)
m4_define([_AC_INIT_PREPARE],
[m4_divert_push([INIT_PREPARE])dnl

# Keep a trace of the command line.
# Strip out --no-create and --no-recursion so they do not pile up.
# Strip out --silent because we don't want to record it for future runs.
# Also quote any args containing shell meta-characters.
# Make two passes to allow for proper duplicate-argument suppression.
ac_configure_args=
ac_configure_args0=
ac_configure_args1=
ac_sep=
ac_must_keep_next=false
for ac_pass in 1 2
do
  for ac_arg
  do
    case $ac_arg in
    -no-create | --no-c* | -n | -no-recursion | --no-r*) continue ;;
    -q | -quiet | --quiet | --quie | --qui | --qu | --q \
    | -silent | --silent | --silen | --sile | --sil)
      continue ;;
dnl If you change this globbing pattern, test it on an old shell --
dnl it's sensitive.  Putting any kind of quote in it causes syntax errors.
  [  *" "*|*"	"*|*[\[\]\~\#\$\^\&\*\(\)\{\}\\\|\;\<\>\?\"\']*)]
      ac_arg=`echo "$ac_arg" | sed "s/'/'\\\\\\\\''/g"` ;;
    esac
    case $ac_pass in
    1) ac_configure_args0="$ac_configure_args0 '$ac_arg'" ;;
    2)
      ac_configure_args1="$ac_configure_args1 '$ac_arg'"
dnl If trying to remove duplicates, be sure to (i) keep the *last*
dnl value (e.g. --prefix=1 --prefix=2 --prefix=1 might keep 2 only),
dnl and (ii) not to strip long options (--prefix foo --prefix bar might
dnl give --prefix foo bar).
      if test $ac_must_keep_next = true; then
	ac_must_keep_next=false # Got value, back to normal.
      else
	case $ac_arg in
dnl Use broad patterns, as arguments that would have already made configure
dnl exit don't matter.
	  *=* | --config-cache | -C | -disable-* | --disable-* \
	  | -enable-* | --enable-* | -gas | --g* | -nfp | --nf* \
	  | -q | -quiet | --q* | -silent | --sil* | -v | -verb* \
	  | -with-* | --with-* | -without-* | --without-* | --x)
	    case "$ac_configure_args0 " in
	      "$ac_configure_args1"*" '$ac_arg' "* ) continue ;;
	    esac
	    ;;
	  -* ) ac_must_keep_next=true ;;
	esac
      fi
      ac_configure_args="$ac_configure_args$ac_sep'$ac_arg'"
      # Get rid of the leading space.
      ac_sep=" "
      ;;
    esac
  done
done
AS_UNSET(ac_configure_args0)
AS_UNSET(ac_configure_args1)

# When interrupted or exit'd, cleanup temporary files, and complete
# config.log.  We remove comments because anyway the quotes in there
# would cause problems or look ugly.
# WARNING: Be sure not to use single quotes in there, as some shells,
# such as our DU 5.0 friend, will then `close' the trap.
trap 'exit_status=$?
  # Save into config.log some information that might help in debugging.
  {
    echo

    AS_BOX([Cache variables.])
    echo
    m4_bpatsubsts(m4_defn([_AC_CACHE_DUMP]),
		  [^ *\(#.*\)?
],                [],
		  ['], ['"'"'])
    echo

    AS_BOX([Output variables.])
    echo
    for ac_var in $ac_subst_vars
    do
      eval ac_val=$`echo $ac_var`
      echo "$ac_var='"'"'$ac_val'"'"'"
    done | sort
    echo

    if test -n "$ac_subst_files"; then
      AS_BOX([Output files.])
      echo
      for ac_var in $ac_subst_files
      do
	eval ac_val=$`echo $ac_var`
	echo "$ac_var='"'"'$ac_val'"'"'"
      done | sort
      echo
    fi

    if test -s confdefs.h; then
      AS_BOX([confdefs.h.])
      echo
      sed "/^$/d" confdefs.h | sort
      echo
    fi
    test "$ac_signal" != 0 &&
      echo "$as_me: caught signal $ac_signal"
    echo "$as_me: exit $exit_status"
  } >&AS_MESSAGE_LOG_FD
  rm -f core *.core &&
  rm -rf conftest* confdefs* conf$[$]* $ac_clean_files &&
    exit $exit_status
     ' 0
for ac_signal in 1 2 13 15; do
  trap 'ac_signal='$ac_signal'; AS_EXIT([1])' $ac_signal
done
ac_signal=0

# confdefs.h avoids OS command line length limits that DEFS can exceed.
rm -rf conftest* confdefs.h
# AIX cpp loses on an empty file, so make sure it contains at least a newline.
echo >confdefs.h

# Predefined preprocessor variables.
AC_DEFINE_UNQUOTED([PACKAGE_NAME], ["$PACKAGE_NAME"],
		   [Define to the full name of this package.])
AC_DEFINE_UNQUOTED([PACKAGE_TARNAME], ["$PACKAGE_TARNAME"],
		   [Define to the one symbol short name of this package.])
AC_DEFINE_UNQUOTED([PACKAGE_VERSION], ["$PACKAGE_VERSION"],
		   [Define to the version of this package.])
AC_DEFINE_UNQUOTED([PACKAGE_STRING], ["$PACKAGE_STRING"],
		   [Define to the full name and version of this package.])
AC_DEFINE_UNQUOTED([PACKAGE_BUGREPORT], ["$PACKAGE_BUGREPORT"],
		   [Define to the address where bug reports for this package
		    should be sent.])

# Let the site file select an alternate cache file if it wants to.
AC_SITE_LOAD
AC_CACHE_LOAD
_AC_ARG_VAR_VALIDATE
_AC_ARG_VAR_PRECIOUS([build_alias])dnl
_AC_ARG_VAR_PRECIOUS([host_alias])dnl
_AC_ARG_VAR_PRECIOUS([target_alias])dnl
AC_LANG_PUSH(C)

dnl Substitute for predefined variables.
AC_SUBST([DEFS])dnl
AC_SUBST([ECHO_C])dnl
AC_SUBST([ECHO_N])dnl
AC_SUBST([ECHO_T])dnl
AC_SUBST([LIBS])dnl
m4_divert_pop([INIT_PREPARE])dnl
])# _AC_INIT_PREPARE


# AU::AC_INIT([UNIQUE-FILE-IN-SOURCE-DIR])
# ----------------------------------------
# This macro is used only for Autoupdate.
AU_DEFUN([AC_INIT],
[m4_ifval([$2], [[AC_INIT($@)]],
	  [m4_ifval([$1],
[[AC_INIT]
AC_CONFIG_SRCDIR([$1])], [[AC_INIT]])])[]dnl
])


# AC_INIT([PACKAGE, VERSION, [BUG-REPORT])
# ----------------------------------------
# Include the user macro files, prepare the diversions, and output the
# preamble of the `configure' script.
# Note that the order is important: first initialize, then set the
# AC_CONFIG_SRCDIR.
m4_define([AC_INIT],
[# Forbidden tokens and exceptions.
m4_pattern_forbid([^_?A[CHUM]_])
m4_pattern_forbid([_AC_])
m4_pattern_forbid([^LIBOBJS$],
		  [do not use LIBOBJS directly, use AC_LIBOBJ (see section `AC_LIBOBJ vs LIBOBJS'])
# Actually reserved by M4sh.
m4_pattern_allow([^AS_FLAGS$])
AS_INIT
AS_PREPARE
m4_ifval([$2], [_AC_INIT_PACKAGE($@)])
_AC_INIT_DEFAULTS
_AC_INIT_PARSE_ARGS
_AC_INIT_SRCDIR
_AC_INIT_HELP
_AC_INIT_VERSION
_AC_INIT_CONFIG_LOG
_AC_INIT_PREPARE
_AC_INIT_NOTICE
_AC_INIT_COPYRIGHT
m4_ifval([$2], , [m4_ifval([$1], [AC_CONFIG_SRCDIR([$1])])])dnl
])




## ----------------------------- ##
## Selecting optional features.  ##
## ----------------------------- ##


# AC_ARG_ENABLE(FEATURE, HELP-STRING, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ------------------------------------------------------------------------
AC_DEFUN([AC_ARG_ENABLE],
[m4_divert_once([HELP_ENABLE], [[
Optional Features:
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]]])dnl
m4_divert_once([HELP_ENABLE], [$2])dnl
# Check whether --enable-$1 or --disable-$1 was given.
if test "[${enable_]m4_bpatsubst([$1], -, _)+set}" = set; then
  enableval="[$enable_]m4_bpatsubst([$1], -, _)"
  $3
m4_ifvaln([$4], [else
  $4])dnl
fi; dnl
])# AC_ARG_ENABLE


AU_DEFUN([AC_ENABLE],
[AC_ARG_ENABLE([$1], [  --enable-$1], [$2], [$3])])


## ------------------------------ ##
## Working with optional software ##
## ------------------------------ ##



# AC_ARG_WITH(PACKAGE, HELP-STRING, ACTION-IF-TRUE, [ACTION-IF-FALSE])
# --------------------------------------------------------------------
AC_DEFUN([AC_ARG_WITH],
[m4_divert_once([HELP_WITH], [[
Optional Packages:
  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)]])
m4_divert_once([HELP_WITH], [$2])dnl
# Check whether --with-$1 or --without-$1 was given.
if test "[${with_]m4_bpatsubst([$1], -, _)+set}" = set; then
  withval="[$with_]m4_bpatsubst([$1], -, _)"
  $3
m4_ifvaln([$4], [else
  $4])dnl
fi; dnl
])# AC_ARG_WITH

AU_DEFUN([AC_WITH],
[AC_ARG_WITH([$1], [  --with-$1], [$2], [$3])])



## ----------------------------------------- ##
## Remembering variables for reconfiguring.  ##
## ----------------------------------------- ##


# _AC_ARG_VAR_PRECIOUS(VARNAME)
# -----------------------------
# Declare VARNAME is precious.
#
# We try to diagnose when precious variables have changed.  To do this,
# make two early snapshots (after the option processing to take
# explicit variables into account) of those variables: one (ac_env_)
# which represents the current run, and a second (ac_cv_env_) which,
# at the first run, will be saved in the cache.  As an exception to
# the cache mechanism, its loading will override these variables (non
# `ac_cv_env_' cache value are only set when unset).
#
# In subsequent runs, after having loaded the cache, compare
# ac_cv_env_foo against ac_env_foo.  See _AC_ARG_VAR_VALIDATE.
m4_define([_AC_ARG_VAR_PRECIOUS],
[AC_SUBST([$1])dnl
m4_divert_once([PARSE_ARGS],
[ac_env_$1_set=${$1+set}
ac_env_$1_value=$$1
ac_cv_env_$1_set=${$1+set}
ac_cv_env_$1_value=$$1])dnl
])


# _AC_ARG_VAR_VALIDATE
# --------------------
# The precious variables are saved twice at the beginning of
# configure.  E.g., PRECIOUS is saved as `ac_env_PRECIOUS_SET' and
# `ac_env_PRECIOUS_VALUE' on the one hand and `ac_cv_env_PRECIOUS_SET'
# and `ac_cv_env_PRECIOUS_VALUE' on the other hand.
#
# Now the cache has just been loaded, so `ac_cv_env_' represents the
# content of the cached values, while `ac_env_' represents that of the
# current values.
#
# So we check that `ac_env_' and `ac_cv_env_' are consistent.  If
# they aren't, die.
m4_define([_AC_ARG_VAR_VALIDATE],
[# Check that the precious variables saved in the cache have kept the same
# value.
ac_cache_corrupted=false
for ac_var in `(set) 2>&1 |
	       sed -n 's/^ac_env_\([[a-zA-Z_0-9]]*\)_set=.*/\1/p'`; do
  eval ac_old_set=\$ac_cv_env_${ac_var}_set
  eval ac_new_set=\$ac_env_${ac_var}_set
  eval ac_old_val="\$ac_cv_env_${ac_var}_value"
  eval ac_new_val="\$ac_env_${ac_var}_value"
  case $ac_old_set,$ac_new_set in
    set,)
      AS_MESSAGE([error: `$ac_var' was set to `$ac_old_val' in the previous run], 2)
      ac_cache_corrupted=: ;;
    ,set)
      AS_MESSAGE([error: `$ac_var' was not set in the previous run], 2)
      ac_cache_corrupted=: ;;
    ,);;
    *)
      if test "x$ac_old_val" != "x$ac_new_val"; then
	AS_MESSAGE([error: `$ac_var' has changed since the previous run:], 2)
	AS_MESSAGE([  former value:  $ac_old_val], 2)
	AS_MESSAGE([  current value: $ac_new_val], 2)
	ac_cache_corrupted=:
      fi;;
  esac
  # Pass precious variables to config.status.
  if test "$ac_new_set" = set; then
    case $ac_new_val in
dnl If you change this globbing pattern, test it on an old shell --
dnl it's sensitive.  Putting any kind of quote in it causes syntax errors.
[    *" "*|*"	"*|*[\[\]\~\#\$\^\&\*\(\)\{\}\\\|\;\<\>\?\"\']*)]
      ac_arg=$ac_var=`echo "$ac_new_val" | sed "s/'/'\\\\\\\\''/g"` ;;
    *) ac_arg=$ac_var=$ac_new_val ;;
    esac
    case " $ac_configure_args " in
      *" '$ac_arg' "*) ;; # Avoid dups.  Use of quotes ensures accuracy.
      *) ac_configure_args="$ac_configure_args '$ac_arg'" ;;
    esac
  fi
done
if $ac_cache_corrupted; then
  AS_MESSAGE([error: changes in the environment can compromise the build], 2)
  AS_ERROR([run `make distclean' and/or `rm $cache_file' and start over])
fi
])# _AC_ARG_VAR_VALIDATE


# AC_ARG_VAR(VARNAME, DOCUMENTATION)
# ----------------------------------
# Register VARNAME as a precious variable, and document it in
# `configure --help' (but only once).
AC_DEFUN([AC_ARG_VAR],
[m4_divert_once([HELP_VAR], [[
Some influential environment variables:]])dnl
m4_divert_once([HELP_VAR_END], [[
Use these variables to override the choices made by `configure' or to help
it to find libraries and programs with nonstandard names/locations.]])dnl
m4_expand_once([m4_divert_once([HELP_VAR],
			       [AS_HELP_STRING([$1], [$2], [              ])])],
	       [$0($1)])dnl
_AC_ARG_VAR_PRECIOUS([$1])dnl
])# AC_ARG_VAR





## ---------------------------- ##
## Transforming program names.  ##
## ---------------------------- ##


# AC_ARG_PROGRAM
# --------------
# This macro is expanded only once, to avoid that `foo' ends up being
# installed as `ggfoo'.
AC_DEFUN_ONCE([AC_ARG_PROGRAM],
[dnl Document the options.
m4_divert_push([HELP_BEGIN])dnl

Program names:
  --program-prefix=PREFIX            prepend PREFIX to installed program names
  --program-suffix=SUFFIX            append SUFFIX to installed program names
  --program-transform-name=PROGRAM   run sed PROGRAM on installed program names
m4_divert_pop([HELP_BEGIN])dnl
test "$program_prefix" != NONE &&
  program_transform_name="s,^,$program_prefix,;$program_transform_name"
# Use a double $ so make ignores it.
test "$program_suffix" != NONE &&
  program_transform_name="s,\$,$program_suffix,;$program_transform_name"
# Double any \ or $.  echo might interpret backslashes.
# By default was `s,x,x', remove it if useless.
cat <<\_ACEOF >conftest.sed
[s/[\\$]/&&/g;s/;s,x,x,$//]
_ACEOF
program_transform_name=`echo $program_transform_name | sed -f conftest.sed`
rm conftest.sed
])# AC_ARG_PROGRAM





## ------------------------- ##
## Finding auxiliary files.  ##
## ------------------------- ##


# AC_CONFIG_AUX_DIR(DIR)
# ----------------------
# Find install-sh, config.sub, config.guess, and Cygnus configure
# in directory DIR.  These are auxiliary files used in configuration.
# DIR can be either absolute or relative to $srcdir.
AC_DEFUN([AC_CONFIG_AUX_DIR],
[AC_CONFIG_AUX_DIRS($1 $srcdir/$1)])


# AC_CONFIG_AUX_DIR_DEFAULT
# -------------------------
# The default is `$srcdir' or `$srcdir/..' or `$srcdir/../..'.
# There's no need to call this macro explicitly; just AC_REQUIRE it.
AC_DEFUN([AC_CONFIG_AUX_DIR_DEFAULT],
[AC_CONFIG_AUX_DIRS($srcdir $srcdir/.. $srcdir/../..)])


# AC_CONFIG_AUX_DIRS(DIR ...)
# ---------------------------
# Internal subroutine.
# Search for the configuration auxiliary files in directory list $1.
# We look only for install-sh, so users of AC_PROG_INSTALL
# do not automatically need to distribute the other auxiliary files.
AC_DEFUN([AC_CONFIG_AUX_DIRS],
[ac_aux_dir=
for ac_dir in $1; do
  if test -f $ac_dir/install-sh; then
    ac_aux_dir=$ac_dir
    ac_install_sh="$ac_aux_dir/install-sh -c"
    break
  elif test -f $ac_dir/install.sh; then
    ac_aux_dir=$ac_dir
    ac_install_sh="$ac_aux_dir/install.sh -c"
    break
  elif test -f $ac_dir/shtool; then
    ac_aux_dir=$ac_dir
    ac_install_sh="$ac_aux_dir/shtool install -c"
    break
  fi
done
if test -z "$ac_aux_dir"; then
  AC_MSG_ERROR([cannot find install-sh or install.sh in $1])
fi
ac_config_guess="$SHELL $ac_aux_dir/config.guess"
ac_config_sub="$SHELL $ac_aux_dir/config.sub"
ac_configure="$SHELL $ac_aux_dir/configure" # This should be Cygnus configure.
AC_PROVIDE([AC_CONFIG_AUX_DIR_DEFAULT])dnl
])# AC_CONFIG_AUX_DIRS




## ------------------------ ##
## Finding aclocal macros.  ##
## ------------------------ ##


# AC_CONFIG_MACRO_DIR(DIR)
# ------------------------
# Declare directory containing additional macros for aclocal.
# DIR can be either absolute or relative to $srcdir.
AC_DEFUN([AC_CONFIG_MACRO_DIR],
[case $1 in
  [[\\/]]* | ?:[[\\/]]* ) ac_macro_dir=$1         ;;
  *)                      ac_macro_dir=$srcdir/$1 ;;
esac
if test -d "$ac_macro_dir"; then :
else
  AC_MSG_ERROR([cannot find macro directory `$1'])
fi
])# AC_CONFIG_MACRO_DIR




## ----------------------------------- ##
## Getting the canonical system type.  ##
## ----------------------------------- ##

# The inputs are:
#    configure --host=HOST --target=TARGET --build=BUILD
#
# The rules are:
# 1. Build defaults to the current platform, as determined by config.guess.
# 2. Host defaults to build.
# 3. Target defaults to host.


# _AC_CANONICAL_SPLIT(THING)
# --------------------------
# Generate the variables THING, THING_{alias cpu vendor os}.
m4_define([_AC_CANONICAL_SPLIT],
[AC_SUBST([$1],       [$ac_cv_$1])dnl
dnl FIXME: AC_SUBST([$1_alias],  [$ac_cv_$1_alias])dnl
AC_SUBST([$1_cpu],
	 [`echo $ac_cv_$1 | sed 's/^\([[^-]]*\)-\([[^-]]*\)-\(.*\)$/\1/'`])dnl
AC_SUBST([$1_vendor],
	 [`echo $ac_cv_$1 | sed 's/^\([[^-]]*\)-\([[^-]]*\)-\(.*\)$/\2/'`])dnl
AC_SUBST([$1_os],
	 [`echo $ac_cv_$1 | sed 's/^\([[^-]]*\)-\([[^-]]*\)-\(.*\)$/\3/'`])dnl
])# _AC_CANONICAL_SPLIT


# AC_CANONICAL_BUILD
# ------------------
AC_DEFUN_ONCE([AC_CANONICAL_BUILD],
[AC_REQUIRE([AC_CONFIG_AUX_DIR_DEFAULT])dnl
m4_divert_text([HELP_CANON],
[[
System types:
  --build=BUILD     configure for building on BUILD [guessed]]])dnl
# Make sure we can run config.sub.
$ac_config_sub sun4 >/dev/null 2>&1 ||
  AC_MSG_ERROR([cannot run $ac_config_sub])

AC_CACHE_CHECK([build system type], [ac_cv_build],
[ac_cv_build_alias=$build_alias
test -z "$ac_cv_build_alias" &&
  ac_cv_build_alias=`$ac_config_guess`
test -z "$ac_cv_build_alias" &&
  AC_MSG_ERROR([cannot guess build type; you must specify one])
ac_cv_build=`$ac_config_sub $ac_cv_build_alias` ||
  AC_MSG_ERROR([$ac_config_sub $ac_cv_build_alias failed])
])
_AC_CANONICAL_SPLIT(build)
])# AC_CANONICAL_BUILD


# AC_CANONICAL_HOST
# -----------------
AC_DEFUN_ONCE([AC_CANONICAL_HOST],
[AC_REQUIRE([AC_CANONICAL_BUILD])dnl
m4_divert_text([HELP_CANON],
[[  --host=HOST       cross-compile to build programs to run on HOST [BUILD]]])dnl
AC_CACHE_CHECK([host system type], [ac_cv_host],
[ac_cv_host_alias=$host_alias
test -z "$ac_cv_host_alias" &&
  ac_cv_host_alias=$ac_cv_build_alias
ac_cv_host=`$ac_config_sub $ac_cv_host_alias` ||
  AC_MSG_ERROR([$ac_config_sub $ac_cv_host_alias failed])
])
_AC_CANONICAL_SPLIT([host])
])# AC_CANONICAL_HOST


# AC_CANONICAL_TARGET
# -------------------
AC_DEFUN_ONCE([AC_CANONICAL_TARGET],
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_BEFORE([$0], [AC_ARG_PROGRAM])dnl
m4_divert_text([HELP_CANON],
[[  --target=TARGET   configure for building compilers for TARGET [HOST]]])dnl
AC_CACHE_CHECK([target system type], [ac_cv_target],
[dnl Set target_alias.
ac_cv_target_alias=$target_alias
test "x$ac_cv_target_alias" = "x" &&
  ac_cv_target_alias=$ac_cv_host_alias
ac_cv_target=`$ac_config_sub $ac_cv_target_alias` ||
  AC_MSG_ERROR([$ac_config_sub $ac_cv_target_alias failed])
])
_AC_CANONICAL_SPLIT([target])

# The aliases save the names the user supplied, while $host etc.
# will get canonicalized.
test -n "$target_alias" &&
  test "$program_prefix$program_suffix$program_transform_name" = \
    NONENONEs,x,x, &&
  program_prefix=${target_alias}-[]dnl
])# AC_CANONICAL_TARGET


AU_ALIAS([AC_CANONICAL_SYSTEM], [AC_CANONICAL_TARGET])


# AU::AC_VALIDATE_CACHED_SYSTEM_TUPLE([CMD])
# ------------------------------------------
# If the cache file is inconsistent with the current host,
# target and build system types, execute CMD or print a default
# error message.  Now handled via _AC_ARG_VAR_PRECIOUS.
AU_DEFUN([AC_VALIDATE_CACHED_SYSTEM_TUPLE], [])


## ---------------------- ##
## Caching test results.  ##
## ---------------------- ##


# AC_SITE_LOAD
# ------------
# Look for site or system specific initialization scripts.
m4_define([AC_SITE_LOAD],
[# Prefer explicitly selected file to automatically selected ones.
if test -z "$CONFIG_SITE"; then
  if test "x$prefix" != xNONE; then
    CONFIG_SITE="$prefix/share/config.site $prefix/etc/config.site"
  else
    CONFIG_SITE="$ac_default_prefix/share/config.site $ac_default_prefix/etc/config.site"
  fi
fi
for ac_site_file in $CONFIG_SITE; do
  if test -r "$ac_site_file"; then
    AC_MSG_NOTICE([loading site script $ac_site_file])
    sed 's/^/| /' "$ac_site_file" >&AS_MESSAGE_LOG_FD
    . "$ac_site_file"
  fi
done
])


# AC_CACHE_LOAD
# -------------
m4_define([AC_CACHE_LOAD],
[if test -r "$cache_file"; then
  # Some versions of bash will fail to source /dev/null (special
  # files actually), so we avoid doing that.
  if test -f "$cache_file"; then
    AC_MSG_NOTICE([loading cache $cache_file])
    case $cache_file in
      [[\\/]]* | ?:[[\\/]]* ) . $cache_file;;
      *)                      . ./$cache_file;;
    esac
  fi
else
  AC_MSG_NOTICE([creating cache $cache_file])
  >$cache_file
fi
])# AC_CACHE_LOAD


# _AC_CACHE_DUMP
# --------------
# Dump the cache to stdout.  It can be in a pipe (this is a requirement).
m4_define([_AC_CACHE_DUMP],
[# The following way of writing the cache mishandles newlines in values,
# but we know of no workaround that is simple, portable, and efficient.
# So, don't put newlines in cache variables' values.
# Ultrix sh set writes to stderr and can't be redirected directly,
# and sets the high bit in the cache file unless we assign to the vars.
{
  (set) 2>&1 |
    case `(ac_space=' '; set | grep ac_space) 2>&1` in
    *ac_space=\ *)
      # `set' does not quote correctly, so add quotes (double-quote
      # substitution turns \\\\ into \\, and sed turns \\ into \).
      sed -n \
	["s/'/'\\\\''/g;
	  s/^\\([_$as_cr_alnum]*_cv_[_$as_cr_alnum]*\\)=\\(.*\\)/\\1='\\2'/p"]
      ;;
    *)
      # `set' quotes correctly as required by POSIX, so do not add quotes.
      sed -n \
	["s/^\\([_$as_cr_alnum]*_cv_[_$as_cr_alnum]*\\)=\\(.*\\)/\\1=\\2/p"]
      ;;
    esac;
}dnl
])# _AC_CACHE_DUMP


# AC_CACHE_SAVE
# -------------
# Save the cache.
# Allow a site initialization script to override cache values.
m4_define([AC_CACHE_SAVE],
[cat >confcache <<\_ACEOF
# This file is a shell script that caches the results of configure
# tests run on this system so they can be shared between configure
# scripts and configure runs, see configure's option --config-cache.
# It is not useful on other systems.  If it contains results you don't
# want to keep, you may remove or edit it.
#
# config.status only pays attention to the cache file if you give it
# the --recheck option to rerun configure.
#
# `ac_cv_env_foo' variables (set or unset) will be overridden when
# loading this file, other *unset* `ac_cv_foo' will be assigned the
# following values.

_ACEOF

_AC_CACHE_DUMP() |
  sed ['
     t clear
     : clear
     s/^\([^=]*\)=\(.*[{}].*\)$/test "${\1+set}" = set || &/
     t end
     /^ac_cv_env/!s/^\([^=]*\)=\(.*\)$/\1=${\1=\2}/
     : end'] >>confcache
if diff $cache_file confcache >/dev/null 2>&1; then :; else
  if test -w $cache_file; then
    test "x$cache_file" != "x/dev/null" && echo "updating cache $cache_file"
    cat confcache >$cache_file
  else
    echo "not updating unwritable cache $cache_file"
  fi
fi
rm -f confcache[]dnl
])# AC_CACHE_SAVE


# AC_CACHE_VAL(CACHE-ID, COMMANDS-TO-SET-IT)
# ------------------------------------------
# The name of shell var CACHE-ID must contain `_cv_' in order to get saved.
# Should be dnl'ed.  Try to catch common mistakes.
m4_defun([AC_CACHE_VAL],
[m4_bmatch([$2], [AC_DEFINE],
	   [AC_DIAGNOSE(syntax,
[$0($1, ...): suspicious presence of an AC_DEFINE in the second argument, ]dnl
[where no actions should be taken])])dnl
AS_VAR_SET_IF([$1],
	      [_AS_ECHO_N([(cached) ])],
	      [$2])])


# AC_CACHE_CHECK(MESSAGE, CACHE-ID, COMMANDS)
# -------------------------------------------
# Do not call this macro with a dnl right behind.
m4_defun([AC_CACHE_CHECK],
[AC_MSG_CHECKING([$1])
AC_CACHE_VAL([$2], [$3])dnl
AC_MSG_RESULT_UNQUOTED([AS_VAR_GET([$2])])])



## ---------------------- ##
## Defining CPP symbols.  ##
## ---------------------- ##


# AC_DEFINE_TRACE_LITERAL(LITERAL-CPP-SYMBOL)
# -------------------------------------------
# This macro is useless, it is used only with --trace to collect the
# list of *literals* CPP values passed to AC_DEFINE/AC_DEFINE_UNQUOTED.
m4_define([AC_DEFINE_TRACE_LITERAL])


# AC_DEFINE_TRACE(CPP-SYMBOL)
# ---------------------------
# This macro is a wrapper around AC_DEFINE_TRACE_LITERAL which filters
# out non literal symbols.
m4_define([AC_DEFINE_TRACE],
[AS_LITERAL_IF([$1], [AC_DEFINE_TRACE_LITERAL([$1])])])


# AC_DEFINE(VARIABLE, [VALUE], [DESCRIPTION])
# -------------------------------------------
# Set VARIABLE to VALUE, verbatim, or 1.  Remember the value
# and if VARIABLE is affected the same VALUE, do nothing, else
# die.  The third argument is used by autoheader.
m4_define([AC_DEFINE],
[AC_DEFINE_TRACE([$1])dnl
m4_ifval([$3], [AH_TEMPLATE([$1], [$3])])dnl
cat >>confdefs.h <<\_ACEOF
[@%:@define] $1 m4_if($#, 2, [$2], $#, 3, [$2], 1)
_ACEOF
])


# AC_DEFINE_UNQUOTED(VARIABLE, [VALUE], [DESCRIPTION])
# ----------------------------------------------------
# Similar, but perform shell substitutions $ ` \ once on VALUE.
m4_define([AC_DEFINE_UNQUOTED],
[AC_DEFINE_TRACE([$1])dnl
m4_ifval([$3], [AH_TEMPLATE([$1], [$3])])dnl
cat >>confdefs.h <<_ACEOF
[@%:@define] $1 m4_if($#, 2, [$2], $#, 3, [$2], 1)
_ACEOF
])



## -------------------------- ##
## Setting output variables.  ##
## -------------------------- ##


# AC_SUBST(VARIABLE, [VALUE])
# ---------------------------
# Create an output variable from a shell VARIABLE.  If VALUE is given
# assign it to VARIABLE.  Use `""' is you want to set VARIABLE to an
# empty value, not an empty second argument.
#
# Beware that if you change this macro, you also have to change the
# sed script at the top of _AC_OUTPUT_FILES.
m4_define([AC_SUBST],
[m4_ifvaln([$2], [$1=$2])[]dnl
m4_append_uniq([_AC_SUBST_VARS], [$1], [ ])dnl
])# AC_SUBST


# AC_SUBST_FILE(VARIABLE)
# -----------------------
# Read the comments of the preceding macro.
m4_define([AC_SUBST_FILE],
[m4_append_uniq([_AC_SUBST_FILES], [$1], [ ])])



## --------------------------------------- ##
## Printing messages at autoconf runtime.  ##
## --------------------------------------- ##

# In fact, I think we should promote the use of m4_warn and m4_fatal
# directly.  This will also avoid to some people to get it wrong
# between AC_FATAL and AC_MSG_ERROR.


# AC_DIAGNOSE(CATEGORY, MESSAGE)
# AC_FATAL(MESSAGE, [EXIT-STATUS])
# --------------------------------
m4_define([AC_DIAGNOSE], [m4_warn($@)])
m4_define([AC_FATAL],    [m4_fatal($@)])


# AC_WARNING(MESSAGE)
# -------------------
# Report a MESSAGE to the user of autoconf if `-W' or `-W all' was
# specified.
m4_define([AC_WARNING],
[AC_DIAGNOSE([syntax], [$1])])




## ---------------------------------------- ##
## Printing messages at configure runtime.  ##
## ---------------------------------------- ##


# AC_MSG_CHECKING(FEATURE)
# ------------------------
m4_define([AC_MSG_CHECKING],
[_AS_ECHO([$as_me:$LINENO: checking $1], AS_MESSAGE_LOG_FD)
_AS_ECHO_N([checking $1... ])[]dnl
])


# AC_MSG_RESULT(RESULT)
# ---------------------
m4_define([AC_MSG_RESULT],
[_AS_ECHO([$as_me:$LINENO: result: $1], AS_MESSAGE_LOG_FD)
_AS_ECHO([${ECHO_T}$1])[]dnl
])


# AC_MSG_RESULT_UNQUOTED(RESULT)
# ------------------------------
# Likewise, but perform $ ` \ shell substitutions.
m4_define([AC_MSG_RESULT_UNQUOTED],
[_AS_ECHO_UNQUOTED([$as_me:$LINENO: result: $1], AS_MESSAGE_LOG_FD)
_AS_ECHO_UNQUOTED([${ECHO_T}$1])[]dnl
])


# AC_MSG_WARN(PROBLEM)
# AC_MSG_NOTICE(STRING)
# AC_MSG_ERROR(ERROR, [EXIT-STATUS = 1])
# AC_MSG_FAILURE(ERROR, [EXIT-STATUS = 1])
# ----------------------------------------
m4_copy([AS_WARN],    [AC_MSG_WARN])
m4_copy([AS_MESSAGE], [AC_MSG_NOTICE])
m4_copy([AS_ERROR],   [AC_MSG_ERROR])
m4_define([AC_MSG_FAILURE],
[AC_MSG_ERROR([$1
See `config.log' for more details.], [$2])])


# _AC_MSG_LOG_CONFTEST
# --------------------
m4_define([_AC_MSG_LOG_CONFTEST],
[echo "$as_me: failed program was:" >&AS_MESSAGE_LOG_FD
sed 's/^/| /' conftest.$ac_ext >&AS_MESSAGE_LOG_FD
])


# AU::AC_CHECKING(FEATURE)
# ------------------------
AU_DEFUN([AC_CHECKING],
[AS_MESSAGE([checking $1...])])


# AU::AC_VERBOSE(STRING)
# ----------------------
AU_ALIAS([AC_VERBOSE], [AC_MSG_RESULT])






## ---------------------------- ##
## Compiler-running mechanics.  ##
## ---------------------------- ##


# _AC_RUN_LOG(COMMAND, LOG-COMMANDS)
# ----------------------------------
# Eval COMMAND, save the exit status in ac_status, and log it.
AC_DEFUN([_AC_RUN_LOG],
[{ ($2) >&AS_MESSAGE_LOG_FD
  ($1) 2>&AS_MESSAGE_LOG_FD
  ac_status=$?
  echo "$as_me:$LINENO: \$? = $ac_status" >&AS_MESSAGE_LOG_FD
  (exit $ac_status); }])


# _AC_RUN_LOG_STDERR(COMMAND, LOG-COMMANDS)
# -----------------------------------------
# Run COMMAND, save its stderr into conftest.err, save the exit status
# in ac_status, and log it.  Don't forget to clean up conftest.err after
# use.
# Note that when tracing, most shells will leave the traces in stderr
# starting with "+": that's what this macro tries to address.
AC_DEFUN([_AC_RUN_LOG_STDERR],
[{ ($2) >&AS_MESSAGE_LOG_FD
  ($1) 2>conftest.er1
  ac_status=$?
  grep -v '^ *+' conftest.er1 >conftest.err
  rm -f conftest.er1
  cat conftest.err >&AS_MESSAGE_LOG_FD
  echo "$as_me:$LINENO: \$? = $ac_status" >&AS_MESSAGE_LOG_FD
  (exit $ac_status); }])


# _AC_EVAL(COMMAND)
# -----------------
# Eval COMMAND, save the exit status in ac_status, and log it.
AC_DEFUN([_AC_EVAL],
[_AC_RUN_LOG([eval $1],
	     [eval echo "$as_me:$LINENO: \"$1\""])])


# _AC_EVAL_STDERR(COMMAND)
# ------------------------
# Same as _AC_RUN_LOG_STDERR, but evals  (instead of the running) the
# COMMAND.
AC_DEFUN([_AC_EVAL_STDERR],
[_AC_RUN_LOG_STDERR([eval $1],
		    [eval echo "$as_me:$LINENO: \"$1\""])])


# AC_TRY_EVAL(VARIABLE)
# ---------------------
# The purpose of this macro is to "configure:123: command line"
# written into config.log for every test run.
AC_DEFUN([AC_TRY_EVAL],
[_AC_EVAL([$$1])])


# AC_TRY_COMMAND(COMMAND)
# -----------------------
AC_DEFUN([AC_TRY_COMMAND],
[{ ac_try='$1'
  _AC_EVAL([$ac_try]); }])


# AC_RUN_LOG(COMMAND)
# -------------------
AC_DEFUN([AC_RUN_LOG],
[_AC_RUN_LOG([$1],
	     [echo "$as_me:$LINENO: AS_ESCAPE([$1])"])])




## ------------------------ ##
## Examining declarations.  ##
## ------------------------ ##



# _AC_PREPROC_IFELSE(PROGRAM, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ----------------------------------------------------------------
# Try to preprocess PROGRAM.
#
# This macro can be used during the selection of a preprocessor.
# Run cpp and set ac_cpp_err to "yes" for an error, to
# "$ac_(c,cxx)_preproc_warn_flag" if there are warnings or to "" if
# neither warnings nor errors have been detected.  eval is necessary
# to expand ac_cpp.
AC_DEFUN([_AC_PREPROC_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
if _AC_EVAL_STDERR([$ac_cpp conftest.$ac_ext]) >/dev/null; then
  if test -s conftest.err; then
    ac_cpp_err=$ac_[]_AC_LANG_ABBREV[]_preproc_warn_flag
    ac_cpp_err=$ac_cpp_err$ac_[]_AC_LANG_ABBREV[]_werror_flag
  else
    ac_cpp_err=
  fi
else
  ac_cpp_err=yes
fi
if test -z "$ac_cpp_err"; then
  m4_default([$2], :)
else
  _AC_MSG_LOG_CONFTEST
  $3
fi
rm -f conftest.err m4_ifval([$1], [conftest.$ac_ext])[]dnl
])# _AC_PREPROC_IFELSE


# AC_PREPROC_IFELSE(PROGRAM, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ---------------------------------------------------------------
# Try to preprocess PROGRAM.  Requires that the preprocessor for the
# current language was checked for, hence do not use this macro in macros
# looking for a preprocessor.
AC_DEFUN([AC_PREPROC_IFELSE],
[AC_LANG_PREPROC_REQUIRE()dnl
_AC_PREPROC_IFELSE($@)])


# AC_TRY_CPP(INCLUDES, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ---------------------------------------------------------
# AC_TRY_CPP is used to check whether particular header files exist.
# (But it actually tests whether INCLUDES produces no CPP errors.)
#
# INCLUDES are not defaulted and are double quoted.
AU_DEFUN([AC_TRY_CPP],
[AC_PREPROC_IFELSE([AC_LANG_SOURCE([[$1]])], [$2], [$3])])


# AC_EGREP_CPP(PATTERN, PROGRAM,
#              [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ------------------------------------------------------
# Because this macro is used by AC_PROG_GCC_TRADITIONAL, which must
# come early, it is not included in AC_BEFORE checks.
AC_DEFUN([AC_EGREP_CPP],
[AC_LANG_PREPROC_REQUIRE()dnl
AC_REQUIRE([AC_PROG_EGREP])dnl
AC_LANG_CONFTEST([AC_LANG_SOURCE([[$2]])])
dnl eval is necessary to expand ac_cpp.
dnl Ultrix and Pyramid sh refuse to redirect output of eval, so use subshell.
if (eval "$ac_cpp conftest.$ac_ext") 2>&AS_MESSAGE_LOG_FD |
dnl Quote $1 to prevent m4 from eating character classes
  $EGREP "[$1]" >/dev/null 2>&1; then
  m4_default([$3], :)
m4_ifvaln([$4], [else
  $4])dnl
fi
rm -f conftest*
])# AC_EGREP_CPP


# AC_EGREP_HEADER(PATTERN, HEADER-FILE,
#                 [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------
AC_DEFUN([AC_EGREP_HEADER],
[AC_EGREP_CPP([$1],
[#include <$2>
], [$3], [$4])])




## ------------------ ##
## Examining syntax.  ##
## ------------------ ##


# _AC_COMPILE_IFELSE(PROGRAM, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------------------
# Try to compile PROGRAM.
# This macro can be used during the selection of a compiler.
m4_define([_AC_COMPILE_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest.$ac_objext
AS_IF([_AC_EVAL_STDERR($ac_compile) &&
	 AC_TRY_COMMAND([test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag"			 || test ! -s conftest.err]) &&
	 AC_TRY_COMMAND([test -s conftest.$ac_objext])],
      [$2],
      [_AC_MSG_LOG_CONFTEST
m4_ifvaln([$3],[$3])dnl])dnl
rm -f conftest.err conftest.$ac_objext m4_ifval([$1], [conftest.$ac_ext])[]dnl
])# _AC_COMPILE_IFELSE


# AC_COMPILE_IFELSE(PROGRAM, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------------------
# Try to compile PROGRAM.  Requires that the compiler for the current
# language was checked for, hence do not use this macro in macros looking
# for a compiler.
AC_DEFUN([AC_COMPILE_IFELSE],
[AC_LANG_COMPILER_REQUIRE()dnl
_AC_COMPILE_IFELSE($@)])


# AC_TRY_COMPILE(INCLUDES, FUNCTION-BODY,
#                [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------
AU_DEFUN([AC_TRY_COMPILE],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[$1]], [[$2]])], [$3], [$4])])



## --------------------- ##
## Examining libraries.  ##
## --------------------- ##


# _AC_LINK_IFELSE(PROGRAM, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ------------------------------------------------------------------
# Try to link PROGRAM.
# This macro can be used during the selection of a compiler.
m4_define([_AC_LINK_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest.$ac_objext conftest$ac_exeext
AS_IF([_AC_EVAL_STDERR($ac_link) &&
	 AC_TRY_COMMAND([test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag"			 || test ! -s conftest.err]) &&
	 AC_TRY_COMMAND([test -s conftest$ac_exeext])],
      [$2],
      [_AC_MSG_LOG_CONFTEST
m4_ifvaln([$3], [$3])dnl])[]dnl
rm -f conftest.err conftest.$ac_objext \
      conftest$ac_exeext m4_ifval([$1], [conftest.$ac_ext])[]dnl
])# _AC_LINK_IFELSE


# AC_LINK_IFELSE(PROGRAM, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------
# Try to link PROGRAM.  Requires that the compiler for the current
# language was checked for, hence do not use this macro in macros looking
# for a compiler.
AC_DEFUN([AC_LINK_IFELSE],
[AC_LANG_COMPILER_REQUIRE()dnl
_AC_LINK_IFELSE($@)])


# AC_TRY_LINK(INCLUDES, FUNCTION-BODY,
#             [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------
# Contrarily to AC_LINK_IFELSE, this macro double quote its first two args.
AU_DEFUN([AC_TRY_LINK],
[AC_LINK_IFELSE([AC_LANG_PROGRAM([[$1]], [[$2]])], [$3], [$4])])


# AC_COMPILE_CHECK(ECHO-TEXT, INCLUDES, FUNCTION-BODY,
#                  ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------
AU_DEFUN([AC_COMPILE_CHECK],
[m4_ifvaln([$1], [AC_CHECKING([for $1])])dnl
AC_LINK_IFELSE([AC_LANG_PROGRAM([[$2]], [[$3]])], [$4], [$5])
])




## -------------------------------- ##
## Checking for run-time features.  ##
## -------------------------------- ##


# _AC_RUN_IFELSE(PROGRAM, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ------------------------------------------------------------
# Compile, link, and run.
# This macro can be used during the selection of a compiler.
# We also remove conftest.o as if the compilation fails, some compilers
# don't remove it.  We remove gmon.out and bb.out, which may be
# created during the run if the program is built with profiling support.
m4_define([_AC_RUN_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest$ac_exeext
AS_IF([AC_TRY_EVAL(ac_link) && AC_TRY_COMMAND(./conftest$ac_exeext)],
      [$2],
      [echo "$as_me: program exited with status $ac_status" >&AS_MESSAGE_LOG_FD
_AC_MSG_LOG_CONFTEST
m4_ifvaln([$3],
	  [( exit $ac_status )
$3])dnl])[]dnl
rm -f core *.core gmon.out bb.out conftest$ac_exeext conftest.$ac_objext m4_ifval([$1],
						     [conftest.$ac_ext])[]dnl
])# _AC_RUN_IFELSE


# AC_RUN_IFELSE(PROGRAM,
#               [ACTION-IF-TRUE], [ACTION-IF-FALSE],
#               [ACTION-IF-CROSS-COMPILING = RUNTIME-ERROR])
# ----------------------------------------------------------
# Compile, link, and run. Requires that the compiler for the current
# language was checked for, hence do not use this macro in macros looking
# for a compiler.
AC_DEFUN([AC_RUN_IFELSE],
[AC_LANG_COMPILER_REQUIRE()dnl
m4_ifval([$4], [],
	 [AC_DIAGNOSE([cross],
		     [$0 called without default to allow cross compiling])])dnl
if test "$cross_compiling" = yes; then
  m4_default([$4],
	   [AC_MSG_ERROR([internal error: not reached in cross-compile])])
else
  _AC_RUN_IFELSE($@)
fi])


# AC_TRY_RUN(PROGRAM,
#            [ACTION-IF-TRUE], [ACTION-IF-FALSE],
#            [ACTION-IF-CROSS-COMPILING = RUNTIME-ERROR])
# --------------------------------------------------------
AU_DEFUN([AC_TRY_RUN],
[AC_RUN_IFELSE([AC_LANG_SOURCE([[$1]])], [$2], [$3], [$4])])



## ------------------------------------- ##
## Checking for the existence of files.  ##
## ------------------------------------- ##

# AC_CHECK_FILE(FILE, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -------------------------------------------------------------
#
# Check for the existence of FILE.
AC_DEFUN([AC_CHECK_FILE],
[AC_DIAGNOSE([cross],
	     [cannot check for file existence when cross compiling])dnl
AS_VAR_PUSHDEF([ac_File], [ac_cv_file_$1])dnl
AC_CACHE_CHECK([for $1], ac_File,
[test "$cross_compiling" = yes &&
  AC_MSG_ERROR([cannot check for file existence when cross compiling])
if test -r "$1"; then
  AS_VAR_SET(ac_File, yes)
else
  AS_VAR_SET(ac_File, no)
fi])
AS_IF([test AS_VAR_GET(ac_File) = yes], [$2], [$3])[]dnl
AS_VAR_POPDEF([ac_File])dnl
])# AC_CHECK_FILE


# AC_CHECK_FILES(FILE..., [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------
AC_DEFUN([AC_CHECK_FILES],
[AC_FOREACH([AC_FILE_NAME], [$1],
  [AC_CHECK_FILE(AC_FILE_NAME,
		 [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_[]AC_FILE_NAME), 1,
				    [Define to 1 if you have the
				     file `]AC_File['.])
$2],
		 [$3])])])


## ------------------------------- ##
## Checking for declared symbols.  ##
## ------------------------------- ##


# AC_CHECK_DECL(SYMBOL,
#               [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#               [INCLUDES = DEFAULT-INCLUDES])
# -------------------------------------------------------
# Check if SYMBOL (a variable or a function) is declared.
AC_DEFUN([AC_CHECK_DECL],
[AS_VAR_PUSHDEF([ac_Symbol], [ac_cv_have_decl_$1])dnl
AC_CACHE_CHECK([whether $1 is declared], ac_Symbol,
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT([$4])],
[#ifndef $1
  char *p = (char *) $1;
#endif
])],
		   [AS_VAR_SET(ac_Symbol, yes)],
		   [AS_VAR_SET(ac_Symbol, no)])])
AS_IF([test AS_VAR_GET(ac_Symbol) = yes], [$2], [$3])[]dnl
AS_VAR_POPDEF([ac_Symbol])dnl
])# AC_CHECK_DECL


# AC_CHECK_DECLS(SYMBOLS,
#                [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#                [INCLUDES = DEFAULT-INCLUDES])
# --------------------------------------------------------
# Defines HAVE_DECL_SYMBOL to 1 if declared, 0 otherwise.  See the
# documentation for a detailed explanation of this difference with
# other AC_CHECK_*S macros.  SYMBOLS is an m4 list.
AC_DEFUN([AC_CHECK_DECLS],
[m4_foreach([AC_Symbol], [$1],
  [AC_CHECK_DECL(AC_Symbol,
		 [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_DECL_]AC_Symbol), 1,
				     [Define to 1 if you have the declaration
				     of `]AC_Symbol[', and to 0 if you don't.])
$2],
		 [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_DECL_]AC_Symbol), 0)
$3],
		 [$4])])
])# AC_CHECK_DECLS



## ---------------------------------- ##
## Replacement of library functions.  ##
## ---------------------------------- ##


# AC_CONFIG_LIBOBJ_DIR(DIRNAME)
# -----------------------------
# Announce LIBOBJ replacement files are in $top_srcdir/DIRNAME.
AC_DEFUN_ONCE([AC_CONFIG_LIBOBJ_DIR],
[m4_bmatch([$1], [^]m4_defn([m4_cr_symbols2]),
	   [AC_WARNING([invalid replacement directory: $1])])dnl
m4_divert_text([DEFAULTS], [ac_config_libobj_dir=$1])[]dnl
])


# AC_LIBSOURCE(FILENAME)
# ----------------------
# Announce we might need the file `FILENAME'.
m4_define([AC_LIBSOURCE], [])


# AC_LIBSOURCES([FILENAME1, ...])
# -------------------------------
# Announce we might need these files.
m4_define([AC_LIBSOURCES],
[m4_foreach([_AC_FILENAME], [$1],
	    [AC_LIBSOURCE(_AC_FILENAME)])])


# _AC_LIBOBJ(FILENAME-NOEXT, ACTION-IF-INDIR)
# -------------------------------------------
# We need `FILENAME-NOEXT.o', save this into `LIBOBJS'.
# We don't use AC_SUBST/2 because it forces an unnecessary eol.
m4_define([_AC_LIBOBJ],
[AS_LITERAL_IF([$1],
	       [AC_LIBSOURCE([$1.c])],
	       [$2])dnl
AC_SUBST([LIB@&t@OBJS])dnl
case $LIB@&t@OBJS in
    "$1.$ac_objext"   | \
  *" $1.$ac_objext"   | \
    "$1.$ac_objext "* | \
  *" $1.$ac_objext "* ) ;;
  *) LIB@&t@OBJS="$LIB@&t@OBJS $1.$ac_objext" ;;
esac
])



# AC_LIBOBJ(FILENAME-NOEXT)
# -------------------------
# We need `FILENAME-NOEXT.o', save this into `LIBOBJS'.
# We don't use AC_SUBST/2 because it forces an unnecessary eol.
m4_define([AC_LIBOBJ],
[_AC_LIBOBJ([$1],
	    [AC_DIAGNOSE(syntax,
			 [$0($1): you should use literals])])dnl
])


# _AC_LIBOBJS_NORMALIZE
# ---------------------
# Clean up LIBOBJS abd LTLIBOBJS so that they work with 1. ac_objext,
# 2. Automake's ANSI2KNR, 3. Libtool, 4. combination of the three.
# Used with AC_CONFIG_COMMANDS_PRE.
AC_DEFUN([_AC_LIBOBJS_NORMALIZE],
[ac_libobjs=
ac_ltlibobjs=
for ac_i in : $LIB@&t@OBJS; do test "x$ac_i" = x: && continue
  # 1. Remove the extension, and $U if already installed.
  ac_i=`echo "$ac_i" |
	 sed 's/\$U\././;s/\.o$//;s/\.obj$//'`
  # 2. Add them.
  ac_libobjs="$ac_libobjs $ac_i\$U.$ac_objext"
  ac_ltlibobjs="$ac_ltlibobjs $ac_i"'$U.lo'
done
AC_SUBST([LIB@&t@OBJS], [$ac_libobjs])
AC_SUBST([LTLIBOBJS], [$ac_ltlibobjs])
])


## ----------------------------------- ##
## Checking compiler characteristics.  ##
## ----------------------------------- ##


# _AC_COMPUTE_INT_COMPILE(EXPRESSION, VARIABLE, [INCLUDES], [IF-FAILS])
# ---------------------------------------------------------------------
# Compute the integer EXPRESSION and store the result in the VARIABLE.
# Works OK if cross compiling, but assumes twos-complement arithmetic.
m4_define([_AC_COMPUTE_INT_COMPILE],
[# Depending upon the size, compute the lo and hi bounds.
AC_COMPILE_IFELSE([AC_LANG_BOOL_COMPILE_TRY([$3], [($1) >= 0])],
 [ac_lo=0 ac_mid=0
  while :; do
    AC_COMPILE_IFELSE([AC_LANG_BOOL_COMPILE_TRY([$3], [($1) <= $ac_mid])],
		   [ac_hi=$ac_mid; break],
		   [ac_lo=`expr $ac_mid + 1`
		    if test $ac_lo -le $ac_mid; then
		      ac_lo= ac_hi=
		      break
		    fi
		    ac_mid=`expr 2 '*' $ac_mid + 1`])
  done],
[AC_COMPILE_IFELSE([AC_LANG_BOOL_COMPILE_TRY([$3], [($1) < 0])],
 [ac_hi=-1 ac_mid=-1
  while :; do
    AC_COMPILE_IFELSE([AC_LANG_BOOL_COMPILE_TRY([$3], [($1) >= $ac_mid])],
		      [ac_lo=$ac_mid; break],
		      [ac_hi=`expr '(' $ac_mid ')' - 1`
		       if test $ac_mid -le $ac_hi; then
			 ac_lo= ac_hi=
			 break
		       fi
		       ac_mid=`expr 2 '*' $ac_mid`])
  done],
 [ac_lo= ac_hi=])])
# Binary search between lo and hi bounds.
while test "x$ac_lo" != "x$ac_hi"; do
  ac_mid=`expr '(' $ac_hi - $ac_lo ')' / 2 + $ac_lo`
  AC_COMPILE_IFELSE([AC_LANG_BOOL_COMPILE_TRY([$3], [($1) <= $ac_mid])],
		     [ac_hi=$ac_mid], [ac_lo=`expr '(' $ac_mid ')' + 1`])
done
case $ac_lo in
?*) $2=$ac_lo;;
'') $4 ;;
esac[]dnl
])# _AC_COMPUTE_INT_COMPILE


# _AC_COMPUTE_INT_RUN(EXPRESSION, VARIABLE, [INCLUDES], [IF-FAILS])
# -----------------------------------------------------------------
# Store the evaluation of the integer EXPRESSION in VARIABLE.
m4_define([_AC_COMPUTE_INT_RUN],
[AC_RUN_IFELSE([AC_LANG_INT_SAVE([$3], [$1])],
	       [$2=`cat conftest.val`], [$4])])


# _AC_COMPUTE_INT(EXPRESSION, VARIABLE, INCLUDES, IF-FAILS)
# ---------------------------------------------------------
m4_define([_AC_COMPUTE_INT],
[if test "$cross_compiling" = yes; then
  _AC_COMPUTE_INT_COMPILE([$1], [$2], [$3], [$4])
else
  _AC_COMPUTE_INT_RUN([$1], [$2], [$3], [$4])
fi
rm -f conftest.val[]dnl
])# _AC_COMPUTE_INT
