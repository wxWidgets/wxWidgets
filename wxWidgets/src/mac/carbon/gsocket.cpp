/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket) for WX
 * Name:        src/mac/carbon/gsocket.cpp
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Authors:     Guilhem Lavaux,
 *              Guillermo Rodriguez Garcia <guille@iies.es> (maintainer)
 *              Stefan CSomor
 * Purpose:     GSocket main mac file
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

/*
 * PLEASE don't put C++ comments here - this is a C source file.
 */

#ifndef __GSOCKET_STANDALONE__
#include "wx/platform.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifdef __DARWIN__
    #include <CoreServices/CoreServices.h>
#else
    #include <MacHeaders.c>
    #define OTUNIXERRORS 1
    #include <OpenTransport.h>
    #include <OpenTransportProviders.h>
    #include <OpenTptInternet.h>
#endif
#if TARGET_CARBON && !defined(OTAssert)
    #define OTAssert( str , cond ) /* does not exists in Carbon */
#endif

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <utime.h>

/*
 * INADDR_BROADCAST is identical to INADDR_NONE which is not defined
 * on all unices. INADDR_BROADCAST should be fine to indicate an error.
 */
#ifndef INADDR_BROADCAST
#define INADDR_BROADCAST 0xFFFFFFFFUL
#endif
#ifndef INADDR_NONE
#define INADDR_NONE INADDR_BROADCAST
#endif
#ifndef INADDR_ANY
#define INADDR_ANY 0x0UL
#endif
#ifndef __GSOCKET_STANDALONE__

    #include "wx/mac/macnotfy.h"
    #include "wx/mac/gsockmac.h"
    #include "wx/gsocket.h"

#else

    #include "gsockmac.h"
    #include "gsocket.h"

#endif /* __GSOCKET_STANDALONE__ */

#ifndef ntohl
    #define  ntohl(x) (x)
    #define  ntohs(x) (x)
    #define  htonl(x) (x)
    #define  htons(x) (x)
#endif

void wxCYield() ;
#ifdef __WXDEBUG__
#define qDebug 1
#define qDebug2 1
extern pascal void OTDebugStr(const char* str);
#endif
#ifndef __DARWIN__
  #include <OTDebug.h>
#endif
InetSvcRef gInetSvcRef = 0 ;
int gOTInited = 0 ;
OTNotifyUPP gOTNotifierUPP = NULL ;

OSStatus DoNegotiateIPReuseAddrOption(EndpointRef ep, Boolean enableReuseIPMode);

/* Input: ep - endpointref on which to negotiate the option
               enableReuseIPMode - desired option setting - true/false
   Return: kOTNoError indicates that the option was successfully negotiated
           OSStatus is an error if < 0, otherwise, the status field is
           returned and is > 0.

   IMPORTANT NOTE: The endpoint is assumed to be in synchronous more, otherwise
                   this code will not function as desired
*/


OSStatus DoNegotiateIPReuseAddrOption(EndpointRef ep, Boolean enableReuseIPMode)

{
    UInt8        buf[kOTFourByteOptionSize]; // define buffer for fourByte Option size
    TOption*     opt;                        // option ptr to make items easier to access
    TOptMgmt     req;
    TOptMgmt     ret;
    OSStatus     err;

    if (!OTIsSynchronous(ep))
    {
        return (-1);
    }
    opt = (TOption*)buf; // set option ptr to buffer
    req.opt.buf = buf;
    req.opt.len = sizeof(buf);
    req.flags   = T_NEGOTIATE;    // negotiate for option

    ret.opt.buf = buf;
    ret.opt.maxlen = kOTFourByteOptionSize;

    opt->level  = INET_IP;        // dealing with an IP Level function
#ifdef __DARWIN__
    opt->name   = kIP_REUSEADDR;
#else
    opt->name   = IP_REUSEADDR;
#endif
    opt->len    = kOTFourByteOptionSize;
    opt->status = 0;
    *(UInt32*)opt->value = enableReuseIPMode;   // set the desired option level, true or false

    err = OTOptionManagement(ep, &req, &ret);

    // if no error then return the option status value
    if (err == kOTNoError)
    {
        if (opt->status != T_SUCCESS)
            err = opt->status;
        else
            err = kOTNoError;
    }

    return err;
}


pascal void OTInetEventHandler(void*s, OTEventCode event, OTResult, void *cookie) ;
pascal void OTInetEventHandler(void*s, OTEventCode event, OTResult result, void *cookie)
{
    int wakeUp = true ;
    GSocket* sock = (GSocket*) s ;

    if ( event == kOTSyncIdleEvent )
    {
        return ;
    }

    if ( s )
    {
        wxMacAddEvent( sock->m_mac_events , _GSocket_Internal_Proc , event , s , wakeUp ) ;
    }

    return;
}

