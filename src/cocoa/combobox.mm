/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/combobox.mm
// Purpose:     wxComboBox
// Author:      David Elliott
// Modified by:
// Created:     2003/07/14
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/app.h"
#include "wx/combobox.h"
#include "wx/log.h"

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSComboBox.h>

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxTextCtrl)
BEGIN_EVENT_TABLE(wxComboBox, wxTextCtrl)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxComboBox,NSComboBox,NSTextField,NSView)

bool wxComboBox::Create(wxWindow *parent, wxWindowID winid,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, winid, value, pos, size, chs.GetCount(),
                  chs.GetStrings(), style, validator, name);
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID winid,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    wxAutoNSAutoreleasePool pool;
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;

    m_cocoaNSView = NULL;
    SetNSTextField([[NSComboBox alloc] initWithFrame:MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    [GetNSTextField() setStringValue:wxNSStringWithWxString(value.c_str())];
    [GetNSControl() sizeToFit];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxComboBox::~wxComboBox()
{
}

void wxComboBox::SetSelection(int)
{
}

wxString wxComboBox::GetStringSelection()
{
    return wxEmptyString;
}

void wxComboBox::Clear()
{
}

void wxComboBox::Delete(int)
{
}

int wxComboBox::GetCount() const
{
    return 0;
}

wxString wxComboBox::GetString(int) const
{
    return wxEmptyString;
}

void wxComboBox::SetString(int, const wxString&)
{
}

int wxComboBox::FindString(const wxString&) const
{
    return wxNOT_FOUND;
}

int wxComboBox::GetSelection() const
{
    return 0;
}

int wxComboBox::DoAppend(const wxString&)
{
    return 0;
}

int wxComboBox::DoInsert(const wxString&, int)
{
    return 0;
}

void wxComboBox::DoSetItemClientData(int, void*)
{
}

void* wxComboBox::DoGetItemClientData(int) const
{
    return NULL;
}

void wxComboBox::DoSetItemClientObject(int, wxClientData*)
{
}

wxClientData* wxComboBox::DoGetItemClientObject(int) const
{
    return NULL;
}

#endif //wxUSE_COMBOBOX
