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

// ==========================================================================
// Declarations
// ==========================================================================

#ifdef __GNUG__
#pragma implementation "socket.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SOCKETS

#include "wx/app.h"
#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/timer.h"
#include "wx/utils.h"
#include "wx/module.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/event.h"

#if wxUSE_GUI
  #include "wx/gdicmn.h"      // for wxPendingDelete
#endif // wxUSE_GUI

#include "wx/sckaddr.h"
#include "wx/socket.h"

// --------------------------------------------------------------------------
// macros and constants
// --------------------------------------------------------------------------

// discard buffer
#define MAX_DISCARD_SIZE (10 * 1024)

// what to do within waits
#if wxUSE_GUI
  #define PROCESS_EVENTS() wxYield()
#else
  #define PROCESS_EVENTS()
#endif

// --------------------------------------------------------------------------
// wxWin macros
// --------------------------------------------------------------------------

IMPLEMENT_CLASS(wxSocketBase, wxObject)
IMPLEMENT_CLASS(wxSocketServer, wxSocketBase)
IMPLEMENT_CLASS(wxSocketClient, wxSocketBase)
IMPLEMENT_CLASS(wxDatagramSocket, wxSocketBase)
IMPLEMENT_DYNAMIC_CLASS(wxSocketEvent, wxEvent)

// --------------------------------------------------------------------------
// private classes
// --------------------------------------------------------------------------

class wxSocketState : public wxObject
{
public:
  wxSocketFlags            m_flags;
  wxSocketEventFlags       m_eventmask;
  bool                     m_notify;
  void                    *m_clientData;
#if WXWIN_COMPATIBILITY
  wxSocketBase::wxSockCbk  m_cbk;
  char                    *m_cdata;
#endif // WXWIN_COMPATIBILITY

public:
  wxSocketState() : wxObject() {}
};

// ==========================================================================
// wxSocketBase
// ==========================================================================

// --------------------------------------------------------------------------
// Ctor and dtor
// --------------------------------------------------------------------------

void wxSocketBase::Init()
{
  m_socket       = NULL;
  m_type         = wxSOCKET_UNINIT;

  // state
  m_flags        = 0;
  m_connected    =
  m_establishing =
  m_reading      =
  m_writing      =
  m_error        = FALSE;
  m_lcount       = 0;
  m_timeout      = 600;
  m_beingDeleted = FALSE;

  // pushback buffer
  m_unread       = NULL;
  m_unrd_size    = 0;
  m_unrd_cur     = 0;

  // events
  m_id           = -1;
  m_handler      = NULL;
  m_clientData   = NULL;
  m_notify       = FALSE;
  m_eventmask    = 0;
#if WXWIN_COMPATIBILITY
  m_cbk          = NULL;
  m_cdata        = NULL;
#endif // WXWIN_COMPATIBILITY
}

wxSocketBase::wxSocketBase()
{
  Init();
}

wxSocketBase::wxSocketBase(wxSocketFlags flags, wxSocketType type)
{
  Init();

  m_flags = flags;
  m_type  = type;
}

wxSocketBase::~wxSocketBase()
{
  // Just in case the app called Destroy() *and* then deleted
  // the socket immediately: don't leave dangling pointers.
#if wxUSE_GUI
  wxPendingDelete.DeleteObject(this);
#endif

  // Shutdown and close the socket
  if (!m_beingDeleted)
    Close();

  // Destroy the GSocket object
  if (m_socket)
    GSocket_destroy(m_socket);

  // Free the pushback buffer
  if (m_unread)
    free(m_unread);
}

bool wxSocketBase::Destroy()
{
  // Delayed destruction: the socket will be deleted during the next
  // idle loop iteration. This ensures that all pending events have
  // been processed.
  m_beingDeleted = TRUE;

  // Shutdown and close the socket
  Close();

  // Supress events from now on
  Notify(FALSE);

#if wxUSE_GUI
  if ( !wxPendingDelete.Member(this) )
    wxPendingDelete.Append(this);
#else
  delete this;
#endif

  return TRUE;
}

