////////////////////////////////////////////////////////////////////////////////
// Name:       socket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux (completely rewritten from a basic API of Andrew
//             Davidson(1995) in wxWeb)
// Created:    April 1997
// Updated:    March 1998
// Copyright:  (C) 1998, 1997, Guilhem Lavaux
// RCS_ID:     $Id$
// License:    see wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "socket.h"
#endif

#ifdef __MWERKS__
typedef int socklen_t ;
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SOCKETS

/////////////////////////////////////////////////////////////////////////////
// wxWindows headers
/////////////////////////////////////////////////////////////////////////////
#include <wx/defs.h>
#include <wx/object.h>
#include <wx/string.h>
#include <wx/timer.h>
#include <wx/utils.h>

// Not enough OS behaviour defined for wxStubs
#ifndef __WXSTUBS__

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/////////////////////////////////////////////////////////////////////////////
// System specific headers
/////////////////////////////////////////////////////////////////////////////
#ifdef __WXMAC__
// in order to avoid problems with our c library and double definitions
#define close closesocket
#define ioctl ioctlsocket

#include <wx/mac/macsock.h>
extern GUSISpinFn GUSISpin;
#define PROCESS_EVENTS() wxMacProcessEvents()
const short kwxMacNetEventsMax = 1000 ;
short wxMacNetEventsTop = 0 ;
short wxMacNetEventsBottom = 0 ;
short wxMacNetEventsEvents[kwxMacNetEventsMax] ;
void *wxMacNetEventsReferences[kwxMacNetEventsMax] ;

#define FD_READ 1
#define FD_WRITE 2
#define FD_CLOSE 4
#define FD_ACCEPT 8
#define FD_CONNECT 16
#define FD_READY 32

extern "C" void wxMacSocketHandlerProc( void *refcon , short event ) ; // adds events
extern "C" void wxMacSocketOnRequestProc( void *refcon , short event ) ; // consumes them
extern "C" void GUSISetReference( short sock , short eventmask , void * data ) ;
void wxMacProcessEvents() ;
#endif

#if defined(__WINDOWS__)
    #include <winsock.h>
#endif // __WINDOWS__

#if defined(__UNIX__)

#ifdef VMS
    #include <socket.h>
#else // !VMS
    #include <sys/socket.h>
#endif // VMS/!VMS

#include <sys/ioctl.h>

#include <sys/time.h>
#include <unistd.h>

#ifdef sun
    #include <sys/filio.h>
#endif

#endif // __UNIX__

#include <signal.h>
#include <errno.h>

#ifdef __VISUALC__
    #include <io.h>
#endif

#if defined(__WXMOTIF__) || defined(__WXXT__)
    #include <X11/Intrinsic.h>

    /////////////////////////////
    // Needs internal variables
    /////////////////////////////
    #ifdef __WXXT__
        #define Uses_XtIntrinsic
    #endif
#endif // Motif or Xt

#if defined(__WXGTK__)
    #include <gtk/gtk.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// wxSocket headers
/////////////////////////////////////////////////////////////////////////////
#include "wx/module.h"

#define WXSOCK_INTERNAL

#include "wx/sckaddr.h"
#include "wx/socket.h"

/////////////////////////////////////////////////////////////////////////////
// Some patch ///// BEGIN
/////////////////////////////////////////////////////////////////////////////
#ifdef __WINDOWS__
#define close closesocket
#define ioctl ioctlsocket
#ifdef errno
#undef errno
#endif
#define errno WSAGetLastError()
#ifdef EWOULDBLOCK
#undef EWOULDBLOCK
#endif
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ETIMEDOUT WSAETIMEDOUT
#undef EINTR
#define EINTR WSAEINTR
#endif

#ifndef __WINDOWS__
#define INVALID_SOCKET -1
#endif

#ifdef __WXMOTIF__
#define wxAPP_CONTEXT ((XtAppContext)wxTheApp->GetAppContext())
#endif

#ifdef __WINDOWS__
// This is an MS TCP/IP routine and is not needed here. Some WinSock
// implementations (such as PC-NFS) will require you to include this
// or a similar routine (see appendix in WinSock doc or help file).

#if defined( NEED_WSAFDIsSet ) || defined( __VISUALC__ )
int PASCAL FAR __WSAFDIsSet(SOCKET fd, fd_set FAR *set)
{
  int i = set->fd_count;

  while (i--)
  {
    if (set->fd_array[i] == fd)
      return 1;
  }

  return 0;
}
#endif
#endif

#if defined(__WINDOWS__)
    #define PROCESS_EVENTS() wxYield()
#elif defined(__WXXT__) || defined(__WXMOTIF__)
    #define PROCESS_EVENTS() XtAppProcessEvent(wxAPP_CONTEXT, XtIMAll)
#elif defined(__WXGTK__)
    #define PROCESS_EVENTS() gtk_main_iteration()
#endif

/////////////////////////////////////////////////////////////////////////////
// Some patch ///// END
/////////////////////////////////////////////////////////////////////////////

#ifdef GetClassInfo
#undef GetClassInfo
#endif

// --------------------------------------------------------------
// Module
// --------------------------------------------------------------
class wxSocketModule: public wxModule 
{
  DECLARE_DYNAMIC_CLASS(wxSocketModule)
public:
  wxSocketModule() {}
  bool OnInit();
  void OnExit();
};

// --------------------------------------------------------------
// ClassInfos
// --------------------------------------------------------------
#if !USE_SHARED_LIBRARY
    IMPLEMENT_CLASS(wxSocketBase, wxObject)
    IMPLEMENT_CLASS(wxSocketServer, wxSocketBase)
    IMPLEMENT_CLASS(wxSocketClient, wxSocketBase)
    IMPLEMENT_CLASS(wxSocketHandler, wxObject)
    IMPLEMENT_DYNAMIC_CLASS(wxSocketEvent, wxEvent)
    IMPLEMENT_DYNAMIC_CLASS(wxSocketModule, wxModule)
#endif

class wxSockWakeUp : public wxTimer 
{
public:
  int *my_id;
  int n_val;
  wxSocketBase *sock;

  wxSockWakeUp(wxSocketBase *_sock, int *id, int new_val) 
  {
    my_id = id; n_val = new_val;
    sock = _sock;
  }
  virtual void Notify() 
  {
    *my_id = n_val;
    if (sock) sock->Notify(FALSE);
  }
};

/// Socket request
class SockRequest : public wxObject 
{
public:
  char *buffer;
  size_t size, nbytes;
  bool done;
  int error;
  wxSockWakeUp *auto_wakeup;
  wxSocketBase::wxRequestNotify type;
};


