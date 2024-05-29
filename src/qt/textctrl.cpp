/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textctrl.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TEXTCTRL

#include "wx/textctrl.h"
#include "wx/settings.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"
#include "wx/qt/private/utils.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>

#include <limits>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#define wxHAS_QT_INPUTREJECTED
#endif

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
    virtual wxTextCtrlHitTestResult HitTest(const wxPoint& pt, long *pos) const = 0;
    virtual QAbstractScrollArea *ScrollBarsContainer() const = 0;
    virtual void WriteText( const wxString &text ) = 0;
    virtual void SetMaxLength(unsigned long len) = 0;
    virtual void MarkDirty() = 0;
    virtual void DiscardEdits() = 0;
    virtual void blockSignals(bool block) = 0;
    virtual void SetValue( const wxString &value ) = 0;
    virtual void SetSelection( long from, long to ) = 0;
    virtual void SetInsertionPoint(long pos) = 0;
    virtual void SetStyleFlags(long flags) = 0;
    virtual void Copy() = 0;
    virtual void Cut() = 0;
    virtual void Paste() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual bool CanUndo() const = 0;
    virtual bool CanRedo() const = 0;
    virtual void EmptyUndoBuffer() = 0;
};

namespace
{

// Helper for SetStyleFlags(): takes care of flags that are handled in the same
// way in both QLineEdit and QTextEdit.
template <typename Edit>
void ApplyCommonStyles(Edit* edit, long flags)
{
    edit->setReadOnly(flags & wxTE_READONLY);

    if ( flags & wxTE_CENTRE )
        edit->setAlignment(Qt::AlignHCenter);
    else if ( flags & wxTE_RIGHT )
        edit->setAlignment(Qt::AlignRight);
    else // wxTE_LEFT is 0, so can't test for it, just use it by default
        edit->setAlignment(Qt::AlignLeft);
}


struct wxQtLineInfo
{
  wxQtLineInfo(size_t start, size_t end) :
    startPos(start),
    endPos(end)
  {
  }

  size_t startPos, endPos;
};

class wxQtLineEdit : public wxQtEventSignalHandler< QLineEdit, wxTextCtrl >
{
public:
    wxQtLineEdit( wxWindow *parent, wxTextCtrl *handler );

    virtual wxString GetValueForProcessEnter() override
    {
        return GetHandler()->GetValue();
    }

    // cursorRect() is protected in base class. Make it public
    // so it can be accessed by wxQtSingleLineEdit::HitTest()
    using QLineEdit::cursorRect;

private:
    void textChanged();

#ifdef wxHAS_QT_INPUTREJECTED
    void inputRejected();
#endif
};

class wxQtTextEdit : public wxQtEventSignalHandler< QTextEdit, wxTextCtrl >
{
public:
    wxQtTextEdit( wxWindow *parent, wxTextCtrl *handler );

    virtual wxString GetValueForProcessEnter() override
    {
        return GetHandler()->GetValue();
    }

    bool IsUndoAvailable() const { return m_undoAvailable; }
    bool IsRedoAvailable() const { return m_redoAvailable; }

private:
    void textChanged();

    bool m_undoAvailable = false,
         m_redoAvailable = false;
};

class wxQtMultiLineEdit : public wxQtEdit
{
public:
    explicit wxQtMultiLineEdit(QTextEdit *edit) : m_edit(edit)
    {
        // wx expects no initial undo when the control is first created, i.e.:
        // CanUndo() should return false. but Qt emits undoAvailable(true) for
        // a freshly created control and we should prevent this by disabling
        // undo/redo here and re-enable it in textChanged signal.
        m_edit->setUndoRedoEnabled(false);
    }

    virtual bool IsModified() const override
    {
        return m_edit->isWindowModified();
    }

    virtual void Copy() override { m_edit->copy(); }
    virtual void Cut() override  { m_edit->cut(); }
    virtual void Paste() override  { m_edit->paste(); }

