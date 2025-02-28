/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textentry.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textentry.h"
#include "wx/textcompleter.h"
#include "wx/window.h"
#include "wx/qt/private/converter.h"

#include <QtCore/QStringListModel>
#include <QtWidgets/QCompleter>
#include <QFileSystemModel>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

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

void wxTextEntry::DoSetValue(const wxString& value, int flags)
{
    wxTextEntryBase::DoSetValue(value, flags);
}

wxWindow *wxTextEntry::GetEditableWindow()
{
    return nullptr;
}

void wxTextEntry::EnableTextChangedEvents(bool enable)
{
    wxWindow* const win = GetEditableWindow();

    if ( win )
        win->GetHandle()->blockSignals(!enable);
}

// ----------------------------------------------------------------------------
// auto-completion
// ----------------------------------------------------------------------------
namespace
{
// This class is taken from Qt documentation "as is" to see "C:\Program Files"
// instead of just "Program Files" as QFileSystemModel does by default.
class FileSystemModel : public QFileSystemModel
{
public:
    explicit FileSystemModel(QObject* parent = nullptr) : QFileSystemModel(parent)
    {
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if ( role == Qt::DisplayRole && index.column() == 0 )
        {
            QString path = QDir::toNativeSeparators(filePath(index));

            if ( path.endsWith(QDir::separator()) )
                path.chop(1);

            return path;
        }

        return QFileSystemModel::data(index, role);
    }
};
}

// This class gathers the all auto-complete-related stuff we use. It is
// allocated on demand by wxTextEntry when AutoComplete() is called.
class wxTextAutoCompleteData
{
public:
    enum class CompleterType
    {
        StringCompleter,
        FileSystemCompleter
    };

    // The constructor associates us with the given text entry.
    explicit wxTextAutoCompleteData(wxTextEntry* entry,
                                    CompleterType type = CompleterType::StringCompleter)
        : m_win(entry->GetEditableWindow())
    {
        if ( m_win )
        {
            auto qtWidget = m_win->GetHandle();
            m_qtCompleter = new QCompleter(qtWidget->parentWidget());

            if ( type == CompleterType::FileSystemCompleter )
            {
                auto fsModel = new FileSystemModel(m_qtCompleter);
                fsModel->setRootPath(QDir::currentPath());
                m_qtCompleter->setModel(fsModel);
            }
            else
            {
                m_qtCompleter->setModel( new QStringListModel(m_qtCompleter) );
                m_qtCompleter->setCaseSensitivity(Qt::CaseInsensitive);
            }

            if ( auto lineEdit = qobject_cast<QLineEdit*>(qtWidget) )
            {
                lineEdit->setCompleter(m_qtCompleter);
            }
            else if ( auto comboBox = qobject_cast<QComboBox*>(qtWidget) )
            {
                comboBox->setCompleter(m_qtCompleter);
            }
            else
            {
                wxDELETE(m_qtCompleter);
            }
        }
    }

    ~wxTextAutoCompleteData()
    {
        ChangeCustomCompleter(nullptr); // disable custom completer if any.
    }

    bool IsOk() const { return m_qtCompleter != nullptr; }

    void ChangeStrings(const wxArrayString& strings)
    {
        ChangeCustomCompleter(nullptr); // disable custom completer if any.

        auto listModel = qobject_cast<QStringListModel*>(m_qtCompleter->model());

        wxASSERT_MSG(listModel, "A QStringListModel object is expected here");

        QStringList list;

        for ( auto str : strings )
        {
            list << wxQtConvertString( str );
        }

        listModel->setStringList(list);
    }

    // Takes ownership of the pointer if it is non-null.
    bool ChangeCustomCompleter(wxTextCompleter* completer)
    {
        if ( m_completer )
            m_win->Unbind(wxEVT_TEXT, &wxTextAutoCompleteData::OnEntryChanged, this);

        delete m_completer;
        m_completer = completer;

        if ( m_completer )
            m_win->Bind(wxEVT_TEXT, &wxTextAutoCompleteData::OnEntryChanged, this);

        return true;
    }

