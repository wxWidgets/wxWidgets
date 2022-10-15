/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textentry.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
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

void wxTextEntry::Remove(long from, long to)
{
    const long insertionPoint = GetInsertionPoint();
    wxString string = GetValue();
    string.erase(from, to - from);
    SetValue(string);
    SetInsertionPoint( std::min(insertionPoint, static_cast<long>(string.length())) );
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
    return GetValue().length();
}

void wxTextEntry::SetSelection(long WXUNUSED(from), long WXUNUSED(to))
{
    wxFAIL_MSG("wxTextEntry::SetSelection should be overridden");
}

void wxTextEntry::GetSelection(long *from, long *to) const
{
    // no unified get selection method in Qt (overridden in textctrl & combobox)
    // only called if no selection
    // If the return values from and to are the same, there is no
    // selection.
    {
        *from =
        *to = GetInsertionPoint();
    }
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
    return nullptr;
}

