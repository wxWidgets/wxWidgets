/////////////////////////////////////////////////////////////////////////////
// Name:       socket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux, Guillermo Rodriguez Garcia
// Created:    April 1997
// Copyright:  (C) 1999-1997, Guilhem Lavaux
//             (C) 2000-1999, Guillermo Rodriguez Garcia
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

#include "wx/app.h"
#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/timer.h"
#include "wx/utils.h"
#include "wx/module.h"
#include "wx/log.h"
#include "wx/intl.h"

/////////////////////////////////////////////////////////////////////////////
// wxSocket headers
/////////////////////////////////////////////////////////////////////////////

#include "wx/sckaddr.h"
#include "wx/socket.h"

// discard buffer
#define MAX_DISCARD_SIZE (10 * 1024)

// what to do within waits
#define PROCESS_EVENTS() wxYield()

// use wxPostEvent or not
#define EXPERIMENTAL_USE_POST 1

// --------------------------------------------------------------
// ClassInfos
// --------------------------------------------------------------

IMPLEMENT_CLASS(wxSocketBase, wxObject)
IMPLEMENT_CLASS(wxSocketServer, wxSocketBase)
IMPLEMENT_CLASS(wxSocketClient, wxSocketBase)
IMPLEMENT_CLASS(wxDatagramSocket, wxSocketBase)
IMPLEMENT_DYNAMIC_CLASS(wxSocketEvent, wxEvent)

class wxSocketState : public wxObject
{
public:
  bool                     m_notify_state;
  GSocketEventFlags        m_neededreq;
  wxSockFlags              m_flags;
  wxSocketBase::wxSockCbk  m_cbk;
  char                    *m_cdata;

public:
  wxSocketState() : wxObject() {}
};

// --------------------------------------------------------------
// wxSocketBase ctor and dtor
// --------------------------------------------------------------

wxSocketBase::wxSocketBase(wxSockFlags _flags, wxSockType _type) :
  wxEvtHandler(),
  m_socket(NULL), m_evt_handler(NULL), m_id(-1),
  m_flags(_flags), m_type(_type),
  m_neededreq(0), m_notify_state(FALSE),
  m_connected(FALSE), m_establishing(FALSE),
  m_reading(FALSE), m_writing(FALSE),
  m_error(FALSE), m_lcount(0), m_timeout(600), m_states(),
  m_unread(NULL), m_unrd_size(0), m_unrd_cur(0),
  m_cbk(NULL), m_cdata(NULL)
{
}

wxSocketBase::wxSocketBase() :
  wxEvtHandler(),
  m_socket(NULL), m_evt_handler(NULL), m_id(-1),
  m_flags(NONE), m_type(SOCK_UNINIT),
  m_neededreq(0), m_notify_state(FALSE),
  m_connected(FALSE), m_establishing(FALSE),
  m_reading(FALSE), m_writing(FALSE),
  m_error(FALSE), m_lcount(0), m_timeout(600), m_states(),
  m_unread(NULL), m_unrd_size(0), m_unrd_cur(0),
  m_cbk(NULL), m_cdata(NULL)
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
  // Interrupt pending waits
  InterruptAllWaits();

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

// All IO operations {Read, Write, ReadMsg, WriteMsg, Peek,
// Unread, Discard} update m_error and m_lcount.
//
// TODO: Should Connect, Accept and AcceptWith update m_error?

class _wxSocketInternalTimer: public wxTimer
{
public:
  int *m_state;
  unsigned long m_new_val;

  void Notify()
  {
    *m_state = (int)m_new_val;  // Change the value
  }
};

wxSocketBase& wxSocketBase::Read(char* buffer, wxUint32 nbytes)
{
  // Mask read events
  m_reading = TRUE;

  m_lcount = _Read(buffer, nbytes);

  // If in wxSOCKET_WAITALL mode, all bytes should have been read.
  if (m_flags & wxSOCKET_WAITALL)
    m_error = (m_lcount != nbytes);
  else
    m_error = (m_lcount == 0);

  // Allow read events from now on
  m_reading = FALSE;

  return *this;
}

