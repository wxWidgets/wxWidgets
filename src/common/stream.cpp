/////////////////////////////////////////////////////////////////////////////
// Name:        stream.cpp
// Purpose:     wxStream base classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#if wxUSE_STREAMS

#include <ctype.h>
#include "wx/stream.h"
#include "wx/datstrm.h"
#include "wx/objstrm.h"

#define BUF_TEMP_SIZE 10000

// ----------------------------------------------------------------------------
// wxStreamBuffer
// ----------------------------------------------------------------------------

#define CHECK_ERROR(err) \
   if (m_stream->m_lasterror == wxStream_NOERROR) \
     m_stream->m_lasterror = err

wxStreamBuffer::wxStreamBuffer(wxStreamBase& stream, BufMode mode)
  : m_buffer_start(NULL), m_buffer_end(NULL), m_buffer_pos(NULL),
    m_buffer_size(0), m_fixed(TRUE), m_flushable(TRUE), m_stream(&stream),
    m_mode(mode), m_destroybuf(FALSE), m_destroystream(FALSE)
{
}

wxStreamBuffer::wxStreamBuffer(BufMode mode)
  : m_buffer_start(NULL), m_buffer_end(NULL), m_buffer_pos(NULL),
    m_buffer_size(0), m_fixed(TRUE), m_flushable(FALSE), m_stream(NULL),
    m_mode(mode), m_destroybuf(FALSE), m_destroystream(TRUE)
{
  m_stream = new wxStreamBase();
}

wxStreamBuffer::wxStreamBuffer(const wxStreamBuffer& buffer)
{
  m_buffer_start = buffer.m_buffer_start;
  m_buffer_end = buffer.m_buffer_end;
  m_buffer_pos = buffer.m_buffer_pos;
  m_buffer_size = buffer.m_buffer_size;
  m_fixed = buffer.m_fixed;
  m_flushable = buffer.m_flushable;
  m_stream = buffer.m_stream;
  m_mode = buffer.m_mode;
  m_destroybuf = FALSE;
  m_destroystream = FALSE;
}

wxStreamBuffer::~wxStreamBuffer()
{
  if (m_destroybuf)
    wxDELETEA(m_buffer_start);
  if (m_destroystream)
    delete m_stream;
}

void wxStreamBuffer::SetBufferIO(char *buffer_start, char *buffer_end)
{
  if (m_destroybuf)
    wxDELETEA(m_buffer_start);
  m_buffer_start = buffer_start;
  m_buffer_end   = buffer_end;

  m_buffer_size = m_buffer_end-m_buffer_start;
  m_destroybuf = FALSE;
  ResetBuffer();
}

void wxStreamBuffer::SetBufferIO(size_t bufsize)
{
  char *b_start;

  if (m_destroybuf)
    wxDELETEA(m_buffer_start);

  if (!bufsize) {
    m_buffer_start = NULL;
    m_buffer_end = NULL;
    m_buffer_pos = NULL;
    m_buffer_size = 0;
    return;
  }

  b_start = new char[bufsize];
  SetBufferIO(b_start, b_start + bufsize);
  m_destroybuf = TRUE;
}

void wxStreamBuffer::ResetBuffer()
{
  m_stream->m_lasterror = wxStream_NOERROR;
  m_stream->m_lastcount = 0;
  if (m_mode == read)
    m_buffer_pos = m_buffer_end;
  else
    m_buffer_pos = m_buffer_start;
}

bool wxStreamBuffer::FillBuffer()
{
  size_t count;

  count = m_stream->OnSysRead(m_buffer_start, m_buffer_size);
  m_buffer_end = m_buffer_start+count;
  m_buffer_pos = m_buffer_start;

  if (count == 0)
    return FALSE;
  return TRUE;
}

bool wxStreamBuffer::FlushBuffer()
{
  size_t count, current;

  if (m_buffer_pos == m_buffer_start || !m_flushable)
    return FALSE;

  current = m_buffer_pos-m_buffer_start;
  count = m_stream->OnSysWrite(m_buffer_start, current);
  if (count != current)
    return FALSE;
  m_buffer_pos = m_buffer_start;

  return TRUE;
}

void wxStreamBuffer::GetFromBuffer(void *buffer, size_t size)
{
  size_t s_toget = m_buffer_end-m_buffer_pos;

  if (size < s_toget)
    s_toget = size;

  memcpy(buffer, m_buffer_pos, s_toget);
  m_buffer_pos += s_toget;
}

