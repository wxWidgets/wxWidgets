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
  : m_input(&s)
{
}

wxDataInputStream::~wxDataInputStream()
{
}

wxUint32 wxDataInputStream::Read32()
{
  char buf[4];

  m_input->Read(buf, 4);

  return (wxUint32)buf[0] |
         ((wxUint32)buf[1] << 8) |
         ((wxUint32)buf[2] << 16) |
         ((wxUint32)buf[3] << 24);
}

wxUint16 wxDataInputStream::Read16()
{
  char buf[2];

  m_input->Read(buf, 2);

  return (wxUint16)buf[0] |
         ((wxUint16)buf[1] << 8);
}

wxUint8 wxDataInputStream::Read8()
{
  wxUint8 buf;

  m_input->Read((char *)&buf, 1);
  return (wxUint8)buf;
}

// Must be at global scope for VC++ 5
extern "C" double ConvertFromIeeeExtended(const unsigned char *bytes);

double wxDataInputStream::ReadDouble()
{
#if wxUSE_APPLE_IEEE
  char buf[10];

  m_input->Read(buf, 10);
  return ConvertFromIeeeExtended((unsigned char *)buf);
#else
  return 0.0;
#endif
}

wxString wxDataInputStream::ReadString()
{
  wxString wx_string;
  char *string;
  unsigned long len;

  len = Read32();
  string = new char[len+1];

  m_input->Read(string, len);

  string[len] = 0;
  wx_string = string;
  delete string;

  return wx_string;
}
  
wxDataInputStream& wxDataInputStream::operator>>(wxString& s)
{
  s = ReadString();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(wxInt8& c)
{
  c = (wxInt8)Read8();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(wxInt16& i)
{
  i = (wxInt16)Read16();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(wxInt32& i)
{
  i = (wxInt32)Read32();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(wxUint8& c)
{
  c = Read8();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(wxUint16& i)
{
  i = Read16();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(wxUint32& i)
{
  i = Read32();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(double& i)
{
  i = ReadDouble();
  return *this;
}

wxDataInputStream& wxDataInputStream::operator>>(float& f)
{
  f = (float)ReadDouble();
  return *this;
}

// ---------------------------------------------------------------------------
// wxDataOutputStream
// ---------------------------------------------------------------------------

wxDataOutputStream::wxDataOutputStream(wxOutputStream& s)
  : m_output(&s)
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
  m_output->Write(buf, 4);
}

void wxDataOutputStream::Write16(wxUint16 i)
{
  char buf[2];

  buf[0] = i & 0xff;
  buf[1] = (i >> 8) & 0xff;
  m_output->Write(buf, 2);
}

void wxDataOutputStream::Write8(wxUint8 i)
{
  m_output->Write(&i, 1);
}

void wxDataOutputStream::WriteString(const wxString& string)
{
  const wxWX2MBbuf buf = string.mb_str();
  Write32(string.Length());
  m_output->Write(buf, string.Len());
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
  m_output->Write(buf, 10);
}

wxDataOutputStream& wxDataOutputStream::operator<<(const wxChar *string)
{
  Write32(wxStrlen(string));
  m_output->Write((const char *)string, wxStrlen(string)*sizeof(wxChar));
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(wxString& string)
{
  WriteString(string);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(wxInt8 c)
{
  Write8((wxUint8)c);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(wxInt16 i)
{
  Write16((wxUint16)i);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(wxInt32 i)
{
  Write32((wxUint32)i);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(wxUint8 c)
{
  Write8(c);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(wxUint16 i)
{
  Write16(i);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(wxUint32 i)
{
  Write32(i);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(double f)
{
  WriteDouble(f);
  return *this;
}

wxDataOutputStream& wxDataOutputStream::operator<<(float f)
{
  WriteDouble((double)f);
  return *this;
}

#endif
  // wxUSE_STREAMS
  
