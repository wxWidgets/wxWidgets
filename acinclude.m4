dnl ---------------------------------------------------------------------------
dnl
dnl Purpose: Cursom macros for autoconf configure script.
dnl Author:  Vadim Zeitlin
dnl Created: 26.05.99
dnl Version: $Id$
dnl ---------------------------------------------------------------------------

dnl ===========================================================================
dnl GKT+ version test
dnl ===========================================================================

dnl ---------------------------------------------------------------------------
dnl AM_PATH_GTK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for GTK, and define GTK_CFLAGS and GTK_LIBS. Uses variables
dnl gtk_config_prefix and/or gtk_config_exec_prefix if defined.
dnl ---------------------------------------------------------------------------
dnl
AC_DEFUN(AM_PATH_GTK,
[
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
          wx_arg_cache_file="configarg.cache"
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

