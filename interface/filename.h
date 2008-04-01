/////////////////////////////////////////////////////////////////////////////
// Name:        filename.h
// Purpose:     interface of wxFileName
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFileName
    @wxheader{filename.h}

    wxFileName encapsulates a file name. This class serves two purposes: first, it
    provides the functions to split the file names into components and to recombine
    these components in the full file name which can then be passed to the OS file
    functions (and @ref overview_filefunctions "wxWidgets functions" wrapping them).
    Second, it includes the functions for working with the files itself. Note that
    to change the file data you should use wxFile class instead.
    wxFileName provides functions for working with the file attributes.

    When working with directory names (i.e. without filename and extension)
    make sure not to misuse the file name part of this class with the last
    directory. Instead initialize the wxFileName instance like this:

    @code
    wxFileName dirname( "C:\mydir", "" );
    MyMethod( dirname.GetPath() );
    @endcode

    The same can be done using the static method wxFileName::DirName:

    @code
    wxFileName dirname = wxFileName::DirName( "C:\mydir" );
    MyMethod( dirname.GetPath() );
    @endcode

    Accordingly, methods dealing with directories or directory names
    like wxFileName::IsDirReadable use
    wxFileName::GetPath whereas methods dealing
    with file names like wxFileName::IsFileReadable
    use wxFileName::GetFullPath.

    If it is not known wether a string contains a directory name or
    a complete file name (such as when interpreting user input) you need to use
    the static function wxFileName::DirExists
    (or its identical variants wxDir::Exists and
    wxDirExists()) and construct the wxFileName
    instance accordingly. This will only work if the directory actually exists,
    of course:

    @code
    wxString user_input;
    // get input from user

    wxFileName fname;
    if (wxDirExists(user_input))
        fname.AssignDir( user_input );
    else
        fname.Assign( user_input );
    @endcode

    @library{wxbase}
    @category{file}

    @see wxFileName::GetCwd
*/
class wxFileName
{
public:
    //@{
    /**
        Constructor from a volume name, a directory name, base file name and extension.
    */
    wxFileName();
    wxFileName(const wxFileName& filename);
    wxFileName(const wxString& fullpath,
               wxPathFormat format = wxPATH_NATIVE);
    wxFileName(const wxString& path, const wxString& name,
               wxPathFormat format = wxPATH_NATIVE);
    wxFileName(const wxString& path, const wxString& name,
               const wxString& ext,
               wxPathFormat format = wxPATH_NATIVE);
    wxFileName(const wxString& volume, const wxString& path,
               const wxString& name,
               const wxString& ext,
               wxPathFormat format = wxPATH_NATIVE);
    //@}

    /**
        Appends a directory component to the path. This component should contain a
        single directory name level, i.e. not contain any path or volume separators nor
        should it be empty, otherwise the function does nothing (and generates an
        assert failure in debug build).
    */
    void AppendDir(const wxString& dir);

    //@{
    /**
        Creates the file name from various combinations of data.
    */
    void Assign(const wxFileName& filepath);
    void Assign(const wxString& fullpath,
                wxPathFormat format = wxPATH_NATIVE);
    void Assign(const wxString& volume, const wxString& path,
                const wxString& name,
                const wxString& ext,
                bool hasExt,
                wxPathFormat format = wxPATH_NATIVE);
    void Assign(const wxString& volume, const wxString& path,
                const wxString& name,
                const wxString& ext,
                wxPathFormat format = wxPATH_NATIVE);
    void Assign(const wxString& path, const wxString& name,
                wxPathFormat format = wxPATH_NATIVE);
    void Assign(const wxString& path, const wxString& name,
                const wxString& ext,
                wxPathFormat format = wxPATH_NATIVE);
    //@}

    /**
        Makes this object refer to the current working directory on the specified
        volume (or current volume if @a volume is empty).

        @see GetCwd()
    */
    static void AssignCwd(const wxString& volume = wxEmptyString);

    /**
        Sets this file name object to the given directory name. The name and extension
        will be empty.
    */
    void AssignDir(const wxString& dir,
                   wxPathFormat format = wxPATH_NATIVE);

    /**
        Sets this file name object to the home directory.
    */
    void AssignHomeDir();

