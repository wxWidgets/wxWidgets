/////////////////////////////////////////////////////////////////////////////
// Name:        uri.h
// Purpose:     wxURI - Class for parsing/validating URIs 
// Author:      Ryan Norton
// Modified By: 
// Created:     07/01/2004
// RCS-ID:      $Id$
// Licence:     wxWindows
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_URI_H_
#define _WX_URI_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "uri.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"

// Host Type that the server can be
enum wxURIHostType
{
    wxURI_REGNAME,		
    wxURI_IPV4ADDRESS,	
    wxURI_IPV6ADDRESS,  
    wxURI_IPVFUTURE		
};

// Component Flags
enum wxURIFieldType
{
    wxURI_SCHEME = 1,
    wxURI_USER = 2,
    wxURI_SERVER = 4,
    wxURI_PORT = 8,
    wxURI_PATH = 16,
    wxURI_QUERY = 32,
    wxURI_FRAGMENT = 64
};

// Miscellaneous other flags
enum wxURIFlags
{
    wxURI_STRICT = 1
};


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

    virtual ~wxURI();

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

    void Resolve(const wxURI& base, int flags = wxURI_STRICT);
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
    const wxChar* ParsePath      (const wxChar* uri,
                                  bool bReference = false,
                                  bool bNormalize = true);
    const wxChar* ParseQuery     (const wxChar* uri);
    const wxChar* ParseFragment  (const wxChar* uri);


    static bool ParseH16(const wxChar*& uri);
    static bool ParseIPv4address(const wxChar*& uri);
    static bool ParseIPv6address(const wxChar*& uri);
    static bool ParseIPvFuture(const wxChar*& uri);


    static void Normalize(wxChar* uri, bool bIgnoreLeads = false);
    static void UpTree(const wxChar* uristart, const wxChar*& uri);

    static wxChar Unescape(const wxChar* s);
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
};

#endif // _WX_URI_H_

