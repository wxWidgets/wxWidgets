/////////////////////////////////////////////////////////////////////////////
// Name:        univ/combobox.cpp
// Purpose:     wxComboControl and wxComboBox implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.12.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univcombobox.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_COMBOBOX

#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/dcclient.h"
    #include "wx/combobox.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxComboControl, wxControl)
    EVT_BUTTON(-1, wxControl::OnButton)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxComboControl
// ----------------------------------------------------------------------------

void wxComboControl::Init()
{
    m_ctrlPopup = (wxControl *)NULL;
}

bool wxComboControl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& value,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    // first create our own window, i.e. the one which will contain all
    // subcontrols
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return FALSE;

    // create the text control and the button as our siblings (*not* children),
    // don't care about size/position here - they will be set in DoMoveWindow()
    m_btn = new wxBitmapButton(parent, -1, bmp);
    m_text = new wxTextCtrl(parent, -1, value,
                            wxDefaultPosition, wxDefaultSize,
                            style & wxCB_READONLY ? wxTE_READONLY : 0,
                            validator);
}

// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

wxSize wxComboControl::DoGetBestSize() const
{
    wxSize sizeBtn = m_btn->GetBestSize(),
           sizeText = m_text->GetBestSize();

    return wxSize(sizeBtn.x + sizeText.x, wxMax(sizeBtn.y, sizeText.y));
}

void wxComboControl::DoMoveWindow(int x, int y, int width, int height)
{
    wxSize sizeBtn = m_btn->GetSize(),
           sizeText = m_text->GetSize();

    wxControl wText = width - sizeBtn.x;
    m_text->SetSize(x, y + (height - sizeText.y)/2, wText, sizeText.y);
    m_btn->Move(x + wText, y + (height - sizeBtn.y)/2);

    wxControl::DoMoveWindow(x, y, width, height);
}

// ----------------------------------------------------------------------------
// event handling
// ----------------------------------------------------------------------------

void wxComboControl::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() == m_btn )
    {
        ShowPopup();
    }
    else // not ours event
    {
        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// popup window handling
// ----------------------------------------------------------------------------

void wxComboControl::ShowPopup()
{
}

void wxComboControl::HidePopup()
{
}

#endif // wxUSE_COMBOBOX
