/////////////////////////////////////////////////////////////////////////////
// Name:        uri.cpp
// Purpose:     Implementation of a uri parser
// Author:      Ryan Norton
// Created:     10/26/04
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Ryan Norton
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "uri.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/uri.h"

// ---------------------------------------------------------------------------
// definitions
// ---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxURI, wxObject);

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// utilities
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//
//                        wxURI
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Constructors
// ---------------------------------------------------------------------------

wxURI::wxURI() : m_hostType(wxURI_REGNAME), m_fields(0)
{
}
 
wxURI::wxURI(const wxString& uri) : m_hostType(wxURI_REGNAME), m_fields(0)
{
    Create(uri);
}

wxURI::wxURI(const wxURI& uri)  : m_hostType(wxURI_REGNAME), m_fields(0)
{
    Assign(uri);
}

// ---------------------------------------------------------------------------
// Destructor and cleanup
// ---------------------------------------------------------------------------

wxURI::~wxURI()
{
    Clear();
}

void wxURI::Clear()
{
    m_scheme = m_user = m_server = m_port = m_path =
    m_query = m_fragment = wxT("");

    m_hostType = wxURI_REGNAME;

    m_fields = 0;
}

// ---------------------------------------------------------------------------
// Create
//
// This creates the URI - all we do here is call the main parsing method 
// ---------------------------------------------------------------------------

const wxChar* wxURI::Create(const wxString& uri)
{   
    if (m_fields)
        Clear();

    return Parse(uri);    
} 

// ---------------------------------------------------------------------------
// Escape Methods
//
// TranslateEscape unencodes a 3 character URL escape sequence 
//
// Escape encodes an invalid URI character into a 3 character sequence
//
// IsEscape determines if the input string contains an escape sequence,
// if it does, then it moves the input string past the escape sequence
//
// Unescape unencodes all 3 character URL escape sequences in a wxString
// ---------------------------------------------------------------------------

wxChar wxURI::TranslateEscape(const wxChar* s)
{
    wxASSERT_MSG(IsHex(*s) && IsHex(*(s+1)), wxT("Invalid escape!"));

    return CharToHex(*s) * 0x10 + CharToHex(*++s);
}

wxString wxURI::Unescape(const wxString& uri)
{
    wxString new_uri;

    for(size_t i = 0; i < uri.length(); ++i)
    {
        if (uri[i] == wxT('%'))
        {
            new_uri += wxURI::TranslateEscape( &(uri.c_str()[i+1]) );
            i += 2;
        }
    }

    return new_uri;
}

void wxURI::Escape(wxString& s, const wxChar& c)
{
    const wxChar* hdig = wxT("0123456789abcdef");
    s += wxT('%');
    s += hdig[(c >> 4) & 15];
	s += hdig[c & 15];
}

bool wxURI::IsEscape(const wxChar*& uri)
{
    // pct-encoded   = "%" HEXDIG HEXDIG
    if(*uri == wxT('%') && IsHex(*(uri+1)) && IsHex(*(uri+2)))
    {
        uri += 3;
        return true;
    }
    else
        return false;
}

// ---------------------------------------------------------------------------
// BuildURI
//
// BuildURI() builds the entire URI into a useable 
// representation, including proper identification characters such as slashes
//
// BuildUnescapedURI() does the same thing as BuildURI(), only it unescapes
// the components that accept escape sequences
// ---------------------------------------------------------------------------

wxString wxURI::BuildURI() const
{   
    wxString ret;

    if (HasScheme())
        ret = ret + m_scheme + wxT(":");

    if (HasServer())
    {
        ret += wxT("//");

        if (HasUser())
            ret = ret + m_user + wxT("@");

        ret += m_server;

        if (HasPort())
            ret = ret + wxT(":") + m_port;
    }

    ret += m_path;

    if (HasQuery())
        ret = ret + wxT("?") + m_query;

    if (HasFragment())
        ret = ret + wxT("#") + m_fragment;

    return ret;
}

