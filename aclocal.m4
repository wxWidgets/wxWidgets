dnl ---------------------------------------------------------------------------
dnl
dnl Purpose: Cursom macros for autoconf configure script.
dnl Authoer: Vadim Zeitlin
dnl Created: 26.05.99
dnl Version: $Id$
dnl ---------------------------------------------------------------------------

dnl ===========================================================================
dnl include automake macros
dnl ===========================================================================

dnl Like AC_CONFIG_HEADER, but automatically create stamp file.
AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_DEFINE_UNQUOTED(VERSION, "$VERSION"))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])


# serial 1

AC_DEFUN(AM_PROG_INSTALL,
[AC_REQUIRE([AC_PROG_INSTALL])
test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL_PROGRAM}'
AC_SUBST(INSTALL_SCRIPT)dnl
])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])


dnl AM_PROG_LEX
dnl Look for flex, lex or missing, then run AC_PROG_LEX and AC_DECL_YYTEXT
AC_DEFUN(AM_PROG_LEX,
[missing_dir=ifelse([$1],,`cd $ac_aux_dir && pwd`,$1)
AC_CHECK_PROGS(LEX, flex lex, "$missing_dir/missing flex")
AC_PROG_LEX
AC_DECL_YYTEXT])


# serial 29 AM_PROG_LIBTOOL
AC_DEFUN(AM_PROG_LIBTOOL,
[AC_REQUIRE([AM_ENABLE_SHARED])dnl
AC_REQUIRE([AM_ENABLE_STATIC])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
AC_REQUIRE([AC_PROG_RANLIB])dnl
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AM_PROG_LD])dnl
AC_REQUIRE([AM_PROG_NM])dnl
AC_REQUIRE([AM_SYS_NM_PARSE])dnl
AC_REQUIRE([AM_SYS_SYMBOL_UNDERSCORE])dnl
AC_REQUIRE([AC_PROG_LN_S])dnl
dnl
# Always use our own libtool.
LIBTOOL='$(SHELL) $(top_builddir)/libtool'
AC_SUBST(LIBTOOL)dnl

# Check for any special flags to pass to ltconfig.
libtool_flags=
test "$enable_shared" = no && libtool_flags="$libtool_flags --disable-shared"
test "$enable_static" = no && libtool_flags="$libtool_flags --disable-static"
test "$silent" = yes && libtool_flags="$libtool_flags --silent"
test "$ac_cv_prog_gcc" = yes && libtool_flags="$libtool_flags --with-gcc"
test "$ac_cv_prog_gnu_ld" = yes && libtool_flags="$libtool_flags --with-gnu-ld"

# Some flags need to be propagated to the compiler or linker for good
# libtool support.
case "$host" in
*-*-irix6*)
  # Find out which ABI we are using.
  echo '[#]line __oline__ "configure"' > conftest.$ac_ext
  if AC_TRY_EVAL(ac_compile); then
    case "`/usr/bin/file conftest.o`" in
    *32-bit*)
      LD="${LD-ld} -32"
      ;;
    *N32*)
      LD="${LD-ld} -n32"
      ;;
    *64-bit*)
      LD="${LD-ld} -64"
      ;;
    esac
  fi
  rm -rf conftest*
  ;;

*-*-sco3.2v5*)
  # On SCO OpenServer 5, we need -belf to get full-featured binaries.
  SAVE_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS -belf"
  AC_CACHE_CHECK([whether the C compiler needs -belf], lt_cv_cc_needs_belf,
    [AC_TRY_LINK([],[],[lt_cv_cc_needs_belf=yes],[lt_cv_cc_needs_belf=no])])
  if test x"$lt_cv_cc_needs_belf" != x"yes"; then
    # this is probably gcc 2.8.0, egcs 1.0 or newer; no need for -belf
    CFLAGS="$SAVE_CFLAGS"
  fi
  ;;

*-*-cygwin32*)
  AM_SYS_LIBTOOL_CYGWIN32
  ;;

esac

# enable the --disable-libtool-lock switch

AC_ARG_ENABLE(libtool-lock,
[  --disable-libtool-lock  force libtool not to do file locking],
need_locks=$enableval,
need_locks=yes)

