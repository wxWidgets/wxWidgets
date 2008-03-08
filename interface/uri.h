/////////////////////////////////////////////////////////////////////////////
// Name:        uri.h
// Purpose:     documentation for wxURI class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxURI
    @wxheader{uri.h}
    
    wxURI is used to extract information from
    a URI (Uniform Resource Identifier).
    
    For information about URIs, see 
    RFC 3986.
    
    In short, a URL is a URI.  In other
    words, URL is a subset of a URI - all 
    acceptable URLs are also acceptable URIs.
    
    wxURI automatically escapes invalid characters in a string,
    so there is no chance of wxURI "failing" on construction/creation.
    
    wxURI supports copy construction and standard assignment
    operators.  wxURI can also be inherited from to provide
    furthur functionality.
    
    @library{wxbase}
    @category{data}
    
    @seealso
    wxURL
*/
class wxURI : public wxObject
{
public:
    //@{
    /**
        Copies this URI from another URI.
        
        @param uri 
        URI (Uniform Resource Identifier) to initialize with
    */
    wxURI();
        wxURI(const wxChar* uri);
        wxURI(const wxURI& uri);
    //@}

    /**
        Builds the URI from its individual components and adds proper separators.
        
        If the URI is not a reference or is not resolved, 
        the URI that is returned from Get is the same one 
        passed to Create.
    */
    wxString BuildURI();

    /**
        Builds the URI from its individual components, adds proper separators, and
        returns escape sequences to normal characters.
        
        Note that it is preferred to call this over Unescape(BuildURI()) since
        BuildUnescapedURI() performs some optimizations over the plain method.
    */
    wxString BuildUnescapedURI();

    /**
        Creates this URI from the string 
        
        @param uri.
        
        Returns the position at which parsing stopped (there 
        is no such thing as an "invalid" wxURI).
        
        uri 
        string to initialize from
    */
    const wxChar* Create(const wxString uri);

    /**
        Note that on URIs with a "file" scheme wxURI does not
        parse the userinfo, server, or port portion.  This is to keep 
        compatability with wxFileSystem, the old wxURL, and older url specifications.
    */


    /**
        Obtains the fragment of this URI.
        
        The fragment of a URI is the last value of the URI,
        and is the value after a '' character after the path 
        of the URI.
        
        @c http://mysite.com/mypath#fragment
    */
    const wxString GetFragment();

    /**
        Obtains the host type of this URI, which is of type
        HostType():
        
        
        @b wxURI_REGNAME
        
        
        Server is a host name, or the Server component itself is undefined.
        
        @b wxURI_IPV4ADDRESS
        
        
        Server is a IP version 4 address (XXX.XXX.XXX.XXX)
        
        @b wxURI_IPV6ADDRESS
        
        
        Server is a IP version 6 address ((XXX:)XXX::(XXX)XXX:XXX
        
        @b wxURI_IPVFUTURE
        
        
        Server is an IP address, but not versions 4 or 6
    */
    const HostType GetHostType();

    /**
        Returns the password part of the userinfo component of
        this URI.  Note that this is explicitly depreciated by
        RFC 1396 and should generally be avoided if possible.
        
        @c http://user:password@mysite.com/mypath
    */
    const wxString GetPassword();

    /**
        Returns the (normalized) path of the URI.
        
        The path component of a URI comes
        directly after the scheme component
        if followed by zero or one slashes ('/'),
        or after the server/port component.
        
        Absolute paths include the leading '/'
        character.
        
        @c http://mysite.compath
    */
    const wxString GetPath();

    /**
        Returns a string representation of the URI's port.
        
        The Port of a URI is a value after the server, and 
        must come after a colon (:).
        
        @c http://mysite.com:port
        
        Note that you can easily get the numeric value of the port
        by using wxAtoi or wxString::Format.
    */
    const wxString GetPort();

    /**
        Returns the Query component of the URI.
        
        The query component is what is commonly passed to a 
        cgi application, and must come after the path component,
        and after a '?' character.
        
        @c http://mysite.com/mypath?query
    */
    const wxString GetQuery();

