/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dirdlg.h"
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
#include "wx/dirdlg.h"
#endif

#if defined(__WIN95__) && !defined(__GNUWIN32__)
#include "shlobj.h" // Win95 shell
#endif

#include "wx/msw/private.h"
#include "wx/cmndata.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

#if !USE_SHARED_LIBRARY
IMPLEMENT_CLASS(wxDirDialog, wxDialog)
#endif

wxDirDialog::wxDirDialog(wxWindow *parent, const wxString& message,
//		const wxString& caption,
        const wxString& defaultPath,
        long style, const wxPoint& pos)
{
    m_message = message;
//    m_caption = caption;
    m_dialogStyle = style;
    m_parent = parent;
	m_path = defaultPath;
}

int wxDirDialog::ShowModal(void)
{
    // Unfortunately Gnu-Win32 doesn't yet have COM support
#if defined(__WIN95__) && !defined(__GNUWIN32__)
  HWND hWnd = 0;
  if (m_parent) hWnd = (HWND) m_parent->GetHWND();

    BROWSEINFO bi;
    LPTSTR lpBuffer;
//    LPITEMIDLIST pidlPrograms;  // PIDL for Programs folder 
    LPITEMIDLIST pidlBrowse;    // PIDL selected by user 
    LPMALLOC pMalloc = NULL;

    HRESULT result = ::SHGetMalloc(&pMalloc);

    if (result != NOERROR)
      return wxID_CANCEL;
    
    // Allocate a buffer to receive browse information. 
    if ((lpBuffer = (LPTSTR) pMalloc->Alloc(MAX_PATH)) == NULL) 
    {
        pMalloc->Release();
        return wxID_CANCEL;
    }

/*
    // Get the PIDL for the Programs folder. 
    if (!SUCCEEDED(SHGetSpecialFolderLocation( 
            parent->GetSafeHwnd(), CSIDL_PROGRAMS, &pidlPrograms))) { 
        pMalloc->Free(lpBuffer);
        pMalloc->Release();
        return wxID_CANCEL;
    } 
*/
 
    // Fill in the BROWSEINFO structure. 
    bi.hwndOwner = hWnd;
    bi.pidlRoot = NULL; // pidlPrograms; 
    bi.pszDisplayName = lpBuffer; 
    bi.lpszTitle = m_message; // BC++ 4.52 says LPSTR, not LPTSTR?
    bi.ulFlags = 0; 
    bi.lpfn = NULL; 
    bi.lParam = 0; 
 
    // Browse for a folder and return its PIDL. 
    pidlBrowse = SHBrowseForFolder(&bi); 

    int id = wxID_OK;
    if (pidlBrowse != NULL) {
 
        // Show the display name, title, and file system path. 
        if (SHGetPathFromIDList(pidlBrowse, lpBuffer))
          m_path = lpBuffer;
          
        // Free the PIDL returned by SHBrowseForFolder. 
        pMalloc->Free(pidlBrowse);
    }
	else
		id = wxID_CANCEL;
 
    // Clean up. 
//    pMalloc->Free(pidlPrograms); 
    pMalloc->Free(lpBuffer); 
    pMalloc->Release();
    
    return id;
#else
	return wxID_CANCEL;
#endif
}