    /**
        The function calls CreateTempFileName() to
        create a temporary file and sets this object to the name of the file. If a
        temporary file couldn't be created, the object is put into the
        @ref isok() invalid state.
    */
    void AssignTempFileName(const wxString& prefix,
                            wxFile* fileTemp = NULL);

    /**
        Reset all components to default, uninitialized state.
    */
    void Clear();

    /**
        Removes the extension from the file name resulting in a
        file name with no trailing dot.

        @see SetExt(), SetEmptyExt()
    */
    void SetClearExt();

    /**
        Returns a temporary file name starting with the given @e prefix. If
        the @a prefix is an absolute path, the temporary file is created in this
        directory, otherwise it is created in the default system directory for the
        temporary files or in the current directory.
        If the function succeeds, the temporary file is actually created. If
        @a fileTemp is not @NULL, this file will be opened using the name of
        the temporary file. When possible, this is done in an atomic way ensuring that
        no race condition occurs between the temporary file name generation and opening
        it which could often lead to security compromise on the multiuser systems.
        If @a fileTemp is @NULL, the file is only created, but not opened.
        Under Unix, the temporary file will have read and write permissions for the
        owner only to minimize the security problems.

        @param prefix
            Prefix to use for the temporary file name construction
        @param fileTemp
            The file to open or @NULL to just get the name

        @returns The full temporary file name or an empty string on error.
    */
    static wxString CreateTempFileName(const wxString& prefix,
                                       wxFile* fileTemp = NULL);

    //@{
    /**
        Returns @true if the directory with this name exists.
    */
    bool DirExists();
    const static bool DirExists(const wxString& dir);
    //@}

    /**
        Returns the object corresponding to the directory with the given name.
        The @a dir parameter may have trailing path separator or not.
    */
    static wxFileName DirName(const wxString& dir,
                              wxPathFormat format = wxPATH_NATIVE);

    /**
        These functions allow to examine and modify the individual directories of the
        path:
        AppendDir()

        InsertDir()

        GetDirCount()
        PrependDir()

        RemoveDir()

        RemoveLastDir()
        To change the components of the file name individually you can use the
        following functions:
        GetExt()

        GetName()

        GetVolume()

        HasExt()

        HasName()

        HasVolume()

        SetExt()

        ClearExt()

        SetEmptyExt()

        SetName()

        SetVolume()
    */


    /**
        You can initialize a wxFileName instance using one of the following functions:
        @ref wxfilename() "wxFileName constructors"

        Assign()

        AssignCwd()

        AssignDir()

        AssignHomeDir()

        @ref assigntempfilename() AssignHomeTempFileName

        DirName()

        FileName()

        @ref operatorassign() "operator ="
    */


    /**
        wxFileName currently supports the file names in the Unix, DOS/Windows, Mac OS
        and VMS formats. Although these formats are quite different, wxFileName tries
        to treat them all in the same generic way. It supposes that all file names
        consist of the following parts: the volume (also known as drive under Windows
        or device under VMS), the path which is a sequence of directory names separated
        by the @ref getpathseparators() "path separators" and the full
        filename itself which, in turn, is composed from the base file name and the
        extension. All of the individual components of the file name may be empty and,
        for example, the volume name is always empty under Unix, but if they are all
        empty simultaneously, the filename object is considered to be in an invalid
        state and IsOk() returns @false for it.
        File names can be case-sensitive or not, the function
        IsCaseSensitive() allows to determine this.
        The rules for determining whether the file name is absolute or relative also
        depend on the file name format and the only portable way to answer this
        question is to use IsAbsolute() or
        IsRelative() method. Note that on Windows, "X:"
        refers to the current working directory on drive X. Therefore, a wxFileName
        instance constructed from for example "X:dir/file.ext" treats the portion
        beyond drive separator as being relative to that directory.
        To ensure that the filename is absolute, you may use
        MakeAbsolute(). There is also an inverse
        function MakeRelativeTo() which undoes
        what @ref normalize() Normalize(wxPATH_NORM_DOTS) does.
        Other functions returning information about the file format provided by this
        class are GetVolumeSeparator(),
        IsPathSeparator().
    */


