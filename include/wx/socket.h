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
// wxSocket headers (generic)
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
// GSocket type alias
// ------------------------------------------------------------------------

typedef enum {
  wxSOCKET_INPUT = GSOCK_INPUT,
  wxSOCKET_OUTPUT = GSOCK_OUTPUT,
  wxSOCKET_CONNECTION = GSOCK_CONNECTION,
  wxSOCKET_LOST = GSOCK_LOST
} wxSocketNotify;

enum {
  wxSOCKET_INPUT_FLAG = GSOCK_INPUT_FLAG,
  wxSOCKET_OUTPUT_FLAG = GSOCK_OUTPUT_FLAG,
  wxSOCKET_CONNECTION_FLAG = GSOCK_CONNECTION_FLAG,
  wxSOCKET_LOST_FLAG = GSOCK_LOST_FLAG
};

typedef GSocketEventFlags wxSocketEventFlags;

typedef enum {
  wxSOCKET_NOERROR = GSOCK_NOERROR,
  wxSOCKET_INPOP = GSOCK_INVOP,
  wxSOCKET_IOERR = GSOCK_IOERR,
  wxSOCKET_INVADDR = GSOCK_INVADDR,
  wxSOCKET_INVSOCK = GSOCK_INVSOCK,
  wxSOCKET_NOHOST = GSOCK_NOHOST,
  wxSOCKET_INVPORT = GSOCK_INVPORT,
  wxSOCKET_WOULDBLOCK = GSOCK_WOULDBLOCK,
  wxSOCKET_TIMEDOUT = GSOCK_TIMEDOUT,
  wxSOCKET_MEMERR = GSOCK_MEMERR,
  wxSOCKET_BUSY
} wxSocketError;

enum {
  wxSOCKET_NONE = 0,
  wxSOCKET_NOWAIT = 1,
  wxSOCKET_WAITALL = 2,
  wxSOCKET_BLOCK = 4
};

typedef int wxSockFlags;

// ------------------------------------------------------------------------
// wxSocket base
// ------------------------------------------------------------------------

class WXDLLEXPORT wxTimer;
class WXDLLEXPORT wxSocketEvent;
class WXDLLEXPORT wxSocketBase : public wxEvtHandler
{
  DECLARE_CLASS(wxSocketBase)
public:

  enum {
    NONE = wxSOCKET_NONE,
    NOWAIT = wxSOCKET_NOWAIT,
    WAITALL = wxSOCKET_WAITALL,
    SPEED = wxSOCKET_BLOCK
  };

  // Type of request

  enum wxSockType { SOCK_CLIENT, SOCK_SERVER, SOCK_INTERNAL, SOCK_UNINIT };
  typedef void (*wxSockCbk)(wxSocketBase& sock, wxSocketNotify evt, char *cdata);

protected:
  GSocket *m_socket;            // GSocket
  int m_id;                     // Socket id (for event handler)

  // Attributes
  wxSockFlags m_flags;			// wxSocket flags
  wxSockType m_type;			// wxSocket type
  wxSocketEventFlags m_neededreq;   // Event mask
  bool m_notify_state;          // Notify events to users?
  bool m_connected;             // Connected ?
  bool m_establishing;          // Establishing connection ?
  bool m_reading;               // Busy reading?
  bool m_writing;               // Busy writing?
  bool m_error;                 // Did last IO call fail ?
  wxUint32 m_lcount;            // Last IO transaction size
  unsigned long m_timeout;		// IO timeout value
  wxList m_states;              // Stack of states

  char *m_unread;               // Pushback buffer
  wxUint32 m_unrd_size;			// Pushback buffer size
  wxUint32 m_unrd_cur;          // Pushback pointer (index into buffer)

  // Async IO variables
  enum
  {
    NO_DEFER = 0,
    DEFER_READ = 1,
    DEFER_WRITE = 2
  } m_defering;                 // Defering state
  char *m_defer_buffer;         // Defering target buffer
  wxUint32 m_defer_nbytes;      // Defering buffer size
  wxTimer *m_defer_timer;       // Timer for defering mode

/*
  char *m_read_buffer;          // Target buffer (read)
  char *m_write_buffer;         // Target buffer (write)
  wxUint32 m_read_nbytes;       // Buffer size (read)
  wxUint32 m_write_nbytes;      // Buffer size (write)
  wxTimer *m_read_timer;        // Timer (read)
  wxTimer *m_write_timer;       // Timer (write)
*/

  wxSockCbk m_cbk;              // C callback
  char *m_cdata;                // C callback data

public:
  wxSocketBase();
  virtual ~wxSocketBase();
  virtual bool Close();

  // Base IO
  wxSocketBase& Peek(char* buffer, wxUint32 nbytes);
  wxSocketBase& Read(char* buffer, wxUint32 nbytes);
  wxSocketBase& Write(const char *buffer, wxUint32 nbytes);
  wxSocketBase& Unread(const char *buffer, wxUint32 nbytes);
  wxSocketBase& ReadMsg(char *buffer, wxUint32 nbytes);
  wxSocketBase& WriteMsg(const char *buffer, wxUint32 nbytes);
  wxSocketBase& Discard();

