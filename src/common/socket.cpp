////////////////////////////////////////////////////////////////////////////////
// Name:       socket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux
// Created:    April 1997
// Updated:    July 1999
// Copyright:  (C) 1999, 1998, 1997, Guilhem Lavaux
// RCS_ID:     $Id$
// License:    see wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "socket.h"
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
#include <wx/module.h>
#include <wx/log.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/////////////////////////////////////////////////////////////////////////////
// wxSocket headers
/////////////////////////////////////////////////////////////////////////////
#include <wx/sckaddr.h>
#include <wx/socket.h>

// --------------------------------------------------------------
// ClassInfos
// --------------------------------------------------------------
#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxSocketBase, wxObject)
IMPLEMENT_CLASS(wxSocketServer, wxSocketBase)
IMPLEMENT_CLASS(wxSocketClient, wxSocketBase)
IMPLEMENT_DYNAMIC_CLASS(wxSocketEvent, wxEvent)
#endif

class wxSocketState : public wxObject {
public:
  bool notify_state;
  GSocketEventFlags evt_notify_state;
  wxSocketBase::wxSockFlags socket_flags;
  wxSocketBase::wxSockCbk c_callback;
  char *c_callback_data;

public:
  wxSocketState() : wxObject() {}
};

// --------------------------------------------------------------
// --------- wxSocketBase CONSTRUCTOR ---------------------------
// --------------------------------------------------------------
wxSocketBase::wxSocketBase(wxSocketBase::wxSockFlags _flags,
         wxSocketBase::wxSockType _type) :
  wxEvtHandler(),
  m_socket(NULL), m_flags(_flags), m_type(_type),
  m_neededreq(GSOCK_INPUT_FLAG | GSOCK_LOST_FLAG),
  m_lcount(0), m_timeout(600),
  m_unread(NULL), m_unrd_size(0), m_unrd_cur(0),
  m_cbk(NULL), m_cdata(NULL),
  m_connected(FALSE), m_establishing(FALSE),
  m_notify_state(FALSE), m_id(-1),
  m_defering(NO_DEFER),
  m_states()
{
}

wxSocketBase::wxSocketBase() :
  wxEvtHandler(),
  m_socket(NULL), m_flags(SPEED | WAITALL), m_type(SOCK_UNINIT),
  m_neededreq(GSOCK_INPUT_FLAG | GSOCK_LOST_FLAG),
  m_lcount(0), m_timeout(600),
  m_unread(NULL), m_unrd_size(0), m_unrd_cur(0),
  m_cbk(NULL), m_cdata(NULL),
  m_connected(FALSE), m_establishing(FALSE),
  m_notify_state(FALSE), m_id(-1),
  m_defering(NO_DEFER),
  m_states()
{
}

// --------------------------------------------------------------
// wxSocketBase destructor
// --------------------------------------------------------------

wxSocketBase::~wxSocketBase()
{
  if (m_unread)
    free(m_unread);

  // At last, close the file descriptor.
  Close();

  if (m_socket)
    GSocket_destroy(m_socket);
}

bool wxSocketBase::Close()
{
  if (m_socket)
  {
    if (m_notify_state == TRUE)
      Notify(FALSE);

    // Shutdown the connection.
    GSocket_Shutdown(m_socket);
    m_connected = FALSE;
    m_establishing = FALSE;
  }

  return TRUE;
}

// --------------------------------------------------------------
// wxSocketBase base IO function
// --------------------------------------------------------------
class _wxSocketInternalTimer: public wxTimer {
 public:
  int *m_state;
  unsigned long m_new_val;

  void Notify()
     {
        *m_state = m_new_val;  // Change the value
     }
};

