# aclocal.m4 generated automatically by aclocal 1.6.3 -*- Autoconf -*-

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl ---------------------------------------------------------------------------
dnl
dnl Macros for configure.in for wxWindows by Robert Roebling, Phil Blecker,
dnl Vadim Zeitlin and Ron Lee
dnl
dnl This script is under the wxWindows licence.
dnl
dnl Version: $Id$
dnl ---------------------------------------------------------------------------

dnl ===========================================================================
dnl macros to find the a file in the list of include/lib paths
dnl ===========================================================================

dnl ---------------------------------------------------------------------------
dnl call WX_PATH_FIND_INCLUDES(search path, header name), sets ac_find_includes
dnl to the full name of the file that was found or leaves it empty if not found
dnl ---------------------------------------------------------------------------
AC_DEFUN([WX_PATH_FIND_INCLUDES],
[
ac_find_includes=
for ac_dir in $1 /usr/include;
  do
    if test -f "$ac_dir/$2"; then
      ac_find_includes=$ac_dir
      break
    fi
  done
])

dnl ---------------------------------------------------------------------------
dnl call WX_PATH_FIND_LIBRARIES(search path, header name), sets ac_find_libraries
dnl to the full name of the file that was found or leaves it empty if not found
dnl ---------------------------------------------------------------------------
AC_DEFUN([WX_PATH_FIND_LIBRARIES],
[
ac_find_libraries=
for ac_dir in $1 /usr/lib;
  do
    for ac_extension in a so sl dylib; do
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
AC_DEFUN([WX_INCLUDE_PATH_EXIST],
[
  dnl never add -I/usr/include to the CPPFLAGS
  if test "x$1" = "x/usr/include"; then
    ac_path_to_include=""
  else
    echo "$2" | grep "\-I$1" > /dev/null
    result=$?
    if test $result = 0; then
      ac_path_to_include=""
    else
      ac_path_to_include=" -I$1"
    fi
  fi
])

dnl ---------------------------------------------------------------------------
dnl Path to link, already defined
dnl ---------------------------------------------------------------------------
AC_DEFUN([WX_LINK_PATH_EXIST],
[
  echo "$2" | grep "\-L$1" > /dev/null
  result=$?
  if test $result = 0; then
    ac_path_to_link=""
  else
    ac_path_to_link=" -L$1"
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
dnl call WX_CPP_NEW_HEADERS(actiof-if-true, action-if-false)
dnl ---------------------------------------------------------------------------

AC_DEFUN([WX_CPP_NEW_HEADERS],
[
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_CHECK_HEADERS(iostream)

    if test "$ac_cv_header_iostream" = "yes" ; then
      ifelse([$1], , :, [$1])
    else
      ifelse([$2], , :, [$2])
    fi

    AC_LANG_RESTORE
])

dnl ---------------------------------------------------------------------------
dnl WX_CPP_BOOL checks whether the C++ compiler has a built in bool type
dnl
dnl call WX_CPP_BOOL - will define HAVE_BOOL if the compiler supports bool
dnl ---------------------------------------------------------------------------

AC_DEFUN([WX_CPP_BOOL],
[
  AC_CACHE_CHECK([if C++ compiler supports bool], wx_cv_cpp_bool,
  [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    AC_TRY_COMPILE(
      [
      ],
      [
        bool b = true;

        return 0;
      ],
      [
        wx_cv_cpp_bool=yes
      ],
      [
        wx_cv_cpp_bool=no
      ]
    )

    AC_LANG_RESTORE
  ])

  if test "$wx_cv_cpp_bool" = "yes"; then
    AC_DEFINE(HAVE_BOOL)
  fi
])

dnl ---------------------------------------------------------------------------
dnl WX_CPP_EXPLICIT checks whether the C++ compiler support the explicit
dnl keyword and defines HAVE_EXPLICIT if this is the case
dnl ---------------------------------------------------------------------------

AC_DEFUN([WX_CPP_EXPLICIT],
[
  AC_CACHE_CHECK([if C++ compiler supports the explicit keyword],
                 wx_cv_explicit,
  [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    dnl do the test in 2 steps: first check that the compiler knows about the
    dnl explicit keyword at all and then verify that it really honours it
    AC_TRY_COMPILE(
      [
        class Foo { public: explicit Foo(int) {} };
      ],
      [
        return 0;
      ],
      [
        AC_TRY_COMPILE(
            [
                class Foo { public: explicit Foo(int) {} };
                static void TakeFoo(const Foo& foo) { }
            ],
            [
                TakeFoo(17);
                return 0;
            ],
            wx_cv_explicit=no,
            wx_cv_explicit=yes
        )
      ],
      wx_cv_explicit=no
    )

    AC_LANG_RESTORE
  ])

  if test "$wx_cv_explicit" = "yes"; then
    AC_DEFINE(HAVE_EXPLICIT)
  fi
])

dnl ---------------------------------------------------------------------------
dnl a slightly better AC_C_BIGENDIAN macro which allows cross-compiling
dnl ---------------------------------------------------------------------------

AC_DEFUN([WX_C_BIGENDIAN],
[AC_CACHE_CHECK([whether byte ordering is bigendian], ac_cv_c_bigendian,
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
}], [ac_cv_c_bigendian=no], [ac_cv_c_bigendian=yes], [ac_cv_c_bigendian=unknown])
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

AC_DEFUN([WX_ARG_CACHE_INIT],
        [
          wx_arg_cache_file="configarg.cache"
          echo "loading argument cache $wx_arg_cache_file"
          rm -f ${wx_arg_cache_file}.tmp
          touch ${wx_arg_cache_file}.tmp
          touch ${wx_arg_cache_file}
        ])

AC_DEFUN([WX_ARG_CACHE_FLUSH],
        [
          echo "saving argument cache $wx_arg_cache_file"
          mv ${wx_arg_cache_file}.tmp ${wx_arg_cache_file}
        ])

dnl this macro checks for a three-valued command line --with argument:
dnl   possible arguments are 'yes', 'no', 'sys', or 'builtin'
dnl usage: WX_ARG_SYS_WITH(option, helpmessage, variable-name)
AC_DEFUN([WX_ARG_SYS_WITH],
        [
          AC_MSG_CHECKING([for --with-$1])
          no_cache=0
          AC_ARG_WITH($1, [$2],
                      [
                        if test "$withval" = yes; then
                          ac_cv_use_$1='$3=yes'
                        elif test "$withval" = no; then
                          ac_cv_use_$1='$3=no'
                        elif test "$withval" = sys; then
                          ac_cv_use_$1='$3=sys'
                        elif test "$withval" = builtin; then
                          ac_cv_use_$1='$3=builtin'
                        else
                          AC_MSG_ERROR([Invalid value for --with-$1: should be yes, no, sys, or builtin])
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
          elif test "$$3" = no; then
            AC_MSG_RESULT(no)
          elif test "$$3" = sys; then
            AC_MSG_RESULT([system version])
          elif test "$$3" = builtin; then
            AC_MSG_RESULT([builtin version])
          else
            AC_MSG_ERROR([Invalid value for --with-$1: should be yes, no, sys, or builtin])
          fi
        ])

dnl this macro checks for a command line argument and caches the result
dnl usage: WX_ARG_WITH(option, helpmessage, variable-name)
AC_DEFUN([WX_ARG_WITH],
        [
          AC_MSG_CHECKING([for --with-$1])
          no_cache=0
          AC_ARG_WITH($1, [$2],
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
dnl usage: WX_ARG_ENABLE(option, helpmessage, variable-name, enablestring)
dnl
dnl enablestring is a hack and allows to show "checking for --disable-foo"
dnl message when running configure instead of the default "checking for
dnl --enable-foo" one whih is useful for the options enabled by default
AC_DEFUN([WX_ARG_ENABLE],
        [
	  enablestring=$4
          AC_MSG_CHECKING([for --${enablestring:-enable}-$1])
          no_cache=0
          AC_ARG_ENABLE($1, [$2],
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


dnl ===========================================================================
dnl Linker features test
dnl ===========================================================================

dnl ---------------------------------------------------------------------------
dnl WX_VERSIONED_SYMBOLS checks whether the linker can create versioned
dnl symbols. If it can, sets LDFLAGS_VERSIONING to $CXX flags needed to use
dnl version script file named versionfile
dnl
dnl call WX_VERSIONED_SYMBOLS(versionfile)
dnl ---------------------------------------------------------------------------
AC_DEFUN([WX_VERSIONED_SYMBOLS],
[
  found_versioning=no

  dnl FIXME - doesn't work, Solaris linker doesn't accept wildcards
  dnl         in the script.
  dnl dnl Check for known non-gcc cases:
  dnl case "${host}" in
  dnl   *-*-solaris2* )
  dnl     if test "x$GCC" != "xyes" ; then
  dnl         LDFLAGS_VERSIONING="-M $1"
  dnl         found_versioning=yes
  dnl     fi
  dnl   ;;
  dnl esac
  
  dnl Generic check for GCC or GCC-like behaviour (Intel C++, GCC):
  if test $found_versioning = no ; then
      AC_CACHE_CHECK([if the linker accepts --version-script], wx_cv_version_script,
      [
        echo "VER_1 { *; };" >conftest.sym
        echo "int main() { return 0; }" >conftest.cpp
  
        if AC_TRY_COMMAND([
                $CXX -o conftest.output $CXXFLAGS $CPPFLAGS $LDFLAGS conftest.cpp
                -Wl,--version-script,conftest.sym >/dev/null 2>conftest.stderr]) ; then
          if test -s conftest.stderr ; then
              wx_cv_version_script=no
          else
              wx_cv_version_script=yes
          fi
        else
          wx_cv_version_script=no
        fi
        rm -f conftest.output conftest.stderr conftest.sym conftest.cpp
      ])
      if test $wx_cv_version_script = yes ; then
        LDFLAGS_VERSIONING="-Wl,--version-script,$1"
      fi
  fi
])


dnl ===========================================================================
dnl "3rd party" macros included here because they are not widely available
dnl ===========================================================================

dnl ---------------------------------------------------------------------------
dnl test for availability of iconv()
dnl ---------------------------------------------------------------------------

dnl From Bruno Haible.

AC_DEFUN([AM_ICONV],
[
  dnl Some systems have iconv in libc, some have it in libiconv (OSF/1 and
  dnl those with the standalone portable GNU libiconv installed).

  AC_ARG_WITH([libiconv-prefix],
[  --with-libiconv-prefix=DIR  search for libiconv in DIR/include and DIR/lib], [
    for dir in `echo "$withval" | tr : ' '`; do
      if test -d $dir/include; then CPPFLAGS="$CPPFLAGS -I$dir/include"; fi
      if test -d $dir/lib; then LDFLAGS="$LDFLAGS -L$dir/lib"; fi
    done
   ])

  AC_CACHE_CHECK(for iconv, am_cv_func_iconv, [
    am_cv_func_iconv="no, consider installing GNU libiconv"
    am_cv_lib_iconv=no
    AC_TRY_LINK([#include <stdlib.h>
#include <iconv.h>],
      [iconv_t cd = iconv_open("","");
       iconv(cd,NULL,NULL,NULL,NULL);
       iconv_close(cd);],
      am_cv_func_iconv=yes)
    if test "$am_cv_func_iconv" != yes; then
      am_save_LIBS="$LIBS"
      LIBS="$LIBS -liconv"
      AC_TRY_LINK([#include <stdlib.h>
#include <iconv.h>],
        [iconv_t cd = iconv_open("","");
         iconv(cd,NULL,NULL,NULL,NULL);
         iconv_close(cd);],
        am_cv_lib_iconv=yes
        am_cv_func_iconv=yes)
      LIBS="$am_save_LIBS"
    fi
  ])
  if test "$am_cv_func_iconv" = yes; then
    AC_DEFINE(HAVE_ICONV, 1, [Define if you have the iconv() function.])
    AC_CACHE_CHECK([if iconv needs const], wx_cv_func_iconv_const,
      AC_TRY_COMPILE([
#include <stdlib.h>
#include <iconv.h>
extern
#ifdef __cplusplus
"C"
#endif
#if defined(__STDC__) || defined(__cplusplus)
size_t iconv (iconv_t cd, char * *inbuf, size_t *inbytesleft, char * *outbuf, size_t *outbytesleft);
#else
size_t iconv();
#endif
        ],
        [],
        wx_cv_func_iconv_const="no",
        wx_cv_func_iconv_const="yes"
      )
    )

    iconv_const=
    if test "x$wx_cv_func_iconv_const" = "xyes"; then
        iconv_const="const"
    fi

    AC_DEFINE_UNQUOTED(ICONV_CONST, $iconv_const,
      [Define as const if the declaration of iconv() needs const.])
  fi
  LIBICONV=
  if test "$am_cv_lib_iconv" = yes; then
    LIBICONV="-liconv"
  fi
  AC_SUBST(LIBICONV)
])

dnl ---------------------------------------------------------------------------
dnl AC_SYS_LARGEFILE (partly based on the code from autoconf 2.5x)
dnl ---------------------------------------------------------------------------

dnl WX_SYS_LARGEFILE_TEST
dnl
dnl NB: original autoconf test was checking if compiler supported 6 bit off_t
dnl     arithmetic properly but this failed miserably with gcc under Linux
dnl     whereas the system still supports 64 bit files, so now simply check
dnl     that off_t is big enough
define(WX_SYS_LARGEFILE_TEST,
[typedef struct {
    unsigned int field: sizeof(off_t) == 8;
} wxlf;
])


dnl WX_SYS_LARGEFILE_MACRO_VALUE(C-MACRO, VALUE, CACHE-VAR)
define(WX_SYS_LARGEFILE_MACRO_VALUE,
[
    AC_CACHE_CHECK([for $1 value needed for large files], [$3],
        [
          AC_TRY_COMPILE([#define $1 $2
                          #include <sys/types.h>],
                         WX_SYS_LARGEFILE_TEST,
                         [$3=$2],
                         [$3=no])
        ]
    )

    if test "$$3" != no; then
        wx_largefile=yes
        AC_DEFINE_UNQUOTED([$1], [$$3])
    fi
])


dnl AC_SYS_LARGEFILE
dnl ----------------
dnl By default, many hosts won't let programs access large files;
dnl one must use special compiler options to get large-file access to work.
dnl For more details about this brain damage please see:
dnl http://www.sas.com/standards/large.file/x_open.20Mar96.html
AC_DEFUN([AC_SYS_LARGEFILE],
[AC_ARG_ENABLE(largefile,
               [  --disable-largefile     omit support for large files])
if test "$enable_largefile" != no; then
    dnl _FILE_OFFSET_BITS==64 is needed for Linux, Solaris, ...
    dnl _LARGE_FILES -- for AIX
    wx_largefile=no
    WX_SYS_LARGEFILE_MACRO_VALUE(_FILE_OFFSET_BITS, 64, ac_cv_sys_file_offset_bits)
    if test "x$wx_largefile" != "xyes"; then
        WX_SYS_LARGEFILE_MACRO_VALUE(_LARGE_FILES, 1, ac_cv_sys_large_files)
    fi

    AC_MSG_CHECKING(if large file support is available)
    if test "x$wx_largefile" = "xyes"; then
        AC_DEFINE(HAVE_LARGEFILE_SUPPORT)
    fi
    AC_MSG_RESULT($wx_largefile)
fi
])


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_const_cast.html
dnl
AC_DEFUN([AC_CXX_CONST_CAST],
[AC_CACHE_CHECK(whether the compiler supports const_cast<>,
ac_cv_cxx_const_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE(,[int x = 0;const int& y = x;int& z = const_cast<int&>(y);return z;],
 ac_cv_cxx_const_cast=yes, ac_cv_cxx_const_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_const_cast" = yes; then
  AC_DEFINE(HAVE_CONST_CAST,,[define if the compiler supports const_cast<>])
fi
])

dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_reinterpret_cast.html
AC_DEFUN([AC_CXX_REINTERPRET_CAST],
[AC_CACHE_CHECK(whether the compiler supports reinterpret_cast<>,
ac_cv_cxx_reinterpret_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0;};
class Derived : public Base { public : Derived () {} virtual void f () {} };
class Unrelated { public : Unrelated () {} };
int g (Unrelated&) { return 0; }],[
Derived d;Base& b=d;Unrelated& e=reinterpret_cast<Unrelated&>(b);return g(e);],
 ac_cv_cxx_reinterpret_cast=yes, ac_cv_cxx_reinterpret_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_reinterpret_cast" = yes; then
  AC_DEFINE(HAVE_REINTERPRET_CAST,,
            [define if the compiler supports reinterpret_cast<>])
fi
])

dnl and http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_static_cast.html
AC_DEFUN([AC_CXX_STATIC_CAST],
[AC_CACHE_CHECK(whether the compiler supports static_cast<>,
ac_cv_cxx_static_cast,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <typeinfo>
class Base { public : Base () {} virtual void f () = 0; };
class Derived : public Base { public : Derived () {} virtual void f () {} };
int g (Derived&) { return 0; }],[
Derived d; Base& b = d; Derived& s = static_cast<Derived&> (b); return g (s);],
 ac_cv_cxx_static_cast=yes, ac_cv_cxx_static_cast=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_static_cast" = yes; then
  AC_DEFINE(HAVE_STATIC_CAST,, [define if the compiler supports static_cast<>])
fi
])

# Configure paths for GTK+
# Owen Taylor     1997-2001

dnl AM_PATH_GTK_2_0([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GTK+, and define GTK_CFLAGS and GTK_LIBS, if gthread is specified in MODULES, 
dnl pass to pkg-config
dnl
AC_DEFUN([AM_PATH_GTK_2_0],
[dnl 
dnl Get the cflags and libraries from pkg-config
dnl
AC_ARG_ENABLE(gtktest, [  --disable-gtktest       do not try to compile and run a test GTK+ program],
		    , enable_gtktest=yes)

  pkg_config_args=gtk+-2.0
  for module in . $4
  do
      case "$module" in
         gthread) 
             pkg_config_args="$pkg_config_args gthread-2.0"
         ;;
      esac
  done

  no_gtk=""

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

  if test x$PKG_CONFIG != xno ; then
    if pkg-config --atleast-pkgconfig-version 0.7 ; then
      :
    else
      echo "*** pkg-config too old; version 0.7 or better required."
      no_gtk=yes
      PKG_CONFIG=no
    fi
  else
    no_gtk=yes
  fi

  min_gtk_version=ifelse([$1], ,2.0.0,$1)
  AC_MSG_CHECKING(for GTK+ - version >= $min_gtk_version)

  if test x$PKG_CONFIG != xno ; then
    ## don't try to run the test against uninstalled libtool libs
    if $PKG_CONFIG --uninstalled $pkg_config_args; then
	  echo "Will use uninstalled version of GTK+ found in PKG_CONFIG_PATH"
	  enable_gtktest=no
    fi

    if $PKG_CONFIG --atleast-version $min_gtk_version $pkg_config_args; then
	  :
    else
	  no_gtk=yes
    fi
  fi

  if test x"$no_gtk" = x ; then
    GTK_CFLAGS=`$PKG_CONFIG $pkg_config_args --cflags`
    GTK_LIBS=`$PKG_CONFIG $pkg_config_args --libs`
    gtk_config_major_version=`$PKG_CONFIG --modversion gtk+-2.0 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtk_config_minor_version=`$PKG_CONFIG --modversion gtk+-2.0 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtk_config_micro_version=`$PKG_CONFIG --modversion gtk+-2.0 | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtktest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GTK_CFLAGS"
      LIBS="$GTK_LIBS $LIBS"
dnl
dnl Now check if the installed GTK+ is sufficiently new. (Also sanity
dnl checks the results of pkg-config to some extent)
dnl
      rm -f conf.gtktest
      AC_TRY_RUN([
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtktest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_gtk_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtk_version");
     exit(1);
   }

  if ((gtk_major_version != $gtk_config_major_version) ||
      (gtk_minor_version != $gtk_config_minor_version) ||
      (gtk_micro_version != $gtk_config_micro_version))
    {
      printf("\n*** 'pkg-config --modversion gtk+-2.0' returned %d.%d.%d, but GTK+ (%d.%d.%d)\n", 
             $gtk_config_major_version, $gtk_config_minor_version, $gtk_config_micro_version,
             gtk_major_version, gtk_minor_version, gtk_micro_version);
      printf ("*** was found! If pkg-config was correct, then it is best\n");
      printf ("*** to remove the old version of GTK+. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If pkg-config was wrong, set the environment variable PKG_CONFIG_PATH\n");
      printf("*** to point to the correct configuration files\n");
    } 
  else if ((gtk_major_version != GTK_MAJOR_VERSION) ||
	   (gtk_minor_version != GTK_MINOR_VERSION) ||
           (gtk_micro_version != GTK_MICRO_VERSION))
    {
      printf("*** GTK+ header files (version %d.%d.%d) do not match\n",
	     GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     gtk_major_version, gtk_minor_version, gtk_micro_version);
    }
  else
    {
      if ((gtk_major_version > major) ||
        ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
        ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GTK+ (%d.%d.%d) was found.\n",
               gtk_major_version, gtk_minor_version, gtk_micro_version);
        printf("*** You need a version of GTK+ newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GTK+ is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the pkg-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GTK+, but you can also set the PKG_CONFIG environment to point to the\n");
        printf("*** correct copy of pkg-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtk" = x ; then
     AC_MSG_RESULT(yes (version $gtk_config_major_version.$gtk_config_minor_version.$gtk_config_micro_version))
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$PKG_CONFIG" = "no" ; then
       echo "*** A new enough version of pkg-config was not found."
       echo "*** See http://pkgconfig.sourceforge.net"
     else
       if test -f conf.gtktest ; then
        :
       else
          echo "*** Could not run GTK+ test program, checking why..."
	  ac_save_CFLAGS="$CFLAGS"
	  ac_save_LIBS="$LIBS"
          CFLAGS="$CFLAGS $GTK_CFLAGS"
          LIBS="$LIBS $GTK_LIBS"
          AC_TRY_LINK([
#include <gtk/gtk.h>
#include <stdio.h>
],      [ return ((gtk_major_version) || (gtk_minor_version) || (gtk_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GTK+ or finding the wrong"
          echo "*** version of GTK+. If it is not finding GTK+, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GTK+ is incorrectly installed."])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTK_CFLAGS=""
     GTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GTK_CFLAGS)
  AC_SUBST(GTK_LIBS)
  rm -f conf.gtktest
])

# Configure paths for GTK+
# Owen Taylor     97-11-3

dnl AM_PATH_GTK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GTK, and define GTK_CFLAGS and GTK_LIBS
dnl
AC_DEFUN(AM_PATH_GTK,
[dnl 
dnl Get the cflags and libraries from the gtk-config script
dnl
AC_ARG_WITH(gtk-prefix,[  --with-gtk-prefix=PFX   Prefix where GTK is installed (optional)],
            gtk_config_prefix="$withval", gtk_config_prefix="")
AC_ARG_WITH(gtk-exec-prefix,[  --with-gtk-exec-prefix=PFX Exec prefix where GTK is installed (optional)],
            gtk_config_exec_prefix="$withval", gtk_config_exec_prefix="")
AC_ARG_ENABLE(gtktest, [  --disable-gtktest       Do not try to compile and run a test GTK program],
		    , enable_gtktest=yes)

  for module in . $4
  do
      case "$module" in
         gthread) 
             gtk_config_args="$gtk_config_args gthread"
         ;;
      esac
  done

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
  if test "$GTK_CONFIG" = "no" ; then
    no_gtk=yes
  else
    GTK_CFLAGS=`$GTK_CONFIG $gtk_config_args --cflags`
    GTK_LIBS=`$GTK_CONFIG $gtk_config_args --libs`
    gtk_config_major_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtk_config_minor_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtk_config_micro_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtktest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GTK_CFLAGS"
      LIBS="$GTK_LIBS $LIBS"
dnl
dnl Now check if the installed GTK is sufficiently new. (Also sanity
dnl checks the results of gtk-config to some extent
dnl
      rm -f conf.gtktest
      AC_TRY_RUN([
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtktest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_gtk_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtk_version");
     exit(1);
   }

  if ((gtk_major_version != $gtk_config_major_version) ||
      (gtk_minor_version != $gtk_config_minor_version) ||
      (gtk_micro_version != $gtk_config_micro_version))
    {
      printf("\n*** 'gtk-config --version' returned %d.%d.%d, but GTK+ (%d.%d.%d)\n", 
             $gtk_config_major_version, $gtk_config_minor_version, $gtk_config_micro_version,
             gtk_major_version, gtk_minor_version, gtk_micro_version);
      printf ("*** was found! If gtk-config was correct, then it is best\n");
      printf ("*** to remove the old version of GTK+. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If gtk-config was wrong, set the environment variable GTK_CONFIG\n");
      printf("*** to point to the correct copy of gtk-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
#if defined (GTK_MAJOR_VERSION) && defined (GTK_MINOR_VERSION) && defined (GTK_MICRO_VERSION)
  else if ((gtk_major_version != GTK_MAJOR_VERSION) ||
	   (gtk_minor_version != GTK_MINOR_VERSION) ||
           (gtk_micro_version != GTK_MICRO_VERSION))
    {
      printf("*** GTK+ header files (version %d.%d.%d) do not match\n",
	     GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     gtk_major_version, gtk_minor_version, gtk_micro_version);
    }
#endif /* defined (GTK_MAJOR_VERSION) ... */
  else
    {
      if ((gtk_major_version > major) ||
        ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
        ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GTK+ (%d.%d.%d) was found.\n",
               gtk_major_version, gtk_minor_version, gtk_micro_version);
        printf("*** You need a version of GTK+ newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GTK+ is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the gtk-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GTK+, but you can also set the GTK_CONFIG environment to point to the\n");
        printf("*** correct copy of gtk-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtk" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GTK_CONFIG" = "no" ; then
       echo "*** The gtk-config script installed by GTK could not be found"
       echo "*** If GTK was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GTK_CONFIG environment variable to the"
       echo "*** full path to gtk-config."
     else
       if test -f conf.gtktest ; then
        :
       else
          echo "*** Could not run GTK test program, checking why..."
          CFLAGS="$CFLAGS $GTK_CFLAGS"
          LIBS="$LIBS $GTK_LIBS"
          AC_TRY_LINK([
#include <gtk/gtk.h>
#include <stdio.h>
],      [ return ((gtk_major_version) || (gtk_minor_version) || (gtk_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GTK or finding the wrong"
          echo "*** version of GTK. If it is not finding GTK, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GTK package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps gtk gtk-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GTK was incorrectly installed"
          echo "*** or that you have moved GTK since it was installed. In the latter case, you"
          echo "*** may want to edit the gtk-config script: $GTK_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTK_CFLAGS=""
     GTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GTK_CFLAGS)
  AC_SUBST(GTK_LIBS)
  rm -f conf.gtktest
])


dnl PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 1.3 glib = 1.3.4, action-if, action-not)
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
AC_DEFUN(PKG_CHECK_MODULES, [
  succeeded=no

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PKG_CONFIG" = "no" ; then
     echo "*** The pkg-config script could not be found. Make sure it is"
     echo "*** in your path, or set the PKG_CONFIG environment variable"
     echo "*** to the full path to pkg-config."
     echo "*** Or see http://www.freedesktop.org/software/pkgconfig to get pkg-config."
  else
     PKG_CONFIG_MIN_VERSION=0.9.0
     if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
        AC_MSG_CHECKING(for $2)

        if $PKG_CONFIG --exists "$2" ; then
            AC_MSG_RESULT(yes)
            succeeded=yes

            AC_MSG_CHECKING($1_CFLAGS)
            $1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
            AC_MSG_RESULT($$1_CFLAGS)

            AC_MSG_CHECKING($1_LIBS)
            $1_LIBS=`$PKG_CONFIG --libs "$2"`
            AC_MSG_RESULT($$1_LIBS)
        else
            $1_CFLAGS=""
            $1_LIBS=""
            ## If we have a custom action on failure, don't print errors, but 
            ## do set a variable so people can do so.
            $1_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
            ifelse([$4], ,echo $$1_PKG_ERRORS,)
        fi

        AC_SUBST($1_CFLAGS)
        AC_SUBST($1_LIBS)
     else
        echo "*** Your version of pkg-config is too old. You need version $PKG_CONFIG_MIN_VERSION or newer."
        echo "*** See http://www.freedesktop.org/software/pkgconfig"
     fi
  fi

  if test $succeeded = yes; then
     ifelse([$3], , :, [$3])
  else
     ifelse([$4], , AC_MSG_ERROR([Library requirements ($2) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.]), [$4])
  fi
])



# Configure paths for SDL
# Sam Lantinga 9/21/99
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_SDL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SDL, and define SDL_CFLAGS and SDL_LIBS
dnl
AC_DEFUN([AM_PATH_SDL],
[dnl 
dnl Get the cflags and libraries from the sdl-config script
dnl
AC_ARG_WITH(sdl-prefix,[  --with-sdl-prefix=PFX   Prefix where SDL is installed (optional)],
            sdl_prefix="$withval", sdl_prefix="")
AC_ARG_WITH(sdl-exec-prefix,[  --with-sdl-exec-prefix=PFX Exec prefix where SDL is installed (optional)],
            sdl_exec_prefix="$withval", sdl_exec_prefix="")
AC_ARG_ENABLE(sdltest, [  --disable-sdltest       Do not try to compile and run a test SDL program],
		    , enable_sdltest=yes)

  if test x$sdl_exec_prefix != x ; then
     sdl_args="$sdl_args --exec-prefix=$sdl_exec_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_exec_prefix/bin/sdl-config
     fi
  fi
  if test x$sdl_prefix != x ; then
     sdl_args="$sdl_args --prefix=$sdl_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_prefix/bin/sdl-config
     fi
  fi

  AC_REQUIRE([AC_CANONICAL_TARGET])
  PATH="$prefix/bin:$prefix/usr/bin:$PATH"
  AC_PATH_PROG(SDL_CONFIG, sdl-config, no, [$PATH])
  min_sdl_version=ifelse([$1], ,0.11.0,$1)
  AC_MSG_CHECKING(for SDL - version >= $min_sdl_version)
  no_sdl=""
  if test "$SDL_CONFIG" = "no" ; then
    no_sdl=yes
  else
    SDL_CFLAGS=`$SDL_CONFIG $sdlconf_args --cflags`
    SDL_LIBS=`$SDL_CONFIG $sdlconf_args --libs`

    sdl_major_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sdl_minor_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sdl_micro_version=`$SDL_CONFIG $sdl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_sdltest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $SDL_CFLAGS"
      CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
      LIBS="$LIBS $SDL_LIBS"
dnl
dnl Now check if the installed SDL is sufficiently new. (Also sanity
dnl checks the results of sdl-config to some extent
dnl
      rm -f conf.sdltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.sdltest");
  */
  { FILE *fp = fopen("conf.sdltest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_sdl_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_sdl_version");
     exit(1);
   }

   if (($sdl_major_version > major) ||
      (($sdl_major_version == major) && ($sdl_minor_version > minor)) ||
      (($sdl_major_version == major) && ($sdl_minor_version == minor) && ($sdl_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'sdl-config --version' returned %d.%d.%d, but the minimum version\n", $sdl_major_version, $sdl_minor_version, $sdl_micro_version);
      printf("*** of SDL required is %d.%d.%d. If sdl-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If sdl-config was wrong, set the environment variable SDL_CONFIG\n");
      printf("*** to point to the correct copy of sdl-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_sdl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_sdl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$SDL_CONFIG" = "no" ; then
       echo "*** The sdl-config script installed by SDL could not be found"
       echo "*** If SDL was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the SDL_CONFIG environment variable to the"
       echo "*** full path to sdl-config."
     else
       if test -f conf.sdltest ; then
        :
       else
          echo "*** Could not run SDL test program, checking why..."
          CFLAGS="$CFLAGS $SDL_CFLAGS"
          CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
          LIBS="$LIBS $SDL_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include "SDL.h"

int main(int argc, char *argv[])
{ return 0; }
#undef  main
#define main K_and_R_C_main
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding SDL or finding the wrong"
          echo "*** version of SDL. If it is not finding SDL, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means SDL was incorrectly installed"
          echo "*** or that you have moved SDL since it was installed. In the latter case, you"
          echo "*** may want to edit the sdl-config script: $SDL_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     SDL_CFLAGS=""
     SDL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SDL_CFLAGS)
  AC_SUBST(SDL_LIBS)
  rm -f conf.sdltest
])

dnl ---------------------------------------------------------------------------
dnl Support macros for makefiles generated by BAKEFILE.
dnl ---------------------------------------------------------------------------

dnl Lots of compiler & linker detection code contained here was taken from
dnl wxWindows configure.in script (see http://www.wxwindows.org)

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
MWCC=`test $bakefile_compiler_mwerks = yes && echo yes`
AC_LANG_POP(C)
])

dnl Loosely based on autoconf AC_PROG_CXX
dnl TODO: Maybe this should wrap the call to AC_PROG_CXX and be used instead.
AC_DEFUN([AC_BAKEFILE_PROG_MWCXX],
[AC_LANG_PUSH(C++)
_AC_BAKEFILE_LANG_COMPILER_MWERKS
MWCXX=`test $bakefile_compiler_mwerks = yes && echo yes`
AC_LANG_POP(C++)
])

dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_GNUMAKE
dnl
dnl Detects GNU make
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_GNUMAKE],
[
    dnl does make support "-include" (only GNU make does AFAIK)?
    AC_CACHE_CHECK([if make is GNU make], bakefile_cv_prog_makeisgnu,
    [
        if ( ${SHELL-sh} -c "${MAKE-make} --version" 2> /dev/null |
                egrep -s GNU > /dev/null); then
            bakefile_cv_prog_makeisgnu="yes"
        else
            bakefile_cv_prog_makeisgnu="no"
        fi
    ])

    if test "x$bakefile_cv_prog_makeisgnu" = "xyes"; then
        IF_GNU_MAKE=""
    else
        IF_GNU_MAKE="#"
    fi
    AC_SUBST(IF_GNU_MAKE)
])

dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_PLATFORM
dnl
dnl Detects platform and sets PLATFORM_XXX variables accordingly
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_PLATFORM],
[
    PLATFORM_UNIX=0
    PLATFORM_WIN32=0
    PLATFORM_MSDOS=0
    PLATFORM_MAC=0
    PLATFORM_MACOS=0
    PLATFORM_MACOSX=0
    PLATFORM_OS2=0
    PLATFORM_BEOS=0

    if test "x$BAKEFILE_FORCE_PLATFORM" = "x"; then 
        case "${BAKEFILE_HOST}" in
            *-*-cygwin* | *-*-mingw32* )
                PLATFORM_WIN32=1
            ;;
            *-pc-msdosdjgpp )
                PLATFORM_MSDOS=1
            ;;
            *-pc-os2_emx | *-pc-os2-emx )
                PLATFORM_OS2=1
            ;;
            powerpc-*-darwin* )
                PLATFORM_MAC=1
                PLATFORM_MACOSX=1
            ;; 
            *-*-beos* )
                PLATFORM_BEOS=1
            ;;
            powerpc-apple-macos* )
                PLATFORM_MAC=1
                PLATFORM_MACOS=1
            ;;
            * )
                PLATFORM_UNIX=1
            ;;
        esac
    else
        case "$BAKEFILE_FORCE_PLATFORM" in
            win32 )
                PLATFORM_WIN32=1
            ;;
            msdos )
                PLATFORM_MSDOS=1
            ;;
            os2 )
                PLATFORM_OS2=1
            ;;
            darwin )
                PLATFORM_MAC=1
                PLATFORM_MACOSX=1
            ;;
            unix )
                PLATFORM_UNIX=1
            ;;
            beos )
                PLATFORM_BEOS=1
            ;;
            * )
                AC_MSG_ERROR([Unknown platform: $BAKEFILE_FORCE_PLATFORM])
            ;;
        esac
    fi

    AC_SUBST(PLATFORM_UNIX)
    AC_SUBST(PLATFORM_WIN32)
    AC_SUBST(PLATFORM_MSDOS)
    AC_SUBST(PLATFORM_MAC)
    AC_SUBST(PLATFORM_MACOS)
    AC_SUBST(PLATFORM_MACOSX)
    AC_SUBST(PLATFORM_OS2)
    AC_SUBST(PLATFORM_BEOS)
])


dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_PLATFORM_SPECIFICS
dnl
dnl Sets misc platform-specific settings
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_PLATFORM_SPECIFICS],
[
    AC_ARG_ENABLE([omf], AS_HELP_STRING([--enable-omf],
                                        [use OMF object format (OS/2)]),
                  [bk_os2_use_omf="$enableval"])
    
    case "${BAKEFILE_HOST}" in
      *-*-darwin* )
        dnl For Unix to MacOS X porting instructions, see:
        dnl http://fink.sourceforge.net/doc/porting/porting.html
        CFLAGS="$CFLAGS -fno-common"
        CXXFLAGS="$CXXFLAGS -fno-common"
        ;;

      *-pc-os2_emx | *-pc-os2-emx )
        if test "x$bk_os2_use_omf" = "xyes" ; then
            AR=emxomfar
            RANLIB=:
            LDFLAGS="-Zomf $LDFLAGS"
            CFLAGS="-Zomf $CFLAGS"
            CXXFLAGS="-Zomf $CXXFLAGS"
            OS2_LIBEXT="lib"
        else
            OS2_LIBEXT="a"
        fi
        ;;
      
      i*86-*-beos* )
        LDFLAGS="-L/boot/develop/lib/x86 $LDFLAGS"
        ;;
    esac
])

dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_SUFFIXES
dnl
dnl Detects shared various suffixes for shared libraries, libraries, programs,
dnl plugins etc.
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_SUFFIXES],
[
    SO_SUFFIX="so"
    SO_SUFFIX_MODULE="so"
    EXEEXT=""
    LIBPREFIX="lib"
    LIBEXT=".a"
    DLLPREFIX="lib"
    DLLPREFIX_MODULE=""
    DLLIMP_SUFFIX=""
    
    case "${BAKEFILE_HOST}" in
        *-hp-hpux* )
            SO_SUFFIX="sl"
            SO_SUFFIX_MODULE="sl"
        ;;
        *-*-aix* )
            dnl quoting from
            dnl http://www-1.ibm.com/servers/esdd/articles/gnu.html:
            dnl     Both archive libraries and shared libraries on AIX have an
            dnl     .a extension. This will explain why you can't link with an
            dnl     .so and why it works with the name changed to .a.
            SO_SUFFIX="a"
            SO_SUFFIX_MODULE="a"
        ;;
        *-*-cygwin* | *-*-mingw32* )
            SO_SUFFIX="dll"
            SO_SUFFIX_MODULE="dll"
            DLLIMP_SUFFIX="dll.a"
            EXEEXT=".exe"
            DLLPREFIX=""
        ;;
        *-pc-msdosdjgpp )
            EXEEXT=".exe"
            DLLPREFIX=""
        ;;
        *-pc-os2_emx | *-pc-os2-emx )
            SO_SUFFIX="dll"
            SO_SUFFIX_MODULE="dll"
            DLLIMP_SUFFIX=$OS2_LIBEXT
            EXEEXT=".exe"
            DLLPREFIX=""
            LIBPREFIX=""
            LIBEXT=".$OS2_LIBEXT"
        ;;
        powerpc-*-darwin* )
            SO_SUFFIX="dylib"
            SO_SUFFIX_MODULE="bundle"
        ;;
    esac

    if test "x$DLLIMP_SUFFIX" = "x" ; then
        DLLIMP_SUFFIX="$SO_SUFFIX"
    fi

    AC_SUBST(SO_SUFFIX)
    AC_SUBST(SO_SUFFIX_MODULE)
    AC_SUBST(DLLIMP_SUFFIX)
    AC_SUBST(EXEEXT)
    AC_SUBST(LIBPREFIX)
    AC_SUBST(LIBEXT)
    AC_SUBST(DLLPREFIX)
    AC_SUBST(DLLPREFIX_MODULE)
])


dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_SHARED_LD
dnl
dnl Detects command for making shared libraries, substitutes SHARED_LD_CC
dnl and SHARED_LD_CXX.
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_SHARED_LD],
[
    dnl the extra compiler flags needed for compilation of shared library
    PIC_FLAG=""
    if test "x$GCC" = "xyes"; then
        dnl the switch for gcc is the same under all platforms
        PIC_FLAG="-fPIC"
    fi
    
    dnl Defaults for GCC and ELF .so shared libs:
    SHARED_LD_CC="\$(CC) -shared ${PIC_FLAG} -o"
    SHARED_LD_CXX="\$(CXX) -shared ${PIC_FLAG} -o"

    case "${BAKEFILE_HOST}" in
      *-hp-hpux* )
        dnl default settings are good for gcc but not for the native HP-UX
        if test "x$GCC" != "xyes"; then
            dnl no idea why it wants it, but it does
            LDFLAGS="$LDFLAGS -L/usr/lib"

            SHARED_LD_CC="${CC} -b -o"
            SHARED_LD_CXX="${CXX} -b -o"
            PIC_FLAG="+Z"
        fi
      ;;

      *-*-linux* )
        if test "x$GCC" != "xyes"; then
            AC_CACHE_CHECK([for Intel compiler], bakefile_cv_prog_icc,
            [
                AC_TRY_COMPILE([],
                    [
                        #ifndef __INTEL_COMPILER
                        #error Not icc
                        #endif
                    ],
                    bakefile_cv_prog_icc=yes,
                    bakefile_cv_prog_icc=no
                )
            ])
            if test "$bakefile_cv_prog_icc" = "yes"; then
                PIC_FLAG="-KPIC"
            fi
        fi
      ;;

      *-*-solaris2* )
        if test "x$GCC" != xyes ; then
            SHARED_LD_CC="${CC} -G -o"
            SHARED_LD_CXX="${CXX} -G -o"
            PIC_FLAG="-KPIC"
        fi
      ;;

      *-*-darwin* )
        dnl Most apps benefit from being fully binded (its faster and static
        dnl variables initialized at startup work).
        dnl This can be done either with the exe linker flag -Wl,-bind_at_load
        dnl or with a double stage link in order to create a single module
        dnl "-init _wxWindowsDylibInit" not useful with lazy linking solved

        dnl If using newer dev tools then there is a -single_module flag that
        dnl we can use to do this, otherwise we'll need to use a helper
        dnl script.  Check the version of gcc to see which way we can go:
        AC_CACHE_CHECK([for gcc 3.1 or later], wx_cv_gcc31, [
           AC_TRY_COMPILE([],
               [
                   #if (__GNUC__ < 3) || \
                       ((__GNUC__ == 3) && (__GNUC_MINOR__ < 1))
                       #error old gcc
                   #endif
               ],
               [
                   wx_cv_gcc31=yes
               ],
               [
                   wx_cv_gcc31=no
               ]
           )
        ])
        if test "$wx_cv_gcc31" = "no"; then
            AC_BAKEFILE_CREATE_FILE_SHARED_LD_SH
            chmod +x shared-ld-sh

            dnl Use the shared-ld-sh helper script
            SHARED_LD_CC="`pwd`/shared-ld-sh -dynamiclib -headerpad_max_install_names -o"
            SHARED_LD_MODULE_CC="`pwd`/shared-ld-sh -bundle -headerpad_max_install_names -o"
            SHARED_LD_CXX="$SHARED_LD_CC"
            SHARED_LD_MODULE_CXX="$SHARED_LD_MODULE_CC"
        else
            dnl Use the -single_module flag and let the linker do it for us
            SHARED_LD_CC="\${CC} -dynamiclib -single_module -headerpad_max_install_names -o"
            SHARED_LD_MODULE_CC="\${CC} -bundle -single_module -headerpad_max_install_names -o"
            SHARED_LD_CXX="\${CXX} -dynamiclib -single_module -headerpad_max_install_names -o"
            SHARED_LD_MODULE_CXX="\${CXX} -bundle -single_module -headerpad_max_install_names -o"
        fi

        PIC_FLAG="-dynamic -fPIC"
      ;;

      *-*-aix* )
        dnl default settings are ok for gcc
        if test "x$GCC" != "xyes"; then
            dnl the abs path below used to be hardcoded here so I guess it must
            dnl be some sort of standard location under AIX?
            AC_CHECK_PROG(AIX_CXX_LD, makeC++SharedLib,
                          makeC++SharedLib, /usr/lpp/xlC/bin/makeC++SharedLib)
            dnl FIXME - what about makeCSharedLib?            
            SHARED_LD_CC="$AIX_CC_LD -p 0 -o"
            SHARED_LD_CXX="$AIX_CXX_LD -p 0 -o"
        fi
      ;;

      *-*-beos* )
        dnl can't use gcc under BeOS for shared library creation because it
        dnl complains about missing 'main'
        SHARED_LD_CC="${LD} -nostart -o"
        SHARED_LD_CXX="${LD} -nostart -o"
      ;;

      *-*-irix* )
        dnl default settings are ok for gcc
        if test "x$GCC" != "xyes"; then
            PIC_FLAG="-KPIC"
        fi
      ;;
      
      *-*-cygwin* | *-*-mingw32* )
        PIC_FLAG=""
        SHARED_LD_CC="\$(CC) -shared -o"
        SHARED_LD_CXX="\$(CXX) -shared -o"
      ;;

      *-pc-os2_emx | *-pc-os2-emx )
        SHARED_LD_CC="`pwd`/dllar.sh -libf INITINSTANCE -libf TERMINSTANCE -o"
        SHARED_LD_CXX="`pwd`/dllar.sh -libf INITINSTANCE -libf TERMINSTANCE -o"
        PIC_FLAG=""
        AC_BAKEFILE_CREATE_FILE_DLLAR_SH
        chmod +x dllar.sh
      ;;
      
      powerpc-apple-macos* | \
      *-*-freebsd* | *-*-openbsd* | *-*-netbsd* | \
      *-*-sunos4* | \
      *-*-osf* | \
      *-*-dgux5* | \
      *-*-sysv5* )
        dnl defaults are ok
      ;;

      *)
        AC_MSG_ERROR(unknown system type $BAKEFILE_HOST.)
    esac

    if test "x$PIC_FLAG" != "x" ; then
        PIC_FLAG="$PIC_FLAG -DPIC"
    fi

    if test "x$SHARED_LD_MODULE_CC" = "x" ; then
        SHARED_LD_MODULE_CC="$SHARED_LD_CC"
    fi
    if test "x$SHARED_LD_MODULE_CXX" = "x" ; then
        SHARED_LD_MODULE_CXX="$SHARED_LD_CXX"
    fi

    AC_SUBST(SHARED_LD_CC)
    AC_SUBST(SHARED_LD_CXX)
    AC_SUBST(SHARED_LD_MODULE_CC)
    AC_SUBST(SHARED_LD_MODULE_CXX)
    AC_SUBST(PIC_FLAG)
])


dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_SHARED_VERSIONS
dnl
dnl Detects linker options for attaching versions (sonames) to shared  libs.
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_SHARED_VERSIONS],
[
    USE_SOVERSION=0
    USE_SOVERLINUX=0
    USE_SOVERSOLARIS=0
    USE_SOSYMLINKS=0
    USE_MACVERSION=0
    SONAME_FLAG=

    case "${BAKEFILE_HOST}" in
      *-*-linux* | *-*-freebsd* )
        SONAME_FLAG="-Wl,-soname,"
        USE_SOVERSION=1
        USE_SOVERLINUX=1
        USE_SOSYMLINKS=1
      ;;

      *-*-solaris2* )
        SONAME_FLAG="-h "
        USE_SOVERSION=1
        USE_SOVERSOLARIS=1
        USE_SOSYMLINKS=1
      ;;

      *-*-darwin* )
        USE_MACVERSION=1
        USE_SOVERSION=1
        USE_SOSYMLINKS=1
      ;;      
    esac

    AC_SUBST(USE_SOVERSION)
    AC_SUBST(USE_SOVERLINUX)
    AC_SUBST(USE_SOVERSOLARIS)
    AC_SUBST(USE_MACVERSION)
    AC_SUBST(USE_SOSYMLINKS)
    AC_SUBST(SONAME_FLAG)
])


dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_DEPS
dnl
dnl Detects available C/C++ dependency tracking options
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_DEPS],
[
    AC_MSG_CHECKING([for dependency tracking method])
    DEPS_TRACKING=0

    if test "x$GCC" = "xyes"; then
        DEPSMODE=gcc
        DEPS_TRACKING=1
        case "${BAKEFILE_HOST}" in
            powerpc-*-darwin* )
                dnl -cpp-precomp (the default) conflicts with -MMD option
                dnl used by bk-deps (see also http://developer.apple.com/documentation/Darwin/Conceptual/PortingUnix/compiling/chapter_4_section_3.html)
                DEPSFLAG_GCC="-no-cpp-precomp -MMD"
            ;;
            * )
                DEPSFLAG_GCC="-MMD"
            ;;
        esac
        AC_MSG_RESULT([gcc])
    elif test "x$MWCC" = "xyes"; then
        DEPSMODE=mwcc
        DEPS_TRACKING=1
        DEPSFLAG_MWCC="-MM"
        AC_MSG_RESULT([mwcc])
    else
        AC_MSG_RESULT([none])
    fi

    if test $DEPS_TRACKING = 1 ; then
        AC_BAKEFILE_CREATE_FILE_BK_DEPS
        chmod +x bk-deps
    fi
    
    AC_SUBST(DEPS_TRACKING)
])

dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_CHECK_BASIC_STUFF
dnl
dnl Checks for presence of basic programs, such as C and C++ compiler, "ranlib"
dnl or "install"
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_CHECK_BASIC_STUFF],
[
    AC_PROG_RANLIB
    AC_PROG_INSTALL
    AC_PROG_LN_S

    AC_PROG_MAKE_SET
    AC_SUBST(MAKE_SET)
    
    AC_CHECK_TOOL(AR, ar, ar)
    AC_CHECK_TOOL(STRIP, strip, :)
    AC_CHECK_TOOL(NM, nm, :)

    case ${BAKEFILE_HOST} in
        *-hp-hpux* )
            dnl HP-UX install doesn't handle the "-d" switch so don't
            dnl use it there
            INSTALL_DIR="mkdir -p"
            ;;
        *)  INSTALL_DIR="$INSTALL -d"
            ;;
    esac
    AC_SUBST(INSTALL_DIR)

    LDFLAGS_GUI=
    case ${BAKEFILE_HOST} in
        *-*-cygwin* | *-*-mingw32* )
        LDFLAGS_GUI="-mwindows"
    esac
    AC_SUBST(LDFLAGS_GUI)
])


dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_RES_COMPILERS
dnl
dnl Checks for presence of resource compilers for win32 or mac
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_RES_COMPILERS],
[
    RESCOMP=
    SETFILE=

    case ${BAKEFILE_HOST} in 
        *-*-cygwin* | *-*-mingw32* )
            dnl Check for win32 resources compiler:
            if test "$build" != "$host" ; then
                RESCOMP=$host_alias-windres
            else
                AC_CHECK_PROG(RESCOMP, windres, windres, windres)
            fi
         ;;
 
      *-*-darwin* | powerpc-apple-macos* )
            AC_CHECK_PROG(RESCOMP, Rez, Rez, /Developer/Tools/Rez)
            AC_CHECK_PROG(SETFILE, SetFile, SetFile, /Developer/Tools/SetFile)
        ;;
    esac

    AC_SUBST(RESCOMP)
    AC_SUBST(SETFILE)
])

dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE_PRECOMP_HEADERS
dnl
dnl Check for precompiled headers support (GCC >= 3.4)
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_PRECOMP_HEADERS],
[

    AC_ARG_ENABLE([precomp-headers],
                  AS_HELP_STRING([--disable-precomp-headers],
                                 [don't use precompiled headers even if compiler can]),
                  [bk_use_pch="$enableval"])

    GCC_PCH=0

    if test "x$bk_use_pch" = "x" -o "x$bk_use_pch" = "xyes" ; then
        if test "x$GCC" = "xyes"; then
            dnl test if we have gcc-3.4:
            AC_MSG_CHECKING([if the compiler supports precompiled headers])
            AC_TRY_COMPILE([],
                [
                    #if !defined(__GNUC__) || !defined(__GNUC_MINOR__)
                        #error "no pch support"
                    #endif
                    #if (__GNUC__ < 3)
                        #error "no pch support"
                    #endif
                    #if (__GNUC__ == 3) && \
                       ((!defined(__APPLE_CC__) && (__GNUC_MINOR__ < 4)) || \
                       ( defined(__APPLE_CC__) && (__GNUC_MINOR__ < 3)))
                        #error "no pch support"
                    #endif
                ],
                [
                    AC_MSG_RESULT([yes])
                    GCC_PCH=1
                ],
                [
                    AC_MSG_RESULT([no])
                ])
            if test $GCC_PCH = 1 ; then
                AC_BAKEFILE_CREATE_FILE_BK_MAKE_PCH
                chmod +x bk-make-pch
            fi
        fi
    fi

    AC_SUBST(GCC_PCH)
])



dnl ---------------------------------------------------------------------------
dnl AC_BAKEFILE
dnl
dnl To be used in configure.in of any project using Bakefile-generated mks
dnl
dnl Behaviour can be modified by setting following variables:
dnl    BAKEFILE_CHECK_BASICS    set to "no" if you don't want bakefile to
dnl                             to perform check for basic tools like ranlib
dnl    BAKEFILE_HOST            set this to override host detection, defaults
dnl                             to ${host}
dnl    BAKEFILE_FORCE_PLATFORM  set to override platform detection
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE],
[
    AC_PREREQ(2.58)

    if test "x$BAKEFILE_HOST" = "x"; then
        BAKEFILE_HOST="${host}"
    fi

    if test "x$BAKEFILE_CHECK_BASICS" != "xno"; then
        AC_BAKEFILE_CHECK_BASIC_STUFF
    fi
    AC_BAKEFILE_GNUMAKE
    AC_BAKEFILE_PLATFORM
    AC_BAKEFILE_PLATFORM_SPECIFICS
    AC_BAKEFILE_SUFFIXES
    AC_BAKEFILE_SHARED_LD
    AC_BAKEFILE_SHARED_VERSIONS
    AC_BAKEFILE_DEPS
    AC_BAKEFILE_RES_COMPILERS

    BAKEFILE_BAKEFILE_M4_VERSION="0.1.4"
    
    m4_include([autoconf_inc.m4])
    
    if test "$BAKEFILE_BAKEFILE_M4_VERSION" != "$BAKEFILE_AUTOCONF_INC_M4_VERSION" ; then
        AC_MSG_ERROR([Versions of Bakefile used to generate makefiles ($BAKEFILE_AUTOCONF_INC_M4_VERSION) and configure ($BAKEFILE_BAKEFILE_M4_VERSION) do not match.])
    fi
])
        

dnl ---------------------------------------------------------------------------
dnl              Embedded copies of helper scripts follow:
dnl ---------------------------------------------------------------------------

AC_DEFUN([AC_BAKEFILE_CREATE_FILE_DLLAR_SH],
[
dnl ===================== dllar.sh begins here =====================
D='$'
cat <<EOF >dllar.sh
#!/bin/sh
#
# dllar - a tool to build both a .dll and an .a file
# from a set of object (.o) files for EMX/OS2.
#
#  Written by Andrew Zabolotny, bit@freya.etu.ru
#  Ported to Unix like shell by Stefan Neis, Stefan.Neis@t-online.de
#
#  This script will accept a set of files on the command line.
#  All the public symbols from the .o files will be exported into
#  a .DEF file, then linker will be run (through gcc) against them to
#  build a shared library consisting of all given .o files. All libraries
#  (.a) will be first decompressed into component .o files then act as
#  described above. You can optionally give a description (-d "description")
#  which will be put into .DLL. To see the list of accepted options (as well
#  as command-line format) simply run this program without options. The .DLL
#  is built to be imported by name (there is no guarantee that new versions
#  of the library you build will have same ordinals for same symbols).
#
#  dllar is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2, or (at your option)
#  any later version.
#
#  dllar is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with dllar; see the file COPYING.  If not, write to the Free
#  Software Foundation, 59 Temple Place - Suite 330, Boston, MA
#  02111-1307, USA.

# To successfuly run this program you will need:
#  - Current drive should have LFN support (HPFS, ext2, network, etc)
#    (Sometimes dllar generates filenames which won't fit 8.3 scheme)
#  - gcc
#    (used to build the .dll)
#  - emxexp
#    (used to create .def file from .o files)
#  - emximp
#    (used to create .a file from .def file)
#  - GNU text utilites (cat, sort, uniq)
#    used to process emxexp output
#  - GNU file utilities (mv, rm)
#  - GNU sed
#  - lxlite (optional, see flag below)
#    (used for general .dll cleanup)
#

flag_USE_LXLITE=1;

#
# helper functions
# basnam, variant of basename, which does _not_ remove the path, _iff_
#                              second argument (suffix to remove) is given
basnam(){
    case ${D}# in
    1)
        echo ${D}1 | sed 's/.*\\///' | sed 's/.*\\\\//'
        ;;
    2)
        echo ${D}1 | sed 's/'${D}2'${D}//'
        ;;
    *)
        echo "error in basnam ${D}*"
        exit 8
        ;;
    esac
}

