/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dialog.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dialog.h"
#include "wx/qt/dialog_qt.h"

wxDialog::wxDialog()
{
}

wxDialog::~wxDialog()
{
    SendDestroyEvent();
    
    delete m_qtDialog;
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

bool wxDialog::Create( wxWindow *parent, wxWindowID id,
        const wxString &title,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name)
{
    if ( GetHandle() == NULL ) {
        // Window has not been created yet
        
        QWidget *qtParent = NULL;
        if ( parent != NULL ) {
            qtParent = parent->QtGetContainer();
            parent->AddChild( this );
        }
        
        m_qtDialog = new wxQtDialog( this, qtParent );
    }
    
    return wxTopLevelWindow::Create( parent, id, title, pos, size, style, name );
}

int wxDialog::ShowModal()
{
    wxCHECK_MSG( GetHandle(), -1, "Invalid dialog" );
    
    return GetHandle()->exec();
}

void wxDialog::EndModal(int retCode)
{
    wxCHECK_RET( GetHandle(), "Invalid dialog" );
    
    GetHandle()->done( retCode );
}

bool wxDialog::IsModal() const
{
    wxCHECK_MSG( GetHandle(), false, "Invalid dialog" );

    return GetHandle()->isModal();
}

QDialog *wxDialog::GetHandle() const
{
    return m_qtDialog;
}

WXWidget wxDialog::QtGetScrollBarsContainer() const
{
    return m_qtDialog;
}
