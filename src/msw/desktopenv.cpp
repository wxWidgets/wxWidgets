///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/desktopenv.cpp
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/log.h"
#include "wx/desktopenv.h"

// ============================================================================
// implementation
// ============================================================================

/* static */
bool wxDesktopEnv::MoveToRecycleBin(const wxString &path)
{
    bool res = wxDesktopEnvBase::MoveToRecycleBin( path );
    if( res )
    {
        SHFILEOPSTRUCT fileOp;
        wxZeroMemory( fileOp );
        fileOp.hwnd = NULL;
        fileOp.wFunc = FO_DELETE;
        wxString temp = path + '\0';
        fileOp.pFrom = temp.t_str();
        fileOp.pTo = NULL;
        fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
        res = SHFileOperation( &fileOp );
        if( res != 0 )
            wxLogSysError( _( "Failed to move '%s' to Recycle Bin" ), path );
        else
            res = true;
    }
    return res;
}
