# This file is part of Autoconf.                       -*- Autoconf -*-
# Parameterizing and creating config.status.
# Copyright (C) 1992, 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2001,
# 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

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


# This file handles about all the preparation aspects for
# `config.status': registering the configuration files, the headers,
# the links, and the commands `config.status' will run.  There is a
# little mixture though of things actually handled by `configure',
# such as running the `configure' in the sub directories.  Minor
# detail.
#
# There are two kinds of commands:
#
# COMMANDS:
#
#   They are output into `config.status' via a quoted here doc.  These
#   commands are always associated to a tag which the user can use to
#   tell `config.status' what are the commands she wants to run.
#
# INIT-CMDS:
#
#   They are output via an *unquoted* here-doc.  As a consequence $var
#   will be output as the value of VAR.  This is typically used by
#   `configure' to give `config.status' some variables it needs to run
#   the COMMANDS.  At the difference of COMMANDS, the INIT-CMDS are
#   always run.
#
#
# Honorable members of this family are AC_CONFIG_FILES,
# AC_CONFIG_HEADERS, AC_CONFIG_LINKS and AC_CONFIG_COMMANDS.  Bad boys
# are AC_LINK_FILES, AC_OUTPUT_COMMANDS and AC_OUTPUT when used with
# arguments.  False members are AC_CONFIG_SRCDIR, AC_CONFIG_SUBDIRS
# and AC_CONFIG_AUX_DIR.  Cousins are AC_CONFIG_COMMANDS_PRE and
# AC_CONFIG_COMMANDS_POST.


## ------------------ ##
## Auxiliary macros.  ##
## ------------------ ##

# _AC_SRCDIRS(BUILD-DIR-NAME)
# ---------------------------
# Inputs:
#   - BUILD-DIR-NAME is `top-build -> build' and `top-src -> src'
#   - `$srcdir' is `top-build -> top-src'
#
# Outputs:
# - `ac_builddir' is `.', for symmetry only.
# - `ac_top_builddir_sub' is `build -> top_build'.
#      This is used for @top_builddir@.
# - `ac_top_build_prefix' is `build -> top_build'.
#      If not empty, has a trailing slash.
# - `ac_srcdir' is `build -> src'.
# - `ac_top_srcdir' is `build -> top-src'.
# and `ac_abs_builddir' etc., the absolute directory names.
m4_define([_AC_SRCDIRS],
[ac_builddir=.

case $1 in
.) ac_dir_suffix= ac_top_builddir_sub=. ac_top_build_prefix= ;;
*)
  ac_dir_suffix=/`echo $1 | sed 's,^\.[[\\/]],,'`
  # A ".." for each directory in $ac_dir_suffix.
  ac_top_builddir_sub=`echo "$ac_dir_suffix" | sed 's,/[[^\\/]]*,/..,g;s,/,,'`
  case $ac_top_builddir_sub in
  "") ac_top_builddir_sub=. ac_top_build_prefix= ;;
  *)  ac_top_build_prefix=$ac_top_builddir_sub/ ;;
  esac ;;
esac
ac_abs_top_builddir=$ac_pwd
ac_abs_builddir=$ac_pwd$ac_dir_suffix
# for backward compatibility:
ac_top_builddir=$ac_top_build_prefix

case $srcdir in
  .)  # We are building in place.
    ac_srcdir=.
    ac_top_srcdir=$ac_top_builddir_sub
    ac_abs_top_srcdir=$ac_pwd ;;
  [[\\/]]* | ?:[[\\/]]* )  # Absolute name.
    ac_srcdir=$srcdir$ac_dir_suffix;
    ac_top_srcdir=$srcdir
    ac_abs_top_srcdir=$srcdir ;;
  *) # Relative name.
    ac_srcdir=$ac_top_build_prefix$srcdir$ac_dir_suffix
    ac_top_srcdir=$ac_top_build_prefix$srcdir
    ac_abs_top_srcdir=$ac_pwd/$srcdir ;;
esac
ac_abs_srcdir=$ac_abs_top_srcdir$ac_dir_suffix
])# _AC_SRCDIRS



## ---------------------- ##
## Registering the tags.  ##
## ---------------------- ##


# _AC_CONFIG_COMMANDS_INIT([INIT-COMMANDS])
# -----------------------------------------
#
# Register INIT-COMMANDS as command pasted *unquoted* in
# `config.status'.  This is typically used to pass variables from
# `configure' to `config.status'.  Note that $[1] is not over quoted as
# was the case in AC_OUTPUT_COMMANDS.
m4_define([_AC_CONFIG_COMMANDS_INIT],
[m4_ifval([$1],
	  [m4_append([_AC_OUTPUT_COMMANDS_INIT],
		     [$1
])])])


# AC_FILE_DEPENDENCY_TRACE(DEST, SOURCE1, [SOURCE2...])
# -----------------------------------------------------
# This macro does nothing, it's a hook to be read with `autoconf --trace'.
#
# It announces DEST depends upon the SOURCE1 etc.
m4_define([AC_FILE_DEPENDENCY_TRACE], [])


# _AC_FILE_DEPENDENCY_TRACE_COLON(DEST:SOURCE1[:SOURCE2...])
# ----------------------------------------------------------
# Declare that DEST depends upon SOURCE1 etc.
#
m4_define([_AC_FILE_DEPENDENCY_TRACE_COLON],
[AC_FILE_DEPENDENCY_TRACE(m4_bpatsubst([$1], [:], [,]))dnl
])


# _AC_CONFIG_DEPENDENCY(MODE, DEST[:SOURCE1...])
# ---------------------------------------------
# MODE is `FILES', `HEADERS', or `LINKS'.
#
# Be sure that a missing dependency is expressed as a dependency upon
# `DEST.in' (except with config links).
#
m4_define([_AC_CONFIG_DEPENDENCY],
[_AC_FILE_DEPENDENCY_TRACE_COLON([$2]_AC_CONFIG_DEPENDENCY_DEFAULT($@))dnl
])


