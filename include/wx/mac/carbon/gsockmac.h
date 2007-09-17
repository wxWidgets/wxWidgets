/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket) for WX
 * Name:        gsockunx.h
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Purpose:     GSocket Macintosh header
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#ifndef __GSOCK_UNX_H
#define __GSOCK_UNX_H

#ifndef __GSOCKET_STANDALONE__
#include "wx/setup.h"
#endif

#if wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__)

#ifndef __GSOCKET_STANDALONE__
#include "wx/gsocket.h"
#else
#include "gsocket.h"
#endif

#ifndef OTUNIXERRORS

#include <MacHeaders.c>
#define OTUNIXERRORS 1
// we get a conflict in OT headers otherwise :
#undef EDEADLK
#include <OpenTransport.h>
#include <OpenTransportProviders.h>
#include <OpenTptInternet.h>

#endif

/* Definition of GSocket */
class GSocket
{
public:
    GSocket();
    ~GSocket();
    bool IsOk() { return m_ok; }

    void Shutdown();
    GSocketError SetLocal(GAddress *address);
    GSocketError SetPeer(GAddress *address);
    GAddress *GetLocal();
    GAddress *GetPeer();
    GSocketError SetServer();
    GSocket *WaitConnection();
    bool SetReusable() { return false; }
    GSocketError SetNonOriented();
    GSocketError Connect(GSocketStream stream);
    int Read(char *buffer, int size);
    int Write(const char *buffer, int size);
    GSocketEventFlags Select(GSocketEventFlags flags);
    void SetNonBlocking(bool non_block);
    void SetTimeout(unsigned long millisec);
    GSocketError WXDLLIMPEXP_NET GetError();
    void SetCallback(GSocketEventFlags flags,
        GSocketCallback callback, char *cdata);
    void UnsetCallback(GSocketEventFlags flags);
    GSocketError GetSockOpt(int level, int optname, void *optval, int *optlen)
    {   return GSOCK_INVOP; }
    GSocketError SetSockOpt(int level, int optname,
        const void *optval, int optlen)
    {   return GSOCK_INVOP; }

protected:
    bool m_ok;

    /* Input / Output */
    GSocketError Input_Timeout();
    GSocketError Output_Timeout();
    int Recv_Stream(char *buffer, int size);
    int Recv_Dgram(char *buffer, int size);
    int Send_Stream(const char *buffer, int size);
    int Send_Dgram(const char *buffer, int size);
    
    /* Callbacks */
    void Enable_Events();
    void Disable_Events();

// TODO: Make these protected
public:
  wxMacNotifierTableRef m_mac_events ;
  EndpointRef m_endpoint;
  GAddress *m_local;
  GAddress *m_peer;
  GSocketError m_error;

  bool m_non_blocking;
  bool m_server;
  bool m_stream;
  bool m_oriented;
  unsigned long m_timeout;

  /* Callbacks */
  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];
  bool m_takesEvents ;
};


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Definition of GAddress */

struct _GAddress
{
  UInt32         m_host ;
  UInt16         m_port ;
  GAddressType m_family;
  GSocketError m_error;
};

/* Callbacks */

void _GSocket_Internal_Proc(unsigned long e , void* data ) ;

/* GAddress */

GSocketError _GAddress_translate_from(GAddress *address,
                                      InetAddress *addr );
GSocketError _GAddress_translate_to(GAddress *address,
                                    InetAddress *addr);

GSocketError _GAddress_Init_INET(GAddress *address);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* wxUSE_SOCKETS || defined(__GSOCKET_STANDALONE__) */

#endif  /* __GSOCK_UNX_H */
