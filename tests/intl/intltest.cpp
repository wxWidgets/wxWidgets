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
    CPPUNIT_ASSERT( _("&Open bogus file") == "&Ouvrir un fichier" );

    // search in our domain only:
    CPPUNIT_ASSERT( wxGetTranslation("&Open bogus file", "internat") ==
                    "&Ouvrir un fichier" );

    // search in a domain that doesn't have this string:
    CPPUNIT_ASSERT( wxGetTranslation("&Open bogus file", "BogusDomain") ==
                    "&Open bogus file" );
}

void IntlTestCase::Headers()
{
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("Project-Id-Version") ==
                    "wxWindows 2.0 i18n sample" );
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("POT-Creation-Date") ==
                    "1999-01-13 18:19+0100" );
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("PO-Revision-Date") ==
                    "YEAR-MO-DA HO:MI+ZONE" );
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("Last-Translator") ==
                    "Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>" );
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("MIME-Version") ==
                    "1.0" );
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("Content-Type") ==
                    "text/plain; charset=iso-8859-1" );
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("Content-Transfer-Encoding") ==
                    "8bit" );

    // check that it fails with a bogus domain:
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("POT-Creation-Date", "Bogus") ==
                    "" );

    // and that it fails for nonexisting header:
    CPPUNIT_ASSERT( m_locale->GetHeaderValue("X-Not-Here") == "" );
}

#endif // wxUSE_INTL
