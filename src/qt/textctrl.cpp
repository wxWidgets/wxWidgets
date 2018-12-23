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



/*
 * Abstract base class for wxQtSingleLineEdit and wxQtMultiLineEdit.
 * This splits the polymorphic behaviour into two separate classes, avoiding
 * unnecessary branches.
 */
class wxQtEdit
{
public:
    virtual ~wxQtEdit() {}

    virtual bool IsModified() const = 0;
    virtual int GetNumberOfLines() const = 0;
    virtual wxString DoGetValue() const = 0;
    virtual long GetInsertionPoint() const = 0;
    virtual QWidget *GetHandle() const = 0;
    virtual int GetLineLength(long lineNo) const = 0;
    virtual wxString GetLineText(long lineNo) const = 0;
    virtual bool GetSelection(long *from, long *to) const = 0;
    virtual long XYToPosition(long x, long y) const = 0;
    virtual bool PositionToXY(long pos, long *x, long *y) const = 0;
    virtual QScrollArea *ScrollBarsContainer() const = 0;
    virtual void WriteText( const wxString &text ) = 0;
    virtual void MarkDirty() = 0;
    virtual void DiscardEdits() = 0;
    virtual void blockSignals(bool block) = 0;
    virtual void SetValue( const wxString &value ) = 0;
    virtual void SetSelection( long from, long to ) = 0;
    virtual void SetInsertionPoint(long pos) = 0;
};

namespace
{
struct wxQtLineInfo 
{
  size_t startPos, endPos;
  wxQtLineInfo(size_t start, size_t end) :
    startPos(start),
    endPos(end)
  {
  }
};

class wxQtLineEdit : public wxQtEventSignalHandler< QLineEdit, wxTextCtrl >
{
public:
    wxQtLineEdit( wxWindow *parent, wxTextCtrl *handler );

private:
    void textChanged();
    void returnPressed();
};

class wxQtTextEdit : public wxQtEventSignalHandler< QTextEdit, wxTextCtrl >
{
public:
    wxQtTextEdit( wxWindow *parent, wxTextCtrl *handler );

private:
    void textChanged();
};

class wxQtMultiLineEdit : public wxQtEdit
{
public:
    wxQtMultiLineEdit(QTextEdit *edit) : edit(edit)
    {
    }

    virtual bool IsModified() const wxOVERRIDE
    {
        return edit->isWindowModified();
    }

    virtual wxString DoGetValue() const wxOVERRIDE
    {
        return wxQtConvertString( edit->toPlainText() );
    }

    virtual long GetInsertionPoint() const wxOVERRIDE
    {
        QTextCursor cursor = edit->textCursor();
        return cursor.anchor();
    }

    virtual QWidget *GetHandle() const wxOVERRIDE
    {
        return edit;
    }

    virtual int GetNumberOfLines() const wxOVERRIDE
    {
        const wxString &value = DoGetValue();
        return std::count(value.begin(), value.end(), '\n') + 1;
    }

    wxQtLineInfo getLineStart(long lineNo, const wxString &value) const
    {
        size_t pos = 0;
        long cnt = 0;

        while(cnt < lineNo)
        {
            size_t tpos = value.find('\n', pos);
            if ( tpos == wxString::npos )
              return wxQtLineInfo(tpos, tpos);

            pos = tpos + 1;
            cnt++;
        }

        size_t end = value.find('\n', pos);
        if ( end == wxString::npos )
            end = value.length();

        return wxQtLineInfo(pos, end);
    }

    virtual int GetLineLength(long lineNo) const wxOVERRIDE
    {
        wxQtLineInfo start = getLineStart(lineNo, DoGetValue());
        if ( start.startPos == wxString::npos )
            return -1;

        return start.endPos - start.startPos;
    }

    virtual wxString GetLineText(long lineNo) const wxOVERRIDE
    {
        const wxString &value = DoGetValue();

        wxQtLineInfo start = getLineStart(lineNo, value);
        if ( start.startPos == wxString::npos )
            return wxString();

        return value.Mid(start.startPos, start.endPos - start.startPos);
    }

    virtual long XYToPosition(long x, long y) const wxOVERRIDE
    {
        if ( x < 0 || y < 0 )
            return -1;

        wxQtLineInfo start = getLineStart(y, DoGetValue());
        if ( start.startPos == wxString::npos )
            return -1;

        if ( start.endPos - start.startPos < static_cast<size_t>(x) )
            return -1;

        return start.startPos + x;
    }

