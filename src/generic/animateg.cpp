///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/animateg.cpp
// Purpose:     wxAnimationGenericImpl and wxGenericAnimationCtrl
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_ANIMATIONCTRL

#include "wx/animate.h"
#include "wx/generic/animate.h"
#include "wx/generic/private/animate.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/image.h"
    #include "wx/dcmemory.h"
    #include "wx/dcclient.h"
#endif

#include "wx/wfstream.h"
#include "wx/weakref.h"

#include <limits>
#include <memory>

namespace
{

// A decoder-provided zero delay must not turn the GUI thread into a busy loop.
// Ten milliseconds bounds the generic fallback to at most 100 transitions/s.
constexpr int wxGENERIC_ANIMATION_MIN_FRAME_DELAY_MS = 10;

std::uint64_t wxGenericAnimationNextGeneration(std::uint64_t generation)
{
    if ( ++generation == 0 )
        ++generation;

    return generation;
}

// Decoder methods are application-code boundaries: a custom decoder can
// synchronously destroy its wxAnimationCtrl owner. Keep a tracker across every
// such call and, crucially, consult it before reading the owner again.
class wxGenericAnimationCtrlLifetime final
{
public:
    explicit wxGenericAnimationCtrlLifetime(wxGenericAnimationCtrl* control)
        : m_control(control)
    {
    }

    bool IsAlive() const
    {
        wxGenericAnimationCtrl* const control = m_control.get();
        return control && !control->IsBeingDeleted();
    }

private:
    wxWeakRef<wxGenericAnimationCtrl> m_control;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxAnimation
// ----------------------------------------------------------------------------

#ifndef wxHAS_NATIVE_ANIMATIONCTRL

/* static */
wxAnimationImpl *wxAnimationImpl::CreateDefault()
{
    return new wxAnimationGenericImpl();
}

#endif // !wxHAS_NATIVE_ANIMATIONCTRL

bool wxAnimationGenericImpl::IsCompatibleWith(wxClassInfo* ci) const
{
    return ci->IsKindOf(&wxGenericAnimationCtrl::ms_classInfo);
}

wxSize wxAnimationGenericImpl::GetSize() const
{
    return m_decoder->GetAnimationSize();
}

unsigned int wxAnimationGenericImpl::GetFrameCount() const
{
    return m_decoder->GetFrameCount();
}

wxImage wxAnimationGenericImpl::GetFrame(unsigned int i) const
{
    wxImage ret;
    if (!m_decoder->ConvertToImage(i, &ret))
        return wxNullImage;
    return ret;
}

int wxAnimationGenericImpl::GetDelay(unsigned int i) const
{
    return m_decoder->GetDelay(i);
}

wxPoint wxAnimationGenericImpl::GetFramePosition(unsigned int frame) const
{
    return m_decoder->GetFramePosition(frame);
}

wxSize wxAnimationGenericImpl::GetFrameSize(unsigned int frame) const
{
    return m_decoder->GetFrameSize(frame);
}

wxAnimationDisposal wxAnimationGenericImpl::GetDisposalMethod(unsigned int frame) const
{
    return m_decoder->GetDisposalMethod(frame);
}

wxColour wxAnimationGenericImpl::GetTransparentColour(unsigned int frame) const
{
    return m_decoder->GetTransparentColour(frame);
}

wxColour wxAnimationGenericImpl::GetBackgroundColour() const
{
    return m_decoder->GetBackgroundColour();
}

bool wxAnimationGenericImpl::LoadFile(const wxString& filename, wxAnimationType type)
{
    wxFileInputStream stream(filename);
    if ( !stream.IsOk() )
        return false;

    return Load(stream, type);
}

bool wxAnimationGenericImpl::Load(wxInputStream &stream, wxAnimationType type)
{
    const wxAnimationDecoder *handler = nullptr;
    if ( type == wxANIMATION_TYPE_ANY )
    {
        for ( wxAnimationDecoderList::compatibility_iterator node = wxAnimation::GetHandlers().GetFirst();
              node; node = node->GetNext() )
        {
            handler=(const wxAnimationDecoder*)node->GetData();

            if ( handler->CanRead(stream) )
                break;

            handler = nullptr;
        }

        if ( !handler )
        {
            wxLogWarning( _("No handler found for animation type.") );
            return false;
        }
    }
    else
    {
        handler = wxAnimation::FindHandler(type);

        if ( !handler )
        {
            wxLogWarning(
                _("No animation handler for type %ld defined."), type );
            return false;
        }

        if ( stream.IsSeekable() && !handler->CanRead(stream) )
        {
            wxLogError(_("Animation file is not of type %ld."), type);
            return false;
        }
    }

    // Decode into an isolated candidate. A malformed/truncated stream must
    // neither make IsOk() lie (m_decoder != nullptr) nor destroy a previously
    // valid animation. wxAnimationDecoder is ref-counted wxObjectRefData, so
    // use DecRef() as the rollback deleter.
    const auto releaseDecoder = [](wxAnimationDecoder* decoder)
    {
        if ( decoder )
            decoder->DecRef();
    };
    std::unique_ptr<wxAnimationDecoder, decltype(releaseDecoder)> candidate(
        handler->Clone(), releaseDecoder);
    if ( !candidate || !candidate->Load(stream) ||
            candidate->GetFrameCount() == 0 ||
            candidate->GetAnimationSize().x <= 0 ||
            candidate->GetAnimationSize().y <= 0 )
        return false;

    UnRef();
    m_decoder = candidate.release();
    return true;
}

void wxAnimationGenericImpl::UnRef()
{
    if ( m_decoder )
    {
        m_decoder->DecRef();
        m_decoder = nullptr;
    }
}

// ----------------------------------------------------------------------------
// wxAnimationCtrl
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGenericAnimationCtrl, wxAnimationCtrlBase);
wxBEGIN_EVENT_TABLE(wxGenericAnimationCtrl, wxAnimationCtrlBase)
    EVT_PAINT(wxGenericAnimationCtrl::OnPaint)
    EVT_SIZE(wxGenericAnimationCtrl::OnSize)
    EVT_SHOW(wxGenericAnimationCtrl::OnShow)
    EVT_TIMER(wxID_ANY, wxGenericAnimationCtrl::OnTimer)
wxEND_EVENT_TABLE()

void wxGenericAnimationCtrl::Init()
{
    m_currentFrame = 0;
    m_looped = false;
    m_isPlaying = false;

    // use the window background colour by default to be consistent
    // with the GTK+ native version
    m_useWinBackgroundColour = true;

    Bind(wxEVT_DPI_CHANGED, &wxGenericAnimationCtrl::WXHandleDPIChanged, this);
}

bool wxGenericAnimationCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxAnimation& animation, const wxPoint& pos,
            const wxSize& size, long style, const wxString& name)
{
    InvalidateFrameSchedule();

    if (!base_type::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    // by default we get the same background colour of our parent
    SetBackgroundColour(parent->GetBackgroundColour());

    SetAnimation(animation);

    return true;
}

wxGenericAnimationCtrl::~wxGenericAnimationCtrl()
{
    // Never refresh or rebuild a backing bitmap from a partially destroyed
    // window. wxTimer::Stop() plus its destructor revoke the native timer.
    m_timer.Stop();
    m_needToShowNextFrame = false;
    m_isPlaying = false;
}

bool wxGenericAnimationCtrl::LoadFile(const wxString& filename, wxAnimationType type)
{
    wxFileInputStream fis(filename);
    if (!fis.IsOk())
        return false;
    return Load(fis, type);
}

bool wxGenericAnimationCtrl::Load(wxInputStream& stream, wxAnimationType type)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const std::uint64_t generation = m_timerGeneration;

    wxAnimation anim(CreateAnimation());
    if ( !lifetime.IsAlive() || m_timerGeneration != generation )
        return false;

    const bool loaded = anim.Load(stream, type);
    if ( !lifetime.IsAlive() || m_timerGeneration != generation ||
            !loaded || !anim.IsOk() )
    {
        return false;
    }

    const std::uint64_t expectedGeneration =
        wxGenericAnimationNextGeneration(generation);
    SetAnimation(anim);
    if ( !lifetime.IsAlive() )
        return false;

    // A nested setter or Stop()/Play() issued by any decoder hook owns the
    // final state. Report success only when this exact candidate was the
    // publication that advanced the generation once.
    return m_timerGeneration == expectedGeneration &&
           m_animation.IsSameAs(anim);
}

