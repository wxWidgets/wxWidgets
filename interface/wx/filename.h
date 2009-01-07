/////////////////////////////////////////////////////////////////////////////
// Name:        filename.h
// Purpose:     interface of wxFileName
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    The various values for the path format: this mainly affects the path
    separator but also whether or not the path has the drive part
    (as under Windows).

    See wxFileName for more info.
*/
enum wxPathFormat
{
    wxPATH_NATIVE = 0,      //!< the path format for the current platform.
    wxPATH_UNIX,
    wxPATH_BEOS = wxPATH_UNIX,
    wxPATH_MAC,
    wxPATH_DOS,
    wxPATH_WIN = wxPATH_DOS,
    wxPATH_OS2 = wxPATH_DOS,
    wxPATH_VMS,

    wxPATH_MAX   //!< Not a valid value for specifying path format
};


/**
    The kind of normalization to do with the file name: these values can be
    or'd together to perform several operations at once.
    See wxFileName::Normalize() for more info.
*/
enum wxPathNormalize
{
    //! Replace environment variables with their values.
    //! wxFileName understands both Unix and Windows (but only under Windows) environment
    //! variables expansion: i.e. @c "$var", @c "$(var)" and @c "${var}" are always understood
    //! and in addition under Windows @c "%var%" is also.
    wxPATH_NORM_ENV_VARS = 0x0001,

    wxPATH_NORM_DOTS     = 0x0002,  //!< Squeeze all @c ".." and @c "." and prepend the current working directory.
    wxPATH_NORM_TILDE    = 0x0004,  //!< Replace @c "~" and @c "~user" (Unix only).
    wxPATH_NORM_CASE     = 0x0008,  //!< If the platform is case insensitive, make lowercase the path.
    wxPATH_NORM_ABSOLUTE = 0x0010,  //!< Make the path absolute.
    wxPATH_NORM_LONG =     0x0020,  //!< Expand the path to the "long" form (Windows only).
    wxPATH_NORM_SHORTCUT = 0x0040,  //!< Resolve the shortcut, if it is a shortcut (Windows only).

    //! A value indicating all normalization flags except for @c wxPATH_NORM_CASE.
    wxPATH_NORM_ALL      = 0x00ff & ~wxPATH_NORM_CASE
};

/**
    The return value of wxFileName::GetSize() in case of error.
*/
wxULongLong wxInvalidSize;


