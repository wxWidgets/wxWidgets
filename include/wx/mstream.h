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

class wxMemoryStreamBase: public wxStream {
  DECLARE_CLASS(wxMemoryStreamBase)
 public:
  wxMemoryStreamBase(char *data, size_t length, int iolimit);
  virtual ~wxMemoryStreamBase();

  // Input part
  wxInputStream& Read(void *buffer, size_t size);
  size_t SeekI(int pos, wxWhenceType whence = wxBeginPosition);
  size_t TellI() const { return m_position_i; }

  bool Eof() const { return m_eof; }
  size_t LastRead() const { return m_lastread; }

  // Output part
  wxOutputStream& Write(const void *buffer, size_t size);
  size_t SeekO(int pos, wxWhenceType whence = wxBeginPosition);
  size_t TellO() const { return m_position_o; }

  bool Bad() const { return m_bad; }
  size_t LastWrite() const { return m_lastwrite; }
  void Sync() {}

 protected:
  bool ChangeBufferSize(size_t new_length);

 protected:
  bool m_bad, m_eof, m_persistent;
  size_t m_lastread, m_lastwrite;
  size_t m_length;
  size_t m_position_i, m_position_o;
  char *m_buffer;
  int m_iolimit;
};


class wxMemoryInputStream: public wxMemoryStreamBase {
  DECLARE_CLASS(wxMemoryInputStream)
 public:
  wxMemoryInputStream(char *data, size_t length)
     : wxMemoryStreamBase(data, length, 1)
    {}
};

class wxMemoryOutputStream: public wxMemoryStreamBase {
  DECLARE_DYNAMIC_CLASS(wxMemoryOutputStream)
 public:
  wxMemoryOutputStream(char *data = NULL, size_t length = 0)
     : wxMemoryStreamBase(data, length, 2)
   {}
};

class wxMemoryStream: public wxMemoryStreamBase {
  DECLARE_DYNAMIC_CLASS(wxMemoryStream)
 public:
  wxMemoryStream(char *data = NULL, size_t length = 0)
     : wxMemoryStreamBase(data, length, 0)
   {}
};

#endif
