/////////////////////////////////////////////////////////////////////////////
// Name:        socket.h
// Purpose:     Socket handling classes
// Authors:     Guilhem Lavaux, Guillermo Rodriguez Garcia
// Modified by:
// Created:     April 1997
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NETWORK_SOCKET_H
#define _WX_NETWORK_SOCKET_H

#ifdef __GNUG__
#pragma interface "socket.h"
#endif

#include "wx/defs.h"

#if wxUSE_SOCKETS

// ---------------------------------------------------------------------------
// wxSocket headers
// ---------------------------------------------------------------------------

#ifdef WXPREC
#  include "wx/wxprec.h"
#else
#  include "wx/event.h"
#  include "wx/string.h"
#endif

#include "wx/sckaddr.h"
#include "wx/gsocket.h"

// ------------------------------------------------------------------------
// Types and constants
// ------------------------------------------------------------------------

enum wxSocketNotify
{
  wxSOCKET_INPUT = GSOCK_INPUT,
  wxSOCKET_OUTPUT = GSOCK_OUTPUT,
  wxSOCKET_CONNECTION = GSOCK_CONNECTION,
  wxSOCKET_LOST = GSOCK_LOST
};

enum
{
  wxSOCKET_INPUT_FLAG = GSOCK_INPUT_FLAG,
  wxSOCKET_OUTPUT_FLAG = GSOCK_OUTPUT_FLAG,
  wxSOCKET_CONNECTION_FLAG = GSOCK_CONNECTION_FLAG,
  wxSOCKET_LOST_FLAG = GSOCK_LOST_FLAG
};

typedef GSocketEventFlags wxSocketEventFlags;

enum wxSocketError
{
  wxSOCKET_NOERROR = GSOCK_NOERROR,
  wxSOCKET_INPOP = GSOCK_INVOP,
  wxSOCKET_IOERR = GSOCK_IOERR,
  wxSOCKET_INVADDR = GSOCK_INVADDR,
  wxSOCKET_INVSOCK = GSOCK_INVSOCK,
  wxSOCKET_NOHOST = GSOCK_NOHOST,
  wxSOCKET_INVPORT = GSOCK_INVPORT,
  wxSOCKET_WOULDBLOCK = GSOCK_WOULDBLOCK,
  wxSOCKET_TIMEDOUT = GSOCK_TIMEDOUT,
  wxSOCKET_MEMERR = GSOCK_MEMERR
};

enum
{
  wxSOCKET_NONE = 0,
  wxSOCKET_NOWAIT = 1,
  wxSOCKET_WAITALL = 2,
  wxSOCKET_BLOCK = 4
};

// Type of socket
enum wxSocketType
{
  wxSOCKET_UNINIT,
  wxSOCKET_CLIENT,
  wxSOCKET_SERVER,
  wxSOCKET_BASE,
  wxSOCKET_DATAGRAM
};

typedef int wxSocketFlags;


// old names

#if WXWIN_COMPATIBILITY

typedef wxSocketType wxSockType;
typedef wxSocketFlags wxSockFlags;

#endif // WXWIN_COMPATIBILITY

// --------------------------------------------------------------------------
// wxSocketBase
// --------------------------------------------------------------------------

class WXDLLEXPORT wxSocketBase : public wxObject
{
  DECLARE_CLASS(wxSocketBase)

public:

#if WXWIN_COMPATIBILITY
  enum
  {
    NONE = wxSOCKET_NONE,
    NOWAIT = wxSOCKET_NOWAIT,
    WAITALL = wxSOCKET_WAITALL,
    SPEED = wxSOCKET_BLOCK
  };

  enum
  {
    SOCK_UNINIT = wxSOCKET_UNINIT,
    SOCK_CLIENT = wxSOCKET_CLIENT,
    SOCK_SERVER = wxSOCKET_SERVER,
    SOCK_INTERNAL = wxSOCKET_BASE,
    SOCK_DATAGRAM = wxSOCKET_DATAGRAM
  };
#endif // WXWIN_COMPATIBILITY

  typedef void (*wxSockCbk)(wxSocketBase& sock, wxSocketNotify evt, char *cdata);

public:

  // Public interface
  // ----------------

  // ctors and dtors
  wxSocketBase();
  wxSocketBase(wxSocketFlags flags, wxSocketType type);
  virtual ~wxSocketBase();
  void Init();
  bool Destroy();

  // state
  inline bool Ok() const { return (m_socket != NULL); };
  inline bool Error() const { return m_error; };
  inline bool IsConnected() const { return m_connected; };
  inline bool IsData() { return WaitForRead(0, 0); };
  inline bool IsDisconnected() const { return !IsConnected(); };
  inline wxUint32 LastCount() const { return m_lcount; }
  inline wxSocketError LastError() const { return (wxSocketError)GSocket_GetError(m_socket); }
  void SaveState();
  void RestoreState();

  // addresses
  virtual bool GetLocal(wxSockAddress& addr_man) const;
  virtual bool GetPeer(wxSockAddress& addr_man) const;

  // base IO
  virtual bool  Close();
  wxSocketBase& Discard();
  wxSocketBase& Peek(char* buffer, wxUint32 nbytes);
  wxSocketBase& Read(char* buffer, wxUint32 nbytes);
  wxSocketBase& ReadMsg(char *buffer, wxUint32 nbytes);
  wxSocketBase& Unread(const char *buffer, wxUint32 nbytes);
  wxSocketBase& Write(const char *buffer, wxUint32 nbytes);
  wxSocketBase& WriteMsg(const char *buffer, wxUint32 nbytes);

