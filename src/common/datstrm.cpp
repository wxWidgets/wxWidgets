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

wxDataStream::wxDataStream(istream& s)
{
  m_istream = &s;
  m_ostream = NULL;
}

wxDataStream::wxDataStream(ostream& s)
{
  m_istream = NULL;
  m_ostream = &s;
}

wxDataStream::wxDataStream(iostream& s)
{
  m_istream = &s;
  m_ostream = &s;
}

wxDataStream::~wxDataStream()
{
}

unsigned long wxDataStream::Read32()
{
  char buf[4];

  if (!m_istream)
    return 0;

  m_istream->read(buf, 4);

  return (unsigned long)buf[0] |
         ((unsigned long)buf[1] << 8) |
         ((unsigned long)buf[2] << 16) |
         ((unsigned long)buf[3] << 24);
}

unsigned short wxDataStream::Read16()
{
  char buf[2];

  if (!m_istream)
    return 0;

  m_istream->read(buf, 2);

  return (unsigned short)buf[0] |
         ((unsigned short)buf[1] << 8);
}

unsigned char wxDataStream::Read8()
{
  char buf;

  if (!m_istream)
    return 0;

  m_istream->read(&buf, 1);
  return (unsigned char)buf;
}

// Must be at global scope for VC++ 5
extern "C" double ConvertFromIeeeExtended(const unsigned char *bytes);

double wxDataStream::ReadDouble()
{
#if USE_APPLE_IEEE
  char buf[10];

  if (!m_istream)
    return 0.0;

  m_istream->read(buf, 10);
  return ConvertFromIeeeExtended((unsigned char *)buf);
#else
  return 0.0;
#endif
}

wxString wxDataStream::ReadLine()
{
  char i_strg[255];

  if (!m_istream)
    return "";

  m_istream->getline(i_strg, 255);
  return i_strg;
}

wxString wxDataStream::ReadString()
{
  wxString wx_string;
  char *string;
  unsigned long len;

  if (!m_istream)
    return "";

  len = Read32();
  string = new char[len+1];

  m_istream->read(string, len);

  string[len] = 0;
  wx_string = string;
  delete string;

  return wx_string; 
}

void wxDataStream::Write32(unsigned long i)
{
  char buf[4];

  if (!m_ostream)
    return;

  buf[0] = i & 0xff;
  buf[1] = (i >> 8) & 0xff;
  buf[2] = (i >> 16) & 0xff;
  buf[3] = (i >> 24) & 0xff;
  m_ostream->write(buf, 4);
}

void wxDataStream::Write16(unsigned short i)
{
  char buf[2];

  if (!m_ostream)
    return;

  buf[0] = i & 0xff;
  buf[1] = (i >> 8) & 0xff;
  m_ostream->write(buf, 2);
}

void wxDataStream::Write8(unsigned char i)
{
  if (!m_ostream)
    return;

  m_ostream->write(&i, 1);
}

void wxDataStream::WriteLine(const wxString& line)
{
#ifdef __WINDOWS__
  wxString tmp_string = line + "\r\n";
#else
  wxString tmp_string = line + '\n';
#endif

  if (!m_ostream)
    return;

  m_ostream->write((const char *) tmp_string, tmp_string.Length());
}

void wxDataStream::WriteString(const wxString& string)
{
  if (!m_ostream)
    return;

  Write32(tmp_string.Length());
  m_ostream->write((const char *) tmp_string, tmp_string.Length());
}

// Must be at global scope for VC++ 5
extern "C" void ConvertToIeeeExtended(double num, unsigned char *bytes);

void wxDataStream::WriteDouble(double d)
{
  char buf[10];

  if (!m_ostream)
    return;

  ConvertToIeeeExtended(d, (unsigned char *)buf);
  m_ostream->write(buf, 10);
}
