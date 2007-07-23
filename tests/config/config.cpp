///////////////////////////////////////////////////////////////////////////////
// Name:        tests/config/config.cpp
// Purpose:     wxConfig unit test
// Author:      Marcin Wojdyr
// Created:     2007-07-07
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Marcin Wojdyr
///////////////////////////////////////////////////////////////////////////////

// see also tests/fileconf/fileconftest.cpp for wxFileConfig specific tests

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

#include "wx/config.h"
#include "wx/colour.h"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class ConfigTestCase : public CppUnit::TestCase
{
public:
    ConfigTestCase() {}

private:
    CPPUNIT_TEST_SUITE( ConfigTestCase );
        CPPUNIT_TEST( ReadWriteLocalTest );
        CPPUNIT_TEST( RecordingDefaultsTest );
    CPPUNIT_TEST_SUITE_END();

    void ReadWriteLocalTest();
    void RecordingDefaultsTest();

    DECLARE_NO_COPY_CLASS(ConfigTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ConfigTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ConfigTestCase, "ConfigTestCase" );

void ConfigTestCase::ReadWriteLocalTest()
{
    wxString app = wxT("wxConfigTestCase");
    wxString vendor = wxT("wxWidgets");
    wxConfig *config = new wxConfig(app, vendor, wxT(""), wxT(""),
                                    wxCONFIG_USE_LOCAL_FILE);
    config->DeleteAll();
    config->Write(wxT("string1"), wxT("abc"));
    config->Write(wxT("string2"), wxString(wxT("def")));
    config->Write(wxT("int1"), 123);
    config->Write(wxString(wxT("long1")), 234L);
    config->Write(wxT("double1"), 345.67);
    config->Write(wxT("bool1"), true);
    config->Write(wxT("color1"), wxColour(11,22,33,44));
    config->Flush();
    delete config;

    config = new wxConfig(app, vendor, wxT(""), wxT(""),
                                    wxCONFIG_USE_LOCAL_FILE);
    wxString string1 = config->Read(wxT("string1"));
    CPPUNIT_ASSERT( string1 == wxT("abc") );
    string1 = config->Read(wxT("string1"), wxT("defaultvalue"));
    CPPUNIT_ASSERT( string1 == wxT("abc") );
    wxString string2;
    bool r = config->Read(wxT("string2"), &string2);
    CPPUNIT_ASSERT( r == true );
    CPPUNIT_ASSERT( string2 == wxT("def") );
    r = config->Read(wxT("string2"), &string2, wxT("defaultvalue"));
    CPPUNIT_ASSERT( r == true );
    CPPUNIT_ASSERT( string2 == wxT("def") );
    int int1 = config->Read(wxT("int1"), 5);
    CPPUNIT_ASSERT( int1 == 123 );
    long long1;
    r = config->Read(wxT("long1"), &long1);
    CPPUNIT_ASSERT( r == true );
    CPPUNIT_ASSERT( long1 == 234 );
    bool bool1;
    r = config->Read(wxT("foo"), &bool1);
    CPPUNIT_ASSERT( r == false );
    r = config->Read(wxT("bool1"), &bool1);
    CPPUNIT_ASSERT( r == true );
    CPPUNIT_ASSERT( bool1 == true );
    wxColour color1;
    r = config->Read(wxT("color1"), &color1);
    CPPUNIT_ASSERT( r == true );
    CPPUNIT_ASSERT( color1 == wxColour(11,22,33,44) );
    config->DeleteAll();
    delete config;
}

void ConfigTestCase::RecordingDefaultsTest()
{
    wxString app = wxT("wxConfigTestCaseRD");
    wxString vendor = wxT("wxWidgets");
    wxConfig *config = new wxConfig(app, vendor, wxT(""), wxT(""),
                                    wxCONFIG_USE_LOCAL_FILE);
    config->DeleteAll();
    config->SetRecordDefaults(false); // by default it is false
    wxString string1, string2, string3, string4;
    string1 = config->Read(wxT("string1"), wxT("abc"));
    string2 = config->Read(wxT("string2"), wxString(wxT("def")));
    config->Read(wxT("string3"), &string3, wxT("abc"));
    config->Read(wxT("string4"), &string4, wxString(wxT("def")));
    int int1, int2;
    config->Read(wxT("int1"), &int1, 123);
    int2 = config->Read(wxT("int2"), 1234);
    long long1;
    config->Read(wxString(wxT("long1")), &long1, 234L);
    double double1;
    config->Read(wxT("double1"), &double1, 345.67);
    bool bool1;
    config->Read(wxT("bool1"), &bool1, true);
    wxColour color1;
    config->Read(wxT("color1"), &color1, wxColour(11,22,33,44));

    CPPUNIT_ASSERT ( config->GetNumberOfEntries() == 0 );

    config->SetRecordDefaults(true);

    bool r;
    string1 = config->Read(wxT("string1"), wxT("abc"));
    string2 = config->Read(wxT("string2"), wxString(wxT("def")));
    r = config->Read(wxT("string3"), &string3, wxT("abc"));
    CPPUNIT_ASSERT( r == false );
    r = config->Read(wxT("string4"), &string4, wxString(wxT("def")));
    CPPUNIT_ASSERT( r == false );
    r = config->Read(wxT("int1"), &int1, 123);
    CPPUNIT_ASSERT( r == false );
    int2 = config->Read(wxT("int2"), 1234);
    r = config->Read(wxString(wxT("long1")), &long1, 234L);
    CPPUNIT_ASSERT( r == false );
    r = config->Read(wxT("double1"), &double1, 345.67);
    CPPUNIT_ASSERT( r == false );
    r = config->Read(wxT("bool1"), &bool1, true);
    CPPUNIT_ASSERT( r == false );
    r = config->Read(wxT("color1"), &color1, wxColour(11,22,33,44));
    CPPUNIT_ASSERT( r == false );

    CPPUNIT_ASSERT ( config->GetNumberOfEntries() == 10 );

    r = config->Read(wxT("string3"), &string3, wxT("abc"));
    CPPUNIT_ASSERT( r == true );
    r = config->Read(wxT("string4"), &string4, wxString(wxT("def")));
    CPPUNIT_ASSERT( r == true );
    r = config->Read(wxT("int1"), &int1, 123);
    CPPUNIT_ASSERT( r == true );
    r = config->Read(wxString(wxT("long1")), &long1, 234L);
    CPPUNIT_ASSERT( r == true );
    r = config->Read(wxT("double1"), &double1, 345.67);
    CPPUNIT_ASSERT( r == true );
    r = config->Read(wxT("bool1"), &bool1, true);
    CPPUNIT_ASSERT( r == true );
    r = config->Read(wxT("color1"), &color1, wxColour(11,22,33,44));
    CPPUNIT_ASSERT( r == true );

    config->DeleteAll();
    delete config;
}


