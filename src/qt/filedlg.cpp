/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:     wxFileDialog
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "filedlg.h"
#endif

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/filedlg.h"

IMPLEMENT_CLASS(wxFileDialog, wxDialog)

char *wxFileSelector(const char *title,
                     const char *defaultDir, const char *defaultFileName,
                     const char *defaultExtension, const char *filter, int flags,
                     wxWindow *parent, int x, int y)
{
    // If there's a default extension specified but no filter, we create a suitable
    // filter.

    wxString filter2("");
    if ( defaultExtension && !filter )
        filter2 = wxString("*.") + wxString(defaultExtension) ;
    else if ( filter )
        filter2 = filter;

    wxString defaultDirString;
    if (defaultDir)
        defaultDirString = defaultDir;
    else
        defaultDirString = "";

    wxString defaultFilenameString;
    if (defaultFileName)
        defaultFilenameString = defaultFileName;
    else
        defaultFilenameString = "";

    wxFileDialog fileDialog(parent, title, defaultDirString, defaultFilenameString, filter2, flags, wxPoint(x, y));

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        strcpy(wxBuffer, (const char *)fileDialog.GetPath());
        return wxBuffer;
    }
    else
        return NULL;
}

char *wxFileSelectorEx(const char *title,
                       const char *defaultDir,
                       const char *defaultFileName,
                       int* defaultFilterIndex,
                       const char *filter,
                       int       flags,
                       wxWindow* parent,
                       int       x,
                       int       y)

{
    wxFileDialog fileDialog(parent, title ? title : "", defaultDir ? defaultDir : "",
        defaultFileName ? defaultFileName : "", filter ? filter : "", flags, wxPoint(x, y));

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        *defaultFilterIndex = fileDialog.GetFilterIndex();
        strcpy(wxBuffer, (const char *)fileDialog.GetPath());
        return wxBuffer;
    }
    else
        return NULL;
}

wxFileDialog::wxFileDialog(wxWindow *parent, const wxString& message,
        const wxString& defaultDir, const wxString& defaultFileName, const wxString& wildCard,
        long style, const wxPoint& pos)
{
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
    m_path = "";
    m_fileName = defaultFileName;
    m_dir = defaultDir;
    m_wildCard = wildCard;
    m_filterIndex = 1;
}

int wxFileDialog::ShowModal()
{
    // TODO
    wxID_CANCEL;
}

// Generic file load/save dialog
static char *
wxDefaultFileSelector(bool load, const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  char *ext = (char *)extension;
  
  char prompt[50];
  wxString str;
  if (load)
    str = "Load %s file";
  else
    str = "Save %s file";
  sprintf(prompt, wxGetTranslation(str), what);

  if (*ext == '.') ext++;
  char wild[60];
  sprintf(wild, "*.%s", ext);

  return wxFileSelector (prompt, NULL, default_name, ext, wild, 0, parent);
}

// Generic file load dialog
char *
wxLoadFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}


// Generic file save dialog
char *
wxSaveFileSelector(const char *what, const char *extension, const char *default_name, wxWindow *parent)
{
  return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}


