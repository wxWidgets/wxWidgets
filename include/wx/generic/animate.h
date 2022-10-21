/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/animate.h
// Purpose:     wxGenericAnimationCtrl
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_ANIMATEH__
#define _WX_GENERIC_ANIMATEH__

#include "wx/bmpbndl.h"


// ----------------------------------------------------------------------------
// wxGenericAnimationCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxGenericAnimationCtrl: public wxAnimationCtrlBase
{
public:
    wxGenericAnimationCtrl() { Init(); }
    wxGenericAnimationCtrl(wxWindow *parent,
                           wxWindowID id,
                           const wxAnimation& anim = wxNullAnimation,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize,
                           long style = wxAC_DEFAULT_STYLE,
                           const wxString& name = wxASCII_STR(wxAnimationCtrlNameStr))
    {
        Init();

        Create(parent, id, anim, pos, size, style, name);
    }

    void Init();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxAnimation& anim = wxNullAnimation,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxAnimationCtrlNameStr));

    ~wxGenericAnimationCtrl();


public:
    virtual bool LoadFile(const wxString& filename, wxAnimationType type = wxANIMATION_TYPE_ANY) override;
    virtual bool Load(wxInputStream& stream, wxAnimationType type = wxANIMATION_TYPE_ANY) override;

    virtual void Stop() override;
    virtual bool Play() override
        { return Play(true /* looped */); }
    virtual bool IsPlaying() const override
        { return m_isPlaying; }

    void SetAnimation(const wxAnimation &animation) override;

    virtual void SetInactiveBitmap(const wxBitmapBundle &bmp) override;

    // override base class method
    virtual bool SetBackgroundColour(const wxColour& col) override;

    static wxAnimation CreateCompatibleAnimation();

public:     // event handlers

    void OnPaint(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnSize(wxSizeEvent& event);

public:     // extended API specific to this implementation of wxAnimateCtrl

    // Specify whether the animation's background colour is to be shown (the default),
    // or whether the window background should show through
    void SetUseWindowBackgroundColour(bool useWinBackground = true)
        { m_useWinBackgroundColour = useWinBackground; }
    bool IsUsingWindowBackgroundColour() const
        { return m_useWinBackgroundColour; }

    // This overload of Play() lets you specify if the animation must loop or not
    bool Play(bool looped);

    // Draw the current frame of the animation into given DC.
    // This is fast as current frame is always cached.
    void DrawCurrentFrame(wxDC& dc);

    // Returns a wxBitmap with the current frame drawn in it
    wxBitmap& GetBackingStore()
        { return m_backingStore; }

protected:      // internal utilities
    virtual wxAnimationImpl* DoCreateAnimationImpl() const override;

    // resize this control to fit m_animation
    void FitToAnimation();

    // Draw the background; use this when e.g. previous frame had wxANIM_TOBACKGROUND disposal.
    void DisposeToBackground();
    void DisposeToBackground(wxDC& dc);
    void DisposeToBackground(wxDC& dc, const wxPoint &pos, const wxSize &sz);

    void IncrementalUpdateBackingStore();
    bool RebuildBackingStoreUpToFrame(unsigned int);
    void DrawFrame(wxDC &dc, unsigned int);

    virtual void DisplayStaticImage() override;
    virtual wxSize DoGetBestSize() const override;

    // This function can be used as event handler for wxEVT_DPI_CHANGED event
    // and simply calls UpdateStaticImage() to refresh the m_bmpStaticReal when it happens.
    void WXHandleDPIChanged(wxDPIChangedEvent& event)
    {
        UpdateStaticImage();

        event.Skip();
    }

    // Helpers to safely access methods in the wxAnimationGenericImpl that are
    // specific to the generic implementation
    wxPoint AnimationImplGetFramePosition(unsigned int frame) const;
    wxSize AnimationImplGetFrameSize(unsigned int frame) const;
    wxAnimationDisposal AnimationImplGetDisposalMethod(unsigned int frame) const;
    wxColour AnimationImplGetTransparentColour(unsigned int frame) const;
    wxColour AnimationImplGetBackgroundColour() const;


protected:
    unsigned int  m_currentFrame;     // Current frame
    bool          m_looped;           // Looped, or not
    wxTimer       m_timer;            // The timer

    bool          m_isPlaying;        // Is the animation playing?
    bool          m_useWinBackgroundColour; // Use animation bg colour or window bg colour?

    wxBitmap      m_backingStore;     // The frames are drawn here and then blitted
                                      // on the screen

private:
    typedef wxAnimationCtrlBase base_type;
    wxDECLARE_DYNAMIC_CLASS(wxGenericAnimationCtrl);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_GENERIC_ANIMATEH__
