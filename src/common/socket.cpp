/////////////////////////////////////////////////////////////////////////////
// Name:       socket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux, Guillermo Rodriguez Garcia
// Created:    April 1997
// Updated:    September 1999
// Copyright:  (C) 1999, 1998, 1997, Guilhem Lavaux
//             (C) 1999, Guillermo Rodriguez Garcia
// RCS_ID:     $Id$
// License:    see wxWindows license
/////////////////////////////////////////////////////////////////////////////

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
#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/timer.h"
#include "wx/utils.h"
#include "wx/module.h"
#include "wx/log.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/////////////////////////////////////////////////////////////////////////////
// wxSocket headers
/////////////////////////////////////////////////////////////////////////////
#include "wx/sckaddr.h"
#include "wx/socket.h"

// --------------------------------------------------------------
// ClassInfos
// --------------------------------------------------------------
#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxSocketBase, wxObject)
IMPLEMENT_CLASS(wxSocketServer, wxSocketBase)
IMPLEMENT_CLASS(wxSocketClient, wxSocketBase)
IMPLEMENT_DYNAMIC_CLASS(wxSocketEvent, wxEvent)
#endif

class wxSocketState : public wxObject
{
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
// wxSocketBase ctor and dtor
// --------------------------------------------------------------

wxSocketBase::wxSocketBase(wxSocketBase::wxSockFlags _flags,
         wxSocketBase::wxSockType _type) :
  wxEvtHandler(),
  m_socket(NULL), m_flags(_flags), m_type(_type),
  m_neededreq(0),
  m_lcount(0), m_timeout(600),
  m_unread(NULL), m_unrd_size(0), m_unrd_cur(0),
  m_cbk(NULL), m_cdata(NULL),
  m_connected(FALSE), m_establishing(FALSE),
  m_notify_state(FALSE), m_id(-1),
  m_defering(NO_DEFER), m_error(FALSE),
  m_states()
{
}

wxSocketBase::wxSocketBase() :
  wxEvtHandler(),
  m_socket(NULL), m_flags(WAITALL | SPEED), m_type(SOCK_UNINIT),
  m_neededreq(0),
  m_lcount(0), m_timeout(600),
  m_unread(NULL), m_unrd_size(0), m_unrd_cur(0),
  m_cbk(NULL), m_cdata(NULL),
  m_connected(FALSE), m_establishing(FALSE),
  m_notify_state(FALSE), m_id(-1),
  m_defering(NO_DEFER), m_error(FALSE),
  m_states()
{
}

wxSocketBase::~wxSocketBase()
{
  if (m_unread)
    free(m_unread);

  // Shutdown and close the socket
  Close();

  // Destroy the GSocket object
  if (m_socket)
    GSocket_destroy(m_socket);
}

bool wxSocketBase::Close()
{
  if (m_socket)
  {
    // Disable callbacks
    GSocket_UnsetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                    GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG);

    // Shutdown the connection
    GSocket_Shutdown(m_socket);
    m_connected = FALSE;
    m_establishing = FALSE;
  }

  return TRUE;
}

// --------------------------------------------------------------
// wxSocketBase basic IO operations
// --------------------------------------------------------------

// GRG: I have made some changes to wxSocket internal event
// system; now, all events (INPUT, OUTPUT, CONNECTION, LOST)
// are always internally monitored; but users will only be
// notified of these events they are interested in. So we
// no longer have to change the event mask with SetNotify()
// in internal functions like DeferRead, DeferWrite, and
// the like. This solves a lot of problems.

// GRG: I added m_error handling to IO operations. Now,
// wxSocketBase::Error() correctly indicates if the last
// operation from {Read, Write, ReadMsg, WriteMsg, Peek,
// Unread, Discard} failed. Note that now, every function
// that updates m_lcount, also updates m_error. While I
// was at it, also fixed an UGLY bug in ReadMsg.

class _wxSocketInternalTimer: public wxTimer
{
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
  // Timer for timeout
  _wxSocketInternalTimer timer;

  wxASSERT(m_defering == NO_DEFER);

  // Set the defering mode to READ.
  m_defering = DEFER_READ;

  // Set the current buffer.
  m_defer_buffer = buffer;
  m_defer_nbytes = nbytes;
  m_defer_timer  = &timer;

