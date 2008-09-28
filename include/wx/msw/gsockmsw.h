/* -------------------------------------------------------------------------
 * Project:     GSocket (Generic Socket) for WX
 * Name:        gsockmsw.h
 * Copyright:   (c) Guilhem Lavaux
 * Licence:     wxWindows Licence
 * Purpose:     GSocket MSW header
 * CVSID:       $Id$
 * -------------------------------------------------------------------------
 */

#ifndef _WX_MSW_GSOCKMSW_H_
#define _WX_MSW_GSOCKMSW_H_

#include "wx/msw/wrapwin.h"

#if defined(__CYGWIN__)
    //CYGWIN gives annoying warning about runtime stuff if we don't do this
#   define USE_SYS_TYPES_FD_SET
#   include <sys/types.h>
#endif

#if defined(__WXWINCE__) || defined(__CYGWIN__)
#include <winsock.h>
#endif

/* Definition of GSocket */
class GSocket
{
public:
  GSocket();
  ~GSocket();
  bool IsOk() { return m_ok; }
  void Close();
  void Shutdown();
  GSocketError SetLocal(GAddress *address);
  GSocketError SetPeer(GAddress *address);
  GAddress *GetLocal();
  GAddress *GetPeer();
  GSocketError SetServer();
  GSocket *WaitConnection();
  // not used under MSW
  void Notify(bool) { }
  bool SetReusable();
  bool SetBroadcast();
  bool DontDoBind();
  GSocketError Connect(GSocketStream stream);
  GSocketError SetNonOriented();
  int Read(char *buffer, int size);
  int Write(const char *buffer, int size);
  GSocketEventFlags Select(GSocketEventFlags flags);
  void SetNonBlocking(bool non_block);
  void SetTimeout(unsigned long millis);
  GSocketError WXDLLIMPEXP_NET GetError();
  void SetCallback(GSocketEventFlags flags,
    GSocketCallback callback, char *cdata);
  void UnsetCallback(GSocketEventFlags flags);
  GSocketError GetSockOpt(int level, int optname,
    void *optval, int *optlen);
  GSocketError SetSockOpt(int level, int optname,
    const void *optval, int optlen);

  void SetInitialSocketBuffers(int recv, int send)
  {
      m_initialRecvBufferSize = recv;
      m_initialSendBufferSize = send;
  }

protected:
  GSocketError Input_Timeout();
  GSocketError Output_Timeout();
  GSocketError Connect_Timeout();
  int Recv_Stream(char *buffer, int size);
  int Recv_Dgram(char *buffer, int size);
  int Send_Stream(const char *buffer, int size);
  int Send_Dgram(const char *buffer, int size);
  bool m_ok;
  int m_initialRecvBufferSize;
  int m_initialSendBufferSize;

/* TODO: Make these protected */
public:
  SOCKET m_fd;
  GAddress *m_local;
  GAddress *m_peer;
  GSocketError m_error;

  /* Attributes */
  bool m_non_blocking;
  bool m_server;
  bool m_stream;
  bool m_establishing;
  bool m_reusable;
  bool m_broadcast;
  bool m_dobind;
  struct timeval m_timeout;

  /* Callbacks */
  GSocketEventFlags m_detected;
  GSocketCallback m_cbacks[GSOCK_MAX_EVENT];
  char *m_data[GSOCK_MAX_EVENT];
  int m_msgnumber;
};

#endif  /* _WX_MSW_GSOCKMSW_H_ */