if test x"$need_locks" = xno; then
  libtool_flags="$libtool_flags --disable-lock"
fi


# Actually configure libtool.  ac_aux_dir is where install-sh is found.
CC="$CC" CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" \
LD="$LD" NM="$NM" RANLIB="$RANLIB" LN_S="$LN_S" \
DLLTOOL="$DLLTOOL" AS="$AS" \
${CONFIG_SHELL-/bin/sh} $ac_aux_dir/ltconfig --no-reexec \
$libtool_flags --no-verify $ac_aux_dir/ltmain.sh $host \
|| AC_MSG_ERROR([libtool configure failed])

# Redirect the config.log output again, so that the ltconfig log is not
# clobbered by the next message.
exec 5>>./config.log
])

# AM_ENABLE_SHARED - implement the --enable-shared flag
# Usage: AM_ENABLE_SHARED[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AM_ENABLE_SHARED,
[define([AM_ENABLE_SHARED_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(shared,
changequote(<<, >>)dnl
<<  --enable-shared[=PKGS]  build shared libraries [default=>>AM_ENABLE_SHARED_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_shared=yes ;;
no) enable_shared=no ;;
*)
  enable_shared=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_shared=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_shared=AM_ENABLE_SHARED_DEFAULT)dnl
])

# AM_DISABLE_SHARED - set the default shared flag to --disable-shared
AC_DEFUN(AM_DISABLE_SHARED,
[AM_ENABLE_SHARED(no)])

# AM_DISABLE_STATIC - set the default static flag to --disable-static
AC_DEFUN(AM_DISABLE_STATIC,
[AM_ENABLE_STATIC(no)])

# AM_ENABLE_STATIC - implement the --enable-static flag
# Usage: AM_ENABLE_STATIC[(DEFAULT)]
#   Where DEFAULT is either `yes' or `no'.  If omitted, it defaults to
#   `yes'.
AC_DEFUN(AM_ENABLE_STATIC,
[define([AM_ENABLE_STATIC_DEFAULT], ifelse($1, no, no, yes))dnl
AC_ARG_ENABLE(static,
changequote(<<, >>)dnl
<<  --enable-static[=PKGS]  build static libraries [default=>>AM_ENABLE_STATIC_DEFAULT],
changequote([, ])dnl
[p=${PACKAGE-default}
case "$enableval" in
yes) enable_static=yes ;;
no) enable_static=no ;;
*)
  enable_static=no
  # Look at the argument we got.  We use all the common list separators.
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:,"
  for pkg in $enableval; do
    if test "X$pkg" = "X$p"; then
      enable_static=yes
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac],
enable_static=AM_ENABLE_STATIC_DEFAULT)dnl
])


# AM_PROG_LD - find the path to the GNU or non-GNU linker
AC_DEFUN(AM_PROG_LD,
[AC_ARG_WITH(gnu-ld,
[  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]],
test "$withval" = no || with_gnu_ld=yes, with_gnu_ld=no)
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl
ac_prog=ld
if test "$ac_cv_prog_gcc" = yes; then
  # Check if gcc -print-prog-name=ld gives a path.
  AC_MSG_CHECKING([for ld used by GCC])
  ac_prog=`($CC -print-prog-name=ld) 2>&5`
  case "$ac_prog" in
    # Accept absolute paths.
changequote(,)dnl
    /* | [A-Za-z]:/*)
      # Canonicalize the path of ld
      re_direlt='/[^/][^/]*/\.\./'
      sub_uncdrive='s%^\([A-Za-z]\):/%//\1/%'
changequote([,])dnl
      while echo $ac_prog | grep "$re_direlt" > /dev/null 2>&1; do
        ac_prog=`echo $ac_prog| sed "s%$re_direlt%/%"`
      done
      case "$host_os" in
      cygwin*)
        # Convert to a UNC path for cygwin
        test -z "$LD" && LD=`echo X$ac_prog | $Xsed -e "$sub_uncdrive"`
	;;
      *)
        test -z "$LD" && LD="$ac_prog"
	;;
      esac
      ;;
    ##
    ## FIXME:  The code fails later on if we try to use an $LD with
    ##         '\\' path separators.
    ##