    virtual void Undo() override  { m_edit->undo(); }
    virtual void Redo() override  { m_edit->redo(); }

    virtual bool CanUndo() const override
    {
        return static_cast<wxQtTextEdit*>(m_edit)->IsUndoAvailable();
    }

    virtual bool CanRedo() const override
    {
        return static_cast<wxQtTextEdit*>(m_edit)->IsRedoAvailable();
    }

    virtual void EmptyUndoBuffer() override
    {
        m_edit->setUndoRedoEnabled(false);
        m_edit->setUndoRedoEnabled(true);
    }

    virtual wxString DoGetValue() const override
    {
        return wxQtConvertString( m_edit->toPlainText() );
    }

    virtual long GetInsertionPoint() const override
    {
        QTextCursor cursor = m_edit->textCursor();
        return cursor.anchor();
    }

    virtual QWidget *GetHandle() const override
    {
        return m_edit;
    }

    virtual int GetNumberOfLines() const override
    {
        const wxString &value = DoGetValue();
        return std::count(value.begin(), value.end(), '\n') + 1;
    }

    virtual int GetLineLength(long lineNo) const override
    {
        wxQtLineInfo start = GetLineInfo(lineNo, DoGetValue());
        if ( start.startPos == wxString::npos )
            return -1;

        return start.endPos - start.startPos;
    }

    virtual wxString GetLineText(long lineNo) const override
    {
        const wxString &value = DoGetValue();

        wxQtLineInfo start = GetLineInfo(lineNo, value);
        if ( start.startPos == wxString::npos )
            return wxString();

        return value.Mid(start.startPos, start.endPos - start.startPos);
    }

    virtual long XYToPosition(long x, long y) const override
    {
        if ( x < 0 || y < 0 )
            return -1;

        wxQtLineInfo start = GetLineInfo(y, DoGetValue());
        if ( start.startPos == wxString::npos )
            return -1;

        if ( start.endPos - start.startPos < static_cast<size_t>(x) )
            return -1;

        return start.startPos + x;
    }

    virtual bool PositionToXY(long pos, long *x, long *y) const override
    {
        const wxString &value = DoGetValue();

        if ( static_cast<size_t>(pos) > value.length() )
          return false;

        int cnt = 0;
        int xval = 0;

        for ( long xx = 0; xx < pos; xx++ )
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

    virtual wxTextCtrlHitTestResult
    HitTest(const wxPoint& pt, long* pos) const override
    {
        auto qtEdit = static_cast<wxQtTextEdit* const>(m_edit);

        auto cursor  = qtEdit->cursorForPosition( wxQtConvertPoint(pt) );
        auto curRect = qtEdit->cursorRect(cursor);

        if ( pos )
            *pos = cursor.position();

        if ( pt.y > curRect.y() + qtEdit->fontMetrics().height() )
            return wxTE_HT_BELOW;

        if ( pt.x > curRect.x() + qtEdit->fontMetrics().averageCharWidth() )
            return wxTE_HT_BEYOND;

        return wxTE_HT_ON_TEXT;
    }

    virtual void WriteText( const wxString &text ) override
    {
        m_edit->insertPlainText(wxQtConvertString( text ));
        // the cursor is moved to the end, ensure it is shown
        m_edit->ensureCursorVisible();
    }

    virtual void SetMaxLength(unsigned long WXUNUSED(len)) override
    {
        wxMISSING_IMPLEMENTATION("not implemented for multiline control");
    }

    virtual void MarkDirty() override
    {
        return m_edit->setWindowModified( true );
    }

    virtual void DiscardEdits() override
    {
        return m_edit->setWindowModified( false );
    }

    virtual void blockSignals(bool block) override
    {
        m_edit->blockSignals(block);
    }

    virtual void SetValue( const wxString &value ) override
    {
        m_edit->setPlainText(wxQtConvertString( value ));
        // the cursor is moved to the end, ensure it is shown
        m_edit->ensureCursorVisible();
    }

    virtual void SetSelection( long from, long to ) override
    {
        QTextCursor cursor = m_edit->textCursor();
        cursor.setPosition(from);

        cursor.setPosition(to, QTextCursor::KeepAnchor);
        m_edit->setTextCursor(cursor);
    }

    virtual bool GetSelection( long *from, long *to ) const override
    {
        QTextCursor cursor = m_edit->textCursor();
        *from = cursor.selectionStart();
        *to = cursor.selectionEnd();
        return cursor.hasSelection();
    }

    virtual void SetInsertionPoint(long pos) override
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

        QTextCursor cursor = m_edit->textCursor();
        cursor.movePosition(op, QTextCursor::MoveAnchor, pos);

        if (op != QTextCursor::End )
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, pos);
        m_edit->setTextCursor(cursor);
        m_edit->ensureCursorVisible();
    }

