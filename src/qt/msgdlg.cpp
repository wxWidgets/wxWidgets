/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/msgdlg.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/modalhook.h"
#include "wx/msgdlg.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/qmessagebox.h>


class wxQtMessageDialog : public wxQtEventSignalHandler< QMessageBox, wxMessageDialog >
{

    public:
        wxQtMessageDialog( wxWindow *parent, wxMessageDialog *handler );
};


wxMessageDialog::wxMessageDialog( wxWindow *parent, const wxString& message,
        const wxString& caption, long style, const wxPoint& pos )
    : wxMessageDialogBase( parent, message, caption, style )
{
    wxQtMessageDialog *dlg = new wxQtMessageDialog( parent, this );
    m_qtWindow = dlg;

    // Set properties
    Move( pos );
    dlg->setText( wxQtConvertString( message ) );
    dlg->setWindowTitle( wxQtConvertString( caption ) );

    // Apply the style
    SetWindowStyleFlag( style );

    // Buttons
    if ( style & wxOK )
        dlg->addButton( QMessageBox::Ok );
    if ( style & wxCANCEL )
        dlg->addButton( QMessageBox::Cancel );
    if ( style & wxYES_NO )
    {
        dlg->addButton( QMessageBox::Yes );
        dlg->addButton( QMessageBox::No );
    }

    // Default button
    if ( style & wxNO_DEFAULT )
        dlg->setDefaultButton( QMessageBox::No );
    else if ( style & wxCANCEL_DEFAULT )
        dlg->setDefaultButton( QMessageBox::Cancel );
    else
    {
        // Default to OK or Yes
        if ( style & wxOK )
            dlg->setDefaultButton( QMessageBox::Ok );
        else
            dlg->setDefaultButton( QMessageBox::Yes );
    }

    // Icon
    int numIcons = 0;
    if ( style & wxICON_NONE )
    {
        numIcons++;
        dlg->setIcon( QMessageBox::NoIcon );
    }

    if ( style & wxICON_EXCLAMATION )
    {
        numIcons++;
        dlg->setIcon( QMessageBox::Warning );
    }

    if ( style & wxICON_ERROR )
    {
        numIcons++;
        dlg->setIcon( QMessageBox::Critical );
    }

    if ( style & wxICON_QUESTION )
    {
        numIcons++;
        dlg->setIcon( QMessageBox::Question );
    }

    if ( style & wxICON_INFORMATION )
    {
        numIcons++;
        dlg->setIcon( QMessageBox::Information );
    }

    wxCHECK_RET( numIcons <= 1, "Multiple icon definitions" );
    if ( numIcons == 0 )
    {
        // Use default
        if ( style & wxYES_NO )
            dlg->setIcon( QMessageBox::Question );
        else
            dlg->setIcon( QMessageBox::Information );
    }

    if ( style & wxSTAY_ON_TOP )
        dlg->setWindowModality( Qt::ApplicationModal );

    PostCreation();

    Centre(wxBOTH | wxCENTER_FRAME);
}

wxIMPLEMENT_CLASS(wxMessageDialog,wxDialog);

int wxMessageDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();
    wxCHECK_MSG( m_qtWindow, -1, "Invalid dialog" );

    // Exec may return a wx identifier if a close event is generated
    int ret = static_cast<QDialog*>(m_qtWindow)->exec();
    switch ( ret )
    {
        case QMessageBox::Ok:
            return wxID_OK;
        case QMessageBox::Cancel:
            return wxID_CANCEL;
        case QMessageBox::Yes:
            return wxID_YES;
        case QMessageBox::No:
            return wxID_NO;
        default:
            //wxFAIL_MSG( "unexpected QMessageBox return code" );
            return ret;
    }
}

wxMessageDialog::~wxMessageDialog()
{
}

//=============================================================================

wxQtMessageDialog::wxQtMessageDialog( wxWindow *parent, wxMessageDialog *handler )
    : wxQtEventSignalHandler< QMessageBox, wxMessageDialog >( parent, handler )
{
}
