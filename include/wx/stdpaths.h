///////////////////////////////////////////////////////////////////////////////
// Name:        wx/stdpaths.h
// Purpose:     declaration of wxStandardPaths class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-10-17
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STDPATHS_H_
#define _WX_STDPATHS_H_

// ----------------------------------------------------------------------------
// wxStandardPaths returns the standard locations in the file system
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPaths
{
public:
#ifdef __UNIX_LIKE__
    // set the program installation directory which is /usr/local by default
    //
    // under some systems (currently only Linux) the program directory can be
    // determined automatically but for portable programs you should always set
    // it explicitely
    static void SetInstallPrefix(const wxString& prefix);

    // get the program installation prefix
    //
    // if the prefix had been previously by SetInstallPrefix, returns that
    // value, otherwise tries to determine it automatically (Linux only right
    // now) and returns /usr/local if it failed
    static wxString GetInstallPrefix();
#endif // __UNIX_LIKE__


    // return the directory with system config files:
    // /etc under Unix, c:\Windows under Windows, /Library/Preferences for Mac
    static wxString GetConfigDir();

    // return the directory for the user config files:
    // $HOME under Unix, c:\Documents and Settings\username under Windows,
    // ~/Library/Preferences under Mac
    //
    // only use this if you have a single file to put there, otherwise
    // GetUserDataDir() is more appropriate
    static wxString GetUserConfigDir();

    // return the location of the applications global, i.e. not user-specific,
    // data files
    //
    // prefix/share/appname under Unix, c:\Program Files\appname under Windows,
    // appname.app/Contents/SharedSupport app bundle directory under Mac
    static wxString GetDataDir();

    // return the location for application data files which are host-specific
    //
    // same as GetDataDir() except under Unix where it is /etc/appname
    static wxString GetLocalDataDir();

    // return the directory for the user-dependent application data files
    //
    // $HOME/.appname under Unix,
    // c:\Documents and Settings\username\Application Data\appname under Windows
    // and ~/Library/Application Support/appname under Mac
    static wxString GetUserDataDir();

    // return the directory for user data files which shouldn't be shared with
    // the other machines
    //
    // same as GetUserDataDir() for all platforms except Windows where it is
    // the "Local Settings\Application Data\appname" directory
    static wxString GetUserLocalDataDir();

    // return the directory where the loadable modules (plugins) live
    //
    // prefix/lib/appname under Unix, program directory under Windows and
    // Contents/Plugins app bundle subdirectory under Mac
    static wxString GetPluginsDir();
};

#endif // _WX_STDPATHS_H_

