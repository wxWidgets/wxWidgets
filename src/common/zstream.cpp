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

// normally, the compiler options should contain -I../zlib, but it is
// apparently not the case for all MSW makefiles and so, unless we use
// configure (which defines __WX_SETUP_H__) or it is explicitly overridden by
// the user (who can define wxUSE_ZLIB_H_IN_PATH), we hardcode the path here
#if defined(__WXMSW__) && !defined(__WX_SETUP_H__) && !defined(wxUSE_ZLIB_H_IN_PATH)
   #include "../zlib/zlib.h"
#elif defined(__WXMAC__) && defined(__UNIX__)
   #include <Kernel/net/zlib.h>
#else
   #include <zlib.h>
#endif

#define ZSTREAM_BUFFER_SIZE 1024

//////////////////////
// wxZlibInputStream
//////////////////////

wxZlibInputStream::wxZlibInputStream(wxInputStream& stream)
  : wxFilterInputStream(stream)
{
  // I need a private stream buffer.
  m_inflate = new z_stream_s;

  m_inflate->zalloc = (alloc_func)0;
  m_inflate->zfree = (free_func)0;
  m_inflate->opaque = (voidpf)0;

  int err = inflateInit(m_inflate);
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

  delete [] m_z_buffer;
}

size_t wxZlibInputStream::OnSysRead(void *buffer, size_t size)
{
  int err;

  m_inflate->next_out = (unsigned char *)buffer;
  m_inflate->avail_out = size;

  while (m_inflate->avail_out > 0) {
    if (m_inflate->avail_in == 0) {

      m_parent_i_stream->Read(m_z_buffer, wxMin(m_z_size, size));
      m_inflate->next_in = m_z_buffer;
      m_inflate->avail_in = m_parent_i_stream->LastRead();

      if (m_parent_i_stream->LastError() != wxStream_NOERROR &&
          m_parent_i_stream->LastError() != wxStream_EOF)
      { 
        m_lasterror = m_parent_i_stream->LastError();
        return 0; // failed to read anything
      }

      if ( m_inflate->avail_in == 0 )
      {
          // EOF
          m_lasterror = wxStream_EOF;
          break;
      }
    }
    err = inflate(m_inflate, Z_FINISH);
    if (err == Z_STREAM_END)
      return (size - m_inflate->avail_out);
  }

  return size-m_inflate->avail_out;
}

//////////////////////
// wxZlibOutputStream
//////////////////////

wxZlibOutputStream::wxZlibOutputStream(wxOutputStream& stream, int level)
 : wxFilterOutputStream(stream)
{
  int err;

  m_deflate = new z_stream_s;

  m_deflate->zalloc = (alloc_func)0;
  m_deflate->zfree = (free_func)0;
  m_deflate->opaque = (voidpf)0;

  if (level == -1) level = Z_DEFAULT_COMPRESSION;
  wxASSERT_MSG(level >= 0 && level <= 9, wxT("wxZlibOutputStream compression level must be between 0 and 9!"));

  err = deflateInit(m_deflate, level);
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
    wxLogDebug( wxT("wxZlibOutputStream: an error occured while closing the stream.\n") );
    return;
  }

  deflateEnd(m_deflate);
  delete m_deflate;

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
  
