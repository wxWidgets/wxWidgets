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

#ifndef __WXSTREAM_H__
#define __WXSTREAM_H__

#ifdef __GNUG__
#pragma interface "stream.h"
#endif

#include <stdio.h>
#include <wx/object.h>
#include <wx/string.h>

/*
 * wxStream: base classes
 */

typedef enum {
  wxBeginPosition = 0, wxCurrentPosition = 1, wxEndPosition = 2
} wxWhenceType;

class wxOutputStream;
class wxInputStream: public wxObject {
  DECLARE_ABSTRACT_CLASS(wxInputStream)
 public:
  wxInputStream();
  virtual ~wxInputStream();

  virtual wxInputStream& Read(void *buffer, size_t size) = 0;
  wxInputStream& Read(wxOutputStream& stream_out);

  virtual size_t SeekI(int pos, wxWhenceType whence = wxBeginPosition) = 0;
  virtual size_t TellI() const = 0;

  virtual bool Eof() const = 0;
  virtual size_t LastRead() const = 0;
};

class wxOutputStream: public wxObject {
  DECLARE_ABSTRACT_CLASS(wxOutputStream)
 public:
  wxOutputStream();
  virtual ~wxOutputStream();

  virtual wxOutputStream& Write(const void *buffer, size_t size) = 0;
  wxOutputStream& Write(wxInputStream& stream_in);

  virtual size_t SeekO(int pos, wxWhenceType whence = wxBeginPosition) = 0;
  virtual size_t TellO() const = 0;

  virtual bool Bad() const = 0;
  virtual size_t LastWrite() const = 0;

  virtual void Sync() {}
};

class wxStream: public wxInputStream, public wxOutputStream {
  DECLARE_ABSTRACT_CLASS(wxStream)
 public:
  wxStream() : wxInputStream(), wxOutputStream() {}
  virtual ~wxStream() {}
};

/*
 * "Filter" streams
 */

class wxFilterInputStream: public wxInputStream {
  DECLARE_CLASS(wxFilterInputStream)
 public:
  wxFilterInputStream(wxInputStream& stream);
  virtual ~wxFilterInputStream();

  virtual wxInputStream& Read(void *buffer, size_t size)
     { return m_parent_i_stream->Read(buffer, size); }
  virtual size_t SeekI(int pos, wxWhenceType whence = wxBeginPosition)
     { return m_parent_i_stream->SeekI(pos, whence); }

  virtual bool Eof() const { return m_parent_i_stream->Eof(); } 
  virtual size_t LastRead() const { return m_parent_i_stream->LastRead(); } 
 protected:
  wxInputStream *m_parent_i_stream;
};

class wxFilterOutputStream: public wxOutputStream {
  DECLARE_CLASS(wxFilterOutputStream)
 public:
  wxFilterOutputStream(wxOutputStream& stream);
  virtual ~wxFilterOutputStream();

  virtual wxOutputStream& Write(const void *buffer, size_t size)
     { return m_parent_o_stream->Write(buffer, size); }
  virtual size_t SeekO(int pos, wxWhenceType whence = wxBeginPosition)
     { return m_parent_o_stream->SeekO(pos, whence); }

  virtual bool Bad() const { return m_parent_o_stream->Bad(); }
  virtual size_t LastWrite() const { return m_parent_o_stream->LastWrite(); }

 protected:
  wxOutputStream *m_parent_o_stream;
};

#endif
