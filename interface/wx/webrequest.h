/////////////////////////////////////////////////////////////////////////////
// Name:        webrequest.h
// Created:     2018-10-14
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWebRequest

    This class allows for simple HTTP requests using the operating systems
    components as implementation.

    The latest features of the operating system will be used if available
    (e.g. HTTP/2, TLS 1.3).
    System wide configuration like proxy and SSL certificates will be used
    when possible.

    Instances of wxWebRequest are created by using
    wxWebSession::CreateRequest().

    The requests are handled asynchronously and event handlers are used to
    communicate the request status.

    Example usage:

    @code
        // Create the request object
        wxObjectDataPtr<wxWebRequest> request(
            wxWebSession::GetDefault().CreateRequest("https://www.wxwidgets.org/downloads/logos/blocks.png"));

        // Bind events
        request->Bind(wxEVT_WEBREQUEST_READY, [](wxWebRequestEvent& evt) {
            wxImage logoImage(evt->GetResponse()->GetStream());
            if (logoImage.IsOK())
                wxLogInfo("Image loaded");
        });
        request->Bind(wxEVT_WEBREQUEST_FAILED, [](wxWebRequestEvent& evt) {
            wxLogError("Could not load logo: %s", evt.GetErrorDescription());
        });

        // Start the request
        request->Start();
    @endcode

    @section descriptions Implementation Descriptions

    The following APIs are used per platform, additional documentation
    about supported features may be found in their documentation

    Available features by implementation and minimum version:
    <table>
    <tr><th>Operating System</th><th>API</th><th>HTTPS</th><th>HTTP/2</th></tr>
    <tr>
        <td>Windows</td>
        <td>
            <a target=_new href="https://docs.microsoft.com/en-us/windows/desktop/WinHttp/about-winhttp">WinHTTP</a>
        </td>
        <td>Yes</td>
        <td>Windows 10 1607</td>
    </tr>
    <tr>
        <td>macOS</td>
        <td>
            <a target=_new href="https://developer.apple.com/documentation/foundation/urlsession">NSURLSession</a>
        </td>
        <td>macOS 10.9</td>
        <td>macOS 10.11</td>
    </tr>
    <tr>
        <td>iOS</td>
        <td>
            <a target=_new href="https://developer.apple.com/documentation/foundation/urlsession">NSURLSession</a>
        </td>
        <td>iOS 7.0</td>
        <td>iOS 9.0</td>
    </tr>
    <tr>
        <td>Linux</td>
        <td>
            <a target=_new href="https://curl.haxx.se/libcurl/">libcurl</a>
        </td>
        <td>Yes</td>
        <td>7.47.0</td>
    </tr>
    </table>

    @beginEventEmissionTable{wxWebRequestEvent}
    @event{wxEVT_WEBREQUEST_READY(id, func)}
        The response data is ready to be used.
    @event{wxEVT_WEBREQUEST_FAILED(id, func)}
        A network error has occured. This could be client side or server side.
        Use wxWebRequestEvent::GetErrorDescription() to get more details.
    @event{wxEVT_WEBREQUEST_AUTH_REQUIRED(id, func)}
        The request needs additional authentication to continue.
    @endEventTable

    @since 3.1.2

    @library{wxnet}
    @category{net}

    @see wxWebResponse, wxWebSession
*/
class wxWebRequest: public wxEvtHandler, public wxRefCounter
{
public:
    /**
        Sets a request header send by this request.

        @param name Name of the header
        @param value String value of the header
    */
    void SetHeader(const wxString& name, const wxString& value);

    /**
        Set <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec9.html">common</a>
        or expanded HTTP method.

        The default method is GET unless request data is provided in which
        case POST is the default.

        @param method
            HTTP method name, e.g. "GET".
    */
    void SetMethod(const wxString& method);

    /**
        Set the text to be posted to the server.

        After a successful call to this method, the request will use HTTP @c
        POST instead of the default @c GET when it's executed.

        @param text
            The text data to post.
        @param contentType
            The value of HTTP "Content-Type" header, e.g. "text/html;
            charset=UTF-8".
    */
    void SetData(const wxString& text, const wxString& contentType);

