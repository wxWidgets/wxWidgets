/////////////////////////////////////////////////////////////////////////////
// Name:        sound.h
// Purpose:     wxSound class (loads and plays short Windows .wav files).
//              Optional on non-Windows platforms.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SOUND_H_
#define _WX_SOUND_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "sound.h"
#endif

#if wxUSE_SOUND

#include "wx/object.h"

class WXDLLEXPORT wxSound : public wxSoundBase
{
public:
  wxSound();
  wxSound(const wxString& fileName, bool isResource = FALSE);
  wxSound(int size, const wxByte* data);
  ~wxSound();

public:
  bool  Create(const wxString& fileName, bool isResource = FALSE);
  bool  IsOk() const { return !m_sndname.IsEmpty(); }

protected:
  // prevent collision with some BSD definitions of macro Free()
  bool  FreeData();
  
  bool  DoPlay(unsigned flags) const;

private:
  void* m_sndChan;

  wxString m_sndname;
  void* m_hSnd;
  int   m_waveLength;
  bool  m_isResource;
};

#endif
#endif
    // _WX_SOUND_H_
