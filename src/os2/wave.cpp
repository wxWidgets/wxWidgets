/////////////////////////////////////////////////////////////////////////////
// Name:        wave.cpp
// Purpose:     wxWave class implementation: optional
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/file.h"
#include "wx/os2/wave.h"
#include "wx/os2/private.h"

#define INCL_DOS
#define INCL_PM
#include <os2.h>
#ifdef RECT
    #undef RECT
#endif
#include <mmio.h>
#include <mmsystem.h>

wxWave::wxWave()
  : m_waveData(NULL), m_waveLength(0), m_isResource(FALSE)
{
}

wxWave::wxWave(const wxString& sFileName, bool isResource)
  : m_waveData(NULL), m_waveLength(0), m_isResource(isResource)
{
  Create(sFileName, isResource);
}

wxWave::wxWave(int size, const wxByte* data)
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
// TODO:
/*
    HRSRC hresInfo;
#ifdef _UNICODE
    hresInfo = ::FindResourceW((HMODULE) wxhInstance, fileName, wxT("WAVE"));
#else
    hresInfo = ::FindResourceA((HMODULE) wxhInstance, fileName, wxT("WAVE"));
#endif
    if (!hresInfo)
        return FALSE;

    HGLOBAL waveData = ::LoadResource((HMODULE) wxhInstance, hresInfo);

    if (waveData)
    {
      m_waveData= (wxByte*)::LockResource(waveData);
      m_waveLength = (int) ::SizeofResource((HMODULE) wxhInstance, hresInfo);
    }

    return (m_waveData ? TRUE : FALSE);
*/
    return FALSE;
  }
  else
  {
    m_isResource = FALSE;

    wxFile fileWave;
    if (!fileWave.Open(fileName, wxFile::read))
        return FALSE;

    m_waveLength = (int) fileWave.Length();
// TODO:
/*
    m_waveData = (wxByte*)::GlobalLock(::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, m_waveLength));
    if (!m_waveData)
        return FALSE;

    fileWave.Read(m_waveData, m_waveLength);
*/
    return TRUE;
  }
}

bool wxWave::Create(int size, const wxByte* data)
{
  Free();
  m_isResource = FALSE;
  m_waveLength=size;
  m_waveData = NULL; // (wxByte*)::GlobalLock(::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, m_waveLength));
  if (!m_waveData)
     return FALSE;

  for (int i=0; i<size; i++) m_waveData[i] = data[i];
  return TRUE;
}

bool wxWave::Play(bool async, bool looped) const
{
  if (!IsOk())
    return FALSE;
// TODO:
/*
  return ( ::PlaySound((LPCTSTR)m_waveData, NULL, SND_MEMORY |
    SND_NODEFAULT | (async ? SND_ASYNC : SND_SYNC) | (looped ? (SND_LOOP | SND_ASYNC) : 0)) != 0 );
*/
    return FALSE;
}

bool wxWave::Free()
{
  if (m_waveData)
  {
//  HGLOBAL waveData = ::GlobalHandle(m_waveData);

// TODO:
/*
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
*/
  }
  return FALSE;
}

