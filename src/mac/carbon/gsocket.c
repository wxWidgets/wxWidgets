/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsocket.c
 * Authors: Guilhem Lavaux,
 *          Guillermo Rodriguez Garcia <guille@iies.es> (maintainer)
 *          Stefan CSomor
 * Purpose: GSocket main mac file
 * CVSID:   $Id$
 * -------------------------------------------------------------------------
 */

/*
 * PLEASE don't put C++ comments here - this is a C source file.
 */

#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifdef __UNIX__
  #include <CoreServices/CoreServices.h>

  #ifndef FALSE
    #define FALSE 0
  #endif
  #ifndef TRUE
    #define TRUE 1
  #endif
#else
  #define OTUNIXERRORS 1
  #include <OpenTransport.h>
  #include <OpenTransportProviders.h>
  #include <OpenTptInternet.h>
#endif
#if TARGET_CARBON
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

void wxCYield() ;
#ifdef __WXDEBUG__
#define qDebug 1
#define qDebug2 1
extern pascal void OTDebugStr(const char* str);
#endif
#ifndef __UNIX__
  #include <OTDebug.h>
#endif
InetSvcRef gInetSvcRef = 0 ;


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
	UInt8		buf[kOTFourByteOptionSize];	// define buffer for fourByte Option size
	TOption*	opt;						// option ptr to make items easier to access
	TOptMgmt	req;
	TOptMgmt	ret;
	OSStatus	err;
	
	if (!OTIsSynchronous(ep))
	{
		return (-1);
	}
	opt = (TOption*)buf;					// set option ptr to buffer
	req.opt.buf	= buf;
	req.opt.len	= sizeof(buf);
	req.flags	= T_NEGOTIATE;				// negotiate for option

	ret.opt.buf = buf;
	ret.opt.maxlen = kOTFourByteOptionSize;

	opt->level	= INET_IP;					// dealing with an IP Level function
#ifdef __UNIX__
	opt->name	= kIP_REUSEADDR;
#else
	opt->name	= IP_REUSEADDR;
#endif
	opt->len	= kOTFourByteOptionSize;
	opt->status = 0;
	*(UInt32*)opt->value = enableReuseIPMode;		// set the desired option level, true or false

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
		YieldToAnyThread() ;
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
	junk = OTInstallNotifier(ep, OTInetEventHandler, data);
	OTAssert("SetDefaultEndpointModes: Could not install notifier", junk == noErr);
/*
	junk = OTUseSyncIdleEvents(ep, true);
	OTAssert("SetDefaultEndpointModes: Could not use sync idle events", junk == noErr);
*/
}

/* Global initialisers */

int GSocket_Init()
{
	OSStatus err ;
#if TARGET_CARBON
	InitOpenTransportInContext( kInitOTForApplicationMask , NULL ) ;
#else	
	InitOpenTransport() ;
#endif
	gInetSvcRef = OTOpenInternetServices(kDefaultInternetServicesPath, NULL, &err);
	if ( gInetSvcRef == NULL ||  err != kOTNoError )
	{
		OTAssert("Could not open Inet Services", err == noErr);
		return FALSE ;
	}
  return TRUE;
}

void GSocket_Cleanup()
{
  if ( gInetSvcRef != NULL )
	OTCloseProvider( gInetSvcRef );
#if TARGET_CARBON
  CloseOpenTransportInContext( NULL ) ;
#else
  CloseOpenTransport() ;
#endif
}

/* Constructors / Destructors for GSocket */

GSocket *GSocket_new()
{
  int i;
  GSocket *socket;

  socket = (GSocket *)malloc(sizeof(GSocket));

  if (socket == NULL)
    return NULL;

  socket->m_endpoint                  = NULL ;
  for (i=0;i<GSOCK_MAX_EVENT;i++)
  {
    socket->m_cbacks[i]         = NULL;
  }
  socket->m_detected            = 0;
  socket->m_local               = NULL;
  socket->m_peer                = NULL;
  socket->m_error               = GSOCK_NOERROR;
  socket->m_server              = FALSE;
  socket->m_stream              = TRUE;
  socket->m_non_blocking        = FALSE;
  socket->m_timeout             = 10*60*1000;
                                /* 10 minutes * 60 sec * 1000 millisec */
  socket->m_takesEvents			= TRUE ;
  socket->m_mac_events			= wxMacGetNotifierTable() ;
  return socket;
}

