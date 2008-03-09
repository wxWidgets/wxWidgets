/////////////////////////////////////////////////////////////////////////////
// Name:        zipstrm.h
// Purpose:     documentation for wxZipNotifier class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxZipNotifier
    @wxheader{zipstrm.h}

    If you need to know when a wxZipInputStream
    updates a wxZipEntry,
    you can create a notifier by deriving from this abstract base class,
    overriding wxZipNotifier::OnEntryUpdated.
    An instance of your notifier class can then be assigned to wxZipEntry
    objects, using wxZipEntry::SetNotifier.

    Setting a notifier is not usually necessary. It is used to handle
    certain cases when modifying an zip in a pipeline (i.e. between
    non-seekable streams).
    See '@ref overview_wxarcnoseek "Archives on non-seekable streams"'.

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarcnoseek "Archives on non-seekable streams", wxZipEntry,
    wxZipInputStream, wxZipOutputStream
*/
class wxZipNotifier
{
public:
    /**
        Override this to receive notifications when
        an wxZipEntry object changes.
    */
    void OnEntryUpdated(wxZipEntry& entry);
};


/**
    @class wxZipEntry
    @wxheader{zipstrm.h}

    Holds the meta-data for an entry in a zip.

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", wxZipInputStream,
    wxZipOutputStream, wxZipNotifier
*/
class wxZipEntry : public wxArchiveEntry
{
public:
    //@{
    /**
        Copy constructor.
    */
    wxZipEntry(const wxString& name = wxEmptyString);
    wxZipEntry(const wxZipEntry& entry);
    //@}

    /**
        Make a copy of this entry.
    */
    wxZipEntry* Clone() const;

    //@{
    /**
        A short comment for this entry.
    */
    wxString GetComment();
    const void SetComment(const wxString& comment);
    //@}

    //@{
    /**
        The low 8 bits are always the DOS/Windows file attributes for this entry.
        The values of these attributes are given in the
        enumeration @c wxZipAttributes.
        The remaining bits can store platform specific permission bits or
        attributes, and their meaning depends on the value
        of @ref systemmadeby() SetSystemMadeBy.
        If IsMadeByUnix() is @true then the
        high 16 bits are unix mode bits.
        The following other accessors access these bits:
        @ref wxArchiveEntry::isreadonly IsReadOnly/SetIsReadOnly
        
        @ref wxArchiveEntry::isdir IsDir/SetIsDir
        
        @ref mode() Get/SetMode
    */
    wxUint32 GetExternalAttributes();
    const void SetExternalAttributes(wxUint32 attr);
    //@}

    //@{
    /**
        The extra field from the entry's central directory record.
        The extra field is used to store platform or application specific
        data. See Pkware's document 'appnote.txt' for information on its format.
    */
    const char* GetExtra();
    const size_t GetExtraLen();
    const void SetExtra(const char* extra, size_t len);
    //@}

    //@{
    /**
        The extra field from the entry's local record.
        The extra field is used to store platform or application specific
        data. See Pkware's document 'appnote.txt' for information on its format.
    */
    const char* GetLocalExtra();
    const size_t GetLocalExtraLen();
    const void SetLocalExtra(const char* extra, size_t len);
    //@}

    //@{
    /**
        The compression method. The enumeration @c wxZipMethod lists the
        possible values.
        The default constructor sets this to wxZIP_METHOD_DEFAULT,
        which allows wxZipOutputStream to
        choose the method when writing the entry.
    */
    int GetMethod();
    const void SetMethod(int method);
    //@}

    //@{
    /**
        Sets the DOS attributes
        in @ref externalattributes() GetExternalAttributes
        to be consistent with the @c mode given.
        If IsMadeByUnix() is @true then also
        stores @c mode in GetExternalAttributes().
        Note that the default constructor
        sets @ref systemmadeby() GetSystemMadeBy to
        wxZIP_SYSTEM_MSDOS by default. So to be able to store unix
        permissions when creating zips, call SetSystemMadeBy(wxZIP_SYSTEM_UNIX).
    */
    int GetMode();
    const void SetMode(int mode);
    //@}

