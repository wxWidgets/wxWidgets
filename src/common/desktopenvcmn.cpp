///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/desktopenv.cpp
// Purpose:     implementation of wxDesktopEnvBase
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/filefn.h"
#include <wx/filename.h>
#include "wx/log.h"
#include "wx/desktopenv.h"

/* static */
bool wxDesktopEnvBase::MoveToRecycleBin(const wxString &path)
{
    bool result = true;
    wxFileName name = wxFileName( path );
    name.MakeAbsolute();
    wxString tempPath = name.GetFullPath();
    if( !wxFileExists( tempPath ) && !wxDirExists( tempPath ) )
    {
        wxLogSysError( _( "Failed to move '%s' to Recycle Bin" ), path.c_str() );
        result = false;
    }
    return result;
}
