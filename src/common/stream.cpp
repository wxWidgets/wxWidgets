/////////////////////////////////////////////////////////////////////////////
// Name:        stream.cpp
// Purpose:     wxStream base classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include <wx/stream.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxInputStream::wxInputStream()
{
}

wxInputStream::~wxInputStream()
{
}

#define BUF_TEMP_SIZE 10000

wxInputStream& wxInputStream::Read(wxOutputStream& stream_out)
{
  char buf[BUF_TEMP_SIZE]; 
  size_t bytes_read = BUF_TEMP_SIZE;

  while (bytes_read == BUF_TEMP_SIZE && !stream_out.Bad()) {
    bytes_read = Read(buf, bytes_read).LastRead();

    stream_out.Write(buf, bytes_read);
  }
  return *this;
}

wxOutputStream::wxOutputStream()
{
}

wxOutputStream::~wxOutputStream()
{
}

wxOutputStream& wxOutputStream::Write(wxInputStream& stream_in)
{
   stream_in.Read(*this);
   return *this;
}

wxFilterInputStream::wxFilterInputStream(wxInputStream& stream)
  : wxInputStream()
{
  m_parent_i_stream = &stream;
}

wxFilterInputStream::~wxFilterInputStream()
{
}

wxFilterOutputStream::wxFilterOutputStream(wxOutputStream& stream)
  : wxOutputStream()
{
  m_parent_o_stream = &stream;
}

wxFilterOutputStream::~wxFilterOutputStream()
{
}
