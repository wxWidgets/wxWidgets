/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/desktopenv.cpp
// Purpose:     wxDesktopEnv
// Author:      Igor Korot
// Modified by:
// Created:     17/11/15
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#include "wx/wx.h"

#include <shellapi.h> // needed for SHFILEOPSTRUCT
#include "wx/filename.h"
#include "wx/log.h"
#include "wx/desktopenv.h"

// ============================================================================
// implementation
// ============================================================================

wxDesktopEnv::wxDesktopEnv() : wxDesktopEnvBase()
{
}

bool wxDesktopEnv::MoveToRecycleBin(wxString &path)
{
    bool result = false;
    if( wxDesktopEnvBase::MoveToRecycleBin( path ) )
    {
        SHFILEOPSTRUCT fileOp;
        ::ZeroMemory( &fileOp, sizeof( fileOp ) );
        fileOp.hwnd = NULL;
        fileOp.wFunc = FO_DELETE;
        wxString temp = path + '\0';
        fileOp.pFrom = temp.c_str();
        fileOp.pTo = NULL;
        fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
        const int res = SHFileOperation( &fileOp );
        if( res != 0 )
           wxLogError( _( "Failed to move file '%s' to Recycle Bin" ), path );
        else
            result = true;
    }
    return result;
}
