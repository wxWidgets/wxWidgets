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

#ifndef _WX_DATSTREAM_H_
#define _WX_DATSTREAM_H_

#ifdef __GNUG__
#pragma interface "datstrm.h"
#endif

#include <wx/stream.h>

#if wxUSE_STREAMS

class WXDLLEXPORT wxDataInputStream: public wxFilterInputStream {
public:
  wxDataInputStream(wxInputStream& s);
  virtual ~wxDataInputStream();

  wxUint32 Read32();
  wxUint16 Read16();
  wxUint8 Read8();
  double ReadDouble();
  wxString ReadString();
};

class WXDLLEXPORT wxDataOutputStream: public wxFilterOutputStream {
 public:
  wxDataOutputStream(wxOutputStream& s);
  virtual ~wxDataOutputStream();

  void Write32(wxUint32 i);
  void Write16(wxUint16 i);
  void Write8(wxUint8 i);
  void WriteDouble(double d);
  void WriteString(const wxString& string);
};

#endif
  // wxUSE_STREAMS

#endif
    // _WX_DATSTREAM_H_
