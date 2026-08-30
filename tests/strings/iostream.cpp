//////////////////////////////////////////////////////////////////////////////
// Name:        tests/strings/iostream.cpp
// Purpose:     unit test of wxString interaction with std::[io]stream
// Author:      Vadim Zeitlin
// Created:     2007-10-09
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif // WX_PRECOMP

#if wxUSE_STD_IOSTREAM

#include <sstream>

#define ASSERT_OSTREAM_EQUAL(p, s) CHECK(s.str() == std::string(p))
#define ASSERT_WOSTREAM_EQUAL(p, s) CHECK(s.str() == std::wstring(p))

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("StringIostream::Out", "[wxString][iostream]")
{
    std::ostringstream s;
    s << wxString("hello");
    ASSERT_OSTREAM_EQUAL("hello", s);

#if defined(HAVE_WOSTREAM)
    std::wostringstream ws;
    ws << wxString("bye");
    ASSERT_WOSTREAM_EQUAL(L"bye", ws);
#endif
}

#endif // wxUSE_STD_IOSTREAM
