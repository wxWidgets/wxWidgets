/////////////////////////////////////////////////////////////////////////////
// Name:       src/mac/carbon/cfsocket.cpp
// Purpose:    Socket handler classes
// Authors:    Guilhem Lavaux, Guillermo Rodriguez Garcia
// Created:    April 1997
// Copyright:  (C) 1999-1997, Guilhem Lavaux
//             (C) 2000-1999, Guillermo Rodriguez Garcia
// RCS_ID:     $Id$
// License:    see wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SOCKETS

#include "wx/socket.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/timer.h"
    #include "wx/module.h"
#endif

#include "wx/apptrait.h"

#include "wx/sckaddr.h"
#include "wx/mac/carbon/private.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define HAVE_INET_ATON

// DLL options compatibility check:
#include "wx/build.h"

WX_CHECK_BUILD_OPTIONS("wxNet")


// discard buffer
#define MAX_DISCARD_SIZE (10 * 1024)

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

// what to do within waits: we have 2 cases: from the main thread itself we
// have to call wxYield() to let the events (including the GUI events and the
// low-level (not wxWidgets) events from GSocket) be processed. From another
// thread it is enough to just call wxThread::Yield() which will give away the
// rest of our time slice: the explanation is that the events will be processed
// by the main thread anyhow, without calling wxYield(), but we don't want to
// eat the CPU time uselessly while sitting in the loop waiting for the data
#if wxUSE_THREADS
    #define PROCESS_EVENTS()        \
    {                               \
        if ( wxThread::IsMain() )   \
            wxYield();              \
        else                        \
            wxThread::Yield();      \
    }
#else // !wxUSE_THREADS
    #define PROCESS_EVENTS() wxYield()
#endif // wxUSE_THREADS/!wxUSE_THREADS

#define wxTRACE_Socket _T("wxSocket")


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

public:
  wxSocketState() : wxObject() {}

    DECLARE_NO_COPY_CLASS(wxSocketState)
};

struct _GSocket
{
  CFSocketNativeHandle m_fd;
  GAddress *m_local;
  GAddress *m_peer;
  GSocketError m_error;

  int m_non_blocking;
  int m_server;
  int m_stream;
  int m_oriented;
  int m_establishing;
  unsigned long m_timeout;

  // Callbacks
  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];

  CFSocketRef           m_cfSocket;
  CFRunLoopSourceRef    m_runLoopSource;
  CFReadStreamRef       m_readStream ;
  CFWriteStreamRef      m_writeStream ;
};

struct _GAddress
{
  struct sockaddr *m_addr;
  size_t m_len;

  GAddressType m_family;
  int m_realfamily;

  GSocketError m_error;
  int somethingElse ;
};

void wxMacCFSocketCallback(CFSocketRef s, CFSocketCallBackType callbackType,
                         CFDataRef address, const void* data, void* info) ;
void _GSocket_Enable(GSocket *socket, GSocketEvent event) ;
void _GSocket_Disable(GSocket *socket, GSocketEvent event) ;

// ==========================================================================
// wxSocketBase
// ==========================================================================

// --------------------------------------------------------------------------
// Initialization and shutdown
// --------------------------------------------------------------------------

// FIXME-MT: all this is MT-unsafe, of course, we should protect all accesses
//           to m_countInit with a crit section
size_t wxSocketBase::m_countInit = 0;

bool wxSocketBase::IsInitialized()
{
    return m_countInit > 0;
}

bool wxSocketBase::Initialize()
{
    if ( !m_countInit++ )
    {
#if 0
        wxAppTraits *traits = wxAppConsole::GetInstance() ?
                              wxAppConsole::GetInstance()->GetTraits() : NULL;
        GSocketGUIFunctionsTable *functions =
            traits ? traits->GetSocketGUIFunctionsTable() : NULL;
        GSocket_SetGUIFunctions(functions);

        if ( !GSocket_Init() )
        {
            m_countInit--;

            return false;
        }
#endif
    }

    return true;
}

void wxSocketBase::Shutdown()
{
    // we should be initialized
    wxASSERT_MSG( m_countInit, wxT("extra call to Shutdown()") );
    if ( !--m_countInit )
    {
#if 0
        GSocket_Cleanup();
#endif
    }
}

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
  m_error        = false;
  m_lcount       = 0;
  m_timeout      = 600;
  m_beingDeleted = false;

  // pushback buffer
  m_unread       = NULL;
  m_unrd_size    = 0;
  m_unrd_cur     = 0;

  // events
  m_id           = -1;
  m_handler      = NULL;
  m_clientData   = NULL;
  m_notify       = false;
  m_eventmask    = 0;

  if ( !IsInitialized() )
  {
      // this Initialize() will be undone by wxSocketModule::OnExit(), all the
      // other calls to it should be matched by a call to Shutdown()
      Initialize();
  }
}

wxSocketBase::wxSocketBase()
{
  Init();
}

wxSocketBase::wxSocketBase( wxSocketFlags flags, wxSocketType type)
{
  Init();

  m_flags = flags;
  m_type  = type;
}

wxSocketBase::~wxSocketBase()
{
  // Just in case the app called Destroy() *and* then deleted
  // the socket immediately: don't leave dangling pointers.
  wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
  if ( traits )
      traits->RemoveFromPendingDelete(this);

  // Shutdown and close the socket
  if (!m_beingDeleted)
    Close();

  // Destroy the GSocket object
  if (m_socket)
  {
    GSocket_destroy(m_socket);
  }

  // Free the pushback buffer
  if (m_unread)
    free(m_unread);
}

