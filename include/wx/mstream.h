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

 protected:
 
  size_t DoRead(void *buffer, size_t size);
  off_t DoSeekInput(off_t pos, wxSeekMode mode);
  off_t DoTellInput() const { return m_position_i; }

 protected:
  off_t m_position_i;
};

class wxMemoryOutputStream: virtual public wxMemoryStreamBase, public wxOutputStream {
 public:
  wxMemoryOutputStream(char *data = NULL, size_t length = 0);
  virtual ~wxMemoryOutputStream();

  char *GetData() { Sync(); return m_buffer; }
  size_t GetLength() { Sync(); return m_length; }

 protected:

  size_t DoWrite(const void *buffer, size_t size);
  off_t DoSeekOutput(off_t pos, wxSeekMode mode);
  off_t DoTellOutput() const { return m_position_o; }

 protected:
  off_t m_position_o;
};

class wxMemoryStream: public wxMemoryInputStream, public wxMemoryOutputStream {
 public:
  wxMemoryStream(char *data = NULL, size_t length = 0);
  virtual ~wxMemoryStream();
};

#endif
