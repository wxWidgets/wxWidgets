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

#include "wx/stream.h"

#if wxUSE_STREAMS

class WXDLLEXPORT wxDataInputStream
{
public:
    wxDataInputStream(wxInputStream& s);
    ~wxDataInputStream();

    wxUint32 Read32();
    wxUint16 Read16();
    wxUint8 Read8();
    double ReadDouble();
    wxString ReadString();

    wxDataInputStream& operator>>(wxString& s);
    wxDataInputStream& operator>>(wxInt8& c);
    wxDataInputStream& operator>>(wxInt16& i);
    wxDataInputStream& operator>>(wxInt32& i);
    wxDataInputStream& operator>>(wxUint8& c);
    wxDataInputStream& operator>>(wxUint16& i);
    wxDataInputStream& operator>>(wxUint32& i);
    wxDataInputStream& operator>>(double& i);
    wxDataInputStream& operator>>(float& f);

    void BigEndianOrdered(bool be_order) { m_be_order = be_order; }
  
protected:
    wxInputStream *m_input;
    bool m_be_order;
};

class WXDLLEXPORT wxDataOutputStream
{
public:
    wxDataOutputStream(wxOutputStream& s);
    ~wxDataOutputStream();

    void Write32(wxUint32 i);
    void Write16(wxUint16 i);
    void Write8(wxUint8 i);
    void WriteDouble(double d);
    void WriteString(const wxString& string);

    wxDataOutputStream& operator<<(const wxChar *string);
    wxDataOutputStream& operator<<(wxString& string);
    wxDataOutputStream& operator<<(wxInt8 c);
    wxDataOutputStream& operator<<(wxInt16 i);
    wxDataOutputStream& operator<<(wxInt32 i);
    wxDataOutputStream& operator<<(wxUint8 c);
    wxDataOutputStream& operator<<(wxUint16 i);
    wxDataOutputStream& operator<<(wxUint32 i);
    wxDataOutputStream& operator<<(double f);
    wxDataOutputStream& operator<<(float f);

    void BigEndianOrdered(bool be_order) { m_be_order = be_order; } 
  
protected:
    wxOutputStream *m_output;
    bool m_be_order;
};

#endif
  // wxUSE_STREAMS

#endif
    // _WX_DATSTREAM_H_