/////////////////////////////////////////////////////////////////////////////
// Some internal define
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------
// --------- wxSocketBase CONSTRUCTOR ---------------------------
// --------------------------------------------------------------
wxSocketBase::wxSocketBase(wxSocketBase::wxSockFlags _flags,
         wxSocketBase::wxSockType _type) :
  wxEvtHandler(),
  m_flags(_flags), m_type(_type), m_connected(FALSE), m_connecting(FALSE),
  m_fd(INVALID_SOCKET), m_waitflags(0), m_cbk(0), m_cdata(0), m_id(-1),
  m_handler(0),
  m_neededreq((wxRequestNotify)(REQ_READ | REQ_LOST)),
  m_cbkon(FALSE),
  m_unread(NULL), m_unrd_size(0),
  m_processing(FALSE),
  m_timeout(3600), m_wantbuf(0)
{
  m_internal = new wxSockInternal;
#if defined(__WXXT__) || defined(__WXMOTIF__) || defined(__WXGTK__)
  m_internal->sock_inputid = 0;
  m_internal->sock_outputid = 0;
  m_internal->sock_exceptid = 0;
#endif
#ifdef __WINDOWS__
  m_internal->my_msg = 0;
#endif
}

wxSocketBase::wxSocketBase() :
  wxEvtHandler(),
  m_flags(WAITALL), m_type(SOCK_UNINIT), m_connected(FALSE),
  m_connecting(FALSE), m_fd(INVALID_SOCKET), m_waitflags(0),
  m_cbk(0), m_cdata(0),
  m_id(-1), m_handler(0),
  m_neededreq((wxRequestNotify)(REQ_READ | REQ_LOST)),
  m_cbkon(FALSE),
  m_unread(NULL), m_unrd_size(0),
  m_processing(FALSE),
  m_timeout(3600), m_wantbuf(0)
{
  m_internal = new wxSockInternal;
#if defined(__WXXT__) || defined(__WXMOTIF__) || defined(__WXGTK__)
  m_internal->sock_inputid = 0;
  m_internal->sock_outputid = 0;
  m_internal->sock_exceptid = 0;
#endif
#ifdef __WINDOWS__
  m_internal->my_msg = 0;
#endif
}

// --------------------------------------------------------------
// wxSocketBase
// --------------------------------------------------------------

wxSocketBase::~wxSocketBase()
{
  DestroyCallbacks();
  Close();

  if (m_unread)
    free(m_unread);
  if (m_handler) 
  {
#ifdef __WINDOWS__
    if (m_internal->my_msg)
      m_handler->DestroyMessage(m_internal->my_msg);
#endif
    m_handler->UnRegister(this);
  }
  m_states.DeleteContents(TRUE);

  delete m_internal;
}

bool wxSocketBase::Close()
{
  if (m_fd != INVALID_SOCKET) 
  {
    for (int i=0;i<3;i++) 
    {
      wxNode *n, *node = req_list[i].First();

      while (node) 
      {
        SockRequest *req = (SockRequest *)node->Data();
        req->done = TRUE;

        n = node->Next();
        delete node;
        node = n;
      }
    }

    DestroyCallbacks();
    shutdown(m_fd, 2);
    close(m_fd);
    m_fd = INVALID_SOCKET;
    m_connected = FALSE;
  }

  return TRUE;
}

// --------------------------------------------------------------
// wxSocketBase base IO function
// --------------------------------------------------------------

wxSocketBase& wxSocketBase::Read(char* buffer, size_t nbytes)
{
  size_t count;

  count = GetPushback(buffer, nbytes, FALSE);
  nbytes -= count;
  buffer += count;

  // If we have got the whole needed buffer or if we don't want to
  // wait then it returns immediately.
  if (!nbytes || (count && !(m_flags & WAITALL)) ) {
    m_lcount = count;
    return *this;
  }

  m_lcount = 0;
  WantBuffer(buffer, nbytes, EVT_READ);
  m_lcount += count;

  return *this;
}

wxSocketBase& wxSocketBase::Peek(char* buffer, size_t nbytes)
{
  size_t count;

  count = GetPushback(buffer, nbytes, TRUE);
  if (nbytes-count == 0) 
  {
    m_lcount = nbytes;
    return *this;
  }
  buffer += count;
  nbytes -= count;

  m_lcount = 0;
  WantBuffer(buffer, nbytes, EVT_PEEK);
  m_lcount += count;

  return *this;
}

wxSocketBase& wxSocketBase::Write(const char *buffer, size_t nbytes)
{
  WantBuffer((char *)buffer, nbytes, EVT_WRITE);
  return *this;
}

wxSocketBase& wxSocketBase::ReadMsg(char* buffer, size_t nbytes)
{
  SockMsg msg;
  size_t len, len2, sig;

  Read((char *)&msg, sizeof(msg));
  if (m_lcount != sizeof(msg))
    return *this;

  sig = msg.sig[0] & 0xff;
  sig |= (size_t)(msg.sig[1] & 0xff) << 8;
  sig |= (size_t)(msg.sig[2] & 0xff) << 16;
  sig |= (size_t)(msg.sig[3] & 0xff) << 24;

  if (sig != 0xfeeddead)
    return *this;
  len = msg.len[0] & 0xff;
  len |= (size_t)(msg.len[1] & 0xff) << 8;
  len |= (size_t)(msg.len[2] & 0xff) << 16;
  len |= (size_t)(msg.len[3] & 0xff) << 24;
  len2 = len;
  if (len > nbytes)
    len = nbytes;
  else
    len2 = 0;

  if (Read(buffer, len).LastCount() != len)
    return *this;
  if (len2 && (Read(NULL, len2).LastCount() != len2))
    return *this;
  if (Read((char *)&msg, sizeof(msg)).LastCount() != sizeof(msg))
    return *this;

  sig = msg.sig[0] & 0xff;
  sig |= (size_t)(msg.sig[1] & 0xff) << 8;
  sig |= (size_t)(msg.sig[2] & 0xff) << 16;
  sig |= (size_t)(msg.sig[3] & 0xff) << 24;
// ERROR
  if (sig != 0xdeadfeed)
    return *this;

  return *this;
}

