/////////////////////////////////////////////////////////////////////////////
// Name:        fstream.h
// Purpose:     File stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef __WXFSTREAM_H__
#define __WXFSTREAM_H__

#include <stdio.h>
#include <wx/object.h>
#include <wx/string.h>
#include <wx/stream.h>

class wxFileStreamBase: public wxStream {
  DECLARE_CLASS(wxFileStreamBase)
 public:
  wxFileStreamBase(const wxString& fileName, int iolimit);
  virtual ~wxFileStreamBase();

  wxInputStream& Read(void *buffer, size_t size);
  size_t SeekI(int pos, wxWhenceType whence = wxBeginPosition);
  size_t TellI() const;

  bool Eof() const { return m_eof; }
  size_t LastRead() const { return m_lastread; }

  wxOutputStream& Write(const void *buffer, size_t size);
  size_t SeekO(int pos, wxWhenceType whence = wxBeginPosition);
  size_t TellO() const;

  bool Bad() const { return m_bad; }
  size_t LastWrite() const { return m_lastwrite; }

  void Sync();

 protected:
  size_t m_lastread, m_lastwrite;
  bool m_eof, m_bad;
  FILE *m_file;
};

class wxFileInputStream: public wxFileStreamBase {
  DECLARE_CLASS(wxFileInputStream)
 public:
  wxFileInputStream(const wxString& fileName) : wxFileStreamBase(fileName, 1) {}
  virtual ~wxFileInputStream() {}
};

class wxFileOutputStream: public wxFileStreamBase {
  DECLARE_CLASS(wxFileOutputStream)
 public:
  wxFileOutputStream(const wxString& fileName) : wxFileStreamBase(fileName, 2) {}
  virtual ~wxFileOutputStream() {}
};

class wxFileStream: public wxFileStreamBase {
  DECLARE_CLASS(wxFileStream)
 public:
  wxFileStream(const wxString& fileName) : wxFileStreamBase(fileName, 0) {}
  virtual ~wxFileStream() {}
};

#endif
