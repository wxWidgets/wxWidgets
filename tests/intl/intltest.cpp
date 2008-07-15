///////////////////////////////////////////////////////////////////////////////
// Name:        tests/intl/intltest.cpp
// Purpose:     wxLocale unit test
// Author:      Vaclav Slavik
// Created:     2007-03-26
// RCS-ID:      $Id$
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
    IntlTestCase() {}

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( IntlTestCase );
        CPPUNIT_TEST( Domain );
        CPPUNIT_TEST( Headers );
    CPPUNIT_TEST_SUITE_END();

    void Domain();
    void Headers();

    wxLocale *m_locale;

    DECLARE_NO_COPY_CLASS(IntlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( IntlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( IntlTestCase, "IntlTestCase" );

void IntlTestCase::setUp()
{
    wxLocale::AddCatalogLookupPathPrefix("./intl");

    m_locale = new wxLocale;
    // don't load default catalog, it may be unavailable:
    bool loaded = m_locale->Init(wxLANGUAGE_FRENCH, wxLOCALE_CONV_ENCODING);
    CPPUNIT_ASSERT( loaded );

    m_locale->AddCatalog("internat");
}

void IntlTestCase::tearDown()
{
    delete m_locale;
    m_locale = NULL;
}

void IntlTestCase::Domain()
{
    // _() searches all domains by default:
    WX_ASSERT_STR_EQUAL( "&Ouvrir un fichier", _("&Open bogus file") );

    // search in our domain only:
    WX_ASSERT_STR_EQUAL( "&Ouvrir un fichier", wxGetTranslation("&Open bogus file", "internat") );

    // search in a domain that doesn't have this string:
    WX_ASSERT_STR_EQUAL( "&Open bogus file", wxGetTranslation("&Open bogus file", "BogusDomain") );
}

void IntlTestCase::Headers()
{
    WX_ASSERT_STR_EQUAL( "wxWindows 2.0 i18n sample", m_locale->GetHeaderValue("Project-Id-Version") );
    WX_ASSERT_STR_EQUAL( "1999-01-13 18:19+0100", m_locale->GetHeaderValue("POT-Creation-Date") );
    WX_ASSERT_STR_EQUAL( "YEAR-MO-DA HO:MI+ZONE", m_locale->GetHeaderValue("PO-Revision-Date") );
    WX_ASSERT_STR_EQUAL( "Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>", m_locale->GetHeaderValue("Last-Translator") );
    WX_ASSERT_STR_EQUAL( "1.0", m_locale->GetHeaderValue("MIME-Version") );
    WX_ASSERT_STR_EQUAL( "text/plain; charset=iso-8859-1", m_locale->GetHeaderValue("Content-Type") );
    WX_ASSERT_STR_EQUAL( "8bit", m_locale->GetHeaderValue("Content-Transfer-Encoding") );

    // check that it fails with a bogus domain:
    WX_ASSERT_STR_EQUAL( "", m_locale->GetHeaderValue("POT-Creation-Date", "Bogus") );

    // and that it fails for nonexisting header:
    WX_ASSERT_STR_EQUAL( "", m_locale->GetHeaderValue("X-Not-Here") );
}

#endif // wxUSE_INTL
