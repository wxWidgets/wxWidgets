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
#if WXWIN_COMPATIBILITY_2_4
    wxZLIB_24COMPATIBLE = 4, // read v2.4.x data without error
#endif
    wxZLIB_NO_HEADER = 0,    // raw deflate stream, no header or checksum
    wxZLIB_ZLIB = 1,         // zlib header and checksum
    wxZLIB_GZIP = 2,         // gzip header and checksum, requires zlib 1.2.1+
    wxZLIB_AUTO = 3          // autodetect header zlib or gzip
};

class WXDLLIMPEXP_BASE wxZlibInputStream: public wxFilterInputStream {
 public:
  wxZlibInputStream(wxInputStream& stream, int flags = wxZLIB_AUTO);
  virtual ~wxZlibInputStream();

  char Peek() { return wxInputStream::Peek(); }
  wxFileOffset GetLength() const { return wxInputStream::GetLength(); }

  static bool CanHandleGZip();

 protected:
  size_t OnSysRead(void *buffer, size_t size);
  wxFileOffset OnSysTell() const { return m_pos; }

 protected:
  size_t m_z_size;
  unsigned char *m_z_buffer;
  struct z_stream_s *m_inflate;
  wxFileOffset m_pos;
#if WXWIN_COMPATIBILITY_2_4
  bool m_24compatibilty;
#endif

  DECLARE_NO_COPY_CLASS(wxZlibInputStream)
};

class WXDLLIMPEXP_BASE wxZlibOutputStream: public wxFilterOutputStream {
 public:
  wxZlibOutputStream(wxOutputStream& stream, int level = -1, int flags = wxZLIB_ZLIB);
  virtual ~wxZlibOutputStream();

  void Sync() { DoFlush(false); }
  wxFileOffset GetLength() const { return m_pos; }

  static bool CanHandleGZip();

 protected:
  size_t OnSysWrite(const void *buffer, size_t size);
  wxFileOffset OnSysTell() const { return m_pos; }

  virtual void DoFlush(bool final);

 protected:
  size_t m_z_size;
  unsigned char *m_z_buffer;
  struct z_stream_s *m_deflate;
  wxFileOffset m_pos;

  DECLARE_NO_COPY_CLASS(wxZlibOutputStream)
};

#endif
  // wxUSE_ZLIB && wxUSE_STREAMS

#endif
   // _WX_WXZSTREAM_H__

