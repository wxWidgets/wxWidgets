// /////////////////////////////////////////////////////////////////////////////
// Name:       vidxanm.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __VID_xanim_H__
#define __VID_xanim_H__

#ifdef __GNUG__
    #pragma interface "vidxanm.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/string.h"
    #include "wx/process.h"
#endif

// ----------------------------------------------------------------------------
// System dependent headers

#if defined(WXMMEDIA_INTERNAL) && (defined(__X__) || defined(__WXGTK__))
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

// ----------------------------------------------------------------------------
// wxMMedia2 headers

#include "wx/mmedia/vidbase.h"

// ----------------------------------------------------------------------------
// Internal types

#ifdef WXMMEDIA_INTERNAL
typedef struct wxXANIMinternal {
    Display *xanim_dpy;
    Window xanim_window;
    Atom xanim_atom, xanim_ret;
} wxXANIMinternal;

#ifndef __XANIM_COMMAND__
    #define __XANIM_COMMAND__ "/usr/X11R6/bin/xanim"
#endif

#endif

// ----------------------------------------------------------------------------
// Class definition

class WXDLLEXPORT wxVideoXANIM : public wxVideoBaseDriver {
    DECLARE_DYNAMIC_CLASS(wxVideoXANIM)
 protected:
    // Remember the state of the subprocess
    bool m_xanim_started, m_paused;
    // Pure X11 variables
    struct wxXANIMinternal *m_internal;
    wxString m_filename;
    wxProcess *m_xanim_detector;
    // Remember to delete the temporary file when necessary
    bool m_remove_file;
    wxUint32 m_size[2];
    wxUint32 m_sampleRate;
    wxUint8 m_channels;
    wxUint8 m_bps;
    wxUint32 m_frames;
    double m_frameRate;
    wxString m_movieCodec, m_audioCodec;
    
 public:
    wxVideoXANIM();
    wxVideoXANIM(wxInputStream& str);
    wxVideoXANIM(const wxString& filename);
    ~wxVideoXANIM();
    
    bool Play();
    bool Pause();
    bool Resume();
    bool Stop();
    
    bool SetVolume(wxUint8 vol);
    bool SetSize(wxSize size);
    bool GetSize(wxSize& size) const;

    // Return the video codec name
    wxString GetMovieCodec() const;
    // Return the audio codec name
    wxString GetAudioCodec() const;
    // Return misc info about audio
    wxUint32 GetSampleRate() const;
    wxUint8 GetChannels() const;
    wxUint8 GetBPS() const;
    // Return frame rate
    double GetFrameRate() const;
    // Return number of frames in the movie
    wxUint32 GetNbFrames() const;
    
    bool IsCapable(wxVideoType v_type) const;
    
    bool AttachOutput(wxWindow& output);
    void DetachOutput();
    
    bool IsPaused() const;
    bool IsStopped() const;
    
    friend class wxVideoXANIMProcess;
    
protected:
    // Start the subprocess with the right parameters
    bool RestartXANIM();
    // Send a command to the subprocess
    bool SendCommand(const char *command,char **ret = NULL,
                     wxUint32 *size = NULL);

    // Collect informations from XAnim
    bool CollectInfo();
};

#endif