void wxStreamBuffer::PutToBuffer(const void *buffer, size_t size)
{
  size_t s_toput = m_buffer_end-m_buffer_pos;

  if (s_toput < size && !m_fixed) {
    m_buffer_start = (char *)realloc(m_buffer_start, m_buffer_size+size);
    // I round a bit
    m_buffer_size += size;
    m_buffer_end = m_buffer_start+m_buffer_size;
    s_toput = size;
  }
  if (s_toput > size)
    s_toput = size;
  memcpy(m_buffer_pos, buffer, s_toput);
  m_buffer_pos += s_toput;
}

void wxStreamBuffer::PutChar(char c)
{
  wxASSERT(m_stream != NULL);

  if (!m_buffer_size) {
    m_stream->OnSysWrite(&c, 1);
    return;
  }

  if (GetDataLeft() == 0 && !FlushBuffer()) {
    CHECK_ERROR(wxStream_WRITE_ERR);
    return;
  }

  PutToBuffer(&c, 1);
  m_stream->m_lastcount = 1;
}

char wxStreamBuffer::Peek()
{
  char c;

  wxASSERT(m_stream != NULL && m_buffer_size != 0);

  if (!GetDataLeft()) {
    CHECK_ERROR(wxStream_READ_ERR);
    return 0;
  }

  GetFromBuffer(&c, 1);
  m_buffer_pos--;

  return c;
}

char wxStreamBuffer::GetChar()
{
  char c;

  wxASSERT(m_stream != NULL);

  if (!m_buffer_size) {
    m_stream->OnSysRead(&c, 1);
    return c;
  }

  if (!GetDataLeft()) {
    CHECK_ERROR(wxStream_READ_ERR);
    return 0;
  }

  GetFromBuffer(&c, 1);
  
  m_stream->m_lastcount = 1;
  return c;
}

size_t wxStreamBuffer::Read(void *buffer, size_t size)
{
  wxASSERT(m_stream != NULL);

  if (m_mode == write)
    return 0;

  // ------------------
  // Buffering disabled
  // ------------------

  m_stream->m_lasterror = wxStream_NOERROR;
  if (!m_buffer_size)
    return (m_stream->m_lastcount += m_stream->OnSysRead(buffer, size));

  // -----------------
  // Buffering enabled
  // -----------------
  size_t buf_left, orig_size = size;

  while (size > 0) {
    buf_left = GetDataLeft(); 

    // First case: the requested buffer is larger than the stream buffer,
    //             we split it.
    if (size > buf_left) {
      GetFromBuffer(buffer, buf_left);
      size  -= buf_left;
      buffer = (char *)buffer + buf_left; // ANSI C++ violation.

      if (!FillBuffer()) {
        CHECK_ERROR(wxStream_EOF);
        return (m_stream->m_lastcount = orig_size-size);
      }
    } else {

      // Second case: we just copy from the stream buffer.
      GetFromBuffer(buffer, size);
      break;
    }
  }
  return (m_stream->m_lastcount += orig_size);
}

size_t wxStreamBuffer::Read(wxStreamBuffer *s_buf)
{
  char buf[BUF_TEMP_SIZE];
  size_t s = 0, bytes_read = BUF_TEMP_SIZE;

  if (m_mode == write)
    return 0;

  while (bytes_read != 0) {
    bytes_read = Read(buf, bytes_read);
    bytes_read = s_buf->Write(buf, bytes_read);
    s += bytes_read;
  }
  return s;
}

size_t wxStreamBuffer::Write(const void *buffer, size_t size)
{
  wxASSERT(m_stream != NULL);

  if (m_mode == read)
    return 0;

  // ------------------
  // Buffering disabled
  // ------------------

  m_stream->m_lasterror = wxStream_NOERROR;
  if (!m_buffer_size)
    return (m_stream->m_lastcount = m_stream->OnSysWrite(buffer, size));

  // ------------------
  // Buffering enabled
  // ------------------

  size_t buf_left, orig_size = size;

  while (size > 0) {
    buf_left = m_buffer_end - m_buffer_pos;

    // First case: the buffer to write is larger than the stream buffer,
    //             we split it
    if (size > buf_left) {
      PutToBuffer(buffer, buf_left);
      size -= buf_left;
      buffer = (char *)buffer + buf_left; // ANSI C++ violation.

      if (!FlushBuffer()) {
	CHECK_ERROR(wxStream_WRITE_ERR);
        return (m_stream->m_lastcount = orig_size-size);
      }

      m_buffer_pos = m_buffer_start;

    } else {

      // Second case: just copy it in the stream buffer.
      PutToBuffer(buffer, size);
      break;
    }
  }
  return (m_stream->m_lastcount = orig_size);
}

