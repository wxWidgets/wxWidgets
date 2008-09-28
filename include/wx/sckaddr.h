/////////////////////////////////////////////////////////////////////////////
// Name:        sckaddr.h
// Purpose:     Network address classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     26/04/1997
// RCS-ID:      $Id$
// Copyright:   (c) 1997, 1998 Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCKADDR_H_
#define _WX_SCKADDR_H_

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include "wx/string.h"

#include "wx/gsocket.h"

// Any socket address kind
class WXDLLIMPEXP_NET wxSockAddress : public wxObject
{
public:
    enum
    {
        IPV4 = 1,
        IPV6 = 2,
        UNIX = 3
    } Addr;

    wxSockAddress();
    wxSockAddress(const wxSockAddress& other);
    virtual ~wxSockAddress();

    wxSockAddress& operator=(const wxSockAddress& other);

    virtual void Clear();
    virtual int Type() = 0;

    // we need to be able to create copies of the addresses polymorphically
    // (i.e. without knowing the exact address class)
    virtual wxSockAddress *Clone() const = 0;


    // implementation only, don't use
    GAddress *GetAddress() const { return m_address; }
    void SetAddress(GAddress *address);

protected:
    GAddress *m_address;

private:
    void Init();
    DECLARE_ABSTRACT_CLASS(wxSockAddress)
};

// An IP address (either IPv4 or IPv6)
class WXDLLIMPEXP_NET wxIPaddress : public wxSockAddress
{
public:
    wxIPaddress();
    wxIPaddress(const wxIPaddress& other);
    virtual ~wxIPaddress();

    virtual bool Hostname(const wxString& name) = 0;
    virtual bool Service(const wxString& name) = 0;
    virtual bool Service(unsigned short port) = 0;

    virtual bool LocalHost() = 0;
    virtual bool IsLocalHost() const = 0;

    virtual bool AnyAddress() = 0;
    virtual bool BroadcastAddress() = 0;

    virtual wxString IPAddress() const = 0;

    virtual wxString Hostname() const = 0;
    virtual unsigned short Service() const = 0;

    DECLARE_ABSTRACT_CLASS(wxIPaddress)
};

// An IPv4 address
class WXDLLIMPEXP_NET wxIPV4address : public wxIPaddress
{
public:
    wxIPV4address();
    wxIPV4address(const wxIPV4address& other);
    virtual ~wxIPV4address();

    bool operator==(const wxIPV4address& addr) const;

    // implement wxSockAddress pure virtuals:
    virtual int Type() { return wxSockAddress::IPV4; }
    virtual wxSockAddress *Clone() const;


    // implement wxIPaddress pure virtuals:

    // handles the usual dotted quad format too
    virtual bool Hostname(const wxString& name);
    virtual bool Service(const wxString& name);
    virtual bool Service(unsigned short port);

    // localhost (127.0.0.1)
    virtual bool LocalHost();
    virtual bool IsLocalHost() const;

    // any (0.0.0.0)
    virtual bool AnyAddress();
    // all (255.255.255.255)
    virtual bool BroadcastAddress();

    // a.b.c.d
    virtual wxString IPAddress() const;

    virtual wxString Hostname() const;
    virtual unsigned short Service() const;


    // IPv4-specific methods:

    bool Hostname(unsigned long addr);
    wxString OrigHostname() { return m_origHostname; }

private:
    wxString m_origHostname;

    DECLARE_DYNAMIC_CLASS(wxIPV4address)
};


#if wxUSE_IPV6

// An IPv6 address
class WXDLLIMPEXP_NET wxIPV6address : public wxIPaddress
{
public:
    wxIPV6address();
    wxIPV6address(const wxIPV6address& other);
    virtual ~wxIPV6address();

    // implement wxSockAddress pure virtuals:

    virtual int Type() { return wxSockAddress::IPV6; }
    virtual wxSockAddress *Clone() const { return new wxIPV6address(*this); }


    // implement wxIPaddress pure virtuals:

    virtual bool Hostname(const wxString& name);
    virtual bool Service(const wxString& name);
    virtual bool Service(unsigned short port);

    // localhost (0000:0000:0000:0000:0000:0000:0000:0001 (::1))
    virtual bool LocalHost();
    virtual bool IsLocalHost() const;

    // any (0000:0000:0000:0000:0000:0000:0000:0000 (::))
    virtual bool AnyAddress();
    // all (?)
    virtual bool BroadcastAddress();

    // 3ffe:ffff:0100:f101:0210:a4ff:fee3:9566
    virtual wxString IPAddress() const;

    virtual wxString Hostname() const;
    virtual unsigned short Service() const;


    // IPv6-specific methods:

    bool Hostname(unsigned char addr[16]);

private:
    wxString m_origHostname;

    DECLARE_DYNAMIC_CLASS(wxIPV6address)
};

#endif // wxUSE_IPV6

#if defined(__UNIX__) && !defined(__WINE__)

#include <sys/socket.h>
#ifndef __VMS__
    #include <sys/un.h>
#endif

// A Unix domain socket address
class WXDLLIMPEXP_NET wxUNIXaddress : public wxSockAddress
{
public:
    wxUNIXaddress();
    wxUNIXaddress(const wxUNIXaddress& other);
    virtual ~wxUNIXaddress();

    void Filename(const wxString& name);
    wxString Filename();

    virtual int Type() { return wxSockAddress::UNIX; }
    virtual wxSockAddress *Clone() const { return new wxUNIXaddress(*this); }

private:
    struct sockaddr_un *m_addr;

    DECLARE_DYNAMIC_CLASS(wxUNIXaddress)
};

#endif // __UNIX__

#endif // wxUSE_SOCKETS

#endif // _WX_SCKADDR_H_
