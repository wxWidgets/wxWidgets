/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/animate.h
// Purpose:     Animation classes
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKANIMATEH__
#define _WX_GTKANIMATEH__

#include "wx/defs.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

// ----------------------------------------------------------------------------
// wxAnimation
// Unlike the generic wxAnimation object (see generic\animate.cpp), we won't
// use directly wxAnimationHandlers as gdk-pixbuf already provides the
// concept of handler and will automatically use the available handlers.
// Like generic wxAnimation object, this implementation of wxAnimation is
// refcounted so that assignment is very fast
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAnimation : public wxAnimationBase
{
public:
    wxAnimation(const wxAnimation &tocopy)
        { m_pixbuf=tocopy.m_pixbuf; if (m_pixbuf) g_object_ref(m_pixbuf); }
    wxAnimation(GdkPixbufAnimation *p = NULL)
        { m_pixbuf=p; }
    ~wxAnimation()
        { UnRef(); }

    wxAnimation &operator= (const wxAnimation &tocopy)
        {
            m_pixbuf=tocopy.m_pixbuf;
            if (m_pixbuf) g_object_ref(m_pixbuf);
            return *this;
        }

    bool operator == (const wxAnimation& anim) const
        { return m_pixbuf == anim.m_pixbuf; }
    bool operator != (const wxAnimation& anim) const
        { return m_pixbuf != anim.m_pixbuf; }

    virtual bool IsOk() const
        { return m_pixbuf != NULL; }


    // unfortunately GdkPixbufAnimation does not expose these info:

    virtual size_t GetFrameCount() const
        { return 0; }
    virtual wxImage GetFrame(size_t i) const
        { return wxNullImage; }

    // we can retrieve the delay for a frame only after building
    // a GdkPixbufAnimationIter...
    virtual int GetDelay(size_t i) const
        { return 0; }

    virtual wxSize GetSize() const
        { return wxSize(gdk_pixbuf_animation_get_width(m_pixbuf),
                        gdk_pixbuf_animation_get_height(m_pixbuf)); }

    virtual bool LoadFile(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY);
    virtual bool Load(wxInputStream &stream, wxAnimationType type = wxANIMATION_TYPE_ANY);

    void UnRef()
    {
        if (m_pixbuf)
            g_object_unref(m_pixbuf);
        m_pixbuf = NULL;
    }

public:     // used by GTK callbacks

    GdkPixbufAnimation *GetPixbuf() const
        { return m_pixbuf; }
    void SetPixbuf(GdkPixbufAnimation *p)
        { m_pixbuf=p; if (m_pixbuf) g_object_ref(m_pixbuf); }

protected:
    GdkPixbufAnimation *m_pixbuf;

    // used temporary by Load()
    //bool m_bLoadComplete;

protected:
    DECLARE_DYNAMIC_CLASS(wxAnimation)
};



// ----------------------------------------------------------------------------
// wxAnimationCtrl
// ----------------------------------------------------------------------------

// Resize to animation size if this is set
#define wxAN_FIT_ANIMATION       0x0010

class WXDLLIMPEXP_ADV wxAnimationCtrl: public wxAnimationCtrlBase
{
public:
    wxAnimationCtrl() {}
    wxAnimationCtrl(wxWindow *parent,
                        wxWindowID id,
                        const wxAnimation& anim = wxNullAnimation,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxAC_DEFAULT_STYLE,
                        const wxString& name = wxAnimationCtrlNameStr)
    {
        Create(parent, id, anim, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxAnimation& anim = wxNullAnimation,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = wxAnimationCtrlNameStr);

    ~wxAnimationCtrl();

public:     // event handler

    void OnTimer(wxTimerEvent &);

public:     // public API

    virtual bool LoadFile(const wxString& filename, wxAnimationType type = wxANIMATION_TYPE_ANY);

    virtual void SetAnimation(const wxAnimation &anim);
    virtual wxAnimation GetAnimation() const
        { return wxAnimation(m_anim); }

    virtual bool Play();
    virtual void Stop();

    virtual bool IsPlaying() const;

    bool SetBackgroundColour( const wxColour &colour );

protected:

    virtual wxSize DoGetBestSize() const;
    void FitToAnimation();
    void ClearToBackgroundColour();

    void ResetAnim()
    {
        if (m_anim)
            g_object_unref(m_anim);
        m_anim = NULL;
    }

    void ResetIter()
    {
        if (m_iter)
            g_object_unref(m_iter);
        m_iter = NULL;
    }

protected:      // internal vars

    GdkPixbufAnimation *m_anim;
    GdkPixbufAnimationIter *m_iter;

    wxTimer m_timer;
    bool m_bPlaying;

private:
    DECLARE_DYNAMIC_CLASS(wxAnimationCtrl)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_GTKANIMATEH__
