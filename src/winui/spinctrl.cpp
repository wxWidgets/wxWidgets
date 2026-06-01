/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/spinctrl.cpp
// Purpose:     wxWinUI wxSpinCtrl / wxSpinCtrlDouble (WinUI NumberBox)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
#endif

#include "private.h"

#include <cmath>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

// Normally defined by the generic wxSpinCtrlDouble implementation, which is not
// built under the WinUI toolkit.
wxIMPLEMENT_DYNAMIC_CLASS(wxSpinDoubleEvent, wxNotifyEvent);

class wxWinUINumberBoxImpl
{
public:
    wxWinUIControlHost host;
    MUXC::NumberBox box{ nullptr };
    winrt::event_token valueChangedToken{};
};

namespace
{

// Create and configure a NumberBox showing inline spin buttons.
MUXC::NumberBox wxWinUICreateNumberBox()
{
    MUXC::NumberBox box;
    box.SpinButtonPlacementMode(MUXC::NumberBoxSpinButtonPlacementMode::Inline);
    box.ValidationMode(MUXC::NumberBoxValidationMode::InvalidInputOverwritten);
    return box;
}

} // namespace

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

wxSpinCtrl::wxSpinCtrl()
{
}

wxSpinCtrl::wxSpinCtrl(wxWindow *parent,
                       wxWindowID id,
                       const wxString& value,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       int min, int max, int initial,
                       const wxString& name)
{
    Create(parent, id, value, pos, size, style, min, max, initial, name);
}

wxSpinCtrl::~wxSpinCtrl() = default;

bool wxSpinCtrl::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        int min, int max, int initial,
                        const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_min = min;
    m_max = max;
    m_value = initial;

    long parsed;
    if ( !value.empty() && value.ToLong(&parsed) )
        m_value = static_cast<int>(parsed);

    m_winui.reset(new wxWinUINumberBoxImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->box = wxWinUICreateNumberBox();
        m_winui->valueChangedToken = m_winui->box.ValueChanged(
            [this](MUXC::NumberBox const&,
                   MUXC::NumberBoxValueChangedEventArgs const& args)
            {
                if ( !m_winui || m_updating )
                    return;
                OnPeerValueChanged(args.NewValue());
            });

        ApplyToPeer();
        m_winui->host.SetContent(m_winui->box);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI NumberBox creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxSpinCtrl::SetValue(int value)
{
    m_value = wxClip(value, m_min, m_max);
    ApplyToPeer();
}

void wxSpinCtrl::SetValue(const wxString& value)
{
    long parsed;
    if ( value.ToLong(&parsed) )
        SetValue(static_cast<int>(parsed));
}

void wxSpinCtrl::SetRange(int minVal, int maxVal)
{
    m_min = minVal;
    m_max = maxVal;
    m_value = wxClip(m_value, m_min, m_max);
    ApplyToPeer();
}

void wxSpinCtrl::SetIncrement(int inc)
{
    m_increment = inc;
    ApplyToPeer();
}

wxString wxSpinCtrl::GetTextValue() const
{
    return wxString::Format("%d", m_value);
}

bool wxSpinCtrl::SetBase(int base)
{
    // WinUI NumberBox only supports decimal input.
    if ( base != 10 )
        return false;
    m_numBase = base;
    return true;
}

void wxSpinCtrl::SetSelection(long WXUNUSED(from), long WXUNUSED(to))
{
    // Text selection is not exposed by the WinUI NumberBox.
}

wxSize wxSpinCtrl::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(110, 32), const_cast<wxSpinCtrl*>(this));
}

