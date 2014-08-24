/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/msgdlg.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
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
    m_qtMessageBox = new wxQtMessageDialog( parent, this );

    // Set properties
    Move( pos );
    m_qtMessageBox->setText( wxQtConvertString( message ) );
    m_qtMessageBox->setWindowTitle( wxQtConvertString( caption ) );
    
    // Apply the style
    SetWindowStyleFlag( style );

    // Buttons
    if ( style & wxOK )
        m_qtMessageBox->addButton( QMessageBox::Ok );
    if ( style & wxCANCEL )
        m_qtMessageBox->addButton( QMessageBox::Cancel );
    if ( style & wxYES_NO )
    {
        m_qtMessageBox->addButton( QMessageBox::Yes );
        m_qtMessageBox->addButton( QMessageBox::No );
    }

    // Default button
    if ( style & wxNO_DEFAULT )
        m_qtMessageBox->setDefaultButton( QMessageBox::No );
    else if ( style & wxCANCEL_DEFAULT )
        m_qtMessageBox->setDefaultButton( QMessageBox::Cancel );
    else
    {
        // Default to OK or Yes
        if ( style & wxOK )
            m_qtMessageBox->setDefaultButton( QMessageBox::Ok );
        else
            m_qtMessageBox->setDefaultButton( QMessageBox::Yes );
    }

    // Icon
    int numIcons = 0;
    if ( style & wxICON_NONE )
    {
        numIcons++;
        m_qtMessageBox->setIcon( QMessageBox::NoIcon );
    }

    if ( style & wxICON_EXCLAMATION )
    {
        numIcons++;
        m_qtMessageBox->setIcon( QMessageBox::Warning );
    }

    if ( style & wxICON_ERROR || style & wxICON_HAND )
    {
        numIcons++;
        m_qtMessageBox->setIcon( QMessageBox::Critical );
    }

    if ( style & wxICON_QUESTION )
    {
        numIcons++;
        m_qtMessageBox->setIcon( QMessageBox::Question );
    }
        
    if ( style & wxICON_INFORMATION )
    {
        numIcons++;
        m_qtMessageBox->setIcon( QMessageBox::Information );
    }

    wxCHECK_RET( numIcons <= 1, "Multiple icon definitions" );
    if ( numIcons == 0 )
    {
        // Use default
        if ( style & wxYES_NO )
            m_qtMessageBox->setIcon( QMessageBox::Question );
        else
            m_qtMessageBox->setIcon( QMessageBox::Information );
    }

    if ( style & wxSTAY_ON_TOP )
        m_qtMessageBox->setWindowModality( Qt::ApplicationModal );

    PostCreation();
}

int wxMessageDialog::ShowModal()
{
    wxCHECK_MSG( m_qtMessageBox, -1, "Invalid dialog" );
    
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
    return m_qtMessageBox;
}

wxMessageDialog::~wxMessageDialog()
{
}

//=============================================================================

wxQtMessageDialog::wxQtMessageDialog( wxWindow *parent, wxMessageDialog *handler )
    : wxQtEventSignalHandler< QMessageBox, wxMessageDialog >( parent, handler )
{
}