static void SetDefaultEndpointModes(EndpointRef ep , void *data )
    // This routine sets the supplied endpoint into the default
    // mode used in this application.  The specifics are:
    // blocking, synchronous, and using synch idle events with
    // the standard YieldingNotifier.
{
    OSStatus junk = kOTNoError ;
    OTAssert ("SetDefaultEndpointModes:invalid ref", ep != kOTInvalidEndpointRef ) ;
    junk = OTSetAsynchronous(ep);
    OTAssert("SetDefaultEndpointModes: Could not set asynchronous", junk == noErr);
/*
    junk = OTSetBlocking(ep);
    OTAssert("SetDefaultEndpointModes: Could not set blocking", junk == noErr);
    junk = OTSetSynchronous(ep);
    OTAssert("SetDefaultEndpointModes: Could not set synchronous", junk == noErr);
    junk = OTSetBlocking(ep);
    OTAssert("SetDefaultEndpointModes: Could not set blocking", junk == noErr);
*/
    junk = OTInstallNotifier(ep, gOTNotifierUPP, data);
    OTAssert("SetDefaultEndpointModes: Could not install notifier", junk == noErr);
/*
    junk = OTUseSyncIdleEvents(ep, true);
    OTAssert("SetDefaultEndpointModes: Could not use sync idle events", junk == noErr);
*/
}

/* Global initialisers */

void GSocket_SetGUIFunctions(GSocketGUIFunctionsTable *table)
{
    // do nothing, wxMac doesn't have wxBase-GUI separation yet
}

int GSocket_Init()
{
    return 1;
}

bool GSocket_Verify_Inited() ;
bool GSocket_Verify_Inited()
{
    OSStatus err ;
#if TARGET_CARBON
    // Marc Newsam: added the clientcontext variable
    //              however, documentation is unclear how this works
    OTClientContextPtr clientcontext;

    if ( gInetSvcRef )
      return true ;

    InitOpenTransportInContext(kInitOTForApplicationMask, &clientcontext);
    gOTInited = 1 ;
    gInetSvcRef = OTOpenInternetServicesInContext(kDefaultInternetServicesPath,
                                                  NULL, &err, clientcontext);
#else
    if ( gInetSvcRef )
      return true ;

    InitOpenTransport() ;
    gOTInited = 1 ;
    gInetSvcRef = OTOpenInternetServices(kDefaultInternetServicesPath, NULL, &err);
#endif
    if ( gInetSvcRef == NULL ||  err != kOTNoError )
    {
        OTAssert("Could not open Inet Services", err == noErr);
        return false ;
    }
    gOTNotifierUPP = NewOTNotifyUPP( OTInetEventHandler ) ;
    return true ;
}

void GSocket_Cleanup()
{
    if ( gOTInited != 0 )
    {
        if ( gInetSvcRef != NULL )
            OTCloseProvider( gInetSvcRef );
    #if TARGET_CARBON
      CloseOpenTransportInContext( NULL ) ;
    #else
      CloseOpenTransport() ;
    #endif
        if ( gOTNotifierUPP )
            DisposeOTNotifyUPP( gOTNotifierUPP ) ;
    }
}

/* Constructors / Destructors for GSocket */

GSocket::GSocket()
{
  int i;

  m_ok = GSocket_Verify_Inited();

  m_endpoint                  = NULL ;
  for (i=0;i<GSOCK_MAX_EVENT;i++)
  {
    m_cbacks[i]         = NULL;
  }
  m_detected            = 0;
  m_local               = NULL;
  m_peer                = NULL;
  m_error               = GSOCK_NOERROR;
  m_server              = false;
  m_stream              = true;
  m_non_blocking        = false;
  m_timeout             = 1*1000;
                                /* 10 sec * 1000 millisec */
  m_takesEvents         = true ;
  m_mac_events          = wxMacGetNotifierTable() ;
}

GSocket::~GSocket()
{
    assert(this);

    /* Check that the socket is really shutdowned */
    if (m_endpoint != kOTInvalidEndpointRef)
        Shutdown();


    /* Destroy private addresses */
    if (m_local)
        GAddress_destroy(m_local);

    if (m_peer)
        GAddress_destroy(m_peer);
}

/* GSocket_Shutdown:
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void GSocket::Shutdown()
{
    OSStatus err ;
    int evt;

    assert(this);

    /* If socket has been created, shutdown it */
    if (m_endpoint != kOTInvalidEndpointRef )
    {
        err = OTSndOrderlyDisconnect( m_endpoint ) ;
        if ( err != kOTNoError )
        {
        }

        err = OTRcvOrderlyDisconnect( m_endpoint ) ;
        err = OTUnbind( m_endpoint ) ;
        err = OTCloseProvider( m_endpoint ) ;
        m_endpoint = kOTInvalidEndpointRef ;
    }

    /* Disable GUI callbacks */
    for (evt = 0; evt < GSOCK_MAX_EVENT; evt++)
        m_cbacks[evt] = NULL;

    m_detected = 0;
    Disable_Events();
    wxMacRemoveAllNotifiersForData( wxMacGetNotifierTable() , this ) ;
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
GSocketError GSocket::SetLocal(GAddress *address)
{
    assert(this);

    /* the socket must be initialized, or it must be a server */
    if ((m_endpoint != kOTInvalidEndpointRef && !m_server))
    {
        m_error = GSOCK_INVSOCK;
        return GSOCK_INVSOCK;
    }

    /* check address */
    if (address == NULL || address->m_family == GSOCK_NOFAMILY)
    {
        m_error = GSOCK_INVADDR;
        return GSOCK_INVADDR;
    }

    if (m_local)
        GAddress_destroy(m_local);

    m_local = GAddress_copy(address);

    return GSOCK_NOERROR;
}