wxUint32 wxSocketBase::_Read(char* buffer, wxUint32 nbytes)
{
  int total;
  int ret = 1;

  // we try this even if the connection has already been closed.
  total = GetPushback(buffer, nbytes, FALSE);
  nbytes -= total;
  buffer += total;

  // If the socket is not connected, or we have got the whole
  // needed buffer, return immedately
  if (!m_socket || !m_connected || !nbytes)
    return total;

  // Possible combinations (they are checked in this order)
  // wxSOCKET_NOWAIT
  // wxSOCKET_WAITALL | wxSOCKET_BLOCK
  // wxSOCKET_WAITALL
  // wxSOCKET_BLOCK
  // wxSOCKET_NONE
  //
  if (m_flags & wxSOCKET_NOWAIT)
  {
    GSocket_SetNonBlocking(m_socket, TRUE);
    ret = GSocket_Read(m_socket, buffer, nbytes);
    GSocket_SetNonBlocking(m_socket, FALSE);

    if (ret > 0)
      total += ret;
  }
  else if (m_flags & wxSOCKET_WAITALL)
  {
    while (ret > 0 && nbytes > 0)
    {
      if (!(m_flags & wxSOCKET_BLOCK) && !WaitForRead())
          break;

      ret = GSocket_Read(m_socket, buffer, nbytes);

      if (ret > 0)
      {
        total  += ret;
        buffer += ret;
        nbytes -= ret;
      }
    }
  }
  else
  {
    if ((m_flags & wxSOCKET_BLOCK) || WaitForRead())
    {
      ret = GSocket_Read(m_socket, buffer, nbytes);

      if (ret > 0)
        total += ret;
    }
  }

  return total;
}

wxSocketBase& wxSocketBase::ReadMsg(char* buffer, wxUint32 nbytes)
{
  wxUint32 len, len2, sig, total;
  bool error;
  int old_flags;
  struct
  {
    unsigned char sig[4];
    unsigned char len[4];
  } msg;

  // Mask read events
  m_reading = TRUE;

  total = 0;
  error = TRUE;
  old_flags = m_flags;
  SetFlags((m_flags & wxSOCKET_BLOCK) | wxSOCKET_WAITALL);

  if (_Read((char *)&msg, sizeof(msg)) != sizeof(msg))
    goto exit;

  sig = (wxUint32)msg.sig[0];
  sig |= (wxUint32)(msg.sig[1] << 8);
  sig |= (wxUint32)(msg.sig[2] << 16);
  sig |= (wxUint32)(msg.sig[3] << 24);

  if (sig != 0xfeeddead)
  {
    wxLogWarning( _("TCP: invalid signature returned to ReadMsg."));
    goto exit;
  }

  len = (wxUint32)msg.len[0];
  len |= (wxUint32)(msg.len[1] << 8);
  len |= (wxUint32)(msg.len[2] << 16);
  len |= (wxUint32)(msg.len[3] << 24);

  //wxLogMessage("Readmsg: %d  %d  %d  %d -> len == %d",
  //  msg.len[0], msg.len[1], msg.len[2], msg.len[3], len);

  if (len > nbytes)
  {
    len2 = len - nbytes;
    len = nbytes;
  }
  else
    len2 = 0;

  // This check is necessary so that we don't attemp to read if
  // the msg was zero bytes long.
  if (len)
  {
    total = _Read(buffer, len);

    if (total != len)
      goto exit;
  }
  if (len2)
  {
    char *discard_buffer = new char[MAX_DISCARD_SIZE];
    long discard_len;

    // NOTE: discarded bytes don't add to m_lcount.
    do
    {
      discard_len = ((len2 > MAX_DISCARD_SIZE)? MAX_DISCARD_SIZE : len2);
      discard_len = _Read(discard_buffer, (wxUint32)discard_len);
      len2 -= (wxUint32)discard_len;
    }
    while ((discard_len > 0) && len2);

    delete [] discard_buffer;

    if (len2 != 0)
      goto exit;
  }
  if (_Read((char *)&msg, sizeof(msg)) != sizeof(msg))
    goto exit;

  sig = (wxUint32)msg.sig[0];
  sig |= (wxUint32)(msg.sig[1] << 8);
  sig |= (wxUint32)(msg.sig[2] << 16);
  sig |= (wxUint32)(msg.sig[3] << 24);

  if (sig != 0xdeadfeed)
  {
    //wxLogMessage(wxT("Warning: invalid signature returned to ReadMsg"));
    goto exit;
  }

  // everything was OK
  error = FALSE;

exit:
  m_error = error;
  m_lcount = total;
  m_reading = FALSE;
  SetFlags(old_flags);

  return *this;
}

