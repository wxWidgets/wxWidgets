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
#include <wx/stream.h>
#include <wx/datstrm.h>
#include <wx/objstrm.h>

#define BUF_TEMP_SIZE 10000

// ----------------------------------------------------------------------------
// wxStreamBuffer
// ----------------------------------------------------------------------------

#define CHECK_ERROR(err) \
   if (m_stream->m_lasterror == wxStream_NOERROR) \
     m_stream->m_lasterror = err

wxStreamBuffer::wxStreamBuffer(wxStreamBase& stream, BufMode mode)
  : m_buffer_start(NULL), m_buffer_end(NULL), m_buffer_pos(NULL),
    m_buffer_size(0), m_wback(NULL), m_wbacksize(0), m_wbackcur(0),
    m_fixed(TRUE), m_flushable(TRUE), m_stream(&stream),
    m_mode(mode), m_destroybuf(FALSE), m_destroystream(FALSE)
{
}

wxStreamBuffer::wxStreamBuffer(BufMode mode)
  : m_buffer_start(NULL), m_buffer_end(NULL), m_buffer_pos(NULL),
    m_buffer_size(0), m_wback(NULL), m_wbacksize(0), m_wbackcur(0),
    m_fixed(TRUE), m_flushable(FALSE), m_stream(NULL),
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
  m_wback = NULL;
  m_wbacksize = 0;
  m_wbackcur = 0;
}

wxStreamBuffer::~wxStreamBuffer()
{
  if (m_wback)
    free(m_wback);
  if (m_destroybuf)
    wxDELETEA(m_buffer_start);
  if (m_destroystream)
    delete m_stream;
}

size_t wxStreamBuffer::WriteBack(const char *buf, size_t bufsize)
{
  char *ptrback;

  if (m_mode != read)
    return 0;

  ptrback = AllocSpaceWBack(bufsize);
  if (!ptrback)
    return 0;

  memcpy(ptrback, buf, bufsize);
  return bufsize;
}

