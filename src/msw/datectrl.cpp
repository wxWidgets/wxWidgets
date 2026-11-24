/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/datectrl.cpp
// Purpose:     wxDatePickerCtrl implementation
// Author:      Vadim Zeitlin
// Created:     2005-01-09
// Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_DATEPICKCTRL

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/msw/private.h"
#endif

#include "wx/datectrl.h"
#include "wx/dateevt.h"
#include "wx/uilocale.h"
#include "wx/msw/private/uilocale.h"
#include "wx/msw/private/custompaint.h"
#include "wx/msw/private/darkmode.h"
#include "wx/renderer.h"
#include <uxtheme.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDatePickerCtrl creation
// ----------------------------------------------------------------------------

bool
wxDatePickerCtrl::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxDateTime& dt,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxValidator& validator,
                         const wxString& name)
{
    // use wxDP_SPIN if wxDP_DEFAULT (0) was given as style
    if ( !(style & wxDP_DROPDOWN) )
        style |= wxDP_SPIN;

    bool ok = MSWCreateDateTimePicker(parent, id, dt,
                                      pos, size, style,
                                      validator, name);
#if wxUSE_INTL
    if (ok)
    {
        MSWSetTimeFormat(wxLOCALE_SHORT_DATE_FMT);
    }
#endif
    return ok;
}

WXDWORD wxDatePickerCtrl::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD styleMSW = wxDatePickerCtrlBase::MSWGetStyle(style, exstyle);

    if ( style & wxDP_SPIN )
        styleMSW |= DTS_UPDOWN;
    //else: drop down by default

    if ( style & wxDP_SHOWCENTURY )
        styleMSW |= DTS_SHORTDATECENTURYFORMAT;
    else
        styleMSW |= DTS_SHORTDATEFORMAT;

    if ( style & wxDP_ALLOWNONE )
        styleMSW |= DTS_SHOWNONE;

    return styleMSW;
}

// TODO: handle WM_WININICHANGE

#if wxUSE_INTL

wxLocaleInfo wxDatePickerCtrl::MSWGetFormat() const
{
    return wxLOCALE_SHORT_DATE_FMT;
}

#endif // wxUSE_INTL

// ----------------------------------------------------------------------------
// wxDatePickerCtrl operations
// ----------------------------------------------------------------------------

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    if ( dt.IsValid() )
    {
        // Don't try setting the date if it's out of range as we can't rely on
        // DateTime_SetSystemtime() always returning FALSE for it.
        wxDateTime dtStart, dtEnd;
        GetRange(&dtStart, &dtEnd);
        if ( (dtStart.IsValid() && dt < dtStart) ||
                (dtEnd.IsValid() && dt > dtEnd) )
        {
            // Fail silently, some existing code relies on SetValue() with an
            // out of range value simply doing nothing -- so don't.
            return;
        }
    }

    wxDateTimePickerCtrl::SetValue(dt);

    // we need to keep only the date part, times don't make sense for this
    // control (in particular, comparisons with other dates would fail)
    if ( m_date.IsValid() )
        m_date.ResetTime();
}

