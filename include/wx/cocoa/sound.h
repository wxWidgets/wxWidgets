/////////////////////////////////////////////////////////////////////////////
// Name:        sound.h
// Purpose:     wxSound class (loads and plays short Windows .wav files).
//              Optional on non-Windows platforms.
// Author:      Ryan Norton
// Modified by:
// Created:     2004-10-02
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_SOUND_H_
#define _WX_COCOA_SOUND_H_

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
  bool  IsOk() const { return m_hSnd != NULL; }
  static void  Stop();
  static bool IsPlaying();

  inline WX_NSSound GetNSSound()
  {		return m_hSnd;	}
protected:  
  bool  DoPlay(unsigned flags) const;

private:
    WX_NSSound m_hSnd; //NSSound handle
    wxString m_sndname; //file path
    int m_waveLength; //size of file in memory mode    
    struct objc_object * m_cocoaSoundDelegate;
};

#endif
#endif
    // _WX_COCOA_SOUND_H_