wxSocketBase& wxSocketBase::Peek(char* buffer, wxUint32 nbytes)
{
  // Mask read events
  m_reading = TRUE;

  m_lcount = _Read(buffer, nbytes);
  Pushback(buffer, m_lcount);

  // If in wxSOCKET_WAITALL mode, all bytes should have been read.
  if (m_flags & wxSOCKET_WAITALL)
    m_error = (m_lcount != nbytes);
  else
    m_error = (m_lcount == 0);

  // Allow read events again
  m_reading = FALSE;

  return *this;
}

wxSocketBase& wxSocketBase::Write(const char *buffer, wxUint32 nbytes)
{
  // Mask write events
  m_writing = TRUE;

  m_lcount = _Write(buffer, nbytes);

  // If in wxSOCKET_WAITALL mode, all bytes should have been written.
  if (m_flags & wxSOCKET_WAITALL)
    m_error = (m_lcount != nbytes);
  else
    m_error = (m_lcount == 0);

  // Allow write events again
  m_writing = FALSE;

  return *this;
}

wxUint32 wxSocketBase::_Write(const char *buffer, wxUint32 nbytes)
{
  wxUint32 total = 0;
  int ret = 1;

  if (!m_connected || !m_socket)
    return 0;

  // Possible combinations (they are checked in this order)
  // wxSOCKET_NOWAIT
  // wxSOCKET_WAITALL | wxSOCKET_BLOCK
  // wxSOCKET_WAITALL
  // wxSOCKET_BLOCK
  // wxSOCKET_NONE
  //
  if (m_flags & wxSOCKET_NOWAIT)
  {
    GSocket_SetNonBlocking(m_socket, TRUE);
    ret = GSocket_Write(m_socket, buffer, nbytes);
    GSocket_SetNonBlocking(m_socket, FALSE);

    if (ret > 0)
      total = ret;
  }
  else if (m_flags & wxSOCKET_WAITALL)
  {
    while (ret > 0 && nbytes > 0)
    {
      if (!(m_flags & wxSOCKET_BLOCK) && !WaitForWrite())
          break;

      ret = GSocket_Write(m_socket, buffer, nbytes);

      if (ret > 0)
      {
        total  += ret;
        buffer += ret;
        nbytes -= ret;
      }
    }
  }
  else
  {
    if ((m_flags & wxSOCKET_BLOCK) || WaitForWrite())
    {
      ret = GSocket_Write(m_socket, buffer, nbytes);

      if (ret > 0)
        total = ret;
    }
  }

  return total;
}

wxSocketBase& wxSocketBase::WriteMsg(const char *buffer, wxUint32 nbytes)
{
  wxUint32 total;
  bool error;
  int old_flags;
  struct {
    unsigned char sig[4];
    unsigned char len[4];
  } msg;

  // Mask write events
  m_writing = TRUE;

  error = TRUE;
  total = 0;
  old_flags = m_flags;
  SetFlags((m_flags & wxSOCKET_BLOCK) | wxSOCKET_WAITALL);

  // warning about 'cast truncates constant value'
#ifdef __VISUALC__
#  pragma warning(disable: 4310)
#endif // __VISUALC__

  msg.sig[0] = (unsigned char) 0xad;
  msg.sig[1] = (unsigned char) 0xde;
  msg.sig[2] = (unsigned char) 0xed;
  msg.sig[3] = (unsigned char) 0xfe;

  msg.len[0] = (unsigned char) nbytes & 0xff;
  msg.len[1] = (unsigned char) (nbytes >> 8) & 0xff;
  msg.len[2] = (unsigned char) (nbytes >> 16) & 0xff;
  msg.len[3] = (unsigned char) (nbytes >> 24) & 0xff;

  //wxLogMessage("Writemsg: %d  %d  %d  %d -> %d",
  //  nbytes & 0xff,
  //  (nbytes >> 8) & 0xff,
  //  (nbytes >> 16) & 0xff,
  //  (nbytes >> 24) & 0xff,
  //  nbytes
  //  );

  if (_Write((char *)&msg, sizeof(msg)) < sizeof(msg))
    goto exit;

  total = _Write(buffer, nbytes);

  if (total < nbytes)
    goto exit;

  msg.sig[0] = (unsigned char) 0xed;
  msg.sig[1] = (unsigned char) 0xfe;
  msg.sig[2] = (unsigned char) 0xad;
  msg.sig[3] = (unsigned char) 0xde;
  msg.len[0] = msg.len[1] = msg.len[2] = msg.len[3] = (char) 0;

  if ((_Write((char *)&msg, sizeof(msg))) < sizeof(msg))
    goto exit;

  // everything was OK
  error = FALSE;

exit:
  m_error = error;
  m_lcount = total;
  m_writing = FALSE;

  return *this;

#ifdef __VISUALC__
#  pragma warning(default: 4310)
#endif // __VISUALC__
}

