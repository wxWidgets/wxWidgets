// ----------------------------------------------------------------------------
// Name:       vidwin.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    February 1998
// Updated:
// Copyright:  (C) 1998, Guilhem Lavaux
// License:    wxWindows license
// ----------------------------------------------------------------------------

#ifndef __VID_windows_H__
#define __VID_windows_H__

#ifdef __GNUG__
    #pragma interface "vidwin.h"
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
    #include "wx/string.h"
    #include "wx/stream.h"
    #include "wx/window.h"
#endif

// ----------------------------------------------------------------------------
// wxMMedia2 headers

#include "wx/mmedia/vidbase.h"

// ----------------------------------------------------------------------------
// System headers and private types

#ifdef WXMMEDIA_INTERNAL
#include <windows.h>
#include <mmsystem.h>

typedef struct VIDW_Internal {
    MCIDEVICEID m_dev_id;
} wxVIDWinternal;
#endif

// ----------------------------------------------------------------------------
// Class definition

class WXDLLEXPORT wxVideoWindows : public wxVideoBaseDriver {
    DECLARE_DYNAMIC_CLASS(wxVideoWindows)
protected:
    struct VIDW_Internal *m_internal;
    bool m_paused, m_stopped, m_remove_file;
    wxString m_filename;
    double m_frameRate;
    wxUint8 m_bps;

    void OpenFile();
public:
    wxVideoWindows(void);
    wxVideoWindows(wxInputStream& str);
    wxVideoWindows(const wxString& fname);
    ~wxVideoWindows(void);

    bool Play();
    bool Stop();
    bool Pause();
    bool Resume();
    
    bool GetSize(wxSize& size) const;
    bool SetSize(wxSize size);

    // Return codec name for each stream.
    wxString GetMovieCodec() const;
    wxString GetAudioCodec() const;
    // Return misc. info about audio
    wxUint32 GetSampleRate() const;
    wxUint8 GetChannels() const;
    wxUint8 GetBPS() const;
    // Return the frame rate of the video (in frames/second)
    double GetFrameRate() const;
    // Return the total number of frames in the movie
    wxUint32 GetNbFrames() const;
    
    bool IsCapable(wxVideoType v_type);
    
    bool AttachOutput(wxWindow& output);
    void DetachOutput(void);
    
    bool IsPaused() const;
    bool IsStopped() const;
};

#endif
