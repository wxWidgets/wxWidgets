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
    #include "wx/msgdlg.h"
    #include "wx/dialog.h"
    #include "wx/filedlg.h"
    #include "wx/intl.h"
#endif

#include <windows.h>

#if !defined(__WIN32__) || defined(__SALFORDC__)
    #include <commdlg.h>
#endif

#include "wx/msw/private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#if !USE_SHARED_LIBRARY
    IMPLEMENT_CLASS(wxFileDialog, wxDialog)
#endif

wxString wxFileSelector(const char *title,
                        const char *defaultDir,
                        const char *defaultFileName,
                        const char *defaultExtension,
                        const char *filter,
                        int flags,
                        wxWindow *parent,
                        int x, int y)
{
    // In the original implementation, defaultExtension is passed to the
    // lpstrDefExt member of OPENFILENAME. This extension, if non-NULL, is
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
        filter2 = wxString("*.") + defaultExtension;
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
    if( Strlen(defaultExtension) != 0 )
    {
        int filterFind = 1,
            filterIndex = 0;

        for( unsigned int i = 0; i < filter2.Len(); i++ )
        {
            if( filter2.GetChar(i) == '|' )
            {
                // save the start index of the new filter
                unsigned int is = i++;
                filterIndex++;

                // find the end of the filter
                for( ; i < filter2.Len(); i++ )
                {
                    if(filter2[i] == '|')
                        break;
                }

                if( i-is-1 > 0 && is+1 < filter2.Len() )
                {
                    if( filter2.Mid(is+1,i-is-1).Contains(defaultExtension) )
//                    if( filter2.Mid(is+1,i-is-1) == defaultExtension )
                    {
                        filterFind = filterIndex;
                        break;
                    }
                }
            }
        }

        fileDialog.SetFilterIndex(filterFind);
    }

    if ( fileDialog.ShowModal() == wxID_OK )
    {
        strcpy(wxBuffer, (const char *)fileDialog.GetPath());
        return wxBuffer;
    }
    else
        return wxGetEmptyString();
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


wxString wxFileSelectorEx(const char *title,
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
        return wxGetEmptyString();
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

    wxString theFilter = ( Strlen(m_wildCard) == 0 ) ? wxString("*.*") : m_wildCard;
    wxString filterBuffer;

    if ( !strchr( theFilter, '|' ) ) {    // only one filter ==> default text
        filterBuffer.Printf(_("Files (%s)|%s"),
                            theFilter.c_str(), theFilter.c_str());
    }
    else {                                // more then one filter
        filterBuffer = theFilter;

    }

    filterBuffer += "|";
    // Replace | with \0
    for ( unsigned int i = 0; i < filterBuffer.Len(); i++ ) {
        if ( filterBuffer.GetChar(i) == '|' ) {
            filterBuffer[i] = '\0';
        }
    }

    of.lpstrFilter  = (LPSTR)(const char *)filterBuffer;
    of.nFilterIndex = m_filterIndex;

    //=== Setting defaultFileName >>=========================================

    strncpy( fileNameBuffer, (const char *)m_fileName, MAXPATH-1 );
    fileNameBuffer[ MAXPATH-1 ] = '\0';

    of.lpstrFile = fileNameBuffer;  // holds returned filename
    of.nMaxFile  = MAXPATH;

    //== Execute FileDialog >>=================================================

    bool success = (m_dialogStyle & wxSAVE) ? (GetSaveFileName(&of) != 0)
                                            : (GetOpenFileName(&of) != 0);

    if ( success )
    {
        const char* extension = NULL;

        //=== Adding the correct extension >>=================================

        m_filterIndex = (int)of.nFilterIndex;

        if ( of.nFileExtension && fileNameBuffer[ of.nFileExtension-1] != '.' )
        {                                    // user has typed an filename
            // without an extension:

            int   maxFilter = (int)(of.nFilterIndex*2L-1L);
            extension = filterBuffer;

            for( int i = 0; i < maxFilter; i++ ) {          // get extension
                extension = extension + strlen( extension ) +1;
            }

            extension = strrchr( extension, '.' );
            if (  extension                                 // != "blabla"
                    && !strrchr( extension, '*' )             // != "blabla.*"
                    && !strrchr( extension, '?' )             // != "blabla.?"
                    && extension[1]                           // != "blabla."
                    && extension[1] != ' ' )                  // != "blabla. "
            {
                // now concat extension to the fileName:
                m_fileName = wxString(fileNameBuffer) + extension;

                int len = strlen( fileNameBuffer );
                strncpy( fileNameBuffer + len, extension, MAXPATH - len );
                fileNameBuffer[ MAXPATH -1 ] = '\0';
            }
        }

        m_path = fileNameBuffer;
        m_fileName = wxFileNameFromPath(fileNameBuffer);


        //=== Simulating the wxOVERWRITE_PROMPT >>============================

        if ( (m_dialogStyle & wxOVERWRITE_PROMPT) &&
             ::wxFileExists( fileNameBuffer ) )
        {
            wxString messageText;
            messageText.Printf(_("Replace file '%s'?"), fileNameBuffer);

            if ( wxMessageBox(messageText, m_message, wxYES_NO ) != wxYES )
            {
                success = FALSE;
            }
        }

    } // END: if ( success )

    return (success ? wxID_OK : wxID_CANCEL) ;

}

// Generic file load/save dialog (for internal use only)
static
wxString wxDefaultFileSelector(bool load,
                               const char *what,
                               const char *extension,
                               const char *default_name,
                               wxWindow *parent)
{
  wxString prompt;
  wxString str = load ? _("Load %s file") : _("Save %s file");
  prompt.Printf(str, what);

  const char *ext = extension;
  if (*ext == '.')
      ext++;

  wxString wild;
  wild.Printf("*.%s", ext);

  return wxFileSelector (prompt, NULL, default_name, ext, wild, 0, parent);
}

// Generic file load dialog
WXDLLEXPORT wxString wxLoadFileSelector(const char *what,
                                        const char *extension,
                                        const char *default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(TRUE, what, extension, default_name, parent);
}

// Generic file save dialog
WXDLLEXPORT wxString wxSaveFileSelector(const char *what,
                                        const char *extension,
                                        const char *default_name,
                                        wxWindow *parent)
{
    return wxDefaultFileSelector(FALSE, what, extension, default_name, parent);
}

