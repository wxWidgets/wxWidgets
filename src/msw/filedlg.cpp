/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/filedlg.cpp
// Purpose:     wxFileDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "filedlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FILEDLG && !(defined(__SMARTPHONE__) && defined(__WXWINCE__))

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/msgdlg.h"
    #include "wx/filedlg.h"
    #include "wx/filefn.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
#endif

#include "wx/msw/wrapcdlg.h"

#include <stdlib.h>
#include <string.h>

#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "wx/math.h"

#include "wx/msw/missing.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifdef __WIN32__
# define wxMAXPATH   65534
#else
# define wxMAXPATH   1024
#endif

# define wxMAXFILE   1024

# define wxMAXEXT    5

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// standard dialog size
static wxRect gs_rectDialog(0, 0, 428, 266);

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase)

// ----------------------------------------------------------------------------
// hook function for moving the dialog
// ----------------------------------------------------------------------------

UINT_PTR APIENTRY
wxFileDialogHookFunction(HWND      hDlg,
                         UINT      iMsg,
                         WPARAM    WXUNUSED(wParam),
                         LPARAM    lParam)
{
    HWND   hwndDialog;
    hwndDialog = ::GetParent( hDlg );
    switch (iMsg)
    {
        case WM_DESTROY:
            {
                RECT dlgRect;
                GetWindowRect( hwndDialog, & dlgRect );
                gs_rectDialog.x = dlgRect.left;
                gs_rectDialog.y = dlgRect.top;
                gs_rectDialog.width = dlgRect.right - dlgRect.left;
                gs_rectDialog.height = dlgRect.bottom - dlgRect.top;
            }
            break;

        case WM_NOTIFY:
            {
                OFNOTIFY *   pNotifyCode;
                pNotifyCode = (LPOFNOTIFY) lParam;
                if (CDN_INITDONE == (pNotifyCode->hdr).code)
                {
                    SetWindowPos( hwndDialog, HWND_TOP,
                                  gs_rectDialog.x,
                                  gs_rectDialog.y,
                                  gs_rectDialog.width,
                                  gs_rectDialog.height,
                                  SWP_NOZORDER|SWP_NOSIZE);
                 }
            }
            break;
    }

    // do the default processing
    return 0;
}

// ----------------------------------------------------------------------------
// wxFileDialog
// ----------------------------------------------------------------------------

wxFileDialog::wxFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFileName,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos)
            : wxFileDialogBase(parent, message, defaultDir, defaultFileName,
                               wildCard, style, pos)

{
    if ( ( m_dialogStyle & wxMULTIPLE ) && ( m_dialogStyle & wxSAVE ) )
        m_dialogStyle &= ~wxMULTIPLE;

    m_bMovedWindow = false;

    // Must set to zero, otherwise the wx routines won't size the window
    // the second time you call the file dialog, because it thinks it is
    // already at the requested size.. (when centering)
    gs_rectDialog.x =
    gs_rectDialog.y = 0;

}
void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    paths.Empty();

    wxString dir(m_dir);
    if ( m_dir.Last() != _T('\\') )
        dir += _T('\\');

    size_t count = m_fileNames.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if (wxFileName(m_fileNames[n]).IsAbsolute())
            paths.Add(m_fileNames[n]);
        else
            paths.Add(dir + m_fileNames[n]);
    }
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    files = m_fileNames;
}

void wxFileDialog::SetPath(const wxString& path)
{
    wxString ext;
    wxSplitPath(path, &m_dir, &m_fileName, &ext);
    if ( !ext.empty() )
        m_fileName << _T('.') << ext;
}

void wxFileDialog::DoGetPosition( int *x, int *y ) const
{
    *x = gs_rectDialog.x;
    *y = gs_rectDialog.y;
}


void wxFileDialog::DoGetSize(int *width, int *height) const
{
    *width  = gs_rectDialog.width;
    *height = gs_rectDialog.height;
}

void wxFileDialog::DoMoveWindow(int x, int y, int WXUNUSED(width), int WXUNUSED(height))
{
    m_bMovedWindow = true;

    gs_rectDialog.x = x;
    gs_rectDialog.y = y;

    /*
        The width and height can not be set by the programmer
        its just not possible.  But the program can get the
        size of the Dlg after it has been shown, in case they need
        that data.
    */
}