  // Status
  inline bool Ok() const { return (m_socket != NULL); };
  inline bool Error() const { return m_error; };
  inline bool IsConnected() const { return m_connected; };
  inline bool IsDisconnected() const { return !IsConnected(); };
  inline bool IsNoWait() const { return ((m_flags & NOWAIT) != 0); };
  inline bool IsData() { return WaitForRead(0, 0); };
  inline wxUint32 LastCount() const { return m_lcount; }
  inline wxSocketError LastError() const { return (wxSocketError)GSocket_GetError(m_socket); }
  inline wxSockType GetType() const { return m_type; }

  // Some info on the socket...
  virtual bool GetPeer(wxSockAddress& addr_man) const;
  virtual bool GetLocal(wxSockAddress& addr_man) const;

  // Set attributes and flags
  void SetTimeout(long seconds);
  void SetFlags(wxSockFlags flags);
  inline wxSockFlags GetFlags() const { return m_flags; };

  // Wait functions
  //   seconds = -1 means default timeout (change with SetTimeout)
  //   seconds, milliseconds = 0 means no wait
  //   seconds, milliseconds > 0 means specified wait
  bool Wait(long seconds = -1, long milliseconds = 0);
  bool WaitForRead(long seconds = -1, long milliseconds = 0);
  bool WaitForWrite(long seconds = -1, long milliseconds = 0);
  bool WaitForLost(long seconds = -1, long milliseconds = 0);

  // Save the current state of Socket
  void SaveState();
  void RestoreState();

  // Setup event handler
  void SetEventHandler(wxEvtHandler& evt_hdlr, int id = -1);

  // Tell wxSocket which events to notify
  void SetNotify(wxSocketEventFlags flags);
  void Notify(bool notify);
  static wxSocketEventFlags EventToNotify(wxSocketNotify evt);
  inline wxSocketEventFlags NeededReq() const { return m_neededreq; }

  // External callback
  wxSockCbk Callback(wxSockCbk cbk_);
  char *CallbackData(char *data);

  // Public internal callback
  virtual void OldOnNotify(wxSocketNotify WXUNUSED(evt));

  // Do NOT use this function; it should be protected!
  void OnRequest(wxSocketNotify req_evt);

protected:
  friend class wxSocketServer;
  friend class wxSocketClient;
  friend class wxSocketHandler;

#ifdef __SALFORDC__
public:
#endif

  wxSocketBase(wxSockFlags flags, wxSockType type);

#ifdef __SALFORDC__
protected:
#endif

  // Low level IO
  wxUint32 _Read(char* buffer, wxUint32 nbytes);
  wxUint32 _Write(const char *buffer, wxUint32 nbytes);
  bool _Wait(long seconds, long milliseconds, wxSocketEventFlags flags);

  wxUint32 DeferRead(char *buffer, wxUint32 nbytes);
  wxUint32 DeferWrite(const char *buffer, wxUint32 nbytes);
  void DoDefer();

  // Pushbacks
  void Pushback(const char *buffer, wxUint32 size);
  wxUint32 GetPushback(char *buffer, wxUint32 size, bool peek);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketServer : public wxSocketBase
{
  DECLARE_CLASS(wxSocketServer)
public:
  // 'service' can be a name or a port-number

  wxSocketServer(wxSockAddress& addr_man, wxSockFlags flags = wxSocketBase::NONE);

  wxSocketBase* Accept(bool wait = TRUE);
  bool AcceptWith(wxSocketBase& sock, bool wait = TRUE);

  bool WaitForAccept(long seconds = -1, long milliseconds = 0);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketClient : public wxSocketBase
{
  DECLARE_CLASS(wxSocketClient)
public:

  wxSocketClient(wxSockFlags flags = wxSocketBase::NONE);
  virtual ~wxSocketClient();

  virtual bool Connect(wxSockAddress& addr_man, bool wait = TRUE);

  bool WaitOnConnect(long seconds = -1, long milliseconds = 0);
};

class WXDLLEXPORT wxSocketEvent : public wxEvent {
  DECLARE_DYNAMIC_CLASS(wxSocketEvent)
public:
  wxSocketEvent(int id = 0);

  wxSocketNotify SocketEvent() const { return m_skevt; }
  wxSocketBase *Socket() const { return m_socket; }

  void CopyObject(wxObject& obj_d) const;

public:
  wxSocketNotify m_skevt;
  wxSocketBase *m_socket;
};

typedef void (wxEvtHandler::*wxSocketEventFunction)(wxSocketEvent&);

#define EVT_SOCKET(id, func) { wxEVT_SOCKET, id, -1, \
  (wxObjectEventFunction) (wxEventFunction) (wxSocketEventFunction) & func, \
  (wxObject *) NULL  },

#endif
  // wxUSE_SOCKETS

#endif
  // _WX_NETWORK_SOCKET_H
