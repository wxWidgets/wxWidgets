///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/filepickerg.cpp
// Purpose:     wxGenericFileDirButton class implementation
// Author:      Francesco Montorsi
// Modified by:
// Created:     15/04/2006
// RCS-ID:      $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
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
    #include "wx/window.h"
#endif //WX_PRECOMP

#include "wx/filepicker.h"
#include "wx/filedlg.h"


// ============================================================================
// implementation
// ============================================================================

#if wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL

IMPLEMENT_DYNAMIC_CLASS(wxGenericFileButton, wxButton)
IMPLEMENT_DYNAMIC_CLASS(wxGenericDirButton, wxButton)

// ----------------------------------------------------------------------------
// wxGenericFileButton
// ----------------------------------------------------------------------------

static wxString s_message, s_wildcard;

bool wxGenericFileDirButton::Create( wxWindow *parent, wxWindowID id,
                        const wxString &label, const wxString &path,
                        const wxString &message, const wxString &wildcard,
                        const wxPoint &pos, const wxSize &size, long style,
                        const wxValidator& validator, const wxString &name)
{
    // create this button
    if (!wxButton::Create(parent, id, label, pos, size, style,
                          validator, name))
    {
        wxFAIL_MSG( wxT("wxGenericFileButton creation failed") );
        return false;
    }

    // and handle user clicks on it
    Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(wxGenericFileDirButton::OnButtonClick),
            NULL, this);

    // create the dialog associated with this button
    m_path = path;
    s_message = message;
    s_wildcard = wildcard;

    m_dialog = NULL;
    return true;
}

void wxGenericFileDirButton::OnButtonClick(wxCommandEvent& WXUNUSED(ev))
{
    CreateDialog(s_message, s_wildcard);

    if (m_dialog->ShowModal() == wxID_OK)
    {
        // save the path
        UpdatePathFromDialog();

        // fire an event
        wxFileDirPickerEvent event(GetEventType(), this, GetId(), m_path);
        GetEventHandler()->ProcessEvent(event);
    }

    wxDELETE(m_dialog);
}

#endif      // wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL
