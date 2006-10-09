///////////////////////////////////////////////////////////////////////////////
// Name:        animateg.cpp
// Purpose:     wxAnimation and wxAnimationCtrl
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__


#if wxUSE_ANIMATIONCTRL

#include "wx/log.h"
#include "wx/wfstream.h"
#include "wx/image.h"
#include "wx/gifdecod.h"
#include "wx/anidecod.h"
#include "wx/dcmemory.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/animate.h"
#include "wx/animdecod.h"


#include <wx/listimpl.cpp>
WX_DEFINE_LIST(wxAnimationDecoderList);

wxAnimationDecoderList wxAnimation::sm_handlers;



// ----------------------------------------------------------------------------
// wxAnimation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxAnimation, wxAnimationBase)
#define M_ANIMDATA      wx_static_cast(wxAnimationDecoder*, m_refData)

wxSize wxAnimation::GetSize() const
{
    wxCHECK_MSG( IsOk(), wxDefaultSize, wxT("invalid animation") );

    return M_ANIMDATA->GetAnimationSize();
}

size_t wxAnimation::GetFrameCount() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid animation") );

    return M_ANIMDATA->GetFrameCount();
}

wxImage wxAnimation::GetFrame(size_t i) const
{
    wxCHECK_MSG( IsOk(), wxNullImage, wxT("invalid animation") );

    wxImage ret;
    if (!M_ANIMDATA->ConvertToImage(i, &ret))
        return wxNullImage;
    return ret;
}

int wxAnimation::GetDelay(size_t i) const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid animation") );

    return M_ANIMDATA->GetDelay(i);
}

wxPoint wxAnimation::GetFramePosition(size_t frame) const
{
    wxCHECK_MSG( IsOk(), wxDefaultPosition, wxT("invalid animation") );

    return M_ANIMDATA->GetFramePosition(frame);
}

wxAnimationDisposal wxAnimation::GetDisposalMethod(size_t frame) const
{
    wxCHECK_MSG( IsOk(), wxANIM_UNSPECIFIED, wxT("invalid animation") );

    return M_ANIMDATA->GetDisposalMethod(frame);
}

wxColour wxAnimation::GetBackgroundColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, wxT("invalid animation") );

    return M_ANIMDATA->GetBackgroundColour();
}

bool wxAnimation::LoadFile(const wxString& filename, wxAnimationType type)
{
    wxFileInputStream stream(filename);
    if (!stream.Ok())
        return false;

    return Load(stream, type);
}

bool wxAnimation::Load(wxInputStream &stream, wxAnimationType type)
{
    UnRef();

    const wxAnimationDecoder *handler;
    if ( type == wxANIMATION_TYPE_ANY )
    {
        for ( wxAnimationDecoderList::compatibility_iterator node = sm_handlers.GetFirst(); 
              node; node = node->GetNext() )
        {
            handler=(const wxAnimationDecoder*)node->GetData();

            if ( handler->CanRead(stream) )
            {
                // do a copy of the handler from the static list which we will own
                // as our reference data
                m_refData = handler->Clone();
                return M_ANIMDATA->Load(stream);
            }

        }

        wxLogWarning( _("No handler found for animation type.") );
        return false;
    }

    handler = FindHandler(type);

    // do a copy of the handler from the static list which we will own
    // as our reference data
    m_refData = handler->Clone();

    if (handler == NULL)
    {
        wxLogWarning( _("No animation handler for type %ld defined."), type );

        return false;
    }

    if (stream.IsSeekable() && !M_ANIMDATA->CanRead(stream))
    {
        wxLogError(_("Animation file is not of type %ld."), type);
        return false;
    }
    else
        return M_ANIMDATA->Load(stream);
}


// ----------------------------------------------------------------------------
// animation decoders
// ----------------------------------------------------------------------------

void wxAnimation::AddHandler( wxAnimationDecoder *handler )
{
    // Check for an existing handler of the type being added.
    if (FindHandler( handler->GetType() ) == 0)
    {
        sm_handlers.Append( handler );
    }
    else
    {
        // This is not documented behaviour, merely the simplest 'fix'
        // for preventing duplicate additions.  If someone ever has
        // a good reason to add and remove duplicate handlers (and they
        // may) we should probably refcount the duplicates.
        //   also an issue in InsertHandler below.

        wxLogDebug( _T("Adding duplicate animation handler for '%d' type"),
                    handler->GetType() );
        delete handler;
    }
}

void wxAnimation::InsertHandler( wxAnimationDecoder *handler )
{
    // Check for an existing handler of the type being added.
    if (FindHandler( handler->GetType() ) == 0)
    {
        sm_handlers.Insert( handler );
    }
    else
    {
        // see AddHandler for additional comments.
        wxLogDebug( _T("Inserting duplicate animation handler for '%d' type"),
                    handler->GetType() );
        delete handler;
    }
}

const wxAnimationDecoder *wxAnimation::FindHandler( wxAnimationType animType )
{
    wxAnimationDecoderList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        const wxAnimationDecoder *handler = (const wxAnimationDecoder *)node->GetData();
        if (handler->GetType() == animType) return handler;
        node = node->GetNext();
    }
    return 0;
}

void wxAnimation::InitStandardHandlers()
{
    AddHandler(new wxGIFDecoder);
    AddHandler(new wxANIDecoder);
}

void wxAnimation::CleanUpHandlers()
{
    wxAnimationDecoderList::compatibility_iterator node = sm_handlers.GetFirst();
    while (node)
    {
        wxAnimationDecoder *handler = (wxAnimationDecoder *)node->GetData();
        wxAnimationDecoderList::compatibility_iterator next = node->GetNext();
        delete handler;
        node = next;
    }

    sm_handlers.Clear();
}


// A module to allow wxAnimation initialization/cleanup
// without calling these functions from app.cpp or from
// the user's application.

class wxAnimationModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxAnimationModule)
public:
    wxAnimationModule() {}
    bool OnInit() { wxAnimation::InitStandardHandlers(); return true; };
    void OnExit() { wxAnimation::CleanUpHandlers(); };
};

IMPLEMENT_DYNAMIC_CLASS(wxAnimationModule, wxModule)




// ----------------------------------------------------------------------------
// wxAnimationCtrl
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxAnimationCtrl, wxAnimationCtrlBase)
BEGIN_EVENT_TABLE(wxAnimationCtrl, wxAnimationCtrlBase)
    EVT_PAINT(wxAnimationCtrl::OnPaint)
    EVT_SIZE(wxAnimationCtrl::OnSize)
    EVT_TIMER(wxID_ANY, wxAnimationCtrl::OnTimer)
END_EVENT_TABLE()

bool wxAnimationCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxAnimation& animation, const wxPoint& pos,
            const wxSize& size, long style, const wxString& name)
{
    m_animation = animation;
    m_currentFrame = 0;
    m_looped = true;
    m_isPlaying = false;
    m_useWinBackgroundColour = false;
    m_timer.SetOwner(this);

    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    // by default we get the same background colour of our parent
    SetBackgroundColour(parent->GetBackgroundColour());
    return true;
}

wxAnimationCtrl::~wxAnimationCtrl()
{
    Stop();
}

bool wxAnimationCtrl::LoadFile(const wxString& filename, wxAnimationType type)
{
    wxAnimation anim;
    if (!anim.LoadFile(filename, type) || 
        !anim.IsOk())
        return false;

    SetAnimation(anim);
    return true;
}

wxSize wxAnimationCtrl::DoGetBestSize() const
{
    if (m_animation.IsOk() && !this->HasFlag(wxAC_NO_AUTORESIZE))
        return m_animation.GetSize();

    return wxSize(100, 100);
}

void wxAnimationCtrl::SetAnimation(const wxAnimation& animation)
{
    if (IsPlaying())
        Stop();

    m_animation = animation;

    if (m_animation.GetBackgroundColour() == wxNullColour)
        SetUseWindowBackgroundColour();
    if (!this->HasFlag(wxAC_NO_AUTORESIZE))
        FitToAnimation();

    // display first frame
    m_currentFrame = 0;
    if (m_animation.IsOk())
        RebuildBackingStoreUpToFrame(0);
    else
    {
        // clear to 
        wxMemoryDC dc;
        dc.SelectObject(m_backingStore);

        // Draw the background
        DisposeToBackground(dc);
    }

    Refresh();
}

void wxAnimationCtrl::FitToAnimation()
{
    SetSize(m_animation.GetSize());
}


// ----------------------------------------------------------------------------
// wxAnimationCtrl - stop/play methods
// ----------------------------------------------------------------------------

void wxAnimationCtrl::Stop()
{
    // leave current frame displayed until Play() is called again
    m_timer.Stop();
    m_isPlaying = false;
}

bool wxAnimationCtrl::Play(bool looped)
{
    if (!m_animation.IsOk())
        return false;

    int oldframe = m_currentFrame;
    m_looped = looped;
    m_currentFrame = 0;
    m_isPlaying = true;

    // small optimization: if the back store was already updated to the
    // first frame, don't rebuild it
    if (oldframe != 0)
        RebuildBackingStoreUpToFrame(0);

    // DrawCurrentFrame() will use our updated backing store
    wxClientDC clientDC(this);
    DrawCurrentFrame(clientDC);

    // start the timer
    int delay = m_animation.GetDelay(0);
    if (delay == 0)
        delay = 1;      // 0 is invalid timeout for wxTimer.
    m_timer.Start(delay);

    return true;
}



// ----------------------------------------------------------------------------
// wxAnimationCtrl - rendering methods
// ----------------------------------------------------------------------------

