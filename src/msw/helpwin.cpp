/////////////////////////////////////////////////////////////////////////////
// Name:        helpwin.cpp
// Purpose:     Help system: WinHelp implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "helpwin.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#if wxUSE_HELP

#include "wx/filefn.h"
#include "wx/msw/helpwin.h"

#include <time.h>

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#include <string.h>

static HWND GetSuitableHWND()
{
    if (wxTheApp->GetTopWindow())
        return (HWND) wxTheApp->GetTopWindow()->GetHWND();
    else
        return GetDesktopWindow();
}

IMPLEMENT_DYNAMIC_CLASS(wxWinHelpController, wxHelpControllerBase)

bool wxWinHelpController::Initialize(const wxString& filename)
{
    m_helpFile = filename;
    return TRUE;
}

bool wxWinHelpController::LoadFile(const wxString& file)
{
    if (!file.IsEmpty())
        m_helpFile = file;
    return TRUE;
}

bool wxWinHelpController::DisplayContents(void)
{
    if (m_helpFile.IsEmpty()) return FALSE;
    
    wxString str = GetValidFilename(m_helpFile);
    
#if defined(__WIN95__)
    WinHelp(GetSuitableHWND(), (const wxChar*) str, HELP_FINDER, 0L);
#else
    WinHelp(GetSuitableHWND(), (const wxChar*) str, HELP_CONTENTS, 0L);
#endif
    return TRUE;
}

bool wxWinHelpController::DisplaySection(int section)
{
    // Use context number
    if (m_helpFile.IsEmpty()) return FALSE;
    
    wxString str = GetValidFilename(m_helpFile);

    WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_CONTEXT, (DWORD)section);
    return TRUE;
}

bool wxWinHelpController::DisplayBlock(long block)
{
    DisplaySection(block);
    return TRUE;
}

bool wxWinHelpController::KeywordSearch(const wxString& k)
{
    if (m_helpFile.IsEmpty()) return FALSE;
    
    wxString str = GetValidFilename(m_helpFile);
    
    WinHelp(GetSuitableHWND(), (const wxChar*) str, HELP_PARTIALKEY, (DWORD)(const wxChar*) k);
    return TRUE;
}

// Can't close the help window explicitly in WinHelp
bool wxWinHelpController::Quit(void)
{
    WinHelp(GetSuitableHWND(), 0, HELP_QUIT, 0L);
    return TRUE;
}

// Append extension if necessary.
wxString wxWinHelpController::GetValidFilename(const wxString& file) const
{
    wxString path, name, ext;
    wxSplitPath(file, & path, & name, & ext);

    wxString fullName;
    if (path.IsEmpty())
        fullName = name + wxT(".hlp");
    else if (path.Last() == wxT('\\'))
        fullName = path + name + wxT(".hlp");
    else
        fullName = path + wxT("\\") + name + wxT(".hlp");
    return fullName;
}

#endif // wxUSE_HELP