    QAbstractScrollArea *ScrollBarsContainer() const override
    {
        return static_cast<QAbstractScrollArea*>(m_edit);
    }

    virtual void SetStyleFlags(long flags) override
    {
        ApplyCommonStyles(m_edit, flags);

        if ( flags & wxNO_BORDER )
            m_edit->setFrameStyle(QFrame::NoFrame);

        if ( flags & wxTE_RICH || flags & wxTE_RICH2 )
            m_edit->setAcceptRichText(true);

        if ( flags & wxTE_DONTWRAP )
            m_edit->setLineWrapMode(QTextEdit::NoWrap);
    }

private:
    wxQtLineInfo GetLineInfo(long lineNo, const wxString &value) const
    {
        size_t pos = 0;
        long cnt = 0;

        while ( cnt < lineNo )
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

    QTextEdit* const m_edit;

    wxDECLARE_NO_COPY_CLASS(wxQtMultiLineEdit);
};

class wxQtSingleLineEdit : public wxQtEdit
{
public:
    explicit wxQtSingleLineEdit(QLineEdit *edit) :
        m_edit(edit)
    {
    }

    virtual bool IsModified() const override
    {
        return m_edit->isModified();
    }

    virtual int GetNumberOfLines() const override
    {
        return 1;
    }

    virtual void Copy() override { m_edit->copy(); }
    virtual void Cut() override  { m_edit->cut(); }
    virtual void Paste() override  { m_edit->paste(); }

    virtual void Undo() override  { m_edit->undo(); }
    virtual void Redo() override  { m_edit->redo(); }
    virtual bool CanUndo() const override  { return m_edit->isUndoAvailable(); }
    virtual bool CanRedo() const override  { return m_edit->isRedoAvailable(); }

    virtual void EmptyUndoBuffer() override
    {
        // how to empty undo buffer for QLineEdit control ?
    }

    virtual wxString DoGetValue() const override
    {
        return wxQtConvertString( m_edit->text() );
    }

    virtual long GetInsertionPoint() const override
    {
        long selectionStart = m_edit->selectionStart();

        if ( selectionStart >= 0 )
            return selectionStart;

        return m_edit->cursorPosition();
    }

    virtual QWidget *GetHandle() const override
    {
        return m_edit;
    }

    virtual int GetLineLength(long WXUNUSED(lineNo)) const override
    {
        return DoGetValue().length();
    }

    virtual wxString GetLineText(long lineNo) const override
    {
        return lineNo == 0 ? DoGetValue() : wxString();
    }

    virtual void WriteText( const wxString &text ) override
    {
        m_edit->insert(wxQtConvertString( text ));
    }

    virtual void SetMaxLength(unsigned long len) override
    {
        // Notice that setMaxLength() takes an int and not an unsigned int
        m_edit->setMaxLength(
            len > std::numeric_limits<int>::max()
                ? std::numeric_limits<int>::max() : len
        );
    }

    virtual void MarkDirty() override
    {
        return m_edit->setModified( true );
    }

