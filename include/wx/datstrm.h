/////////////////////////////////////////////////////////////////////////////
// Name:        datstrm.h
// Purpose:     Data stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     28/06/1998
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DATSTREAMH__
#define __DATSTREAMH__

#ifdef __GNUG__
#pragma interface "datstrm.h"
#endif

#include <wx/stream.h>

class wxDataStream: public wxFilterInputStream {
public:
  wxDataInputStream(wxInputStream& s);
  virtual ~wxDataInputStream();

  unsigned long Read32();
  unsigned short Read16();
  unsigned char Read8();
  double ReadDouble();
  wxString ReadLine();
  wxString ReadString();
};

class wxDataOutputStream: public wxFilterOutputStream {
 public:
  wxDataOutputStream(wxOutputStream& s);
  virtual ~wxDataOutputStream();

  void Write32(unsigned long i);
  void Write16(unsigned short i);
  void Write8(unsigned char i);
  void WriteDouble(double d);
  void WriteLine(const wxString& line);
  void WriteString(const wxString& string);
};

#endif
    // __DATSTREAMH__