int wxSocketBase::DeferRead(char *buffer, wxUint32 nbytes)
{
  wxSocketEventFlags old_event_flags;
  bool old_notify_state;
  // Timer for timeout
  _wxSocketInternalTimer timer;

  wxASSERT(m_defering == NO_DEFER);

  // Set the defering mode to READ.
  m_defering = DEFER_READ;

  // Save the old state.
  old_event_flags = NeededReq();
  old_notify_state = m_notify_state;

  // Set the new async flag.
  SetNotify(GSOCK_INPUT_FLAG | GSOCK_LOST_FLAG);
  Notify(TRUE);

  // Set the current buffer.
  m_defer_buffer = buffer;
  m_defer_nbytes = nbytes;
  m_defer_timer  = &timer;

  timer.m_state = (int *)&m_defer_buffer;
  timer.m_new_val = 0;

  timer.Start(m_timeout * 1000, FALSE);

  // Wait for buffer completion. 
  while (m_defer_buffer != NULL)
    wxYield();

  timer.Stop();

  // Restore the old state.
  Notify(old_notify_state);
  SetNotify(old_event_flags);

  // Disable defering mode.
  m_defering = NO_DEFER;
  m_defer_timer = NULL;

  // Return the number of bytes read from the socket.
  return nbytes-m_defer_nbytes;
}

wxSocketBase& wxSocketBase::Read(char* buffer, wxUint32 nbytes)
{
  int ret = 1;

  m_lcount = GetPushback(buffer, nbytes, FALSE);
  nbytes -= m_lcount;
  buffer += m_lcount;

  if (!m_connected)
    return *this;

  // If we have got the whole needed buffer or if we don't want to
  // wait then it returns immediately.
  if (!nbytes || (m_lcount && !(m_flags & WAITALL)) ) {
    return *this;
  }

  if ((m_flags & SPEED) != 0) {

    if ((m_flags & WAITALL) != 0) {
      while (ret > 0 && nbytes > 0) {
        ret = GSocket_Read(m_socket, buffer, nbytes);
        m_lcount += ret;
        buffer += ret;
        nbytes -= ret;
      }
    // In case the last call was an error ...
      if (ret < 0)
        m_lcount ++;
    } else {
      ret = GSocket_Read(m_socket, buffer, nbytes);
      if (ret > 0)
        m_lcount += ret;
    }

  } else {
    ret = DeferRead(buffer, nbytes);

    if (ret > 0)
      m_lcount += ret;
  }

  return *this;
}

wxSocketBase& wxSocketBase::ReadMsg(char* buffer, wxUint32 nbytes)
{
  unsigned long len, len2, sig;
  struct {
    char sig[4];
    char len[4];
  } msg;

  // sig should be an explicit 32-bit unsigned integer; I've seen
  // compilers in which wxUint32 was actually a 16-bit unsigned integer

  Read((char *)&msg, sizeof(msg));
  if (m_lcount != sizeof(msg))
    return *this;

  sig = msg.sig[0] & 0xff;
  sig |= (wxUint32)(msg.sig[1] & 0xff) << 8;
  sig |= (wxUint32)(msg.sig[2] & 0xff) << 16;
  sig |= (wxUint32)(msg.sig[3] & 0xff) << 24;

  if (sig != 0xfeeddead)
    return *this;
  len = msg.len[0] & 0xff;
  len |= (wxUint32)(msg.len[1] & 0xff) << 8;
  len |= (wxUint32)(msg.len[2] & 0xff) << 16;
  len |= (wxUint32)(msg.len[3] & 0xff) << 24;

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
  sig |= (wxUint32)(msg.sig[1] & 0xff) << 8;
  sig |= (wxUint32)(msg.sig[2] & 0xff) << 16;
  sig |= (wxUint32)(msg.sig[3] & 0xff) << 24;

// ERROR
  if (sig != 0xdeadfeed)
    wxLogDebug(_T("Warning: invalid signature returned to ReadMsg\n"));

  return *this;
}

wxSocketBase& wxSocketBase::Peek(char* buffer, wxUint32 nbytes)
{
  Read(buffer, nbytes);
  CreatePushbackAfter(buffer, nbytes);

  return *this;
}