wxAnimation wxGenericAnimationCtrl::CreateCompatibleAnimation()
{
    return MakeAnimFromImpl(new wxAnimationGenericImpl());
}

wxAnimationImpl* wxGenericAnimationCtrl::DoCreateAnimationImpl() const
{
    return new wxAnimationGenericImpl();
}

wxSize wxGenericAnimationCtrl::DoGetBestSize() const
{
    if (m_animation.IsOk() && !this->HasFlag(wxAC_NO_AUTORESIZE))
    {
        const wxGenericAnimationCtrlLifetime lifetime(
            const_cast<wxGenericAnimationCtrl*>(this));
        const wxAnimation animation = m_animation;
        const wxSize size = animation.GetSize();
        return lifetime.IsAlive() ? size : wxDefaultSize;
    }

    return FromDIP(wxSize(100, 100));
}

void wxGenericAnimationCtrl::SetAnimation(const wxAnimationBundle& animations)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    wxAnimations candidates = animations.GetAll();

    // Reset animation if we don't have any valid ones.
    if ( candidates.empty() )
    {
        InvalidateFrameSchedule();
        m_isPlaying = false;
        m_pausedForHidden = false;
        m_currentFrame = 0;
        m_animations.clear();
        m_animation.UnRef();
        DisplayStaticImage();
        return;
    }

    // Decoder access is an application-code boundary for custom handlers.
    // Select and validate the candidate without publishing it, and abandon
    // this outer setter if a nested Stop(), Play() or SetAnimation() won.
    const std::uint64_t selectionGeneration = m_timerGeneration;
    const double scale = GetDPIScaleFactor();
    const wxAnimation selected =
        SelectAnimationForScale(candidates, scale);
    if ( !lifetime.IsAlive() ||
            m_timerGeneration != selectionGeneration )
        return;

    // Validate before withdrawing the current generation: an incompatible
    // bundle is a failed setter, not a partially applied animation.
    wxClassInfo* const classInfo = GetClassInfo();
    const bool compatible = selected.IsCompatibleWith(classInfo);
    if ( !lifetime.IsAlive() ||
            m_timerGeneration != selectionGeneration )
        return;
    wxCHECK_RET(compatible, wxT("incompatible animation") );

    InvalidateFrameSchedule();
    const std::uint64_t publicationGeneration = m_timerGeneration;
    m_isPlaying = false;
    m_pausedForHidden = false;
    m_currentFrame = 0;
    m_animations.swap(candidates);
    m_animation = selected;

    const wxColour background = AnimationImplGetBackgroundColour();
    if ( !lifetime.IsAlive() ||
            m_timerGeneration != publicationGeneration )
        return;
    if (background == wxNullColour)
        SetUseWindowBackgroundColour();
    if (!this->HasFlag(wxAC_NO_AUTORESIZE))
    {
        FitToAnimation();
        if ( !lifetime.IsAlive() ||
                m_timerGeneration != publicationGeneration )
            return;
    }

    DisplayStaticImage();
}

void wxGenericAnimationCtrl::SetInactiveBitmap(const wxBitmapBundle &bmp)
{
    // if the bitmap has an associated mask, we need to set our background to
    // the colour of our parent otherwise when calling DrawCurrentFrame()
    // (which uses the bitmap's mask), our background colour would be used for
    // transparent areas - and that's not what we want (at least for
    // consistency with the GTK version)
    if ( bmp.IsOk() && bmp.GetBitmapFor(this).GetMask() != nullptr && GetParent() != nullptr )
        SetBackgroundColour(GetParent()->GetBackgroundColour());

    wxAnimationCtrlBase::SetInactiveBitmap(bmp);
}

void wxGenericAnimationCtrl::FitToAnimation()
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const std::uint64_t generation = m_timerGeneration;
    const wxAnimation animation = m_animation;
    wxAnimationImpl* const implementation = GetAnimImpl();
    const wxSize size = animation.GetSize();
    if ( !lifetime.IsAlive() || m_timerGeneration != generation ||
            GetAnimImpl() != implementation )
    {
        return;
    }

    InvalidateBestSize();
    SetSize(size);
}

bool wxGenericAnimationCtrl::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxWindow::SetBackgroundColour(colour) )
        return false;

    // if not playing, then this change must be seen immediately (unless
    // there's an inactive bitmap set which has higher priority than bg colour)
    if ( !IsPlaying() )
        DisplayStaticImage();

    return true;
}


// ----------------------------------------------------------------------------
// wxAnimationCtrl - stop/play methods
// ----------------------------------------------------------------------------

void wxGenericAnimationCtrl::Stop()
{
    InvalidateFrameSchedule();
    m_isPlaying = false;
    m_pausedForHidden = false;

    // reset frame counter
    m_currentFrame = 0;

    DisplayStaticImage();
}

bool wxGenericAnimationCtrl::Play(bool looped)
{
    if (!m_animation.IsOk())
        return false;

    const wxGenericAnimationCtrlLifetime lifetime(this);

    // Play() is also a restart operation. Revoke the previous timer identity
    // first so an already queued notification can't advance the new run.
    InvalidateFrameSchedule();
    m_isPlaying = false;
    m_pausedForHidden = false;
    m_looped = looped;
    m_currentFrame = 0;

    const std::uint64_t playGeneration = m_timerGeneration;
    if (!RebuildBackingStoreUpToFrame(0))
    {
        if ( !lifetime.IsAlive() )
            return false;

        // A decoder callback may have deliberately replaced this request.
        // Preserve the nested verdict instead of stopping its newer run.
        return m_timerGeneration != playGeneration ? m_isPlaying : false;
    }
    if ( !lifetime.IsAlive() )
        return false;
    if ( m_timerGeneration != playGeneration )
        return m_isPlaying;

    m_isPlaying = true;
    m_pausedForHidden = !IsShown();
    m_needToShowNextFrame = !m_pausedForHidden;

    Refresh();

    return true;
}

void wxGenericAnimationCtrl::InvalidateFrameSchedule()
{
    m_timer.Stop();
    m_needToShowNextFrame = false;

    m_timerGeneration =
        wxGenericAnimationNextGeneration(m_timerGeneration);

    const std::uint64_t firstTimerId =
        static_cast<std::uint64_t>(wxID_HIGHEST) + 1;
    const std::uint64_t timerIdRange =
        static_cast<std::uint64_t>((std::numeric_limits<int>::max)()) -
        firstTimerId;
    m_activeTimerId = static_cast<int>(
        firstTimerId + (m_timerGeneration % timerIdRange));
    m_timer.SetOwner(this, m_activeTimerId);
}



// ----------------------------------------------------------------------------
// wxAnimationCtrl - rendering methods
// ----------------------------------------------------------------------------

bool wxGenericAnimationCtrl::RebuildBackingStoreUpToFrame(unsigned int frame)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const std::uint64_t generation = m_timerGeneration;
    const wxAnimation animation = m_animation;
    if ( !animation.IsOk() )
        return false;

    wxAnimationGenericImpl* const implementation =
        static_cast<wxAnimationGenericImpl*>(GetAnimImpl());
    if ( !implementation )
        return false;

    const auto isCurrent = [this, &lifetime, generation, implementation]()
    {
        return lifetime.IsAlive() &&
               m_timerGeneration == generation && m_animation.IsOk() &&
               GetAnimImpl() == implementation;
    };

    const unsigned int frameCount = implementation->GetFrameCount();
    if ( !isCurrent() || frame >= frameCount )
        return false;

    const wxSize animationSize = implementation->GetSize();
    if ( !isCurrent() )
        return false;

    const wxSize windowSize = GetClientSize();
    const int w = wxMin(animationSize.GetWidth(), windowSize.GetWidth());
    const int h = wxMin(animationSize.GetHeight(), windowSize.GetHeight());

    if ( w <= 0 || h <= 0 )
        return false;

    // Render into an isolated bitmap. Decoder calls are extension boundaries:
    // a custom decoder can re-enter Stop(), Play() or SetAnimation(). Nothing
    // becomes the published backing store unless this generation still owns
    // the control after every one of those calls.
    wxBitmap candidate;
    if ( !candidate.Create(w, h) )
        return false;

    wxMemoryDC dc;
    dc.SelectObject(candidate);

    const auto clearBackground = [&](wxDC& target) -> bool
    {
        wxColour colour = GetBackgroundColour();
        if ( !IsUsingWindowBackgroundColour() )
        {
            colour = implementation->GetBackgroundColour();
            if ( !isCurrent() )
                return false;
        }

        target.SetBackground(wxBrush(colour));
        target.Clear();
        return true;
    };

    const auto clearFrameRect = [&](wxDC& target,
                                    unsigned int index) -> bool
    {
        const wxPoint position = implementation->GetFramePosition(index);
        if ( !isCurrent() )
            return false;
        const wxSize size = implementation->GetFrameSize(index);
        if ( !isCurrent() )
            return false;

        wxColour colour = GetBackgroundColour();
        if ( !IsUsingWindowBackgroundColour() )
        {
            colour = implementation->GetBackgroundColour();
            if ( !isCurrent() )
                return false;
        }

        target.SetBrush(wxBrush(colour));
        target.SetPen(*wxTRANSPARENT_PEN);
        target.DrawRectangle(position, size);
        return true;
    };

    const auto drawFrame = [&](wxDC& target, unsigned int index) -> bool
    {
        const wxImage image = implementation->GetFrame(index);
        if ( !isCurrent() || !image.IsOk() )
            return false;
        const wxPoint position = implementation->GetFramePosition(index);
        if ( !isCurrent() )
            return false;

        const wxBitmap bitmap(image);
        if ( !bitmap.IsOk() )
            return false;
        target.DrawBitmap(bitmap, position, true /* use mask */);
        return true;
    };

    if ( !clearBackground(dc) )
    {
        dc.SelectObject(wxNullBitmap);
        return false;
    }

    // Draw all intermediate frames that haven't been removed from the animation
    for (unsigned int i = 0; i < frame; i++)
    {
        const wxAnimationDisposal disposal =
            implementation->GetDisposalMethod(i);
        if ( !isCurrent() )
        {
            dc.SelectObject(wxNullBitmap);
            return false;
        }

        if (disposal == wxANIM_DONOTREMOVE ||
            disposal == wxANIM_UNSPECIFIED)
        {
            if ( !drawFrame(dc, i) )
            {
                dc.SelectObject(wxNullBitmap);
                return false;
            }
        }
        else if (disposal == wxANIM_TOBACKGROUND &&
                 !clearFrameRect(dc, i))
        {
            dc.SelectObject(wxNullBitmap);
            return false;
        }
    }

    // finally draw this frame
    if ( !drawFrame(dc, frame) )
    {
        dc.SelectObject(wxNullBitmap);
        return false;
    }

    dc.SelectObject(wxNullBitmap);
    if ( !isCurrent() )
        return false;

    // Animation sizes and client sizes are both expressed in native pixels.
    // A high-DPI bundle member is already larger and must not be scaled twice.
    m_backingStore = candidate;

    return true;
}

bool wxGenericAnimationCtrl::IncrementalUpdateBackingStore()
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const std::uint64_t generation = m_timerGeneration;
    const wxAnimation animation = m_animation;
    if ( !animation.IsOk() || !m_backingStore.IsOk() )
        return false;
    wxBitmap backingStore = m_backingStore;

    wxAnimationGenericImpl* const implementation =
        static_cast<wxAnimationGenericImpl*>(GetAnimImpl());
    if ( !implementation )
        return false;

    const auto isCurrent = [this, &lifetime, generation, implementation]()
    {
        return lifetime.IsAlive() &&
               m_timerGeneration == generation && m_animation.IsOk() &&
               GetAnimImpl() == implementation;
    };

    const unsigned int frame = m_currentFrame;
    wxBitmap candidate;
    if ( !candidate.Create(backingStore.GetWidth(),
                           backingStore.GetHeight(),
                           backingStore.GetDepth()) )
    {
        return false;
    }

    wxMemoryDC dc;
    wxMemoryDC source;
    dc.SelectObject(candidate);
    source.SelectObject(backingStore);
    const bool copied = dc.Blit(0, 0,
                                backingStore.GetWidth(),
                                backingStore.GetHeight(),
                                &source, 0, 0, wxCOPY);
    source.SelectObject(wxNullBitmap);
    if ( !copied )
    {
        dc.SelectObject(wxNullBitmap);
        return false;
    }

    const auto clearBackground = [&](wxDC& target) -> bool
    {
        wxColour colour = GetBackgroundColour();
        if ( !IsUsingWindowBackgroundColour() )
        {
            colour = implementation->GetBackgroundColour();
            if ( !isCurrent() )
                return false;
        }
        target.SetBackground(wxBrush(colour));
        target.Clear();
        return true;
    };

    const auto clearFrameRect = [&](wxDC& target,
                                    unsigned int index) -> bool
    {
        const wxPoint position = implementation->GetFramePosition(index);
        if ( !isCurrent() )
            return false;
        const wxSize size = implementation->GetFrameSize(index);
        if ( !isCurrent() )
            return false;

        wxColour colour = GetBackgroundColour();
        if ( !IsUsingWindowBackgroundColour() )
        {
            colour = implementation->GetBackgroundColour();
            if ( !isCurrent() )
                return false;
        }
        target.SetBrush(wxBrush(colour));
        target.SetPen(*wxTRANSPARENT_PEN);
        target.DrawRectangle(position, size);
        return true;
    };

    const auto drawFrame = [&](wxDC& target, unsigned int index) -> bool
    {
        const wxImage image = implementation->GetFrame(index);
        if ( !isCurrent() || !image.IsOk() )
            return false;
        const wxPoint position = implementation->GetFramePosition(index);
        if ( !isCurrent() )
            return false;
        const wxBitmap bitmap(image);
        if ( !bitmap.IsOk() )
            return false;
        target.DrawBitmap(bitmap, position, true /* use mask */);
        return true;
    };

    // OPTIMIZATION:
    // since wxAnimationCtrl can only play animations forward, without skipping
    // frames, we can be sure that m_backingStore contains the m_currentFrame-1
    // frame and thus we just need to dispose the m_currentFrame-1 frame and
    // render the m_currentFrame-th one.

    if (frame == 0)
    {
        // before drawing the first frame always dispose to bg colour
        if ( !clearBackground(dc) )
        {
            dc.SelectObject(wxNullBitmap);
            return false;
        }
    }
    else
    {
        const wxAnimationDisposal disposal =
            implementation->GetDisposalMethod(frame - 1);
        if ( !isCurrent() )
        {
            dc.SelectObject(wxNullBitmap);
            return false;
        }

        switch (disposal)
        {
        case wxANIM_TOBACKGROUND:
            if ( !clearFrameRect(dc, frame - 1) )
            {
                dc.SelectObject(wxNullBitmap);
                return false;
            }
            break;

        case wxANIM_TOPREVIOUS:
            // this disposal should never be used too often.
            // E.g. GIF specification explicitly say to keep the usage of this
            //      disposal limited to the minimum.
            // In fact it may require a lot of time to restore
            if (frame == 1)
            {
                // if 0-th frame disposal is to restore to previous frame,
                // the best we can do is to restore to background
                if ( !clearBackground(dc) )
                {
                    dc.SelectObject(wxNullBitmap);
                    return false;
                }
            }
            else
            {
                dc.SelectObject(wxNullBitmap);
                return RebuildBackingStoreUpToFrame(frame);
            }
            break;

        case wxANIM_DONOTREMOVE:
        case wxANIM_UNSPECIFIED:
            break;
        }
    }

    // now just draw the current frame on the top of the backing store
    if ( !drawFrame(dc, frame) )
    {
        dc.SelectObject(wxNullBitmap);
        return false;
    }

    dc.SelectObject(wxNullBitmap);
    if ( !isCurrent() )
        return false;

    m_backingStore = candidate;
    return true;
}

void wxGenericAnimationCtrl::DisplayStaticImage()
{
    wxASSERT(!IsPlaying());
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const std::uint64_t displayGeneration = m_timerGeneration;

    const wxSize clientSize = GetClientSize();
    if ( clientSize.x <= 0 || clientSize.y <= 0 )
    {
        // A transient 0x0 layout is not evidence that the decoded animation is
        // invalid. Drop only the derived surface and rebuild it on the next
        // positive size event.
        m_backingStore = wxNullBitmap;
        Refresh();
        return;
    }

    // m_bmpStaticReal will be updated only if necessary. Do this after the
    // zero-size guard: the base implementation cannot create a 0x0 bitmap and
    // treats that allocation failure as an invalid inactive-bitmap source.
    UpdateStaticImage();
    if ( !lifetime.IsAlive() ||
            m_timerGeneration != displayGeneration )
    {
        return;
    }

    if (m_bmpStaticReal.IsOk() && m_backingStore.IsOk())
    {
        // Retain the selected bundle member across all extension callbacks.
        // In particular, never keep a DC selected into a member bitmap while
        // asking a decoder for its background colour: that callback may delete
        // this control and destroy the member bitmap synchronously.
        const wxBitmap staticBitmap = m_bmpStaticReal;

        // copy the inactive bitmap in the backing store
        // eventually using the mask or the alpha if the static
        // bitmap has one
        if ( staticBitmap.GetMask() || staticBitmap.HasAlpha() )
        {
            wxColour background = GetBackgroundColour();
            if ( !IsUsingWindowBackgroundColour() )
            {
                background = AnimationImplGetBackgroundColour();
                if ( !lifetime.IsAlive() ||
                        m_timerGeneration != displayGeneration )
                {
                    return;
                }
            }

            wxBitmap candidate;
            if ( !candidate.Create(staticBitmap.GetWidth(),
                                   staticBitmap.GetHeight(),
                                   staticBitmap.GetDepth()) )
            {
                m_backingStore = wxNullBitmap;
                Refresh();
                return;
            }
            wxMemoryDC temp;
            temp.SelectObject(candidate);
            temp.SetBackground(wxBrush(background));
            temp.Clear();
            temp.DrawBitmap(staticBitmap, 0, 0, true /* use mask */);
            temp.SelectObject(wxNullBitmap);
            m_backingStore = candidate;
        }
        else
            m_backingStore = staticBitmap;
    }
    else
    {
        // put in the backing store the first frame of the animation
        if (!m_animation.IsOk())
        {
            m_backingStore = wxNullBitmap;
        }
        else
        {
            const std::uint64_t generation = m_timerGeneration;
            const bool rebuilt = RebuildBackingStoreUpToFrame(0);
            if ( !lifetime.IsAlive() )
                return;
            if ( !rebuilt && m_timerGeneration == generation )
            {
                // Rendering failure invalidates the derived surface only. The
                // decoder remains a valid, retryable animation.
                m_backingStore = wxNullBitmap;
            }
        }
    }

    if ( lifetime.IsAlive() )
        Refresh();
}

void wxGenericAnimationCtrl::DrawFrame(wxDC &dc, unsigned int frame)
{
    // PERFORMANCE NOTE:
    // this draw stuff is not as fast as possible: the wxAnimationDecoder
    // needs first to convert from its internal format to wxImage RGB24;
    // the wxImage is then converted as a wxBitmap and finally blitted.
    // If wxAnimationDecoder had a function to convert directly from its
    // internal format to a port-specific wxBitmap, it would be somewhat faster.
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const std::uint64_t generation = m_timerGeneration;
    const wxAnimation animation = m_animation;
    wxAnimationGenericImpl* const implementation =
        static_cast<wxAnimationGenericImpl*>(GetAnimImpl());
    if ( !implementation )
        return;

    const wxImage image = animation.GetFrame(frame);
    if ( !lifetime.IsAlive() || m_timerGeneration != generation ||
            GetAnimImpl() != implementation || !image.IsOk() )
    {
        return;
    }

    const wxPoint position = implementation->GetFramePosition(frame);
    if ( !lifetime.IsAlive() || m_timerGeneration != generation ||
            GetAnimImpl() != implementation )
    {
        return;
    }

    const wxBitmap bitmap(image);
    if ( bitmap.IsOk() )
        dc.DrawBitmap(bitmap, position, true /* use mask */);
}

void wxGenericAnimationCtrl::DrawCurrentFrame(wxDC& dc)
{
    wxASSERT( m_backingStore.IsOk() );

    // m_backingStore always contains the current frame
    dc.DrawBitmap(m_backingStore, 0, 0, true /* use mask in case it's present */);
}

void wxGenericAnimationCtrl::DisposeToBackground()
{
    // clear the backing store
    wxBitmap backingStore = m_backingStore;
    wxMemoryDC dc;
    dc.SelectObject(backingStore);
    if ( dc.IsOk() )
        DisposeToBackground(dc);
}

void wxGenericAnimationCtrl::DisposeToBackground(wxDC& dc)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const bool useWindowBackground = IsUsingWindowBackgroundColour();
    wxColour col = GetBackgroundColour();
    if ( !useWindowBackground )
    {
        col = AnimationImplGetBackgroundColour();
        if ( !lifetime.IsAlive() )
            return;
    }

    wxBrush brush(col);
    dc.SetBackground(brush);
    dc.Clear();
}

void wxGenericAnimationCtrl::DisposeToBackground(wxDC& dc, const wxPoint &pos, const wxSize &sz)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const bool useWindowBackground = IsUsingWindowBackgroundColour();
    wxColour col = GetBackgroundColour();
    if ( !useWindowBackground )
    {
        col = AnimationImplGetBackgroundColour();
        if ( !lifetime.IsAlive() )
            return;
    }
    wxBrush brush(col);
    dc.SetBrush(brush);         // SetBrush and not SetBackground !!
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(pos, sz);
}

// ----------------------------------------------------------------------------
// wxAnimationCtrl - event handlers
// ----------------------------------------------------------------------------

bool wxGenericAnimationCtrl::ScheduleNextFrame()
{
    const wxGenericAnimationCtrlLifetime lifetime(this);

    if ( !m_needToShowNextFrame )
        return true;

    m_needToShowNextFrame = false;
    if ( !m_isPlaying || m_pausedForHidden ||
            !IsShown() || !m_animation.IsOk() )
    {
        return true;
    }

    const std::uint64_t generation = m_timerGeneration;
    const wxAnimation animation = m_animation;
    wxAnimationImpl* const implementation = GetAnimImpl();
    const unsigned int frame = m_currentFrame;

    // GetDelay() belongs to a custom decoder and can re-enter or destroy this
    // control. Never inspect the owner until its weak lifetime was checked.
    int delay = animation.GetDelay(frame);
    if ( !lifetime.IsAlive() )
        return false;
    if ( m_timerGeneration != generation || !m_animation.IsOk() ||
            GetAnimImpl() != implementation )
    {
        return true;
    }

    if ( delay == -1 )
    {
        // -1 is the decoder contract for a frame that remains displayed
        // forever. Keep IsPlaying() true so Stop() stays meaningful, but
        // consume no timer or decoder work while holding this frame.
        return true;
    }
    if ( delay < -1 )
    {
        // No other negative value belongs to the decoder contract.
        Stop();
        return lifetime.IsAlive();
    }
    if ( delay == 0 )
        delay = wxGENERIC_ANIMATION_MIN_FRAME_DELAY_MS;
    if ( !m_timer.StartOnce(delay) )
    {
        Stop();
        return lifetime.IsAlive();
    }

    return true;
}

bool wxGenericAnimationCtrl::ResumeFrameSchedule(std::uint64_t generation)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);

    m_pausedForHidden = false;
    m_needToShowNextFrame = true;
    Refresh();
    if ( !lifetime.IsAlive() )
        return false;

    // The frame is already fully rebuilt and published. Arm it independently
    // of WM_PAINT: some ports don't synchronously paint a just-restored child,
    // and forcing Update() here would let a decoder delete the control from a
    // nested native paint stack.
    if ( m_timerGeneration == generation )
        return ScheduleNextFrame();

    return true;
}

void wxGenericAnimationCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    const wxGenericAnimationCtrlLifetime lifetime(this);

    // VERY IMPORTANT: the wxPaintDC *must* be created in any case
    wxPaintDC dc(this);

    if ( m_backingStore.IsOk() )
    {
        // NOTE: we draw the bitmap explicitly ignoring the mask (if any);
        //       i.e. we don't want to combine the backing store with the
        //       possibly wrong preexisting contents of the window!
        dc.DrawBitmap(m_backingStore, 0, 0, false /* no mask */);
    }
    else
    {
        // m_animation is not valid and thus we don't have a valid backing store...
        // clear then our area to the background colour
        DisposeToBackground(dc);
    }

    if ( !lifetime.IsAlive() )
        return;

    (void)ScheduleNextFrame();
}

void wxGenericAnimationCtrl::OnTimer(wxTimerEvent& event)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);

    if ( event.GetId() != m_activeTimerId ||
            !m_isPlaying || m_pausedForHidden ||
            !m_animation.IsOk() )
    {
        return;
    }

    // A one-shot notification is consumed exactly once. Advance the event ID
    // before touching decoder state so a duplicate/stale queued event is inert.
    InvalidateFrameSchedule();
    const std::uint64_t generation = m_timerGeneration;
    if ( !IsShown() )
    {
        m_pausedForHidden = true;
        return;
    }

    const wxAnimation animation = m_animation;
    wxAnimationImpl* const implementation = GetAnimImpl();
    const unsigned int frameCount = animation.GetFrameCount();
    if ( !lifetime.IsAlive() ||
            m_timerGeneration != generation || !m_animation.IsOk() ||
            GetAnimImpl() != implementation )
    {
        return;
    }
    if ( frameCount == 0 )
    {
        Stop();
        return;
    }

    m_currentFrame++;
    if (m_currentFrame == frameCount)
    {
        // Should a non-looped animation display the last frame?
        if (!m_looped)
        {
            Stop();
            return;
        }
        else
            m_currentFrame = 0;     // let's restart
    }

    if ( !IncrementalUpdateBackingStore() )
    {
        if ( lifetime.IsAlive() && m_timerGeneration == generation )
            Stop();
        return;
    }

    if ( !lifetime.IsAlive() || m_timerGeneration != generation )
        return;

    m_needToShowNextFrame = true;

    Refresh();
}

void wxGenericAnimationCtrl::OnSize(wxSizeEvent& event)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);

    // NB: resizing an animation control may take a lot of time
    //     for big animations as the backing store must be
    //     extended and rebuilt. Try to avoid it e.g. using
    //     a null proportion value for your wxAnimationCtrls
    //     when using them inside sizers.
    if (m_animation.IsOk() && IsPlaying())
    {
        // be careful to change the backing store *only* if we are
        // playing the animation as otherwise we may be displaying
        // the inactive bitmap and overwriting the backing store
        // with the last played frame is wrong in this case
        const wxSize clientSize = GetClientSize();
        if ( !IsShown() || clientSize.x <= 0 || clientSize.y <= 0 )
        {
            InvalidateFrameSchedule();
            m_pausedForHidden = true;
            if ( clientSize.x <= 0 || clientSize.y <= 0 )
                m_backingStore = wxNullBitmap;
        }
        else
        {
            const std::uint64_t generation = m_timerGeneration;
            const bool rebuilt =
                RebuildBackingStoreUpToFrame(m_currentFrame);
            if ( !lifetime.IsAlive() )
            {
                event.Skip();
                return;
            }

            if ( rebuilt )
            {
                if ( m_timerGeneration == generation && m_pausedForHidden )
                {
                    if ( !ResumeFrameSchedule(generation) )
                    {
                        event.Skip();
                        return;
                    }
                }
            }
            else if ( m_timerGeneration == generation )
            {
                Stop();
            }
        }
    }
    else
    {
        // Resize the inactive bitmap (or rebuild frame zero) as well. This is
        // especially important for wxAC_NO_AUTORESIZE controls.
        DisplayStaticImage();
    }

    event.Skip();
}

