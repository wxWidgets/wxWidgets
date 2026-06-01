/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/gauge.cpp
// Purpose:     wxWinUI wxGauge implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GAUGE

#include "wx/gauge.h"

#include "private.h"

class wxWinUIGaugeImpl
{
public:
    wxWinUIControlHost host;
    winrt::Microsoft::UI::Xaml::Controls::ProgressBar progressBar{ nullptr };
};

wxGauge::wxGauge()
{
}

wxGauge::wxGauge(wxWindow *parent,
                 wxWindowID id,
                 int range,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxValidator& validator,
                 const wxString& name)
{
    Create(parent, id, range, pos, size, style, validator, name);
}

wxGauge::~wxGauge() = default;

bool wxGauge::Create(wxWindow *parent,
                     wxWindowID id,
                     int range,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_rangeMax = range;
    m_gaugePos = 0;

    m_winui.reset(new wxWinUIGaugeImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->progressBar =
            winrt::Microsoft::UI::Xaml::Controls::ProgressBar();
        m_winui->progressBar.Minimum(0);
        ApplyToPeer();
        m_winui->host.SetContent(m_winui->progressBar);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ProgressBar creation", e);
        return false;
    }

    return true;
}

void wxGauge::SetRange(int range)
{
    m_rangeMax = range;
    ApplyToPeer();
}

int wxGauge::GetRange() const
{
    return m_rangeMax;
}

void wxGauge::SetValue(int pos)
{
    m_gaugePos = pos;
    m_indeterminate = false;
    ApplyToPeer();
}

int wxGauge::GetValue() const
{
    return m_gaugePos;
}

void wxGauge::Pulse()
{
    m_indeterminate = true;
    ApplyToPeer();
}

wxSize wxGauge::DoGetBestSize() const
{
    // A WinUI ProgressBar is a thin horizontal bar; use a sensible default
    // width and the control's natural (small) height.
    return wxWindow::FromDIP(wxSize(100, 18), const_cast<wxGauge *>(this));
}

void wxGauge::ApplyToPeer()
{
    if ( !m_winui || !m_winui->progressBar )
        return;

    m_winui->progressBar.IsIndeterminate(m_indeterminate);
    if ( !m_indeterminate )
    {
        m_winui->progressBar.Maximum(m_rangeMax > 0 ? m_rangeMax : 1);
        m_winui->progressBar.Value(m_gaugePos);
    }

    m_winui->host.ForceRender();
}

#endif // wxUSE_GAUGE
