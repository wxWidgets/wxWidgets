/////////////////////////////////////////////////////////////////////////////
// Name:        _stdpaths.i
// Purpose:     SWIG interface for wxStandardPaths
//
// Author:      Robin Dunn
//
// Created:     10-Nov-2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/stdpaths.h>
%}


//---------------------------------------------------------------------------
%newgroup

DocStr(wxStandardPaths,
"wx.StandardPaths returns the standard locations in the file system and
should be used by the programs to find their data files in a portable
way.

In the description of the methods below, the example return values are
given for the Unix, Windows and Mac OS X systems, however please note
that these are just the examples and the actual values may differ. For
example, under Windows: the system administrator may change the
standard directories locations, i.e. the Windows directory may be
named W:\Win2003 instead of the default C:\Windows.

The strings appname and username should be replaced with the value
returned by `wx.App.GetAppName` and the name of the currently logged
in user, respectively. The string prefix is only used under Unix and
is /usr/local by default but may be changed using `SetInstallPrefix`.

The directories returned by the methods of this class may or may not
exist. If they don't exist, it's up to the caller to create them,
wxStandardPaths doesn't do it.

Finally note that these functions only work with standardly packaged
applications. I.e. under Unix you should follow the standard
installation conventions and under Mac you should create your
application bundle according to the Apple guidelines. Again, this
class doesn't help you to do it.", "");

class wxStandardPaths
{
public:

    DocStr(
        Get,
        "Return the global standard paths singleton", "");    
    %extend {
        static wxStandardPaths* Get() {
            return (wxStandardPaths*) &wxStandardPaths::Get();
        }
    }


    
    DocDeclStr(
        virtual wxString , GetConfigDir() const,
        "Return the directory with system config files: /etc under Unix,
'c:\\Documents and Settings\\All Users\\Application Data' under Windows,
/Library/Preferences for Mac", "");

    
    DocDeclStr(
        virtual wxString , GetUserConfigDir() const,
        "Return the directory for the user config files: $HOME under Unix,
'c:\\Documents and Settings\\username' under Windows, and 
~/Library/Preferences under Mac
    
Only use this if you have a single file to put there, otherwise
`GetUserDataDir` is more appropriate", "");

    
    DocDeclStr(
        virtual wxString , GetDataDir() const,
        "Return the location of the application's global, (i.e. not
user-specific,) data files: prefix/share/appname under Unix,
'c:\\Program Files\\appname' under Windows,
appname.app/Contents/SharedSupport app bundle directory under Mac.", "");

    
    DocDeclStr(
        virtual wxString , GetLocalDataDir() const,
        "Return the location for application data files which are
host-specific.  Same as `GetDataDir` except under Unix where it is
/etc/appname", "");


    DocDeclStr(
        virtual wxString , GetUserDataDir() const,
        "Return the directory for the user-dependent application data files:
$HOME/.appname under Unix, c:\\Documents and
Settings\\username\\Application Data\\appname under Windows and
~/Library/Application Support/appname under Mac", "");
    

    DocDeclStr(
        virtual wxString , GetUserLocalDataDir() const,
        "Return the directory for user data files which shouldn't be shared
with the other machines

Same as `GetUserDataDir` for all platforms except Windows where it is
the 'Local Settings\\Application Data\\appname' directory.", "");


    DocDeclStr(
        virtual wxString , GetPluginsDir() const,
        "Return the directory where the loadable modules (plugins) live:
prefix/lib/appname under Unix, program directory under Windows and
Contents/Plugins app bundle subdirectory under Mac", "");

    
#ifdef __WXGTK__
    DocDeclStr(
        void , SetInstallPrefix(const wxString& prefix),
        "Set the program installation directory which is /usr/local by default.
This value will be used by other methods such as `GetDataDir` and
`GetPluginsDir` as the prefix for what they return. (This function
only has meaning on Unix systems.)", "");
    

    DocDeclStr(
        wxString , GetInstallPrefix() const,
        "Get the program installation prefix. The default is the prefix where
Python is installed. (This function only has meaning on Unix systems.)", "");
#else
    %extend {
        void SetInstallPrefix(const wxString& prefix) {}
        wxString GetInstallPrefix() { return wxEmptyString; }
    }
#endif
};


//---------------------------------------------------------------------------
