/////////////////////////////////////////////////////////////////////////////
// Name:        common/fldlgcmn.cpp
// Purpose:     wxFileDialog common functions
// Author:      John Labenski
// Modified by:
// Created:     14.06.03 (extracted from src/*/filedlg.cpp)
// RCS-ID:
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "filedlg.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/filedlg.h"

#if wxUSE_FILEDLG

//----------------------------------------------------------------------------
// wxFileDialogBase
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileDialogBase, wxDialog)

wxFileDialogBase::wxFileDialogBase(wxWindow *parent,
                                   const wxString& message,
                                   const wxString& defaultDir,
                                   const wxString& defaultFile,
                                   const wxString& wildCard,
                                   long style,
                                   const wxPoint& pos)
{
    m_parent = parent;
    m_message = message;
    m_dir = defaultDir;
    m_fileName = defaultFile;
    m_wildCard = wildCard;
    m_dialogStyle = style;
    m_path = wxT("");
    m_filterIndex = 0;

    if (m_wildCard.IsEmpty())
        m_wildCard = wxFileSelectorDefaultWildcardStr;

    // convert m_wildCard from "*.bar" to "Files (*.bar)|*.bar"
    if ( m_wildCard.Find(wxT('|')) == wxNOT_FOUND )
    {
        m_wildCard.Printf(_("Files (%s)|%s"),
                          m_wildCard.c_str(), m_wildCard.c_str());
    }
}

// Parses the filterStr, returning the number of filters.
// Returns 0 if none or if there's a problem.
// filterStr is in the form: "All files (*.*)|*.*|JPEG Files (*.jpeg)|*.jpg"

int wxFileDialogBase::ParseWildcard(const wxString& filterStr,
                                    wxArrayString& descriptions,
                                    wxArrayString& filters)
{
    descriptions.Clear();
    filters.Clear();

    wxString str(filterStr);

    wxString description, filter;
    for ( int pos = 0; pos != wxNOT_FOUND; )
    {
        pos = str.Find(wxT('|'));
        if ( pos == wxNOT_FOUND )
        {
            // if there are no '|'s at all in the string just take the entire
            // string as filter
            if ( filters.IsEmpty() )
            {
                descriptions.Add(filterStr);
                filters.Add(filterStr);
            }
            else
            {
                wxFAIL_MSG( _T("missing '|' in the wildcard string!") );
            }

            break;
        }

        description = str.Left(pos);
        str = str.Mid(pos + 1);
        pos = str.Find(wxT('|'));
        if ( pos == wxNOT_FOUND )
        {
            filter = str;
        }
        else
        {
            filter = str.Left(pos);
            str = str.Mid(pos + 1);
        }

        descriptions.Add(description);
        filters.Add(filter);
    }

    return filters.GetCount();
}

wxString wxFileDialogBase::AppendExtension(const wxString &filePath,
                                           const wxString &extensionList)
{
    // strip off path, to avoid problems with "path.bar/foo"
    wxString fileName = filePath.AfterLast(wxFILE_SEP_PATH);

    // if fileName is of form "foo.bar" it's ok, return it
    int idx_dot = fileName.Find(wxT('.'), TRUE);
    if ((idx_dot != wxNOT_FOUND) && (idx_dot < (int)fileName.Len() - 1))
        return filePath;

    // get the first extension from extensionList, or all of it
    wxString ext = extensionList.BeforeFirst(wxT(';'));

    // if ext == "foo" or "foo." there's no extension
    int idx_ext_dot = ext.Find(wxT('.'), TRUE);
    if ((idx_ext_dot == wxNOT_FOUND) || (idx_ext_dot == (int)ext.Len() - 1))
        return filePath;
    else
        ext = ext.AfterLast(wxT('.'));

    // if ext == "*" or "bar*" or "b?r" or " " then its not valid
    if ((ext.Find(wxT('*')) != wxNOT_FOUND) ||
        (ext.Find(wxT('?')) != wxNOT_FOUND) ||
        (ext.Strip(wxString::both).IsEmpty()))
        return filePath;

    // if fileName doesn't have a '.' then add one
    if (filePath.Last() != wxT('.'))
        ext = wxT(".") + ext;

    return filePath + ext;
}

//----------------------------------------------------------------------------
// wxFileDialog convenience functions
//----------------------------------------------------------------------------

