///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fontmap/fontmap.cpp
// Purpose:     wxFontMapper unit test
// Author:      Vadim Zeitlin
// Created:     14.02.04
// RCS-ID:      $Id$
// Copyright:   (c) 2003 TT-Solutions
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#if wxUSE_FONTMAP

#include "wx/fontmap.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class FontMapperTestCase : public CppUnit::TestCase
{
public:
    FontMapperTestCase() { }

private:
    CPPUNIT_TEST_SUITE( FontMapperTestCase );
        CPPUNIT_TEST( NamesAndDesc );
    CPPUNIT_TEST_SUITE_END();

    void NamesAndDesc();

    DECLARE_NO_COPY_CLASS(FontMapperTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FontMapperTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FontMapperTestCase, "FontMapperTestCase" );


void FontMapperTestCase::NamesAndDesc()
{
    static const wxChar *charsets[] =
    {
        // some valid charsets
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
        // some valid charsets
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
        // some valid charsets
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
        wxFontEncoding enc = wxFontMapperBase::Get()->CharsetToEncoding(charsets[n]);
        CPPUNIT_ASSERT( wxFontMapperBase::Get()->GetEncodingName(enc).CmpNoCase(names[n]) == 0 );
        CPPUNIT_ASSERT( wxFontMapperBase::Get()->GetEncodingDescription(enc) == descriptions[n] );
    }
}

#endif // wxUSE_FONTMAP
