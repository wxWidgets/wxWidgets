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

#include "wx/stream.h"

#if wxUSE_STREAMS

class WXDLLEXPORT wxTextInputStream;
class WXDLLEXPORT wxTextOutputStream;

typedef wxTextInputStream& (*__wxTextInputManip)(wxTextInputStream&);
typedef wxTextOutputStream& (*__wxTextOutputManip)(wxTextOutputStream&);

WXDLLEXPORT wxTextOutputStream &endl( wxTextOutputStream &stream );

class WXDLLEXPORT wxTextInputStream {
public:
  wxTextInputStream(wxInputStream& s,  const wxString &sep=wxT(" \t"));
  ~wxTextInputStream();

  wxUint32 Read32();
  wxUint16 Read16();
  wxUint8  Read8();
  double   ReadDouble();
  wxString ReadString();  // deprecated use ReadLine or ReadWord instead
  wxString ReadLine();
  wxString ReadWord();

  wxString GetStringSeparators() const          { return m_separators;}
  void     SetStringSeparators(const wxString &c) { m_separators=c;}

  // Operators
  wxTextInputStream& operator>>(wxString& word);
  wxTextInputStream& operator>>(wxChar& c);
  wxTextInputStream& operator>>(wxInt16& i);
  wxTextInputStream& operator>>(wxInt32& i);
  wxTextInputStream& operator>>(wxUint16& i);
  wxTextInputStream& operator>>(wxUint32& i);
  wxTextInputStream& operator>>(double& i);
  wxTextInputStream& operator>>(float& f);
  
  wxTextInputStream& operator>>( __wxTextInputManip func) { return func(*this); }
  
 protected:
  wxInputStream &m_input;
  wxString m_separators;

  bool   EatEOL(const wxChar &c);
  wxChar NextNonSeparators();
  void   SkipIfEndOfLine( wxChar c );
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
 
  wxTextOutputStream& operator<<( __wxTextOutputManip func) { return func(*this); }
  
 protected:
  wxOutputStream &m_output;
};

#endif
  // wxUSE_STREAMS

#endif
    // _WX_DATSTREAM_H_
