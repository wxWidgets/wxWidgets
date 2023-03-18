/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/uri.cpp
// Purpose:     Implementation of a URI parser
// Author:      Ryan Norton,
//              Vadim Zeitlin (UTF-8 URI support, many other changes)
// Created:     10/26/04
// Copyright:   (c) 2004 Ryan Norton,
//                  2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/crt.h"
#endif

#include "wx/uri.h"

// ---------------------------------------------------------------------------
// definitions
// ---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxURI, wxObject);

// ===========================================================================
// wxURI implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// Constructors and cleanup
// ---------------------------------------------------------------------------

wxURI::wxURI()
     : m_hostType(wxURI_REGNAME),
       m_fields(0)
{
}

wxURI::wxURI(const wxString& uri)
     : m_hostType(wxURI_REGNAME),
       m_fields(0)
{
    Create(uri);
}

bool wxURI::Create(const wxString& uri)
{
    if (m_fields)
        Clear();

    return Parse(uri.utf8_str());
}

void wxURI::Clear()
{
    m_scheme.clear();
    m_userinfo.clear();
    m_server.clear();
    m_port.clear();
    m_path.clear();
    m_query.clear();
    m_fragment.clear();

    m_hostType = wxURI_REGNAME;

    m_fields = 0;
}

// ---------------------------------------------------------------------------
// Escaped characters handling
// ---------------------------------------------------------------------------

// Converts a character into a numeric hexadecimal value, or -1 if the passed
// in character is not a valid hex character

/* static */
int wxURI::CharToHex(char c)
{
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    if ((c >= '0') && (c <= '9'))
        return c - '0';

    return -1;
}

/* static */
wxString wxURI::Unescape(const wxString& uri)
{
    // URIs can contain escaped 8-bit characters that have to be decoded using
    // UTF-8 (see RFC 3986), however in our (probably broken...) case we can
    // also end up with not escaped Unicode characters in the URI string which
    // can't be decoded as UTF-8. So what we do here is to encode all Unicode
    // characters as UTF-8 only to decode them back below. This is obviously
    // inefficient but there doesn't seem to be anything else to do, other than
    // not allowing to mix Unicode characters with escapes in the first place,
    // but this seems to be done in a lot of places, unfortunately.
    const wxScopedCharBuffer& uriU8(uri.utf8_str());
    const size_t len = uriU8.length();

    // the unescaped version can't be longer than the original one
    wxCharBuffer buf(uriU8.length());
    char *p = buf.data();

    const char* const end = uriU8.data() + len;
    for ( const char* s = uriU8.data(); s != end; ++s, ++p )
    {
        char c = *s;
        if ( c == '%' && s < end - 2 && IsHex(s[1]) && IsHex(s[2]) )
        {
            c = (CharToHex(s[1]) << 4) | CharToHex(s[2]);
            s += 2;
        }

        *p = c;
    }

    *p = '\0';

    return wxString::FromUTF8(buf);
}

void wxURI::AppendNextEscaped(wxString& s, const char *& p)
{
    // check for an already encoded character:
    //
    // pct-encoded   = "%" HEXDIG HEXDIG
    if ( p[0] == '%' && IsHex(p[1]) && IsHex(p[2]) )
    {
        s += *p++;
        s += *p++;
        s += *p++;
    }
    else // really needs escaping
    {
        static const char* hexDigits = "0123456789abcdef";

        const char c = *p++;

        s += '%';
        s += hexDigits[(c >> 4) & 15];
        s += hexDigits[c & 15];
    }
}

// ---------------------------------------------------------------------------
// GetUser
// GetPassword
//
// Gets the username and password via the old URL method.
// ---------------------------------------------------------------------------
wxString wxURI::GetUser() const
{
    // if there is no colon at all, find() returns npos and this method returns
    // the entire string which is correct as it means that password was omitted
    return m_userinfo(0, m_userinfo.find(':'));
}

wxString wxURI::GetPassword() const
{
      size_t posColon = m_userinfo.find(':');

      if ( posColon == wxString::npos )
          return wxString();

      return m_userinfo(posColon + 1, wxString::npos);
}

