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

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#if wxUSE_WEBREQUEST

#include "wx/webrequest.h"
#include "wx/filename.h"
#include "wx/wfstream.h"

// This test uses httpbin service and by default uses the mirror at the
// location below, which seems to be more reliable than the main site at
// https://httpbin.org. Any other mirror, including a local one, which can be
// set by running kennethreitz/httpbin Docker container, can be used by setting
// WX_TEST_WEBREQUEST_URL environment variable to its URL.
//
// This variable can also be set to a special value "0" to disable running the
// test entirely.
static const char* WX_TEST_WEBREQUEST_URL_DEFAULT = "https://nghttp2.org/httpbin";

class RequestFixture : public wxTimer
{
public:
    RequestFixture()
    {
        expectedFileSize = 0;
        dataSize = 0;
        stateFromEvent = wxWebRequest::State_Idle;
        statusFromEvent = 0;
    }

    // All tests should call this function first and skip the test entirely if
    // it returns false, as this indicates that web requests tests are disabled.
    bool InitBaseURL()
    {
        if ( wxGetEnv("WX_TEST_WEBREQUEST_URL", &baseURL) )
        {
            static bool s_shown = false;
            if ( !s_shown )
            {
                s_shown = true;
                WARN("Using non-default root URL " << baseURL);
            }
        }
        else
        {
            baseURL = WX_TEST_WEBREQUEST_URL_DEFAULT;
        }

        return baseURL != "0";
    }

    void Create(const wxString& subURL)
    {
        CreateAbs(baseURL + subURL);
    }

    void CreateAbs(const wxString& url)
    {
        request = wxWebSession::GetDefault().CreateRequest(this, url);
        Bind(wxEVT_WEBREQUEST_STATE, &RequestFixture::OnRequestState, this);
        Bind(wxEVT_WEBREQUEST_DATA, &RequestFixture::OnData, this);
    }

    void OnRequestState(wxWebRequestEvent& evt)
    {
        stateFromEvent = evt.GetState();
        const wxWebResponse& response = evt.GetResponse();
        if ( response.IsOk() )
        {
            // Note that the response object itself may be deleted if request
            // using it is, so we need to copy its data to use it later.
            statusFromEvent = response.GetStatus();
            responseStringFromEvent = response.AsString();
        }

        switch ( stateFromEvent )
        {
        case wxWebRequest::State_Idle:
            FAIL("should never get events with State_Idle");
            break;

        case wxWebRequest::State_Active:
            CHECK( request.GetNativeHandle() );
            break;

        case wxWebRequest::State_Completed:
            if ( request.IsOk() && request.GetStorage() == wxWebRequest::Storage_File )
            {
                wxFileName fn(evt.GetDataFile());
                CHECK( fn.GetSize() == expectedFileSize );
            }
            wxFALLTHROUGH;

        case wxWebRequest::State_Unauthorized:
        case wxWebRequest::State_Failed:
        case wxWebRequest::State_Cancelled:
            errorDescription = evt.GetErrorDescription();
            loop.Exit();
            break;
        }
    }

    void Notify() wxOVERRIDE
    {
        WARN("Exiting loop on timeout");
        loop.Exit();
    }

    void OnData(wxWebRequestEvent& evt)
    {
        // Count all bytes recieved via data event for Storage_None
        dataSize += evt.GetDataSize();
    }

    void RunLoopWithTimeout()
    {
        StartOnce(30000); // Ensure that we exit the loop after 30s.
        loop.Run();
        Stop();
    }

    void Run(wxWebRequest::State requiredState = wxWebRequest::State_Completed,
        int requiredStatus = 200)
    {
        REQUIRE( request.GetState() == wxWebRequest::State_Idle );
        request.Start();
        RunLoopWithTimeout();

        if ( stateFromEvent != requiredState )
        {
            errorDescription.Trim();
            if ( !errorDescription.empty() )
                WARN("Error: " << errorDescription);
        }

        REQUIRE( stateFromEvent == requiredState );

        CHECK( request.GetState() == stateFromEvent );

        if (requiredStatus)
        {
            CHECK( statusFromEvent == requiredStatus );
            CHECK( request.GetResponse().GetStatus() == requiredStatus );
        }
    }

    // Precondition: we must have an auth challenge.
    void UseCredentials(const wxString& user, const wxString& password)
    {
        request.GetAuthChallenge().SetCredentials(
            wxWebCredentials(user, wxSecretValue(password)));
    }