/**
    @class wxFileName

    wxFileName encapsulates a file name.

    This class serves two purposes: first, it provides the functions to split the
    file names into components and to recombine these components in the full file
    name which can then be passed to the OS file functions
    (and @ref group_funcmacro_file "wxWidgets functions" wrapping them).
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

    The same can be done using the static method wxFileName::DirName():

    @code
    wxFileName dirname = wxFileName::DirName( "C:\mydir" );
    MyMethod( dirname.GetPath() );
    @endcode

    Accordingly, methods dealing with directories or directory names like
    wxFileName::IsDirReadable() use wxFileName::GetPath() whereas methods dealing
    with file names like wxFileName::IsFileReadable() use wxFileName::GetFullPath().

    If it is not known wether a string contains a directory name or a complete
    file name (such as when interpreting user input) you need to use the static
    function wxFileName::DirExists() (or its identical variants wxDir::Exists() and
    wxDirExists()) and construct the wxFileName instance accordingly.
    This will only work if the directory actually exists, of course:

    @code
    wxString user_input;
    // get input from user

    wxFileName fname;
    if (wxDirExists(user_input))
        fname.AssignDir( user_input );
    else
        fname.Assign( user_input );
    @endcode

    Please note that many wxFileName methods accept the path format argument
    which is by @c wxPATH_NATIVE by default meaning to use the path format
    native for the current platform.
    The path format affects the operation of wxFileName functions in several ways:
    first and foremost, it defines the path separator character to use, but it
    also affects other things such as whether the path has the drive part or not.
    See wxPathFormat for more info.


    @section filename_format File name format

    wxFileName currently supports the file names in the Unix, DOS/Windows,
    Mac OS and VMS formats. Although these formats are quite different,
    wxFileName tries to treat them all in the same generic way.
    It supposes that all file names consist of the following parts: the volume
    (also known as drive under Windows or device under VMS), the path which is
    a sequence of directory names separated by the path separators and the full
    filename itself which, in turn, is composed from the base file name and the
    extension. All of the individual components of the file name may be empty
    and, for example, the volume name is always empty under Unix, but if they
    are all empty simultaneously, the filename object is considered to be in an
    invalid state and wxFileName::IsOk() returns false for it.

    File names can be case-sensitive or not, the function wxFileName::IsCaseSensitive()
    allows to determine this. The rules for determining whether the file name is
    absolute or relative also depend on the file name format and the only portable way
    to answer this question is to use wxFileName::IsAbsolute() or wxFileName::IsRelative()
    method.

    Note that on Windows,"X:" refers to the current working directory on drive X.
    Therefore, a wxFileName instance constructed from for example "X:dir/file.ext"
    treats the portion beyond drive separator as being relative to that directory.
    To ensure that the filename is absolute, you may use wxFileName::MakeAbsolute().
    There is also an inverse function wxFileName::MakeRelativeTo() which undoes
    what wxFileName::Normalize(wxPATH_NORM_DOTS) does.
    Other functions returning information about the file format provided by this
    class are wxFileName::GetVolumeSeparator(), wxFileName::IsPathSeparator().


    @section filename_construction File name construction

    You can initialize a wxFileName instance using one of the following functions:

    @li wxFileName::wxFileName()
    @li wxFileName::Assign()
    @li wxFileName::AssignCwd()
    @li wxFileName::AssignDir()
    @li wxFileName::AssignHomeDir()
    @li wxFileName::AssignTempFileName()
    @li wxFileName::DirName()
    @li wxFileName::FileName()
    @li wxFileName::operator=()


    @section filename_tests File name tests

    Before doing other tests, you should use wxFileName::IsOk() to verify that
    the filename is well defined. If it is, FileExists() can be used to test whether
    a file with such name exists and wxFileName::DirExists() can be used to test
    for directory existence.
    File names should be compared using the wxFileName::SameAs() method or
    wxFileName::operator==(). For testing basic access modes, you can use:

    @li wxFileName::IsDirWritable()
    @li wxFileName::IsDirReadable()
    @li wxFileName::IsFileWritable()
    @li wxFileName::IsFileReadable()
    @li wxFileName::IsFileExecutable()


    @section filename_components File name components

    These functions allow to examine and modify the individual directories
    of the path:

    @li wxFileName::AppendDir()
    @li wxFileName::InsertDir()
    @li wxFileName::GetDirCount()
    @li wxFileName::PrependDir()
    @li wxFileName::RemoveDir()
    @li wxFileName::RemoveLastDir()

    To change the components of the file name individually you can use the
    following functions:

    @li wxFileName::GetExt()
    @li wxFileName::GetName()
    @li wxFileName::GetVolume()
    @li wxFileName::HasExt()
    @li wxFileName::HasName()
    @li wxFileName::HasVolume()
    @li wxFileName::SetExt()
    @li wxFileName::ClearExt()
    @li wxFileName::SetEmptyExt()
    @li wxFileName::SetName()
    @li wxFileName::SetVolume()

    You can initialize a wxFileName instance using one of the following functions:


    @section filename_operations File name operations

    These methods allow to work with the file creation, access and modification
    times. Note that not all filesystems under all platforms implement these times
    in the same way. For example, the access time under Windows has a resolution of
    one day (so it is really the access date and not time). The access time may be
    updated when the file is executed or not depending on the platform.

    @li wxFileName::GetModificationTime()
    @li wxFileName::GetTimes()
    @li wxFileName::SetTimes()
    @li wxFileName::Touch()

    Other file system operations functions are:

    @li wxFileName::Mkdir()
    @li wxFileName::Rmdir()


    @library{wxbase}
    @category{file}
*/
class wxFileName
{
public:
    /**
        Default constructor.
    */
    wxFileName();

