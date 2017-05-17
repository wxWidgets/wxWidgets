///////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/stdpaths.cpp
// Purpose:     wxStandardPaths implementation for Unix & OpenVMS systems
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-10-19
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
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

#if wxUSE_STDPATHS

#include "wx/stdpaths.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/wxcrt.h"
    #include "wx/utils.h"
#endif //WX_PRECOMP

#include "wx/filename.h"
#include "wx/log.h"
#include "wx/textfile.h"

#if defined( __LINUX__ ) || defined( __VMS )
    #include <unistd.h>
#endif

// ============================================================================
// common VMS/Unix part of wxStandardPaths implementation
// ============================================================================

void wxStandardPaths::SetInstallPrefix(const wxString& prefix)
{
    m_prefix = prefix;
}

// Helper function returning the value of XDG_CONFIG_HOME environment variable
// or its default value if it is not defined.
static wxString GetXDGConfigHome()
{
    wxString dir;
    if ( !wxGetEnv(wxS("XDG_CONFIG_HOME"), &dir) || dir.empty() )
        dir = wxFileName::GetHomeDir() + wxS("/.config");
    return dir;
}

wxString wxStandardPaths::GetUserConfigDir() const
{
    wxString dir;

    switch ( GetFileLayout() )
    {
        case FileLayout_Classic:
            dir = wxFileName::GetHomeDir();
            break;

        case FileLayout_XDG:
            dir = GetXDGConfigHome();
            break;
    }

    wxASSERT_MSG( !dir.empty(), wxS("unsupported file layout") );

    return dir;
}


// ============================================================================
// wxStandardPaths implementation for VMS
// ============================================================================

#ifdef __VMS

wxString wxStandardPaths::GetInstallPrefix() const
{
    if ( m_prefix.empty() )
    {
        const_cast<wxStandardPaths *>(this)->m_prefix = wxT("/sys$system");
    }

    return m_prefix;
}

wxString wxStandardPaths::GetConfigDir() const
{
   return wxT("/sys$manager");
}

wxString wxStandardPaths::GetDataDir() const
{
   return AppendAppInfo(GetInstallPrefix() + wxT("/sys$share"));
}

wxString wxStandardPaths::GetLocalDataDir() const
{
   return AppendAppInfo(wxT("/sys$manager"));
}

wxString wxStandardPaths::GetUserDataDir() const
{
   return wxFileName::GetHomeDir();
}

wxString wxStandardPaths::GetPluginsDir() const
{
    return wxString(); // TODO: this is wrong, it should return something
}

wxString
wxStandardPaths::GetLocalizedResourcesDir(const wxString& lang,
                                          ResourceCat category) const
{
    return wxStandardPathsBase::GetLocalizedResourcesDir(lang, category);
}

wxString wxStandardPaths::GetExecutablePath() const
{
    return wxStandardPathsBase::GetExecutablePath();
}

#else // !__VMS

// ============================================================================
// wxStandardPaths implementation for Unix
// ============================================================================

wxString wxStandardPaths::GetExecutablePath() const
{
#ifdef __LINUX__
    wxString exeStr;

    char buf[4096];
    int result = readlink("/proc/self/exe", buf, WXSIZEOF(buf) - 1);
    if ( result != -1 )
    {
        buf[result] = '\0'; // readlink() doesn't NUL-terminate the buffer

        // if the /proc/self/exe symlink has been dropped by the kernel for
        // some reason, then readlink() could also return success but
        // "(deleted)" as link destination...
        if ( strcmp(buf, "(deleted)") != 0 )
            exeStr = wxString(buf, wxConvLibc);
    }

    if ( exeStr.empty() )
    {
        // UPX-specific hack: when using UPX on linux, the kernel will drop the
        // /proc/self/exe link; in this case we try to look for a special
        // environment variable called "   " which is created by UPX to save
        // /proc/self/exe contents. See
        //      http://sf.net/tracker/?func=detail&atid=309863&aid=1565357&group_id=9863
        // for more information about this issue.
        wxGetEnv(wxT("   "), &exeStr);
    }

    if ( !exeStr.empty() )
        return exeStr;
#endif // __LINUX__

    return wxStandardPathsBase::GetExecutablePath();
}

void wxStandardPaths::DetectPrefix()
{
    // we can try to infer the prefix from the location of the executable
    wxString exeStr = GetExecutablePath();
    if ( !exeStr.empty() )
    {
        // consider that we're in the last "bin" subdirectory of our prefix
        size_t pos = exeStr.rfind(wxT("/bin/"));
        if ( pos != wxString::npos )
            m_prefix.assign(exeStr, 0, pos);
    }

    if ( m_prefix.empty() )
    {
        m_prefix = wxT("/usr/local");
    }
}

