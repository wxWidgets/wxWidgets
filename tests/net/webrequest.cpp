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

#include <memory>
#include <unordered_map>

// This test uses httpbin service and by default uses the mirror at the
// location below, which seems to be more reliable than the main site at
// https://httpbin.org. Any other mirror, including a local one, which can be
// set by running kennethreitz/httpbin Docker container, can be used by setting
// WX_TEST_WEBREQUEST_URL environment variable to its URL.
//
// This variable can also be set to a special value "0" to disable running the
// test entirely.
static const char* WX_TEST_WEBREQUEST_URL_DEFAULT = "https://nghttp2.org/httpbin";

// Other environment variables used by this test:
//
// - WX_TEST_WEBREQUEST_USE_BADSSL: setting this to 1 is equivalent to setting
//   all the variables below to their example values using badssl.com.
//
// - WX_TEST_WEBREQUEST_URL_SELF_SIGNED: set to https://self-signed.badssl.com/
//   or any other server using self-signed certificate to test disabling SSL
//   certificate trust chain verification.
//
// - WX_TEST_WEBREQUEST_URL_EXPIRED: set to https://expired.badssl.com/ or any
//   other server using expired certificate to test disabling SSL certificate
//   date validity verification.
//
// - WX_TEST_WEBREQUEST_URL_BADHOST: set to https://wrong.host.badssl.com/ or
//   any other server using certificate with wrong host name to test disabling
//   SSL host name verification.

// Common base for sync and async tests fixtures.
class BaseRequestFixture
{
protected:
    // All tests should call this function first and skip the test entirely if
    // it returns false, as this indicates that web requests tests are disabled.
    bool InitBaseURL()
    {
        if ( wxGetEnv("WX_TEST_WEBREQUEST_URL", &baseURL) )
        {
            if ( baseURL == "0" )
                return false;

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

        REQUIRE( GetSession().SetBaseURL(baseURL) );

        return true;
    }

    enum class BadSSLKind
    {
        SelfSigned,
        Expired,
        BadHost
    };

    // Return true if the test is enabled by setting the corresponding
    // environment variable, false if it should be skipped.
    bool GetBadSSLURL(BadSSLKind kind, wxString* url) const
    {
        const auto useBadSSL = [](const char* badSSLURL, wxString* url)
        {
            wxString s;
            if ( !wxGetEnv("WX_TEST_WEBREQUEST_USE_BADSSL", &s) || s != "1" )
                return false;

            *url = badSSLURL;

            return true;
        };

        switch ( kind )
        {
            case BadSSLKind::SelfSigned:
                if ( wxGetEnv("WX_TEST_WEBREQUEST_URL_SELF_SIGNED", url) )
                    return true;

                if ( useBadSSL("https://self-signed.badssl.com/", url) )
                    return true;
                break;

            case BadSSLKind::Expired:
                if ( wxGetEnv("WX_TEST_WEBREQUEST_URL_EXPIRED", url) )
                    return true;

                if ( useBadSSL("https://expired.badssl.com/", url) )
                    return true;
                break;

            case BadSSLKind::BadHost:
                if ( wxGetEnv("WX_TEST_WEBREQUEST_URL_BADHOST", url) )
                    return true;

                if ( useBadSSL("https://wrong.host.badssl.com/", url) )
                    return true;
                break;
        }

        return false;
    }

    void
    CreateWithAuth(const wxString& relURL,
                   const wxString& user,
                   const wxString& password)
    {
        wxString schema;
        wxString rest;
        if ( baseURL.StartsWith("https://", &rest) )
        {
            schema = "https";
        }
        else if ( baseURL.StartsWith("http://", &rest) )
        {
            schema = "http";
        }
        else
        {
            FAIL("Base URL must be an HTTP(S) one: " << baseURL);
        }

        Create(wxString::Format("%s://%s:%s@%s/%s",
                                schema, user, password, rest, relURL));
    }

    virtual void Create(const wxString& url) = 0;

    virtual wxWebSessionBase& GetSession() = 0;

    virtual wxWebRequestBase& GetRequest() = 0;

    // Check that the response is a JSON object containing a key "pi" with the
    // expected value.
    void CheckExpectedJSON(const wxString& response)
    {
        // We ought to really parse the returned JSON object, but to keep things as
        // simple as possible for now we just treat it as a string.
        INFO("Response: " << response);

        const char* expectedKey = "\"pi\":";
        size_t pos = response.find(expectedKey);
        REQUIRE( pos != wxString::npos );

        pos += strlen(expectedKey);

        // There may, or not, be a space after it.
        // And the value may be returned in an array.
        while ( wxIsspace(response[pos]) || response[pos] == '[' )
            pos++;

        const char* expectedValue = "\"3.14159265358979323\"";
        REQUIRE( response.compare(pos, strlen(expectedValue), expectedValue) == 0 );
    }

    // Special helper for "manual" tests taking the URL from the environment.
    void InitManualRequest()
    {
        // Allow getting 8-bit strings from the environment correctly.
        setlocale(LC_ALL, "");

        wxString url;
        if ( !wxGetEnv("WX_TEST_WEBREQUEST_URL", &url) )
        {
            FAIL("Specify WX_TEST_WEBREQUEST_URL");
        }

        wxString proxyURL;
        if ( wxGetEnv("WX_TEST_WEBREQUEST_PROXY", &proxyURL) )
        {
            wxWebProxy proxy = wxWebProxy::Default();

            // Interpret some values specially.
            if ( proxyURL == "0" )
                proxy = wxWebProxy::Disable();
            else if ( proxyURL != "1" )
                proxy = wxWebProxy::FromURL(proxyURL);

            REQUIRE( GetSession().SetProxy(proxy) );
        }

        Create(url);

        wxString insecure;
        if ( wxGetEnv("WX_TEST_WEBREQUEST_INSECURE", &insecure) )
        {
            int flags = 0;
            REQUIRE( insecure.ToInt(&flags) );

            GetRequest().MakeInsecure(flags);
        }
    }

private:
    wxString baseURL;
};

class RequestFixture : public wxTimer, public BaseRequestFixture
{
public:
    RequestFixture()
    {
        expectedFileSize = 0;
        dataSize = 0;
        stateFromEvent = wxWebRequest::State_Idle;
        statusFromEvent = 0;
    }

