///////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/filepickerg.cpp
// Purpose:     wxGenericFileDirButton class implementation
// Author:      Francesco Montorsi
// Created:     15/04/2006
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


#if wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL

#include "wx/filename.h"
#include "wx/filepicker.h"

#include "wx/generic/filepickerg.h"
#include "wx/weakref.h"

#include <memory>


// ============================================================================
// implementation
// ============================================================================

namespace
{

wxWindow* GetPickerDialogParent(wxWindow* requestedParent, long dialogStyle)
{
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The public picker is a composite and may be destroyed by a modal hook
    // while its native dialog is still unwinding. Storing it as the dialog's
    // wx parent would then make wxWindow teardown delete the dialog out from
    // under ShowModal(). Use the same owner-selection contract as all other
    // modal dialogs and retain the resolved TLW as the stable wx parent.
    wxDialog ownerResolver;
    return ownerResolver.GetParentForModalDialog(requestedParent, dialogStyle);
#else
    wxUnusedVar(dialogStyle);
    return requestedParent;
#endif
}

} // anonymous namespace

wxIMPLEMENT_DYNAMIC_CLASS(wxGenericFileButton, wxButton);
wxIMPLEMENT_DYNAMIC_CLASS(wxGenericDirButton, wxButton);

// ----------------------------------------------------------------------------
// wxGenericFileButton
// ----------------------------------------------------------------------------

bool wxGenericFileDirButton::Create(wxWindow *parent,
                                    wxWindowID id,
                                    const wxString& label,
                                    const wxString& path,
                                    const wxString& message,
                                    const wxString& wildcard,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    const wxValidator& validator,
                                    const wxString& name)
{
    m_pickerStyle = style;

    // If the special wxPB_SMALL flag is used, ignore the provided label and
    // use the shortest possible label and the smallest possible button fitting
    // it.
    long styleButton = 0;
    wxString labelButton;
    if ( m_pickerStyle & wxPB_SMALL )
    {
        labelButton = _("...");
        styleButton = wxBU_EXACTFIT;
    }
    else
    {
        labelButton = label;
    }

    // create this button
    if ( !wxButton::Create(parent, id, labelButton,
                           pos, size, styleButton, validator, name) )
    {
        wxFAIL_MSG( wxT("wxGenericFileButton creation failed") );
        return false;
    }

    // and handle user clicks on it
    Bind(wxEVT_BUTTON, &wxGenericFileDirButton::OnButtonClick, this, GetId());

    // create the dialog associated with this button
    m_path = path;
    m_message = message;
    m_wildcard = wildcard;

    return true;
}

void wxGenericFileDirButton::OnButtonClick(wxCommandEvent& WXUNUSED(ev))
{
    // Showing the hybrid shell dialog runs application modal hooks and can
    // destroy this button together with its public picker parent. Keep no raw
    // wx identity authoritative across that boundary.
    const wxWeakRef<wxGenericFileDirButton> weakThis(this);
    std::unique_ptr<wxDialog> dialog(CreateDialog());
    if ( !dialog )
        return;

    wxWindow * const dialogParent = dialog->GetParent();
    const wxWeakRef<wxWindow> weakDialogParent(dialogParent);

    // A modal hook can also explicitly destroy the dialog before ShowModal()
    // unwinds. Track its lifetime independently so the local unique_ptr never
    // performs a second deletion.
    const wxWeakRef<wxDialog> weakDialog(dialog.get());
    const int rc = dialog->ShowModal();
    if ( !weakDialog )
    {
        (void)dialog.release();
        return;
    }

    // Native file/directory dialogs retain their wx parent without always
    // being present in its child list. If the resolved TLW was destroyed by a
    // modal hook, clear that stale identity before the dialog destructor runs.
    if ( dialogParent && !weakDialogParent &&
         dialog->GetParent() == dialogParent )
    {
        dialog->SetParent(nullptr);
    }

    if ( rc != wxID_OK )
        return;

    wxGenericFileDirButton * const live = weakThis.get();
    if ( !live )
        return;

    // Copy the accepted result transactionally, then retire the dialog before
    // touching application-visible state. SetPath() and the public event can
    // both enter application code which destroys the resolved TLW; no dialog
    // may still be parented to it at that point.
    const wxString acceptedPath = live->GetPathFromDialog(dialog.get());
    dialog.reset();

    wxGenericFileDirButton * const acceptedLive = weakThis.get();
    if ( !acceptedLive )
        return;

    acceptedLive->SetPath(acceptedPath);
    wxGenericFileDirButton * const eventLive = weakThis.get();
    if ( !eventLive )
        return;

    wxFileDirPickerEvent event(
        eventLive->GetEventType(), eventLive, eventLive->GetId(),
        eventLive->m_path);

    // Processing the public notification may synchronously delete the picker.
    // It must remain the final operation in this handler.
    eventLive->GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// wxGenericFileButton
// ----------------------------------------------------------------------------

wxDialog *wxGenericFileButton::CreateDialog()
{
    // Determine the initial directory for the dialog: it comes either from the
    // default path, if it has it, or from the separately specified initial
    // directory that can be set even if the path is e.g. empty.
    wxFileName fn(m_path);
    wxString initialDir = fn.GetPath();
    if ( initialDir.empty() )
        initialDir = m_initialDir;

    return new wxFileDialog
               (
                    GetPickerDialogParent(GetDialogParent(), GetDialogStyle()),
                    m_message,
                    initialDir,
                    fn.GetFullName(),
                    m_wildcard,
                    GetDialogStyle()
               );
}

// ----------------------------------------------------------------------------
// wxGenericDirButton
// ----------------------------------------------------------------------------

wxDialog *wxGenericDirButton::CreateDialog()
{
    wxDirDialog* const dialog = new wxDirDialog
                                    (
                                        GetPickerDialogParent(GetDialogParent(),
                                                              GetDialogStyle()),
                                        m_message,
                                        m_path.empty() ? m_initialDir : m_path,
                                        GetDialogStyle()
                                    );
    return dialog;
}

#endif      // wxUSE_FILEPICKERCTRL || wxUSE_DIRPICKERCTRL
