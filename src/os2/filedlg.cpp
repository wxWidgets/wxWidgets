/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:     wxFileDialog
// Author:      David Webster
// Modified by:
// Created:     10/05/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/defs.h"
    #include "wx/utils.h"
    #include "wx/msgdlg.h"
    #include "wx/dialog.h"
    #include "wx/filedlg.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#define INCL_PM
#include <os2.h>

#include "wx/os2/private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

IMPLEMENT_CLASS(wxFileDialog, wxDialog)

wxString wxFileSelector( const char* title
                        ,const char* defaultDir
                        ,const char* defaultFileName
                        ,const char* defaultExtension
                        ,const char* filter
                        ,int         flags
                        ,wxWindow*   parent
                        ,int         x
                        ,int         y
                       )
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
        return fileDialog.GetPath();
    }
    else
        return wxEmptyString;
}

# ifndef MAXPATH
# define MAXPATH   400
# endif

# ifndef MAXDRIVE
# define MAXDRIVE  3
# endif

# ifndef MAXFILE
# define MAXFILE   9
# endif

# ifndef MAXEXT
# define MAXEXT    5
# endif

wxString wxFileSelectorEx( const char* title
                          ,const char* defaultDir
                          ,const char* defaultFileName
                          ,int*        defaultFilterIndex
                          ,const char* filter
                          ,int         flags
                          ,wxWindow*   parent
                          ,int         x
                          ,int         y
                         )
{
    wxFileDialog fileDialog(parent, title ? title : "", defaultDir ? defaultDir : "",
        defaultFileName ? defaultFileName : "", filter ? filter : "", flags, wxPoint(x, y));

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        *defaultFilterIndex = fileDialog.GetFilterIndex();
        return fileDialog.GetPath();
    }
    else
        return wxEmptyString;
}

wxFileDialog::wxFileDialog( wxWindow*       parent
                           ,const wxString& message
                           ,const wxString& defaultDir
                           ,const wxString& defaultFileName
                           ,const wxString& wildCard
                           ,long            style
                           ,const wxPoint&  pos
                          )
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
    return wxID_CANCEL;
}

// Generic file load/save dialog
static wxString wxDefaultFileSelector( bool        load
                                      ,const char* what
                                      ,const char* extension
                                      ,const char* default_name
                                      ,wxWindow*   parent
                                     )
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
wxString wxLoadFileSelector( const char* what
                            ,const char* extension
                            ,const char* default_name
                            ,wxWindow*   parent
                           )
{
  return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}


// Generic file save dialog
wxString wxSaveFileSelector( const char* what
                            ,const char* extension
                            ,const char* default_name
                            ,wxWindow*   parent
                           )
{
  return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}


