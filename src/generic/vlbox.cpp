///////////////////////////////////////////////////////////////////////////////
// Name:        generic/vlbox.cpp
// Purpose:     implementation of wxVListBox
// Author:      Vadim Zeitlin
// Modified by:
// Created:     31.05.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/dcclient.h"
#endif //WX_PRECOMP

#include "wx/vlbox.h"

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxVListBox, wxVScrolledWindow)
    EVT_PAINT(wxVListBox::OnPaint)

    EVT_KEY_DOWN(wxVListBox::OnKeyDown)
    EVT_LEFT_DOWN(wxVListBox::OnLeftDown)
    EVT_LEFT_DCLICK(wxVListBox::OnLeftDClick)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxVListBox creation
// ----------------------------------------------------------------------------

void wxVListBox::Init()
{
    m_selection = -1;
}

bool wxVListBox::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        size_t countItems,
                        long style,
                        const wxString& name)
{
    if ( !wxVScrolledWindow::Create(parent, id, pos, size, style, name) )
        return false;

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));

    SetItemCount(countItems);

    return true;
}

// ----------------------------------------------------------------------------
// selection handling
// ----------------------------------------------------------------------------

void wxVListBox::DoSetSelection(int selection, bool sendEvent)
{
    if ( selection == m_selection )
    {
        // nothing to do
        return;
    }

    if ( m_selection != -1 )
        RefreshLine(m_selection);

    m_selection = selection;

    if ( m_selection != -1 )
    {
        // if the line is not visible at all, we scroll it into view but we
        // don't need to refresh it -- it will be redrawn anyhow
        if ( !IsVisible(m_selection) )
        {
            ScrollToLine(m_selection);
        }
        else // line is at least partly visible
        {
            // it is, indeed, only partly visible, so scroll it into view to
            // make it entirely visible
            if ( (size_t)m_selection == GetLastVisibleLine() )
            {
                ScrollToLine(m_selection);
            }

            // but in any case refresh it as even if it was only partly visible
            // before we need to redraw it entirely as its background changed
            RefreshLine(m_selection);
        }

        // send a notification event if we were not called directly by user
        if ( sendEvent )
        {
            wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, GetId());
            event.SetEventObject(this);
            event.m_commandInt = selection;

            (void)GetEventHandler()->ProcessEvent(event);
        }
    }
}

// ----------------------------------------------------------------------------
// wxVListBox painting
// ----------------------------------------------------------------------------

void wxVListBox::SetMargins(const wxPoint& pt)
{
    if ( pt != m_ptMargins )
    {
        m_ptMargins = pt;

        Refresh();
    }
}

wxCoord wxVListBox::OnGetLineHeight(size_t line) const
{
    return OnMeasureItem(line) + 2*m_ptMargins.y;
}

void wxVListBox::OnDrawSeparator(wxDC& WXUNUSED(dc),
                                 wxRect& WXUNUSED(rect),
                                 size_t WXUNUSED(n)) const
{
}

void wxVListBox::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    // the update rectangle
    wxRect rectUpdate = GetUpdateClientRect();

    // the bounding rectangle of the current line
    wxRect rectLine;
    rectLine.width = GetClientSize().x;

    // iterate over all visible lines
    const size_t lineMax = GetLastVisibleLine();
    for ( size_t line = GetFirstVisibleLine(); line <= lineMax; line++ )
    {
        const wxCoord hLine = OnGetLineHeight(line);

        rectLine.height = hLine;

        // and draw the ones which intersect the update rect
        if ( rectLine.Intersects(rectUpdate) )
        {
            // don't allow drawing outside of the lines rectangle
            wxDCClipper clip(dc, rectLine);

            if ( IsSelected(line) )
            {
                wxBrush brush(wxSystemSettings::
                                GetColour(wxSYS_COLOUR_HIGHLIGHT),
                                wxSOLID);
                dc.SetBrush(brush);
                dc.SetPen(*wxTRANSPARENT_PEN);
                dc.DrawRectangle(rectLine);
            }

            wxRect rect = rectLine;
            OnDrawSeparator(dc, rect, line);

            rect.Deflate(m_ptMargins.x, m_ptMargins.y);
            OnDrawItem(dc, rect, line);
        }
        else // no intersection
        {
            if ( rectLine.GetTop() > rectUpdate.GetBottom() )
            {
                // we are already below the update rect, no need to continue
                // further
                break;
            }
            //else: the next line may intersect the update rect
        }

        rectLine.y += hLine;
    }
}

// ----------------------------------------------------------------------------
// wxVListBox keyboard handling
// ----------------------------------------------------------------------------

void wxVListBox::OnKeyDown(wxKeyEvent& event)
{
    int selection = 0; // just to silent the stupid compiler warnings
    switch ( event.GetKeyCode() )
    {
        case WXK_HOME:
            selection = 0;
            break;

        case WXK_END:
            selection = GetLineCount() - 1;
            break;

        case WXK_DOWN:
            if ( m_selection == (int)GetLineCount() - 1 )
                return;

            selection = m_selection + 1;
            break;

        case WXK_UP:
            if ( m_selection == -1 )
                selection = GetLineCount() - 1;
            else if ( m_selection != 0 )
                selection = m_selection - 1;
            else // m_selection == 0
                return;
            break;

        case WXK_PAGEDOWN:
        case WXK_NEXT:
            PageDown();
            selection = GetFirstVisibleLine();
            break;

        case WXK_PAGEUP:
        case WXK_PRIOR:
            if ( m_selection == (int)GetFirstVisibleLine() )
            {
                PageUp();
            }

            selection = GetFirstVisibleLine();
            break;

        default:
            event.Skip();
            return;
    }

    DoSetSelection(selection);
}

// ----------------------------------------------------------------------------
// wxVListBox mouse handling
// ----------------------------------------------------------------------------

void wxVListBox::OnLeftDown(wxMouseEvent& event)
{
    int item = HitTest(event.GetPosition());

    DoSetSelection(item);
}

void wxVListBox::OnLeftDClick(wxMouseEvent& event)
{
    int item = HitTest(event.GetPosition());
    if ( item != -1 )
    {
        wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, GetId());
        event.SetEventObject(this);
        event.m_commandInt = item;

        (void)GetEventHandler()->ProcessEvent(event);
    }
}

