/////////////////////////////////////////////////////////////////////////////
// Name:        stdpaths.h
// Purpose:     interface of wxStandardPaths
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStandardPaths

    wxStandardPaths returns the standard locations in the file system and should be
    used by applications to find their data files in a portable way.

    In the description of the methods below, the example return values are given
    for the Unix, Windows and Mac OS X systems, however please note that these are
    just the examples and the actual values may differ. For example, under Windows:
    the system administrator may change the standard directories locations, i.e.
    the Windows directory may be named @c W:\\Win2003 instead of
    the default @c C:\\Windows.

    The strings @c appname and @c username should be
    replaced with the value returned by wxApp::GetAppName
    and the name of the currently logged in user, respectively. The string
    @c prefix is only used under Unix and is @c /usr/local by
    default but may be changed using wxStandardPaths::SetInstallPrefix.

    The directories returned by the methods of this class may or may not exist. If
    they don't exist, it's up to the caller to create them, wxStandardPaths doesn't
    do it.

    Finally note that these functions only work with standardly packaged
    applications. I.e. under Unix you should follow the standard installation
    conventions and under Mac you should create your application bundle according
    to the Apple guidelines. Again, this class doesn't help you to do it.

    This class is MT-safe: its methods may be called concurrently from different
    threads without additional locking.

    Note that you don't allocate an instance of class wxStandardPaths, but retrieve the 
    global standard paths object using @c wxStandardPaths::Get on which you call the 
    desired methods.

    @library{wxbase}
    @category{file}

    @see wxFileConfig
        */
class wxStandardPaths
{
public:
    /**
        Returns reference to the unique global standard paths object.
    */
    static wxStandardPathsBase Get();

    /**
        Return the directory containing the system config files.
        Example return values:
             - Unix: @c /etc
             - Windows: @c C:\\Documents @c and @c Settings\\All @c Users\\Application Data
             - Mac: @c /Library/Preferences

        @see wxFileConfig
    */
    virtual wxString GetConfigDir() const;

    /**
        Return the location of the applications global, i.e. not user-specific,
        data files.
        Example return values:
             - Unix: @c prefix/share/appname
             - Windows: the directory where the executable file is located
             - Mac: @c appname.app/Contents/SharedSupport bundle subdirectory

        @see GetLocalDataDir()
    */
    virtual wxString GetDataDir() const;

    /**
        Return the directory containing the current user's documents.
        Example return values:
             - Unix: @c ~ (the home directory)
             - Windows: @c C:\\Documents @c and @c Settings\\username\\Documents
             - Mac: @c ~/Documents

        @since 2.7.0
    */
    virtual wxString GetDocumentsDir() const;

    /**
        Return the directory and the filename for the current executable.
        Example return values:
             - Unix: @c /usr/local/bin/exename
             - Windows: @c C:\\Programs\\AppFolder\\exename.exe
             - Mac: @c /Programs/exename
    */
    virtual wxString GetExecutablePath() const;

    /**
        @note This function is only available under Unix.
        Return the program installation prefix, e.g. @c /usr, @c /opt or
        @c /home/zeitlin.
        If the prefix had been previously by SetInstallPrefix(), returns that
        value, otherwise tries to determine it automatically (Linux only right
        now) and finally returns the default @c /usr/local value if it failed.
    */
    wxString GetInstallPrefix() const;

    /**
        Return the location for application data files which are host-specific and
        can't, or shouldn't, be shared with the other machines.
        This is the same as GetDataDir() except
        under Unix where it returns @c /etc/appname.
    */
    virtual wxString GetLocalDataDir() const;

    /**
        Return the localized resources directory containing the resource files of the
        specified category for the given language.
        In general this is just the same as @a lang subdirectory of
        GetResourcesDir() (or @c lang.lproj under Mac OS X) but is something quite
        different for message catalog category under Unix where it returns the standard
        @c prefix/share/locale/lang/LC_MESSAGES directory.

        @since 2.7.0
    */
    wxString GetLocalizedResourcesDir(const wxString& lang,
                                      ResourceCat category = ResourceCat_None) const;

    /**
        Return the directory where the loadable modules (plugins) live.
        Example return values:
             - Unix: @c prefix/lib/appname
             - Windows: the directory of the executable file
             - Mac: @c appname.app/Contents/PlugIns bundle subdirectory

        @see wxDynamicLibrary
    */
    virtual wxString GetPluginsDir() const;

    /**
        Return the directory where the application resource files are located. The
        resources are the auxiliary data files needed for the application to run and
        include, for example, image and sound files it might use.
        This function is the same as GetDataDir() for
        all platforms except Mac OS X.
        Example return values:
             - Unix: @c prefix/share/@e appname
             - Windows: the directory where the executable file is located
             - Mac: @c appname.app/Contents/Resources bundle subdirectory

        @since 2.7.0

        @see GetLocalizedResourcesDir()
    */
    virtual wxString GetResourcesDir() const;

    /**
        Return the directory for storing temporary files. To create unique temporary
        files,
        it is best to use wxFileName::CreateTempFileName for correct behaviour when
        multiple processes are attempting to create temporary files.

        @since 2.7.2
    */
    virtual wxString GetTempDir() const;

    /**
        Return the directory for the user config files:
             - Unix: @c ~ (the home directory)
             - Windows: @c C:\\Documents @c and @c Settings\\username\\Application Data
             - Mac: @c ~/Library/Preferences
        Only use this method if you have a single configuration file to put in this
        directory, otherwise GetUserDataDir() is
        more appropriate.
    */
    virtual wxString GetUserConfigDir() const;

    /**
        Return the directory for the user-dependent application data files:
             - Unix: @c ~/.appname
             - Windows: @c C:\\Documents @c and @c Settings\\username\\Application @c Data\\appname
             - Mac: @c ~/Library/Application @c Support/appname
    */
    virtual wxString GetUserDataDir() const;

    /**
        Return the directory for user data files which shouldn't be shared with
        the other machines.
        This is the same as GetUserDataDir() for all platforms except Windows where it returns
        @c C:\\Documents @c and @c Settings\\username\\Local @c Settings\\Application @c Data\\appname
    */
    virtual wxString GetUserLocalDataDir() const;

    /**
        @note This function is only available under Unix.
        Lets wxStandardPaths know about the real program installation prefix on a Unix
        system. By default, the value returned by
        GetInstallPrefix() is used.
        Although under Linux systems the program prefix may usually be determined
        automatically, portable programs should call this function. Usually the prefix
        is set during program configuration if using GNU autotools and so it is enough
        to pass its value defined in @c config.h to this function.
    */
    void SetInstallPrefix(const wxString& prefix);

    /**
        Controls what application information is used when constructing paths that
        should be unique to this program, such as the application data directory, the
        plugins directory on Unix, etc.
        Valid values for @a info are @c AppInfo_None and either one or
        combination of @c AppInfo_AppName and @c AppInfo_VendorName. The
        first one tells this class to not use neither application nor vendor name in
        the paths.
        By default, only the application name is used under Unix systems but both
        application and vendor names are used under Windows and Mac.
    */
    void UseAppInfo(int info);
};

