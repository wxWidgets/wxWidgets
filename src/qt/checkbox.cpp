/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "checkbox.h"
#endif

#include "wx/checkbox.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)
#endif

// Single check box item
bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: create checkbox

    return FALSE;
}

void wxCheckBox::SetLabel(const wxString& label)
{
    // TODO
}

void wxCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxCheckBox::SetValue(bool val)
{
    // TODO
}

bool wxCheckBox::GetValue() const
{
    // TODO
    return FALSE;
}

void wxCheckBox::Command (wxCommandEvent & event)
{
    SetValue ((event.GetInt() != 0));
    ProcessCommand (event);
}

// Bitmap checkbox
bool wxBitmapCheckBox::Create(wxWindow *parent, wxWindowID id, const wxBitmap *label,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    SetName(name);
    SetValidator(validator);
    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    if ( id == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: Create the bitmap checkbox

    return FALSE;
}

void wxBitmapCheckBox::SetLabel(const wxBitmap *bitmap)
{
    // TODO
}

void wxBitmapCheckBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxBitmapCheckBox::SetValue(bool val)
{
    // TODO
}

bool wxBitmapCheckBox::GetValue() const
{
    // TODOD
    return FALSE;
}