changequote(,)dnl
    [A-Za-z]:[\\]*)
      # Canonicalize the path of ld
      re_direlt='\\[^\\][^\\]*\\\.\.\(\\\)'
      sub_uncdrive='s%^\([A-Za-z]\):\\%//\1/%'
changequote([,])dnl
      sub_uncdir='s%\\%/%g'
      while echo $ac_prog | grep "$re_direlt" > /dev/null 2>&1; do
        ac_prog=`echo $ac_prog| sed "s%$re_direlt%\1%"`
      done
      case "$host_os" in
      cygwin*)
        # Convert to a UNC path for cygwin
        test -z "$LD" && LD=`echo X$ac_prog | sed -e 's%^X%%' -e "$sub_uncdrive" -e "$sub_uncdir"`
	;;
      *)
        test -z "$LD" && LD="$ac_prog"
	;;
      esac
      ;;
  "")
    # If it fails, then pretend we aren't using GCC.
    ac_prog=ld
    ;;
  *)
    # If it is relative, then search for the first ld in PATH.
    with_gnu_ld=unknown
    ;;
  esac
elif test "$with_gnu_ld" = yes; then
  AC_MSG_CHECKING([for GNU ld])
else
  AC_MSG_CHECKING([for non-GNU ld])
fi
AC_CACHE_VAL(ac_cv_path_LD,
[if test -z "$LD"; then
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in $PATH; do
    test -z "$ac_dir" && ac_dir=.
    if test -f "$ac_dir/$ac_prog"; then
      ac_cv_path_LD="$ac_dir/$ac_prog"
      # Check to see if the program is GNU ld.  I'd rather use --version,
      # but apparently some GNU ld's only accept -v.
      # Break only if it was the GNU/non-GNU ld that we prefer.
      if "$ac_cv_path_LD" -v 2>&1 < /dev/null | egrep '(GNU|with BFD)' > /dev/null; then
	test "$with_gnu_ld" != no && break
      else
        test "$with_gnu_ld" != yes && break
      fi
    fi
  done
  IFS="$ac_save_ifs"
else
  ac_cv_path_LD="$LD" # Let the user override the test with a path.
fi])
LD="$ac_cv_path_LD"
if test -n "$LD"; then
  AC_MSG_RESULT($LD)
else
  AC_MSG_RESULT(no)
fi
test -z "$LD" && AC_MSG_ERROR([no acceptable ld found in \$PATH])
AC_SUBST(LD)
AM_PROG_LD_GNU
])

AC_DEFUN(AM_PROG_LD_GNU,
[AC_CACHE_CHECK([if the linker ($LD) is GNU ld], ac_cv_prog_gnu_ld,
[# I'd rather use --version here, but apparently some GNU ld's only accept -v.
if $LD -v 2>&1 </dev/null | egrep '(GNU|with BFD)' 1>&5; then
  ac_cv_prog_gnu_ld=yes
else
  ac_cv_prog_gnu_ld=no
fi])
])

