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
#include "wx/scopeguard.h"

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
#elif defined(__CYGWIN__)
    static const char *FRENCH_DATE_FMT = "%d/%m/%Y";
    static const char *FRENCH_LONG_DATE_FMT = "%a %e %b %Y";
    static const char *FRENCH_DATE_TIME_FMT = "%a %e %b %Y %H:%M:%S";
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

TEST_CASE("wxTranslations::AddCatalog", "[translations]")
{
    // We currently have translations for British English, French and Japanese
    // in this test directory, check that loading those succeeds but loading
    // others doesn't.
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix("./intl");

    const wxString domain("internat");

    wxTranslations trans;

    SECTION("All")
    {
        auto available = trans.GetAvailableTranslations(domain);
        REQUIRE( available.size() == 4 );

        available.Sort();
        CHECK( available[0] == "en_GB" );
        CHECK( available[1] == "fr" );
        CHECK( available[2] == "ja" );
        CHECK( available[3] == "xart-dothraki" );
    }

    SECTION("French")
    {
        trans.SetLanguage(wxLANGUAGE_FRENCH);
        CHECK( trans.AddAvailableCatalog(domain) );
    }

    SECTION("Italian")
    {
        trans.SetLanguage(wxLANGUAGE_ITALIAN);
        CHECK_FALSE( trans.AddAvailableCatalog(domain) );
    }

    // And loading catalog using the same language as message IDs should
    // "succeed" too, even if there is no such file, as in this case the
    // message IDs themselves can be used directly.
    SECTION("Untranslated")
    {
        trans.SetLanguage(wxLANGUAGE_GERMAN);
        CHECK( trans.AddCatalog(domain, wxLANGUAGE_GERMAN) );

        // Using a different region should still work.
        CHECK( trans.AddCatalog(domain, wxLANGUAGE_GERMAN_SWISS) );

        // But using a completely different language should not.
        CHECK_FALSE( trans.AddCatalog(domain, wxLANGUAGE_DUTCH) );
    }
}

TEST_CASE("wxTranslations::GetBestTranslation", "[translations]")
{
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix("./intl");

    const wxString domain("internat");

    wxTranslations trans;
    wxON_BLOCK_EXIT1( wxUnsetEnv, "WXLANGUAGE" );

    SECTION("ChooseLanguage")
    {
        // Simple case.
        wxSetEnv("WXLANGUAGE", "fr:en");
        CHECK( trans.GetBestTranslation(domain) == "fr" );
        CHECK( trans.GetBestAvailableTranslation(domain) == "fr" );

        // Choose 2nd language _and_ its base form.
        wxSetEnv("WXLANGUAGE", "cs:fr_CA:en");
        CHECK( trans.GetBestTranslation(domain) == "fr" );
        CHECK( trans.GetBestAvailableTranslation(domain) == "fr" );
    }

    SECTION("EnglishHandling")
    {
        // Check that existing en_GB file isn't used for msgid language.
        wxSetEnv("WXLANGUAGE", "en_US");

        CHECK( trans.GetBestTranslation(domain) == "en" );
        // GetBestAvailableTranslation() will wrongly return "en_GB", don't test that.

        wxSetEnv("WXLANGUAGE", "es:en");
        CHECK( trans.GetBestTranslation(domain) == "en" );
        // GetBestAvailableTranslation() will wrongly return "en_GB", don't test that.

        // And that it is used when it should be
        wxSetEnv("WXLANGUAGE", "en_GB");
        CHECK( trans.GetBestTranslation(domain) == "en_GB" );
        CHECK( trans.GetBestAvailableTranslation(domain) == "en_GB" );

    }

    SECTION("DontSkipMsgidLanguage")
    {
        // Check that msgid language will be used if it's the best match.
        wxSetEnv("WXLANGUAGE", "cs:en:fr");
        CHECK( trans.GetBestTranslation(domain) == "en" );

        // ...But won't be used if there's a suitable translation file.
        wxSetEnv("WXLANGUAGE", "fr:en:cs");
        CHECK( trans.GetBestTranslation(domain) == "fr" );
        CHECK( trans.GetBestAvailableTranslation(domain) == "fr" );
    }

    SECTION("PassthroughUnknown")
    {
        // Check that even a language not known to wx (in this case, made up)
        // will be correctly served if it is known to the OS and has available
        // translation.
        //
        // Notice that this would normally be x-art-dothraki, but we
        // intentionally use an incorrect code to be future-proof and
        // explicitly test for unrecognized passthrough even if wx starts fully
        // understanding language tags.
        wxSetEnv("WXLANGUAGE", "xart-dothraki:en:fr");
        CHECK( trans.GetBestTranslation(domain) == "xart-dothraki" );
    }
}