  timer.m_state = (int *)&m_defer_buffer;
  timer.m_new_val = 0;

  timer.Start(m_timeout * 1000, FALSE);

  // If the socket is readable, call DoDefer for the first time
  if (GSocket_Select(m_socket, GSOCK_INPUT_FLAG))
    DoDefer();

  // Wait for buffer completion. 
  while (m_defer_buffer != NULL)
    wxYield();

  timer.Stop();

  // Disable defering mode.
  m_defering = NO_DEFER;
  m_defer_timer = NULL;

  // Return the number of bytes read from the socket.
  return nbytes-m_defer_nbytes;
}

wxSocketBase& wxSocketBase::Read(char* buffer, wxUint32 nbytes)
{
  int ret = 1;

  m_error = FALSE;

  // we try this even if the connection has already been closed.
  m_lcount = GetPushback(buffer, nbytes, FALSE);
  nbytes -= m_lcount;
  buffer += m_lcount;

  if (!m_connected || !m_socket)
  {
    // if no data retrieved AND not connected, it is an error.
    if (!m_lcount)
      m_error = TRUE;

    return *this;
  }

  // If we have got the whole needed buffer, return immediately
  if (!nbytes)
    return *this;

  // Possible combinations (they are checked in this order)
  // NOWAIT
  // SPEED | WAITALL
  // SPEED          
  // WAITALL
  // NONE
  //
  if (m_flags & NOWAIT)                 // NOWAIT
  {
    GSocket_SetNonBlocking(m_socket, TRUE);
    ret = GSocket_Read(m_socket, buffer, nbytes);
    GSocket_SetNonBlocking(m_socket, FALSE);

    if (ret > 0)
      m_lcount += ret;
  }
  else if (m_flags & SPEED & WAITALL)   // SPEED, WAITALL
  {
    while (ret > 0 && nbytes > 0)
    {
      ret = GSocket_Read(m_socket, buffer, nbytes);
      m_lcount += ret;
      buffer += ret;
      nbytes -= ret;
    }
    // In case the last call was an error ...
    if (ret < 0)
      m_lcount ++;
  }
  else if (m_flags & SPEED)             // SPEED, !WAITALL
  {
    ret = GSocket_Read(m_socket, buffer, nbytes);

    if (ret > 0)
      m_lcount += ret;
  }
  else                                  // NONE or WAITALL
  {
    ret = DeferRead(buffer, nbytes);

    if (ret > 0)
      m_lcount += ret;
  }

  // If we have read some data, then it is not an error, even
  // when in WAITALL mode, the last low-level IO call might
  // have failed.
  if (!m_lcount)
    m_error = TRUE;

  return *this;
}

wxSocketBase& wxSocketBase::ReadMsg(char* buffer, wxUint32 nbytes)
{
#define MAX_BUFSIZE (10 * 1024)

  int old_flags;
  unsigned long len, len2, sig;
  struct
  {
    char sig[4];
    char len[4];
  } msg;

  // sig should be an explicit 32-bit unsigned integer; I've seen
  // compilers in which wxUint32 was actually a 16-bit unsigned integer

  old_flags = m_flags;
  SetFlags(old_flags | WAITALL);

  Read((char *)&msg, sizeof(msg));
  if (m_lcount != sizeof(msg))
  {
    SetFlags(old_flags);
    m_error = TRUE;
    return *this;
  }

  sig = msg.sig[0] & 0xff;
  sig |= (wxUint32)(msg.sig[1] & 0xff) << 8;
  sig |= (wxUint32)(msg.sig[2] & 0xff) << 16;
  sig |= (wxUint32)(msg.sig[3] & 0xff) << 24;

  if (sig != 0xfeeddead)
  {
    wxLogDebug(_T("Warning: invalid signature returned to ReadMsg\n"));
    SetFlags(old_flags);
    m_error = TRUE;
    return *this;
  }

  len = msg.len[0] & 0xff;
  len |= (wxUint32)(msg.len[1] & 0xff) << 8;
  len |= (wxUint32)(msg.len[2] & 0xff) << 16;
  len |= (wxUint32)(msg.len[3] & 0xff) << 24;

  if (len > nbytes)
  {
    len2 = len - nbytes;
    len = nbytes;
  }
  else
    len2 = 0;

  // The "len &&" in the following statements is necessary so 
  // that we don't attempt to read (and possibly hang the system)
  // if the message was zero bytes long
  if (len && Read(buffer, len).LastCount() != len)
  {
    SetFlags(old_flags);
    m_error = TRUE;
    return *this;
  }
  if (len2)
  {
    char *discard_buffer = new char[MAX_BUFSIZE];
    long discard_len;

    do
    {
      discard_len = ((len2 > MAX_BUFSIZE)? MAX_BUFSIZE : len2);
      discard_len = Read(discard_buffer, discard_len).LastCount();
      len2 -= discard_len;
    }
    while ((discard_len > 0) && len2);

    delete [] discard_buffer;

    if (len2 != 0)
    {
      SetFlags(old_flags);
      m_error = TRUE;
      return *this;
    }
  }
  if (Read((char *)&msg, sizeof(msg)).LastCount() != sizeof(msg))
  {
    SetFlags(old_flags);
    m_error = TRUE;
    return *this;
  }

  sig = msg.sig[0] & 0xff;
  sig |= (wxUint32)(msg.sig[1] & 0xff) << 8;
  sig |= (wxUint32)(msg.sig[2] & 0xff) << 16;
  sig |= (wxUint32)(msg.sig[3] & 0xff) << 24;

  if (sig != 0xdeadfeed)
  {
    m_error = TRUE;
    wxLogDebug(_T("Warning: invalid signature returned to ReadMsg\n"));
  }

  SetFlags(old_flags);
  return *this;

#undef MAX_BUFSIZE
}

