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

#if USE_HELP
#include <time.h>

#ifdef __WINDOWS__
#include <wx/msw/private.h>
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

bool wxWinHelpController::Initialize(const wxString& filename, int server)
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
    if (m_helpFile == "") return FALSE;

    char buf[_MAXPATHLEN];
    strcpy(buf, (const char*) m_helpFile);
    size_t len = strlen(buf);
    if (!(buf[len-1] == 'p' && buf[len-2] == 'l' && buf[len-3] == 'h' && buf[len-4] == '.'))
      strcat(buf, ".hlp");
    if (wxTheApp->GetTopWindow())
    {
#if defined(__WIN95__)
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), buf, HELP_FINDER, 0L);
#else
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), buf, HELP_CONTENTS, 0L);
#endif
     return TRUE;
    }
	return FALSE;
}

bool wxWinHelpController::DisplaySection(int section)
{
  // No WinHelp equivalent for this
  return FALSE;
}

bool wxWinHelpController::DisplayBlock(long block)
{
    // Use context number -- a very rough equivalent to block id!
    if (!m_helpFile) return FALSE;

    char buf[_MAXPATHLEN];
    strcpy(buf, m_helpFile);
    size_t len = strlen(buf);
    if (!(buf[len-1] == 'p' && buf[len-2] == 'l' && buf[len-3] == 'h' && buf[len-4] == '.'))
      strcat(buf, ".hlp");
    if (wxTheApp->GetTopWindow())
	{
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), buf, HELP_CONTEXT, (DWORD)block);
      return TRUE;
	}
    return FALSE;
}

bool wxWinHelpController::KeywordSearch(const wxString& k)
{
    if (m_helpFile == "") return FALSE;

    char buf[_MAXPATHLEN];
    strcpy(buf, m_helpFile);
    size_t len = strlen(buf);
    if (!(buf[len-1] == 'p' && buf[len-2] == 'l' && buf[len-3] == 'h' && buf[len-4] == '.'))
      strcat(buf, ".hlp");
    if (wxTheApp->GetTopWindow())
    {
      WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), buf, HELP_PARTIALKEY, (DWORD)(const char*) k);
      return TRUE;
    }
    return FALSE;
}

// Can't close the help window explicitly in WinHelp
bool wxWinHelpController::Quit(void)
{
  return TRUE;
}

// Don't get notified of WinHelp quitting
void wxWinHelpController::OnQuit(void)
{
}

#endif // USE_HELP
