/////////////////////////////////////////////////////////////////////////////
// Name:        socket.h
// Purpose:     Socket handling classes
// Author:      Guilhem Lavaux
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
#  include <wx/wxprec.h>
#else
#  include <wx/event.h>
#  include <wx/string.h>
#endif

#include <wx/sckaddr.h>
#include "wx/gsocket.h"

class WXDLLEXPORT wxTimer;
class WXDLLEXPORT wxSocketEvent;
class WXDLLEXPORT wxSocketBase : public wxEvtHandler
{
  DECLARE_CLASS(wxSocketBase)
public:

  enum { NONE=0, NOWAIT=1, WAITALL=2, SPEED=4 };
  typedef int wxSockFlags;
  // Type of request

  enum wxSockType { SOCK_CLIENT, SOCK_SERVER, SOCK_INTERNAL, SOCK_UNINIT };
  typedef void (*wxSockCbk)(wxSocketBase& sock,GSocketEvent evt,char *cdata);

protected:
  GSocket *m_socket;			// wxSocket socket
  wxSockFlags m_flags;			// wxSocket flags
  wxSockType m_type;			// wxSocket type
  GSocketEventFlags m_neededreq;	// Specify which requet signals we need
  size_t m_lcount;			// Last IO request size
  unsigned long m_timeout;		// IO timeout value

  char *m_unread;			// Pushback buffer
  size_t m_unrd_size;			// Pushback buffer size
  size_t m_unrd_cur;			// Pushback pointer

  wxSockCbk m_cbk;			// C callback
  char *m_cdata;			// C callback data

  bool m_connected;			// Connected ?
  bool m_notify_state;			// Notify state
  int m_id;				// Socket id (for event handler)

  // Defering variables
  enum {
    DEFER_READ, DEFER_WRITE, NO_DEFER
  } m_defering;                         // Defering state
  char *m_defer_buffer;                 // Defering target buffer
  size_t m_defer_nbytes;                // Defering buffer size
  wxTimer *m_defer_timer;		// Timer for defering mode

  wxList m_states;			// Stack of states

public:
  wxSocketBase();
  virtual ~wxSocketBase();
  virtual bool Close();

  // Base IO
  wxSocketBase& Peek(char* buffer, size_t nbytes);
  wxSocketBase& Read(char* buffer, size_t nbytes);
  wxSocketBase& Write(const char *buffer, size_t nbytes);
  wxSocketBase& Unread(const char *buffer, size_t nbytes);
  wxSocketBase& ReadMsg(char *buffer, size_t nbytes);
  wxSocketBase& WriteMsg(const char *buffer, size_t nbytes);
  void Discard();

  // Try not to use this two methods (they sould be protected)
  void CreatePushbackAfter(const char *buffer, size_t size);
  void CreatePushbackBefore(const char *buffer, size_t size);

  // Status
  inline bool Ok() const { return (m_socket != NULL); };
  inline bool Error() const
           { return (GSocket_GetError(m_socket) != GSOCK_NOERROR); };
  inline bool IsConnected() const { return m_connected; };
  inline bool IsDisconnected() const { return !IsConnected(); };
  inline bool IsNoWait() const { return ((m_flags & NOWAIT) != 0); };
  bool IsData() const;
  inline size_t LastCount() const { return m_lcount; }
  inline GSocketError LastError() const { return GSocket_GetError(m_socket); }
  inline wxSockType GetType() const { return m_type; }

  void SetFlags(wxSockFlags _flags);
  wxSockFlags GetFlags() const;
  inline void SetTimeout(unsigned long sec) { m_timeout = sec; }

  // seconds = -1 means infinite wait
  // seconds = 0 means no wait
  // seconds > 0 means specified wait
  bool Wait(long seconds = -1, long microseconds = 0);
  bool WaitForRead(long seconds = -1, long microseconds = 0);
  bool WaitForWrite(long seconds = -1, long microseconds = 0);
  bool WaitForLost(long seconds = -1, long microseconds = 0);

  // Save the current state of Socket
  void SaveState();
  void RestoreState();

  // Setup external callback
  wxSockCbk Callback(wxSockCbk cbk_);
  char *CallbackData(char *data);

  // Setup event handler
  void SetEventHandler(wxEvtHandler& evt_hdlr, int id = -1);

  // Method called when it happens something on the socket
  void SetNotify(GSocketEventFlags flags);
  virtual void OnRequest(GSocketEvent req_evt);

  // Public internal callback
  virtual void OldOnNotify(GSocketEvent WXUNUSED(evt));

  // Some info on the socket...
  virtual bool GetPeer(wxSockAddress& addr_man) const;
  virtual bool GetLocal(wxSockAddress& addr_man) const;

  // Install or uninstall callbacks
  void Notify(bool notify);

  // So you can know what the socket driver is looking for ...
  inline GSocketEventFlags NeededReq() const { return m_neededreq; }

  static GSocketEventFlags EventToNotify(GSocketEvent evt);

protected:
  friend class wxSocketServer;
  friend class wxSocketHandler;
  friend class wxSocketInternal;

#ifdef __SALFORDC__
public:
#endif

  wxSocketBase(wxSockFlags flags, wxSockType type);

#ifdef __SALFORDC__
protected:
#endif

  bool _Wait(long seconds, long microseconds, int type);

  int DeferRead(char *buffer, size_t nbytes);
  int DeferWrite(const char *buffer, size_t nbytes);
  void DoDefer(GSocketEvent evt);

  // Pushback library
  size_t GetPushback(char *buffer, size_t size, bool peek);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketServer : public wxSocketBase
{
  DECLARE_CLASS(wxSocketServer)
public:

  // 'service' can be a name or a port-number

  wxSocketServer(wxSockAddress& addr_man, wxSockFlags flags = wxSocketBase::NONE);

  wxSocketBase* Accept();
  bool AcceptWith(wxSocketBase& sock);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketClient : public wxSocketBase
{
  DECLARE_CLASS(wxSocketClient)
public:

  wxSocketClient(wxSockFlags flags = wxSocketBase::NONE);
  virtual ~wxSocketClient();

  virtual bool Connect(wxSockAddress& addr_man, bool wait = TRUE);

  bool WaitOnConnect(long seconds = -1, long microseconds = 0);

  virtual void OnRequest(GSocketEvent flags);
};

class WXDLLEXPORT wxSocketEvent : public wxEvent {
  DECLARE_DYNAMIC_CLASS(wxSocketEvent)
public:
  wxSocketEvent(int id = 0);

  GSocketEvent SocketEvent() const { return m_skevt; }
  wxSocketBase *Socket() const { return m_socket; }

  void CopyObject(wxObject& obj_d) const;

public:
  GSocketEvent m_skevt;
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
