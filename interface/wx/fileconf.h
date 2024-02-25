/////////////////////////////////////////////////////////////////////////////
// Name:        fileconf.h
// Purpose:     interface of wxFileConfig
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFileConfig

    wxFileConfig implements wxConfigBase interface for
    storing and retrieving configuration information using plain text files. The
    files have a simple format reminiscent of Windows INI files with lines of the
    form @c "key = value" defining the keys and lines of special form
    @c "[group]" indicating the start of each group.

    This class is used by default for wxConfig on Unix platforms but may also be
    used explicitly if you want to use files and not the registry even under
    Windows.

    @section fileconf_paths Configuration Files Paths

    The default path for local (or user) configuration file is `~/.appname`,
    i.e. it is stored directly in the user home directory. This default
    path is backwards-compatible but not recommended any more and it is advised
    to call wxStandardPaths::SetFileLayout() with
    wxStandardPaths::FileLayout_XDG parameter to change the default path to
    `~/.config/appname.conf`. MigrateLocalFile() may be helpful for moving the
    existing configuration file to the new location.

    Alternatively, it is possible to specify ::wxCONFIG_USE_XDG flag in the
    style parameter of the constructor to use this XDG-compliant path without
    changing the global file layout.

    And for the programs using multiple configuration files it is recommended
    to use both ::wxCONFIG_USE_XDG and ::wxCONFIG_USE_SUBDIR which change the
    default file path to `~/.config/appname/appname.conf` -- and allow the
    program to store other files in the same `~/.config/appname` directory.

    @library{wxbase}
    @category{cfg}

    @see wxFileConfig::Save
*/
class wxFileConfig : public wxConfigBase
{
public:
    /**
        Constructor allowing to choose the file names to use.

        If @a localFilename and/or @a globalFilename are explicitly specified,
        they are used as the names of the user and system-wide configuration
        files (the latter is only read by the program while the former is read
        from and written to). Otherwise the behaviour depends on @a style
        parameter. If it includes ::wxCONFIG_USE_LOCAL_FILE, then the local
        file name is constructed from the information in @a appName and @a
        vendorName arguments in a system-dependent way. If
        ::wxCONFIG_USE_GLOBAL_FILE is not specified at all (and @a
        globalFilename is empty) then the system-wide file is not used at all.
        Otherwise its name and path are also constructed in the way appropriate
        for the current platform from the application and vendor names.
     */
    wxFileConfig(const wxString& appName = wxEmptyString,
               const wxString& vendorName = wxEmptyString,
               const wxString& localFilename = wxEmptyString,
               const wxString& globalFilename = wxEmptyString,
               long style = wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_GLOBAL_FILE,
               const wxMBConv& conv = wxConvAuto());

    /**
        Read the config data from the specified stream instead of the associated file,
        as usual.

        @see Save()
    */
    wxFileConfig(wxInputStream& is, const wxMBConv& conv = wxConvAuto());

    /**
        Return the full path to the file which would be used by wxFileConfig as global,
        system-wide, file if it were constructed with @a basename as "global filename"
        parameter in the constructor.

        Notice that this function cannot be used if @a basename is already a full path name.
    */
    static wxFileName GetGlobalFile(const wxString& basename);

    /**
        Return the full path to the file which would be used by wxFileConfig as local,
        user-specific, file if it were constructed with @a basename as "local filename"
        parameter in the constructor.

        @a style has the same meaning as in @ref wxConfigBase::wxConfigBase "wxConfig constructor"
        and can contain any combination of styles but only wxCONFIG_USE_SUBDIR,
        wxCONFIG_USE_XDG and wxCONFIG_USE_HOME are really used by this function.

        Notice that this function cannot be used if @a basename is already a full path name.
    */
    static wxFileName GetLocalFile(const wxString& basename, int style = 0);

    static wxString GetGlobalFileName(const wxString& szFile);
    static wxString GetLocalFileName(const wxString& szFile, int style = 0);

    /**
        Contains return value of MigrateLocalFile().

        @since 3.3.0
    */
    struct MigrationResult
    {
        /// If empty, it means the old file wasn't found and nothing was done.
        wxString oldPath;

        /// The name of the new file.
        wxString newPath;

        /// If empty, means the file was successfully migrated.
        wxString error;
    };

