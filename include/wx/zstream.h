/////////////////////////////////////////////////////////////////////////////
// Name:        zstream.h
// Purpose:     Memory stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef __WXZSTREAM_H__
#define __WXZSTREAM_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <wx/stream.h>
#include "../../src/zlib/zlib.h"

class wxZlibInputStream: public wxFilterInputStream {
 public:
  wxZlibInputStream(wxInputStream& stream);
  virtual ~wxZlibInputStream();

  wxInputStream& Read(void *buffer, size_t size);
  off_t SeekI(off_t pos, wxSeekMode mode = wxFromStart);
  off_t TellI() const;

  size_t LastRead() const { return m_lastread; }
  bool Eof() const;

 protected:
  size_t m_lastread;
  size_t m_z_size;
  unsigned char *m_z_buffer;
  bool m_eof;
  struct z_stream_s m_inflate;
};

class wxZlibOutputStream: public wxFilterOutputStream {
 public:
  wxZlibOutputStream(wxOutputStream& stream);
  virtual ~wxZlibOutputStream();

  wxOutputStream& Write(const void *buffer, size_t size);
  off_t SeekO(off_t pos, wxSeekMode mode = wxFromStart);
  off_t TellO() const;

  size_t LastWrite() const { return m_lastwrite; }
  bool Bad() const;

 protected:
  size_t m_lastwrite;
  size_t m_z_size;
  unsigned char *m_z_buffer;
  bool m_bad;
  struct z_stream_s m_deflate;
};

#endif
