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
#pragma interface
#endif

// ---------------------------------------------------------------------------
// Windows(tm) specific
// ---------------------------------------------------------------------------
#if defined(__WINDOWS__) && defined(WXSOCK_INTERNAL)
#include <winsock.h>
#include <wx/msw/private.h>

struct wxSockInternal {
  UINT my_msg;
};

struct wxSockHandlerInternal {
  HWND sockWin;
  UINT firstAvailableMsg;
};
#endif // defined(__WINDOWS__) && defined(WXSOCK_INTERNAL)

// ---------------------------------------------------------------------------
// Unix specific
// ---------------------------------------------------------------------------
#if defined(__UNIX__) && defined(WXSOCK_INTERNAL)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// ---------------------------------------------------------------------------
// Athena specific
// ---------------------------------------------------------------------------
#if defined(__WXXT__) || defined(__WXMOTIF__)
#include <X11/Intrinsic.h>

struct wxSockInternal {
  XtInputId sock_inputid, sock_outputid, sock_exceptid;
};
#endif

// ---------------------------------------------------------------------------
// GTK specific
// ---------------------------------------------------------------------------
#if defined(__WXGTK__)
#include <gdk/gdk.h>

struct wxSockInternal {
  gint sock_inputid, sock_outputid, sock_exceptid;
};
#endif

#endif // defined(__UNIX__) && defined(WXSOCK_INTERNAL)

// ---------------------------------------------------------------------------
// wxSocket headers (generic)
// ---------------------------------------------------------------------------
#ifdef WXPREC
#include <wx/wxprec.h>
#else
#include <wx/wx.h>
#endif
#include "wx/sckaddr.h"

class WXDLLEXPORT wxSocketEvent;
class WXDLLEXPORT wxSocketHandler;
class WXDLLEXPORT wxSocketBase : public wxEvtHandler
{
  DECLARE_CLASS(wxSocketBase)
public:

  enum wxSockFlags { NONE=0, NOWAIT=1, WAITALL=2, SPEED=4 };
  // Type of request
  enum { REQ_READ=0x1, REQ_PEEK=0x2, REQ_WRITE=0x4, REQ_LOST=0x8,
         REQ_ACCEPT=0x10, REQ_CONNECT=0x20};
  enum { EVT_READ=0, EVT_PEEK=1, EVT_WRITE=2, EVT_LOST=3, EVT_ACCEPT=4,
         EVT_CONNECT=5 };

  typedef int wxRequestNotify;
  typedef int wxRequestEvent;
  typedef void (*wxSockCbk)(wxSocketBase& sock,wxRequestEvent evt,char *cdata);

protected:
  wxList req_list[EVT_WRITE+1];

  // Internal use for SaveState() and RestoreState()
  class wxSockState : public wxObject {
  public:
    bool cbk_on;
    wxSockCbk cbk;
    char *cdata;
    bool notif;
    wxRequestNotify cbk_set;
    wxSockFlags flags;
  };
  typedef struct {
    char sig[4];
    char len[4];
  } SockMsg;
  enum wxSockType { SOCK_CLIENT, SOCK_SERVER, SOCK_INTERNAL, SOCK_UNINIT }; 

  wxSockFlags m_flags;
  wxSockType m_type;			// wxSocket type
  bool m_connected, m_connecting;	// State of the socket
  int m_fd;				// Socket file descriptors
  int m_waitflags;			// Wait flags
  wxList m_states;			// States list
  wxSockCbk m_cbk;			// C callback
  char *m_cdata;			// C callback data
  int m_id;				// Socket id (for event handler)
  wxSocketHandler *m_handler;		// the current socket handler
  wxRequestNotify m_neededreq;		// Specify which requet signals we need
  bool m_cbkon;
  char *m_unread;			// The unread buf
  size_t m_unrd_size;			// The size of the unread buf
  bool m_processing;			// To prevent some endless loop
  unsigned long m_timeout;
  int m_wantbuf;
  size_t m_lcount;			// Last IO request size
  int m_error;				// Last IO error
  bool m_notifyme;

  struct wxSockInternal *m_internal;	// System specific variables

public:
  wxSocketBase();
  virtual ~wxSocketBase();
  virtual bool Close();

  // Base IO
  wxSocketBase& Peek(char* buffer, size_t nbytes);
  wxSocketBase& Read(char* buffer, size_t nbytes);
  wxSocketBase& Write(const char *buffer, size_t nbytes);
  wxSocketBase& WriteMsg(const char *buffer, size_t nbytes);
  wxSocketBase& ReadMsg(char* buffer, size_t nbytes);
  wxSocketBase& Unread(const char *buffer, size_t nbytes);
  void Discard();

  // Try not to use this two methods (they sould be protected)
  void CreatePushbackAfter(const char *buffer, size_t size);
  void CreatePushbackBefore(const char *buffer, size_t size);

  // Status
  inline bool Ok() const { return (m_fd < 0 ? 0 : 1); };
  inline bool Error() const { return (m_error != 0); };
  inline bool IsConnected() const { return m_connected; };
  inline bool IsDisconnected() const { return !IsConnected(); };
  inline bool IsNoWait() const { return m_flags & NOWAIT; };
  bool IsData() const;
  inline size_t LastCount() const { return m_lcount; }
  inline int LastError() const { return m_error; }
  
  inline void SetFlags(wxSockFlags _flags);
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
  void SetNotify(wxRequestNotify flags);
  virtual void OnRequest(wxRequestEvent req_evt);

  // Public internal callback
  virtual void OldOnNotify(wxRequestEvent WXUNUSED(evt));

  // Some info on the socket...
  virtual bool GetPeer(wxSockAddress& addr_man) const;
  virtual bool GetLocal(wxSockAddress& addr_man) const;

  // Install or uninstall callbacks
  void Notify(bool notify);

  // So you can know what the socket driver is looking for ...
  inline wxRequestNotify NeededReq() const { return m_neededreq; }

  static wxRequestNotify EventToNotify(wxRequestEvent evt);

protected:
  friend class wxSocketServer;
  friend class wxSocketHandler;

  wxSocketBase(wxSockFlags flags, wxSockType type);
  
  bool _Wait(long seconds, long microseconds, int type);
  
  // Set "my" handler
  inline virtual void SetHandler(wxSocketHandler *handler)
         { m_handler = handler; }

  // Activate or disactivate callback
  void SetupCallbacks();
  void DestroyCallbacks();

  // Pushback library
  size_t GetPushback(char *buffer, size_t size, bool peek);

  // To prevent many read or write on the same socket at the same time
  //   ==> cause strange things :-)
  void WantSpeedBuffer(char *buffer, size_t size, wxRequestEvent req);
  void WantBuffer(char *buffer, size_t size, wxRequestEvent req);

  virtual bool DoRequests(wxRequestEvent req);
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
  virtual void OnRequest(wxRequestEvent flags);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketClient : public wxSocketBase
{
  DECLARE_CLASS(wxSocketClient)
public:

  wxSocketClient(wxSockFlags flags = wxSocketBase::NONE);
  virtual ~wxSocketClient();

  virtual bool Connect(wxSockAddress& addr_man, bool wait = TRUE);

  bool WaitOnConnect(long seconds = -1);

  virtual void OnRequest(wxRequestEvent flags);
};

////////////////////////////////////////////////////////////////////////

class WXDLLEXPORT wxSocketHandler : public wxObject
{
  DECLARE_CLASS(wxSocketHandler)
protected:
  static wxSocketHandler *master;
#if defined(__WINDOWS__)
  wxList *smsg_list;
  struct wxSockHandlerInternal *internal;
#endif
  wxList *socks;

public:
  enum SockStatus { SOCK_NONE, SOCK_DATA, SOCK_CONNECT, SOCK_DISCONNECT,
		    SOCK_ERROR };

  wxSocketHandler();
  virtual ~wxSocketHandler();
  
  void Register(wxSocketBase* sock);
  void UnRegister(wxSocketBase* sock);
  
  unsigned long Count() const;
  
  // seconds = -1 means indefinite wait
  // seconds = 0 means no wait
  // seconds > 0 means specified wait
  
  int Wait(long seconds = -1, long microseconds = 0);
  void YieldSock();
  
  wxSocketServer *CreateServer
                       (wxSockAddress& addr,
			wxSocketBase::wxSockFlags flags = wxSocketBase::NONE);
  wxSocketClient *CreateClient
                       (wxSocketBase::wxSockFlags flags = wxSocketBase::NONE);

  // Create or reuse a socket handler
  static wxSocketHandler& Master()
  { return *((master) ? (master) : (master = new wxSocketHandler())); }

#if defined(WXSOCK_INTERNAL) && defined(__WINDOWS__)

  friend LRESULT APIENTRY _EXPORT wxSocketHandlerWndProc(HWND hWnd,
                        UINT message, WPARAM wParam, LPARAM lParam);

  UINT NewMessage(wxSocketBase *sock);
  void DestroyMessage(UINT msg);

  HWND GetHWND() const;
#endif
};

class WXDLLEXPORT wxSocketEvent : public wxEvent {
  DECLARE_DYNAMIC_CLASS(wxSocketEvent)
public:
  wxSocketEvent(int id = 0);

  wxSocketBase::wxRequestEvent SocketEvent() const { return m_skevt; }
public:
  wxSocketBase::wxRequestEvent m_skevt;
};

typedef void (wxEvtHandler::*wxSocketEventFunction)(wxSocketEvent&);

#define wxEVT_SOCKET wxEVT_FIRST+301

#define EVT_SOCKET(id, func) { wxEVT_SOCKET, id, 0, (wxObjectEventFunction) (wxEventFunction) (wxSocketEventFunction) & func },

#endif
