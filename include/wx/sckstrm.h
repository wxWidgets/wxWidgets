/////////////////////////////////////////////////////////////////////////////
// Name:        sckstrm.h
// Purpose:     wxSocket*Stream
// Author:      Guilhem Lavaux
// Modified by:
// Created:     17/07/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef __SCK_STREAM_H__
#define __SCK_STREAM_H__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/stream.h"
#include "wx/socket.h"

class WXDLLEXPORT wxSocketOutputStream : public virtual wxOutputStream
{
 public:
  wxSocketOutputStream(wxSocketBase& s);
  virtual ~wxSocketOutputStream();

  wxOutputStream& Write(const void *buffer, size_t size);
  off_t SeekO( off_t WXUNUSED(pos), wxSeekMode WXUNUSED(mode) ) 
    { return -1; }
  off_t TellO() 
    { return -1; }

  bool Bad() { return m_o_socket->IsDisconnected(); }
  size_t LastWrite() { return m_o_socket->LastCount(); }
 protected:
  wxSocketBase *m_o_socket;
};

class WXDLLEXPORT wxSocketInputStream : public virtual wxInputStream
{
 public:
  wxSocketInputStream(wxSocketBase& s);
  ~wxSocketInputStream();

  wxInputStream& Read(void *buffer, size_t size);
  off_t SeekI( off_t WXUNUSED(pos), wxSeekMode WXUNUSED(mode) ) 
    { return -1; }
  off_t TellI() 
    { return -1; }

  bool Eof() { return m_i_socket->IsDisconnected(); }
  size_t LastRead() { return m_i_socket->LastCount(); }
 protected:
  wxSocketBase *m_i_socket;
};

class WXDLLEXPORT wxSocketStream : public wxSocketInputStream,
                                   public wxSocketOutputStream,
				   public wxStream {

 public:
  wxSocketStream(wxSocketBase& i_s, wxSocketBase& o_s);
  wxSocketStream(wxSocketBase& s);
};

#endif