// --------------------------------------------------------------------------
// Basic IO calls
// --------------------------------------------------------------------------

// The following IO operations update m_error and m_lcount:
// {Read, Write, ReadMsg, WriteMsg, Peek, Unread, Discard}
//
// TODO: Should Connect, Accept and AcceptWith update m_error?

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
  }

  m_connected = FALSE;
  m_establishing = FALSE;
  return TRUE;
}

wxSocketBase& wxSocketBase::Read(void* buffer, wxUint32 nbytes)
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

wxUint32 wxSocketBase::_Read(void* buffer, wxUint32 nbytes)
{
  int total;
  int ret = 1;

  // Try the pushback buffer first
  total = GetPushback(buffer, nbytes, FALSE);
  nbytes -= total;
  buffer  = (char *)buffer + total;

  // If the socket is invalid or we got all the data, return now
  if (!m_socket || !nbytes)
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
    ret = GSocket_Read(m_socket, (char *)buffer, nbytes);
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

      ret = GSocket_Read(m_socket, (char *)buffer, nbytes);

      if (ret > 0)
      {
        total  += ret;
        nbytes -= ret;
        buffer  = (char *)buffer + ret;
      }
    }
  }
  else
  {
    if ((m_flags & wxSOCKET_BLOCK) || WaitForRead())
    {
      ret = GSocket_Read(m_socket, (char *)buffer, nbytes);

      if (ret > 0)
        total += ret;
    }
  }

  return total;
}

wxSocketBase& wxSocketBase::ReadMsg(void* buffer, wxUint32 nbytes)
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

  if (_Read(&msg, sizeof(msg)) != sizeof(msg))
    goto exit;

  sig = (wxUint32)msg.sig[0];
  sig |= (wxUint32)(msg.sig[1] << 8);
  sig |= (wxUint32)(msg.sig[2] << 16);
  sig |= (wxUint32)(msg.sig[3] << 24);

  if (sig != 0xfeeddead)
  {
    wxLogWarning( _("wxSocket: invalid signature in ReadMsg."));
    goto exit;
  }

  len = (wxUint32)msg.len[0];
  len |= (wxUint32)(msg.len[1] << 8);
  len |= (wxUint32)(msg.len[2] << 16);
  len |= (wxUint32)(msg.len[3] << 24);

  if (len > nbytes)
  {
    len2 = len - nbytes;
    len = nbytes;
  }
  else
    len2 = 0;

  // Don't attemp to read if the msg was zero bytes long.
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
  if (_Read(&msg, sizeof(msg)) != sizeof(msg))
    goto exit;

  sig = (wxUint32)msg.sig[0];
  sig |= (wxUint32)(msg.sig[1] << 8);
  sig |= (wxUint32)(msg.sig[2] << 16);
  sig |= (wxUint32)(msg.sig[3] << 24);

  if (sig != 0xdeadfeed)
  {
    wxLogWarning( _("wxSocket: invalid signature in ReadMsg."));
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

wxSocketBase& wxSocketBase::Peek(void* buffer, wxUint32 nbytes)
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

wxSocketBase& wxSocketBase::Write(const void *buffer, wxUint32 nbytes)
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

wxUint32 wxSocketBase::_Write(const void *buffer, wxUint32 nbytes)
{
  wxUint32 total = 0;
  int ret = 1;

  // If the socket is invalid, return immediately
  if (!m_socket)
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
    ret = GSocket_Write(m_socket, (const char *)buffer, nbytes);
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

      ret = GSocket_Write(m_socket, (const char *)buffer, nbytes);

      if (ret > 0)
      {
        total  += ret;
        nbytes -= ret;
        buffer  = (const char *)buffer + ret;
      }
    }
  }
  else
  {
    if ((m_flags & wxSOCKET_BLOCK) || WaitForWrite())
    {
      ret = GSocket_Write(m_socket, (const char *)buffer, nbytes);

      if (ret > 0)
        total = ret;
    }
  }

  return total;
}

