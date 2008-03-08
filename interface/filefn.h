/////////////////////////////////////////////////////////////////////////////
// Name:        filefn.h
// Purpose:     documentation for wxPathList class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPathList
    @wxheader{filefn.h}
    
    The path list is a convenient way of storing a number of directories, and
    when presented with a filename without a directory, searching for an existing
    file
    in those directories.
    
    Be sure to look also at wxStandardPaths if you only
    want to search files in some standard paths.
    
    @library{wxbase}
    @category{file}
    
    @seealso
    wxArrayString, wxStandardPaths, wxFileName
*/
class wxPathList : public wxArrayString
{
public:
    //@{
    /**
        Constructs the object calling the Add() function.
    */
    wxPathList();
        wxPathList(const wxArrayString& arr);
    //@}

    //@{
    /**
        The first form adds the given directory to the path list, if the path is not
        already in the list.
        If the path cannot be normalized for some reason, it returns @false.
        
        The second form just calls the first form on all elements of the given array.
        
        The @e path is always considered a directory but no existence checks will be
        done on it
        (because if it doesn't exist, it could be created later and thus result a valid
        path when
        FindValidPath() is called).
        
        @b Note: if the given path is relative, it won't be made absolute before adding
        it
        (this is why FindValidPath() may return relative paths).
    */
    bool Add(const wxString& path);
        void Add(const wxArrayString& arr);
    //@}

    /**
        Finds the value of the given environment variable, and adds all paths
        to the path list. Useful for finding files in the @c PATH variable, for
        example.
    */
    void AddEnvList(const wxString& env_variable);

    /**
        Given a full filename (with path), calls Add() with the path
        of the file.
    */
    bool EnsureFileAccessible(const wxString& filename);

    /**
        Like FindValidPath() but this function always
        returns an absolute path (eventually prepending the current working directory
        to the value returned wxPathList::FindValidPath) or an
        empty string.
    */
    wxString FindAbsoluteValidPath(const wxString& file);

    /**
        Searches the given file in all paths stored in this class.
        The first path which concatenated to the given string points to an existing
        file (see @ref wxFile::exists wxFileExists) is returned.
        
        If the file wasn't found in any of the stored paths, an empty string is
        returned.
        
        The given string must be a file name, eventually with a path prefix (if the path
        prefix is absolute, only its name will be searched); i.e. it must not end with
        a directory separator (see wxFileName::GetPathSeparator)
        otherwise an assertion will fail.
        
        The returned path may be relative to the current working directory.
        Note in fact that wxPathList can be used to store both relative and absolute
        paths so that
        if you Added() relative paths, then the current working directory
        (see wxGetCwd and wxSetWorkingDirectory)
        may affect the value returned by this function!
    */
    wxString FindValidPath(const wxString& file);
};


// ============================================================================
// Global functions/macros
// ============================================================================

/**
    This function returns the total number of bytes and number of free bytes on
    the disk containing the directory @e path (it should exist). Both
    @e total and @e free parameters may be @NULL if the corresponding
    information is not needed.
*/
bool wxGetDiskSpace(const wxString& path,
                    wxLongLong total = @NULL,
                    wxLongLong free = @NULL);

/**
    Returns the Windows directory under Windows; on other platforms returns the
    empty string.
*/
wxString wxGetOSDirectory();

/**
    Parses the @e wildCard, returning the number of filters.
    Returns 0 if none or if there's a problem.
    The arrays will contain an equal number of items found before the error.
    On platforms where native dialogs handle only one filter per entry,
    entries in arrays are automatically adjusted.
    @e wildCard is in the form:
    
    @code
    "All files (*)|*|Image Files (*.jpeg *.png)|*.jpg;*.png"
    @endcode
*/
int wxParseCommonDialogsFilter(const wxString& wildCard,
                               wxArrayString& descriptions,
                               wxArrayString& filters);

/**
    This function is deprecated, use wxFileName instead.
    
    Converts a Unix to a DOS filename by replacing forward
    slashes with backslashes.
*/
void wxUnix2DosFilename(wxChar * s);

/**
    Returns @true if @e dirname exists and is a directory.
*/
bool wxDirExists(const wxString& dirname);

/**
    @b NB: This function is obsolete, please use
    wxFileName::SplitPath instead.
    
    This function splits a full file name into components: the path (including
    possible disk/drive
    specification under Windows), the base name and the extension. Any of the
    output parameters
    (@e path, @e name or @e ext) may be @NULL if you are not interested in the value
    of
    a particular component.
    
    wxSplitPath() will correctly handle filenames with both DOS and Unix path
    separators under
    Windows, however it will not consider backslashes as path separators under Unix
    (where backslash
    is a valid character in a filename).
    
    On entry, @e fullname should be non-@NULL (it may be empty though).
    
    On return, @e path contains the file path (without the trailing separator), @e
    name
    contains the file name and @e ext contains the file extension without leading
    dot. All
    three of them may be empty if the corresponding component is. The old contents
    of the
    strings pointed to by these parameters will be overwritten in any case (if the
    pointers
    are not @NULL).
*/
void wxSplitPath(const wxString& fullname, wxString * path,
                 wxString * name,
                 wxString * ext);

