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

// Avoid pre-compiled headers at all
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/setup.h"

#if !wxUSE_UTF8_LOCALE_ONLY
#define wxNO_IMPLICIT_WXSTRING_ENCODING
#endif

#include "testprec.h"

#include "allheaders.h"

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