wxString wxURI::BuildUnescapedURI() const
{
    wxString ret;

    if (HasScheme())
        ret = ret + m_scheme + wxT(":");

    if (HasServer())
    {
        ret += wxT("//");

        if (HasUser())
            ret = ret + wxURI::Unescape(m_user) + wxT("@");

        if (m_hostType == wxURI_REGNAME)
            ret += wxURI::Unescape(m_server);
        else
            ret += m_server;

        if (HasPort())
            ret = ret + wxT(":") + m_port;
    }

    ret += wxURI::Unescape(m_path);

    if (HasQuery())
        ret = ret + wxT("?") + wxURI::Unescape(m_query);

    if (HasFragment())
        ret = ret + wxT("#") + wxURI::Unescape(m_fragment);

    return ret;
}

// ---------------------------------------------------------------------------
// Assignment
// ---------------------------------------------------------------------------

wxURI& wxURI::Assign(const wxURI& uri)
{
    //assign fields
    m_fields = uri.m_fields;

    //ref over components
    m_scheme = uri.m_scheme;
    m_user = uri.m_user;
    m_server = uri.m_server;
    m_hostType = uri.m_hostType;
    m_port = uri.m_port;
    m_path = uri.m_path;
    m_query = uri.m_query;
    m_fragment = uri.m_fragment;

    return *this;
}

wxURI& wxURI::operator = (const wxURI& uri)
{
    return Assign(uri);
}

wxURI& wxURI::operator = (const wxString& string)
{   
    Create(string);
    return *this;
}

// ---------------------------------------------------------------------------
// Comparison
// ---------------------------------------------------------------------------

bool wxURI::operator == (const wxURI& uri) const
{    
    if (HasScheme())
    {
        if(m_scheme != uri.m_scheme)
            return false;
    }
    else if (uri.HasScheme())
        return false;


    if (HasServer())
    {
        if (HasUser())
        {
            if (m_user != uri.m_user)
                return false;
        }
        else if (uri.HasUser())
            return false;

        if (m_server != uri.m_server ||
            m_hostType != uri.m_hostType)
            return false;

        if (HasPort())
        {
            if(m_port != uri.m_port)
                return false;
        }
        else if (uri.HasPort())
            return false;
    }
    else if (uri.HasServer())
        return false;


    if (HasPath())
    {
        if(m_path != uri.m_path)
            return false;
    }
    else if (uri.HasPath())
        return false;

    if (HasQuery())
    {
        if (m_query != uri.m_query)
            return false;
    }
    else if (uri.HasQuery())
        return false;

    if (HasFragment())
    {
        if (m_fragment != uri.m_fragment)
            return false;
    }
    else if (uri.HasFragment())
        return false;

    return true;
}

// ---------------------------------------------------------------------------
// IsReference
//
// if there is no authority or scheme, it is a reference
// ---------------------------------------------------------------------------

bool wxURI::IsReference() const
{   return !HasScheme() || !HasServer();  }

// ---------------------------------------------------------------------------
// Parse
//
// Master URI parsing method.  Just calls the individual parsing methods
//
// URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
// URI-reference = URI / relative-URITestCase
// ---------------------------------------------------------------------------

const wxChar* wxURI::Parse(const wxChar* uri)
{
    uri = ParseScheme(uri);
    uri = ParseAuthority(uri);
    uri = ParsePath(uri);
    uri = ParseQuery(uri);
    return ParseFragment(uri);
}

// ---------------------------------------------------------------------------
// ParseXXX
//
// Individual parsers for each URI component
// ---------------------------------------------------------------------------

