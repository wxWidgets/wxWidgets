/////////////////////////////////////////////////////////////////////////////
// Name:        wx/sound.h
// Purpose:     wxSoundBase class
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/02/01
// RCS-ID:      $Id$
// Copyright:   (c) 2004, Vaclav Slavik
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SOUND_H_BASE_
#define _WX_SOUND_H_BASE_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "soundbase.h"
#endif

#include "wx/object.h"

// ----------------------------------------------------------------------------
// wxSoundBase: common wxSound code and interface
// ----------------------------------------------------------------------------

// Flags for wxSound::Play
enum wxSoundFlags
{
    wxSOUND_SYNC   = 0,
    wxSOUND_ASYNC  = 1,
    wxSOUND_LOOP   = 2
};

class wxSoundBase : public wxObject
{
public:
    // Play the sound:
    bool Play(unsigned flags = wxSOUND_ASYNC)
    {
        return DoPlay(flags);
    }
#if WXWIN_COMPATIBILITY_2_4
    wxDEPRECATED( bool Play(bool async = true, bool looped = false) );
#endif

protected:
    virtual bool DoPlay(unsigned flags) = 0;
};
    
#if WXWIN_COMPATIBILITY_2_4
inline bool wxSoundBase::Play(bool async, bool looped)
{
    unsigned flags = 0;
    if (async) flags |= wxSOUND_ASYNC;
    if (looped) flags |= wxSOUND_LOOP | wxSOUND_ASYNC;
    return DoPlay(flags);
}
#endif

// ----------------------------------------------------------------------------
// wxSound class implementation
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
#include "wx/msw/wave.h"
#elif defined(__UNIX__)
#include "wx/unix/sound.h"
#elif defined(__WXMAC__)
#include "wx/mac/wave.h"
#elif defined(__WXPM__)
#include "wx/os2/wave.h"
#endif

// wxSound used to be called wxWave before wxWindows 2.5.1:
#ifdef __UNIX__ // FIXME: on all platforms when everything is renamed
    #if WXWIN_COMPATIBILITY_2_4
        typedef wxSound wxWave;
    #endif
#else
    typedef wxWave wxSound;
#endif

#endif // _WX_SOUND_H_BASE_