size_t wxStreamBuffer::Write(wxStreamBuffer *sbuf)
{
  char buf[BUF_TEMP_SIZE];
  size_t s = 0, bytes_count = BUF_TEMP_SIZE, b_count2;
  wxInputStream *in_stream;

  if (m_mode == read)
    return 0;

  in_stream = (wxInputStream *)sbuf->Stream();

  while (bytes_count == BUF_TEMP_SIZE) {
    b_count2 = sbuf->Read(buf, bytes_count);
    bytes_count = Write(buf, b_count2);
    if (b_count2 > bytes_count)
      in_stream->Ungetch(buf+bytes_count, b_count2-bytes_count);
    s += bytes_count;
  }
  return s;
}

off_t wxStreamBuffer::Seek(off_t pos, wxSeekMode mode)
{
  off_t ret_off, diff, last_access;

  last_access = GetLastAccess();

  if (!m_flushable) {
    diff = pos + GetIntPosition();
    if (diff < 0 || diff > last_access)
      return wxInvalidOffset;
    SetIntPosition(diff);
    return diff;
  }

  switch (mode) {
  case wxFromStart: {
    // We'll try to compute an internal position later ...
    ret_off = m_stream->OnSysSeek(pos, wxFromStart);
    ResetBuffer();
    return ret_off;
  }
  case wxFromCurrent: {
    diff = pos + GetIntPosition();

    if ( (diff > last_access) || (diff < 0) ) {
      // We must take into account the fact that we have read something
      // previously.
      ret_off = m_stream->OnSysSeek(diff-last_access, wxFromCurrent);
      ResetBuffer();
      return ret_off;
    } else {
      SetIntPosition(diff);
      return pos;
    }
  }
  case wxFromEnd:
    // Hard to compute: always seek to the requested position.
    ret_off = m_stream->OnSysSeek(pos, wxFromEnd);
    ResetBuffer();
    return ret_off;
  }
  return wxInvalidOffset;
}

off_t wxStreamBuffer::Tell() const
{
  off_t pos;

  if (m_flushable) {
    pos = m_stream->OnSysTell();
    if (pos == wxInvalidOffset)
      return wxInvalidOffset;
    return pos - GetLastAccess() + GetIntPosition();
  } else
    return GetIntPosition();
}

size_t wxStreamBuffer::GetDataLeft()
{
  if (m_buffer_end == m_buffer_pos && m_flushable)
    FillBuffer();
  return m_buffer_end-m_buffer_pos;
}

// ----------------------------------------------------------------------------
// wxStreamBase
// ----------------------------------------------------------------------------

wxStreamBase::wxStreamBase()
{
  m_lasterror = wxStream_NOERROR;
  m_lastcount = 0;
}

wxStreamBase::~wxStreamBase()
{
}

size_t wxStreamBase::OnSysRead(void *WXUNUSED(buffer), size_t WXUNUSED(size))
{
  return 0;
}

size_t wxStreamBase::OnSysWrite(const void *WXUNUSED(buffer), size_t WXUNUSED(bufsize))
{
  return 0;
}

off_t wxStreamBase::OnSysSeek(off_t WXUNUSED(seek), wxSeekMode WXUNUSED(mode))
{
  return wxInvalidOffset;
}

off_t wxStreamBase::OnSysTell() const
{
  return wxInvalidOffset;
}

// ----------------------------------------------------------------------------
// wxInputStream
// ----------------------------------------------------------------------------

wxInputStream::wxInputStream()
  : wxStreamBase(),
    m_wback(NULL), m_wbacksize(0), m_wbackcur(0)
{
}

wxInputStream::~wxInputStream()
{
  if (m_wback)
    free(m_wback);
}

char *wxInputStream::AllocSpaceWBack(size_t needed_size)
{
  char *temp_b;
  size_t old_size;

  old_size = m_wbacksize;
  m_wbacksize += needed_size;

  if (!m_wback)
    temp_b = (char *)malloc(m_wbacksize);
  else
    temp_b = (char *)realloc(m_wback, m_wbacksize);

  if (!temp_b)
    return NULL;
  m_wback = temp_b;
  m_wbackcur += needed_size;

  memmove(m_wback+needed_size, m_wback, old_size);
  
  return (char *)(m_wback);
}

size_t wxInputStream::GetWBack(char *buf, size_t bsize)
{
  size_t s_toget = m_wbackcur;

  if (!m_wback)
    return 0;

  if (bsize < s_toget)
    s_toget = bsize;

  memcpy(buf, (m_wback+m_wbackcur-bsize), s_toget);

  m_wbackcur -= s_toget;
  if (m_wbackcur == 0) {
    free(m_wback);
    m_wback = (char *)NULL;
    m_wbacksize = 0;
    m_wbackcur = 0;
  }
    
  return s_toget;
}

