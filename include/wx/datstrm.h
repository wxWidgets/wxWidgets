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

#include "wx/wx.h"

class wxDataStream {
public:
  wxDataStream(iostream& s);
  wxDataStream(istream& s);
  wxDataStream(ostream& s);

  virtual ~wxDataStream();

  unsigned long Read32();
  unsigned short Read16();
  unsigned char Read8();
  double ReadDouble();
  wxString ReadLine();
  wxString ReadString();

  void Write32(unsigned long i);
  void Write16(unsigned short i);
  void Write8(unsigned char i);
  void WriteDouble(double d);
  void WriteLine(const wxString& line);
  void WriteString(const wxString& string);
protected:
  istream *m_istream;
  ostream *m_ostream;
};

#endif
    // __HELPBASEH__