bool wxStreamBuffer::WriteBack(char c)
{
  char *ptrback;

  ptrback = AllocSpaceWBack(1);
  if (!ptrback)
    return FALSE;

  *ptrback = c;
  return TRUE;
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

char *wxStreamBuffer::AllocSpaceWBack(size_t needed_size)
{
  char *temp_b;

  m_wbacksize += needed_size;

  if (!m_wback)
    temp_b = (char *)malloc(m_wbacksize);
  else
    temp_b = (char *)realloc(m_wback, m_wbacksize);

  if (!temp_b)
    return NULL;
  m_wback = temp_b;
  
  return (char *)(m_wback+(m_wbacksize-needed_size));
}

size_t wxStreamBuffer::GetWBack(char *buf, size_t bsize)
{
  size_t s_toget = m_wbacksize-m_wbackcur;

  if (bsize < s_toget)
    s_toget = bsize;

  memcpy(buf, (m_wback+m_wbackcur), s_toget);

  m_wbackcur += s_toget;
  if (m_wbackcur == m_wbacksize) {
    free(m_wback);
    m_wback = (char *)NULL;
    m_wbacksize = 0;
    m_wbackcur = 0;
  }
    
  return s_toget;
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
  m_stream->m_lastcount = GetWBack((char *)buffer, size);
  size -= m_stream->m_lastcount;
  if (size == 0)
    return m_stream->m_lastcount;

  buffer = (void *)((char *)buffer+m_stream->m_lastcount);

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
        CHECK_ERROR(wxStream_READ_ERR);
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

  if (m_mode == read)
    return 0;

  while (bytes_count == BUF_TEMP_SIZE) {
    b_count2 = sbuf->Read(buf, bytes_count);
    bytes_count = Write(buf, b_count2);
    if (b_count2 > bytes_count)
      sbuf->WriteBack(buf+bytes_count, b_count2-bytes_count);
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
      ret_off = m_stream->OnSysSeek(pos, wxFromCurrent);
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
  : wxStreamBase()
{
  m_i_destroybuf = TRUE;
  m_i_streambuf = new wxStreamBuffer(*this, wxStreamBuffer::read);
}

wxInputStream::wxInputStream(wxStreamBuffer *buffer)
  : wxStreamBase()
{
  m_i_destroybuf = FALSE;
  m_i_streambuf = buffer;
}

wxInputStream::~wxInputStream()
{
  if (m_i_destroybuf)
    delete m_i_streambuf;
}

char wxInputStream::GetC()
{
  char c;
  m_i_streambuf->Read(&c, 1);
  return c;
}

wxInputStream& wxInputStream::Read(void *buffer, size_t size)
{
  m_i_streambuf->Read(buffer, size);
  // wxStreamBuffer sets all variables for us
  return *this;
}

char wxInputStream::Peek()
{
  m_i_streambuf->GetDataLeft();

  return *(m_i_streambuf->GetBufferPos());
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
  return m_i_streambuf->Seek(pos, mode);
}

off_t wxInputStream::TellI() const
{
  return m_i_streambuf->Tell();
}

// --------------------
// Overloaded operators
// --------------------

wxInputStream& wxInputStream::operator>>(wxString& line)
{
  wxDataInputStream s(*this);

  line = s.ReadLine();
  return *this;
}

wxInputStream& wxInputStream::operator>>(char& c)
{
  c = GetC();
  return *this;
}

wxInputStream& wxInputStream::operator>>(signed short& i)
{
  signed long l;

  *this >> l;
  i = (signed short)l;
  return *this;
}

wxInputStream& wxInputStream::operator>>(signed int& i)
{
  signed long l;

  *this >> l;
  i = (signed int)l;
  return *this;
}

wxInputStream& wxInputStream::operator>>(signed long& i)
{
  /* I only implemented a simple integer parser */
  int c; 
  int sign;

  while (isspace( c = GetC() ) )
     /* Do nothing */ ;

  i = 0;
  if (! (c == '-' || isdigit(c)) ) {
    InputStreamBuffer()->WriteBack(c);
    return *this;
  }

  if (c == '-') {
    sign = -1;
    c = GetC();
  } else if (c == '+') {
    sign = 1;
    c = GetC();
  } else {
    sign = 1;
  }

  while (isdigit(c)) {
    i = i*10 + (c - (int)'0');
    c = GetC();
  }

  i *= sign;

  return *this;
}

wxInputStream& wxInputStream::operator>>(unsigned short& i)
{
  unsigned long l;

  *this >> l;
  i = (unsigned short)l;
  return *this;
}

wxInputStream& wxInputStream::operator>>(unsigned int& i)
{
  unsigned long l;

  *this >> l;
  i = (unsigned int)l;
  return *this;
}

wxInputStream& wxInputStream::operator>>(unsigned long& i)
{
  /* I only implemented a simple integer parser */
  int c;

  while (isspace( c = GetC() ) )
     /* Do nothing */ ;

  i = 0;
  if (!isdigit(c)) {
    InputStreamBuffer()->WriteBack(c);
    return *this;
  }

  while (isdigit(c)) {
    i = i*10 + c - '0';
    c = GetC();
  }

  return *this;
}

wxInputStream& wxInputStream::operator>>(double& f)
{
  /* I only implemented a simple float parser */
  int c, sign;

  while (isspace( c = GetC() ) )
     /* Do nothing */ ;

  f = 0.0;
  if (! (c == '-' || isdigit(c)) ) {
    InputStreamBuffer()->WriteBack(c);
    return *this;
  }

  if (c == '-') {
    sign = -1;
    c = GetC();
  } else if (c == '+') {
    sign = 1;
    c = GetC();
  } else {
    sign = 1;
  }

  while (isdigit(c)) {
    f = f*10 + (c - '0');
    c = GetC();
  }

  if (c == '.') {
    double f_multiplicator = (double) 0.1;
    c = GetC();

    while (isdigit(c)) {
      f += (c-'0')*f_multiplicator;
      f_multiplicator /= 10;
      c = GetC();
    }
  }

  f *= sign;

  return *this;
}

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
  m_o_destroybuf = TRUE;
  m_o_streambuf = new wxStreamBuffer(*this, wxStreamBuffer::write);
}

wxOutputStream::wxOutputStream(wxStreamBuffer *buffer)
  : wxStreamBase()
{
  m_o_destroybuf = FALSE;
  m_o_streambuf = buffer;
}

wxOutputStream::~wxOutputStream()
{
  if (m_o_destroybuf)
    delete m_o_streambuf;
}

wxOutputStream& wxOutputStream::Write(const void *buffer, size_t size)
{
  m_o_streambuf->Write(buffer, size);
  return *this;
}

wxOutputStream& wxOutputStream::Write(wxInputStream& stream_in)
{
  stream_in.Read(*this);
  return *this;
}

off_t wxOutputStream::TellO() const
{
  return m_o_streambuf->Tell();
}

off_t wxOutputStream::SeekO(off_t pos, wxSeekMode mode)
{
  return m_o_streambuf->Seek(pos, mode);
}

void wxOutputStream::Sync()
{
  m_o_streambuf->FlushBuffer();
}

wxOutputStream& wxOutputStream::operator<<(const char *string)
{
  return Write(string, strlen(string));
}

wxOutputStream& wxOutputStream::operator<<(wxString& string)
{
#if wxUSE_UNICODE
  const wxWX2MBbuf buf = string.mb_str();
  return *this << buf;
#else
  return Write(string, string.Len());
#endif
}

wxOutputStream& wxOutputStream::operator<<(char c)
{
  return Write(&c, 1);
}

wxOutputStream& wxOutputStream::operator<<(signed short i)
{
  signed long l = (signed long)i;
  return *this << l;
}

wxOutputStream& wxOutputStream::operator<<(signed int i)
{
  signed long l = (signed long)i;
  return *this << l;
}

wxOutputStream& wxOutputStream::operator<<(signed long i)
{
  wxString strlong;
  strlong.Printf(_T("%ld"), i);
  return *this << strlong;
}

wxOutputStream& wxOutputStream::operator<<(unsigned short i)
{
  unsigned long l = (unsigned long)i;
  return *this << l;
}

wxOutputStream& wxOutputStream::operator<<(unsigned int i)
{
  unsigned long l = (unsigned long)i;
  return *this << l;
}

wxOutputStream& wxOutputStream::operator<<(unsigned long i)
{
  wxString strlong;
  strlong.Printf(_T("%lu"), i);
  return *this << strlong;
}

wxOutputStream& wxOutputStream::operator<<(double f)
{
  wxString strfloat;

  strfloat.Printf(_T("%f"), f);
  return *this << strfloat;
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
// wxFilterInputStream
// ----------------------------------------------------------------------------
wxFilterInputStream::wxFilterInputStream()
  : wxInputStream(NULL)
{
  // WARNING streambuf set to NULL !
}

wxFilterInputStream::wxFilterInputStream(wxInputStream& stream)
  : wxInputStream(stream.InputStreamBuffer())
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
  : wxOutputStream(NULL)
{
}

wxFilterOutputStream::wxFilterOutputStream(wxOutputStream& stream)
  : wxOutputStream(stream.OutputStreamBuffer())
{
  m_parent_o_stream = &stream;
}

wxFilterOutputStream::~wxFilterOutputStream()
{
}

// ----------------------------------------------------------------------------
// Some IOManip function
// ----------------------------------------------------------------------------

wxOutputStream& wxEndL(wxOutputStream& stream)
{
#ifdef __MSW__
  return stream.Write("\r\n", 2);
#else
  return stream.Write("\n", 1);
#endif
}

#endif
  // wxUSE_STREAMS
