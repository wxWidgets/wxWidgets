/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/filedlg.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) 2014 Sean D'Epagnier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/filename.h"

#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"
#include "wx/filedlg.h"
#include "wx/dirdlg.h"

#include <QtWidgets/QFileDialog>

class wxQtFileDialog : public wxQtEventSignalHandler< QFileDialog, wxDialog >
{
public:
    wxQtFileDialog( wxWindow *parent, wxDialog *handler,
                    const wxString& message, const wxString& defaultDir,
                    const wxString& defaultFile, const wxString& wildCard, long style )
        : wxQtEventSignalHandler<QFileDialog,wxDialog>(parent, handler)
    {
        setLabelText(QFileDialog::LookIn, wxQtConvertString(message));
        setDirectory(wxQtConvertString(defaultDir));
        selectFile(wxQtConvertString(defaultFile));

        SetWildcard(wildCard);

        if (style & wxFD_FILE_MUST_EXIST)
            setFileMode(ExistingFile);
        else if (style & wxFD_MULTIPLE)
            setFileMode(ExistingFiles);

        if (style & wxFD_SAVE)
            setAcceptMode(AcceptSave);

        if (style & wxFD_CHANGE_DIR)
            connect(this, &QDialog::accepted, this, &wxQtFileDialog::changeDirectory);
    }

    void SetWildcard(const wxString& wildCard)
    {
        QStringList wildCards = wxQtConvertString(wildCard).split("|");
        QStringList filters;
        for (int i=0; i<wildCards.size()-1; i+=2)
        {
            // discard everything after first (
            QString name = wildCards.at(i);
            name = name.left(name.indexOf("("));

            // replace filter ; separator with qt style space
            QString filter = wildCards.at(i+1);
            filter.replace(";", " ");

            filters += name + " (" + filter + ")";
        }

        setNameFilters(filters);
    }

    void changeDirectory()
    {
        wxSetWorkingDirectory(wxQtConvertString(directory().absolutePath()));
    }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxFileDialog, wxDialog);

wxFileDialog::wxFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFile,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos,
                           const wxSize& sz,
                           const wxString& name)
{
    Create(parent, message, defaultDir, defaultFile,
           wildCard, style, pos, sz, name);
}

bool wxFileDialog::Create(wxWindow *parent,
                          const wxString& message,
                          const wxString& defaultDir,
                          const wxString& defaultFile,
                          const wxString& wildCard,
                          long style,
                          const wxPoint& pos,
                          const wxSize& sz,
                          const wxString& name)
{
    m_qtWindow = new wxQtFileDialog( parent, this, message, defaultDir,
                                     defaultFile, wildCard, style);

    return wxTopLevelWindow::Create( parent, wxID_ANY, message, pos, sz, style, name );
}

wxString wxFileDialog::GetPath() const
{
    wxCHECK_MSG( !HasFlag(wxFD_MULTIPLE), wxString(), "When using wxFD_MULTIPLE, must call GetPaths() instead" );

    wxArrayString paths;
    GetPaths(paths);
    if (paths.empty())
        return "";

    return paths[0];
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    QStringList selectedfiles = GetQFileDialog()->selectedFiles();
    foreach (QString file, selectedfiles)
        paths.Add(wxQtConvertString(file));
}

wxString wxFileDialog::GetFilename() const
{
    wxCHECK_MSG( !HasFlag(wxFD_MULTIPLE), wxString(), "When using wxFD_MULTIPLE, must call GetFilenames() instead" );

    wxArrayString filenames;
    GetFilenames(filenames);
    if ( filenames.empty() )
        return "";

    return filenames[0];
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    QStringList selectedfiles = GetQFileDialog()->selectedFiles();
    foreach (QString file, selectedfiles )
    {
        wxFileName fn = wxQtConvertString(file);
        files.Add(fn.GetFullName());
    }
}

int wxFileDialog::GetFilterIndex() const
{
    int index = 0;
    foreach (QString filter, GetQFileDialog()->nameFilters())
    {
        if (filter == GetQFileDialog()->selectedNameFilter())
            return index;
        index++;
    }

    return -1;
}

void wxFileDialog::SetMessage(const wxString& message)
{
    GetQFileDialog()->setLabelText(QFileDialog::LookIn, wxQtConvertString(message));
}

void wxFileDialog::SetPath(const wxString& path)
{
    GetQFileDialog()->selectFile(wxQtConvertString(path));
}

void wxFileDialog::SetDirectory(const wxString& dir)
{
    GetQFileDialog()->setDirectory(wxQtConvertString(dir));
}

void wxFileDialog::SetFilename(const wxString& name)
{
    SetPath(name);
}

void wxFileDialog::SetWildcard(const wxString& wildCard)
{
    GetQFileDialog()->setNameFilter(wxQtConvertString(wildCard));
}

void wxFileDialog::SetFilterIndex(int filterIndex)
{
    QStringList filters = GetQFileDialog()->nameFilters();
    if (filterIndex < 0 || filterIndex >= filters.size())
        return;

    GetQFileDialog()->setNameFilter(filters.at(filterIndex));
}

QFileDialog *wxFileDialog::GetQFileDialog() const
{
    return static_cast<QFileDialog *>(m_qtWindow);
}

// Qt implements File and Dir Dialogs in the same class, so use a derived
// helper class to save duplication
class wxQtDirDialog : public wxQtFileDialog
{
public:
    wxQtDirDialog(wxWindow *parent,
                  wxDialog *handler,
                  const wxString& message,
                  const wxString& defaultPath,
                  long style )

        : wxQtFileDialog( parent, handler, message, defaultPath, "", "",
                          (style & wxDD_DIR_MUST_EXIST ? wxFD_FILE_MUST_EXIST : 0) |
                          (style & wxDD_CHANGE_DIR ? wxFD_CHANGE_DIR : 0 ))
        {
            setFileMode(QFileDialog::Directory);
            setOption(ShowDirsOnly, true);
        }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxDirDialog, wxDialog);

wxDirDialog::wxDirDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxString& name)
{
    Create(parent, message, defaultPath, style, pos, size, name);
}

bool wxDirDialog::Create(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxString& name)
{
    m_qtWindow = new wxQtDirDialog( parent, this, message, defaultPath, style);
    return wxTopLevelWindow::Create( parent, wxID_ANY, message, pos, size, style, name );
}

wxString wxDirDialog::GetPath() const
{
    QStringList selectedfiles = GetQFileDialog()->selectedFiles();
    if (selectedfiles.size() > 0)
        return wxQtConvertString(selectedfiles.first());

    return "";
}

void wxDirDialog::SetPath(const wxString& path)
{
    GetQFileDialog()->selectFile(wxQtConvertString(path));
}

QFileDialog *wxDirDialog::GetQFileDialog() const
{
    return static_cast<QFileDialog *>(m_qtWindow);
}