wxSocketBase& wxSocketBase::WriteMsg(const char *buffer, size_t nbytes)
{
  SockMsg msg;

  // warning about 'cast truncates constant value'
#ifdef __VISUALC__
    #pragma warning(disable: 4310)
#endif // __VISUALC__

  msg.sig[0] = (char) 0xad;
  msg.sig[1] = (char) 0xde;
  msg.sig[2] = (char) 0xed;
  msg.sig[3] = (char) 0xfe;

  msg.len[0] = (char) nbytes & 0xff;
  msg.len[1] = (char) (nbytes >> 8) & 0xff;
  msg.len[2] = (char) (nbytes >> 16) & 0xff;
  msg.len[3] = (char) (nbytes >> 24) & 0xff;

  if (Write((char *)&msg, sizeof(msg)).LastCount() < sizeof(msg))
    return *this;
  if (Write(buffer, nbytes).LastCount() < nbytes)
    return *this;

  msg.sig[0] = (char) 0xed;
  msg.sig[1] = (char) 0xfe;
  msg.sig[2] = (char) 0xad;
  msg.sig[3] = (char) 0xde;
  msg.len[0] = msg.len[1] = msg.len[2] = msg.len[3] = (char) 0;
  Write((char *)&msg, sizeof(msg));

  return *this;

#ifdef __VISUALC__
    #pragma warning(default: 4310)
#endif // __VISUALC__
}

wxSocketBase& wxSocketBase::Unread(const char *buffer, size_t nbytes)
{
  CreatePushbackAfter(buffer, nbytes);
  return *this;
}

bool wxSocketBase::IsData() const
{
  struct timeval tv;
  fd_set sock_set;

  if (m_fd < 0)
    return FALSE;
  if (m_unrd_size > 0)
    return TRUE;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&sock_set);
  FD_SET(m_fd, &sock_set);
  select(FD_SETSIZE, &sock_set, NULL, NULL, &tv);
  return (FD_ISSET(m_fd, &sock_set) != 0);
}

// ---------------------------------------------------------------------
// --------- wxSocketBase Discard(): deletes all byte in the input queue
// ---------------------------------------------------------------------
void wxSocketBase::Discard()
{
#define MAX_BUFSIZE (10*1024)
  char *my_data = new char[MAX_BUFSIZE];
  size_t recv_size = MAX_BUFSIZE;

  SaveState();
  SetFlags((wxSockFlags)(NOWAIT | SPEED));

  while (recv_size == MAX_BUFSIZE) 
  {
    recv_size = Read(my_data, MAX_BUFSIZE).LastCount();
  }

  RestoreState();
  delete [] my_data;

#undef MAX_BUFSIZE
}

// this is normally defined by configure, but if it wasn't try to do it here
#ifndef SOCKLEN_T
    #if wxHAVE_GLIBC2
        typedef socklen_t SOCKLEN_T;
    #else
        typedef int SOCKLEN_T;
    #endif
#endif // SOCKLEN_T

// --------------------------------------------------------------
// wxSocketBase socket info functions
// --------------------------------------------------------------

bool wxSocketBase::GetPeer(wxSockAddress& addr_man) const
{
  struct sockaddr my_addr;
  SOCKLEN_T len_addr = (SOCKLEN_T)sizeof(my_addr);

  if (m_fd < 0)
    return FALSE;

  if (getpeername(m_fd, (struct sockaddr *)&my_addr, &len_addr) < 0)
    return FALSE;

  addr_man.Disassemble(&my_addr, len_addr);
  return TRUE;
}

bool wxSocketBase::GetLocal(wxSockAddress& addr_man) const
{
  struct sockaddr my_addr;
  SOCKLEN_T len_addr = (SOCKLEN_T)sizeof(my_addr);

  if (m_fd < 0)
    return FALSE;

  if (getsockname(m_fd, (struct sockaddr *)&my_addr, &len_addr) < 0)
    return FALSE;

  addr_man.Disassemble(&my_addr, len_addr);
  return TRUE;
}

// --------------------------------------------------------------
// wxSocketBase wait functions
// --------------------------------------------------------------

void wxSocketBase::SaveState()
{
  wxSockState *state = new wxSockState;

  state->cbk_on = m_cbkon;
  state->cbk_set= m_neededreq;
  state->cbk    = m_cbk;
  state->cdata  = m_cdata;
  state->flags  = m_flags;
  state->notif  = m_notifyme;

  m_states.Append(state);
}

void wxSocketBase::RestoreState()
{
  wxNode *node;

  node = m_states.Last();
  if (!node)
    return;

  wxSockState *state = (wxSockState *)node->Data();

  SetFlags(state->flags);
  m_neededreq = state->cbk_set;
  m_cbk       = state->cbk;
  m_cdata     = state->cdata;
  m_notifyme  = state->notif;
  if (state->cbk_on)
    SetupCallbacks();
  else
    DestroyCallbacks();

  delete node;
  delete state;
}

// --------------------------------------------------------------
// --------- wxSocketBase wait functions ------------------------
// --------------------------------------------------------------

bool wxSocketBase::_Wait(long seconds, long microseconds, int type)
{
  if ((!m_connected && !m_connecting) || m_fd < 0)
    return FALSE;

  wxSockWakeUp wakeup(this, &m_waitflags, 0);

  SaveState();
  SetNotify((wxRequestNotify)type);
  SetupCallbacks();

  if (seconds != -1)
    wakeup.Start((int)(seconds*1000 + (microseconds / 1000)), TRUE);

  m_waitflags = 0x80 | type;
  while (m_waitflags & 0x80)
    PROCESS_EVENTS();

  RestoreState();

  if (m_waitflags & 0x40) 
  {
    m_waitflags = 0;
    return TRUE;
  }
  m_waitflags = 0;

  return FALSE;
}

bool wxSocketBase::Wait(long seconds, long microseconds)
{
  return _Wait(seconds, microseconds, REQ_ACCEPT | REQ_CONNECT |
                                      REQ_READ | REQ_WRITE | REQ_LOST);
}

bool wxSocketBase::WaitForRead(long seconds, long microseconds)
{
  return _Wait(seconds, microseconds, REQ_READ | REQ_LOST);
}

bool wxSocketBase::WaitForWrite(long seconds, long microseconds)
{
  return _Wait(seconds, microseconds, REQ_WRITE);
}

bool wxSocketBase::WaitForLost(long seconds, long microseconds)
{
  return _Wait(seconds, microseconds, REQ_LOST);
}

// --------------------------------------------------------------
// --------- wxSocketBase callback management -------------------
// --------------------------------------------------------------

#ifdef __WXGTK__
void wxPrereadSocket(wxSocketBase *sock)
{
  char tmp_buf[1024];
  int got = 0;

  do {
    got = recv(sock->m_fd, tmp_buf, 1024, 0);
    if (got > 0)
      sock->CreatePushbackAfter(tmp_buf, got);
  } while (got > 0);
}
#endif

#if defined(__WXMOTIF__) || defined(__WXXT__) || defined(__WXGTK__)
#if defined(__WXMOTIF__) || defined(__WXXT__)
static void wx_socket_read(XtPointer client, int *fid,
         XtInputId *WXUNUSED(id))
