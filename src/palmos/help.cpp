/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/helpwin.cpp
// Purpose:     Help system: WinHelp implementation
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#include <time.h>

#include "wx/palmos/private.h"

#include <string.h>


IMPLEMENT_DYNAMIC_CLASS(wxWinHelpController, wxHelpControllerBase)

bool wxWinHelpController::Initialize(const wxString& filename)
{
    return false;
}

bool wxWinHelpController::LoadFile(const wxString& file)
{
    return false;
}

bool wxWinHelpController::DisplayContents(void)
{
    return false;
}

bool wxWinHelpController::DisplaySection(int section)
{
    return false;
}

bool wxWinHelpController::DisplayContextPopup(int contextId)
{
    return false;
}

bool wxWinHelpController::DisplayBlock(long block)
{
    return false;
}

bool wxWinHelpController::KeywordSearch(const wxString& k,
                                        wxHelpSearchMode WXUNUSED(mode))
{
    return false;
}

bool wxWinHelpController::Quit(void)
{
    return false;
}

wxString wxWinHelpController::GetValidFilename(const wxString& file) const
{
    wxString ret;

    return ret;
}

#endif // wxUSE_HELP