// This test can be used to check how GetBestTranslation() and
// GetAvailableTranslations() work with the given preferred languages: set
// WXLANGUAGE environment variable to the colon-separated list of preferred
// languages to test before running it.
TEST_CASE("wxTranslations::ShowAvailable", "[.]")
{
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix("./intl");

    const wxString domain("internat");

    wxTranslations trans;

    WARN
    (
        "Available: [" << wxJoin(trans.GetAvailableTranslations(domain), ',') << "]\n"
        "Best:      [" << trans.GetBestTranslation(domain) << "]"
    );
}

TEST_CASE("wxLocale::Default", "[locale]")
{
    const int langDef = wxUILocale::GetSystemLanguage();
    INFO("System language: " << wxUILocale::GetLanguageName(langDef));
    CHECK( wxLocale::IsAvailable(langDef) );

    wxLocale loc;

    REQUIRE( loc.Init(langDef, wxLOCALE_DONT_LOAD_DEFAULT) );
}

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
        // CompareStringsEx() was only implemented correctly in Wine 7.10.
        wxVersionInfo wineVer;
        if ( wxIsRunningUnderWine(&wineVer) && !wineVer.AtLeast(7, 10) )
            return;

        CHECK( l.CompareStrings(u8("ß"), "ss", wxCompare_CaseInsensitive) == 0 );
#endif
    }

    SECTION("Swedish")
    {
        wxVersionInfo wineVer;
        if ( wxIsRunningUnderWine(&wineVer) && !wineVer.AtLeast(7, 10) )
            return;

        const wxUILocale l(wxUILocale::FromTag("sv"));

        if ( !CheckSupported(l, "Swedish") )
            return;

        // And this shows that sort order really depends on the language.
        CHECK( l.CompareStrings(u8("ä"), "ae") == 1 );
        CHECK( l.CompareStrings(u8("ö"), "z" ) == 1 );
    }
}

// Small helper for making the test below more concise.
static void CheckTag(const wxString& tag)
{
    CHECK( wxLocaleIdent::FromTag(tag).GetTag() == tag );
}

static wxString TagToPOSIX(const char* tag)
{
    return wxLocaleIdent::FromTag(tag).GetTag(wxLOCALE_TAGTYPE_POSIX);
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

    CHECK( TagToPOSIX("zh-Hans-CN") == "zh_CN" );
    CHECK( TagToPOSIX("zh-Hant-TW") == "zh_TW");
    CHECK( TagToPOSIX("sr-Latn-RS") == "sr_RS@latin");
    CHECK( TagToPOSIX("sr-Cyrl-RS") == "sr_RS");
}

static wxString FindBestMatch(const wxVector<wxString>& desired, const wxVector<wxString>& supported)
{
    return wxLocaleIdent::GetBestMatch(desired, supported);
}