const wxChar* wxURI::ParseScheme(const wxChar* uri)
{
    wxASSERT(uri != NULL);

    //copy of the uri - used for figuring out
    //length of each component
    const wxChar* uricopy = uri;

    //Does the uri have a scheme (first character alpha)?
    if (IsAlpha(*uri))
    {
        m_scheme += *uri++;

        //scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        while (IsAlpha(*uri) || IsDigit(*uri) || 
               *uri == wxT('+')   ||
               *uri == wxT('-')   ||
               *uri == wxT('.')) 
        { 
            m_scheme += *uri++; 
        }

        //valid scheme?
        if (*uri == wxT(':'))
        { 
            //mark the scheme as valid
            m_fields |= wxURI_SCHEME;

            //move reference point up to input buffer
            uricopy = ++uri;
        }
        else 
            //relative uri with relative path reference
            m_scheme = wxT("");
    }
//    else 
        //relative uri with _possible_ relative path reference

    return uricopy;
}

const wxChar* wxURI::ParseAuthority(const wxChar* uri)
{
    // authority     = [ userinfo "@" ] host [ ":" port ]
    if (*uri == wxT('/') && *(uri+1) == wxT('/')) 
    {
        uri += 2;

        uri = ParseUser(uri);
        uri = ParseServer(uri);
        return ParsePort(uri);
    }

    return uri;
}

const wxChar* wxURI::ParseUser(const wxChar* uri)
{
    wxASSERT(uri != NULL);

    //copy of the uri - used for figuring out
    //length of each component
    const wxChar* uricopy = uri;

    // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    while(*uri && *uri != wxT('@') && *uri != wxT('/') && *uri != wxT('#') && *uri != wxT('?')) 
    {
        if(IsUnreserved(*uri) || IsEscape(uri) || 
           IsSubDelim(*uri) || *uri == wxT(':'))
            m_user += *uri++;
        else
            Escape(m_user, *uri++);
    }

    if(*uri == wxT('@'))
    {
        //valid userinfo
        m_fields |= wxURI_USER;

        uricopy = ++uri;
    }
    else
        m_user = wxT("");

    return uricopy;
}

const wxChar* wxURI::ParseServer(const wxChar* uri)
{
    wxASSERT(uri != NULL);

    //copy of the uri - used for figuring out
    //length of each component
    const wxChar* uricopy = uri;

    // host          = IP-literal / IPv4address / reg-name
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    if (*uri == wxT('['))
    {
        if (ParseIPv6address(++uri) && *uri == wxT(']'))
        {
            ++uri;
            m_hostType = wxURI_IPV6ADDRESS;
           
            wxStringBufferLength theBuffer(m_server, uri - uricopy);
            wxMemcpy(theBuffer, uricopy, uri-uricopy);
            theBuffer.SetLength(uri-uricopy);
        }
        else
        {
            uri = uricopy;

            if (ParseIPvFuture(++uri) && *uri == wxT(']'))
            {
                ++uri;
                m_hostType = wxURI_IPVFUTURE; 
           
                wxStringBufferLength theBuffer(m_server, uri - uricopy);
                wxMemcpy(theBuffer, uricopy, uri-uricopy);
                theBuffer.SetLength(uri-uricopy);
            }
            else 
                uri = uricopy;
        }
    }
    else 
    {
        if (ParseIPv4address(uri))
        {
            m_hostType = wxURI_IPV4ADDRESS;

            wxStringBufferLength theBuffer(m_server, uri - uricopy);
            wxMemcpy(theBuffer, uricopy, uri-uricopy);
            theBuffer.SetLength(uri-uricopy);
        }
        else 
            uri = uricopy;
    }

    if(m_hostType == wxURI_REGNAME)
    {
        uri = uricopy;
        // reg-name      = *( unreserved / pct-encoded / sub-delims )
        while(*uri && *uri != wxT('/') && *uri != wxT(':') && *uri != wxT('#') && *uri != wxT('?')) 
        {
            if(IsUnreserved(*uri) || IsEscape(uri) ||  IsSubDelim(*uri))
                m_server += *uri++;
            else
                Escape(m_server, *uri++);
        }                
    }

    //mark the server as valid
    m_fields |= wxURI_SERVER;

    return uri;
}

 
const wxChar* wxURI::ParsePort(const wxChar* uri)
{
    wxASSERT(uri != NULL);

    // port          = *DIGIT
    if(*uri == wxT(':'))
    {
        ++uri;
        while(IsDigit(*uri)) 
        {
            m_port += *uri++;
        }                

        //mark the port as valid
        m_fields |= wxURI_PORT;
    }

    return uri;
}

