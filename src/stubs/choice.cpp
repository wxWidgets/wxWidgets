/////////////////////////////////////////////////////////////////////////////
// Name:        choice.cpp
// Purpose:     wxChoice
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "choice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/choice.h"

IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)

bool wxChoice::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
		   int n, const wxString choices[],
		   long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_noStrings = n;
    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    if ( id == -1 )
    	m_windowId = (int)NewControlId();
    else
	m_windowId = id;

    // TODO: create choice control
    return FALSE;
}

void wxChoice::Append(const wxString& item)
{
    // TODO
    m_noStrings ++;
}

void wxChoice::Delete(int n)
{
    // TODO
    m_noStrings --;
}

void wxChoice::Clear()
{
    // TODO
    m_noStrings = 0;
}

int wxChoice::GetSelection() const
{
    // TODO
    return 0;
}

void wxChoice::SetSelection(int n)
{
    // TODO
}

int wxChoice::FindString(const wxString& s) const
{
    // TODO
    return 0;
}

wxString wxChoice::GetString(int n) const
{
    // TODO
    return wxString("");
}

void wxChoice::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

wxString wxChoice::GetStringSelection () const
{
    int sel = GetSelection ();
    if (sel > -1)
        return wxString(this->GetString (sel));
    else
        return wxString("");
}

bool wxChoice::SetStringSelection (const wxString& s)
{
    int sel = FindString (s);
    if (sel > -1)
        {
            SetSelection (sel);
            return TRUE;
        }
    else
        return FALSE;
}

void wxChoice::Command(wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    ProcessCommand (event);
}