    /**
        Set the binary data to be posted to the server.

        The next request will be a HTTP @c POST instead of the default HTTP
        @c GET and the given @a dataStream will be posted as the body of
        this request.

        @param dataStream
            The data in this stream will be posted as the request body
        @param contentType
            The value of HTTP "Content-Type" header, e.g.
            "application/octet-stream".
    */
    void SetData(const wxInputStream& dataStream, const wxString& contentType);

    /**
        Instructs the request to ignore server error status codes.

        Per default, server side errors (status code 400-599) will send
        a wxEVT_WEBREQUEST_FAILED event just like network errors, but
        if the response is still required in this cases (e.g. to get more
        details from the response body), set this option to ignore all errors.
        If ignored, wxWebResponse::GetStatus() has to be checked
        from the wxEVT_WEBREQUEST_READY event handler.
    */
    void SetIgnoreServerErrorStatus(bool ignore);

    /**
        Send the request to the server asynchronously.

        Events will be triggered on success or failure.

        @see Cancel()
    */
    void Start();

    /**
        Cancel an active request.
    */
    void Cancel();

    /**
        Returns a response object after a successful request.

        Before sending a request or after a failed request this will return
        @c NULL.
    */
    const wxWebResponse* GetResponse() const;
};

/**
    A wxWebResponse allows access to the response sent by the server.

    @since 3.1.2

    @library{wxnet}
    @category{net}

    @see wxWebRequest
*/
class wxWebResponse
{
public:
    /**
        Returns the final URL.
        This URL might be different than the request URL when a redirection
        occurred.
    */
    wxString GetURL() const;

    /**
        Returns a header from the response or an empty string if the header
        could not be found.

        @param name Name of the header field
    */
    wxString GetHeader(const wxString& name) const;

    /**
        Returns the status code returned by the server.
    */
    int GetStatus() const;

    /**
        Returns the status text of the response.
    */
    wxString GetStatusText() const;

    /**
        Returns a stream which represents the response data sent by the server.
    */
    wxInputStream& GetStream() const;

    /**
        Returns all response data as a string.

        @param conv wxMBConv used to convert the response to a string.
                    If @c NULL, the conversion will be determined by
                    response headers. The default is UTF-8.
    */
    wxString AsString(wxMBConv* conv = NULL) const;
};

/**
    @class wxWebSession

    This class handles session-wide parameters and data used by wxWebRequest
    instances.

    Usually the default session available via wxWebSession::GetDefault()
    should be used. Additional instances might be useful if session separation
    is required. Instances <b>must not</b> be deleted before every active web
    request has finished.

    Every wxWebRequest sharing the same session object will use the same
    cookies. Additionally, an underlying network connection might be kept
    alive to achieve faster additional responses.

    @since 3.1.2

    @library{wxnet}
    @category{net}

    @see wxWebRequest
*/
class wxWebSession
{
public:
    /**
        Constructor for the session

        All requests created by a call to CreateRequest() will use this session
        for communication and to store cookies.
    */
    wxWebSession();

    /**
        Create a new request for the specified URL

        @param url
            The URL of the HTTP resource for this request
        @param id
            Optional id sent with events
    */
    wxWebRequest* CreateRequest(const wxString& url, int id = wxID_ANY);


    /**
        Returns the default session
    */
    static wxWebSession& GetDefault();

    /**
        Sets a request header in every wxWebRequest created from this session
        after is has been set.

        A good example for a session-wide request header is the @c User-Agent
        header.

        @param name Name of the header
        @param value String value of the header
    */
    void SetHeader(const wxString& name, const wxString& value);
};

/**
    @class wxWebRequestEvent

    A web request event sent during or after server communication.

    @since 3.1.2

    @library{wxnet}
    @category{net}

    @see wxWebRequest
*/
class wxWebRequestEvent : public wxEvent
{
public:
    wxWebRequestEvent();
    wxWebRequestEvent(wxEventType type, int id);

    /**
        The response for a wxEVT_WEBREQUEST_READY event or @c NULL for other
        events.
    */
    const wxWebResponse* GetResponse() const;

    /**
        A textual error description for a client side error
        in case of wxEVT_WEBREQUEST_FAILED
    */
    const wxString& GetErrorDescription() const;
};

wxEventType wxEVT_WEBREQUEST_READY;
wxEventType wxEVT_WEBREQUEST_FAILED;
wxEventType wxEVT_WEBREQUEST_AUTH_REQUIRED;
