/////////////////////////////////////////////////////////////////////////////
// Name:        archive.h
// Purpose:     documentation for wxArchiveInputStream class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxArchiveInputStream
    @wxheader{archive.h}

    An abstract base class which serves as a common interface to
    archive input streams such as wxZipInputStream.

    wxArchiveInputStream::GetNextEntry returns an
     wxArchiveEntry object containing the meta-data
    for the next entry in the archive (and gives away ownership). Reading from
    the wxArchiveInputStream then returns the entry's data. Eof() becomes @true
    after an attempt has been made to read past the end of the entry's data.
    When there are no more entries, GetNextEntry() returns @NULL and sets Eof().

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", wxArchiveEntry,
    wxArchiveOutputStream
*/
class wxArchiveInputStream : public wxFilterInputStream
{
public:
    /**
        Closes the current entry. On a non-seekable stream reads to the end of
        the current entry first.
    */
    bool CloseEntry();

    /**
        Closes the current entry if one is open, then reads the meta-data for
        the next entry and returns it in a wxArchiveEntry
        object, giving away ownership. Reading this wxArchiveInputStream then
        returns the entry's data.
    */
    wxArchiveEntry* GetNextEntry();

    /**
        Closes the current entry if one is open, then opens the entry specified
        by the wxArchiveEntry object.
        
        @e entry must be from the same archive file that this
        wxArchiveInputStream is reading, and it must be reading it from a
        seekable stream.
    */
    bool OpenEntry(wxArchiveEntry& entry);
};


/**
    @class wxArchiveOutputStream
    @wxheader{archive.h}

    An abstract base class which serves as a common interface to
    archive output streams such as wxZipOutputStream.

    wxArchiveOutputStream::PutNextEntry is used
    to create a new entry in the output archive, then the entry's data is
    written to the wxArchiveOutputStream.  Another call to PutNextEntry()
    closes the current entry and begins the next.

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", wxArchiveEntry,
    wxArchiveInputStream
*/
class wxArchiveOutputStream : public wxFilterOutputStream
{
public:
    /**
        Calls Close() if it has not already
        been called.
    */
    ~wxArchiveOutputStream();

    /**
        Closes the archive, returning @true if it was successfully written.
        Called by the destructor if not called explicitly.
    */
    bool Close();

    /**
        Close the current entry. It is called implicitly whenever another new
        entry is created with CopyEntry()
        or PutNextEntry(), or
        when the archive is closed.
    */
    bool CloseEntry();

    /**
        Some archive formats have additional meta-data that applies to the archive
        as a whole.  For example in the case of zip there is a comment, which
        is stored at the end of the zip file.  CopyArchiveMetaData() can be used
        to transfer such information when writing a modified copy of an archive.
        
        Since the position of the meta-data can vary between the various archive
        formats, it is best to call CopyArchiveMetaData() before transferring
        the entries.  The wxArchiveOutputStream
        will then hold on to the meta-data and write it at the correct point in
        the output file.
        
        When the input archive is being read from a non-seekable stream, the
        meta-data may not be available when CopyArchiveMetaData() is called,
        in which case the two streams set up a link and transfer the data
        when it becomes available.
    */
    bool CopyArchiveMetaData(wxArchiveInputStream& stream);

    /**
        Takes ownership of @e entry and uses it to create a new entry in the
        archive. @e entry is then opened in the input stream @e stream
        and its contents copied to this stream.
        
        For archive types which compress entry data, CopyEntry() is likely to be
        much more efficient than transferring the data using Read() and Write()
        since it will copy them without decompressing and recompressing them.
        
        @e entry must be from the same archive file that @e stream is
        accessing. For non-seekable streams, @e entry must also be the last
        thing read from @e stream.
    */
    bool CopyEntry(wxArchiveEntry* entry,
                   wxArchiveInputStream& stream);

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
        
        Create a new entry with the given name, timestamp and size. The entry's
        data can then be written by writing to this wxArchiveOutputStream.
    */
    bool PutNextEntry(wxArchiveEntry* entry);
    bool PutNextEntry(const wxString& name);
    //@}
};