const wxChar* wxURI::ParsePath(const wxChar* uri, bool bReference, bool bNormalize)
{
    wxASSERT(uri != NULL);

    //copy of the uri - used for figuring out
    //length of each component
    const wxChar* uricopy = uri;

    /// hier-part     = "//" authority path-abempty
    ///               / path-absolute
    ///               / path-rootless
    ///               / path-empty
    ///
    /// relative-part = "//" authority path-abempty
    ///               / path-absolute
    ///               / path-noscheme
    ///               / path-empty
    ///
    /// path-abempty  = *( "/" segment )
    /// path-absolute = "/" [ segment-nz *( "/" segment ) ]
    /// path-noscheme = segment-nz-nc *( "/" segment )
    /// path-rootless = segment-nz *( "/" segment )
    /// path-empty    = 0<pchar>
    ///
    /// segment       = *pchar
    /// segment-nz    = 1*pchar
    /// segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    ///               ; non-zero-length segment without any colon ":"
    ///
    /// pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    if (*uri == wxT('/'))
    {
        m_path += *uri++;

        while(*uri && *uri != wxT('#') && *uri != wxT('?')) 
        { 
            if( IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                *uri == wxT(':') || *uri == wxT('@') || *uri == wxT('/'))
                m_path += *uri++; 
            else    
                Escape(m_path, *uri++);    
        }

        if (bNormalize)
        {
            wxStringBufferLength theBuffer(m_path, m_path.length() + 1);
            Normalize(theBuffer, true);
            theBuffer.SetLength(wxStrlen(theBuffer));
        }
        //mark the path as valid
        m_fields |= wxURI_PATH;
    }
    else if(*uri) //Relative path
    {
        if (bReference)
        {
            //no colon allowed
            while(*uri && *uri != wxT('#') && *uri != wxT('?')) 
            {
                if(IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                  *uri == wxT('@') || *uri == wxT('/'))
                    m_path += *uri++; 
                else    
                    Escape(m_path, *uri++);    
            }
        } 
        else
        {
            while(*uri && *uri != wxT('#') && *uri != wxT('?')) 
            {
                if(IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                   *uri == wxT(':') || *uri == wxT('@') || *uri == wxT('/'))
                    m_path += *uri++; 
                else    
                    Escape(m_path, *uri++);    
            }
        }

        if (uri != uricopy)
        {         
            if (bNormalize)
            {
                wxStringBufferLength theBuffer(m_path, m_path.length() + 1);
                Normalize(theBuffer);
                theBuffer.SetLength(wxStrlen(theBuffer));
            }

            //mark the path as valid
            m_fields |= wxURI_PATH;
        }
    }

    return uri;
}


const wxChar* wxURI::ParseQuery(const wxChar* uri)
{
    wxASSERT(uri != NULL);

    // query         = *( pchar / "/" / "?" )
    if (*uri == wxT('?'))
    {
        ++uri;
        while(*uri && *uri != wxT('#'))
        {
            if (IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                *uri == wxT(':') || *uri == wxT('@') || *uri == wxT('/') || *uri == wxT('?'))
                  m_query += *uri++;  
            else
                  Escape(m_query, *uri++); 
        }

        //mark the server as valid
        m_fields |= wxURI_QUERY;
    }

    return uri;
}


const wxChar* wxURI::ParseFragment(const wxChar* uri)
{
    wxASSERT(uri != NULL);

    // fragment      = *( pchar / "/" / "?" )
    if (*uri == wxT('#'))
    {
        ++uri;
        while(*uri)
        {
            if (IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                *uri == wxT(':') || *uri == wxT('@') || *uri == wxT('/') || *uri == wxT('?'))
                  m_fragment += *uri++;  
            else
                  Escape(m_fragment, *uri++); 
        }

        //mark the server as valid
        m_fields |= wxURI_FRAGMENT;
    }

    return uri;
}

