///////////////////////////////////////////////////////////////////////////////
// Name:        tests/allheaders_utf8.cpp
// Purpose:     Compilation test for all headers with wxALLOW_UTF8_LITERALS
// Author:      Vadim Zeitlin
// Created:     2026-10-20
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// This file doesn't contain any tests, it only checks that all headers can be
// compiled when wxALLOW_UTF8_LITERALS is defined, see strings/utf8literals.cpp
// for the tests of its behaviour.

// See strings/utf8literals.cpp for the explanation of these checks.
#include "wx/platform.h"

#if !wxUSE_UTF8_LOCALE_ONLY && \
    (!defined(_MSC_VER) || \
        (defined(_MSVC_EXECUTION_CHARACTER_SET) && \
            _MSVC_EXECUTION_CHARACTER_SET == 65001))
    #define wxNO_IMPLICIT_WXSTRING_ENCODING
    #define wxALLOW_UTF8_LITERALS
#endif

#include "testprec.h"

#ifdef wxALLOW_UTF8_LITERALS

// Temporarily suppressing warnings about deprecated functions is broken in
// gcc 4.8, so disable these warnings globally for it.
#if wxCHECK_GCC_VERSION(4, 8) && !wxCHECK_GCC_VERSION(4, 9)
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "allheaders.h"

#include <type_traits>

// Check that the symbols defined above were really taken into account, which
// wouldn't be the case if precompiled headers were used for this file.
static_assert(std::is_convertible<const char (&)[4], wxString>::value,
              "string literals should be convertible to wxString");
static_assert(!std::is_convertible<const char*, wxString>::value,
              "char pointers shouldn't be convertible to wxString");

#endif // wxALLOW_UTF8_LITERALS