# _AC_CONFIG_DEPENDENCY_DEFAULT(MODE, DEST[:SOURCE1...])
# ------------------------------------------------------
# Expand to `:DEST.in' if appropriate, or to empty string otherwise.
#
# More detailed edscription:
# If the tag contains `:', expand to nothing.
# Otherwise, for a config file or header, add `:DEST.in'.
# For config link, DEST.in is not appropriate:
#  - if the tag is literal, complain.
#  - otherwise, just expand to nothing and proceed with fingers crossed.
#    (We get to this case from the obsolete AC_LINK_FILES, for example.)
#
m4_define([_AC_CONFIG_DEPENDENCY_DEFAULT],
[m4_bmatch([$2], [:], [],
	   [m4_if([$1], [LINKS],
		  [AS_LITERAL_IF([$2], [AC_FATAL([Invalid AC_CONFIG_LINKS tag: `$2'])])],
		  [:$2.in])])dnl
])


# _AC_CONFIG_UNIQUE(MODE, DEST)
# -----------------------------
# MODE is `FILES', `HEADERS', `LINKS', `COMMANDS', or `SUBDIRS'.
#
# Verify that there is no double definition of an output file.
#
m4_define([_AC_CONFIG_UNIQUE],
[m4_ifdef([_AC_SEEN_TAG($2)],
   [AC_FATAL([`$2' is already registered with AC_CONFIG_]m4_defn([_AC_SEEN_TAG($2)]).)],
   [m4_define([_AC_SEEN_TAG($2)], [$1])])dnl
])


# _AC_CONFIG_FOOS(MODE, TAGS..., [COMMANDS], [INIT-CMDS])
# -------------------------------------------------------
# MODE is `FILES', `HEADERS', `LINKS', or `COMMANDS'.
#
# Associate the COMMANDS to each TAG, i.e., when config.status creates TAG,
# run COMMANDS afterwards.  (This is done in _AC_CONFIG_REGISTER_DEST.)
#
# For COMMANDS, do not m4_normalize TAGS before adding it to ac_config_commands.
# This historical difference allows macro calls in TAGS.
#
m4_define([_AC_CONFIG_FOOS],
[m4_foreach_w([AC_File], [$2],
	      [_AC_CONFIG_REGISTER([$1], m4_defn([AC_File]), [$3])])dnl
m4_define([_AC_SEEN_CONFIG(ANY)])dnl
m4_define([_AC_SEEN_CONFIG($1)])dnl
_AC_CONFIG_COMMANDS_INIT([$4])dnl
ac_config_[]m4_tolower([$1])="$ac_config_[]m4_tolower([$1]) dnl
m4_if([$1], [COMMANDS], [$2], [m4_normalize([$2])])"
])


# _AC_CONFIG_REGISTER(MODE, TAG, [COMMANDS])
# ------------------------------------------
# MODE is `FILES', `HEADERS', `LINKS', or `COMMANDS'.
#
m4_define([_AC_CONFIG_REGISTER],
[m4_if([$1], [COMMANDS],
       [],
       [_AC_CONFIG_DEPENDENCY([$1], [$2])])dnl
_AC_CONFIG_REGISTER_DEST([$1], [$2], m4_bpatsubst([[$2]], [:.*\(.\)$], [\1]), [$3])dnl
])


# _AC_CONFIG_REGISTER_DEST(MODE, TAG, DEST, [COMMANDS])
# -----------------------------------------------------
# MODE is `FILES', `HEADERS', `LINKS', or `COMMANDS'.
# TAG is in the form DEST[:SOURCE...].
# Thus parameter $3 is the first part of $2.
#
# With CONFIG_LINKS, reject DEST=., because it is makes it hard for ./config.status
# to guess the links to establish (`./config.status .').
#
# Save the name of the first config header to AH_HEADER.
#
m4_define([_AC_CONFIG_REGISTER_DEST],
[_AC_CONFIG_UNIQUE([$1], [$3])dnl
m4_if([$1 $3], [LINKS .],
      [AC_FATAL([invalid destination of a config link: `.'])])dnl
m4_if([$1], [HEADERS],
      [m4_define_default([AH_HEADER], [$3])])dnl
dnl
dnl Recognize TAG as an argument to config.status:
dnl
m4_append([_AC_LIST_TAGS],
[    "$3") CONFIG_$1="$CONFIG_$1 $2" ;;
])dnl
dnl
dnl Register the associated commands, if any:
dnl
m4_ifval([$4],
[m4_append([_AC_LIST_TAG_COMMANDS],
[    "$3":]m4_bpatsubst([$1], [^\(.\).*$], [\1])[) $4 ;;
])])dnl
])# _AC_CONFIG_REGISTER_DEST




## --------------------- ##
## Configuration files.  ##
## --------------------- ##


# AC_CONFIG_FILES(FILE..., [COMMANDS], [INIT-CMDS])
# -------------------------------------------------
# Specify output files, i.e., files that are configured with AC_SUBST.
#
AC_DEFUN([AC_CONFIG_FILES], [_AC_CONFIG_FOOS([FILES], $@)])


# _AC_SED_CMD_LIMIT
# -----------------
# Evaluate to an m4 number equal to the maximum number of commands to put
# in any single sed program, not counting ":" commands.
#
# Some seds have small command number limits, like on Digital OSF/1 and HP-UX.
m4_define([_AC_SED_CMD_LIMIT],
dnl One cannot portably go further than 99 commands because of HP-UX.
[99])


# _AC_OUTPUT_FILES_PREPARE
# ------------------------
# Create the sed scripts needed for CONFIG_FILES.
# Support multiline substitutions and make sure that the substitutions are
# not evaluated recursively.
# The intention is to have readable config.status and configure, even
# though this m4 code might be scaring.
#
# This code was written by Dan Manthey.
#
# This macro is expanded inside a here document.  If the here document is
# closed, it has to be reopened with "cat >>$CONFIG_STATUS <<\_ACEOF".
#
m4_define([_AC_OUTPUT_FILES_PREPARE],
[#
# Set up the sed scripts for CONFIG_FILES section.
#
dnl ... and define _AC_SED_CMDS, the pipeline which executes them.
m4_define([_AC_SED_CMDS], [])dnl

# No need to generate the scripts if there are no CONFIG_FILES.
# This happens for instance when ./config.status config.h
if test -n "$CONFIG_FILES"; then

_ACEOF

m4_pushdef([_AC_SED_FRAG_NUM], 0)dnl Fragment number.
m4_pushdef([_AC_SED_CMD_NUM], 2)dnl Num of commands in current frag so far.
m4_pushdef([_AC_SED_DELIM_NUM], 0)dnl Expected number of delimiters in file.
m4_pushdef([_AC_SED_FRAG], [])dnl The constant part of the current fragment.
dnl
m4_ifdef([_AC_SUBST_FILES],
[# Create sed commands to just substitute file output variables.

m4_foreach_w([_AC_Var], m4_defn([_AC_SUBST_FILES]),
[dnl End fragments at beginning of loop so that last fragment is not ended.
m4_if(m4_eval(_AC_SED_CMD_NUM + 3 > _AC_SED_CMD_LIMIT), 1,
[dnl Fragment is full and not the last one, so no need for the final un-escape.
dnl Increment fragment number.
m4_define([_AC_SED_FRAG_NUM], m4_incr(_AC_SED_FRAG_NUM))dnl
dnl Record that this fragment will need to be used.
m4_define([_AC_SED_CMDS],
  m4_defn([_AC_SED_CMDS])[| sed -f "$tmp/subs-]_AC_SED_FRAG_NUM[.sed" ])dnl
[cat >>$CONFIG_STATUS <<_ACEOF
cat >"\$tmp/subs-]_AC_SED_FRAG_NUM[.sed" <<\CEOF
/@[a-zA-Z_][a-zA-Z_0-9]*@/!b
]m4_defn([_AC_SED_FRAG])dnl
[CEOF

_ACEOF
]m4_define([_AC_SED_CMD_NUM], 2)m4_define([_AC_SED_FRAG])dnl
])dnl Last fragment ended.
m4_define([_AC_SED_CMD_NUM], m4_eval(_AC_SED_CMD_NUM + 3))dnl
m4_define([_AC_SED_FRAG],
m4_defn([_AC_SED_FRAG])dnl
[/^[	 ]*@]_AC_Var[@[	 ]*$/{
r $]_AC_Var[
d
}
])dnl
])dnl
# Remaining file output variables are in a fragment that also has non-file
# output varibles.

])
dnl
m4_define([_AC_SED_FRAG], [
]m4_defn([_AC_SED_FRAG]))dnl
m4_foreach_w([_AC_Var],
m4_ifdef([_AC_SUBST_VARS], [m4_defn([_AC_SUBST_VARS]) ])[@END@],
[m4_if(_AC_SED_DELIM_NUM, 0,
[m4_if(_AC_Var, [@END@],
[dnl The whole of the last fragment would be the final deletion of `|#_!!_#|'.
m4_define([_AC_SED_CMDS], m4_defn([_AC_SED_CMDS])[| sed 's/|#_!!_#|//g' ])],
[
ac_delim='%!_!# '
for ac_last_try in false false false false false :; do
  cat >conf$$subs.sed <<_ACEOF
])])dnl
m4_if(_AC_Var, [@END@],
      [m4_if(m4_eval(_AC_SED_CMD_NUM + 2 <= _AC_SED_CMD_LIMIT), 1,
             [m4_define([_AC_SED_FRAG], [ end]m4_defn([_AC_SED_FRAG]))])],
[m4_define([_AC_SED_CMD_NUM], m4_incr(_AC_SED_CMD_NUM))dnl
m4_define([_AC_SED_DELIM_NUM], m4_incr(_AC_SED_DELIM_NUM))dnl
_AC_Var!$_AC_Var$ac_delim
])dnl
m4_if(_AC_SED_CMD_LIMIT,
      m4_if(_AC_Var, [@END@], m4_if(_AC_SED_CMD_NUM, 2, 2, _AC_SED_CMD_LIMIT), _AC_SED_CMD_NUM),
[_ACEOF

dnl Do not use grep on conf$$subs.sed, since AIX grep has a line length limit.
  if test `sed -n "s/.*$ac_delim\$/X/p" conf$$subs.sed | grep -c X` = _AC_SED_DELIM_NUM; then
    break
  elif $ac_last_try; then
    AC_MSG_ERROR([could not make $CONFIG_STATUS])
  else
    ac_delim="$ac_delim!$ac_delim _$ac_delim!! "
  fi
done

dnl Similarly, avoid grep here too.
ac_eof=`sed -n '/^CEOF[[0-9]]*$/s/CEOF/0/p' conf$$subs.sed`
if test -n "$ac_eof"; then
  ac_eof=`echo "$ac_eof" | sort -nru | sed 1q`
  ac_eof=`expr $ac_eof + 1`
fi

dnl Increment fragment number.
m4_define([_AC_SED_FRAG_NUM], m4_incr(_AC_SED_FRAG_NUM))dnl
dnl Record that this fragment will need to be used.
m4_define([_AC_SED_CMDS],
m4_defn([_AC_SED_CMDS])[| sed -f "$tmp/subs-]_AC_SED_FRAG_NUM[.sed" ])dnl
[cat >>$CONFIG_STATUS <<_ACEOF
cat >"\$tmp/subs-]_AC_SED_FRAG_NUM[.sed" <<\CEOF$ac_eof
/@[a-zA-Z_][a-zA-Z_0-9]*@/!b]m4_defn([_AC_SED_FRAG])dnl
[_ACEOF
sed '
s/[,\\&]/\\&/g; s/@/@|#_!!_#|/g
s/^/s,@/; s/!/@,|#_!!_#|/
:n
t n
s/'"$ac_delim"'$/,g/; t
s/$/\\/; p
N; s/^.*\n//; s/[,\\&]/\\&/g; s/@/@|#_!!_#|/g; b n
' >>$CONFIG_STATUS <conf$$subs.sed
rm -f conf$$subs.sed
cat >>$CONFIG_STATUS <<_ACEOF
]m4_if(_AC_Var, [@END@],
[m4_if(m4_eval(_AC_SED_CMD_NUM + 2 > _AC_SED_CMD_LIMIT), 1,
[m4_define([_AC_SED_CMDS], m4_defn([_AC_SED_CMDS])[| sed 's/|#_!!_#|//g' ])],
[[:end
s/|#_!!_#|//g
]])])dnl
CEOF$ac_eof
_ACEOF
m4_define([_AC_SED_FRAG], [
])m4_define([_AC_SED_DELIM_NUM], 0)m4_define([_AC_SED_CMD_NUM], 2)dnl

])])dnl
dnl
m4_popdef([_AC_SED_FRAG_NUM])dnl
m4_popdef([_AC_SED_CMD_NUM])dnl
m4_popdef([_AC_SED_DELIM_NUM])dnl
m4_popdef([_AC_SED_FRAG])dnl

# VPATH may cause trouble with some makes, so we remove $(srcdir),
# ${srcdir} and @srcdir@ from VPATH if srcdir is ".", strip leading and
# trailing colons and then remove the whole line if VPATH becomes empty
# (actually we leave an empty line to preserve line numbers).
if test "x$srcdir" = x.; then
  ac_vpsub=['/^[	 ]*VPATH[	 ]*=/{
s/:*\$(srcdir):*/:/
s/:*\${srcdir}:*/:/
s/:*@srcdir@:*/:/
s/^\([^=]*=[	 ]*\):*/\1/
s/:*$//
s/^[^=]*=[	 ]*$//
}']
fi

cat >>$CONFIG_STATUS <<\_ACEOF
fi # test -n "$CONFIG_FILES"

])# _AC_OUTPUT_FILES_PREPARE


# _AC_OUTPUT_FILE
# ---------------
# Do the variable substitutions to create the Makefiles or whatever.
#
# This macro is expanded inside a here document.  If the here document is
# closed, it has to be reopened with "cat >>$CONFIG_STATUS <<\_ACEOF".
#
m4_define([_AC_OUTPUT_FILE],
[
  #
  # CONFIG_FILE
  #

AC_PROVIDE_IFELSE([AC_PROG_INSTALL],
[  case $INSTALL in
  [[\\/$]]* | ?:[[\\/]]* ) ac_INSTALL=$INSTALL ;;
  *) ac_INSTALL=$ac_top_build_prefix$INSTALL ;;
  esac
])dnl
AC_PROVIDE_IFELSE([AC_PROG_MKDIR_P],
[  ac_MKDIR_P=$MKDIR_P
  case $MKDIR_P in
  [[\\/$]]* | ?:[[\\/]]* ) ;;
  */*) ac_MKDIR_P=$ac_top_build_prefix$MKDIR_P ;;
  esac
])dnl
_ACEOF

m4_ifndef([AC_DATAROOTDIR_CHECKED],
[cat >>$CONFIG_STATUS <<\_ACEOF
# If the template does not know about datarootdir, expand it.
# FIXME: This hack should be removed a few years after 2.60.
ac_datarootdir_hack=; ac_datarootdir_seen=
m4_define([_AC_datarootdir_vars],
          [datadir, docdir, infodir, localedir, mandir])
case `sed -n '/datarootdir/ {
  p
  q
}
m4_foreach([_AC_Var], m4_defn([_AC_datarootdir_vars]),
           [/@_AC_Var@/p
])' $ac_file_inputs` in
*datarootdir*) ac_datarootdir_seen=yes;;
*@[]m4_join([@*|*@], _AC_datarootdir_vars)@*)
  AC_MSG_WARN([$ac_file_inputs seems to ignore the --datarootdir setting])
_ACEOF
cat >>$CONFIG_STATUS <<_ACEOF
  ac_datarootdir_hack='
  m4_foreach([_AC_Var], m4_defn([_AC_datarootdir_vars]),
               [s&@_AC_Var@&$_AC_Var&g
  ])dnl
  s&\\\${datarootdir}&$datarootdir&g' ;;
esac
_ACEOF
])dnl

# Neutralize VPATH when `$srcdir' = `.'.
# Shell code in configure.ac might set extrasub.
# FIXME: do we really want to maintain this feature?
cat >>$CONFIG_STATUS <<_ACEOF
  sed "$ac_vpsub
$extrasub
_ACEOF
cat >>$CONFIG_STATUS <<\_ACEOF
:t
[/@[a-zA-Z_][a-zA-Z_0-9]*@/!b]
dnl configure_input is a somewhat special, so we don't call AC_SUBST_TRACE.
s&@configure_input@&$configure_input&;t t
dnl During the transition period, this is a special case:
s&@top_builddir@&$ac_top_builddir_sub&;t t[]AC_SUBST_TRACE([top_builddir])
m4_foreach([_AC_Var], [srcdir, abs_srcdir, top_srcdir, abs_top_srcdir,
			builddir, abs_builddir,
			abs_top_builddir]AC_PROVIDE_IFELSE([AC_PROG_INSTALL], [[, INSTALL]])AC_PROVIDE_IFELSE([AC_PROG_MKDIR_P], [[, MKDIR_P]]),
	   [s&@_AC_Var@&$ac_[]_AC_Var&;t t[]AC_SUBST_TRACE(_AC_Var)
])dnl
m4_ifndef([AC_DATAROOTDIR_CHECKED], [$ac_datarootdir_hack
])dnl
" $ac_file_inputs m4_defn([_AC_SED_CMDS])>$tmp/out

m4_ifndef([AC_DATAROOTDIR_CHECKED],
[test -z "$ac_datarootdir_hack$ac_datarootdir_seen" &&
  { ac_out=`sed -n '/\${datarootdir}/p' "$tmp/out"`; test -n "$ac_out"; } &&
  { ac_out=`sed -n '/^[[	 ]]*datarootdir[[	 ]]*:*=/p' "$tmp/out"`; test -z "$ac_out"; } &&
  AC_MSG_WARN([$ac_file contains a reference to the variable `datarootdir'
which seems to be undefined.  Please make sure it is defined.])
])dnl

  rm -f "$tmp/stdin"
  case $ac_file in
  -) cat "$tmp/out"; rm -f "$tmp/out";;
  *) rm -f "$ac_file"; mv "$tmp/out" $ac_file;;
  esac
dnl This would break Makefile dependencies:
dnl  if diff $ac_file "$tmp/out" >/dev/null 2>&1; then
dnl    echo "$ac_file is unchanged"
dnl  else
dnl     rm -f $ac_file; mv "$tmp/out" $ac_file
dnl  fi
])# _AC_OUTPUT_FILE




## ----------------------- ##
## Configuration headers.  ##
## ----------------------- ##


# AC_CONFIG_HEADERS(HEADERS..., [COMMANDS], [INIT-CMDS])
# ------------------------------------------------------
# Specify that the HEADERS are to be created by instantiation of the
# AC_DEFINEs.
#
AC_DEFUN([AC_CONFIG_HEADERS], [_AC_CONFIG_FOOS([HEADERS], $@)])


# AC_CONFIG_HEADER(HEADER-TO-CREATE ...)
# --------------------------------------
# FIXME: Make it obsolete?
AC_DEFUN([AC_CONFIG_HEADER],
[AC_CONFIG_HEADERS([$1])])


# _AC_OUTPUT_HEADER
# -----------------
#
# Output the code which instantiates the `config.h' files from their
# `config.h.in'.
#
# This macro is expanded inside a here document.  If the here document is
# closed, it has to be reopened with "cat >>$CONFIG_STATUS <<\_ACEOF".
#
m4_define([_AC_OUTPUT_HEADER],
[
  #
  # CONFIG_HEADER
  #
_ACEOF

# Transform confdefs.h into a sed script `conftest.defines', that
# substitutes the proper values into config.h.in to produce config.h.
rm -f conftest.defines conftest.tail
# First, append a space to every undef/define line, to ease matching.
echo 's/$/ /' >conftest.defines
# Then, protect against being on the right side of a sed subst, or in
# an unquoted here document, in config.status.  If some macros were
# called several times there might be several #defines for the same
# symbol, which is useless.  But do not sort them, since the last
# AC_DEFINE must be honored.
dnl
dnl Quote, for `[ ]' and `define'.
[ac_word_re=[_$as_cr_Letters][_$as_cr_alnum]*
# These sed commands are passed to sed as "A NAME B PARAMS C VALUE D", where
# NAME is the cpp macro being defined, VALUE is the value it is being given.
# PARAMS is the parameter list in the macro definition--in most cases, it's
# just an empty string.
ac_dA='s,^\\([	 #]*\\)[^	 ]*\\([	 ]*'
ac_dB='\\)[	 (].*,\\1define\\2'
ac_dC=' '
ac_dD=' ,']
dnl ac_dD used to contain `;t' at the end, but that was both slow and incorrect.
dnl 1) Since the script must be broken into chunks containing 100 commands,
dnl the extra command meant extra calls to sed.
dnl 2) The code was incorrect: in the unusual case where a symbol has multiple
dnl different AC_DEFINEs, the last one should be honored.
dnl
dnl ac_dB works because every line has a space appended.  ac_dD reinserts
dnl the space, because some symbol may have been AC_DEFINEd several times.
dnl
dnl The first use of ac_dA has a space prepended, so that the second
dnl use does not match the initial 's' of $ac_dA.
[
uniq confdefs.h |
  sed -n '
	t rset
	:rset
	s/^[	 ]*#[	 ]*define[	 ][	 ]*//
	t ok
	d
	:ok
	s/[\\&,]/\\&/g
	s/^\('"$ac_word_re"'\)\(([^()]*)\)[	 ]*\(.*\)/ '"$ac_dA"'\1'"$ac_dB"'\2'"${ac_dC}"'\3'"$ac_dD"'/p
	s/^\('"$ac_word_re"'\)[	 ]*\(.*\)/'"$ac_dA"'\1'"$ac_dB$ac_dC"'\2'"$ac_dD"'/p
  ' >>conftest.defines
]
# Remove the space that was appended to ease matching.
# Then replace #undef with comments.  This is necessary, for
# example, in the case of _POSIX_SOURCE, which is predefined and required
# on some systems where configure will not decide to define it.
# (The regexp can be short, since the line contains either #define or #undef.)
echo 's/ $//
[s,^[	 #]*u.*,/* & */,]' >>conftest.defines

# Break up conftest.defines:
dnl If we cared only about not exceeding line count limits, we would use this:
dnl ac_max_sed_lines=m4_eval(_AC_SED_CMD_LIMIT - 3)
dnl But in practice this can generate scripts that contain too many bytes;
dnl and this can cause obscure 'sed' failures, e.g.,
dnl http://lists.gnu.org/archive/html/bug-coreutils/2006-05/msg00127.html
dnl So instead, we use the following, which is about half the size we'd like:
ac_max_sed_lines=50
dnl In the future, let's use awk or sh instead of sed to do substitutions,
dnl since we have so many problems with sed.

# First sed command is:	 sed -f defines.sed $ac_file_inputs >"$tmp/out1"
# Second one is:	 sed -f defines.sed "$tmp/out1" >"$tmp/out2"
# Third one will be:	 sed -f defines.sed "$tmp/out2" >"$tmp/out1"
# et cetera.
ac_in='$ac_file_inputs'
ac_out='"$tmp/out1"'
ac_nxt='"$tmp/out2"'

while :
do
  # Write a here document:
  dnl Quote, for the `[ ]' and `define'.
[  cat >>$CONFIG_STATUS <<_ACEOF
    # First, check the format of the line:
    cat >"\$tmp/defines.sed" <<\\CEOF
/^[	 ]*#[	 ]*undef[	 ][	 ]*$ac_word_re[	 ]*\$/b def
/^[	 ]*#[	 ]*define[	 ][	 ]*$ac_word_re[(	 ]/b def
b
:def
_ACEOF]
  sed ${ac_max_sed_lines}q conftest.defines >>$CONFIG_STATUS
  echo 'CEOF
    sed -f "$tmp/defines.sed"' "$ac_in >$ac_out" >>$CONFIG_STATUS
  ac_in=$ac_out; ac_out=$ac_nxt; ac_nxt=$ac_in
  sed 1,${ac_max_sed_lines}d conftest.defines >conftest.tail
  grep . conftest.tail >/dev/null || break
  rm -f conftest.defines
  mv conftest.tail conftest.defines
done
rm -f conftest.defines conftest.tail

dnl Now back to your regularly scheduled config.status.
echo "ac_result=$ac_in" >>$CONFIG_STATUS
cat >>$CONFIG_STATUS <<\_ACEOF
  if test x"$ac_file" != x-; then
    echo "/* $configure_input  */" >"$tmp/config.h"
    cat "$ac_result" >>"$tmp/config.h"
    if diff $ac_file "$tmp/config.h" >/dev/null 2>&1; then
      AC_MSG_NOTICE([$ac_file is unchanged])
    else
      rm -f $ac_file
      mv "$tmp/config.h" $ac_file
    fi
  else
    echo "/* $configure_input  */"
    cat "$ac_result"
  fi
  rm -f "$tmp/out[12]"
dnl If running for Automake, be ready to perform additional
dnl commands to set up the timestamp files.
m4_ifdef([_AC_AM_CONFIG_HEADER_HOOK],
	 [_AC_AM_CONFIG_HEADER_HOOK([$ac_file])
])dnl
])# _AC_OUTPUT_HEADER



## --------------------- ##
## Configuration links.  ##
## --------------------- ##


# AC_CONFIG_LINKS(DEST:SOURCE..., [COMMANDS], [INIT-CMDS])
# --------------------------------------------------------
# Specify that config.status should establish a (symbolic if possible)
# link from TOP_SRCDIR/SOURCE to TOP_SRCDIR/DEST.
# Reject DEST=., because it is makes it hard for ./config.status
# to guess the links to establish (`./config.status .').
#
AC_DEFUN([AC_CONFIG_LINKS], [_AC_CONFIG_FOOS([LINKS], $@)])


# AC_LINK_FILES(SOURCE..., DEST...)
# ---------------------------------
# Link each of the existing files SOURCE... to the corresponding
# link name in DEST...
#
# Unfortunately we can't provide a very good autoupdate service here,
# since in `AC_LINK_FILES($from, $to)' it is possible that `$from'
# and `$to' are actually lists.  It would then be completely wrong to
# replace it with `AC_CONFIG_LINKS($to:$from).  It is possible in the
# case of literal values though, but because I don't think there is any
# interest in creating config links with literal values, no special
# mechanism is implemented to handle them.
#
# _AC_LINK_FILES_CNT is used to be robust to multiple calls.
AU_DEFUN([AC_LINK_FILES],
[m4_if($#, 2, ,
       [m4_fatal([$0: incorrect number of arguments])])dnl
m4_define_default([_AC_LINK_FILES_CNT], 0)dnl
m4_define([_AC_LINK_FILES_CNT], m4_incr(_AC_LINK_FILES_CNT))dnl
ac_sources="$1"
ac_dests="$2"
while test -n "$ac_sources"; do
  set $ac_dests; ac_dest=$[1]; shift; ac_dests=$[*]
  set $ac_sources; ac_source=$[1]; shift; ac_sources=$[*]
  [ac_config_links_]_AC_LINK_FILES_CNT="$[ac_config_links_]_AC_LINK_FILES_CNT $ac_dest:$ac_source"
done
AC_CONFIG_LINKS($[ac_config_links_]_AC_LINK_FILES_CNT)dnl
],
[It is technically impossible to `autoupdate' cleanly from AC_LINK_FILES
to AC_CONFIG_LINKS.  `autoupdate' provides a functional but inelegant
update, you should probably tune the result yourself.])# AC_LINK_FILES


# _AC_OUTPUT_LINK
# ---------------
# This macro is expanded inside a here document.  If the here document is
# closed, it has to be reopened with "cat >>$CONFIG_STATUS <<\_ACEOF".
m4_define([_AC_OUTPUT_LINK],
[
  #
  # CONFIG_LINK
  #

  AC_MSG_NOTICE([linking $srcdir/$ac_source to $ac_file])

  if test ! -r "$srcdir/$ac_source"; then
    AC_MSG_ERROR([$srcdir/$ac_source: file not found])
  fi
  rm -f "$ac_file"

  # Try a relative symlink, then a hard link, then a copy.
  case $srcdir in
  [[\\/$]]* | ?:[[\\/]]* ) ac_rel_source=$srcdir/$ac_source ;;
      *) ac_rel_source=$ac_top_build_prefix$srcdir/$ac_source ;;
  esac
  ln -s "$ac_rel_source" "$ac_file" 2>/dev/null ||
    ln "$srcdir/$ac_source" "$ac_file" 2>/dev/null ||
    cp -p "$srcdir/$ac_source" "$ac_file" ||
    AC_MSG_ERROR([cannot link or copy $srcdir/$ac_source to $ac_file])
])# _AC_OUTPUT_LINK



## ------------------------ ##
## Configuration commands.  ##
## ------------------------ ##


# AC_CONFIG_COMMANDS(NAME...,[COMMANDS], [INIT-CMDS])
# ---------------------------------------------------
#
# Specify additional commands to be run by config.status.  This
# commands must be associated with a NAME, which should be thought
# as the name of a file the COMMANDS create.
#
AC_DEFUN([AC_CONFIG_COMMANDS], [_AC_CONFIG_FOOS([COMMANDS], $@)])


# AC_OUTPUT_COMMANDS(EXTRA-CMDS, INIT-CMDS)
# -----------------------------------------
#
# Add additional commands for AC_OUTPUT to put into config.status.
#
# This macro is an obsolete version of AC_CONFIG_COMMANDS.  The only
# difficulty in mapping AC_OUTPUT_COMMANDS to AC_CONFIG_COMMANDS is
# to give a unique key.  The scheme we have chosen is `default-1',
# `default-2' etc. for each call.
#
# Unfortunately this scheme is fragile: bad things might happen
# if you update an included file and configure.ac: you might have
# clashes :(  On the other hand, I'd like to avoid weird keys (e.g.,
# depending upon __file__ or the pid).
AU_DEFUN([AC_OUTPUT_COMMANDS],
[m4_define_default([_AC_OUTPUT_COMMANDS_CNT], 0)dnl
m4_define([_AC_OUTPUT_COMMANDS_CNT], m4_incr(_AC_OUTPUT_COMMANDS_CNT))dnl
dnl Double quoted since that was the case in the original macro.
AC_CONFIG_COMMANDS([default-]_AC_OUTPUT_COMMANDS_CNT, [[$1]], [[$2]])dnl
])


# _AC_OUTPUT_COMMAND
# ------------------
# This macro is expanded inside a here document.  If the here document is
# closed, it has to be reopened with "cat >>$CONFIG_STATUS <<\_ACEOF".
m4_define([_AC_OUTPUT_COMMAND],
[  AC_MSG_NOTICE([executing $ac_file commands])
])



## -------------------------------------- ##
## Pre- and post-config.status commands.  ##
## -------------------------------------- ##


# AC_CONFIG_COMMANDS_PRE(CMDS)
# ----------------------------
# Commands to run right before config.status is created. Accumulates.
AC_DEFUN([AC_CONFIG_COMMANDS_PRE],
[m4_append([AC_OUTPUT_COMMANDS_PRE], [$1
])])


# AC_OUTPUT_COMMANDS_PRE
# ----------------------
# A *variable* in which we append all the actions that must be
# performed before *creating* config.status.  For a start, clean
# up all the LIBOBJ mess.
m4_define([AC_OUTPUT_COMMANDS_PRE],
[_AC_LIBOBJS_NORMALIZE
])


# AC_CONFIG_COMMANDS_POST(CMDS)
# -----------------------------
# Commands to run after config.status was created.  Accumulates.
AC_DEFUN([AC_CONFIG_COMMANDS_POST],
[m4_append([AC_OUTPUT_COMMANDS_POST], [$1
])])

# Initialize.
m4_define([AC_OUTPUT_COMMANDS_POST])



## ----------------------- ##
## Configuration subdirs.  ##
## ----------------------- ##


# AC_CONFIG_SUBDIRS(DIR ...)
# --------------------------
# We define two variables:
# - _AC_LIST_SUBDIRS
#   A statically built list, should contain *all* the arguments of
#   AC_CONFIG_SUBDIRS.  The final value is assigned to ac_subdirs_all in
#   the `default' section, and used for --help=recursive.
#   It is also used in _AC_CONFIG_UNIQUE.
#   It makes no sense for arguments which are sh variables.
# - subdirs
#   Shell variable built at runtime, so some of these dirs might not be
#   included, if for instance the user refused a part of the tree.
#   This is used in _AC_OUTPUT_SUBDIRS.
AC_DEFUN([AC_CONFIG_SUBDIRS],
[AC_REQUIRE([AC_CONFIG_AUX_DIR_DEFAULT])dnl
m4_foreach_w([_AC_Sub], [$1],
	     [_AC_CONFIG_UNIQUE([SUBDIRS],
				m4_bpatsubst(m4_defn([_AC_Sub]), [:.*]))])dnl
m4_append([_AC_LIST_SUBDIRS], [$1], [
])dnl
AS_LITERAL_IF([$1], [],
	      [AC_DIAGNOSE([syntax], [$0: you should use literals])])dnl
AC_SUBST([subdirs], ["$subdirs m4_normalize([$1])"])dnl
])


# _AC_OUTPUT_SUBDIRS
# ------------------
# This is a subroutine of AC_OUTPUT, but it does not go into
# config.status, rather, it is called after running config.status.
m4_define([_AC_OUTPUT_SUBDIRS],
[
#
# CONFIG_SUBDIRS section.
#
if test "$no_recursion" != yes; then

  # Remove --cache-file and --srcdir arguments so they do not pile up.
  ac_sub_configure_args=
  ac_prev=
  eval "set x $ac_configure_args"
  shift
  for ac_arg
  do
    if test -n "$ac_prev"; then
      ac_prev=
      continue
    fi
    case $ac_arg in
    -cache-file | --cache-file | --cache-fil | --cache-fi \
    | --cache-f | --cache- | --cache | --cach | --cac | --ca | --c)
      ac_prev=cache_file ;;
    -cache-file=* | --cache-file=* | --cache-fil=* | --cache-fi=* \
    | --cache-f=* | --cache-=* | --cache=* | --cach=* | --cac=* | --ca=* \
    | --c=*)
      ;;
    --config-cache | -C)
      ;;
    -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
      ac_prev=srcdir ;;
    -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
      ;;
    -prefix | --prefix | --prefi | --pref | --pre | --pr | --p)
      ac_prev=prefix ;;
    -prefix=* | --prefix=* | --prefi=* | --pref=* | --pre=* | --pr=* | --p=*)
      ;;
    *)
      case $ac_arg in
      *\'*) ac_arg=`echo "$ac_arg" | sed "s/'/'\\\\\\\\''/g"` ;;
      esac
      ac_sub_configure_args="$ac_sub_configure_args '$ac_arg'" ;;
    esac
  done

  # Always prepend --prefix to ensure using the same prefix
  # in subdir configurations.
  ac_arg="--prefix=$prefix"
  case $ac_arg in
  *\'*) ac_arg=`echo "$ac_arg" | sed "s/'/'\\\\\\\\''/g"` ;;
  esac
  ac_sub_configure_args="'$ac_arg' $ac_sub_configure_args"

  # Pass --silent
  if test "$silent" = yes; then
    ac_sub_configure_args="--silent $ac_sub_configure_args"
  fi

  ac_popdir=`pwd`
  for ac_dir in : $subdirs; do test "x$ac_dir" = x: && continue

    # Do not complain, so a configure script can configure whichever
    # parts of a large source tree are present.
    test -d "$srcdir/$ac_dir" || continue

    ac_msg="=== configuring in $ac_dir (`pwd`/$ac_dir)"
    _AS_ECHO_LOG([$ac_msg])
    _AS_ECHO([$ac_msg])
    AS_MKDIR_P(["$ac_dir"])
    _AC_SRCDIRS(["$ac_dir"])

    cd "$ac_dir"

    # Check for guested configure; otherwise get Cygnus style configure.
    if test -f "$ac_srcdir/configure.gnu"; then
      ac_sub_configure=$ac_srcdir/configure.gnu
    elif test -f "$ac_srcdir/configure"; then
      ac_sub_configure=$ac_srcdir/configure
    elif test -f "$ac_srcdir/configure.in"; then
      # This should be Cygnus configure.
      ac_sub_configure=$ac_aux_dir/configure
    else
      AC_MSG_WARN([no configuration information is in $ac_dir])
      ac_sub_configure=
    fi

    # The recursion is here.
    if test -n "$ac_sub_configure"; then
      # Make the cache file name correct relative to the subdirectory.
      case $cache_file in
      [[\\/]]* | ?:[[\\/]]* ) ac_sub_cache_file=$cache_file ;;
      *) # Relative name.
	ac_sub_cache_file=$ac_top_build_prefix$cache_file ;;
      esac

      AC_MSG_NOTICE([running $SHELL $ac_sub_configure $ac_sub_configure_args --cache-file=$ac_sub_cache_file --srcdir=$ac_srcdir])
      # The eval makes quoting arguments work.
      eval "\$SHELL \"\$ac_sub_configure\" $ac_sub_configure_args \
	   --cache-file=\"\$ac_sub_cache_file\" --srcdir=\"\$ac_srcdir\"" ||
	AC_MSG_ERROR([$ac_sub_configure failed for $ac_dir])
    fi

    cd "$ac_popdir"
  done
fi
])# _AC_OUTPUT_SUBDIRS




## -------------------------- ##
## Outputting config.status.  ##
## -------------------------- ##


# AU::AC_OUTPUT([CONFIG_FILES...], [EXTRA-CMDS], [INIT-CMDS])
# -----------------------------------------------------------
#
# If there are arguments given to AC_OUTPUT, dispatch them to the
# proper modern macros.
AU_DEFUN([AC_OUTPUT],
[m4_ifvaln([$1],
	   [AC_CONFIG_FILES([$1])])dnl
m4_ifvaln([$2$3],
	  [AC_CONFIG_COMMANDS(default, [$2], [$3])])dnl
[AC_OUTPUT]])


# AC_OUTPUT([CONFIG_FILES...], [EXTRA-CMDS], [INIT-CMDS])
# -------------------------------------------------------
# The big finish.
# Produce config.status, config.h, and links; and configure subdirs.
#
m4_define([AC_OUTPUT],
[dnl Dispatch the extra arguments to their native macros.
m4_ifvaln([$1],
	  [AC_CONFIG_FILES([$1])])dnl
m4_ifvaln([$2$3],
	  [AC_CONFIG_COMMANDS(default, [$2], [$3])])dnl
m4_ifval([$1$2$3],
	 [AC_DIAGNOSE([obsolete],
		      [$0 should be used without arguments.
You should run autoupdate.])])dnl
AC_CACHE_SAVE

test "x$prefix" = xNONE && prefix=$ac_default_prefix
# Let make expand exec_prefix.
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'

m4_ifdef([_AC_SEEN_CONFIG(HEADERS)], [DEFS=-DHAVE_CONFIG_H], [AC_OUTPUT_MAKE_DEFS()])

dnl Commands to run before creating config.status.
AC_OUTPUT_COMMANDS_PRE()dnl

: ${CONFIG_STATUS=./config.status}
ac_clean_files_save=$ac_clean_files
ac_clean_files="$ac_clean_files $CONFIG_STATUS"
_AC_OUTPUT_CONFIG_STATUS()dnl
ac_clean_files=$ac_clean_files_save

dnl Commands to run after config.status was created
AC_OUTPUT_COMMANDS_POST()dnl

# configure is writing to config.log, and then calls config.status.
# config.status does its own redirection, appending to config.log.
# Unfortunately, on DOS this fails, as config.log is still kept open
# by configure, so config.status won't be able to write to it; its
# output is simply discarded.  So we exec the FD to /dev/null,
# effectively closing config.log, so it can be properly (re)opened and
# appended to by config.status.  When coming back to configure, we
# need to make the FD available again.
if test "$no_create" != yes; then
  ac_cs_success=:
  ac_config_status_args=
  test "$silent" = yes &&
    ac_config_status_args="$ac_config_status_args --quiet"
  exec AS_MESSAGE_LOG_FD>/dev/null
  $SHELL $CONFIG_STATUS $ac_config_status_args || ac_cs_success=false
  exec AS_MESSAGE_LOG_FD>>config.log
  # Use ||, not &&, to avoid exiting from the if with $? = 1, which
  # would make configure fail if this is the last instruction.
  $ac_cs_success || AS_EXIT([1])
fi
dnl config.status should not do recursion.
AC_PROVIDE_IFELSE([AC_CONFIG_SUBDIRS], [_AC_OUTPUT_SUBDIRS()])dnl
])# AC_OUTPUT


# _AC_OUTPUT_CONFIG_STATUS
# ------------------------
# Produce config.status.  Called by AC_OUTPUT.
# Pay special attention not to have too long here docs: some old
# shells die.  Unfortunately the limit is not known precisely...
m4_define([_AC_OUTPUT_CONFIG_STATUS],
[AC_MSG_NOTICE([creating $CONFIG_STATUS])
dnl AS_MESSAGE_LOG_FD is not available yet:
m4_rename([AS_MESSAGE_LOG_FD], [_AC_save_AS_MESSAGE_LOG_FD])dnl
cat >$CONFIG_STATUS <<_ACEOF
#! $SHELL
# Generated by $as_me.
# Run this file to recreate the current configuration.
# Compiler output produced by configure, useful for debugging
# configure, is in config.log if it exists.

debug=false
ac_cs_recheck=false
ac_cs_silent=false
SHELL=\${CONFIG_SHELL-$SHELL}
_ACEOF

cat >>$CONFIG_STATUS <<\_ACEOF
AS_SHELL_SANITIZE
dnl Watch out, this is directly the initializations, do not use
dnl AS_PREPARE, otherwise you'd get it output in the initialization
dnl of configure, not config.status.
_AS_PREPARE
exec AS_MESSAGE_FD>&1

# Save the log message, to keep $[0] and so on meaningful, and to
# report actual input values of CONFIG_FILES etc. instead of their
# values after options handling.
ac_log="
This file was extended by m4_ifset([AC_PACKAGE_NAME], [AC_PACKAGE_NAME ])dnl
$as_me[]m4_ifset([AC_PACKAGE_VERSION], [ AC_PACKAGE_VERSION]), which was
generated by m4_PACKAGE_STRING.  Invocation command line was

  CONFIG_FILES    = $CONFIG_FILES
  CONFIG_HEADERS  = $CONFIG_HEADERS
  CONFIG_LINKS    = $CONFIG_LINKS
  CONFIG_COMMANDS = $CONFIG_COMMANDS
  $ $[0] $[@]

on `(hostname || uname -n) 2>/dev/null | sed 1q`
"

_ACEOF

cat >>$CONFIG_STATUS <<_ACEOF
# Files that config.status was made for.
m4_ifdef([_AC_SEEN_CONFIG(FILES)],
[config_files="$ac_config_files"
])dnl
m4_ifdef([_AC_SEEN_CONFIG(HEADERS)],
[config_headers="$ac_config_headers"
])dnl
m4_ifdef([_AC_SEEN_CONFIG(LINKS)],
[config_links="$ac_config_links"
])dnl
m4_ifdef([_AC_SEEN_CONFIG(COMMANDS)],
[config_commands="$ac_config_commands"
])dnl

_ACEOF

cat >>$CONFIG_STATUS <<\_ACEOF
ac_cs_usage="\
\`$as_me' instantiates files from templates according to the
current configuration.

Usage: $[0] [[OPTIONS]] [[FILE]]...

  -h, --help       print this help, then exit
  -V, --version    print version number and configuration settings, then exit
  -q, --quiet      do not print progress messages
  -d, --debug      don't remove temporary files
      --recheck    update $as_me by reconfiguring in the same conditions
m4_ifdef([_AC_SEEN_CONFIG(FILES)],
[[  --file=FILE[:TEMPLATE]
		   instantiate the configuration file FILE
]])dnl
m4_ifdef([_AC_SEEN_CONFIG(HEADERS)],
[[  --header=FILE[:TEMPLATE]
		   instantiate the configuration header FILE
]])dnl

m4_ifdef([_AC_SEEN_CONFIG(FILES)],
[Configuration files:
$config_files

])dnl
m4_ifdef([_AC_SEEN_CONFIG(HEADERS)],
[Configuration headers:
$config_headers

])dnl
m4_ifdef([_AC_SEEN_CONFIG(LINKS)],
[Configuration links:
$config_links

])dnl
m4_ifdef([_AC_SEEN_CONFIG(COMMANDS)],
[Configuration commands:
$config_commands

])dnl
Report bugs to <bug-autoconf@gnu.org>."

_ACEOF
cat >>$CONFIG_STATUS <<_ACEOF
ac_cs_version="\\
m4_ifset([AC_PACKAGE_NAME], [AC_PACKAGE_NAME ])config.status[]dnl
m4_ifset([AC_PACKAGE_VERSION], [ AC_PACKAGE_VERSION])
configured by $[0], generated by m4_PACKAGE_STRING,
  with options \\"`echo "$ac_configure_args" | sed 's/^ //; s/[[\\""\`\$]]/\\\\&/g'`\\"

Copyright (C) 2006 Free Software Foundation, Inc.
This config.status script is free software; the Free Software Foundation
gives unlimited permission to copy, distribute and modify it."

ac_pwd='$ac_pwd'
srcdir='$srcdir'
AC_PROVIDE_IFELSE([AC_PROG_INSTALL],
[INSTALL='$INSTALL'
])dnl
AC_PROVIDE_IFELSE([AC_PROG_MKDIR_P],
[MKDIR_P='$MKDIR_P'
])dnl
_ACEOF

cat >>$CONFIG_STATUS <<\_ACEOF
# If no file are specified by the user, then we need to provide default
# value.  By we need to know if files were specified by the user.
ac_need_defaults=:
while test $[#] != 0
do
  case $[1] in
  --*=*)
    ac_option=`expr "X$[1]" : 'X\([[^=]]*\)='`
    ac_optarg=`expr "X$[1]" : 'X[[^=]]*=\(.*\)'`
    ac_shift=:
    ;;
  *)
    ac_option=$[1]
    ac_optarg=$[2]
    ac_shift=shift
    ;;
  esac

  case $ac_option in
  # Handling of the options.
  -recheck | --recheck | --rechec | --reche | --rech | --rec | --re | --r)
    ac_cs_recheck=: ;;
  --version | --versio | --versi | --vers | --ver | --ve | --v | -V )
    echo "$ac_cs_version"; exit ;;
  --debug | --debu | --deb | --de | --d | -d )
    debug=: ;;
m4_ifdef([_AC_SEEN_CONFIG(FILES)], [dnl
  --file | --fil | --fi | --f )
    $ac_shift
    CONFIG_FILES="$CONFIG_FILES $ac_optarg"
    ac_need_defaults=false;;
])dnl
m4_ifdef([_AC_SEEN_CONFIG(HEADERS)], [dnl
  --header | --heade | --head | --hea )
    $ac_shift
    CONFIG_HEADERS="$CONFIG_HEADERS $ac_optarg"
    ac_need_defaults=false;;
  --he | --h)
    # Conflict between --help and --header
    AC_MSG_ERROR([ambiguous option: $[1]
Try `$[0] --help' for more information.]);;
], [  --he | --h |])dnl
  --help | --hel | -h )
    echo "$ac_cs_usage"; exit ;;
  -q | -quiet | --quiet | --quie | --qui | --qu | --q \
  | -silent | --silent | --silen | --sile | --sil | --si | --s)
    ac_cs_silent=: ;;

  # This is an error.
  -*) AC_MSG_ERROR([unrecognized option: $[1]
Try `$[0] --help' for more information.]) ;;

  *) ac_config_targets="$ac_config_targets $[1]"
     ac_need_defaults=false ;;

  esac
  shift
done

ac_configure_extra_args=

if $ac_cs_silent; then
  exec AS_MESSAGE_FD>/dev/null
  ac_configure_extra_args="$ac_configure_extra_args --silent"
fi

_ACEOF
cat >>$CONFIG_STATUS <<_ACEOF
dnl Check this before opening the log, to avoid a bug on MinGW,
dnl which prohibits the recursive instance from truncating an open log.
if \$ac_cs_recheck; then
  echo "running CONFIG_SHELL=$SHELL $SHELL $[0] "$ac_configure_args \$ac_configure_extra_args " --no-create --no-recursion" >&AS_MESSAGE_FD
  CONFIG_SHELL=$SHELL
  export CONFIG_SHELL
  exec $SHELL "$[0]"$ac_configure_args \$ac_configure_extra_args --no-create --no-recursion
fi

_ACEOF
cat >>$CONFIG_STATUS <<\_ACEOF
dnl Open the log:
m4_rename([_AC_save_AS_MESSAGE_LOG_FD], [AS_MESSAGE_LOG_FD])dnl
exec AS_MESSAGE_LOG_FD>>config.log
{
  echo
  AS_BOX([Running $as_me.])
  echo "$ac_log"
} >&AS_MESSAGE_LOG_FD

_ACEOF
cat >>$CONFIG_STATUS <<_ACEOF
m4_ifdef([_AC_OUTPUT_COMMANDS_INIT],
[#
# INIT-COMMANDS
#
_AC_OUTPUT_COMMANDS_INIT
])dnl
_ACEOF

cat >>$CONFIG_STATUS <<\_ACEOF

# Handling of arguments.
for ac_config_target in $ac_config_targets
do
  case $ac_config_target in
m4_ifdef([_AC_LIST_TAGS], [_AC_LIST_TAGS])
  *) AC_MSG_ERROR([invalid argument: $ac_config_target]);;
  esac
done

m4_ifdef([_AC_SEEN_CONFIG(ANY)], [_AC_OUTPUT_MAIN_LOOP])[]dnl

AS_EXIT(0)
_ACEOF
chmod +x $CONFIG_STATUS
])# _AC_OUTPUT_CONFIG_STATUS

