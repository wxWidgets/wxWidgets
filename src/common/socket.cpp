////////////////////////////////////////////////////////////////////////////////
// Name:       socket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux (completely rewritten from a basic API of Andrew
//             Davidson(1995) in wxWeb)
// Created:    April 1997
// Updated:    April 1999
// Copyright:  (C) 1999 1998, 1997, Guilhem Lavaux
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

#endif

#if defined(__WINDOWS__)
#include <winsock.h>
#endif // __WINDOWS__

#if defined(__UNIX__)

#ifdef VMS
#include <socket.h>
#else
#include <sys/socket.h>
#endif
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

/////////////////////////////////////////////////////////////////////////////
// wxSocket headers
/////////////////////////////////////////////////////////////////////////////
#include <wx/module.h>
#define WXSOCK_INTERNAL
#include <wx/sckaddr.h>
#include <wx/socket.h>
#include <wx/sckint.h>

// ----------------------
// Some patch ----- BEGIN
// ----------------------
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

// -------------------
// Some patch ---- END
// -------------------

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

// --------------------------------------------------------------
// --------- wxSocketBase CONSTRUCTOR ---------------------------
// --------------------------------------------------------------
wxSocketBase::wxSocketBase(wxSocketBase::wxSockFlags _flags,
         wxSocketBase::wxSockType _type) :
  wxEvtHandler(),
  m_flags(_flags), m_type(_type), m_connected(FALSE), m_connecting(FALSE),
  m_fd(INVALID_SOCKET), m_id(-1),
  m_handler(0),
  m_neededreq((wxRequestNotify)(REQ_READ | REQ_LOST)),
  m_timeout(3600),
  m_unread(NULL), m_unrd_size(0),
  m_cbk(NULL), m_cdata(NULL),
  m_notify_state(FALSE)
{
  m_internal = new wxSocketInternal(this);
}

wxSocketBase::wxSocketBase() :
  wxEvtHandler(),
  m_flags(WAITALL), m_type(SOCK_UNINIT), m_connected(FALSE),
  m_connecting(FALSE), m_fd(INVALID_SOCKET),
  m_id(-1), m_handler(0),
  m_neededreq((wxRequestNotify)(REQ_READ | REQ_LOST)),
  m_timeout(3600),
  m_unread(NULL), m_unrd_size(0),
  m_cbk(NULL), m_cdata(NULL),
  m_notify_state(FALSE)
{
  m_internal = new wxSocketInternal(this);
}

// --------------------------------------------------------------
// wxSocketBase
// --------------------------------------------------------------

wxSocketBase::~wxSocketBase()
{
  // First, close the file descriptor.
  Close();

  if (m_unread)
    free(m_unread);
  // Unregister from the handler database.
  if (m_handler) 
    m_handler->UnRegister(this);

  // Destroy all saved states.
  m_states.DeleteContents(TRUE);

  // Destroy the socket manager.
  delete m_internal;
}

