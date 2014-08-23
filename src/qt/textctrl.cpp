/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textctrl.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textctrl.h"
#include "wx/settings.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"
#include "wx/qt/private/utils.h"

class wxQtLineEdit : public wxQtEventSignalHandler< QLineEdit, wxTextCtrl >
{
public:
    wxQtLineEdit( wxWindow *parent, wxTextCtrl *handler );

private:
    void textChanged(const QString &text);
    void returnPressed();
};

wxQtLineEdit::wxQtLineEdit( wxWindow *parent, wxTextCtrl *handler )
    : wxQtEventSignalHandler< QLineEdit, wxTextCtrl >( parent, handler )
{
    connect(this, &QLineEdit::textChanged,
            this, &wxQtLineEdit::textChanged);
    connect(this, &QLineEdit::returnPressed,
            this, &wxQtLineEdit::returnPressed);
}

void wxQtLineEdit::textChanged(const QString &text)
{
    wxTextCtrl *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_TEXT, handler->GetId() );
        event.SetString( wxQtConvertString( text ) );
        EmitEvent( event );
    }
}

void wxQtLineEdit::returnPressed()
{
    wxTextCtrl *handler = GetHandler();
    if ( handler )
    {
        if ( handler->HasFlag(wxTE_PROCESS_ENTER) )
        {
            wxCommandEvent event( wxEVT_TEXT_ENTER, handler->GetId() );
            event.SetString( handler->GetValue() );
            EmitEvent( event );
        }
    }
}


class wxQtTextEdit : public wxQtEventSignalHandler< QTextEdit, wxTextCtrl >
{
public:
    wxQtTextEdit( wxWindow *parent, wxTextCtrl *handler );

private:
    void textChanged();
};

wxQtTextEdit::wxQtTextEdit( wxWindow *parent, wxTextCtrl *handler )
    : wxQtEventSignalHandler< QTextEdit, wxTextCtrl >( parent, handler )
{
    connect(this, &QTextEdit::textChanged,
            this, &wxQtTextEdit::textChanged);
}

void wxQtTextEdit::textChanged()
{
    wxTextCtrl *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_TEXT, handler->GetId() );
        event.SetString( handler->GetValue() );
        EmitEvent( event );
    }
}


wxTextCtrl::wxTextCtrl()
{
}

wxTextCtrl::wxTextCtrl(wxWindow *parent,
           wxWindowID id,
           const wxString &value,
           const wxPoint &pos,
           const wxSize &size,
           long style,
           const wxValidator& validator,
           const wxString &name)
{
    Create( parent, id, value, pos, size, style, validator, name );
}

bool wxTextCtrl::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &value,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxValidator& validator,
            const wxString &name)
{
    bool multiline = (style & wxTE_MULTILINE) != 0;

    if (!multiline)
    {
        m_qtLineEdit = new wxQtLineEdit( parent, this );
        m_qtTextEdit = NULL;
    }
    else
    {
        m_qtTextEdit =  new wxQtTextEdit( parent, this );
        m_qtLineEdit = NULL;
    }
    if ( QtCreateControl( parent, id, pos, size, style, validator, name ) )
    {
        // set the initial text value without sending the event:
        // (done here as needs CreateBase called to set flags for IsMultiLine)
        ChangeValue( value );
        // set the default inner color (white), as it is replaced by PostCreation
        SetBackgroundColour( wxSystemSettingsNative::GetColour( wxSYS_COLOUR_LISTBOX ) );
        return true;
    }
    return false;
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    return wxTextCtrlBase::DoGetBestSize();

}
int wxTextCtrl::GetLineLength(long WXUNUSED(lineNo)) const
{
    return 0;
}

wxString wxTextCtrl::GetLineText(long WXUNUSED(lineNo)) const
{
    return wxString();
}

int wxTextCtrl::GetNumberOfLines() const
{
    return 0;
}

bool wxTextCtrl::IsModified() const
{
    return false;
}

void wxTextCtrl::MarkDirty()
{
}

void wxTextCtrl::DiscardEdits()
{
}

bool wxTextCtrl::SetStyle(long WXUNUSED(start), long WXUNUSED(end), const wxTextAttr& WXUNUSED(style))
{
    return false;
}

bool wxTextCtrl::GetStyle(long WXUNUSED(position), wxTextAttr& WXUNUSED(style))
{
    return false;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& WXUNUSED(style))
{
    return false;
}

long wxTextCtrl::XYToPosition(long WXUNUSED(x), long WXUNUSED(y)) const
{
    return 0;
}

