/////////////////////////////////////////////////////////////////////////////////
// Name:        txtstrm.cpp
// Purpose:     Text stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     28/06/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux 
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "txtstrm.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include "wx/txtstrm.h"
#include <ctype.h>

wxTextInputStream::wxTextInputStream(wxInputStream& s)
  : m_input(&s)
{
}

wxTextInputStream::~wxTextInputStream()
{
}

wxUint32 wxTextInputStream::Read32()
{
  /* I only implemented a simple integer parser */
  int c; 
  int sign;
  wxInt32 i;

  while (isspace( c = m_input->GetC() ) )
     /* Do nothing */ ;

  i = 0;
  if (! (c == '-' || isdigit(c)) ) {
    m_input->Ungetch(c);
    return 0;
  }

  if (c == '-') {
    sign = -1;
    c = m_input->GetC();
  } else if (c == '+') {
    sign = 1;
    c = m_input->GetC();
  } else {
    sign = 1;
  }

  while (isdigit(c)) {
    i = i*10 + (c - (int)'0');
    c = m_input->GetC();
  }

  if (c != '\n' && c != '\r')
    m_input->Ungetch(c);

  i *= sign;

  return (wxUint32)i;
}

wxUint16 wxTextInputStream::Read16()
{
  return (wxUint16)Read32();
}

wxUint8 wxTextInputStream::Read8()
{
  return (wxUint8)Read32();
}

double wxTextInputStream::ReadDouble()
{
  /* I only implemented a simple float parser */
  double f;
  int c, sign;

  while (isspace( c = m_input->GetC() ) || c == '\n' || c == '\r')
     /* Do nothing */ ;

  f = 0.0;
  if (! (c == '-' || isdigit(c)) ) {
    m_input->Ungetch(c);
    return 0.0;
  }

  if (c == '-') {
    sign = -1;
    c = m_input->GetC();
  } else if (c == '+') {
    sign = 1;
    c = m_input->GetC();
  } else {
    sign = 1;
  }

  while (isdigit(c)) {
    f = f*10 + (c - '0');
    c = m_input->GetC();
  }

  if (c == '.') {
    double f_multiplicator = (double) 0.1;

    c = m_input->GetC();

    while (isdigit(c)) {
      f += (c-'0')*f_multiplicator;
      f_multiplicator /= 10;
      c = m_input->GetC();
    }

    if (c == 'e') {
      double f_multiplicator = 0.0;
      int i, e;

      c = m_input->GetC();

      switch(c) {
      case '-':
        f_multiplicator = 0.1;
        break;
      case '+':
        f_multiplicator = 10.0;
        break;
      }

      e = Read8();

      for (i=0;i<e;i++)
        f *= f_multiplicator;
    } else if (c != '\n' && c != '\r')
      m_input->Ungetch(c);
 
  }

  f *= sign;

  return f;
}

wxString wxTextInputStream::ReadString()
{
  char c, last_endl = 0;
  bool end_line = FALSE;
  wxString line;

  while (!end_line) {
    c = m_input->GetC();
    if (m_input->LastError() != wxStream_NOERROR)
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
        m_input->Ungetch(c);
        break;
      }
      line += c;
      break;
    } 
  }
  return line;
}
  
wxTextInputStream& wxTextInputStream::operator>>(wxString& line)
{
  line = ReadString();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxChar& c)
{
  // TODO
/*
  m_input->Read(&c, sizeof(wxChar));
*/
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxInt16& i)
{
  i = (wxInt16)Read16();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxInt32& i)
{
  i = (wxInt32)Read32();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxUint16& i)
{
  i = Read16();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(wxUint32& i)
{
  i = Read32();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(double& i)
{
  i = ReadDouble();
  return *this;
}

wxTextInputStream& wxTextInputStream::operator>>(float& f)
{
  f = (float)ReadDouble();
  return *this;
}

wxTextOutputStream::wxTextOutputStream(wxOutputStream& s)
  : m_output(&s)
{
}

wxTextOutputStream::~wxTextOutputStream()
{
}

void wxTextOutputStream::Write32(wxUint32 i)
{
  wxString str;

  str.Printf(_T("%u"), i);
  WriteString(str);
}

void wxTextOutputStream::Write16(wxUint16 i)
{
  wxString str;

  str.Printf(_T("%u"), i);
  WriteString(str);
}

void wxTextOutputStream::Write8(wxUint8 i)
{
  wxString str;

  str.Printf(_T("%u"), i);
  WriteString(str);
}

void wxTextOutputStream::WriteDouble(double d)
{
  wxString str;

  str.Printf(_T("%f"), d);
  WriteString(str);
}

void wxTextOutputStream::WriteString(const wxString& string)
{
#if wxUSE_UNICODE
  const wxWX2MBbuf buf = string.mb_str();
  m_output->Write(buf, string.Len());
#else
  m_output->Write(string, string.Len());
#endif
}

wxTextOutputStream& wxTextOutputStream::operator<<(const wxChar *string)
{
  WriteString(wxString(string));
  return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(const wxString& string)
{
  WriteString(string);
  return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxChar c)
{
  wxString tmp_str;
  tmp_str.Printf(_T("%c"), c);
  WriteString(tmp_str);
  return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt16 c)
{
  Write16((wxUint16)c);
  return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxInt32 c)
{
  Write32((wxUint32)c);
  return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint16 c)
{
  Write16(c);
  return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(wxUint32 c)
{
  Write32(c);
  return *this;
}

wxTextOutputStream &wxTextOutputStream::operator<<(double f)
{
  WriteDouble(f);
  return *this;
}

wxTextOutputStream& wxTextOutputStream::operator<<(float f)
{
  WriteDouble((double)f);
  return *this;
}

#endif
  // wxUSE_STREAMS
