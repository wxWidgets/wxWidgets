/////////////////////////////////////////////////////////////////////////////
// Name:        sound.cpp
// Purpose:     wxSound
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "sound.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#if wxUSE_SOUND

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/file.h"
#include "wx/sound.h"
#include "wx/msw/private.h"

#include <windowsx.h>

#if defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__)
    #include "wx/msw/gnuwin32/extra.h"
#else
    #include <mmsystem.h>
#endif

wxSound::wxSound()
  : m_waveData(NULL), m_waveLength(0), m_isResource(false)
{
}

wxSound::wxSound(const wxString& sFileName, bool isResource)
  : m_waveData(NULL), m_waveLength(0), m_isResource(isResource)
{
  Create(sFileName, isResource);
}

wxSound::wxSound(int size, const wxByte* data)
  : m_waveData(NULL), m_waveLength(0), m_isResource(false)
{
  Create(size, data);
}

wxSound::~wxSound()
{
  Free();
}

bool wxSound::Create(const wxString& fileName, bool isResource)
{
  Free();

  if (isResource)
  {
    m_isResource = true;

    HRSRC hresInfo;
    hresInfo = ::FindResource((HMODULE) wxhInstance, fileName, wxT("WAVE"));
    if (!hresInfo)
        return false;

    HGLOBAL waveData = ::LoadResource((HMODULE) wxhInstance, hresInfo);

    if (waveData)
    {
      m_waveData= (wxByte*)::LockResource(waveData);
      m_waveLength = (int) ::SizeofResource((HMODULE) wxhInstance, hresInfo);
    }

    return (m_waveData ? true : false);
  }
  else
  {
    m_isResource = false;

    wxFile fileWave;
    if (!fileWave.Open(fileName, wxFile::read))
        return false;

    m_waveLength = (int) fileWave.Length();

    m_waveData = (wxByte*)GlobalLock(GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, m_waveLength));
    if (!m_waveData)
        return false;

    fileWave.Read(m_waveData, m_waveLength);

    return true;
  }
}

bool wxSound::Create(int size, const wxByte* data)
{
  Free();
  m_isResource = true;
  m_waveLength=size;
  m_waveData = (wxByte*)GlobalLock(GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, m_waveLength));
  if (!m_waveData)
     return false;

  for (int i=0; i<size; i++) m_waveData[i] = data[i];
  return true;
}

bool wxSound::DoPlay(unsigned flags) const
{
  if (!IsOk())
    return false;

  return (::PlaySound((LPCTSTR)m_waveData, NULL,
                      SND_MEMORY | SND_NODEFAULT |
                      ((flags & wxSOUND_ASYNC) ? SND_ASYNC : SND_SYNC) |
                      ((flags & wxSOUND_LOOP) ? (SND_LOOP | SND_ASYNC) : 0))
          != 0);
}

bool wxSound::Free()
{
  if (m_waveData)
  {
#ifdef __WXWINCE__
    HGLOBAL waveData = (HGLOBAL) m_waveData;
#else
    HGLOBAL waveData = GlobalHandle(m_waveData);
#endif

    if (waveData)
    {
#ifndef __WXWINCE__
        if (m_isResource)
        ::FreeResource(waveData);
      else
#endif
      {
        GlobalUnlock(waveData);
        GlobalFree(waveData);
      }

      m_waveData = NULL;
      m_waveLength = 0;
      return true;
    }
  }
  return false;
}

/*static*/ void wxSound::Stop()
{
    ::PlaySound(NULL, NULL, 0);
}

#endif // wxUSE_SOUND
