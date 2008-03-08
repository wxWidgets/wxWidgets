/////////////////////////////////////////////////////////////////////////////
// Name:        url.h
// Purpose:     documentation for wxURL class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxURL
    @wxheader{url.h}

    wxURL is a specialization of wxURI for parsing URLs.
    Please look at wxURI documentation for more info about the functions
    you can use to retrieve the various parts of the URL (scheme, server, port,
    etc).

    Supports standard assignment operators, copy constructors,
    and comparison operators.

    @library{wxnet}
    @category{net}

    @seealso
    wxSocketBase, wxProtocol
*/
class wxURL : public wxURI
{
public:
    /**
        Constructs a URL object from the string.  The URL must be valid according
        to RFC 1738.  In particular, file URLs must be of the format
        @c file://hostname/path/to/file otherwise GetError()
        will return a value different from @c wxURL_NOERR.
        
        It is valid to leave out the hostname but slashes must remain in place -
        i.e. a file URL without a hostname must contain three consecutive slashes
        (e.g. @c file:///somepath/myfile).
        
        @param url
        Url string to parse.
    */
#define wxURL(const wxString& url = wxEmptyString)     /* implementation is private */

    /**
        Destroys the URL object.
    */
#define ~wxURL()     /* implementation is private */

    /**
        Returns the last error. This error refers to the URL parsing or to the protocol.
        It can be one of these errors:
        
        
        @b wxURL_NOERR
        
        
        No error.
        
        @b wxURL_SNTXERR
        
        
        Syntax error in the URL string.
        
        @b wxURL_NOPROTO
        
        
        Found no protocol which can get this URL.
        
        @b wxURL_NOHOST
        
        
        A host name is required for this protocol.
        
        @b wxURL_NOPATH
        
        
        A path is required for this protocol.
        
        @b wxURL_CONNERR
        
        
        Connection error.
        
        @b wxURL_PROTOERR
        
        
        An error occurred during negotiation.
    */
    wxURLError GetError();

    /**
        Creates a new input stream on the specified URL. You can use all but seek
        functionality of wxStream. Seek isn't available on all streams. For example,
        HTTP or FTP streams don't deal with it.
        
        Note that this method is somewhat deprecated, all future wxWidgets applications
        should really use wxFileSystem instead.
        
        Example:
        
        @returns Returns the initialized stream. You will have to delete it
                   yourself.
        
        @sa wxInputStream
    */
    wxInputStream * GetInputStream();

    /**
        Returns a reference to the protocol which will be used to get the URL.
    */
    wxProtocol GetProtocol();

    /**
        Returns @true if this object is correctly initialized, i.e. if
        GetError() returns @c wxURL_NOERR.
    */
#define bool IsOk()     /* implementation is private */

    /**
        Sets the default proxy server to use to get the URL. The string specifies
        the proxy like this: hostname:port number.
        
        @param url_proxy
        Specifies the proxy to use
        
        @sa SetProxy()
    */
    static void SetDefaultProxy(const wxString& url_proxy);

    /**
        Sets the proxy to use for this URL.
        
        @sa SetDefaultProxy()
    */
    void SetProxy(const wxString& url_proxy);

    /**
        Initializes this object with the given URL and returns @c wxURL_NOERR
        if it's valid (see GetError() for more info).
    */
#define wxURLError SetURL(const wxString& url)     /* implementation is private */
};
