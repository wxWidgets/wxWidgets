/////////////////////////////////////////////////////////////////////////////
// Name:        stream.h
// Purpose:     "wxWindows stream" base classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXSTREAM_H__
#define _WX_WXSTREAM_H__

#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>
#include "wx/object.h"
#include "wx/string.h"
#include "wx/filefn.h"  // for off_t, wxInvalidOffset and wxSeekMode

class WXDLLEXPORT wxInputStream;
class WXDLLEXPORT wxOutputStream;

typedef wxInputStream& (*__wxInputManip)(wxInputStream&);
typedef wxOutputStream& (*__wxOutputManip)(wxOutputStream&);

wxOutputStream& WXDLLEXPORT wxEndL(wxOutputStream& o_stream);

// ---------------------------------------------------------------------------
// Stream buffer
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxStreamBuffer {
 public:
  wxStreamBuffer(wxInputStream& stream);
  wxStreamBuffer(wxOutputStream& stream);
  ~wxStreamBuffer();

  void Read(void *buffer, size_t size);
  void Write(const void *buffer, size_t size);
  void WriteBack(char c);

  void SetBufferIO(char *buffer_start, char *buffer_end);
  void SetBufferIO(size_t bufsize);
  void ResetBuffer(); 

  void SetBufferPosition(char *buffer_position)
                                  { m_buffer_pos = buffer_position; }
  void SetIntPosition(size_t pos)
                     { m_buffer_pos = m_buffer_start + pos; }
  char *GetBufferPosition() const { return m_buffer_pos; }
  size_t GetIntPosition() const { return m_buffer_pos - m_buffer_start; }
  
  char *GetBufferStart() const    { return m_buffer_start; }
  char *GetBufferEnd() const      { return m_buffer_end; }
  size_t GetBufferSize() const    { return m_buffer_size; }
  size_t GetLastAccess() const      { return m_buffer_end - m_buffer_start; }

 protected:
  char *m_buffer_start, *m_buffer_end, *m_buffer_pos;
  size_t m_buffer_size;

  wxInputStream *m_istream;
  wxOutputStream *m_ostream;
};

// ---------------------------------------------------------------------------
// wxStream: base classes
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxInputStream {
 public:
  wxInputStream();
  virtual ~wxInputStream();

  // IO functions
  virtual char Peek() { return 0; }
  virtual char GetC();
  virtual wxInputStream& Read(void *buffer, size_t size);
  wxInputStream& Read(wxOutputStream& stream_out);

  // Position functions
  virtual off_t SeekI(off_t pos, wxSeekMode mode = wxFromStart);
  virtual off_t TellI() const;

  // State functions
  bool Eof() const { return m_eof; }
  size_t LastRead() { return m_lastread; }
  wxStreamBuffer *InputStreamBuffer() { return m_i_streambuf; }

  // Operators
  wxInputStream& operator>>(wxOutputStream& out) { return Read(out); }
  wxInputStream& operator>>(wxString& line);
  wxInputStream& operator>>(char& c);
  wxInputStream& operator>>(short& i);
  wxInputStream& operator>>(int& i);
  wxInputStream& operator>>(long& i);
  wxInputStream& operator>>(float& i);
#if USE_SERIAL
  wxInputStream& operator>>(wxObject *& obj);
#endif

  wxInputStream& operator>>(unsigned char& c) { return operator>>((char&)c); }
  wxInputStream& operator>>(unsigned short& i) { return operator>>((short&)i); }
  wxInputStream& operator>>(unsigned int& i) { return operator>>((int&)i); }
  wxInputStream& operator>>(unsigned long& i) { return operator>>((long&)i); }
  wxInputStream& operator>>( __wxInputManip func) { return func(*this); }

 protected:
  friend class wxStreamBuffer;
  friend class wxFilterInputStream;

  wxInputStream(wxStreamBuffer *buffer);

  virtual size_t DoRead(void *WXUNUSED(buffer), size_t WXUNUSED(size) ) 
         { return 0; }
  virtual off_t DoSeekInput( off_t WXUNUSED(pos), wxSeekMode WXUNUSED(mode) )
         { return wxInvalidOffset; }
  virtual off_t DoTellInput() const
         { return wxInvalidOffset; }

 protected:
  bool m_eof, m_i_destroybuf;
  size_t m_lastread;
  wxStreamBuffer *m_i_streambuf;
};