    virtual void DiscardEdits() override
    {
        return m_edit->setModified( false );
    }

    virtual void blockSignals(bool block) override
    {
        m_edit->blockSignals(block);
    }

    virtual void SetValue( const wxString &value ) override
    {
        m_edit->setText(wxQtConvertString( value ));
    }

    virtual void SetSelection( long from, long to ) override
    {
        m_edit->setSelection(from, to - from);
    }

    virtual bool GetSelection( long *from, long *to ) const override
    {
        *from = m_edit->selectionStart();
        if ( *from < 0 )
            return false;

        *to = *from + m_edit->selectedText().length();
        return true;
    }

    virtual void SetInsertionPoint(long pos) override
    {
        // check if pos indicates end of text:
        if ( pos == -1 )
            m_edit->end(false);
        else
            m_edit->setCursorPosition(pos);
    }

    virtual long XYToPosition(long x, long y) const override
    {
        if ( y == 0 && x >= 0 )
        {
            if ( static_cast<size_t>(x) <= DoGetValue().length() )
              return x;
        }

        return -1;
    }

    virtual bool PositionToXY(long pos, long *x, long *y) const override
    {
        const wxString &value = DoGetValue();

        if ( static_cast<size_t>(pos) > value.length() )
          return false;

        *y = 0;
        *x = pos;
        return true;
    }

    virtual wxTextCtrlHitTestResult
    HitTest(const wxPoint& pt, long *pos) const override
    {
        auto qtEdit  = static_cast<wxQtLineEdit* const>(m_edit);
        auto curPos  = qtEdit->cursorPositionAt( wxQtConvertPoint(pt) );
        auto curRect = qtEdit->cursorRect();

        if ( pos )
            *pos = curPos;

        if ( pt.y > curRect.y() + qtEdit->fontMetrics().height() )
            return wxTE_HT_BELOW;

        if ( pt.x > curRect.x() + qtEdit->fontMetrics().averageCharWidth() )
            return wxTE_HT_BEYOND;

        return wxTE_HT_ON_TEXT;
    }

    virtual QAbstractScrollArea *ScrollBarsContainer() const override
    {
        return nullptr;
    }

    virtual void SetStyleFlags(long flags) override
    {
        ApplyCommonStyles(m_edit, flags);

        m_edit->setFrame(!(flags & wxNO_BORDER));

        if ( flags & wxTE_PASSWORD )
            m_edit->setEchoMode(QLineEdit::Password);
    }

private:
    QLineEdit *m_edit;

    wxDECLARE_NO_COPY_CLASS(wxQtSingleLineEdit);
};

wxQtLineEdit::wxQtLineEdit( wxWindow *parent, wxTextCtrl *handler )
    : wxQtEventSignalHandler< QLineEdit, wxTextCtrl >( parent, handler )
{
    connect(this, &QLineEdit::textChanged,
            this, &wxQtLineEdit::textChanged);

#ifdef wxHAS_QT_INPUTREJECTED
    connect(this, &QLineEdit::inputRejected,
            this, &wxQtLineEdit::inputRejected);
#endif
}

void wxQtLineEdit::textChanged()
{
    wxTextEntryBase *handler = GetHandler();
    if ( handler )
    {
        handler->SendTextUpdatedEventIfAllowed();
    }
}

#ifdef wxHAS_QT_INPUTREJECTED
void wxQtLineEdit::inputRejected()
{
    wxCommandEvent event(wxEVT_TEXT_MAXLEN, GetHandler()->GetId());
    event.SetString( GetHandler()->GetValue() );
    EmitEvent( event );
}
#endif // wxHAS_QT_INPUTREJECTED

wxQtTextEdit::wxQtTextEdit( wxWindow *parent, wxTextCtrl *handler )
    : wxQtEventSignalHandler< QTextEdit, wxTextCtrl >( parent, handler )
{
    connect(this, &QTextEdit::textChanged,
            this, &wxQtTextEdit::textChanged);

    connect(this, &QTextEdit::undoAvailable, [this](bool available) {
                m_undoAvailable = available;
            });
    connect(this, &QTextEdit::redoAvailable, [this](bool available) {
                m_redoAvailable = available;
            });
}

void wxQtTextEdit::textChanged()
{
    wxTextEntryBase *handler = GetHandler();
    if ( handler )
    {
        handler->SendTextUpdatedEventIfAllowed();
    }

    if ( !isUndoRedoEnabled() )
        setUndoRedoEnabled(true);
}

} // anonymous namespace


