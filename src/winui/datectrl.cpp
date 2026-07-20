/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/datectrl.cpp
// Purpose:     wxWinUI wxDatePickerCtrl implementation (CalendarDatePicker)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_DATEPICKCTRL

#include "wx/datectrl.h"
#include "wx/dateevt.h"

#include "private.h"

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace WF = winrt::Windows::Foundation;

namespace
{

WF::DateTime wxWinUIToDateTime(const wxDateTime& dt)
{
    return winrt::clock::from_time_t(dt.GetTicks());
}

wxDateTime wxWinUIFromDateTime(const WF::DateTime& wdt)
{
    wxDateTime dt;
    dt.Set(static_cast<time_t>(winrt::clock::to_time_t(wdt)));
    return dt;
}

WF::IReference<WF::DateTime> wxWinUIDateRef(const wxDateTime& dt)
{
    if ( !dt.IsValid() )
        return nullptr;
    return winrt::box_value(wxWinUIToDateTime(dt))
               .as<WF::IReference<WF::DateTime>>();
}

} // namespace

class wxWinUIDatePickerImpl
{
public:
    wxWinUIControlHost host;
    MUXC::CalendarDatePicker picker{ nullptr };
    winrt::event_token dateChangedToken{};
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl);

wxDatePickerCtrl::wxDatePickerCtrl()
{
}

wxDatePickerCtrl::wxDatePickerCtrl(wxWindow *parent,
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

wxDatePickerCtrl::~wxDatePickerCtrl() = default;

bool wxDatePickerCtrl::Create(wxWindow *parent,
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

    if ( dt.IsValid() || (style & wxDP_ALLOWNONE) )
        m_value = dt;
    else
        m_value = wxDateTime::Today();

    m_winui.reset(new wxWinUIDatePickerImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->picker = MUXC::CalendarDatePicker();
        m_winui->dateChangedToken = m_winui->picker.DateChanged(
            [this](MUXC::CalendarDatePicker const&,
                   MUXC::CalendarDatePickerDateChangedEventArgs const&)
            {
                if ( !m_winui || m_updating )
                    return;
                OnPeerDateChanged();
            });

        ApplyToPeer();
        m_winui->host.SetContent(m_winui->picker);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CalendarDatePicker creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxCHECK_RET( dt.IsValid() || HasFlag(wxDP_ALLOWNONE),
                 wxT("this control requires a valid date") );

    m_value = dt;
    ApplyToPeer();
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
    return m_value;
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    m_rangeMin = dt1;
    m_rangeMax = dt2;
    ApplyToPeer();
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    if ( dt1 )
        *dt1 = m_rangeMin;
    if ( dt2 )
        *dt2 = m_rangeMax;
    return m_rangeMin.IsValid() || m_rangeMax.IsValid();
}

wxSize wxDatePickerCtrl::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(200, 32), const_cast<wxDatePickerCtrl*>(this));
}

void wxDatePickerCtrl::ApplyToPeer()
{
    if ( !m_winui || !m_winui->picker )
        return;

    m_updating = true;
    try
    {
        if ( m_rangeMin.IsValid() )
            m_winui->picker.MinDate(wxWinUIToDateTime(m_rangeMin));
        if ( m_rangeMax.IsValid() )
            m_winui->picker.MaxDate(wxWinUIToDateTime(m_rangeMax));
        m_winui->picker.Date(wxWinUIDateRef(m_value));
    }
    catch ( const winrt::hresult_error& )
    {
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

void wxDatePickerCtrl::OnPeerDateChanged()
{
    try
    {
        if ( auto ref = m_winui->picker.Date() )
        {
            m_value = wxWinUIFromDateTime(ref.Value());
        }
        else if ( HasFlag(wxDP_ALLOWNONE) )
        {
            m_value = wxDefaultDateTime;
        }
        else
        {
            // The picker cleared the date but we require one: restore it.
            ApplyToPeer();
            return;
        }
    }
    catch ( const winrt::hresult_error& )
    {
        return;
    }

    wxDateEvent event(this, m_value, wxEVT_DATE_CHANGED);
    HandleWindowEvent(event);
}

#endif // wxUSE_DATEPICKCTRL
