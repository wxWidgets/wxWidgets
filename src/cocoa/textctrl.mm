/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/textctrl.mm
// Purpose:     wxTextCtrl
// Author:      David Elliott
// Modified by: Mark Oxenham
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
#include "wx/cocoa/ObjcRef.h"

#import <Foundation/NSString.h>
#import <AppKit/NSTextField.h>
#import <AppKit/NSSecureTextField.h>
#import <AppKit/NSCell.h>

#include <math.h>

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxTextCtrlBase)
BEGIN_EVENT_TABLE(wxTextCtrl, wxTextCtrlBase)
END_EVENT_TABLE()

// Replaced by special 2.8 code:
//WX_IMPLEMENT_COCOA_OWNER(wxTextCtrl,NSTextField,NSControl,NSView)
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
    SetNSTextField([(style & wxTE_PASSWORD)?[NSSecureTextField alloc]:[NSTextField alloc] initWithFrame:MakeDefaultNSRect(size)]);
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

    [(NSTextField*)m_cocoaNSView setTarget: sm_cocoaTarget];
    [(NSTextField*)m_cocoaNSView setAction:@selector(wxNSControlAction:)];

    // set the text alignment option
    NSTextAlignment alignStyle;
    if (style & wxTE_RIGHT)
        alignStyle = NSRightTextAlignment;
    else if (style & wxTE_CENTRE)
        alignStyle = NSCenterTextAlignment;
    else // default to wxTE_LEFT because it is 0 and can't be tested
        alignStyle = NSLeftTextAlignment;
    [GetNSControl() setAlignment:alignStyle];

    // if Read-only then set as such, this flag is overwritable by wxTextCtrl::SetEditable(TRUE)
    if (style & wxTE_READONLY)
    {
        SetEditable(FALSE);
    }

    return true;
}

wxTextCtrl::~wxTextCtrl()
{
    DisassociateNSTextField(GetNSTextField());
}

void wxTextCtrl::Cocoa_didChangeText(void)
{
}

void wxTextCtrl::CocoaTarget_action(void)
{
    // NSTextField only sends the action message on enter key press and thus
    // we send the appropriate event type.
    wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, GetId());

    // See wxTextCtrlBase::SendTextUpdatedEvent for why we don't set the string.
    //event.SetString(GetValue());

    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

void wxTextCtrl::AppendText(wxString const&)
{
}

void wxTextCtrl::SetEditable(bool editable)
{
    // first ensure that the current value is stored (in case the user had not finished editing
    // before SetEditable(FALSE) was called)
    DoSetValue(GetValue(),1);

    [GetNSTextField() setEditable: editable];

    // forces the focus on the textctrl to be lost - while focus is still maintained
    // after SetEditable(FALSE) the user may still edit the control
    // (might not the best way to do this..)
    [GetNSTextField() abortEditing];
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
    return [GetNSTextField() isEditable];
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
    // working - returns the size of the wxString
    return (long)(GetValue().Len());
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

///////////////////////////////////////////////////////////////////////////
// Added within the 2.8 release cycle, just after 2.8.7:

@interface WXNSTextFieldDelegate : NSObject
{
}

- (void)controlTextDidChange:(NSNotification*)notification;
@end // @interface WXNSTextFieldDelegate : NSObject

WX_DECLARE_HASH_MAP(WX_NSTextField, wxTextCtrl*, wxPointerHash, wxPointerEqual, wxCocoaNSTextField_wxTextCtrl_Hash);

static wxCocoaNSTextField_wxTextCtrl_Hash sg_textField_textCtrl_hash;
static wxObjcAutoRefFromAlloc<WXNSTextFieldDelegate*> sg_cocoaDelegate([[WXNSTextFieldDelegate alloc] init]);

static inline wxTextCtrl* GetTextCtrlFromCocoaTextField(WX_NSTextField cocoaObjcClass)
{
    wxCocoaNSTextField_wxTextCtrl_Hash::iterator iter = sg_textField_textCtrl_hash.find(cocoaObjcClass);
    if(iter != sg_textField_textCtrl_hash.end())
        return iter->second;
    return NULL;
}

void wxTextCtrl::SetNSTextField(WX_NSTextField cocoaObjcClass)
{
    DisassociateNSTextField((WX_NSTextField)m_cocoaNSView);
    if(m_cocoaNSView)
    {
        sg_textField_textCtrl_hash.erase((WX_NSTextField)m_cocoaNSView);
        [(WX_NSTextField)m_cocoaNSView setDelegate:nil];
    }
    SetNSControl(cocoaObjcClass);
    AssociateNSTextField((WX_NSTextField)m_cocoaNSView);
    if(m_cocoaNSView)
    {
        sg_textField_textCtrl_hash.insert(wxCocoaNSTextField_wxTextCtrl_Hash::value_type((WX_NSTextField)m_cocoaNSView, this));
        [(WX_NSTextField)m_cocoaNSView setDelegate:sg_cocoaDelegate];
    }
}

inline void wxTextCtrl::CocoaNotification_controlTextDidChange(WX_NSNotification notification)
{
    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, GetId());

    // See wxTextCtrlBase::SendTextUpdatedEvent for why we don't set the string.
    //event.SetString(GetValue());

    event.SetEventObject(this);
    GetEventHandler()->ProcessEvent(event);
}

@implementation WXNSTextFieldDelegate : NSObject

- (void)controlTextDidChange:(NSNotification*)notification
{
    wxTextCtrl *tc = GetTextCtrlFromCocoaTextField([notification object]);
    if(tc != NULL)
        tc->CocoaNotification_controlTextDidChange(notification);
}


@end // @implementation WXNSTextFieldDelegate : NSObject