wxDateTime wxDatePickerCtrl::MSWGetControlValue() const
{
    wxDateTime dt;
    SYSTEMTIME st;
    if ( DateTime_GetSystemtime(GetHwnd(), &st) == GDT_VALID )
    {
        dt.SetFromMSWSysDate(st);
    }

    return dt;
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
#if wxDEBUG_LEVEL
    const wxDateTime dt = MSWGetControlValue();

    wxASSERT_MSG( m_date.IsValid() == dt.IsValid() &&
                    (!dt.IsValid() || dt == m_date),
                  wxT("bug in wxDateTimePickerCtrl: m_date not in sync") );
#endif // wxDEBUG_LEVEL

    return wxDateTimePickerCtrl::GetValue();
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    SYSTEMTIME st[2];

    DWORD flags = 0;
    if ( dt1.IsValid() )
    {
        dt1.GetAsMSWSysTime(st + 0);
        flags |= GDTR_MIN;
    }

    if ( dt2.IsValid() )
    {
        dt2.GetAsMSWSysTime(st + 1);
        flags |= GDTR_MAX;
    }

    if ( !DateTime_SetRange(GetHwnd(), flags, st) )
    {
        wxLogDebug(wxT("DateTime_SetRange() failed"));
        return;
    }

    // Setting the range could have changed the current control value if the
    // old one wasn't inside the new range, so update it.
    m_date = MSWGetControlValue();
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    SYSTEMTIME st[2];

    DWORD flags = DateTime_GetRange(GetHwnd(), st);
    if ( dt1 )
    {
        // Workaround for https://bugs.winehq.org/show_bug.cgi?id=40301: WINE
        // returns GDTR_MIN even if there is no lower range bound. Luckily we
        // can check for it easily as the SYSTEMTIME contains only zeroes in
        // this case and 0 is invalid value for wMonth field.
        if ( (flags & GDTR_MIN) && (st[0].wMonth != 0) )
            dt1->SetFromMSWSysDate(st[0]);
        else
            *dt1 = wxDefaultDateTime;
    }

    if ( dt2 )
    {
        if ( flags & GDTR_MAX )
            dt2->SetFromMSWSysDate(st[1]);
        else
            *dt2 = wxDefaultDateTime;
    }

    return flags != 0;
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrl events
// ----------------------------------------------------------------------------

bool wxDatePickerCtrl::MSWOnDateTimeChange(const NMDATETIMECHANGE& dtch)
{
    wxDateTime dt;
    if ( dtch.dwFlags == GDT_VALID )
        dt.SetFromMSWSysDate(dtch.st);

    // filter out duplicate DTN_DATETIMECHANGE events which the native
    // control sends us when using wxDP_DROPDOWN style
    if ( (m_date.IsValid() == dt.IsValid()) &&
            (!m_date.IsValid() || dt == m_date) )
        return false;

    m_date = dt;
    wxDateEvent event(this, dt, wxEVT_DATE_CHANGED);
    return HandleWindowEvent(event);
}

WXLRESULT wxDatePickerCtrl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    // Custom painting for high DPI and dark mode.
    if ( nMsg == WM_PAINT && (GetDPIScaleFactor() > 1.0 || wxMSWDarkMode::IsActive()) )
    {
        // Paint the control into a memory DC, specified by wParam.
        auto defPaint = [this](HWND WXUNUSED(hwnd), WPARAM wParam) {
            MSWDefWindowProc(WM_PAINT, wParam, 0);
        };
        auto postProcess = [this](const wxBitmap& bmpIn) {
            wxBitmap bmpOut(bmpIn);
            wxMemoryDC dc(bmpOut);
            dc.SetLayoutDirection(GetLayoutDirection());
            WinStruct<DATETIMEPICKERINFO> info;
            ::SendMessage(m_hWnd, DTM_GETDATETIMEPICKERINFO, 0, (LPARAM)&info);

            // Draw the checkbox (for both light mode and dark). In light
            // mode, the check box is stretched too big. In dark mode, the
            // check box is missing.
            if ( HasFlag(wxDP_ALLOWNONE) )
            {
                // Get the old check box rectangle.
                wxRect rcOld = wxRectFromRECT(info.rcCheck);
                // Check if the rectangle is valid. Apparently, painting
                // occurs before the layout is finished.
                if ( rcOld.width > 0 )
                {
                    // Erase the old check box. Use the existing background
                    // colour by sampling a pixel to the right of the old
                    // check box rectangle.
                    wxColour bg;
                    dc.GetPixel(rcOld.x + rcOld.width + 1, rcOld.y, &bg);
                    dc.SetBrush(wxBrush(bg));
                    dc.SetPen(*wxTRANSPARENT_PEN);
                    dc.DrawRectangle(rcOld);

                    // Locate the new check box in the center of the old.
                    wxRect rcNew = wxRendererNative::Get().GetCheckBoxSize(this);
                    rcNew.MakeCenteredIn(rcOld);

                    // Draw the new check box. The native control always draws
                    // the check box in the hot state.
                    int flags = wxCONTROL_CURRENT;
                    if ( info.stateCheck & STATE_SYSTEM_CHECKED )
                        flags |= wxCONTROL_CHECKED;
                    if ( !IsEnabled() )
                        flags |= wxCONTROL_DISABLED;
                    wxRendererNative::Get().DrawCheckBox(this, dc, rcNew, flags);
                }
            }

            // Draw the drop-down button, for dark mode.
            if ( wxMSWDarkMode::IsActive() && HasFlag(wxDP_DROPDOWN) )
            {
                // Draw the button using font icons.
                auto facename = (wxGetWinVersion() == wxWinVersion_10) ?
                    "Segoe MDL2 Assets" : "Segoe Fluent Icons";
                wxRect rect = wxRectFromRECT(info.rcButton);
                wxFont font(wxFontInfo(rect.GetHeight() / 3).FaceName(facename));
                dc.SetFont(font);
                auto fg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
                dc.SetTextForeground(fg);
                // Draw 'Calendar' and 'BLACK DOWN-POINTING TRIANGLE'
                dc.DrawLabel(L"\uE787 \u25BC", rect, wxALIGN_CENTER);
            }

            return bmpOut;
        };
        wxMSWImpl::CustomPaint(m_hWnd, defPaint, postProcess);
        return 0;
    }

    return wxDatePickerCtrlBase::MSWWindowProc(nMsg, wParam, lParam);
}

