////////////////////////////////////////////////////////////////////////////////
// Name:       mmfile.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation
#endif
#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include <wx/stream.h>
#include <wx/fstream.h>
#include <wx/mstream.h>

#include "mmfile.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxMMediaFile::wxMMediaFile()
  : m_istream(NULL), m_i_temp(NULL), m_ostream(NULL), m_o_temp(NULL),
    m_tmpfname((char *)NULL), m_mfname((char *)NULL),
    m_seekable(FALSE)
{
}

wxMMediaFile::wxMMediaFile(wxOutputStream& os, bool seekable)
  : m_istream(NULL), m_i_temp(NULL), m_ostream(&os), m_o_temp(NULL),
    m_tmpfname((char *)NULL), m_mfname((char *)NULL),
    m_seekable(seekable)
{
}

wxMMediaFile::wxMMediaFile(wxInputStream& is, bool preload, bool seekable)
  : m_istream(&is), m_i_temp(NULL), m_ostream(NULL), m_o_temp(NULL),
    m_tmpfname((char *)NULL), m_mfname((char *)NULL),
    m_seekable(seekable)
{
/*
  if (preload) {
    wxMemoryStream *tmpstream = new wxMemoryStream();

    m_o_temp = tmpstream;
    m_i_temp = tmpstream;

    m_o_temp->Write(is);
  }
*/
}

wxMMediaFile::wxMMediaFile(const wxString& fname)
  : m_istream(NULL), m_i_temp(NULL), m_ostream(NULL), m_o_temp(NULL),
    m_tmpfname((char *)NULL), m_mfname(fname),
    m_seekable(TRUE)
{
  wxFileStream *s = new wxFileStream(fname);

  m_mfname = fname;
  m_istream = s;
  m_ostream = s;
}

void wxMMediaFile::SetFile(wxInputStream& str, bool preload, bool seekable)
{
  CleanUpPrevious();
  m_istream = &str;
  m_ostream = NULL;
  m_seekable = seekable;

/*
  if (preload) {
    wxMemoryStream *tmpstream = new wxMemoryStream();

    m_i_temp = tmpstream;
    m_o_temp = tmpstream;

    m_o_temp->Write(str);
  }
*/
}

void wxMMediaFile::SetFile(wxOutputStream& str, bool seekable)
{
  CleanUpPrevious();
  m_ostream = &str;
  m_istream = NULL;
  m_seekable = seekable;
}

void wxMMediaFile::SetFile(const wxString& fname)
{
  CleanUpPrevious();

  m_mfname = fname;
  wxFileStream *f = new wxFileStream(fname);

  SetFile(*f, FALSE, TRUE);
}

void wxMMediaFile::CleanUpPrevious()
{
  if (m_i_temp) {
    m_i_temp->SeekI(0);

    if (m_ostream)
      m_ostream->Write(*m_i_temp);

    delete m_i_temp; // Only one delete because m_tmpo* and m_tmpi* are linked

    if (m_tmpfname)
      wxRemoveFile(m_tmpfname);
  }

  if (!m_mfname.IsNull() && m_mfname != "")
    delete m_istream;

  m_i_temp = NULL;
  m_o_temp = NULL;
  m_istream = NULL;
  m_ostream = NULL;
}

wxMMediaFile::~wxMMediaFile()
{
  CleanUpPrevious();
}

wxMMtime wxMMediaFile::GetLength()
{
  wxMMtime mm_time = {-1, 0, 0};
  return mm_time;
}

wxMMtime wxMMediaFile::GetPosition()
{
  wxMMtime mm_time = {0, 0, 0};
  return mm_time;
}

wxInputStream *wxMMediaFile::GetIRawData()
{
  if (!m_istream)
    return NULL;

  if (!m_seekable && !m_i_temp)
    CacheIO();

  return (m_i_temp) ? m_i_temp : m_istream;
}

wxOutputStream *wxMMediaFile::GetORawData()
{
  if (!m_ostream)
    return NULL;

  if (!m_seekable && !m_i_temp)
    CacheIO();

  return (m_o_temp) ? m_o_temp : m_ostream;
}

wxString wxMMediaFile::GetCurrentFile()
{
  if (!m_istream && !m_ostream)
    return wxString((char *)NULL);

  if (!m_mfname && !m_i_temp) 
    CacheIO();

  return (!m_tmpfname.IsEmpty()) ? m_tmpfname : m_mfname;
}

void wxMMediaFile::CacheIO()
{
  if ((!m_istream && !m_ostream) || m_i_temp)
    return;

  m_tmpfname = wxGetTempFileName("mmd");

  wxFileStream *tmpstream = new wxFileStream(m_tmpfname);
  m_i_temp = tmpstream;
  m_o_temp = tmpstream;

  if (m_istream)
    tmpstream->wxOutputStream::Write(*m_istream);
}