// combine all URI fields in a single string, applying funcDecode to each
// component which it may make sense to decode (i.e. "unescape")
wxString wxURI::DoBuildURI(wxString (*funcDecode)(const wxString&)) const
{
    wxString ret;

    if (HasScheme())
        ret += m_scheme + ":";

    if (HasServer())
    {
        ret += "//";

        if (HasUserInfo())
            ret += funcDecode(m_userinfo) + "@";

        if (m_hostType == wxURI_REGNAME)
            ret += funcDecode(m_server);
        else
            ret += m_server;

        if (HasPort())
            ret += ":" + m_port;
    }

    ret += funcDecode(m_path);

    if (HasQuery())
        ret += "?" + funcDecode(m_query);

    if (HasFragment())
        ret += "#" + funcDecode(m_fragment);

    return ret;
}

// ---------------------------------------------------------------------------
// Comparison
// ---------------------------------------------------------------------------

bool wxURI::operator==(const wxURI& uri) const
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
        if (HasUserInfo())
        {
            if (m_userinfo != uri.m_userinfo)
                return false;
        }
        else if (uri.HasUserInfo())
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
// if there is no authority or scheme, it is a reference.
// May be needed to have an RFC Deviation here where file doesn't need to
// have a server so that raw file paths work as users expect. I.E.:
// return !HasScheme() || (!HasServer() && m_scheme != wxT("file"));
// ---------------------------------------------------------------------------

bool wxURI::IsReference() const
{
    return !HasScheme() || !HasServer();
}

// ---------------------------------------------------------------------------
// IsRelative
// ---------------------------------------------------------------------------

bool wxURI::IsRelative() const
{
    return !HasScheme() && !HasServer();
}

// ---------------------------------------------------------------------------
// Parse
//
// Master URI parsing method.  Just calls the individual parsing methods
//
// URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
// URI-reference = URI / relative
// ---------------------------------------------------------------------------

bool wxURI::Parse(const char *uri)
{
    uri = ParseScheme(uri);
    if ( uri )
        uri = ParseAuthority(uri);
    if ( uri )
        uri = ParsePath(uri);
    if ( uri )
        uri = ParseQuery(uri);
    if ( uri )
        uri = ParseFragment(uri);

    // we only succeed if we parsed the entire string
    return uri && *uri == '\0';
}

const char* wxURI::ParseScheme(const char *uri)
{
    const char * const start = uri;

    // assume that we have a scheme if we have the valid start of it
    if ( IsAlpha(*uri) )
    {
        m_scheme += *uri++;

        //scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
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
            ++uri;
        }
        else // no valid scheme finally
        {
            uri = start; // rewind
            m_scheme.clear();
        }
    }
    //else: can't have schema, possible a relative URI

    return uri;
}

const char* wxURI::ParseAuthority(const char* uri)
{
    // URI			 = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    // hier-part	 = "//" authority path-abempty
    bool leadswithdoubleslash = uri[0] == '/' && uri[1] == '/';
    if(leadswithdoubleslash)
    {
        //skip past the two slashes
        uri += 2;
    }

    // ############# DEVIATION FROM RFC #########################
    // Evidence shows us that users expect us to parse the
    // server component in the case of [server][#fragment].
    // Otherwise they don't want us to parse the server at all
    // for file uris.
    if (m_scheme == "file") // RFC 8089 allows for leading "//" with file
    { // look for [server][#fragment] case
        bool serverfragmentfound = false;
        const char* uristart = uri;
        uri = ParseServer(uri);
        if(HasServer())
        {
            uri = ParseFragment(uri);
            serverfragmentfound = HasFragment();
        }
        if(!serverfragmentfound)
        { // nope... clear fields, return to point after opening slashes
            uri = uristart;
            m_server.erase();
            m_fields &= ~wxURI_SERVER;
        }
    }
    else if(leadswithdoubleslash)
    { // normal RFC-based authority parsing
        // authority	 = [ userinfo "@" ] host [ ":" port ]
        // file-hier-part = ( "//" auth-path ) / local-path
        const char* uristart = uri;
        uri = ParseUserInfo(uri);
        uri = ParseServer(uri);
        if (!HasServer() && HasUserInfo()) // in practice this case doesn't happen without scheme-based parsing
        { // userinfo can't exist without host, backtrack and reparse
            m_userinfo.erase();
            m_fields &= ~wxURI_USERINFO;
            uri = uristart;
            uri = ParseServer(uri);
        }
        uri = ParsePort(uri);
        if (!HasServer()) // in practice this case doesn't happen without scheme-based parsing
        {
            m_port.erase(); // port can't exist without host
            m_fields &= ~wxURI_PORT;
            uri = uristart - 2; // nothing found, skip to before the leading "//"
        }
    }

    return uri;
}

