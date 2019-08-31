///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/desktopenv.cpp
// Purpose:     implementation of wxDesktopEnvBase
// Author:      Igor Korot
// Created:     2015-11-17
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
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
#include "wx/log.h"
#include "wx/desktopenv.h"

bool wxDesktopEnvBase::MoveFileToRecycleBin(const wxString &fileName)
{
    bool result = true;
    if( !wxFileExists( fileName ) && !wxDirExists( fileName ) )
    {
        wxLogSysError( "Failed to move the file '%s' to Recycle Bin", fileName.c_str() );
        result = false;
    }
    return result;
}