size_t wxInputStream::Ungetch(const void *buf, size_t bufsize)
{
  char *ptrback;

  ptrback = AllocSpaceWBack(bufsize);
  if (!ptrback)
    return 0;

  memcpy(ptrback, buf, bufsize);
  return bufsize;
}

bool wxInputStream::Ungetch(char c)
{
  char *ptrback;

  ptrback = AllocSpaceWBack(1);
  if (!ptrback)
    return FALSE;

  *ptrback = c;
  return TRUE;
}

char wxInputStream::GetC()
{
  char c;
  Read(&c, 1);
  return c;
}

wxInputStream& wxInputStream::Read(void *buffer, size_t size)
{
  size_t retsize;
  char *buf = (char *)buffer;

  retsize = GetWBack(buf, size);
  if (retsize == size) {
    m_lastcount = size;
    m_lasterror = wxStream_NOERROR;
    return *this;
  }
  size     -= retsize;
  buf      += retsize;

  m_lastcount = OnSysRead(buf, size);
  return *this;
}

char wxInputStream::Peek()
{
  char c;

  Read(&c, 1);
  if (m_lasterror == wxStream_NOERROR) {
    Ungetch(c);
    return c;
  }
  return 0;
}

wxInputStream& wxInputStream::Read(wxOutputStream& stream_out)
{
  char buf[BUF_TEMP_SIZE]; 
  size_t bytes_read = BUF_TEMP_SIZE;

  while (bytes_read == BUF_TEMP_SIZE) {
    bytes_read = Read(buf, bytes_read).LastRead();
    bytes_read = stream_out.Write(buf, bytes_read).LastWrite();
  }
  return *this;
}

off_t wxInputStream::SeekI(off_t pos, wxSeekMode mode)
{
  //should be check and improve, just to remove a slight bug !
  // I don't know whether it should be put as well in wxFileInputStream::OnSysSeek ?
  if (m_lasterror==wxSTREAM_EOF) m_lasterror=wxSTREAM_NOERROR;

  return OnSysSeek(pos, mode);
}

off_t wxInputStream::TellI() const
{
  return OnSysTell();
}

// --------------------
// Overloaded operators
// --------------------

#if wxUSE_SERIAL
wxInputStream& wxInputStream::operator>>(wxObject *& obj)
{
  wxObjectInputStream obj_s(*this);
  obj = obj_s.LoadObject();
  return *this;
}
#endif


// ----------------------------------------------------------------------------
// wxOutputStream
// ----------------------------------------------------------------------------
wxOutputStream::wxOutputStream()
  : wxStreamBase()
{
}

wxOutputStream::~wxOutputStream()
{
}

wxOutputStream& wxOutputStream::Write(const void *buffer, size_t size)
{
  m_lastcount = OnSysWrite(buffer, size);
  return *this;
}

wxOutputStream& wxOutputStream::Write(wxInputStream& stream_in)
{
  stream_in.Read(*this);
  return *this;
}

off_t wxOutputStream::TellO() const
{
  return OnSysTell();
}

off_t wxOutputStream::SeekO(off_t pos, wxSeekMode mode)
{
  return OnSysSeek(pos, mode);
}

void wxOutputStream::Sync()
{
}

#if wxUSE_SERIAL
wxOutputStream& wxOutputStream::operator<<(wxObject& obj)
{
  wxObjectOutputStream obj_s(*this);
  obj_s.SaveObject(obj);
  return *this;
}
#endif

// ----------------------------------------------------------------------------
// wxCountingOutputStream
// ----------------------------------------------------------------------------

wxCountingOutputStream::wxCountingOutputStream ()
  : wxOutputStream()
{
   m_currentPos = 0;
}

size_t wxCountingOutputStream::GetSize() const
{
  return m_lastcount;
}

size_t wxCountingOutputStream::OnSysWrite(const void *WXUNUSED(buffer), size_t size)
{
  m_currentPos += size;
  if (m_currentPos > m_lastcount) m_lastcount = m_currentPos;
  return m_currentPos;
}

off_t wxCountingOutputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
  if (mode == wxFromStart)
  {
    m_currentPos = pos;
  }
  if (mode == wxFromEnd)
  {
    m_currentPos = m_lastcount + pos;
  }
  else
  {
    m_currentPos += pos;
  }
  if (m_currentPos > m_lastcount) m_lastcount = m_currentPos;
  
  return m_currentPos;  // ?
}

off_t wxCountingOutputStream::OnSysTell() const
{
  return m_currentPos;  // ?
}
  