bool wxSocketBase::Close()
{
  if (m_fd != INVALID_SOCKET) 
  {
    if (m_notify_state == TRUE)
      Notify(FALSE);

    // Shutdown the connection.
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
  m_lcount = GetPushback(buffer, nbytes, FALSE);
  nbytes -= m_lcount;
  buffer += m_lcount;

  // If we have got the whole needed buffer or if we don't want to
  // wait then it returns immediately.
  if (!nbytes || (m_lcount && !(m_flags & WAITALL)) ) {
    return *this;
  }

  WantBuffer(buffer, nbytes, EVT_READ);

  return *this;
}

wxSocketBase& wxSocketBase::ReadMsg(char* buffer, size_t nbytes)
{
  unsigned long len, len2, sig;
  struct {
    char sig[4];
    char len[4];
  } msg;

  // sig should be an explicit 32-bit unsigned integer; I've seen
  // compilers in which size_t was actually a 16-bit unsigned integer

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

  // len2 is incorrectly computed in the original; this sequence is
  // the fix
  if (len > nbytes) {
    len2 = len - nbytes;
    len = nbytes;
  }
  else
    len2 = 0;

  // the "len &&" in the following statement is necessary so that
  // we don't attempt to read (and possibly hang the system)
  // if the message was zero bytes long
  if (len && Read(buffer, len).LastCount() != len)
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
// we return *this either way, so a smart optimizer will
// optimize the following sequence out; I'm leaving it in anyway
  if (sig != 0xdeadfeed)
    return *this;

  return *this;
}

wxSocketBase& wxSocketBase::Peek(char* buffer, size_t nbytes)
{
  m_lcount = GetPushback(buffer, nbytes, TRUE);
  if (nbytes-m_lcount == 0) 
  {
    return *this;
  }
  buffer += m_lcount;
  nbytes -= m_lcount;

  WantBuffer(buffer, nbytes, EVT_PEEK);

  return *this;
}

wxSocketBase& wxSocketBase::Write(const char *buffer, size_t nbytes)
{
  m_lcount = 0;
  WantBuffer((char *)buffer, nbytes, EVT_WRITE);
  return *this;
}

wxSocketBase& wxSocketBase::WriteMsg(const char *buffer, size_t nbytes)
{
  struct {
    char sig[4];
    char len[4];
  } msg;

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
  m_lcount = 0;
  if (nbytes != 0) {
    CreatePushbackAfter(buffer, nbytes);
    m_lcount = nbytes;
  }
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

  m_internal->AcquireFD();

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&sock_set);
  FD_SET(m_fd, &sock_set);
  select(m_fd+1, &sock_set, NULL, NULL, &tv);

  m_internal->ReleaseFD();

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

// If what? Who seems to need unsigned int?
// BTW uint isn't even defined on wxMSW for VC++ for some reason. Even if it
// were, getpeername/getsockname don't take unsigned int*, they take int*.
//
// Under glibc 2.0.7, socketbits.h declares socklen_t to be unsigned int
// and it uses *socklen_t as the 3rd parameter. Robert.

// JACS - How can we detect this?
// Meanwhile, if your compiler complains about socklen_t,
// switch lines below.

#if wxHAVE_GLIBC2
#   define wxSOCKET_INT socklen_t
#else
#   define wxSOCKET_INT int
#endif

// --------------------------------------------------------------
// wxSocketBase socket info functions
// --------------------------------------------------------------

bool wxSocketBase::GetPeer(wxSockAddress& addr_man) const
{
  struct sockaddr my_addr;
  wxSOCKET_INT len_addr = sizeof(my_addr);

  if (m_fd < 0)
    return FALSE;

  m_internal->AcquireFD();

  if (getpeername(m_fd, (struct sockaddr *)&my_addr, &len_addr) < 0) {
    m_internal->ReleaseFD();
    return FALSE;
  }

  m_internal->ReleaseFD();
  addr_man.Disassemble(&my_addr, len_addr);
  return TRUE;
}

bool wxSocketBase::GetLocal(wxSockAddress& addr_man) const
{
  struct sockaddr my_addr;
  wxSOCKET_INT len_addr = sizeof(my_addr);

  if (m_fd < 0)
    return FALSE;

  m_internal->AcquireFD();

  if (getsockname(m_fd, (struct sockaddr *)&my_addr, &len_addr) < 0) {
    m_internal->ReleaseFD();
    return FALSE;
  }
  m_internal->ReleaseFD();

  addr_man.Disassemble(&my_addr, len_addr);
  return TRUE;
}

// --------------------------------------------------------------
// wxSocketBase wait functions
// --------------------------------------------------------------

void wxSocketBase::SaveState()
{
  SocketState *state = new SocketState;

  state->notify_state     = m_notify_state;
  state->evt_notify_state = m_neededreq;
  state->socket_flags     = m_flags;
  state->c_callback       = m_cbk;
  state->c_callback_data  = m_cdata;

  m_states.Append((wxObject *)state);
}

void wxSocketBase::RestoreState()
{
  wxNode *node;
  SocketState *state;

  node = m_states.Last();
  if (!node)
    return;

  state = (SocketState *)node->Data();

  SetFlags(state->socket_flags);
  m_internal->AcquireData();
  m_neededreq = state->evt_notify_state;
  m_internal->ReleaseData();
  m_cbk       = state->c_callback;
  m_cdata     = state->c_callback_data;
  Notify(state->notify_state);

  delete node;
  delete state;
}

// --------------------------------------------------------------
// --------- wxSocketBase callback functions --------------------
// --------------------------------------------------------------

wxSocketBase::wxSockCbk wxSocketBase::Callback(wxSockCbk cbk_)
{
  wxSockCbk old_cbk = cbk_;

  m_cbk = cbk_;
  return old_cbk;
}

char *wxSocketBase::CallbackData(char *data)
{
  char *old_data = m_cdata;

  m_cdata = data;
  return old_data;
}

// --------------------------------------------------------------
// --------- wxSocketBase wait functions ------------------------
// --------------------------------------------------------------

bool wxSocketBase::_Wait(long seconds, long milliseconds, int type)
{
  SockRequest *req;

  if ((!m_connected && !m_connecting) || m_fd < 0)
    return FALSE;

  req = new SockRequest;

  req->type = REQ_WAIT | type;
  req->timeout = seconds * 1000 + milliseconds;
  req->done = FALSE;
  req->buffer = NULL;
  req->size = 0;
  req->error = 0;
  req->wait = TRUE;
  m_internal->QueueRequest(req, TRUE);

  return (req->io_nbytes != 0);
}

bool wxSocketBase::Wait(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, REQ_ACCEPT | REQ_CONNECT |
                                      REQ_READ | REQ_WRITE | REQ_LOST);
}

bool wxSocketBase::WaitForRead(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, REQ_READ | REQ_LOST);
}

bool wxSocketBase::WaitForWrite(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, REQ_WRITE);
}

bool wxSocketBase::WaitForLost(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, REQ_LOST);
}

// --------------------------------------------------------------
// --------- wxSocketBase callback management -------------------
// --------------------------------------------------------------

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
  if (_flags & SPEED) {
    // SPEED and WAITALL are antagonists.
    m_flags = (wxSockFlags)(m_flags & ~WAITALL);
  }
}

