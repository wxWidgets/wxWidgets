/////////////////////////////////////////////////////////////////////////////
// Name:        zstream.cpp
// Purpose:     Compressed stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "zstream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include <wx/stream.h>
#include <wx/zstream.h>
#include <wx/utils.h>
#include "zlib.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxZlibInputStream, wxFilterInputStream)
IMPLEMENT_CLASS(wxZlibOutputStream, wxFilterOutputStream)
#endif

//////////////////////
// wxZlibInputStream
//////////////////////

wxZlibInputStream::wxZlibInputStream(wxInputStream& stream)
  : wxFilterInputStream(stream)
{
  int err;

  m_inflate.zalloc = (alloc_func)0;
  m_inflate.zfree = (free_func)0;
  m_inflate.opaque = (voidpf)0;

  err = inflateInit(&m_inflate);
  if (err != Z_OK) {
    inflateEnd(&m_inflate);
    return;
  }

  m_inflate.avail_in = 0;
}

wxZlibInputStream::~wxZlibInputStream()
{
  inflateEnd(&m_inflate);
}

wxInputStream& wxZlibInputStream::Read(void *buffer, size_t size)
{
  int err;

  m_inflate.next_out = (unsigned char *)buffer;
  m_inflate.avail_out = size;
  m_eof = FALSE;

  while (m_inflate.avail_out > 0) {
    if (m_inflate.avail_in == 0) {
      wxFilterInputStream::Read(m_z_buffer, m_z_size);
      m_inflate.next_in = m_z_buffer;
      m_inflate.avail_in = wxFilterInputStream::LastRead();
      if (wxFilterInputStream::Eof()) {
        m_lastread = size - m_inflate.avail_out;
        return *this;
      }
    }
    err = inflate(&m_inflate, Z_FINISH);
    if (err == Z_STREAM_END) {
      m_lastread = size - m_inflate.avail_out;
      m_eof = TRUE;
      return *this;
    }
  }

  m_lastread = size;
  return *this;
}

off_t wxZlibInputStream::SeekI(off_t WXUNUSED(pos), wxSeekMode WXUNUSED(mode))
{
  return 0;
}

off_t wxZlibInputStream::TellI() const
{
  return 0;
}

bool wxZlibInputStream::Eof() const
{
  if (!m_eof)
    return wxFilterInputStream::Eof();
  return m_eof;
}

//////////////////////
// wxZlibOutputStream
//////////////////////

wxZlibOutputStream::wxZlibOutputStream(wxOutputStream& stream)
 : wxFilterOutputStream(stream)
{
  int err;

  m_deflate.zalloc = (alloc_func)0;
  m_deflate.zfree = (free_func)0;
  m_deflate.opaque = (voidpf)0;

  err = deflateInit(&m_deflate, Z_DEFAULT_COMPRESSION);
  if (err != Z_OK) {
    deflateEnd(&m_deflate);
    return;
  }
  m_deflate.avail_in = 0;
  m_deflate.next_out = m_z_buffer;
  m_deflate.avail_out = m_z_size;
}

wxZlibOutputStream::~wxZlibOutputStream()
{
  int err;

  while (1) {
    err = deflate(&m_deflate, Z_FINISH);
    if (err == Z_STREAM_END)
      break;
    if (err < 0) {
      wxDebugMsg("wxZlibOutputStream: error during final deflate");
      break;
    }
    if (m_deflate.avail_out == 0) {
      wxFilterOutputStream::Write(m_z_buffer, m_z_size);
      if (wxFilterOutputStream::Bad()) {
        wxDebugMsg("wxZlibOutputStream: error during final write");
        break;
      }
      m_deflate.next_out = m_z_buffer;
      m_deflate.avail_out = m_z_size;
    }
  }
  wxFilterOutputStream::Write(m_z_buffer, m_z_size-m_deflate.avail_out);

  deflateEnd(&m_deflate);
}

wxOutputStream& wxZlibOutputStream::Write(const void *buffer, size_t size)
{
  int err;

  m_deflate.next_in = (unsigned char *)buffer;
  m_deflate.avail_in = size;

  m_bad = FALSE;
  while (m_deflate.avail_in > 0) {
    if (m_deflate.avail_out == 0) {
      wxFilterOutputStream::Write(m_z_buffer, m_z_size);
      if (wxFilterOutputStream::Bad()) {
        m_lastwrite = size - m_deflate.avail_in;
        return *this;
      }
            
      m_deflate.next_out = m_z_buffer;
      m_deflate.avail_out = m_z_size;
    }
    err = deflate(&m_deflate, Z_NO_FLUSH);
    if (err < 0) {
      m_bad = TRUE;
      m_lastwrite = size - m_deflate.avail_in;
      return *this;
    }
  }
  m_lastwrite = size;
  return *this;
}

off_t wxZlibOutputStream::SeekO(off_t WXUNUSED(pos), wxSeekMode WXUNUSED(mode))
{
  return 0;
}

off_t wxZlibOutputStream::TellO() const
{
  return 0;
}

bool wxZlibOutputStream::Bad() const
{
  if (!m_bad)
    return wxFilterOutputStream::Bad();
  return m_bad;
}