const char* wxURI::ParseUserInfo(const char* uri)
{
    const char * const start = uri;

    // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    while ( *uri && *uri != '@' && *uri != '/' && *uri != '#' && *uri != '?' )
    {
        if ( IsUnreserved(*uri) || IsSubDelim(*uri) || *uri == ':' )
            m_userinfo += *uri++;
        else
            AppendNextEscaped(m_userinfo, uri);
    }

    if ( *uri++ == '@' )
    {
        // valid userinfo
        m_fields |= wxURI_USERINFO;
    }
    else
    {
        uri = start; // rewind
        m_userinfo.clear();
    }

    return uri;
}

const char* wxURI::ParseServer(const char* uri)
{
    const char * const start = uri;

    // host          = IP-literal / IPv4address / reg-name
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    if (*uri == '[')
    {
        ++uri;
        if (ParseIPv6address(uri) && *uri == ']')
        {
            m_hostType = wxURI_IPV6ADDRESS;

            m_server.assign(start + 1, uri - start - 1);
            ++uri;
        }
        else
        {
            uri = start + 1; // skip the leading '[' again

            if (ParseIPvFuture(uri) && *uri == ']')
            {
                m_hostType = wxURI_IPVFUTURE;

                m_server.assign(start + 1, uri - start - 1);
                ++uri;
            }
            else // unrecognized IP literal
            {
                uri = start;
            }
        }
    }
    else // IPv4 or a reg-name
    {
        if (ParseIPv4address(uri) && (!*uri || *uri == '/' || *uri == ':'))
        {
            m_hostType = wxURI_IPV4ADDRESS;

            m_server.assign(start, uri - start);
        }
        else
        {
            uri = start;
        }
    }

    if ( m_hostType == wxURI_REGNAME )
    {
        uri = start;
        // reg-name      = *( unreserved / pct-encoded / sub-delims )
        while ( *uri && *uri != '/' && *uri != ':' && *uri != '#' && *uri != '?' )
        {
            if ( IsUnreserved(*uri) || IsSubDelim(*uri) )
                m_server += *uri++;
            else
                AppendNextEscaped(m_server, uri);
        }
    }

    m_fields |= wxURI_SERVER;

    return uri;
}


const char* wxURI::ParsePort(const char* uri)
{
    // port          = *DIGIT
    if( *uri == ':' )
    {
        ++uri;
        while ( IsDigit(*uri) )
        {
            m_port += *uri++;
        }

        m_fields |= wxURI_PORT;
    }

    return uri;
}

const char* wxURI::ParsePath(const char* uri)
{
    // hier-part     = "//" authority path-abempty
    //               / path-absolute
    //               / path-rootless
    //               / path-empty
    //
    // relative-part = "//" authority path-abempty
    //               / path-absolute
    //               / path-noscheme
    //               / path-empty
    //
    // path-abempty  = *( "/" segment )
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    // path-noscheme = segment-nz-nc *( "/" segment )
    // path-rootless = segment-nz *( "/" segment )
    // path-empty    = 0<pchar>
    //
    // segment       = *pchar
    // segment-nz    = 1*pchar
    // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    //               ; non-zero-length segment without any colon ":"
    //
    // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"

    if (IsEndPath(*uri)
        // When authority is present,
        // the path must either be empty or begin with a slash ("/") character.
        // When authority is not present,
        // the path cannot begin with two slash characters ("//").
        || (m_server.length() ? (!(!*uri || *uri == '/')) : (*uri == '/' && *(uri+1) == '/')))
        return uri;
    const char* uristart = uri;
    const bool isAbsolute = *uri == '/';
    if (isAbsolute)
    {
        m_path += *uri++;
        // segment-nz	 = 1*pchar
        while (!IsEndPath(*uri))
        {
            if (IsPCharNE(*uri) || *uri == '/')
                m_path += *uri++;
            else
                AppendNextEscaped(m_path, uri);
        }
        m_fields |= wxURI_PATH; //mark the path as valid
    }
    else if (*uri) //Relative path
    {
        if (!HasScheme())
        {
            // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
            //				 ; non-zero-length segment without any colon ":"
            while (!IsEndPath(*uri))
            {
                if (IsPCharNCNE(*uri) || *uri == '/')
                    m_path += *uri++;
                else
                    AppendNextEscaped(m_path, uri);
            }
        }
        else
        {
            // segment-nz	 = 1*pchar
            while (!IsEndPath(*uri))
            {
                if (IsPCharNE(*uri) || *uri == '/')
                    m_path += *uri++;
                else
                    AppendNextEscaped(m_path, uri);
            }
        }
        if (uri != uristart)
            m_fields |= wxURI_PATH; // mark the path as valid
    }
    return uri;
}