    /**
        Copy constructor.
    */
    wxFileName(const wxFileName& filename);

    /**
        Constructor taking a full filename.

        If it terminates with a '/', a directory path is constructed
        (the name will be empty), otherwise a file name and extension
        are extracted from it.
    */
    wxFileName(const wxString& fullpath,
               wxPathFormat format = wxPATH_NATIVE);

    /**
        Constructor a directory name and file name.
    */
    wxFileName(const wxString& path, const wxString& name,
               wxPathFormat format = wxPATH_NATIVE);

    /**
        Constructor from a directory name, base file name and extension.
    */
    wxFileName(const wxString& path, const wxString& name,
               const wxString& ext,
               wxPathFormat format = wxPATH_NATIVE);

    /**
        Constructor from a volume name, a directory name, base file name and extension.
    */
    wxFileName(const wxString& volume, const wxString& path,
               const wxString& name,
               const wxString& ext,
               wxPathFormat format = wxPATH_NATIVE);

    /**
        Appends a directory component to the path. This component should contain a
        single directory name level, i.e. not contain any path or volume separators nor
        should it be empty, otherwise the function does nothing (and generates an
        assert failure in debug build).
    */
    void AppendDir(const wxString& dir);

    /**
        Creates the file name from another filename object.
    */
    void Assign(const wxFileName& filepath);

    /**
        Creates the file name from a full file name with a path.
    */
    void Assign(const wxString& fullpath,
                wxPathFormat format = wxPATH_NATIVE);

    /**
        Creates the file name from volumne, path, name and extension.
    */
    void Assign(const wxString& volume, const wxString& path,
                const wxString& name,
                const wxString& ext,
                bool hasExt,
                wxPathFormat format = wxPATH_NATIVE);

    /**
        Creates the file name from volumne, path, name and extension.
    */
    void Assign(const wxString& volume, const wxString& path,
                const wxString& name,
                const wxString& ext,
                wxPathFormat format = wxPATH_NATIVE);

    /**
        Creates the file name from file path and file name.
    */
    void Assign(const wxString& path, const wxString& name,
                wxPathFormat format = wxPATH_NATIVE);

    /**
        Creates the file name from path, name and extension.
    */
    void Assign(const wxString& path, const wxString& name,
                const wxString& ext,
                wxPathFormat format = wxPATH_NATIVE);

    /**
        Makes this object refer to the current working directory on the specified
        volume (or current volume if @a volume is empty).

        @see GetCwd()
    */
    static void AssignCwd(const wxString& volume = wxEmptyString);

    /**
        Sets this file name object to the given directory name.
        The name and extension will be empty.
    */
    void AssignDir(const wxString& dir,
                   wxPathFormat format = wxPATH_NATIVE);

    /**
        Sets this file name object to the home directory.
    */
    void AssignHomeDir();

    /**
        The function calls CreateTempFileName() to create a temporary file
        and sets this object to the name of the file.

        If a temporary file couldn't be created, the object is put into
        an invalid state (see IsOk()).
    */
    void AssignTempFileName(const wxString& prefix);

    /**
        The function calls CreateTempFileName() to create a temporary
        file name and open @a fileTemp with it.

        If the file couldn't be opened, the object is put into
        an invalid state (see IsOk()).
    */
    void AssignTempFileName(const wxString& prefix, wxFile* fileTemp);

    /**
        The function calls CreateTempFileName() to create a temporary
        file name and open @a fileTemp with it.

        If the file couldn't be opened, the object is put into
        an invalid state (see IsOk()).
    */
    void AssignTempFileName(const wxString& prefix, wxFFile* fileTemp);

    /**
        Reset all components to default, uninitialized state.
    */
    void Clear();

    /**
        Removes the extension from the file name resulting in a
        file name with no trailing dot.

        @see SetExt(), SetEmptyExt()
    */
    void ClearExt();

