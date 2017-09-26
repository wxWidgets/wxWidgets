///////////////////////////////////////////////////////////////////////////////
// Name:        tests/config/regconf.cpp
// Purpose:     wxRegConfig unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-02
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CONFIG && wxUSE_REGKEY

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/msw/regconf.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class RegConfigTestCase : public CppUnit::TestCase
{
public:
    RegConfigTestCase() { }

private:
    CPPUNIT_TEST_SUITE( RegConfigTestCase );
        CPPUNIT_TEST( ReadWrite );
        CPPUNIT_TEST( DeleteRegistryKeyFromRedirectedView );
    CPPUNIT_TEST_SUITE_END();

    void ReadWrite();
    void DeleteRegistryKeyFromRedirectedView();

    wxDECLARE_NO_COPY_CLASS(RegConfigTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RegConfigTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RegConfigTestCase, "RegConfigTestCase" );

void RegConfigTestCase::ReadWrite()
{
    wxString app = wxT("wxRegConfigTestCase");
    wxString vendor = wxT("wxWidgets");

    // NOTE: we use wxCONFIG_USE_LOCAL_FILE explicitly to test wxRegConfig
    //       with something different from the default value wxCONFIG_USE_GLOBAL_FILE
    wxConfigBase *config = new wxRegConfig(app, vendor, wxT(""), wxT(""),
                                           wxCONFIG_USE_LOCAL_FILE);

    // test writing
    config->SetPath(wxT("/group1"));
    CPPUNIT_ASSERT( config->Write(wxT("entry1"), wxT("foo")) );
    config->SetPath(wxT("/group2"));
    CPPUNIT_ASSERT( config->Write(wxT("entry1"), wxT("bar")) );

    // test reading
    wxString str;
    long dummy;

    config->SetPath(wxT("/"));
    CPPUNIT_ASSERT( config->GetFirstGroup(str, dummy) );
    CPPUNIT_ASSERT( str == "group1" );
    CPPUNIT_ASSERT( config->Read(wxT("group1/entry1"), wxT("INVALID DEFAULT")) == "foo" );
    CPPUNIT_ASSERT( config->GetNextGroup(str, dummy) );
    CPPUNIT_ASSERT( str == "group2" );
    CPPUNIT_ASSERT( config->Read(wxT("group2/entry1"), wxT("INVALID DEFAULT")) == "bar" );

    config->DeleteAll();
    delete config;
}

void RegConfigTestCase::DeleteRegistryKeyFromRedirectedView()
{
    if ( !wxIsPlatform64Bit() )
    {
        // Test needs WoW64.
        return;
    }

    // Test inside a key that's known to be redirected and is in HKCU so that
    // admin rights are not required (unlike with HKLM).
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Classes\\CLSID\\wxWidgetsTestKey",
        sizeof(void *) == 4
            ? wxRegKey::WOW64ViewMode_64
            : wxRegKey::WOW64ViewMode_32);

    CPPUNIT_ASSERT( key.Create() );
    CPPUNIT_ASSERT( key.DeleteSelf() );
    CPPUNIT_ASSERT( !key.Exists() );
}

#endif // wxUSE_CONFIG && wxUSE_REGKEY