void wxAnimationCtrl::RebuildBackingStoreUpToFrame(size_t frame)
{
    // if we've not created the backing store yet or it's too
    // small, then recreate it
    wxSize sz = m_animation.GetSize(),
           winsz = GetClientSize();
    int w = wxMin(sz.GetWidth(), winsz.GetWidth());
    int h = wxMin(sz.GetHeight(), winsz.GetHeight());

    if (m_backingStore.GetWidth() < w ||
        m_backingStore.GetHeight() < h)
        m_backingStore.Create(w, h);

    wxMemoryDC dc;
    dc.SelectObject(m_backingStore);

    // Draw the background
    DisposeToBackground(dc);

    // Draw all intermediate frames that haven't been removed from the animation
    for (size_t i = 0; i < frame; i++)
    {
        if (m_animation.GetDisposalMethod(i) == wxANIM_DONOTREMOVE ||
            m_animation.GetDisposalMethod(i) == wxANIM_UNSPECIFIED)
        {
            DrawFrame(dc, i);
        }
    }

    // finally draw this frame
    DrawFrame(dc, frame);
    dc.SelectObject(wxNullBitmap);
}

void wxAnimationCtrl::IncrementalUpdateBackingStore()
{
    wxMemoryDC dc;
    dc.SelectObject(m_backingStore);

    // OPTIMIZATION:
    // since wxAnimationCtrl can only play animations forward, without skipping
    // frames, we can be sure that m_backingStore contains the m_currentFrame-1
    // frame and thus we just need to dispose the m_currentFrame-1 frame and
    // render the m_currentFrame-th one.

    if (m_currentFrame == 0)
    {
        // before drawing the first frame always dispose to bg colour
        DisposeToBackground(dc);
    }
    else
    {
        switch (m_animation.GetDisposalMethod(m_currentFrame-1))
        {
        case wxANIM_TOBACKGROUND:
            DisposeToBackground(dc);
            break;

        case wxANIM_TOPREVIOUS:
            // this disposal should never be used too often.
            // E.g. GIF specification explicitely say to keep the usage of this
            //      disposal limited to the minimum. 
            // In fact it may require a lot of time to restore 
            if (m_currentFrame == 1)
            {
                // if 0-th frame disposal is to restore to previous frame,
                // the best we can do is to restore to background
                DisposeToBackground(dc);
            }
            else
                RebuildBackingStoreUpToFrame(m_currentFrame-2);
            break;

        case wxANIM_DONOTREMOVE:
        case wxANIM_UNSPECIFIED:
            break;
        }
    }

    // now just draw the current frame on the top of the backing store
    DrawFrame(dc, m_currentFrame);
    dc.SelectObject(wxNullBitmap);
}

void wxAnimationCtrl::DrawFrame(wxDC &dc, size_t frame)
{
    // PERFORMANCE NOTE:
    // this draw stuff is not as fast as possible: the wxAnimationDecoder
    // needs first to convert from its internal format to wxImage RGB24;
    // the wxImage is then converted as a wxBitmap and finally blitted.
    // If wxAnimationDecoder had a function to convert directly from its
    // internal format to a port-specific wxBitmap, it would be somewhat faster.
    wxBitmap bmp(m_animation.GetFrame(frame));
    dc.DrawBitmap(bmp, m_animation.GetFramePosition(frame),
                  true /* use mask */);
}

void wxAnimationCtrl::DrawCurrentFrame(wxDC& dc)
{
    wxASSERT(m_backingStore.Ok());

    // m_backingStore always contains the current frame
    dc.DrawBitmap(m_backingStore, 0, 0);
}

void wxAnimationCtrl::DisposeToBackground(wxDC& dc)
{
    wxBrush brush(IsUsingWindowBackgroundColour() ? 
                  this->GetBackgroundColour() : m_animation.GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

// ----------------------------------------------------------------------------
// wxAnimationCtrl - event handlers
// ----------------------------------------------------------------------------

void wxAnimationCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // VERY IMPORTANT: the wxPaintDC *must* be created in any case
    wxPaintDC dc(this);

    // both if we are playing or not, we need to refresh the current frame
    if (m_backingStore.Ok())
        DrawCurrentFrame(dc);
    //else: m_animation is not valid and thus we don't have a valid backing store...
}

void wxAnimationCtrl::OnTimer(wxTimerEvent &WXUNUSED(event))
{
    m_currentFrame++;
    if (m_currentFrame == m_animation.GetFrameCount())
    {
        // Should a non-looped animation display the last frame?
        if (!m_looped)
        {
            m_timer.Stop();
            m_isPlaying = false;
            return;
        }
        else
            m_currentFrame = 0;     // let's restart
    }

    IncrementalUpdateBackingStore();

    wxClientDC dc(this);
    DrawCurrentFrame(dc);

    // Set the timer for the next frame
    int delay = m_animation.GetDelay(m_currentFrame);
    if (delay == 0)
        delay = 1;      // 0 is invalid timeout for wxTimer.
    m_timer.Start(delay);
}

void wxAnimationCtrl::OnSize(wxSizeEvent &WXUNUSED(event))
{
    // NB: resizing an animation control may take a lot of time
    //     for big animations as the backing store must be
    //     extended and rebuilt. Try to avoid it!!
    if (m_animation.IsOk())
        RebuildBackingStoreUpToFrame(m_currentFrame);
}

#endif      // wxUSE_ANIMATIONCTRL

