
dnl Checks for availability of GCC's atomic operations builtins.
dnl Defines HAVE_GCC_ATOMIC_BUILTINS if available.

AC_DEFUN([WX_ATOMIC_BUILTINS],
[
  AC_REQUIRE([AC_PROG_CC])
  if test -n "$GCC"; then
    AC_MSG_CHECKING([for __sync_xxx_and_fetch builtins])
    AC_CACHE_VAL(wx_cv_cc_gcc_atomic_builtins, [
      AC_TRY_LINK(
        [],
        [
          unsigned int value=0;
          volatile unsigned int r1 = __sync_add_and_fetch(&value, 2);
          volatile unsigned int r2 = __sync_sub_and_fetch(&value, 1);
        ],
        wx_cv_cc_gcc_atomic_builtins=yes,
        wx_cv_cc_gcc_atomic_builtins=no)
      ])
    AC_MSG_RESULT([$wx_cv_cc_gcc_atomic_builtins])
    if test $wx_cv_cc_gcc_atomic_builtins = yes; then
      AC_DEFINE([HAVE_GCC_ATOMIC_BUILTINS])
    fi
  fi
])
