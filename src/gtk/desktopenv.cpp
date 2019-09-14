////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/desktopenv.cpp
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

#include "wx/wx.h"

#include "gio/gio.h"
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
        GError *error = NULL;
        GFile *f = g_file_new_for_path( fileName.c_str() );
        gboolean res = g_file_trash( f, NULL, &error );
        if( !res || ( error && error->code == G_IO_ERROR_NOT_SUPPORTED ) )
           wxLogError( _( "Failed to move file '%s' to Recycle Bin" ), fileName );
        else
        {
            result = true;
            g_object_unref( f );
        }
    }
    return result;
}
