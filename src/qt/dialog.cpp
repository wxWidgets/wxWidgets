/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dialog.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dialog.h"
#include "wx/qt/utils.h"
#include "wx/qt/private/winevent.h"


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
    m_qtWindow = m_qtDialog = new wxQtDialog( parent, this );
    
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