    /**
        Before doing other tests, you should use IsOk() to
        verify that the filename is well defined. If it is,
        FileExists() can be used to test whether a file
        with such name exists and DirExists() can be used
        to test for directory existence.
        File names should be compared using SameAs() method
        or @ref operatorequal() "operator ==".
        For testing basic access modes, you can use:
        IsDirWritable()

        IsDirReadable()

        IsFileWritable()

        IsFileReadable()

        IsFileExecutable()
    */


    //@{
    /**
        Returns @true if the file with this name exists.

        @see DirExists()
    */
    bool FileExists();
    const static bool FileExists(const wxString& file);
    //@}

    /**
        Returns the file name object corresponding to the given @e file. This
        function exists mainly for symmetry with DirName().
    */
    static wxFileName FileName(const wxString& file,
                               wxPathFormat format = wxPATH_NATIVE);

    /**
        Retrieves the value of the current working directory on the specified volume. If
        the volume is empty, the program's current working directory is returned for the
        current volume.

        @returns The string containing the current working directory or an empty
                 string on error.

        @see AssignCwd()
    */
    static wxString GetCwd(const wxString& volume = "");

    /**
        Returns the number of directories in the file name.
    */
    size_t GetDirCount() const;

    /**
        Returns the directories in string array form.
    */
    const wxArrayString GetDirs() const;

    /**
        Returns the file name extension.
    */
    wxString GetExt() const;

