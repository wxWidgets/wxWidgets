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
#include "wx/timer.h"
#include "wx/settings.h"

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

BEGIN_EVENT_TABLE(wxTipWindow, wxPopupTransientWindow)
    EVT_LEFT_DOWN(wxTipWindow::OnMouseClick)
    EVT_RIGHT_DOWN(wxTipWindow::OnMouseClick)
    EVT_MIDDLE_DOWN(wxTipWindow::OnMouseClick)
    EVT_PAINT(wxTipWindow::OnPaint)
END_EVENT_TABLE()


#if 0
// Viewer window to put in the frame
class wxTipWindowView: public wxWindow
{
public:
    wxTipWindowView(wxWindow *parent);

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseClick(wxMouseEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    // calculate the client rect we need to display the text
    void Adjust(const wxString& text, wxCoord maxLength);

    long m_creationTime;

  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxTipWindowView, wxWindow)
    EVT_PAINT(wxTipWindowView::OnPaint)
    EVT_LEFT_DOWN(wxTipWindowView::OnMouseClick)
    EVT_RIGHT_DOWN(wxTipWindowView::OnMouseClick)
    EVT_MIDDLE_DOWN(wxTipWindowView::OnMouseClick)
    EVT_KILL_FOCUS(wxTipWindowView::OnKillFocus)
END_EVENT_TABLE()
#endif

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

wxTipWindow::wxTipWindow(wxWindow *parent,
                         const wxString& text,
                         wxCoord maxLength, wxTipWindow** windowPtr)
           : wxPopupTransientWindow(parent)
{
    m_windowPtr = windowPtr;

    // set colours
    SetForegroundColour(*wxBLACK);

#ifdef __WXMSW__
    wxColour bkCol(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_INFOBK));
#else
    wxColour bkCol(wxColour(255, 255, 225));
#endif
    SetBackgroundColour(bkCol);

    // set size and position
    Adjust(text, maxLength);
    int x, y;
    wxGetMousePosition(&x, &y);
    Position(wxPoint(x, y+10), wxSize(0,0));

    //Show(TRUE);
    Popup();
    SetFocus();
}

wxTipWindow::~wxTipWindow()
{
    if (m_windowPtr)
    {
        *m_windowPtr = NULL;
    }
}

void wxTipWindow::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    Close();
}


void wxTipWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // first filll the background
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));

    dc.SetPen( * wxBLACK_PEN );
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
                  2*(TEXT_MARGIN_Y + 1) + m_textLines.GetCount() * m_heightLine);
}


void wxTipWindow::Close()
{
    Show(FALSE);
    Destroy();
}


// ----------------------------------------------------------------------------
// wxTipWindowView
// ----------------------------------------------------------------------------
#if 0
wxTipWindowView::wxTipWindowView(wxWindow *parent)
           : wxWindow(parent, -1,
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_BORDER)
{
    // set colours
    SetForegroundColour(*wxBLACK);
#ifdef __WXMSW__
    wxColour bkCol(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_INFOBK));
#else
    wxColour bkCol(wxColour(255, 255, 225));
#endif
    SetBackgroundColour(bkCol);
    m_creationTime = wxGetLocalTime();
}

void wxTipWindowView::Adjust(const wxString& text, wxCoord maxLength)
{
    wxTipWindow* parent = (wxTipWindow*) GetParent();
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    // calculate the length: we want each line be no longer than maxLength
    // pixels and we only break lines at words boundary
    wxString current;
    wxCoord height, width,
            widthMax = 0;
    parent->m_heightLine = 0;

    bool breakLine = FALSE;
    for ( const wxChar *p = text.c_str(); ; p++ )
    {
        if ( *p == _T('\n') || *p == _T('\0') )
        {
            dc.GetTextExtent(current, &width, &height);
            if ( width > widthMax )
                widthMax = width;

            if ( height > parent->m_heightLine )
                parent->m_heightLine = height;

            parent->m_textLines.Add(current);

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
            parent->m_textLines.Add(current);
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

            if ( height > parent->m_heightLine )
                parent->m_heightLine = height;
        }
    }

    // take into account the border size and the margins
    GetParent()->SetClientSize(2*(TEXT_MARGIN_X + 1) + widthMax,
                  2*(TEXT_MARGIN_Y + 1) + parent->m_textLines.GetCount()*parent->m_heightLine);
}

void wxTipWindowView::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxTipWindow* parent = (wxTipWindow*) GetParent();
    if (!parent)
      return;

    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // first filll the background
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));

    dc.SetPen( * wxBLACK_PEN );
    dc.DrawRectangle(rect);

    // and then draw the text line by line
    dc.SetFont(GetFont());

    wxPoint pt;
    pt.x = TEXT_MARGIN_X;
    pt.y = TEXT_MARGIN_Y;
    size_t count = parent->m_textLines.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        dc.DrawText(parent->m_textLines[n], pt);

        pt.y += parent->m_heightLine;
    }
}

void wxTipWindowView::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    GetParent()->Close();
}

void wxTipWindowView::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    // Workaround the kill focus event happening just after creation in wxGTK
    if (wxGetLocalTime() > m_creationTime + 1)
        GetParent()->Close();
}

#endif