wxSocketBase& wxSocketBase::WriteMsg(const void *buffer, wxUint32 nbytes)
{
  wxUint32 total;
  bool error;
  int old_flags;
  struct
  {
    unsigned char sig[4];
    unsigned char len[4];
  } msg;

  // Mask write events
  m_writing = TRUE;

  error = TRUE;
  total = 0;
  old_flags = m_flags;
  SetFlags((m_flags & wxSOCKET_BLOCK) | wxSOCKET_WAITALL);

  msg.sig[0] = (unsigned char) 0xad;
  msg.sig[1] = (unsigned char) 0xde;
  msg.sig[2] = (unsigned char) 0xed;
  msg.sig[3] = (unsigned char) 0xfe;

  msg.len[0] = (unsigned char) (nbytes & 0xff);
  msg.len[1] = (unsigned char) ((nbytes >> 8) & 0xff);
  msg.len[2] = (unsigned char) ((nbytes >> 16) & 0xff);
  msg.len[3] = (unsigned char) ((nbytes >> 24) & 0xff);

  if (_Write(&msg, sizeof(msg)) < sizeof(msg))
    goto exit;

  total = _Write(buffer, nbytes);

  if (total < nbytes)
    goto exit;

  msg.sig[0] = (unsigned char) 0xed;
  msg.sig[1] = (unsigned char) 0xfe;
  msg.sig[2] = (unsigned char) 0xad;
  msg.sig[3] = (unsigned char) 0xde;
  msg.len[0] = msg.len[1] = msg.len[2] = msg.len[3] = (char) 0;

  if ((_Write(&msg, sizeof(msg))) < sizeof(msg))
    goto exit;

  // everything was OK
  error = FALSE;

exit:
  m_error = error;
  m_lcount = total;
  m_writing = FALSE;

  return *this;
}

wxSocketBase& wxSocketBase::Unread(const void *buffer, wxUint32 nbytes)
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
  char *buffer = new char[MAX_DISCARD_SIZE];
  wxUint32 ret;
  wxUint32 total = 0;

  // Mask read events
  m_reading = TRUE;

  old_flags = m_flags;
  SetFlags(wxSOCKET_NOWAIT);

  do
  {
    ret = _Read(buffer, MAX_DISCARD_SIZE);
    total += ret;
  }
  while (ret == MAX_DISCARD_SIZE);

  delete[] buffer;
  m_lcount = total;
  m_error  = FALSE;

  // Allow read events again
  m_reading = FALSE;

  return *this;
}

// --------------------------------------------------------------------------
// Wait functions
// --------------------------------------------------------------------------

// All Wait functions poll the socket using GSocket_Select() to
// check for the specified combination of conditions, until one
// of these conditions become true, an error occurs, or the
// timeout elapses. The polling loop calls PROCESS_EVENTS(), so
// this won't block the GUI.

