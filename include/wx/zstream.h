/////////////////////////////////////////////////////////////////////////////
// Name:        zstream.h
// Purpose:     Memory stream classes
// Author:      Guilhem Lavaux
// Modified by: Mike Wetherell
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WXZSTREAM_H__
#define _WX_WXZSTREAM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "zstream.h"
#endif

#include "wx/defs.h"

#if wxUSE_ZLIB && wxUSE_STREAMS

#include "wx/stream.h"

// Compression level
enum {
    wxZ_DEFAULT_COMPRESSION = -1,
    wxZ_NO_COMPRESSION = 0,
    wxZ_BEST_SPEED = 1,
    wxZ_BEST_COMPRESSION = 9
};

// Flags
enum {
    wxZLIB_NO_HEADER = 1   // required for use in Gzip and Zip files
};

class WXDLLIMPEXP_BASE wxZlibInputStream: public wxFilterInputStream {
 public:
  wxZlibInputStream(wxInputStream& stream, int flags = 0);
  virtual ~wxZlibInputStream();

  char Peek() { return wxInputStream::Peek(); }
  size_t GetSize() const { return wxInputStream::GetSize(); }

 protected:
  size_t OnSysRead(void *buffer, size_t size);
  off_t OnSysTell() const { return m_pos; }

 protected:
  size_t m_z_size;
  unsigned char *m_z_buffer;
  struct z_stream_s *m_inflate;
  off_t m_pos;

    DECLARE_NO_COPY_CLASS(wxZlibInputStream)
};

class WXDLLIMPEXP_BASE wxZlibOutputStream: public wxFilterOutputStream {
 public:
  wxZlibOutputStream(wxOutputStream& stream, int level = -1, int flags = 0);
  virtual ~wxZlibOutputStream();

  void Sync() { DoFlush(false); }
  size_t GetSize() const { return (size_t)m_pos; }

 protected:
  size_t OnSysWrite(const void *buffer, size_t size);
  off_t OnSysTell() const { return m_pos; }

  virtual void DoFlush(bool final);

 protected:
  size_t m_z_size;
  unsigned char *m_z_buffer;
  struct z_stream_s *m_deflate;
  off_t m_pos;

    DECLARE_NO_COPY_CLASS(wxZlibOutputStream)
};

#endif
  // wxUSE_ZLIB && wxUSE_STREAMS

#endif
   // _WX_WXZSTREAM_H__