wxSocketBase& wxSocketBase::Peek(char* buffer, wxUint32 nbytes)
{
  Read(buffer, nbytes);
  CreatePushbackAfter(buffer, nbytes);

  return *this;
}

int wxSocketBase::DeferWrite(const char *buffer, wxUint32 nbytes)
{
  // Timer for timeout
  _wxSocketInternalTimer timer;

  wxASSERT(m_defering == NO_DEFER);

  m_defering = DEFER_WRITE;

  // Set the current buffer
  m_defer_buffer = (char *)buffer;
  m_defer_nbytes = nbytes;
  m_defer_timer  = &timer;

  // Start timer
  timer.m_state   = (int *)&m_defer_buffer;
  timer.m_new_val = 0;

  timer.Start(m_timeout * 1000, FALSE);

  // If the socket is writable, call DoDefer for the first time
  if (GSocket_Select(m_socket, GSOCK_OUTPUT_FLAG))
    DoDefer();

  // Wait for buffer completion. 
  while (m_defer_buffer != NULL)
    wxYield();

  // Stop timer
  m_defer_timer = NULL;
  timer.Stop();

  m_defering = NO_DEFER;

  return nbytes-m_defer_nbytes;
}

wxSocketBase& wxSocketBase::Write(const char *buffer, wxUint32 nbytes)
{
  int ret = 1;

  m_lcount = 0;
  m_error = FALSE;

  if (!m_connected || !m_socket)
  {
    m_error = TRUE;
    return *this;
  }

  // Possible combinations (they are checked in this order)
  // NOWAIT
  // SPEED | WAITALL
  // SPEED          
  // WAITALL
  // NONE
  //
  if (m_flags & NOWAIT)                 // NOWAIT
  {
    GSocket_SetNonBlocking(m_socket, TRUE);
    ret = GSocket_Write(m_socket, buffer, nbytes);
    GSocket_SetNonBlocking(m_socket, FALSE);

    if (ret > 0)
      m_lcount += ret;
  }
  else if (m_flags & SPEED & WAITALL)   // SPEED, WAITALL
  {
    while (ret > 0 && nbytes > 0)
    {
      ret = GSocket_Write(m_socket, buffer, nbytes);
      m_lcount += ret;
      buffer += ret;
      nbytes -= ret;
    }
    // In case the last call was an error ...
    if (ret < 0)
      m_lcount ++;
  }
  else if (m_flags & SPEED)             // SPEED, !WAITALL
  {
    ret = GSocket_Write(m_socket, buffer, nbytes);

    if (ret > 0)
      m_lcount += ret;
  }
  else                                  // NONE or WAITALL
  {
    ret = DeferWrite(buffer, nbytes);

    if (ret > 0)
      m_lcount += ret;
  }

  // If we have written some data, then it is not an error,
  // even when in WAITALL mode, the last low-level IO call
  // might have failed.
  if (!m_lcount)
    m_error = TRUE;

  return *this;
}