bool wxTextCtrl::PositionToXY(long WXUNUSED(pos), long *WXUNUSED(x), long *WXUNUSED(y)) const
{
    return false;
}

void wxTextCtrl::ShowPosition(long WXUNUSED(pos))
{
}

bool wxTextCtrl::DoLoadFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

bool wxTextCtrl::DoSaveFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    QTextCursor::MoveOperation op;
    QTextCursor cursor;

    // check if pos indicates end of text:
    if ( pos == -1 )
    {
        pos = 0;
        op = QTextCursor::End;
    }
    else
    {
        op = QTextCursor::Start;
    }
    if ( !IsMultiLine() )
    {
        if (op == QTextCursor::End)
            m_qtLineEdit->end(false);
        else
            m_qtLineEdit->setCursorPosition(pos);
    }
    else
    {
        cursor = m_qtTextEdit->textCursor();
        cursor.movePosition(op, QTextCursor::MoveAnchor, pos);
        m_qtTextEdit->setTextCursor(cursor);
        m_qtTextEdit->ensureCursorVisible();
    }
}

long wxTextCtrl::GetInsertionPoint() const
{
    QTextCursor cursor;

    if ( !IsMultiLine() )
    {
        return m_qtLineEdit->cursorPosition();
    }
    else
    {
        cursor = m_qtTextEdit->textCursor();
        return cursor.position();
    }
}

wxString wxTextCtrl::DoGetValue() const
{
    if ( IsMultiLine() )
        return wxQtConvertString( m_qtTextEdit->toPlainText() );
    else
        return wxQtConvertString( m_qtLineEdit->text() );
}

void wxTextCtrl::SetSelection( long from, long to )
{
    // SelectAll uses -1 to -1, adjust for qt:
    if (from == -1 && to == -1)
    {
        from = 0;
        to = GetValue().length();
    }
    if ( IsMultiLine() )
    {
        QTextCursor cursor = m_qtTextEdit->textCursor();
        cursor.setPosition(from);
        cursor.setPosition(to, QTextCursor::KeepAnchor);
        m_qtTextEdit->setTextCursor(cursor);
    }
    else // single line
    {
        m_qtLineEdit->setSelection(from, to);
    }
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
    if ( IsMultiLine() )
    {
        QTextCursor cursor = m_qtTextEdit->textCursor();
        *from = cursor.selectionStart();
        *to = cursor.selectionEnd();
        if(cursor.hasSelection())
                return;
    }
    else // single line
    {
        *from = m_qtLineEdit->selectionStart();
        if ( *from >= 0 )
        {
            *to = *from + m_qtLineEdit->selectedText().length();
            return;
        }
    }
    // No selection, call base for default behaviour:
    wxTextEntry::GetSelection(from, to);
}

void wxTextCtrl::WriteText( const wxString &text )
{
    // Insert the text
    if ( !IsMultiLine() )
    {
        m_qtLineEdit->insert(wxQtConvertString( text ));
    }
    else
    {
        m_qtTextEdit->insertPlainText(wxQtConvertString( text ));
        // the cursor is moved to the end, ensure it is shown
        m_qtTextEdit->ensureCursorVisible();
    }
}

void wxTextCtrl::DoSetValue( const wxString &text, int flags )
{
    // do not fire qt signals for certain methods (i.e. ChangeText)
    if ( !(flags & SetValue_SendEvent) )
    {
        if ( !IsMultiLine() )
            m_qtLineEdit->blockSignals(true);
        else
            m_qtTextEdit->blockSignals(true);
    }

    // Replace the text int the control
    if ( !IsMultiLine() )
    {
        m_qtLineEdit->setText(wxQtConvertString( text ));
    }
    else
    {
        m_qtTextEdit->setPlainText(wxQtConvertString( text ));
        // the cursor is moved to the end, ensure it is shown
        m_qtTextEdit->ensureCursorVisible();
    }

    // re-enable qt signals
    if ( !(flags & SetValue_SendEvent) )
    {
        if ( !IsMultiLine() )
            m_qtLineEdit->blockSignals(false);
        else
            m_qtTextEdit->blockSignals(false);
    }
}

QWidget *wxTextCtrl::GetHandle() const
{
    if (m_qtLineEdit!=NULL)
        return (QWidget *) m_qtLineEdit;
    else
        return (QWidget *) m_qtTextEdit;
}

QScrollArea *wxTextCtrl::QtGetScrollBarsContainer() const
{
    if (m_qtTextEdit!=NULL)
        return (QScrollArea *) m_qtTextEdit;
    else
        return NULL;
}
