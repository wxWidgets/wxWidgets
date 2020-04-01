/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/animatecmn.cpp
// Purpose:     wxAnimation and wxAnimationCtrl
// Author:      Francesco Montorsi
// Modified By:
// Created:     24/09/2006
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_ANIMATIONCTRL

#include "wx/animate.h"
#include "wx/bitmap.h"
#include "wx/log.h"
#include "wx/brush.h"
#include "wx/image.h"
#include "wx/dcmemory.h"

const char wxAnimationCtrlNameStr[] = "animationctrl";
wxAnimation wxNullAnimation;

wxIMPLEMENT_ABSTRACT_CLASS(wxAnimationImpl, wxObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxAnimation, wxObject);
wxIMPLEMENT_ABSTRACT_CLASS(wxAnimationCtrlBase, wxControl);

#if !defined(__WXGTK20__)
    // In this case the "native" ctrl is the generic ctrl. See wx/animate.h
    wxIMPLEMENT_CLASS(wxAnimationCtrl, wxGenericAnimationCtrl);
#endif


// ----------------------------------------------------------------------------
// wxAnimation
// ----------------------------------------------------------------------------

wxAnimation::wxAnimation(wxAnimationImplType implType)
{
    m_refData = wxAnimationCtrl::CreateAnimationImpl(implType);
}

wxAnimation::wxAnimation(const wxString &name, wxAnimationType type,
                         wxAnimationImplType implType)
{
    m_refData = wxAnimationCtrl::CreateAnimationImpl(implType);
    LoadFile(name, type);
}

wxAnimation::wxAnimation(const wxAnimation& other)
{
    Ref(other);
}

int wxAnimation::GetDelay(unsigned int frame) const
{
    wxCHECK_MSG( IsOk(), -1, wxT("invalid animation") );
    return GetImpl()->GetDelay(frame);
}

unsigned int wxAnimation::GetFrameCount() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid animation") );
    return GetImpl()->GetFrameCount();
}

wxImage wxAnimation::GetFrame(unsigned int frame)
{
    wxCHECK_MSG( IsOk(), wxNullImage, wxT("invalid animation") );
    return GetImpl()->GetFrame(frame);
}

wxSize wxAnimation::GetSize() const
{
    wxCHECK_MSG( IsOk(), wxDefaultSize, wxT("invalid animation") );
    return GetImpl()->GetSize();
}

bool wxAnimation::LoadFile(const wxString& name, wxAnimationType type)
{
    // the animation impl may not be fully ready until after it has loaded the
    // file, so just check GetImpl the Load methods
    wxCHECK_MSG( GetImpl(), false, wxT("invalid animation") );
    return GetImpl()->LoadFile(name, type);
}

bool wxAnimation::Load(wxInputStream& stream, wxAnimationType type)
{
    wxCHECK_MSG( GetImpl(), false, wxT("invalid animation") );
    return GetImpl()->Load(stream, type);
}

wxPoint wxAnimation::GetFramePosition(unsigned int frame) const
{
    wxCHECK_MSG( IsOk(), wxDefaultPosition, wxT("invalid animation") );
    return GetImpl()->GetFramePosition(frame);
}

wxSize wxAnimation::GetFrameSize(unsigned int frame) const
{
    wxCHECK_MSG( IsOk(), wxDefaultSize, wxT("invalid animation") );
    return GetImpl()->GetFrameSize(frame);
}

wxAnimationDisposal wxAnimation::GetDisposalMethod(unsigned int frame) const
{
    wxCHECK_MSG( IsOk(), wxANIM_UNSPECIFIED, wxT("invalid animation") );
    return GetImpl()->GetDisposalMethod(frame);
}

wxColour wxAnimation::GetTransparentColour(unsigned int frame) const
{
    wxCHECK_MSG( IsOk(), wxNullColour, wxT("invalid animation") );
    return GetImpl()->GetTransparentColour(frame);
}

wxColour wxAnimation::GetBackgroundColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, wxT("invalid animation") );
    return GetImpl()->GetBackgroundColour();
}


// ----------------------------------------------------------------------------
// wxAnimationCtrlBase
// ----------------------------------------------------------------------------

void wxAnimationCtrlBase::UpdateStaticImage()
{
    if (!m_bmpStaticReal.IsOk() || !m_bmpStatic.IsOk())
        return;

    // if given bitmap is not of the right size, recreate m_bmpStaticReal accordingly
    const wxSize &sz = GetClientSize();
    if (sz.GetWidth() != m_bmpStaticReal.GetWidth() ||
        sz.GetHeight() != m_bmpStaticReal.GetHeight())
    {
        if (!m_bmpStaticReal.IsOk() ||
            m_bmpStaticReal.GetWidth() != sz.GetWidth() ||
            m_bmpStaticReal.GetHeight() != sz.GetHeight())
        {
            // need to (re)create m_bmpStaticReal
            if (!m_bmpStaticReal.Create(sz.GetWidth(), sz.GetHeight(),
                                        m_bmpStatic.GetDepth()))
            {
                wxLogDebug(wxT("Cannot create the static bitmap"));
                m_bmpStatic = wxNullBitmap;
                return;
            }
        }

        if (m_bmpStatic.GetWidth() <= sz.GetWidth() &&
            m_bmpStatic.GetHeight() <= sz.GetHeight())
        {
            // clear the background of m_bmpStaticReal
            wxBrush brush(GetBackgroundColour());
            wxMemoryDC dc;
            dc.SelectObject(m_bmpStaticReal);
            dc.SetBackground(brush);
            dc.Clear();

            // center the user-provided bitmap in m_bmpStaticReal
            dc.DrawBitmap(m_bmpStatic,
                        (sz.GetWidth()-m_bmpStatic.GetWidth())/2,
                        (sz.GetHeight()-m_bmpStatic.GetHeight())/2,
                        true /* use mask */ );
        }
        else
        {
            // the user-provided bitmap is bigger than our control, strech it
            wxImage temp(m_bmpStatic.ConvertToImage());
            temp.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);
            m_bmpStaticReal = wxBitmap(temp);
        }
    }
}

void wxAnimationCtrlBase::SetInactiveBitmap(const wxBitmap &bmp)
{
    m_bmpStatic = bmp;
    m_bmpStaticReal = bmp;

    // if not playing, update the control now
    // NOTE: DisplayStaticImage() will call UpdateStaticImage automatically
    if ( !IsPlaying() )
        DisplayStaticImage();
}

#endif      // wxUSE_ANIMATIONCTRL
