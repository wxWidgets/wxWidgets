/////////////////////////////////////////////////////////////////////////////
// Name:        config.h
// Purpose:     interface of wxConfigBase
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxConfigBase
    @wxheader{config.h}

    wxConfigBase class defines the basic interface of all config classes. It can
    not be used by itself (it is an abstract base class) and you will always use one
    of its derivations: wxFileConfig,
    wxRegConfig or any other.

    However, usually you don't even need to know the precise nature of the class
    you're working with but you would just use the wxConfigBase methods. This
    allows you to write the same code regardless of whether you're working with
    the registry under Win32 or text-based config files under Unix (or even
    Windows 3.1 .INI files if you're really unlucky). To make writing the portable
    code even easier, wxWidgets provides a typedef wxConfig
    which is mapped onto the native wxConfigBase implementation on the given
    platform: i.e. wxRegConfig under Win32 and
    wxFileConfig otherwise.

    See @ref overview_wxconfigoverview "config overview" for the descriptions of all
    features of this class.

    It is highly recommended to use static functions @e Get() and/or @e Set(),
    so please have a @ref overview_wxconfigstaticfunctions "look at them."

    @library{wxbase}
    @category{misc}
*/
class wxConfigBase : public wxObject
{
public:
    /**
        )
        This is the default and only constructor of the wxConfigBase class, and
        derived classes.

        @param appName
            The application name. If this is empty, the class will
            normally use wxApp::GetAppName to set it. The
            application name is used in the registry key on Windows, and can be used to
            deduce the local filename parameter if that is missing.
        @param vendorName
            The vendor name. If this is empty, it is assumed that
            no vendor name is wanted, if this is optional for the current config class.
            The vendor name is appended to the application name for wxRegConfig.
        @param localFilename
            Some config classes require a local filename. If this
            is not present, but required, the application name will be used instead.
        @param globalFilename
            Some config classes require a global filename. If
            this is not present, but required, the application name will be used
        instead.
        @param style
            Can be one of wxCONFIG_USE_LOCAL_FILE and
            wxCONFIG_USE_GLOBAL_FILE. The style interpretation depends on the config
            class and is ignored by some implementations. For wxFileConfig, these styles
            determine whether a local or global config file is created or used: if
            wxCONFIG_USE_GLOBAL_FILE is used, then settings are read from the global
            config file and if wxCONFIG_USE_LOCAL_FILE is used, settings are read from
            and written to local config file (if they are both set, global file is read
            first, then local file, overwriting global settings). If the
            flag is present but the parameter is empty, the parameter will be set to a
            default. If the parameter is present but the style flag not, the relevant
        flag
            will be added to the style. For wxRegConfig, thie GLOBAL flag refers to HKLM
            key while LOCAL one is for the usual HKCU one.
            For wxFileConfig you can also add wxCONFIG_USE_RELATIVE_PATH by logically
            or'ing it to either of the _FILE options to tell wxFileConfig to use
        relative
            instead of absolute paths.
            On non-VMS Unix systems, the default local configuration file is ~/.appname.
            However, this path may be also used as user data directory
            (see wxStandardPaths::GetUserDataDir) if
            the application has several data files. In this case wxCONFIG_USE_SUBDIR
            flag, which changes the default local configuration file to
        ~/.appname/appname
            should be used. Notice that this flag is ignored if localFilename is
            provided.
        @wxsince{2.8.2}
            For wxFileConfig, you can also add wxCONFIG_USE_NO_ESCAPE_CHARACTERS which
            will turn off character escaping for the values of entries stored in the
        config
            file: for example a foo key with some backslash characters will be stored
            as foo=C:\mydir instead of the usual storage of
            foo=C:\\mydir.
            The wxCONFIG_USE_NO_ESCAPE_CHARACTERS style can be helpful if your config
            file must be read or written to by a non-wxWidgets program (which might not
            understand the escape characters). Note, however, that if
            wxCONFIG_USE_NO_ESCAPE_CHARACTERS style is used, it is is now
            your application's responsibility to ensure that there is no newline or
            other illegal characters in a value, before writing that value to the file.
        @param conv
            This parameter is only used by wxFileConfig when compiled
            in Unicode mode. It specifies the encoding in which the configuration file
            is written.

        @remarks By default, environment variable expansion is on and recording
                 defaults is off.
    */
    wxConfigBase(const wxString& appName = wxEmptyString,
                 const wxString& vendorName = wxEmptyString,
                 const wxString& localFilename = wxEmptyString,
                 const wxString& globalFilename = wxEmptyString,
                 long style = 0);

    /**
        Empty but ensures that dtor of all derived classes is virtual.
    */
    ~wxConfigBase();

    /**
        @ref ctor() wxConfigBase

        @ref dtor() ~wxConfigBase
    */


    /**
        Create a new config object: this function will create the "best"
        implementation of wxConfig available for the current platform, see comments
        near the definition of wxCONFIG_WIN32_NATIVE for details. It returns the
        created object and also sets it as the current one.
    */
    static wxConfigBase* Create();

    /**
        The functions in this section delete entries and/or groups of entries from the
        config file. @e DeleteAll() is especially useful if you want to erase all
        traces of your program presence: for example, when you uninstall it.
        DeleteEntry()

        DeleteGroup()

        DeleteAll()
    */


    /**
        Delete the whole underlying object (disk file, registry key, ...). Primarly
        for use by uninstallation routine.
    */
    bool DeleteAll();

    /**
        Deletes the specified entry and the group it belongs to if it was the last key
        in it and the second parameter is @true.
    */
    bool DeleteEntry(const wxString& key,
                     bool bDeleteGroupIfEmpty = true);

    /**
        Delete the group (with all subgroups). If the current path is under the group
        being deleted it is changed to its deepest still existing component. E.g. if
        the current path is @c /A/B/C/D and the group @c C is deleted the
        path becomes @c /A/B.
    */
    bool DeleteGroup(const wxString& key);

    /**
        Calling this function will prevent @e Get() from automatically creating a
        new config object if the current one is @NULL. It might be useful to call it
        near the program end to prevent "accidental" creation of a new config object.
    */
    void DontCreateOnDemand();

    /**
        The functions in this section allow to enumerate all entries and groups in the
        config file. All functions here return @false when there are no more items.
        You must pass the same index to GetNext and GetFirst (don't modify it).
        Please note that it is @b not the index of the current item (you will have
        some great surprises with wxRegConfig if you assume this) and you shouldn't
        even look at it: it is just a "cookie" which stores the state of the
        enumeration. It can't be stored inside the class because it would prevent you
        from running several enumerations simultaneously, that's why you must pass it
        explicitly.
        Having said all this, enumerating the config entries/groups is very simple:

        There are also functions to get the number of entries/subgroups without
        actually enumerating them, but you will probably never need them.
        GetFirstGroup()

        GetNextGroup()

        GetFirstEntry()

        GetNextEntry()

        GetNumberOfEntries()

        GetNumberOfGroups()
    */


    /**
        returns @true if either a group or an entry with a given name exists
    */
    bool Exists(wxString& strName) const;

    /**
        permanently writes all changes (otherwise, they're only written from object's
        destructor)
    */
    bool Flush(bool bCurrentOnly = false);

    /**
        Get the current config object. If there is no current object and
        @a CreateOnDemand is @true, creates one
        (using @e Create) unless DontCreateOnDemand was called previously.
    */
    static wxConfigBase* Get(bool CreateOnDemand = true);

    /**
        Returns the application name.
    */
    wxString GetAppName() const;

    /**
        Returns the type of the given entry or @e Unknown if the entry doesn't
        exist. This function should be used to decide which version of Read() should
        be used because some of wxConfig implementations will complain about type
        mismatch otherwise: e.g., an attempt to read a string value from an integer
        key with wxRegConfig will fail.
        The result is an element of enum EntryType:
    */
    enum wxConfigBase::EntryType GetEntryType(const wxString& name) const;

    /**
        Gets the first entry.
    */
    bool GetFirstEntry(wxString& str, long& index) const;

    /**
        Gets the first group.
    */
    bool GetFirstGroup(wxString& str, long& index) const;

    /**
        Gets the next entry.
    */
    bool GetNextEntry(wxString& str, long& index) const;

    /**
        Gets the next group.
    */
    bool GetNextGroup(wxString& str, long& index) const;

    /**

    */
    uint GetNumberOfEntries(bool bRecursive = false) const;

    /**
        Get number of entries/subgroups in the current group, with or without its
        subgroups.
    */
    uint GetNumberOfGroups(bool bRecursive = false) const;

    /**
        Retrieve the current path (always as absolute path).
    */
    const wxString GetPath() const;

    /**
        Returns the vendor name.
    */
    wxString GetVendorName() const;

    /**
        returns @true if the entry by this name exists
    */
    bool HasEntry(wxString& strName) const;

    /**
        returns @true if the group by this name exists
    */
    bool HasGroup(const wxString& strName) const;

    /**
        Returns @true if we are expanding environment variables in key values.
    */
    bool IsExpandingEnvVars() const;

    /**
        Returns @true if we are writing defaults back to the config file.
    */
    bool IsRecordingDefaults() const;

    /**
        These function are the core of wxConfigBase class: they allow you to read and
        write config file data. All @e Read function take a default value which
        will be returned if the specified key is not found in the config file.
        Currently, supported types of data are:
        wxString, @e long, @e double, @e bool,
        wxColour and any other types,
        for which functions wxToString()
        and wxFromString() are defined.
        Try not to read long values into string variables and vice versa: although it
        just might work with wxFileConfig, you will get a system error with
        wxRegConfig because in the Windows registry the different types of entries are
        indeed used.
        Final remark: the @e szKey parameter for all these functions can contain an
        arbitrary path (either relative or absolute), not just the key name.
        Read()

        Write()

        Flush()
    */


    /**
        GetAppName()

        GetVendorName()

        wxFileConfig::SetUmask
    */


    /**
        Some aspects of wxConfigBase behaviour can be changed during run-time. The
        first of them is the expansion of environment variables in the string values
        read from the config file: for example, if you have the following in your
        config file:

        the call to @c config-Read("UserData") will return something like
        @c "/home/zeitlin/data" if you're lucky enough to run a Linux system ;-)
        Although this feature is very useful, it may be annoying if you read a value
        which containts '$' or '%' symbols (% is used for environment variables
        expansion under Windows) which are not used for environment variable
        expansion. In this situation you may call SetExpandEnvVars(@false) just before
        reading this value and SetExpandEnvVars(@true) just after. Another solution
        would be to prefix the offending symbols with a backslash.
        The following functions control this option:
        IsExpandingEnvVars()

        SetExpandEnvVars()

        SetRecordDefaults()

        IsRecordingDefaults()
    */


    /**
        As explained in @ref overview_wxconfigoverview "config overview", the config
        classes
        support a file system-like hierarchy of keys (files) and groups (directories).
        As in the file system case, to specify a key in the config class you must use
        a path to it. Config classes also support the notion of the current group,
        which makes it possible to use the relative paths. To clarify all this, here
        is an example (it is only for the sake of demonstration, it doesn't do anything
        sensible!):

        @e Warning: it is probably a good idea to always restore the path to its
        old value on function exit:

        because otherwise the assert in the following example will surely fail
        (we suppose here that @e foo() function is the same as above except that it
        doesn't save and restore the path):

        Finally, the path separator in wxConfigBase and derived classes is always '/',
        regardless of the platform (i.e. it is @b not '\\' under Windows).
        SetPath()

        GetPath()
    */


    //@{
    /**
        Reads a value of type T, for which function
        wxFromString() is defined,
        returning @true if the value was found.
        If the value was not found, @a defaultVal is used instead.
        bool Read(const wxStringkey, T* value) const;


        @b Read(key, default="")

        Returns a string

        @b ReadInt(key, default=0)

        Returns an integer

        @b ReadFloat(key, default=0.0)

        Returns a floating point number

        @b ReadBool(key, default=0)

        Returns a boolean
    */
    bool Read(const wxString& key, wxString* str) const;
    const bool Read(const wxString& key, wxString* str,
                    const wxString& defaultVal) const;
    const wxString  Read(const wxString& key,
                         const
                         wxString& defaultVal) const;
    const bool Read(const wxString& key, long* l) const;
    const bool Read(const wxString& key, long* l,
                    long defaultVal) const;
    const bool Read(const wxString& key, double* d) const;
    const bool Read(const wxString& key, double* d,
                    double defaultVal) const;
    const bool Read(const wxString& key, bool* b) const;
    const bool Read(const wxString& key, bool* d,
                    bool defaultVal) const;
    const bool Read(const wxString& key, wxMemoryBuffer* buf) const;
    const bool Read(const wxString& key, T* value) const;
    const bool Read(const wxString& key, T* value,
                    T const& defaultVal) const;
    //@}

    /**
        Reads a bool value from the key and returns it. @a defaultVal is returned
        if the key is not found.
    */
    long ReadBool(const wxString& key, bool defaultVal) const;

    /**
        Reads a double value from the key and returns it. @a defaultVal is returned
        if the key is not found.
    */
    long ReadDouble(const wxString& key, double defaultVal) const;

    /**
        Reads a long value from the key and returns it. @a defaultVal is returned
        if the key is not found.
    */
    long ReadLong(const wxString& key, long defaultVal) const;

    /**
        Reads a value of type T, for which function
        wxFromString() is defined, from the key and returns it.
        @a defaultVal is returned if the key is not found.
    */
    T ReadObject(const wxString& key, T const& defaultVal) const;

    /**
        The functions in this section allow to rename entries or subgroups of the
        current group. They will return @false on error. typically because either the
        entry/group with the original name doesn't exist, because the entry/group with
        the new name already exists or because the function is not supported in this
        wxConfig implementation.
        RenameEntry()

        RenameGroup()
    */


    /**
        Renames an entry in the current group. The entries names (both the old and
        the new one) shouldn't contain backslashes, i.e. only simple names and not
        arbitrary paths are accepted by this function.
        Returns @false if @a oldName doesn't exist or if @a newName already
        exists.
    */
    bool RenameEntry(const wxString& oldName,
                     const wxString& newName);

    /**
        Renames a subgroup of the current group. The subgroup names (both the old and
        the new one) shouldn't contain backslashes, i.e. only simple names and not
        arbitrary paths are accepted by this function.
        Returns @false if @a oldName doesn't exist or if @a newName already
        exists.
    */
    bool RenameGroup(const wxString& oldName,
                     const wxString& newName);

    /**
        Sets the config object as the current one, returns the pointer to the previous
        current object (both the parameter and returned value may be @NULL)
    */
    static wxConfigBase* Set(wxConfigBase* pConfig);

    /**
        Determine whether we wish to expand environment variables in key values.
    */
    void SetExpandEnvVars(bool bDoIt = true);

    /**
        Set current path: if the first character is '/', it is the absolute path,
        otherwise it is a relative path. '..' is supported. If strPath doesn't
        exist it is created.
    */
    void SetPath(const wxString& strPath);

    /**
        Sets whether defaults are recorded to the config file whenever an attempt to
        read the value which is not present in it is done.
        If on (default is off) all default values for the settings used by the program
        are written back to the config file. This allows the user to see what config
        options may be changed and is probably useful only for wxFileConfig.
    */
    void SetRecordDefaults(bool bDoIt = true);

    /**
        These functions deal with the "default" config object. Although its usage is
        not at all mandatory it may be convenient to use a global config object
        instead of creating and deleting the local config objects each time you need
        one (especially because creating a wxFileConfig object might be a time
        consuming operation). In this case, you may create this global config object
        in the very start of the program and @e Set() it as the default. Then, from
        anywhere in your program, you may access it using the @e Get() function.
        This global wxConfig object will be deleted by wxWidgets automatically if it
        exists. Note that this implies that if you do delete this object yourself
        (usually in wxApp::OnExit) you must use @e Set(@NULL)
        to prevent wxWidgets from deleting it the second time.
        As it happens, you may even further simplify the procedure described above:
        you may forget about calling @e Set(). When @e Get() is called and there
        is no current object, it will create one using @e Create() function. To
        disable this behaviour @e DontCreateOnDemand() is provided.
        @b Note: You should use either @e Set() or @e Get() because wxWidgets
        library itself would take advantage of it and could save various information
        in it. For example wxFontMapper or Unix version
        of wxFileDialog have the ability to use wxConfig class.
        Set()

        Get()

        Create()

        DontCreateOnDemand()
    */


    /**
        HasGroup()

        HasEntry()

        Exists()

        GetEntryType()
    */


    //@{
    /**
        These functions write the specified value to the config file and return @true
        on success. In the last one, function wxToString() must be
        defined for type @e T.


        @b Write(key, value)

        Writes a string

        @b WriteInt(key, value)

        Writes an integer

        @b WriteFloat(key, value)

        Writes a floating point number

        @b WriteBool(key, value)

        Writes a boolean
    */
    bool Write(const wxString& key, const wxString& value);
    bool Write(const wxString& key, long value);
    bool Write(const wxString& key, double value);
    bool Write(const wxString& key, bool value);
    bool Write(const wxString& key, const wxMemoryBuffer& buf);
    bool Write(const wxString& key, const T& buf);
    //@}
};

