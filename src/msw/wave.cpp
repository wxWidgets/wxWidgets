/////////////////////////////////////////////////////////////////////////////
// Name:        wave.cpp
// Purpose:     wxWave
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wave.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/file.h"
#include "wx/msw/wave.h"
#include "wx/msw/private.h"

#include <windows.h>
#include <windowsx.h>

#ifndef __GNUWIN32__
#include <mmsystem.h>
#endif

#ifdef __GNUWIN32__
#include "wx/msw/gnuwin32/extra.h"
#endif

wxWave::wxWave()
  : m_waveData(NULL), m_waveLength(0), m_isResource(FALSE)
{
}

wxWave::wxWave(const wxString& sFileName, bool isResource)
  : m_waveData(NULL), m_waveLength(0), m_isResource(isResource)
{
  Create(sFileName, isResource);
}

wxWave::wxWave(int size, const byte* data)
  : m_waveData(NULL), m_waveLength(0), m_isResource(FALSE)
{
  Create(size, data);
}

wxWave::~wxWave()
{
  Free();
}

bool wxWave::Create(const wxString& fileName, bool isResource)
{
  Free();

  if (isResource)
  {
    m_isResource = TRUE;

    HRSRC hresInfo;
#ifdef __WIN32__
    hresInfo = ::FindResourceA((HMODULE) wxhInstance, fileName, "WAVE");
#else
    hresInfo = ::FindResource((HMODULE) wxhInstance, fileName, "WAVE");
#endif
    if (!hresInfo)
        return FALSE;

    HGLOBAL waveData = ::LoadResource((HMODULE) wxhInstance, hresInfo);

    if (waveData)
    {
      m_waveData= (byte*)::LockResource(waveData);
      m_waveLength = (int) ::SizeofResource((HMODULE) wxhInstance, hresInfo);
    }

    return (m_waveData ? TRUE : FALSE);
  }
  else
  {
    m_isResource = FALSE;

    wxFile fileWave;
    if (!fileWave.Open(fileName, wxFile::read))
        return FALSE;

    m_waveLength = (int) fileWave.Length();

    m_waveData = (byte*)::GlobalLock(::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, m_waveLength));
    if (!m_waveData)
        return FALSE;

    fileWave.Read(m_waveData, m_waveLength);

    return TRUE;
  }
}

bool wxWave::Create(int size, const byte* data)
{
  Free();
  m_isResource = FALSE;
  m_waveLength=size;
  m_waveData = (byte*)::GlobalLock(::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, m_waveLength));
  if (!m_waveData)
     return FALSE;

  for (int i=0; i<size; i++) m_waveData[i] = data[i];
  return TRUE;
}

bool wxWave::Play(bool async, bool looped) const
{
  if (!IsOk())
    return FALSE;

#ifdef __WIN32__
  return ( ::PlaySound((LPCSTR)m_waveData, NULL, SND_MEMORY |
    SND_NODEFAULT | (async ? SND_ASYNC : SND_SYNC) | (looped ? (SND_LOOP | SND_ASYNC) : 0)) != 0 );
#else
  return ( ::sndPlaySound((LPCSTR)m_waveData, SND_MEMORY |
    SND_NODEFAULT | (async ? SND_ASYNC : SND_SYNC) | (looped ? (SND_LOOP | SND_ASYNC) : 0)) != 0 );
#endif
}

bool wxWave::Free()
{
  if (m_waveData)
  {
#ifdef __WIN32__
    HGLOBAL waveData = ::GlobalHandle(m_waveData);
#else
    HGLOBAL waveData = GlobalPtrHandle(m_waveData);
#endif

    if (waveData)
    {
      if (m_isResource)
        ::FreeResource(waveData);
      else
      {
        ::GlobalUnlock(waveData);
        ::GlobalFree(waveData);
      }

      m_waveData = NULL;
      m_waveLength = 0;
      return TRUE;
    }
  }
  return FALSE;
}