bool wxSocketBase::Destroy()
{
  // Delayed destruction: the socket will be deleted during the next
  // idle loop iteration. This ensures that all pending events have
  // been processed.
  m_beingDeleted = true;

  // Shutdown and close the socket
  Close();

  // Supress events from now on
  Notify(false);

  // schedule this object for deletion
  wxAppTraits *traits = wxTheApp ? wxTheApp->GetTraits() : NULL;
  if ( traits )
  {
      // let the traits object decide what to do with us
      traits->ScheduleForDestroy(this);
  }
  else // no app or no traits
  {
      // in wxBase we might have no app object at all, don't leak memory
      delete this;
  }

  return true;
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
  InterruptWait();

  if (m_socket)
    GSocket_Shutdown(m_socket);

  m_connected = false;
  m_establishing = false;

  return true;
}

wxSocketBase& wxSocketBase::Read(void* buffer, wxUint32 nbytes)
{
  // Mask read events
  m_reading = true;

  m_lcount = _Read(buffer, nbytes);

  // If in wxSOCKET_WAITALL mode, all bytes should have been read.
  if (m_flags & wxSOCKET_WAITALL)
    m_error = (m_lcount != nbytes);
  else
    m_error = (m_lcount == 0);

  // Allow read events from now on
  m_reading = false;

  return *this;
}

wxUint32 wxSocketBase::_Read(void* buffer, wxUint32 nbytes)
{
  int total = 0;

  // Try the pushback buffer first
  total = GetPushback(buffer, nbytes, false);
  nbytes -= total;
  buffer  = (char *)buffer + total;

  // Return now in one of the following cases:
  // - the socket is invalid,
  // - we got all the data,
  // - we got *some* data and we are not using wxSOCKET_WAITALL.
  if ( !m_socket ||
       !nbytes ||
       ((total != 0) && !(m_flags & wxSOCKET_WAITALL)) )
    return total;

  // Possible combinations (they are checked in this order)
  // wxSOCKET_NOWAIT
  // wxSOCKET_WAITALL (with or without wxSOCKET_BLOCK)
  // wxSOCKET_BLOCK
  // wxSOCKET_NONE
  //

  int ret;
  if (m_flags & wxSOCKET_NOWAIT)
  {
    GSocket_SetNonBlocking(m_socket, 1);
    ret = GSocket_Read(m_socket, (char *)buffer, nbytes);
    GSocket_SetNonBlocking(m_socket, 0);

    if (ret > 0)
      total += ret;
  }
  else
  {
    bool more = true;

    while (more)
    {
      if ( !(m_flags & wxSOCKET_BLOCK) && !WaitForRead() )
        break;

      ret = GSocket_Read(m_socket, (char *)buffer, nbytes);

      if (ret > 0)
      {
        total  += ret;
        nbytes -= ret;
        buffer  = (char *)buffer + ret;
      }

      // If we got here and wxSOCKET_WAITALL is not set, we can leave
      // now. Otherwise, wait until we recv all the data or until there
      // is an error.
      //
      more = (ret > 0 && nbytes > 0 && (m_flags & wxSOCKET_WAITALL));
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
  }
  msg;

  // Mask read events
  m_reading = true;

  total = 0;
  error = true;
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
    wxLogWarning( wxT("wxSocket: invalid signature in ReadMsg.") );
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
    wxLogWarning( wxT("wxSocket: invalid signature in ReadMsg.") );
    goto exit;
  }

  // everything was OK
  error = false;

exit:
  m_error = error;
  m_lcount = total;
  m_reading = false;
  SetFlags(old_flags);

  return *this;
}

wxSocketBase& wxSocketBase::Peek(void* buffer, wxUint32 nbytes)
{
  // Mask read events
  m_reading = true;

  m_lcount = _Read(buffer, nbytes);
  Pushback(buffer, m_lcount);

  // If in wxSOCKET_WAITALL mode, all bytes should have been read.
  if (m_flags & wxSOCKET_WAITALL)
    m_error = (m_lcount != nbytes);
  else
    m_error = (m_lcount == 0);

  // Allow read events again
  m_reading = false;

  return *this;
}

wxSocketBase& wxSocketBase::Write(const void *buffer, wxUint32 nbytes)
{
  // Mask write events
  m_writing = true;

  m_lcount = _Write(buffer, nbytes);

  // If in wxSOCKET_WAITALL mode, all bytes should have been written.
  if (m_flags & wxSOCKET_WAITALL)
    m_error = (m_lcount != nbytes);
  else
    m_error = (m_lcount == 0);

  // Allow write events again
  m_writing = false;

  return *this;
}

wxUint32 wxSocketBase::_Write(const void *buffer, wxUint32 nbytes)
{
  wxUint32 total = 0;

  // If the socket is invalid or parameters are ill, return immediately
  if (!m_socket || !buffer || !nbytes)
    return 0;

  // Possible combinations (they are checked in this order)
  // wxSOCKET_NOWAIT
  // wxSOCKET_WAITALL (with or without wxSOCKET_BLOCK)
  // wxSOCKET_BLOCK
  // wxSOCKET_NONE
  //
  int ret;
  if (m_flags & wxSOCKET_NOWAIT)
  {
    GSocket_SetNonBlocking(m_socket, 1);
    ret = GSocket_Write(m_socket, (const char *)buffer, nbytes);
    GSocket_SetNonBlocking(m_socket, 0);

    if (ret > 0)
      total = ret;
  }
  else
  {
    bool more = true;

    while (more)
    {
      if ( !(m_flags & wxSOCKET_BLOCK) && !WaitForWrite() )
        break;

      ret = GSocket_Write(m_socket, (const char *)buffer, nbytes);

      if (ret > 0)
      {
        total  += ret;
        nbytes -= ret;
        buffer  = (const char *)buffer + ret;
      }

      // If we got here and wxSOCKET_WAITALL is not set, we can leave
      // now. Otherwise, wait until we send all the data or until there
      // is an error.
      //
      more = (ret > 0 && nbytes > 0 && (m_flags & wxSOCKET_WAITALL));
    }
  }

  return total;
}

