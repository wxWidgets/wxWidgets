///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmlparser.cpp
// Purpose:     wxHtmlParser tests
// Author:      Vadim Zeitlin
// Created:     2011-01-13
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_HTML

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcmemory.h"
#endif // WX_PRECOMP

#include "wx/html/winpars.h"

// Test that parsing invalid HTML simply fails but doesn't crash for example.
TEST_CASE("wxHtmlParser::ParseInvalid", "[html][parser][error]")
{
    class NullParser : public wxHtmlWinParser
    {
    public:
        virtual wxObject *GetProduct() { return NULL; }

    protected:
        virtual void AddText(const wxString& WXUNUSED(txt)) { }
    };

    NullParser p;
    wxMemoryDC dc;
    p.SetDC(&dc);

    p.Parse("<");
    p.Parse("<foo");
    p.Parse("<!--");
    p.Parse("<!---");
}

#endif //wxUSE_HTML
