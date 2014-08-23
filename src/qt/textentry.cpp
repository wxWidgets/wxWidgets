/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textentry.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textentry.h"

wxTextEntry::wxTextEntry()
{
}

void wxTextEntry::WriteText(const wxString& WXUNUSED(text))
{
}

void wxTextEntry::Remove(long WXUNUSED(from), long WXUNUSED(to))
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

void wxTextEntry::SetInsertionPoint(long WXUNUSED(pos))
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

void wxTextEntry::SetSelection(long WXUNUSED(from), long WXUNUSED(to))
{
}

void wxTextEntry::GetSelection(long *WXUNUSED(from), long *WXUNUSED(to)) const
{
}

bool wxTextEntry::IsEditable() const
{
    return false;
}

void wxTextEntry::SetEditable(bool WXUNUSED(editable))
{
}

wxString wxTextEntry::DoGetValue() const
{
    return wxString();
}

void wxTextEntry::DoSetValue(const wxString &WXUNUSED(value), int WXUNUSED(flags))
{
}

wxWindow *wxTextEntry::GetEditableWindow()
{
    return NULL;
}

