/////////////////////////////////////////////////////////////////////////////
// Name:        utilsexec.cpp
// Purpose:     Execution-related utilities
// Author:      Stefan Csomor
// Modified by: 
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
#endif //ndef WX_PRECOMP

#ifndef __DARWIN__

#include "wx/mac/private.h"
#include "LaunchServices.h"

long wxExecute(const wxString& command, int flags, wxProcess *WXUNUSED(handler))
{
    wxASSERT_MSG( flags == wxEXEC_ASYNC,
        wxT("wxExecute: Only wxEXEC_ASYNC is supported") );

    FSRef fsRef ;
    OSErr err = noErr ;
    err = wxMacPathToFSRef( command , &fsRef ) ;
    if ( noErr == err )
    {
        err = LSOpenFSRef( &fsRef , NULL ) ;
    }

    // 0 means execution failed. Returning non-zero is a PID, but not
    // on Mac where PIDs are 64 bits and won't fit in a long, so we
    // return a dummy value for now.
    return ( err == noErr ) ? -1 : 0;
}

#endif //ndef __DARWIN__