# AM_PROG_NM - find the path to a BSD-compatible name lister
AC_DEFUN(AM_PROG_NM,
[AC_MSG_CHECKING([for BSD-compatible nm])
AC_CACHE_VAL(ac_cv_path_NM,
[if test -n "$NM"; then
  # Let the user override the test.
  ac_cv_path_NM="$NM"
else
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in /usr/ucb /usr/ccs/bin $PATH /bin; do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/nm; then
      # Check to see if the nm accepts a BSD-compat flag.
      # Adding the `sed 1q' prevents false positives on HP-UX, which says:
      #   nm: unknown option "B" ignored
      if ($ac_dir/nm -B /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
        ac_cv_path_NM="$ac_dir/nm -B"
      elif ($ac_dir/nm -p /dev/null 2>&1 | sed '1q'; exit 0) | egrep /dev/null >/dev/null; then
        ac_cv_path_NM="$ac_dir/nm -p"
      else
        ac_cv_path_NM="$ac_dir/nm"
      fi
      break
    fi
  done
  IFS="$ac_save_ifs"
  test -z "$ac_cv_path_NM" && ac_cv_path_NM=nm
fi])
NM="$ac_cv_path_NM"
AC_MSG_RESULT([$NM])
AC_SUBST(NM)
])

# AM_SYS_NM_PARSE - Check for command ro grab the raw symbol name followed
# by C symbol name from nm.
AC_DEFUN(AM_SYS_NM_PARSE,
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AM_PROG_NM])dnl
# Check for command to grab the raw symbol name followed by C symbol from nm.
AC_MSG_CHECKING([command to parse $NM output])
AC_CACHE_VAL(ac_cv_sys_global_symbol_pipe,
[# These are sane defaults that work on at least a few old systems.
# {They come from Ultrix.  What could be older than Ultrix?!! ;)}

changequote(,)dnl
# Character class describing NM global symbol codes.
ac_symcode='[BCDEGRSTU]'

# Regexp to match symbols that can be accessed directly from C.
ac_sympat='\([_A-Za-z][_A-Za-z0-9]*\)'

# Transform the above into a raw symbol and a C symbol.
ac_symxfrm='\1 \1'

# Define system-specific variables.
case "$host_os" in
aix*)
  ac_symcode='[BCDTU]'
  ;;
sunos* | cygwin32* | mingw32*)
  ac_sympat='_\([_A-Za-z][_A-Za-z0-9]*\)'
  ac_symxfrm='_\1 \1'
  ;;
irix*)
  # Cannot use undefined symbols on IRIX because inlined functions mess us up.
  ac_symcode='[BCDEGRST]'
  ;;
solaris*)
  ac_symcode='[BDTU]'
  ;;
esac

# If we're using GNU nm, then use its standard symbol codes.
if $NM -V 2>&1 | egrep '(GNU|with BFD)' > /dev/null; then
  ac_symcode='[ABCDGISTUW]'
fi

case "$host_os" in
cygwin32* | mingw32*)
  # We do not want undefined symbols on cygwin32.  The user must
  # arrange to define them via -l arguments.
  ac_symcode='[ABCDGISTW]'
  ;;
esac
changequote([,])dnl

# Write the raw and C identifiers.
ac_cv_sys_global_symbol_pipe="sed -n -e 's/^.* $ac_symcode $ac_sympat$/$ac_symxfrm/p'"

# Check to see that the pipe works correctly.
ac_pipe_works=no
cat > conftest.$ac_ext <<EOF
#ifdef __cplusplus
extern "C" {
#endif
char nm_test_var;
void nm_test_func(){}
#ifdef __cplusplus
}
#endif
int main(){nm_test_var='a';nm_test_func;return 0;}
EOF
if AC_TRY_EVAL(ac_compile); then
  # Now try to grab the symbols.
  ac_nlist=conftest.nm
  if AC_TRY_EVAL(NM conftest.$ac_objext \| $ac_cv_sys_global_symbol_pipe \> $ac_nlist) && test -s "$ac_nlist"; then

    # Try sorting and uniquifying the output.
    if sort "$ac_nlist" | uniq > "$ac_nlist"T; then
      mv -f "$ac_nlist"T "$ac_nlist"
      ac_wcout=`wc "$ac_nlist" 2>/dev/null`
changequote(,)dnl
      ac_count=`echo "X$ac_wcout" | sed -e 's,^X,,' -e 's/^[ 	]*\([0-9][0-9]*\).*$/\1/'`
changequote([,])dnl
      (test "$ac_count" -ge 0) 2>/dev/null || ac_count=-1
    else
      rm -f "$ac_nlist"T
      ac_count=-1
    fi

    # Make sure that we snagged all the symbols we need.
    if egrep ' nm_test_var$' "$ac_nlist" >/dev/null; then
      if egrep ' nm_test_func$' "$ac_nlist" >/dev/null; then
	cat <<EOF > conftest.c