#define fd *fid
#else
static void wx_socket_read(gpointer client, gint fd,
                           GdkInputCondition WXUNUSED(cond))
#define fd fd
#endif
{
  wxSocketBase *sock = (wxSocketBase *)client;
  char c;
  int i;

  i = recv(fd, &c, 1, MSG_PEEK);

  if (i == -1 && (sock->NeededReq() & wxSocketBase::REQ_ACCEPT)) 
  {
    sock->OnRequest(wxSocketBase::EVT_ACCEPT);
    return;
  }

  if (i != 0) 
  {
    if (!(sock->NeededReq() & wxSocketBase::REQ_READ))
    {
#ifdef __WXGTK__
      // We can't exit from the GDK main loop because it doesn't accept
      // destroying input event while we are in a event dispatch.
      // So we will preread socket and we put the data in the pushback.
      wxPrereadSocket(sock);
      // Then we set the socket as BLOCKING
      int flag = 0;
      ioctl(fd, FIONBIO, &flag);
#endif
      return;
    }

    sock->OnRequest(wxSocketBase::EVT_READ);
  } 
  else 
  {
    if (!(sock->NeededReq() & wxSocketBase::REQ_LOST)) 
    {
      sock->Close();
      return;
    }

    sock->OnRequest(wxSocketBase::EVT_LOST);
  }
}
#undef fd

#if defined(__WXMOTIF__) || defined(__WXXT__)
static void wx_socket_write(XtPointer client, int *WXUNUSED(fid),
          XtInputId *WXUNUSED(id))
#else
static void wx_socket_write(gpointer client, gint WXUNUSED(fd),
          GdkInputCondition WXUNUSED(cond))
#endif
{
  wxSocketBase *sock = (wxSocketBase *)client;

  if (!sock->IsConnected())
    sock->OnRequest(wxSocketBase::EVT_CONNECT);
  else
    sock->OnRequest(wxSocketBase::EVT_WRITE);
}
#endif

#ifdef wx_xview
Notify_value wx_sock_read_xview (Notify_client client, int fd)
{
  wxSocketBase *sock = (wxSocketBase *)client;
  char c;
  int i;

  i = recv(fd, &c, 1, MSG_PEEK);

  if (i == -1 && (sock->NeededReq() & wxSocketBase::REQ_ACCEPT)) 
  {
    sock->OnRequest(wxSocketBase::EVT_ACCEPT);
    return;
  }

  /* Bytes arrived */
  if (i != 0) 
  {
    if (!(sock->NeededReq() & wxSocketBase::REQ_READ))
      return (Notify_value) FALSE;

    sock->OnRequest(wxSocketBase::EVT_READ);
  } 
  else 
  {
    if (!(sock->NeededReq() & wxSocketBase::REQ_LOST))
      return;

    sock->OnRequest(wxSocketBase::EVT_LOST);
  }

  return (Notify_value) FALSE;
}

Notify_value wx_sock_write_xview (Notify_client client, int fd)
{
  wxSocketBase *sock = (wxSocketBase *)client;

  if (!sock->IsConnected())
    sock->OnRequest(wxSocketBase::EVT_CONNECT);
  else
    sock->OnRequest(wxSocketBase::EVT_WRITE);

  return (Notify_value) TRUE;
}
#endif

wxSocketBase::wxRequestNotify wxSocketBase::EventToNotify(wxRequestEvent evt)
{
  switch (evt) 
  {
  case EVT_READ:
    return REQ_READ;
  case EVT_PEEK:
    return REQ_PEEK;
  case EVT_WRITE:
    return REQ_WRITE;
  case EVT_LOST:
    return REQ_LOST;
  case EVT_ACCEPT:
    return REQ_ACCEPT;
  case EVT_CONNECT:
    return REQ_CONNECT;
  }
  return 0;
}

void wxSocketBase::SetFlags(wxSockFlags _flags)
{
  m_flags = _flags;
  if (_flags & SPEED) 
  {
    unsigned long flag = 0;
    ioctl(m_fd, FIONBIO, &flag);

    // SPEED and WAITALL are antagonists.
    m_flags = (wxSockFlags)(m_flags & ~WAITALL);

    Notify(FALSE);
  } 
  else 
  {
    unsigned long flag = 1;
    ioctl(m_fd, FIONBIO, &flag);
  }
}

void wxSocketBase::SetNotify(wxRequestNotify flags)
{
  wxRequestNotify old_needed_req = m_neededreq;
  if (flags & REQ_ACCEPT) 
  {
    /* Check if server */
    if (!(GetClassInfo()->IsKindOf(CLASSINFO(wxSocketServer))))
      flags &= ~REQ_ACCEPT;
  }
  m_neededreq = flags;

/*
  if (m_cbkon && old_needed_req != flags)    seems to be wrong, Robert Roebling
    SetupCallbacks();
*/

  if (old_needed_req != flags)
    SetupCallbacks();
}

