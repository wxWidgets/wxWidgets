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
#include "wx/qt/utils.h"
#include "wx/qt/dialog_qt.h"

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
    SendDestroyEvent();
}


bool wxDialog::Create( wxWindow *parent, wxWindowID id,
        const wxString &title,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name)
{
    m_qtDialog = new wxQtDialog( parent, this );
    
    return wxTopLevelWindow::Create( parent, id, title, pos, size, style, name );
}

int wxDialog::ShowModal()
{
    wxCHECK_MSG( GetHandle() != NULL, -1, "Invalid dialog" );
    
    return GetHandle()->exec();
}

void wxDialog::EndModal(int retCode)
{
    wxCHECK_RET( GetHandle() != NULL, "Invalid dialog" );
    
    GetHandle()->done( retCode );
}

bool wxDialog::IsModal() const
{
    wxCHECK_MSG( GetHandle() != NULL, false, "Invalid dialog" );

    return GetHandle()->isModal();
}

QDialog *wxDialog::GetHandle() const
{
    return m_qtDialog;
}