# _AC_OUTPUT_MAIN_LOOP
# --------------------
# The main loop in $CONFIG_STATUS.
#
# This macro is expanded inside a here document.  If the here document is
# closed, it has to be reopened with "cat >>$CONFIG_STATUS <<\_ACEOF".
#
AC_DEFUN([_AC_OUTPUT_MAIN_LOOP],
[
# If the user did not use the arguments to specify the items to instantiate,
# then the envvar interface is used.  Set only those that are not.
# We use the long form for the default assignment because of an extremely
# bizarre bug on SunOS 4.1.3.
if $ac_need_defaults; then
m4_ifdef([_AC_SEEN_CONFIG(FILES)],
[  test "${CONFIG_FILES+set}" = set || CONFIG_FILES=$config_files
])dnl
m4_ifdef([_AC_SEEN_CONFIG(HEADERS)],
[  test "${CONFIG_HEADERS+set}" = set || CONFIG_HEADERS=$config_headers
])dnl
m4_ifdef([_AC_SEEN_CONFIG(LINKS)],
[  test "${CONFIG_LINKS+set}" = set || CONFIG_LINKS=$config_links
])dnl
m4_ifdef([_AC_SEEN_CONFIG(COMMANDS)],
[  test "${CONFIG_COMMANDS+set}" = set || CONFIG_COMMANDS=$config_commands
])dnl
fi

# Have a temporary directory for convenience.  Make it in the build tree
# simply because there is no reason against having it here, and in addition,
# creating and moving files from /tmp can sometimes cause problems.
# Hook for its removal unless debugging.
# Note that there is a small window in which the directory will not be cleaned:
# after its creation but before its name has been assigned to `$tmp'.
$debug ||
{
  tmp=
  trap 'exit_status=$?
  { test -z "$tmp" || test ! -d "$tmp" || rm -fr "$tmp"; } && exit $exit_status
' 0
  trap 'AS_EXIT([1])' 1 2 13 15
}
dnl The comment above AS_TMPDIR says at most 4 chars are allowed.
AS_TMPDIR([conf], [.])

m4_ifdef([_AC_SEEN_CONFIG(FILES)], [_AC_OUTPUT_FILES_PREPARE])[]dnl

for ac_tag in[]dnl
  m4_ifdef([_AC_SEEN_CONFIG(FILES)],    [:F $CONFIG_FILES])[]dnl
  m4_ifdef([_AC_SEEN_CONFIG(HEADERS)],  [:H $CONFIG_HEADERS])[]dnl
  m4_ifdef([_AC_SEEN_CONFIG(LINKS)],    [:L $CONFIG_LINKS])[]dnl
  m4_ifdef([_AC_SEEN_CONFIG(COMMANDS)], [:C $CONFIG_COMMANDS])
do
  case $ac_tag in
  :[[FHLC]]) ac_mode=$ac_tag; continue;;
  esac
  case $ac_mode$ac_tag in
  :[[FHL]]*:*);;
  :L* | :C*:*) AC_MSG_ERROR([Invalid tag $ac_tag.]);;
  :[[FH]]-) ac_tag=-:-;;
  :[[FH]]*) ac_tag=$ac_tag:$ac_tag.in;;
  esac
  ac_save_IFS=$IFS
  IFS=:
  set x $ac_tag
  IFS=$ac_save_IFS
  shift
  ac_file=$[1]
  shift

  case $ac_mode in
  :L) ac_source=$[1];;
  :[[FH]])
    ac_file_inputs=
    for ac_f
    do
      case $ac_f in
      -) ac_f="$tmp/stdin";;
      *) # Look for the file first in the build tree, then in the source tree
	 # (if the path is not absolute).  The absolute path cannot be DOS-style,
	 # because $ac_f cannot contain `:'.
	 test -f "$ac_f" ||
	   case $ac_f in
	   [[\\/$]]*) false;;
	   *) test -f "$srcdir/$ac_f" && ac_f="$srcdir/$ac_f";;
	   esac ||
	   AC_MSG_ERROR([cannot find input file: $ac_f]);;
      esac
      ac_file_inputs="$ac_file_inputs $ac_f"
    done

    # Let's still pretend it is `configure' which instantiates (i.e., don't
    # use $as_me), people would be surprised to read:
    #    /* config.h.  Generated by config.status.  */
    configure_input="Generated from "`IFS=:
	  echo $[*] | sed ['s|^[^:]*/||;s|:[^:]*/|, |g']`" by configure."
    if test x"$ac_file" != x-; then
      configure_input="$ac_file.  $configure_input"
      AC_MSG_NOTICE([creating $ac_file])
    fi

    case $ac_tag in
    *:-:* | *:-) cat >"$tmp/stdin";;
    esac
    ;;
  esac

  ac_dir=`AS_DIRNAME(["$ac_file"])`
  AS_MKDIR_P(["$ac_dir"])
  _AC_SRCDIRS(["$ac_dir"])

  case $ac_mode in
  m4_ifdef([_AC_SEEN_CONFIG(FILES)],    [:F)_AC_OUTPUT_FILE ;;])
  m4_ifdef([_AC_SEEN_CONFIG(HEADERS)],  [:H)_AC_OUTPUT_HEADER ;;])
  m4_ifdef([_AC_SEEN_CONFIG(LINKS)],    [:L)_AC_OUTPUT_LINK ;;])
  m4_ifdef([_AC_SEEN_CONFIG(COMMANDS)], [:C)_AC_OUTPUT_COMMAND ;;])
  esac

