/////////////////////////////////////////////////////////////////////////////
// Name:        cocoa/choice.mm
// Purpose:     wxChoice
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id: 
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"
#include "wx/choice.h"
#include "wx/log.h"

IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)
BEGIN_EVENT_TABLE(wxChoice, wxChoiceBase)
END_EVENT_TABLE()
// WX_IMPLEMENT_COCOA_OWNER(wxChoice,NSButton,NSControl,NSView)

bool wxChoice::Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if(!CreateControl(parent,winid,pos,size,style,validator,name))
        return false;

    if(m_parent)
        m_parent->CocoaAddChild(this);
    return true;
}

wxChoice::~wxChoice()
{
    CocoaRemoveFromParent();
}

void wxChoice::Clear()
{
}

void wxChoice::Delete(int)
{
}

int wxChoice::GetCount() const
{
    return 0;
}

wxString wxChoice::GetString(int) const
{
    return wxEmptyString;
}

void wxChoice::SetString(int, const wxString&)
{
}

int wxChoice::FindString(const wxString&) const
{
    return 0;
}

int wxChoice::GetSelection() const
{
    return 0;
}

int wxChoice::DoAppend(const wxString&)
{
    return 0;
}

int wxChoice::DoInsert(const wxString&, int)
{
    return 0;
}

void wxChoice::DoSetItemClientData(int, void*)
{
}

void* wxChoice::DoGetItemClientData(int) const
{
    return NULL;
}

void wxChoice::DoSetItemClientObject(int, wxClientData*)
{
}

wxClientData* wxChoice::DoGetItemClientObject(int) const
{
    return NULL;
}

void wxChoice::SetSelection(int)
{
}

