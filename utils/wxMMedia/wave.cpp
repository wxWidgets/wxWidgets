/////////////////////////////////////////////////////////////////////////////
// Name:        wave.cpp
// Purpose:     wxWave class
// Author:      Guilhem Lavaux / API by Julian Smart
// Modified by:
// Created:     04/23/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "wave.h"
#endif

#include <wx/wfstream.h>
#include "wave.h"

wxWave::wxWave()
{
  m_wave = NULL;
  m_iowave = NULL;
}

wxWave::wxWave(const wxString& fileName, bool isResource = FALSE)
{
  Create(fileName, isResource);
}

wxWave::~wxWave()
{
  Free();
}

bool wxWave::Create(const wxString& sFileName, bool isResource = FALSE)
{
  m_iowave = new wxFileInputStream(sFileName);
  m_wave = new wxSndWavCodec(*m_iowave);

  return TRUE;
}

bool wxWave::Play(bool async, bool looped) const
{
  if (!m_wave)
    return FALSE;

  if (looped)
    m_wave->Set(wxSND_LOOP);
  if (!m_wave->StartPlay());
    return FALSE;
  if (!async)
    m_wave->Wait();

  m_wave->Clear(wxSND_LOOP);
  return TRUE;
}

bool wxWave::Free()
{
  if (m_wave) {
    delete m_wave;
    delete m_iowave;
  }
  return TRUE;
}