wxString wxFileSelector(const wxChar *title,
                               const wxChar *defaultDir,
                               const wxChar *defaultFileName,
                               const wxChar *defaultExtension,
                               const wxChar *filter,
                               int flags,
                               wxWindow *parent,
                               int x, int y)
{
    // The defaultExtension, if non-NULL, is
    // appended to the filename if the user fails to type an extension. The new
    // implementation (taken from wxFileSelectorEx) appends the extension
    // automatically, by looking at the filter specification. In fact this
    // should be better than the native Microsoft implementation because
    // Windows only allows *one* default extension, whereas here we do the
    // right thing depending on the filter the user has chosen.

    // If there's a default extension specified but no filter, we create a
    // suitable filter.

    wxString filter2;
    if ( defaultExtension && !filter )
        filter2 = wxString(wxT("*.")) + defaultExtension;
    else if ( filter )
        filter2 = filter;

    wxString defaultDirString;
    if (defaultDir)
        defaultDirString = defaultDir;

    wxString defaultFilenameString;
    if (defaultFileName)
        defaultFilenameString = defaultFileName;

    wxFileDialog fileDialog(parent, title, defaultDirString,
                            defaultFilenameString, filter2,
                            flags, wxPoint(x, y));

   // if filter is of form "All files (*)|*|..." set correct filter index
    if((wxStrlen(defaultExtension) != 0) && (filter2.Find(wxT('|')) != wxNOT_FOUND))
            {
        int filterIndex = 0;

        wxArrayString descriptions, filters;
        // don't care about errors, handled already by wxFileDialog
        (void)wxFileDialogBase::ParseWildcard(filter2, descriptions, filters);
        for (size_t n=0; n<filters.GetCount(); n++)
                {
            if (filters[n].Contains(defaultExtension))
                    {
                filterIndex = n;
                        break;
                    }
                }

        if (filterIndex > 0)
            fileDialog.SetFilterIndex(filterIndex);
        }

    wxString filename;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        filename = fileDialog.GetPath();
    }

    return filename;
}

//----------------------------------------------------------------------------
// wxFileSelectorEx
//----------------------------------------------------------------------------

wxString wxFileSelectorEx(const wxChar *title,
                          const wxChar *defaultDir,
                          const wxChar *defaultFileName,
                          int* defaultFilterIndex,
                          const wxChar *filter,
                          int       flags,
                          wxWindow* parent,
                          int       x,
                          int       y)

{
    wxFileDialog fileDialog(parent,
                            title ? title : wxT(""),
                            defaultDir ? defaultDir : wxT(""),
                            defaultFileName ? defaultFileName : wxT(""),
                            filter ? filter : wxT(""),
                            flags, wxPoint(x, y));

    wxString filename;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        if ( defaultFilterIndex )
            *defaultFilterIndex = fileDialog.GetFilterIndex();

        filename = fileDialog.GetPath();
    }

    return filename;
}

//----------------------------------------------------------------------------
// wxDefaultFileSelector - Generic load/save dialog (for internal use only)
//----------------------------------------------------------------------------

static wxString wxDefaultFileSelector(bool load,
                                      const wxChar *what,
                                      const wxChar *extension,
                                      const wxChar *default_name,
                                      wxWindow *parent)
{
    wxString prompt;
    wxString str;
    if (load)
        str = _("Load %s file");
    else
        str = _("Save %s file");
    prompt.Printf(str, what);

    wxString wild;
    const wxChar *ext = extension;
    if ( ext )
    {
        if ( *ext == wxT('.') )
            ext++;

        wild.Printf(wxT("*.%s"), ext);
    }
    else // no extension specified
    {
        wild = wxFileSelectorDefaultWildcardStr;
    }

    return wxFileSelector(prompt, NULL, default_name, ext, wild,
                          load ? wxOPEN : wxSAVE, parent);
}

//----------------------------------------------------------------------------
// wxLoadFileSelector
//----------------------------------------------------------------------------

WXDLLEXPORT wxString wxLoadFileSelector(const wxChar *what,
                                        const wxChar *extension,
                                        const wxChar *default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}

//----------------------------------------------------------------------------
// wxSaveFileSelector
//----------------------------------------------------------------------------

WXDLLEXPORT wxString wxSaveFileSelector(const wxChar *what,
                                        const wxChar *extension,
                                        const wxChar *default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}

#endif // wxUSE_FILEDLG

