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
class wxAnimationImpl;

extern WXDLLIMPEXP_DATA_CORE(wxAnimation) wxNullAnimation;
extern WXDLLIMPEXP_DATA_CORE(const char) wxAnimationCtrlNameStr[];

WX_DECLARE_LIST_WITH_DECL(wxAnimationDecoder, wxAnimationDecoderList, class WXDLLIMPEXP_CORE);

// ----------------------------------------------------------------------------
// wxAnimation
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnimation : public wxObject
{
public:
    wxAnimation();
    explicit wxAnimation(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY);
    wxAnimation(const wxAnimation& other);

    wxAnimation& operator=(const wxAnimation& other);

    bool IsOk() const;
    bool IsCompatibleWith(wxClassInfo* ci) const;

    int GetDelay(unsigned int frame) const;
    unsigned int GetFrameCount() const;
    wxImage GetFrame(unsigned int frame);
    wxSize GetSize() const;

    bool LoadFile(const wxString& name, wxAnimationType type = wxANIMATION_TYPE_ANY);
    bool Load(wxInputStream& stream, wxAnimationType type = wxANIMATION_TYPE_ANY);

    // Methods for managing the list of decoders
    static inline wxAnimationDecoderList& GetHandlers() { return sm_handlers; }
    static void AddHandler(wxAnimationDecoder *handler);
    static void InsertHandler(wxAnimationDecoder *handler);
    static const wxAnimationDecoder *FindHandler( wxAnimationType animType );

    static void CleanUpHandlers();
    static void InitStandardHandlers();

protected:
    wxAnimationImpl* GetImpl() const;

private:
    static wxAnimationDecoderList sm_handlers;

    // Ctor used by wxAnimationCtrl::CreateAnimation() only.
    explicit wxAnimation(wxAnimationImpl* impl);

    // Give it permission to create objects of this class using specific impl
    // and access our GetImpl().
    friend class wxAnimationCtrlBase;

    wxDECLARE_DYNAMIC_CLASS(wxAnimation);
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

    virtual void SetAnimation(const wxAnimation &anim) = 0;
    virtual wxAnimation GetAnimation() const = 0;

    virtual bool Play() = 0;
    virtual void Stop() = 0;

    virtual bool IsPlaying() const = 0;

    virtual void SetInactiveBitmap(const wxBitmap &bmp);

    // always return the original bitmap set in this control
    wxBitmap GetInactiveBitmap() const
        { return m_bmpStatic; }

    wxAnimation CreateAnimation() const
        { return wxAnimation(DoCreateAnimationImpl()); }

protected:
    virtual wxAnimationImpl* DoCreateAnimationImpl() const = 0;

    // This method allows derived classes access to wxAnimation::GetImpl().
    wxAnimationImpl* GetAnimImpl(const wxAnimation& anim) const
        { return anim.GetImpl(); }

    // the inactive bitmap as it was set by the user
    wxBitmap m_bmpStatic;

    // the inactive bitmap currently shown in the control
    // (may differ in the size from m_bmpStatic)
    wxBitmap m_bmpStaticReal;

    // updates m_bmpStaticReal from m_bmpStatic if needed
    virtual void UpdateStaticImage();

    // called by SetInactiveBitmap
    virtual void DisplayStaticImage() = 0;
};


// ----------------------------------------------------------------------------
// include the platform-specific version of the wxAnimationCtrl class
// ----------------------------------------------------------------------------

#include "wx/generic/animate.h"

#if defined(__WXGTK20__)
    #include "wx/gtk/animate.h"

    #define wxHAS_NATIVE_ANIMATIONCTRL
#else
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

    private:
        wxDECLARE_DYNAMIC_CLASS(wxAnimationCtrl);
    };
#endif // defined(__WXGTK20__)

#endif // wxUSE_ANIMATIONCTRL

#endif // _WX_ANIMATE_H_