// ----------------------------------------------------------------------------
// wxFilterInputStream
// ----------------------------------------------------------------------------

wxFilterInputStream::wxFilterInputStream()
  : wxInputStream()
{
}

wxFilterInputStream::wxFilterInputStream(wxInputStream& stream)
  : wxInputStream()
{
  m_parent_i_stream = &stream;
}

wxFilterInputStream::~wxFilterInputStream()
{
}

// ----------------------------------------------------------------------------
// wxFilterOutputStream
// ----------------------------------------------------------------------------
wxFilterOutputStream::wxFilterOutputStream()
  : wxOutputStream()
{
}

wxFilterOutputStream::wxFilterOutputStream(wxOutputStream& stream)
  : wxOutputStream()
{
  m_parent_o_stream = &stream;
}

wxFilterOutputStream::~wxFilterOutputStream()
{
}

// ----------------------------------------------------------------------------
// wxBufferedInputStream
// ----------------------------------------------------------------------------
wxBufferedInputStream::wxBufferedInputStream(wxInputStream& s)
  : wxFilterInputStream(s)
{
  m_i_streambuf = new wxStreamBuffer(*this, wxStreamBuffer::read);
  m_i_streambuf->SetBufferIO(1024);
}

wxBufferedInputStream::~wxBufferedInputStream()
{
  delete m_i_streambuf;
}

char wxBufferedInputStream::Peek()
{
  return m_i_streambuf->Peek();
}

wxInputStream& wxBufferedInputStream::Read(void *buffer, size_t size)
{
  size_t retsize;
  char *buf = (char *)buffer;

  retsize = GetWBack(buf, size);
  m_lastcount = retsize;
  if (retsize == size) {
    m_lasterror = wxStream_NOERROR;
    return *this;
  }
  size     -= retsize;
  buf      += retsize;

  m_i_streambuf->Read(buf, size);

  return *this;
}

off_t wxBufferedInputStream::SeekI(off_t pos, wxSeekMode mode)
{
  return m_i_streambuf->Seek(pos, mode);
}

off_t wxBufferedInputStream::TellI() const
{
  return m_i_streambuf->Tell();
}

size_t wxBufferedInputStream::OnSysRead(void *buffer, size_t bufsize)
{
  return m_parent_i_stream->Read(buffer, bufsize).LastRead();
}

off_t wxBufferedInputStream::OnSysSeek(off_t seek, wxSeekMode mode)
{
  return m_parent_i_stream->SeekI(seek, mode);
}

off_t wxBufferedInputStream::OnSysTell() const
{
  return m_parent_i_stream->TellI();
}

// ----------------------------------------------------------------------------
// wxBufferedOutputStream
// ----------------------------------------------------------------------------

wxBufferedOutputStream::wxBufferedOutputStream(wxOutputStream& s)
  : wxFilterOutputStream(s)
{
  m_o_streambuf = new wxStreamBuffer(*this, wxStreamBuffer::write);
  m_o_streambuf->SetBufferIO(1024);
}

wxBufferedOutputStream::~wxBufferedOutputStream()
{
  delete m_o_streambuf;
}

wxOutputStream& wxBufferedOutputStream::Write(const void *buffer, size_t size)
{
  m_lastcount = 0;
  m_o_streambuf->Write(buffer, size);
  return *this;
}

off_t wxBufferedOutputStream::SeekO(off_t pos, wxSeekMode mode)
{
  return m_o_streambuf->Seek(pos, mode);
}

off_t wxBufferedOutputStream::TellO() const
{
  return m_o_streambuf->Tell();
}

void wxBufferedOutputStream::Sync()
{
  m_o_streambuf->FlushBuffer();
  m_parent_o_stream->Sync();
}

size_t wxBufferedOutputStream::OnSysWrite(const void *buffer, size_t bufsize)
{
  return m_parent_o_stream->Write(buffer, bufsize).LastWrite();
}

off_t wxBufferedOutputStream::OnSysSeek(off_t seek, wxSeekMode mode)
{
  return m_parent_o_stream->SeekO(seek, mode);
}

off_t wxBufferedOutputStream::OnSysTell() const
{
  return m_parent_o_stream->TellO();
}

// ----------------------------------------------------------------------------
// Some IOManip function
// ----------------------------------------------------------------------------

wxOutputStream& wxEndL(wxOutputStream& stream)
{
#ifdef __MSW__
  return stream.Write("\r\n", 2);
#else
#ifdef __WXMAC__
  return stream.Write("\r", 1);
#else
  return stream.Write("\n", 1);
#endif
#endif
}

#endif
  // wxUSE_STREAMS