wxSocketBase& wxSocketBase::WriteMsg(const void *buffer, wxUint32 nbytes)
{
  wxUint32 total;
  bool error;
  struct
  {
    unsigned char sig[4];
    unsigned char len[4];
  }
  msg;

  // Mask write events
  m_writing = true;

  error = true;
  total = 0;
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
  error = false;

exit:
  m_error = error;
  m_lcount = total;
  m_writing = false;

  return *this;
}

wxSocketBase& wxSocketBase::Unread(const void *buffer, wxUint32 nbytes)
{
  if (nbytes != 0)
    Pushback(buffer, nbytes);

  m_error = false;
  m_lcount = nbytes;

  return *this;
}

wxSocketBase& wxSocketBase::Discard()
{
  char *buffer = new char[MAX_DISCARD_SIZE];
  wxUint32 ret;
  wxUint32 total = 0;

  // Mask read events
  m_reading = true;

  SetFlags(wxSOCKET_NOWAIT);

  do
  {
    ret = _Read(buffer, MAX_DISCARD_SIZE);
    total += ret;
  }
  while (ret == MAX_DISCARD_SIZE);

  delete[] buffer;
  m_lcount = total;
  m_error  = false;

  // Allow read events again
  m_reading = false;

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

  // Set this to true to interrupt ongoing waits
  m_interrupt = false;

  // Check for valid socket
  if (!m_socket)
    return false;

  // Check for valid timeout value.
  if (seconds != -1)
    timeout = seconds * 1000 + milliseconds;
  else
    timeout = m_timeout * 1000;

#if !defined(wxUSE_GUI) || !wxUSE_GUI
  GSocket_SetTimeout(m_socket, timeout);
#endif

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
  bool done = false;

  while (!done)
  {
    result = GSocket_Select(m_socket, flags | GSOCK_LOST_FLAG);

    // Incoming connection (server) or connection established (client)
    if (result & GSOCK_CONNECTION_FLAG)
    {
      m_connected = true;
      m_establishing = false;

      return true;
    }

    // Data available or output buffer ready
    if ((result & GSOCK_INPUT_FLAG) || (result & GSOCK_OUTPUT_FLAG))
    {
      return true;
    }

    // Connection lost
    if (result & GSOCK_LOST_FLAG)
    {
      m_connected = false;
      m_establishing = false;

      return (flags & GSOCK_LOST_FLAG) != 0;
    }

    // Wait more?
    if ((!timeout) || (chrono.Time() > timeout) || (m_interrupt))
      done = true;
    else
      PROCESS_EVENTS();
  }

  return false;
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
    return true;

  // Note that GSOCK_INPUT_LOST has to be explicitly passed to
  // _Wait becuase of the semantics of WaitForRead: a return
  // value of true means that a GSocket_Read call will return
  // immediately, not that there is actually data to read.

  return _Wait(seconds, milliseconds, GSOCK_INPUT_FLAG | GSOCK_LOST_FLAG);
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
    return false;

  peer = GSocket_GetPeer(m_socket);

    // copying a null address would just trigger an assert anyway

  if (!peer)
    return false;

  addr_man.SetAddress(peer);
  GAddress_destroy(peer);

  return true;
}

bool wxSocketBase::GetLocal(wxSockAddress& addr_man) const
{
#if 0
  GAddress *local;

  if (!m_socket)
    return false;

  local = GSocket_GetLocal(m_socket);
  addr_man.SetAddress(local);
  GAddress_destroy(local);
#endif

  return true;
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

  m_states.Append(state);
}

void wxSocketBase::RestoreState()
{
  wxList::compatibility_iterator node;
  wxSocketState *state;

  node = m_states.GetLast();
  if (!node)
    return;

  state = (wxSocketState *)node->GetData();

  m_flags      = state->m_flags;
  m_notify     = state->m_notify;
  m_eventmask  = state->m_eventmask;
  m_clientData = state->m_clientData;

  m_states.Erase(node);
  delete state;
}

//
// Timeout and flags
//

void wxSocketBase::SetTimeout(long seconds)
{
  m_timeout = seconds;

#if 0
  if (m_socket)
    GSocket_SetTimeout(m_socket, m_timeout * 1000);
#endif
}

void wxSocketBase::SetFlags(wxSocketFlags flags)
{
  m_flags = flags;
}


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

extern "C"
void LINKAGEMODE wx_socket_callback(GSocket * WXUNUSED(socket),
                                    GSocketEvent notification,
                                    char *cdata)
{
  wxSocketBase *sckobj = (wxSocketBase *)cdata;

  sckobj->OnRequest((wxSocketNotify) notification);
}

