/////////////////////////////////////////////////////////////////////////////
// Name:        datstrm.h
// Purpose:     Data stream classes
// Author:      Guilhem Lavaux
// Modified by: Mickael Gilabert
// Created:     28/06/1998
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATSTREAM_H_
#define _WX_DATSTREAM_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "datstrm.h"
#endif

#include "wx/stream.h"
#include "wx/longlong.h"
#include "wx/strconv.h"

#if wxUSE_STREAMS

class WXDLLIMPEXP_BASE wxDataInputStream
{
public:
#if wxUSE_UNICODE
    wxDataInputStream(wxInputStream& s, wxMBConv& conv = wxConvUTF8);
#else
    wxDataInputStream(wxInputStream& s);
#endif
    ~wxDataInputStream(){}

    bool IsOk() { return m_input->IsOk(); }

    wxUint64 Read64();
    wxUint32 Read32();
    wxUint16 Read16();
    wxUint8 Read8();
    double ReadDouble();
    wxString ReadString();

    void Read64(wxUint64 *buffer, size_t size);
    void Read32(wxUint32 *buffer, size_t size);
    void Read16(wxUint16 *buffer, size_t size);
    void Read8(wxUint8 *buffer, size_t size);
    void ReadDouble(double *buffer, size_t size);

    wxDataInputStream& operator>>(wxString& s);
    wxDataInputStream& operator>>(wxInt8& c);
    wxDataInputStream& operator>>(wxInt16& i);
    wxDataInputStream& operator>>(wxInt32& i);
    wxDataInputStream& operator>>(wxUint8& c);
    wxDataInputStream& operator>>(wxUint16& i);
    wxDataInputStream& operator>>(wxUint32& i);
    wxDataInputStream& operator>>(wxUint64& i);
    wxDataInputStream& operator>>(double& i);
    wxDataInputStream& operator>>(float& f);

    void BigEndianOrdered(bool be_order) { m_be_order = be_order; }

protected:
    wxInputStream *m_input;
    bool m_be_order;
#if wxUSE_UNICODE
    wxMBConv& m_conv;
#endif

    DECLARE_NO_COPY_CLASS(wxDataInputStream)
};

class WXDLLIMPEXP_BASE wxDataOutputStream
{
public:
#if wxUSE_UNICODE
    wxDataOutputStream(wxOutputStream& s, wxMBConv& conv = wxConvUTF8);
#else
    wxDataOutputStream(wxOutputStream& s);
#endif
    ~wxDataOutputStream(){}

    bool IsOk() { return m_output->IsOk(); }

    void Write64(wxUint64 i);
    void Write32(wxUint32 i);
    void Write16(wxUint16 i);
    void Write8(wxUint8 i);
    void WriteDouble(double d);
    void WriteString(const wxString& string);

    void Write64(const wxUint64 *buffer, size_t size);
    void Write32(const wxUint32 *buffer, size_t size);
    void Write16(const wxUint16 *buffer, size_t size);
    void Write8(const wxUint8 *buffer, size_t size);
    void WriteDouble(const double *buffer, size_t size);

    wxDataOutputStream& operator<<(const wxChar *string);
    wxDataOutputStream& operator<<(const wxString& string);
    wxDataOutputStream& operator<<(wxInt8 c);
    wxDataOutputStream& operator<<(wxInt16 i);
    wxDataOutputStream& operator<<(wxInt32 i);
    wxDataOutputStream& operator<<(wxUint8 c);
    wxDataOutputStream& operator<<(wxUint16 i);
    wxDataOutputStream& operator<<(wxUint32 i);
    wxDataOutputStream& operator<<(wxUint64 i);
    wxDataOutputStream& operator<<(double f);
    wxDataOutputStream& operator<<(float f);

    void BigEndianOrdered(bool be_order) { m_be_order = be_order; }

protected:
    wxOutputStream *m_output;
    bool m_be_order;
#if wxUSE_UNICODE
    wxMBConv& m_conv;
#endif

    DECLARE_NO_COPY_CLASS(wxDataOutputStream)
};

#endif
  // wxUSE_STREAMS

#endif
    // _WX_DATSTREAM_H_
