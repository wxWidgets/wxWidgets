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
#include <QtWidgets/qpushbutton.h>


class wxQtMessageDialog : public wxQtEventSignalHandler< QMessageBox, wxMessageDialog >
{
public:
    wxQtMessageDialog( wxWindow *parent, wxMessageDialog *handler, const wxPoint& pos );

    // Initialize the dialog to match the state of the associated wxMessageDialog.
    //
    // Does nothing if it had been already initialized.
    void InitializeIfNeeded(wxMessageDialog& msgdlg);

private:
    // Initial position, may be wxDefaultPosition.
    wxPoint m_pos;

    // Set to true once the dialog has been initialized, which happens before
    // it's shown for the first time.
    bool m_initialized = false;
};


wxMessageDialog::wxMessageDialog( wxWindow *parent, const wxString& message,
        const wxString& caption, long style, const wxPoint& pos )
    : wxMessageDialogBase( parent, message, caption, style )
{
    m_qtWindow = new wxQtMessageDialog( parent, this, pos );

    // The rest of the initialization will be done in InitializeIfNeeded()
    // called from ShowModal().
}

wxIMPLEMENT_CLASS(wxMessageDialog,wxDialog);

int wxMessageDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();
    wxCHECK_MSG( m_qtWindow, -1, "Invalid dialog" );

    auto dlg = static_cast<wxQtMessageDialog*>(m_qtWindow);
    dlg->InitializeIfNeeded(*this);

    // Exec may return a wx identifier if a close event is generated
    int ret = dlg->exec();
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

wxQtMessageDialog::wxQtMessageDialog(wxWindow *parent,
                                     wxMessageDialog *handler,
                                     const wxPoint& pos)
    : wxQtEventSignalHandler< QMessageBox, wxMessageDialog >( parent, handler ),
      m_pos(pos)
{
}

void wxQtMessageDialog::InitializeIfNeeded(wxMessageDialog& msgdlg)
{
    setText( wxQtConvertString( msgdlg.GetMessage() ) );
    setWindowTitle( wxQtConvertString( msgdlg.GetTitle() ) );

    const wxString& extendedMessage = msgdlg.GetExtendedMessage();
    if ( !extendedMessage.empty() )
    {
        setInformativeText( wxQtConvertString( extendedMessage ) );
    }

    // Buttons
    const long style = msgdlg.GetMessageDialogStyle();

    const auto applyCustomLabelIfSet = [](QPushButton* button, const wxString& label)
    {
        if ( !label.empty() )
            button->setText( wxQtConvertString( label ) );
    };

    if ( style & wxOK )
        applyCustomLabelIfSet(addButton( QMessageBox::Ok ), msgdlg.GetOKLabel());
    if ( style & wxCANCEL )
        applyCustomLabelIfSet(addButton( QMessageBox::Cancel ), msgdlg.GetCancelLabel());
    if ( style & wxYES_NO )
    {
        applyCustomLabelIfSet(addButton( QMessageBox::Yes ), msgdlg.GetYesLabel());
        applyCustomLabelIfSet(addButton( QMessageBox::No ), msgdlg.GetNoLabel());
    }

    // Default button
    if ( style & wxNO_DEFAULT )
        setDefaultButton( QMessageBox::No );
    else if ( style & wxCANCEL_DEFAULT )
        setDefaultButton( QMessageBox::Cancel );
    else
    {
        // Default to OK or Yes
        if ( style & wxOK )
            setDefaultButton( QMessageBox::Ok );
        else
            setDefaultButton( QMessageBox::Yes );
    }

    // Icon
    int numIcons = 0;
    if ( style & wxICON_NONE )
    {
        numIcons++;
        setIcon( QMessageBox::NoIcon );
    }

    if ( style & wxICON_EXCLAMATION )
    {
        numIcons++;
        setIcon( QMessageBox::Warning );
    }

    if ( style & wxICON_ERROR )
    {
        numIcons++;
        setIcon( QMessageBox::Critical );
    }

    if ( style & wxICON_QUESTION )
    {
        numIcons++;
        setIcon( QMessageBox::Question );
    }

    if ( style & wxICON_INFORMATION )
    {
        numIcons++;
        setIcon( QMessageBox::Information );
    }

    wxCHECK_RET( numIcons <= 1, "Multiple icon definitions" );
    if ( numIcons == 0 )
    {
        // Use default
        if ( style & wxYES_NO )
            setIcon( QMessageBox::Question );
        else
            setIcon( QMessageBox::Information );
    }

    if ( msgdlg.HasFlag(wxSTAY_ON_TOP) )
        setWindowModality( Qt::ApplicationModal );

    // Now that the dialog has its size, we can position it.
    if ( m_pos != wxDefaultPosition )
        msgdlg.Move( m_pos );
    else
        msgdlg.Centre(wxBOTH | wxCENTER_FRAME);
}
