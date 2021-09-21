///////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/desktopenv.cpp
// Purpose:     implementation of wxDesktopEnvBase
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
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
#include "wx/translation.h"
#include "wx/log.h"
#include "wx/desktopenv.h"
#include "wx/gtk/private/error.h"

// ============================================================================
// implementation
// ============================================================================

/* static */
bool wxDesktopEnv::MoveToRecycleBin(const wxString &path)
{
    bool result = false;
    if( wxDesktopEnvBase::MoveToRecycleBin( path ) )
    {
        GFile *f = g_file_new_for_path( path.c_str() );
        wxGtkError error;
        gboolean res = g_file_trash( f, NULL, error.Out() );
        if( !res )
            wxLogSysError( _( "Failed to move '%s' to Recycle Bin, error %s" ), path, error.GetMessage() );
        else
        {
            result = true;
        }
        g_object_unref( f );
    }
    return result;
}

/* static */
bool wxDesktopEnv::GetFilesInRecycleBin(std::vector<wxString> &files)
{
    bool result = true;
    GFileInfo *info = NULL;
    wxGtkError error;
    GFileEnumerator *trashedFiles = g_file_enumerate_children( g_file_new_for_uri( "trash:///" ), "*", G_FILE_QUERY_INFO_NONE, NULL, error.Out() );
    if( trashedFiles )
    {
        while( ( info = g_file_enumerator_next_file( trashedFiles, NULL, error.Out() ) ) != NULL )
        {
            files.push_back( wxString( g_file_info_get_name( info ) ) );
        }
    }
    else
    {
        result = false;
        wxLogSysError( _( "Failed to retrieve file list from Recycle Bin, error: %s" ), error.GetMessage() );
    }
    g_object_unref( trashedFiles );
    return result;
}

/*static*/
bool wxDesktopEnv::RestoreFromRecycleBin(const wxString &path)
{
    bool res = true;
    wxGtkError error;
    GFile *f = g_file_new_for_uri( "trash:///" + path.c_str() );
    g_autoptr (GFileInfo) info = g_file_query_info( f, G_FILE_ATTRIBUTE_TRASH_ORIG_PATH, G_FILE_QUERY_INFO_NONE, NULL, error.Out() );
    if( !info )
    {
        wxLogSysError( _( "Failed to obtain the original path for the %, error %s" ), path, error.GetMessage() );
        res = false;
    }
    else
    {
        g_autofree char *orig_path = g_file_info_get_attribute_as_string( info, G_FILE_ATTRIBUTE_TRASH_ORIG_PATH );
        g_autoptr (GFile) target = g_file_new_for_commandline_arg( orig_path );
        res = g_file_move( f, target, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, error.Out() );
        if( !res )
            wxLogSysError( _( "Failed to restore the %s, error %s" ), path.c_str(), error.GetMessage() );
    }
    g_object_unref( f );
    return res;
}
