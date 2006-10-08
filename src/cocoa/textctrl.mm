/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/textctrl.mm
// Purpose:     wxTextCtrl
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/textctrl.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/cocoa/string.h"

#include "wx/cocoa/autorelease.h"

#import <Foundation/NSString.h>
#import <AppKit/NSTextField.h>
#import <AppKit/NSCell.h>

#include <math.h>

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxTextCtrlBase)
BEGIN_EVENT_TABLE(wxTextCtrl, wxTextCtrlBase)
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
    wxAutoNSAutoreleasePool pool;
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    m_cocoaNSView = NULL;
    SetNSTextField([[NSTextField alloc] initWithFrame:MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    [GetNSTextField() setStringValue:wxNSStringWithWxString(value)];

    [GetNSControl() sizeToFit];
    NSRect currentFrame = [m_cocoaNSView frame];
    if(currentFrame.size.width < 70)
    {
        currentFrame.size.width = 70;
        [m_cocoaNSView setFrame:currentFrame];
    }
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);
    return true;
}

wxTextCtrl::~wxTextCtrl()
{
    DisassociateNSTextField(GetNSTextField());
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

void wxTextCtrl::MarkDirty()
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

void wxTextCtrl::DoSetValue(wxString const& value, int flags)
{
    wxAutoNSAutoreleasePool pool;
    [GetNSTextField() setStringValue: wxNSStringWithWxString(value)];

    if ( flags & SetValue_SendEvent )
        SendTextUpdatedEvent();
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

wxTextPos wxTextCtrl::GetLastPosition() const
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
    wxAutoNSAutoreleasePool pool;
    return wxStringWithNSString([GetNSTextField() stringValue]);
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    wxAutoNSAutoreleasePool pool;
    wxASSERT(GetNSControl());
    NSCell *cell = [GetNSControl() cell];
    wxASSERT(cell);
    NSSize cellSize = [cell cellSize];
    wxSize size(100,(int)ceil(cellSize.height));

    wxLogTrace(wxTRACE_COCOA_Window_Size,wxT("wxTextCtrl=%p::DoGetBestSize()==(%d,%d)"),this,size.x,size.y);
    return size;
}
