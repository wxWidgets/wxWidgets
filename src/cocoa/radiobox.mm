/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/radiobox.mm
// Purpose:     wxRadioBox
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/radiobox.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
BEGIN_EVENT_TABLE(wxRadioBox, wxControl)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxRadioBox,NSTextField,NSControl,NSView)

bool wxRadioBox::Create(wxWindow *parent, wxWindowID winid,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            int majorDim,
            long style, const wxValidator& validator,
            const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;
    m_cocoaNSView = NULL;
    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxRadioBox::~wxRadioBox()
{
    CocoaRemoveFromParent();
}

    // selection
void wxRadioBox::SetSelection(int n)
{
}

int wxRadioBox::GetSelection() const
{
    return 0;
}

    // string access
int wxRadioBox::GetCount() const
{
    return 0;
}

wxString wxRadioBox::GetString(int n) const
{
    return wxEmptyString;
}

void wxRadioBox::SetString(int n, const wxString& label)
{
}

    // change the individual radio button state
void wxRadioBox::Enable(int n, bool enable)
{
}

void wxRadioBox::Show(int n, bool show)
{
}

    // layout parameters
int wxRadioBox::GetColumnCount() const
{
    return 0;
}

int wxRadioBox::GetRowCount() const
{
    return 0;
}

