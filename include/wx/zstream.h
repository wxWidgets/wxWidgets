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
#include "zlib.h"

class wxZlibInputStream: public wxFilterInputStream {
 public:
  wxZlibInputStream(wxInputStream& stream);
  virtual ~wxZlibInputStream();

  bool Eof() const;

 protected:
  size_t DoRead(void *buffer, size_t size);
  off_t DoSeekInput(off_t pos, wxSeekMode WXUNUSED(mode)) { return wxInvalidOffset; }
  off_t DoTellInput() const { return wxInvalidOffset; }

 protected:
  size_t m_z_size;
  unsigned char *m_z_buffer;
  struct z_stream_s m_inflate;
};

class wxZlibOutputStream: public wxFilterOutputStream {
 public:
  wxZlibOutputStream(wxOutputStream& stream);
  virtual ~wxZlibOutputStream();

  void Sync();

  bool Bad() const;

 protected:
  size_t DoWrite(const void *buffer, size_t size);
  off_t DoSeekOutput(off_t pos, wxSeekMode WXUNUSED(mode)) { return wxInvalidOffset; }
  off_t DoTellOutput() const { return wxInvalidOffset; }

 protected:
  size_t m_z_size;
  unsigned char *m_z_buffer;
  struct z_stream_s m_deflate;
};

#endif