    virtual bool PositionToXY(long pos, long *x, long *y) const wxOVERRIDE
    {
        const wxString &value = DoGetValue();

        if ( static_cast<size_t>(pos) > value.length() )
          return false;

        int cnt = 0;
        int xval = 0;

        for(long xx = 0; xx < pos; xx++)
        {
            if ( value[xx] == '\n' )
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
    virtual void WriteText( const wxString &text ) wxOVERRIDE
    {
        edit->insertPlainText(wxQtConvertString( text ));
        // the cursor is moved to the end, ensure it is shown
        edit->ensureCursorVisible();
    }

    virtual void MarkDirty() wxOVERRIDE
    {
        return edit->setWindowModified( true );
    }

    virtual void DiscardEdits() wxOVERRIDE
    {
        return edit->setWindowModified( false );
    }

    virtual void blockSignals(bool block) wxOVERRIDE
    {
        edit->blockSignals(block);
    }

    virtual void SetValue( const wxString &value ) wxOVERRIDE
    {
        edit->setPlainText(wxQtConvertString( value ));
        // the cursor is moved to the end, ensure it is shown
        edit->ensureCursorVisible();
    }

    virtual void SetSelection( long from, long to ) wxOVERRIDE
    {
        QTextCursor cursor = edit->textCursor();
        cursor.setPosition(from);

        cursor.setPosition(to, QTextCursor::KeepAnchor);
        edit->setTextCursor(cursor);
    }

    virtual bool GetSelection( long *from, long *to ) const wxOVERRIDE
    {
        QTextCursor cursor = edit->textCursor();
        *from = cursor.selectionStart();
        *to = cursor.selectionEnd();
        return cursor.hasSelection();
    }

    virtual void SetInsertionPoint(long pos) wxOVERRIDE
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

        QTextCursor cursor = edit->textCursor();
        cursor.movePosition(op, QTextCursor::MoveAnchor, pos);

        if (op != QTextCursor::End )
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos);
        edit->setTextCursor(cursor);
        edit->ensureCursorVisible();
    }

    QScrollArea *ScrollBarsContainer() const wxOVERRIDE
    {
        return (QScrollArea *) edit;
    }

private:
    QTextEdit *edit;
};

class wxQtSingleLineEdit : public wxQtEdit
{
public:
    wxQtSingleLineEdit(QLineEdit *edit) :
        edit(edit)
    {
    }

    virtual bool IsModified() const wxOVERRIDE
    {
        return edit->isModified();
    }

    virtual int GetNumberOfLines() const wxOVERRIDE
    {
        return 1;
    }

    virtual wxString DoGetValue() const wxOVERRIDE
    {
        return wxQtConvertString( edit->text() );
    }

    virtual long GetInsertionPoint() const wxOVERRIDE
    {
        long selectionStart = edit->selectionStart();

        if ( selectionStart >= 0 )
            return selectionStart;

        return edit->cursorPosition();
    }

    virtual QWidget *GetHandle() const wxOVERRIDE
    {
        return edit;
    }

    virtual int GetLineLength(long WXUNUSED(lineNo)) const wxOVERRIDE
    {
        return DoGetValue().length();
    }

    virtual wxString GetLineText(long lineNo) const wxOVERRIDE
    {
        return lineNo == 0 ? DoGetValue() : wxString();
    }

    virtual void WriteText( const wxString &text ) wxOVERRIDE
    {
        edit->insert(wxQtConvertString( text ));
    }

    virtual void MarkDirty() wxOVERRIDE
    {
        return edit->setModified( true );
    }

    virtual void DiscardEdits() wxOVERRIDE
    {
        return edit->setModified( false );
    }

    virtual void blockSignals(bool block) wxOVERRIDE
    {
        edit->blockSignals(block);
    }

    virtual void SetValue( const wxString &value ) wxOVERRIDE
    {
        edit->setText(wxQtConvertString( value ));
    }

    virtual void SetSelection( long from, long to ) wxOVERRIDE
    {
        edit->setSelection(from, to - from);
    }

    virtual bool GetSelection( long *from, long *to ) const wxOVERRIDE
    {
        *from = edit->selectionStart();
        if ( *from < 0 ) return false;

        *to = *from + edit->selectedText().length();
        return true;
    }

    virtual void SetInsertionPoint(long pos) wxOVERRIDE
    {
        // check if pos indicates end of text:
        if (pos == -1)
            edit->end(false);
        else
            edit->setCursorPosition(pos);
    }

    virtual long XYToPosition(long x, long y) const wxOVERRIDE
    {
        if ( y == 0 && x >= 0 )
        {
            if ( static_cast<size_t>(x) <= DoGetValue().length() )
              return x;
        }

        return -1;
    }

    virtual bool PositionToXY(long pos, long *x, long *y) const wxOVERRIDE
    {
        const wxString &value = DoGetValue();

        if ( static_cast<size_t>(pos) > value.length() )
          return false;

        *y = 0;
        *x = pos;
        return true;
    }