int wxSocketBase::DeferWrite(const char *buffer, wxUint32 nbytes)
{
  wxSocketEventFlags old_event_flags;
  bool old_notify_state;
  // Timer for timeout
  _wxSocketInternalTimer timer;

  wxASSERT(m_defering == NO_DEFER);

  m_defering = DEFER_WRITE;

  // Save the old state
  old_event_flags = NeededReq();
  old_notify_state = m_notify_state;

  SetNotify(GSOCK_OUTPUT_FLAG | GSOCK_LOST_FLAG);
  Notify(TRUE);

  // Set the current buffer
  m_defer_buffer = (char *)buffer;
  m_defer_nbytes = nbytes;

  // Start timer
  timer.m_state   = (int *)&m_defer_buffer;
  timer.m_new_val = 0;

  m_defer_timer   = &timer;
  timer.Start(m_timeout * 1000, FALSE);

  while (m_defer_buffer != NULL)
    wxYield();

  // Stop timer
  m_defer_timer = NULL;
  timer.Stop();

  // Restore the old state
  Notify(old_notify_state);
  SetNotify(old_event_flags);

  m_defering = NO_DEFER;

  return nbytes-m_defer_nbytes;
}

wxSocketBase& wxSocketBase::Write(const char *buffer, wxUint32 nbytes)
{
  int ret;

  if ((m_flags & SPEED) != 0)
    ret = GSocket_Write(m_socket, buffer, nbytes);
  else
    ret = DeferWrite(buffer, nbytes);

  if (ret != -1)
    m_lcount += ret;

  return *this;
}

wxSocketBase& wxSocketBase::WriteMsg(const char *buffer, wxUint32 nbytes)
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

wxSocketBase& wxSocketBase::Unread(const char *buffer, wxUint32 nbytes)
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
  if (!m_socket)
    return FALSE;

  return (GSocket_DataAvailable(m_socket));
}

void wxSocketBase::DoDefer(wxSocketNotify req_evt)
{
  int ret;

  if (req_evt == wxSOCKET_LOST) {
    Close();
    m_defer_buffer = NULL;
    return;
  }
  switch (m_defering) {
  case DEFER_READ:
    ret = GSocket_Read(m_socket, m_defer_buffer, m_defer_nbytes);
    break;
  case DEFER_WRITE:
    ret = GSocket_Write(m_socket, m_defer_buffer, m_defer_nbytes);
    break;
  default:
    ret = -1;
    break;
  }

  m_defer_nbytes -= ret;

  if (ret < 0)
    m_defer_nbytes++;

  // If we are waiting for all bytes to be acquired, keep the defering modei
  // enabled.
  if ((m_flags & WAITALL) == 0 || m_defer_nbytes == 0 || ret < 0) {
    m_defer_buffer = NULL;
    Notify(FALSE);
  } else {
    m_defer_buffer += ret;
    m_defer_timer->Start(m_timeout * 1000, FALSE);
  }
}

// ---------------------------------------------------------------------
// --------- wxSocketBase Discard(): deletes all byte in the input queue
// ---------------------------------------------------------------------
void wxSocketBase::Discard()
{
#define MAX_BUFSIZE (10*1024)
  char *my_data = new char[MAX_BUFSIZE];
  wxUint32 recv_size = MAX_BUFSIZE;

  SaveState();
  SetFlags(NOWAIT | SPEED);

  while (recv_size == MAX_BUFSIZE)
  {
    recv_size = Read(my_data, MAX_BUFSIZE).LastCount();
  }

  RestoreState();
  delete [] my_data;

#undef MAX_BUFSIZE
}

// --------------------------------------------------------------
// wxSocketBase socket info functions
// --------------------------------------------------------------

bool wxSocketBase::GetPeer(wxSockAddress& addr_man) const
{
  GAddress *peer;

  if (!m_socket)
    return FALSE;

  peer = GSocket_GetPeer(m_socket);
  addr_man.SetAddress(peer);
  GAddress_destroy(peer);

  return TRUE;
}

bool wxSocketBase::GetLocal(wxSockAddress& addr_man) const
{
  GAddress *local;

  if (!m_socket)
    return FALSE;

  local = GSocket_GetLocal(m_socket);
  addr_man.SetAddress(local);
  GAddress_destroy(local);

  return TRUE;
}

