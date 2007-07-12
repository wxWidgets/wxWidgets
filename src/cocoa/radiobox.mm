/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/radiobox.mm
// Purpose:     wxRadioBox
// Author:      David Elliott
// Modified by:
// Created:     2003/02/15
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_RADIOBOX

#include "wx/radiobox.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/arrstr.h"
#endif //WX_PRECOMP

#include "wx/cocoa/objc/NSView.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
BEGIN_EVENT_TABLE(wxRadioBox, wxControl)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxRadioBox,NSTextField,NSControl,NSView)

bool wxRadioBox::Create(wxWindow *parent, wxWindowID winid,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            int majorDim,
            long style, const wxValidator& validator,
            const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, winid, title, pos, size, chs.GetCount(),
                  chs.GetStrings(), majorDim, style, validator, name);
}

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
    SetNSView([[WXNSView alloc] initWithFrame: MakeDefaultNSRect(size)]);
    [m_cocoaNSView release];
    if(m_parent)
        m_parent->CocoaAddChild(this);
    SetInitialFrameRect(pos,size);

    return true;
}

wxRadioBox::~wxRadioBox()
{
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
unsigned int wxRadioBox::GetCount() const
{
    return 0;
}

wxString wxRadioBox::GetString(unsigned int n) const
{
    return wxEmptyString;
}

void wxRadioBox::SetString(unsigned int n, const wxString& label)
{
}

    // change the individual radio button state
bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    // TODO
    return false;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{
    // TODO
    return false;
}

wxSize wxRadioBox::DoGetBestSize() const
{
    return wxSize(50,50);
}

#endif
