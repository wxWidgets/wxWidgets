/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textentry.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textentry.h"
#include "wx/window.h"

#include <QtWidgets/QPushButton>
#include <QPointer>

namespace
{
// From Qt documentation (under default:bool property):
// -----------------------------------------------------
// > A button with this property set to true (i.e., the dialog's default button,)
// > will automatically be pressed when the user presses enter...
// > In a dialog, only one push button at a time can be the default button.
// > The default button behavior is provided only in dialogs.
//
// At Qt level, there is no way to stop the default button from being pressed when
// the user presses enter [even though the text entry has wxTE_PROCESS_ENTER flag and
// the handler doesn't call Skip() which means "don't propagate the event to the parent
// which will eventually close the window"] other than setting the default property of
// the button to false. We'll toggle this property off in focusInEvent(), and if the
// wxEVT_TEXT_ENTER handler decides to Skip() the event, toggle it on again and let
// the default processing to take place.
QPointer<QPushButton> g_defaultButton;

}

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

void wxTextEntry::QtToggleDefaultButton()
{
    if ( g_defaultButton.isNull() )
    {
        wxWindow* const wxwin = GetEditableWindow();
        QWidget* const qwin = wxwin->GetHandle()->window();

        if ( qwin && qwin->windowType() == Qt::Dialog )
        {
            QList<QPushButton*> childButtons =
                qwin->findChildren<QPushButton*>(QString(), Qt::FindDirectChildrenOnly);

            for ( auto button : childButtons )
            {
                if ( button->isDefault() )
                {
                    button->setDefault(false);
                    g_defaultButton = QPointer<QPushButton>(button);
                    return;
                }
            }
        }
    }

    if ( g_defaultButton )
        g_defaultButton->setDefault(!g_defaultButton->isDefault());
}
