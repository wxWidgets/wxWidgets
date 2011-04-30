///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/url.cpp
// Purpose:     wxURL unit test
// Author:      Francesco Montorsi
// Created:     2009-5-31
// RCS-ID:      $Id: uris.cpp 58272 2009-01-21 17:02:11Z VZ $
// Copyright:   (c) 2009 Francesco Montorsi
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

#include "wx/url.h"
#include "wx/mstream.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class URLTestCase : public CppUnit::TestCase
{
public:
    URLTestCase(); 
    ~URLTestCase();

private:
    CPPUNIT_TEST_SUITE( URLTestCase );
        CPPUNIT_TEST( GetInputStream );
        CPPUNIT_TEST( CopyAndAssignment );
    CPPUNIT_TEST_SUITE_END();

    void GetInputStream();
    void CopyAndAssignment();

    DECLARE_NO_COPY_CLASS(URLTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( URLTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( URLTestCase, "URLTestCase" );


URLTestCase::URLTestCase()
{
    wxSocketBase::Initialize();
}

URLTestCase::~URLTestCase()
{
    wxSocketBase::Shutdown();
}

void URLTestCase::GetInputStream()
{
    if (!IsNetworkAvailable())      // implemented in test.cpp
    {
        wxLogWarning("No network connectivity; skipping the URLTestCase::GetInputStream test unit.");
        return;
    }

    wxURL url("http://wxwidgets.org/logo9.jpg");
    CPPUNIT_ASSERT_EQUAL(wxURL_NOERR, url.GetError());

    wxInputStream *in_stream = url.GetInputStream();
    CPPUNIT_ASSERT(in_stream && in_stream->IsOk());

    wxMemoryOutputStream ostream;
    CPPUNIT_ASSERT(in_stream->Read(ostream).GetLastError() == wxSTREAM_EOF);

    // wx logo image currently is 13219 bytes
    CPPUNIT_ASSERT(ostream.GetSize() == 13219);

    // we have to delete the object created by GetInputStream()
    delete in_stream;
}

void URLTestCase::CopyAndAssignment()
{
    wxURL url1("http://www.example.org/");
    wxURL url2;
    wxURI *puri = &url2;        // downcast

    { // Copy constructor
        wxURL url3(url1);
        CPPUNIT_ASSERT(url1 == url3);
    }
    { // Constructor for string
        wxURL url3(url1.GetURL());
        CPPUNIT_ASSERT(url1 == url3);
    }
    { // 'Copy' constructor for uri
        wxURL url3(*puri);
        CPPUNIT_ASSERT(url2 == url3);
    }

    // assignment for uri
    *puri = url1;
    CPPUNIT_ASSERT(url1 == url2);

    // assignment to self through base pointer
    *puri = url2;

    // Assignment of string
    url1 = wxS("http://www.example2.org/index.html");
    *puri = wxS("http://www.example2.org/index.html");
    CPPUNIT_ASSERT(url1 == url2);

    // Assignment
    url1 = wxS("");
    url2 = url1;
    CPPUNIT_ASSERT(url1 == url2);

    // assignment to self
    url2 = url2;

    // check for destructor (with base pointer!)
    puri = new wxURL();
    delete puri;
}
