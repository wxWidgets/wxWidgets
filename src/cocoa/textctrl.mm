/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/textctrl.mm
// Purpose:     wxTextCtrl
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id:
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/textctrl.h"

#import <Foundation/NSString.h>
#import <AppKit/NSTextField.h>

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)
BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
END_EVENT_TABLE()
WX_IMPLEMENT_COCOA_OWNER(wxTextCtrl,NSTextField,NSControl,NSView)

bool wxTextCtrl::Create(wxWindow *parent, wxWindowID winid,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    m_cocoaNSView = NULL;
    SetNSTextField([[NSTextField alloc] initWithFrame:NSMakeRect(0,0,30,30)]);
    [m_cocoaNSView release];
    [GetNSTextField() setStringValue:[NSString stringWithCString:value.c_str()]];
    [GetNSControl() sizeToFit];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxTextCtrl::~wxTextCtrl()
{
    CocoaRemoveFromParent();
    SetNSTextField(NULL);
}

void wxTextCtrl::Cocoa_didChangeText(void)
{
}

void wxTextCtrl::AppendText(wxString const&)
{
}

void wxTextCtrl::SetEditable(bool)
{
}

void wxTextCtrl::DiscardEdits()
{
}

void wxTextCtrl::SetSelection(long, long)
{
}

void wxTextCtrl::ShowPosition(long)
{
}

void wxTextCtrl::SetInsertionPoint(long)
{
}

void wxTextCtrl::SetInsertionPointEnd()
{
}

void wxTextCtrl::Cut()
{
}

void wxTextCtrl::Copy()
{
}

void wxTextCtrl::Redo()
{
}

void wxTextCtrl::Undo()
{
}

void wxTextCtrl::Clear()
{
}

void wxTextCtrl::Paste()
{
}

void wxTextCtrl::Remove(long, long)
{
}

void wxTextCtrl::Replace(long, long, wxString const&)
{
}

void wxTextCtrl::SetValue(wxString const&)
{
}

void wxTextCtrl::WriteText(wxString const&)
{
}

bool wxTextCtrl::IsEditable() const
{
    return true;
}

bool wxTextCtrl::IsModified() const
{
    return false;
}

wxString wxTextCtrl::GetLineText(long) const
{
    return wxEmptyString;
}

void wxTextCtrl::GetSelection(long*, long*) const
{
}

bool wxTextCtrl::PositionToXY(long, long*, long*) const
{
    return false;
}

long wxTextCtrl::XYToPosition(long, long) const
{
    return 0;
}

int wxTextCtrl::GetLineLength(long) const
{
    return 0;
}

long wxTextCtrl::GetLastPosition() const
{
    return 0;
}

int wxTextCtrl::GetNumberOfLines() const
{
    return 0;
}

long wxTextCtrl::GetInsertionPoint() const
{
    return 0;
}

bool wxTextCtrl::CanRedo() const
{
    return false;
}

bool wxTextCtrl::CanUndo() const
{
    return false;
}

wxString wxTextCtrl::GetValue() const
{
    return wxEmptyString;
}