/**
    @class wxArchiveEntry
    @wxheader{archive.h}

    An abstract base class which serves as a common interface to
    archive entry classes such as wxZipEntry.
    These hold the meta-data (filename, timestamp, etc.), for entries
    in archive files such as zips and tars.

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", @ref overview_wxarcgeneric
    "Generic archive programming", wxArchiveInputStream, wxArchiveOutputStream, wxArchiveNotifier
*/
class wxArchiveEntry : public wxObject
{
public:
    /**
        Returns a copy of this entry object.
    */
    wxArchiveEntry* Clone();

    //@{
    /**
        The entry's timestamp.
    */
    wxDateTime GetDateTime();
    void SetDateTime(const wxDateTime& dt);
    //@}

    //@{
    /**
        The entry's name, by default in the native format. The name can include
        directory components, i.e. it can be a full path.
        
        If this is a directory entry, (i.e. if IsDir()
        is @true) then GetName() returns the name with a trailing path separator.
        
        Similarly, setting a name with a trailing path separator sets IsDir().
    */
    wxString GetName(wxPathFormat format = wxPATH_NATIVE);
    void SetName(const wxString& name,
                 wxPathFormat format = wxPATH_NATIVE);
    //@}

    //@{
    /**
        The size of the entry's data in bytes.
    */
    off_t GetSize();
    void SetSize(off_t size);
    //@}

    /**
        Returns the path format used internally within the archive to store
        filenames.
    */
    wxPathFormat GetInternalFormat();

    /**
        Returns the entry's filename in the internal format used within the
        archive. The name can include directory components, i.e. it can be a
        full path.
        
        The names of directory entries are returned without any trailing path
        separator. This gives a canonical name that can be used in comparisons.
        
        @sa @ref overview_wxarcbyname "Looking up an archive entry by name"
    */
    wxString GetInternalName();

    /**
        Returns a numeric value unique to the entry within the archive.
    */
    off_t GetOffset();

    //@{
    /**
        True if this is a directory entry.
        
        Directory entries are entries with no data, which are used to store
        the meta-data of directories. They also make it possible for completely
        empty directories to be stored.
        
        The names of entries within an archive can be complete paths, and
        unarchivers typically create whatever directories are necessary as they
        restore files, even if the archive contains no explicit directory entries.
    */
    bool IsDir();
    void SetIsDir(bool isDir = @true);
    //@}

    //@{
    /**
        True if the entry is a read-only file.
    */
    bool IsReadOnly();
    void SetIsReadOnly(bool isReadOnly = @true);
    //@}

    //@{
    /**
        Sets the notifier for this entry.
        Whenever the wxArchiveInputStream updates
        this entry, it will then invoke the associated
        notifier's wxArchiveNotifier::OnEntryUpdated
        method.
        
        Setting a notifier is not usually necessary. It is used to handle
        certain cases when modifying an archive in a pipeline (i.e. between
        non-seekable streams).
    */
    void SetNotifier(wxArchiveNotifier& notifier);
    void UnsetNotifier();
    //@}
};


/**
    @class wxArchiveClassFactory
    @wxheader{archive.h}

    Allows the creation of streams to handle archive formats such
    as zip and tar.

    For example, given a filename you can search for a factory that will
    handle it and create a stream to read it:

    @code
    factory = wxArchiveClassFactory::Find(filename, wxSTREAM_FILEEXT);
        if (factory)
            stream = factory-NewStream(new wxFFileInputStream(filename));
    @endcode

    wxArchiveClassFactory::Find can also search
    for a factory by MIME type or wxFileSystem protocol.
    The available factories can be enumerated
    using @ref wxArchiveClassFactory::getfirst "GetFirst() and GetNext".

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarc "Archive formats such as zip", @ref overview_wxarcgeneric
    "Generic archive programming", wxArchiveEntry, wxArchiveInputStream, wxArchiveOutputStream, wxFilterClassFactory
*/
class wxArchiveClassFactory : public wxObject
{
public:
    /**
        Returns @true if this factory can handle the given protocol, MIME type
        or file extension.
        
        When using wxSTREAM_FILEEXT for the second parameter, the first parameter
        can be a complete filename rather than just an extension.
    */
    bool CanHandle(const wxChar* protocol,
                   wxStreamProtocolType type = wxSTREAM_PROTOCOL);

