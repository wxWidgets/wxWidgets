///////////////////////////////////////////////////////////////////////////////
// Name:        tests/allheaders.cpp
// Purpose:     Compilation test for all headers
// Author:      Vadim Zeitlin, Arrigo Marchiori
// Created:     2020-04-20
// Copyright:   (c) 2010,2020 Vadim Zeitlin, Wlodzimierz Skiba, Arrigo Marchiori
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// Note: can't use wxCHECK_GCC_VERSION() here as it's not defined yet.
#if defined(__GNUC__)
    #define CHECK_GCC_VERSION(major, minor) \
      (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#else
    #define CHECK_GCC_VERSION(major, minor) 0
#endif

#if CHECK_GCC_VERSION(4, 6)
    // As above, we can't reuse wxCONCAT() and wxSTRINGIZE macros from wx/cpp.h
    // here, so define their equivalents here.
    #define CONCAT_HELPER(x, y) x ## y
    #define CONCAT(x1, x2)      CONCAT_HELPER(x1, x2)

    #define STRINGIZE_HELPER(x) #x
    #define STRINGIZE(x)        STRINGIZE_HELPER(x)

    #define GCC_TURN_ON(warn) \
        _Pragma(STRINGIZE(GCC diagnostic error STRINGIZE(CONCAT(-W,warn))))
    #define GCC_TURN_OFF(warn) \
        _Pragma(STRINGIZE(GCC diagnostic ignored STRINGIZE(CONCAT(-W,warn))))
#endif

// Due to what looks like a bug in gcc, some warnings enabled after including
// the standard headers still result in warnings being given when instantiating
// some functions defined in these headers later and we need to explicitly
// disable these warnings to avoid them, even if they're not enabled yet.
#ifdef GCC_TURN_OFF
    #pragma GCC diagnostic push

    GCC_TURN_OFF(aggregate-return)
    GCC_TURN_OFF(conversion)
    GCC_TURN_OFF(format)
    GCC_TURN_OFF(padded)
    GCC_TURN_OFF(parentheses)
    GCC_TURN_OFF(sign-compare)
    GCC_TURN_OFF(sign-conversion)
    GCC_TURN_OFF(unused-parameter)
    GCC_TURN_OFF(zero-as-null-pointer-constant)
#endif

// We have to include this one first in order to check for HAVE_XXX below.
#include "wx/setup.h"

// Include all standard headers that are used in wx headers before enabling the
// warnings below.
#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#if defined(HAVE_STD_UNORDERED_MAP)
    #include <unordered_map>
#endif
#if defined(HAVE_STD_UNORDERED_SET)
    #include <unordered_set>
#endif

#if defined(HAVE_DLOPEN)
    #include <dlfcn.h>
#endif
#include <fcntl.h>

#include "catch.hpp"

#if defined(__WXMSW__)
    #include <windows.h>

    // Avoid warnings about redeclaring standard functions such as chmod() in
    // various standard headers when using MinGW/Cygwin.
    #if defined(__MINGW32__) || defined(__CYGWIN__)
        #include <stdio.h>
        #include <unistd.h>
        #include <sys/stat.h>
        #include <io.h>
    #endif
#elif defined(__WXQT__)
    #include <QtGui/QFont>
#endif

#ifdef GCC_TURN_OFF
    #pragma GCC diagnostic pop
#endif

// Enable max warning level for headers if possible, using gcc pragmas.
#ifdef GCC_TURN_ON
    // Enable all the usual "maximal" warnings.
    GCC_TURN_ON(all)
    GCC_TURN_ON(extra)
    GCC_TURN_ON(pedantic)

    // Enable all the individual warnings not enabled by one of the warnings
    // above.

    // Start of semi-auto-generated part, don't modify it manually.
    //
    // Manual changes:
    //  - Globally replace HANDLE_GCC_WARNING with GCC_TURN_ON.
    //  - Add v6 check for -Wabi, gcc < 6 don't seem to support turning it off
    //    once it's turned on and gives it for the standard library symbols.
    // {{{
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(abi)
#endif // 6.1
#if CHECK_GCC_VERSION(4,8)
    GCC_TURN_ON(abi-tag)
#endif // 4.8
    GCC_TURN_ON(address)
    GCC_TURN_ON(aggregate-return)
#if CHECK_GCC_VERSION(7,1)
    GCC_TURN_ON(alloc-zero)
#endif // 7.1
#if CHECK_GCC_VERSION(7,1)
    GCC_TURN_ON(alloca)
#endif // 7.1
#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_ON(arith-conversion)
#endif // 10.1
    GCC_TURN_ON(array-bounds)
    GCC_TURN_ON(builtin-macro-redefined)
    GCC_TURN_ON(cast-align)
#if CHECK_GCC_VERSION(8,1)
    GCC_TURN_ON(cast-align=strict)
#endif // 8.1
    GCC_TURN_ON(cast-qual)
    GCC_TURN_ON(char-subscripts)
#if CHECK_GCC_VERSION(9,1)
    GCC_TURN_ON(chkp)
#endif // 9.1
#if CHECK_GCC_VERSION(8,1)
    GCC_TURN_ON(class-memaccess)
#endif // 8.1
    GCC_TURN_ON(clobbered)
#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_ON(comma-subscript)
#endif // 10.1
    GCC_TURN_ON(comment)
#if CHECK_GCC_VERSION(4,9)
    GCC_TURN_ON(conditionally-supported)
#endif // 4.9
    GCC_TURN_ON(conversion)
    GCC_TURN_ON(ctor-dtor-privacy)
#if CHECK_GCC_VERSION(4,9)
    GCC_TURN_ON(date-time)
#endif // 4.9
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(delete-non-virtual-dtor)
#endif // 4.7
#if CHECK_GCC_VERSION(9,1)
    GCC_TURN_ON(deprecated-copy)
#endif // 9.1
#if CHECK_GCC_VERSION(9,1)
    GCC_TURN_ON(deprecated-copy-dtor)
#endif // 9.1
    GCC_TURN_ON(disabled-optimization)
    GCC_TURN_ON(double-promotion)
#if CHECK_GCC_VERSION(7,1)
    GCC_TURN_ON(duplicated-branches)
#endif // 7.1
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(duplicated-cond)
#endif // 6.1
    GCC_TURN_ON(empty-body)
    GCC_TURN_ON(endif-labels)
    GCC_TURN_ON(enum-compare)
#if CHECK_GCC_VERSION(8,1)
    GCC_TURN_ON(extra-semi)
#endif // 8.1
    GCC_TURN_ON(float-equal)
    GCC_TURN_ON(format)
    GCC_TURN_ON(format-contains-nul)
    GCC_TURN_ON(format-extra-args)
    GCC_TURN_ON(format-nonliteral)
    GCC_TURN_ON(format-security)
#if CHECK_GCC_VERSION(5,1)
    GCC_TURN_ON(format-signedness)
#endif // 5.1
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(format-zero-length)
#endif // 4.7
    GCC_TURN_ON(ignored-qualifiers)
    GCC_TURN_ON(init-self)
    GCC_TURN_ON(inline)
    GCC_TURN_ON(invalid-pch)
#if CHECK_GCC_VERSION(4,8)
    GCC_TURN_ON(literal-suffix)
#endif // 4.8
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(logical-op)
#endif // 6.1
    GCC_TURN_ON(long-long)
    GCC_TURN_ON(main)
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(maybe-uninitialized)
#endif // 4.7
#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_ON(mismatched-tags)
#endif // 10.1
    GCC_TURN_ON(missing-braces)
    GCC_TURN_ON(missing-declarations)
    GCC_TURN_ON(missing-field-initializers)
    GCC_TURN_ON(missing-format-attribute)
    GCC_TURN_ON(missing-include-dirs)
    GCC_TURN_ON(missing-noreturn)
    GCC_TURN_ON(multichar)
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(namespaces)
#endif // 6.1
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(narrowing)
#endif // 4.7
    GCC_TURN_ON(noexcept)
#if CHECK_GCC_VERSION(7,1)
    GCC_TURN_ON(noexcept-type)
#endif // 7.1
    GCC_TURN_ON(non-virtual-dtor)
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(nonnull)
#endif // 4.7
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(null-dereference)
#endif // 6.1
    GCC_TURN_ON(old-style-cast)
    GCC_TURN_ON(overlength-strings)
    GCC_TURN_ON(overloaded-virtual)
    GCC_TURN_ON(packed)
    GCC_TURN_ON(packed-bitfield-compat)
    GCC_TURN_ON(padded)
    GCC_TURN_ON(parentheses)
#if CHECK_GCC_VERSION(9,1)
    GCC_TURN_ON(pessimizing-move)
#endif // 9.1
    GCC_TURN_ON(pointer-arith)
    GCC_TURN_ON(redundant-decls)
#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_ON(redundant-tags)
#endif // 10.1
#if CHECK_GCC_VERSION(7,1)
    GCC_TURN_ON(register)
#endif // 7.1
    GCC_TURN_ON(reorder)
#if CHECK_GCC_VERSION(7,1)
    GCC_TURN_ON(restrict)
#endif // 7.1
    GCC_TURN_ON(return-type)
    GCC_TURN_ON(sequence-point)
    GCC_TURN_ON(shadow)
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(shift-negative-value)
#endif // 6.1
    GCC_TURN_ON(sign-compare)
    GCC_TURN_ON(sign-conversion)
    GCC_TURN_ON(sign-promo)
    GCC_TURN_ON(stack-protector)
    GCC_TURN_ON(strict-aliasing)
    GCC_TURN_ON(strict-null-sentinel)
    GCC_TURN_ON(strict-overflow)
#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_ON(string-compare)
#endif // 10.1
#if CHECK_GCC_VERSION(8,1)
    GCC_TURN_ON(stringop-truncation)
#endif // 8.1
#if CHECK_GCC_VERSION(8,1)
    GCC_TURN_ON(suggest-attribute=cold)
#endif // 8.1
    GCC_TURN_ON(suggest-attribute=const)
#if CHECK_GCC_VERSION(4,8)
    GCC_TURN_ON(suggest-attribute=format)
#endif // 4.8
#if CHECK_GCC_VERSION(8,1)
    GCC_TURN_ON(suggest-attribute=malloc)
#endif // 8.1
    GCC_TURN_ON(suggest-attribute=noreturn)
    GCC_TURN_ON(suggest-attribute=pure)
#if CHECK_GCC_VERSION(5,1)
    GCC_TURN_ON(suggest-final-methods)
#endif // 5.1
#if CHECK_GCC_VERSION(5,1)
    GCC_TURN_ON(suggest-final-types)
#endif // 5.1
#if CHECK_GCC_VERSION(5,1)
    GCC_TURN_ON(suggest-override)
#endif // 5.1
    GCC_TURN_ON(switch)
    GCC_TURN_ON(switch-default)
    GCC_TURN_ON(switch-enum)
    GCC_TURN_ON(synth)
    GCC_TURN_ON(system-headers)
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(templates)
#endif // 6.1
    GCC_TURN_ON(trampolines)
    GCC_TURN_ON(trigraphs)
    GCC_TURN_ON(type-limits)
    GCC_TURN_ON(undef)
    GCC_TURN_ON(uninitialized)
    GCC_TURN_ON(unknown-pragmas)
    GCC_TURN_ON(unreachable-code)
    GCC_TURN_ON(unsafe-loop-optimizations)
    GCC_TURN_ON(unused)
    GCC_TURN_ON(unused-but-set-parameter)
    GCC_TURN_ON(unused-but-set-variable)
    GCC_TURN_ON(unused-function)
    GCC_TURN_ON(unused-label)
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(unused-local-typedefs)
#endif // 4.7
    GCC_TURN_ON(unused-macros)
    GCC_TURN_ON(unused-parameter)
    GCC_TURN_ON(unused-value)
    GCC_TURN_ON(unused-variable)
#if CHECK_GCC_VERSION(4,8)
    GCC_TURN_ON(useless-cast)
#endif // 4.8
    GCC_TURN_ON(variadic-macros)
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(vector-operation-performance)
#endif // 4.7
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(virtual-inheritance)
#endif // 6.1
    GCC_TURN_ON(vla)
#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_ON(volatile)
#endif // 10.1
    GCC_TURN_ON(volatile-register-var)
    GCC_TURN_ON(write-strings)
#if CHECK_GCC_VERSION(4,7)
    GCC_TURN_ON(zero-as-null-pointer-constant)
#endif // 4.7
    // }}}

    #undef GCC_TURN_ON

    /*
        Some warnings still must be disabled, so turn them back off explicitly:
    */

    // Those are potentially useful warnings, but there are just too many
    // occurrences of them in our code currently.
    GCC_TURN_OFF(conversion)
    GCC_TURN_OFF(sign-compare)
    GCC_TURN_OFF(sign-conversion)

    GCC_TURN_OFF(old-style-cast)
#if CHECK_GCC_VERSION(4,8)
    GCC_TURN_OFF(useless-cast)
#endif // 4.8

#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_OFF(redundant-tags) // "struct tm" triggers this
#endif // 10.1

    // This one is given for NULL, and not just literal 0, up to gcc 10, and so
    // has to remain disabled for as long as we use any NULLs in our code.
#if CHECK_GCC_VERSION(4,7) && !CHECK_GCC_VERSION(10,1)
    GCC_TURN_OFF(zero-as-null-pointer-constant)
#endif

    // These ones could be useful to explore, but for now we don't use "final"
    // at all anywhere.
#if CHECK_GCC_VERSION(5,1)
    GCC_TURN_OFF(suggest-final-methods)
    GCC_TURN_OFF(suggest-final-types)
#endif // 5.1

    // wxWARNING_SUPPRESS_MISSING_OVERRIDE() inside wxRTTI macros is just
    // ignored by gcc up to 9.x for some reason, so we have no choice but to
    // disable it.
#if CHECK_GCC_VERSION(5,1) && !CHECK_GCC_VERSION(9,0)
    GCC_TURN_OFF(suggest-override)
#endif

    // The rest are the warnings that we don't ever want to enable.

    // This one is given whenever inheriting from std:: classes without using
    // C++11 ABI tag explicitly, probably harmless.
#if CHECK_GCC_VERSION(4,8)
    GCC_TURN_OFF(abi-tag)
#endif // 4.8

    // This can be used to ask the compiler to explain why some function is not
    // inlined, but it's perfectly normal for some functions not to be inlined.
    GCC_TURN_OFF(inline)

    // All those are about using perfectly normal C++ features that are
    // actually used in our code.
    GCC_TURN_OFF(aggregate-return)
    GCC_TURN_OFF(long-long)
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_OFF(namespaces)
    GCC_TURN_OFF(multiple-inheritance)
    GCC_TURN_OFF(templates)
#endif // 6.1

    // This warns about missing "default" label in exhaustive switches over
    // enums, so it's completely useless.
    GCC_TURN_OFF(switch-default)

    // We don't care about padding being added to our structs.
    GCC_TURN_OFF(padded)
#endif // gcc >= 4.6

// ANSI build hasn't been updated to work without implicit wxString encoding
// and never will be, as it will be removed soon anyhow. And in UTF-8-only
// build we actually want to use implicit encoding (UTF-8).
#if wxUSE_UNICODE && !wxUSE_UTF8_LOCALE_ONLY
#define wxNO_IMPLICIT_WXSTRING_ENCODING
#endif

#include "testprec.h"

#include "allheaders.h"

#ifdef GCC_TURN_OFF
    // Just using REQUIRE() below triggers -Wparentheses, so avoid it.
    GCC_TURN_OFF(parentheses)
#endif

TEST_CASE("wxNO_IMPLICIT_WXSTRING_ENCODING", "[string]")
{
    wxString s = wxASCII_STR("Hello, ASCII");
    REQUIRE(s == L"Hello, ASCII");
#ifdef TEST_IMPLICIT_WXSTRING_ENCODING
    // Compilation of this should fail, because the macro
    // wxNO_IMPLICIT_WXSTRING_ENCODING must be set
    s = "Hello, implicit encoding";
#endif
}
