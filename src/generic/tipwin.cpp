///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/tipwin.cpp
// Purpose:     implementation of wxTipWindow
// Author:      Vadim Zeitlin
// Modified by:
// Created:     10.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tipwin.h"
#endif

// For compilers that support precompilatixon, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
#endif // WX_PRECOMP

#include "wx/tipwin.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const wxCoord TEXT_MARGIN_X = 3;
static const wxCoord TEXT_MARGIN_Y = 3;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxTipWindow, wxFrame)
    EVT_PAINT(wxTipWindow::OnPaint)

    EVT_LEFT_DOWN(wxTipWindow::OnMouseClick)
    EVT_RIGHT_DOWN(wxTipWindow::OnMouseClick)
    EVT_MIDDLE_DOWN(wxTipWindow::OnMouseClick)
    EVT_KILL_FOCUS(wxTipWindow::OnKillFocus)
    EVT_ACTIVATE(wxTipWindow::OnActivate)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

wxTipWindow::wxTipWindow(wxWindow *parent,
                         const wxString& text,
                         wxCoord maxLength)
           : wxFrame(parent, -1, _T(""),
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_BORDER | wxFRAME_FLOAT_ON_PARENT)
{
    // set colours
    SetForegroundColour(*wxBLACK);
    SetBackgroundColour(wxColour(0xc3ffff));

    // set position and size
    int x, y;
    wxGetMousePosition(&x, &y);
    Move(x, y + 20);

    Adjust(text, maxLength);

    SetFocus();

    Show(TRUE);
}

void wxTipWindow::Adjust(const wxString& text, wxCoord maxLength)
{
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    // calculate the length: we want each line be no longer than maxLength
    // pixels and we only break lines at words boundary
    wxString current;
    wxCoord height, width,
            widthMax = 0;
    m_heightLine = 0;

    bool breakLine = FALSE;
    for ( const wxChar *p = text.c_str(); ; p++ )
    {
        if ( *p == _T('\n') || *p == _T('\0') )
        {
            dc.GetTextExtent(current, &width, &height);
            if ( width > widthMax )
                widthMax = width;

            if ( height > m_heightLine )
                m_heightLine = height;

            m_textLines.Add(current);

            if ( !*p )
            {
                // end of text
                break;
            }

            current.clear();
            breakLine = FALSE;
        }
        else if ( breakLine && (*p == _T(' ') || *p == _T('\t')) )
        {
            // word boundary - break the line here
            m_textLines.Add(current);
            current.clear();
            breakLine = FALSE;
        }
        else // line goes on
        {
            current += *p;
            dc.GetTextExtent(current, &width, &height);
            if ( width > maxLength )
                breakLine = TRUE;

            if ( width > widthMax )
                widthMax = width;

            if ( height > m_heightLine )
                m_heightLine = height;
        }
    }

    // take into account the border size and the margins
    SetClientSize(2*(TEXT_MARGIN_X + 1) + widthMax,
                  2*(TEXT_MARGIN_Y + 1) + m_textLines.GetCount()*m_heightLine);
}

void wxTipWindow::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // first filll the background
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));

    // Under Windows, you apparently get a thin black border whether you like it or not :-(
#ifdef __WXMSW__
    dc.SetPen( * wxTRANSPARENT_PEN );
#else
    dc.SetPen( * wxBLACK_PEN );
#endif
    dc.DrawRectangle(rect);

    // and then draw the text line by line
    dc.SetFont(GetFont());

    wxPoint pt;
    pt.x = TEXT_MARGIN_X;
    pt.y = TEXT_MARGIN_Y;
    size_t count = m_textLines.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        dc.DrawText(m_textLines[n], pt);

        pt.y += m_heightLine;
    }
}

void wxTipWindow::OnMouseClick(wxMouseEvent& event)
{
    Close();
}

void wxTipWindow::OnActivate(wxActivateEvent& event)
{
    if (!event.GetActive())
        Close();
}

void wxTipWindow::OnKillFocus(wxFocusEvent& event)
{
    Close();
}
