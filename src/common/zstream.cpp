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

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_ZLIB && wxUSE_STREAMS

#include "wx/zstream.h"
#include "wx/utils.h"
#include "wx/intl.h"
#include "wx/log.h"

// When using configure, the path must be "zlib.h" I don't know
// what other ports (wxMac, wxMotif without configure) need here.
// If we are building with configure (defines __WX_SETUP_H__), 
// we trust the zlib path is given as a -I option.
#if defined(__WXMSW__) && !defined(__WX_SETUP_H__)
   #include "..\zlib\zlib.h"
#else
   #include "zlib.h"
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
  m_inflate = new z_stream_s;

  m_inflate->zalloc = (alloc_func)0;
  m_inflate->zfree = (free_func)0;
  m_inflate->opaque = (voidpf)0;

  err = inflateInit(m_inflate);
  if (err != Z_OK) {
    inflateEnd(m_inflate);
    delete m_inflate;
    return;
  }

  m_z_buffer = new unsigned char[ZSTREAM_BUFFER_SIZE];
  m_z_size = ZSTREAM_BUFFER_SIZE;

  m_inflate->avail_in = 0;
  m_inflate->next_in = NULL;
}

wxZlibInputStream::~wxZlibInputStream()
{
  inflateEnd(m_inflate);
  delete m_inflate;
}

size_t wxZlibInputStream::OnSysRead(void *buffer, size_t size)
{
  int err;

  m_inflate->next_out = (unsigned char *)buffer;
  m_inflate->avail_out = size;

  while (m_inflate->avail_out > 0) {
    if (m_inflate->avail_in == 0) {

      m_parent_i_stream->Read(m_z_buffer, m_z_size);
      m_inflate->next_in = m_z_buffer;
      m_inflate->avail_in = m_parent_i_stream->LastRead();

      if (m_parent_i_stream->LastError() != wxStream_NOERROR)
        return (size - m_inflate->avail_in);
    }
    err = inflate(m_inflate, Z_FINISH);
    if (err == Z_STREAM_END)
      return (size - m_inflate->avail_in);
  }

  return size-m_inflate->avail_in;
}

//////////////////////
// wxZlibOutputStream
//////////////////////

wxZlibOutputStream::wxZlibOutputStream(wxOutputStream& stream)
 : wxFilterOutputStream(stream)
{
  int err;

  m_deflate = new z_stream_s;

  m_deflate->zalloc = (alloc_func)0;
  m_deflate->zfree = (free_func)0;
  m_deflate->opaque = (voidpf)0;

  err = deflateInit(m_deflate, Z_DEFAULT_COMPRESSION);
  if (err != Z_OK) {
    deflateEnd(m_deflate);
    return;
  }

  m_z_buffer = new unsigned char[ZSTREAM_BUFFER_SIZE];
  m_z_size = ZSTREAM_BUFFER_SIZE;

  m_deflate->avail_in = 0;
  m_deflate->next_out = m_z_buffer;
  m_deflate->avail_out = m_z_size;
}

wxZlibOutputStream::~wxZlibOutputStream()
{
  int err;

  Sync();

  err = deflate(m_deflate, Z_FINISH);
  if (err != Z_STREAM_END) 
  {
    wxLogDebug( _T("wxZlibOutputStream: an error occured while closing the stream.\n") );
    return;
  }

  deflateEnd(m_deflate);

  delete[] m_z_buffer;
}

void wxZlibOutputStream::Sync()
{
  int err;

  m_parent_o_stream->Write(m_z_buffer, m_z_size-m_deflate->avail_out);
  m_deflate->next_out  = m_z_buffer;
  m_deflate->avail_out = m_z_size;

  err = deflate(m_deflate, Z_FULL_FLUSH);
  if (err != Z_OK) {
    return;
  }

  // Fixed by "Stefan Csomor" <csomor@advancedconcepts.ch>
  while( m_deflate->avail_out == 0 )
  {
     m_parent_o_stream->Write(m_z_buffer, m_z_size );
     m_deflate->next_out  = m_z_buffer;
     m_deflate->avail_out = m_z_size;
     err = deflate(m_deflate, Z_FULL_FLUSH);
     if (err != Z_OK) {
        return;
     }
  }
  // End

  m_parent_o_stream->Write(m_z_buffer, m_z_size-m_deflate->avail_out);
  m_deflate->next_out  = m_z_buffer;
  m_deflate->avail_out = m_z_size;
}

size_t wxZlibOutputStream::OnSysWrite(const void *buffer, size_t size)
{
  int err;

  m_deflate->next_in = (unsigned char *)buffer;
  m_deflate->avail_in = size;

  while (m_deflate->avail_in > 0) {

    if (m_deflate->avail_out == 0) {
      m_parent_o_stream->Write(m_z_buffer, m_z_size);
      if (m_parent_o_stream->LastError() != wxStream_NOERROR)
        return (size - m_deflate->avail_in);

      m_deflate->next_out = m_z_buffer;
      m_deflate->avail_out = m_z_size;
    }

    err = deflate(m_deflate, Z_NO_FLUSH);
    if (err != Z_OK)
      return (size - m_deflate->avail_in);
  }
  return size;
}

#endif
  // wxUSE_ZLIB && wxUSE_STREAMS
  
