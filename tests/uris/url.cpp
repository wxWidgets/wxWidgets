///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/url.cpp
// Purpose:     wxURL unit test
// Author:      Francesco Montorsi
// Created:     2009-5-31
// Copyright:   (c) 2009 Francesco Montorsi
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/url.h"
#include "wx/mstream.h"
#include "wx/scopedptr.h"
#include "wx/utils.h"

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

    wxDECLARE_NO_COPY_CLASS(URLTestCase);
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

    wxURL url("http://www.wxwidgets.org/assets/img/header-logo.png");
    CPPUNIT_ASSERT_EQUAL(wxURL_NOERR, url.GetError());

    wxScopedPtr<wxInputStream> in_stream(url.GetInputStream());
    if ( !in_stream && IsAutomaticTest() )
    {
        // Sometimes the connection fails during CI runs, don't consider this
        // as a fatal error because it happens from time to time and there is
        // nothing we can do about it.
        WARN("Connection to www.wxwidgets.org failed, skipping the test.");
        return;
    }

    CPPUNIT_ASSERT(in_stream);
    CPPUNIT_ASSERT(in_stream->IsOk());

    wxMemoryOutputStream ostream;
    CPPUNIT_ASSERT(in_stream->Read(ostream).GetLastError() == wxSTREAM_EOF);

    CPPUNIT_ASSERT_EQUAL(17334, ostream.GetSize());
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
    wxCLANG_WARNING_SUPPRESS(self-assign-overloaded)
    url2 = url2;
    wxCLANG_WARNING_RESTORE(self-assign-overloaded)

    // check for destructor (with base pointer!)
    puri = new wxURL();
    delete puri;
}
