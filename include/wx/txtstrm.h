/////////////////////////////////////////////////////////////////////////////
// Name:        txtstrm.h
// Purpose:     Text stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     28/06/1998
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TXTSTREAM_H_
#define _WX_TXTSTREAM_H_

#ifdef __GNUG__
#pragma interface "txtstrm.h"
#endif

#include <wx/stream.h>

#if wxUSE_STREAMS

class WXDLLEXPORT wxTextInputStream {
public:
  wxTextInputStream(wxInputStream& s);
  ~wxTextInputStream();

  wxUint32 Read32();
  wxUint16 Read16();
  wxUint8 Read8();
  double ReadDouble();
  wxString ReadString();

  // Operators
  wxTextInputStream& operator>>(wxString& line);
  wxTextInputStream& operator>>(wxChar& c);
  wxTextInputStream& operator>>(wxInt16& i);
  wxTextInputStream& operator>>(wxInt32& i);
  wxTextInputStream& operator>>(wxUint16& i);
  wxTextInputStream& operator>>(wxUint32& i);
  wxTextInputStream& operator>>(double& i);
  wxTextInputStream& operator>>(float& f);

 protected:
  wxInputStream *m_input;
};

class WXDLLEXPORT wxTextOutputStream {
 public:
  wxTextOutputStream(wxOutputStream& s);
  ~wxTextOutputStream();

  void Write32(wxUint32 i);
  void Write16(wxUint16 i);
  void Write8(wxUint8 i);
  void WriteDouble(double d);
  void WriteString(const wxString& string);

  wxTextOutputStream& operator<<(const wxChar *string);
  wxTextOutputStream& operator<<(const wxString& string);
  wxTextOutputStream& operator<<(wxChar c);
  wxTextOutputStream& operator<<(wxInt16 c);
  wxTextOutputStream& operator<<(wxInt32 c);
  wxTextOutputStream& operator<<(wxUint16 c);
  wxTextOutputStream& operator<<(wxUint32 c);
  wxTextOutputStream& operator<<(double f);
  wxTextOutputStream& operator<<(float f);
 
 protected:
  wxOutputStream *m_output;
};

#endif
  // wxUSE_STREAMS

#endif
    // _WX_DATSTREAM_H_