void wxSocketBase::SetupCallbacks()
{
  if (m_fd == INVALID_SOCKET || !m_handler || (m_flags & SPEED))
    return;

#if defined(__WXMOTIF__) || defined(__WXXT__)
  if (m_neededreq & (REQ_ACCEPT | REQ_READ | REQ_LOST)) 
  {
    if (m_internal->sock_inputid <= 0)
    {
      m_internal->sock_inputid = XtAppAddInput (wxAPP_CONTEXT, m_fd,
					(XtPointer *) XtInputReadMask,
					(XtInputCallbackProc) wx_socket_read,
					(XtPointer) this);
    }
  }
  else
  {
    if (m_internal->sock_inputid > 0)
    {
      XtRemoveInput(m_internal->sock_inputid);
      m_internal->sock_inputid = 0;
    }
  }
  
  if (m_neededreq & (REQ_CONNECT | REQ_WRITE)) 
  {
    if (m_internal->sock_outputid <= 0)
    {
      m_internal->sock_outputid = XtAppAddInput (wxAPP_CONTEXT, m_fd,
					(XtPointer *) XtInputWriteMask,
					(XtInputCallbackProc) wx_socket_write,
					(XtPointer) this);
    }
  }
  else
  {
    if (m_internal->sock_outputid > 0)
    {
      XtRemoveInput(m_internal->sock_outputid);
      m_internal->sock_outputid = 0;
    }
  }
#endif


#ifdef __WXGTK__
  if (m_neededreq & (REQ_ACCEPT | REQ_READ | REQ_LOST)) 
  {
    if (m_internal->sock_inputid <= 0)
    {
       m_internal->sock_inputid = gdk_input_add(m_fd, GDK_INPUT_READ,
                                           wx_socket_read, (gpointer)this);
    }
  }
  else
  {
    if (m_internal->sock_inputid > 0)
    {
/*
      gdk_input_remove(m_internal->sock_inputid);
      m_internal->sock_inputid = 0;
*/
    }
  }
  
  if (m_neededreq & (REQ_CONNECT | REQ_WRITE)) 
  {
    if (m_internal->sock_outputid <= 0)
    {
      m_internal->sock_outputid = gdk_input_add(m_fd, GDK_INPUT_WRITE,
                                             wx_socket_write, (gpointer)this);
    }
  }
  else
  {
    if (m_internal->sock_outputid > 0)
    {
/*
      gdk_input_remove(m_internal->sock_outputid);
      m_internal->sock_outputid = 0;
*/
    }
  }
#endif


#ifdef __WXMSW__
  WORD mask = 0;

  if (m_neededreq & REQ_READ)
    mask |= FD_READ;
  if (m_neededreq & REQ_WRITE)
    mask |= FD_WRITE;
  if (m_neededreq & REQ_LOST)
    mask |= FD_CLOSE;
  if (m_neededreq & REQ_ACCEPT)
    mask |= FD_ACCEPT;
  if (m_neededreq & REQ_CONNECT)
    mask |= FD_CONNECT;

  if (!m_internal->my_msg)
    m_internal->my_msg = m_handler->NewMessage(this);
  WSAAsyncSelect(m_fd, m_handler->GetHWND(), m_internal->my_msg, mask);
#endif
#ifdef __WXMAC__
  short mask = 0;

  if (m_neededreq & REQ_READ)
    mask |= FD_READ;
  if (m_neededreq & REQ_WRITE)
    mask |= FD_WRITE;
  if (m_neededreq & REQ_LOST)
    mask |= FD_CLOSE;
  if (m_neededreq & REQ_ACCEPT)
    mask |= FD_ACCEPT;
  if (m_neededreq & REQ_CONNECT)
    mask |= FD_CONNECT;

  GUSISetReference( m_fd ,mask, this ) ;
  unsigned long flag = 1;
  ioctl(m_fd, FIONBIO, &flag);
#endif
  m_cbkon = TRUE;
  m_processing = FALSE;
}

void wxSocketBase::DestroyCallbacks()
{
  if (!m_cbkon || !m_handler)
    return;
    
  m_cbkon = FALSE;
  m_processing = FALSE;
#if defined(__WXMOTIF__) || defined(__WXXT__)
  if (m_internal->sock_inputid > 0)
    XtRemoveInput(m_internal->sock_inputid);
  m_internal->sock_inputid = 0;
  if (m_internal->sock_outputid > 0)
    XtRemoveInput(m_internal->sock_outputid);
  m_internal->sock_outputid = 0;
#endif
#ifdef __WXGTK__
  if (m_internal->sock_inputid > 0)
    gdk_input_remove(m_internal->sock_inputid);
  m_internal->sock_inputid = 0;
  if (m_internal->sock_outputid > 0)
    gdk_input_remove(m_internal->sock_outputid);
  m_internal->sock_outputid = 0;
#endif
#ifdef __WINDOWS__
  WSAAsyncSelect(m_fd, m_handler->GetHWND(), 0, 0);
#endif
#ifdef __WXMAC__
  GUSISetReference( m_fd , 0 , 0 ) ;
  int bottom = wxMacNetEventsBottom ;
  while ( wxMacNetEventsTop != bottom )
  {
    // set all events that reference this socket to nil
    if ( wxMacNetEventsReferences[bottom] == (void*) this )
      wxMacNetEventsReferences[bottom] = NULL ;
    bottom++ ;
    if ( bottom == kwxMacNetEventsMax )
      bottom = 0 ;
  }
  SetFlags( m_flags ) ;
#endif
}

void wxSocketBase::Notify(bool notify)
{
  if (m_notifyme == notify)
    return;
  if (notify)
    SetupCallbacks();
  else
    DestroyCallbacks();
  m_notifyme = notify;
}

void wxSocketBase::OnRequest(wxRequestEvent req_evt)
{
  wxRequestNotify req_notif = EventToNotify(req_evt);

  // Mask the current event
  SetNotify(m_neededreq & ~req_notif);  
 
  if (req_evt <= EVT_WRITE && DoRequests(req_evt))
    return;

  if (m_waitflags & 0xF0) 
  {
    // Wake up
    if ((m_waitflags & 0x0F) == req_evt) 
    {
      m_waitflags = 0x80;
#ifndef __WXGTK__
      DestroyCallbacks();
#endif
    }
    return;
  }

  if (req_evt == EVT_LOST) 
  {
    m_connected = FALSE;
    Close();
  }
  if (m_notifyme)
    OldOnNotify(req_evt);

  // Unmask
  SetNotify(m_neededreq | req_notif);
}

wxSocketEvent::wxSocketEvent(int id)
  : wxEvent(id)
{
  wxEventType type = (wxEventType)wxEVT_SOCKET;

  SetEventType(type);
}

void wxSocketBase::OldOnNotify(wxRequestEvent evt)
{
  wxSocketEvent event(m_id);

  event.SetEventObject(this);
  event.m_skevt = evt;
  ProcessEvent(event);

  if (m_cbk)
    m_cbk(*this, evt, m_cdata);
}

// --------------------------------------------------------------
// --------- wxSocketBase functions [Callback, CallbackData] ----
// --------------------------------------------------------------

wxSocketBase::wxSockCbk wxSocketBase::Callback(wxSocketBase::wxSockCbk _cbk)
{
  wxSockCbk old_cbk = m_cbk;

  m_cbk = _cbk;
  return old_cbk;
}

char *wxSocketBase::CallbackData(char *cdata_)
{
  char *old_cdata = m_cdata;

  m_cdata = cdata_;
  return old_cdata;
}

void wxSocketBase::SetEventHandler(wxEvtHandler& h_evt, int id)
{
  SetNextHandler(&h_evt);
  m_id = id;
}

// --------------------------------------------------------------
// --------- wxSocketBase pushback library ----------------------
// --------------------------------------------------------------

void wxSocketBase::CreatePushbackAfter(const char *buffer, size_t size)
{
  char *curr_pos;

  if (m_unread != NULL)
    m_unread = (char *) realloc(m_unread, m_unrd_size+size);
  else
    m_unread = (char *) malloc(size);
  curr_pos = m_unread + m_unrd_size;

  memcpy(curr_pos, buffer, size);
  m_unrd_size += size;
}