    /**
        A static member that finds a factory that can handle a given protocol, MIME
        type or file extension.  Returns a pointer to the class factory if found, or
        @NULL otherwise. It does not give away ownership of the factory.
        
        When using wxSTREAM_FILEEXT for the second parameter, the first parameter
        can be a complete filename rather than just an extension.
    */
    static const wxArchiveClassFactory* Find(const wxChar* protocol,
            wxStreamProtocolType type = wxSTREAM_PROTOCOL);

    //@{
    /**
        The wxMBConv object that the created streams
        will use when translating meta-data. The initial default, set by the
        constructor, is wxConvLocal.
    */
    wxMBConv GetConv();
    void SetConv(wxMBConv& conv);
    //@}

    //@{
    /**
        GetFirst and GetNext can be used to enumerate the available factories.
        
        For example, to list them:
        GetFirst()/GetNext() return a pointer to a factory or @NULL if no more
        are available. They do not give away ownership of the factory.
    */
    static const wxArchiveClassFactory* GetFirst();
    const wxArchiveClassFactory* GetNext();
    //@}

    /**
        Calls the static GetInternalName() function for the archive entry type,
        for example
         wxZipEntry::GetInternalName.
    */
    wxString GetInternalName(const wxString& name,
                             wxPathFormat format = wxPATH_NATIVE);

    /**
        Returns the wxFileSystem protocol supported by this factory. Equivalent
        to wxString(*GetProtcols()).
    */
    wxString GetProtocol();

    /**
        Returns the protocols, MIME types or file extensions supported by this
        factory, as an array of null terminated strings. It does not give away
        ownership of the array or strings.
        
        For example, to list the file extensions a factory supports:
    */
    const wxChar * const* GetProtocols(wxStreamProtocolType type = wxSTREAM_PROTOCOL);

    /**
        Create a new wxArchiveEntry object of the
        appropriate type.
    */
    wxArchiveEntry* NewEntry();

    //@{
    /**
        Create a new input or output stream to read or write an archive.
        
        If the parent stream is passed as a pointer then the new archive stream
        takes ownership of it. If it is passed by reference then it does not.
    */
    wxArchiveInputStream* NewStream(wxInputStream& stream);
    wxArchiveOutputStream* NewStream(wxOutputStream& stream);
    wxArchiveInputStream* NewStream(wxInputStream* stream);
    wxArchiveOutputStream* NewStream(wxOutputStream* stream);
    //@}

    /**
        Adds this class factory to the list returned
        by @ref getfirst() GetFirst()/GetNext.
        
        It is not necessary to do this to use the archive streams. It is usually
        used when implementing streams, typically the implementation will
        add a static instance of its factory class.
        
        It can also be used to change the order of a factory already in the list,
        bringing it to the front. This isn't a thread safe operation
        so can't be done when other threads are running that will be using the list.
        
        The list does not take ownership of the factory.
    */
    void PushFront();

    /**
        Removes this class factory from the list returned
        by @ref getfirst() GetFirst()/GetNext.
        
        Removing from the list isn't a thread safe operation
        so can't be done when other threads are running that will be using the list.
        
        The list does not own the factories, so removing a factory does not delete it.
    */
    void Remove();
};


/**
    @class wxArchiveNotifier
    @wxheader{archive.h}

    If you need to know when a
     wxArchiveInputStream updates a
     wxArchiveEntry object, you can create
    a notifier by deriving from this abstract base class, overriding
     wxArchiveNotifier::OnEntryUpdated.  An instance
    of your notifier class can then be assigned to the wxArchiveEntry object
    using wxArchiveEntry::SetNotifier.
    Your OnEntryUpdated() method will then be invoked whenever the input
    stream updates the entry.

    Setting a notifier is not usually necessary. It is used to handle
    certain cases when modifying an archive in a pipeline (i.e. between
    non-seekable streams).
    See @ref overview_wxarcnoseek "Archives on non-seekable streams".

    @library{wxbase}
    @category{FIXME}

    @seealso
    @ref overview_wxarcnoseek "Archives on non-seekable streams", wxArchiveEntry,
    wxArchiveInputStream, wxArchiveOutputStream
*/
class wxArchiveNotifier
{
public:
    /**
        This method must be overridden in your derived class.
    */
    void OnEntryUpdated(class wxArchiveEntry& entry);
};


