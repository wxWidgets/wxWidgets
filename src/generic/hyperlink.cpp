/////////////////////////////////////////////////////////////////////////////
// Name:        hyperlink.cpp
// Purpose:     Hyperlink control
// Author:      David Norris <danorris@gmail.com>, Otto Wyss
// Modified by: Ryan Norton, Francesco Montorsi
// Created:     04/02/2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 David Norris
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "hyperlink.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------

#include "wx/hyperlink.h"
#include "wx/utils.h" // for wxLaunchDefaultBrowser
#include "wx/clipbrd.h"


#if wxUSE_HYPERLINKCTRL

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxHyperlinkCtrl, wxControl)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_HYPERLINK)

// reserved for internal use only
#define wxHYPERLINKCTRL_POPUP_COPY_ID           16384

// ----------------------------------------------------------------------------
// wxHyperlinkCtrl
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxHyperlinkCtrl, wxControl)
    EVT_PAINT(wxHyperlinkCtrl::OnPaint)
    EVT_LEFT_DOWN(wxHyperlinkCtrl::OnLeftDown)
    EVT_LEFT_UP(wxHyperlinkCtrl::OnLeftUp)
    EVT_RIGHT_UP(wxHyperlinkCtrl::OnRightUp)
    EVT_ENTER_WINDOW(wxHyperlinkCtrl::OnEnterWindow)
    EVT_LEAVE_WINDOW(wxHyperlinkCtrl::OnLeaveWindow)

    // for the context menu
    EVT_MENU(wxHYPERLINKCTRL_POPUP_COPY_ID, wxHyperlinkCtrl::OnPopUpCopy)
END_EVENT_TABLE()

bool wxHyperlinkCtrl::Create(wxWindow *parent, wxWindowID id,
    const wxString& label, const wxString& url, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
{
    wxASSERT_MSG(!url.IsEmpty() || !label.IsEmpty(),
                 wxT("Both URL and label are empty ?"));

    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    // set to non empty strings both the url and the label
    SetURL(url.IsEmpty() ? label : url);
    SetLabel(label.IsEmpty() ? url : label);

    // by default the cursor to use in this window is wxCURSOR_HAND
    SetCursor(wxCursor(wxCURSOR_HAND));

    m_rollover = false;
    m_clicking = false;
    m_visited = false;

    // colours
    m_normalColour = *wxBLUE;
    m_hoverColour = *wxRED;
    SetForegroundColour(m_normalColour);

    // by default the font of an hyperlink control is underlined
    wxFont f = GetFont();
    f.SetUnderlined(true);
    SetFont(f);

    CacheBestSize(DoGetBestSize());
    SetMinSize(GetBestSize());
    SetSize (DoGetBestSize());

    return true;
}

wxSize wxHyperlinkCtrl::DoGetBestSize() const
{
    int w, h;

    wxClientDC dc((wxWindow *)this);
    dc.SetFont(GetFont());
    dc.GetTextExtent(GetLabel(), &w, &h);

    return wxSize(w, h);
}

void wxHyperlinkCtrl::DoGetSize(int *width, int *height) const
{
    if (width) *width = GetBestSize().GetWidth();
    if (height) *height = GetBestSize().GetHeight();
}

void wxHyperlinkCtrl::SetNormalColour(const wxColour &colour)
{
    m_normalColour = colour;
    if (!m_visited)
    {
        SetForegroundColour(m_normalColour);
        Refresh();
    }
}

void wxHyperlinkCtrl::SetVisitedColour(const wxColour &colour)
{
    m_visitedColour = colour;
    if (m_visited)
    {
        SetForegroundColour(m_visitedColour);
        Refresh();
    }
}

void wxHyperlinkCtrl::DoContextMenu(const wxPoint &pos)
{
    wxMenu *menuPopUp = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    menuPopUp->Append(wxHYPERLINKCTRL_POPUP_COPY_ID, wxT("Copy URL"));
    PopupMenu( menuPopUp, pos );
    delete menuPopUp;
}



// ----------------------------------------------------------------------------
// wxHyperlinkCtrl - event handlers
// ----------------------------------------------------------------------------

void wxHyperlinkCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    dc.SetFont(GetFont());
    dc.SetTextForeground(GetForegroundColour());
    dc.SetTextBackground(GetBackgroundColour());
    dc.DrawText(GetLabel(), 0, 0);
}

void wxHyperlinkCtrl::OnLeftDown(wxMouseEvent& WXUNUSED(event))
{
    m_clicking = true;
}

void wxHyperlinkCtrl::OnLeftUp(wxMouseEvent& WXUNUSED(event))
{
    if (!m_clicking) return;

    SetForegroundColour(m_visitedColour);
    m_visited = true;
    m_clicking = false;

    // send the event
    wxHyperlinkEvent linkEvent(this, GetId(), m_url);
    if (!GetEventHandler()->ProcessEvent(linkEvent))     // was the event skipped ?
        if (!wxLaunchDefaultBrowser(m_url))
            wxLogWarning(wxT("Could not launch the default browser with url '%s' !"), m_url.c_str());
}

void wxHyperlinkCtrl::OnRightUp(wxMouseEvent& event)
{
    if( GetWindowStyle() & wxHL_CONTEXTMENU )
        DoContextMenu(wxPoint(event.m_x, event.m_y));
}

void wxHyperlinkCtrl::OnEnterWindow(wxMouseEvent& WXUNUSED(event))
{
    SetForegroundColour(m_hoverColour);
    m_rollover = true;
    Refresh();
}

void wxHyperlinkCtrl::OnLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
    if (m_rollover)
    {
        SetForegroundColour(!m_visited ? m_normalColour : m_visitedColour);
        m_rollover = false;
        Refresh();
    }
}

void wxHyperlinkCtrl::OnPopUpCopy( wxCommandEvent& WXUNUSED(event) )
{
    if (!wxTheClipboard->Open())
        return;

    wxTextDataObject *data = new wxTextDataObject( m_url );
    wxTheClipboard->SetData( data );
    wxTheClipboard->Close();
}

#endif // wxUSE_HYPERLINKCTRL