#ifdef __cplusplus
extern "C" {
#endif

EOF
        # Now generate the symbol file.
        sed 's/^.* \(.*\)$/extern char \1;/' < "$ac_nlist" >> conftest.c

	cat <<EOF >> conftest.c
#if defined (__STDC__) && __STDC__
# define __ptr_t void *
#else
# define __ptr_t char *
#endif

/* The number of symbols in dld_preloaded_symbols, -1 if unsorted. */
int dld_preloaded_symbol_count = $ac_count;

/* The mapping between symbol names and symbols. */
struct {
  char *name;
  __ptr_t address;
}
changequote(,)dnl
dld_preloaded_symbols[] =
changequote([,])dnl
{
EOF
        sed 's/^\(.*\) \(.*\)$/  {"\1", (__ptr_t) \&\2},/' < "$ac_nlist" >> conftest.c
        cat <<\EOF >> conftest.c
  {0, (__ptr_t) 0}
};

#ifdef __cplusplus
}
#endif
EOF
        # Now try linking the two files.
        mv conftest.$ac_objext conftestm.$ac_objext
	ac_save_LIBS="$LIBS"
	ac_save_CFLAGS="$CFLAGS"
        LIBS="conftestm.$ac_objext"
	CFLAGS="$CFLAGS$no_builtin_flag"
        if AC_TRY_EVAL(ac_link) && test -s conftest; then
          ac_pipe_works=yes
        else
          echo "configure: failed program was:" >&AC_FD_CC
          cat conftest.c >&AC_FD_CC
        fi
        LIBS="$ac_save_LIBS"
	CFLAGS="$ac_save_CFLAGS"
      else
        echo "cannot find nm_test_func in $ac_nlist" >&AC_FD_CC
      fi
    else
      echo "cannot find nm_test_var in $ac_nlist" >&AC_FD_CC
    fi
  else
    echo "cannot run $ac_cv_sys_global_symbol_pipe" >&AC_FD_CC
  fi
else
  echo "$progname: failed program was:" >&AC_FD_CC
  cat conftest.c >&AC_FD_CC
fi
rm -rf conftest*

# Do not use the global_symbol_pipe unless it works.
test "$ac_pipe_works" = yes || ac_cv_sys_global_symbol_pipe=
])

ac_result=yes
if test -z "$ac_cv_sys_global_symbol_pipe"; then
   ac_result=no
fi
AC_MSG_RESULT($ac_result)
])

# AM_SYS_LIBTOOL_CYGWIN32 - find tools needed on cygwin32
AC_DEFUN(AM_SYS_LIBTOOL_CYGWIN32,
[AC_CHECK_TOOL(DLLTOOL, dlltool, false)
AC_CHECK_TOOL(AS, as, false)
])

# AM_SYS_SYMBOL_UNDERSCORE - does the compiler prefix global symbols
#                            with an underscore?
AC_DEFUN(AM_SYS_SYMBOL_UNDERSCORE,
[AC_REQUIRE([AM_PROG_NM])dnl
AC_REQUIRE([AM_SYS_NM_PARSE])dnl
AC_MSG_CHECKING([for _ prefix in compiled symbols])
AC_CACHE_VAL(ac_cv_sys_symbol_underscore,
[ac_cv_sys_symbol_underscore=no
cat > conftest.$ac_ext <<EOF
void nm_test_func(){}
int main(){nm_test_func;return 0;}
EOF
if AC_TRY_EVAL(ac_compile); then
  # Now try to grab the symbols.
  ac_nlist=conftest.nm
  if AC_TRY_EVAL(NM conftest.$ac_objext \| $ac_cv_sys_global_symbol_pipe \> $ac_nlist) && test -s "$ac_nlist"; then
    # See whether the symbols have a leading underscore.
    if egrep '^_nm_test_func' "$ac_nlist" >/dev/null; then
      ac_cv_sys_symbol_underscore=yes
    else
      if egrep '^nm_test_func ' "$ac_nlist" >/dev/null; then
        :
      else
        echo "configure: cannot find nm_test_func in $ac_nlist" >&AC_FD_CC
      fi
    fi
  else
    echo "configure: cannot run $ac_cv_sys_global_symbol_pipe" >&AC_FD_CC
  fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.c >&AC_FD_CC
fi
rm -rf conftest*
])
AC_MSG_RESULT($ac_cv_sys_symbol_underscore)
if test x$ac_cv_sys_symbol_underscore = xyes; then
  AC_DEFINE(WITH_SYMBOL_UNDERSCORE,1,
  [define if compiled symbols have a leading underscore])
fi
])


dnl ===========================================================================
dnl GKT+ version test
dnl ===========================================================================

