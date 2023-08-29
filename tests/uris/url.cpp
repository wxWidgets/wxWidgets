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
#include "wx/utils.h"

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("wxURL::GetInputStream", "[url]")
{
    if (!IsNetworkAvailable())      // implemented in test.cpp
    {
        WARN("No network connectivity; skipping the wxURL::GetInputStream test unit.");
        return;
    }

    wxSocketInitializer socketInit;

    // We need a site never redirecting to HTTPs and this one seems better than
    // the other alternatives such as Microsoft's www.msftconnecttest.com or
    // Apple's captive.apple.com. IANAs example.com might be another good
    // choice but it's not clear if it's never going to redirect to HTTPs.
    wxURL url("http://detectportal.firefox.com/");
    CHECK(url.GetError() == wxURL_NOERR);

    std::unique_ptr<wxInputStream> in_stream(url.GetInputStream());
    if ( !in_stream && IsAutomaticTest() )
    {
        // Sometimes the connection fails during CI runs, don't consider this
        // as a fatal error because it happens from time to time and there is
        // nothing we can do about it.
        WARN("Connection to HTTP URL failed, skipping the test.");
        return;
    }

    REQUIRE(in_stream);
    CHECK(in_stream->IsOk());

    wxMemoryOutputStream ostream;
    CHECK(in_stream->Read(ostream).GetLastError() == wxSTREAM_EOF);

    CHECK(ostream.GetSize() == strlen("success\n"));
}

TEST_CASE("wxURL::CopyAndAssignment", "[url]")
{
    wxURL url1("http://www.example.org/");
    wxURL url2;
    wxURI *puri = &url2;        // downcast

    { // Copy constructor
        wxURL url3(url1);
        CHECK(url1 == url3);
    }
    { // Constructor for string
        wxURL url3(url1.GetURL());
        CHECK(url1 == url3);
    }
    { // 'Copy' constructor for uri
        wxURL url3(*puri);
        CHECK(url2 == url3);
    }

    // assignment for uri
    *puri = url1;
    CHECK(url1 == url2);

    // assignment to self through base pointer
    *puri = url2;

    // Assignment of string
    url1 = wxS("http://www.example2.org/index.html");
    *puri = wxS("http://www.example2.org/index.html");
    CHECK(url1 == url2);

    // Assignment
    url1 = wxS("");
    url2 = url1;
    CHECK(url1 == url2);

    // assignment to self
    wxCLANG_WARNING_SUPPRESS(self-assign-overloaded)
    url2 = url2;
    wxCLANG_WARNING_RESTORE(self-assign-overloaded)

    // check for destructor (with base pointer!)
    puri = new wxURL();
    delete puri;
}