void wxSocketBase::OnRequest(wxSocketNotify notification)
{
  // NOTE: We duplicate some of the code in _Wait, but this doesn't
  // hurt. It has to be here because the (GSocket) event might arrive
  // a bit delayed, and it has to be in _Wait as well because we don't
  // know whether the Wait functions are being used.

  switch (notification)
  {
    case wxSOCKET_CONNECTION:
      m_establishing = false;
      m_connected = true;
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
      m_connected = false;
      m_establishing = false;
      break;

    default:
      break;
  }

  // Schedule the event

  wxSocketEventFlags flag = 0;
  wxUnusedVar(flag);
  switch (notification)
  {
    case GSOCK_INPUT:
      flag = GSOCK_INPUT_FLAG;
      break;

    case GSOCK_OUTPUT:
      flag = GSOCK_OUTPUT_FLAG;
      break;

    case GSOCK_CONNECTION:
      flag = GSOCK_CONNECTION_FLAG;
      break;

    case GSOCK_LOST:
      flag = GSOCK_LOST_FLAG;
      break;

    default:
      wxLogWarning( wxT("wxSocket: unknown event!") );
      return;
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
  if (!size)
    return;

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
    wxLogTrace( wxTRACE_Socket, wxT("Opening wxSocketServer") );

    m_socket = GSocket_new();

    if (!m_socket)
    {
        wxLogTrace( wxTRACE_Socket, wxT("*** GSocket_new failed") );
        return;
    }

    // Setup the socket as server

#if 0
    GSocket_SetLocal(m_socket, addr_man.GetAddress());
    if (GSocket_SetServer(m_socket) != GSOCK_NOERROR)
    {
        GSocket_destroy(m_socket);
        m_socket = NULL;

        wxLogTrace( wxTRACE_Socket, wxT("*** GSocket_SetServer failed") );
        return;
    }

    GSocket_SetTimeout(m_socket, m_timeout * 1000);
    GSocket_SetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                  GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                  wx_socket_callback, (char *)this);
#endif
}

// --------------------------------------------------------------------------
// Accept
// --------------------------------------------------------------------------

bool wxSocketServer::AcceptWith(wxSocketBase& sock, bool wait)
{
  GSocket *child_socket;

  if (!m_socket)
    return false;

  // If wait == false, then the call should be nonblocking.
  // When we are finished, we put the socket to blocking mode
  // again.

#if 0
  if (!wait)
    GSocket_SetNonBlocking(m_socket, 1);

  child_socket = GSocket_WaitConnection(m_socket);

  if (!wait)
    GSocket_SetNonBlocking(m_socket, 0);

  if (!child_socket)
    return false;

  sock.m_type = wxSOCKET_BASE;
  sock.m_socket = child_socket;
  sock.m_connected = true;

  GSocket_SetTimeout(sock.m_socket, sock.m_timeout * 1000);
  GSocket_SetCallback(sock.m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                     GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                     wx_socket_callback, (char *)&sock);
#endif

  return true;
}

wxSocketBase *wxSocketServer::Accept(bool wait)
{
  wxSocketBase* sock = new wxSocketBase();

  sock->SetFlags(m_flags);

  if (!AcceptWith(*sock, wait))
  {
    sock->Destroy();
    sock = NULL;
  }

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
  m_connected = false;
  m_establishing = false;

  if (!m_socket)
    return false;

  GSocket_SetTimeout(m_socket, m_timeout * 1000);
  GSocket_SetCallback(m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                wx_socket_callback, (char *)this);

  // If wait == false, then the call should be nonblocking.
  // When we are finished, we put the socket to blocking mode
  // again.

  if (!wait)
    GSocket_SetNonBlocking(m_socket, 1);

  GSocket_SetPeer(m_socket, addr_man.GetAddress());
  err = GSocket_Connect(m_socket, GSOCK_STREAMED);

  if (!wait)
    GSocket_SetNonBlocking(m_socket, 0);

  if (err != GSOCK_NOERROR)
  {
    if (err == GSOCK_WOULDBLOCK)
      m_establishing = true;

    return false;
  }

  m_connected = true;
  return true;
}

bool wxSocketClient::WaitOnConnect(long seconds, long milliseconds)
{
  if (m_connected)                      // Already connected
    return true;

  if (!m_establishing || !m_socket)     // No connection in progress
    return false;

  return _Wait(seconds, milliseconds, GSOCK_CONNECTION_FLAG | GSOCK_LOST_FLAG);
}

// ==========================================================================
// wxDatagramSocket
// ==========================================================================

/* NOTE: experimental stuff - might change */

wxDatagramSocket::wxDatagramSocket( wxSockAddress& addr,
                                    wxSocketFlags flags )
                : wxSocketBase( flags, wxSOCKET_DATAGRAM )
{
#if 0
  // Create the socket
  m_socket = GSocket_new();

  if (!m_socket)
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
  m_connected = false;
  m_establishing = false;
  GSocket_SetTimeout( m_socket, m_timeout );
  GSocket_SetCallback( m_socket, GSOCK_INPUT_FLAG | GSOCK_OUTPUT_FLAG |
                                 GSOCK_LOST_FLAG | GSOCK_CONNECTION_FLAG,
                                 wx_socket_callback, (char*)this );
#endif
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

/*
 * -------------------------------------------------------------------------
 * GAddress
 * -------------------------------------------------------------------------
 */

/* CHECK_ADDRESS verifies that the current address family is either
 * GSOCK_NOFAMILY or GSOCK_*family*, and if it is GSOCK_NOFAMILY, it
 * initalizes it to be a GSOCK_*family*. In other cases, it returns
 * an appropiate error code.
 *
 * CHECK_ADDRESS_RETVAL does the same but returning 'retval' on error.
 */
#define CHECK_ADDRESS(address, family)                              \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return address->m_error;                                      \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return GSOCK_INVADDR;                                           \
  }                                                                 \
}

#define CHECK_ADDRESS_RETVAL(address, family, retval)               \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return retval;                                                \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return retval;                                                  \
  }                                                                 \
}


