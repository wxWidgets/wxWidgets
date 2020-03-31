/////////////////////////////////////////////////////////////////////////////
// Name:        wx/animate.h
// Purpose:     wxAnimation and wxAnimationCtrl
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ANIMATE_H_
#define _WX_ANIMATE_H_

#include "wx/defs.h"

#if wxUSE_ANIMATIONCTRL

#include "wx/animdecod.h"
#include "wx/control.h"
#include "wx/timer.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_FWD_CORE wxAnimation;
class WXDLLIMPEXP_FWD_CORE wxGenericAnimation;

extern WXDLLIMPEXP_DATA_CORE(wxAnimation) wxNullAnimation;
extern WXDLLIMPEXP_DATA_CORE(wxGenericAnimation) wxNullGenericAnimation;
extern WXDLLIMPEXP_DATA_CORE(const char) wxAnimationCtrlNameStr[];


// ----------------------------------------------------------------------------
// wxAnimation
// ----------------------------------------------------------------------------

WX_DECLARE_LIST_WITH_DECL(wxAnimationDecoder, wxAnimationDecoderList, class WXDLLIMPEXP_ADV);

// ----------------------------------------------------------------------------
// wxAnimationBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnimationBase : public wxObject
{
public:
    wxAnimationBase() {}

    virtual bool IsOk() const = 0;

    // can be -1
    virtual int GetDelay(unsigned int frame) const = 0;

    virtual unsigned int GetFrameCount() const = 0;
    virtual wxImage GetFrame(unsigned int frame) const = 0;
    virtual wxSize GetSize() const = 0;

    virtual bool LoadFile(const wxString& name,
                          wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

    // extended interface used only by the generic implementation of wxAnimationCtrl
    virtual wxPoint GetFramePosition(unsigned int frame) const = 0;
    virtual wxSize GetFrameSize(unsigned int frame) const = 0;
    virtual wxAnimationDisposal GetDisposalMethod(unsigned int frame) const = 0;
    virtual wxColour GetTransparentColour(unsigned int frame) const = 0;
    virtual wxColour GetBackgroundColour() const = 0;

protected:
    wxDECLARE_ABSTRACT_CLASS(wxAnimationBase);
};



// ----------------------------------------------------------------------------
// wxAnimationCtrlBase
// ----------------------------------------------------------------------------

// do not autoresize to the animation's size when SetAnimation() is called
#define wxAC_NO_AUTORESIZE       (0x0010)

// default style does not include wxAC_NO_AUTORESIZE, that is, the control
// auto-resizes by default to fit the new animation when SetAnimation() is called
#define wxAC_DEFAULT_STYLE       (wxBORDER_NONE)

class WXDLLIMPEXP_CORE wxAnimationCtrlBase : public wxControl
{
public:
    wxAnimationCtrlBase() { }

    // public API
    virtual bool LoadFile(const wxString& filename,
                          wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

    virtual bool Play() = 0;
    virtual void Stop() = 0;

    virtual bool IsPlaying() const = 0;

    virtual void SetInactiveBitmap(const wxBitmap &bmp);

    // always return the original bitmap set in this control
    wxBitmap GetInactiveBitmap() const
        { return m_bmpStatic; }

protected:
    // the inactive bitmap as it was set by the user
    wxBitmap m_bmpStatic;

    // the inactive bitmap currently shown in the control
    // (may differ in the size from m_bmpStatic)
    wxBitmap m_bmpStaticReal;

    // updates m_bmpStaticReal from m_bmpStatic if needed
    virtual void UpdateStaticImage();

    // called by SetInactiveBitmap
    virtual void DisplayStaticImage() = 0;

private:
    wxDECLARE_ABSTRACT_CLASS(wxAnimationCtrlBase);
};


// ----------------------------------------------------------------------------
// include the platform-specific version of the wxAnimationCtrl class
// ----------------------------------------------------------------------------

#if defined(__WXGTK20__)
    #include "wx/generic/animate.h"
    #include "wx/gtk/animate.h"
#else
    #include "wx/generic/animate.h"

    class WXDLLIMPEXP_ADV wxAnimation : public wxGenericAnimation
    {
    public:
        wxAnimation()
            : wxGenericAnimation() {}
        wxAnimation(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY)
            : wxGenericAnimation(name, type) {}
        wxAnimation(const wxGenericAnimation& other)
            : wxGenericAnimation(other) {}
    private:
        wxDECLARE_DYNAMIC_CLASS(wxAnimation);
    };

    class WXDLLIMPEXP_ADV wxAnimationCtrl : public wxGenericAnimationCtrl
    {
    public:
        wxAnimationCtrl()
            : wxGenericAnimationCtrl()
            {}
        wxAnimationCtrl(wxWindow *parent,
                        wxWindowID id,
                        const wxAnimation& anim = wxNullAnimation,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxAC_DEFAULT_STYLE,
                        const wxString& name = wxAnimationCtrlNameStr)
            : wxGenericAnimationCtrl(parent, id, anim, pos, size, style, name)
            {}

    bool Create(wxWindow *parent, wxWindowID id,
                const wxAnimation& anim = wxNullGenericAnimation,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = wxAnimationCtrlNameStr)
        { return wxGenericAnimationCtrl::Create(parent, id, anim, pos, size, style, name); }

    void SetAnimation(const wxAnimation &anim)
        { wxGenericAnimationCtrl::SetAnimation(anim); }
    virtual wxAnimation GetAnimation() const
         { return wxAnimation(m_animation) ; }

    private:
        wxDECLARE_DYNAMIC_CLASS(wxAnimationCtrl);
    };
#endif

#endif // wxUSE_ANIMATIONCTRL

#endif // _WX_ANIMATE_H_
