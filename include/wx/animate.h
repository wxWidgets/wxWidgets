/////////////////////////////////////////////////////////////////////////////
// Name:        wx/animate.h
// Purpose:     wxAnimation and wxAnimationCtrl
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ANIMATEH__
#define _WX_ANIMATEH__

#include "wx/defs.h"

#if wxUSE_ANIMATIONCTRL

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/list.h>
#include <wx/timer.h>
#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/control.h>
#include <wx/animdecod.h>

class WXDLLIMPEXP_ADV wxAnimation;

extern WXDLLEXPORT_DATA(wxAnimation) wxNullAnimation;
extern WXDLLEXPORT_DATA(const wxChar) wxAnimationCtrlNameStr[];


// ----------------------------------------------------------------------------
// wxAnimationBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAnimationBase : public wxGDIObject
{
public:
    wxAnimationBase() {}

    virtual bool IsOk() const = 0;

    // can be -1
    virtual int GetDelay(size_t i) const = 0;

    virtual size_t GetFrameCount() const = 0;
    virtual wxImage GetFrame(size_t i) const = 0;
    virtual wxSize GetSize() const = 0;

    virtual bool LoadFile(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;
    virtual bool Load(wxInputStream &stream, wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

protected:
    DECLARE_ABSTRACT_CLASS(wxAnimationBase)
};



// ----------------------------------------------------------------------------
// wxAnimationCtrlBase
// ----------------------------------------------------------------------------

// do not autoresize to the animation's size when SetAnimation() is called
#define wxAC_NO_AUTORESIZE       (0x0010)

// default style does not include wxAC_NO_AUTORESIZE, that is, the control
// auto-resizes by default to fit the new animation when SetAnimation() is called
#define wxAC_DEFAULT_STYLE       (wxNO_BORDER)


class WXDLLIMPEXP_ADV wxAnimationCtrlBase : public wxControl
{
public:
    wxAnimationCtrlBase() {}

public:     // public API

    virtual bool LoadFile(const wxString& filename, wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

    virtual void SetAnimation(const wxAnimation &anim) = 0;
    virtual wxAnimation GetAnimation() const = 0;

    virtual bool Play() = 0;
    virtual void Stop() = 0;

    virtual bool IsPlaying() const = 0;

private:
    DECLARE_ABSTRACT_CLASS(wxAnimationCtrlBase)
};


// ----------------------------------------------------------------------------
// include the platform-specific version of the wxAnimationCtrl class
// ----------------------------------------------------------------------------

#if defined(__WXGTK__)
    #include "wx/gtk/animate.h"
#else
    #include "wx/generic/animate.h"
#endif

#endif      // wxUSE_ANIMATIONCTRL

#endif // _WX_ANIMATEH__