    /**
        Returns the Scheme component of the URI.
        
        The first part of the uri.
        
        @c scheme://mysite.com
    */
    const wxString GetScheme();

    /**
        Returns the Server component of the URI.
        
        The server of the uri can be a server name or 
        a type of ip address.  See
        GetHostType() for the
        possible values for the host type of the 
        server component.
        
        @c http://server/mypath
    */
    const wxString GetServer();

    /**
        Returns the username part of the userinfo component of
        this URI.  Note that this is explicitly depreciated by
        RFC 1396 and should generally be avoided if possible.
        
        @c http://user:password@mysite.com/mypath
    */
    const wxString GetUser();

    /**
        Returns the UserInfo component of the URI.
        
        The component of a URI before the server component
        that is postfixed by a '@' character.
        
        @c http://userinfo@mysite.com/mypath
    */
    const wxString GetUserInfo();

    /**
        Returns @true if the Fragment component of the URI exists.
    */
    bool HasFragment();

    /**
        Returns @true if the Path component of the URI exists.
    */
    bool HasPath();

    /**
        Returns @true if the Port component of the URI exists.
    */
    bool HasPort();

    /**
        Returns @true if the Query component of the URI exists.
    */
    bool HasQuery();

    /**
        Returns @true if the Scheme component of the URI exists.
    */
    bool HasScheme();

    /**
        Returns @true if the Server component of the URI exists.
    */
    bool HasServer();

    /**
        Returns @true if the User component of the URI exists.
    */
    bool HasUser();

    /**
        Returns @true if a valid [absolute] URI, otherwise this URI
        is a URI reference and not a full URI, and IsReference
        returns @false.
    */
    bool IsReference();

    /**
        To obtain individual components you can use 
        one of the following methods
        
        GetScheme()
        
        GetUserInfo()
        
        GetServer()
        
        GetPort()
        
        GetPath()
        
        GetQuery()
        
        GetFragment()
        
        However, you should check HasXXX before
        calling a get method, which determines whether or not the component referred
        to by the method is defined according to RFC 2396.
        
        Consider an undefined component equivalent to a 
        @NULL C string.
        
         
        HasScheme()
        
        HasUserInfo()
        
        HasServer()
        
        @ref hasserver() HasPort
        
        HasPath()
        
        HasQuery()
        
        HasFragment()
        
        Example:
    */


    /**
        Inherits this URI from a base URI - components that do not
        exist in this URI are copied from the base, and if this URI's
        path is not an absolute path (prefixed by a '/'), then this URI's
        path is merged with the base's path.
        
        For instance, resolving "../mydir" from "http://mysite.com/john/doe" 
        results in the scheme (http) and server (mysite.com) being copied into 
        this URI, since they do not exist.  In addition, since the path
        of this URI is not absolute (does not begin with '/'), the path
        of the base's is merged with this URI's path, resulting in the URI
        "http://mysite.com/john/mydir".
        
        @param base 
        Base URI to inherit from.  Must be a full URI and not a reference
        
        @param flags 
        Currently either wxURI_STRICT or 0, in non-strict
        mode some compatibility layers are enabled to allow loopholes from RFCs prior
        to 2396
    */
    void Resolve(const wxURI& base, int flags = wxURI_STRICT);

    /**
        Translates all escape sequences (normal characters and returns the result.
        
        This is the preferred over deprecated wxURL::ConvertFromURI.
        
        If you want to unescape an entire wxURI, use BuildUnescapedURI() instead,
        as it performs some optimizations over this method.
        
        @param uri 
        string with escaped characters to convert
    */
    wxString Unescape(const wxString& uri);

    /**
        Compares this URI to another URI, and returns @true if 
        this URI equals 
        
        @param uricomp, otherwise it returns @false.
        
        uricomp 
        URI to compare to
    */
    void operator ==(const wxURI& uricomp);
};