/**
    @class wxArchiveIterator
    @wxheader{archive.h}

    An input iterator template class that can be used to transfer an archive's
    catalogue to a container. It is only available if wxUSE_STL is set to 1
    in setup.h, and the uses for it outlined below require a compiler which
    supports member templates.

    @code
    template class Arc, class T = typename Arc::entry_type*
    class wxArchiveIterator
    {
        // this constructor creates an 'end of sequence' object
        wxArchiveIterator();

        // template parameter 'Arc' should be the type of an archive input stream
        wxArchiveIterator(Arc& arc) {

        /* ... */
};
@endcode

The first template parameter should be the type of archive input stream
(e.g. wxArchiveInputStream) and the
second can either be a pointer to an entry
(e.g. wxArchiveEntry*), or a string/pointer pair
(e.g. std::pairwxString, wxArchiveEntry*).

The @c wx/archive.h header defines the following typedefs:

@code
typedef wxArchiveIteratorwxArchiveInputStream wxArchiveIter;

typedef wxArchiveIteratorwxArchiveInputStream,
std::pairwxString, wxArchiveEntry*  wxArchivePairIter;
@endcode

The header for any implementation of this interface should define similar
typedefs for its types, for example in @c wx/zipstrm.h there is:

        @code
        typedef wxArchiveIteratorwxZipInputStream wxZipIter;

typedef wxArchiveIteratorwxZipInputStream,
std::pairwxString, wxZipEntry*  wxZipPairIter;
@endcode

Transferring the catalogue of an archive @e arc to a vector @e cat,
can then be done something like this:

@code
std::vectorwxArchiveEntry* cat((wxArchiveIter)arc, wxArchiveIter());
@endcode

When the iterator is dereferenced, it gives away ownership of an entry
object. So in the above example, when you have finished with @e cat
you must delete the pointers it contains.

If you have smart pointers with normal copy semantics (i.e. not auto_ptr
        or wxScopedPtr), then you can create an iterator
which uses them instead.  For example, with a smart pointer class for
zip entries @e ZipEntryPtr:

@code
typedef std::vectorZipEntryPtr ZipCatalog;
typedef wxArchiveIteratorwxZipInputStream, ZipEntryPtr ZipIter;
ZipCatalog cat((ZipIter)zip, ZipIter());
@endcode

Iterators that return std::pair objects can be used to
populate a std::multimap, to allow entries to be looked
up by name. The string is initialised using the wxArchiveEntry object's
wxArchiveEntry::GetInternalName function.

@code
typedef std::multimapwxString, wxZipEntry* ZipCatalog;
ZipCatalog cat((wxZipPairIter)zip, wxZipPairIter());
@endcode


Note that this iterator also gives away ownership of an entry
object each time it is dereferenced. So in the above example, when
you have finished with @e cat you must delete the pointers it contains.

Or if you have them, a pair containing a smart pointer can be used
(again @e ZipEntryPtr), no worries about ownership:

@code
typedef std::multimapwxString, ZipEntryPtr ZipCatalog;
typedef wxArchiveIteratorwxZipInputStream,
std::pairwxString, ZipEntryPtr  ZipPairIter;
ZipCatalog cat((ZipPairIter)zip, ZipPairIter());
@endcode

@library{wxbase}
@category{FIXME}

@seealso
wxArchiveEntry, wxArchiveInputStream, wxArchiveOutputStream
*/
class wxArchiveIterator
{
public:
//@{
/**
Construct iterator that returns all the entries in the archive input
stream @e arc.
*/
wxArchiveIterator();
wxArchiveIterator(Arc& arc);
//@}

/**
Returns an entry object from the archive input stream, giving away
ownership.
*/
const T operator*();

//@{
/**
Position the input iterator at the next entry in the archive input stream.
*/
wxArchiveIterator operator++();
wxArchiveIterator operator++(int );
//@}
};