GSocketError GSocket::SetPeer(GAddress *address)
{
    assert(this);

    /* check address */
    if (address == NULL || address->m_family == GSOCK_NOFAMILY)
    {
        m_error = GSOCK_INVADDR;
        return GSOCK_INVADDR;
    }

    if (m_peer)
        GAddress_destroy(m_peer);

    m_peer = GAddress_copy(address);

    return GSOCK_NOERROR;
}

GAddress *GSocket::GetLocal()
{
    GAddress *address = NULL ;
    GSocketError err;
    InetAddress loc ;

    assert(this);

    /* try to get it from the m_local var first */
    if (m_local)
        return GAddress_copy(m_local);

    /* else, if the socket is initialized, try getsockname */
    if (m_endpoint == kOTInvalidEndpointRef)
    {
        m_error = GSOCK_INVSOCK;
        return NULL;
    }


/* we do not support multihoming with this code at the moment
   OTGetProtAddress will have to be used then - but we don't have a handy
   method to use right now
*/
    {
        InetInterfaceInfo info;
        OTInetGetInterfaceInfo(&info, kDefaultInetInterface);
        loc.fHost = info.fAddress ;
        loc.fPort = 0 ;
        loc.fAddressType = AF_INET ;
    }

    /* got a valid address from getsockname, create a GAddress object */
    address = GAddress_new();
    if (address == NULL)
    {
        m_error = GSOCK_MEMERR;
        return NULL;
    }

    err = _GAddress_translate_from(address, &loc);
    if (err != GSOCK_NOERROR)
    {
        GAddress_destroy(address);
        m_error = err;
        return NULL;
    }

    return address;
}

GAddress *GSocket::GetPeer()
{
    assert(this);

    /* try to get it from the m_peer var */
    if (m_peer)
        return GAddress_copy(m_peer);

    return NULL;
}

/* Server specific parts */

/* GSocket_SetServer:
 *  Sets up this socket as a server. The local address must have been
 *  set with GSocket_SetLocal() before GSocket_SetServer() is called.
 *  Returns GSOCK_NOERROR on success, one of the following otherwise:
 *
 *  Error codes:
 *    GSOCK_INVSOCK - the socket is in use.
 *    GSOCK_INVADDR - the local address has not been set.
 *    GSOCK_IOERR   - low-level error.
 */
