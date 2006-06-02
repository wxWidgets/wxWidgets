///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/filepickercmn.cpp
// Purpose:     wxFilePickerCtrl class implementation
// Author:      Francesco Montorsi (readapted code written by Vadim Zeitlin)
// Modified by:
// Created:     15/04/2006
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin, Francesco Montorsi
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

#if wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL

#include "wx/filepicker.h"

// ============================================================================
// implementation
// ============================================================================

DEFINE_EVENT_TYPE(wxEVT_COMMAND_FILEPICKER_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DIRPICKER_CHANGED)
IMPLEMENT_DYNAMIC_CLASS(wxFileDirPickerEvent, wxCommandEvent)

// ----------------------------------------------------------------------------
// wxFileDirPickerCtrlBase
// ----------------------------------------------------------------------------

#define M_PICKER     ((wxFilePickerWidget*)m_picker)

bool wxFileDirPickerCtrlBase::CreateBase( wxWindow *parent, wxWindowID id,
                        const wxString &path, const wxString &message,
                        const wxString &wildcard,
                        const wxPoint &pos, const wxSize &size,
                        long style, const wxValidator& validator,
                        const wxString &name )
{
    wxASSERT_MSG(path.empty() || CheckPath(path), wxT("Invalid initial path !"));

    if (!wxPickerBase::CreateBase(parent, id, path, pos, size,
                                   style, validator, name))
        return false;

    if (!HasFlag(wxFLP_OPEN) && !HasFlag(wxFLP_SAVE))
        m_windowStyle |= wxFLP_OPEN;     // wxFD_OPEN is the default

    // check that the styles are not contradictory
    wxASSERT_MSG( !(HasFlag(wxFLP_SAVE) && HasFlag(wxFLP_OPEN)),
                  _T("can't specify both wxFLP_SAVE and wxFLP_OPEN at once") );

    wxASSERT_MSG( !HasFlag(wxFLP_SAVE) || !HasFlag(wxFLP_FILE_MUST_EXIST),
                   _T("wxFLP_FILE_MUST_EXIST can't be used with wxFLP_SAVE" ) );

    wxASSERT_MSG( !HasFlag(wxFLP_OPEN) || !HasFlag(wxFLP_OVERWRITE_PROMPT),
                  _T("wxFLP_OVERWRITE_PROMPT can't be used with wxFLP_OPEN") );

    // create a wxFilePickerWidget or a wxDirPickerWidget...
    if (!CreatePicker(this, path, message, wildcard))
        return false;
    m_picker->Connect(GetEventType(),
            wxFileDirPickerEventHandler(wxFileDirPickerCtrlBase::OnFileDirChange),
            NULL, this);

    // default's wxPickerBase textctrl limit is too small for this control:
    // make it bigger
    if (m_text) m_text->SetMaxLength(512);

    return true;
}

void wxFileDirPickerCtrlBase::SetPath(const wxString &path)
{
    M_PICKER->SetPath(path);
    UpdateTextCtrlFromPicker();
}

void wxFileDirPickerCtrlBase::UpdatePickerFromTextCtrl()
{
    wxASSERT(m_text);

    if (m_bIgnoreNextTextCtrlUpdate)
    {
        // ignore this update
        m_bIgnoreNextTextCtrlUpdate = false;
        return;
    }

    // remove the eventually present path-separator from the end of the textctrl
    // string otherwise we would generate a wxFileDirPickerEvent when changing
    // from e.g. /home/user to /home/user/ and we want to avoid it !
    wxString newpath(m_text->GetValue());
    if (!newpath.empty() && wxFileName::IsPathSeparator(newpath.Last()))
        newpath.RemoveLast();
    if (!CheckPath(newpath))
        return;       // invalid user input

    if (M_PICKER->GetPath() != newpath)
    {
        M_PICKER->SetPath(newpath);

        // update current working directory, if necessary
        // NOTE: the path separator is required because if newpath is "C:"
        //       then no change would happen
        if (IsCwdToUpdate())
            wxSetWorkingDirectory(newpath + wxFileName::GetPathSeparator());

        // fire an event
        wxFileDirPickerEvent event(GetEventType(), this, GetId(), newpath);
        GetEventHandler()->ProcessEvent(event);
    }
}

void wxFileDirPickerCtrlBase::UpdateTextCtrlFromPicker()
{
    if (!m_text)
        return;     // no textctrl to update

    // NOTE: this SetValue() will generate an unwanted wxEVT_COMMAND_TEXT_UPDATED
    //       which will trigger a unneeded UpdateFromTextCtrl(); thus before using
    //       SetValue() we set the m_bIgnoreNextTextCtrlUpdate flag...
    m_bIgnoreNextTextCtrlUpdate = true;
    m_text->SetValue(M_PICKER->GetPath());
}



// ----------------------------------------------------------------------------
// wxFileDirPickerCtrlBase - event handlers
// ----------------------------------------------------------------------------

void wxFileDirPickerCtrlBase::OnFileDirChange(wxFileDirPickerEvent &ev)
{
    UpdateTextCtrlFromPicker();

    // the wxFilePickerWidget sent us a colour-change notification.
    // forward this event to our parent
    wxFileDirPickerEvent event(GetEventType(), this, GetId(), ev.GetPath());
    GetEventHandler()->ProcessEvent(event);
}

#endif  // wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL

#if wxUSE_FILEPICKERCTRL
IMPLEMENT_DYNAMIC_CLASS(wxFilePickerCtrl, wxPickerBase)
#endif
#if wxUSE_DIRPICKERCTRL
IMPLEMENT_DYNAMIC_CLASS(wxDirPickerCtrl, wxPickerBase)
#endif