bool wxSocketBase::_Wait(long seconds,
                         long milliseconds,
                         wxSocketEventFlags flags)
{
  GSocketEventFlags result;
  long timeout;

  // Set this to TRUE to interrupt ongoing waits
  m_interrupt = FALSE;

  // Check for valid socket
  if (!m_socket)
    return FALSE;

  // Check for valid timeout value.
  if (seconds != -1)
    timeout = seconds * 1000 + milliseconds;
  else
    timeout = m_timeout * 1000;

  // Wait in an active polling loop.
  //
  // NOTE: We duplicate some of the code in OnRequest, but this doesn't
  //   hurt. It has to be here because the (GSocket) event might arrive
  //   a bit delayed, and it has to be in OnRequest as well because we
  //   don't know whether the Wait functions are being used.
  //
  // Do this at least once (important if timeout == 0, when
  // we are just polling). Also, if just polling, do not yield.

  wxStopWatch chrono;
  bool done = FALSE;

  while (!done)
  {
    result = GSocket_Select(m_socket, flags | GSOCK_LOST_FLAG);

    // Incoming connection (server) or connection established (client)
    if (result & GSOCK_CONNECTION_FLAG)
    {
      m_connected = TRUE;
      m_establishing = FALSE;
      return TRUE;
    }

    // Data available or output buffer ready
    if ((result & GSOCK_INPUT_FLAG) || (result & GSOCK_OUTPUT_FLAG))
    {
      return TRUE;
    }

    // Connection lost
    if (result & GSOCK_LOST_FLAG)
    {
      m_connected = FALSE;
      m_establishing = FALSE;
      return (flags & GSOCK_LOST_FLAG);
    }

    // Wait more?
    if ((!timeout) || (chrono.Time() > timeout) || (m_interrupt))
      done = TRUE;
    else
      PROCESS_EVENTS();
  }

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
  // Check pushback buffer before entering _Wait
  if (m_unread)
    return TRUE;

  // Note that GSOCK_INPUT_LOST has to be explicitly passed to
  // _Wait becuase of the semantics of WaitForRead: a return
  // value of TRUE means that a GSocket_Read call will return
  // immediately, not that there is actually data to read.

  return _Wait(seconds, milliseconds, GSOCK_INPUT_FLAG |
                                      GSOCK_LOST_FLAG);
}

bool wxSocketBase::WaitForWrite(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_OUTPUT_FLAG);
}

bool wxSocketBase::WaitForLost(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_LOST_FLAG);
}

// --------------------------------------------------------------------------
// Miscellaneous
// --------------------------------------------------------------------------

//
// Get local or peer address
//

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

//
// Save and restore socket state
//

void wxSocketBase::SaveState()
{
  wxSocketState *state;

  state = new wxSocketState();

  state->m_flags      = m_flags;
  state->m_notify     = m_notify;
  state->m_eventmask  = m_eventmask;
  state->m_clientData = m_clientData;
#if WXWIN_COMPATIBILITY
  state->m_cbk        = m_cbk;
  state->m_cdata      = m_cdata;
#endif // WXWIN_COMPATIBILITY

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

  m_flags      = state->m_flags;
  m_notify     = state->m_notify;
  m_eventmask  = state->m_eventmask;
  m_clientData = state->m_clientData;
#if WXWIN_COMPATIBILITY
  m_cbk        = state->m_cbk;
  m_cdata      = state->m_cdata;
#endif // WXWIN_COMPATIBILITY

  delete node;
  delete state;
}

//
// Timeout and flags
//

void wxSocketBase::SetTimeout(long seconds)
{
  m_timeout = seconds;

  if (m_socket)
    GSocket_SetTimeout(m_socket, m_timeout * 1000);
}

void wxSocketBase::SetFlags(wxSocketFlags flags)
{
  m_flags = flags;
}


// --------------------------------------------------------------------------
// Callbacks (now obsolete - use events instead)
// --------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY

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

#endif // WXWIN_COMPATIBILITY

// --------------------------------------------------------------------------
// Event handling
// --------------------------------------------------------------------------

// A note on how events are processed, which is probably the most
// difficult thing to get working right while keeping the same API
// and functionality for all platforms.
//
// When GSocket detects an event, it calls wx_socket_callback, which in
// turn just calls wxSocketBase::OnRequest in the corresponding wxSocket
// object. OnRequest does some housekeeping, and if the event is to be
// propagated to the user, it creates a new wxSocketEvent object and
// posts it. The event is not processed immediately, but delayed with
// AddPendingEvent instead. This is necessary in order to decouple the
// event processing from wx_socket_callback; otherwise, subsequent IO
// calls made from the user event handler would fail, as gtk callbacks
// are not reentrant.
//
// Note that, unlike events, user callbacks (now deprecated) are _not_
// decoupled from wx_socket_callback and thus they suffer from a variety
// of problems. Avoid them where possible and use events instead.

