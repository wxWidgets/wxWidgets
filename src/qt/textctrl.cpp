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

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>

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
        if ( style & wxTE_PASSWORD )
            m_qtLineEdit->setEchoMode( QLineEdit::Password );
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

int wxTextCtrl::GetLineLength(long lineNo) const
{
    const wxString &value = GetValue();

    if ( IsSingleLine() )
        return value.length();

    size_t pos = 0;
    long cnt = 0;

    while(pos < value.length())
    {
        size_t tpos = value.find('\n', pos);

        if(cnt == lineNo)
        {
            if(tpos == wxString::npos)
                tpos = value.length();
            return tpos - pos;
        }

        if(tpos == wxString::npos) break;

        pos = tpos + 1;
        cnt++;
    }

    return -1;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    const wxString &value = GetValue();

    if ( IsSingleLine() )
        return value;

    size_t pos = 0;
    long cnt = 0;

    while(pos < value.length())
    {
        size_t tpos = value.find('\n', pos);
        if(tpos == wxString::npos) break;

        if(cnt == lineNo)
            return value.Mid(pos,  tpos - pos);

        pos = tpos + 1;
        cnt++;
    }

    return wxString();
}

int wxTextCtrl::GetNumberOfLines() const
{
    if ( IsSingleLine() )
        return 1;

    const wxString &value = GetValue();
    size_t pos = 0;
    size_t cnt = 1;

    while(pos < value.length())
    {
        pos = value.find('\n', pos);
        if(pos == wxString::npos) break;
        pos++;
        cnt++;
    }

    return cnt;
}

bool wxTextCtrl::IsModified() const
{
    if ( IsSingleLine() )
        return m_qtLineEdit->isModified();
    else
        return m_qtTextEdit->isWindowModified();
}

void wxTextCtrl::MarkDirty()
{
    if ( IsSingleLine() )
        return m_qtLineEdit->setModified( true );
    else
        return m_qtTextEdit->setWindowModified( true );
}

void wxTextCtrl::DiscardEdits()
{
    if ( IsSingleLine() )
        return m_qtLineEdit->setModified( false );
    else
        return m_qtTextEdit->setWindowModified( false );
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

long wxTextCtrl::XYToPosition(long x, long y) const
{
    const wxString &value = GetValue();

    size_t pos = 0;
    long cnt = 0;

	if(x < 0 || y < 0)
		return -1;

    if( IsSingleLine() )
    {
        if(y == 0)
        {
            if(static_cast<size_t>(x) <= value.length()) return x;
        }

        return -1;
    }

    while(pos < value.length() && cnt < y)
    {
        size_t tpos = value.find('\n', pos);
        if(tpos == wxString::npos) break;
        pos = tpos + 1;
        cnt++;
    }

    if(cnt == y && pos + x <= value.length())
    {
        size_t tpos = value.find('\n', pos);
        if(tpos == wxString::npos || tpos - pos >= static_cast<size_t>(x))
            return pos + x;
    }

    return -1;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if(x == NULL || y == NULL || pos < 0)
        return false;

    const wxString &value = GetValue();

    if(static_cast<size_t>(pos) > value.length()) return false;

    if( IsSingleLine() )
    {
        *y = 0;
        *x = pos;
        return true;
    }

    int cnt = 0;
    int xval = 0;

    for(long xx = 0; xx < pos; xx++)
    {
        if(value[xx] == '\n')
        {
            xval = 0;
            cnt++;
        }
        else xval++;
    }
    *y = cnt;
    *x = xval;

    return true;
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
    if ( IsSingleLine() )
    {
        if (op == QTextCursor::End)
            m_qtLineEdit->end(false);
        else
            m_qtLineEdit->setCursorPosition(pos);
    }
    else
    {
        QTextCursor cursor = m_qtTextEdit->textCursor();
        cursor.movePosition(op, QTextCursor::MoveAnchor, pos);

        if (op != QTextCursor::End )
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos);
        m_qtTextEdit->setTextCursor(cursor);
        m_qtTextEdit->ensureCursorVisible();
    }
}

long wxTextCtrl::GetInsertionPoint() const
{
    if ( IsSingleLine() )
    {
        long selectionStart = m_qtLineEdit->selectionStart();

        if(selectionStart >= 0)
            return selectionStart;

        return m_qtLineEdit->cursorPosition();
    }
    else
    {
        QTextCursor cursor = m_qtTextEdit->textCursor();
        return cursor.anchor();
    }
}

wxString wxTextCtrl::DoGetValue() const
{
    if ( IsSingleLine() )
        return wxQtConvertString( m_qtLineEdit->text() );
    else
        return wxQtConvertString( m_qtTextEdit->toPlainText() );
}

void wxTextCtrl::SetSelection( long from, long to )
{
    // SelectAll uses -1 to -1, adjust for qt:
    if (from == -1 && to == -1)
    {
        from = 0;
        to = GetValue().length();
    }

    if(to == -1)
        to = GetValue().length();

    if ( IsSingleLine() )
    {
        m_qtLineEdit->setSelection(from, to - from);
    }
    else
    {
        QTextCursor cursor = m_qtTextEdit->textCursor();
        cursor.setPosition(from);

        cursor.setPosition(to, QTextCursor::KeepAnchor);
        m_qtTextEdit->setTextCursor(cursor);
    }
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
    if ( IsMultiLine() )
    {
        QTextCursor cursor = m_qtTextEdit->textCursor();
        *from = cursor.selectionStart();
        *to = cursor.selectionEnd();
        if ( cursor.hasSelection() )
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
    if ( IsSingleLine() )
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
        if ( IsSingleLine() )
            m_qtLineEdit->blockSignals(true);
        else
            m_qtTextEdit->blockSignals(true);
    }

    // Replace the text int the control
    if ( IsSingleLine() )
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
        if ( IsSingleLine() )
            m_qtLineEdit->blockSignals(false);
        else
            m_qtTextEdit->blockSignals(false);
    }
    SetInsertionPoint(0);
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
