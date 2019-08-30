/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/desktopenv.cpp
// Purpose:     wxDesktopEnv
// Author:      Igor Korot
// Modified by:
// Created:     17/11/15
// Copyright:   (c) Julian Smart
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

#include "wx/desktopenv.h"

// ============================================================================
// implementation
// ============================================================================

wxDesktopEnv::wxDesktopEnv() : wxDesktopEnvBase()
{
}

bool wxDesktopEnv::MoveFileToRecycleBin(const wxString &fileName)
{
    bool result = false;
    if( wxDesktopEnvBase::MoveFileToRecycleBin( fileName ) )
    {
        SHFILEOPSTRUCT fileOp;
        wxZeroMemory( fileOp );
        fileOp.hwnd = NULL;
        fileOp.wFunc = FO_DELETE;
        wxString temp = fileName + '\0';
        fileOp.pFrom = temp.c_str();
        fileOp.pTo = NULL;
        fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
        int res = SHFileOperation( &fileOp );
        if( res != 0 )
           wxLogSysError( _ ( "Failed to move file '%s' to Recycle Bin" ), fileName.c_str() );
        else
            result = true;
    }
    return result;
}
