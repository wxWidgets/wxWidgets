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
#include <ctype.h>
#include <wx/stream.h>
#include <wx/datstrm.h>
#include <wx/objstrm.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// wxStreamBuffer
// ----------------------------------------------------------------------------

wxStreamBuffer::wxStreamBuffer(wxInputStream& i_stream)
  : m_buffer_start(NULL), m_buffer_end(NULL), m_buffer_pos(NULL),
    m_buffer_size(0), m_istream(&i_stream), m_ostream(NULL)
{
}

wxStreamBuffer::wxStreamBuffer(wxOutputStream& o_stream)
  : m_buffer_start(NULL), m_buffer_end(NULL), m_buffer_pos(NULL),
    m_buffer_size(0), m_istream(NULL), m_ostream(&o_stream)
{
}

wxStreamBuffer::~wxStreamBuffer()
{
  wxDELETEA(m_buffer_start);
}

void wxStreamBuffer::WriteBack(char c)
{
  if (m_ostream)
    return;

  // Assume that if we write "back" we have read a few bytes: so we have some
  // space.

  m_buffer_pos--;
  *m_buffer_pos = c;
}

void wxStreamBuffer::SetBufferIO(char *buffer_start, char *buffer_end)
{
  size_t ret;

  m_buffer_start = buffer_start;
  m_buffer_end   = buffer_end;

  m_buffer_size = m_buffer_end-m_buffer_start;

  if (m_istream) {
    ret = m_istream->DoRead(m_buffer_start, m_buffer_size);
    m_buffer_end = m_buffer_start + ret;
  }
  m_buffer_pos = m_buffer_start;
}

void wxStreamBuffer::SetBufferIO(size_t bufsize)
{
  char *b_start;

  wxDELETE(m_buffer_start);

  if (!bufsize) {
    m_buffer_start = NULL;
    m_buffer_end = NULL;
    m_buffer_pos = NULL;
    m_buffer_size = 0;
    return;
  }

  b_start = new char[bufsize];

  SetBufferIO(b_start, b_start + bufsize);
}

void wxStreamBuffer::ResetBuffer()
{
  if (m_istream)
    m_buffer_pos = m_buffer_end;
  else
    m_buffer_pos = m_buffer_start;
}

void wxStreamBuffer::Read(void *buffer, size_t size)
{
  wxASSERT(m_istream != NULL);

  // ------------------
  // Buffering disabled
  // ------------------

  if (!m_buffer_size) {
    m_istream->m_lastread = m_istream->DoRead(buffer, size);
    return;
  }

  // -----------------
  // Buffering enabled
  // -----------------
  size_t buf_left, orig_size = size;
  size_t read_ret;

  while (size > 0) {
    buf_left = m_buffer_end - m_buffer_pos; 

    // First case: the requested buffer is larger than the stream buffer,
    //             we split
    if (size > buf_left) {
      memcpy(buffer, m_buffer_pos, buf_left);
      size -= buf_left;
      buffer = (char *)buffer + buf_left; // ANSI C++ violation.

      read_ret = m_istream->DoRead(m_buffer_start, m_buffer_size);

      // Read failed: EOF
      if (read_ret == 0) {
        m_istream->m_lastread = orig_size-size;
        m_istream->m_eof = TRUE;
        m_buffer_pos = m_buffer_end = m_buffer_start;
        return;
      } else {
        m_buffer_end = m_buffer_start+read_ret;
        m_buffer_pos = m_buffer_start;
      }
    } else {

      // Second case: we just copy from the stream buffer.
      memcpy(buffer, m_buffer_pos, size);
      m_buffer_pos += size;
      break;
    }
  }
  m_istream->m_lastread = orig_size;
}

void wxStreamBuffer::Write(const void *buffer, size_t size)
{
  wxASSERT(m_ostream != NULL);

  // ------------------
  // Buffering disabled
  // ------------------

  if (!m_buffer_size) {
    m_ostream->m_lastwrite = m_ostream->DoWrite(buffer, size);
    return;
  }

  // ------------------
  // Buffering enabled
  // ------------------

  size_t buf_left, orig_size = size;
  size_t write_ret;

  while (size > 0) {
    buf_left = m_buffer_end - m_buffer_pos;

    // First case: the buffer to write is larger than the stream buffer,
    //             we split it
    if (size > buf_left) {
      memcpy(m_buffer_pos, buffer, buf_left);
      size -= buf_left;
      buffer = (char *)buffer + buf_left; // ANSI C++ violation.

      write_ret = m_ostream->DoWrite(m_buffer_start, m_buffer_size);
      if (write_ret != m_buffer_size) {
        m_ostream->m_bad = TRUE;
        m_ostream->m_lastwrite = orig_size-size;
        m_buffer_pos = m_buffer_end = m_buffer_start;
        return;
      }
      m_buffer_pos = m_buffer_start;

    } else {

      // Second case: just copy it in the stream buffer.

      memcpy(m_buffer_pos, buffer, size);
      m_buffer_pos += size;
      break;
    }
  }
  m_ostream->m_lastwrite = orig_size;
}

