/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/timectrl.cpp
// Purpose:     wxWinUI wxTimePickerCtrl implementation (WinUI TimePicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_TIMEPICKCTRL

#include "wx/timectrl.h"
#include "wx/dateevt.h"

#include "private.h"

#include <chrono>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace WF = winrt::Windows::Foundation;

namespace
{

// A fixed date (on which DST does not change) used to hold the time value.
wxDateTime wxWinUITimeFromSeconds(int totalSeconds)
{
    return wxDateTime(1, wxDateTime::Jan, 2012,
                      totalSeconds / 3600,
                      (totalSeconds / 60) % 60,
                      totalSeconds % 60);
}

} // namespace

class wxWinUITimePickerImpl
{
public:
    wxWinUIControlHost host;
    MUXC::TimePicker picker{ nullptr };
    winrt::event_token selectedTimeChangedToken{};
};

wxIMPLEMENT_DYNAMIC_CLASS(wxTimePickerCtrl, wxControl);

wxTimePickerCtrl::wxTimePickerCtrl()
{
}

wxTimePickerCtrl::wxTimePickerCtrl(wxWindow *parent,
                                   wxWindowID id,
                                   const wxDateTime& dt,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator& validator,
                                   const wxString& name)
{
    Create(parent, id, dt, pos, size, style, validator, name);
}

wxTimePickerCtrl::~wxTimePickerCtrl() = default;

bool wxTimePickerCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxDateTime& dt,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_value = dt.IsValid() ? dt : wxDateTime::Now();

    m_winui.reset(new wxWinUITimePickerImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->picker = MUXC::TimePicker();
        m_winui->picker.ClockIdentifier(L"24HourClock");
        m_winui->selectedTimeChangedToken = m_winui->picker.SelectedTimeChanged(
            [this](MUXC::TimePicker const&,
                   MUXC::TimePickerSelectedValueChangedEventArgs const&)
            {
                if ( !m_winui || m_updating )
                    return;
                OnPeerTimeChanged();
            });

        ApplyToPeer();
        m_winui->host.SetContent(m_winui->picker);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI TimePicker creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    if ( dt.IsValid() )
        m_value = dt;
    ApplyToPeer();
}

wxDateTime wxTimePickerCtrl::GetValue() const
{
    return m_value;
}

wxSize wxTimePickerCtrl::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(200, 32), const_cast<wxTimePickerCtrl*>(this));
}

void wxTimePickerCtrl::ApplyToPeer()
{
    if ( !m_winui || !m_winui->picker || !m_value.IsValid() )
        return;

    const wxDateTime::Tm tm = m_value.GetTm();
    const WF::TimeSpan ts = std::chrono::hours(tm.hour) +
                            std::chrono::minutes(tm.min) +
                            std::chrono::seconds(tm.sec);

    m_updating = true;
    try
    {
        m_winui->picker.SelectedTime(
            winrt::box_value(ts).as<WF::IReference<WF::TimeSpan>>());
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

void wxTimePickerCtrl::OnPeerTimeChanged()
{
    try
    {
        if ( auto ref = m_winui->picker.SelectedTime() )
        {
            const auto secs =
                std::chrono::duration_cast<std::chrono::seconds>(ref.Value());
            m_value = wxWinUITimeFromSeconds(static_cast<int>(secs.count()));
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return;
    }

    wxDateEvent event(this, m_value, wxEVT_TIME_CHANGED);
    HandleWindowEvent(event);
}

#endif // wxUSE_TIMEPICKCTRL