int wxFileDialog::ShowModal()
{
    HWND hWnd = 0;
    if (m_parent) hWnd = (HWND) m_parent->GetHWND();
    if (!hWnd && wxTheApp->GetTopWindow())
        hWnd = (HWND) wxTheApp->GetTopWindow()->GetHWND();

    static wxChar fileNameBuffer [ wxMAXPATH ];           // the file-name
    wxChar        titleBuffer    [ wxMAXFILE+1+wxMAXEXT ];  // the file-name, without path

    *fileNameBuffer = wxT('\0');
    *titleBuffer    = wxT('\0');

#if WXWIN_COMPATIBILITY_2_4
    long msw_flags = 0;
    if ( (m_dialogStyle & wxHIDE_READONLY) || (m_dialogStyle & wxSAVE) )
        msw_flags |= OFN_HIDEREADONLY;
#else
    long msw_flags = OFN_HIDEREADONLY;
#endif

    if ( m_dialogStyle & wxFILE_MUST_EXIST )
        msw_flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    /*
        If the window has been moved the programmer is probably
        trying to center or position it.  Thus we set the callback
        or hook function so that we can actually adjust the position.
        Without moving or centering the dlg, it will just stay
        in the upper left of the frame, it does not center
        automatically..  One additional note, when the hook is
        enabled, the PLACES BAR in the dlg (shown on later versions
        of windows (2000 and XP) will automatically be turned off
        according to the MSDN docs.  This is normal.  If the
        programmer needs the PLACES BAR (left side of dlg) they
        just shouldn't move or center the dlg.
    */
    if (m_bMovedWindow) // we need these flags.
    {
        msw_flags |= OFN_EXPLORER|OFN_ENABLEHOOK;
#ifndef __WXWINCE__
        msw_flags |= OFN_ENABLESIZING;
#endif
    }

    if (m_dialogStyle & wxMULTIPLE )
    {
        // OFN_EXPLORER must always be specified with OFN_ALLOWMULTISELECT
        msw_flags |= OFN_EXPLORER | OFN_ALLOWMULTISELECT;
    }

    // if wxCHANGE_DIR flag is not given we shouldn't change the CWD which the
    // standard dialog does by default
    if ( !(m_dialogStyle & wxCHANGE_DIR) )
    {
        msw_flags |= OFN_NOCHANGEDIR;
    }

    if ( m_dialogStyle & wxOVERWRITE_PROMPT )
    {
        msw_flags |= OFN_OVERWRITEPROMPT;
    }

    OPENFILENAME of;
    wxZeroMemory(of);

    // the OPENFILENAME struct has been extended in newer version of
    // comcdlg32.dll, but as we don't use the extended fields anyhow, set
    // the struct size to the old value - otherwise, the programs compiled
    // with new headers will not work with the old libraries
#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500)
    of.lStructSize       = sizeof(OPENFILENAME) -
                           (sizeof(void *) + 2*sizeof(DWORD));
#else // old headers
    of.lStructSize       = sizeof(OPENFILENAME);
#endif

    of.hwndOwner         = hWnd;
    of.lpstrTitle        = WXSTRINGCAST m_message;
    of.lpstrFileTitle    = titleBuffer;
    of.nMaxFileTitle     = wxMAXFILE + 1 + wxMAXEXT;    // Windows 3.0 and 3.1

    // Convert forward slashes to backslashes (file selector doesn't like
    // forward slashes) and also squeeze multiple consecutive slashes into one
    // as it doesn't like two backslashes in a row neither

    wxString  dir;
    size_t    i, len = m_dir.length();
    dir.reserve(len);
    for ( i = 0; i < len; i++ )
    {
        wxChar ch = m_dir[i];
        switch ( ch )
        {
            case _T('/'):
                // convert to backslash
                ch = _T('\\');

                // fall through

            case _T('\\'):
                while ( i < len - 1 )
                {
                    wxChar chNext = m_dir[i + 1];
                    if ( chNext != _T('\\') && chNext != _T('/') )
                        break;

                    // ignore the next one, unless it is at the start of a UNC path
                    if (i > 0)
                        i++;
                    else
                        break;
                }
                // fall through

            default:
                // normal char
                dir += ch;
        }
    }

    of.lpstrInitialDir   = dir.c_str();

    of.Flags             = msw_flags;
    of.lpfnHook          = wxFileDialogHookFunction;

    wxArrayString wildDescriptions, wildFilters;

    size_t items = wxParseCommonDialogsFilter(m_wildCard, wildDescriptions, wildFilters);

    wxASSERT_MSG( items > 0 , _T("empty wildcard list") );

    wxString filterBuffer;

    for (i = 0; i < items ; i++)
    {
        filterBuffer += wildDescriptions[i];
        filterBuffer += wxT("|");
        filterBuffer += wildFilters[i];
        filterBuffer += wxT("|");
    }

    // Replace | with \0
    for (i = 0; i < filterBuffer.Len(); i++ ) {
        if ( filterBuffer.GetChar(i) == wxT('|') ) {
            filterBuffer[i] = wxT('\0');
        }
    }

    of.lpstrFilter  = (LPTSTR)filterBuffer.c_str();
    of.nFilterIndex = m_filterIndex + 1;

    //=== Setting defaultFileName >>=========================================

    wxStrncpy( fileNameBuffer, (const wxChar *)m_fileName, wxMAXPATH-1 );
    fileNameBuffer[ wxMAXPATH-1 ] = wxT('\0');

    of.lpstrFile = fileNameBuffer;  // holds returned filename
    of.nMaxFile  = wxMAXPATH;

    // we must set the default extension because otherwise Windows would check
    // for the existing of a wrong file with wxOVERWRITE_PROMPT (i.e. if the
    // user types "foo" and the default extension is ".bar" we should force it
    // to check for "foo.bar" existence and not "foo")
    wxString defextBuffer; // we need it to be alive until GetSaveFileName()!
    if (m_dialogStyle & wxSAVE)
    {
        const wxChar* extension = filterBuffer;
        int maxFilter = (int)(of.nFilterIndex*2L) - 1;

        for( int i = 0; i < maxFilter; i++ )           // get extension
            extension = extension + wxStrlen( extension ) + 1;

        // use dummy name a to avoid assert in AppendExtension
        defextBuffer = AppendExtension(wxT("a"), extension);
        if (defextBuffer.StartsWith(wxT("a.")))
        {
            defextBuffer.Mid(2);
            of.lpstrDefExt = defextBuffer.c_str();
        }
    }

    //== Execute FileDialog >>=================================================

    bool success = (m_dialogStyle & wxSAVE ? GetSaveFileName(&of)
                                           : GetOpenFileName(&of)) != 0;

    DWORD errCode = CommDlgExtendedError();

#ifdef __WIN32__
    if (!success && (errCode == CDERR_STRUCTSIZE))
    {
        // The struct size has changed so try a smaller or bigger size

        int oldStructSize = of.lStructSize;
        of.lStructSize       = oldStructSize - (sizeof(void *) + 2*sizeof(DWORD));
        success = (m_dialogStyle & wxSAVE) ? (GetSaveFileName(&of) != 0)
                                            : (GetOpenFileName(&of) != 0);
        errCode = CommDlgExtendedError();

        if (!success && (errCode == CDERR_STRUCTSIZE))
        {
            of.lStructSize       = oldStructSize + (sizeof(void *) + 2*sizeof(DWORD));
            success = (m_dialogStyle & wxSAVE) ? (GetSaveFileName(&of) != 0)
                                            : (GetOpenFileName(&of) != 0);
        }
    }
#endif // __WIN32__

    if ( success )
    {
        m_fileNames.Empty();

        if ( ( m_dialogStyle & wxMULTIPLE ) &&
#if defined(OFN_EXPLORER)
             ( fileNameBuffer[of.nFileOffset-1] == wxT('\0') )
#else
             ( fileNameBuffer[of.nFileOffset-1] == wxT(' ') )
#endif // OFN_EXPLORER
           )
        {
#if defined(OFN_EXPLORER)
            m_dir = fileNameBuffer;
            i = of.nFileOffset;
            m_fileName = &fileNameBuffer[i];
            m_fileNames.Add(m_fileName);
            i += m_fileName.Len() + 1;

            while (fileNameBuffer[i] != wxT('\0'))
            {
                m_fileNames.Add(&fileNameBuffer[i]);
                i += wxStrlen(&fileNameBuffer[i]) + 1;
            }
#else
            wxStringTokenizer toke(fileNameBuffer, _T(" \t\r\n"));
            m_dir = toke.GetNextToken();
            m_fileName = toke.GetNextToken();
            m_fileNames.Add(m_fileName);

            while (toke.HasMoreTokens())
                m_fileNames.Add(toke.GetNextToken());
#endif // OFN_EXPLORER

            wxString dir(m_dir);
            if ( m_dir.Last() != _T('\\') )
                dir += _T('\\');

            m_path = dir + m_fileName;
            m_filterIndex = (int)of.nFilterIndex - 1;
        }
        else
        {
            //=== Adding the correct extension >>=================================

            m_filterIndex = (int)of.nFilterIndex - 1;

            if ( !of.nFileExtension ||
                 (of.nFileExtension && fileNameBuffer[of.nFileExtension] == wxT('\0')) )
            {
                // User has typed a filename without an extension:
                const wxChar* extension = filterBuffer;
                int   maxFilter = (int)(of.nFilterIndex*2L) - 1;

                for( int i = 0; i < maxFilter; i++ )           // get extension
                    extension = extension + wxStrlen( extension ) + 1;

                m_fileName = AppendExtension(fileNameBuffer, extension);
                wxStrncpy(fileNameBuffer, m_fileName.c_str(), wxMin(m_fileName.Len(), wxMAXPATH-1));
                fileNameBuffer[wxMin(m_fileName.Len(), wxMAXPATH-1)] = wxT('\0');
            }

            m_path = fileNameBuffer;
            m_fileName = wxFileNameFromPath(fileNameBuffer);
            m_fileNames.Add(m_fileName);
            m_dir = wxPathOnly(fileNameBuffer);
        }
    }
    else
    {
        // common dialog failed - why?
#ifdef __WXDEBUG__
        DWORD dwErr = CommDlgExtendedError();
        if ( dwErr != 0 )
        {
            // this msg is only for developers
            wxLogError(wxT("Common dialog failed with error code %0lx."),
                       dwErr);
        }
        //else: it was just cancelled
#endif
    }

    return success ? wxID_OK : wxID_CANCEL;

}

#endif // wxUSE_FILEDLG && !(__SMARTPHONE__ && __WXWINCE__)