wxSocketBase& wxSocketBase::WriteMsg(const char *buffer, wxUint32 nbytes)
{
  int old_flags;
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

  old_flags = m_flags;
  SetFlags(old_flags | WAITALL);

  if (Write((char *)&msg, sizeof(msg)).LastCount() < sizeof(msg))
  {
    SetFlags(old_flags);
    m_error = TRUE;
    return *this;
  }
  if (Write(buffer, nbytes).LastCount() < nbytes)
  {
    SetFlags(old_flags);
    m_error = TRUE;
    return *this;
  }

  msg.sig[0] = (char) 0xed;
  msg.sig[1] = (char) 0xfe;
  msg.sig[2] = (char) 0xad;
  msg.sig[3] = (char) 0xde;
  msg.len[0] = msg.len[1] = msg.len[2] = msg.len[3] = (char) 0;

  if (Write((char *)&msg, sizeof(msg)).LastCount() < sizeof(msg))
    m_error = TRUE;

  SetFlags(old_flags);
  return *this;

#ifdef __VISUALC__
    #pragma warning(default: 4310)
#endif // __VISUALC__
}

wxSocketBase& wxSocketBase::Unread(const char *buffer, wxUint32 nbytes)
{
  m_error = FALSE;
  m_lcount = 0;

  if (nbytes != 0)
  {
    CreatePushbackAfter(buffer, nbytes);
    m_lcount = nbytes;
  }
  return *this;
}

bool wxSocketBase::IsData() const
{
  if (!m_socket)
    return FALSE;

  return (GSocket_Select(m_socket, GSOCK_INPUT_FLAG));
}

// GRG: DoDefer() no longer needs to know which event occured,
// because this was only used to catch LOST events and set
// m_defer_buffer = NULL; this is done in OnRequest() now.

void wxSocketBase::DoDefer()
{
  int ret;

  if (!m_defer_buffer)
    return;

  switch (m_defering)
  {
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

  if (ret >= 0)
    m_defer_nbytes -= ret;

  // If we are waiting for all bytes to be acquired, keep the defering
  // mode enabled.
  if ((m_flags & WAITALL) == 0 || m_defer_nbytes == 0 || ret < 0)
  {
    m_defer_buffer = NULL;
  }
  else
  {
    m_defer_buffer += ret;
    m_defer_timer->Start(m_timeout * 1000, FALSE);
  }
}

void wxSocketBase::Discard()
{
#define MAX_BUFSIZE (10*1024)

  char *my_data = new char[MAX_BUFSIZE];
  wxUint32 recv_size = MAX_BUFSIZE;
  wxUint32 total = 0;

  SaveState();
  SetFlags(NOWAIT);     // GRG: SPEED was not needed here!

  while (recv_size == MAX_BUFSIZE)
  {
    recv_size = Read(my_data, MAX_BUFSIZE).LastCount();
    total += recv_size;
  }

  RestoreState();
  delete [] my_data;

  m_lcount = total;
  m_error = FALSE;

#undef MAX_BUFSIZE
}

// --------------------------------------------------------------
// wxSocketBase get local or peer addresses
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
// wxSocketBase save and restore socket state
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
// wxSocketBase Wait functions
// --------------------------------------------------------------

// GRG: I have completely rewritten this family of functions
// so that they don't depend on event notifications; instead,
// they poll the socket, using GSocket_Select(), to check for
// the specified combination of event flags, until an event
// occurs or until the timeout ellapses. The polling loop
// calls wxYield(), so this won't block the GUI.

bool wxSocketBase::_Wait(long seconds, long milliseconds, wxSocketEventFlags flags)
{
  GSocketEventFlags result;
  _wxSocketInternalTimer timer;
  long timeout;
  int state = -1;

  // Check for valid socket
  if (!m_socket)
    return FALSE;

  // If it is not a server, it must be connected or establishing connection
  if ((m_type != SOCK_SERVER) && (!m_connected && !m_establishing))
    return FALSE;

  // Check for valid timeout value
  if (seconds != -1)
    timeout = seconds * 1000 + milliseconds;
  else
    timeout = m_timeout * 1000;

  // Activate timer
  timer.m_state = &state;
  timer.m_new_val = 0;
  timer.Start(timeout, TRUE);

  // Active polling (without using events)
  //
  // NOTE: this duplicates some of the code in OnRequest (lost
  // connection and connection establishment handling) but this
  // doesn't hurt. It has to be here because the event might
  // be a bit delayed, and it has to be in OnRequest as well
  // because maybe the WaitXXX functions are not being used.
  //
  while (state == -1)
  {
    result = GSocket_Select(m_socket, flags | GSOCK_LOST_FLAG);

    // Connection lost
    if (result & GSOCK_LOST_FLAG)
    {
      timer.Stop();
      m_defer_buffer = NULL;
      Close();
      return TRUE;
    }

    // Incoming connection (server) or connection established (client)
    if (result & GSOCK_CONNECTION_FLAG)
    {
      timer.Stop();
      m_connected = TRUE;
      m_establishing = FALSE;
      return TRUE;
    }

    // If we are in the middle of a deferred R/W, ignore these.
    if ((result & GSOCK_INPUT_FLAG) || (result & GSOCK_OUTPUT_FLAG))
    {
      if (m_defer_buffer == NULL)
      {
        timer.Stop();
        return TRUE;
      }
    }

    wxYield();
  }

  timer.Stop();
  return FALSE;
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
  return _Wait(seconds, milliseconds, GSOCK_INPUT_FLAG);
}

bool wxSocketBase::WaitForWrite(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_OUTPUT_FLAG);
}

