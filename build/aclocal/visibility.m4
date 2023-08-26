dnl visibility.m4 serial 1 (gettext-0.15)
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Bruno Haible.

dnl Modified for use in wxWidgets by Vaclav Slavik:
dnl    - don't define HAVE_VISIBILITY (=0) if not supported
dnl    - use -fvisibility-inlines-hidden too
dnl    - test in C++ mode

dnl Tests whether the compiler supports the command-line option
dnl -fvisibility=hidden and the function and variable attributes
dnl __attribute__((__visibility__("hidden"))) and
dnl __attribute__((__visibility__("default"))).
dnl Does *not* test for __visibility__("protected") - which has tricky
dnl semantics (see the 'vismain' test in glibc) and does not exist e.g. on
dnl MacOS X.
dnl Does *not* test for __visibility__("internal") - which has processor
dnl dependent semantics.
dnl Does *not* test for #pragma GCC visibility push(hidden) - which is
dnl "really only recommended for legacy code".
dnl Set the variable CFLAG_VISIBILITY.
dnl Defines and sets the variable HAVE_VISIBILITY.

AC_DEFUN([WX_VISIBILITY],
[
  AC_REQUIRE([AC_PROG_CC])
  if test -n "$GCC"; then
    CFLAGS_VISIBILITY="-fvisibility=hidden"
    CXXFLAGS_VISIBILITY="-fvisibility=hidden -fvisibility-inlines-hidden"
    AC_MSG_CHECKING([for symbols visibility support])
    AC_CACHE_VAL(wx_cv_cc_visibility, [
      wx_save_CXXFLAGS="$CXXFLAGS"
      CXXFLAGS="$CXXFLAGS $CXXFLAGS_VISIBILITY"
      AC_LANG_PUSH(C++)
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
        [
         /* we need gcc >= 4.0, older versions with visibility support
            didn't have class visibility: */
         #if defined(__GNUC__) && __GNUC__ < 4
         error this gcc is too old;
         #endif

         /* visibility only makes sense for ELF shared libs: */
         #if !defined(__ELF__) && !defined(__APPLE__)
         error this platform has no visibility;
         #endif

         /* At the time of Xcode 4.1 / Clang 3, Clang++ still didn't
            have the bugs sorted out. These were fixed starting with
            Xcode 4.6.0 / Apple Clang 4.2 (which is based on Clang 3.2 so
            check for that version too). */
         #ifdef __clang__
            #ifdef __APPLE__
                #if __clang_major__ < 4 \
                    || (__clang_major__ == 4 && __clang_minor__ < 2)
                    error Clang compiler version < 4.2 is broken w.r.t. visibility;
                #endif
            #else
                #if __clang_major__ < 3 \
                    || (__clang_major__ == 3 && __clang_minor__ < 2)
                    error Clang compiler version < 3.2 is broken w.r.t. visibility;
                #endif
            #endif
         #endif

         extern __attribute__((__visibility__("hidden"))) int hiddenvar;
         extern __attribute__((__visibility__("default"))) int exportedvar;
         extern __attribute__((__visibility__("hidden"))) int hiddenfunc (void);
         extern __attribute__((__visibility__("default"))) int exportedfunc (void);
         class __attribute__((__visibility__("default"))) Foo {
           Foo() {}
         };
        ],
        [])],
        wx_cv_cc_visibility=yes,
        wx_cv_cc_visibility=no)
      AC_LANG_POP()
      CXXFLAGS="$wx_save_CXXFLAGS"])
    AC_MSG_RESULT([$wx_cv_cc_visibility])
    if test $wx_cv_cc_visibility = yes; then
      dnl we do have basic visibility support, now check if we can use it:
      dnl
      dnl Debian/Ubuntu's gcc 4.1 is affected:
      dnl https://bugs.launchpad.net/ubuntu/+source/gcc-4.1/+bug/109262
      AC_MSG_CHECKING([for broken libstdc++ visibility])
      AC_CACHE_VAL(wx_cv_cc_broken_libstdcxx_visibility, [
        wx_save_CXXFLAGS="$CXXFLAGS"
        wx_save_LDFLAGS="$LDFLAGS"
        CXXFLAGS="$CXXFLAGS $CXXFLAGS_VISIBILITY"
        LDFLAGS="$LDFLAGS -shared -fPIC"
        AC_LANG_PUSH(C++)
        AC_LINK_IFELSE([AC_LANG_PROGRAM(
          [
            #include <string>
          ],
          [
            std::string s("hello");
            return s.length();
          ])],
          wx_cv_cc_broken_libstdcxx_visibility=no,
          wx_cv_cc_broken_libstdcxx_visibility=yes)
        AC_LANG_POP()
        CXXFLAGS="$wx_save_CXXFLAGS"
        LDFLAGS="$wx_save_LDFLAGS"])
      AC_MSG_RESULT([$wx_cv_cc_broken_libstdcxx_visibility])

      if test $wx_cv_cc_broken_libstdcxx_visibility = yes; then
        AC_MSG_CHECKING([whether we can work around it])
        AC_CACHE_VAL(wx_cv_cc_visibility_workaround, [
          AC_LANG_PUSH(C++)
          AC_LINK_IFELSE([AC_LANG_PROGRAM(
            [
              #pragma GCC visibility push(default)
              #include <string>
              #pragma GCC visibility pop
            ],
            [
              std::string s("hello");
              return s.length();
            ])],
            wx_cv_cc_visibility_workaround=no,
            wx_cv_cc_visibility_workaround=yes)
          AC_LANG_POP()
        ])
        AC_MSG_RESULT([$wx_cv_cc_visibility_workaround])

        if test $wx_cv_cc_visibility_workaround = no; then
          dnl we can't use visibility at all then
          wx_cv_cc_visibility=no
        fi
      fi
    fi

    if test $wx_cv_cc_visibility = yes; then
      AC_DEFINE([HAVE_VISIBILITY])
      if test $wx_cv_cc_broken_libstdcxx_visibility = yes; then
        AC_DEFINE([HAVE_BROKEN_LIBSTDCXX_VISIBILITY])
      fi
    else
      CFLAGS_VISIBILITY=""
      CXXFLAGS_VISIBILITY=""
    fi
    AC_SUBST([CFLAGS_VISIBILITY])
    AC_SUBST([CXXFLAGS_VISIBILITY])
  fi
])
