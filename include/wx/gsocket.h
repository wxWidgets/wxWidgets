/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket)
 * Name:        gsocket.h
 * Author:      Guilhem Lavaux
 *              Guillermo Rodriguez Garcia <guille@iies.es> 
 *              Angel Vidal Veiga <kry@amule.org> (mantainer)
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Purpose:     GSocket include file (system independent)
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#ifndef __GSOCKET_H
#define __GSOCKET_H

#include "wx/defs.h"

#if wxUSE_SOCKETS

#include "wx/dlimpexp.h" /* for WXDLLIMPEXP_NET */
#include "wx/setup.h"

#include <stddef.h>

#include <sys/time.h>

/*
   Including sys/types.h under cygwin results in the warnings about "fd_set
   having been defined in sys/types.h" when winsock.h is included later and
   doesn't seem to be necessary anyhow. It's not needed under Mac neither.
 */
#if !defined(__WXMAC__) && !defined(__CYGWIN__) && !defined(__WXWINCE__)
#include <sys/types.h>
#endif

#ifdef __WINDOWS__
  #include "wx/msw/wrapwin.h"
  #ifdef __WXWINCE__
    #include <stdlib.h>
  #endif
  #if defined(__WXWINCE__) || defined(__CYGWIN__)
    #include <winsock.h>
  #endif
#endif

class GSocket;

#ifndef SOCKET
/* Most probably any non-MSW platform */
#define SOCKET int
#endif

#ifndef SOCKET_ERROR
/* Most probably any non-MSW platform */
#define SOCKET_ERROR -1
#endif

typedef struct _GAddress GAddress;

typedef enum {
  GSOCK_STREAMED,
  GSOCK_UNSTREAMED
} GSocketStream;

typedef enum {
  GSOCK_NOERROR = 0,
  GSOCK_INVOP,
  GSOCK_IOERR,
  GSOCK_INVADDR,
  GSOCK_INVSOCK,
  GSOCK_NOHOST,
  GSOCK_INVPORT,
  GSOCK_WOULDBLOCK,
  GSOCK_TIMEDOUT,
  GSOCK_MEMERR,
  GSOCK_OPTERR
} GSocketError;

/* See below for an explanation on how events work.
 */
typedef enum {
  GSOCK_INPUT  = 0,
  GSOCK_OUTPUT = 1,
  GSOCK_CONNECTION = 2,
  GSOCK_LOST = 3,
  GSOCK_MAX_EVENT = 4
} GSocketEvent;

enum {
  GSOCK_INPUT_FLAG = 1 << GSOCK_INPUT,
  GSOCK_OUTPUT_FLAG = 1 << GSOCK_OUTPUT,
  GSOCK_CONNECTION_FLAG = 1 << GSOCK_CONNECTION,
  GSOCK_LOST_FLAG = 1 << GSOCK_LOST
};

typedef int GSocketEventFlags;

typedef void (*GSocketCallback)(GSocket *socket, GSocketEvent event,
                                char *cdata);


/* Functions tables for internal use by GSocket code: */

/* Actually this is a misnomer now, but reusing this name means I don't
   have to ifdef app traits or common socket code */
class GSocketGUIFunctionsTable
{
public:
    // needed since this class declares virtual members
    virtual ~GSocketGUIFunctionsTable() { }
    virtual bool OnInit() = 0;
    virtual void OnExit() = 0;
    virtual bool CanUseEventLoop() = 0;
    virtual bool Init_Socket(GSocket *socket) = 0;
    virtual void Destroy_Socket(GSocket *socket) = 0;
    virtual void Enable_Event(GSocket *socket, GSocketEvent event) = 0;
    virtual void Disable_Event(GSocket *socket, GSocketEvent event) = 0;
    virtual void Enable_Events(GSocket *socket) = 0;
    virtual void Disable_Events(GSocket *socket) = 0;
};


/* Global initializers */

/* Sets GUI functions callbacks. Must be called *before* GSocket_Init
   if the app uses async sockets. */
void GSocket_SetGUIFunctions(GSocketGUIFunctionsTable *guifunc);

/* GSocket_Init() must be called at the beginning */
int GSocket_Init(void);

/* GSocket_Cleanup() must be called at the end */
void GSocket_Cleanup(void);


/* Constructors / Destructors */

GSocket *GSocket_new(void);


class GSocketGUIFunctionsTableConcrete: public GSocketGUIFunctionsTable
{
public:
    virtual bool OnInit();
    virtual void OnExit();
    virtual bool CanUseEventLoop();
    virtual bool Init_Socket(GSocket *socket);
    virtual void Destroy_Socket(GSocket *socket);
    virtual void Enable_Event(GSocket *socket, GSocketEvent event);
    virtual void Disable_Event(GSocket *socket, GSocketEvent event);
    virtual void Enable_Events(GSocket *socket);
    virtual void Disable_Events(GSocket *socket);
};

class GSocket
{
public:
  GSocket();
  virtual ~GSocket();

  GSocketError WXDLLIMPEXP_NET GetError();

  bool IsOk() { return m_ok; }

  void Shutdown();

  GSocketError Connect(GSocketStream stream);
  GSocket *WaitConnection();

  GSocketEventFlags Select(GSocketEventFlags flags);
  int Read(char *buffer, int size);
  int Write(const char *buffer, int size);

  GSocketError SetNonOriented();

  void SetNonBlocking(bool non_block);

  GSocketError SetServer();

  bool SetReusable();

  void SetTimeout(unsigned long millisec);

  GAddress *GetLocal();
  GSocketError SetLocal(GAddress *address);

  GAddress *GetPeer();
  GSocketError SetPeer(GAddress *address);

  void SetCallback(GSocketEventFlags flags,
      GSocketCallback callback, char *cdata);
  void UnsetCallback(GSocketEventFlags flags);


  GSocketError GetSockOpt(int level, int optname, void *optval, int *optlen);
  GSocketError SetSockOpt(int level, int optname,
      const void *optval, int optlen);

  virtual void Detected_Read();
  virtual void Detected_Write();
  virtual void Detected_Connect();
  virtual void Detected_Lost();

public:
  #warning To be changed to accessors

  SOCKET m_fd;
  bool m_server;

  void* m_platform_specific_data;

  int m_eventflags;

private:

  void Close();
  void Enable(GSocketEvent event);
  void Disable(GSocketEvent event);
  bool UDPDisconnect();

  GSocketError Input_Timeout();
  GSocketError Output_Timeout();
  GSocketError Connect_Timeout();
  int Recv_Stream(char *buffer, int size);
  int Recv_Dgram(char *buffer, int size);
  int Send_Stream(const char *buffer, int size);
  int Send_Dgram(const char *buffer, int size);
  bool m_ok;

  struct timeval m_timeout;

  GAddress *m_local;
  GAddress *m_peer;
  GSocketError m_error;

  bool m_non_blocking;
  bool m_stream;
  bool m_establishing;
  bool m_reusable;
  bool m_udpconnected;

  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];

};

#endif  /* wxUSE_SOCKETS */

#endif    /* __GSOCKET_H */
