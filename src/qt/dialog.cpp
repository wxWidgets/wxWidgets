/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dialog.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/modalhook.h"
#include "wx/dialog.h"
#include "wx/weakref.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>

class wxQtDialog : public wxQtEventSignalHandler< QDialog, wxDialog >
{

    public:
        wxQtDialog( wxWindow *parent, wxDialog *handler );
};


wxQtDialog::wxQtDialog( wxWindow *parent, wxDialog *handler )
    : wxQtEventSignalHandler< QDialog, wxDialog >( parent, handler )
{
}

wxDialog::wxDialog( wxWindow *parent, wxWindowID id,
        const wxString &title,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name)
{
    Create( parent, id, title, pos, size, style, name );
}

wxDialog::~wxDialog()
{
}


bool wxDialog::Create( wxWindow *parent, wxWindowID id,
        const wxString &title,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name)
{
    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);

    // all dialogs should have tab traversal enabled
    style |= wxTAB_TRAVERSAL;

    m_qtWindow = new wxQtDialog(parent, this);

    // Qt adds the context help button by default and we need to explicitly
    // remove it to avoid having it if it's not explicitly requested.
    if ( !HasExtraStyle(wxDIALOG_EX_CONTEXTHELP) )
    {
        Qt::WindowFlags qtFlags = m_qtWindow->windowFlags();
        qtFlags &= ~Qt::WindowContextHelpButtonHint;
        m_qtWindow->setWindowFlags(qtFlags);
    }

    return wxTopLevelWindow::Create( parent, id, title, pos, size, style, name );
}

int wxDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();
    wxCHECK_MSG( GetHandle() != nullptr, -1, "Invalid dialog" );

    const wxWeakRef<wxDialog> self(this);
    const QPointer<QDialog> qDialog(GetDialogHandle());

    // Release the mouse if it's currently captured as the window having it
    // will be disabled when this dialog is shown -- but will still keep the
    // capture making it impossible to do anything in the modal dialog itself
    QtReleaseMouseAndNotify();

    if ( !self || !qDialog || self->IsBeingDeleted() )
        return wxID_CANCEL;

    // A reused dialog must not retain the previous modal invocation's result.
    SetReturnCode(0);
    qDialog->setModal(true);

    Show(true);

    if ( !self || !qDialog )
        return wxID_CANCEL;

    // InitDialog() can already have called EndModal(). QDialog::done() hides
    // the native dialog without updating wx's shown flag, and exec() would
    // show it again and wait for a second close which might never come.
    int result = qDialog->result();
    if ( !self->IsBeingDeleted() && qDialog->isVisible() )
        result = qDialog->exec();

    if ( !self || !qDialog )
        return wxID_CANCEL;

    const int returnCode = self->GetReturnCode();
    qDialog->setModal(false);
    if ( self )
        self->wxWindowBase::Show(false);

    return returnCode ? returnCode : (result ? wxID_OK : wxID_CANCEL);
}

void wxDialog::EndModal(int retCode)
{
    wxCHECK_RET( GetDialogHandle() != nullptr, "Invalid dialog" );

    SetReturnCode(retCode);
    GetDialogHandle()->done( QDialog::Accepted );
}

bool wxDialog::IsModal() const
{
    wxCHECK_MSG( GetDialogHandle() != nullptr, false, "Invalid dialog" );

    return GetDialogHandle()->isModal();
}

bool wxDialog::Show(bool show)
{
    if ( show == IsShown() )
        return false;

    if ( !show && IsModal() )
        EndModal(wxID_CANCEL);

    if ( show && CanDoLayoutAdaptation() )
        DoLayoutAdaptation();

    const wxWeakRef<wxDialog> self(this);
    const bool ret = wxDialogBase::Show(show);

    if ( show && self && !self->IsBeingDeleted() )
        self->InitDialog();

    return ret;
}

QDialog *wxDialog::GetDialogHandle() const
{
    return static_cast<QDialog*>(m_qtWindow);
}

