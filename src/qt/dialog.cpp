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
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

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

wxDialog::wxDialog()
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

    if ( !wxTopLevelWindow::Create( parent, id, title, pos, size, style, name ) )
        return false;

    PostCreation();

    return true;
}

int wxDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();
    wxCHECK_MSG( GetHandle() != nullptr, -1, "Invalid dialog" );

    QDialog *qDialog = GetDialogHandle();
    qDialog->setModal(true);

    Show(true);

    bool ret = qDialog->exec();
    if ( GetReturnCode() == 0 )
        return ret ? wxID_OK : wxID_CANCEL;
    return GetReturnCode();
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

    const bool ret = wxDialogBase::Show(show);

    if (show)
        InitDialog();

    return ret;
}

QDialog *wxDialog::GetDialogHandle() const
{
    return static_cast<QDialog*>(m_qtWindow);
}

