/////////////////////////////////////////////////////////////////////////////
// Name:        mstream.h
// Purpose:     Memory stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef __WXMMSTREAM_H__
#define __WXMMSTREAM_H__

#include <wx/stream.h>

class wxMemoryStreamBase {
 protected:
  wxMemoryStreamBase();
  virtual ~wxMemoryStreamBase();

  bool ChangeBufferSize(size_t new_length);

 protected:
  bool m_persistent;
  size_t m_length;
  char *m_buffer;
  int m_iolimit;
};

class wxMemoryInputStream: virtual public wxMemoryStreamBase, public wxInputStream {
 public:
  wxMemoryInputStream(const char *data, size_t length);
  virtual ~wxMemoryInputStream();
 
  wxInputStream& Read(void *buffer, size_t size);
  off_t SeekI(off_t pos, wxSeekMode mode);
  off_t TellI() const { return m_position_i; }

  bool Eof() const { return m_eof; }
  size_t LastRead() const { return m_lastread; }

 protected:
  bool m_eof;
  off_t m_position_i;
  size_t m_lastread;
};

class wxMemoryOutputStream: virtual public wxMemoryStreamBase, public wxOutputStream {
 public:
  wxMemoryOutputStream(char *data = NULL, size_t length = 0);
  virtual ~wxMemoryOutputStream();

  wxOutputStream& Write(const void *buffer, size_t size);
  off_t SeekO(off_t pos, wxSeekMode mode);
  off_t TellO() const { return m_position_o; }

  bool Bad() const { return m_bad; }
  size_t LastWrite() const { return m_lastwrite; }

  char *GetData() { return m_buffer; }
  size_t GetLength() { return m_length; }

 protected:
  bool m_bad;
  off_t m_position_o;
  size_t m_lastwrite;
};

class wxMemoryStream: public wxMemoryInputStream, public wxMemoryOutputStream {
 public:
  wxMemoryStream(char *data = NULL, size_t length = 0);
  virtual ~wxMemoryStream();
};

#endif
