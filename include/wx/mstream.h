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
#ifndef _WX_WXMMSTREAM_H__
#define _WX_WXMMSTREAM_H__

#include <wx/stream.h>

class wxMemoryInputStream: public wxInputStream {
 private:
  size_t m_length;
  
 public:
  wxMemoryInputStream(const char *data, size_t length);
  virtual ~wxMemoryInputStream();
  virtual size_t StreamSize() const { return m_length; }

  char Peek();
};

class wxMemoryOutputStream:  public wxOutputStream {
 public:
  wxMemoryOutputStream(char *data = NULL, size_t length = 0);
  virtual ~wxMemoryOutputStream();
};

#endif
