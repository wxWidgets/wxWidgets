///////////////////////////////////////////////////////////////////////////////
// Name:        generic/calctrl.cpp
// Purpose:     implementation fo the generic wxCalendarCtrl
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29.12.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "calctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif //WX_PRECOMP

#include "wx/generic/calctrl.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxCalendarCtrl, wxControl)
    EVT_PAINT(wxCalendarCtrl::OnPaint)

    EVT_LEFT_DOWN(wxCalendarCtrl::OnClick)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxCalendarCtrl, wxControl)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCalendarCtrl
// ----------------------------------------------------------------------------

void wxCalendarCtrl::Init()
{
    m_comboMonth = NULL;
    m_spinYear = NULL;

    m_widthCol =
    m_heightRow = 0;
}

bool wxCalendarCtrl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    m_date = date.IsValid() ? date : wxDateTime::Today();

    wxSize sizeReal;
    if ( size.x == -1 || size.y == -1 )
    {
        sizeReal = DoGetBestSize();
        if ( size.x != -1 )
            sizeReal.x = size.x;
        if ( size.y != -1 )
            sizeReal.y = size.y;
    }
    else
    {
        sizeReal = size;
    }

    SetSize(sizeReal);

    SetBackgroundColour(*wxWHITE);

    return TRUE;
}

// ----------------------------------------------------------------------------
// date helpers
// ----------------------------------------------------------------------------

wxDateTime wxCalendarCtrl::GetStartDate() const
{
    wxDateTime::Tm tm = m_date.GetTm();

    wxDateTime date = wxDateTime(1, tm.mon, tm.year);
    date.SetToPrevWeekDay(wxDateTime::Sun);

    return date;
}

bool wxCalendarCtrl::IsDateShown(const wxDateTime& date) const
{
    return date.GetMonth() == m_date.GetMonth();
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

wxSize wxCalendarCtrl::DoGetBestSize() const
{
    return wxSize(230, 200);
}

void wxCalendarCtrl::DoSetSize(int x, int y,
                               int width, int height,
                               int sizeFlags)
{
    wxControl::DoSetSize(x, y, width, height, sizeFlags);
}

void wxCalendarCtrl::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow(x, y, width, height);
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxCalendarCtrl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    wxDateTime::WeekDay wd;
    wxString weekdays[7];

    dc.SetFont(*wxSWISS_FONT);

    // determine the column width (we assume that the weekday names are always
    // wider (in any language) than the numbers)
    m_widthCol = 0;
    for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
    {
        weekdays[wd] = wxDateTime::GetWeekDayName(wd, wxDateTime::Name_Abbr);

        wxCoord width;
        dc.GetTextExtent(weekdays[wd], &width, &m_heightRow);
        if ( width > m_widthCol )
        {
            m_widthCol = width;
        }
    }

    // leave some margins
    m_widthCol += 2;
    m_heightRow += 2;

    // first draw the week days
    dc.SetTextForeground(*wxBLUE);
    dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawRectangle(0, 0, 7*m_widthCol - 1, m_heightRow);
    for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
    {
        dc.DrawText(weekdays[wd], wd*m_widthCol + 1, 0);
    }

    // then the calendar itself
    dc.SetTextForeground(*wxBLACK);
    //dc.SetFont(*wxNORMAL_FONT);

    wxCoord y = m_heightRow;

    wxDateTime date = GetStartDate();
    dc.SetBackgroundMode(wxSOLID);
    for ( size_t nWeek = 0; nWeek < 6; nWeek++ )
    {
        for ( wd = wxDateTime::Sun; wd < wxDateTime::Inv_WeekDay; wxNextWDay(wd) )
        {
            if ( IsDateShown(date) )
            {
                wxString day = wxString::Format(_T("%u"), date.GetDay());
                wxCoord width;
                dc.GetTextExtent(day, &width, (wxCoord *)NULL);

                bool isSel = m_date == date;
                if ( isSel )
                {
                    dc.SetTextForeground(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
                    dc.SetTextBackground(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_HIGHLIGHT));
                }

                dc.DrawText(day, wd*m_widthCol + (m_widthCol - width) / 2, y);

                if ( isSel )
                {
                    dc.SetTextForeground(*wxBLACK);
                    dc.SetTextBackground(m_backgroundColour);
                }
            }
            //else: just don't draw it

            date += wxDateSpan::Day();
        }

        y += m_heightRow;
    }
}

// ----------------------------------------------------------------------------
// mouse handling
// ----------------------------------------------------------------------------

void wxCalendarCtrl::OnClick(wxMouseEvent& event)
{
    wxDateTime date;
    if ( !HitTest(event.GetPosition(), &date) )
    {
        event.Skip();
    }
    else
    {
        m_date = date;

        Refresh();
    }
}

bool wxCalendarCtrl::HitTest(const wxPoint& pos, wxDateTime *date)
{
    wxCoord y = pos.y;
    if ( y < m_heightRow )
        return FALSE;

    y -= m_heightRow;
    int week = y / m_heightRow,
        wday = pos.x / m_widthCol;

    if ( week >= 6 || wday >= 7 )
        return FALSE;

    wxCHECK_MSG( date, FALSE, _T("bad pointer in wxCalendarCtrl::HitTest") );

    *date = GetStartDate();
    *date += wxDateSpan::Days(7*week + wday);
    return IsDateShown(*date);
}