wxString wxStandardPaths::GetInstallPrefix() const
{
    if ( m_prefix.empty() )
    {
        wxStandardPaths *pathPtr = const_cast<wxStandardPaths *>(this);
        pathPtr->DetectPrefix();
    }

    return m_prefix;
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

wxString wxStandardPaths::GetConfigDir() const
{
   return wxT("/etc");
}

wxString wxStandardPaths::GetDataDir() const
{
    // allow to override the location of the data directory by setting
    // WX_APPNAME_DATA_DIR environment variable: this is very useful in
    // practice for running well-written (and so using wxStandardPaths to find
    // their files) wx applications without installing them
    static const wxString
      envOverride(
        getenv(
            ("WX_" + wxTheApp->GetAppName().Upper() + "_DATA_DIR").c_str()
        )
      );

    if ( !envOverride.empty() )
        return envOverride;

   return AppendAppInfo(GetInstallPrefix() + wxT("/share"));
}

wxString wxStandardPaths::GetLocalDataDir() const
{
   return AppendAppInfo(wxT("/etc"));
}

wxString wxStandardPaths::GetUserDataDir() const
{
   return AppendAppInfo(wxFileName::GetHomeDir() + wxT("/."));
}

wxString wxStandardPaths::GetPluginsDir() const
{
    return AppendAppInfo(GetInstallPrefix() + wxT("/lib"));
}

wxString
wxStandardPaths::GetLocalizedResourcesDir(const wxString& lang,
                                          ResourceCat category) const
{
    if ( category != ResourceCat_Messages )
        return wxStandardPathsBase::GetLocalizedResourcesDir(lang, category);

    return GetInstallPrefix() + wxT("/share/locale/") + lang + wxT("/LC_MESSAGES");
}

wxString wxStandardPaths::GetUserDir(Dir userDir) const
{
    // Note that we do not use the file layout here because there is no reason
    // not to respect the XDG convention even if SetFileLayout(FileLayout_XDG)
    // hadn't been called: we're not bound by any backwards compatibility
    // considerations as there can't be any pre-existing config or data files
    // in the home directory that wouldn't be found any longer after updating
    // the version of wxWidgets used by the application.

    wxLogNull logNull;
    const wxString homeDir = wxFileName::GetHomeDir();
    if (userDir == Dir_Cache)
    {
        wxString cacheDir;
        if ( !wxGetEnv(wxS("XDG_CACHE_HOME"), &cacheDir) )
          cacheDir = homeDir + wxS("/.cache");

        return cacheDir;
    }

    const wxFileName dirsFile(GetXDGConfigHome(), wxS("user-dirs.dirs"));
    if ( dirsFile.FileExists() )
    {
        wxString userDirId;
        switch (userDir)
        {
            case Dir_Desktop:
                userDirId = "XDG_DESKTOP_DIR";
                break;
            case Dir_Downloads:
                userDirId = "XDG_DOWNLOAD_DIR";
                break;
            case Dir_Music:
                userDirId = "XDG_MUSIC_DIR";
                break;
            case Dir_Pictures:
                userDirId = "XDG_PICTURES_DIR";
                break;
            case Dir_Videos:
                userDirId = "XDG_VIDEOS_DIR";
                break;
            default:
                userDirId = "XDG_DOCUMENTS_DIR";
                break;
        }

        wxTextFile textFile;
        if ( textFile.Open(dirsFile.GetFullPath()) )
        {
            for ( wxString line = textFile.GetFirstLine();
                  !textFile.Eof();
                  line = textFile.GetNextLine() )
            {
                int pos = line.Find(userDirId);
                if (pos != wxNOT_FOUND)
                {
                    wxString value = line.AfterFirst(wxT('='));
                    value.Replace(wxT("$HOME"), homeDir);
                    value.Trim(true);
                    value.Trim(false);
                    // Remove quotes
                    value.Replace("\"", "", true /* replace all */);
                    if (!value.IsEmpty() && wxDirExists(value))
                        return value;
                    else
                        break;
                }
            }
        }
    }

    return wxStandardPathsBase::GetUserDir(userDir);
}

#endif // __VMS/!__VMS

wxString
wxStandardPaths::MakeConfigFileName(const wxString& basename,
                                    ConfigFileConv conv) const
{
    wxFileName fn(wxEmptyString, basename);

    bool addExt = false;

    switch ( GetFileLayout() )
    {
        case FileLayout_Classic:
            switch ( conv )
            {
                case ConfigFileConv_Dot:
                    fn.SetName(wxT('.') + fn.GetName());
                    break;

                case ConfigFileConv_Ext:
                    addExt = true;
                    break;
            }
            break;

        case FileLayout_XDG:
            // Dot files are never used in XDG mode.
            addExt = true;
            break;
    }

    if ( addExt )
        fn.SetExt(wxS("conf"));

    return fn.GetFullName();
}

#endif // wxUSE_STDPATHS