wxSocketBase::wxSockFlags wxSocketBase::GetFlags() const
{
  return m_flags;
}

void wxSocketBase::SetNotify(wxRequestNotify flags)
{
  /* Check if server */
  if (m_type != SOCK_SERVER)
    flags &= ~REQ_ACCEPT;

  m_internal->AcquireData();
  m_neededreq = flags;
  m_internal->ReleaseData();
  if (m_neededreq == 0)
    m_internal->StopWaiter();
  else
    Notify(m_notify_state);
}

void wxSocketBase::Notify(bool notify)
{
  m_notify_state = notify;
  if (m_fd == INVALID_SOCKET)
    return;

  if (notify)
    m_internal->ResumeWaiter();
  else
    m_internal->StopWaiter();
}

void wxSocketBase::OnRequest(wxRequestEvent req_evt)
{
  wxSocketEvent event(m_id);
  wxRequestNotify notify = EventToNotify(req_evt);

  if ((m_neededreq & notify) == notify) {
    event.m_socket = this;
    event.m_skevt = req_evt;
    ProcessEvent(event);
    // TODOTODO
    // OldOnNotify(req_evt);

    // We disable the event reporting.
    m_neededreq &= ~notify;
  }
}

wxSocketEvent::wxSocketEvent(int id)
  : wxEvent(id)
{
  wxEventType type = (wxEventType)wxEVT_SOCKET;

  SetEventType(type);
}

void wxSocketEvent::CopyObject(wxObject& obj_d) const
{
  wxSocketEvent *event = (wxSocketEvent *)&obj_d;

  wxEvent::CopyObject(obj_d);

  event->m_skevt = m_skevt;
  event->m_socket = m_socket;
}

void wxSocketBase::OldOnNotify(wxRequestEvent evt)
{
}

// --------------------------------------------------------------
// --------- wxSocketBase functions [Callback, CallbackData] ----
// --------------------------------------------------------------

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
// --------- wxSocketBase buffer core requester -----------------
// --------------------------------------------------------------

void wxSocketBase::WantBuffer(char *buffer, size_t nbytes,
            wxRequestEvent evt)
{
  bool buf_timed_out;

  if (m_fd == INVALID_SOCKET || !m_handler || !m_connected)
    return;

  SockRequest *buf = new SockRequest;

  SaveState();
  buf->buffer = buffer;
  buf->size = nbytes;
  buf->done = FALSE;
  buf->type = EventToNotify(evt);
  buf->io_nbytes = 0;
  buf->error = 0;
  buf->wait = TRUE;
  buf->timeout = 1000;
  buf_timed_out = FALSE;

  if ((m_flags & SPEED) != 0) 
    m_internal->QueueRequest(buf, FALSE);
  else
    if ((m_flags & NOWAIT) != 0) 
      m_internal->QueueRequest(buf, TRUE);
    else 
      m_internal->QueueRequest(buf, TRUE);
  m_lcount += buf->io_nbytes;
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

  m_internal->SetFD(m_fd);

  Notify(TRUE);
}

// --------------------------------------------------------------
// wxSocketServer Accept
// --------------------------------------------------------------

bool wxSocketServer::AcceptWith(wxSocketBase& sock)
{
  int fd2;

  m_internal->AcquireFD();
  if ((fd2 = accept(m_fd, 0, 0)) < 0) {
    m_internal->ReleaseFD();
    return FALSE;
  }
  m_internal->ReleaseFD();

  struct linger linger;
  linger.l_onoff = 0;
  linger.l_linger = 1;

  setsockopt(fd2, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

  int flag = 0;
  setsockopt(fd2, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, sizeof(int));

  sock.m_type = SOCK_INTERNAL;
  sock.m_fd = fd2;
  sock.m_connected = TRUE;

  sock.m_internal->SetFD(fd2);

  sock.m_internal->ResumeWaiter();

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

  // Initializes all socket stuff ...
  // --------------------------------
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

  m_internal->SetFD(m_fd);

  // Enables bg events.
  // ------------------
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
      m_neededreq &= ~REQ_CONNECT;
      return;
    }
    m_connected = TRUE;
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
wxSocketHandler::wxSocketHandler()
{
#if defined(__WINDOWS__)
  if (!win_initialized) 
  {
    WSADATA wsaData;

    WSAStartup((1 << 8) | 1, &wsaData);
    win_initialized = 1;
  }
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
  WSACleanup();
  win_initialized = 0;
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

int wxSocketHandler::Wait(long seconds, long microseconds)
{
  // TODO Needs the completely asynchronous notifier.

  /*
  int i;
  int on_wait;
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
  */
  return 0;
}

void wxSocketHandler::YieldSock()
{
  wxNode *node;

  // Nothing to do anymore here except waiting for the queue emptying.
  for (node = socks->First(); node; node=node->Next()) {
    wxSocketBase *sock = (wxSocketBase *)node->Data();

    sock->m_internal->WaitForEnd(NULL);
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

#endif
  // __WXSTUBS__

#endif
  // wxUSE_SOCKETS
