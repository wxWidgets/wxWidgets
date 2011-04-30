///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/htmlparser.cpp
// Purpose:     wxHtmlParser tests
// Author:      Vadim Zeitlin
// Created:     2011-01-13
// RCS-ID:      $Id$
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
#endif // WX_PRECOMP

#include "wx/html/htmlpars.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class HtmlParserTestCase : public CppUnit::TestCase
{
public:
    HtmlParserTestCase() { }

private:
    CPPUNIT_TEST_SUITE( HtmlParserTestCase );
        CPPUNIT_TEST( Invalid );
    CPPUNIT_TEST_SUITE_END();

    void Invalid();

    wxDECLARE_NO_COPY_CLASS(HtmlParserTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HtmlParserTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HtmlParserTestCase, "HtmlParserTestCase" );

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

// Test that parsing invalid HTML simply fails but doesn't crash for example.
void HtmlParserTestCase::Invalid()
{
    class NullParser : public wxHtmlParser
    {
    public:
        virtual wxObject *GetProduct() { return NULL; }

    protected:
        virtual void AddText(const wxString& WXUNUSED(txt)) { }
    };

    NullParser p;
    p.Parse("<");
    p.Parse("<foo");
    p.Parse("<!--");
    p.Parse("<!---");
}

#endif //wxUSE_HTML
