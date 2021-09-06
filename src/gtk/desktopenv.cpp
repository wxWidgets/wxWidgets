///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/desktopenv.cpp
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

#include <gio/gio.h>
#include "wx/desktopenv.h"

// ============================================================================
// implementation
// ============================================================================

/* static */
bool wxDesktopEnv::MoveToRecycleBin(const wxString &path)
{
    bool result = false;
    if( wxDesktopEnvBase::MoveToRecycleBin( path ) )
    {
        GError *error = NULL;
        GFile *f = g_file_new_for_path( path.c_str() );
        gboolean res = g_file_trash( f, NULL, &error );
        if( error && error->code == G_IO_ERROR_NOT_SUPPORTED )
            wxLogSysError( _( "MoveToRecycleBin: Operation not supported." ) );
        else if( !res )
            wxLogSysError( _( "Failed to move '%s' to Recycle Bin" ), path );
        else
        {
            result = true;
            g_object_unref( f );
        }
    }
    return result;
}