    wxString baseURL;
    wxEventLoop loop;
    wxWebRequest request;
    wxWebRequest::State stateFromEvent;
    int statusFromEvent;
    wxString responseStringFromEvent;
    wxInt64 expectedFileSize;
    wxInt64 dataSize;
    wxString errorDescription;
};

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::Bytes", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    Create("/bytes/65536");
    Run();
    CHECK( request.GetResponse().GetContentLength() == 65536 );
    CHECK( request.GetBytesExpectedToReceive() == 65536 );
    CHECK( request.GetBytesReceived() == 65536 );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::Simple", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    // Note that the session may be initialized on demand, so don't check the
    // native handle before actually using it.
    wxWebSession& session = wxWebSession::GetDefault();
    REQUIRE( session.IsOpened() );

    // Request is not initialized yet.
    CHECK( !request.IsOk() );
    CHECK( !request.GetNativeHandle() );

    Create("/status/200");
    CHECK( request.IsOk() );
    CHECK( session.GetNativeHandle() );

    // Note that the request must be started to have a valid native handle.
    request.Start();
    CHECK( request.GetNativeHandle() );
    RunLoopWithTimeout();
    CHECK( request.GetState() == wxWebRequest::State_Completed );
    CHECK( request.GetResponse().GetStatus() == 200 );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::String", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    Create("/base64/VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==");
    Run();
    CHECK( request.GetResponse().AsString() == "The quick brown fox jumps over the lazy dog" );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::Param", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    Create("/get?pi=3.14159265358979323");
    Run();

    // We ought to really parse the returned JSON object, but to keep things as
    // simple as possible for now we just treat it as a string.
    const wxString& response = request.GetResponse().AsString();
    INFO("Response: " << response);

    const char* expectedKey = "\"pi\":";
    size_t pos = response.find(expectedKey);
    REQUIRE( pos != wxString::npos );

    pos += strlen(expectedKey);

    // There may, or not, be a space after it.
    while ( wxIsspace(response[pos]) )
        pos++;

    const char* expectedValue = "\"3.14159265358979323\"";
    REQUIRE( response.compare(pos, strlen(expectedValue), expectedValue) == 0 );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::File", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    expectedFileSize = 99 * 1024;
    Create(wxString::Format("/bytes/%lld", expectedFileSize));
    request.SetStorage(wxWebRequest::Storage_File);
    Run();
    CHECK( request.GetBytesReceived() == expectedFileSize );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::None", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    int processingSize = 99 * 1024;
    Create(wxString::Format("/bytes/%d", processingSize));
    request.SetStorage(wxWebRequest::Storage_None);
    Run();
    CHECK( request.GetBytesReceived() == processingSize );
    CHECK( dataSize == processingSize );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Error::HTTP", "[net][webrequest][error]")
{
    if ( !InitBaseURL() )
        return;

    Create("/status/404");
    Run(wxWebRequest::State_Failed, 404);
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Error::Body", "[net][webrequest][error]")
{
    if ( !InitBaseURL() )
        return;

    Create("/status/418");
    Run(wxWebRequest::State_Failed, 0);

    CHECK( request.GetResponse().GetStatus() == 418 );

    const wxString& response = request.GetResponse().AsString();
    INFO( "Response: " << response);
    CHECK( response.Contains("teapot") );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Error::Connect", "[net][webrequest][error]")
{
    if ( !InitBaseURL() )
        return;

    CreateAbs("http://127.0.0.1:51234");
    Run(wxWebRequest::State_Failed, 0);
}

TEST_CASE_METHOD(RequestFixture,
    "WebRequest::SSL::Error", "[net][webrequest][error]")
{
    wxString selfsignedURL;
    if ( !wxGetEnv("WX_TEST_WEBREQUEST_URL_SELF_SIGNED", &selfsignedURL) )
    {
        WARN("Skipping because WX_TEST_WEBREQUEST_URL_SELF_SIGNED is not set.");
        return;
    }

    if (!InitBaseURL())
        return;

    CreateAbs(selfsignedURL);
    Run(wxWebRequest::State_Failed, 0);
}

TEST_CASE_METHOD(RequestFixture,
    "WebRequest::SSL::Ignore", "[net][webrequest]")
{
    wxString selfsignedURL;
    if ( !wxGetEnv("WX_TEST_WEBREQUEST_URL_SELF_SIGNED", &selfsignedURL) )
    {
        WARN("Skipping because WX_TEST_WEBREQUEST_URL_SELF_SIGNED is not set.");
        return;
    }

    if (!InitBaseURL())
        return;

    // For some reason this test sporadically fails under AppVeyor, so don't
    // run it there.
#ifdef __WINDOWS__
    if ( IsAutomaticTest() )
    {
        WARN("Skipping DisablePeerVerify() test known to sporadically fail.");
        return;
    }
#endif // __WINDOWS__

    CreateAbs(selfsignedURL);
    request.DisablePeerVerify();
    Run(wxWebRequest::State_Completed, 200);
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Post", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("/post");
    request.SetData("app=WebRequestSample&version=1", "application/x-www-form-urlencoded");
    Run();
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Put", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("/put");
    wxScopedPtr<wxInputStream> is(new wxFileInputStream("horse.png"));
    REQUIRE( is->IsOk() );

    request.SetData(is.release(), "image/png");
    request.SetMethod("PUT");
    Run();
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Auth::Basic", "[net][webrequest][auth]")
{
    if ( !InitBaseURL() )
        return;

    Create("/basic-auth/wxtest/wxwidgets");
    Run(wxWebRequest::State_Unauthorized, 401);
    REQUIRE( request.GetAuthChallenge().IsOk() );

    SECTION("Good password")
    {
        UseCredentials("wxtest", "wxwidgets");
        RunLoopWithTimeout();
        CHECK( request.GetResponse().GetStatus() == 200 );
        CHECK( request.GetState() == wxWebRequest::State_Completed );
    }

    SECTION("Bad password")
    {
        UseCredentials("wxtest", "foobar");
        RunLoopWithTimeout();
        CHECK( request.GetResponse().GetStatus() == 401 );
        CHECK( request.GetState() == wxWebRequest::State_Unauthorized );
    }
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Auth::Digest", "[net][webrequest][auth]")
{
    if ( !InitBaseURL() )
        return;

    Create("/digest-auth/auth/wxtest/wxwidgets");
    Run(wxWebRequest::State_Unauthorized, 401);
    REQUIRE( request.GetAuthChallenge().IsOk() );

    SECTION("Good password")
    {
        UseCredentials("wxtest", "wxwidgets");
        RunLoopWithTimeout();
        CHECK( request.GetResponse().GetStatus() == 200 );
        CHECK( request.GetState() == wxWebRequest::State_Completed );
    }

    SECTION("Bad password")
    {
        UseCredentials("foo", "bar");
        RunLoopWithTimeout();
        CHECK( request.GetResponse().GetStatus() == 401 );
        CHECK( request.GetState() == wxWebRequest::State_Unauthorized );
    }
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Cancel", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("/delay/10");
    request.Start();
    request.Cancel();
    RunLoopWithTimeout();

#ifdef __WINDOWS__
    // This is another weird test failure that happens only on AppVeyor:
    // sometimes (perhaps because the test machine is too slow?) the request
    // fails instead of (before?) being cancelled.
    if ( IsAutomaticTest() )
    {
        if ( request.GetState() == wxWebRequest::State_Failed )
        {
            WARN("Request unexpectedly failed after cancelling.");
            return;
        }
    }
#endif // __WINDOWS__

    REQUIRE( request.GetState() == wxWebRequest::State_Cancelled );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Destroy", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("/base64/U3RpbGwgYWxpdmUh");
    request.Start();

    // Destroy the original request: this shouldn't prevent it from running to
    // the completion!
    request = wxWebRequest();

    RunLoopWithTimeout();

    CHECK( stateFromEvent == wxWebRequest::State_Completed );
    CHECK( statusFromEvent == 200 );
    CHECK( responseStringFromEvent == "Still alive!" );
}

// This test is not run by default and has to be explicitly selected to run.
TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Manual", "[.]")
{
    // Allow getting 8-bit strings from the environment correctly.
    setlocale(LC_ALL, "");

    wxString url;
    if ( !wxGetEnv("WX_TEST_WEBREQUEST_URL", &url) )
    {
        FAIL("Specify WX_TEST_WEBREQUEST_URL");
    }

    CreateAbs(url);
    request.Start();
    RunLoopWithTimeout();

    WARN("Request state " << request.GetState());
    wxWebResponse response = request.GetResponse();
    REQUIRE( response.IsOk() );
    WARN("Status: " << response.GetStatus()
                    << " (" << response.GetStatusText() << ")\n" <<
         "Body length: " << response.GetContentLength() << "\n" <<
         "Body: " << response.AsString() << "\n");
}

WX_DECLARE_STRING_HASH_MAP(wxString, wxWebRequestHeaderMap);

namespace wxPrivate
{
WXDLLIMPEXP_NET wxString
SplitParameters(const wxString& s, wxWebRequestHeaderMap& parameters);
}

TEST_CASE("WebRequestUtils", "[net][webrequest]")
{
    wxString value;
    wxWebRequestHeaderMap params;

    wxString header = "multipart/mixed; boundary=\"MIME_boundary_01234567\"";

    value = wxPrivate::SplitParameters(header, params);
    CHECK( value == "multipart/mixed" );
    CHECK( params.size() == 1 );
    CHECK( params["boundary"] == "MIME_boundary_01234567" );
}

#endif // wxUSE_WEBREQUEST