// ---------------------------------------------------------------------------
// Resolve
//
// Builds missing components of this uri from a base uri
//
// A version of the algorithm outlined in the RFC is used here
// (it is shown in comments)
//
// Note that an empty URI inherits all components 
// ---------------------------------------------------------------------------

void wxURI::Resolve(const wxURI& base, int flags)
{
    wxASSERT_MSG(!base.IsReference(), 
                wxT("wxURI to inherit from must not be a reference!"));

    // If we arn't being strict, enable the older (pre-RFC2396)
    // loophole that allows this uri to inherit other
    // properties from the base uri - even if the scheme
    // is defined
    if ( !(flags & wxURI_STRICT) &&
            HasScheme() && base.HasScheme() &&
                m_scheme == base.m_scheme )
    {   
        m_fields -= wxURI_SCHEME;
    }


    // Do nothing if this is an absolute wxURI
    //    if defined(R.scheme) then
    //       T.scheme    = R.scheme;
    //       T.authority = R.authority;
    //       T.path      = remove_dot_segments(R.path);
    //       T.query     = R.query;
    if (HasScheme())
    {
        return;
    }

    //No sheme - inherit
    m_scheme = base.m_scheme;
    m_fields |= wxURI_SCHEME;

    // All we need to do for relative URIs with an
    // authority component is just inherit the scheme
    //       if defined(R.authority) then
    //          T.authority = R.authority;
    //          T.path      = remove_dot_segments(R.path);
    //          T.query     = R.query;
    if (HasServer())
    {
        return;
    }

    //No authority - inherit
    if (base.HasUser())
    {
        m_user = base.m_user;
        m_fields |= wxURI_USER;
    }
    
    m_server = base.m_server;
    m_hostType = base.m_hostType;
    m_fields |= wxURI_SERVER;
    
    if (base.HasPort())
    {
        m_port = base.m_port;
        m_fields |= wxURI_PORT;
    }
    

    // Simple path inheritance from base
    if (!HasPath())
    {
        //             T.path = Base.path;
        m_path = base.m_path;
        m_fields |= wxURI_PATH;
        

        //             if defined(R.query) then
        //                T.query = R.query;
        //             else
        //                T.query = Base.query;
        //             endif;
        if (!HasQuery())
        {
            m_query = base.m_query;
            m_fields |= wxURI_QUERY;
        }
    }
    else
    {
        //             if (R.path starts-with "/") then
        //                T.path = remove_dot_segments(R.path);
        //             else
        //                T.path = merge(Base.path, R.path);
        //                T.path = remove_dot_segments(T.path);
        //             endif;
        //             T.query = R.query;
        if (m_path[0u] != wxT('/'))
        {
            //Marge paths
            const wxChar* op = m_path.c_str();
            const wxChar* bp = base.m_path.c_str() + base.m_path.Length();

            //not a ending directory?  move up
            if (base.m_path[0] && *(bp-1) != wxT('/'))
                UpTree(base.m_path, bp);

            //normalize directories
            while(*op == wxT('.') && *(op+1) == wxT('.') && 
                       (*(op+2) == '\0' || *(op+2) == wxT('/')) )
            {
                UpTree(base.m_path, bp);

                if (*(op+2) == '\0')
                    op += 2;
                else
                    op += 3;
            }

            m_path = base.m_path.substr(0, bp - base.m_path.c_str()) + 
                    m_path.Mid((op - m_path.c_str()), m_path.Length());
        }
    }

    //T.fragment = R.fragment; 
}

// ---------------------------------------------------------------------------
// UpTree 
//
// Moves a URI path up a directory
// ---------------------------------------------------------------------------