    /**
        Returns a temporary file name starting with the given @e prefix.
        If the @a prefix is an absolute path, the temporary file is created in this
        directory, otherwise it is created in the default system directory for the
        temporary files or in the current directory.

        If the function succeeds, the temporary file is actually created.
        If @a fileTemp is not @NULL, this file will be opened using the name of
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

        @return The full temporary file name or an empty string on error.
    */
    static wxString CreateTempFileName(const wxString& prefix,
                                       wxFile* fileTemp = NULL);

    /**
        Returns @true if the directory with this name exists.
    */
    bool DirExists() const;

    /**
        Returns @true if the directory with this name exists.
    */
    static bool DirExists(const wxString& dir);

    /**
        Returns the object corresponding to the directory with the given name.
        The @a dir parameter may have trailing path separator or not.
    */
    static wxFileName DirName(const wxString& dir,
                              wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns @true if the file with this name exists.

        @see DirExists()
    */
    bool FileExists() const;

    /**
        Returns @true if the file with this name exists.

        @see DirExists()
    */
    static bool FileExists(const wxString& file);

    /**
        Returns the file name object corresponding to the given @e file. This
        function exists mainly for symmetry with DirName().
    */
    static wxFileName FileName(const wxString& file,
                               wxPathFormat format = wxPATH_NATIVE);

    /**
        Retrieves the value of the current working directory on the specified volume.
        If the volume is empty, the program's current working directory is returned for
        the current volume.

        @return The string containing the current working directory or an empty
                string on error.

        @see AssignCwd()
    */
    static wxString GetCwd(const wxString& volume = wxEmptyString);

    /**
        Returns the number of directories in the file name.
    */
    size_t GetDirCount() const;

    /**
        Returns the directories in string array form.
    */
    const wxArrayString& GetDirs() const;

    /**
        Returns the file name extension.
    */
    wxString GetExt() const;

    /**
        Returns the characters that can't be used in filenames and directory names
        for the specified format.
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

    /**
        Returns the size of the file in a human-readable form.

        If the size could not be retrieved the @c failmsg string
        is returned. In case of success, the returned string is
        a floating-point number with @c precision decimal digits
        followed by the size unit (B, kB, MB, GB, TB: respectively
        bytes, kilobytes, megabytes, gigabytes, terabytes).
    */
    wxString GetHumanReadableSize(const wxString& failmsg = "Not available",
                                  int precision = 1) const;

    /**
        Returns the size of the given number of bytes in a human-readable form.

        If @a bytes is ::wxInvalidSize or zero, then @a nullsize is returned.

        In case of success, the returned string is a floating-point number with
        @a precision decimal digits followed by the size unit (B, kB, MB, GB,
        TB: respectively bytes, kilobytes, megabytes, gigabytes, terabytes).
    */
    static wxString GetHumanReadableSize(const wxULongLong& bytes,
                                         const wxString& nullsize = "Not available",
                                         int precision = 1);

    /**
        Return the long form of the path (returns identity on non-Windows platforms).
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
        Returns the path part of the filename (without the name or extension).

        The possible flags values are:

        - @b wxPATH_GET_VOLUME:
        Return the path with the volume (does nothing for the filename formats
        without volumes), otherwise the path without volume part is returned.

        - @b wxPATH_GET_SEPARATOR:
        Return the path with the trailing separator, if this flag is not given
        there will be no separator at the end of the path.

        - @b wxPATH_NO_SEPARATOR:
        Don't include the trailing separator in the returned string. This is
        the default (the value of this flag is 0) and exists only for symmetry
        with wxPATH_GET_SEPARATOR.
    */
    wxString GetPath(int flags = wxPATH_GET_VOLUME,
                     wxPathFormat format = wxPATH_NATIVE) const;

    /**
        Returns the usually used path separator for this format.
        For all formats but @c wxPATH_DOS there is only one path separator anyhow,
        but for DOS there are two of them and the native one, i.e. the backslash
        is returned by this method.

        @see GetPathSeparators()
    */
    static wxUniChar GetPathSeparator(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the string containing all the path separators for this format.
        For all formats but @c wxPATH_DOS this string contains only one character
        but for DOS and Windows both @c '/' and @c '\' may be used as separators.

        @see GetPathSeparator()
    */
    static wxString GetPathSeparators(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the string of characters which may terminate the path part.
        This is the same as GetPathSeparators() except for VMS
        path format where ] is used at the end of the path part.
    */
    static wxString GetPathTerminators(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the path with the trailing separator, useful for appending the name
        to the given path.

        This is the same as calling
        @code
        GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, format)
        @endcode
    */
    wxString GetPathWithSep(wxPathFormat format = wxPATH_NATIVE) const;

    /**
        Return the short form of the path (returns identity on non-Windows platforms).
    */
    wxString GetShortPath() const;

    /**
        Returns the size of the file If the file does not exist or its size could
        not be read (because e.g. the file is locked by another process) the returned
        value is ::wxInvalidSize.
    */
    wxULongLong GetSize() const;

    /**
        Returns the size of the file If the file does not exist or its size could
        not be read (because e.g. the file is locked by another process) the returned
        value is ::wxInvalidSize.
    */
    const static wxULongLong GetSize(const wxString& filename);

    /**
        Returns the directory used for temporary files.
    */
    static wxString GetTempDir();

    /**
        Returns the last access, last modification and creation times.
        The last access time is updated whenever the file is read or written
        (or executed in the case of Windows), last modification time is only
        changed when the file is written to.
        Finally, the creation time is indeed the time when the file was created
        under Windows and the inode change time under Unix (as it is impossible to
        retrieve the real file creation time there anyhow) which can also be changed
        by many operations after the file creation.

        If no filename or extension is specified in this instance of wxFileName
        (and therefore IsDir() returns @true) then this function will return the
        directory times of the path specified by GetPath(), otherwise the file
        times of the file specified by GetFullPath().
        Any of the pointers may be @NULL if the corresponding time is not needed.

        @return @true on success, @false if we failed to retrieve the times.
    */
    bool GetTimes(wxDateTime* dtAccess, wxDateTime* dtMod,
                  wxDateTime* dtCreate) const;

    /**
        Returns the string containing the volume for this file name, empty if it
        doesn't have one or if the file system doesn't support volumes at all
        (for example, Unix).
    */
    wxString GetVolume() const;

    /**
        Returns the string separating the volume from the path for this format.
    */
    static wxString GetVolumeSeparator(wxPathFormat format = wxPATH_NATIVE);

     /**
        This function builds a volume path string, for example "C:\\".

        Implemented for the platforms which use drive letters, i.e. DOS, MSW
        and OS/2 only.

        @since 2.9.0

        @param drive
            The drive letter, 'A' through 'Z' or 'a' through 'z'.

        @param flags
            @c wxPATH_NO_SEPARATOR or @c wxPATH_GET_SEPARATOR to omit or include
            the trailing path separator, the default is to include it.

        @return Volume path string.
    */
    static wxString GetVolumeString(char drive, int flags = wxPATH_GET_SEPARATOR);

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
    bool IsAbsolute(wxPathFormat format = wxPATH_NATIVE) const;

    /**
        Returns @true if the file names of this type are case-sensitive.
    */
    static bool IsCaseSensitive(wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns @true if this object represents a directory, @false otherwise
        (i.e. if it is a file).

        Note that this method doesn't test whether the directory or file really
        exists, you should use DirExists() or FileExists() for this.
    */
    bool IsDir() const;

    /**
        Returns @true if the directory component of this instance is an existing
        directory and this process has read permissions on it. Read permissions
        on a directory mean that you can list the directory contents but it
        doesn't imply that you have read permissions on the files contained.
    */
    bool IsDirReadable() const;

    /**
        Returns @true if the given @e dir is an existing directory and this process
        has read permissions on it. Read permissions on a directory mean that you
        can list the directory contents but it doesn't imply that you have read
        permissions on the files contained.
    */
    static bool IsDirReadable(const wxString& dir);

    /**
        Returns @true if the directory component of this instance
        is an existing directory and this process has write permissions on it.
        Write permissions on a directory mean that you can create new files in the
        directory.
    */
    bool IsDirWritable() const;

    /**
        Returns @true if the  given @a dir is an existing directory and this
        process has write permissions on it.
        Write permissions on a directory mean that you can create new files in the
        directory.
    */
    static bool IsDirWritable(const wxString& dir);

    /**
        Returns @true if a file with this name exists and if this process has execute
        permissions on it.
    */
    bool IsFileExecutable() const;

    /**
        Returns @true if a file with this name exists and if this process has execute
        permissions on it.
    */
    static bool IsFileExecutable(const wxString& file);

    /**
        Returns @true if a file with this name exists and if this process has read
        permissions on it.
    */
    bool IsFileReadable() const;

    /**
        Returns @true if a file with this name exists and if this process has read
        permissions on it.
    */
    static bool IsFileReadable(const wxString& file);

    /**
        Returns @true if a file with this name exists and if this process has write
        permissions on it.
    */
    bool IsFileWritable() const;

    /**
        Returns @true if a file with this name exists and if this process has write
        permissions on it.
    */
    static bool IsFileWritable(const wxString& file);

    /**
        Returns @true if the filename is valid, @false if it is not initialized yet.
        The assignment functions and Clear() may reset the object to the uninitialized,
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
    bool IsRelative(wxPathFormat format = wxPATH_NATIVE) const;

    /**
        On Mac OS, gets the common type and creator for the given extension.
    */
    static bool MacFindDefaultTypeAndCreator(const wxString& ext,
                                            wxUint32* type,
                                            wxUint32* creator);

    /**
        On Mac OS, registers application defined extensions and their default type
        and creator.
    */
    static void MacRegisterDefaultTypeAndCreator(const wxString& ext,
                                                wxUint32 type,
                                                wxUint32 creator);

    /**
        On Mac OS, looks up the appropriate type and creator from the registration
        and then sets it.
    */
    bool MacSetDefaultTypeAndCreator();

    /**
        Make the file name absolute.
        This is a shortcut for
        @code
        wxFileName::Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE |
                              wxPATH_NORM_TILDE, cwd, format)
        @endcode

        @see MakeRelativeTo(), Normalize(), IsAbsolute()
    */
    bool MakeAbsolute(const wxString& cwd = wxEmptyString,
                      wxPathFormat format = wxPATH_NATIVE);

    /**
        This function tries to put this file name in a form relative to
        @a pathBase.
        In other words, it returns the file name which should be used to access
        this file if the current directory were pathBase.

        @param pathBase
            The directory to use as root, current directory is used by default
        @param format
            The file name format, native by default

        @return @true if the file name has been changed, @false if we failed to do
                anything with it (currently this only happens if the file name
                is on a volume different from the volume specified by @a pathBase).

        @see Normalize()
    */
    bool MakeRelativeTo(const wxString& pathBase = wxEmptyString,
                        wxPathFormat format = wxPATH_NATIVE);

    /**
        Creates a directory.

        @param perm
            The permissions for the newly created directory.
            See the ::wxPosixPermissions enumeration for more info.
        @param flags
            If the flags contain @c wxPATH_MKDIR_FULL flag, try to create each
            directory in the path and also don't return an error if the target
            directory already exists.

        @return Returns @true if the directory was successfully created, @false
                otherwise.
    */
    bool Mkdir(int perm = wxS_DIR_DEFAULT, int flags = 0);

    /**
        Creates a directory.

        @param dir
            The directory to create
        @param perm
            The permissions for the newly created directory.
            See the ::wxPosixPermissions enumeration for more info.
        @param flags
            If the flags contain @c wxPATH_MKDIR_FULL flag, try to create each
            directory in the path and also don't return an error if the target
            directory already exists.

        @return Returns @true if the directory was successfully created, @false
                otherwise.
    */
    static bool Mkdir(const wxString& dir, int perm = wxS_DIR_DEFAULT,
                      int flags = 0);

    /**
        Normalize the path. With the default flags value, the path will be
        made absolute, without any ".." and "." and all environment
        variables will be expanded in it.

        @param flags
            The kind of normalization to do with the file name. It can be
            any or-combination of the ::wxPathNormalize enumeration values.
        @param cwd
            If not empty, this directory will be used instead of current
            working directory in normalization (see @c wxPATH_NORM_ABSOLUTE).
        @param format
            The file name format to use when processing the paths, native by default.

        @return @true if normalization was successfully or @false otherwise.
    */
    bool Normalize(int flags = wxPATH_NORM_ALL,
                   const wxString& cwd = wxEmptyString,
                   wxPathFormat format = wxPATH_NATIVE);

    /**
        Prepends a directory to the file path.
        Please see AppendDir() for important notes.
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

    /**
        If the path contains the value of the environment variable named @a envname
        then this function replaces it with the string obtained from
        wxString::Format(replacementFmtString, value_of_envname_variable).

        This function is useful to make the path shorter or to make it dependent
        from a certain environment variable.
        Normalize() with @c wxPATH_NORM_ENV_VARS can perform the opposite of this
        function (depending on the value of @a replacementFmtString).

        The name and extension of this filename are not modified.

        Example:
        @code
            wxFileName fn("/usr/openwin/lib/someFile");
            fn.ReplaceEnvVariable("OPENWINHOME");
                    // now fn.GetFullPath() == "$OPENWINHOME/lib/someFile"
        @endcode

        @since 2.9.0

        @return @true if the operation was successful (which doesn't mean
                that something was actually replaced, just that ::wxGetEnv
                didn't fail).
    */
    bool ReplaceEnvVariable(const wxString& envname,
                            const wxString& replacementFmtString = "$%s",
                            wxPathFormat format = wxPATH_NATIVE);

    /**
        Replaces, if present in the path, the home directory for the given user
        (see ::wxGetHomeDir) with a tilde (~).

        Normalize() with @c wxPATH_NORM_TILDE performs the opposite of this
        function.

        The name and extension of this filename are not modified.

        @since 2.9.0

        @return @true if the operation was successful (which doesn't mean
                that something was actually replaced, just that ::wxGetHomeDir
                didn't fail).
    */
    bool ReplaceHomeDir(wxPathFormat format = wxPATH_NATIVE);


    /**
        Deletes the specified directory from the file system.
    */
    bool Rmdir();

    /**
        Deletes the specified directory from the file system.
    */
    static bool Rmdir(const wxString& dir);

    /**
        Compares the filename using the rules of this platform.
    */
    bool SameAs(const wxFileName& filepath,
                wxPathFormat format = wxPATH_NATIVE) const;

    /**
        Changes the current working directory.
    */
    bool SetCwd();

    /**
        Changes the current working directory.
    */
    static bool SetCwd(const wxString& cwd);

    /**
        Sets the extension of the file name to be an empty extension.
        This is different from having no extension at all as the file
        name will have a trailing dot after a call to this method.

        @see SetExt(), ClearExt()
    */
    void SetEmptyExt();

    /**
        Sets the extension of the file name.

        Setting an empty string as the extension will remove the extension
        resulting in a file name without a trailing dot, unlike a call to
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
        and the extension.

        Any of the output parameters (@e volume, @e path, @a name or @e ext) may
        be @NULL if you are not interested in the value of a particular component.
        Also, @a fullpath may be empty on entry.
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

    /**
        Returns @true if the filenames are different. The string @e filenames
        is interpreted as a path in the native filename format.
    */
    bool operator!=(const wxFileName& filename) const;

    /**
        Returns @true if the filenames are different. The string @e filenames
        is interpreted as a path in the native filename format.
    */
    bool operator!=(const wxString& filename) const;

    /**
        Returns @true if the filenames are equal. The string @e filenames is
        interpreted as a path in the native filename format.
    */
    bool operator==(const wxFileName& filename) const;

    /**
        Returns @true if the filenames are equal. The string @e filenames is
        interpreted as a path in the native filename format.
    */
    bool operator==(const wxString& filename) const;

    /**
        Assigns the new value to this filename object.
    */
    wxFileName& operator=(const wxFileName& filename);

    /**
        Assigns the new value to this filename object.
    */
    wxFileName& operator=(const wxString& filename);
};