    void Create(const wxString& url) override
    {
        request = wxWebSession::GetDefault().CreateRequest(this, url);
        REQUIRE( request.IsOk() );

        Bind(wxEVT_WEBREQUEST_STATE, &RequestFixture::OnRequestState, this);
        Bind(wxEVT_WEBREQUEST_DATA, &RequestFixture::OnData, this);
    }

    wxWebSessionBase& GetSession() override
    {
        return wxWebSession::GetDefault();
    }

    wxWebRequestBase& GetRequest() override
    {
        return request;
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

    void Notify() override
    {
        WARN("Exiting loop on timeout");
        loop.Exit();
    }

    void OnData(wxWebRequestEvent& evt)
    {
        // Count all bytes received via data event for Storage_None
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

    wxEventLoop loop;
    wxWebRequest request;
    wxWebRequest::State stateFromEvent;
    int statusFromEvent;
    wxString responseStringFromEvent;
    wxInt64 expectedFileSize;
    wxInt64 dataSize;
    wxString errorDescription;
};

// Download more than 64KiB bytes to test that downloading more than the
// default buffer size works correctly.
constexpr int DOWNLOAD_BYTES = 99999;

// Substring used to check that we got the expected response after
// authenticating successfully. It is so weird because httpbin and go-httpbin
// use different strings for this: one uses "authenticated" while the other
// ones uses "authorized", so we use a substring common to both of them.
constexpr char AUTHORIZED_SUBSTRING[] = R"(ed": true)";

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::Bytes", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    Create(wxString::Format("bytes/%d", DOWNLOAD_BYTES));
    Run();
    CHECK( request.GetResponse().GetContentLength() == DOWNLOAD_BYTES );
    CHECK( request.GetBytesExpectedToReceive() == DOWNLOAD_BYTES );
    CHECK( request.GetBytesReceived() == DOWNLOAD_BYTES );
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

    Create("status/200");
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

    Create("base64/VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==");
    Run();
    CHECK( request.GetResponse().AsString() == "The quick brown fox jumps over the lazy dog" );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::Header", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    Create("response-headers?freeform=wxWidgets%20works!");
    Run();
    CHECK( request.GetResponse().GetHeader("freeform") == "wxWidgets works!" );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::Param", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    Create("get?pi=3.14159265358979323");
    Run();

