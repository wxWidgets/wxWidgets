/////////////////////////////////////////////////////////////////////////////
// Name:        txtstrm.h
// Purpose:     Text stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     28/06/1998
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TXTSTREAM_H_
#define _WX_TXTSTREAM_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "txtstrm.h"
#endif

#include "wx/stream.h"

#if wxUSE_STREAMS

class WXDLLEXPORT wxTextInputStream;
class WXDLLEXPORT wxTextOutputStream;

typedef wxTextInputStream& (*__wxTextInputManip)(wxTextInputStream&);
typedef wxTextOutputStream& (*__wxTextOutputManip)(wxTextOutputStream&);

WXDLLEXPORT wxTextOutputStream &endl( wxTextOutputStream &stream );


class WXDLLEXPORT wxTextInputStream
{
public:
#if wxUSE_UNICODE
    wxTextInputStream(wxInputStream& s, const wxString &sep=wxT(" \t"), wxMBConv& conv = wxConvUTF8 );
#else
    wxTextInputStream(wxInputStream& s, const wxString &sep=wxT(" \t") );
#endif
    ~wxTextInputStream();

    wxUint32 Read32();
    wxUint16 Read16();
    wxUint8  Read8();
    double   ReadDouble();
    wxString ReadString();  // deprecated use ReadLine or ReadWord instead
    wxString ReadLine();
    wxString ReadWord();

    wxString GetStringSeparators() const { return m_separators; }
    void SetStringSeparators(const wxString &c) { m_separators = c; }

    // Operators
    wxTextInputStream& operator>>(wxString& word);
    wxTextInputStream& operator>>(char& c);
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
    
#if wxUSE_UNICODE
    wxMBConv &m_conv;
#endif

    bool   EatEOL(const wxChar &c);
    wxChar NextNonSeparators();
    void   SkipIfEndOfLine( wxChar c );
};

typedef enum
{
  wxEOL_NATIVE,
  wxEOL_UNIX,
  wxEOL_MAC,
  wxEOL_DOS
} wxEOL;

class WXDLLEXPORT wxTextOutputStream
{
public:
#if wxUSE_UNICODE
    wxTextOutputStream( wxOutputStream& s, wxEOL mode = wxEOL_NATIVE, wxMBConv& conv = wxConvUTF8  );
#else
    wxTextOutputStream( wxOutputStream& s, wxEOL mode = wxEOL_NATIVE );
#endif
    virtual ~wxTextOutputStream();

    void SetMode( wxEOL mode = wxEOL_NATIVE );
    wxEOL GetMode() { return m_mode; }

    void Write32(wxUint32 i);
    void Write16(wxUint16 i);
    void Write8(wxUint8 i);
    virtual void WriteDouble(double d);
    virtual void WriteString(const wxString& string);

    wxTextOutputStream& operator<<(const wxChar *string);
    wxTextOutputStream& operator<<(const wxString& string);
    wxTextOutputStream& operator<<(char c);
    wxTextOutputStream& operator<<(wxInt16 c);
    wxTextOutputStream& operator<<(wxInt32 c);
    wxTextOutputStream& operator<<(wxUint16 c);
    wxTextOutputStream& operator<<(wxUint32 c);
    wxTextOutputStream& operator<<(double f);
    wxTextOutputStream& operator<<(float f);

    wxTextOutputStream& operator<<( __wxTextOutputManip func) { return func(*this); }

protected:
    wxOutputStream &m_output;
    wxEOL           m_mode;
    
#if wxUSE_UNICODE
    wxMBConv &m_conv;
#endif

};

#endif
  // wxUSE_STREAMS

#endif
    // _WX_DATSTREAM_H_