static void LINKAGEMODE wx_socket_callback(GSocket * WXUNUSED(socket),
                                           GSocketEvent notification,
                                           char *cdata)
{
  wxSocketBase *sckobj = (wxSocketBase *)cdata;

  sckobj->OnRequest((wxSocketNotify) notification);
}

void wxSocketBase::OnRequest(wxSocketNotify notification)
{
  // NOTE: We duplicate some of the code in _Wait, but this doesn't
  //   hurt. It has to be here because the (GSocket) event might arrive
  //   a bit delayed, and it has to be in _Wait as well because we don't
  //   know whether the Wait functions are being used.

  switch(notification)
  {
    case wxSOCKET_CONNECTION:
      m_establishing = FALSE;
      m_connected = TRUE;
      break;

    // If we are in the middle of a R/W operation, do not
    // propagate events to users. Also, filter 'late' events
    // which are no longer valid.

    case wxSOCKET_INPUT:
      if (m_reading || !GSocket_Select(m_socket, GSOCK_INPUT_FLAG))
        return;
      break;

    case wxSOCKET_OUTPUT:
      if (m_writing || !GSocket_Select(m_socket, GSOCK_OUTPUT_FLAG))
        return;
      break;

    case wxSOCKET_LOST:
      m_connected = FALSE;
      m_establishing = FALSE;
      break;

    default:
      break;
  }

  // Schedule the event

  wxSocketEventFlags flag = -1;
  switch (notification)
  {
    case GSOCK_INPUT:      flag = GSOCK_INPUT_FLAG; break;
    case GSOCK_OUTPUT:     flag = GSOCK_OUTPUT_FLAG; break;
    case GSOCK_CONNECTION: flag = GSOCK_CONNECTION_FLAG; break;
    case GSOCK_LOST:       flag = GSOCK_LOST_FLAG; break;
  }

  if (((m_eventmask & flag) == flag) && m_notify)
  {
    if (m_handler)
    {
      wxSocketEvent event(m_id);
      event.m_event      = notification;
      event.m_clientData = m_clientData;
      event.SetEventObject(this);

      m_handler->AddPendingEvent(event);
    }

#if WXWIN_COMPATIBILITY
    if (m_cbk)
      m_cbk(*this, notification, m_cdata);
#endif // WXWIN_COMPATIBILITY
  }
}

void wxSocketBase::Notify(bool notify)
{
  m_notify = notify;
}

void wxSocketBase::SetNotify(wxSocketEventFlags flags)
{
  m_eventmask = flags;
}

void wxSocketBase::SetEventHandler(wxEvtHandler& handler, int id)
{
  m_handler = &handler;
  m_id      = id;
}

// --------------------------------------------------------------------------
// Pushback buffer
// --------------------------------------------------------------------------

void wxSocketBase::Pushback(const void *buffer, wxUint32 size)
{
  if (!size) return;

  if (m_unread == NULL)
    m_unread = malloc(size);
  else
  {
    void *tmp;

    tmp = malloc(m_unrd_size + size);
    memcpy((char *)tmp + size, m_unread, m_unrd_size);
    free(m_unread);

    m_unread = tmp;
  }

  m_unrd_size += size;

  memcpy(m_unread, buffer, size);
}

