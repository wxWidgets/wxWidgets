/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/combobox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"
#include "wx/window.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QApplication>
#include <QtWidgets/QAbstractItemView>
#include <QtGui/QFocusEvent>

class wxQtComboBox : public wxQtEventSignalHandler< QComboBox, wxComboBox >
{
public:
    wxQtComboBox( wxWindow *parent, wxComboBox *handler );
    virtual void showPopup() override;
    virtual void hidePopup() override;

    virtual wxString GetValueForProcessEnter() override
    {
        return GetHandler()->GetValue();
    }

private:
    void activated(int index);
    void editTextChanged(const QString &text);
};

wxQtComboBox::wxQtComboBox( wxWindow *parent, wxComboBox *handler )
    : wxQtEventSignalHandler< QComboBox, wxComboBox >( parent, handler )
{
    connect(this, static_cast<void (QComboBox::*)(int index)>(&QComboBox::activated),
            this, &wxQtComboBox::activated);
    connect(this, &QComboBox::editTextChanged,
            this, &wxQtComboBox::editTextChanged);
}

void wxQtComboBox::showPopup()
{
    wxCommandEvent event( wxEVT_COMBOBOX_DROPDOWN, GetHandler()->GetId() );
    EmitEvent( event );
    QComboBox::showPopup();
}

void wxQtComboBox::hidePopup()
{
    wxCommandEvent event( wxEVT_COMBOBOX_CLOSEUP, GetHandler()->GetId() );
    EmitEvent( event );
    QComboBox::hidePopup();
}

void wxQtComboBox::activated(int WXUNUSED(index))
{
    wxComboBox *handler = GetHandler();
    if ( handler )
    {
        handler->SendSelectionChangedEvent(wxEVT_COMBOBOX);
    }
}

void wxQtComboBox::editTextChanged(const QString &text)
{
    wxComboBox *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_TEXT, handler->GetId() );
        event.SetString( wxQtConvertString( text ) );
        EmitEvent( event );
    }
}

void wxComboBox::SetSelection( int n )
{
    wxQtEnsureSignalsBlocked blocker(m_qtComboBox);
    wxChoice::SetSelection( n );
}

wxComboBox::wxComboBox()
{
}


wxComboBox::wxComboBox(wxWindow *parent,
           wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, n, choices, style, validator, name );
}


wxComboBox::wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, choices, style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    const wxString *pChoices = choices.size() ? &choices[ 0 ] : nullptr;
    return Create(parent, id, value, pos, size, choices.size(), pChoices,
                  style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    m_qtComboBox = new wxQtComboBox( parent, this );
    m_qtComboBox->setEditable(!(style & wxCB_READONLY));
    QtInitSort( m_qtComboBox );

    while ( n-- > 0 )
        m_qtComboBox->addItem( wxQtConvertString( *choices++ ));
    m_qtComboBox->setCurrentText( wxQtConvertString( value ));

    return wxChoiceBase::Create( parent, id, pos, size, style, validator, name );
}

bool wxComboBox::IsReadOnly() const
{
    return HasFlag( wxCB_READONLY );
}

bool wxComboBox::IsEditable() const
{
    // Only editable combo boxes have a line edit.
    QLineEdit* const lineEdit = m_qtComboBox->lineEdit();
    return lineEdit && !lineEdit->isReadOnly();
}

void wxComboBox::SetEditable(bool editable)
{
    QLineEdit* const lineEdit = m_qtComboBox->lineEdit();
    if ( lineEdit )
        lineEdit->setReadOnly(!editable);
}

void wxComboBox::SetValue(const wxString& value)
{
    if ( IsReadOnly() )
    {
        SetStringSelection( value );
    }
    else
    {
        wxTextEntry::SetValue(value);
    }
}

void wxComboBox::ChangeValue(const wxString &value)
{
    if ( IsReadOnly() )
    {
        SetStringSelection( value );
    }
    else
    {
        wxTextEntry::ChangeValue(value);
    }
}

void wxComboBox::AppendText(const wxString &value)
{
    wxTextEntry::AppendText(value);
}

void wxComboBox::Replace(long from, long to, const wxString &value)
{
    const wxString original( GetValue() );

    if ( to < 0 )
    {
        to = original.length();
    }

    if ( from == 0 )
    {
        SetValue( value + original.substr(to, original.length()) );
    }

    wxString front = original.substr( 0, from ) + value;

    long iPoint = front.length();
    if ( front.length() <= original.length() )
    {
        SetValue( front + original.substr(to, original.length()) );
    }
    else
    {
        SetValue( front );
    }
    SetInsertionPoint( iPoint );
}

void wxComboBox::WriteText(const wxString &value)
{
    if ( IsEditable() )
    {
        m_qtComboBox->lineEdit()->insert( wxQtConvertString( value ) );
    }
}

wxString wxComboBox::DoGetValue() const
{
    return wxQtConvertString( m_qtComboBox->currentText() );
}

void wxComboBox::Popup()
{
    static_cast<QComboBox *>(GetHandle())->showPopup();
}

void wxComboBox::Dismiss()
{
    static_cast<QComboBox *>(GetHandle())->hidePopup();
}

bool wxComboBox::QtHandleFocusEvent(QWidget *handler, QFocusEvent *event)
{
    if ( !event->gotFocus() )
    {
        // Qt treats the combobox and its drop-down as distinct widgets, but in
        // wxWidgets they're both part of the same control, so we have to avoid
        // generating a lose focus event if the combobox or its drop-down still
        // have focus.
        QWidget* const widget = qApp->focusWidget();
        if ( widget == m_qtComboBox || widget == m_qtComboBox->view() )
            return false;
    }

    return wxChoice::QtHandleFocusEvent(handler, event);
}

void wxComboBox::Clear()
{
    if ( !IsReadOnly() )
        wxTextEntry::Clear();

    wxItemContainer::Clear();
}

void wxComboBox::SetSelection( long from, long to )
{
    if ( from == -1 )
    {
        from = 0;
    }
    if ( to == -1 )
    {
        to = GetValue().length();
    }

    SetInsertionPoint( from );
    // use the inner text entry widget (note that can be null if not editable)
    if ( QLineEdit* const lineEdit = m_qtComboBox->lineEdit() )
    {
        lineEdit->setSelection( from, to - from );
    }
}

void wxComboBox::SetInsertionPoint( long pos )
{
    if ( QLineEdit* const lineEdit = m_qtComboBox->lineEdit() )
    {
        // check if pos indicates end of text:
        if ( pos == -1 )
            lineEdit->end( false );
        else
            lineEdit->setCursorPosition( pos );
    }
}

long wxComboBox::GetInsertionPoint() const
{
    QLineEdit* const lineEdit = m_qtComboBox->lineEdit();

    if ( !lineEdit )
        return -1;

    long selectionStart = lineEdit->selectionStart();

    if ( selectionStart >= 0 )
        return selectionStart;

    return lineEdit->cursorPosition();
}

void wxComboBox::GetSelection(long* from, long* to) const
{
    // use the inner text entry widget (note that can be null if not editable)
    if ( QLineEdit* const lineEdit = m_qtComboBox->lineEdit() )
    {
        *from = lineEdit->selectionStart();
        if ( *from >= 0 )
        {
            *to = *from + lineEdit->selectedText().length();
            return;
        }
    }
    // No selection or text control, call base for default behaviour:
    wxTextEntry::GetSelection(from, to);
}

#endif // wxUSE_COMBOBOX