    /**
        Returns the characters that can't be used in filenames and directory names for
        the specified format.
    */
    static wxString GetForbiddenChars(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the canonical path format for this platform.
    */
    static wxPathFormat GetFormat(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the full name (including extension but excluding directories).
    */
    wxString GetFullName() const;

    /**
        Returns the full path with name and extension.
    */
    wxString GetFullPath(wxPathFormat format = wxPATH_NATIVE) const;

    /**
        Returns the home directory.
    */
    static wxString GetHomeDir();

    //@{
    /**
        Returns the size of this file (first form) or the given number of bytes (second
        form)
        in a human-readable form.
        If the size could not be retrieved the @c failmsg string is returned (first
        form).
        If @c bytes is @c wxInvalidSize or zero, then @c nullsize is returned (second
        form).
        In case of success, the returned string is a floating-point number with @c
        precision decimal digits
        followed by the size unit (B, kB, MB, GB, TB: respectively bytes, kilobytes,
        megabytes, gigabytes, terabytes).
    */
    wxString GetHumanReadableSize(const wxString& failmsg = "Not available",
                                  int precision = 1);
    const static wxString  GetHumanReadableSize(const wxULongLong& bytes,
            const wxString& nullsize = "Not available",
            int precision = 1);
    //@}

    /**
        Return the long form of the path (returns identity on non-Windows platforms)
    */
    wxString GetLongPath() const;

    /**
        Returns the last time the file was last modified.
    */
    wxDateTime GetModificationTime() const;

    /**
        Returns the name part of the filename (without extension).

        @see GetFullName()
    */
    wxString GetName() const;

    /**
        Returns the path part of the filename (without the name or extension). The
        possible flags values are:

        @b wxPATH_GET_VOLUME

        Return the path with the volume (does
        nothing for the filename formats without volumes), otherwise the path without
        volume part is returned.

        @b wxPATH_GET_SEPARATOR

        Return the path with the trailing
        separator, if this flag is not given there will be no separator at the end of
        the path.
    */
    wxString GetPath(int flags = wxPATH_GET_VOLUME,
                     wxPathFormat format = wxPATH_NATIVE) const;

    /**
        Returns the usually used path separator for this format. For all formats but
        @c wxPATH_DOS there is only one path separator anyhow, but for DOS there
        are two of them and the native one, i.e. the backslash is returned by this
        method.

        @see GetPathSeparators()
    */
    static wxChar GetPathSeparator(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the string containing all the path separators for this format. For all
        formats but @c wxPATH_DOS this string contains only one character but for
        DOS and Windows both @c '/' and @c '\' may be used as
        separators.

        @see GetPathSeparator()
    */
    static wxString GetPathSeparators(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the string of characters which may terminate the path part. This is the
        same as GetPathSeparators() except for VMS
        path format where ] is used at the end of the path part.
    */
    static wxString GetPathTerminators(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the path with the trailing separator, useful for appending the name to
        the given path.
        This is the same as calling GetPath()
        @c (wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, format).
    */
    wxString GetPathWithSep(wxPathFormat format = wxPATH_NATIVE) const;

    /**
        Return the short form of the path (returns identity on non-Windows platforms).
    */
    wxString GetShortPath() const;

    //@{
    /**
        Returns the size of this file (first form) or the size of the given file
        (second form).
        If the file does not exist or its size could not be read (because e.g. the file
        is locked
        by another process) the returned value is @c wxInvalidSize.
    */
    wxULongLong GetSize();
    const static wxULongLong GetSize(const wxString& filename);
    //@}

    /**
        Returns the directory used for temporary files.
    */
    static wxString GetTempDir();

    /**
        Returns the last access, last modification and creation times. The last access
        time is updated whenever the file is read or written (or executed in the case
        of Windows), last modification time is only changed when the file is written
        to. Finally, the creation time is indeed the time when the file was created
        under Windows and the inode change time under Unix (as it is impossible to
        retrieve the real file creation time there anyhow) which can also be changed
        by many operations after the file creation.
        If no filename or extension is specified in this instance of wxFileName
        (and therefore IsDir() returns @true) then
        this function will return the directory times of the path specified by
        GetPath(), otherwise the file times of the
        file specified by GetFullPath().
        Any of the pointers may be @NULL if the corresponding time is not
        needed.

        @returns @true on success, @false if we failed to retrieve the times.
    */
    bool GetTimes(wxDateTime* dtAccess, wxDateTime* dtMod,
                  wxDateTime* dtCreate) const;

    /**
        Returns the string containing the volume for this file name, empty if it
        doesn't have one or if the file system doesn't support volumes at all (for
        example, Unix).
    */
    wxString GetVolume() const;

    /**
        Returns the string separating the volume from the path for this format.
    */
    static wxString GetVolumeSeparator(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns @true if an extension is present.
    */
    bool HasExt() const;

    /**
        Returns @true if a name is present.
    */
    bool HasName() const;

    /**
        Returns @true if a volume specifier is present.
    */
    bool HasVolume() const;

    /**
        Inserts a directory component before the zero-based position in the directory
        list. Please see AppendDir() for important notes.
    */
    void InsertDir(size_t before, const wxString& dir);

    /**
        Returns @true if this filename is absolute.
    */
    bool IsAbsolute(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns @true if the file names of this type are case-sensitive.
    */
    static bool IsCaseSensitive(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns @true if this object represents a directory, @false otherwise
        (i.e. if it is a file). Note that this method doesn't test whether the
        directory or file really exists, you should use
        DirExists() or
        FileExists() for this.
    */
    bool IsDir() const;

    //@{
    /**
        Returns @true if the directory component of this instance (or given @e dir)
        is an existing directory and this process has read permissions on it.
        Read permissions on a directory mean that you can list the directory contents
        but it
        doesn't imply that you have read permissions on the files contained.
    */
    bool IsDirReadable();
    const static bool IsDirReadable(const wxString& dir);
    //@}

    //@{
    /**
        Returns @true if the directory component of this instance (or given @e dir)
        is an existing directory and this process has write permissions on it.
        Write permissions on a directory mean that you can create new files in the
        directory.
    */
    bool IsDirWritable();
    const static bool IsDirWritable(const wxString& dir);
    //@}

    //@{
    /**
        Returns @true if a file with this name exists and if this process has execute
        permissions on it.
    */
    bool IsFileExecutable();
    const static bool IsFileExecutable(const wxString& file);
    //@}

    //@{
    /**
        Returns @true if a file with this name exists and if this process has read
        permissions on it.
    */
    bool IsFileReadable();
    const static bool IsFileReadable(const wxString& file);
    //@}

    //@{
    /**
        Returns @true if a file with this name exists and if this process has write
        permissions on it.
    */
    bool IsFileWritable();
    const static bool IsFileWritable(const wxString& file);
    //@}

    /**
        Returns @true if the filename is valid, @false if it is not
        initialized yet. The assignment functions and
        Clear() may reset the object to the uninitialized,
        invalid state (the former only do it on failure).
    */
    bool IsOk() const;

    /**
        Returns @true if the char is a path separator for this format.
    */
    static bool IsPathSeparator(wxChar ch,
                                wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns @true if this filename is not absolute.
    */
    bool IsRelative(wxPathFormat format = wxPATH_NATIVE);

    /**
        On Mac OS, gets the common type and creator for the given extension.
    */
    static bool MacFindDefaultTypeAndCreator(const wxString& ext,
            wxUint32* type,
            wxUint32* creator);

    /**
        On Mac OS, registers application defined extensions and their default type and
        creator.
    */
    static void MacRegisterDefaultTypeAndCreator(const wxString& ext,
            wxUint32 type,
            wxUint32 creator);

    /**
        On Mac OS, looks up the appropriate type and creator from the registration and
        then sets it.
    */
    bool MacSetDefaultTypeAndCreator();

    /**
        Make the file name absolute. This is a shortcut for
        @c wxFileName::Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE |
        wxPATH_NORM_TILDE, cwd, format).

        @see MakeRelativeTo(), Normalize(), IsAbsolute()
    */
    bool MakeAbsolute(const wxString& cwd = wxEmptyString,
                      wxPathFormat format = wxPATH_NATIVE);

    /**
        This function tries to put this file name in a form relative to

        @param pathBase.
        In other words, it returns the file name which should be used to access this
        file if the current directory were pathBase.

        pathBase
            the directory to use as root, current directory is used by
            default
        @param format
            the file name format, native by default

        @returns @true if the file name has been changed, @false if we failed to do
                 anything with it (currently this only happens if the
                 file name is on a volume different from the volume
                 specified by pathBase).

        @see Normalize()
    */
    bool MakeRelativeTo(const wxString& pathBase = wxEmptyString,
                        wxPathFormat format = wxPATH_NATIVE);

    //@{
    /**
        @param dir
            the directory to create
        @param parm
            the permissions for the newly created directory
        @param flags
            if the flags contain wxPATH_MKDIR_FULL flag,
            try to create each directory in the path and also don't return an error
            if the target directory already exists.

        @returns Returns @true if the directory was successfully created, @false
                 otherwise.
    */
    bool Mkdir(int perm = 0777, int flags = 0);
    static bool Mkdir(const wxString& dir, int perm = 0777,
                      int flags = 0);
    //@}

    /**
        Normalize the path. With the default flags value, the path will be
        made absolute, without any ".." and "." and all environment
        variables will be expanded in it.

        @param flags
            The kind of normalization to do with the file name. It can be
            any or-combination of the following constants:






            wxPATH_NORM_ENV_VARS




            replace env vars with their values





            wxPATH_NORM_DOTS




            squeeze all .. and . when possible; if there are too many .. and thus they
        cannot be all removed, @false will be returned





            wxPATH_NORM_CASE




            if filesystem is case insensitive, transform to lower case





            wxPATH_NORM_ABSOLUTE




            make the path absolute prepending cwd





            wxPATH_NORM_LONG




            make the path the long form





            wxPATH_NORM_SHORTCUT




            resolve if it is a shortcut (Windows only)





            wxPATH_NORM_TILDE




            replace ~ and ~user (Unix only)





            wxPATH_NORM_ALL




            all of previous flags except wxPATH_NORM_CASE
        @param cwd
            If not empty, this directory will be used instead of current
            working directory in normalization (see wxPATH_NORM_ABSOLUTE).
        @param format
            The file name format to use when processing the paths, native by default.

        @returns @true if normalization was successfully or @false otherwise.
    */
    bool Normalize(int flags = wxPATH_NORM_ALL,
                   const wxString& cwd = wxEmptyString,
                   wxPathFormat format = wxPATH_NATIVE);

    /**
        These methods allow to work with the file creation, access and modification
        times. Note that not all filesystems under all platforms implement these times
        in the same way. For example, the access time under Windows has a resolution of
        one day (so it is really the access date and not time). The access time may be
        updated when the file is executed or not depending on the platform.
        GetModificationTime()

        GetTimes()

        SetTimes()

        Touch()
        Other file system operations functions are:
        Mkdir()

        Rmdir()
    */


    /**
        Prepends a directory to the file path. Please see
        AppendDir() for important notes.
    */
    void PrependDir(const wxString& dir);

    /**
        Removes the specified directory component from the path.

        @see GetDirCount()
    */
    void RemoveDir(size_t pos);

    /**
        Removes last directory component from the path.
    */
    void RemoveLastDir();

    //@{
    /**
        Deletes the specified directory from the file system.
    */
    bool Rmdir();
    static bool Rmdir(const wxString& dir);
    //@}

    /**
        Compares the filename using the rules of this platform.
    */
    bool SameAs(const wxFileName& filepath,
                wxPathFormat format = wxPATH_NATIVE) const;

    //@{
    /**
        Changes the current working directory.
    */
    bool SetCwd();
    static bool SetCwd(const wxString& cwd);
    //@}

    /**
        Sets the extension of the file name to be an empty extension.
        This is different from having no extension at all as the file
        name will have a trailing dot after a call to this method.

        @see SetExt(), ClearExt()
    */
    void SetEmptyExt();

    /**
        Sets the extension of the file name. Setting an empty string
        as the extension will remove the extension resulting in a file
        name without a trailing dot, unlike a call to
        SetEmptyExt().

        @see SetEmptyExt(), ClearExt()
    */
    void SetExt(const wxString& ext);

    /**
        The full name is the file name and extension (but without the path).
    */
    void SetFullName(const wxString& fullname);

    /**
        Sets the name part (without extension).

        @see SetFullName()
    */
    void SetName(const wxString& name);

    /**
        Sets the file creation and last access/modification times (any of the pointers
        may be @NULL).
    */
    bool SetTimes(const wxDateTime* dtAccess,
                  const wxDateTime* dtMod,
                  const wxDateTime* dtCreate);

    /**
        Sets the volume specifier.
    */
    void SetVolume(const wxString& volume);

    //@{
    /**
        This function splits a full file name into components: the volume (with the
        first version) path (including the volume in the second version), the base name
        and the extension. Any of the output parameters (@e volume, @e path,
        @a name or @e ext) may be @NULL if you are not interested in the
        value of a particular component. Also, @a fullpath may be empty on entry.
        On return, @a path contains the file path (without the trailing separator),
        @a name contains the file name and @a ext contains the file extension
        without leading dot. All three of them may be empty if the corresponding
        component is. The old contents of the strings pointed to by these parameters
        will be overwritten in any case (if the pointers are not @NULL).
        Note that for a filename "foo." the extension is present, as indicated by the
        trailing dot, but empty. If you need to cope with such cases, you should use
        @a hasExt instead of relying on testing whether @a ext is empty or not.
    */
    static void SplitPath(const wxString& fullpath, wxString* volume,
                          wxString* path,
                          wxString* name,
                          wxString* ext,
                          bool hasExt = NULL,
                          wxPathFormat format = wxPATH_NATIVE);
    static void SplitPath(const wxString& fullpath,
                          wxString* volume,
                          wxString* path,
                          wxString* name,
                          wxString* ext,
                          wxPathFormat format = wxPATH_NATIVE);
    static void SplitPath(const wxString& fullpath,
                          wxString* path,
                          wxString* name,
                          wxString* ext,
                          wxPathFormat format = wxPATH_NATIVE);
    //@}

    /**
        Splits the given @a fullpath into the volume part (which may be empty) and
        the pure path part, not containing any volume.

        @see SplitPath()
    */
    static void SplitVolume(const wxString& fullpath,
                            wxString* volume,
                            wxString* path,
                            wxPathFormat format = wxPATH_NATIVE);

    /**
        Sets the access and modification times to the current moment.
    */
    bool Touch();

    //@{
    /**
        Returns @true if the filenames are different. The string @e filenames
        is interpreted as a path in the native filename format.
    */
    bool operator operator!=(const wxFileName& filename) const;
    const bool operator operator!=(const wxString& filename) const;
    //@}

    //@{
    /**
        Assigns the new value to this filename object.
    */
    wxFileName& operator operator=(const wxFileName& filename);
    wxFileName& operator operator=(const wxString& filename);
    //@}

    //@{
    /**
        Returns @true if the filenames are equal. The string @e filenames is
        interpreted as a path in the native filename format.
    */
    bool operator operator==(const wxFileName& filename) const;
    const bool operator operator==(const wxString& filename) const;
    //@}
};

