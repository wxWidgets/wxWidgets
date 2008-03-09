/////////////////////////////////////////////////////////////////////////////
// Name:        file.h
// Purpose:     documentation for wxTempFile class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxTempFile
    @wxheader{file.h}

    wxTempFile provides a relatively safe way to replace the contents of the
    existing file. The name is explained by the fact that it may be also used as
    just a temporary file if you don't replace the old file contents.

    Usually, when a program replaces the contents of some file it first opens it for
    writing, thus losing all of the old data and then starts recreating it. This
    approach is not very safe because during the regeneration of the file bad things
    may happen: the program may find that there is an internal error preventing it
    from completing file generation, the user may interrupt it (especially if file
    generation takes long time) and, finally, any other external interrupts (power
    supply failure or a disk error) will leave you without either the original file
    or the new one.

    wxTempFile addresses this problem by creating a temporary file which is meant to
    replace the original file - but only after it is fully written. So, if the user
    interrupts the program during the file generation, the old file won't be lost.
    Also, if the program discovers itself that it doesn't want to replace the old
    file there is no problem - in fact, wxTempFile will @b not replace the old
    file by default, you should explicitly call wxTempFile::Commit
    to do it. Calling wxTempFile::Discard explicitly discards any
    modifications: it closes and deletes the temporary file and leaves the original
    file unchanged. If you don't call neither of Commit() and Discard(), the
    destructor will call Discard() automatically.

    To summarize: if you want to replace another file, create an instance of
    wxTempFile passing the name of the file to be replaced to the constructor (you
    may also use default constructor and pass the file name to
    wxTempFile::Open). Then you can wxTempFile::write
    to wxTempFile using wxFile-like functions and later call
    Commit() to replace the old file (and close this one) or call Discard() to
    cancel
    the modifications.

    @library{wxbase}
    @category{file}
*/
class wxTempFile
{
public:
    /**
        Associates wxTempFile with the file to be replaced and opens it. You should use
        IsOpened() to verify if the constructor succeeded.
    */
    wxTempFile(const wxString& strName);

    /**
        Destructor calls Discard() if temporary file
        is still opened.
    */
    ~wxTempFile();

    /**
        Validate changes: deletes the old file of name m_strName and renames the new
        file to the old name. Returns @true if both actions succeeded. If @false is
        returned it may unfortunately mean two quite different things: either that
        either the old file couldn't be deleted or that the new file couldn't be renamed
        to the old name.
    */
    bool Commit();

    /**
        Discard changes: the old file contents is not changed, temporary file is
        deleted.
    */
    void Discard();

    /**
        Returns @true if the file was successfully opened.
    */
    bool IsOpened() const;

    /**
        Returns the length of the file.
    */
    wxFileOffset Length() const;

    /**
        Open the temporary file, returns @true on success, @false if an error
        occurred.
        @a strName is the name of file to be replaced. The temporary file is always
        created in the directory where @a strName is. In particular, if
        @a strName doesn't include the path, it is created in the current directory
        and the program should have write access to it for the function to succeed.
    */
    bool Open(const wxString& strName);

    /**
        Seeks to the specified position.
    */
    wxFileOffset Seek(wxFileOffset ofs,
                      wxSeekMode mode = wxFromStart);

    /**
        Returns the current position or wxInvalidOffset if file is not opened or if
        another
        error occurred.
    */
    wxFileOffset Tell() const;

    /**
        Write to the file, return @true on success, @false on failure.
        The second argument is only meaningful in Unicode build of wxWidgets when
        @a conv is used to convert @a str to multibyte representation.
    */
    bool Write(const wxString& str,
               const wxMBConv& conv = wxConvUTF8);
};


/**
    @class wxFile
    @wxheader{file.h}

    A wxFile performs raw file I/O. This is a very small class designed to
    minimize the overhead of using it - in fact, there is hardly any overhead at
    all, but using it brings you automatic error checking and hides differences
    between platforms and compilers. wxFile also automatically closes the file in
    its destructor making it unnecessary to worry about forgetting to do it.
    wxFile is a wrapper around @c file descriptor. - see also
    wxFFile for a wrapper around @c FILE structure.

    @c wxFileOffset is used by the wxFile functions which require offsets as
    parameter or return them. If the platform supports it, wxFileOffset is a typedef
    for a native 64 bit integer, otherwise a 32 bit integer is used for
    wxFileOffset.

    @library{wxbase}
    @category{file}
*/
class wxFile
{
public:
    //@{
    /**
        Associates the file with the given file descriptor, which has already been
        opened.
        
        @param filename
            The filename.
        @param mode
            The mode in which to open the file. May be one of read(), write() and
        wxFile::read_write.
        @param fd
            An existing file descriptor (see Attach() for the list of predefined
        descriptors)
    */
    wxFile();
    wxFile(const wxString& filename,
           wxFile::OpenMode mode = wxFile::read);
    wxFile(int fd);
    //@}

