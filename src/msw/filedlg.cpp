/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.cpp
// Purpose:     wxFileDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "filedlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/filedlg.h"
#endif

#include <windows.h>

#ifndef __WIN32__
#include <commdlg.h>
#endif

#include "wx/msw/private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxFileDialog, wxDialog)
#endif

char *wxFileSelector(const char *title,
                     const char *defaultDir, const char *defaultFileName,
                     const char *defaultExtension, const char *filter, int flags,
                     wxWindow *parent, int x, int y)
{
    // In the original implementation, defaultExtension is passed to the lpstrDefExt member
    // of OPENFILENAME. This extension, if non-NULL, is appended to the filename if the user
    // fails to type an extension.
    // The new implementation (taken from wxFileSelectorEx) appends the extension automatically,
    // by looking at the filter specification. In fact this should be better than the
    // native Microsoft implementation because Windows only allows *one* default extension,
    // whereas here we do the right thing depending on the filter the user has chosen.

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

# if __BORLANDC__
#   include <dir.h>  // for MAXPATH etc. ( Borland 3.1 ) 
# endif

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

int wxFileDialog::ShowModal(void)
{
  HWND hWnd = 0;
  if (m_parent) hWnd = (HWND) m_parent->GetHWND();

    static char  fileNameBuffer [ MAXPATH ];           // the file-name
    char         titleBuffer    [ MAXFILE+1+MAXEXT ];  // the file-name, without path

    *fileNameBuffer = '\0';
    *titleBuffer    = '\0';

    char* filterBuffer = NULL;
    char* extension    = NULL;
    char* theFilter    = (char *)(const char *)m_wildCard;

    long msw_flags = 0;
    if ( (m_dialogStyle & wxHIDE_READONLY) || (m_dialogStyle & wxSAVE) )
      msw_flags |= OFN_HIDEREADONLY;
    if ( m_dialogStyle & wxFILE_MUST_EXIST )
      msw_flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    OPENFILENAME of;
    memset(&of, 0, sizeof(OPENFILENAME));

    of.lpstrCustomFilter = NULL;   // system should not save custom filter
    of.nMaxCustFilter    = 0L;

    of.nFileOffset       = 0;      // 0-based pointer to filname in lpstFile
    of.nFileExtension    = 0;      // 0-based pointer to extension in lpstrFile
    of.lpstrDefExt       = NULL;   // no default extension

    of.lStructSize       = sizeof(OPENFILENAME);
    of.hwndOwner         = hWnd;
    of.lpstrTitle        = (char *)(const char *)m_message;


    of.lpstrFileTitle    = titleBuffer;
    of.nMaxFileTitle     = MAXFILE + 1 + MAXEXT;    // Windows 3.0 and 3.1

    of.lpstrInitialDir   = (const char *) m_dir;

    of.Flags             = msw_flags;



    //=== Like Alejandro Sierra's wildcard modification >>===================
    /*
        In wxFileSelector you can put, instead of a single wild_card,
        pairs of strings separated by '|'.
        The first string is a description, and the
        second is the wild card. You can put any number of pairs.

        eg.  "description1 (*.ex1)|*.ex1|description2 (*.ex2)|*.ex2"

        If you put a single wild card, it works as before the modification.
    */
    //=======================================================================

    if ( !theFilter || (strcmp(theFilter, "") == 0)) theFilter = "*.*";

    int filterBufferLen = 0;

    if ( !strchr( theFilter, '|' ) ) {          // only one filter ==> default text:
        char buffText[] = "Files (%s)|%s";
        filterBufferLen = strlen( theFilter )*2 + strlen( buffText ) -4;
        filterBuffer    = new char[ filterBufferLen +2 ];

        if ( filterBuffer ) {
            sprintf( filterBuffer, buffText, theFilter, theFilter );
        }
    }
    else {                                      // more then one filter
        filterBufferLen = strlen( theFilter );
        filterBuffer    = new char[ filterBufferLen +2 ];

        if ( filterBuffer ) {
            strcpy( filterBuffer, theFilter );
        }
    }

    if ( filterBuffer ) {                       // Substituting '|' with '\0'
        for ( int i = 0; i < filterBufferLen; i++ ) {
            if ( filterBuffer[i] == '|' ) { filterBuffer[i] = '\0'; }
        }
    }

    filterBuffer[filterBufferLen+1] = '\0';

    of.lpstrFilter  = (LPSTR)filterBuffer;
    of.nFilterIndex = m_filterIndex;

    //=== Setting defaultFileName >>=========================================

    strncpy( fileNameBuffer, (const char *)m_fileName, MAXPATH-1 );
    fileNameBuffer[ MAXPATH-1 ] = '\0';

    of.lpstrFile = fileNameBuffer;  // holds returned filename
    of.nMaxFile  = MAXPATH;

    //== Execute FileDialog >>=================================================

    bool success = (m_dialogStyle & wxSAVE) ? (GetSaveFileName(&of) != 0) : (GetOpenFileName(&of) != 0);

    if ( success )
    {
        //=== Adding the correct extension >>=================================

        m_filterIndex = (int)of.nFilterIndex;

        if ( of.nFileExtension && fileNameBuffer[ of.nFileExtension-1] != '.' )
        {                                    // user has typed an filename
                                             // without an extension:

            int   maxFilter = (int)(of.nFilterIndex*2L-1L);
            extension       = filterBuffer;

            for( int i = 0; i < maxFilter; i++ ) {          // get extension
                extension = extension + strlen( extension ) +1;
            }

            if (  (extension = strrchr( extension, '.' ))   // != "blabla" 
                  && !strrchr( extension, '*' )             // != "blabla.*"
                  && !strrchr( extension, '?' )             // != "blabla.?"
                  && extension[1]                           // != "blabla."
                  && extension[1] != ' ' )                  // != "blabla. "
            {
                              // now concat extension to the fileName:
                m_fileName = wxString(fileNameBuffer) + wxString(extension);

                int len = strlen( fileNameBuffer );
                strncpy( fileNameBuffer + len, extension, MAXPATH - len );
                fileNameBuffer[ MAXPATH -1 ] = '\0';
            }
        }

        m_path = fileNameBuffer;
        m_fileName = wxFileNameFromPath(fileNameBuffer);


        //=== Simulating the wxOVERWRITE_PROMPT >>============================

        if ( (m_dialogStyle & wxOVERWRITE_PROMPT) && ::wxFileExists( fileNameBuffer ) )
        {
            char  questionText[] = "Replace file\n%s?";
            char* messageText    = new char[strlen(questionText)+strlen(fileNameBuffer)-1];
            sprintf( messageText, questionText, fileNameBuffer );

            if ( messageText && ( wxMessageBox( (const char *)messageText, m_message, wxYES_NO ) != wxYES ) )
            {
                success = FALSE;
            }

            delete[] messageText;
        }

    } // END: if ( success )


    delete[] filterBuffer;

    return (success ? wxID_OK : wxID_CANCEL) ;

}

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

// Generic file load/save dialog
// static inline char * // HP compiler complains
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