dnl ---------------------------------------------------------------------------
dnl AM_PATH_GTK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GTK, and define GTK_CFLAGS and GTK_LIBS
dnl ---------------------------------------------------------------------------
dnl
AC_DEFUN(AM_PATH_GTK,
[dnl
dnl Get the cflags and libraries from the gtk-config script
dnl
AC_ARG_WITH(gtk-prefix,[**--with-gtk-prefix=PFX       Prefix where GTK is installed],
            gtk_config_prefix="$withval", gtk_config_prefix="")
AC_ARG_WITH(gtk-exec-prefix,[**--with-gtk-exec-prefix=PFX  Exec prefix where GTK is installed],
            gtk_config_exec_prefix="$withval", gtk_config_exec_prefix="")

  if test x$gtk_config_exec_prefix != x ; then
     gtk_config_args="$gtk_config_args --exec-prefix=$gtk_config_exec_prefix"
     if test x${GTK_CONFIG+set} != xset ; then
        GTK_CONFIG=$gtk_config_exec_prefix/bin/gtk-config
     fi
  fi
  if test x$gtk_config_prefix != x ; then
     gtk_config_args="$gtk_config_args --prefix=$gtk_config_prefix"
     if test x${GTK_CONFIG+set} != xset ; then
        GTK_CONFIG=$gtk_config_prefix/bin/gtk-config
     fi
  fi

  AC_PATH_PROG(GTK_CONFIG, gtk-config, no)
  min_gtk_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for GTK - version >= $min_gtk_version)
  no_gtk=""
  if test "$GTK_CONFIG" != "no" ; then
    GTK_CFLAGS=`$GTK_CONFIG --cflags`
    GTK_LIBS=`$GTK_CONFIG --libs`
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="$CFLAGS $GTK_CFLAGS"
    LIBS="$LIBS $GTK_LIBS"
dnl
dnl Now check if the installed GTK is sufficiently new. (Also sanity
dnl checks the results of gtk-config to some extent)
dnl
    AC_TRY_RUN([
#include <gtk/gtk.h>
#include <stdio.h>

int
main ()
{
  int major, minor, micro;

  if (sscanf("$min_gtk_version", "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtk_version");
     exit(1);
   }

   if (gtk_minor_version == 1) return FALSE;

   return !((gtk_major_version > major) ||
	    ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
	    ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)));
}
],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
     CFLAGS="$ac_save_CFLAGS"
     LIBS="$ac_save_LIBS"
  else
     no_gtk=yes
  fi
  if test "x$no_gtk" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])
  else
     AC_MSG_RESULT(no)
     GTK_CFLAGS=""
     GTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GTK_CFLAGS)
  AC_SUBST(GTK_LIBS)
])

dnl ===========================================================================
dnl macros to find the a file in the list of include/lib paths
dnl ===========================================================================

dnl ---------------------------------------------------------------------------
dnl call WX_PATH_FIND_INCLUDES(search path, header name), sets ac_find_includes
dnl to the full name of the file that was found or leaves it empty if not found
dnl ---------------------------------------------------------------------------
AC_DEFUN(WX_PATH_FIND_INCLUDES,
[
ac_find_includes=
for ac_dir in $1;
  do
    if test -f "$ac_dir/$2"; then
      ac_find_includes=$ac_dir
      break
    fi
  done
])

dnl ---------------------------------------------------------------------------
dnl call WX_PATH_FIND_LIBRARIES(search path, header name), sets ac_find_includes
dnl to the full name of the file that was found or leaves it empty if not found
dnl ---------------------------------------------------------------------------
AC_DEFUN(WX_PATH_FIND_LIBRARIES,
[
ac_find_libraries=
for ac_dir in $1;
  do
    for ac_extension in a so sl; do
      if test -f "$ac_dir/lib$2.$ac_extension"; then
        ac_find_libraries=$ac_dir
        break 2
      fi
    done
  done
])

dnl ---------------------------------------------------------------------------
dnl Path to include, already defined
dnl ---------------------------------------------------------------------------
AC_DEFUN(WX_INCLUDE_PATH_EXIST,
[
  ac_path_to_include=$1
  echo "$2" | grep "\-I$1" > /dev/null
  result=$?
  if test $result = 0; then
    ac_path_to_include=""
  else
    ac_path_to_include="-I$1"
  fi
])

