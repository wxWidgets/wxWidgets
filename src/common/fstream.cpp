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

#include <wx/object.h>
#include <stdio.h>
#include <wx/stream.h>
#include <wx/fstream.h>

#define BUF_TEMP_SIZE 10000

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxFileStreamBase, wxStream)
IMPLEMENT_CLASS(wxFileInputStream, wxFileStreamBase)
IMPLEMENT_CLASS(wxFileOutputStream, wxFileStreamBase)
IMPLEMENT_CLASS(wxFileStream, wxFileStreamBase)
#endif

wxFileStreamBase::wxFileStreamBase(const wxString& fileName, int iolimit)
  : wxStream()
{
  char *open_mode;

  switch (iolimit) {
  case 0:
    open_mode = "a+";
    break;
  case 1:
    open_mode = "rb";
    break;
  case 2: 
    open_mode = "wb";
    break;
  }
  m_file = fopen(fileName, open_mode);
  fseek(m_file, 0, SEEK_SET);

  m_eof = FALSE;
  m_bad = FALSE;
}

wxFileStreamBase::~wxFileStreamBase()
{
  fclose(m_file);
}

wxInputStream& wxFileStreamBase::Read(void *buffer, size_t size)
{
  m_lastread = fread(buffer, 1, size, m_file);
  m_eof = feof(m_file);
  return *this;
}

wxOutputStream& wxFileStreamBase::Write(const void *buffer, size_t size)
{
  m_lastwrite = fwrite(buffer, 1, size, m_file);
  m_bad = ferror(m_file) != 0;
  return *this;
}

size_t wxFileStreamBase::SeekI(int pos, wxWhenceType whence)
{
  int real_whence;

  if (whence == wxBeginPosition)
    real_whence = SEEK_SET;
  else if (whence == wxCurrentPosition)
    real_whence = SEEK_CUR;
  else if (whence == wxEndPosition)
    real_whence = SEEK_END;

  fseek(m_file, pos, real_whence);
  return ftell(m_file);
}

size_t wxFileStreamBase::TellI() const
{
  return ftell(m_file);
}

size_t wxFileStreamBase::TellO() const
{
  return ftell(m_file);
}

size_t wxFileStreamBase::SeekO(int pos, wxWhenceType whence)
{
  return SeekI(pos, whence);
}

void wxFileStreamBase::Sync()
{
  fflush(m_file);
}