bool wxSocketBase::WaitForLost(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_LOST_FLAG);
}

void wxSocketBase::SetTimeout(long seconds)
{
  m_timeout = seconds;

  if (m_socket)
    GSocket_SetTimeout(m_socket, m_timeout);
}

// --------------------------------------------------------------
// wxSocketBase flags
// --------------------------------------------------------------

void wxSocketBase::SetFlags(wxSockFlags _flags)
{
  m_flags = _flags;
}
                    
wxSocketBase::wxSockFlags wxSocketBase::GetFlags() const
{
  return m_flags;
}

// --------------------------------------------------------------
// wxSocketBase callback management
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
// wxSocketBase automatic notifier
// --------------------------------------------------------------

static void LINKAGEMODE wx_socket_callback(GSocket * WXUNUSED(socket),
                                           GSocketEvent event, char *cdata)
{
  wxSocketBase *sckobj = (wxSocketBase *)cdata;

  sckobj->OnRequest((wxSocketNotify)event);
}

wxSocketEventFlags wxSocketBase::EventToNotify(wxSocketNotify evt)
{
  switch (evt)
  {
    case GSOCK_INPUT:      return GSOCK_INPUT_FLAG;
    case GSOCK_OUTPUT:     return GSOCK_OUTPUT_FLAG;
    case GSOCK_CONNECTION: return GSOCK_CONNECTION_FLAG;
    case GSOCK_LOST:       return GSOCK_LOST_FLAG;
  }
  return 0;
}

void wxSocketBase::SetNotify(wxSocketEventFlags flags)
{
  m_neededreq = flags;
}

void wxSocketBase::Notify(bool notify)
{
  m_notify_state = notify;
}

void wxSocketBase::OnRequest(wxSocketNotify req_evt)
{
  wxSocketEvent event(m_id);
  wxSocketEventFlags flag = EventToNotify(req_evt);

  // NOTE: this duplicates some of the code in _Wait (lost
  // connection and connection establishment handling) but
  // this doesn't hurt. It has to be here because maybe the
  // WaitXXX are not being used, and it has to be in _Wait
  // as well because the event might be a bit delayed.
  //
  switch(req_evt)
  {
    case wxSOCKET_CONNECTION:
      m_establishing = FALSE;
      m_connected = TRUE;
      break;
    case wxSOCKET_LOST:
      m_defer_buffer = NULL;
      Close();
      break;
    case wxSOCKET_INPUT:
    case wxSOCKET_OUTPUT:
      if (m_defer_buffer)
      {
        // GRG: DoDefer() no longer needs to know which
        // event occured, because this was only used to
        // catch LOST events and set m_defer_buffer to
        // NULL, and this is done in OnRequest() now.
        DoDefer();
        // Do not notify to user
        return;
      }
      break;
  }

  if (((m_neededreq & flag) == flag) && m_notify_state)
  {
    event.m_socket = this;
    event.m_skevt  = req_evt;
    ProcessEvent(event);
    OldOnNotify(req_evt);

    if (m_cbk)
      m_cbk(*this, req_evt, m_cdata);
  }
}