    CheckExpectedJSON( request.GetResponse().AsString() );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Get::File", "[net][webrequest][get]")
{
    if ( !InitBaseURL() )
        return;

    expectedFileSize = 99 * 1024;
    Create(wxString::Format("bytes/%lld", expectedFileSize));
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
    Create(wxString::Format("bytes/%d", processingSize));
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

    Create("status/404");
    Run(wxWebRequest::State_Failed, 404);
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Error::Body", "[net][webrequest][error]")
{
    if ( !InitBaseURL() )
        return;

    Create("status/418");
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

    Create("http://127.0.0.1:51234");
    Run(wxWebRequest::State_Failed, 0);
}

TEST_CASE_METHOD(RequestFixture,
    "WebRequest::SSL::Error", "[net][webrequest][error]")
{
    wxString url;

    if ( GetBadSSLURL(BadSSLKind::SelfSigned, &url) )
    {
        INFO("Testing self-signed certificate at " << url);

        Create(url);
        Run(wxWebRequest::State_Failed, 0);

        Create(url);
        request.DisablePeerVerify();
        Run(wxWebRequest::State_Completed, 200);
    }

    if ( GetBadSSLURL(BadSSLKind::Expired, &url) )
    {
        INFO("Testing expired certificate at " << url);

        Create(url);
        Run(wxWebRequest::State_Failed, 0);

        Create(url);
        request.MakeInsecure(wxWebRequest::Ignore_Certificate);
        Run(wxWebRequest::State_Completed, 200);
    }

    if ( GetBadSSLURL(BadSSLKind::BadHost, &url) )
    {
        INFO("Testing certificate with bad host at " << url);

        Create(url);
        Run(wxWebRequest::State_Failed, 0);

        // Currently disabling certificate verification also disables host name
        // verification in NSURLSession backend, so skip this test with it.
        if ( wxWebSession::GetDefault().GetLibraryVersionInfo().GetName()
                != "URLSession" )
        {
            Create(url);
            request.MakeInsecure(wxWebRequest::Ignore_Certificate);
            Run(wxWebRequest::State_Failed, 0);
        }

        Create(url);
        request.MakeInsecure(wxWebRequest::Ignore_Host);
        Run(wxWebRequest::State_Completed, 200);
    }
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Post", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("post");
    request.SetData("app=WebRequestSample&version=1", "application/x-www-form-urlencoded");
    Run();
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Put", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("put");
    std::unique_ptr<wxInputStream> is(new wxFileInputStream("horse.png"));
    REQUIRE( is->IsOk() );

    request.SetData(is.release(), "image/png");
    request.SetMethod("PUT");
    Run();
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Delete", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("delete");
    request.SetData(R"({"bloordyblop": 17})", "application/json");
    request.SetMethod("DELETE");
    Run();

    const wxString& response = request.GetResponse().AsString();
    CHECK_THAT( response.utf8_string(),
                Catch::Contains(R"("bloordyblop": 17)") );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Auth::Basic", "[net][webrequest][auth]")
{
    if ( !InitBaseURL() )
        return;

    Create("basic-auth/wxtest/wxwidgets");
    Run(wxWebRequest::State_Unauthorized, 401);
    REQUIRE( request.GetAuthChallenge().IsOk() );

    SECTION("Good password")
    {
        UseCredentials("wxtest", "wxwidgets");
        RunLoopWithTimeout();
        CHECK( request.GetState() == wxWebRequest::State_Completed );

        const auto& response = request.GetResponse();
        CHECK( response.GetStatus() == 200 );
        CHECK_THAT( response.AsString().utf8_string(),
                    Catch::Contains(AUTHORIZED_SUBSTRING) );
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

    Create("digest-auth/auth/wxtest/wxwidgets");
    Run(wxWebRequest::State_Unauthorized, 401);
    REQUIRE( request.GetAuthChallenge().IsOk() );

    SECTION("Good password")
    {
        UseCredentials("wxtest", "wxwidgets");
        RunLoopWithTimeout();
        CHECK( request.GetState() == wxWebRequest::State_Completed );

        const auto& response = request.GetResponse();
        CHECK( response.GetStatus() == 200 );
        CHECK_THAT( response.AsString().utf8_string(),
                    Catch::Contains(AUTHORIZED_SUBSTRING) );
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
                 "WebRequest::Auth::BasicInURL", "[net][webrequest][auth]")
{
    if ( !InitBaseURL() )
        return;

    CreateWithAuth("basic-auth/wxtest/wxwidgets", "wxtest", "wxwidgets");
    Run();

    CHECK( request.GetState() == wxWebRequest::State_Completed );

    const auto& response = request.GetResponse();
    CHECK( response.GetStatus() == 200 );
    CHECK_THAT( response.AsString().utf8_string(),
                Catch::Contains(AUTHORIZED_SUBSTRING) );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Auth::DigestInURL", "[net][webrequest][auth]")
{
    if ( !InitBaseURL() )
        return;

    CreateWithAuth("digest-auth/auth/wxtest/wxwidgets", "wxtest", "wxwidgets");
    Run();

    CHECK( request.GetState() == wxWebRequest::State_Completed );

    const auto& response = request.GetResponse();
    CHECK( response.GetStatus() == 200 );
    CHECK_THAT( response.AsString().utf8_string(),
                Catch::Contains(AUTHORIZED_SUBSTRING) );
}

TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Cancel", "[net][webrequest]")
{
    if ( !InitBaseURL() )
        return;

    Create("delay/10");
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

    Create("base64/U3RpbGwgYWxpdmUh");
    request.Start();

    // Destroy the original request: this shouldn't prevent it from running to
    // the completion!
    request = wxWebRequest();

    RunLoopWithTimeout();

    CHECK( stateFromEvent == wxWebRequest::State_Completed );
    CHECK( statusFromEvent == 200 );
    CHECK( responseStringFromEvent == "Still alive!" );
}

class SyncRequestFixture : public BaseRequestFixture
{
public:
    void Create(const wxString& url) override
    {
        request = wxWebSessionSync::GetDefault().CreateRequest(url);
        REQUIRE( request.IsOk() );
    }

    wxWebSessionBase& GetSession() override
    {
        return wxWebSessionSync::GetDefault();
    }

    wxWebRequestBase& GetRequest() override
    {
        return request;
    }

    bool Execute(const wxString& url)
    {
        Create(url);
        return Execute();
    }

    bool Execute()
    {
        const auto result = request.Execute();

        response = request.GetResponse();
        state = result.state;
        error = result.error;

        switch ( state )
        {
            case wxWebRequest::State_Idle:
            case wxWebRequest::State_Active:
            case wxWebRequest::State_Cancelled:
                wxFAIL_MSG("Unexpected state");
                wxFALLTHROUGH;

            case wxWebRequest::State_Failed:
            case wxWebRequest::State_Unauthorized:
                break;

            case wxWebRequest::State_Completed:
                return true;
        }

        return false;
    }

    wxWebRequestSync request;
    wxWebResponse response;
    wxWebRequest::State state = wxWebRequest::State_Idle;
    wxString error;
};

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Get::Bytes", "[net][webrequest][sync][get]")
{
    if ( !InitBaseURL() )
        return;

    REQUIRE( Execute(wxString::Format("bytes/%d", DOWNLOAD_BYTES)) );

    CHECK( response.GetStatus() == 200 );
    CHECK( response.GetContentLength() == DOWNLOAD_BYTES );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Get::Simple", "[net][webrequest][sync][get]")
{
    if ( !InitBaseURL() )
        return;

    // Note that the session may be initialized on demand, so don't check the
    // native handle before actually using it.
    wxWebSessionSync& session = wxWebSessionSync::GetDefault();
    REQUIRE( session.IsOpened() );

    // Request is not initialized yet.
    CHECK( !request.IsOk() );
    CHECK( !request.GetNativeHandle() );

    REQUIRE( Execute("status/200") );
    CHECK( request.IsOk() );
    CHECK( request.GetNativeHandle() );
    CHECK( session.GetNativeHandle() );

    CHECK( response.GetStatus() == 200 );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Get::String", "[net][webrequest][sync][get]")
{
    if ( !InitBaseURL() )
        return;

    REQUIRE( Execute("base64/VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw==") );

    CHECK( response.AsString() == "The quick brown fox jumps over the lazy dog" );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Get::Header", "[net][webrequest][sync][get]")
{
    if ( !InitBaseURL() )
        return;

    REQUIRE( Execute("response-headers?freeform=wxWidgets%20works!") );

    CHECK( response.GetHeader("freeform") == "wxWidgets works!" );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Get::Param", "[net][webrequest][sync][get]")
{
    if ( !InitBaseURL() )
        return;

    REQUIRE( Execute("get?pi=3.14159265358979323") );

    CheckExpectedJSON( response.AsString() );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Get::File", "[net][webrequest][sync][get]")
{
    if ( !InitBaseURL() )
        return;

    wxInt64 expectedFileSize = 99 * 1024;
    Create(wxString::Format("bytes/%lld", expectedFileSize));
    request.SetStorage(wxWebRequest::Storage_File);

    REQUIRE( Execute() );

    CHECK( request.GetBytesReceived() == expectedFileSize );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Get::None", "[net][webrequest][sync][get]")
{
    if ( !InitBaseURL() )
        return;

    int processingSize = 99 * 1024;
    Create(wxString::Format("bytes/%d", processingSize));
    request.SetStorage(wxWebRequest::Storage_None);

    REQUIRE( Execute() );

    CHECK( request.GetBytesReceived() == processingSize );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Error::HTTP", "[net][webrequest][sync][error]")
{
    if ( !InitBaseURL() )
        return;

    CHECK_FALSE( Execute("status/404") );

    CHECK( response.GetStatus() == 404 );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Error::Body", "[net][webrequest][sync][error]")
{
    if ( !InitBaseURL() )
        return;

    CHECK_FALSE( Execute("status/418") );

    CHECK( response.GetStatus() == 418 );
    CHECK_THAT( response.AsString().utf8_string(), Catch::Contains("teapot") );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Error::Connect", "[net][webrequest][sync][error]")
{
    if ( !InitBaseURL() )
        return;

    Create("http://127.0.0.1:51234");
    CHECK( !Execute() );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::SSL::Error", "[net][webrequest][sync][error]")
{
    if ( wxWebSession::GetDefault().GetLibraryVersionInfo().GetName()
            == "URLSession" )
    {
        WARN("Disabling SSL verification is not supported with URLSession backend");
        return;
    }

    wxString url;

    if ( GetBadSSLURL(BadSSLKind::SelfSigned, &url) )
    {
        INFO("Testing self-signed certificate at " << url);

        Create(url);
        CHECK( !Execute() );

        Create(url);
        request.DisablePeerVerify();

        REQUIRE( Execute() );
        CHECK( response.GetStatus() == 200 );
    }

    if ( GetBadSSLURL(BadSSLKind::Expired, &url) )
    {
        INFO("Testing expired certificate at " << url);

        Create(url);
        CHECK( !Execute() );

        Create(url);
        request.MakeInsecure(wxWebRequest::Ignore_Certificate);
        REQUIRE( Execute() );
        CHECK( response.GetStatus() == 200 );
    }

    if ( GetBadSSLURL(BadSSLKind::BadHost, &url) )
    {
        INFO("Testing certificate with bad host at " << url);

        Create(url);
        CHECK( !Execute() );

        Create(url);
        request.MakeInsecure(wxWebRequest::Ignore_Certificate);
        CHECK( !Execute() );

        Create(url);
        request.MakeInsecure(wxWebRequest::Ignore_Host);
        REQUIRE( Execute() );
        CHECK( response.GetStatus() == 200 );
    }
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Post", "[net][webrequest][sync]")
{
    if ( !InitBaseURL() )
        return;

    Create("post");
    request.SetData("app=WebRequestSample&version=1", "application/x-www-form-urlencoded");
    REQUIRE( Execute() );
    CHECK( response.GetStatus() == 200 );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Put", "[net][webrequest][sync]")
{
    if ( !InitBaseURL() )
        return;

    Create("put");
    std::unique_ptr<wxInputStream> is(new wxFileInputStream("horse.png"));
    REQUIRE( is->IsOk() );

    request.SetData(is.release(), "image/png");
    request.SetMethod("PUT");
    REQUIRE( Execute() );
    CHECK( response.GetStatus() == 200 );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Delete", "[net][webrequest][sync]")
{
    if ( !InitBaseURL() )
        return;

    Create("delete");
    request.SetData(R"({"bloordyblop": 17})", "application/json");
    request.SetMethod("DELETE");
    REQUIRE( Execute() );

    CHECK( response.GetStatus() == 200 );
    CHECK_THAT( response.AsString().utf8_string(),
                Catch::Contains(R"("bloordyblop": 17)") );
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Auth::Basic", "[net][webrequest][sync][auth]")
{
    if ( !InitBaseURL() )
        return;

    SECTION("No password")
    {
        Create("basic-auth/wxtest/wxwidgets");
        CHECK_FALSE( Execute() );
        CHECK( state == wxWebRequest::State_Unauthorized );
        CHECK( response.GetStatus() == 401 );
    }

    SECTION("Good password")
    {
        CreateWithAuth("basic-auth/wxtest/wxwidgets", "wxtest", "wxwidgets");
        CHECK( Execute() );
        CHECK( response.GetStatus() == 200 );
        CHECK( state == wxWebRequest::State_Completed );

        CHECK_THAT( response.AsString().utf8_string(),
                    Catch::Contains(AUTHORIZED_SUBSTRING) );
    }

    SECTION("Bad password")
    {
        CreateWithAuth("basic-auth/wxtest/wxwidgets", "wxtest", "foobar");
        CHECK_FALSE( Execute() );
        CHECK( response.GetStatus() == 401 );
        CHECK( state == wxWebRequest::State_Unauthorized );
    }
}

TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Auth::Digest", "[net][webrequest][sync][auth]")
{
    if ( !InitBaseURL() )
        return;

    const auto& versionInfo = wxWebSession::GetDefault().GetLibraryVersionInfo();
    if ( versionInfo.GetName() == "libcurl" && !versionInfo.AtLeast(7, 60) )
    {
        // This test fails under Ubuntu 18.04 which uses libcurl 7.58 with GnuTLS.
        // It's not clear whether it does it because libcurl is too old or
        // because of using GnuTLS instead of OpenSSL used elsewhere, but for
        // now just skip it.
        WARN("Skipping Digest auth test because it's known to fail with old libcurl");
        return;
    }

    SECTION("No password")
    {
        Create("digest-auth/auth/wxtest/wxwidgets");
        CHECK_FALSE( Execute() );
        CHECK( state == wxWebRequest::State_Unauthorized );
        CHECK( response.GetStatus() == 401 );
    }

    SECTION("Good password")
    {
        CreateWithAuth("digest-auth/auth/wxtest/wxwidgets", "wxtest", "wxwidgets");
        CHECK( Execute() );
        CHECK( response.GetStatus() == 200 );
        CHECK( state == wxWebRequest::State_Completed );

        CHECK_THAT( response.AsString().utf8_string(),
                    Catch::Contains(AUTHORIZED_SUBSTRING) );
    }

    SECTION("Bad password")
    {
        CreateWithAuth("digest-auth/auth/wxtest/wxwidgets", "foo", "bar");
        CHECK_FALSE( Execute() );
        CHECK( response.GetStatus() == 401 );
        CHECK( state == wxWebRequest::State_Unauthorized );
    }
}


static void DumpResponse(const wxWebResponse& response)
{
    REQUIRE( response.IsOk() );
    WARN("URL: " << response.GetURL() << "\n" <<
         "Status: " << response.GetStatus()
                    << " (" << response.GetStatusText() << ")\n" <<
         "Body length: " << response.GetContentLength() << "\n" <<
         "Body: " << response.AsString() << "\n");

    // Also show the value of the given header if requested.
    wxString header;
    if ( wxGetEnv("WX_TEST_WEBREQUEST_HEADER", &header) )
    {
        WARN("Header " << header << ": " << response.GetHeader(header));
    }
}

// This test is not run by default and has to be explicitly selected to run.
TEST_CASE_METHOD(RequestFixture,
                 "WebRequest::Manual", "[.]")
{
    InitManualRequest();

    request.Start();
    RunLoopWithTimeout();

    INFO("Error: \"" << errorDescription << "\"");
    CHECK( request.GetState() == wxWebRequest::State_Completed );

    DumpResponse(request.GetResponse());
}

// A sync version of the pseudo-test above.
TEST_CASE_METHOD(SyncRequestFixture,
                 "WebRequest::Sync::Manual", "[.]")
{
    InitManualRequest();

    if ( !Execute() )
    {
        FAIL_CHECK("Error: \"" << error << "\"");
    }

    DumpResponse(request.GetResponse());
}

using wxWebRequestHeaderMap = std::unordered_map<wxString, wxString>;

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

// This is not a real test, run it to see the version of the library used.
TEST_CASE("WebRequest::Version", "[.]")
{
    const auto& info = wxWebSession::GetDefault().GetLibraryVersionInfo();
    WARN("Using " << info.GetName() << " backend (" << info.ToString() << ")");
}

#endif // wxUSE_WEBREQUEST