wxUint32 wxSocketBase::GetPushback(void *buffer, wxUint32 size, bool peek)
{
  if (!m_unrd_size)
    return 0;

  if (size > (m_unrd_size-m_unrd_cur))
    size = m_unrd_size-m_unrd_cur;

  memcpy(buffer, (char *)m_unread + m_unrd_cur, size);

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


// ==========================================================================
// wxSocketServer                             
// ==========================================================================

// --------------------------------------------------------------------------
// Ctor
// --------------------------------------------------------------------------

wxSocketServer::wxSocketServer(wxSockAddress& addr_man,
                               wxSocketFlags flags)
              : wxSocketBase(flags, wxSOCKET_SERVER)
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

// --------------------------------------------------------------------------
// Accept
// --------------------------------------------------------------------------

bool wxSocketServer::AcceptWith(wxSocketBase& sock, bool wait)
{
  GSocket *child_socket;

  if (!m_socket)
    return FALSE;

  // If wait == FALSE, then the call should be nonblocking.
  // When we are finished, we put the socket to blocking mode
  // again.

  if (!wait)
    GSocket_SetNonBlocking(m_socket, TRUE);

  child_socket = GSocket_WaitConnection(m_socket);

  if (!wait)
    GSocket_SetNonBlocking(m_socket, FALSE);

  if (!child_socket)
    return FALSE;

  sock.m_type = wxSOCKET_BASE;
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

  sock->SetFlags(m_flags);

  if (!AcceptWith(*sock, wait))
    return NULL;

  return sock;
}

bool wxSocketServer::WaitForAccept(long seconds, long milliseconds)
{
  return _Wait(seconds, milliseconds, GSOCK_CONNECTION_FLAG);
}

// ==========================================================================
// wxSocketClient
// ==========================================================================

// --------------------------------------------------------------------------
// Ctor and dtor
// --------------------------------------------------------------------------

wxSocketClient::wxSocketClient(wxSocketFlags flags)
              : wxSocketBase(flags, wxSOCKET_CLIENT)
{
}

wxSocketClient::~wxSocketClient()
{
}

// --------------------------------------------------------------------------
// Connect
// --------------------------------------------------------------------------

bool wxSocketClient::Connect(wxSockAddress& addr_man, bool wait)
{
  GSocketError err;

  if (m_socket)
  {
    // Shutdown and destroy the socket
    Close();
    GSocket_destroy(m_socket);
  }

  m_socket = GSocket_new();
  m_connected = FALSE;
  m_establishing = FALSE;

  if (!m_socket)
    return FALSE;

  GSocket_SetTimeout(m_socket, m_timeout * 1000);
  GSocket_SetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                wx_socket_callback, (char *)this);

  // If wait == FALSE, then the call should be nonblocking.
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

  return _Wait(seconds, milliseconds, GSOCK_CONNECTION_FLAG |
                                      GSOCK_LOST_FLAG);
}

// ==========================================================================
// wxDatagramSocket
// ==========================================================================

/* NOTE: experimental stuff - might change */

wxDatagramSocket::wxDatagramSocket( wxSockAddress& addr,
                                    wxSocketFlags flags )
                : wxSocketBase( flags, wxSOCKET_DATAGRAM )
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
                                              void* buf,
                                              wxUint32 nBytes )
{
    Read(buf, nBytes);
    GetPeer(addr);
    return (*this);
}

wxDatagramSocket& wxDatagramSocket::SendTo( wxSockAddress& addr,
                                            const void* buf,
                                            wxUint32 nBytes )
{
    GSocket_SetPeer(m_socket, addr.GetAddress());
    Write(buf, nBytes);
    return (*this);
}

// ==========================================================================
// wxSocketEvent
// ==========================================================================

wxSocketEvent::wxSocketEvent(int id) : wxEvent(id)
{
  SetEventType( (wxEventType)wxEVT_SOCKET );
}

void wxSocketEvent::CopyObject(wxObject& object_dest) const
{
  wxSocketEvent *event = (wxSocketEvent *)&object_dest;

  wxEvent::CopyObject(object_dest);

  event->m_event      = m_event;
  event->m_clientData = m_clientData;
}

// ==========================================================================
// wxSocketModule
// ==========================================================================

class WXDLLEXPORT wxSocketModule : public wxModule
{
  DECLARE_DYNAMIC_CLASS(wxSocketModule)

public:
  bool OnInit() { return GSocket_Init(); }
  void OnExit() { GSocket_Cleanup(); }
};

IMPLEMENT_DYNAMIC_CLASS(wxSocketModule, wxModule)

#endif
  // wxUSE_SOCKETS