void wxSocketBase::CreatePushbackBefore(const char *buffer, size_t size)
{
  char *curr_pos, *new_buf;

  new_buf = (char *) malloc(m_unrd_size+size);
  curr_pos = new_buf + size;

  memcpy(new_buf, buffer, size);
  if (m_unrd_size != 0) {
    memcpy(curr_pos, m_unread, m_unrd_size);
    free(m_unread);
  }
  m_unread = new_buf;
  m_unrd_size += size;
}

size_t wxSocketBase::GetPushback(char *buffer, size_t size, bool peek)
{
  if (!m_unrd_size)
    return 0;

  if (size > m_unrd_size)
    size = m_unrd_size;
  memcpy(buffer, m_unread, size);

  if (!peek) {
    m_unrd_size -= size;
    if (m_unrd_size == 0) {
      free(m_unread);
      m_unread = NULL;
    }
  }

  return size;
}

// --------------------------------------------------------------
// --------- wxSocketBase "multi-thread" core -------------------
// --------------------------------------------------------------

bool wxSocketBase::DoRequests(wxRequestEvent req_flag)
{
  wxNode *node = req_list[req_flag].First();
  size_t len;
  int ret;

  if (!node)
    return FALSE;

  SockRequest *req = (SockRequest *)node->Data();

  delete node;

  switch (req->type) 
  {
  case EVT_READ:
  case EVT_PEEK:
    ret = recv(m_fd, req->buffer, req->size,
	       (req->type == EVT_PEEK) ? MSG_PEEK : 0);
    if (ret < 0) 
    {
      req->error = errno;
      req->done = TRUE;
      break;
    }
    len = ret;
    if ((len < req->size) && (m_flags & WAITALL)) 
    {
      req->size -= len;
      req->nbytes += len;
      req->buffer += len;
      req->auto_wakeup->Start(m_timeout*1000, TRUE);
      req_list[req_flag].Insert(req);
      break;
    }
    req->done = TRUE;
    req->nbytes += len;
#ifndef __WXGTK__
    DestroyCallbacks();
#endif
    break;
  case EVT_WRITE:
    ret = send(m_fd, req->buffer, req->size, 0);
    if (ret < 0) 
    {
      req->error = errno;
      req->done = TRUE;
      break;
    }
    len = ret;
    if ((len < req->size) && (m_flags & WAITALL)) 
    {
      req->size -= len;
      req->nbytes += len;
      req->buffer += len;
      req->auto_wakeup->Start(m_timeout*1000, TRUE);
      req_list[req_flag].Insert(req);
      break;
    }
    req->done = TRUE;
    req->nbytes += len;
#ifndef __WXGTK__
    DestroyCallbacks();
#endif
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

void wxSocketBase::WantSpeedBuffer(char *buffer, size_t nbytes,
                                   wxRequestEvent evt)
{
  int ret = 0;

  switch (evt) 
  {
  case EVT_PEEK:
  case EVT_READ:
    ret = recv(m_fd, buffer, nbytes,
               (evt == EVT_PEEK) ? MSG_PEEK : 0);
    break;
  case EVT_WRITE:
    ret = send(m_fd, buffer, nbytes, 0);
    break;
  }
  if (ret < 0) 
  {
    m_lcount = 0;
    m_error = errno;
  } 
  else 
  {
    m_lcount = ret;
    m_error = 0;
  }
}

void wxSocketBase::WantBuffer(char *buffer, size_t nbytes,
            wxRequestEvent evt)
{
  bool buf_timed_out;

  if (m_fd == INVALID_SOCKET || !m_handler || !m_connected)
    return;

  if (m_flags & SPEED) 
  {
    WantSpeedBuffer(buffer, nbytes, evt);
    return;
  }

  SockRequest *buf = new SockRequest;
  wxSockWakeUp s_wake(NULL, (int *)&buf_timed_out, (int)TRUE);

  m_wantbuf++;
  req_list[evt].Append(buf);

  SaveState();
  SetNotify(REQ_LOST | EventToNotify(evt));
  SetupCallbacks();
  buf->buffer = buffer;
  buf->size = nbytes;
  buf->done = FALSE;
  buf->type = evt;
  buf->nbytes = 0;
  buf->auto_wakeup = &s_wake;
  buf->error = 0;
  buf_timed_out = FALSE;

  s_wake.Start(m_timeout*1000, TRUE);
  if (m_flags & NOWAIT) 
  {
    DoRequests(evt);
  } 
  else 
  {
    while (!buf->done && !buf_timed_out)
      PROCESS_EVENTS();
  }
  m_wantbuf--;
  m_lcount = buf->nbytes;
  if (buf_timed_out)
    m_error = ETIMEDOUT;
  else
    m_error = buf->error;

  delete buf;
  RestoreState();
}

// --------------------------------------------------------------
// wxSocketServer
// --------------------------------------------------------------

wxSocketServer::wxSocketServer(wxSockAddress& addr_man,
             wxSockFlags flags) :
  wxSocketBase(flags, SOCK_SERVER)
{
  m_fd = socket(addr_man.GetFamily(), SOCK_STREAM, 0);

  if (m_fd == INVALID_SOCKET)
    return;

  int flag = 1;
  setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));

  struct sockaddr *myaddr;
  size_t len;

  addr_man.Build(myaddr, len);
  if (bind(m_fd, myaddr, addr_man.SockAddrLen()) < 0)
    return;

  if (listen(m_fd, 5) < 0) {
    m_fd = INVALID_SOCKET;
    return;
  }
}

// --------------------------------------------------------------
// wxSocketServer Accept
// --------------------------------------------------------------

