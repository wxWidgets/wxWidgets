/////////////////////////////////////////////////////////////////////////////
// Name:        fstream.cpp
// Purpose:     "File stream" classes
// Author:      Julian Smart
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux 
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "fstream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include <stdio.h>
#include <wx/stream.h>
#include <wx/fstream.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// wxFileInputStream
// ----------------------------------------------------------------------------

wxFileInputStream::wxFileInputStream(const wxString& fileName)
  : wxFile(fileName, read)
{
  m_i_streambuf->SetBufferIO(1024);
}

wxFileInputStream::~wxFileInputStream()
{
}

char wxFileInputStream::Peek()
{
  return 0;
}

size_t wxFileInputStream::DoRead(void *buffer, size_t size)
{
  return wxFile::Read(buffer, size);
}

off_t wxFileInputStream::DoSeekInput(off_t pos, wxSeekMode mode)
{
  return wxFile::Seek(pos, mode);
}

off_t wxFileInputStream::DoTellInput() const
{
  return wxFile::Tell();
}

// ----------------------------------------------------------------------------
// wxFileOutputStream
// ----------------------------------------------------------------------------

wxFileOutputStream::wxFileOutputStream(const wxString& fileName)
 : wxFile(fileName, write)
{
  m_o_streambuf->SetBufferIO(1024);
}

wxFileOutputStream::~wxFileOutputStream()
{
  Sync();
}

size_t wxFileOutputStream::DoWrite(const void *buffer, size_t size)
{
  size_t ret = wxFile::Write(buffer, size);
  m_bad = wxFile::Error();
  return ret;
}

off_t wxFileOutputStream::DoTellOutput() const
{
  return wxFile::Tell();
}

off_t wxFileOutputStream::DoSeekOutput(off_t pos, wxSeekMode mode)
{
  return wxFile::Seek(pos, mode);
}

void wxFileOutputStream::Sync()
{
  wxOutputStream::Sync();
  wxFile::Flush();
}

// ----------------------------------------------------------------------------
// wxFileStream
// ----------------------------------------------------------------------------

wxFileStream::wxFileStream(const wxString& fileName)
  : wxFile(fileName, read_write)
{
}

wxFileStream::~wxFileStream()
{
}