bool wxDatePickerCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam,
    WXLPARAM* result)
{
    NMHDR* hdr = (NMHDR*)lParam;
    switch ( hdr->code )
    {
        case DTN_DROPDOWN:
            if ( wxMSWDarkMode::IsActive() )
            {
                // Set dark mode on the calendar control, which is created
                // upon every drop-down.
                HWND hwndCal = (HWND)::SendMessage(hdr->hwndFrom,
                    DTM_GETMONTHCAL, 0, 0);
                if ( !hwndCal )
                    break;

                // Disable visual themes so that MCM_SETCOLOR takes effect.
                ::SetWindowTheme(hwndCal, L"", L"");
                wxMSWDarkMode::CalendarSetColour(hwndCal);

                // Disabling visual themes changes the calendar size. Resize
                // the parent to fit.
                RECT rcCal = { };
                ::SendMessage(hwndCal, MCM_GETMINREQRECT, 0, (LPARAM)&rcCal);
                // Parent size is calendar size plus an observed fixed margin.
                auto w = rcCal.right - rcCal.left + 10;
                auto h = rcCal.bottom - rcCal.top + 10;
                ::SetWindowPos(::GetParent(hwndCal), nullptr, 0, 0, w, h,
                    SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

                // Subclass the control to handle painting and mouse tracking.
                ::SetWindowSubclass(hwndCal, CalendarDarkProc, 1,
                    (DWORD_PTR)&m_mousePos);
            }
            break;
    }

    return wxDatePickerCtrlBase::MSWOnNotify(idCtrl, lParam, result);
}

LRESULT CALLBACK wxDatePickerCtrl::CalendarDarkProc(HWND hwnd, UINT uMsg,
    WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch ( uMsg )
    {
        case WM_PAINT:
            wxMSWDarkMode::CalendarPaint(hwnd, ::DefSubclassProc, (int*)dwRefData, 0);
            return 0;

        case WM_MOUSELEAVE:
        case WM_MOUSEMOVE:
            wxMSWDarkMode::CalendarMouseEvent(hwnd, uMsg, lParam, (int*)dwRefData);
            break;

        case WM_NCDESTROY:
            ::RemoveWindowSubclass(hwnd, CalendarDarkProc, uIdSubclass);
            break;
    }

    return ::DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

#endif // wxUSE_DATEPICKCTRL
