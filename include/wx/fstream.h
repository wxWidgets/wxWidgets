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

#include <wx/object.h>
#include <wx/string.h>
#include <wx/stream.h>
#include <wx/file.h>

class wxFileInputStream: virtual public wxFile, public wxInputStream {
  DECLARE_CLASS(wxFileInputStream)
 public:
  wxFileInputStream(const wxString& fileName);
  virtual ~wxFileInputStream();

  wxInputStream& Read(void *buffer, size_t size);
  off_t SeekI(off_t pos, wxSeekMode mode = wxFromStart);
  off_t TellI() const;

  bool Eof() const { return m_eof; }
  size_t LastRead() const { return m_lastread; }

  bool Ok() const { return wxFile::IsOpened(); }

 protected:
  wxFileInputStream() {}

 protected:
  bool m_eof;
  bool m_ok_i;
  size_t m_lastread;
};

class wxFileOutputStream: virtual wxFile, public wxOutputStream {
  DECLARE_CLASS(wxFileOutputStream)
 public:
  wxFileOutputStream(const wxString& fileName);
  virtual ~wxFileOutputStream();

  wxOutputStream& Write(const void *buffer, size_t size);
  off_t SeekO(off_t pos, wxSeekMode mode = wxFromStart);
  off_t TellO() const;

  bool Bad() const { return m_bad; }
  size_t LastWrite() const { return m_lastwrite; }

  void Sync();

  bool IsOpened() const { return wxFile::IsOpened(); }

 protected:
  wxFileOutputStream() {}

 protected:
  bool m_bad;
  size_t m_lastwrite;
};

class wxFileStream: public wxFileInputStream, public wxFileOutputStream {
  DECLARE_CLASS(wxFileStream)
 public:
  wxFileStream(const wxString& fileName);
  virtual ~wxFileStream();
};

#endif