wxTextCtrl::wxTextCtrl() :
    m_qtEdit(nullptr)
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
    if ( style & wxTE_MULTILINE )
    {
        m_qtEdit = new wxQtMultiLineEdit(new wxQtTextEdit(parent, this));

        if ( style & wxTE_NO_VSCROLL )
            style &= ~wxVSCROLL;
        else
            style |= wxVSCROLL;
    }
    else
    {
        m_qtEdit = new wxQtSingleLineEdit(new wxQtLineEdit(parent, this));
    }

    m_qtEdit->SetStyleFlags(style);

    m_qtWindow = m_qtEdit->ScrollBarsContainer();

    // set the initial text value without sending the event
    ChangeValue( value );

    return wxTextCtrlBase::Create( parent, id, pos, size, style, validator, name );
}

wxTextCtrl::~wxTextCtrl()
{
    delete m_qtEdit;
    m_qtEdit = nullptr;
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    if (IsSingleLine())
        return wxQtConvertSize(m_qtEdit->GetHandle()->sizeHint());

    return wxSize(80,
                  1 + GetCharHeight() * wxMax(wxMin(GetNumberOfLines(), 10), 2));
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
    if ( x == nullptr || y == nullptr || pos < 0 )
        return false;

    return m_qtEdit->PositionToXY(pos, x, y);
}

void wxTextCtrl::ShowPosition(long WXUNUSED(pos))
{
}

wxTextCtrlHitTestResult
wxTextCtrl::HitTest(const wxPoint& pt, long *pos) const
{
    return m_qtEdit->HitTest(pt, pos);
}

bool wxTextCtrl::DoLoadFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

bool wxTextCtrl::DoSaveFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

void wxTextCtrl::Copy()
{
    m_qtEdit->Copy();
}

void wxTextCtrl::Cut()
{
    m_qtEdit->Cut();
}

void wxTextCtrl::Paste()
{
    m_qtEdit->Paste();
}

void wxTextCtrl::Undo()
{
    m_qtEdit->Undo();
}

void wxTextCtrl::Redo()
{
    m_qtEdit->Redo();
}

bool wxTextCtrl::CanUndo() const
{
    return m_qtEdit->CanUndo();
}

bool wxTextCtrl::CanRedo() const
{
    return m_qtEdit->CanRedo();
}

void wxTextCtrl::EmptyUndoBuffer()
{
    m_qtEdit->EmptyUndoBuffer();
}

bool wxTextCtrl::IsEditable() const
{
    return HasFlag(wxTE_READONLY);
}

void wxTextCtrl::SetEditable(bool editable)
{
    long flags = GetWindowStyle();

    if ( editable )
        flags &= ~wxTE_READONLY;
    else
        flags |= wxTE_READONLY;

    SetWindowStyle(flags);
    m_qtEdit->SetStyleFlags(flags);
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

void wxTextCtrl::SetMaxLength(unsigned long len)
{
    m_qtEdit->SetMaxLength(len);
}

void wxTextCtrl::DoSetValue( const wxString &text, int flags )
{
    if ( text != DoGetValue() )
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
    else
    {
        if ( flags & SetValue_SendEvent )
            SendTextUpdatedEventIfAllowed();
    }
}

QWidget *wxTextCtrl::GetHandle() const
{
    return (QWidget *) m_qtEdit->GetHandle();
}

#endif // wxUSE_TEXTCTRL
