/////////////////////////////////////////////////////////////////////////////
// Name:        helpchm.cpp
// Purpose:     Help system: MS HTML Help implementation
// Author:      Julian Smart
// Modified by:
// Created:     16/04/2000
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "helpchm.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/filefn.h"

#if wxUSE_HELP && wxUSE_MS_HTML_HELP && defined(__WIN95__)
#include "wx/msw/helpchm.h"

// This is found in the HTML Help Workshop installation,
// along with htmlhelp.lib.
#include <htmlhelp.h>

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

IMPLEMENT_DYNAMIC_CLASS(wxCHMHelpController, wxHelpControllerBase)

bool wxCHMHelpController::Initialize(const wxString& filename)
{
    m_helpFile = filename;
    return TRUE;
}

bool wxCHMHelpController::LoadFile(const wxString& file)
{
    m_helpFile = file;
    return TRUE;
}

bool wxCHMHelpController::DisplayContents()
{
    if (m_helpFile.IsEmpty()) return FALSE;

    wxString str = GetValidFilename(m_helpFile);

    HtmlHelp(GetSuitableHWND(), (const wxChar*) str, HH_HELP_FINDER, 0L);
    return TRUE;
}

// Use topic or HTML filename
bool wxCHMHelpController::DisplaySection(const wxString& section)
{
    if (m_helpFile.IsEmpty()) return FALSE;

    wxString str = GetValidFilename(m_helpFile);

    // Is this an HTML file or a keyword?
    bool isFilename = (section.Find(wxT(".htm")) != -1);

    if (isFilename)
        HtmlHelp(GetSuitableHWND(), (const wxChar*) str, HH_DISPLAY_TOPIC, (DWORD) (const wxChar*) section);
    else
        KeywordSearch(section);
    return TRUE;
}

// Use context number
bool wxCHMHelpController::DisplaySection(int section)
{
    if (m_helpFile.IsEmpty()) return FALSE;

    wxString str = GetValidFilename(m_helpFile);

    HtmlHelp(GetSuitableHWND(), (const wxChar*) str, HH_HELP_CONTEXT, (DWORD)section);
    return TRUE;
}

bool wxCHMHelpController::DisplayBlock(long block)
{
    return DisplaySection(block);
}

bool wxCHMHelpController::KeywordSearch(const wxString& k)
{
    if (m_helpFile.IsEmpty()) return FALSE;

    wxString str = GetValidFilename(m_helpFile);

    HH_AKLINK link;
    link.cbStruct =     sizeof(HH_AKLINK) ;
    link.fReserved =    FALSE ;
    link.pszKeywords =  k.c_str() ;
    link.pszUrl =       NULL ;
    link.pszMsgText =   NULL ;
    link.pszMsgTitle =  NULL ;
    link.pszWindow =    NULL ;
    link.fIndexOnFail = TRUE ;

    HtmlHelp(GetSuitableHWND(), (const wxChar*) str, HH_KEYWORD_LOOKUP, (DWORD)& link);
    return TRUE;
}

bool wxCHMHelpController::Quit()
{
    HtmlHelp(GetSuitableHWND(), 0, HH_CLOSE_ALL, 0L);
    return TRUE;
}

// Append extension if necessary.
wxString wxCHMHelpController::GetValidFilename(const wxString& file) const
{
    wxString path, name, ext;
    wxSplitPath(file, & path, & name, & ext);

    wxString fullName;
    if (path.IsEmpty())
        fullName = name + wxT(".chm");
    else if (path.Last() == wxT('\\'))
        fullName = path + name + wxT(".chm");
    else
        fullName = path + wxT("\\") + name + wxT(".chm");
    return fullName;
}

#endif // wxUSE_HELP
