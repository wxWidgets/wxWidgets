// /////////////////////////////////////////////////////////////////////////////
// Name:       vidbase.h
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// CVS:	       $Id$
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
/* Real -*- C++ -*- */
#ifndef __VID_bdrv_H__
#define __VID_bdrv_H__

#ifdef __GNUG__
#pragma interface "vidbase.h"
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
    #include "wx/stream.h"
    #include "wx/string.h"
    #include "wx/window.h"
    #include "wx/frame.h"
#endif

// ----------------------------------------------------------------------------
// wxMMedia2 (video) types

typedef enum {
  wxVIDEO_MSAVI,
  wxVIDEO_MPEG,
  wxVIDEO_QT,
  wxVIDEO_GIF,
  wxVIDEO_JMOV,
  wxVIDEO_FLI,
  wxVIDEO_IFF,
  wxVIDEO_SGI,
  wxVIDEO_MPEG2
} wxVideoType;

// ----------------------------------------------------------------------------
// Classes definition

class WXDLLEXPORT wxVideoBaseDriver : public wxObject {
    DECLARE_ABSTRACT_CLASS(wxVideoBaseDriver)
protected:
    wxWindow *m_video_output;
public:
    // Ctors
    wxVideoBaseDriver();
    wxVideoBaseDriver(wxInputStream& str);
    wxVideoBaseDriver(const wxString& filename);
    // Dtor
    virtual ~wxVideoBaseDriver();
    
    // Usual functions ... They all return FALSE in case of errors.
    virtual bool Play() = 0;
    virtual bool Stop() = 0;
    virtual bool Pause() = 0;
    virtual bool Resume() = 0;
    
    // Size management
    virtual bool SetSize(wxSize size) = 0;
    virtual bool GetSize(wxSize& size) const = 0;
    
    // Test the capability of the driver to handle the specified type
    virtual bool IsCapable(wxVideoType WXUNUSED(v_type)) const { return FALSE; }

    // Return the video codec name
    virtual wxString GetMovieCodec() const = 0;
    // Return the audio codec name
    virtual wxString GetAudioCodec() const = 0;
    // Return misc info about audio
    virtual wxUint32 GetSampleRate() const = 0;
    virtual wxUint8 GetChannels() const = 0;
    virtual wxUint8 GetBPS() const = 0;
    // Return frame rate
    virtual double GetFrameRate() const = 0;
    // Return number of frames
    virtual wxUint32 GetNbFrames() const = 0;
    
    // Called when the movie finished
    virtual void OnFinished() {}
    
    // Attaches the video output to a window. The video will be shown in that window.
    virtual bool AttachOutput(wxWindow& output);
    virtual void DetachOutput();
    
    // They return the state of the movie.
    virtual bool IsPaused() const = 0;
    virtual bool IsStopped() const = 0;
};

WXDLLEXPORT wxFrame *wxVideoCreateFrame(wxVideoBaseDriver *vid_drv);


#endif