GAddress *GAddress_new(void)
{
  GAddress *address;

  if ((address = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  address->m_family  = GSOCK_NOFAMILY;
  address->m_addr    = NULL;
  address->m_len     = 0;

  return address;
}

GAddress *GAddress_copy(GAddress *address)
{
  GAddress *addr2;

  assert(address != NULL);

  if ((addr2 = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  memcpy(addr2, address, sizeof(GAddress));

  if (address->m_addr && address->m_len > 0)
  {
    addr2->m_addr = (struct sockaddr *)malloc(addr2->m_len);
    if (addr2->m_addr == NULL)
    {
      free(addr2);
      return NULL;
    }

    memcpy(addr2->m_addr, address->m_addr, addr2->m_len);
  }

  return addr2;
}

void GAddress_destroy(GAddress *address)
{
  assert( address != NULL );

  if (address->m_addr)
    free(address->m_addr);

  free(address);
}

void GAddress_SetFamily(GAddress *address, GAddressType type)
{
  assert(address != NULL);

  address->m_family = type;
}

GAddressType GAddress_GetFamily(GAddress *address)
{
  assert( address != NULL );

  return address->m_family;
}

GSocketError _GAddress_translate_from(GAddress *address,
                                      struct sockaddr *addr, int len)
{
  address->m_realfamily = addr->sa_family;
  switch (addr->sa_family)
  {
    case AF_INET:
      address->m_family = GSOCK_INET;
      break;

    case AF_UNIX:
      address->m_family = GSOCK_UNIX;
      break;

#ifdef AF_INET6
    case AF_INET6:
      address->m_family = GSOCK_INET6;
      break;
#endif

    default:
    {
      address->m_error = GSOCK_INVOP;
      return GSOCK_INVOP;
    }
  }

  if (address->m_addr)
    free(address->m_addr);

  address->m_len  = len;
  address->m_addr = (struct sockaddr *)malloc(len);

  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  memcpy(address->m_addr, addr, len);

  return GSOCK_NOERROR;
}

GSocketError _GAddress_translate_to(GAddress *address,
                                    struct sockaddr **addr, int *len)
{
  if (!address->m_addr)
  {
    address->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  *len = address->m_len;
  *addr = (struct sockaddr *)malloc(address->m_len);
  if (*addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  memcpy(*addr, address->m_addr, address->m_len);
  return GSOCK_NOERROR;
}

/*
 * -------------------------------------------------------------------------
 * Internet address family
 * -------------------------------------------------------------------------
 */

GSocketError _GAddress_Init_INET(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_in);
  address->m_addr = (struct sockaddr *) malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  memset( address->m_addr , 0 , address->m_len ) ;
  address->m_family = GSOCK_INET;
  address->m_realfamily = PF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_family = AF_INET;
  ((struct sockaddr_in *)address->m_addr)->sin_addr.s_addr = INADDR_ANY;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{
  struct hostent *he;
  struct in_addr *addr;

  assert( address != NULL );
  CHECK_ADDRESS( address, INET );

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);

  // If it is a numeric host name, convert it now
#if defined(HAVE_INET_ATON)
  if (inet_aton(hostname, addr) == 0)
  {
#elif defined(HAVE_INET_ADDR)
  if ( (addr->s_addr = inet_addr(hostname)) == -1 )
  {
#else
  // Use gethostbyname by default
#ifndef __WXMAC__
  int val = 1;  // VA doesn't like constants in conditional expressions
  if (val)
#endif
  {
#endif
    struct in_addr *array_addr;

    // It is a real name, we solve it
    if ((he = gethostbyname(hostname)) == NULL)
    {
      // Reset to invalid address
      addr->s_addr = INADDR_NONE;
      address->m_error = GSOCK_NOHOST;
      return GSOCK_NOHOST;
    }
    array_addr = (struct in_addr *) *(he->h_addr_list);
    addr->s_addr = array_addr[0].s_addr;
  }

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetAnyAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_ANY);
}

GSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr)
{
  struct in_addr *addr;

  assert( address != NULL );
  CHECK_ADDRESS( address, INET );

  addr = &(((struct sockaddr_in *)address->m_addr)->sin_addr);
  addr->s_addr = htonl(hostaddr) ;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  struct servent *se;
  struct sockaddr_in *addr;

  assert( address != NULL );
  CHECK_ADDRESS( address, INET );

  if (!port)
  {
    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

  se = getservbyname(port, protocol);
  if (!se)
  {
    // the cast to int suppresses compiler warnings
    // about subscript having the type char
    if (isdigit((int)port[0]))
    {
      int port_int;

      port_int = atoi(port);
      addr = (struct sockaddr_in *)address->m_addr;
      addr->sin_port = htons(port_int);
      return GSOCK_NOERROR;
    }

    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }

  addr = (struct sockaddr_in *)address->m_addr;
  addr->sin_port = se->s_port;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port)
{
  struct sockaddr_in *addr;

  assert( address != NULL );
  CHECK_ADDRESS( address, INET );

  addr = (struct sockaddr_in *)address->m_addr;
  addr->sin_port = htons(port);

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_GetHostName(GAddress *address, char *hostname, size_t sbuf)
{
  struct hostent *he;
  char *addr_buf;
  struct sockaddr_in *addr;

  assert( address != NULL );
  CHECK_ADDRESS( address, INET );

  addr = (struct sockaddr_in *)address->m_addr;
  addr_buf = (char *)&(addr->sin_addr);

  he = gethostbyaddr(addr_buf, sizeof(addr->sin_addr), AF_INET);
  if (he == NULL)
  {
    address->m_error = GSOCK_NOHOST;
    return GSOCK_NOHOST;
  }

  strncpy(hostname, he->h_name, sbuf);

  return GSOCK_NOERROR;
}

unsigned long GAddress_INET_GetHostAddress(GAddress *address)
{
  struct sockaddr_in *addr;

  assert( address != NULL );
  CHECK_ADDRESS_RETVAL( address, INET, 0 );

  addr = (struct sockaddr_in *)address->m_addr;

  return ntohl(addr->sin_addr.s_addr) ;
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  struct sockaddr_in *addr;

  assert( address != NULL );
  CHECK_ADDRESS_RETVAL( address, INET, 0 );

  addr = (struct sockaddr_in *)address->m_addr;

  return ntohs(addr->sin_port);
}

/*
 * -------------------------------------------------------------------------
 * Unix address family
 * -------------------------------------------------------------------------
 */

GSocketError _GAddress_Init_UNIX(GAddress *address)
{
  address->m_len  = sizeof(struct sockaddr_un);
  address->m_addr = (struct sockaddr *)malloc(address->m_len);
  if (address->m_addr == NULL)
  {
    address->m_error = GSOCK_MEMERR;
    return GSOCK_MEMERR;
  }

  address->m_family = GSOCK_UNIX;
  address->m_realfamily = PF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_family = AF_UNIX;
  ((struct sockaddr_un *)address->m_addr)->sun_path[0] = 0;

  return GSOCK_NOERROR;
}

#define UNIX_SOCK_PATHLEN (sizeof(addr->sun_path)/sizeof(addr->sun_path[0]))

GSocketError GAddress_UNIX_SetPath(GAddress *address, const char *path)
{
  struct sockaddr_un *addr;

  assert( address != NULL );
  CHECK_ADDRESS( address, UNIX );

  addr = ((struct sockaddr_un *)address->m_addr);
  strncpy(addr->sun_path, path, UNIX_SOCK_PATHLEN);
  addr->sun_path[UNIX_SOCK_PATHLEN - 1] = '\0';

  return GSOCK_NOERROR;
}

GSocketError GAddress_UNIX_GetPath(GAddress *address, char *path, size_t sbuf)
{
  struct sockaddr_un *addr;

  assert( address != NULL );
  CHECK_ADDRESS( address, UNIX );

  addr = (struct sockaddr_un *)address->m_addr;

  strncpy(path, addr->sun_path, sbuf);

  return GSOCK_NOERROR;
}

/* Address handling */

/* GSocket_SetLocal:
 * GSocket_GetLocal:
 * GSocket_SetPeer:
 * GSocket_GetPeer:
 *  Set or get the local or peer address for this socket. The 'set'
 *  functions return GSOCK_NOERROR on success, an error code otherwise.
 *  The 'get' functions return a pointer to a GAddress object on success,
 *  or NULL otherwise, in which case they set the error code of the
 *  corresponding GSocket.
 *
 *  Error codes:
 *    GSOCK_INVSOCK - the socket is not valid.
 *    GSOCK_INVADDR - the address is not valid.
 */

GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address)
{
  assert( socket != NULL );

  // the socket must be initialized, or it must be a server
  if ((socket->m_fd != INVALID_SOCKET && !socket->m_server))
  {
    socket->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  // check address
  if (address == NULL || address->m_family == GSOCK_NOFAMILY)
  {
    socket->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  if (socket->m_local)
    GAddress_destroy(socket->m_local);

  socket->m_local = GAddress_copy(address);

  return GSOCK_NOERROR;
}

GSocketError GSocket_SetPeer(GSocket *socket, GAddress *address)
{
  assert(socket != NULL);

  // check address
  if (address == NULL || address->m_family == GSOCK_NOFAMILY)
  {
    socket->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  if (socket->m_peer)
    GAddress_destroy(socket->m_peer);

  socket->m_peer = GAddress_copy(address);

  return GSOCK_NOERROR;
}

GAddress *GSocket_GetLocal(GSocket *socket)
{
  GAddress *address;
  struct sockaddr addr;
  socklen_t size = sizeof(addr);
  GSocketError err;

  assert( socket != NULL );

  // try to get it from the m_local var first
  if (socket->m_local)
    return GAddress_copy(socket->m_local);

  // else, if the socket is initialized, try getsockname
  if (socket->m_fd == INVALID_SOCKET)
  {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  if (getsockname(socket->m_fd, &addr, (socklen_t *) &size) < 0)
  {
    socket->m_error = GSOCK_IOERR;
    return NULL;
  }

  // got a valid address from getsockname, create a GAddress object
  address = GAddress_new();
  if (address == NULL)
  {
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }

  err = _GAddress_translate_from(address, &addr, size);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(address);
    socket->m_error = err;
    return NULL;
  }

  return address;
}

GAddress *GSocket_GetPeer(GSocket *socket)
{
  assert(socket != NULL);

  // try to get it from the m_peer var
  if (socket->m_peer)
    return GAddress_copy(socket->m_peer);

  return NULL;
}


GSocket *GSocket_new(void)
{
  GSocket *socket;
  socket = (GSocket *)malloc(sizeof(GSocket));

  if (socket == NULL)
    return NULL;

  socket->m_fd                  = INVALID_SOCKET;

  for (int i=0;i<GSOCK_MAX_EVENT;i++)
  {
    socket->m_cbacks[i] = NULL;
  }

  socket->m_detected            = 0;

  socket->m_local               = NULL;
  socket->m_peer                = NULL;
  socket->m_error               = GSOCK_NOERROR;

  socket->m_non_blocking        = false ;
  socket->m_stream              = true;
//  socket->m_oriented            = true;
  socket->m_server              = false;
  socket->m_establishing        = false;
  socket->m_timeout             = 10 * 60 * 1000;
                                // 10 minutes * 60 sec * 1000 millisec

  socket->m_cfSocket            = NULL ;
  socket->m_runLoopSource       = NULL ;
  socket->m_readStream          = NULL;
  socket->m_writeStream         = NULL;

  return socket ;
}

void GSocket_close(GSocket *socket)
{
    if ( socket->m_cfSocket != NULL )
    {
        if ( socket->m_readStream )
        {
            CFReadStreamClose(socket->m_readStream);
            CFRelease( socket->m_readStream ) ;
            socket->m_readStream = NULL ;
        }

        if ( socket->m_writeStream )
        {
            CFWriteStreamClose(socket->m_writeStream);
            CFRelease( socket->m_writeStream ) ;
            socket->m_writeStream = NULL ;
        }

        CFSocketInvalidate( socket->m_cfSocket ) ;
        CFRelease( socket->m_cfSocket ) ;
        socket->m_cfSocket = NULL ;
        socket->m_fd = INVALID_SOCKET ;
    }
}

void GSocket_Shutdown(GSocket *socket)
{
    GSocket_close( socket );

    // Disable GUI callbacks
    for (int evt = 0; evt < GSOCK_MAX_EVENT; evt++)
        socket->m_cbacks[evt] = NULL;

    socket->m_detected = GSOCK_LOST_FLAG;
}

void GSocket_destroy(GSocket *socket)
{
  assert( socket != NULL );

  // Check that the socket is really shut down
  if (socket->m_fd != INVALID_SOCKET)
    GSocket_Shutdown(socket);

  // Destroy private addresses
  if (socket->m_local)
    GAddress_destroy(socket->m_local);

  if (socket->m_peer)
    GAddress_destroy(socket->m_peer);

  // Destroy the socket itself
  free(socket);
}

GSocketError GSocket_Connect(GSocket *socket, GSocketStream stream)
{
  assert( socket != NULL );

  if (socket->m_fd != INVALID_SOCKET)
  {
    socket->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!socket->m_peer)
  {
    socket->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  // Streamed or dgram socket?
  socket->m_stream   = (stream == GSOCK_STREAMED);
  socket->m_oriented = true;
  socket->m_server = false;
  socket->m_establishing = false;

  GSocketError returnErr = GSOCK_NOERROR ;
  CFSocketError err ;

  CFAllocatorRef alloc = kCFAllocatorDefault ;
  CFSocketContext ctx ;
  memset( &ctx , 0 , sizeof( ctx ) ) ;
  ctx.info = socket ;
  socket->m_cfSocket = CFSocketCreate( alloc , socket->m_peer->m_realfamily ,
     stream == GSOCK_STREAMED ? SOCK_STREAM : SOCK_DGRAM , 0 ,
      kCFSocketReadCallBack | kCFSocketWriteCallBack | kCFSocketConnectCallBack , wxMacCFSocketCallback , &ctx  ) ;
  _GSocket_Enable(socket, GSOCK_CONNECTION);

  socket->m_fd = CFSocketGetNative( socket->m_cfSocket ) ;

  CFStreamCreatePairWithSocket ( alloc , socket->m_fd , &socket->m_readStream , &socket->m_writeStream  );
  if ((socket->m_readStream == NULL) || (socket->m_writeStream == NULL))
  {
    GSocket_close(socket);
    socket->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  if ( !CFReadStreamOpen( socket->m_readStream ) || !CFWriteStreamOpen( socket->m_writeStream ) )
  {
    GSocket_close(socket);
    socket->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  CFRunLoopSourceRef rls = CFSocketCreateRunLoopSource(alloc , socket->m_cfSocket , 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent() , rls, kCFRunLoopCommonModes);
  CFRelease(rls);

  CFDataRef address = CFDataCreateWithBytesNoCopy(alloc, (const UInt8*) socket->m_peer->m_addr, socket->m_peer->m_len , kCFAllocatorNull);
  if ( !address )
    return GSOCK_MEMERR ;

  err = CFSocketConnectToAddress( socket->m_cfSocket , address, socket->m_non_blocking ? -1 : socket->m_timeout / 1000  ) ;
  CFRelease(address);

  if (err != kCFSocketSuccess)
  {
    if ( err == kCFSocketTimeout )
    {
      GSocket_close(socket);
      socket->m_error = GSOCK_TIMEDOUT ;
      return GSOCK_TIMEDOUT ;
    }

    // we don't know whether a connect in progress will be issued like this
    if ( err != kCFSocketTimeout && socket->m_non_blocking )
    {
      socket->m_establishing = true;
      socket->m_error = GSOCK_WOULDBLOCK;
      return GSOCK_WOULDBLOCK;
    }

    GSocket_close(socket);
    socket->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  return GSOCK_NOERROR;
}

/* Flags */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode.
 *  All IO calls will return immediately.
 */
void GSocket_SetNonBlocking(GSocket *socket, int non_block)
{
  assert( socket != NULL );

//  GSocket_Debug( ("GSocket_SetNonBlocking: %d\n", (int)non_block) );

  socket->m_non_blocking = non_block;
}

/*
 *  GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void GSocket_SetTimeout(GSocket *socket, unsigned long millisec)
{
  assert( socket != NULL );

  socket->m_timeout = millisec;
}

/* GSocket_GetError:
 *  Returns the last error which occurred for this socket. Note that successful
 *  operations do not clear this back to GSOCK_NOERROR, so use it only
 *  after an error.
 */
GSocketError GSocket_GetError(GSocket *socket)
{
  assert( socket != NULL );

  return socket->m_error;
}

/* Callbacks */

/* GSOCK_INPUT:
 *   There is data to be read in the input buffer. If, after a read
 *   operation, there is still data available, the callback function will
 *   be called again.
 * GSOCK_OUTPUT:
 *   The socket is available for writing. That is, the next write call
 *   won't block. This event is generated only once, when the connection is
 *   first established, and then only if a call failed with GSOCK_WOULDBLOCK,
 *   when the output buffer empties again. This means that the app should
 *   assume that it can write since the first OUTPUT event, and no more
 *   OUTPUT events will be generated unless an error occurs.
 * GSOCK_CONNECTION:
 *   Connection successfully established, for client sockets, or incoming
 *   client connection, for server sockets. Wait for this event (also watch
 *   out for GSOCK_LOST) after you issue a nonblocking GSocket_Connect() call.
 * GSOCK_LOST:
 *   The connection is lost (or a connection request failed); this could
 *   be due to a failure, or due to the peer closing it gracefully.
 */

/* GSocket_SetCallback:
 *  Enables the callbacks specified by 'flags'. Note that 'flags'
 *  may be a combination of flags OR'ed toghether, so the same
 *  callback function can be made to accept different events.
 *  The callback function must have the following prototype:
 *
 *  void function(GSocket *socket, GSocketEvent event, char *cdata)
 */
void GSocket_SetCallback(GSocket *socket, GSocketEventFlags flags,
                         GSocketCallback callback, char *cdata)
{
  int count;

  assert( socket != NULL );

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      socket->m_cbacks[count] = callback;
      socket->m_data[count] = cdata;
    }
  }
}

/* GSocket_UnsetCallback:
 *  Disables all callbacks specified by 'flags', which may be a
 *  combination of flags OR'ed toghether.
 */
void GSocket_UnsetCallback(GSocket *socket, GSocketEventFlags flags)
{
  int count;

  assert(socket != NULL);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      socket->m_cbacks[count] = NULL;
      socket->m_data[count] = NULL;
    }
  }
}


#define CALL_CALLBACK(socket, event) {                                  \
  _GSocket_Disable(socket, event);                                      \
  if (socket->m_cbacks[event])                                          \
    socket->m_cbacks[event](socket, event, socket->m_data[event]);      \
}

void _GSocket_Install_Callback(GSocket *socket, GSocketEvent event)
{
    int c;
    switch (event)
    {
     case GSOCK_CONNECTION:
         if (socket->m_server)
            c = kCFSocketReadCallBack;
         else
            c = kCFSocketConnectCallBack;
         break;

     case GSOCK_LOST:
     case GSOCK_INPUT:
         c = kCFSocketReadCallBack;
         break;

     case GSOCK_OUTPUT:
         c = kCFSocketWriteCallBack;
         break;

     default:
         c = 0;
    }

    CFSocketEnableCallBacks(socket->m_cfSocket, c);
}

void _GSocket_Uninstall_Callback(GSocket *socket, GSocketEvent event)
{
    int c;
    switch (event)
    {
     case GSOCK_CONNECTION:
         if (socket->m_server)
            c = kCFSocketReadCallBack;
         else
            c = kCFSocketConnectCallBack;
         break;

     case GSOCK_LOST:
     case GSOCK_INPUT:
         c = kCFSocketReadCallBack;
         break;

     case GSOCK_OUTPUT:
         c = kCFSocketWriteCallBack;
         break;

     default:
         c = 0;
         break;
    }

    CFSocketDisableCallBacks(socket->m_cfSocket, c);
}

void _GSocket_Enable(GSocket *socket, GSocketEvent event)
{
  socket->m_detected &= ~(1 << event);
  _GSocket_Install_Callback(socket, event);
}

void _GSocket_Disable(GSocket *socket, GSocketEvent event)
{
  socket->m_detected |= (1 << event);
  _GSocket_Uninstall_Callback(socket, event);
}

void wxMacCFSocketCallback(CFSocketRef s, CFSocketCallBackType callbackType,
                         CFDataRef address, const void* data, void* info)
{
  GSocket* socket = (GSocket*)info;

  switch (callbackType)
  {
    case kCFSocketConnectCallBack:
      if ( data )
      {
        SInt32 error = *((SInt32*)data) ;
        CALL_CALLBACK( socket , GSOCK_LOST ) ;
        GSocket_Shutdown(socket);
      }
      else
      {
        CALL_CALLBACK( socket , GSOCK_CONNECTION ) ;
      }
      break;

    case kCFSocketReadCallBack:
      CALL_CALLBACK( socket , GSOCK_INPUT ) ;
      break;

    case kCFSocketWriteCallBack:
      CALL_CALLBACK( socket , GSOCK_OUTPUT ) ;
      break;

    default:
      break;  // We shouldn't get here.
  }
}

int GSocket_Read(GSocket *socket, char *buffer, int size)
{
  int ret = 0 ;

  assert(socket != NULL);
  // if ( !CFReadStreamHasBytesAvailable() )
  ret = CFReadStreamRead( socket->m_readStream , (UInt8*) buffer , size ) ;

  return ret;
}

int GSocket_Write(GSocket *socket, const char *buffer, int size)
{
  int ret;

  assert(socket != NULL);
  ret = CFWriteStreamWrite( socket->m_writeStream , (UInt8*) buffer , size ) ;

  return ret;
}

GSocketEventFlags GSocket_Select(GSocket *socket, GSocketEventFlags flags)
{
  assert( socket != NULL );

  return flags & socket->m_detected;
}

// ==========================================================================
// wxSocketModule
// ==========================================================================

class wxSocketModule : public wxModule
{
public:
    virtual bool OnInit()
    {
        // wxSocketBase will call GSocket_Init() itself when/if needed
        return true;
    }

    virtual void OnExit()
    {
        if ( wxSocketBase::IsInitialized() )
            wxSocketBase::Shutdown();
    }

private:
    DECLARE_DYNAMIC_CLASS(wxSocketModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxSocketModule, wxModule)

#endif
  // wxUSE_SOCKETS