# Cleanup temporary files and output
CleanUp() {
    cd ${D}curDir
    for i in ${D}inputFiles ; do
        case ${D}i in
        *!)
            rm -rf \`basnam ${D}i !\`
            ;;
        *)
            ;;
        esac
    done

    # Kill result in case of failure as there is just to many stupid make/nmake
    # things out there which doesn't do this.
    if @<:@ ${D}# -eq 0 @:>@; then
        rm -f ${D}arcFile ${D}arcFile2 ${D}defFile ${D}dllFile
    fi
}

# Print usage and exit script with rc=1.
PrintHelp() {
 echo 'Usage: dllar.sh @<:@-o@<:@utput@:>@ output_file@:>@ @<:@-i@<:@mport@:>@ importlib_name@:>@'
 echo '       @<:@-name-mangler-script script.sh@:>@'
 echo '       @<:@-d@<:@escription@:>@ "dll descrption"@:>@ @<:@-cc "CC"@:>@ @<:@-f@<:@lags@:>@ "CFLAGS"@:>@'
 echo '       @<:@-ord@<:@inals@:>@@:>@ -ex@<:@clude@:>@ "symbol(s)"'
 echo '       @<:@-libf@<:@lags@:>@ "{INIT|TERM}{GLOBAL|INSTANCE}"@:>@ @<:@-nocrt@<:@dll@:>@@:>@ @<:@-nolxl@<:@ite@:>@@:>@'
 echo '       @<:@*.o@:>@ @<:@*.a@:>@'
 echo '*> "output_file" should have no extension.'
 echo '   If it has the .o, .a or .dll extension, it is automatically removed.'
 echo '   The import library name is derived from this and is set to "name".a,'
 echo '   unless overridden by -import'
 echo '*> "importlib_name" should have no extension.'
 echo '   If it has the .o, or .a extension, it is automatically removed.'
 echo '   This name is used as the import library name and may be longer and'
 echo '   more descriptive than the DLL name which has to follow the old '
 echo '   8.3 convention of FAT.'
 echo '*> "script.sh may be given to override the output_file name by a'
 echo '   different name. It is mainly useful if the regular make process'
 echo '   of some package does not take into account OS/2 restriction of'
 echo '   DLL name lengths. It takes the importlib name as input and is'
 echo '   supposed to procude a shorter name as output. The script should'
 echo '   expect to get importlib_name without extension and should produce'
 echo '   a (max.) 8 letter name without extension.'
 echo '*> "cc" is used to use another GCC executable.   (default: gcc.exe)'
 echo '*> "flags" should be any set of valid GCC flags. (default: -s -Zcrtdll)'
 echo '   These flags will be put at the start of GCC command line.'
 echo '*> -ord@<:@inals@:>@ tells dllar to export entries by ordinals. Be careful.'
 echo '*> -ex@<:@clude@:>@ defines symbols which will not be exported. You can define'
 echo '   multiple symbols, for example -ex "myfunc yourfunc _GLOBAL*".'
 echo '   If the last character of a symbol is "*", all symbols beginning'
 echo '   with the prefix before "*" will be exclude, (see _GLOBAL* above).'
 echo '*> -libf@<:@lags@:>@ can be used to add INITGLOBAL/INITINSTANCE and/or'
 echo '   TERMGLOBAL/TERMINSTANCE flags to the dynamically-linked library.'
 echo '*> -nocrt@<:@dll@:>@ switch will disable linking the library against emx''s'
 echo '   C runtime DLLs.'
 echo '*> -nolxl@<:@ite@:>@ switch will disable running lxlite on the resulting DLL.'
 echo '*> All other switches (for example -L./ or -lmylib) will be passed'
 echo '   unchanged to GCC at the end of command line.'
 echo '*> If you create a DLL from a library and you do not specify -o,'
 echo '   the basename for DLL and import library will be set to library name,'
 echo '   the initial library will be renamed to 'name'_s.a (_s for static)'
 echo '   i.e. "dllar gcc.a" will create gcc.dll and gcc.a, and the initial'
 echo '   library will be renamed into gcc_s.a.'
 echo '--------'
 echo 'Example:'
 echo '   dllar -o gcc290.dll libgcc.a -d "GNU C runtime library" -ord'
 echo '    -ex "__main __ctordtor*" -libf "INITINSTANCE TERMINSTANCE"'
 CleanUp
 exit 1
}

# Execute a command.
# If exit code of the commnad <> 0 CleanUp() is called and we'll exit the script.
# @Uses    Whatever CleanUp() uses.
doCommand() {
    echo "${D}*"
    eval ${D}*
    rcCmd=${D}?

    if @<:@ ${D}rcCmd -ne 0 @:>@; then
        echo "command failed, exit code="${D}rcCmd
        CleanUp
        exit ${D}rcCmd
    fi
}

# main routine
# setup globals
cmdLine=${D}*
outFile=""
outimpFile=""
inputFiles=""
renameScript=""
description=""
CC=gcc.exe
CFLAGS="-s -Zcrtdll"
EXTRA_CFLAGS=""
EXPORT_BY_ORDINALS=0
exclude_symbols=""
library_flags=""
curDir=\`pwd\`
curDirS=curDir
case ${D}curDirS in
*/)
  ;;
*)
  curDirS=${D}{curDirS}"/"
  ;;
esac
# Parse commandline
libsToLink=0
omfLinking=0
while @<:@ ${D}1 @:>@; do
    case ${D}1 in
    -ord*)
        EXPORT_BY_ORDINALS=1;
        ;;
    -o*)
	shift
        outFile=${D}1
	;;
    -i*)
        shift
        outimpFile=${D}1
        ;;
    -name-mangler-script)
        shift
        renameScript=${D}1
        ;;
    -d*)
        shift
        description=${D}1
        ;;
    -f*)
        shift
        CFLAGS=${D}1
        ;;
    -c*)
        shift
        CC=${D}1
        ;;
    -h*)
        PrintHelp
        ;;
    -ex*)
        shift
        exclude_symbols=${D}{exclude_symbols}${D}1" "
        ;;
    -libf*)
        shift
        library_flags=${D}{library_flags}${D}1" "
        ;;
    -nocrt*)
        CFLAGS="-s"
        ;;
    -nolxl*)
        flag_USE_LXLITE=0
        ;;
    -* | /*)
        case ${D}1 in
        -L* | -l*)
            libsToLink=1
            ;;
        -Zomf)
            omfLinking=1
            ;;
        *)
            ;;
        esac
        EXTRA_CFLAGS=${D}{EXTRA_CFLAGS}" "${D}1
        ;;
    *.dll)
        EXTRA_CFLAGS="${D}{EXTRA_CFLAGS} \`basnam ${D}1 .dll\`"
        if @<:@ ${D}omfLinking -eq 1 @:>@; then
            EXTRA_CFLAGS="${D}{EXTRA_CFLAGS}.lib"
	else
            EXTRA_CFLAGS="${D}{EXTRA_CFLAGS}.a"
        fi
        ;;
    *)
        found=0;
        if @<:@ ${D}libsToLink -ne 0 @:>@; then
            EXTRA_CFLAGS=${D}{EXTRA_CFLAGS}" "${D}1
        else
            for file in ${D}1 ; do
                if @<:@ -f ${D}file @:>@; then
                    inputFiles="${D}{inputFiles} ${D}file"
                    found=1
                fi
            done
            if @<:@ ${D}found -eq 0 @:>@; then
                echo "ERROR: No file(s) found: "${D}1
                exit 8
            fi
        fi
      ;;
    esac
    shift
done # iterate cmdline words

#
if @<:@ -z "${D}inputFiles" @:>@; then
    echo "dllar: no input files"
    PrintHelp
fi

# Now extract all .o files from .a files
newInputFiles=""
for file in ${D}inputFiles ; do
    case ${D}file in
    *.a | *.lib)
        case ${D}file in
        *.a)
            suffix=".a"
            AR="ar"
            ;;
        *.lib)
            suffix=".lib"
            AR="emxomfar"
            EXTRA_CFLAGS="${D}EXTRA_CFLAGS -Zomf"
            ;;
        *)
            ;;
        esac
        dirname=\`basnam ${D}file ${D}suffix\`"_%"
        mkdir ${D}dirname
        if @<:@ ${D}? -ne 0 @:>@; then
            echo "Failed to create subdirectory ./${D}dirname"
            CleanUp
            exit 8;
        fi
        # Append '!' to indicate archive
        newInputFiles="${D}newInputFiles ${D}{dirname}!"
        doCommand "cd ${D}dirname; ${D}AR x ../${D}file"
        cd ${D}curDir
        found=0;
        for subfile in ${D}dirname/*.o* ; do
            if @<:@ -f ${D}subfile @:>@; then
                found=1
                if @<:@ -s ${D}subfile @:>@; then
	            # FIXME: This should be: is file size > 32 byte, _not_ > 0!
                    newInputFiles="${D}newInputFiles ${D}subfile"
                fi
            fi
        done
        if @<:@ ${D}found -eq 0 @:>@; then
            echo "WARNING: there are no files in archive \\'${D}file\\'"
        fi
        ;;
    *)
        newInputFiles="${D}{newInputFiles} ${D}file"
        ;;
    esac
done
inputFiles="${D}newInputFiles"

# Output filename(s).
do_backup=0;
if @<:@ -z ${D}outFile @:>@; then
    do_backup=1;
    set outFile ${D}inputFiles; outFile=${D}2
fi

# If it is an archive, remove the '!' and the '_%' suffixes
case ${D}outFile in
*_%!)
    outFile=\`basnam ${D}outFile _%!\`
    ;;
*)
    ;;
esac
case ${D}outFile in
*.dll)
    outFile=\`basnam ${D}outFile .dll\`
    ;;
*.DLL)
    outFile=\`basnam ${D}outFile .DLL\`
    ;;
*.o)
    outFile=\`basnam ${D}outFile .o\`
    ;;
*.obj)
    outFile=\`basnam ${D}outFile .obj\`
    ;;
*.a)
    outFile=\`basnam ${D}outFile .a\`
    ;;
*.lib)
    outFile=\`basnam ${D}outFile .lib\`
    ;;
*)
    ;;
esac
case ${D}outimpFile in
*.a)
    outimpFile=\`basnam ${D}outimpFile .a\`
    ;;
*.lib)
    outimpFile=\`basnam ${D}outimpFile .lib\`
    ;;
*)
    ;;
esac
if @<:@ -z ${D}outimpFile @:>@; then
    outimpFile=${D}outFile
fi
defFile="${D}{outFile}.def"
arcFile="${D}{outimpFile}.a"
arcFile2="${D}{outimpFile}.lib"

#create ${D}dllFile as something matching 8.3 restrictions,
if @<:@ -z ${D}renameScript @:>@ ; then
    dllFile="${D}outFile"
else
    dllFile=\`${D}renameScript ${D}outimpFile\`
fi

if @<:@ ${D}do_backup -ne 0 @:>@ ; then
    if @<:@ -f ${D}arcFile @:>@ ; then
        doCommand "mv ${D}arcFile ${D}{outFile}_s.a"
    fi
    if @<:@ -f ${D}arcFile2 @:>@ ; then
        doCommand "mv ${D}arcFile2 ${D}{outFile}_s.lib"
    fi
fi

# Extract public symbols from all the object files.
tmpdefFile=${D}{defFile}_%
rm -f ${D}tmpdefFile
for file in ${D}inputFiles ; do
    case ${D}file in
    *!)
        ;;
    *)
        doCommand "emxexp -u ${D}file >> ${D}tmpdefFile"
        ;;
    esac
done

# Create the def file.
rm -f ${D}defFile
echo "LIBRARY \`basnam ${D}dllFile\` ${D}library_flags" >> ${D}defFile
dllFile="${D}{dllFile}.dll"
if @<:@ ! -z ${D}description @:>@; then
    echo "DESCRIPTION  \\"${D}{description}\\"" >> ${D}defFile
fi
echo "EXPORTS" >> ${D}defFile

doCommand "cat ${D}tmpdefFile | sort.exe | uniq.exe > ${D}{tmpdefFile}%"
grep -v "^ *;" < ${D}{tmpdefFile}% | grep -v "^ *${D}" >${D}tmpdefFile

# Checks if the export is ok or not.
for word in ${D}exclude_symbols; do
    grep -v ${D}word < ${D}tmpdefFile >${D}{tmpdefFile}%
    mv ${D}{tmpdefFile}% ${D}tmpdefFile
done


if @<:@ ${D}EXPORT_BY_ORDINALS -ne 0 @:>@; then
    sed "=" < ${D}tmpdefFile | \\
    sed '
      N
      : loop
      s/^\\(@<:@0-9@:>@\\+\\)\\(@<:@^;@:>@*\\)\\(;.*\\)\\?/\\2 @\\1 NONAME/
      t loop
    ' > ${D}{tmpdefFile}%
    grep -v "^ *${D}" < ${D}{tmpdefFile}% > ${D}tmpdefFile
else
    rm -f ${D}{tmpdefFile}%
fi
cat ${D}tmpdefFile >> ${D}defFile
rm -f ${D}tmpdefFile

# Do linking, create implib, and apply lxlite.
gccCmdl="";
for file in ${D}inputFiles ; do
    case ${D}file in
    *!)
        ;;
    *)
        gccCmdl="${D}gccCmdl ${D}file"
        ;;
    esac
done
doCommand "${D}CC ${D}CFLAGS -Zdll -o ${D}dllFile ${D}defFile ${D}gccCmdl ${D}EXTRA_CFLAGS"
touch "${D}{outFile}.dll"

doCommand "emximp -o ${D}arcFile ${D}defFile"
if @<:@ ${D}flag_USE_LXLITE -ne 0 @:>@; then
    add_flags="";
    if @<:@ ${D}EXPORT_BY_ORDINALS -ne 0 @:>@; then
        add_flags="-ynd"
    fi
    doCommand "lxlite -cs -t: -mrn -mln ${D}add_flags ${D}dllFile"
fi
doCommand "emxomf -s -l ${D}arcFile"

# Successful exit.
CleanUp 1
exit 0
EOF
dnl ===================== dllar.sh ends here =====================
])

AC_DEFUN([AC_BAKEFILE_CREATE_FILE_BK_DEPS],
[
dnl ===================== bk-deps begins here =====================
D='$'
cat <<EOF >bk-deps
#!/bin/sh

# This script is part of Bakefile (http://bakefile.sourceforge.net) autoconf
# script. It is used to track C/C++ files dependencies in portable way.
#
# Permission is given to use this file in any way.

DEPSMODE=${DEPSMODE}
DEPSDIR=.deps
DEPSFLAG_GCC="${DEPSFLAG_GCC}"
DEPSFLAG_MWCC="${DEPSFLAG_MWCC}"

mkdir -p ${D}DEPSDIR

if test ${D}DEPSMODE = gcc ; then
    ${D}* ${D}{DEPSFLAG_GCC}
    status=${D}?
    if test ${D}{status} != 0 ; then
        exit ${D}{status}
    fi
    # move created file to the location we want it in:
    while test ${D}# -gt 0; do
        case "${D}1" in
            -o )
                shift
                objfile=${D}1
            ;;
            -* )
            ;;
            * )
                srcfile=${D}1
            ;;
        esac
        shift
    done
    depfile=\`basename ${D}srcfile | sed -e 's/\\..*${D}/.d/g'\`
    depobjname=\`echo ${D}depfile |sed -e 's/\\.d/.o/g'\`
    if test -f ${D}depfile ; then
        sed -e "s,${D}depobjname:,${D}objfile:,g" ${D}depfile >${D}{DEPSDIR}/${D}{objfile}.d
        rm -f ${D}depfile
    else
        depfile=\`basename ${D}objfile | sed -e 's/\\..*${D}/.d/g'\`
        if test -f ${D}depfile ; then
            sed -e "/^${D}objfile/!s,${D}depobjname:,${D}objfile:,g" ${D}depfile >${D}{DEPSDIR}/${D}{objfile}.d
            rm -f ${D}depfile
        fi
    fi
    exit 0
elif test ${D}DEPSMODE = mwcc ; then
    ${D}*
    status=${D}?
    if test ${D}{status} != 0 ; then
        exit ${D}{status}
    fi
    # Run mwcc again with -MM and redirect into the dep file we want
    # NOTE: We can't use shift here because we need ${D}* to be valid
    prevarg=
    for arg in ${D}* ; do
        if test "${D}prevarg" = "-o"; then
            objfile=${D}arg
        else
            case "${D}arg" in
                -* )
                ;;
                * )
                    srcfile=${D}arg
                ;;
            esac
        fi
        prevarg="${D}arg"
    done
    ${D}* ${D}DEPSFLAG_MWCC >${D}{DEPSDIR}/${D}{objfile}.d
    exit 0
else
    ${D}*
    exit ${D}?
fi
EOF
dnl ===================== bk-deps ends here =====================
])

AC_DEFUN([AC_BAKEFILE_CREATE_FILE_SHARED_LD_SH],
[
dnl ===================== shared-ld-sh begins here =====================
D='$'
cat <<EOF >shared-ld-sh
#!/bin/sh
#-----------------------------------------------------------------------------
#-- Name:        distrib/mac/shared-ld-sh
#-- Purpose:     Link a mach-o dynamic shared library for Darwin / Mac OS X
#-- Author:      Gilles Depeyrot
#-- Copyright:   (c) 2002 Gilles Depeyrot
#-- Licence:     any use permitted
#-----------------------------------------------------------------------------

verbose=0
args=""
objects=""
linking_flag="-dynamiclib"

while test ${D}# -gt 0; do
    case ${D}1 in

       -v)
        verbose=1
        ;;

       -o|-compatibility_version|-current_version|-framework|-undefined|-install_name)
        # collect these options and values
        args="${D}{args} ${D}1 ${D}2"
        shift
        ;;

       -l*|-L*|-flat_namespace|-headerpad_max_install_names)
        # collect these options
        args="${D}{args} ${D}1"
        ;;

       -dynamiclib|-bundle)
        linking_flag="${D}1"
        ;;

       -*)
        echo "shared-ld: unhandled option '${D}1'"
        exit 1
        ;;

        *.o | *.a | *.dylib)
        # collect object files
        objects="${D}{objects} ${D}1"
        ;;

        *)
        echo "shared-ld: unhandled argument '${D}1'"
        exit 1
        ;;

    esac
    shift
done

#
# Link one module containing all the others
#
if test ${D}{verbose} = 1; then
    echo "c++ -r -keep_private_externs -nostdlib ${D}{objects} -o master.${D}${D}.o"
fi
c++ -r -keep_private_externs -nostdlib ${D}{objects} -o master.${D}${D}.o
status=${D}?
if test ${D}{status} != 0; then
    exit ${D}{status}
fi

#
# Link the shared library from the single module created
#
if test ${D}{verbose} = 1; then
    echo "cc ${D}{linking_flag} master.${D}${D}.o ${D}{args}"
fi
c++ ${D}{linking_flag} master.${D}${D}.o ${D}{args}
status=${D}?
if test ${D}{status} != 0; then
    exit ${D}{status}
fi

#
# Remove intermediate module
#
rm -f master.${D}${D}.o

exit 0
EOF
dnl ===================== shared-ld-sh ends here =====================
])

AC_DEFUN([AC_BAKEFILE_CREATE_FILE_BK_MAKE_PCH],
[
dnl ===================== bk-make-pch begins here =====================
D='$'
cat <<EOF >bk-make-pch
#!/bin/sh

# This script is part of Bakefile (http://bakefile.sourceforge.net) autoconf
# script. It is used to generated precompiled headers.
#
# Permission is given to use this file in any way.

outfile="${D}{1}"
header="${D}{2}"
shift
shift

compiler=
headerfile=
while test ${D}{#} -gt 0; do
    case "${D}{1}" in
        -I* )
            incdir=\`echo ${D}{1} | sed -e 's/-I\\(.*\\)/\\1/g'\`
            if test "x${D}{headerfile}" = "x" -a -f "${D}{incdir}/${D}{header}" ; then
                headerfile="${D}{incdir}/${D}{header}"
            fi
        ;;
    esac
    compiler="${D}{compiler} ${D}{1}"
    shift
done

if test "x${D}{headerfile}" = "x" ; then
    echo "error: can't find header ${D}{header} in include paths" >2
else
    if test -f ${D}{outfile} ; then
        rm -f ${D}{outfile}
    else
        mkdir -p \`dirname ${D}{outfile}\`
    fi
    depsfile=".deps/\`echo ${D}{outfile} | tr '/.' '__'\`.d"
    mkdir -p .deps
    # can do this because gcc is >= 3.4:
    ${D}{compiler} -o ${D}{outfile} -MMD -MF "${D}{depsfile}" "${D}{headerfile}"
    exit ${D}{?}
fi
EOF
dnl ===================== bk-make-pch ends here =====================
])

dnl
dnl AM_PATH_CPPUNIT(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(AM_PATH_CPPUNIT,
[

AC_ARG_WITH(cppunit-prefix,[  --with-cppunit-prefix=PFX   Prefix where CppUnit is installed (optional)],
            cppunit_config_prefix="$withval", cppunit_config_prefix="")
AC_ARG_WITH(cppunit-exec-prefix,[  --with-cppunit-exec-prefix=PFX  Exec prefix where CppUnit is installed (optional)],
            cppunit_config_exec_prefix="$withval", cppunit_config_exec_prefix="")

  if test x$cppunit_config_exec_prefix != x ; then
     cppunit_config_args="$cppunit_config_args --exec-prefix=$cppunit_config_exec_prefix"
     if test x${CPPUNIT_CONFIG+set} != xset ; then
        CPPUNIT_CONFIG=$cppunit_config_exec_prefix/bin/cppunit-config
     fi
  fi
  if test x$cppunit_config_prefix != x ; then
     cppunit_config_args="$cppunit_config_args --prefix=$cppunit_config_prefix"
     if test x${CPPUNIT_CONFIG+set} != xset ; then
        CPPUNIT_CONFIG=$cppunit_config_prefix/bin/cppunit-config
     fi
  fi

  AC_PATH_PROG(CPPUNIT_CONFIG, cppunit-config, no)
  cppunit_version_min=$1

  AC_MSG_CHECKING(for Cppunit - version >= $cppunit_version_min)
  no_cppunit=""
  if test "$CPPUNIT_CONFIG" = "no" ; then
    no_cppunit=yes
  else
    CPPUNIT_CFLAGS=`$CPPUNIT_CONFIG --cflags`
    CPPUNIT_LIBS=`$CPPUNIT_CONFIG --libs`
    cppunit_version=`$CPPUNIT_CONFIG --version`

    cppunit_major_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    cppunit_minor_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    cppunit_micro_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    cppunit_major_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    cppunit_minor_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    cppunit_micro_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    cppunit_version_proper=`expr \
        $cppunit_major_version \> $cppunit_major_min \| \
        $cppunit_major_version \= $cppunit_major_min \& \
        $cppunit_minor_version \> $cppunit_minor_min \| \
        $cppunit_major_version \= $cppunit_major_min \& \
        $cppunit_minor_version \= $cppunit_minor_min \& \
        $cppunit_micro_version \>= $cppunit_micro_min `

    if test "$cppunit_version_proper" = "1" ; then
      AC_MSG_RESULT([$cppunit_major_version.$cppunit_minor_version.$cppunit_micro_version])
    else
      AC_MSG_RESULT(no)
      no_cppunit=yes
    fi
  fi

  if test "x$no_cppunit" = x ; then
     ifelse([$2], , :, [$2])     
  else
     CPPUNIT_CFLAGS=""
     CPPUNIT_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(CPPUNIT_CFLAGS)
  AC_SUBST(CPPUNIT_LIBS)
])