class WXDLLEXPORT wxOutputStream {
 public:
  wxOutputStream();
  virtual ~wxOutputStream();

  virtual wxOutputStream& Write(const void *buffer, size_t size);
  wxOutputStream& Write(wxInputStream& stream_in);

  virtual off_t SeekO(off_t pos, wxSeekMode mode = wxFromStart);
  virtual off_t TellO() const;

  virtual bool Bad() const { return m_bad; }
  virtual size_t LastWrite() const { return m_lastwrite; }
  wxStreamBuffer *OutputStreamBuffer() { return m_o_streambuf; }

  virtual void Sync();

  wxOutputStream& operator<<(wxInputStream& out) { return Write(out); }
  wxOutputStream& operator<<(const char *string);
  wxOutputStream& operator<<(wxString& string);
  wxOutputStream& operator<<(char c);
  wxOutputStream& operator<<(short i);
  wxOutputStream& operator<<(int i);
  wxOutputStream& operator<<(long i);
  wxOutputStream& operator<<(double f);
#if USE_SERIAL
  wxOutputStream& operator<<(wxObject& obj);
#endif

  wxOutputStream& operator<<(float f) { return operator<<((double)f); }
  wxOutputStream& operator<<(unsigned char c) { return operator<<((char)c); }
  wxOutputStream& operator<<(unsigned short i) { return operator<<((short)i); }
  wxOutputStream& operator<<(unsigned int i) { return operator<<((int)i); }
  wxOutputStream& operator<<(unsigned long i) { return operator<<((long)i); }
  wxOutputStream& operator<<( __wxOutputManip func) { return func(*this); }

 protected:
  friend class wxStreamBuffer;
  friend class wxFilterOutputStream;

  wxOutputStream(wxStreamBuffer *buffer);

  virtual size_t DoWrite( const void *WXUNUSED(buffer), size_t WXUNUSED(size) ) 
            { return 0; }
  virtual off_t DoSeekOutput( off_t WXUNUSED(pos), wxSeekMode WXUNUSED(mode) )
            { return wxInvalidOffset; }
  virtual off_t DoTellOutput() const
            { return wxInvalidOffset; }

 protected:
  bool m_bad, m_o_destroybuf;
  size_t m_lastwrite;
  wxStreamBuffer *m_o_streambuf;
};

class wxStream: virtual public wxInputStream,
		virtual public wxOutputStream
{
 public:
  wxStream();
};

// ---------------------------------------------------------------------------
// "Filter" streams
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxFilterInputStream: virtual public wxInputStream {
 public:
  wxFilterInputStream();
  wxFilterInputStream(wxInputStream& stream);
  ~wxFilterInputStream();

  char Peek() { return m_parent_i_stream->Peek(); }

  bool Eof() const { return m_parent_i_stream->Eof(); } 
  size_t LastRead() const { return m_parent_i_stream->LastRead(); } 
  off_t TellI() const { return m_parent_i_stream->TellI(); }

 protected:
  size_t DoRead(void *buffer, size_t size);
  off_t DoSeekInput(off_t pos, wxSeekMode mode);
  off_t DoTellInput() const;

 protected:
  wxInputStream *m_parent_i_stream;
};

class WXDLLEXPORT wxFilterOutputStream: virtual public wxOutputStream {
 public:
  wxFilterOutputStream();
  wxFilterOutputStream(wxOutputStream& stream);
  virtual ~wxFilterOutputStream();

  bool Bad() const { return m_parent_o_stream->Bad(); }
  size_t LastWrite() const { return m_parent_o_stream->LastWrite(); }
  off_t TellO() const { return m_parent_o_stream->TellO(); }

 protected:
  // The forward is implicitely done by wxStreamBuffer.

  size_t DoWrite(const void *buffer, size_t size);
  off_t DoSeekOutput(off_t pos, wxSeekMode mode);
  off_t DoTellOutput() const;

 protected:
  wxOutputStream *m_parent_o_stream;
};

class WXDLLEXPORT wxFilterStream: public wxStream,
				  virtual public wxFilterInputStream,
				  virtual public wxFilterOutputStream {
 public:
  wxFilterStream(wxStream& stream);
  wxFilterStream();
};

#endif
