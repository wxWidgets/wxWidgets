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
    if( wxStrlen(defaultExtension) != 0 )
    {
        int filterFind = 0,
            filterIndex = 0;

        for( unsigned int i = 0; i < filter2.Len(); i++ )
        {
            if( filter2.GetChar(i) == wxT('|') )
            {
                // save the start index of the new filter
                unsigned int is = i++;

                // find the end of the filter
                for( ; i < filter2.Len(); i++ )
                {
                    if(filter2[i] == wxT('|'))
                        break;
                }

                if( i-is-1 > 0 && is+1 < filter2.Len() )
                {
                    if( filter2.Mid(is+1,i-is-1).Contains(defaultExtension) )
                    {
                        filterFind = filterIndex;
                        break;
                    }
                }

                filterIndex++;
            }
        }

        fileDialog.SetFilterIndex(filterFind);
    }

    wxString filename;
    if ( fileDialog.ShowModal() == wxID_OK )
    {
        filename = fileDialog.GetPath();
    }

    return filename;
}


/*
wxString wxFileSelector( const wxChar *title,
                         const wxChar *defaultDir,
                         const wxChar *defaultFileName,
                         const wxChar *defaultExtension,
                         const wxChar *filter,
                         int flags,
                         wxWindow *parent,
                         int x,
                         int y )
{
    wxString filter2;
    if ( defaultExtension && !filter )
        filter2 = wxString(wxT("*.")) + wxString(defaultExtension) ;
    else if ( filter )
        filter2 = filter;

    wxString defaultDirString;
    if (defaultDir)
        defaultDirString = defaultDir;

    wxString defaultFilenameString;
    if (defaultFileName)
        defaultFilenameString = defaultFileName;

    wxFileDialog fileDialog( parent, title, defaultDirString, defaultFilenameString, filter2, flags, wxPoint(x, y) );

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        return fileDialog.GetPath();
    }
    else
    {
        return wxEmptyString;
    }
}
*/


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



// Generic file load/save dialog (for internal use only)
//   see wx[Load/Save]FileSelector
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

// Generic file load dialog
WXDLLEXPORT wxString wxLoadFileSelector(const wxChar *what,
                                        const wxChar *extension,
                                        const wxChar *default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}

// Generic file save dialog
WXDLLEXPORT wxString wxSaveFileSelector(const wxChar *what,
                                        const wxChar *extension,
                                        const wxChar *default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}


// Parses the filterStr, returning the number of filters.
// Returns 0 if none or if there's a problem.
// filterStr is in the form: "All files (*.*)|*.*|JPEG Files (*.jpeg)|*.jpg"

int wxParseFileFilter(const wxString& filterStr,
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
                descriptions.Add(wxEmptyString);
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

#endif // wxUSE_FILEDLG