    //@{
    /**
        The originating file-system. The default constructor sets this to
        wxZIP_SYSTEM_MSDOS. Set it to wxZIP_SYSTEM_UNIX in order to be
        able to store unix permissions using @ref mode() SetMode.
    */
    int GetSystemMadeBy();
    const void SetSystemMadeBy(int system);
    //@}

    /**
        The compressed size of this entry in bytes.
    */
    off_t GetCompressedSize() const;

    /**
        CRC32 for this entry's data.
    */
    wxUint32 GetCrc() const;

    /**
        Returns a combination of the bits flags in the enumeration @c wxZipFlags.
    */
    int GetFlags() const;

    //@{
    /**
        A static member that translates a filename into the internal format used
        within the archive. If the third parameter is provided, the bool pointed
        to is set to indicate whether the name looks like a directory name
        (i.e. has a trailing path separator).
        
        @see @ref overview_wxarcbyname "Looking up an archive entry by name"
    */
    wxString GetInternalName();
    const wxString  GetInternalName(const wxString& name,
                                    wxPathFormat format = wxPATH_NATIVE,
                                    bool* pIsDir = NULL);
    //@}

    /**
        Returns @true if @ref systemmadeby() GetSystemMadeBy
        is a flavour of unix.
    */
    bool IsMadeByUnix() const;

    //@{
    /**
        Indicates that this entry's data is text in an 8-bit encoding.
    */
    bool IsText();
    const void SetIsText(bool isText = true);
    //@}

    //@{
    /**
        Sets the notifier for this entry.
        Whenever the wxZipInputStream updates
        this entry, it will then invoke the associated
        notifier's wxZipNotifier::OnEntryUpdated
        method.
        Setting a notifier is not usually necessary. It is used to handle
        certain cases when modifying an zip in a pipeline (i.e. between
        non-seekable streams).
        
        @see @ref overview_wxarcnoseek "Archives on non-seekable streams", wxZipNotifier
    */
    void SetNotifier(wxZipNotifier& notifier);
    void UnsetNotifier();
    //@}

    /**
        Assignment operator.
    */
    wxZipEntry& operator operator=(const wxZipEntry& entry);
};


/**
    @class wxZipInputStream
    @wxheader{zipstrm.h}

    Input stream for reading zip files.

    wxZipInputStream::GetNextEntry returns an
     wxZipEntry object containing the meta-data
    for the next entry in the zip (and gives away ownership). Reading from
    the wxZipInputStream then returns the entry's data. Eof() becomes @true
    after an attempt has been made to read past the end of the entry's data.
    When there are no more entries, GetNextEntry() returns @NULL and sets Eof().

    Note that in general zip entries are not seekable, and
    wxZipInputStream::SeekI() always returns wxInvalidOffset.

    @library{wxbase}
    @category{streams}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", wxZipEntry, wxZipOutputStream
*/
class wxZipInputStream : public wxArchiveInputStream
{
public:
    //@{
    /**
        Compatibility constructor (requires WXWIN_COMPATIBILITY_2_6).
        When this constructor is used, an emulation of seeking is
        switched on for compatibility with previous versions. Note however,
        that it is deprecated.
    */
    wxZipInputStream(wxInputStream& stream,
                     wxMBConv& conv = wxConvLocal);
    wxZipInputStream(wxInputStream* stream,
                     wxMBConv& conv = wxConvLocal);
    wxZipInputStream(const wxString& archive,
                     const wxString& file);
    //@}

    /**
        Closes the current entry. On a non-seekable stream reads to the end of
        the current entry first.
    */
    bool CloseEntry();

    /**
        Returns the zip comment.
        This is stored at the end of the zip, therefore when reading a zip
        from a non-seekable stream, it returns the empty string until the
        end of the zip has been reached, i.e. when GetNextEntry() returns
        @NULL.
    */
    wxString GetComment();

    /**
        Closes the current entry if one is open, then reads the meta-data for
        the next entry and returns it in a wxZipEntry
        object, giving away ownership. The stream is then open and can be read.
    */
    wxZipEntry* GetNextEntry();

