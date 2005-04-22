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

#include "wx/defs.h"

#if wxUSE_STDPATHS

#include "wx/string.h"

// ----------------------------------------------------------------------------
// wxStandardPaths returns the standard locations in the file system
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPathsBase
{
public:
    // return the global standard paths object
    static wxStandardPathsBase& Get();


    // return the directory with system config files:
    // /etc under Unix, c:\Documents and Settings\All Users\Application Data
    // under Windows, /Library/Preferences for Mac
    virtual wxString GetConfigDir() const = 0;

    // return the directory for the user config files:
    // $HOME under Unix, c:\Documents and Settings\username under Windows,
    // ~/Library/Preferences under Mac
    //
    // only use this if you have a single file to put there, otherwise
    // GetUserDataDir() is more appropriate
    virtual wxString GetUserConfigDir() const = 0;

    // return the location of the applications global, i.e. not user-specific,
    // data files
    //
    // prefix/share/appname under Unix, c:\Program Files\appname under Windows,
    // appname.app/Contents/SharedSupport app bundle directory under Mac
    virtual wxString GetDataDir() const = 0;

    // return the location for application data files which are host-specific
    //
    // same as GetDataDir() except under Unix where it is /etc/appname
    virtual wxString GetLocalDataDir() const;

    // return the directory for the user-dependent application data files
    //
    // $HOME/.appname under Unix,
    // c:\Documents and Settings\username\Application Data\appname under Windows
    // and ~/Library/Application Support/appname under Mac
    virtual wxString GetUserDataDir() const = 0;

    // return the directory for user data files which shouldn't be shared with
    // the other machines
    //
    // same as GetUserDataDir() for all platforms except Windows where it is
    // the "Local Settings\Application Data\appname" directory
    virtual wxString GetUserLocalDataDir() const;

    // return the directory where the loadable modules (plugins) live
    //
    // prefix/lib/appname under Unix, program directory under Windows and
    // Contents/Plugins app bundle subdirectory under Mac
    virtual wxString GetPluginsDir() const = 0;


    // virtual dtor for the base class
    virtual ~wxStandardPathsBase();

protected:
    // append "/appname" suffix if the app name is set (doesn't append the
    // slash if dir already ends with a slash or dot)
    static wxString AppendAppName(const wxString& dir);
};

#if defined(__WXMSW__)
    #include "wx/msw/stdpaths.h"
// We want CoreFoundation paths on both CarbonLib and Darwin (for all ports)
#elif defined(__WXMAC__) || defined(__DARWIN__)
    #include "wx/mac/corefoundation/stdpaths.h"
#elif defined(__OS2__)
    #include "wx/os2/stdpaths.h"
#elif defined(__UNIX__)
    #include "wx/unix/stdpaths.h"
#elif defined(__PALMOS__)
    #include "wx/palmos/stdpaths.h"
#else

// ----------------------------------------------------------------------------
// Minimal generic implementation
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxStandardPaths : public wxStandardPathsBase
{
public:
    void SetInstallPrefix(const wxString& prefix) { m_prefix = prefix; }
    wxString GetInstallPrefix() const { return m_prefix; }
    virtual wxString GetConfigDir() const { return m_prefix; }
    virtual wxString GetUserConfigDir() const { return m_prefix; }
    virtual wxString GetDataDir() const { return m_prefix; }
    virtual wxString GetLocalDataDir() const { return m_prefix; }
    virtual wxString GetUserDataDir() const { return m_prefix; }
    virtual wxString GetPluginsDir() const { return m_prefix; }

private:
    wxString m_prefix;
};

#endif

#endif // wxUSE_STDPATHS

#endif // _WX_STDPATHS_H_

