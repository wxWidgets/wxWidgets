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

#include "wx/msw/helpwin.h"

#if wxUSE_HELP
#include <time.h>

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#include <string.h>

// MAX path length
#define _MAXPATHLEN 500

// MAX length of Help descriptor
#define _MAX_HELP_LEN 500

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWinHelpController, wxHelpControllerBase)
#endif

wxWinHelpController::wxWinHelpController(void)
{
  m_helpFile = "";
}

wxWinHelpController::~wxWinHelpController(void)
{
}

bool wxWinHelpController::Initialize(const wxString& filename)
{
  m_helpFile = filename;
  return TRUE;
}

bool wxWinHelpController::LoadFile(const wxString& file)
{
  m_helpFile = file;
  return TRUE;
}

bool wxWinHelpController::DisplayContents(void)
{
    if (m_helpFile == wxT("")) return FALSE;

    wxString str = m_helpFile;
    size_t len = str.Length();
    if (!(str[(size_t)(len-1)] == wxT('p') && str[(size_t)(len-2)] == wxT('l') && str[(size_t)(len-3)] == wxT('h') && str[(size_t)(len-4)] == wxT('.')))
      str += wxT(".hlp");

    if (wxTheApp->GetTopWindow())
    {
#if defined(__WIN95__)
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_FINDER, 0L);
#else
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_CONTENTS, 0L);
#endif
     return TRUE;
    }
	return FALSE;
}

bool wxWinHelpController::DisplaySection(int section)
{
    // Use context number
    if (m_helpFile == wxT("")) return FALSE;

    wxString str = m_helpFile;
    size_t len = str.Length();
    if (!(str[(size_t)(len-1)] == wxT('p') && str[(size_t)(len-2)] == wxT('l') && str[(size_t)(len-3)] == wxT('h') && str[(size_t)(len-4)] == wxT('.')))
      str += wxT(".hlp");

    if (wxTheApp->GetTopWindow())
	{
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_CONTEXT, (DWORD)section);
      return TRUE;
	}
    return FALSE;
}

bool wxWinHelpController::DisplayBlock(long block)
{
    // Use context number -- a very rough equivalent to block id!
    if (m_helpFile == wxT("")) return FALSE;

    wxString str = m_helpFile;
    size_t len = str.Length();
    if (!(str[(size_t)(len-1)] == 'p' && str[(size_t)(len-2)] == 'l' && str[(size_t)(len-3)] == 'h' && str[(size_t)(len-4)] == '.'))
      str += wxT(".hlp");

    if (wxTheApp->GetTopWindow())
	{
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_CONTEXT, (DWORD)block);
      return TRUE;
	}
    return FALSE;
}

bool wxWinHelpController::KeywordSearch(const wxString& k)
{
    if (m_helpFile == wxT("")) return FALSE;

    wxString str = m_helpFile;
    size_t len = str.Length();
    if (!(str[(size_t)(len-1)] == wxT('p') && str[(size_t)(len-2)] == wxT('l') && str[(size_t)(len-3)] == wxT('h') && str[(size_t)(len-4)] == wxT('.')))
      str += wxT(".hlp");

    if (wxTheApp->GetTopWindow())
    {
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_PARTIALKEY, (DWORD)(const wxChar*) k);
      return TRUE;
    }
    return FALSE;
}

// Can't close the help window explicitly in WinHelp
bool wxWinHelpController::Quit(void)
{
  if (wxTheApp->GetTopWindow())
  {
    WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), 0, HELP_QUIT, 0L);
    return TRUE;
  }
  else
    return FALSE;
}

// Don't get notified of WinHelp quitting
void wxWinHelpController::OnQuit(void)
{
}

#endif // wxUSE_HELP
