/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textentry.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textentry.h"

wxTextEntry::wxTextEntry()
{
}

void wxTextEntry::WriteText(const wxString& text)
{
}

void wxTextEntry::Remove(long from, long to)
{
}

void wxTextEntry::Copy()
{
}

void wxTextEntry::Cut()
{
}

void wxTextEntry::Paste()
{
}

void wxTextEntry::Undo()
{
}

void wxTextEntry::Redo()
{
}

bool wxTextEntry::CanUndo() const
{
    return false;
}

bool wxTextEntry::CanRedo() const
{
    return false;
}

void wxTextEntry::SetInsertionPoint(long pos)
{
}

long wxTextEntry::GetInsertionPoint() const
{
    return 0;
}

long wxTextEntry::GetLastPosition() const
{
    return 0;
}

void wxTextEntry::SetSelection(long from, long to)
{
}

void wxTextEntry::GetSelection(long *from, long *to) const
{
}

bool wxTextEntry::IsEditable() const
{
    return false;
}

void wxTextEntry::SetEditable(bool editable)
{
}

wxString wxTextEntry::DoGetValue() const
{
    return wxString();
}

wxWindow *wxTextEntry::GetEditableWindow()
{
    return NULL;
}