wxSocketBase& wxSocketBase::Unread(const char *buffer, wxUint32 nbytes)
{
  if (nbytes != 0)
    Pushback(buffer, nbytes);

  m_error = FALSE;
  m_lcount = nbytes;

  return *this;
}

wxSocketBase& wxSocketBase::Discard()
{
  int old_flags;
  char *my_data = new char[MAX_DISCARD_SIZE];
  wxUint32 recv_size = MAX_DISCARD_SIZE;
  wxUint32 total = 0;

  // Mask read events
  m_reading = TRUE;

  old_flags = m_flags;
  SetFlags(wxSOCKET_NOWAIT);

  while (recv_size == MAX_DISCARD_SIZE)
  {
    recv_size = _Read(my_data, MAX_DISCARD_SIZE);
    total += recv_size;
  }

  delete [] my_data;
  m_lcount = total;
  m_error = FALSE;

  // Allow read events again
  m_reading = FALSE;

  return *this;
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

  state->m_notify_state = m_notify_state;
  state->m_neededreq    = m_neededreq;
  state->m_flags        = m_flags;
  state->m_cbk          = m_cbk;
  state->m_cdata        = m_cdata;

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

  SetFlags(state->m_flags);
  m_cbk       = state->m_cbk;
  m_cdata     = state->m_cdata;
  m_neededreq = state->m_neededreq;
  Notify(state->m_notify_state);

  delete node;
  delete state;
}


// --------------------------------------------------------------
// wxSocketBase Wait functions
// --------------------------------------------------------------