// ----------------------------------------------------------------------------
// wxInputStream
// ----------------------------------------------------------------------------

wxInputStream::wxInputStream()
{
  m_i_destroybuf = TRUE;
  m_i_streambuf = new wxStreamBuffer(*this);
  m_eof = FALSE;
}

wxInputStream::wxInputStream(wxStreamBuffer *buffer)
{
  m_i_destroybuf = FALSE;
  m_i_streambuf = buffer;
  m_eof = FALSE;
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

#define BUF_TEMP_SIZE 10000

wxInputStream& wxInputStream::Read(wxOutputStream& stream_out)
{
  char buf[BUF_TEMP_SIZE]; 
  size_t bytes_read = BUF_TEMP_SIZE;

  while (bytes_read == BUF_TEMP_SIZE && !stream_out.Bad()) {
    bytes_read = Read(buf, bytes_read).LastRead();

    stream_out.Write(buf, bytes_read);
  }
  return *this;
}

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

wxInputStream& wxInputStream::operator>>(short& i)
{
  long l;

  *this >> l;
  i = (short)l;
  return *this;
}

wxInputStream& wxInputStream::operator>>(int& i)
{
  long l;

  *this >> l;
  i = (short)l;
  return *this;
}

wxInputStream& wxInputStream::operator>>(long& i)
{
  /* I only implemented a simple integer parser */
  int c, sign;

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
  } else
    sign = 1;

  while (isdigit(c)) {
    i = i*10 + c;
    c = GetC();
  }

  i *= sign;

  return *this;
}

wxInputStream& wxInputStream::operator>>(float& f)
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
  } else
    sign = 1;

  while (isdigit(c)) {
    f = f*10 + c;
    c = GetC();
  }

  if (c == '.') {
    float f_multiplicator = 0.1;
    c = GetC();

    while (isdigit(c)) {
      f += c*f_multiplicator;
      f_multiplicator /= 10;
      c = GetC();
    }
  }

  f *= sign;

  return *this;
}

wxInputStream& wxInputStream::operator>>(wxObject *& obj)
{
  wxObjectInputStream obj_s(*this);
  obj = obj_s.LoadObject();
  return *this;
}

off_t wxInputStream::SeekI(off_t pos, wxSeekMode mode)
{
  off_t ret_off, diff, last_access;

  last_access = m_i_streambuf->GetLastAccess();

  switch (mode) {
  case wxFromStart:
    diff = DoTellInput() - pos;
    if ( diff < 0 || diff > last_access  ) {
      ret_off = DoSeekInput(pos, wxFromStart);
      m_i_streambuf->ResetBuffer();
      return ret_off;
    } else {
      m_i_streambuf->SetIntPosition(last_access - diff);
      return pos;
    }
  case wxFromCurrent:
    diff = pos + m_i_streambuf->GetIntPosition();

    if ( (diff > last_access) || (diff < 0) ) {
      ret_off = DoSeekInput(pos, wxFromCurrent);
      m_i_streambuf->ResetBuffer();
      return ret_off;
    } else {
      m_i_streambuf->SetIntPosition(diff);
      return pos;
    }
  case wxFromEnd:
    // Hard to compute: always seek to the requested position.
    ret_off = DoSeekInput(pos, wxFromEnd);
    m_i_streambuf->ResetBuffer();
    return ret_off;
  }
  return wxInvalidOffset;
}

off_t wxInputStream::TellI() const
{
  return DoTellInput() - m_i_streambuf->GetLastAccess() +
                         m_i_streambuf->GetIntPosition();
}

// ----------------------------------------------------------------------------
// wxOutputStream
// ----------------------------------------------------------------------------
wxOutputStream::wxOutputStream()
{
  m_o_destroybuf = TRUE;
  m_o_streambuf = new wxStreamBuffer(*this);
}

wxOutputStream::wxOutputStream(wxStreamBuffer *buffer)
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

