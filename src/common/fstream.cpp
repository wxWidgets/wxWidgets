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

size_t wxFileInputStream::OnSysRead(void *buffer, size_t size)
{
  return m_file->Read(buffer, size);
}

off_t wxFileInputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
  return m_file->Seek(pos, mode);
}

off_t wxFileInputStream::OnSysTell() const
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

size_t wxFileOutputStream::OnSysWrite(const void *buffer, size_t size)
{
  size_t ret = m_file->Write(buffer, size);
  m_lasterror = wxStream_EOF; // TODO
  return ret;
}

off_t wxFileOutputStream::OnSysTell() const
{
  return m_file->Tell();
}

off_t wxFileOutputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
  return m_file->Seek(pos, mode);
}

void wxFileOutputStream::Sync()
{
  wxOutputStream::Sync();
  m_file->Flush();
}
