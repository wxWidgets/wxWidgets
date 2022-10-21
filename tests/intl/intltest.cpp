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


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/intl.h"
#include "wx/uilocale.h"

#include "wx/private/glibc.h"

#if wxUSE_INTL

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class IntlTestCase : public CppUnit::TestCase
{
public:
    IntlTestCase() { m_locale=nullptr; }

    virtual void setUp() override;
    virtual void tearDown() override;

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
    // and all the other function need to check whether m_locale is non-null
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
        m_locale = nullptr;
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
#if wxCHECK_GLIBC_VERSION(2, 8)
    static const char *FRENCH_DATE_FMT = "%d/%m/%Y";
#else
    static const char *FRENCH_DATE_FMT = "%d.%m.%Y";
#endif
    static const char *FRENCH_LONG_DATE_FMT = "%a %d %b %Y";
    static const char *FRENCH_DATE_TIME_FMT = "%a %d %b %Y %H:%M:%S";
#elif defined(__FREEBSD__)
    static const char *FRENCH_DATE_FMT = "%d.%m.%Y";
    static const char *FRENCH_LONG_DATE_FMT = "%a %e %b %Y";
    static const char *FRENCH_DATE_TIME_FMT = "%a %e %b %X %Y";
#else
    static const char *FRENCH_DATE_FMT = "%d/%m/%Y";
    static const char *FRENCH_LONG_DATE_FMT = "%A %d %B %Y";
    static const char *FRENCH_DATE_TIME_FMT = "%d/%m/%Y %H:%M:%S";
#endif

    WX_ASSERT_EQUAL_FORMAT( "French short date", FRENCH_DATE_FMT,
                            wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT) );
    WX_ASSERT_EQUAL_FORMAT( "French long date", FRENCH_LONG_DATE_FMT,
                            wxLocale::GetInfo(wxLOCALE_LONG_DATE_FMT) );

    wxString fmtDT = wxLocale::GetInfo(wxLOCALE_DATE_TIME_FMT);
    INFO("French date and time format is \"" << fmtDT << "\"");

#ifdef __WXOSX__
    // Things are difficult to test under macOS as the format keeps changing,
    // e.g. at some time between 10.10 and 10.12 a new " à " string appeared in
    // its middle, so test it piece-wise and hope it doesn't change too much.
    CHECK( fmtDT.StartsWith("%A %d %B %Y") );
    CHECK( fmtDT.EndsWith("%H:%M:%S") );

    wxUnusedVar(FRENCH_DATE_TIME_FMT);
#else
    // Some glic versions have " %Z" at the end of the locale and some don't.
    // The test is still useful if we just ignore this difference.
    #ifdef __GLIBC__
        wxString fmtDTWithoutTZ;
        if ( fmtDT.EndsWith(" %Z", &fmtDTWithoutTZ) )
            fmtDT.swap(fmtDTWithoutTZ);
    #endif

    WX_ASSERT_EQUAL_FORMAT( "French date and time", FRENCH_DATE_TIME_FMT, fmtDT );
#endif
    WX_ASSERT_EQUAL_FORMAT( "French time", "%H:%M:%S",
                            wxLocale::GetInfo(wxLOCALE_TIME_FMT) );
}

void IntlTestCase::IsAvailable()
{
    const wxString origLocale(setlocale(LC_ALL, nullptr));

    // Calling IsAvailable() shouldn't change the locale.
    wxLocale::IsAvailable(wxLANGUAGE_ENGLISH);

    CPPUNIT_ASSERT_EQUAL( origLocale, setlocale(LC_ALL, nullptr) );
}

// The test may fail in ANSI builds because of unsupported encoding, but we
// don't really care about this build anyhow, so just skip it there.
#if wxUSE_UNICODE

TEST_CASE("wxLocale::Default", "[locale]")
{
    CHECK( wxLocale::IsAvailable(wxLANGUAGE_DEFAULT) );

    wxLocale loc;

    REQUIRE( loc.Init(wxLANGUAGE_DEFAULT, wxLOCALE_DONT_LOAD_DEFAULT) );
}

#endif // wxUSE_UNICODE

// Under MSW and macOS all the locales used below should be supported, but
// under Linux some locales may be unavailable.
static inline bool CheckSupported(const wxUILocale& loc, const char* desc)
{
#if defined(__WINDOWS__) || defined(__WXOSX__)
    INFO(desc << " locale");
    CHECK( loc.IsSupported() );
#else // Unix (not Darwin)
    if ( !loc.IsSupported() )
    {
        WARN(desc << " locale not supported.");
        return false;
    }
#endif

    return true;
}

TEST_CASE("wxUILocale::IsSupported", "[uilocale]")
{
    CheckSupported(wxUILocale::FromTag("en"), "English");
    CheckSupported(wxUILocale(wxLocaleIdent().Language("fr").Region("FR")), "French");
    CHECK_FALSE( wxUILocale::FromTag("bloordyblop").IsSupported() );
    CHECK_FALSE( wxUILocale::FromTag("xyz").IsSupported() );
}

