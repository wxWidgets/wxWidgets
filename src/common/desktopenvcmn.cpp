///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/desktopenv.cpp
// Purpose:     implementation of wxDesktopEnvBase
// Author:      Igor Korot
// Created:     2015-11-17
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
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
#include "wx/filename.h"
#include "wx/log.h"
#include "wx/desktopenv.h"

bool wxDesktopEnvBase::MoveToRecycleBin(wxString &path)
{
    bool result = true;
    wxFileName name = wxFileName( path );
    name.MakeAbsolute();
    path = name.GetFullPath();
    if( !wxFileExists( path ) && !wxDirExists( path ) )
    {
        wxLogSysError( "Failed to move the file '%s' to Recycle Bin", path.c_str() );
        result = false;
    }
    return result;
}
