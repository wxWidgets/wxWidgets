/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/calctrl.cpp
// Purpose:     wxWinUI wxCalendarCtrl implementation (WinUI CalendarView)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CALENDARCTRL

#include "wx/calctrl.h"

#include "private.h"

#include <winrt/Windows.Globalization.h>

namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace WF = winrt::Windows::Foundation;

// wxCalendarCtrl RTTI and the wxEVT_CALENDAR_* events are provided by
// src/common/calctrlcmn.cpp.

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

} // namespace

class wxWinUICalendarImpl
{
public:
    wxWinUIControlHost host;
    MUXC::CalendarView cal{ nullptr };
    winrt::event_token selectionToken{};
    winrt::event_token doubleTappedToken{};
};

wxCalendarCtrl::wxCalendarCtrl()
{
}

wxCalendarCtrl::wxCalendarCtrl(wxWindow *parent, wxWindowID id,
                               const wxDateTime& date, const wxPoint& pos,
                               const wxSize& size, long style,
                               const wxString& name)
{
    Create(parent, id, date, pos, size, style, name);
}

wxCalendarCtrl::~wxCalendarCtrl() = default;

bool wxCalendarCtrl::Create(wxWindow *parent, wxWindowID id,
                            const wxDateTime& date, const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxString& name)
{
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_date = date.IsValid() ? date : wxDateTime::Today();

    m_winui.reset(new wxWinUICalendarImpl);
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->cal = MUXC::CalendarView();
        m_winui->cal.SelectionMode(MUXC::CalendarViewSelectionMode::Single);

        if ( style & wxCAL_MONDAY_FIRST )
            m_winui->cal.FirstDayOfWeek(winrt::Windows::Globalization::DayOfWeek::Monday);
        else if ( style & wxCAL_SUNDAY_FIRST )
            m_winui->cal.FirstDayOfWeek(winrt::Windows::Globalization::DayOfWeek::Sunday);
        // Otherwise keep the locale default.

        m_winui->doubleTappedToken = m_winui->cal.DoubleTapped(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   winrt::Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const&)
            {
                if ( !m_winui )
                    return;

                wxCalendarEvent event(this, m_date, wxEVT_CALENDAR_DOUBLECLICKED);
                HandleWindowEvent(event);
            });

        m_winui->selectionToken = m_winui->cal.SelectedDatesChanged(
            [this](MUXC::CalendarView const&,
                   MUXC::CalendarViewSelectedDatesChangedEventArgs const&)
            {
                if ( !m_winui || m_updating )
                    return;
                OnPeerSelectionChanged();
            });

        ApplyToPeer();
        m_winui->host.SetContent(m_winui->cal);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CalendarView creation", e);
        return false;
    }

    SetInitialSize(size);
    return true;
}

bool wxCalendarCtrl::SetDate(const wxDateTime& date)
{
    if ( !date.IsValid() )
        return false;

    m_date = date;
    ApplyToPeer();
    return true;
}

wxDateTime wxCalendarCtrl::GetDate() const
{
    return m_date;
}

bool wxCalendarCtrl::SetDateRange(const wxDateTime& lowerdate,
                                  const wxDateTime& upperdate)
{
    m_lowerDate = lowerdate;
    m_upperDate = upperdate;
    ApplyToPeer();
    return true;
}

bool wxCalendarCtrl::GetDateRange(wxDateTime *lowerdate,
                                  wxDateTime *upperdate) const
{
    if ( lowerdate )
        *lowerdate = m_lowerDate;
    if ( upperdate )
        *upperdate = m_upperDate;
    return m_lowerDate.IsValid() || m_upperDate.IsValid();
}

bool wxCalendarCtrl::EnableMonthChange(bool WXUNUSED(enable))
{
    // The WinUI CalendarView always allows navigating between months.
    return true;
}

void wxCalendarCtrl::Mark(size_t WXUNUSED(day), bool WXUNUSED(mark))
{
    // Day marking would require a CalendarViewDayItemChanging handler; not
    // implemented yet.
}

wxSize wxCalendarCtrl::DoGetBestSize() const
{
    return wxWindow::FromDIP(wxSize(296, 348), const_cast<wxCalendarCtrl*>(this));
}

void wxCalendarCtrl::ApplyToPeer()
{
    if ( !m_winui || !m_winui->cal )
        return;

    m_updating = true;
    try
    {
        if ( m_lowerDate.IsValid() )
            m_winui->cal.MinDate(wxWinUIToDateTime(m_lowerDate));
        if ( m_upperDate.IsValid() )
            m_winui->cal.MaxDate(wxWinUIToDateTime(m_upperDate));

        if ( m_date.IsValid() )
        {
            const WF::DateTime wdt = wxWinUIToDateTime(m_date);
            auto selected = m_winui->cal.SelectedDates();
            selected.Clear();
            selected.Append(wdt);
            m_winui->cal.SetDisplayDate(wdt);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CalendarView apply", e);
    }
    m_updating = false;
    m_winui->host.ForceRender();
}

void wxCalendarCtrl::OnPeerSelectionChanged()
{
    try
    {
        auto selected = m_winui->cal.SelectedDates();
        if ( selected.Size() == 0 )
            return;
        m_date = wxWinUIFromDateTime(selected.GetAt(0));
    }
    catch ( const winrt::hresult_error& )
    {
        return;
    }

    wxCalendarEvent event(this, m_date, wxEVT_CALENDAR_SEL_CHANGED);
    HandleWindowEvent(event);
}

#endif // wxUSE_CALENDARCTRL
