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
#include <wx/intl.h>
#include "../zlib/zlib.h"   // don't change this, Robert

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#define ZSTREAM_BUFFER_SIZE 1024

//////////////////////
// wxZlibInputStream
//////////////////////

wxZlibInputStream::wxZlibInputStream(wxInputStream& stream)
  : wxFilterInputStream(stream)
{
  int err;

  // I need a private stream buffer.
  m_i_streambuf = new wxStreamBuffer(*this);
  m_i_destroybuf = TRUE;

  m_inflate.zalloc = (alloc_func)0;
  m_inflate.zfree = (free_func)0;
  m_inflate.opaque = (voidpf)0;

  err = inflateInit(&m_inflate);
  if (err != Z_OK) {
    inflateEnd(&m_inflate);
    return;
  }

  m_z_buffer = new unsigned char[ZSTREAM_BUFFER_SIZE];
  m_z_size = ZSTREAM_BUFFER_SIZE;

  m_inflate.avail_in = 0;
  m_inflate.next_in = NULL;
}

wxZlibInputStream::~wxZlibInputStream()
{
  inflateEnd(&m_inflate);
}

size_t wxZlibInputStream::DoRead(void *buffer, size_t size)
{
  int err;

  m_inflate.next_out = (unsigned char *)buffer;
  m_inflate.avail_out = size;

  while (m_inflate.avail_out > 0) {
    if (m_inflate.avail_in == 0) {

      m_parent_i_stream->Read(m_z_buffer, m_z_size);
      m_inflate.next_in = m_z_buffer;
      m_inflate.avail_in = m_parent_i_stream->LastRead();

      if (m_parent_i_stream->Eof())
        return (size - m_inflate.avail_in);
    }
    err = inflate(&m_inflate, Z_FINISH);
    if (err == Z_STREAM_END)
      return (size - m_inflate.avail_in);
  }

  return size-m_inflate.avail_in;
}

bool wxZlibInputStream::Eof() const
{
  if (!m_eof)
    return m_parent_i_stream->Eof();
  return m_eof;
}

//////////////////////
// wxZlibOutputStream
//////////////////////

wxZlibOutputStream::wxZlibOutputStream(wxOutputStream& stream)
 : wxFilterOutputStream(stream)
{
  int err;

  m_o_streambuf = new wxStreamBuffer(*this);
  m_o_destroybuf = TRUE;

  m_deflate.zalloc = (alloc_func)0;
  m_deflate.zfree = (free_func)0;
  m_deflate.opaque = (voidpf)0;

  err = deflateInit(&m_deflate, Z_DEFAULT_COMPRESSION);
  if (err != Z_OK) {
    deflateEnd(&m_deflate);
    return;
  }

  m_z_buffer = new unsigned char[ZSTREAM_BUFFER_SIZE];
  m_z_size = ZSTREAM_BUFFER_SIZE;

  m_deflate.avail_in = 0;
  m_deflate.next_out = m_z_buffer;
  m_deflate.avail_out = m_z_size;
}

wxZlibOutputStream::~wxZlibOutputStream()
{
  int err;

  Sync();

  err = deflate(&m_deflate, Z_FINISH);
  if (err != Z_STREAM_END) {
    wxDebugMsg(_("wxZlibOutputStream: an error occured while we was closing "
               "the stream.\n"));
    return;
  }

  deflateEnd(&m_deflate);

  delete[] m_z_buffer;
}

void wxZlibOutputStream::Sync()
{
  int err;

  m_parent_o_stream->Write(m_z_buffer, m_z_size-m_deflate.avail_out);
  m_deflate.next_out  = m_z_buffer;
  m_deflate.avail_out = m_z_size;

  err = deflate(&m_deflate, Z_FULL_FLUSH);
  if (err != Z_OK) {
    m_bad = TRUE;
    return;
  }

  m_parent_o_stream->Write(m_z_buffer, m_z_size-m_deflate.avail_out);
  m_deflate.next_out  = m_z_buffer;
  m_deflate.avail_out = m_z_size;
}

size_t wxZlibOutputStream::DoWrite(const void *buffer, size_t size)
{
  int err;

  m_deflate.next_in = (unsigned char *)buffer;
  m_deflate.avail_in = size;

  while (m_deflate.avail_in > 0) {

    if (m_deflate.avail_out == 0) {
      m_parent_o_stream->Write(m_z_buffer, m_z_size);
      if (m_parent_o_stream->Bad())
        return (size - m_deflate.avail_in);

      m_deflate.next_out = m_z_buffer;
      m_deflate.avail_out = m_z_size;
    }

    err = deflate(&m_deflate, Z_NO_FLUSH);
    if (err != Z_OK)
      return (size - m_deflate.avail_in);
  }
  return size;
}

bool wxZlibOutputStream::Bad() const
{
  if (!m_bad)
    return m_parent_o_stream->Bad();
  return m_bad;
}