/**
    Under Unix this macro changes the current process umask to the given value,
    unless it is equal to -1 in which case nothing is done, and restores it to
    the original value on scope exit. It works by declaring a variable which sets
    umask to @e mask in its constructor and restores it in its destructor.
    
    Under other platforms this macro expands to nothing.
*/
#define wxCHANGE_UMASK(int mask)     /* implementation is private */

/**
    Returns time of last modification of given file.
    
    The function returns @c (time_t)-1 if an error occurred (e.g. file not
    found).
*/
time_t wxFileModificationTime(const wxString& filename);

//@{
/**
    @b NB: This function is obsolete, please use
    wxFileName::SplitPath instead.
    
    Returns the filename for a full path. The second form returns a pointer to
    temporary storage that should not be deallocated.
*/
wxString wxFileNameFromPath(const wxString& path);
    char * wxFileNameFromPath(char * path);
//@}

/**
    Renames @e file1 to @e file2, returning @true if successful.
    
    If @e overwrite parameter is @true (default), the destination file is
    overwritten if it exists, but if @e overwrite is @false, the functions fails
    in this case.
*/
bool wxRenameFile(const wxString& file1, const wxString& file2,
                  bool overwrite = @true);

/**
    Copies @e file1 to @e file2, returning @true if successful. If
    @e overwrite parameter is @true (default), the destination file is overwritten
    if it exists, but if @e overwrite is @false, the functions fails in this
    case.
    
    This function supports resources forks under Mac OS.
*/
bool wxCopyFile(const wxString& file1, const wxString& file2,
                bool overwrite = @true);

/**
    Returns @true if the file exists and is a plain file.
*/
bool wxFileExists(const wxString& filename);

/**
    Returns @true if the @e pattern matches the @e text; if @e dot_special is @true,
    filenames beginning with a dot are not matched
    with wildcard characters. See wxIsWild.
*/
bool wxMatchWild(const wxString& pattern, const wxString& text,
                 bool dot_special);

/**
    @b NB: This function is deprecated: use wxGetCwd instead.
    
    Copies the current working directory into the buffer if supplied, or
    copies the working directory into new storage (which you must delete
    yourself) if the buffer is @NULL.
    
    @e sz is the size of the buffer if supplied.
*/
wxString wxGetWorkingDirectory(char * buf=@NULL, int sz=1000);

/**
    Returns the directory part of the filename.
*/
wxString wxPathOnly(const wxString& path);

/**
    Returns @true if the pattern contains wildcards. See wxMatchWild.
*/
bool wxIsWild(const wxString& pattern);

/**
    Returns a string containing the current (or working) directory.
*/
wxString wxGetCwd();

/**
    Converts a DOS to a Unix filename by replacing backslashes with forward
    slashes.
*/
void wxDos2UnixFilename(wxChar * s);

/**
    Concatenates @e file1 and @e file2 to @e file3, returning
    @true if successful.
*/
bool wxConcatFiles(const wxString& file1, const wxString& file2,
                   const wxString& file3);

/**
    Removes @e file, returning @true if successful.
*/
bool wxRemoveFile(const wxString& file);

/**
    Sets the current working directory, returning @true if the operation succeeded.
    Under MS Windows, the current drive is also changed if @e dir contains a drive
    specification.
*/
bool wxSetWorkingDirectory(const wxString& dir);

/**
    Makes the directory @e dir, returning @true if successful.
    
    @e perm is the access mask for the directory for the systems on which it is
    supported (Unix) and doesn't have any effect on the other ones.
*/
bool wxMkdir(const wxString& dir, int perm = 0777);

/**
    Returns @true if the argument is an absolute filename, i.e. with a slash
    or drive name at the beginning.
*/
bool wxIsAbsolutePath(const wxString& filename);

/**
    Returns the next file that matches the path passed to wxFindFirstFile.
    
    See wxFindFirstFile for an example.
*/
wxString wxFindNextFile();

/**
    This function does directory searching; returns the first file
    that matches the path @e spec, or the empty string. Use wxFindNextFile to
    get the next matching file. Neither will report the current directory "." or the
    parent directory "..".
*/
wxString wxFindFirstFile(const wxString& spec, int flags = 0);

//@{
/**
    Returns the type of an open file. Possible return values are:
    @code
    enum wxFileKind
    {
      wxFILE_KIND_UNKNOWN,
      wxFILE_KIND_DISK,     // a file supporting seeking to arbitrary offsets
      wxFILE_KIND_TERMINAL, // a tty
      wxFILE_KIND_PIPE      // a pipe
    };
    @endcode
*/
wxFileKind wxGetFileKind(int fd);
    wxFileKind wxGetFileKind(FILE * fp);
//@}

//@{
/**
    @b NB: These functions are obsolete, please use
    wxFileName::CreateTempFileName
    instead.
*/
char * wxGetTempFileName(const wxString& prefix, char * buf=@NULL);
    bool wxGetTempFileName(const wxString& prefix, wxString& buf);
//@}

/**
    Removes the directory @e dir, returning @true if successful. Does not work under
    VMS.
    
    The @e flags parameter is reserved for future use.
    
    Please notice that there is also a wxRmDir() function which simply wraps the
    standard POSIX rmdir() function and so return an integer error code instead of
    a boolean value (but otherwise is currently identical to wxRmdir), don't
    confuse these two functions.
*/
bool wxRmdir(const wxString& dir, int flags=0);