dnl ---------------------------------------------------------------------------
dnl Path to link, already defined
dnl ---------------------------------------------------------------------------
AC_DEFUN(WX_LINK_PATH_EXIST,
[
  echo "$2" | grep "\-L$1" > /dev/null
  result=$?
  if test $result = 0; then
    ac_path_to_link=""
  else
    ac_path_to_link="-L$1"
  fi
])

dnl ===========================================================================
dnl C++ features test
dnl ===========================================================================

dnl ---------------------------------------------------------------------------
dnl WX_CPP_NEW_HEADERS checks whether the compiler has "new" <iostream> header
dnl or only the old <iostream.h> one - it may be generally assumed that if
dnl <iostream> exists, the other "new" headers (without .h) exist too.
dnl
dnl call WX_CPP_NEW_HEADERS(actiof-if-true, action-if-false-or-cross-compiling)
dnl ---------------------------------------------------------------------------

AC_DEFUN(WX_CPP_NEW_HEADERS,
[
  if test "$cross_compiling" = "yes"; then
    ifelse([$2], , :, [$2])
  else
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_CHECK_HEADERS(iostream)

    if test "x$HAVE_IOSTREAM" = x ; then
      ifelse([$2], , :, [$2])
    else
      ifelse([$1], , :, [$1])
    fi

    AC_LANG_RESTORE
  fi
])

dnl ---------------------------------------------------------------------------
dnl WX_CPP_BOOL checks whether the C++ compiler has a built in bool type
dnl
dnl call WX_CPP_BOOL - will define HAVE_BOOL if the compiler supports bool
dnl ---------------------------------------------------------------------------

AC_DEFUN(WX_CPP_BOOL,
[
  AC_CACHE_CHECK([if C++ compiler supports bool], wx_cv_cpp_bool,
  [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_TRY_RUN([
        int main()
        {
            bool b = true;

            return 0;
        }
      ],
      [
        AC_DEFINE(HAVE_BOOL)
        wx_cv_cpp_bool=yes
      ],
      wx_cv_cpp_bool=no,
      wx_cv_cpp_bool=no
    )

    AC_LANG_RESTORE
  ])

  if test "$wx_cv_cpp_bool" = "yes"; then
    AC_DEFINE(HAVE_BOOL)
  fi
])

dnl ---------------------------------------------------------------------------
dnl WX_CPP_SIZE_T_IS_NOT_INT checks whether size_t and int are different types,
dnl i.e. whether we may overload operator[] on its argument type
dnl ---------------------------------------------------------------------------

AC_DEFUN(WX_CPP_SIZE_T_IS_NOT_INT,
[
  AC_CACHE_CHECK([if size_t and int are different types], wx_cv_cpp_sizet_not_int,
  [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_TRY_RUN([
        #include <stdlib.h>

        class S
        {
        public:
          S(char *s) { m_s = s; }

          char operator[](size_t n) const { return m_s[n]; }
          char operator[](int n) const { return m_s[n]; }

        private:
          char *m_s;
        };

        int main()
        {
            S s("dummy");
            size_t n1 = 2;
            int n2 = 3;

            return s[n1] == s[n2];
        }
      ],
      AC_DEFINE(wxUSE_SIZE_T_STRING_OPERATOR) wx_cv_cpp_sizet_not_int=yes,
      wx_cv_cpp_sizet_not_int=no,
      wx_cv_cpp_sizet_not_int=no
    )

    AC_LANG_RESTORE
  ])
])

dnl ---------------------------------------------------------------------------
dnl a slightly better AC_C_BIGENDIAN macro which allows cross-compiling
dnl ---------------------------------------------------------------------------