    virtual QScrollArea *ScrollBarsContainer() const wxOVERRIDE
    {
        return NULL;
    }

private:
    QLineEdit *edit;
};

wxQtEdit *createwxQtEdit(wxWindow *parent, wxTextCtrl *textCtrl, long style)
{
    bool multiline = (style & wxTE_MULTILINE) != 0;

    if (multiline)
        return new wxQtMultiLineEdit(new wxQtTextEdit( parent, textCtrl ));

    wxQtLineEdit *lineEdit = new wxQtLineEdit( parent, textCtrl );
    if ( style & wxTE_PASSWORD )
        lineEdit->setEchoMode( QLineEdit::Password );

    return new wxQtSingleLineEdit(lineEdit);
}

wxQtLineEdit::wxQtLineEdit( wxWindow *parent, wxTextCtrl *handler )
    : wxQtEventSignalHandler< QLineEdit, wxTextCtrl >( parent, handler )
{
    connect(this, &QLineEdit::textChanged,
            this, &wxQtLineEdit::textChanged);
    connect(this, &QLineEdit::returnPressed,
            this, &wxQtLineEdit::returnPressed);
}

void wxQtLineEdit::textChanged()
{
    wxTextEntryBase *handler = GetHandler();
    if ( handler )
    {
        handler->SendTextUpdatedEventIfAllowed();
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

wxQtTextEdit::wxQtTextEdit( wxWindow *parent, wxTextCtrl *handler )
    : wxQtEventSignalHandler< QTextEdit, wxTextCtrl >( parent, handler )
{
    connect(this, &QTextEdit::textChanged,
            this, &wxQtTextEdit::textChanged);
}

void wxQtTextEdit::textChanged()
{
    wxTextEntryBase *handler = GetHandler();
    if ( handler )
    {
        handler->SendTextUpdatedEventIfAllowed();
    }
}
}

wxTextCtrl::wxTextCtrl() :
    m_qtEdit(NULL)
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
    m_qtEdit = createwxQtEdit(parent, this, style);

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

wxTextCtrl::~wxTextCtrl()
{
    delete m_qtEdit;
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    return wxTextCtrlBase::DoGetBestSize();
}

int wxTextCtrl::GetLineLength(long lineNo) const
{
    return m_qtEdit->GetLineLength(lineNo);
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    return m_qtEdit->GetLineText(lineNo);
}

int wxTextCtrl::GetNumberOfLines() const
{
    return m_qtEdit->GetNumberOfLines();
}

bool wxTextCtrl::IsModified() const
{
    return m_qtEdit->IsModified();
}

void wxTextCtrl::MarkDirty()
{
    m_qtEdit->MarkDirty();
}

void wxTextCtrl::DiscardEdits()
{
    m_qtEdit->DiscardEdits();
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
    return m_qtEdit->XYToPosition(x, y);
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if ( x == NULL || y == NULL || pos < 0 )
        return false;

    return m_qtEdit->PositionToXY(pos, x, y);
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
    m_qtEdit->SetInsertionPoint(pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
    return m_qtEdit->GetInsertionPoint();
}

wxString wxTextCtrl::DoGetValue() const
{
    return m_qtEdit->DoGetValue();
}

void wxTextCtrl::SetSelection( long from, long to )
{
    // SelectAll uses -1 to -1, adjust for qt:
    if ( to == -1 )
        to = GetValue().length();

    if ( from == -1 )
        from = 0;

    m_qtEdit->SetSelection( from, to );
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
    if ( m_qtEdit->GetSelection(from, to) )
        return;
    // No selection, call base for default behaviour:
    wxTextEntry::GetSelection(from, to);
}

void wxTextCtrl::WriteText( const wxString &text )
{
    // Insert the text
    m_qtEdit->WriteText(text);
}

void wxTextCtrl::DoSetValue( const wxString &text, int flags )
{
    // do not fire qt signals for certain methods (i.e. ChangeText)
    if ( !(flags & SetValue_SendEvent) )
    {
        m_qtEdit->blockSignals(true);
    }

    m_qtEdit->SetValue( text );

    // re-enable qt signals
    if ( !(flags & SetValue_SendEvent) )
    {
        m_qtEdit->blockSignals(false);
    }
    SetInsertionPoint(0);
}

QWidget *wxTextCtrl::GetHandle() const
{
    return (QWidget *) m_qtEdit->GetHandle();
}

QScrollArea *wxTextCtrl::QtGetScrollBarsContainer() const
{
    return m_qtEdit->ScrollBarsContainer();
}