const char* wxURI::ParseQuery(const char* uri)
{
    // query         = *( pchar / "/" / "?" )
    if ( *uri == '?' )
    {
        ++uri;
        while ( *uri && *uri != '#' )
        {
            if ( IsPCharNE(*uri) || *uri == '/' || *uri == '?' )
                m_query += *uri++;
            else
                AppendNextEscaped(m_query, uri);
        }

        m_fields |= wxURI_QUERY;
    }

    return uri;
}


const char* wxURI::ParseFragment(const char* uri)
{
    // fragment      = *( pchar / "/" / "?" )
    if ( *uri == '#' )
    {
        ++uri;
        while ( *uri )
        {
            if ( IsPCharNE(*uri) || *uri == '/' || *uri == '?' )
                m_fragment += *uri++;
            else
                AppendNextEscaped(m_fragment, uri);
        }

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

/* static */
wxArrayString wxURI::SplitInSegments(const wxString& path)
{
    return wxSplit(path, '/', '\0' /* no escape character */);
}

void wxURI::Resolve(const wxURI& base, int flags)
{
    wxASSERT_MSG((!base.IsReference()),
        "wxURI to inherit from must not be a reference!");

    // If we aren't being strict, enable the older (pre-RFC2396) loophole that
    // allows this uri to inherit other properties from the base uri - even if
    // the scheme is defined
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
        return;

    //No scheme - inherit
    m_scheme = base.m_scheme;
    m_fields |= wxURI_SCHEME;

    // All we need to do for relative URIs with an
    // authority component is just inherit the scheme
    //       if defined(R.authority) then
    //          T.authority = R.authority;
    //          T.path      = remove_dot_segments(R.path);
    //          T.query     = R.query;
    if (HasServer())
        return;

    //No authority - inherit
    if (base.HasUserInfo())
    {
        m_userinfo = base.m_userinfo;
        m_fields |= wxURI_USERINFO;
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
        // T.path = Base.path;
        m_path = base.m_path;
        m_fields |= wxURI_PATH;


        // if defined(R.query) then
        //     T.query = R.query;
        // else
        //     T.query = Base.query;
        // endif;
        if (!HasQuery())
        {
            m_query = base.m_query;
            m_fields |= wxURI_QUERY;
        }
    }
    else
    {
        // if (R.path starts-with "/") then
        //     T.path = remove_dot_segments(R.path);
        // else
        //     T.path = merge(Base.path, R.path);
        //     T.path = remove_dot_segments(T.path);
        // endif;
        //     T.query = R.query;
        //
        // So we just normalize (./.. handling) absolute paths and
        // merge the two together if the one we are resolving with
        // (right side/operand) is relative

        wxArrayString our(SplitInSegments(m_path)),
                      result(SplitInSegments(base.m_path));
        bool isRightAbsolute = !m_path.empty() && m_path[0u] == '/';

        if ( !result.empty() )
        {
            if ( isRightAbsolute )
                result.clear(); // just resolve right side
            else
                result.pop_back();
        }

        if ( our.empty() )
        {
            // if we have an empty path it means we were constructed from a "."
            // string or something similar (e.g. "././././"), it should count
            // as (empty) segment
            our.push_back(wxString());
        }

        const wxArrayString::const_iterator end = our.end();
        for ( wxArrayString::const_iterator i = our.begin(); i != end; ++i )
        {
            //. _or_ ./ - remove (6.2.2.3. Path Segment Normalization)
            if ( i->empty() || *i == "." || *i == "/")
            {
                // as in ParsePath(), while normally we ignore the empty
                // segments, we need to take account of them at the end
                if ( i == end - 1 )
                    result.push_back(wxString());
                continue;
            }

            //.. _or_ ../ - remove as well as previous path if it exists (6.2.2.3)
            if ( *i == ".." || *i == "../")
            {
                if ( !result.empty() )
                {
                    result.pop_back();

                    if ( i == end - 1 )
                        result.push_back(wxString());
                }
                //else: just ignore, extra ".." don't accumulate
            }
            else
            {
                if ( result.empty() )
                {
                    // ensure that the resulting path will always be absolute
                    result.push_back(wxString());
                }

                result.push_back(*i);
            }
        }

        m_path = wxJoin(result, '/', '\0');
    }

    //T.fragment = R.fragment;
    // (done implicitly)
}

// ---------------------------------------------------------------------------
// ParseH16
//
// Parses 1 to 4 hex values.  Returns true if the first character of the input
// string is a valid hex character.  It is the caller's responsibility to move
// the input string back to its original position on failure.
// ---------------------------------------------------------------------------

bool wxURI::ParseH16(const char*& uri)
{
    // h16           = 1*4HEXDIG
    if(!IsHex(*uri))
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
// address.  It is the caller's responsibility to move the input string back
// to its original position on failure.
// ---------------------------------------------------------------------------

bool wxURI::ParseIPv4address(const char*& uri)
{
    //IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
    //
    //dec-octet     =      DIGIT                    ; 0-9
    //                / %x31-39 DIGIT               ; 10-99
    //                / "1" 2DIGIT                  ; 100-199
    //                / "2" %x30-34 DIGIT           ; 200-249
    //                / "25" %x30-35                ; 250-255
    size_t iIPv4 = 0;
    const char* uriOrig = uri;
    if (IsDigit(*uri))
    {
        ++iIPv4;


        //each ip part must be between 0-255 (dupe of version in for loop)
        if( (IsDigit(*++uri) && IsDigit(*++uri) &&
           //100 or less  (note !)
           !( (*(uri-2) == '1') ||
           //240 or less
             (*(uri-2) == '2' &&
               (*(uri-1) < '5' || (*(uri-1) == '5' && *uri <= '5'))
             )
            )
          ) || ((uri - uriOrig >= 2) && (*uriOrig == '0')) // leading 0
         )
        {
            return false;
        }

        if(IsDigit(*uri))
            ++uri;

        //compilers should unroll this loop
        for(; iIPv4 < 4; ++iIPv4)
        {
            if (*uri != '.' || !IsDigit(*++uri))
                break;

            //each ip part must be between 0-255
            uriOrig = uri;
            if( (IsDigit(*++uri) && IsDigit(*++uri) &&
               //100 or less  (note !)
               !( (*(uri-2) == '1') ||
               //240 or less
                 (*(uri-2) == '2' &&
                   (*(uri-1) < '5' || (*(uri-1) == '5' && *uri <= '5'))
                 )
                )
               ) || ((uri - uriOrig >= 2) && (*uriOrig == '0')) // leading 0
              )
            {
                return false;
            }
            if(IsDigit(*uri))
                ++uri;
        }
    }
    return iIPv4 == 4;
}

bool wxURI::ParseIPv6address(const char*& uri)
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

    size_t leftHexpairs = 0, rightHexpairs, maxRightHexpairs;
    const char* uristart;
    bool doublecolon = false;
    if (*uri == ':' && *(uri+1) == ':')
        doublecolon = true;
    else
    {
        uristart = uri;
        if (ParseH16(uri))
        {
            ++leftHexpairs;
            if (*uri == ':' && *(uri+1) == ':')
                doublecolon = true;
            else
            {
                for (;leftHexpairs < 7;)
                { // skip up to 6 leading [":" h16] pairs
                    if (*uri == ':')
                    {
                        ++uri; // skip over single colon
                        uristart = uri;
                        if (!ParseH16(uri) || (*uri != ':' && *uri))
                        {
                            uri = uristart;
                            break;
                        }
                        ++leftHexpairs;
                    }
                    else
                        break;
                    if (*uri == ':' && *(uri+1) == ':')
                    {
                        doublecolon = true;
                        break;
                    }
                } // [":" h16] skipping loop
                if (!doublecolon && leftHexpairs == 7 && *uri == ':')
                    ++uri; // skip over single colon
            }//leading h16 :: check
        }//h16
        else
            uri = uristart;
    }
    if (doublecolon)
    {
        uri += 2;
        if (leftHexpairs < 5)
            maxRightHexpairs = 5 - leftHexpairs;
        else
            maxRightHexpairs = 0;
    }
    else
    {
        if (leftHexpairs < 6)
            maxRightHexpairs = 6 - leftHexpairs;
        else
            maxRightHexpairs = 0;
    }
    for (rightHexpairs = 0; rightHexpairs < maxRightHexpairs; ++rightHexpairs)
    { // skip up to 6 trailing [h16 ":"] pairs
        uristart = uri;
        if (!ParseH16(uri) || *uri != ':')
        {
            uri = uristart;
            break;
        }
        ++uri;
    }
    if (!doublecolon)
    {
        if (leftHexpairs < 6)
            return false;
        rightHexpairs = leftHexpairs;
        leftHexpairs = 0;
    }
    uristart = uri;
    if (leftHexpairs < 6 && rightHexpairs < 7) // ls32 = ( h16 ":" h16 ) / IPv4address
    {
        if (ParseH16(uri) && *uri++ == ':' && ParseH16(uri) && *uri == ']')
            return true;
        uri = uristart;
        if (ParseIPv4address(uri) && *uri == ']')
            return true;
        uri = uristart;
    }
    if (leftHexpairs < 7 && (doublecolon || rightHexpairs == 7)
        && ParseH16(uri) && *uri == ']') // final single h16 case or tail end of ending ls32
        return true;
    return doublecolon && *uri == ']'; // can only be empty if a "::" was detecte
}

bool wxURI::ParseIPvFuture(const char*& uri)
{
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    if (*++uri != 'v' || !IsHex(*++uri))
        return false;

    while (IsHex(*++uri))
        ;

    if (*uri != '.' || !(IsUnreserved(*++uri) || IsSubDelim(*uri) || *uri == ':'))
        return false;

    while(IsUnreserved(*++uri) || IsSubDelim(*uri) || *uri == ':') {}

    return true;
}


// ---------------------------------------------------------------------------
// IsXXX
//
// Returns true if the passed in character meets the criteria of the method
// ---------------------------------------------------------------------------

// unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
bool wxURI::IsUnreserved(char c)
{
    return IsAlpha(c) ||
           IsDigit(c) ||
           c == '-' ||
           c == '.' ||
           c == '_' ||
           c == '~'
           ;
}

bool wxURI::IsReserved(char c)
{
    return IsGenDelim(c) || IsSubDelim(c);
}

// gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
bool wxURI::IsGenDelim(char c)
{
    return c == ':' ||
           c == '/' ||
           c == '?' ||
           c == '#' ||
           c == '[' ||
           c == ']' ||
           c == '@';
}

// sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//               / "*" / "+" / "," / ";" / "="
bool wxURI::IsSubDelim(char c)
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

bool wxURI::IsHex(char c)
{
    return IsDigit(c) ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

bool wxURI::IsAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool wxURI::IsDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool wxURI::IsEndPath(char c)
{
    return c == '\0' || c == '#' || c == '?';
}

// pchar		 = unreserved / pct-encoded / sub-delims / ":" / "@"
// pct-encoded handled outside, NC is for no colon for relative paths
bool wxURI::IsPCharNCNE(char c)
{
    return IsUnreserved(c) || IsSubDelim(c) || c == '@' || c == '/';
}

bool wxURI::IsPCharNE(char c)
{
    return IsPCharNCNE(c) || c == ':';
}