void wxSpinCtrl::ApplyToPeer()
{
    if ( !m_winui || !m_winui->box )
        return;

    m_updating = true;
    try
    {
        m_winui->box.Minimum(m_min);
        m_winui->box.Maximum(m_max);
        m_winui->box.SmallChange(m_increment);
        m_winui->box.LargeChange(m_increment);
        m_winui->box.Value(m_value);
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

void wxSpinCtrl::OnPeerValueChanged(double newValue)
{
    if ( std::isnan(newValue) )
        return;

    const int v = wxClip(static_cast<int>(std::lround(newValue)), m_min, m_max);
    if ( v == m_value )
        return;

    m_value = v;

    wxSpinEvent event(wxEVT_SPINCTRL, GetId());
    event.SetEventObject(this);
    event.SetInt(m_value);
    HandleWindowEvent(event);
}

//-----------------------------------------------------------------------------
// wxSpinCtrlDouble
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxSpinCtrlDouble, wxControl);

wxSpinCtrlDouble::wxSpinCtrlDouble()
{
}

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent,
                                   wxWindowID id,
                                   const wxString& value,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   double min, double max, double initial,
                                   double inc,
                                   const wxString& name)
{
    Create(parent, id, value, pos, size, style, min, max, initial, inc, name);
}

wxSpinCtrlDouble::~wxSpinCtrlDouble() = default;

bool wxSpinCtrlDouble::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& value,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              double min, double max, double initial,
                              double inc,
                              const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_min = min;
    m_max = max;
    m_value = initial;
    m_increment = inc;

    double parsed;
    if ( !value.empty() && value.ToDouble(&parsed) )
        m_value = parsed;

    m_winui.reset(new wxWinUINumberBoxImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->box = wxWinUICreateNumberBox();
        m_winui->valueChangedToken = m_winui->box.ValueChanged(
            [this](MUXC::NumberBox const&,
                   MUXC::NumberBoxValueChangedEventArgs const& args)
            {
                if ( !m_winui || m_updating )
                    return;
                OnPeerValueChanged(args.NewValue());
            });

        ApplyToPeer();
        m_winui->host.SetContent(m_winui->box);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI NumberBox creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxSpinCtrlDouble::SetValue(double value)
{
    m_value = wxClip(value, m_min, m_max);
    ApplyToPeer();
}

void wxSpinCtrlDouble::SetValue(const wxString& value)
{
    double parsed;
    if ( value.ToDouble(&parsed) )
        SetValue(parsed);
}

void wxSpinCtrlDouble::SetRange(double minVal, double maxVal)
{
    m_min = minVal;
    m_max = maxVal;
    m_value = wxClip(m_value, m_min, m_max);
    ApplyToPeer();
}

void wxSpinCtrlDouble::SetIncrement(double inc)
{
    m_increment = inc;
    ApplyToPeer();
}

void wxSpinCtrlDouble::SetDigits(unsigned digits)
{
    m_digits = digits;
}

wxString wxSpinCtrlDouble::GetTextValue() const
{
    return wxString::FromDouble(m_value, static_cast<int>(m_digits));
}

void wxSpinCtrlDouble::SetSelection(long WXUNUSED(from), long WXUNUSED(to))
{
}

wxSize wxSpinCtrlDouble::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(120, 32), const_cast<wxSpinCtrlDouble*>(this));
}

void wxSpinCtrlDouble::ApplyToPeer()
{
    if ( !m_winui || !m_winui->box )
        return;

    m_updating = true;
    try
    {
        m_winui->box.Minimum(m_min);
        m_winui->box.Maximum(m_max);
        m_winui->box.SmallChange(m_increment);
        m_winui->box.LargeChange(m_increment);
        m_winui->box.Value(m_value);
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

void wxSpinCtrlDouble::OnPeerValueChanged(double newValue)
{
    if ( std::isnan(newValue) )
        return;

    const double v = wxClip(newValue, m_min, m_max);
    if ( v == m_value )
        return;

    m_value = v;

    wxSpinDoubleEvent event(wxEVT_SPINCTRLDOUBLE, GetId(), m_value);
    event.SetEventObject(this);
    HandleWindowEvent(event);
}

#endif // wxUSE_SPINCTRL
