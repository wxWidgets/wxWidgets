/////////////////////////////////////////////////////////////////////////////
// Name:        wave.h
// Purpose:     wxWave class (loads and plays short Windows .wav files).
//              Optional on non-Windows platforms.
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WAVE_H_
#define _WX_WAVE_H_

#include "wx/object.h"

class wxWave : public wxObject
{
public:
  wxWave();
  wxWave(const wxString& fileName, bool isResource = FALSE);
  wxWave(int size, const wxByte* data);
  ~wxWave();

public:
  // Create from resource or file
  bool  Create(const wxString& fileName, bool isResource = FALSE);
  // Create from data
  bool Create(int size, const wxByte* data);

  bool  IsOk() const { return (m_waveData ? TRUE : FALSE); };
  bool  Play(bool async = TRUE, bool looped = FALSE) const;

protected:
  bool  Free();

private:
  wxByte* m_waveData;
  int   m_waveLength;
  bool  m_isResource;
};

#endif
    // _WX_WAVE_H_