AC_DEFUN(WX_C_BIGENDIAN,
[AC_CACHE_CHECK(whether byte ordering is bigendian, ac_cv_c_bigendian,
[ac_cv_c_bigendian=unknown
# See if sys/param.h defines the BYTE_ORDER macro.
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/param.h>], [
#if !BYTE_ORDER || !BIG_ENDIAN || !LITTLE_ENDIAN
 bogus endian macros
#endif], [# It does; now see whether it defined to BIG_ENDIAN or not.
AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/param.h>], [
#if BYTE_ORDER != BIG_ENDIAN
 not big endian
#endif], ac_cv_c_bigendian=yes, ac_cv_c_bigendian=no)])
if test $ac_cv_c_bigendian = unknown; then
AC_TRY_RUN([main () {
  /* Are we little or big endian?  From Harbison&Steele.  */
  union
  {
    long l;
    char c[sizeof (long)];
  } u;
  u.l = 1;
  exit (u.c[sizeof (long) - 1] == 1);
}], ac_cv_c_bigendian=no, ac_cv_c_bigendian=yes, ac_cv_c_bigendian=unknown)
fi])
if test $ac_cv_c_bigendian = unknown; then
  AC_MSG_WARN([Assuming little-endian target machine - this may be overriden by adding the line "ac_cv_c_bigendian=${ac_cv_c_bigendian='yes'}" to config.cache file])
fi
if test $ac_cv_c_bigendian = yes; then
  AC_DEFINE(WORDS_BIGENDIAN)
fi
])

dnl ---------------------------------------------------------------------------
dnl override AC_ARG_ENABLE/WITH to cache the results in .cache file
dnl ---------------------------------------------------------------------------

AC_DEFUN(WX_ARG_CACHE_INIT,
        [
          wx_arg_cache_file="./configarg.cache"
          echo "loading argument cache $wx_arg_cache_file"
          rm -f ${wx_arg_cache_file}.tmp
          touch ${wx_arg_cache_file}.tmp
          touch ${wx_arg_cache_file}
        ])

AC_DEFUN(WX_ARG_CACHE_FLUSH,
        [
          echo "saving argument cache $wx_arg_cache_file"
          mv ${wx_arg_cache_file}.tmp ${wx_arg_cache_file}
        ])

dnl this macro checks for a command line argument and caches the result
dnl usage: WX_ARG_WITH(option, helpmessage, variable-name)
AC_DEFUN(WX_ARG_WITH,
        [
          AC_MSG_CHECKING("for --with-$1")
          no_cache=0
          AC_ARG_WITH($1, $2,
                      [
                        if test "$withval" = yes; then
                          ac_cv_use_$1='$3=yes'
                        else
                          ac_cv_use_$1='$3=no'
                        fi
                      ],
                      [
                        LINE=`grep "$3" ${wx_arg_cache_file}`
                        if test "x$LINE" != x ; then
                          eval "DEFAULT_$LINE"
                        else
                          no_cache=1
                        fi

                        ac_cv_use_$1='$3='$DEFAULT_$3
                      ])

          eval "$ac_cv_use_$1"
          if test "$no_cache" != 1; then
            echo $ac_cv_use_$1 >> ${wx_arg_cache_file}.tmp
          fi

          if test "$$3" = yes; then
            AC_MSG_RESULT(yes)
          else
            AC_MSG_RESULT(no)
          fi
        ])

dnl like WX_ARG_WITH but uses AC_ARG_ENABLE instead of AC_ARG_WITH
dnl usage: WX_ARG_ENABLE(option, helpmessage, variable-name)
AC_DEFUN(WX_ARG_ENABLE,
        [
          AC_MSG_CHECKING("for --enable-$1")
          no_cache=0
          AC_ARG_ENABLE($1, $2,
                        [
                          if test "$enableval" = yes; then
                            ac_cv_use_$1='$3=yes'
                          else
                            ac_cv_use_$1='$3=no'
                          fi
                        ],
                        [
                          LINE=`grep "$3" ${wx_arg_cache_file}`
                          if test "x$LINE" != x ; then
                            eval "DEFAULT_$LINE"
                          else
                            no_cache=1
                          fi

                          ac_cv_use_$1='$3='$DEFAULT_$3
                        ])

          eval "$ac_cv_use_$1"
          if test "$no_cache" != 1; then
            echo $ac_cv_use_$1 >> ${wx_arg_cache_file}.tmp
          fi

          if test "$$3" = yes; then
            AC_MSG_RESULT(yes)
          else
            AC_MSG_RESULT(no)
          fi
        ])