  void InterruptAllWaits() { m_interrupt = TRUE; };
  bool Wait(long seconds = -1, long milliseconds = 0);
  bool WaitForRead(long seconds = -1, long milliseconds = 0);
  bool WaitForWrite(long seconds = -1, long milliseconds = 0);
  bool WaitForLost(long seconds = -1, long milliseconds = 0);

  inline wxSocketFlags GetFlags() const { return m_flags; };
  void SetFlags(wxSocketFlags flags);
  void SetTimeout(long seconds);

  // event handling
  void SetEventHandler(wxEvtHandler& handler, int id = -1);
  void SetNotify(wxSocketEventFlags flags);
  void Notify(bool notify);

  // callbacks - deprecated, avoid if possible
  wxSockCbk Callback(wxSockCbk cbk_);
  char *CallbackData(char *data);


  // Implementation from now on
  // --------------------------

  // do not use, this should be private
  void OnRequest(wxSocketNotify req_evt);

  // do not use, not documented nor supported
  inline bool IsNoWait() const { return ((m_flags & wxSOCKET_NOWAIT) != 0); };
  inline wxSocketType GetType() const { return m_type; }

protected:

  // low level IO
  wxUint32 _Read(char* buffer, wxUint32 nbytes);
  wxUint32 _Write(const char *buffer, wxUint32 nbytes);
  bool _Wait(long seconds, long milliseconds, wxSocketEventFlags flags);

  // pushback buffer
  void Pushback(const char *buffer, wxUint32 size);
  wxUint32 GetPushback(char *buffer, wxUint32 size, bool peek);

protected:

  GSocket      *m_socket;           // GSocket
  wxSocketType  m_type;             // wxSocket type

  // state
  wxSocketFlags m_flags;            // wxSocket flags
  bool          m_connected;        // connected?
  bool          m_establishing;     // establishing connection?
  bool          m_reading;          // busy reading?
  bool          m_writing;          // busy writing?
  bool          m_error;            // did last IO call fail?
  wxUint32      m_lcount;           // last IO transaction size
  unsigned long m_timeout;          // IO timeout value
  wxList        m_states;           // stack of states
  bool          m_interrupt;        // interrupt ongoing wait operations?
  bool          m_beingDeleted;     // marked for delayed deletion?

  // pushback buffer
  char         *m_unread;           // pushback buffer
  wxUint32      m_unrd_size;        // pushback buffer size
  wxUint32      m_unrd_cur;         // pushback pointer (index into buffer)

  // events
  wxEvtHandler *m_handler;          // event handler
  int           m_id;               // socket id
  bool          m_notify_state;     // notify events to users?
  wxSocketEventFlags
                m_neededreq;        // event mask
  wxSockCbk     m_cbk;              // callback
  char         *m_cdata;            // callback data
};


// --------------------------------------------------------------------------
// wxSocketServer
// --------------------------------------------------------------------------

class WXDLLEXPORT wxSocketServer : public wxSocketBase
{
  DECLARE_CLASS(wxSocketServer)

public:
  wxSocketServer(wxSockAddress& addr, wxSocketFlags flags = wxSOCKET_NONE);

  wxSocketBase* Accept(bool wait = TRUE);
  bool AcceptWith(wxSocketBase& socket, bool wait = TRUE);

  bool WaitForAccept(long seconds = -1, long milliseconds = 0);
};


// --------------------------------------------------------------------------
// wxSocketClient
// --------------------------------------------------------------------------

class WXDLLEXPORT wxSocketClient : public wxSocketBase
{
  DECLARE_CLASS(wxSocketClient)

public:
  wxSocketClient(wxSocketFlags flags = wxSOCKET_NONE);
  virtual ~wxSocketClient();

  virtual bool Connect(wxSockAddress& addr, bool wait = TRUE);

  bool WaitOnConnect(long seconds = -1, long milliseconds = 0);
};


// --------------------------------------------------------------------------
// wxDatagramSocket
// --------------------------------------------------------------------------

// WARNING: still in alpha stage

class wxDatagramSocket : public wxSocketBase
{
  DECLARE_CLASS(wxDatagramSocket)

public:
  wxDatagramSocket(wxSockAddress& addr, wxSocketFlags flags = wxSOCKET_NONE);

  wxDatagramSocket& RecvFrom( wxSockAddress& addr,
                              char* buf,
                              wxUint32 nBytes );
  wxDatagramSocket& SendTo( wxSockAddress& addr,
                            const char* buf,
                            wxUint32 nBytes );
};


// --------------------------------------------------------------------------
// wxSocketEvent
// --------------------------------------------------------------------------

class WXDLLEXPORT wxSocketEvent : public wxEvent
{
  DECLARE_DYNAMIC_CLASS(wxSocketEvent)

public:
  wxSocketEvent(int id = 0);

  wxSocketNotify SocketEvent() const { return m_event; }
  wxSocketBase *Socket() const       { return m_socket; }

  void CopyObject(wxObject& object_dest) const;

public:
  wxSocketNotify  m_event;
  wxSocketBase   *m_socket;
};


typedef void (wxEvtHandler::*wxSocketEventFunction)(wxSocketEvent&);

#define EVT_SOCKET(id, func) { wxEVT_SOCKET, id, -1, \
  (wxObjectEventFunction) (wxEventFunction) (wxSocketEventFunction) & func, \
  (wxObject *) NULL  },


#endif
  // wxUSE_SOCKETS

#endif
  // _WX_NETWORK_SOCKET_H