//static
void wxURI::UpTree(const wxChar* uristart, const wxChar*& uri)
{
    if (uri != uristart && *(uri-1) == wxT('/'))
    {
        uri -= 2;
    }
    
    for(;uri != uristart; --uri)
    {
        if (*uri == wxT('/'))
        {
            ++uri;
            break;
        }
    }

    //!!!TODO:HACK!!!//
    if (uri == uristart && *uri == wxT('/'))
        ++uri;
    //!!!//
}

// ---------------------------------------------------------------------------
// Normalize
//
// Normalizes directories in-place
//
// I.E. ./ and . are ignored
//
// ../ and .. are removed if a directory is before it, along
// with that directory (leading .. and ../ are kept)
// ---------------------------------------------------------------------------

//static
void wxURI::Normalize(wxChar* s, bool bIgnoreLeads)
{
    wxChar* cp = s;
    wxChar* bp = s;

    if(s[0] == wxT('/'))
        ++bp;

    while(*cp)
    {
        if (*cp == wxT('.') && (*(cp+1) == wxT('/') || *(cp+1) == '\0')
            && (bp == cp || *(cp-1) == wxT('/')))
        {
            //. _or_ ./  - ignore
            if (*(cp+1) == '\0')
                cp += 1;
            else
                cp += 2;
        }
        else if (*cp == wxT('.') && *(cp+1) == wxT('.') && 
                (*(cp+2) == wxT('/') || *(cp+2) == '\0')
                && (bp == cp || *(cp-1) == wxT('/')))
        {
            //.. _or_ ../ - go up the tree
            if (s != bp)
            {
                UpTree((const wxChar*)bp, (const wxChar*&)s);

                if (*(cp+2) == '\0')
                    cp += 2;
                else
                    cp += 3;
            }
            else if (!bIgnoreLeads)

            {
                *bp++ = *cp++;
                *bp++ = *cp++;
                if (*cp)
                    *bp++ = *cp++;

                s = bp;
            }
            else
            {
                if (*(cp+2) == '\0')
                    cp += 2;
                else
                    cp += 3;
            }
        }
        else
            *s++ = *cp++; 
    }

    *s = '\0';
}

// ---------------------------------------------------------------------------
// ParseH16
//
// Parses 1 to 4 hex values.  Returns true if the first character of the input
// string is a valid hex character.  It is the caller's responsability to move 
// the input string back to its original position on failure.
// ---------------------------------------------------------------------------

bool wxURI::ParseH16(const wxChar*& uri)
{
    // h16           = 1*4HEXDIG
    if(!IsHex(*++uri))
        return false;

    if(IsHex(*++uri) && IsHex(*++uri) && IsHex(*++uri))
        ++uri;

    return true;
}

// ---------------------------------------------------------------------------
// ParseIPXXX
//
// Parses a certain version of an IP address and moves the input string past 
// it.  Returns true if the input  string contains the proper version of an ip 
// address.  It is the caller's responsability to move the input string back 
// to its original position on failure.
// ---------------------------------------------------------------------------

bool wxURI::ParseIPv4address(const wxChar*& uri)
{
    //IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
    //
    //dec-octet     =      DIGIT                    ; 0-9
    //                / %x31-39 DIGIT               ; 10-99
    //                / "1" 2DIGIT                  ; 100-199
    //                / "2" %x30-34 DIGIT           ; 200-249
    //                / "25" %x30-35                ; 250-255
    size_t iIPv4 = 0;
    if (IsDigit(*uri))
    {
        ++iIPv4;

        
        //each ip part must be between 0-255 (dupe of version in for loop)
        if( IsDigit(*++uri) && IsDigit(*++uri) &&
           //100 or less  (note !)
           !( (*(uri-2) < wxT('2')) || 
           //240 or less   
             (*(uri-2) == wxT('2') && 
               (*(uri-1) < wxT('5') || (*(uri-1) == wxT('5') && *uri <= wxT('5')))
             )
            )
          )
        {
            return false;
        }

        if(IsDigit(*uri))++uri;

        //compilers should unroll this loop
        for(; iIPv4 < 4; ++iIPv4)
        {
            if (*uri != wxT('.') || !IsDigit(*++uri))
                break;

            //each ip part must be between 0-255
            if( IsDigit(*++uri) && IsDigit(*++uri) &&
               //100 or less  (note !)
               !( (*(uri-2) < wxT('2')) || 
               //240 or less   
                 (*(uri-2) == wxT('2') && 
                   (*(uri-1) < wxT('5') || (*(uri-1) == wxT('5') && *uri <= wxT('5')))
                 )
                )
              )
            {
                return false;
            }
            if(IsDigit(*uri))++uri;
        }
    }
    return iIPv4 == 4;
}

