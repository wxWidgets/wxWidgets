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

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/datstrm.h"

wxDataInputStream::wxDataInputStream(wxInputStream& s)
  : wxFilterInputStream(s)
{
}

wxDataInputStream::~wxDataInputStream()
{
}

unsigned long wxDataInputStream::Read32()
{
  char buf[4];

  if (!m_istream)
    return 0;

  Read(buf, 4);

  return (unsigned long)buf[0] |
         ((unsigned long)buf[1] << 8) |
         ((unsigned long)buf[2] << 16) |
         ((unsigned long)buf[3] << 24);
}

unsigned short wxDataInputStream::Read16()
{
  char buf[2];

  if (!m_istream)
    return 0;

  Read(buf, 2);

  return (unsigned short)buf[0] |
         ((unsigned short)buf[1] << 8);
}

unsigned char wxDataInputStream::Read8()
{
  char buf;

  if (!m_istream)
    return 0;

  Read(&buf, 1);
  return (unsigned char)buf;
}

// Must be at global scope for VC++ 5
extern "C" double ConvertFromIeeeExtended(const unsigned char *bytes);

double wxDataInputStream::ReadDouble()
{
#if USE_APPLE_IEEE
  char buf[10];

  if (!m_istream)
    return 0.0;

  Read(buf, 10);
  return ConvertFromIeeeExtended((unsigned char *)buf);
#else
  return 0.0;
#endif
}

wxString wxDataInputStream::ReadLine()
{
  char i_strg[255];

  if (!m_istream)
    return "";

  // TODO: Implement ReadLine
  return i_strg;
}

wxString wxDataInputStream::ReadString()
{
  wxString wx_string;
  char *string;
  unsigned long len;

  if (!m_istream)
    return "";

  len = Read32();
  string = new char[len+1];

  Read(string, len);

  string[len] = 0;
  wx_string = string;
  delete string;

  return wx_string; 
}

wxDataOutputStream::wxDataOutputStream(wxOutputStream& s)
 : wxFilterOutputStream(s)
{
}

void wxDataOutputStream::Write32(unsigned long i)
{
  char buf[4];

  if (!m_ostream)
    return;

  buf[0] = i & 0xff;
  buf[1] = (i >> 8) & 0xff;
  buf[2] = (i >> 16) & 0xff;
  buf[3] = (i >> 24) & 0xff;
  Write(buf, 4);
}

void wxDataOutputStream::Write16(unsigned short i)
{
  char buf[2];

  if (!m_ostream)
    return;

  buf[0] = i & 0xff;
  buf[1] = (i >> 8) & 0xff;
  Write(buf, 2);
}

void wxDataOutputStream::Write8(unsigned char i)
{
  if (!m_ostream)
    return;

  Write(&i, 1);
}

void wxDataOutputStream::WriteLine(const wxString& line)
{
#ifdef __WXMSW__
  wxString tmp_string = line + "\r\n";
#else
  wxString tmp_string = line + '\n';
#endif

  if (!m_ostream)
    return;

  Write((const char *) tmp_string, tmp_string.Length());
}

void wxDataOutputStream::WriteString(const wxString& string)
{
  if (!m_ostream)
    return;

  Write32(string.Length());
  Write((const char *) string, string.Length());
}

// Must be at global scope for VC++ 5
extern "C" void ConvertToIeeeExtended(double num, unsigned char *bytes);

void wxDataOutputStream::WriteDouble(double d)
{
  char buf[10];

  if (!m_ostream)
    return;

#if USE_APPLE_IEEE
  ConvertToIeeeExtended(d, (unsigned char *)buf);
#else
#	pragma warning "wxDataStream::WriteDouble() not using IeeeExtended - will not work!"
 buf[0] = '\0';
#endif
  Write(buf, 10);
}
