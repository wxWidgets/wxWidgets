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

#define BUF_TEMP_SIZE 10000

// ----------------------------------------------------------------------------
// wxFileInputStream
// ----------------------------------------------------------------------------

wxFileInputStream::wxFileInputStream(const wxString& fileName)
  : wxFile(fileName, read)
{
  m_lastread = 0;
}

wxFileInputStream::~wxFileInputStream()
{
}

wxInputStream& wxFileInputStream::Read(void *buffer, size_t size)
{
  m_lastread = wxFile::Read(buffer, size);
  return *this;
}

off_t wxFileInputStream::SeekI(off_t pos, wxSeekMode mode)
{
  return wxFile::Seek(pos, mode);
}

off_t wxFileInputStream::TellI() const
{
  return wxFile::Tell();
}

// ----------------------------------------------------------------------------
// wxFileOutputStream
// ----------------------------------------------------------------------------

wxFileOutputStream::wxFileOutputStream(const wxString& fileName)
 : wxFile(fileName, write)
{
  m_lastwrite = 0;
}

wxFileOutputStream::~wxFileOutputStream()
{
}

wxOutputStream& wxFileOutputStream::Write(const void *buffer, size_t size)
{
  m_lastwrite = wxFile::Write(buffer, size);
  m_bad = wxFile::Error();
  return *this;
}

off_t wxFileOutputStream::TellO() const
{
  return wxFile::Tell();
}

off_t wxFileOutputStream::SeekO(off_t pos, wxSeekMode mode)
{
  return wxFile::Seek(pos, mode);
}

void wxFileOutputStream::Sync()
{
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
