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
    #include "wx/utils.h"
    #include "wx/msgdlg.h"
    #include "wx/dialog.h"
    #include "wx/filedlg.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
#endif

#define INCL_PM
#include <os2.h>

#include "wx/os2/private.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "wx/tokenzr.h"

#define wxMAXPATH                    1024
#define wxMAXFILE                    1024
#define wxMAXEXT                        5

#ifndef MAXPATH
# define MAXPATH                      400
#endif

#ifndef MAXDRIVE
# define MAXDRIVE                       3
#endif

#ifndef MAXFILE
# define MAXFILE                        9
#endif

#ifndef MAXEXT
# define MAXEXT                         5
#endif
IMPLEMENT_CLASS(wxFileDialog, wxDialog)

// ----------------------------------------------------------------------------
// CLASS wxFileDialog
// ----------------------------------------------------------------------------

wxFileDialog::wxFileDialog (
  wxWindow*                         pParent
, const wxString&                   rsMessage
, const wxString&                   rsDefaultDir
, const wxString&                   rsDefaultFileName
, const wxString&                   rsWildCard
, long                              lStyle
, const wxPoint&                    rPos
)
{
    m_sMessage     = rsMessage;
    m_lDialogStyle = lStyle;
    if ((m_lDialogStyle & wxMULTIPLE) && (m_lDialogStyle & wxSAVE))
        m_lDialogStyle &= ~wxMULTIPLE;
    m_pParent      = pParent;
    m_sPath        = "";
    m_sFileName    = rsDefaultFileName;
    m_sDir         = rsDefaultDir;
    m_sWildCard    = rsWildCard;
    m_nFilterIndex = 1;
    m_vPos         = rPos;
} // end of wxFileDialog::wxFileDialog

void wxFileDialog::GetPaths (
  wxArrayString&                    rasPaths
) const
{
    wxString                        sDir(m_sDir);
    size_t                          nCount = m_asFileNames.GetCount();

    rasPaths.Empty();
    if (m_sDir.Last() != _T('\\'))
        sDir += _T('\\');

    for ( size_t n = 0; n < nCount; n++ )
    {
        rasPaths.Add(sDir + m_asFileNames[n]);
    }
} // end of wxFileDialog::GetPaths

int wxFileDialog::ShowModal()
{
    wxString                        sTheFilter;
    wxString                        sFilterBuffer;
    wxChar*                         pzFilterBuffer;
    static wxChar                   zFileNameBuffer[wxMAXPATH];           // the file-name
    HWND                            hWnd = 0;
    wxChar                          zTitleBuffer[wxMAXFILE + 1 + wxMAXEXT];  // the file-name, without path
    wxString                        sDir;
    size_t                          i;
    size_t                          nLen = m_sDir.length();
    int                             nCount = 0;
    FILEDLG                         vFileDlg;
    ULONG                           lFlags = 0L;

    memset(&vFileDlg, '\0', sizeof(FILEDLG));
    if (m_pParent)
        hWnd = (HWND) m_pParent->GetHWND();
    if (!hWnd && wxTheApp->GetTopWindow())
        hWnd = (HWND) wxTheApp->GetTopWindow()->GetHWND();


    *zFileNameBuffer = wxT('\0');
    *zTitleBuffer    = wxT('\0');

    if (m_lDialogStyle & wxSAVE)
        lFlags = FDS_SAVEAS_DIALOG;
    else
        lFlags = FDS_OPEN_DIALOG;

    if ((m_lDialogStyle & wxHIDE_READONLY) || (m_lDialogStyle & wxSAVE))
        lFlags |= FDS_SAVEAS_DIALOG;
    if (m_lDialogStyle & wxMULTIPLE )
        lFlags |= FDS_OPEN_DIALOG | FDS_MULTIPLESEL;

    vFileDlg.cbSize = sizeof(FILEDLG);
    vFileDlg.fl = lFlags;
    vFileDlg.pszTitle = zTitleBuffer;

    //
    // Convert forward slashes to backslashes (file selector doesn't like
    // forward slashes) and also squeeze multiple consecutive slashes into one
    // as it doesn't like two backslashes in a row neither
    //
    sDir.reserve(nLen);
    for ( i = 0; i < nLen; i++ )
    {
        wxChar                      ch = m_sDir[i];

        switch (ch)
        {
            case _T('/'):
                //
                // Convert to backslash
                //
                ch = _T('\\');

                //
                // Fall through
                //
            case _T('\\'):
                while (i < nLen - 1)
                {
                    wxChar          chNext = m_sDir[i + 1];

                    if (chNext != _T('\\') && chNext != _T('/'))
                        break;

                    //
                    // Ignore the next one, unless it is at the start of a UNC path
                    //
                    if (i > 0)
                        i++;
                    else
                        break;
                }

                //
                // Fall through
                //

            default:
                //
                // Normal char
                sDir += ch;
        }
    }
    if ( wxStrlen(m_sWildCard) == 0 )
        sTheFilter = "";
    else
        sTheFilter = m_sWildCard;

    pzFilterBuffer = strtok((char*)sTheFilter.c_str(), "|");
    while(pzFilterBuffer != NULL)
    {
        if (nCount > 0 && !(nCount % 2))
            sDir += wxT(";");
        if (nCount % 2)
        {
            sDir += pzFilterBuffer;
        }
        pzFilterBuffer = strtok(NULL, "|");
        nCount++;
    }
    if (nCount == 0)
        sDir += m_sFileName;
    if (sDir.IsEmpty())
        sDir = "*.*";
    wxStrcpy(vFileDlg.szFullFile, sDir.c_str());
    sFilterBuffer = sDir;

    hWnd = ::WinFileDlg( HWND_DESKTOP
                        ,GetHwndOf(m_pParent)
                        ,&vFileDlg
                       );
    if (hWnd && vFileDlg.lReturn == DID_OK)
    {
        m_asFileNames.Empty();
        if ((m_lDialogStyle & wxMULTIPLE ) && vFileDlg.ulFQFCount > 1)
        {
            for (int i = 0; i < vFileDlg.ulFQFCount; i++)
            {
                if (i == 0)
                {
                    m_sDir = wxPathOnly(wxString((const char*)*vFileDlg.papszFQFilename[0]));
                    m_sPath = (const char*)*vFileDlg.papszFQFilename[0];
                }
                m_sFileName = wxFileNameFromPath(wxString((const char*)*vFileDlg.papszFQFilename[i]));
                m_asFileNames.Add(m_sFileName);
            }
            ::WinFreeFileDlgList(vFileDlg.papszFQFilename);
        }
        else if (!(m_lDialogStyle & wxSAVE))
        {
            m_sPath = vFileDlg.szFullFile;
            m_sFileName = wxFileNameFromPath(vFileDlg.szFullFile);
            m_sDir = wxPathOnly(vFileDlg.szFullFile);
        }
        else // save file
        {
            const wxChar*           pzExtension = NULL;

            wxStrcpy(zFileNameBuffer, vFileDlg.szFullFile);

            int                     nIdx = wxStrlen(zFileNameBuffer) - 1;
            wxString                sExt;

            wxSplitPath( zFileNameBuffer
                        ,&m_sPath
                        ,&m_sFileName
                        ,&sExt
                       );
            if (zFileNameBuffer[nIdx] == wxT('.') || sExt.IsEmpty())
            {
                zFileNameBuffer[nIdx] = wxT('\0');

                //
                // User has typed a filename without an extension:
                //
                // A filename can end in a "." here ("abc."), this means it
                // does not have an extension. Because later on a "." with
                // the default extension is appended we remove the "." if
                // filename ends with one (We don't want files called
                // "abc..ext")
                //
                pzExtension = sFilterBuffer.c_str();

                for( int i = 0; i < sFilterBuffer.length(); i++ )
                {
                    //
                    // Get extension
                    //
                    pzExtension = wxStrrchr(pzExtension, wxT('.'));
                    if ( pzExtension                      &&
                        !wxStrrchr(pzExtension, wxT('*')) &&
                        !wxStrrchr(pzExtension, wxT('?')) &&
                        pzExtension[1]                    &&
                        pzExtension[1] != wxT(' ')
                       )              // != "blabla. "
                    {
                        //
                        // Now concat extension to the fileName:
                        //
                        m_sPath = wxString(zFileNameBuffer) + pzExtension;
                    }
                }
            }
            else
            {
                m_sPath = vFileDlg.szFullFile;
            }
            m_sFileName = wxFileNameFromPath(vFileDlg.szFullFile);
            m_sDir = wxPathOnly(vFileDlg.szFullFile);

            //
            // === Simulating the wxOVERWRITE_PROMPT >>============================
            //
            if ((m_lDialogStyle & wxOVERWRITE_PROMPT) &&
                (m_lDialogStyle & wxSAVE) &&
                (wxFileExists(m_sPath.c_str())))
            {
                wxString            sMessageText;

                sMessageText.Printf( _("File '%s' already exists.\nDo you want to replace it?")
                                    ,m_sPath.c_str()
                                   );
                if (wxMessageBox( sMessageText
                                 ,wxT("Save File As")
                                 ,wxYES_NO | wxICON_EXCLAMATION
                                ) != wxYES)
                {
                    return wxID_CANCEL;
                }
            }
        }
        return wxID_OK;
    }
    return wxID_CANCEL;
} // end of wxFileDialog::ShowModal