GSocketError GSocket::SetServer()
{
  assert(this);

  /* must not be in use */
  if (m_endpoint != kOTInvalidEndpointRef )
  {
    m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  /* the local addr must have been set */
  if (!m_local)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  m_stream   = true;
  m_server   = true;
  m_oriented = true;

// TODO
#if 0
  /* Create the socket */
  m_endpoint = socket(m_local->m_realfamily, SOCK_STREAM, 0);
  socket_set_ref( m_endpoint , (unsigned long) &gMacNetEvents ,  (unsigned long) this ) ;
  if (m_endpoint == kOTInvalidEndpointRef)
  {
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(m_endpoint, FIONBIO, &arg);
  Enable_Events();

  /* Bind to the local address,
   * retrieve the actual address bound,
   * and listen up to 5 connections.
   */
  if ((bind(m_endpoint, m_local->m_addr, m_local->m_len) != 0) ||
      (getsockname(m_endpoint,
                   m_local->m_addr,
                   (WX_SOCKLEN_T *) &m_local->m_len) != 0) ||
      (listen(m_endpoint, 5) != 0))
  {
    close(m_endpoint);
    m_endpoint = -1;
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }
#endif
  return GSOCK_NOERROR;
}

/* GSocket_WaitConnection:
 *  Waits for an incoming client connection. Returns a pointer to
 *  a GSocket object, or NULL if there was an error, in which case
 *  the last error field will be updated for the calling GSocket.
 *
 *  Error codes (set in the calling GSocket)
 *    GSOCK_INVSOCK    - the socket is not valid or not a server.
 *    GSOCK_TIMEDOUT   - timeout, no incoming connections.
 *    GSOCK_WOULDBLOCK - the call would block and the socket is nonblocking.
 *    GSOCK_MEMERR     - couldn't allocate memory.
 *    GSOCK_IOERR      - low-level error.
 */
GSocket *GSocket::WaitConnection()
{
  GSocket *connection = NULL ;

  assert(this);

  /* Reenable CONNECTION events */
  m_detected &= ~GSOCK_CONNECTION_FLAG;

  /* If the socket has already been created, we exit immediately */
  if (m_endpoint == kOTInvalidEndpointRef || !m_server)
  {
    m_error = GSOCK_INVSOCK;
    return NULL;
  }

  /* Create a GSocket object for the new connection */
  connection = GSocket_new();

  if (!connection)
  {
    m_error = GSOCK_MEMERR;
    return NULL;
  }

  /* Wait for a connection (with timeout) */
  if (Input_Timeout() == GSOCK_TIMEDOUT)
  {
    delete connection;
    /* m_error set by _GSocket_Input_Timeout */
    return NULL;
  }

// TODO
#if 0
  connection->m_endpoint = accept(m_endpoint, &from, (WX_SOCKLEN_T *) &fromlen);
#endif

  if (connection->m_endpoint == kOTInvalidEndpointRef )
  {
    if (errno == EWOULDBLOCK)
      m_error = GSOCK_WOULDBLOCK;
    else
      m_error = GSOCK_IOERR;

    delete connection;
    return NULL;
  }

  /* Initialize all fields */
  connection->m_server   = false;
  connection->m_stream   = true;
  connection->m_oriented = true;

  /* Setup the peer address field */
  connection->m_peer = GAddress_new();
  if (!connection->m_peer)
  {
    delete connection;
    m_error = GSOCK_MEMERR;
    return NULL;
  }
 // TODO
 #if 0
  err = _GAddress_translate_from(connection->m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(connection->m_peer);
    GSocket_destroy(connection);
    m_error = err;
    return NULL;
  }

  ioctl(connection->m_endpoint, FIONBIO, &arg);
#endif
  connection->Enable_Events();

  return connection;
}

/* Datagram sockets */

/* GSocket_SetNonOriented:
 *  Sets up this socket as a non-connection oriented (datagram) socket.
 *  Before using this function, the local address must have been set
 *  with GSocket_SetLocal(), or the call will fail. Returns GSOCK_NOERROR
 *  on success, or one of the following otherwise.
 *
 *  Error codes:
 *    GSOCK_INVSOCK - the socket is in use.
 *    GSOCK_INVADDR - the local address has not been set.
 *    GSOCK_IOERR   - low-level error.
 */
GSocketError GSocket::SetNonOriented()
{
  assert(this);

  if (m_endpoint != kOTInvalidEndpointRef )
  {
    m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!m_local)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  m_stream   = false;
  m_server   = false;
  m_oriented = false;

  /* Create the socket */

// TODO
#if 0
  m_endpoint = socket(m_local->m_realfamily, SOCK_DGRAM, 0);
  socket_set_ref( m_endpoint , (unsigned long) &gMacNetEvents ,  (unsigned long) this ) ;
#endif
  if (m_endpoint == kOTInvalidEndpointRef )
  {
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

// TODO
#if 0
  ioctl(m_endpoint, FIONBIO, &arg);
#endif
  Enable_Events();

  /* Bind to the local address,
   * and retrieve the actual address bound.
   */
// TODO
#if 0
  if ((bind(m_endpoint, m_local->m_addr, m_local->m_len) != 0) ||
      (getsockname(m_endpoint,
                   m_local->m_addr,
                   (WX_SOCKLEN_T *) &m_local->m_len) != 0))
  {
    close(m_endpoint);
    m_endpoint    = -1;
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }
#endif
  return GSOCK_NOERROR;
}

/* Client specific parts */

/* GSocket_Connect:
 *  For stream (connection oriented) sockets, GSocket_Connect() tries
 *  to establish a client connection to a server using the peer address
 *  as established with GSocket_SetPeer(). Returns GSOCK_NOERROR if the
 *  connection has been successfully established, or one of the error
 *  codes listed below. Note that for nonblocking sockets, a return
 *  value of GSOCK_WOULDBLOCK doesn't mean a failure. The connection
 *  request can be completed later; you should use GSocket_Select()
 *  to poll for GSOCK_CONNECTION | GSOCK_LOST, or wait for the
 *  corresponding asynchronous events.
 *
 *  For datagram (non connection oriented) sockets, GSocket_Connect()
 *  just sets the peer address established with GSocket_SetPeer() as
 *  default destination.
 *
 *  Error codes:
 *    GSOCK_INVSOCK    - the socket is in use or not valid.
 *    GSOCK_INVADDR    - the peer address has not been established.
 *    GSOCK_TIMEDOUT   - timeout, the connection failed.
 *    GSOCK_WOULDBLOCK - connection in progress (nonblocking sockets only)
 *    GSOCK_MEMERR     - couldn't allocate memory.
 *    GSOCK_IOERR      - low-level error.
 */
GSocketError GSocket::Connect(GSocketStream stream)
{
  InetAddress addr ;
  TEndpointInfo   info;
   OSStatus       err = kOTNoError;
  TCall peer ;

  assert(this);

  /* Enable CONNECTION events (needed for nonblocking connections) */
  m_detected &= ~GSOCK_CONNECTION_FLAG;

  if (m_endpoint != kOTInvalidEndpointRef )
  {
    m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!m_peer)
  {
    m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Streamed or dgram socket? */
  m_stream   = (stream == GSOCK_STREAMED);
  m_oriented = true;
  m_server   = false;

  /* Create the socket */
#if TARGET_CARBON
  m_endpoint =
      OTOpenEndpointInContext( OTCreateConfiguration( kTCPName) , 0 , &info , &err , NULL ) ;
#else
  m_endpoint =
      OTOpenEndpoint( OTCreateConfiguration( kTCPName) , 0 , &info , &err ) ;
#endif
  if ( m_endpoint == kOTInvalidEndpointRef || err != kOTNoError )
  {
    m_endpoint = kOTInvalidEndpointRef ;
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }
  err = OTBind( m_endpoint , nil , nil ) ;
  if ( err != kOTNoError )
  {
    return GSOCK_IOERR;
  }
  SetDefaultEndpointModes( m_endpoint , this ) ;
// TODO
#if 0
  ioctl(m_endpoint, FIONBIO, &arg);
#endif
  Enable_Events();

  _GAddress_translate_to( m_peer , &addr ) ;
  memset( &peer , 0 , sizeof( TCall ) ) ;
  peer.addr.len = sizeof( InetAddress ) ;
  peer.addr.buf = (unsigned char*) &addr ;
  err = OTConnect( m_endpoint , &peer , nil ) ;
  if ( err != noErr )
  {
    /* If connect failed with EINPROGRESS and the GSocket object
     * is in blocking mode, we select() for the specified timeout
     * checking for writability to see if the connection request
     * completes.
     */

    if ((err == kOTNoDataErr ) && (!m_non_blocking))
    {
      if (Output_Timeout() == GSOCK_TIMEDOUT)
      {
        OTSndOrderlyDisconnect( m_endpoint ) ;
        m_endpoint = kOTInvalidEndpointRef ;
        /* m_error is set in _GSocket_Output_Timeout */
        return GSOCK_TIMEDOUT;
      }
      else
      {
/*
        int error;
        WX_SOCKLEN_T len = sizeof(error);

        getsockopt(m_endpoint, SOL_SOCKET, SO_ERROR, (void*) &error, &len);

        if (!error)
*/
          return GSOCK_NOERROR;
      }
    }

    /* If connect failed with EINPROGRESS and the GSocket object
     * is set to nonblocking, we set m_error to GSOCK_WOULDBLOCK
     * (and return GSOCK_WOULDBLOCK) but we don't close the socket;
     * this way if the connection completes, a GSOCK_CONNECTION
     * event will be generated, if enabled.
     */
    if ((err == kOTNoDataErr) && (m_non_blocking))
    {
      m_error = GSOCK_WOULDBLOCK;
      return GSOCK_WOULDBLOCK;
    }

    /* If connect failed with an error other than EINPROGRESS,
     * then the call to GSocket_Connect has failed.
     */
    OTSndOrderlyDisconnect( m_endpoint ) ;

    m_endpoint = kOTInvalidEndpointRef ;
    m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }
//  OTInetEventHandler(this, T_CONNECT , kOTNoError , NULL ) ;
  return GSOCK_NOERROR;
}

/* Generic IO */

/* Like recv(), send(), ... */
int GSocket::Read(char *buffer, int size)
{
  int ret = 0 ;

  assert(this);

  /* Reenable INPUT events */
  m_detected &= ~GSOCK_INPUT_FLAG;

  if (m_endpoint == kOTInvalidEndpointRef || m_server)
  {
    m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for data (with a timeout) */
  if (Input_Timeout() == GSOCK_TIMEDOUT)
    return -1;

  /* Read the data */
  if (m_stream)
    ret = Recv_Stream(buffer, size);
  else
    ret = Recv_Dgram(buffer, size);

  if (ret == -1)
  {
    if (errno == EWOULDBLOCK)
      m_error = GSOCK_WOULDBLOCK;
    else
      m_error = GSOCK_IOERR;
  }

  return ret;
}

int GSocket::Write(const char *buffer, int size)
{
  int ret;

  assert(this);

  if (m_endpoint == kOTInvalidEndpointRef || m_server)
  {
    m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for writability (with a timeout) */
  if (Output_Timeout() == GSOCK_TIMEDOUT)
    return -1;

  /* Write the data */
  if (m_stream)
    ret = Send_Stream(buffer, size);
  else
    ret = Send_Dgram(buffer, size);

  if (ret == -1)
  {
    if (errno == EWOULDBLOCK)
      m_error = GSOCK_WOULDBLOCK;
    else
      m_error = GSOCK_IOERR;

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * in MSW). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    m_detected &= ~GSOCK_OUTPUT_FLAG;
    return -1;
  }

  return ret;
}

/* GSocket_Select:
 *  Polls the socket to determine its status. This function will
 *  check for the events specified in the 'flags' parameter, and
 *  it will return a mask indicating which operations can be
 *  performed. This function won't block, regardless of the
 *  mode (blocking | nonblocking) of the socket.
 */
GSocketEventFlags GSocket::Select(GSocketEventFlags flags)
{
  assert(this);
  wxMacProcessNotifierEvents() ;
  /*
  state = OTGetEndpointState(m_endpoint);

  if ( ( flags & GSOCK_INPUT_FLAG ) && ! ( m_detected & GSOCK_INPUT_FLAG ) )
  {
    size_t sz = 0 ;
    OTCountDataBytes( m_endpoint , &sz ) ;
    if ( state == T_INCON || sz > 0 )
    {
      m_detected |= GSOCK_INPUT_FLAG ;
      (m_cbacks[GSOCK_INPUT])(this, GSOCK_INPUT, m_data[GSOCK_INPUT]);
    }
  }
  if ( ( flags & GSOCK_INPUT_FLAG ) && ! ( m_detected & GSOCK_OUTPUT_FLAG ) )
  {
    if ( state == T_DATAXFER || state == T_INREL )
    {
      m_detected |=GSOCK_OUTPUT_FLAG ;
      (m_cbacks[GSOCK_OUTPUT])(this, GSOCK_OUTPUT, m_data[GSOCK_OUTPUT]);
    }
  }
  */
  return ( flags & m_detected ) ;
}

/* Flags */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. All IO calls will return
 *  immediately.
 */
void GSocket::SetNonBlocking(bool non_block)
{
  assert(this);

  m_non_blocking = non_block;
}

/* GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void GSocket::SetTimeout(unsigned long millisec)
{
  assert(this);

//  this is usually set too high and we have not yet been able to detect a closed
//  stream, thus we leave the 10 sec timeout
//  m_timeout = millisec;
}

/* GSocket_GetError:
 *  Returns the last error which occurred for this socket. Note that successful
 *  operations do not clear this back to GSOCK_NOERROR, so use it only
 *  after an error.
 */
GSocketError WXDLLIMPEXP_NET GSocket::GetError()
{
  assert(this);

  return m_error;
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
void GSocket::SetCallback(GSocketEventFlags flags,
                         GSocketCallback callback, char *cdata)
{
  int count;

  assert(this);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      m_cbacks[count] = callback;
      m_data[count] = cdata;
    }
  }
}

/* GSocket_UnsetCallback:
 *  Disables all callbacks specified by 'flags', which may be a
 *  combination of flags OR'ed toghether.
 */
void GSocket::UnsetCallback(GSocketEventFlags flags)
{
  int count;

  assert(this);

  for (count = 0; count < GSOCK_MAX_EVENT; count++)
  {
    if ((flags & (1 << count)) != 0)
    {
      m_cbacks[count] = NULL;
      m_data[count] = NULL;
    }
  }
}


#define CALL_CALLBACK(socket, event) {                                  \
  _GSocket_Disable(socket, event);                                      \
  if (socket->m_cbacks[event])                                          \
    socket->m_cbacks[event](socket, event, socket->m_data[event]);      \
}

int GSocket::Recv_Stream(char *buffer, int size)
{
    OTFlags flags ;
    OTResult res ;
    OTByteCount sz = 0 ;

    OTCountDataBytes( m_endpoint , &sz ) ;
    if ( size > (int)sz )
        size = sz ;
    res = OTRcv( m_endpoint , buffer , size , &flags ) ;
    if ( res < 0 )
    {
        return -1 ;
    }

    // we simulate another read event if there are still bytes
    if ( m_takesEvents )
    {
        OTByteCount sz = 0 ;
        OTCountDataBytes( m_endpoint , &sz ) ;
        if ( sz > 0 )
        {
            m_detected |= GSOCK_INPUT_FLAG ;
            (m_cbacks[GSOCK_INPUT])(this, GSOCK_INPUT, m_data[GSOCK_INPUT]);
        }
    }
    return res ;
}

int GSocket::Recv_Dgram(char *buffer, int size)
{
// TODO
  int ret = -1;
#if 0
  struct sockaddr from;
  WX_SOCKLEN_T fromlen = sizeof(from);
  GSocketError err;

  fromlen = sizeof(from);

  ret = recvfrom(m_endpoint, buffer, size, 0, &from, (WX_SOCKLEN_T *) &fromlen);

  if (ret == -1)
    return -1;

  /* Translate a system address into a GSocket address */
  if (!m_peer)
  {
    m_peer = GAddress_new();
    if (!m_peer)
    {
      m_error = GSOCK_MEMERR;
      return -1;
    }
  }
  err = _GAddress_translate_from(m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(m_peer);
    m_peer  = NULL;
    m_error = err;
    return -1;
  }
#endif
  return ret;
}

int GSocket::Send_Stream(const char *buffer, int size)
{
    OTFlags flags = 0 ;
    OTResult res ;

    res = OTSnd( m_endpoint , (void*) buffer , size , flags ) ;
    return res ;
}

int GSocket::Send_Dgram(const char *buffer, int size)
{
  int ret = -1 ;
// TODO
#if 0
  struct sockaddr *addr;
  int len ;
  GSocketError err;

  if (!m_peer)
  {
    m_error = GSOCK_INVADDR;
    return -1;
  }

  err = _GAddress_translate_to(m_peer, &addr, &len);
  if (err != GSOCK_NOERROR)
  {
    m_error = err;
    return -1;
  }

  ret = sendto(m_endpoint, buffer, size, 0, addr, len);

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);
#endif
  return ret;
}

/* Compatibility functions for GSocket */
GSocket *GSocket_new(void)
{
    GSocket *newsocket = new GSocket();
    if(newsocket->IsOk())
        return newsocket;
    delete newsocket;
    return NULL;
}


/*
 * -------------------------------------------------------------------------
 * GAddress
 * -------------------------------------------------------------------------
 */

/* CHECK_ADDRESS verifies that the current family is either GSOCK_NOFAMILY
 * or GSOCK_*family*, and if it is GSOCK_NOFAMILY, it initalizes address
 * to be a GSOCK_*family*. In other cases, it returns GSOCK_INVADDR.
 */
#define CHECK_ADDRESS(address, family, retval)                      \
{                                                                   \
  if (address->m_family == GSOCK_NOFAMILY)                          \
    if (_GAddress_Init_##family(address) != GSOCK_NOERROR)          \
      return address->m_error;                                      \
  if (address->m_family != GSOCK_##family)                          \
  {                                                                 \
    address->m_error = GSOCK_INVADDR;                               \
    return retval;                                                  \
  }                                                                 \
}

GAddress *GAddress_new()
{
  GAddress *address;

  if ((address = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  address->m_family  = GSOCK_NOFAMILY;
  address->m_host = INADDR_NONE ;
  address->m_port = 0 ;

  return address;
}

GAddress *GAddress_copy(GAddress *address)
{
  GAddress *addr2;

  assert(address != NULL);

  if ((addr2 = (GAddress *) malloc(sizeof(GAddress))) == NULL)
    return NULL;

  memcpy(addr2, address, sizeof(GAddress));
  return addr2;
}

void GAddress_destroy(GAddress *address)
{
  assert(address != NULL);

  free(address);
}

void GAddress_SetFamily(GAddress *address, GAddressType type)
{
  assert(address != NULL);

  address->m_family = type;
}

GAddressType GAddress_GetFamily(GAddress *address)
{
  assert(address != NULL);

  return address->m_family;
}

GSocketError _GAddress_translate_from(GAddress *address,
                                      InetAddress *addr)
{
  switch (addr->fAddressType)
  {
    case AF_INET:
      address->m_family = GSOCK_INET;
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
  address->m_host = addr->fHost ;
  address->m_port = addr->fPort ;
  return GSOCK_NOERROR;
}

GSocketError _GAddress_translate_to(GAddress *address,
                                    InetAddress *addr)
{
  if ( !GSocket_Verify_Inited() )
    return GSOCK_IOERR ;
  memset(addr, 0 , sizeof(struct InetAddress));
  OTInitInetAddress( addr , address->m_port , address->m_host ) ;
  return GSOCK_NOERROR;
}

/*
 * -------------------------------------------------------------------------
 * Internet address family
 * -------------------------------------------------------------------------
 */

GSocketError _GAddress_Init_INET(GAddress *address)
{
  address->m_family = GSOCK_INET;
  address->m_host = kOTAnyInetAddress ;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetHostName(GAddress *address, const char *hostname)
{
  InetHostInfo hinfo ;
  OSStatus ret ;

 if ( !GSocket_Verify_Inited() )
    return GSOCK_IOERR ;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);
  ret = OTInetStringToAddress( gInetSvcRef , (char*) hostname , &hinfo ) ;
  if ( ret != kOTNoError )
  {
    address->m_host = INADDR_NONE ;
    address->m_error = GSOCK_NOHOST;
    return GSOCK_NOHOST;
  }
  address->m_host = hinfo.addrs[0] ;
  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_SetAnyAddress(GAddress *address)
{
  return GAddress_INET_SetHostAddress(address, INADDR_ANY);
}

GSocketError GAddress_INET_SetHostAddress(GAddress *address,
                                          unsigned long hostaddr)
{
  assert(address != NULL);

  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  address->m_host = htonl(hostaddr) ;

  return GSOCK_NOERROR;
}

struct service_entry
{
    const char * name ;
    unsigned short port ;
    const char * protocol ;
} ;
typedef struct service_entry service_entry ;

service_entry gServices[] =
{
    { "http" , 80 , "tcp" }
} ;

GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  size_t i ;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  if (!port)
  {
    address->m_error = GSOCK_INVPORT;
    return GSOCK_INVPORT;
  }
  for ( i = 0 ; i < sizeof( gServices) / sizeof( service_entry ) ; ++i )
  {
    if ( strcmp( port , gServices[i].name ) == 0 )
    {
      if ( protocol == NULL || strcmp( protocol , gServices[i].protocol ) )
      {
        address->m_port = gServices[i].port ;
        return GSOCK_NOERROR;
      }
    }
  }

  if (isdigit(port[0]))
  {
     address->m_port = atoi(port);
     return GSOCK_NOERROR;
  }

  address->m_error = GSOCK_INVPORT;
  return GSOCK_INVPORT;
}

GSocketError GAddress_INET_SetPort(GAddress *address, unsigned short port)
{
  assert(address != NULL);
  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);
  address->m_port = port ;

  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_GetHostName(GAddress *address, char *hostname, size_t sbuf)
{
  InetDomainName name ;
  if ( !GSocket_Verify_Inited() )
    return GSOCK_IOERR ;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  OTInetAddressToName( gInetSvcRef , address->m_host , name ) ;
  strncpy( hostname , name , sbuf ) ;
  return GSOCK_NOERROR;
}

unsigned long GAddress_INET_GetHostAddress(GAddress *address)
{
  assert(address != NULL);
  CHECK_ADDRESS(address, INET, 0);

  return ntohl(address->m_host);
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  assert(address != NULL);
  CHECK_ADDRESS(address, INET, 0);

  return address->m_port;
}

void GSocket::Enable_Events()
{
    if ( m_takesEvents )
        return ;

    {
        OTResult  state ;
        m_takesEvents = true ;
        state = OTGetEndpointState(m_endpoint);

        {
            OTByteCount sz = 0 ;
            OTCountDataBytes( m_endpoint , &sz ) ;
            if ( state == T_INCON || sz > 0 )
            {
                m_detected |= GSOCK_INPUT_FLAG ;
                (m_cbacks[GSOCK_INPUT])(this, GSOCK_INPUT, m_data[GSOCK_INPUT]);
            }
        }
        {
            if ( state == T_DATAXFER || state == T_INREL )
            {
                m_detected |=GSOCK_OUTPUT_FLAG ;
                (m_cbacks[GSOCK_OUTPUT])(this, GSOCK_OUTPUT, m_data[GSOCK_OUTPUT]);
            }
        }
    }
}

void GSocket::Disable_Events()
{
    m_takesEvents = false ;
}

/* _GSocket_Input_Timeout:
 *  For blocking sockets, wait until data is available or
 *  until timeout ellapses.
 */
GSocketError GSocket::Input_Timeout()
{
  if ( !m_non_blocking )
  {
    UnsignedWide now , start ;
    bool formerTakesEvents = m_takesEvents ;
    Microseconds(&start);
    now = start ;
    m_takesEvents = false ;

    while( (now.hi * 4294967296.0 + now.lo) - (start.hi * 4294967296.0 + start.lo) < m_timeout * 1000.0 )
    {
        OTResult state ;
        OTByteCount sz = 0 ;
        state = OTGetEndpointState(m_endpoint);

        OTCountDataBytes( m_endpoint , &sz ) ;
        if ( state == T_INCON || sz > 0 )
        {
            m_takesEvents = formerTakesEvents ;
            return GSOCK_NOERROR;
        }
        Microseconds(&now);
    }
    m_takesEvents = formerTakesEvents ;
    m_error = GSOCK_TIMEDOUT;
    return GSOCK_TIMEDOUT;
  }
  return GSOCK_NOERROR;
}

/* _GSocket_Output_Timeout:
 *  For blocking sockets, wait until data can be sent without
 *  blocking or until timeout ellapses.
 */
GSocketError GSocket::Output_Timeout()
{
  if ( !m_non_blocking )
  {
    UnsignedWide now , start ;
    bool formerTakesEvents = m_takesEvents ;
    Microseconds(&start);
    now = start ;
    m_takesEvents = false ;

    while( (now.hi * 4294967296.0 + now.lo) - (start.hi * 4294967296.0 + start.lo) < m_timeout * 1000.0 )
    {
        OTResult state ;
        state = OTGetEndpointState(m_endpoint);

        if ( state == T_DATAXFER || state == T_INREL )
        {
            m_takesEvents = formerTakesEvents ;
            return GSOCK_NOERROR;
        }
        Microseconds(&now);
    }
    m_takesEvents = formerTakesEvents ;
    m_error = GSOCK_TIMEDOUT;
    return GSOCK_TIMEDOUT;
  }
  return GSOCK_NOERROR;
}

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

void _GSocket_Internal_Proc(unsigned long e , void* d )
{
    GSocket *socket = (GSocket*) d ;

    if ( !socket )
        return ;

    OTEventCode ev = (OTEventCode) e ;
    GSocketEvent event;
    GSocketEvent event2;
    GSocketCallback cback;
    char *data;
    GSocketCallback cback2;
    char *data2;

    event = GSOCK_MAX_EVENT ;
    event2 = GSOCK_MAX_EVENT ;
    cback = NULL;
    data = NULL;
    cback2 = NULL;
    data2 = NULL;

    /* Check that the socket still exists (it has not been
     * destroyed) and for safety, check that the m_endpoint field
     * is what we expect it to be.
     */
    if ( /* (socket != NULL) && */ (socket->m_takesEvents))
    {
        switch (ev)
        {
            case T_LISTEN :
                event = GSOCK_CONNECTION ;
                break ;
            case T_CONNECT :
                event = GSOCK_CONNECTION ;
                event2 = GSOCK_OUTPUT ;
                {
                    TCall retCall;

                    retCall.addr.buf     = NULL;
                    retCall.addr.maxlen  = 0;
                    retCall.opt.buf      = NULL;
                    retCall.opt.maxlen   = 0;
                    retCall.udata.buf    = NULL;
                    retCall.udata.maxlen = 0;
                    OTRcvConnect( socket->m_endpoint , &retCall ) ;
                }
                break ;
            case T_DISCONNECT :
                event = GSOCK_LOST ;
                break ;
            case T_GODATA :
            case T_GOEXDATA :
                event = GSOCK_OUTPUT ;
                break ;
            case T_DATA :
                event = GSOCK_INPUT ;
                break ;
            case T_EXDATA :
                event = GSOCK_INPUT ;
                break ;
      }
      if (event != GSOCK_MAX_EVENT)
      {
        cback = socket->m_cbacks[event];
        data = socket->m_data[event];

        if (event == GSOCK_LOST)
          socket->m_detected = GSOCK_LOST_FLAG;
        else
          socket->m_detected |= (1 << event);
      }
      if (event2 != GSOCK_MAX_EVENT)
      {
        cback2 = socket->m_cbacks[event2];
        data2 = socket->m_data[event2];

        if (event2 == GSOCK_LOST)
          socket->m_detected = GSOCK_LOST_FLAG;
        else
          socket->m_detected |= (1 << event2);
      }
    }

    /* OK, we can now leave the critical section because we have
     * already obtained the callback address (we make no further
     * accesses to socket->whatever). However, the app should
     * be prepared to handle events from a socket that has just
     * been closed!
     */

    if (cback != NULL)
      (cback)(socket, event, data);
    if (cback2 != NULL)
      (cback2)(socket, event2, data2);

}

/* Hack added for Mac OS X */
GSocketError GAddress_UNIX_GetPath(GAddress *addr, char *path, size_t buf)
{
    return GSOCK_INVADDR;
}

GSocketError GAddress_UNIX_SetPath(GAddress *addr, const char *path)
{
    return GSOCK_INVADDR;
}

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */
