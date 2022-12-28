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


#if wxUSE_ANIMATIONCTRL

#include "wx/animate.h"
#include "wx/bitmap.h"
#include "wx/log.h"
#include "wx/brush.h"
#include "wx/image.h"
#include "wx/dcmemory.h"
#include "wx/module.h"

#include "wx/gifdecod.h"
#include "wx/anidecod.h"

#include "wx/private/animate.h"

// global objects
const char wxAnimationCtrlNameStr[] = "animationctrl";
wxAnimation wxNullAnimation;

wxIMPLEMENT_DYNAMIC_CLASS(wxAnimation, wxObject);

#if !defined(wxHAS_NATIVE_ANIMATIONCTRL)
    // In this case the "native" ctrl is the generic ctrl. See wx/animate.h
    // Notice that it's important to use wxGenericAnimationCtrl here because
    // wxAnimation::IsCompatibleWith() relies on control deriving from
    // wxGenericAnimationCtrl when using generic wxAnimation implementation.
    wxIMPLEMENT_CLASS(wxAnimationCtrl, wxGenericAnimationCtrl);
#endif

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxAnimationDecoderList)

wxAnimationDecoderList wxAnimation::sm_handlers;

// ----------------------------------------------------------------------------
// wxAnimation
// ----------------------------------------------------------------------------

wxAnimation::wxAnimation()
{
    m_refData = wxAnimationImpl::CreateDefault();
}

wxAnimation::wxAnimation(wxAnimationImpl* impl)
{
    m_refData = impl;
}

wxAnimation::wxAnimation(const wxString &name, wxAnimationType type)
{
    m_refData = wxAnimationImpl::CreateDefault();
    LoadFile(name, type);
}

wxAnimationImpl* wxAnimation::GetImpl() const
{
    return static_cast<wxAnimationImpl*>(m_refData);
}

bool wxAnimation::IsOk() const
{
    return GetImpl() && GetImpl()->IsOk();
}

bool wxAnimation::IsCompatibleWith(wxClassInfo* ci) const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid animation") );

    return GetImpl()->IsCompatibleWith(ci);
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

wxImage wxAnimation::GetFrame(unsigned int frame) const
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
    // file, so just check GetImpl in the Load methods
    wxCHECK_MSG( GetImpl(), false, wxT("invalid animation") );
    return GetImpl()->LoadFile(name, type);
}

bool wxAnimation::Load(wxInputStream& stream, wxAnimationType type)
{
    wxCHECK_MSG( GetImpl(), false, wxT("invalid animation") );
    return GetImpl()->Load(stream, type);
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
    if (sz.GetWidth() != m_bmpStaticReal.GetLogicalWidth() ||
        sz.GetHeight() != m_bmpStaticReal.GetLogicalHeight())
    {
        wxBitmap bmpCurrent = m_bmpStatic.GetBitmapFor(this);

        if (!m_bmpStaticReal.IsOk() ||
            m_bmpStaticReal.GetLogicalWidth() != sz.GetWidth() ||
            m_bmpStaticReal.GetLogicalHeight() != sz.GetHeight())
        {
            // need to (re)create m_bmpStaticReal
            if (!m_bmpStaticReal.CreateWithDIPSize(sz,
                                          bmpCurrent.GetScaleFactor(),
                                          bmpCurrent.GetDepth()))
            {
                wxLogDebug(wxT("Cannot create the static bitmap"));
                m_bmpStatic = wxNullBitmap;
                return;
            }
        }

        if (bmpCurrent.GetLogicalWidth() <= sz.GetWidth() &&
            bmpCurrent.GetLogicalHeight() <= sz.GetHeight())
        {
            // clear the background of m_bmpStaticReal
            wxBrush brush(GetBackgroundColour());
            wxMemoryDC dc;
            dc.SelectObject(m_bmpStaticReal);
            dc.SetBackground(brush);
            dc.Clear();

            // center the user-provided bitmap in m_bmpStaticReal
            dc.DrawBitmap(bmpCurrent,
                        (sz.GetWidth()-bmpCurrent.GetLogicalWidth())/2,
                        (sz.GetHeight()-bmpCurrent.GetLogicalHeight())/2,
                        true /* use mask */ );
        }
        else
        {
            // the user-provided bitmap is bigger than our control, strech it
            wxImage temp(bmpCurrent.ConvertToImage());
            temp.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);
            m_bmpStaticReal = wxBitmap(temp);
        }
    }
}

void wxAnimationCtrlBase::SetInactiveBitmap(const wxBitmapBundle &bmp)
{
    m_bmpStatic = bmp;
    m_bmpStaticReal = bmp.GetBitmapFor(this);

    // if not playing, update the control now
    // NOTE: DisplayStaticImage() will call UpdateStaticImage automatically
    if ( !IsPlaying() )
        DisplayStaticImage();
}

// ----------------------------------------------------------------------------
// animation decoders
// ----------------------------------------------------------------------------

void wxAnimation::AddHandler( wxAnimationDecoder *handler )
{
    // Check for an existing handler of the type being added.
    if (FindHandler( handler->GetType() ) == nullptr)
    {
        sm_handlers.Append( handler );
    }
    else
    {
        // This is not documented behaviour, merely the simplest 'fix'
        // for preventing duplicate additions.  If someone ever has
        // a good reason to add and remove duplicate handlers (and they
        // may) we should probably refcount the duplicates.

        wxLogDebug( wxT("Adding duplicate animation handler for '%d' type"),
                    handler->GetType() );
        delete handler;
    }
}

void wxAnimation::InsertHandler( wxAnimationDecoder *handler )
{
    // Check for an existing handler of the type being added.
    if (FindHandler( handler->GetType() ) == nullptr)
    {
        sm_handlers.Insert( handler );
    }
    else
    {
        // see AddHandler for additional comments.
        wxLogDebug( wxT("Inserting duplicate animation handler for '%d' type"),
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
    return nullptr;
}

void wxAnimation::InitStandardHandlers()
{
#if wxUSE_GIF
    AddHandler(new wxGIFDecoder);
#endif // wxUSE_GIF
#if wxUSE_ICO_CUR
    AddHandler(new wxANIDecoder);
#endif // wxUSE_ICO_CUR
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
    wxDECLARE_DYNAMIC_CLASS(wxAnimationModule);
public:
    wxAnimationModule() {}
    bool OnInit() override { wxAnimation::InitStandardHandlers(); return true; }
    void OnExit() override { wxAnimation::CleanUpHandlers(); }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxAnimationModule, wxModule);

#endif      // wxUSE_ANIMATIONCTRL
