/* -------------------------------------------------------------------------
 * Project: GSocket (Generic Socket) for WX
 * Name:    gsockunx.h
 * Purpose: GSocket Macintosh header
 * CVSID:   $Id$
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

#include <MacHeaders.c>
#define OTUNIXERRORS 1
#include <OpenTransport.h>
#include <OpenTransportProviders.h>
#include <OpenTptInternet.h>

/* Definition of GSocket */
class GSocket
{
public:
    GSocket();
    ~GSocket();
    bool IsOK() { return m_ok; }

    void Shutdown();
    GSocketError SetLocal(GAddress *address);
    GSocketError SetPeer(GAddress *address);
    GAddress *GetLocal();
    GAddress *GetPeer();
    GSocketError SetServer();
    GSocket *WaitConnection();
    int SetReusable() { return 0; }
    GSocketError SetNonOriented();
    GSocketError Connect(GSocketStream stream);
    int Read(char *buffer, int size);
    int Write(const char *buffer, int size);
    GSocketEventFlags Select(GSocketEventFlags flags);
    void SetNonBlocking(int non_block);
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

  int m_non_blocking;
  int m_server;
  int m_stream;
  int m_oriented;
  unsigned long m_timeout;

  /* Callbacks */
  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];
  int m_takesEvents ;
};

/* Compatibility methods to support old C API (from gsocket.h) */
inline void GSocket_Shutdown(GSocket *socket)
{   socket->Shutdown(); }
inline GSocketError GSocket_SetLocal(GSocket *socket, GAddress *address)
{   return socket->SetLocal(address); }
inline GSocketError GSocket_SetPeer(GSocket *socket, GAddress *address)
{   return socket->SetPeer(address); }
inline GAddress *GSocket_GetLocal(GSocket *socket)
{   return socket->GetLocal(); }
inline GAddress *GSocket_GetPeer(GSocket *socket)
{   return socket->GetPeer(); }
inline GSocketError GSocket_SetServer(GSocket *socket)
{   return socket->SetServer(); }
inline GSocket *GSocket_WaitConnection(GSocket *socket)
{   return socket->WaitConnection(); }
inline int GSocket_SetReusable(GSocket *socket)
{   return socket->SetReusable(); }
inline GSocketError GSocket_Connect(GSocket *socket, GSocketStream stream)
{   return socket->Connect(stream); }
inline GSocketError GSocket_SetNonOriented(GSocket *socket)
{   return socket->SetNonOriented(); }
inline int GSocket_Read(GSocket *socket, char *buffer, int size)
{   return socket->Read(buffer,size); }
inline int GSocket_Write(GSocket *socket, const char *buffer, int size)
{   return socket->Write(buffer,size); }
inline GSocketEventFlags GSocket_Select(GSocket *socket, GSocketEventFlags flags)
{   return socket->Select(flags); }
inline void GSocket_SetNonBlocking(GSocket *socket, int non_block)
{   socket->SetNonBlocking(non_block); }
inline void GSocket_SetTimeout(GSocket *socket, unsigned long millisec)
{   socket->SetTimeout(millisec); }
inline GSocketError GSocket_GetError(GSocket *socket)
{   return socket->GetError(); }
inline void GSocket_SetCallback(GSocket *socket, GSocketEventFlags flags,
                         GSocketCallback fallback, char *cdata)
{   socket->SetCallback(flags,fallback,cdata); }
inline void GSocket_UnsetCallback(GSocket *socket, GSocketEventFlags flags)
{   socket->UnsetCallback(flags); }
inline GSocketError GSocket_GetSockOpt(GSocket *socket, int level, int optname,
                        void *optval, int *optlen)
{   return socket->GetSockOpt(level,optname,optval,optlen); }
inline GSocketError GSocket_SetSockOpt(GSocket *socket, int level, int optname,
                        const void *optval, int optlen)
{   return socket->SetSockOpt(level,optname,optval,optlen); }
inline void GSocket_destroy(GSocket *socket)
{   delete socket; }

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
