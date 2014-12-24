/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/msgdlg.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

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
    m_qtWindow = new wxQtMessageDialog( parent, this );

    // Set properties
    Move( pos );
    GetHandle()->setText( wxQtConvertString( message ) );
    GetHandle()->setWindowTitle( wxQtConvertString( caption ) );
    
    // Apply the style
    SetWindowStyleFlag( style );

    // Buttons
    if ( style & wxOK )
        GetHandle()->addButton( QMessageBox::Ok );
    if ( style & wxCANCEL )
        GetHandle()->addButton( QMessageBox::Cancel );
    if ( style & wxYES_NO )
    {
        GetHandle()->addButton( QMessageBox::Yes );
        GetHandle()->addButton( QMessageBox::No );
    }

    // Default button
    if ( style & wxNO_DEFAULT )
        GetHandle()->setDefaultButton( QMessageBox::No );
    else if ( style & wxCANCEL_DEFAULT )
        GetHandle()->setDefaultButton( QMessageBox::Cancel );
    else
    {
        // Default to OK or Yes
        if ( style & wxOK )
            GetHandle()->setDefaultButton( QMessageBox::Ok );
        else
            GetHandle()->setDefaultButton( QMessageBox::Yes );
    }

    // Icon
    int numIcons = 0;
    if ( style & wxICON_NONE )
    {
        numIcons++;
        GetHandle()->setIcon( QMessageBox::NoIcon );
    }

    if ( style & wxICON_EXCLAMATION )
    {
        numIcons++;
        GetHandle()->setIcon( QMessageBox::Warning );
    }

    if ( style & wxICON_ERROR || style & wxICON_HAND )
    {
        numIcons++;
        GetHandle()->setIcon( QMessageBox::Critical );
    }

    if ( style & wxICON_QUESTION )
    {
        numIcons++;
        GetHandle()->setIcon( QMessageBox::Question );
    }
        
    if ( style & wxICON_INFORMATION )
    {
        numIcons++;
        GetHandle()->setIcon( QMessageBox::Information );
    }

    wxCHECK_RET( numIcons <= 1, "Multiple icon definitions" );
    if ( numIcons == 0 )
    {
        // Use default
        if ( style & wxYES_NO )
            GetHandle()->setIcon( QMessageBox::Question );
        else
            GetHandle()->setIcon( QMessageBox::Information );
    }

    if ( style & wxSTAY_ON_TOP )
        GetHandle()->setWindowModality( Qt::ApplicationModal );

    PostCreation();
}

int wxMessageDialog::ShowModal()
{
    wxCHECK_MSG( GetHandle(), -1, "Invalid dialog" );
    
    // Exec may return a wx identifier if a close event is generated
    int ret = GetHandle()->exec();
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

QMessageBox *wxMessageDialog::GetHandle() const
{
    return static_cast<QMessageBox*>(m_qtWindow);
}

wxMessageDialog::~wxMessageDialog()
{
}

//=============================================================================

wxQtMessageDialog::wxQtMessageDialog( wxWindow *parent, wxMessageDialog *handler )
    : wxQtEventSignalHandler< QMessageBox, wxMessageDialog >( parent, handler )
{
}