bool wxSocketServer::AcceptWith(wxSocketBase& sock)
{
  int fd2;

  if ((fd2 = accept(m_fd, 0, 0)) < 0)
    return FALSE;

  struct linger linger;
  linger.l_onoff = 0;
  linger.l_linger = 1;

  setsockopt(fd2, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

  int flag = 0;
  setsockopt(fd2, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, sizeof(int));

  if (!(sock.m_flags & SPEED)) 
  {
    unsigned long flag2 = 1;
    ioctl(fd2, FIONBIO, &flag2);
  }

  sock.m_type = SOCK_INTERNAL;
  sock.m_fd = fd2;
  sock.m_connected = TRUE;

  return TRUE;
}

wxSocketBase *wxSocketServer::Accept()
{
  wxSocketBase* sock = new wxSocketBase();

  sock->SetFlags((wxSockFlags)m_flags);

  if (!AcceptWith(*sock))
    return NULL;

  if (m_handler)
    m_handler->Register(sock);

  return sock;
}

// --------------------------------------------------------------
// wxSocketServer callbacks
// --------------------------------------------------------------

void wxSocketServer::OnRequest(wxRequestEvent evt)
{
  if (evt == EVT_ACCEPT) 
  {
    OldOnNotify(EVT_ACCEPT);
  }
}

// --------------------------------------------------------------
// wxSocketClient
// --------------------------------------------------------------

// --------- wxSocketClient CONSTRUCTOR -------------------------
// --------------------------------------------------------------
wxSocketClient::wxSocketClient(wxSockFlags _flags) :
  wxSocketBase(_flags, SOCK_CLIENT)
{
}

// --------------------------------------------------------------
// --------- wxSocketClient DESTRUCTOR --------------------------
// --------------------------------------------------------------
wxSocketClient::~wxSocketClient()
{
}

// --------------------------------------------------------------
// --------- wxSocketClient Connect functions -------------------
// --------------------------------------------------------------
bool wxSocketClient::Connect(wxSockAddress& addr_man, bool WXUNUSED(wait) )
{
  struct linger linger;

  if (IsConnected())
    Close();

  m_fd = socket(addr_man.GetFamily(), SOCK_STREAM, 0);

  if (m_fd < 0)
    return FALSE;

  m_connected = FALSE;

  linger.l_onoff = 1;
  linger.l_linger = 5;
  setsockopt(m_fd, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

  // Stay in touch with the state of things...

  unsigned long flag = 1;
  setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, sizeof(int));

  // Disable the nagle algorithm, which delays sends till the
  // buffer is full (or a certain time period has passed?)...

#if defined(__WINDOWS__) || (defined(IPPROTO_TCP) && defined(TCP_NODELAY))
  flag = 1;
  setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
#endif

  struct sockaddr *remote;
  size_t len;

  addr_man.Build(remote, len);

  if (connect(m_fd, remote, len) != 0)
    return FALSE;

  if (!(m_flags & SPEED)) 
  {
    flag = 1;
    ioctl(m_fd, FIONBIO, &flag);
  }

  Notify(TRUE);

  m_connected = TRUE;
  return TRUE;
}

bool wxSocketClient::WaitOnConnect(long seconds, long microseconds)
{
  int ret = _Wait(seconds, microseconds, REQ_CONNECT | REQ_LOST);

  if (ret)
    m_connected = TRUE;

  return m_connected;
}

void wxSocketClient::OnRequest(wxRequestEvent evt)
{
  if (evt == EVT_CONNECT) 
  {
    if (m_connected) 
    {
#ifndef __WXGTK__
      SetNotify(m_neededreq & ~REQ_CONNECT);
#endif
      return;
    }
    m_waitflags = 0x40;
    m_connected = TRUE;
    OldOnNotify(EVT_CONNECT);
#ifndef __WXGTK__
    DestroyCallbacks();
#endif
    return;
  }
  wxSocketBase::OnRequest(evt);
}

/////////////////////////////////////////////////////////////////
// wxSocketHandler ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

wxSocketHandler *wxSocketHandler::master = NULL;
#if defined(__WINDOWS__)
static int win_initialized = 0;
#endif

// --------------------------------------------------------------
// --------- wxSocketHandler CONSTRUCTOR ------------------------
// --------------------------------------------------------------
#ifdef __WXMAC__

extern "C" int updatestatus(int s) ;

void wxMacSocketOnRequestProc( void *refcon , short event )
{
  if ( refcon )
  {
    wxSocketBase *sock = (wxSocketBase *) refcon ;  
    
    wxSocketBase::wxRequestEvent sk_req;
    
    int canRead ;
    int canWrite ;
    int exception ;
    
    switch (event) {
    case FD_READ:
      sk_req = wxSocketBase::EVT_READ;
     sock->OnRequest(sk_req);
      break;
    case FD_WRITE:
      sk_req = wxSocketBase::EVT_WRITE;
      sock->OnRequest(sk_req);
      break;
    case FD_CLOSE:
      sk_req = wxSocketBase::EVT_LOST;
     sock->OnRequest(sk_req);
      break;
    case FD_ACCEPT:
      sk_req = wxSocketBase::EVT_ACCEPT;
      sock->OnRequest(sk_req);
      break;
    case FD_CONNECT:
      sk_req = wxSocketBase::EVT_CONNECT;
      sock->OnRequest(sk_req);
      break;
    case FD_READY :
      break ;
    }
    updatestatus ( sock->m_fd ) ;
  }
}

void wxMacSocketHandlerProc( void *refcon , short event )
{
  wxMacNetEventsReferences[wxMacNetEventsTop] = refcon ;
  wxMacNetEventsEvents[wxMacNetEventsTop] = event ;

  // clumsy construct in order to never have a incorrect wxMacNetEventsTop (above limits)

  if ( wxMacNetEventsTop + 1 == kwxMacNetEventsMax )
    wxMacNetEventsTop = 0 ;
  else
    wxMacNetEventsTop++ ;
}

#endif
#ifdef __WINDOWS__

extern wxChar wxPanelClassName[];

LRESULT APIENTRY _EXPORT wxSocketHandlerWndProc(HWND hWnd, UINT message,
   WPARAM wParam, LPARAM lParam)
{
  if(message==WM_DESTROY)
  {
    ::SetWindowLong(hWnd, GWL_WNDPROC, (LONG) DefWindowProc);
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  wxSocketHandler *h_sock = (wxSocketHandler *)GetWindowLong(hWnd, GWL_USERDATA);
  wxNode *node = h_sock->smsg_list->Find(message);
  wxSocketBase *sock;
  wxSocketBase::wxRequestEvent sk_req;
  UINT event = WSAGETSELECTEVENT(lParam);

  if (!node)
    return DefWindowProc(hWnd, message, wParam, lParam);

  sock = (wxSocketBase *)node->Data();

  switch (event) {
  case FD_READ:
    sk_req = wxSocketBase::EVT_READ;
    break;
  case FD_WRITE:
    sk_req = wxSocketBase::EVT_WRITE;
    break;
  case FD_CLOSE:
    sk_req = wxSocketBase::EVT_LOST;
    break;
  case FD_ACCEPT:
    sk_req = wxSocketBase::EVT_ACCEPT;
    break;
  case FD_CONNECT:
    sk_req = wxSocketBase::EVT_CONNECT;
    break;

  default:
      wxFAIL_MSG(_T("invalid socket event"));
      return (LRESULT)0;
  }

  sock->OnRequest(sk_req);

  return (LRESULT)0;
}

FARPROC wxSocketSubClassProc = NULL;

#endif

wxSocketHandler::wxSocketHandler()
{
#if defined(__WINDOWS__)
  if (!win_initialized) 
  {
    WSADATA wsaData;

    WSAStartup((1 << 8) | 1, &wsaData);
    win_initialized = 1;
  }
  internal = new wxSockHandlerInternal;
  internal->sockWin = ::CreateWindow(wxPanelClassName, NULL, 0,
    0, 0, 0, 0, NULL, (HMENU) NULL,
    wxhInstance, 0);

  // Subclass the window
  if (!wxSocketSubClassProc)
    wxSocketSubClassProc = MakeProcInstance((FARPROC) wxSocketHandlerWndProc, wxhInstance);
  ::SetWindowLong(internal->sockWin, GWL_WNDPROC, (LONG) wxSocketSubClassProc);
  ::SetWindowLong(internal->sockWin, GWL_USERDATA, (LONG) this);

  internal->firstAvailableMsg = 5000;
  smsg_list = new wxList(wxKEY_INTEGER);
#endif

  socks = new wxList;

#ifndef __WINDOWS__
  signal(SIGPIPE, SIG_IGN);
#endif
}

// --------------------------------------------------------------
// --------- wxSocketHandler DESTRUCTOR -------------------------
// --------------------------------------------------------------
wxSocketHandler::~wxSocketHandler()
{
  wxNode *next_node, *node = socks->First();

  while (node) 
  {
    wxSocketBase* sock = (wxSocketBase*)node->Data();

    delete sock;
    next_node = node->Next();
    delete node;
    node = next_node;
  }

  delete socks;

#ifdef __WINDOWS__
  delete smsg_list;

  ::DestroyWindow(internal->sockWin);
  WSACleanup();
  win_initialized = 0;

  delete internal;
#endif
}

// --------------------------------------------------------------
// --------- wxSocketHandler registering functions --------------
// --------------------------------------------------------------

void wxSocketHandler::Register(wxSocketBase* sock)
{
  wxNode *node;

  for (node = socks->First(); node != NULL; node = node->Next()) 
  {
    wxSocketBase* s = (wxSocketBase*)node->Data();

    if (s == sock)
      return;
  }

  if (sock) 
  {
    socks->Append(sock);
    sock->SetHandler(this);
    sock->SetupCallbacks();
  }
}

void wxSocketHandler::UnRegister(wxSocketBase* sock)
{
  wxNode *node;

  for (node = socks->First(); node; node = node->Next()) 
  {
    wxSocketBase* s = (wxSocketBase*)node->Data();

    if (s == sock) 
    {
      delete node;
      sock->DestroyCallbacks();
      sock->SetHandler(NULL);
      return;
    }
  }
}

unsigned long wxSocketHandler::Count() const
{
  return socks->Number();
}

// --------------------------------------------------------------
// --------- wxSocketHandler "big" wait functions ---------------
// --------------------------------------------------------------
void handler_cbk(wxSocketBase& sock,
     wxSocketBase::wxRequestEvent WXUNUSED(flags),
     char *cdata)
{
  int *a_wait = (int *)cdata;

  (*a_wait)++;
  sock.Notify(FALSE);
}

int wxSocketHandler::Wait(long seconds, long microseconds)
{
  int i;
  int on_wait;
  wxSockWakeUp s_wake(NULL, &on_wait, -2);
  wxNode *node;

  for (node = socks->First(), i=0; node; node = node->Next(), i++) 
  {
    wxSocketBase *sock = (wxSocketBase *)node->Data();

    sock->SaveState();

    sock->SetupCallbacks();

    sock->Callback(handler_cbk);
    sock->CallbackData((char *)&on_wait);
  }
  on_wait = 0;
  if (seconds != -1)
    s_wake.Start((seconds*1000) + (microseconds/1000), TRUE);

  while (!on_wait)
    PROCESS_EVENTS();

  for (node = socks->First(), i=0; node; node = node->Next(), i++) 
  {
    wxSocketBase *sock = (wxSocketBase *)node->Data();

    sock->RestoreState();
  }

  if (on_wait == -2)
    return 0;

  return on_wait;
}

void wxSocketHandler::YieldSock()
{
  wxNode *node;

  for (node = socks->First(); node; node = node->Next() ) 
  {
    wxSocketBase *sock = (wxSocketBase *)node->Data();

    sock->SaveState();

    sock->SetFlags(wxSocketBase::SPEED);
    if (sock->IsData())
      sock->DoRequests(wxSocketBase::EVT_READ);
    sock->DoRequests(wxSocketBase::EVT_WRITE);

    sock->RestoreState();
  }
}

// --------------------------------------------------------------
// --------- wxSocketHandler: create and register the socket ----
// --------------------------------------------------------------
wxSocketServer *wxSocketHandler::CreateServer(wxSockAddress& addr,
                wxSocketBase::wxSockFlags flags)
{
  wxSocketServer *serv = new wxSocketServer(addr, flags);

  Register(serv);
  return serv;
}

wxSocketClient *wxSocketHandler::CreateClient(wxSocketBase::wxSockFlags flags)
{
  wxSocketClient *client = new wxSocketClient(flags);

  Register(client);
  return client;
}

#ifdef __WINDOWS__
// --------------------------------------------------------------
// --------- wxSocketHandler: Windows specific methods ----------
// --------------------------------------------------------------

UINT wxSocketHandler::NewMessage(wxSocketBase *sock)
{
  internal->firstAvailableMsg++;
  smsg_list->Append(internal->firstAvailableMsg, sock);
  return internal->firstAvailableMsg;
}

void wxSocketHandler::DestroyMessage(UINT msg)
{
  wxNode *node = smsg_list->Find(msg);
  delete node;
}

HWND wxSocketHandler::GetHWND() const
{
  return internal->sockWin;
}

#endif

bool wxSocketModule::OnInit() 
{
  wxSocketHandler::master = new wxSocketHandler();
  return TRUE;
}

void wxSocketModule::OnExit() 
{
  delete wxSocketHandler::master;
  wxSocketHandler::master = NULL;
}

#ifdef __WXMAC__
void wxMacProcessSocketEvents() ;
void wxMacProcessEvents()
{
  wxMacProcessSocketEvents() ;
   (*GUSISpin)(SP_MISC, 0) ;
}

void wxMacProcessSocketEvents()
{
  while ( wxMacNetEventsTop != wxMacNetEventsBottom )
  {
    // consume event at wxMacNetEventsBottom
    wxMacSocketOnRequestProc(wxMacNetEventsReferences[wxMacNetEventsBottom] , wxMacNetEventsEvents[wxMacNetEventsBottom]  ) ;
    wxMacNetEventsBottom++ ;
    if ( wxMacNetEventsBottom == kwxMacNetEventsMax )
      wxMacNetEventsBottom = 0 ;
  }
}
#endif

#endif
  // __WXSTUBS__

#endif
  // wxUSE_SOCKETS