// --------------------------------------------------------------
// wxSocketBase wait functions
// --------------------------------------------------------------

void wxSocketBase::SaveState()
{
  wxSocketState *state;

  state = new wxSocketState();

  state->notify_state     = m_notify_state;
  state->evt_notify_state = m_neededreq;
  state->socket_flags     = m_flags;
  state->c_callback       = m_cbk;
  state->c_callback_data  = m_cdata;

  m_states.Append(state);
}

void wxSocketBase::RestoreState()
{
  wxNode *node;
  wxSocketState *state;

  node = m_states.Last();
  if (!node)
    return;

  state = (wxSocketState *)node->Data();

  SetFlags(state->socket_flags);
  m_neededreq = state->evt_notify_state;
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

static void wx_socket_wait(GSocket *socket, GSocketEvent event, char *cdata)
{
  int *state = (int *)cdata;

  *state = event;
}

bool wxSocketBase::_Wait(long seconds, long milliseconds, int type)
{
  bool old_notify_state = m_notify_state;
  int state = -1;
  _wxSocketInternalTimer timer;

  if ((!m_connected && !m_establishing) || !m_socket)
    return FALSE;

  // Set the variable to change
  timer.m_state = &state;
  timer.m_new_val = GSOCK_MAX_EVENT;

  // Disable the previous handler
  Notify(FALSE);

  // Set the timeout
  timer.Start(seconds * 1000 + milliseconds, TRUE);
  GSocket_SetCallback(m_socket, type, wx_socket_wait, (char *)&state);

  while (state == -1)
    wxYield();

  GSocket_UnsetCallback(m_socket, type);
  timer.Stop();

  // Notify will restore automatically the old GSocket flags
  Notify(old_notify_state);

  // GRG: If a LOST event occured, we set m_establishing to
  // FALSE here (this is a quick hack to make WaitOnConnect
  // work; it will be removed when this function is modified
  // so that it tells the caller which event occured).
  //
  if (state == GSOCK_LOST)
    m_establishing = FALSE;

  return (state != GSOCK_MAX_EVENT);
}

bool wxSocketBase::Wait(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_INPUT_FLAG |
                                      GSOCK_OUTPUT_FLAG |
                                      GSOCK_CONNECTION_FLAG |
                                      GSOCK_LOST_FLAG);
}

bool wxSocketBase::WaitForRead(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_INPUT_FLAG | GSOCK_LOST_FLAG);
}

bool wxSocketBase::WaitForWrite(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_OUTPUT_FLAG | GSOCK_LOST_FLAG);
}

bool wxSocketBase::WaitForLost(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_LOST_FLAG);
}

// --------------------------------------------------------------
// --------- wxSocketBase callback management -------------------
// --------------------------------------------------------------

wxSocketEventFlags wxSocketBase::EventToNotify(wxSocketNotify evt)
{
  switch (evt)
  {
    case GSOCK_INPUT:
      return GSOCK_INPUT_FLAG;
    case GSOCK_OUTPUT:
      return GSOCK_OUTPUT_FLAG;
    case GSOCK_CONNECTION:
      return GSOCK_CONNECTION_FLAG;
    case GSOCK_LOST:
      return GSOCK_LOST_FLAG;
  }
  return 0;
}

void wxSocketBase::SetFlags(wxSockFlags _flags)
{
  m_flags = _flags;
}

wxSocketBase::wxSockFlags wxSocketBase::GetFlags() const
{
  return m_flags;
}

void wxSocketBase::SetNotify(wxSocketEventFlags flags)
{
  // GRG: A client can also have connection events!
/*
  if (m_type != SOCK_SERVER)
    flags &= ~GSOCK_CONNECTION_FLAG;
*/

  m_neededreq = flags;
  if (m_neededreq == 0)
    Notify(FALSE);
  else
    Notify(m_notify_state);
}

