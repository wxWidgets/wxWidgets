/////////////////////////////////////////////////////////////////////////////
// Name:        datstrm.cpp
// Purpose:     Data stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     28/06/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux 
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "datstrm.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include "wx/datstrm.h"

// ---------------------------------------------------------------------------
// wxDataInputStream
// ---------------------------------------------------------------------------

wxDataInputStream::wxDataInputStream(wxInputStream& s)
  : wxFilterInputStream(s)
{
}

wxDataInputStream::~wxDataInputStream()
{
}

wxUint32 wxDataInputStream::Read32()
{
  char buf[4];

  Read(buf, 4);

  return (wxUint32)buf[0] |
         ((wxUint32)buf[1] << 8) |
         ((wxUint32)buf[2] << 16) |
         ((wxUint32)buf[3] << 24);
}

wxUint16 wxDataInputStream::Read16()
{
  char buf[2];

  Read(buf, 2);

  return (wxUint16)buf[0] |
         ((wxUint16)buf[1] << 8);
}

wxUint8 wxDataInputStream::Read8()
{
  wxUint8 buf;

  Read((char *)&buf, 1);
  return (wxUint8)buf;
}

// Must be at global scope for VC++ 5
extern "C" double ConvertFromIeeeExtended(const unsigned char *bytes);

double wxDataInputStream::ReadDouble()
{
#if wxUSE_APPLE_IEEE
  char buf[10];

  Read(buf, 10);
  return ConvertFromIeeeExtended((unsigned char *)buf);
#else
  return 0.0;
#endif
}

wxString wxDataInputStream::ReadLine()
{
  char c, last_endl = 0;
  bool end_line = FALSE;
  wxString line;

  while (!end_line) {
    c = GetC();
    if (LastError() != wxStream_NOERROR)
      break;

    switch (c) {
    case '\n':
      end_line = TRUE;
      break;
    case '\r':
      last_endl = '\r';
      break;
    default:
      if (last_endl == '\r') {
        end_line = TRUE;
        InputStreamBuffer()->WriteBack(c);
        break;
      }
      line += c;
      break;
    } 
  }
  return line;
}

wxString wxDataInputStream::ReadString()
{
  wxString wx_string;
  char *string;
  unsigned long len;

  len = Read32();
  string = new char[len+1];

  Read(string, len);

  string[len] = 0;
  wx_string = string;
  delete string;

  return wx_string;
}

// ---------------------------------------------------------------------------
// wxDataOutputStream
// ---------------------------------------------------------------------------

wxDataOutputStream::wxDataOutputStream(wxOutputStream& s)
 : wxFilterOutputStream(s)
{
}

wxDataOutputStream::~wxDataOutputStream()
{
}

void wxDataOutputStream::Write32(wxUint32 i)
{
  char buf[4];

  buf[0] = i & 0xff;
  buf[1] = (i >> 8) & 0xff;
  buf[2] = (i >> 16) & 0xff;
  buf[3] = (i >> 24) & 0xff;
  Write(buf, 4);
}

void wxDataOutputStream::Write16(wxUint16 i)
{
  char buf[2];

  buf[0] = i & 0xff;
  buf[1] = (i >> 8) & 0xff;
  Write(buf, 2);
}

void wxDataOutputStream::Write8(wxUint8 i)
{
  Write(&i, 1);
}

void wxDataOutputStream::WriteLine(const wxString& line)
{
#ifdef __WXMSW__
  wxString tmp_string = line + _T("\r\n");
#else
  wxString tmp_string = line + _T('\n');
#endif

  Write((const wxChar *) tmp_string, tmp_string.Length()*sizeof(wxChar));
}

void wxDataOutputStream::WriteString(const wxString& string)
{
  Write32(string.Length());
  Write((const wxChar *) string, string.Length()*sizeof(wxChar));
}

// Must be at global scope for VC++ 5
extern "C" void ConvertToIeeeExtended(double num, unsigned char *bytes);

void wxDataOutputStream::WriteDouble(double d)
{
  char buf[10];

#if wxUSE_APPLE_IEEE
  ConvertToIeeeExtended(d, (unsigned char *)buf);
#else
#  pragma warning "wxDataOutputStream::WriteDouble() not using IeeeExtended - will not work!"
 buf[0] = '\0';
#endif
  Write(buf, 10);
}

#endif
  // wxUSE_STREAMS
  