void wxSocketBase::OldOnNotify(wxSocketNotify WXUNUSED(evt))
{
}

// --------------------------------------------------------------
// wxSocketBase set event handler
// --------------------------------------------------------------

void wxSocketBase::SetEventHandler(wxEvtHandler& h_evt, int id)
{
  SetNextHandler(&h_evt);
  m_id = id;
}

// --------------------------------------------------------------
// wxSocketBase pushback library
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

// --------------------------------------------------------------
// wxSocketServer ctor and dtor
// --------------------------------------------------------------

wxSocketServer::wxSocketServer(wxSockAddress& addr_man,
             wxSockFlags flags) :
  wxSocketBase(flags, SOCK_SERVER)
{
  // Create the socket
  m_socket = GSocket_new();

  if (!m_socket)
    return;

  // Setup the socket as server
  GSocket_SetLocal(m_socket, addr_man.GetAddress());
  if (GSocket_SetServer(m_socket) != GSOCK_NOERROR)
  {
    GSocket_destroy(m_socket);
    m_socket = NULL;
    return;
  }

  GSocket_SetTimeout(m_socket, m_timeout);
  GSocket_SetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                wx_socket_callback, (char *)this);

}

// --------------------------------------------------------------
// wxSocketServer Accept
// --------------------------------------------------------------

bool wxSocketServer::AcceptWith(wxSocketBase& sock, bool wait)
{
  GSocket *child_socket;

  if (!m_socket)
    return FALSE;

  // GRG: If wait == FALSE, then the call should be nonblocking.
  // When we are finished, we put the socket to blocking mode
  // again.

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

  GSocket_SetTimeout(sock.m_socket, sock.m_timeout);
  GSocket_SetCallback(sock.m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                     GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                     wx_socket_callback, (char *)&sock);

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

bool wxSocketServer::WaitForAccept(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_CONNECTION_FLAG);
}

// --------------------------------------------------------------
// wxSocketClient
// --------------------------------------------------------------

// --------------------------------------------------------------
// wxSocketClient ctor and dtor
// --------------------------------------------------------------

wxSocketClient::wxSocketClient(wxSockFlags _flags) :
  wxSocketBase(_flags, SOCK_CLIENT)
{
}

wxSocketClient::~wxSocketClient()
{
}

// --------------------------------------------------------------
// wxSocketClient Connect functions
// --------------------------------------------------------------
bool wxSocketClient::Connect(wxSockAddress& addr_man, bool wait)
{
  GSocketError err;

  if (IsConnected())
    Close();

  // This should never happen.
  if (m_socket)
    GSocket_destroy(m_socket);

  // Initialize all socket stuff ...
  m_socket = GSocket_new();
  m_connected = FALSE;
  m_establishing = FALSE;

  if (!m_socket)
    return FALSE;

  GSocket_SetTimeout(m_socket, m_timeout);

  // GRG: If wait == FALSE, then the call should be nonblocking.
  // When we are finished, we put the socket to blocking mode
  // again.

  if (!wait)
    GSocket_SetNonBlocking(m_socket, TRUE);

  GSocket_SetPeer(m_socket, addr_man.GetAddress());
  err = GSocket_Connect(m_socket, GSOCK_STREAMED);
  GSocket_SetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                wx_socket_callback, (char *)this);

  if (!wait)
    GSocket_SetNonBlocking(m_socket, FALSE);

  if (err != GSOCK_NOERROR)
  {
    if (err == GSOCK_WOULDBLOCK)
      m_establishing = TRUE;

    return FALSE;
  }

  m_connected = TRUE;
  return TRUE;
}

bool wxSocketClient::WaitOnConnect(long seconds, long milliseconds)
{
  if (m_connected)                      // Already connected
    return TRUE;

  if (!m_establishing || !m_socket)     // No connection in progress
    return FALSE;

  return _Wait(seconds, milliseconds, GSOCK_CONNECTION_FLAG);
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