// These WaitXXX unctions do not depend on the event system any
// longer; instead, they poll the socket, using GSocket_Select()
// to check for the specified combination of event flags, until
// an event occurs or until the timeout ellapses. The polling
// loop calls PROCESS_EVENTS(), so this won't block the GUI.
//
// XXX: Should it honour the wxSOCKET_BLOCK flag ?
//
bool wxSocketBase::_Wait(long seconds, long milliseconds,
                         wxSocketEventFlags flags)
{
  GSocketEventFlags result;
  _wxSocketInternalTimer timer;
  long timeout;
  int state = -1;

  // Set this to TRUE to interrupt ongoing waits
  m_interrupt = FALSE;

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
  if (timeout)
  {
    timer.m_state = &state;
    timer.m_new_val = 0;
    timer.Start((int)timeout, TRUE);
  }

  // Active polling (without using events)
  //
  // NOTE: this duplicates some of the code in OnRequest (lost
  // connection and connection establishment handling) but this
  // doesn't hurt. It has to be here because the event might
  // be a bit delayed, and it has to be in OnRequest as well
  // because maybe the WaitXXX functions are not being used.
  //
  // Do this at least once (important if timeout == 0, when
  // we are just polling). Also, if just polling, do not yield.
  //
  while (state == -1)
  {
    result = GSocket_Select(m_socket, flags | GSOCK_LOST_FLAG);

    // Connection lost
    if (result & GSOCK_LOST_FLAG)
    {
      timer.Stop();
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

    if ((result & GSOCK_INPUT_FLAG) || (result & GSOCK_OUTPUT_FLAG))
    {
      timer.Stop();
      return TRUE;
    }

    // Wait more?
    if ((timeout == 0) || (m_interrupt))
      break;
    else
      PROCESS_EVENTS();
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
  // Check pushback buffer
  if (m_unread)
    return TRUE;

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
    GSocket_SetTimeout(m_socket, m_timeout * 1000);
}

// --------------------------------------------------------------
// wxSocketBase flags
// --------------------------------------------------------------

void wxSocketBase::SetFlags(wxSockFlags _flags)
{
  m_flags = _flags;
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

// All events (INPUT, OUTPUT, CONNECTION, LOST) are now always
// internally watched; but users will only be notified of those
// events they are interested in.

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

  // dbg("Entering OnRequest (evt %d)\n", req_evt);

  switch (req_evt)
  {
    // This duplicates some code in _Wait(), but this doesn't
    // hurt. It has to be here because we don't know whether
    // WaitXXX will be used, and it has to be in _Wait as well
    // because the event might be a bit delayed.
    //
    case wxSOCKET_CONNECTION :
      m_establishing = FALSE;
      m_connected = TRUE;
      break;
    case wxSOCKET_LOST:
      Close();
      break;

    // If we are in the middle of a R/W operation, do not
    // propagate events to users. Also, filter 'late' events
    // which are no longer valid.
    //
    case wxSOCKET_INPUT:
      if (m_reading || !GSocket_Select(m_socket, GSOCK_INPUT_FLAG))
        return;
      else
        break;

    case wxSOCKET_OUTPUT:
      if (m_writing || !GSocket_Select(m_socket, GSOCK_OUTPUT_FLAG))
        return;
      else
        break;
  }

  if (((m_neededreq & flag) == flag) && m_notify_state)
  {
    // dbg("Evt %d delivered\n", req_evt);
    event.m_socket = this;
    event.m_skevt  = req_evt;

    if (m_evt_handler)
#if EXPERIMENTAL_USE_POST
      wxPostEvent(m_evt_handler, event);
#else
      ProcessEvent(event);
#endif

    OldOnNotify(req_evt);
    if (m_cbk)
      m_cbk(*this, req_evt, m_cdata);

  }

  // dbg("Exiting OnRequest (evt %d)\n", req_evt);
}

void wxSocketBase::OldOnNotify(wxSocketNotify WXUNUSED(evt))
{
}

// --------------------------------------------------------------
// wxSocketBase set event handler
// --------------------------------------------------------------

void wxSocketBase::SetEventHandler(wxEvtHandler& h_evt, int id)
{
  m_evt_handler = &h_evt;
  m_id = id;

  SetNextHandler(&h_evt);
}

// --------------------------------------------------------------
// wxSocketBase pushback
// --------------------------------------------------------------

void wxSocketBase::Pushback(const char *buffer, wxUint32 size)
{
  if (!size) return;

  if (m_unread == NULL)
    m_unread = (char *)malloc(size);
  else
  {
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

  if (!peek)
  {
    m_unrd_cur += size;
    if (m_unrd_size == m_unrd_cur)
    {
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

  GSocket_SetTimeout(m_socket, m_timeout * 1000);
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

  GSocket_SetTimeout(sock.m_socket, sock.m_timeout * 1000);
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

  GSocket_SetTimeout(m_socket, m_timeout * 1000);
  GSocket_SetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                wx_socket_callback, (char *)this);

  // GRG: If wait == FALSE, then the call should be nonblocking.
  // When we are finished, we put the socket to blocking mode
  // again.

  if (!wait)
    GSocket_SetNonBlocking(m_socket, TRUE);

  GSocket_SetPeer(m_socket, addr_man.GetAddress());
  err = GSocket_Connect(m_socket, GSOCK_STREAMED);

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
// wxDatagramSocket
// --------------------------------------------------------------

/* NOTE: experimental stuff - might change */

wxDatagramSocket::wxDatagramSocket( wxSockAddress& addr, wxSockFlags flags )
                : wxSocketBase( flags, SOCK_DATAGRAM )
{
  // Create the socket
  m_socket = GSocket_new();

  if(!m_socket)
    return;

  // Setup the socket as non connection oriented
  GSocket_SetLocal(m_socket, addr.GetAddress());
  if( GSocket_SetNonOriented(m_socket) != GSOCK_NOERROR )
  {
    GSocket_destroy(m_socket);
    m_socket = NULL;
    return;
  }

  // Initialize all stuff
  m_connected = FALSE;
  m_establishing = FALSE;
  GSocket_SetTimeout( m_socket, m_timeout );
  GSocket_SetCallback( m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                 GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                 wx_socket_callback, (char*)this );

}

wxDatagramSocket& wxDatagramSocket::RecvFrom( wxSockAddress& addr,
                                              char* buf,
                                              wxUint32 nBytes )
{
    Read(buf, nBytes);
    GetPeer(addr);
    return (*this);
}

wxDatagramSocket& wxDatagramSocket::SendTo( wxSockAddress& addr,
                                            const char* buf,
                                            wxUint32 nBytes )
{
    GSocket_SetPeer(m_socket, addr.GetAddress());
    Write(buf, nBytes);
    return (*this);
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

class WXDLLEXPORT wxSocketModule: public wxModule
{
  DECLARE_DYNAMIC_CLASS(wxSocketModule)
public:
  bool OnInit()
  {
    return GSocket_Init();
  }
  void OnExit()
  {
    GSocket_Cleanup();
  }
};

IMPLEMENT_DYNAMIC_CLASS(wxSocketModule, wxModule)

#endif
  // wxUSE_SOCKETS
