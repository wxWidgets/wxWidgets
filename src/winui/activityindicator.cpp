/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/activityindicator.cpp
// Purpose:     wxWinUI wxActivityIndicator implementation (WinUI ProgressRing)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_ACTIVITYINDICATOR

#include "wx/activityindicator.h"

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

class wxWinUIActivityIndicatorImpl
{
public:
    wxWinUIControlHost host;
    MUXC::ProgressRing ring{ nullptr };
};

wxIMPLEMENT_DYNAMIC_CLASS(wxActivityIndicator, wxActivityIndicatorBase);

wxActivityIndicator::wxActivityIndicator()
{
}

wxActivityIndicator::wxActivityIndicator(wxWindow* parent,
                                         wxWindowID winid,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style,
                                         const wxString& name)
{
    Create(parent, winid, pos, size, style, name);
}

wxActivityIndicator::~wxActivityIndicator() = default;

bool wxActivityIndicator::Create(wxWindow* parent,
                                 wxWindowID winid,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, winid, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_winui.reset(new wxWinUIActivityIndicatorImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->ring = MUXC::ProgressRing();
        m_winui->ring.IsActive(false);
        m_winui->host.SetContent(m_winui->ring);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ProgressRing creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxActivityIndicator::Start()
{
    m_running = true;
    if ( m_winui && m_winui->ring )
    {
        m_winui->ring.IsActive(true);
        m_winui->host.ForceRender();
    }
}

void wxActivityIndicator::Stop()
{
    m_running = false;
    if ( m_winui && m_winui->ring )
    {
        m_winui->ring.IsActive(false);
        m_winui->host.ForceRender();
    }
}

bool wxActivityIndicator::IsRunning() const
{
    return m_running;
}

wxSize wxActivityIndicator::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(40, 40), const_cast<wxActivityIndicator*>(this));
}

#endif // wxUSE_ACTIVITYINDICATOR
