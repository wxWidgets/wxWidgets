///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/stdpaths.h
// Purpose:     wxStandardPaths for Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-10-19
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_STDPATHS_H_
#define _WX_MSW_STDPATHS_H_

struct _GUID;

// ----------------------------------------------------------------------------
// wxStandardPaths
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPaths : public wxStandardPathsBase
{
public:
    // implement base class pure virtuals
    virtual wxString GetExecutablePath() const wxOVERRIDE;
    virtual wxString GetConfigDir() const wxOVERRIDE;
    virtual wxString GetUserConfigDir() const wxOVERRIDE;
    virtual wxString GetDataDir() const wxOVERRIDE;
    virtual wxString GetUserDataDir() const wxOVERRIDE;
    virtual wxString GetUserLocalDataDir() const wxOVERRIDE;
    virtual wxString GetPluginsDir() const wxOVERRIDE;
    virtual wxString GetUserDir(Dir userDir) const wxOVERRIDE;
    virtual wxString MakeConfigFileName(const wxString& basename,
                                        ConfigFileConv conv = ConfigFileConv_Ext
                                        ) const wxOVERRIDE;


    // MSW-specific methods

    // This class supposes that data, plugins &c files are located under the
    // program directory which is the directory containing the application
    // binary itself. But sometimes this binary may be in a subdirectory of the
    // main program directory, e.g. this happens in at least the following
    // common cases:
    //  1. The program is in "bin" subdirectory of the installation directory.
    //  2. The program is in "debug" subdirectory of the directory containing
    //     sources and data files during development
    //
    // By calling this function you instruct the class to remove the last
    // component of the path if it matches its argument. Notice that it may be
    // called more than once, e.g. you can call both IgnoreAppSubDir("bin") and
    // IgnoreAppSubDir("debug") to take care of both production and development
    // cases above but that each call will only remove the last path component.
    // Finally note that the argument can contain wild cards so you can also
    // call IgnoreAppSubDir("vc*msw*") to ignore all build directories at once
    // when using wxWidgets-inspired output directories names.
    void IgnoreAppSubDir(const wxString& subdirPattern);

    // This function is used to ignore all common build directories and is
    // called from the ctor -- use DontIgnoreAppSubDir() to undo this.
    void IgnoreAppBuildSubDirs();

    // Undo the effects of all preceding IgnoreAppSubDir() calls.
    void DontIgnoreAppSubDir();


    // Returns the directory corresponding to the specified Windows shell CSIDL
    static wxString MSWGetShellDir(int csidl);

protected:
    // Ctor is protected, use wxStandardPaths::Get() instead of instantiating
    // objects of this class directly.
    //
    // It calls IgnoreAppBuildSubDirs() and also sets up the object to use
    // both vendor and application name by default.
    wxStandardPaths();

    // get the path corresponding to the given standard CSIDL_XXX constant
    static wxString DoGetDirectory(int csidl);

    static wxString DoGetKnownFolder(const _GUID& rfid);

    // return the directory of the application itself
    wxString GetAppDir() const;

    // directory returned by GetAppDir()
    mutable wxString m_appDir;
};

// ----------------------------------------------------------------------------
// wxStandardPathsWin16: this class is for internal use only
// ----------------------------------------------------------------------------

// override config file locations to be compatible with the values used by
// wxFileConfig (dating from Win16 days which explains the class name)
class WXDLLIMPEXP_BASE wxStandardPathsWin16 : public wxStandardPaths
{
public:
    virtual wxString GetConfigDir() const wxOVERRIDE;
    virtual wxString GetUserConfigDir() const wxOVERRIDE;
};

#endif // _WX_MSW_STDPATHS_H_