bool wxURI::ParseIPv6address(const wxChar*& uri)
{
    // IPv6address   =                            6( h16 ":" ) ls32
    //               /                       "::" 5( h16 ":" ) ls32
    //               / [               h16 ] "::" 4( h16 ":" ) ls32
    //               / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
    //               / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
    //               / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
    //               / [ *4( h16 ":" ) h16 ] "::"              ls32
    //               / [ *5( h16 ":" ) h16 ] "::"              h16
    //               / [ *6( h16 ":" ) h16 ] "::"

    size_t numPrefix = 0,
              maxPostfix;

    bool bEndHex = false;

    for( ; numPrefix < 6; ++numPrefix)
    {
        if(!ParseH16(uri))
        {
            --uri;
            bEndHex = true;
            break;
        }
        
        if(*uri != wxT(':'))
        {
            break;
        }
    }

    if(!bEndHex && !ParseH16(uri))
    {
        --uri;

        if (numPrefix)
            return false;

        if (*uri == wxT(':'))
        {
            if (*++uri != wxT(':'))
                return false;

            maxPostfix = 5;
        }
        else
            maxPostfix = 6;
    }
    else
    {
        if (*uri != wxT(':') || *(uri+1) != wxT(':'))
        {
            if (numPrefix != 6)
                return false;

            while (*--uri != wxT(':')) {}
            ++uri;

            const wxChar* uristart = uri;
            //parse ls32
            // ls32          = ( h16 ":" h16 ) / IPv4address
            if (ParseH16(uri) && *uri == wxT(':') && ParseH16(uri)) 
                return true;

            uri = uristart;

            if (ParseIPv4address(uri))
                return true;
            else
                return false;
        }
        else
        {
            uri += 2;
    
            if (numPrefix > 3)
                maxPostfix = 0;
            else
                maxPostfix = 4 - numPrefix;
        }
    }

    bool bAllowAltEnding = maxPostfix == 0;

    for(; maxPostfix != 0; --maxPostfix)
    {
        if(!ParseH16(uri) || *uri != wxT(':'))
            return false;
    }

    if(numPrefix <= 4)
    {
        const wxChar* uristart = uri;
        //parse ls32
        // ls32          = ( h16 ":" h16 ) / IPv4address
        if (ParseH16(uri) && *uri == wxT(':') && ParseH16(uri)) 
            return true;

        uri = uristart;

        if (ParseIPv4address(uri))
            return true;

        uri = uristart;
        
        if (!bAllowAltEnding)
            return false;
    }

    if(numPrefix <= 5 && ParseH16(uri))
        return true;

    return true;
}

bool wxURI::ParseIPvFuture(const wxChar*& uri)
{
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    if (*++uri != wxT('v') || !IsHex(*++uri))
        return false;

    while (IsHex(*++uri)) {}

    if (*uri != wxT('.') || !(IsUnreserved(*++uri) || IsSubDelim(*uri) || *uri == wxT(':')))
        return false;

    while(IsUnreserved(*++uri) || IsSubDelim(*uri) || *uri == wxT(':')) {}

    return true;
}


// ---------------------------------------------------------------------------
// CharToHex
//
// Converts a character into a numeric hexidecimal value, or 0 if the 
// passed in character is not a valid hex character
// ---------------------------------------------------------------------------