TEST_CASE("wxLocaleIdent::GetBestMatch", "[uilocale][localeident]")
{
    CHECK(FindBestMatch({ "en-AU" }, { "en-US", "en-NZ" }) == "en-NZ");
    CHECK(FindBestMatch({ "pt-AO" }, { "pt-BR", "pt-PT" }) == "pt-PT");
    CHECK(FindBestMatch({ "fr" }, { "fr-FR", "fr", "fr-CA", "en"}) == "fr");
    CHECK(FindBestMatch({ "fr-FR" }, { "en", "fr", "fr-CA" }) == "fr");
    CHECK(FindBestMatch({ "fr-FR" }, { "en", "fr-CA" }) == "fr-CA");
    CHECK(FindBestMatch({ "fr" }, { "fr-CA", "fr-FR"}) == "fr-FR");
    CHECK(FindBestMatch({ "fr-SN" }, { "fr-CA", "fr-FR" }) == "fr-FR");
    CHECK(FindBestMatch({ "fr" }, { "de", "en", "it"}) == "");

    // favor a more-default locale among equally imperfect matches
    CHECK(FindBestMatch({ "fr-SN" }, { "fr-CA", "fr-CH", "fr-FR", "fr-GB"}) == "fr-FR");

    CHECK(FindBestMatch({ "zh-TW" }, { "zh" }) == "");
    CHECK(FindBestMatch({ "zh-HK" }, { "zh-Hant", "zh-TW" }) == "zh-TW");

    // same language over exact, but distinguish when user is explicit
    CHECK(FindBestMatch({ "ja", "de"}, {"fr", "en-GB", "ja", "es-ES", "ex-MX"}) == "ja");
    CHECK(FindBestMatch({ "de-CH", "fr" }, { "en", "de", "fr", "ja" }) == "de");
    CHECK(FindBestMatch({ "en", "nl" }, { "en-GB", "nl" }) == "en-GB");
    CHECK(FindBestMatch({ "en", "nl", "en-GB"}, {"en-GB", "nl"}) == "en-GB");

    // pick best maximized match
    CHECK(FindBestMatch({ "ja-Jpan-JP", "ru" }, { "ja", "ja-Jpan", "ja-JP", "en", "ru"}) == "ja-Jpan");
    CHECK(FindBestMatch({ "ja-Jpan", "ru" }, { "ja", "ja-Jpan", "ja-JP", "en", "ru" }) == "ja-Jpan");

    // match on maximized tag
    CHECK(FindBestMatch({ "ja-JP", "en-GB" }, { "fr", "en-GB", "ja", "es-ES", "es-MX" }) == "ja");
    CHECK(FindBestMatch({ "ja-Jpan-JP", "en-GB" }, { "fr", "en-GB", "ja", "es-ES", "es-MX" }) == "ja");

    // region distance German
    CHECK(FindBestMatch({ "de" }, { "de-AT", "de-DE", "de-CH" }) == "de-DE");

    // en-AU is closer to en-GB than to en (which is en-US)
    CHECK(FindBestMatch({ "en-AU" }, { "en", "en-GB", "es-ES" }) == "en-GB");

    // if no preferred locale specified, pick top language, not regional
    CHECK(FindBestMatch({ "fr-US" }, { "en", "fr", "fr-CA", "fr-CH"}) == "fr");

    // return most originally similar among likely-subtags equivalent locales
    CHECK(FindBestMatch({ "af" }, { "af", "af-Latn", "af-Arab" }) == "af");
    CHECK(FindBestMatch({ "af-ZA" }, { "af", "af-Latn", "af-Arab" }) == "af");
    CHECK(FindBestMatch({ "af-Latn-ZA" }, { "af", "af-Latn", "af-Arab" }) == "af-Latn");
    CHECK(FindBestMatch({ "af-Latn" }, { "af", "af-Latn", "af-Arab" }) == "af-Latn");

    CHECK(FindBestMatch({ "nl" }, { "nl", "nl-NL", "nl-BE" }) == "nl");
    CHECK(FindBestMatch({ "nl-Latn" }, { "nl", "nl-NL", "nl-BE" }) == "nl");
    CHECK(FindBestMatch({ "nl-Latn-NL" }, { "nl", "nl-NL", "nl-BE" }) == "nl-NL");
    CHECK(FindBestMatch({ "nl-NL" }, { "nl", "nl-NL", "nl-BE" }) == "nl-NL");

    CHECK(FindBestMatch({ "nl" }, { "nl", "nl-Latn", "nl-NL", "nl-BE" }) == "nl");
    CHECK(FindBestMatch({ "nl-Latn" }, { "nl", "nl-Latn", "nl-NL", "nl-BE" }) == "nl-Latn");
    CHECK(FindBestMatch({ "nl-NL" }, { "nl", "nl-Latn", "nl-NL", "nl-BE" }) == "nl-NL");
    CHECK(FindBestMatch({ "nl-Latn-NL" }, { "nl", "nl-Latn", "nl-NL", "nl-BE" }) == "nl-Latn");

    // nearby languages: Danish matches no
    CHECK(FindBestMatch({ "da" }, { "en", "no" }) == "no");
    // nearby languages: Nynorsk to Bokmål
    CHECK(FindBestMatch({ "nn" }, { "en", "nb" }) == "nb");
    // nearby languages: Danish does not match nn
    CHECK(FindBestMatch({ "da" }, { "en", "nn" }) == "");

    // script fallbacks
    CHECK(FindBestMatch({ "zh-Hant" }, { "zh-CN", "zh-TW" }) == "zh-TW");
    CHECK(FindBestMatch({ "zh" }, { "zh-CN", "zh-TW" }) == "zh-CN");
    CHECK(FindBestMatch({ "zh-Hans" }, { "zh-CN", "zh-TW" }) == "zh-CN");
    CHECK(FindBestMatch({ "zh-Hans-CN" }, { "zh-CN", "zh-TW" }) == "zh-CN");
    CHECK(FindBestMatch({ "zh-Hant-HK" }, { "zh-CN", "zh-TW" }) == "zh-TW");
    CHECK(FindBestMatch({ "zh-Hans-DE" }, { "zh-CN", "zh-TW" }) == "zh-CN");
    CHECK(FindBestMatch({ "zh-Hant-DE" }, { "zh-CN", "zh-TW" }) == "zh-TW");

    // language-specific script fallbacks
    CHECK(FindBestMatch({ "sr" }, { "en", "sr-Latn" }) == "sr-Latn");
    CHECK(FindBestMatch({ "sr" }, { "en", "sr-Cyrl" }) == "sr-Cyrl");
    CHECK(FindBestMatch({ "sr" }, { "en", "sr-Latn", "sr-Cyrl"}) == "sr-Cyrl");
    CHECK(FindBestMatch({ "sr-Latn" }, { "en", "sr-Latn", "sr-Cyrl" }) == "sr-Latn");
    CHECK(FindBestMatch({ "sr-Cyrl" }, { "en", "sr-Latn" }) == "sr-Latn");
    CHECK(FindBestMatch({ "sr-Latn" }, { "en", "sr-Cyrl" }) == "sr-Cyrl");

    CHECK(FindBestMatch({ "de", "en-US"}, {"fr", "en-GB", "ja", "es-ES", "es-MX"}) == "en-GB");
    CHECK(FindBestMatch({ "de", "zh" }, { "fr", "en-GB", "ja", "es-ES", "es-MX" }) == "");

    // match on maximized
    CHECK(FindBestMatch({ "ja-JP", "en-GB" }, { "fr", "en-GB", "ja", "es-ES", "es-MX" }) == "ja");
    CHECK(FindBestMatch({ "ja-Jpan-JP", "en-GB" }, { "fr", "en-GB", "ja", "es-ES", "es-MX" }) == "ja");
    CHECK(FindBestMatch({ "zh", "en" }, { "fr", "zh-Hant", "en" }) == "en");

    // close enough match on maximized
    CHECK(FindBestMatch({ "de-CH", "fr" }, { "en-GB", "en", "de", "fr", "ja" }) == "de");
    CHECK(FindBestMatch({ "en-US", "ar", "nl", "de", "ja" }, { "en-GB", "en", "de", "fr", "ja"}) == "en");

    // best matches for Portuguese
    CHECK(FindBestMatch({ "pt-PT", "es", "pt" }, { "pt-PT", "pt-BR", "es", "es-AR"}) == "pt-PT");
    CHECK(FindBestMatch({ "pt-PT", "es", "pt" }, { "pt-PT", "pt", "es", "es-AR" }) == "pt-PT");
    CHECK(FindBestMatch({ "pt-PT", "es", "pt" }, { "pt-BR", "es", "es-AR" }) == "pt-BR");

    CHECK(FindBestMatch({ "pt", "es", "pt-PT" }, { "pt-PT", "pt-BR", "es", "es-AR" }) == "pt-BR");
    CHECK(FindBestMatch({ "pt", "es", "pt-PT" }, { "pt-PT", "pt", "es", "es-AR" }) == "pt");
    CHECK(FindBestMatch({ "pt", "es", "pt-PT" }, { "pt-BR", "es", "es-AR" }) == "pt-BR");
    CHECK(FindBestMatch({ "pt-US", "pt-PT" }, { "pt-PT", "pt-BR", "es", "es-AR" }) == "pt-BR");
    CHECK(FindBestMatch({ "pt-US", "pt-PT" }, { "pt-PT", "pt", "es", "es-AR" }) == "pt");

    // regional specials
    CHECK(FindBestMatch({ "en-AU" }, { "en", "en-GB", "es-ES", "es-AR" }) == "en-GB");
    CHECK(FindBestMatch({ "es-MX" }, { "en", "en-GB", "es-ES", "es-AR" }) == "es-AR");
    CHECK(FindBestMatch({ "es-PT" }, { "en", "en-GB", "es-ES", "es-AR" }) == "es-ES");

    // best match for traditional chinese
    CHECK(FindBestMatch({ "zh-TW" }, { "fr", "zh-Hans", "zh-Hans-CN", "en-US" }) == "");
    CHECK(FindBestMatch({ "zh-Hant" }, { "fr", "zh-Hans", "zh-Hans-CN", "en-US" }) == "");

    CHECK(FindBestMatch({ "zh-TW", "en" }, {"fr", "zh-Hans", "zh-Hans-CN", "en-US"}) == "en-US");
    CHECK(FindBestMatch({ "zh-Hant-CN", "en" }, {"fr", "zh-Hans", "zh-Hans-CN", "en-US"}) == "en-US");
    CHECK(FindBestMatch({ "zh-Hans", "en" }, {"fr", "zh-Hans", "zh-Hans-CN", "en-US"}) == "zh-Hans");

    CHECK(FindBestMatch({ "zh-TW", "en" }, { "fr", "zh-Hans", "zh-Hans-CN", "zh-Hant", "en-US" }) == "zh-Hant");
    CHECK(FindBestMatch({ "zh-Hant-CN", "en" }, { "fr", "zh-Hans", "zh-Hans-CN", "zh-Hant", "en-US" }) == "zh-Hant");
    CHECK(FindBestMatch({ "zh-Hans", "en" }, { "fr", "zh-Hans", "zh-Hans-CN", "zh-Hant", "en-US" }) == "zh-Hans");
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

    // Test mixed locales: we should still detect the language correctly, even
    // if we don't recognize the full locale.
    CheckFindLanguage("en_FR", "en");
    CheckFindLanguage("fr_DE", "fr");
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
    WARN("Locale \"" << tag << "\":\n"
         "language:\t" << locId.GetLanguage() << "\n"
         "region:\t" << locId.GetRegion() << "\n"
         "script:\t" << locId.GetScript() << "\n"
         "charset:\t" << locId.GetCharset() << "\n"
         "modifier:\t" << locId.GetModifier() << "\n"
         "extension:\t" << locId.GetExtension() << "\n"
         "sort order:\t" << locId.GetSortorder() << "\n"
         "supported:\t" << (loc.IsSupported() ? "yes" : "no"));
}