// --------------------------------------------------------------
// Automatic notifier
// --------------------------------------------------------------

static void wx_socket_callback(GSocket *socket, GSocketEvent event, char *cdata)
{
  wxSocketBase *sckobj = (wxSocketBase *)cdata;

  sckobj->OnRequest((wxSocketNotify)event);
}

void wxSocketBase::Notify(bool notify)
{
  m_notify_state = notify;
  if (!m_socket)
    return;

  GSocket_UnsetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                  GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG);
  if (!notify)
    return;

  GSocket_SetCallback(m_socket, m_neededreq, wx_socket_callback, (char *)this);
}

void wxSocketBase::OnRequest(wxSocketNotify req_evt)
{
  wxSocketEvent event(m_id);
  wxSocketEventFlags notify = EventToNotify(req_evt);

  switch(req_evt)
  {
    case wxSOCKET_CONNECTION:
      m_establishing = FALSE;
      m_connected = TRUE;
      break;
    case wxSOCKET_LOST:
      Close();
      break;
    case wxSOCKET_INPUT:
    case wxSOCKET_OUTPUT:
      if (m_defering != NO_DEFER)
      {
        DoDefer(req_evt);
        // Do not notify to user
        return;
      }
      break;
  }

  if ((m_neededreq & notify) == notify)
  {
    event.m_socket = this;
    event.m_skevt  = req_evt;
    ProcessEvent(event);
    OldOnNotify(req_evt);
  }
}

void wxSocketBase::OldOnNotify(wxSocketNotify evt)
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

void wxSocketBase::CreatePushbackAfter(const char *buffer, wxUint32 size)
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

void wxSocketBase::CreatePushbackBefore(const char *buffer, wxUint32 size)
{
  if (m_unread == NULL)
    m_unread = (char *)malloc(size);
  else {
    char *tmp;

    tmp = (char *)malloc(m_unrd_size + size);
    memcpy(tmp+size, m_unread, m_unrd_size);
    free(m_unread);

    m_unread = tmp;
  }

  m_unrd_size += size;

  memcpy(m_unread, buffer, size);
}

wxUint32 wxSocketBase::GetPushback(char *buffer, wxUint32 size, bool peek)
{
  if (!m_unrd_size)
    return 0;

  if (size > (m_unrd_size-m_unrd_cur))
    size = m_unrd_size-m_unrd_cur;
  memcpy(buffer, (m_unread+m_unrd_cur), size);

  if (!peek) {
    m_unrd_cur += size;
    if (m_unrd_size == m_unrd_cur) {
      free(m_unread);
      m_unread = NULL;
      m_unrd_size = 0;
      m_unrd_cur  = 0;
    }
  }

  return size;
}

// --------------------------------------------------------------
// wxSocketServer
// --------------------------------------------------------------

wxSocketServer::wxSocketServer(wxSockAddress& addr_man,
             wxSockFlags flags) :
  wxSocketBase(flags, SOCK_SERVER)
{
  m_socket = GSocket_new();

  if (!m_socket)
    return;

  GSocket_SetLocal(m_socket, addr_man.GetAddress());
  if (GSocket_SetServer(m_socket) != GSOCK_NOERROR) {
    GSocket_destroy(m_socket);
    m_socket = NULL;
    return;
  }

  Notify(TRUE);
}

// --------------------------------------------------------------
// wxSocketServer Accept
// --------------------------------------------------------------

bool wxSocketServer::AcceptWith(wxSocketBase& sock, bool wait)
{
  GSocket *child_socket;

  // GRG: If wait == FALSE, then set the socket to
  // nonblocking. I will set it back to blocking later,
  // but I am not sure if this is really a good idea.
  // IMO, all GSockets objects used in wxSocket should
  // be non-blocking.
  // --------------------------------

  if (!wait)
    GSocket_SetNonBlocking(m_socket, TRUE);

  child_socket = GSocket_WaitConnection(m_socket);

  if (!wait)
    GSocket_SetNonBlocking(m_socket, FALSE);

  // GRG: this was not being handled!
  if (child_socket == NULL)
    return FALSE;

  sock.m_type = SOCK_INTERNAL;
  sock.m_socket = child_socket;
  sock.m_connected = TRUE;

  return TRUE;
}