dnl Some shells don't like empty case/esac
m4_ifdef([_AC_LIST_TAG_COMMANDS], [
  case $ac_file$ac_mode in
_AC_LIST_TAG_COMMANDS
  esac
])dnl
done # for ac_tag

])# _AC_OUTPUT_MAIN_LOOP


# AC_OUTPUT_MAKE_DEFS
# -------------------
# Set the DEFS variable to the -D options determined earlier.
# This is a subroutine of AC_OUTPUT.
# It is called inside configure, outside of config.status.
m4_define([AC_OUTPUT_MAKE_DEFS],
[[# Transform confdefs.h into DEFS.
# Protect against shell expansion while executing Makefile rules.
# Protect against Makefile macro expansion.
#
# If the first sed substitution is executed (which looks for macros that
# take arguments), then branch to the quote section.  Otherwise,
# look for a macro that doesn't take arguments.
ac_script='
t clear
:clear
s/^[	 ]*#[	 ]*define[	 ][	 ]*\([^	 (][^	 (]*([^)]*)\)[	 ]*\(.*\)/-D\1=\2/g
t quote
s/^[	 ]*#[	 ]*define[	 ][	 ]*\([^	 ][^	 ]*\)[	 ]*\(.*\)/-D\1=\2/g
t quote
b any
:quote
s/[	 `~#$^&*(){}\\|;'\''"<>?]/\\&/g
s/\[/\\&/g
s/\]/\\&/g
s/\$/$$/g
H
:any
${
	g
	s/^\n//
	s/\n/ /g
	p
}
'
DEFS=`sed -n "$ac_script" confdefs.h`
]])# AC_OUTPUT_MAKE_DEFS