    void DisableCompletion()
    {
        if ( m_qtCompleter )
        {
            auto qtWidget = m_qtCompleter->widget();

            if ( auto lineEdit = qobject_cast<QLineEdit*>(qtWidget) )
            {
                lineEdit->setCompleter(nullptr);
            }
            else if ( auto comboBox = qobject_cast<QComboBox*>(qtWidget) )
            {
                comboBox->setCompleter(nullptr);
            }

            m_qtCompleter = nullptr;
        }
    }

private:
    // Update the strings returned by QCompleter to correspond to
    // the currently valid choices according to the custom completer.
    void UpdateStringsFromCustomCompleter()
    {
        if ( !m_completer )
            return;

        auto listModel = qobject_cast<QStringListModel*>(m_qtCompleter->model());

        wxASSERT_MSG(listModel, "A QStringListModel object is expected here");

        const wxString prefix = wxQtConvertString(m_qtCompleter->completionPrefix());

        if ( m_prefix == prefix )
            return;

        QStringList list;

        if ( m_completer->Start(prefix) )
        {
            m_prefix = prefix;

            for (;;)
            {
                const wxString s = m_completer->GetNext();

                if ( s.empty() )
                    break;

                list << wxQtConvertString(s);
            }
        }

        listModel->setStringList(list);
    }

    void OnEntryChanged(wxCommandEvent& event)
    {
        UpdateStringsFromCustomCompleter();

        event.Skip();
    }

    // The window of this text entry.
    wxWindow* const m_win;

    // Custom completer or nullptr if none.
    wxTextCompleter* m_completer = nullptr;

    // This pointer is owned by the underlying QWidget (QLineEdit, QComboBox, ...)
    QCompleter* m_qtCompleter = nullptr;

    wxString m_prefix;

    wxDECLARE_NO_COPY_CLASS(wxTextAutoCompleteData);
};

bool wxTextEntry::DoAutoCompleteFileNames(int WXUNUSED(flags))
{
    if ( m_autoCompleteData )
    {
        m_autoCompleteData->DisableCompletion();

        wxDELETE(m_autoCompleteData);
    }

    auto fsCompleter = wxTextAutoCompleteData::CompleterType::FileSystemCompleter;

    std::unique_ptr<wxTextAutoCompleteData>
        autoCompleteData( new wxTextAutoCompleteData(this, fsCompleter) );

    if ( autoCompleteData->IsOk() )
    {
        m_autoCompleteData = autoCompleteData.release();
    }

    return m_autoCompleteData != nullptr;
}

bool wxTextEntry::DoAutoCompleteStrings(const wxArrayString& choices)
{
    if ( m_autoCompleteData )
    {
        m_autoCompleteData->DisableCompletion();

        wxDELETE(m_autoCompleteData);
    }

    std::unique_ptr<wxTextAutoCompleteData>
        autoCompleteData( new wxTextAutoCompleteData(this) );

    if ( autoCompleteData->IsOk() )
    {
        m_autoCompleteData = autoCompleteData.release();
        m_autoCompleteData->ChangeStrings(choices);
    }

    return m_autoCompleteData != nullptr;
}

bool wxTextEntry::DoAutoCompleteCustom(wxTextCompleter* completer)
{
    if ( m_autoCompleteData )
    {
        m_autoCompleteData->DisableCompletion();

        wxDELETE(m_autoCompleteData);
    }

    std::unique_ptr<wxTextAutoCompleteData>
        autoCompleteData( new wxTextAutoCompleteData(this) );

    if ( autoCompleteData->IsOk() )
    {
        m_autoCompleteData = autoCompleteData.release();
        m_autoCompleteData->ChangeCustomCompleter(completer);
    }

    return m_autoCompleteData != nullptr;
}
