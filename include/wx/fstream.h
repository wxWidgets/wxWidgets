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

class wxFileInputStream: public wxInputStream, virtual public wxFile {
 public:
  wxFileInputStream(const wxString& fileName);
  virtual ~wxFileInputStream();

  virtual char Peek();

  virtual bool Eof() const { return wxFile::Eof(); }

  bool Ok() const { return wxFile::IsOpened(); }

 protected:
  wxFileInputStream() {}

  size_t DoRead(void *buffer, size_t size);
  off_t DoSeekInput(off_t pos, wxSeekMode mode);
  off_t DoTellInput() const;
};

class wxFileOutputStream: public wxOutputStream, virtual public wxFile {
 public:
  wxFileOutputStream(const wxString& fileName);
  virtual ~wxFileOutputStream();

  // To solve an ambiguity on GCC
  inline wxOutputStream& Write(const void *buffer, size_t size)
     { return wxOutputStream::Write(buffer, size); }

  void Sync();

  bool Ok() const { return wxFile::IsOpened(); }

 protected:
  wxFileOutputStream() {}

  size_t DoWrite(const void *buffer, size_t size);
  off_t DoSeekOutput(off_t pos, wxSeekMode mode);
  off_t DoTellOutput() const;
};

class wxFileStream: public wxFileInputStream, public wxFileOutputStream {
 public:
  wxFileStream(const wxString& fileName);
  virtual ~wxFileStream();
};

#endif
