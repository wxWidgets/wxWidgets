/////////////////////////////////////////////////////////////////////////////
// Name:        uri.h
// Purpose:     wxURI - Class for parsing/validating URIs 
// Author:      Ryan Norton
// Modified By: 
// Created:     07/01/2004
// RCS-ID:      $Id$
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_URIH__
#define _WX_URIH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "uri.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"

// Host Type that the server can be
typedef enum 
{
    wxURI_REGNAME,		
    wxURI_IPV4ADDRESS,	
    wxURI_IPV6ADDRESS,  
    wxURI_IPVFUTURE		
} wxURIHostType;

// Component Flags
typedef enum
{
    wxURI_SCHEME = 1,
    wxURI_USER = 2,
    wxURI_SERVER = 4,
    wxURI_PORT = 8,
    wxURI_PATH = 16,
    wxURI_QUERY = 32,
    wxURI_FRAGMENT = 64
} wxURIFieldType;

// Generic class for parsing URIs.
//
// Originally based off of RFC 2396 - then
// extended to meet the newer RFC 2396.bis
// specifications.
class wxURI : public wxObject
{
public:
    wxURI();
    wxURI(const wxString& uri);
    wxURI(const wxURI& uri);

    ~wxURI();

    void Create(const wxString& uri);

    bool HasScheme() const;
    bool HasUser() const;
    bool HasServer() const;
    bool HasPort() const;
    bool HasPath() const;
    bool HasQuery() const;
    bool HasFragment() const;

    const wxString& GetScheme() const;
    const wxString& GetPath() const;
    const wxString& GetQuery() const;
    const wxString& GetFragment() const;
    const wxString& GetPort() const;
    const wxString& GetUser() const;
    const wxString& GetServer() const;
    const wxURIHostType& GetHostType() const;

    wxString Get() const;

    void Resolve(const wxURI& base, const bool& bStrict = true);
    bool IsReference() const;

    wxURI& operator = (const wxURI& uri);
    wxURI& operator = (const wxChar* string);
    bool operator == (const wxURI& uri) const;
    
protected:

    void Clear();

    const wxChar* Parse          (const wxChar* uri);
    const wxChar* ParseAuthority (const wxChar* uri);
    const wxChar* ParseScheme    (const wxChar* uri);
    const wxChar* ParseUser      (const wxChar* uri);
    const wxChar* ParseServer    (const wxChar* uri);
    const wxChar* ParsePort      (const wxChar* uri);
    const wxChar* ParsePath      (const wxChar* uri, const bool& bReference = false,
                                                            const bool& bNormalize = true);
    const wxChar* ParseQuery     (const wxChar* uri);
    const wxChar* ParseFragment  (const wxChar* uri);


    static bool ParseH16(const wxChar*& uri);
    static bool ParseIPv4address(const wxChar*& uri);
    static bool ParseIPv6address(const wxChar*& uri);
    static bool ParseIPvFuture(const wxChar*& uri);


    static void Normalize(wxChar* uri, const bool& bIgnoreLeads = false);
    static void UpTree(const wxChar* uristart, const wxChar*& uri);

    static void Unescape(const wxChar*& s, wxChar& c);
    static void Escape  (wxString& s, const wxChar& c);
    static bool IsEscape(const wxChar*& uri);

    static int CharToHex(const wxChar& c);
	

    static bool IsUnreserved (const wxChar& c);
    static bool IsReserved (const wxChar& c);
    static bool IsGenDelim (const wxChar& c);
    static bool IsSubDelim (const wxChar& c);
    static bool IsHex(const wxChar& c);
    static bool IsAlpha(const wxChar& c);
    static bool IsDigit(const wxChar& c);

    wxString m_scheme;
    wxString m_path;
    wxString m_query;
    wxString m_fragment;

    wxString m_user;
    wxString m_server;
    wxString m_port;

    wxURIHostType m_hostType;

    size_t m_fields;

    DECLARE_DYNAMIC_CLASS(wxURI)
};//end of wxURI

#endif //_WX_URIH__
