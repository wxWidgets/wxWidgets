/////////////////////////////////////////////////////////////////////////////
// Name:        uri.cpp
// Purpose:     Implementation of a uri parser
// Author:      Ryan Norton
// Created:     10/26/04
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Ryan Norton
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

//
//TODO:  RN:  I had some massive doxygen docs, I need to move these
//in a presentable form in these sources
//

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
// Escape/TranslateEscape/IsEscape
//
// TranslateEscape unencodes a 3 character URL escape sequence 
// Escape encodes an invalid URI character into a 3 character sequence
// IsEscape determines if the input string contains an escape sequence,
// if it does, then it moves the input string past the escape sequence
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
    s += '%';
    s += hdig[(c >> 4) & 15];
	s += hdig[c & 15];
}

bool wxURI::IsEscape(const wxChar*& uri)
{
    if(*uri == '%' && IsHex(*(uri+1)) && IsHex(*(uri+2)))
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
// operator = and ==
// ---------------------------------------------------------------------------

wxURI& wxURI::operator = (const wxURI& uri)
{
    return Assign(uri);
}

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

wxURI& wxURI::operator = (const wxString& string)
{   
    Create(string);
    return *this;
}

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
        //RN: Scheme can not be escaped
        while (IsAlpha(*uri) || IsDigit(*uri) || 
               *uri == '+'   ||
               *uri == '-'   ||
               *uri == '.') 
        { 
            m_scheme += *uri++; 
        }

        //valid scheme?
        if (*uri == ':')
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
    if (*uri == '/' && *(uri+1) == '/') 
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
    while(*uri && *uri != '@' && *uri != '/' && *uri != '#' && *uri != '?') 
    {
        if(IsUnreserved(*uri) || IsEscape(uri) || 
           IsSubDelim(*uri) || *uri == ':')
            m_user += *uri++;
        else
            Escape(m_user, *uri++);
    }

    if(*uri == '@')
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
    if (*uri == '[')
    {
        if (ParseIPv6address(++uri) && *uri == ']')
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

            if (ParseIPvFuture(++uri) && *uri == ']')
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
        while(*uri && *uri != '/' && *uri != ':' && *uri != '#' && *uri != '?') 
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
    if(*uri == ':')
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
    if (*uri == '/')
    {
        m_path += *uri++;

        while(*uri && *uri != '#' && *uri != '?') 
        { 
            if( IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                *uri == ':' || *uri == '@' || *uri == '/')
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
            while(*uri && *uri != '#' && *uri != '?') 
            {
                if(IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                  *uri == '@' || *uri == '/')
                    m_path += *uri++; 
                else    
                    Escape(m_path, *uri++);    
            }
        } 
        else
        {
            while(*uri && *uri != '#' && *uri != '?') 
            {
                if(IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                   *uri == ':' || *uri == '@' || *uri == '/')
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
    if (*uri == '?')
    {
        ++uri;
        while(*uri && *uri != '#')
        {
            if (IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                *uri == ':' || *uri == '@' || *uri == '/' || *uri == '?')
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
    if (*uri == '#')
    {
        ++uri;
        while(*uri)
        {
            if (IsUnreserved(*uri) || IsSubDelim(*uri) || IsEscape(uri) ||
                *uri == ':' || *uri == '@' || *uri == '/' || *uri == '?')
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
//  Resolve URI
//
//  Builds missing components of this uri from a base uri
//
//  A version of the algorithm outlined in the RFC is used here
//  (it is shown in comments)
// ---------------------------------------------------------------------------

void wxURI::Resolve(const wxURI& base, int flags)
{
    wxASSERT_MSG(!base.IsReference(), 
                wxT("wxURI to inherit from must not be a reference!"));

    // If we arn't being strict, enable the older
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
        if (!m_path.StartsWith(wxT("/")))
        {
            //Marge paths
            const wxChar* op = m_path.c_str();
            const wxChar* bp = base.m_path.c_str() + base.m_path.Length();

            //not a ending directory?  move up
            if (base.m_path[0] && *(bp-1) != '/')
                UpTree(base.m_path, bp);

            //normalize directories
            while(*op == '.' && *(op+1) == '.' && 
                       (*(op+2) == '\0' || *(op+2) == '/') )
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
}

// ---------------------------------------------------------------------------
// Directory Normalization (static)
//
// UpTree goes up a directory in a string and moves the pointer as such,
// while Normalize gets rid of duplicate/erronues directories in a URI
// according to RFC 2396 and modified quite a bit to meet the unit tests
// in it.
// ---------------------------------------------------------------------------

void wxURI::UpTree(const wxChar* uristart, const wxChar*& uri)
{
    if (uri != uristart && *(uri-1) == '/')
    {
        uri -= 2;
    }
    
    for(;uri != uristart; --uri)
    {
        if (*uri == '/')
        {
            ++uri;
            break;
        }
    }

    //!!!TODO:HACK!!!//
    if (uri == uristart && *uri == '/')
        ++uri;
    //!!!//
}

void wxURI::Normalize(wxChar* s, bool bIgnoreLeads)
{
    wxChar* cp = s;
    wxChar* bp = s;

    if(s[0] == '/')
        ++bp;

    while(*cp)
    {
        if (*cp == '.' && (*(cp+1) == '/' || *(cp+1) == '\0')
            && (bp == cp || *(cp-1) == '/'))
        {
            //. _or_ ./  - ignore
            if (*(cp+1) == '\0')
                cp += 1;
            else
                cp += 2;
        }
        else if (*cp == '.' && *(cp+1) == '.' && 
                (*(cp+2) == '/' || *(cp+2) == '\0')
                && (bp == cp || *(cp-1) == '/'))
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
// Misc. Parsing Methods
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
           !( (*(uri-2) < '2') || 
           //240 or less   
             (*(uri-2) == '2' && 
               (*(uri-1) < '5' || (*(uri-1) == '5' && *uri <= '5'))
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
            if (*uri != '.' || !IsDigit(*++uri))
                break;

            //each ip part must be between 0-255
            if( IsDigit(*++uri) && IsDigit(*++uri) &&
               //100 or less  (note !)
               !( (*(uri-2) < '2') || 
               //240 or less   
                 (*(uri-2) == '2' && 
                   (*(uri-1) < '5' || (*(uri-1) == '5' && *uri <= '5'))
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

bool wxURI::ParseH16(const wxChar*& uri)
{
    // h16           = 1*4HEXDIG
    if(!IsHex(*++uri))
        return false;

    if(IsHex(*++uri) && IsHex(*++uri) && IsHex(*++uri))
        ++uri;

    return true;
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
        
        if(*uri != ':')
        {
            break;
        }
    }

    if(!bEndHex && !ParseH16(uri))
    {
        --uri;

        if (numPrefix)
            return false;

        if (*uri == ':')
        {
            if (*++uri != ':')
                return false;

            maxPostfix = 5;
        }
        else
            maxPostfix = 6;
    }
    else
    {
        if (*uri != ':' || *(uri+1) != ':')
        {
            if (numPrefix != 6)
                return false;

            while (*--uri != ':') {}
            ++uri;

            const wxChar* uristart = uri;
            //parse ls32
            // ls32          = ( h16 ":" h16 ) / IPv4address
            if (ParseH16(uri) && *uri == ':' && ParseH16(uri)) 
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
        if(!ParseH16(uri) || *uri != ':')
            return false;
    }

    if(numPrefix <= 4)
    {
        const wxChar* uristart = uri;
        //parse ls32
        // ls32          = ( h16 ":" h16 ) / IPv4address
        if (ParseH16(uri) && *uri == ':' && ParseH16(uri)) 
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
    if (*++uri != 'v' || !IsHex(*++uri))
        return false;

    while (IsHex(*++uri)) {}

    if (*uri != '.' || !(IsUnreserved(*++uri) || IsSubDelim(*uri) || *uri == ':'))
        return false;

    while(IsUnreserved(*++uri) || IsSubDelim(*uri) || *uri == ':') {}

    return true;
}


// ---------------------------------------------------------------------------
// Misc methods - IsXXX and CharToHex
// ---------------------------------------------------------------------------

wxInt32 wxURI::CharToHex(const wxChar& c)
{
	if ((c >= 'A') && (c <= 'Z'))	return c - 'A' + 0x0A;
	if ((c >= 'a') && (c <= 'z'))	return c - 'a' + 0x0a;
	if ((c >= '0') && (c <= '9'))	return c - '0' + 0x00;

	return 0;
}

//! unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
bool wxURI::IsUnreserved (const wxChar& c)
{   return IsAlpha(c) || IsDigit(c) || 
           c == '-' ||
           c == '.' ||
           c == '_' ||
           c == '~' //tilde
           ;  
}

bool wxURI::IsReserved (const wxChar& c)
{   
    return IsGenDelim(c) || IsSubDelim(c);
}

//! gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
bool wxURI::IsGenDelim (const wxChar& c)
{
    return c == ':' ||
           c == '/' ||
           c == '?' ||
           c == '#' ||
           c == '[' ||
           c == ']' ||
           c == '@';
}

//! sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//!               / "*" / "+" / "," / ";" / "="
bool wxURI::IsSubDelim (const wxChar& c)
{
    return c == '!' ||
           c == '$' ||
           c == '&' ||
           c == '\'' ||
           c == '(' ||
           c == ')' ||
           c == '*' ||
           c == '+' ||
           c == ',' ||
           c == ';' ||
           c == '=' 
           ;
}

bool wxURI::IsHex(const wxChar& c)
{   return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

bool wxURI::IsAlpha(const wxChar& c)
{   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');  }

bool wxURI::IsDigit(const wxChar& c)
{   return c >= '0' && c <= '9';        }


// ---------------------------------------------------------------------------
//
//                        wxURL Compatability
//
// ---------------------------------------------------------------------------

#if wxUSE_URL

#if WXWIN_COMPATIBILITY_2_4

#include "wx/url.h"

wxString wxURL::ConvertToValidURI(const wxString& uri, const wxChar* WXUNUSED(delims))
{
    return wxURI(uri).BuildURI();
}

wxString wxURL::ConvertFromURI(const wxString& uri)
{
    return wxURI::Unescape(uri);
}

#endif //WXWIN_COMPATIBILITY_2_4

#endif //wxUSE_URL

//end of uri.cpp



