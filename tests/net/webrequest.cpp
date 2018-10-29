/////////////////////////////////////////////////////////////////////////////
// Name:        tests/net/webrequest.cpp
// Purpose:     wxWebRequest test
// Author:      Tobias Taschner
// Created:     2018-10-24
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#include "wx/webrequest.h"
#include "wx/wfstream.h"

// This test requires the URL to an httpbin instance.
// httpbin is a HTTP Request & Response Service available at:
// https://httpbin.org
// It can also be run locally via a simple docker run
//
// For this test to run the the base URL has to be specified with
// an environment variable, e.g.:
//   WX_TEST_WEBREQUEST_URL=https://httpbin.org

#if wxUSE_WEBREQUEST

class RequestFixture
{
public:
    RequestFixture() { }

    void Create(const wxString& subURL)
    {
        wxString baseURL;
        wxGetEnv("WX_TEST_WEBREQUEST_URL", &baseURL);
        CreateAbs(baseURL + subURL);
    }

    void CreateAbs(const wxString& url)
    {
        request.reset(wxWebSession::GetDefault().CreateRequest(url));
        request->Bind(wxEVT_WEBREQUEST_STATE, &RequestFixture::OnRequestState, this);
    }

    void OnRequestState(wxWebRequestEvent& evt)
    {
        switch (evt.GetState())
        {
        case wxWebRequest::State_Unauthorized:
        case wxWebRequest::State_Completed:
        case wxWebRequest::State_Failed:
        case wxWebRequest::State_Cancelled:
            loop.Exit();
            break;
        }
    }

    void Run(wxWebRequest::State requiredState = wxWebRequest::State_Completed,
        int requiredStatus = 200)
    {
        REQUIRE( request->GetState() == wxWebRequest::State_Idle );
        request->Start();
        loop.Run();
        REQUIRE( request->GetState() == requiredState );
        if (requiredStatus)
            REQUIRE( request->GetResponse()->GetStatus() == requiredStatus );
    }

    wxEventLoop loop;
    wxObjectDataPtr<wxWebRequest> request;
};

TEST_CASE_METHOD(RequestFixture, "WebRequest", "[net][.]")
{
    wxString baseURL;
    if ( !wxGetEnv("WX_TEST_WEBREQUEST_URL", &baseURL) )
    {
        WARN("Skipping WebRequest test because required WX_TEST_WEBREQUEST_URL"
            " environment variable is not defined.");
        return;
    }

    SECTION("GET 64kb to memory")
    {
        Create("/bytes/65536");
        Run();
        REQUIRE( request->GetResponse()->GetContentLength() == 65536 );
        REQUIRE( request->GetBytesExpectedToReceive() == 65536 );
        REQUIRE( request->GetBytesReceived() == 65536 );
    }

    SECTION("GET 404 error")
    {
        Create("/status/404");
        Run(wxWebRequest::State_Failed, 404);
    }

    SECTION("Connect to invalid host")
    {
        CreateAbs("http://127.0.0.1:51234");
        Run(wxWebRequest::State_Failed, 0);
    }

    SECTION("POST form data")
    {
        Create("/post");
        request->SetData("app=WebRequestSample&version=1", "application/x-www-form-urlencoded");
        Run();
    }

    SECTION("GET data as string")
    {
        Create("/base64/VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==");
        Run();
        REQUIRE( request->GetResponse()->AsString() == "The quick brown fox jumps over the lazy dog" );
    }

    SECTION("PUT file data")
    {
        Create("/put");
        request->SetData(wxSharedPtr<wxInputStream>(new wxFileInputStream("horse.png")),
            "image/png");
        request->SetMethod("PUT");
        Run();
    }

    SECTION("Server auth BASIC")
    {
        Create("/digest-auth/auth/wxtest/wxwidgets");
        Run(wxWebRequest::State_Unauthorized, 401);
        REQUIRE( request->GetAuthChallenge() != NULL );
        request->GetAuthChallenge()->SetCredentials("wxtest", "wxwidgets");
        loop.Run();
        REQUIRE( request->GetResponse()->GetStatus() == 200 );
        REQUIRE( request->GetState() == wxWebRequest::State_Completed );
    }

    SECTION("Server auth DIGEST")
    {
        Create("/digest-auth/auth/wxtest/wxwidgets");
        Run(wxWebRequest::State_Unauthorized, 401);
        REQUIRE( request->GetAuthChallenge() != NULL );
        request->GetAuthChallenge()->SetCredentials("wxtest", "wxwidgets");
        loop.Run();
        REQUIRE( request->GetResponse()->GetStatus() == 200 );
        REQUIRE( request->GetState() == wxWebRequest::State_Completed );
    }
}

#endif // wxUSE_WEBREQUEST