//static
wxInt32 wxURI::CharToHex(const wxChar& c)
{
	if ((c >= wxT('A')) && (c <= wxT('Z')))	return c - wxT('A') + 0x0A;
	if ((c >= wxT('a')) && (c <= wxT('z')))	return c - wxT('a') + 0x0a;
	if ((c >= wxT('0')) && (c <= wxT('9')))	return c - wxT('0') + 0x00;

	return 0;
}

// ---------------------------------------------------------------------------
// IsXXX
//
// Returns true if the passed in character meets the criteria of the method
// ---------------------------------------------------------------------------

//! unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
bool wxURI::IsUnreserved (const wxChar& c)
{   return IsAlpha(c) || IsDigit(c) || 
           c == wxT('-') ||
           c == wxT('.') ||
           c == wxT('_') ||
           c == wxT('~') //tilde
           ;  
}

bool wxURI::IsReserved (const wxChar& c)
{   
    return IsGenDelim(c) || IsSubDelim(c);
}

//! gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
bool wxURI::IsGenDelim (const wxChar& c)
{
    return c == wxT(':') ||
           c == wxT('/') ||
           c == wxT('?') ||
           c == wxT('#') ||
           c == wxT('[') ||
           c == wxT(']') ||
           c == wxT('@');
}

//! sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//!               / "*" / "+" / "," / ";" / "="
bool wxURI::IsSubDelim (const wxChar& c)
{
    return c == wxT('!') ||
           c == wxT('$') ||
           c == wxT('&') ||
           c == wxT('\'') ||
           c == wxT('(') ||
           c == wxT(')') ||
           c == wxT('*') ||
           c == wxT('+') ||
           c == wxT(',') ||
           c == wxT(';') ||
           c == wxT('=') 
           ;
}

bool wxURI::IsHex(const wxChar& c)
{   return IsDigit(c) || (c >= wxT('a') && c <= wxT('f')) || (c >= wxT('A') && c <= wxT('F')); }

bool wxURI::IsAlpha(const wxChar& c)
{   return (c >= wxT('a') && c <= wxT('z')) || (c >= wxT('A') && c <= wxT('Z'));  }

bool wxURI::IsDigit(const wxChar& c)
{   return c >= wxT('0') && c <= wxT('9');        }


// ---------------------------------------------------------------------------
//
//                        wxURL Compatability
//
// ---------------------------------------------------------------------------

#if wxUSE_URL

#if WXWIN_COMPATIBILITY_2_4

#include "wx/url.h"

//Note that this old code really doesn't convert to a URI that well and looks
//more like a dirty hack than anything else...

wxString wxURL::ConvertToValidURI(const wxString& uri, const wxChar* delims)
{
  wxString out_str;
  wxString hexa_code;
  size_t i;

  for (i = 0; i < uri.Len(); i++)
  {
    wxChar c = uri.GetChar(i);

    if (c == wxT(' '))
    {
      // GRG, Apr/2000: changed to "%20" instead of '+'

      out_str += wxT("%20");
    }
    else
    {
      // GRG, Apr/2000: modified according to the URI definition (RFC 2396)
      //
      // - Alphanumeric characters are never escaped
      // - Unreserved marks are never escaped
      // - Delimiters must be escaped if they appear within a component
      //     but not if they are used to separate components. Here we have
      //     no clear way to distinguish between these two cases, so they
      //     are escaped unless they are passed in the 'delims' parameter
      //     (allowed delimiters).

      static const wxChar marks[] = wxT("-_.!~*()'");

      if ( !wxIsalnum(c) && !wxStrchr(marks, c) && !wxStrchr(delims, c) )
      {
        hexa_code.Printf(wxT("%%%02X"), c);
        out_str += hexa_code;
      }
      else
      {
        out_str += c;
      }
    }
  }

  return out_str;
}

wxString wxURL::ConvertFromURI(const wxString& uri)
{
    return wxURI::Unescape(uri);
}

#endif //WXWIN_COMPATIBILITY_2_4

#endif //wxUSE_URL

//end of uri.cpp



