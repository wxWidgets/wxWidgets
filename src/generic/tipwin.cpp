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

#if wxUSE_TIPWINDOW

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

#if wxUSE_POPUPWIN
BEGIN_EVENT_TABLE(wxTipWindow, wxPopupTransientWindow)
#else
BEGIN_EVENT_TABLE(wxTipWindow, wxFrame)
#endif
    EVT_LEFT_DOWN(wxTipWindow::OnMouseClick)
    EVT_RIGHT_DOWN(wxTipWindow::OnMouseClick)
    EVT_MIDDLE_DOWN(wxTipWindow::OnMouseClick)
#if !wxUSE_POPUPWIN
    EVT_KILL_FOCUS(wxTipWindow::OnKillFocus)
    EVT_ACTIVATE(wxTipWindow::OnActivate)
#endif
END_EVENT_TABLE()


// Viewer window to put in the frame
class wxTipWindowView: public wxWindow
{
public:
    wxTipWindowView(wxWindow *parent);

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnMouseClick(wxMouseEvent& event);
#if !wxUSE_POPUPWIN
    void OnKillFocus(wxFocusEvent& event);
#endif
    // calculate the client rect we need to display the text
    void Adjust(const wxString& text, wxCoord maxLength);

private:
    long m_creationTime;
    wxTipWindow* m_parent;

  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxTipWindowView, wxWindow)
    EVT_PAINT(wxTipWindowView::OnPaint)
    EVT_LEFT_DOWN(wxTipWindowView::OnMouseClick)
    EVT_RIGHT_DOWN(wxTipWindowView::OnMouseClick)
    EVT_MIDDLE_DOWN(wxTipWindowView::OnMouseClick)
#if !wxUSE_POPUPWIN
    EVT_KILL_FOCUS(wxTipWindowView::OnKillFocus)
#endif
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxTipWindow
// ----------------------------------------------------------------------------

wxTipWindow::wxTipWindow(wxWindow *parent,
                         const wxString& text,
                         wxCoord maxLength, wxTipWindow** windowPtr)
#if wxUSE_POPUPWIN
           : wxPopupTransientWindow(parent)
#else
           : wxFrame(parent, -1, _T(""),
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_BORDER | wxFRAME_NO_TASKBAR )
#endif
{
    m_windowPtr = windowPtr;

    // set colours
    SetForegroundColour(*wxBLACK);

#ifdef __WXMSW__
    wxColour bkCol(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
#else
    wxColour bkCol(wxColour(255, 255, 225));
#endif
    SetBackgroundColour(bkCol);

    // set size, position and show it
    wxTipWindowView* tipWindowView = new wxTipWindowView(this);
    tipWindowView->Adjust(text, maxLength);
    tipWindowView->SetFocus();
    int x, y;
    wxGetMousePosition(&x, &y);
#if wxUSE_POPUPWIN
    Position(wxPoint(x, y+10), wxSize(0,0));
    Popup(tipWindowView);
#else
    Move(x, y + 10);
    Show(TRUE);
#endif
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

#if !wxUSE_POPUPWIN
void wxTipWindow::OnActivate(wxActivateEvent& event)
{
    if (!event.GetActive())
        Close();
}

void wxTipWindow::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    // Under Windows at least, we will get this immediately
    // because when the view window is focussed, the
    // tip window goes out of focus.
#ifdef __WXGTK__
    Close();
#endif
}
#endif


void wxTipWindow::Close()
{
#if wxUSE_POPUPWIN
    Show(FALSE);
    Destroy();
#else
    wxFrame::Close();
#endif
}

// ----------------------------------------------------------------------------
// wxTipWindowView
// ----------------------------------------------------------------------------

wxTipWindowView::wxTipWindowView(wxWindow *parent)
           : wxWindow(parent, -1,
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_BORDER)
{
    // set colours
    SetForegroundColour(*wxBLACK);
#ifdef __WXMSW__
    wxColour bkCol(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
#else
    wxColour bkCol(wxColour(255, 255, 225));
#endif
    SetBackgroundColour(bkCol);
    m_creationTime = wxGetLocalTime();
    m_parent = (wxTipWindow*)parent;
}

void wxTipWindowView::Adjust(const wxString& text, wxCoord maxLength)
{
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    // calculate the length: we want each line be no longer than maxLength
    // pixels and we only break lines at words boundary
    wxString current;
    wxCoord height, width,
            widthMax = 0;
    m_parent->m_heightLine = 0;

    bool breakLine = FALSE;
    for ( const wxChar *p = text.c_str(); ; p++ )
    {
        if ( *p == _T('\n') || *p == _T('\0') )
        {
            dc.GetTextExtent(current, &width, &height);
            if ( width > widthMax )
                widthMax = width;

            if ( height > m_parent->m_heightLine )
                m_parent->m_heightLine = height;

            m_parent->m_textLines.Add(current);

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
            m_parent->m_textLines.Add(current);
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

            if ( height > m_parent->m_heightLine )
                m_parent->m_heightLine = height;
        }
    }

    // take into account the border size and the margins
    width  = 2*(TEXT_MARGIN_X + 1) + widthMax;
    height = 2*(TEXT_MARGIN_Y + 1) + m_parent->m_textLines.GetCount()*m_parent->m_heightLine;
    m_parent->SetClientSize(width, height);
    SetSize(0, 0, width, height);
}

void wxTipWindowView::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxRect rect;
    wxSize size = GetClientSize();
    rect.width = size.x;
    rect.height = size.y;

    // first filll the background
    dc.SetBrush(wxBrush(GetBackgroundColour(), wxSOLID));
    dc.SetPen( wxPen(GetForegroundColour(), 1, wxSOLID) );
    dc.DrawRectangle(rect);

    // and then draw the text line by line
    dc.SetTextBackground(GetBackgroundColour());
    dc.SetTextForeground(GetForegroundColour());
    dc.SetFont(GetFont());

    wxPoint pt;
    pt.x = TEXT_MARGIN_X;
    pt.y = TEXT_MARGIN_Y;
    size_t count = m_parent->m_textLines.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        dc.DrawText(m_parent->m_textLines[n], pt);

        pt.y += m_parent->m_heightLine;
    }
}

void wxTipWindowView::OnMouseClick(wxMouseEvent& WXUNUSED(event))
{
    m_parent->Close();
}

#if !wxUSE_POPUPWIN
void wxTipWindowView::OnKillFocus(wxFocusEvent& WXUNUSED(event))
{
    // Workaround the kill focus event happening just after creation in wxGTK
    if (wxGetLocalTime() > m_creationTime + 1)
        m_parent->Close();
}
#endif

#endif
