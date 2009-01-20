/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/dialog_osx.cpp
// Purpose:     wxDialog class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: dialog.cpp 54820 2008-07-29 20:04:11Z SC $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/dialog.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/private.h"


// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs

wxList wxModalDialogs;

IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxTopLevelWindow)

void wxDialog::Init()
{
    m_isModalStyle = false;
}

bool wxDialog::Create( wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name )
{
    SetExtraStyle( GetExtraStyle() | wxTOPLEVEL_EX_DIALOG );

    // All dialogs should really have this style...
    style |= wxTAB_TRAVERSAL;

    // ...but not these styles
    style &= ~(wxYES | wxOK | wxNO); // | wxCANCEL

    if ( !wxTopLevelWindow::Create( parent, id, title, pos, size, style, name ) )
        return false;

    return true;
}

void wxDialog::SetModal( bool flag )
{
    if ( flag )
    {
        m_isModalStyle = true;
    }
    else
    {
        m_isModalStyle = false;
    }
}

wxDialog::~wxDialog()
{
    SendDestroyEvent();

    // if the dialog is modal, this will end its event loop
    Show(false);
}

// On mac command-stop does the same thing as Esc, let the base class know
// about it
bool wxDialog::IsEscapeKey(const wxKeyEvent& event)
{
    if ( event.GetKeyCode() == '.' && event.GetModifiers() == wxMOD_CMD )
        return true;

    return wxDialogBase::IsEscapeKey(event);
}

bool wxDialog::IsModal() const
{
    return wxModalDialogs.Find((wxDialog *)this) != NULL; // const_cast
    //    return m_isModalStyle;
}


bool wxDialog::Show(bool show)
{
    if ( !wxDialogBase::Show(show) )
        // nothing to do
        return false;

    if (show && CanDoLayoutAdaptation())
        DoLayoutAdaptation();

    if ( show )
        // usually will result in TransferDataToWindow() being called
        InitDialog();

    if ( m_isModalStyle )
    {
        if ( show )
        {
            DoShowModal();
        }
        else // end of modal dialog
        {
            // this will cause IsModalShowing() return false and our local
            // message loop will terminate
            wxModalDialogs.DeleteObject(this);
        }
    }

    return true;
}

// Replacement for Show(true) for modal dialogs - returns return code
int wxDialog::ShowModal()
{
    if ( !m_isModalStyle )
        SetModal(true);

    if ( IsShown() )
        DoShowModal();
    else
        Show(true);

    return GetReturnCode();
}

// NB: this function (surprizingly) may be called for both modal and modeless
//     dialogs and should work for both of them
void wxDialog::EndModal(int retCode)
{
    SetReturnCode(retCode);
    Show(false);
    SetModal(false);
}

