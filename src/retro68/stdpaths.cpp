///////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/stdpaths.cpp
// Purpose:     wxStandardPaths implementation for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STDPATHS

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/stdpaths.h"
#include "wx/filename.h"

#include "wx/retro68/private.h"

namespace
{

// Folder types which may be missing from Multiverse.h
const OSType wxMAC_DOCUMENTS_FOLDER = 'docs';
const OSType wxMAC_APP_SUPPORT_FOLDER = 'asup';

// Remove the trailing separator, if any, from the path to the directory as
// all wxStandardPaths functions return the paths without it. Note that this
// is not done for the volume root directories, as this would transform them
// into relative paths.
wxString RemoveTrailingSep(const wxString& path)
{
    if ( path.length() > 1 && path.Last() == wxFILE_SEP_PATH &&
            path.find(wxFILE_SEP_PATH) != path.length() - 1 )
    {
        return path.substr(0, path.length() - 1);
    }

    return path;
}

wxString GetFolder(OSType folderType)
{
    return RemoveTrailingSep(wxMacFindFolder(folderType));
}

wxString GetAppDir()
{
    FSSpec spec;
    if ( !wxMacGetApplicationSpec(&spec) )
        return wxString(":");

    return RemoveTrailingSep(wxMacDirToPath(spec.vRefNum, spec.parID));
}

} // anonymous namespace

// ============================================================================
// wxStandardPaths implementation
// ============================================================================

wxString wxStandardPaths::GetExecutablePath() const
{
    FSSpec spec;
    if ( !wxMacGetApplicationSpec(&spec) )
        return wxStandardPathsBase::GetExecutablePath();

    return wxMacFSSpecToPath(spec);
}

wxString wxStandardPaths::GetConfigDir() const
{
    return GetFolder(kPreferencesFolderType);
}

wxString wxStandardPaths::GetUserConfigDir() const
{
    // There are no separate users under classic Mac OS.
    return GetFolder(kPreferencesFolderType);
}

wxString wxStandardPaths::GetDataDir() const
{
    return GetAppDir();
}

wxString wxStandardPaths::GetLocalDataDir() const
{
    return GetAppDir();
}

wxString wxStandardPaths::GetUserDataDir() const
{
    // Application Support folder only exists since Mac OS 9, use Preferences
    // if it is not available.
    wxString dir = GetFolder(wxMAC_APP_SUPPORT_FOLDER);
    if ( dir.empty() )
        dir = GetFolder(kPreferencesFolderType);

    return AppendAppInfo(dir);
}

wxString wxStandardPaths::GetPluginsDir() const
{
    return AppendPathComponent(GetAppDir(), "Plug-ins");
}

wxString wxStandardPaths::GetDocumentsDir() const
{
    return GetUserDir(Dir_Documents);
}

wxString wxStandardPaths::GetTempDir() const
{
    wxString dir = GetFolder(kTemporaryFolderType);
    if ( dir.empty() )
        dir = wxFileName::GetTempDir();

    return dir;
}

wxString wxStandardPaths::GetUserDir(Dir userDir) const
{
    wxString dir;
    switch ( userDir )
    {
        case Dir_Cache:
            dir = GetTempDir();
            break;

        case Dir_Config:
            dir = GetUserConfigDir();
            break;

        case Dir_Desktop:
            dir = GetFolder(kDesktopFolderType);
            break;

        case Dir_Documents:
        case Dir_Downloads:
        case Dir_Music:
        case Dir_Pictures:
        case Dir_Videos:
            // Documents folder only exists since Mac OS 8.5.
            dir = GetFolder(wxMAC_DOCUMENTS_FOLDER);
            break;
    }

    // Fall back to the desktop folder, which always exists.
    if ( dir.empty() )
        dir = GetFolder(kDesktopFolderType);

    return dir;
}

wxString
wxStandardPaths::MakeConfigFileName(const wxString& basename,
                                    ConfigFileConv WXUNUSED(conv)) const
{
    // Follow the traditional Mac convention for the preferences files.
    return basename + " Preferences";
}

#endif // wxUSE_STDPATHS
