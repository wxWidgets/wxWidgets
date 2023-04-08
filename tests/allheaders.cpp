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

#if defined(__GNUC__) || defined(__clang__)
    // As above, we can't reuse wxCONCAT() and wxSTRINGIZE macros from wx/cpp.h
    // here, so define their equivalents here.
    #define CONCAT_HELPER(x, y) x ## y
    #define CONCAT(x1, x2)      CONCAT_HELPER(x1, x2)

    #define STRINGIZE_HELPER(x) #x
    #define STRINGIZE(x)        STRINGIZE_HELPER(x)

    #define WARNING_TURN_ON(comp, warn) \
        _Pragma(STRINGIZE(comp diagnostic error STRINGIZE(CONCAT(-W,warn))))
    #define WARNING_TURN_OFF(comp, warn) \
        _Pragma(STRINGIZE(comp diagnostic ignored STRINGIZE(CONCAT(-W,warn))))

    // Test for clang before gcc as clang defines __GNUC__ too.
    #if defined(__clang__)
        #define CLANG_TURN_ON(warn) WARNING_TURN_ON(clang, warn)
        #define CLANG_TURN_OFF(warn) WARNING_TURN_OFF(clang, warn)
    #elif defined(__GNUC__)
        #define GCC_TURN_ON(warn) WARNING_TURN_ON(GCC, warn)
        #define GCC_TURN_OFF(warn) WARNING_TURN_OFF(GCC, warn)
    #endif
#endif

// We have to include this one first in order to check for wxUSE_XXX below.
#include "wx/setup.h"

// Normally this is done in include/wx/msw/gccpriv.h included from wx/defs.h,
// but as we don't include it here, we need to do it manually to avoid warnings
// inside the standard headers included from catch.hpp.
#if defined(__CYGWIN__) && defined(__WINDOWS__)
    #define __USE_W32_SOCKETS
#endif

#include "catch2/catch.hpp"

#if defined(__WXQT__)
    // Include this one before enabling the warnings as doing it later, as it
    // happens when it's included from wx/fontutil.h, results in -Wsign-promo.
    #include <QtGui/QFont>
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
    //  - Remove GCC_TURN_ON(system-headers) from the list because this option
    //    will enable the warnings to be thrown inside the system headers that
    //    should instead be ignored.
    // {{{
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(abi)
#endif // 6.1
    GCC_TURN_ON(abi-tag)
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
    GCC_TURN_ON(delete-non-virtual-dtor)
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
    GCC_TURN_ON(format-zero-length)
    GCC_TURN_ON(ignored-qualifiers)
    GCC_TURN_ON(init-self)
    GCC_TURN_ON(inline)
    GCC_TURN_ON(invalid-pch)
    GCC_TURN_ON(literal-suffix)
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(logical-op)
#endif // 6.1
    GCC_TURN_ON(long-long)
    GCC_TURN_ON(main)
    GCC_TURN_ON(maybe-uninitialized)
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
    GCC_TURN_ON(narrowing)
    GCC_TURN_ON(noexcept)
#if CHECK_GCC_VERSION(7,1)
    GCC_TURN_ON(noexcept-type)
#endif // 7.1
    GCC_TURN_ON(non-virtual-dtor)
    GCC_TURN_ON(nonnull)
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
    GCC_TURN_ON(suggest-attribute=format)
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
    GCC_TURN_ON(unused-local-typedefs)
    GCC_TURN_ON(unused-macros)
    GCC_TURN_ON(unused-parameter)
    GCC_TURN_ON(unused-value)
    GCC_TURN_ON(unused-variable)
    GCC_TURN_ON(useless-cast)
    GCC_TURN_ON(variadic-macros)
    GCC_TURN_ON(vector-operation-performance)
#if CHECK_GCC_VERSION(6,1)
    GCC_TURN_ON(virtual-inheritance)
#endif // 6.1
    GCC_TURN_ON(vla)
#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_ON(volatile)
#endif // 10.1
    GCC_TURN_ON(volatile-register-var)
    GCC_TURN_ON(write-strings)
    GCC_TURN_ON(zero-as-null-pointer-constant)
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
    GCC_TURN_OFF(useless-cast)

#if CHECK_GCC_VERSION(10,1)
    GCC_TURN_OFF(redundant-tags) // "struct tm" triggers this
#endif // 10.1

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
    GCC_TURN_OFF(abi-tag)

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

// Do the same for clang too except here most of the useful warnings are
// already included in -Wall, so we just need a few more.
#ifdef CLANG_TURN_ON
    CLANG_TURN_ON(all)
    CLANG_TURN_ON(extra)
    CLANG_TURN_ON(pedantic)

    CLANG_TURN_ON(shorten-64-to-32)
#endif // clang


// UTF-8-only build is the only one in which we actually want to use implicit
// encoding (UTF-8).
#if !wxUSE_UTF8_LOCALE_ONLY
#define wxNO_IMPLICIT_WXSTRING_ENCODING
#endif

#include "testprec.h"

#include "allheaders.h"

// Check that using wx macros doesn't result in -Wsuggest-override or
// equivalent warnings in classes using and not using "override".
struct Base : wxEvtHandler
{
    virtual ~Base() { }

    virtual void Foo() { }
};

struct DerivedWithoutOverride : Base
{
    void OnIdle(wxIdleEvent&) { }

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(DerivedWithoutOverride);
    wxDECLARE_EVENT_TABLE();
};

struct DerivedWithOverride : Base
{
    virtual void Foo() override { }

    void OnIdle(wxIdleEvent&) { }

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(DerivedWithOverride);
    wxDECLARE_EVENT_TABLE();
};

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