off_t wxOutputStream::SeekO(off_t pos, wxSeekMode mode)
{
  off_t ret_off;

  switch (mode) {
  case wxFromStart:
    if ( (unsigned)abs (DoTellOutput()-pos) > m_o_streambuf->GetLastAccess()  ) {
      ret_off = DoSeekOutput(pos, wxFromStart);
      m_o_streambuf->ResetBuffer();
      return ret_off;
    } else {
      m_o_streambuf->SetIntPosition( DoTellOutput() - pos);
      return pos;
    }
  case wxFromCurrent:
    if ( ((unsigned)pos > m_o_streambuf->GetLastAccess()) || (pos < 0) ) {
      ret_off = DoSeekOutput(pos, wxFromCurrent);
      m_o_streambuf->ResetBuffer();
      return ret_off;
    } else {
      m_o_streambuf->SetIntPosition(pos);
      return pos;
    }
  case wxFromEnd:
    // Hard to compute: always seek to the requested position.
    ret_off = DoSeekOutput(pos, wxFromEnd);
    m_o_streambuf->ResetBuffer();
    return ret_off;
  }
  return wxInvalidOffset;
}

off_t wxOutputStream::TellO() const
{
  return DoTellOutput() - m_o_streambuf->GetLastAccess()
                        + m_o_streambuf->GetIntPosition();
}

void wxOutputStream::Sync()
{
  DoWrite(m_o_streambuf->GetBufferStart(), m_o_streambuf->GetIntPosition());

  m_o_streambuf->ResetBuffer();
}

wxOutputStream& wxOutputStream::operator<<(const char *string)
{
  return Write(string, strlen(string));
}

wxOutputStream& wxOutputStream::operator<<(wxString& string)
{
  return Write(string, string.Len());
}

wxOutputStream& wxOutputStream::operator<<(char c)
{
  return Write(&c, 1);
}

wxOutputStream& wxOutputStream::operator<<(short i)
{
  wxString strint;

  strint.Printf("%i", i);
  return Write(strint, strint.Len());
}

wxOutputStream& wxOutputStream::operator<<(int i)
{
  wxString strint;

  strint.Printf("%i", i);
  return Write(strint, strint.Len());
}

wxOutputStream& wxOutputStream::operator<<(long i)
{
  wxString strlong;

  strlong.Printf("%i", i);
  return Write((const char *)strlong, strlong.Len());
}

wxOutputStream& wxOutputStream::operator<<(double f)
{
  wxString strfloat;

  strfloat.Printf("%f", f);
  return Write(strfloat, strfloat.Len());
}

wxOutputStream& wxOutputStream::operator<<(wxObject& obj)
{
  wxObjectOutputStream obj_s(*this);
  obj_s.SaveObject(obj);
  return *this;
}

// ----------------------------------------------------------------------------
// wxFilterInputStream
// ----------------------------------------------------------------------------
wxFilterInputStream::wxFilterInputStream(wxInputStream& stream)
  : wxInputStream(NULL)
{
  m_parent_i_stream = &stream;
  m_i_streambuf = stream.InputStreamBuffer();
}

wxFilterInputStream::~wxFilterInputStream()
{
}

size_t wxFilterInputStream::DoRead(void *buffer, size_t size)
{
  return m_parent_i_stream->Read(buffer, size).LastRead();
}

off_t wxFilterInputStream::DoSeekInput(off_t pos, wxSeekMode mode)
{
  return m_parent_i_stream->SeekI(pos, mode);
}

off_t wxFilterInputStream::DoTellInput() const
{
  return m_parent_i_stream->TellI();
}


// ----------------------------------------------------------------------------
// wxFilterOutputStream
// ----------------------------------------------------------------------------
wxFilterOutputStream::wxFilterOutputStream(wxOutputStream& stream)
  : wxOutputStream(NULL)
{
  m_parent_o_stream = &stream;
  m_o_streambuf = stream.OutputStreamBuffer();
}

wxFilterOutputStream::~wxFilterOutputStream()
{
}

size_t wxFilterOutputStream::DoWrite(const void *buffer, size_t size)
{
  return m_parent_o_stream->Write(buffer, size).LastWrite();
}

off_t wxFilterOutputStream::DoSeekOutput(off_t pos, wxSeekMode mode)
{
  return m_parent_o_stream->SeekO(pos, mode);
}

off_t wxFilterOutputStream::DoTellOutput() const
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
  return stream.Write("\n", 1);
#endif
}
