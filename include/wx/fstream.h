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

#ifndef _WX_WXFSTREAM_H__
#define _WX_WXFSTREAM_H__

#ifdef __GNUG__
#pragma interface "fstream.h"
#endif

#include <wx/object.h>
#include <wx/string.h>
#include <wx/stream.h>
#include <wx/file.h>

// Disable warnings such as
// 'wxFileStream' : inherits 'wxFileInputStream::Peek' via dominance

#ifdef _MSC_VER
#pragma warning(disable:4250)
#endif

class wxFileStreamBase {
protected:
  wxFile *m_file;
  bool m_file_destroy;
};

class wxFileInputStream: public virtual wxInputStream,
                         public virtual wxFileStreamBase {
 public:
  wxFileInputStream(const wxString& fileName);
  virtual ~wxFileInputStream();

  virtual char Peek();

  bool Ok() const { return m_file->IsOpened(); }

 protected:
  wxFileInputStream();

  size_t DoRead(void *buffer, size_t size);
  off_t DoSeekInput(off_t pos, wxSeekMode mode);
  off_t DoTellInput() const;
};

class wxFileOutputStream: public virtual wxOutputStream,
                          public virtual wxFileStreamBase {
 public:
  wxFileOutputStream(const wxString& fileName);
  virtual ~wxFileOutputStream();

  // To solve an ambiguity on GCC
  inline wxOutputStream& Write(const void *buffer, size_t size)
     { return wxOutputStream::Write(buffer, size); }

  void Sync();

  bool Ok() const { return m_file->IsOpened(); }

 protected:
  wxFileOutputStream();

  size_t DoWrite(const void *buffer, size_t size);
  off_t DoSeekOutput(off_t pos, wxSeekMode mode);
  off_t DoTellOutput() const;
};

class wxFileStream: public wxStream,
                    public wxFileInputStream, public wxFileOutputStream {
 public:
  wxFileStream(const wxString& fileName);
  virtual ~wxFileStream();
};

#ifdef _MSC_VER
#pragma warning(default:4250)
#endif

#endif
