dnl @synopsis AX_FUNC_WHICH_GETHOSTBYNAME_R
dnl
dnl Determines which historical variant of the gethostbyname_r() call
dnl (taking three, five, or six arguments) is available on the system
dnl and defines one of the following macros accordingly:
dnl
dnl   HAVE_FUNC_GETHOSTBYNAME_R_6
dnl   HAVE_FUNC_GETHOSTBYNAME_R_5
dnl   HAVE_FUNC_GETHOSTBYNAME_R_3
dnl
dnl If used in conjunction with gethostname.c, the API demonstrated in
dnl test.c can be used regardless of which gethostbyname_r() is
dnl available. These example files can be found at
dnl http://www.csn.ul.ie/~caolan/publink/gethostbyname_r
dnl
dnl based on David Arnold's autoconf suggestion in the threads faq
dnl
dnl Originally named "AC_caolan_FUNC_WHICH_GETHOSTBYNAME_R". Rewritten
dnl for Autoconf 2.5x by Daniel Richard G.
dnl
dnl @category InstalledPackages
dnl @author Caolan McNamara <caolan@skynet.ie>
dnl @author Daniel Richard G. <skunk@iskunk.org>
dnl @version 2005-01-21
dnl @license GPLWithACException

AC_DEFUN([AX_FUNC_WHICH_GETHOSTBYNAME_R], [

    AC_LANG_PUSH(C)
    AC_MSG_CHECKING([how many arguments gethostbyname_r() takes])

    AC_CACHE_VAL(ac_cv_func_which_gethostbyname_r, [

################################################################

ac_cv_func_which_gethostbyname_r=unknown

#
# ONE ARGUMENT (sanity check)
#

# This should fail, as there is no variant of gethostbyname_r() that takes
# a single argument. If it actually compiles, then we can assume that
# netdb.h is not declaring the function, and the compiler is thereby
# assuming an implicit prototype. In which case, we're out of luck.
#
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [[#include <netdb.h>]],
        [[
            char *name = "www.gnu.org";
            (void)gethostbyname_r(name) /* ; */
        ]]
    )],
    ac_cv_func_which_gethostbyname_r=no
    )

#
# SIX ARGUMENTS
# (e.g. Linux)
#

if test "$ac_cv_func_which_gethostbyname_r" = "unknown"; then

AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [[#include <netdb.h>]],
        [[
            char *name = "www.gnu.org";
            struct hostent ret, *retp;
            char buf@<:@1024@:>@;
            int buflen = 1024;
            int my_h_errno;
            (void)gethostbyname_r(name, &ret, buf, buflen, &retp, &my_h_errno) /* ; */
        ]]
    )],
    ac_cv_func_which_gethostbyname_r=six
    )

fi

#
# FIVE ARGUMENTS
# (e.g. Solaris)
#

if test "$ac_cv_func_which_gethostbyname_r" = "unknown"; then

AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
	[[#include <netdb.h>]],
        [[
            char *name = "www.gnu.org";
            struct hostent ret;
            char buf@<:@1024@:>@;
            int buflen = 1024;
            int my_h_errno;
            (void)gethostbyname_r(name, &ret, buf, buflen, &my_h_errno) /* ; */
        ]]
    )],
    ac_cv_func_which_gethostbyname_r=five
    )

fi

#
# THREE ARGUMENTS
# (e.g. AIX, HP-UX, Tru64)
#

if test "$ac_cv_func_which_gethostbyname_r" = "unknown"; then

AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
        [[#include <netdb.h>]],
        [[
            char *name = "www.gnu.org";
            struct hostent ret;
            struct hostent_data data;
            (void)gethostbyname_r(name, &ret, &data) /* ; */
        ]]
    )],
    ac_cv_func_which_gethostbyname_r=three
    )

fi

################################################################

]) dnl end AC_CACHE_VAL

case "$ac_cv_func_which_gethostbyname_r" in
    three)
    AC_MSG_RESULT([three])
    AC_DEFINE(HAVE_FUNC_GETHOSTBYNAME_R_3)
    ;;

    five)
    AC_MSG_RESULT([five])
    AC_DEFINE(HAVE_FUNC_GETHOSTBYNAME_R_5)
    ;;

    six)
    AC_MSG_RESULT([six])
    AC_DEFINE(HAVE_FUNC_GETHOSTBYNAME_R_6)
    ;;

    no)
    AC_MSG_RESULT([cannot find function declaration in netdb.h])
    ;;

    unknown)
    AC_MSG_RESULT([can't tell])
    ;;

    *)
    AC_MSG_ERROR([internal error])
    ;;
esac

AC_LANG_POP(C)

]) dnl end AC_DEFUN