wxSocketBase *wxSocketServer::Accept(bool wait)
{
  wxSocketBase* sock = new wxSocketBase();

  sock->SetFlags((wxSockFlags)m_flags);

  if (!AcceptWith(*sock, wait))
    return NULL;

  return sock;
}

bool wxSocketServer::WaitOnAccept(long seconds, long milliseconds)
{
  // TODO: return immediately if there is already a pending connection.

  return _Wait(seconds, milliseconds, GSOCK_CONNECTION_FLAG | GSOCK_LOST_FLAG);
}

// --------------------------------------------------------------
// wxSocketClient
// --------------------------------------------------------------

// --------- wxSocketClient CONSTRUCTOR -------------------------
// --------------------------------------------------------------
wxSocketClient::wxSocketClient(wxSockFlags _flags) :
  wxSocketBase(_flags, SOCK_CLIENT)
{
  m_establishing = FALSE;
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
bool wxSocketClient::Connect(wxSockAddress& addr_man, bool wait)
{
  GSocketError err;

  if (IsConnected())
    Close();

  // This should never happen.
  if (m_socket)
    GSocket_destroy(m_socket);

  // Initializes all socket stuff ...
  // --------------------------------
  m_socket = GSocket_new();
  m_connected = FALSE;
  m_establishing = FALSE;

  if (!m_socket)
    return FALSE;

  // Update the flags of m_socket and enable BG events
  SetFlags(m_flags);
  Notify(TRUE);

  // GRG: If wait == FALSE, then set the socket to
  // nonblocking. I will set it back to blocking later,
  // but I am not sure if this is really a good idea.
  // IMO, all GSockets objects used in wxSocket should
  // be non-blocking.
  // --------------------------------
  if (!wait)
    GSocket_SetNonBlocking(m_socket, TRUE);

  GSocket_SetPeer(m_socket, addr_man.GetAddress());
  err = GSocket_Connect(m_socket, GSOCK_STREAMED);

  if (!wait)
    GSocket_SetNonBlocking(m_socket, FALSE);

  if (err == GSOCK_WOULDBLOCK)
    m_establishing = TRUE;

  if (err != GSOCK_NOERROR)
    return FALSE;

  m_connected = TRUE;
  return TRUE;
}

bool wxSocketClient::WaitOnConnect(long seconds, long milliseconds)
{
  bool ret;

  // Already connected
  if (m_connected)
    return TRUE;

  // There is no connection in progress
  if (!m_establishing)
    return FALSE;

  ret = _Wait(seconds, milliseconds, GSOCK_CONNECTION_FLAG | GSOCK_LOST_FLAG);

  if (ret)
  {
    if (m_establishing)     // it wasn't GSOCK_LOST
      m_connected = TRUE;

    m_establishing = FALSE;
  }

  return m_connected;
}

void wxSocketClient::OnRequest(wxSocketNotify evt)
{
  if ((GSocketEvent)evt == GSOCK_CONNECTION)
  {
    if (m_connected)
    {
      m_neededreq &= ~GSOCK_CONNECTION_FLAG;
      return;
    }
    m_connected = TRUE;
    return;
  }
  wxSocketBase::OnRequest(evt);
}

// --------------------------------------------------------------
// wxSocketEvent
// --------------------------------------------------------------

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

// --------------------------------------------------------------------------
// wxSocketModule
// --------------------------------------------------------------------------
class WXDLLEXPORT wxSocketModule: public wxModule {
  DECLARE_DYNAMIC_CLASS(wxSocketModule)
 public:
  bool OnInit() {
    return GSocket_Init();
  }
  void OnExit() {
    GSocket_Cleanup();
  }
};

IMPLEMENT_DYNAMIC_CLASS(wxSocketModule, wxModule)

#endif
  // wxUSE_SOCKETS
