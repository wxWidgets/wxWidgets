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
  : wxInputStream()
{
  m_file = new wxFile(fileName, wxFile::read);
  m_file_destroy = TRUE;
  m_i_streambuf->SetBufferIO(1024);
}

wxFileInputStream::wxFileInputStream()
  : wxInputStream()
{
  m_file_destroy = FALSE;
  m_file = NULL;
}

wxFileInputStream::~wxFileInputStream()
{
  if (m_file_destroy)
    delete m_file;
}

char wxFileInputStream::Peek()
{
  return 0;
}

size_t wxFileInputStream::DoRead(void *buffer, size_t size)
{
  return m_file->Read(buffer, size);
}

off_t wxFileInputStream::DoSeekInput(off_t pos, wxSeekMode mode)
{
  return m_file->Seek(pos, mode);
}

off_t wxFileInputStream::DoTellInput() const
{
  return m_file->Tell();
}

// ----------------------------------------------------------------------------
// wxFileOutputStream
// ----------------------------------------------------------------------------

wxFileOutputStream::wxFileOutputStream(const wxString& fileName)
{
  m_file = new wxFile(fileName, wxFile::write);
  m_file_destroy = TRUE;
  m_o_streambuf->SetBufferIO(1024);
}

wxFileOutputStream::wxFileOutputStream()
  : wxOutputStream()
{
  m_o_streambuf->SetBufferIO(1024);
  m_file_destroy = FALSE;
  m_file = NULL;
}

wxFileOutputStream::~wxFileOutputStream()
{
  if (m_file_destroy) {
    Sync();
    delete m_file;
  }
}

size_t wxFileOutputStream::DoWrite(const void *buffer, size_t size)
{
  size_t ret = m_file->Write(buffer, size);
  m_bad = m_file->Error();
  return ret;
}

off_t wxFileOutputStream::DoTellOutput() const
{
  return m_file->Tell();
}

off_t wxFileOutputStream::DoSeekOutput(off_t pos, wxSeekMode mode)
{
  return m_file->Seek(pos, mode);
}

void wxFileOutputStream::Sync()
{
  wxOutputStream::Sync();
  m_file->Flush();
}

// ----------------------------------------------------------------------------
// wxFileStream
// ----------------------------------------------------------------------------

wxFileStream::wxFileStream(const wxString& fileName)
  : wxFileInputStream(), wxFileOutputStream()
{
  m_file = new wxFile(fileName, wxFile::read_write);
  // Reread the initial buffer.
  m_i_streambuf->SetBufferIO(1024);
}

wxFileStream::~wxFileStream()
{
  Sync();
  delete m_file;
}
