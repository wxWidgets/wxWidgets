///////////////////////////////////////////////////////////////////////////////
// Name:        tests/intl/intltest.cpp
// Purpose:     wxLocale unit test
// Author:      Vaclav Slavik
// Created:     2007-03-26
// Copyright:   (c) 2007 Vaclav Slavik
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

#include "wx/intl.h"

#if wxUSE_INTL

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class IntlTestCase : public CppUnit::TestCase
{
public:
    IntlTestCase() { m_locale=NULL; }

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( IntlTestCase );
        CPPUNIT_TEST( RestoreLocale );
        CPPUNIT_TEST( Domain );
        CPPUNIT_TEST( Headers );
        CPPUNIT_TEST( DateTimeFmtFrench );
        CPPUNIT_TEST( IsAvailable );
    CPPUNIT_TEST_SUITE_END();

    void RestoreLocale();
    void Domain();
    void Headers();
    void DateTimeFmtFrench();
    void IsAvailable();

    static wxString GetDecimalPoint()
    {
        return wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
    }

    wxLocale *m_locale;

    wxDECLARE_NO_COPY_CLASS(IntlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( IntlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( IntlTestCase, "IntlTestCase" );

void IntlTestCase::setUp()
{
    // Check that French locale is supported, this test doesn't work without it
    // and all the other function need to check whether m_locale is non-NULL
    // before continuing
    if ( !wxLocale::IsAvailable(wxLANGUAGE_FRENCH) )
        return;

    wxLocale::AddCatalogLookupPathPrefix("./intl");

    m_locale = new wxLocale;
    CPPUNIT_ASSERT( m_locale );

    // don't load default catalog, it may be unavailable:
    bool loaded = m_locale->Init(wxLANGUAGE_FRENCH, wxLOCALE_DONT_LOAD_DEFAULT);
    CPPUNIT_ASSERT( loaded );

    m_locale->AddCatalog("internat");
}

void IntlTestCase::tearDown()
{
    if (m_locale)
    {
        delete m_locale;
        m_locale = NULL;
    }
}

void IntlTestCase::RestoreLocale()
{
    if ( !m_locale )
        return;

    // We must be using the French locale now, it was changed in setUp().
    CPPUNIT_ASSERT_EQUAL( ",", GetDecimalPoint() );

    // Switch to the English locale.
    {
        wxLocale locEn(wxLANGUAGE_ENGLISH);
        CPPUNIT_ASSERT_EQUAL( ".", GetDecimalPoint() );
    }

    // Verify that after destroying the English locale object, French locale is
    // restored.
    CPPUNIT_ASSERT_EQUAL( ",", GetDecimalPoint() );
}

void IntlTestCase::Domain()
{
    if (!m_locale)
        return;

    // _() searches all domains by default:
    CPPUNIT_ASSERT_EQUAL( "&Ouvrir un fichier", _("&Open bogus file") );

    // search in our domain only:
    CPPUNIT_ASSERT_EQUAL( "&Ouvrir un fichier", wxGetTranslation("&Open bogus file", "internat") );

    // search in a domain that doesn't have this string:
    CPPUNIT_ASSERT_EQUAL( "&Open bogus file", wxGetTranslation("&Open bogus file", "BogusDomain") );
}

void IntlTestCase::Headers()
{
    if ( !m_locale )
        return;

    CPPUNIT_ASSERT_EQUAL( "wxWindows 2.0 i18n sample", m_locale->GetHeaderValue("Project-Id-Version") );
    CPPUNIT_ASSERT_EQUAL( "1999-01-13 18:19+0100", m_locale->GetHeaderValue("POT-Creation-Date") );
    CPPUNIT_ASSERT_EQUAL( "YEAR-MO-DA HO:MI+ZONE", m_locale->GetHeaderValue("PO-Revision-Date") );
    CPPUNIT_ASSERT_EQUAL( "Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>", m_locale->GetHeaderValue("Last-Translator") );
    CPPUNIT_ASSERT_EQUAL( "1.0", m_locale->GetHeaderValue("MIME-Version") );
    CPPUNIT_ASSERT_EQUAL( "text/plain; charset=utf-8", m_locale->GetHeaderValue("Content-Type") );
    CPPUNIT_ASSERT_EQUAL( "8bit", m_locale->GetHeaderValue("Content-Transfer-Encoding") );

    // check that it fails with a bogus domain:
    CPPUNIT_ASSERT_EQUAL( "", m_locale->GetHeaderValue("POT-Creation-Date", "Bogus") );

    // and that it fails for nonexisting header:
    CPPUNIT_ASSERT_EQUAL( "", m_locale->GetHeaderValue("X-Not-Here") );
}

static wxString
NormalizeFormat(const wxString& fmtOrig)
{
    wxString fmt(fmtOrig);

#ifdef __GLIBC__
    // glibc uses some extensions in its formats which we need to convert to
    // standard form
    fmt.Replace("%T", "%H:%M:%S");
    fmt.Replace("%e", "%d");
#endif // __GLIBC__

    return fmt;
}

#define WX_ASSERT_EQUAL_FORMAT(msg, expected, actual) \
    if ( !actual.empty() ) \
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, expected, NormalizeFormat(actual))

void IntlTestCase::DateTimeFmtFrench()
{
    if ( !m_locale )
        return;

#ifdef __GLIBC__
    // Versions of glibc up to 2.7 wrongly used periods for French locale
    // separator.
#if __GLIBC__ > 2 || __GLIBC_MINOR__ >= 8
    static const char *FRENCH_DATE_FMT = "%d/%m/%Y";
#else
    static const char *FRENCH_DATE_FMT = "%d.%m.%Y";
#endif
    static const char *FRENCH_LONG_DATE_FMT = "%a %d %b %Y";
    static const char *FRENCH_DATE_TIME_FMT = "%a %d %b %Y %H:%M:%S %Z";
#else
    static const char *FRENCH_DATE_FMT = "%d/%m/%Y";
    static const char *FRENCH_LONG_DATE_FMT = "%A %d %B %Y";
    static const char *FRENCH_DATE_TIME_FMT = "%d/%m/%Y %H:%M:%S";
#endif

    WX_ASSERT_EQUAL_FORMAT( "French short date", FRENCH_DATE_FMT,
                            wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT) );
    WX_ASSERT_EQUAL_FORMAT( "French long date", FRENCH_LONG_DATE_FMT,
                            wxLocale::GetInfo(wxLOCALE_LONG_DATE_FMT) );

    const wxString fmtDT = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
#ifdef __WXOSX__
    // Things are difficult to test under macOS as the format keeps changing,
    // e.g. at some time between 10.10 and 10.12 a new " � " string appeared in
    // its middle, so test it piece-wise and hope it doesn't change too much.
    INFO("French date and time format is \"" << fmtDT << "\"");
    CHECK( fmtDT.StartsWith("%A %d %B %Y") );
    CHECK( fmtDT.EndsWith("%H:%M:%S") );
#else
    WX_ASSERT_EQUAL_FORMAT( "French date and time", FRENCH_DATE_TIME_FMT, fmtDT );
#endif
    WX_ASSERT_EQUAL_FORMAT( "French time", "%H:%M:%S",
                            wxLocale::GetInfo(wxLOCALE_TIME_FMT) );
}

void IntlTestCase::IsAvailable()
{
    const wxString origLocale(setlocale(LC_ALL, NULL));

    // Calling IsAvailable() shouldn't change the locale.
    wxLocale::IsAvailable(wxLANGUAGE_ENGLISH);

    CPPUNIT_ASSERT_EQUAL( origLocale, setlocale(LC_ALL, NULL) );
}

#endif // wxUSE_INTL