    /**
        For a zip on a seekable stream returns the total number of entries in
        the zip. For zips on non-seekable streams returns the number of entries
        returned so far by GetNextEntry().
    */
    int GetTotalEntries();

    /**
        Closes the current entry if one is open, then opens the entry specified
        by the @a entry object.
        @a entry should be from the same zip file, and the zip should
        be on a seekable stream.
    */
    bool OpenEntry(wxZipEntry& entry);
};


/**
    @class wxZipClassFactory
    @wxheader{zipstrm.h}

    Class factory for the zip archive format. See the base class
    for details.

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", @ref overview_wxarcgeneric
    "Generic archive programming", wxZipEntry, wxZipInputStream, wxZipOutputStream
*/
class wxZipClassFactory : public wxArchiveClassFactory
{
public:

};


/**
    @class wxZipOutputStream
    @wxheader{zipstrm.h}

    Output stream for writing zip files.

    wxZipOutputStream::PutNextEntry is used to create
    a new entry in the output zip, then the entry's data is written to the
    wxZipOutputStream.  Another call to PutNextEntry() closes the current
    entry and begins the next.

    @library{wxbase}
    @category{streams}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", wxZipEntry, wxZipInputStream
*/
class wxZipOutputStream : public wxArchiveOutputStream
{
public:
    //@{
    /**
        Constructor. @c level is the compression level to use.
        It can be a value between 0 and 9 or -1 to use the default value
        which currently is equivalent to 6.
        If the parent stream is passed as a pointer then the new filter stream
        takes ownership of it. If it is passed by reference then it does not.
        In a Unicode build the third parameter @c conv is used to translate
        the filename and comment fields to an 8-bit encoding. It has no effect on the
        stream's data.
    */
    wxZipOutputStream(wxOutputStream& stream, int level = -1,
                      wxMBConv& conv = wxConvLocal);
    wxZipOutputStream(wxOutputStream* stream, int level = -1,
                      wxMBConv& conv = wxConvLocal);
    //@}

    /**
        The destructor calls Close() to finish
        writing the zip if it has not been called already.
    */
    ~wxZipOutputStream();

    /**
        Finishes writing the zip, returning @true if successful.
        Called by the destructor if not called explicitly.
    */
    bool Close();

    /**
        Close the current entry. It is called implicitly whenever another new
        entry is created with CopyEntry()
        or PutNextEntry(), or
        when the zip is closed.
    */
    bool CloseEntry();

    /**
        Transfers the zip comment from the wxZipInputStream
        to this output stream.
    */
    bool CopyArchiveMetaData(wxZipInputStream& inputStream);

    /**
        Takes ownership of @c entry and uses it to create a new entry
        in the zip. @c entry is then opened in @c inputStream and its contents
        copied to this stream.
        CopyEntry() is much more efficient than transferring the data using
        Read() and Write() since it will copy them without decompressing and
        recompressing them.
        For zips on seekable streams, @c entry must be from the same zip file
        as @c stream. For non-seekable streams, @c entry must also be the
        last thing read from @c inputStream.
    */
    bool CopyEntry(wxZipEntry* entry, wxZipInputStream& inputStream);

    //@{
    /**
        Set the compression level that will be used the next time an entry is
        created. It can be a value between 0 and 9 or -1 to use the default value
        which currently is equivalent to 6.
    */
    int GetLevel();
    const void SetLevel(int level);
    //@}

    /**
        )
        Create a new directory entry
        (see wxArchiveEntry::IsDir)
        with the given name and timestamp.
        PutNextEntry() can
        also be used to create directory entries, by supplying a name with
        a trailing path separator.
    */
    bool PutNextDirEntry(const wxString& name);

    //@{
    /**
        , @b off_t@e size = wxInvalidOffset)
        Create a new entry with the given name, timestamp and size.
    */
    bool PutNextEntry(wxZipEntry* entry);
    bool PutNextEntry(const wxString& name);
    //@}

    /**
        Sets a comment for the zip as a whole. It is written at the end of the
        zip.
    */
    void SetComment(const wxString& comment);
};
