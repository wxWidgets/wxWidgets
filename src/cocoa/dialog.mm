/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/dialog.mm
// Purpose:     wxDialog class
// Author:      David Elliott
// Modified by:
// Created:     2002/12/15
// RCS-ID:      $Id: 
// Copyright:   2002 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/dialog.h"
#include "wx/app.h"
#include "wx/settings.h"
#include "wx/log.h"

#import <AppKit/NSPanel.h>
#import <AppKit/NSApplication.h>

// Lists to keep track of windows, so we can disable/enable them
// for modal dialogs
static wxWindowList wxModalDialogs;

IMPLEMENT_DYNAMIC_CLASS(wxDialog, wxTopLevelWindow)

BEGIN_EVENT_TABLE(wxDialog, wxDialogBase)
  EVT_BUTTON(wxID_OK, wxDialog::OnOK)
  EVT_BUTTON(wxID_APPLY, wxDialog::OnApply)
  EVT_BUTTON(wxID_CANCEL, wxDialog::OnCancel)
  EVT_CLOSE(wxDialog::OnCloseWindow)
END_EVENT_TABLE()

WX_IMPLEMENT_COCOA_OWNER(wxDialog,NSPanel,NSWindow,NSWindow)

void wxDialog::Init()
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE));
}

bool wxDialog::Create(wxWindow *parent, wxWindowID winid,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    wxTopLevelWindows.Append(this);

    if(!CreateBase(parent,winid,pos,size,style,wxDefaultValidator,name))
        return false;

    if (parent)
        parent->AddChild(this);

    NSRect cocoaRect = NSMakeRect(300,300,200,200);

    unsigned int cocoaStyle = 0;
    cocoaStyle |= NSTitledWindowMask;
    cocoaStyle |= NSClosableWindowMask;
    cocoaStyle |= NSMiniaturizableWindowMask;
    cocoaStyle |= NSResizableWindowMask;

    m_cocoaNSWindow = NULL;
    SetNSPanel([[NSPanel alloc] initWithContentRect:cocoaRect styleMask:cocoaStyle backing:NSBackingStoreBuffered defer:NO]);
    // NOTE: SetNSWindow has retained the Cocoa object for this object.
    // Because we do not release on close, the following release matches the
    // above alloc and thus the retain count will be 1.
    [m_cocoaNSWindow release];
    wxLogDebug("wxDialog m_cocoaNSWindow retainCount=%d",[m_cocoaNSWindow retainCount]);

    return true;
}

wxDialog::~wxDialog()
{
    wxLogDebug("Destroying");
    // setReleasedWhenClosed: NO
    [m_cocoaNSWindow close];
    DisassociateNSPanel(m_cocoaNSWindow);
}

void wxDialog::Cocoa_close(void)
{
    m_closed = true;
    /* Actually, this isn't true anymore */
    wxLogDebug("Woah: Dialogs are not generally closed");
}

void wxDialog::SetModal(bool flag)
{
    if ( flag )
    {
        wxModelessWindows.DeleteObject(this);
        m_windowStyle |= wxDIALOG_MODAL ;
    }
    else
    {
        m_windowStyle &= ~wxDIALOG_MODAL ;
        wxModelessWindows.Append(this);
    }
}

bool wxDialog::Show(bool show)
{
    if(show)
        InitDialog();
    if(IsModal())
    {
        if(show)
        {
            wxModalDialogs.Append(this);
            wxLogDebug("runModal");
            [wxTheApp->GetNSApplication() runModalForWindow:m_cocoaNSWindow];
            wxLogDebug("runModal END");
        }
        else
        {
            wxLogDebug("abortModal");
            [wxTheApp->GetNSApplication() abortModal];
            wxModalDialogs.DeleteObject(this);
        }
    }
    return true;
}

// Replacement for Show(TRUE) for modal dialogs - returns return code
int wxDialog::ShowModal()
{
    if(!IsModal())
        SetModal(true);
    Show(true);
    return GetReturnCode();
}

// EndModal will work for any dialog
void wxDialog::EndModal(int retCode)
{
    SetReturnCode(retCode);
    Show(false);
}

bool wxDialog::IsModal() const
{
    return (GetWindowStyleFlag() & wxDIALOG_MODAL);
}

void wxDialog::OnCloseWindow(wxCloseEvent& event)
{
    // We'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close().
    // We wouldn't want to destroy the dialog by default, since the dialog may have been
    // created on the stack.
    // However, this does mean that calling dialog->Close() won't delete the dialog
    // unless the handler for wxID_CANCEL does so. So use Destroy() if you want to be
    // sure to destroy the dialog.
    // The default OnCancel (above) simply ends a modal dialog, and hides a modeless dialog.
    // ALWAYS VETO THIS EVENT!!!!
    event.Veto();

    static wxList closing;
    
    if ( closing.Member(this) )
    {
        wxLogDebug("WARNING: Attempting to recursively call Close for dialog");
        return;
    }
    
    closing.Append(this);
    
    wxLogDebug("Sending Cancel Event");
    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent); // This may close the dialog

    closing.DeleteObject(this);
}

// Standard buttons
void wxDialog::OnOK(wxCommandEvent& event)
{
    if ( Validate() && TransferDataFromWindow() )
    {
        EndModal(wxID_OK);
    }
}

void wxDialog::OnApply(wxCommandEvent& event)
{
	if (Validate())
		TransferDataFromWindow();
	// TODO probably need to disable the Apply button until things change again
}

void wxDialog::OnCancel(wxCommandEvent& event)
{
    wxLogDebug("Cancelled!");
    EndModal(wxID_CANCEL);
}