    /**
        Destructor will close the file.
        @b NB: it is not virtual so you should not use wxFile polymorphically.
    */
    ~wxFile();

    /**
        This function verifies if we may access the given file in specified mode. Only
        values of read() or write() really make sense here.
    */
    static bool Access(const wxString& name, OpenMode mode);

    /**
        Attaches an existing file descriptor to the wxFile object. Example of predefined
        file descriptors are 0, 1 and 2 which correspond to stdin, stdout and stderr
        (and
        have symbolic names of @b wxFile::fd_stdin, @b wxFile::fd_stdout and @b
        wxFile::fd_stderr).
        The descriptor should be already opened and it will be closed by wxFile
        object.
    */
    void Attach(int fd);

    /**
        Closes the file.
    */
    void Close();

    /**
        Creates a file for writing. If the file already exists, setting @b overwrite to
        @true
        will ensure it is overwritten.
    */
    bool Create(const wxString& filename, bool overwrite = false,
                int access = wxS_DEFAULT);

    /**
        Get back a file descriptor from wxFile object - the caller is responsible for
        closing the file if this
        descriptor is opened. IsOpened() will return @false after call to Detach().
    */
    void Detach();

    /**
        Returns @true if the end of the file has been reached.
        Note that the behaviour of the file pointer based class
        wxFFile is different as wxFFile::Eof
        will return @true here only if an attempt has been made to read
        @e past the last byte of the file, while wxFile::Eof() will return @true
        even before such attempt is made if the file pointer is at the last position
        in the file.
        Note also that this function doesn't work on unseekable file descriptors
        (examples include pipes, terminals and sockets under Unix) and an attempt to
        use it will result in an error message in such case. So, to read the entire
        file into memory, you should write a loop which uses
        Read() repeatedly and tests its return condition instead
        of using Eof() as this will not work for special files under Unix.
    */
    bool Eof() const;

    /**
        Returns @true if the given name specifies an existing regular file (not a
        directory or a link)
    */
    static bool Exists(const wxString& filename);

    /**
        Flushes the file descriptor.
        Note that Flush() is not implemented on some Windows compilers
        due to a missing fsync function, which reduces the usefulness of this function
        (it can still be called but it will do nothing on unsupported compilers).
    */
    bool Flush();

    /**
        Returns the type of the file. Possible return values are:
    */
    wxFileKind GetKind() const;

    /**
        Returns @true if the file has been opened.
    */
    bool IsOpened() const;

    /**
        Returns the length of the file.
    */
    wxFileOffset Length() const;

    /**
        Opens the file, returning @true if successful.
        
        @param filename
            The filename.
        @param mode
            The mode in which to open the file. May be one of read(), write() and
        wxFile::read_write.
    */
    bool Open(const wxString& filename,
              wxFile::OpenMode mode = wxFile::read);

    //@{
    /**
        if there was an error.
    */
    size_t Read(void* buffer, size_t count);
    Parameters Return value
    The number of bytes read, or the symbol wxInvalidOffset();
    //@}

    /**
        Seeks to the specified position.
        
        @param ofs
            Offset to seek to.
        @param mode
            One of wxFromStart, wxFromEnd, wxFromCurrent.
        
        @returns The actual offset position achieved, or wxInvalidOffset on
                 failure.
    */
    wxFileOffset Seek(wxFileOffset ofs,
                      wxSeekMode mode = wxFromStart);

    /**
        Moves the file pointer to the specified number of bytes relative to the end of
        the file. For example, @c SeekEnd(-5) would position the pointer 5
        bytes before the end.
        
        @param ofs
            Number of bytes before the end of the file.
        
        @returns The actual offset position achieved, or wxInvalidOffset on
                 failure.
    */
    wxFileOffset SeekEnd(wxFileOffset ofs = 0);

    /**
        Returns the current position or wxInvalidOffset if file is not opened or if
        another
        error occurred.
    */
    wxFileOffset Tell() const;

    /**
        Writes the contents of the string to the file, returns @true on success.
        The second argument is only meaningful in Unicode build of wxWidgets when
        @a conv is used to convert @a s to multibyte representation.
        Note that this method only works with @c NUL-terminated strings, if you want
        to write data with embedded @c NULs to the file you should use the other
        @ref write() "Write() overload".
    */
    bool Write(const wxString& s, const wxMBConv& conv = wxConvUTF8);

    /**
        Returns the file descriptor associated with the file.
    */
    int fd() const;
};