    /**
        Move the existing configuration file to a new location.

        This function is useful for moving legacy configuration files in
        `~/.appname` or `~/.appname/appname.conf` to the XDG-compliant location
        under `~/.config`. To do this, simply specify ::wxCONFIG_USE_XDG as
        part of @a newStyle.

        The returned MigrationResult object describes what, if anything, was
        done: if its `oldPath` member is empty, it means that the file
        corresponding to @a oldStyle was not found and nothing was done.
        Otherwise, if its `error` member is empty, the old file was found and
        moved to `newPath`. And if `error` is not empty, it contains the
        user-readable error message describing why moving the file failed.

        Typical example of using this function is shown in the widgets sample:
        @code
        // Execute this early during the application startup, before the
        // global wxConfig object is created.
        const auto res = wxFileConfig::MigrateLocalFile("app", wxCONFIG_USE_XDG);
        if ( !res.oldPath.empty() ) {
            if ( res.error.empty() ) {
                wxLogMessage("Config file was migrated from \"%s\" to \"%s\"",
                             res.oldPath, res.newPath);
            } else {
                wxLogWarning("Migrating old config failed: %s.", res.error);
            }
        }

        // Prefer doing it only after successfully calling MigrateLocalFile(),
        // otherwise, i.e. if it failed, the old config file wouldn't be used.
        wxStandardPaths::Get().SetFileLayout(wxStandardPaths::FileLayout_XDG);
        @endcode

        @param name Name of the configuration file.
        @param newStyle Style which is used by the current version of the
            program, typically including ::wxCONFIG_USE_XDG and possibly also
            including ::wxCONFIG_USE_SUBDIR.
        @param oldStyle Style which was used by the previous versions of the
            program, possibly including ::wxCONFIG_USE_SUBDIR and typically
            including ::wxCONFIG_USE_HOME.

        @since 3.3.0
    */
    static MigrationResult
    MigrateLocalFile(const wxString& name,
                     int newStyle,
                     int oldStyle = wxCONFIG_USE_HOME);

    /**
        Saves all config data to the given stream, returns @true if data was saved
        successfully or @false on error.

        Note the interaction of this function with the internal "dirty flag": the
        data is saved unconditionally, i.e. even if the object is not dirty. However
        after saving it successfully, the dirty flag is reset so no changes will be
        written back to the file this object is associated with until you change its
        contents again.

        @see wxConfigBase::Flush
    */
    virtual bool Save(wxOutputStream& os, const wxMBConv& conv = wxConvAuto());

    /**
        Enables saving data to the disk file when this object is destroyed.

        This is the default behaviour and this function doesn't need to be
        called explicitly unless DisableAutoSave() had been previously called.

        @since 3.1.3
    */
    void EnableAutoSave();

    /**
        Prevent this object from saving data to the disk file when it is
        destroyed.

        By default, changes to this object are only saved permanently when
        Flush() is explicitly called or when it is destroyed. If this method is
        called, Flush() won't be called automatically from the destructor,
        meaning that any non-explicitly-flushed changes will be lost.

        @since 3.1.3
    */
    void DisableAutoSave();

    /**
        Allows setting the mode to be used for the config file creation. For example, to
        create a config file which is not readable by other users (useful if it stores
        some sensitive information, such as passwords), you could use @c SetUmask(0077).

        This function doesn't do anything on non-Unix platforms.

        @see wxCHANGE_UMASK()
    */
    void SetUmask(int mode);

  // implement inherited pure virtual functions
  virtual void SetPath(const wxString& strPath);
  virtual const wxString& GetPath() const;

  virtual bool GetFirstGroup(wxString& str, long& lIndex) const;
  virtual bool GetNextGroup (wxString& str, long& lIndex) const;
  virtual bool GetFirstEntry(wxString& str, long& lIndex) const;
  virtual bool GetNextEntry (wxString& str, long& lIndex) const;

  virtual size_t GetNumberOfEntries(bool bRecursive = false) const;
  virtual size_t GetNumberOfGroups(bool bRecursive = false) const;

  virtual bool HasGroup(const wxString& strName) const;
  virtual bool HasEntry(const wxString& strName) const;

  virtual bool Flush(bool bCurrentOnly = false);

  virtual bool RenameEntry(const wxString& oldName, const wxString& newName);
  virtual bool RenameGroup(const wxString& oldName, const wxString& newName);

  virtual bool DeleteEntry(const wxString& key, bool bGroupIfEmptyAlso = true);
  virtual bool DeleteGroup(const wxString& szKey);
  virtual bool DeleteAll();
};