namespace
{

const wxString GetLangName(int lang)
{
    switch ( lang )
    {
        case wxLANGUAGE_DEFAULT:
            return "DEFAULT";

        case wxLANGUAGE_UNKNOWN:
            return "UNKNOWN";

        default:
            return wxUILocale::GetLanguageName(lang);
    }
}

wxString GetLocaleDesc(const char* when)
{
    const wxUILocale& curloc = wxUILocale::GetCurrent();
    const wxLocaleIdent locid = curloc.GetLocaleId();

    // Make the output slightly more readable.
    wxString decsep = curloc.GetInfo(wxLOCALE_DECIMAL_POINT);
    if ( decsep == "." )
        decsep = "point";
    else if ( decsep == "," )
        decsep = "comma";
    else
        decsep = wxString::Format("UNKNOWN (%s)", decsep);

    return wxString::Format("%s\ncurrent locale:\t%s "
                            "(decimal separator: %s, date format=%s)",
                            when,
                            locid.IsEmpty() ? wxString("NONE") : locid.GetTag(),
                            decsep,
                            curloc.GetInfo(wxLOCALE_SHORT_DATE_FMT));
}

} // anonymous namespace

// Test to show information about the system locale and the effects of various
// ways to change the current locale.
TEST_CASE("wxUILocale::ShowSystem", "[.]")
{
    WARN("System locale identifier:\t"
            << wxUILocale::GetSystemLocaleId().GetTag() << "\n"
         "System locale as language:\t"
            << GetLangName(wxUILocale::GetSystemLocale()) << "\n"
         "System language identifier:\t"
            << GetLangName(wxUILocale::GetSystemLanguage()));

    WARN(GetLocaleDesc("Before calling any locale functions"));

    wxLocale locDef;
    CHECK( locDef.Init(wxLANGUAGE_DEFAULT, wxLOCALE_DONT_LOAD_DEFAULT) );
    WARN(GetLocaleDesc("After wxLocale::Init(wxLANGUAGE_DEFAULT)"));

    CHECK( wxUILocale::UseDefault() );
    WARN(GetLocaleDesc("After wxUILocale::UseDefault()"));

    wxString preferredLangsStr;
    const auto preferredLangs = wxUILocale::GetPreferredUILanguages();
    for (const auto& lang: preferredLangs)
    {
        if ( !preferredLangsStr.empty() )
            preferredLangsStr += ", ";
        preferredLangsStr += lang;
    }
    WARN("Preferred UI languages:\n" << preferredLangsStr);
}

#endif // wxUSE_INTL
