///////////////////////////////////////////////////////////////////////////////
// Name:        tests/mbconv/main.cpp
// Purpose:     wxMBConv unit test
// Author:      Vadim Zeitlin
// Created:     14.02.04
// RCS-ID:      $Id$
// Copyright:   (c) 2003 TT-Solutions
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/strconv.h"
#include "wx/string.h"

#if wxUSE_FONTMAP
    #include "wx/fontmap.h"
#endif // wxUSE_FONTMAP

#include "wx/cppunit.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MBConvTestCase : public CppUnit::TestCase
{
public:
    MBConvTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MBConvTestCase );
        CPPUNIT_TEST( WC2CP1250 );
        CPPUNIT_TEST( Charsets );
    CPPUNIT_TEST_SUITE_END();

    void WC2CP1250();
    void Charsets();

    DECLARE_NO_COPY_CLASS(MBConvTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MBConvTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MBConvTestCase, "MBConvTestCase" );

void MBConvTestCase::WC2CP1250()
{
    static const struct Data
    {
        const wchar_t *wc;
        const char *cp1250;
    } data[] =
    {
        { L"hello", "hello" },  // test that it works in simplest case
        { L"½ of ½ is ¼", NULL }, // this should fail as cp1250 doesn't have 1/2
    };

    wxCSConv cs1250(wxFONTENCODING_CP1250);
    for ( size_t n = 0; n < WXSIZEOF(data); n++ )
    {
        const Data& d = data[n];
        if (d.cp1250)
        {
            CPPUNIT_ASSERT( strcmp(cs1250.cWC2MB(d.wc), d.cp1250) == 0 );
        }
        else
        {
            CPPUNIT_ASSERT( (const char*)cs1250.cWC2MB(d.wc) == NULL );
        }
    }
}

void MBConvTestCase::Charsets()
{
#if wxUSE_FONTMAP

    static const wxChar *charsets[] =
    {
        // some vali charsets
        _T("us-ascii"    ),
        _T("iso8859-1"   ),
        _T("iso-8859-12" ),
        _T("koi8-r"      ),
        _T("utf-7"       ),
        _T("cp1250"      ),
        _T("windows-1252"),

        // and now some bogus ones
        _T(""            ),
        _T("cp1249"      ),
        _T("iso--8859-1" ),
        _T("iso-8859-19" ),
    };

    static const wxChar *names[] =
    {
        // some vali charsets
        _T("default"     ),
        _T("iso-8859-1"  ),
        _T("iso-8859-12" ),
        _T("koi8-r"      ),
        _T("utf-7"       ),
        _T("windows-1250"),
        _T("windows-1252"),

        // and now some bogus ones
        _T("default"     ),
        _T("unknown--1"  ),
        _T("unknown--1"  ),
        _T("unknown--1"  ),
    };

    static const wxChar *descriptions[] =
    {
        // some vali charsets
        _T("Default encoding"                  ),
        _T("Western European (ISO-8859-1)"     ),
        _T("Indian (ISO-8859-12)"              ),
        _T("KOI8-R"                            ),
        _T("Unicode 7 bit (UTF-7)"             ),
        _T("Windows Central European (CP 1250)"),
        _T("Windows Western European (CP 1252)"),

        // and now some bogus ones
        _T("Default encoding"                  ),
        _T("Unknown encoding (-1)"             ),
        _T("Unknown encoding (-1)"             ),
        _T("Unknown encoding (-1)"             ),
    };

    for ( size_t n = 0; n < WXSIZEOF(charsets); n++ )
    {
        wxFontEncoding enc = wxFontMapper::Get()->CharsetToEncoding(charsets[n]);
        CPPUNIT_ASSERT( wxFontMapper::Get()->GetEncodingName(enc) == names[n] );
        CPPUNIT_ASSERT( wxFontMapper::Get()->GetEncodingDescription(enc) == descriptions[n] );
    }

#endif // wxUSE_FONTMAP
}