void GSocket_destroy(GSocket *socket)
{
  assert(socket != NULL);

  /* Check that the socket is really shutdowned */
  if (socket->m_endpoint != kOTInvalidEndpointRef)
    GSocket_Shutdown(socket);


  /* Destroy private addresses */
  if (socket->m_local)
    GAddress_destroy(socket->m_local);

  if (socket->m_peer)
    GAddress_destroy(socket->m_peer);

  /* Destroy the socket itself */
  free(socket);
}

/* GSocket_Shutdown:
 *  Disallow further read/write operations on this socket, close
 *  the fd and disable all callbacks.
 */
void GSocket_Shutdown(GSocket *socket)
{
  OSStatus err ;
  int evt;

  assert(socket != NULL);

  /* If socket has been created, shutdown it */
  if (socket->m_endpoint != kOTInvalidEndpointRef )
  {
    err = OTSndOrderlyDisconnect( socket->m_endpoint ) ;
  	if ( err != kOTNoError )
  	{
  		
  	}
    err = OTRcvOrderlyDisconnect( socket->m_endpoint ) ;
  	err = OTUnbind( socket->m_endpoint ) ;
  	err = OTCloseProvider( socket->m_endpoint ) ;
  	socket->m_endpoint = kOTInvalidEndpointRef ;
  }

  /* Disable GUI callbacks */
  for (evt = 0; evt < GSOCK_MAX_EVENT; evt++)
    socket->m_cbacks[evt] = NULL;

  socket->m_detected = 0;
  _GSocket_Disable_Events(socket);
  wxMacRemoveAllNotifiersForData( wxMacGetNotifierTable() , socket ) ;
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
  assert(socket != NULL);

  /* the socket must be initialized, or it must be a server */
  if ((socket->m_endpoint != kOTInvalidEndpointRef && !socket->m_server))
  {
    socket->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  /* check address */
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

  /* check address */
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
  GAddress *address = NULL ;
  GSocketError err;
  InetAddress loc ;

  assert(socket != NULL);

  /* try to get it from the m_local var first */
  if (socket->m_local)
    return GAddress_copy(socket->m_local);

  /* else, if the socket is initialized, try getsockname */
  if (socket->m_endpoint == kOTInvalidEndpointRef)
  {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

	
/* we do not support multihoming with this code at the moment
   OTGetProtAddress will have to be used then - but we don't have a handy 
   method to use right now
*/
  {
    InetInterfaceInfo	info;
    OTInetGetInterfaceInfo(&info, kDefaultInetInterface);
    loc.fHost = info.fAddress ;
    loc.fPort = 0 ;
   	loc.fAddressType = AF_INET ;
  }

  /* got a valid address from getsockname, create a GAddress object */
  address = GAddress_new();
  if (address == NULL)
  {
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }

  err = _GAddress_translate_from(address, &loc);
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

  /* try to get it from the m_peer var */
  if (socket->m_peer)
    return GAddress_copy(socket->m_peer);

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
GSocketError GSocket_SetServer(GSocket *sck)
{
  int type;
  int arg = 1;

  assert(sck != NULL);

  /* must not be in use */
  if (sck->m_endpoint != kOTInvalidEndpointRef )
  {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  /* the local addr must have been set */
  if (!sck->m_local)
  {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  sck->m_stream   = TRUE;
  sck->m_server   = TRUE;
  sck->m_oriented = TRUE;

// TODO
#if 0
  /* Create the socket */
  sck->m_endpoint = socket(sck->m_local->m_realfamily, SOCK_STREAM, 0);
  socket_set_ref( sck->m_endpoint , (unsigned long) &gMacNetEvents ,  (unsigned long) sck ) ;
  if (sck->m_endpoint == kOTInvalidEndpointRef)
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

  ioctl(sck->m_endpoint, FIONBIO, &arg);
  _GSocket_Enable_Events(sck);

  /* Bind to the local address,
   * retrieve the actual address bound,
   * and listen up to 5 connections.
   */
  if ((bind(sck->m_endpoint, sck->m_local->m_addr, sck->m_local->m_len) != 0) ||
      (getsockname(sck->m_endpoint,
                   sck->m_local->m_addr,
                   (SOCKLEN_T *) &sck->m_local->m_len) != 0) ||
      (listen(sck->m_endpoint, 5) != 0))
  {
    close(sck->m_endpoint);
    sck->m_endpoint = -1;
    sck->m_error = GSOCK_IOERR;
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
GSocket *GSocket_WaitConnection(GSocket *socket)
{
  GSocket *connection = NULL ;
  GSocketError err;

  int arg = 1;

  assert(socket != NULL);

  /* Reenable CONNECTION events */
  socket->m_detected &= ~GSOCK_CONNECTION_FLAG;

  /* If the socket has already been created, we exit immediately */
  if (socket->m_endpoint == kOTInvalidEndpointRef || !socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return NULL;
  }

  /* Create a GSocket object for the new connection */
  connection = GSocket_new();

  if (!connection)
  {
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }

  /* Wait for a connection (with timeout) */
  if (_GSocket_Input_Timeout(socket) == GSOCK_TIMEDOUT)
  {
    GSocket_destroy(connection);
    /* socket->m_error set by _GSocket_Input_Timeout */
    return NULL;
  }

// TODO
#if 0
  connection->m_endpoint = accept(socket->m_endpoint, &from, (SOCKLEN_T *) &fromlen);
#endif

  if (connection->m_endpoint == kOTInvalidEndpointRef )
  {
    if (errno == EWOULDBLOCK)
      socket->m_error = GSOCK_WOULDBLOCK;
    else
      socket->m_error = GSOCK_IOERR;

    GSocket_destroy(connection);
    return NULL;
  }

  /* Initialize all fields */
  connection->m_server   = FALSE;
  connection->m_stream   = TRUE;
  connection->m_oriented = TRUE;

  /* Setup the peer address field */
  connection->m_peer = GAddress_new();
  if (!connection->m_peer)
  {
    GSocket_destroy(connection);
    socket->m_error = GSOCK_MEMERR;
    return NULL;
  }
 // TODO
 #if 0
  err = _GAddress_translate_from(connection->m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(connection->m_peer);
    GSocket_destroy(connection);
    socket->m_error = err;
    return NULL;
  }

  ioctl(connection->m_endpoint, FIONBIO, &arg);
#endif
  _GSocket_Enable_Events(connection);

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
GSocketError GSocket_SetNonOriented(GSocket *sck)
{
  int arg = 1;

  assert(sck != NULL);

  if (sck->m_endpoint != kOTInvalidEndpointRef )
  {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!sck->m_local)
  {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Initialize all fields */
  sck->m_stream   = FALSE;
  sck->m_server   = FALSE;
  sck->m_oriented = FALSE;

  /* Create the socket */
  
// TODO
#if 0
  sck->m_endpoint = socket(sck->m_local->m_realfamily, SOCK_DGRAM, 0);
  socket_set_ref( sck->m_endpoint , (unsigned long) &gMacNetEvents ,  (unsigned long) sck ) ;
#endif
  if (sck->m_endpoint == kOTInvalidEndpointRef )
  {
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }

// TODO
#if 0
  ioctl(sck->m_endpoint, FIONBIO, &arg);
#endif
  _GSocket_Enable_Events(sck);

  /* Bind to the local address,
   * and retrieve the actual address bound.
   */
// TODO
#if 0
  if ((bind(sck->m_endpoint, sck->m_local->m_addr, sck->m_local->m_len) != 0) ||
      (getsockname(sck->m_endpoint,
                   sck->m_local->m_addr,
                   (SOCKLEN_T *) &sck->m_local->m_len) != 0))
  {
    close(sck->m_endpoint);
    sck->m_endpoint    = -1;
    sck->m_error = GSOCK_IOERR;
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
 *  connection has been succesfully established, or one of the error
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
GSocketError GSocket_Connect(GSocket *sck, GSocketStream stream)
{
  int ret;
  int arg = 1;
  InetAddress addr ;
  TEndpointInfo	info;
  OTFlags flags = 0;
  OSStatus		err = kOTNoError;
  TCall peer ;

  assert(sck != NULL);

  /* Enable CONNECTION events (needed for nonblocking connections) */
  sck->m_detected &= ~GSOCK_CONNECTION_FLAG;

  if (sck->m_endpoint != kOTInvalidEndpointRef )
  {
    sck->m_error = GSOCK_INVSOCK;
    return GSOCK_INVSOCK;
  }

  if (!sck->m_peer)
  {
    sck->m_error = GSOCK_INVADDR;
    return GSOCK_INVADDR;
  }

  /* Streamed or dgram socket? */
  sck->m_stream   = (stream == GSOCK_STREAMED);
  sck->m_oriented = TRUE;
  sck->m_server   = FALSE;

  /* Create the socket */
#if TARGET_CARBON
  sck->m_endpoint = 
  	OTOpenEndpointInContext( OTCreateConfiguration( kTCPName) , 0 , &info , &err , NULL ) ;
#else
  sck->m_endpoint = 
  	OTOpenEndpoint( OTCreateConfiguration( kTCPName) , 0 , &info , &err ) ;
#endif
  if ( sck->m_endpoint == kOTInvalidEndpointRef || err != kOTNoError )
  {
		sck->m_endpoint = kOTInvalidEndpointRef ;
    	sck->m_error = GSOCK_IOERR;
    	return GSOCK_IOERR;
  }
  err = OTBind( sck->m_endpoint , nil , nil ) ;
  if ( err != kOTNoError )
  {
    	return GSOCK_IOERR;
  }
  SetDefaultEndpointModes( sck->m_endpoint , sck ) ;
// TODO
#if 0
  ioctl(sck->m_endpoint, FIONBIO, &arg);
#endif
  _GSocket_Enable_Events(sck);

  _GAddress_translate_to( sck->m_peer , &addr ) ;
  memset( &peer , 0 , sizeof( TCall ) ) ;
  peer.addr.len = sizeof( InetAddress ) ;
  peer.addr.buf = (unsigned char*) &addr ;
  err = OTConnect( sck->m_endpoint , &peer , nil ) ;
  if ( err != noErr )
  {
    /* If connect failed with EINPROGRESS and the GSocket object
     * is in blocking mode, we select() for the specified timeout
     * checking for writability to see if the connection request
     * completes.
     */ 
	
    if ((err == kOTNoDataErr ) && (!sck->m_non_blocking))
    {
      if (_GSocket_Output_Timeout(sck) == GSOCK_TIMEDOUT)
      {
      	OTSndOrderlyDisconnect( sck->m_endpoint ) ;
        sck->m_endpoint = kOTInvalidEndpointRef ;
        /* sck->m_error is set in _GSocket_Output_Timeout */
        return GSOCK_TIMEDOUT;
      }
      else
      {
/*
        int error;
        SOCKLEN_T len = sizeof(error);

        getsockopt(sck->m_endpoint, SOL_SOCKET, SO_ERROR, (void*) &error, &len);

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
    if ((err == kOTNoDataErr) && (sck->m_non_blocking))
    {
      sck->m_error = GSOCK_WOULDBLOCK;
      return GSOCK_WOULDBLOCK;
    }

    /* If connect failed with an error other than EINPROGRESS,
     * then the call to GSocket_Connect has failed.
     */
    OTSndOrderlyDisconnect( sck->m_endpoint ) ;

    sck->m_endpoint = kOTInvalidEndpointRef ;
    sck->m_error = GSOCK_IOERR;
    return GSOCK_IOERR;
  }
//  OTInetEventHandler(sck, T_CONNECT , kOTNoError , NULL ) ;
  return GSOCK_NOERROR;
}

/* Generic IO */

/* Like recv(), send(), ... */
int GSocket_Read(GSocket *socket, char *buffer, int size)
{
  int ret = 0 ;

  assert(socket != NULL);

  /* Reenable INPUT events */
  socket->m_detected &= ~GSOCK_INPUT_FLAG;

  if (socket->m_endpoint == kOTInvalidEndpointRef || socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for data (with a timeout) */
  if (_GSocket_Input_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

  /* Read the data */
  if (socket->m_stream)
    ret = _GSocket_Recv_Stream(socket, buffer, size);
  else
    ret = _GSocket_Recv_Dgram(socket, buffer, size);
    
  if (ret == -1)
  {
    if (errno == EWOULDBLOCK)
      socket->m_error = GSOCK_WOULDBLOCK;
    else
      socket->m_error = GSOCK_IOERR;
  }
  
  return ret;
}

int GSocket_Write(GSocket *socket, const char *buffer, int size)
{                        
  int ret;

  assert(socket != NULL);

  if (socket->m_endpoint == kOTInvalidEndpointRef || socket->m_server)
  {
    socket->m_error = GSOCK_INVSOCK;
    return -1;
  }

  /* If the socket is blocking, wait for writability (with a timeout) */
  if (_GSocket_Output_Timeout(socket) == GSOCK_TIMEDOUT)
    return -1;

  /* Write the data */
  if (socket->m_stream)
    ret = _GSocket_Send_Stream(socket, buffer, size);
  else
    ret = _GSocket_Send_Dgram(socket, buffer, size);
    
  if (ret == -1)
  {
    if (errno == EWOULDBLOCK)
      socket->m_error = GSOCK_WOULDBLOCK;
    else
      socket->m_error = GSOCK_IOERR;

    /* Only reenable OUTPUT events after an error (just like WSAAsyncSelect
     * in MSW). Once the first OUTPUT event is received, users can assume
     * that the socket is writable until a read operation fails. Only then
     * will further OUTPUT events be posted.
     */
    socket->m_detected &= ~GSOCK_OUTPUT_FLAG;
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
GSocketEventFlags GSocket_Select(GSocket *socket, GSocketEventFlags flags)
{
  OTResult	state ;
  assert(socket != NULL);
  wxMacProcessNotifierEvents() ;
  /*
  state = OTGetEndpointState(socket->m_endpoint);
  
  if ( ( flags & GSOCK_INPUT_FLAG ) && ! ( socket->m_detected & GSOCK_INPUT_FLAG ) )
  {
  	size_t sz = 0 ;
  	OTCountDataBytes( socket->m_endpoint , &sz ) ;
  	if ( state == T_INCON || sz > 0 )
  	{
        socket->m_detected |= GSOCK_INPUT_FLAG ;
		(socket->m_cbacks[GSOCK_INPUT])(socket, GSOCK_INPUT, socket->m_data[GSOCK_INPUT]);
 	}
  }
  if ( ( flags & GSOCK_INPUT_FLAG ) && ! ( socket->m_detected & GSOCK_OUTPUT_FLAG ) )
  {
  	if ( state == T_DATAXFER || state == T_INREL )
  	{
        socket->m_detected |=GSOCK_OUTPUT_FLAG ;
		(socket->m_cbacks[GSOCK_OUTPUT])(socket, GSOCK_OUTPUT, socket->m_data[GSOCK_OUTPUT]);
  	}
  }
  */
  return ( flags & socket->m_detected ) ;
}

/* Flags */

/* GSocket_SetNonBlocking:
 *  Sets the socket to non-blocking mode. All IO calls will return
 *  immediately.
 */
void GSocket_SetNonBlocking(GSocket *socket, int non_block)
{
  assert(socket != NULL);

  socket->m_non_blocking = non_block;
}

/* GSocket_SetTimeout:
 *  Sets the timeout for blocking calls. Time is expressed in
 *  milliseconds.
 */
void GSocket_SetTimeout(GSocket *socket, unsigned long millisec)
{
  assert(socket != NULL);

  socket->m_timeout = millisec;
}

/* GSocket_GetError:
 *  Returns the last error occured for this socket. Note that successful
 *  operations do not clear this back to GSOCK_NOERROR, so use it only
 *  after an error.
 */
GSocketError GSocket_GetError(GSocket *socket)
{
  assert(socket != NULL);

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
 *   Connection succesfully established, for client sockets, or incoming
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

  assert(socket != NULL);

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

int _GSocket_Recv_Stream(GSocket *socket, char *buffer, int size)
{
	OTFlags flags ;
	OTResult res ;
	OTByteCount sz = 0 ;

  	OTCountDataBytes( socket->m_endpoint , &sz ) ;
	res = OTRcv( socket->m_endpoint , buffer , size , &flags ) ;
	if ( res < 0 )
	{
		return -1 ;
	}
	
	// we simulate another read event if there are still bytes
	if ( socket->m_takesEvents )
	{
  		OTByteCount sz = 0 ;
  		OTCountDataBytes( socket->m_endpoint , &sz ) ;
  		if ( sz > 0 )
  		{
        	socket->m_detected |= GSOCK_INPUT_FLAG ;
			(socket->m_cbacks[GSOCK_INPUT])(socket, GSOCK_INPUT, socket->m_data[GSOCK_INPUT]);
 		}
 	}
 	return res ;
}

int _GSocket_Recv_Dgram(GSocket *socket, char *buffer, int size)
{
// TODO
  int ret = -1;
#if 0
  struct sockaddr from;
  SOCKLEN_T fromlen = sizeof(from);
  GSocketError err;

  fromlen = sizeof(from);

  ret = recvfrom(socket->m_endpoint, buffer, size, 0, &from, (SOCKLEN_T *) &fromlen);

  if (ret == -1)
    return -1;

  /* Translate a system address into a GSocket address */
  if (!socket->m_peer)
  {
    socket->m_peer = GAddress_new();
    if (!socket->m_peer)
    {
      socket->m_error = GSOCK_MEMERR;
      return -1;
    }
  }
  err = _GAddress_translate_from(socket->m_peer, &from, fromlen);
  if (err != GSOCK_NOERROR)
  {
    GAddress_destroy(socket->m_peer);
    socket->m_peer  = NULL;
    socket->m_error = err;
    return -1;
  }
#endif
  return ret;
}

int _GSocket_Send_Stream(GSocket *socket, const char *buffer, int size)
{
	OTFlags flags = 0 ;
	OTResult res ;

	res = OTSnd( socket->m_endpoint , (void*) buffer , size , flags ) ;
	return res ;
}

int _GSocket_Send_Dgram(GSocket *socket, const char *buffer, int size)
{
  int ret = -1 ;
// TODO
#if 0
  struct sockaddr *addr;
  int len ;
  GSocketError err;

  if (!socket->m_peer)
  {
    socket->m_error = GSOCK_INVADDR;
    return -1;
  }

  err = _GAddress_translate_to(socket->m_peer, &addr, &len);
  if (err != GSOCK_NOERROR)
  {
    socket->m_error = err;
    return -1;
  }

  ret = sendto(socket->m_endpoint, buffer, size, 0, addr, len);

  /* Frees memory allocated from _GAddress_translate_to */
  free(addr);
#endif
  return ret;
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
  struct in_addr *addr;

  assert(address != NULL);

  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);

  address->m_host = hostaddr ;

  return GSOCK_NOERROR;
}

struct service_entry 
{
	char * name ;
	unsigned short port ;
	char * protocol ; 
} ;
typedef struct service_entry service_entry ;

service_entry gServices[] =
{
	{ "http" , 80 , "tcp" } 
} ;

GSocketError GAddress_INET_SetPortName(GAddress *address, const char *port,
                                       const char *protocol)
{
  InetAddress *addr;
  int i ;

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
  InetAddress *addr;

  assert(address != NULL);
  CHECK_ADDRESS(address, INET, GSOCK_INVADDR);
  address->m_port = port ;
 
  return GSOCK_NOERROR;
}

GSocketError GAddress_INET_GetHostName(GAddress *address, char *hostname, size_t sbuf)
{
  InetDomainName name ;
  
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

  return address->m_host;
}

unsigned short GAddress_INET_GetPort(GAddress *address)
{
  assert(address != NULL); 
  CHECK_ADDRESS(address, INET, 0); 

  return address->m_port;
}

void _GSocket_Enable_Events(GSocket *socket)
{
  if ( socket->m_takesEvents )
  	return ;
  
  {
	  OTResult	state ;
	  socket->m_takesEvents			= TRUE ;
	  state = OTGetEndpointState(socket->m_endpoint);
	  
	  {
	  	OTByteCount sz = 0 ;
	  	OTCountDataBytes( socket->m_endpoint , &sz ) ;
	  	if ( state == T_INCON || sz > 0 )
	  	{
	        socket->m_detected |= GSOCK_INPUT_FLAG ;
			(socket->m_cbacks[GSOCK_INPUT])(socket, GSOCK_INPUT, socket->m_data[GSOCK_INPUT]);
	 	}
	  }
	  {
	  	if ( state == T_DATAXFER || state == T_INREL )
	  	{
	        socket->m_detected |=GSOCK_OUTPUT_FLAG ;
			(socket->m_cbacks[GSOCK_OUTPUT])(socket, GSOCK_OUTPUT, socket->m_data[GSOCK_OUTPUT]);
	  	}
	  }
  }
}

void _GSocket_Disable_Events(GSocket *socket)
{
  socket->m_takesEvents			= FALSE ;
}

/* _GSocket_Input_Timeout:
 *  For blocking sockets, wait until data is available or
 *  until timeout ellapses.
 */
GSocketError _GSocket_Input_Timeout(GSocket *socket)
{
  if ( !socket->m_non_blocking )
  {
    UnsignedWide now , start ;
    short formerTakesEvents = socket->m_takesEvents ;
    Microseconds(&start);
    now = start ;
    socket->m_takesEvents = FALSE ;
    
    while( (now.hi * 4294967296.0 + now.lo) - (start.hi * 4294967296.0 + start.lo) < socket->m_timeout * 1000.0 )
    {
    	OTResult state ;
   		OTByteCount sz = 0 ;
 		state = OTGetEndpointState(socket->m_endpoint);
  
  		OTCountDataBytes( socket->m_endpoint , &sz ) ;
  		if ( state == T_INCON || sz > 0 )
  		{
        	socket->m_takesEvents = formerTakesEvents ;
  			return GSOCK_NOERROR;
    	}
    	Microseconds(&now); 
    }
    socket->m_takesEvents = formerTakesEvents ;
    socket->m_error = GSOCK_TIMEDOUT;
    return GSOCK_TIMEDOUT;
  }
  return GSOCK_NOERROR;
}

/* _GSocket_Output_Timeout:
 *  For blocking sockets, wait until data can be sent without
 *  blocking or until timeout ellapses.
 */
GSocketError _GSocket_Output_Timeout(GSocket *socket)
{
  if ( !socket->m_non_blocking )
  {
    UnsignedWide now , start ;
    short formerTakesEvents = socket->m_takesEvents ;
    Microseconds(&start);
    now = start ;
    socket->m_takesEvents = FALSE ;
    
    while( (now.hi * 4294967296.0 + now.lo) - (start.hi * 4294967296.0 + start.lo) < socket->m_timeout * 1000.0 )
    {
    	OTResult state ;
 		state = OTGetEndpointState(socket->m_endpoint);
  
  		if ( state == T_DATAXFER || state == T_INREL )
 		{
        	socket->m_takesEvents = formerTakesEvents ;
  			return GSOCK_NOERROR;
    	}
    	Microseconds(&now); 
    }
    socket->m_takesEvents = formerTakesEvents ;
    socket->m_error = GSOCK_TIMEDOUT;
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
 *   Connection succesfully established, for client sockets, or incoming
 *   client connection, for server sockets. Wait for this event (also watch
 *   out for GSOCK_LOST) after you issue a nonblocking GSocket_Connect() call.
 * GSOCK_LOST:
 *   The connection is lost (or a connection request failed); this could
 *   be due to a failure, or due to the peer closing it gracefully.
 */

void _GSocket_Internal_Proc(unsigned long e , void* d )
{
		
  GSocket * socket = (GSocket*) d ;
  OTEventCode ev = (OTEventCode) e ;
  GSocketEvent event;
  GSocketEvent event2;
  GSocketCallback cback;
  char *data;
  GSocketCallback cback2;
  char *data2;

	if ( !socket )
		return ;
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
    if ((socket != NULL) && (socket->m_takesEvents))
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
					
					retCall.addr.buf	= NULL;
					retCall.addr.maxlen	= 0;
					retCall.opt.buf		= NULL;
					retCall.opt.maxlen	= 0;
					retCall.udata.buf	= NULL;
					retCall.udata.maxlen= 0;
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
}

GSocketError GAddress_UNIX_SetPath(GAddress *addr, const char *path)
{
}

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */
