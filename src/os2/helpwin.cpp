/////////////////////////////////////////////////////////////////////////////
// Name:        helpwin.cpp
// Purpose:     Help system: native implementation
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/os2/helpwin.h"

#if wxUSE_HELP
#include <time.h>

#include <wx/os2/private.h>

#include <string.h>

// MAX path length
#define _MAXPATHLEN 500

// MAX length of Help descriptor
#define _MAX_HELP_LEN 500

IMPLEMENT_DYNAMIC_CLASS(wxWinHelpController, wxHelpControllerBase)

wxWinHelpController::wxWinHelpController()
{
    m_helpFile = "";
}

wxWinHelpController::~wxWinHelpController()
{
}

bool wxWinHelpController::Initialize(const wxString& filename)
{
    m_helpFile = filename;
    // TODO any other inits
    return TRUE;
}

bool wxWinHelpController::LoadFile(const wxString& file)
{
    m_helpFile = file;
    // TODO
    return TRUE;
}

bool wxWinHelpController::DisplayContents()
{
    if (m_helpFile == wxT("")) return FALSE;

    wxString str = m_helpFile;
    size_t len = str.Length();
    if (!(str[(size_t)(len-1)] == wxT('p') && str[(size_t)(len-2)] == wxT('l') && str[(size_t)(len-3)] == wxT('h') && str[(size_t)(len-4)] == wxT('.')))
      str += wxT(".hlp");

    if (wxTheApp->GetTopWindow())
    {
    // TODO : display the help
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
    // TODO ::
    //  WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_CONTEXT, (DWORD)section);
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
    // TODO:
    //  WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_CONTEXT, (DWORD)block);
      return TRUE;
	}
    return FALSE;
}

bool wxWinHelpController::KeywordSearch(const wxString& k)
{
    if (m_helpFile == "") return FALSE;

    wxString str = m_helpFile;
    size_t len = str.Length();
    if (!(str[(size_t)(len-1)] == wxT('p') && str[(size_t)(len-2)] == wxT('l') && str[(size_t)(len-3)] == wxT('h') && str[(size_t)(len-4)] == wxT('.')))
      str += wxT(".hlp");

    if (wxTheApp->GetTopWindow())
    {
      // TODO:
      // WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), (const wxChar*) str, HELP_PARTIALKEY, (DWORD)(const wxChar*) k);
      return TRUE;
    }
    return FALSE;
}

// Can't close the help window explicitly in WinHelp
bool wxWinHelpController::Quit()
{
  if (wxTheApp->GetTopWindow())
  {
    // TODO:
    // WinHelp((HWND) wxTheApp->GetTopWindow()->GetHWND(), 0, HELP_QUIT, 0L);
    return TRUE;
  }
  else
    return FALSE;
}

void wxWinHelpController::OnQuit()
{
}
#endif // wxUSE_HELP

