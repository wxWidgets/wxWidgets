/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:     wxButton
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
#endif

#include "wx/os2/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)
#endif

// Button

bool wxButton::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_windowStyle = style;

    parent->AddChild((wxButton *)this);

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: create button

    return FALSE;
}

wxButton::~wxButton()
{
    wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    if ( panel )
    {
        if ( panel->GetDefaultItem() == this )
        {
            // don't leave the panel with invalid default item
            panel->SetDefaultItem(NULL);
        }
    }
}

// ----------------------------------------------------------------------------
// size management including autosizing
// ----------------------------------------------------------------------------

wxSize wxButton::DoGetBestSize()
{
    wxString label = wxGetWindowText(GetHWND());
    int wBtn;
    GetTextExtent(label, &wBtn, NULL);

    int wChar, hChar;
    wxGetCharSize(GetHWND(), &wChar, &hChar, &GetFont());

    // add a margin - the button is wider than just its label
    wBtn += 3*wChar;

    // the button height is proportional to the height of the font used
    int hBtn = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(hChar);

    return wxSize(wBtn, hBtn);
}

/* static */
wxSize wxButton::GetDefaultSize()
{
    static wxSize s_sizeBtn;

    if ( s_sizeBtn.x == 0 )
    {
        wxScreenDC dc;
        dc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

        // the size of a standard button in the dialog units is 50x14,
        // translate this to pixels
        // NB1: the multipliers come from the Windows convention
        // NB2: the extra +1/+2 were needed to get the size be the same as the
        //      size of the buttons in the standard dialog - I don't know how
        //      this happens, but on my system this size is 75x23 in pixels and
        //      23*8 isn't even divisible by 14... Would be nice to understand
        //      why these constants are needed though!
        s_sizeBtn.x = (50 * (dc.GetCharWidth() + 1))/4;
        s_sizeBtn.y = ((14 * dc.GetCharHeight()) + 2)/8;
    }

    return s_sizeBtn;
}

void wxButton::Command (wxCommandEvent & event)
{
    ProcessCommand (event);
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

bool wxButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    event.SetEventObject(this);

    return ProcessCommand(event);
}

void wxButton::SetDefault()
{
    wxWindow *parent = GetParent();
    wxButton *btnOldDefault = NULL;
    wxPanel *panel = wxDynamicCast(parent, wxPanel);
    if (panel)
        panel->SetDefaultItem(this);

    // TODO: make button the default
}

// ----------------------------------------------------------------------------
// event/message handlers
// ----------------------------------------------------------------------------

bool wxButton::OS2Command(WXUINT param, WXWORD id)
{
    bool processed = FALSE;
    // TODO
    /*
    switch ( param )
    {
        case 1:                                             // 1 for accelerator
        case BN_CLICKED:
            processed = SendClickEvent();
            break;
    }
    */
    return processed;
}

WXHBRUSH wxButton::OnCtlColor(WXHDC pDC,
                              WXHWND pWnd,
                              WXUINT nCtlColor,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
  wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

  return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}


