/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/listbox.mm
// Purpose:     wxListBox
// Author:      David Elliott
// Modified by:
// Created:     2003/03/18
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/listbox.h"
#include "wx/log.h"

#import <AppKit/NSView.h>

IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)
BEGIN_EVENT_TABLE(wxListBox, wxListBoxBase)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxListBox,NSButton,NSControl,NSView)

bool wxListBox::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;

    SetNSView([[NSView alloc] initWithFrame: NSMakeRect(10,10,20,20)]);
    [m_cocoaNSView release];

    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxListBox::~wxListBox()
{
    CocoaRemoveFromParent();
    SetNSView(NULL);
}

// pure virtuals from wxListBoxBase
bool wxListBox::IsSelected(int n) const
{
    return false;
}

void wxListBox::SetSelection(int n, bool select)
{
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    return 0;
}

void wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
}

void wxListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
}

void wxListBox::DoSetFirstItem(int n)
{
}


// pure virtuals from wxItemContainer
    // deleting items
void wxListBox::Clear()
{
}

void wxListBox::Delete(int n)
{
}

    // accessing strings
int wxListBox::GetCount() const
{
    return 0;
}

wxString wxListBox::GetString(int n) const
{
    return wxEmptyString;
}

void wxListBox::SetString(int n, const wxString& s)
{
}

int wxListBox::FindString(const wxString& s) const
{
    return 0;
}

    // selection
void wxListBox::Select(int n)
{
}

int wxListBox::GetSelection() const
{
    return 0;
}

int wxListBox::DoAppend(const wxString& item)
{
    return 0;
}

void wxListBox::DoSetItemClientData(int n, void* clientData)
{
}

void* wxListBox::DoGetItemClientData(int n) const
{
    return NULL;
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
}

wxClientData* wxListBox::DoGetItemClientObject(int n) const
{
    return NULL;
}

