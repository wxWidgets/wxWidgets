/////////////////////////////////////////////////////////////////////////////
// Name:        wfstream.h
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
#pragma interface "wfstream.h"
#endif

#include "wx/defs.h"

#if wxUSE_STREAMS && wxUSE_FILE

#include "wx/object.h"
#include "wx/string.h"
#include "wx/stream.h"
#include "wx/file.h"
#include "wx/ffile.h"

// ----------------------------------------------------------------------------
// wxFileStream using wxFile
// ----------------------------------------------------------------------------

class wxFileInputStream: public wxInputStream {
 public:
  wxFileInputStream(const wxString& ifileName);
  wxFileInputStream(wxFile& file);
  wxFileInputStream(int fd);
  ~wxFileInputStream();

  size_t GetSize() const;

  bool Ok() const { return m_file->IsOpened(); }

 protected:
  wxFileInputStream();

  size_t OnSysRead(void *buffer, size_t size);
  off_t OnSysSeek(off_t pos, wxSeekMode mode);
  off_t OnSysTell() const;

 protected:
  wxFile *m_file;
  bool m_file_destroy;
};

class wxFileOutputStream: public wxOutputStream {
 public:
  wxFileOutputStream(const wxString& fileName);
  wxFileOutputStream(wxFile& file);
  wxFileOutputStream(int fd);
  virtual ~wxFileOutputStream();

  // To solve an ambiguity on GCC
//  inline wxOutputStream& Write(const void *buffer, size_t size)
//     { return wxOutputStream::Write(buffer, size); }

  void Sync();
  size_t GetSize() const;

  bool Ok() const { return m_file->IsOpened(); }

 protected:
  wxFileOutputStream();

  size_t OnSysWrite(const void *buffer, size_t size);
  off_t OnSysSeek(off_t pos, wxSeekMode mode);
  off_t OnSysTell() const;

 protected:
  wxFile *m_file;
  bool m_file_destroy;
};

class wxFileStream: public wxFileInputStream, public wxFileOutputStream {
 public:
  wxFileStream(const wxString& fileName);
};

// ----------------------------------------------------------------------------
// wxFFileStream using wxFFile
// ----------------------------------------------------------------------------

class wxFFileInputStream: public wxInputStream {
 public:
  wxFFileInputStream(const wxString& ifileName);
  wxFFileInputStream(wxFFile& file);
  wxFFileInputStream(FILE *file);
  ~wxFFileInputStream();

  size_t GetSize() const;

  bool Ok() const { return m_file->IsOpened(); }

 protected:
  wxFFileInputStream();

  size_t OnSysRead(void *buffer, size_t size);
  off_t OnSysSeek(off_t pos, wxSeekMode mode);
  off_t OnSysTell() const;

 protected:
  wxFFile *m_file;
  bool m_file_destroy;
};

class wxFFileOutputStream: public wxOutputStream {
 public:
  wxFFileOutputStream(const wxString& fileName);
  wxFFileOutputStream(wxFFile& file);
  wxFFileOutputStream(FILE *file);
  virtual ~wxFFileOutputStream();

  // To solve an ambiguity on GCC
//  inline wxOutputStream& Write(const void *buffer, size_t size)
//     { return wxOutputStream::Write(buffer, size); }

  void Sync();
  size_t GetSize() const;

  bool Ok() const { return m_file->IsOpened(); }

 protected:
  wxFFileOutputStream();

  size_t OnSysWrite(const void *buffer, size_t size);
  off_t OnSysSeek(off_t pos, wxSeekMode mode);
  off_t OnSysTell() const;

 protected:
  wxFFile *m_file;
  bool m_file_destroy;
};

class wxFFileStream: public wxFFileInputStream, public wxFFileOutputStream {
 public:
  wxFFileStream(const wxString& fileName);
};
#endif
  // wxUSE_STREAMS && wxUSE_FILE

#endif
  // _WX_WXFSTREAM_H__

