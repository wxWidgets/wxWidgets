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
#ifdef __UNIX__
#include "wx/unix/execute.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __UNIX__
#define wxEXECUTE_WIN_MESSAGE 10000

long wxExecute(const wxString& command, bool sync, wxProcess *handler)
{
    // TODO
    return 0;
}
#endif

#ifdef __UNIX__
int wxAddProcessCallback(wxEndProcessData *proc_data, int fd)
{
   wxFAIL_MSG(wxT("wxAddProcessCallback() function not ready"));
   return 0;
}
#endif