void wxGenericAnimationCtrl::OnShow(wxShowEvent& event)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);

    if ( !event.IsShown() )
    {
        if ( m_isPlaying )
        {
            InvalidateFrameSchedule();
            m_pausedForHidden = true;
        }
    }
    else if ( m_isPlaying && m_pausedForHidden )
    {
        const wxSize clientSize = GetClientSize();
        if ( clientSize.x <= 0 || clientSize.y <= 0 )
        {
            // Remain paused until a positive OnSize notification arrives.
        }
        else
        {
            const std::uint64_t generation = m_timerGeneration;
            const bool rebuilt =
                RebuildBackingStoreUpToFrame(m_currentFrame);
            if ( !lifetime.IsAlive() )
            {
                event.Skip();
                return;
            }

            if ( rebuilt )
            {
                if ( m_timerGeneration == generation )
                {
                    if ( !ResumeFrameSchedule(generation) )
                    {
                        event.Skip();
                        return;
                    }
                }
            }
            else if ( m_timerGeneration == generation )
            {
                Stop();
            }
        }
    }

    event.Skip();
}

void wxGenericAnimationCtrl::WXHandleDPIChanged(wxDPIChangedEvent& event)
{
    const wxGenericAnimationCtrlLifetime lifetime(this);
    const std::uint64_t generation = m_timerGeneration;
    const bool wasPlaying = m_isPlaying;
    const bool wasLooped = m_looped;
    const wxAnimations animations = m_animations;

    const wxAnimation selected =
        SelectAnimationForScale(animations, GetDPIScaleFactor());
    if ( !lifetime.IsAlive() || m_timerGeneration != generation )
    {
        event.Skip();
        return;
    }

    wxAnimationBundle bundle;
    for ( const wxAnimation& animation : animations )
    {
        bundle.Add(animation);
        if ( !lifetime.IsAlive() || m_timerGeneration != generation )
        {
            event.Skip();
            return;
        }
    }

    const std::uint64_t selectionGeneration =
        wxGenericAnimationNextGeneration(generation);
    SetAnimation(bundle);
    if ( !lifetime.IsAlive() )
    {
        event.Skip();
        return;
    }

    bool sameBundle = m_animations.size() == animations.size();
    for ( std::size_t i = 0; sameBundle && i < animations.size(); ++i )
        sameBundle = m_animations[i].IsSameAs(animations[i]);

    if ( m_timerGeneration != selectionGeneration || !sameBundle ||
            !m_animation.IsSameAs(selected) )
    {
        // A decoder callback published a newer animation transaction.
        event.Skip();
        return;
    }

    UpdateStaticImage();
    if ( !lifetime.IsAlive() ||
            m_timerGeneration != selectionGeneration )
    {
        event.Skip();
        return;
    }

    if ( wasPlaying )
    {
        const std::uint64_t playGeneration =
            wxGenericAnimationNextGeneration(selectionGeneration);
        (void)Play(wasLooped);
        if ( !lifetime.IsAlive() ||
                m_timerGeneration != playGeneration )
        {
            event.Skip();
            return;
        }
    }

    event.Skip();
}

// ----------------------------------------------------------------------------
// helpers to safely access wxAnimationGenericImpl methods
// ----------------------------------------------------------------------------
#define ANIMATION (static_cast<wxAnimationGenericImpl*>(GetAnimImpl()))

wxPoint wxGenericAnimationCtrl::AnimationImplGetFramePosition(unsigned int frame) const
{
    wxCHECK_MSG( m_animation.IsOk(), wxDefaultPosition, wxT("invalid animation") );
    return ANIMATION->GetFramePosition(frame);
}

wxSize wxGenericAnimationCtrl::AnimationImplGetFrameSize(unsigned int frame) const
{
    wxCHECK_MSG( m_animation.IsOk(), wxDefaultSize, wxT("invalid animation") );
    return ANIMATION->GetFrameSize(frame);
}

wxAnimationDisposal wxGenericAnimationCtrl::AnimationImplGetDisposalMethod(unsigned int frame) const
{
    wxCHECK_MSG( m_animation.IsOk(), wxANIM_UNSPECIFIED, wxT("invalid animation") );
    return ANIMATION->GetDisposalMethod(frame);
}

wxColour wxGenericAnimationCtrl::AnimationImplGetTransparentColour(unsigned int frame) const
{
    wxCHECK_MSG( m_animation.IsOk(), wxNullColour, wxT("invalid animation") );
    return ANIMATION->GetTransparentColour(frame);
}

wxColour wxGenericAnimationCtrl::AnimationImplGetBackgroundColour() const
{
    wxCHECK_MSG( m_animation.IsOk(), wxNullColour, wxT("invalid animation") );
    return ANIMATION->GetBackgroundColour();
}

#endif // wxUSE_ANIMATIONCTRL
