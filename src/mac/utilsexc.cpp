/////////////////////////////////////////////////////////////////////////////
// Name:        utilsexec.cpp
// Purpose:     Execution-related utilities
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/utils.h"
#ifdef __DARWIN__
#include "wx/unix/execute.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __DARWIN__
#define wxEXECUTE_WIN_MESSAGE 10000

long wxExecute(const wxString& command, int flags, wxProcess *handler)
{
    // TODO
    wxFAIL_MSG( _T("wxExecute() not yet implemented") );
    return 0;
}
#endif

#ifdef __DARWIN__
int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
   wxFAIL_MSG( _T("wxAddProcessCallback() not yet implemented") );
   return 0;
}
#endif
