/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:     wxRadioBox
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "radiobox.h"
#endif

#include "wx/radiobox.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
#endif

// Radio box item
wxRadioBox::wxRadioBox()
{
    m_selectedButton = -1;
    m_noItems = 0;
    m_noRowsOrCols = 0;
    m_majorDim = 0 ;
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos, const wxSize& size,
             int n, const wxString choices[],
             int majorDim, long style,
             const wxValidator& val, const wxString& name)
{
    m_selectedButton = -1;
    m_noItems = n;

    SetName(name);
    SetValidator(val);

    parent->AddChild(this);

    m_windowStyle = (long&)style;

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    m_noRowsOrCols = majorDim;

    if (majorDim==0)
        m_majorDim = n ;
    else
        m_majorDim = majorDim ;


    // TODO create radiobox
    return FALSE;
}


wxRadioBox::~wxRadioBox()
{
    // TODO
}

wxString wxRadioBox::GetLabel(int item) const
{
    // TODO
    return wxString("");
}

void wxRadioBox::SetLabel(int item, const wxString& label)
{
    // TODO
}

int wxRadioBox::FindString(const wxString& s) const
{
    // TODO
    return -1;
}

void wxRadioBox::SetSelection(int n)
{
    if ((n < 0) || (n >= m_noItems))
        return;
    // TODO

    m_selectedButton = n;
}

// Get single selection, for single choice list items
int wxRadioBox::GetSelection() const
{
    return m_selectedButton;
}

// Find string for position
wxString wxRadioBox::GetString(int n) const
{
    // TODO
    return wxString("");
}

void wxRadioBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxRadioBox::GetSize(int *width, int *height) const
{
    // TODO
}

void wxRadioBox::GetPosition(int *x, int *y) const
{
    // TODO
}

wxString wxRadioBox::GetLabel() const
{
    // TODO
    return wxString("");
}

void wxRadioBox::SetLabel(const wxString& label)
{
    // TODO
}

void wxRadioBox::SetFocus()
{
    // TODO
}

bool wxRadioBox::Show(bool show)
{
    // TODO
    return FALSE;
}

// Enable a specific button
void wxRadioBox::Enable(int item, bool enable)
{
    // TODO
}

// Enable all controls
void wxRadioBox::Enable(bool enable)
{
    wxControl::Enable(enable);

    // TODO
}

// Show a specific button
void wxRadioBox::Show(int item, bool show)
{
    // TODO
}

// For single selection items only
wxString wxRadioBox::GetStringSelection () const
{
    int sel = GetSelection ();
    if (sel > -1)
        return this->GetString (sel);
    else
        return wxString("");
}

bool wxRadioBox::SetStringSelection (const wxString& s)
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

void wxRadioBox::Command (wxCommandEvent & event)
{
    SetSelection (event.m_commandInt);
    ProcessCommand (event);
}