TEST_CASE("wxUILocale::GetInfo", "[uilocale]")
{
    CHECK( wxUILocale::FromTag("en").GetInfo(wxLOCALE_DECIMAL_POINT) == "." );

    const wxUILocale locDE(wxUILocale::FromTag("de"));
    if ( CheckSupported(locDE, "German") )
        CHECK( locDE.GetInfo(wxLOCALE_DECIMAL_POINT) == "," );

    // This one shows that "Swiss High German" locale (de_CH) correctly uses
    // dot, and not comma, as decimal separator, even under macOS, where POSIX
    // APIs use incorrect (identical to "German") definitions for this locale.
    const wxUILocale locDE_CH(wxLocaleIdent().Language("de").Region("CH"));
    if ( CheckSupported(locDE_CH, "Swiss German") )
        CHECK( locDE_CH.GetInfo(wxLOCALE_DECIMAL_POINT) == "." );
}

// Just a small helper to make the test below shorter.
static inline wxString u8(const char* s) { return wxString::FromUTF8(s); }

TEST_CASE("wxUILocale::CompareStrings", "[uilocale]")
{
    SECTION("English")
    {
        const wxUILocale l(wxUILocale::FromTag("en"));

        // This is not very interesting, but check that comparison works at all.
        CHECK( l.CompareStrings("x", "x") ==  0 );
        CHECK( l.CompareStrings("x", "y") == -1 );
        CHECK( l.CompareStrings("z", "y") == +1 );

        // Also check for some degenerate cases.
        CHECK( l.CompareStrings("", "")  ==  0 );
        CHECK( l.CompareStrings("", "a") == -1 );

        // The rest of the tests won't work unless it's really English locale
        // and not the default "C" one.
        if ( !CheckSupported(l, "English") )
            return;

        // And for case handling.
        CHECK( l.CompareStrings("a", "A") == -1 );
        CHECK( l.CompareStrings("b", "A") ==  1 );
        CHECK( l.CompareStrings("B", "a") ==  1 );

        // Case insensitive comparison is not supported with POSIX APIs.
#if defined(__WINDOWS__) || defined(__WXOSX__)
        CHECK( l.CompareStrings("a", "A", wxCompare_CaseInsensitive) == 0 );
#endif
    }

    // UTF-8 strings are not supported in ASCII build.
#if wxUSE_UNICODE
    SECTION("German")
    {
        const wxUILocale l(wxLocaleIdent().Language("de").Region("DE"));

        if ( !CheckSupported(l, "German") )
            return;

        // This is more interesting and shows that CompareStrings() uses German
        // dictionary rules (DIN 5007-1 variant 1).
        CHECK( l.CompareStrings("a",  u8("ä")) == -1 );
        CHECK( l.CompareStrings(u8("ä"), "ae") == -1 );

#if defined(__WINDOWS__) || defined(__WXOSX__)
        // Unfortunately CompareStringsEx() doesn't seem to work correctly
        // under Wine.
        if ( wxIsRunningUnderWine() )
            return;

        CHECK( l.CompareStrings(u8("ß"), "ss", wxCompare_CaseInsensitive) == 0 );
#endif
    }

    SECTION("Swedish")
    {
        if ( wxIsRunningUnderWine() )
            return;

        const wxUILocale l(wxUILocale::FromTag("sv"));

        if ( !CheckSupported(l, "Swedish") )
            return;

        // And this shows that sort order really depends on the language.
        CHECK( l.CompareStrings(u8("ä"), "ae") == 1 );
        CHECK( l.CompareStrings(u8("ö"), "z" ) == 1 );
    }
#endif // wxUSE_UNICODE
}

// Small helper for making the test below more concise.
static void CheckTag(const wxString& tag)
{
    CHECK( wxLocaleIdent::FromTag(tag).GetTag() == tag );
}

TEST_CASE("wxLocaleIdent::FromTag", "[uilocale][localeident]")
{
    CheckTag("");
    CheckTag("en");
    CheckTag("en_US");
    CheckTag("en_US.utf8");
    CheckTag("English");
    CheckTag("English_United States");
    CheckTag("English_United States.utf8");
}

// Yet another helper for the test below.
static void CheckFindLanguage(const wxString& tag, const char* expected)
{
    const wxLanguageInfo* const
        info = wxUILocale::FindLanguageInfo(wxLocaleIdent::FromTag(tag));
    if ( !expected )
    {
        if ( info )
            FAIL_CHECK("Found " << info->CanonicalName << " for " << tag);

        return;
    }
    else
    {
        if ( !info )
        {
            FAIL_CHECK("Not found for " << tag);
            return;
        }
    }

    CHECK( info->CanonicalName == expected );
}

TEST_CASE("wxUILocale::FindLanguageInfo", "[uilocale]")
{
    CheckFindLanguage("", nullptr);
    CheckFindLanguage("en", "en");
    CheckFindLanguage("en_US", "en_US");
    CheckFindLanguage("en_US.utf8", "en_US");
    CheckFindLanguage("English", "en");
    CheckFindLanguage("English_United States", "en_US");
    CheckFindLanguage("English_United States.utf8", "en_US");
    // Test tag that includes an explicit script
    CheckFindLanguage("sr-Latn-RS", "sr_RS@latin");
}

// Test which can be used to check if the given locale tag is supported.
TEST_CASE("wxUILocale::FromTag", "[.]")
{
    wxString tag;
    if ( !wxGetEnv("WX_TEST_LOCALE_TAG", &tag) )
    {
        FAIL("Specify WX_TEST_LOCALE_TAG");
    }

    const wxLocaleIdent locId = wxLocaleIdent::FromTag(tag);
    REQUIRE( !locId.IsEmpty() );

    const wxUILocale loc(locId);
    WARN("Locale \"" << tag << "\" supported: " << loc.IsSupported() );
}

#endif // wxUSE_INTL
