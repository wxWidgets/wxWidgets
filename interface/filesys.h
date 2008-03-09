/////////////////////////////////////////////////////////////////////////////
// Name:        filesys.h
// Purpose:     documentation for wxFileSystem class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFileSystem
    @wxheader{filesys.h}

    This class provides an interface for opening files on different
    file systems. It can handle absolute and/or local filenames.
    It uses a system of handlers to
    provide access to user-defined virtual file systems.

    @library{wxbase}
    @category{vfs}

    @seealso
    wxFileSystemHandler, wxFSFile, Overview
*/
class wxFileSystem : public wxObject
{
public:
    /**
        Constructor.
    */
    wxFileSystem();

    /**
        This static function adds new handler into the list of
        handlers which provide access to virtual FS.
        Note that if two handlers for the same protocol are added, the last one added
        takes precedence.
    */
    static void AddHandler(wxFileSystemHandler handler);

    /**
        Sets the current location. @a location parameter passed to
        OpenFile() is relative to this path.
        @b Caution! Unless @a is_dir is @true the @a location parameter
        is not the directory name but the name of the file in this directory. All these
        commands change the path to "dir/subdir/":
        
        @param location
            the new location. Its meaning depends on the value of is_dir
        @param is_dir
            if @true location is new directory. If @false (default)
            location is file in the new directory.
    */
    void ChangePathTo(const wxString& location, bool is_dir = false);

    /**
        Converts filename into URL.
        
        @see URLToFileName(), wxFileName
    */
    static wxString FileNameToURL(wxFileName filename);

    /**
        Looks for the file with the given name @a file in a colon or semi-colon
        (depending on the current platform) separated list of directories in
        @e path. If the file is found in any directory, returns @true and the full
        path of the file in @e str, otherwise returns @false and doesn't modify
        @e str.
        
        @param str
            Receives the full path of the file, must not be @NULL
        @param path
            wxPATH_SEP-separated list of directories
        @param file
            the name of the file to look for
    */
    bool FindFileInPath(wxString str, const wxString& path,
                        const wxString& file);

    /**
        Works like wxFindFirstFile. Returns name of the first
        filename (within filesystem's current path) that matches @e wildcard. @a flags
        may be one of
        wxFILE (only files), wxDIR (only directories) or 0 (both).
    */
    wxString FindFirst(const wxString& wildcard, int flags = 0);

    /**
        Returns the next filename that matches parameters passed to FindFirst().
    */
    wxString FindNext();

    /**
        Returns actual path (set by wxFileSystem::ChangePathTo).
    */
    wxString GetPath();

    /**
        This static function returns @true if there is a registered handler which can
        open the given
        location.
    */
    static bool HasHandlerForPath(const wxString& location);

    /**
        Opens the file and returns a pointer to a wxFSFile object
        or @NULL if failed. It first tries to open the file in relative scope
        (based on value passed to ChangePathTo() method) and then as an
        absolute path.  Note that the user is responsible for deleting the returned
        wxFSFile.
        @a flags can be one or more of the following bit values ored together:
        
        A stream opened with just the default @e wxFS_READ flag may
        or may not be seekable depending on the underlying source.
        Passing @e wxFS_READ | wxFS_SEEKABLE for @a flags will
        back a stream that is not natively seekable with memory or a file
        and return a stream that is always seekable.
    */
    wxFSFile* OpenFile(const wxString& location,
                       int flags = wxFS_READ);

    /**
        Converts URL into a well-formed filename. The URL must use the @c file
        protocol.
    */
    static wxFileName URLToFileName(const wxString& url);
};


/**
    @class wxFSFile
    @wxheader{filesys.h}

    This class represents a single file opened by wxFileSystem.
    It provides more information than wxWindow's input stream
    (stream, filename, mime type, anchor).

    @b Note: Any pointer returned by a method of wxFSFile is valid
    only as long as the wxFSFile object exists. For example a call to GetStream()
    doesn't @e create the stream but only returns the pointer to it. In
    other words after 10 calls to GetStream() you will have obtained ten identical
    pointers.

    @library{wxbase}
    @category{vfs}

    @seealso
    wxFileSystemHandler, wxFileSystem, Overview
*/
class wxFSFile : public wxObject
{
public:
    /**
        Constructor. You probably won't use it. See Notes for details.
        
        @param stream
            The input stream that will be used to access data
        @param location
            The full location (aka filename) of the file
        @param mimetype
            MIME type of this file. It may be left empty, in which
            case the type will be determined from file's extension (location must
            not be empty in this case).
        @param anchor
            Anchor. See GetAnchor() for details.
    */
    wxFSFile(wxInputStream stream, const wxString& loc,
             const wxString& mimetype,
             const wxString& anchor, wxDateTime modif);

    /**
        Detaches the stream from the wxFSFile object. That is, the
        stream obtained with @c GetStream() will continue its existance
        after the wxFSFile object is deleted. You will have to delete
        the stream yourself.
    */
    void DetachStream();

    /**
        Returns anchor (if present). The term of @b anchor can be easily
        explained using few examples:
        
        Usually an anchor is presented only if the MIME type is 'text/html'.
        But it may have some meaning with other files;
        for example myanim.avi#200 may refer to position in animation
        or reality.wrl#MyView may refer to a predefined view in VRML.
    */
    const wxString GetAnchor();

    /**
        Returns full location of the file, including path and protocol.
        Examples :
    */
    const wxString GetLocation();

    /**
        Returns the MIME type of the content of this file. It is either
        extension-based (see wxMimeTypesManager) or extracted from
        HTTP protocol Content-Type header.
    */
    const wxString GetMimeType();

    /**
        Returns time when this file was modified.
    */
    wxDateTime GetModificationTime();

    /**
        Returns pointer to the stream. You can use the returned
        stream to directly access data. You may suppose
        that the stream provide Seek and GetSize functionality
        (even in the case of the HTTP protocol which doesn't provide
        this by default. wxHtml uses local cache to work around
        this and to speed up the connection).
    */
    wxInputStream* GetStream();
};


/**
    @class wxFileSystemHandler
    @wxheader{filesys.h}

    Classes derived from wxFileSystemHandler are used
    to access virtual file systems. Its public interface consists
    of two methods: wxFileSystemHandler::CanOpen
    and wxFileSystemHandler::OpenFile.
    It provides additional protected methods to simplify the process
    of opening the file: GetProtocol, GetLeftLocation, GetRightLocation,
    GetAnchor, GetMimeTypeFromExt.

    Please have a look at overview if you don't know how locations
    are constructed.

    Also consult @ref overview_fs "list of available handlers".

    @b wxPerl note: In wxPerl, you need to derive your file system handler class
    from Wx::PlFileSystemHandler.

    @library{wxbase}
    @category{vfs}

    @seealso
    wxFileSystem, wxFSFile, Overview
*/
class wxFileSystemHandler : public wxObject
{
public:
    /**
        Constructor.
    */
    wxFileSystemHandler();

    /**
        Returns @true if the handler is able to open this file. This function doesn't
        check whether the file exists or not, it only checks if it knows the protocol.
        Example:
        
        Must be overridden in derived handlers.
    */
    virtual bool CanOpen(const wxString& location);

    /**
        Works like wxFindFirstFile. Returns name of the first
        filename (within filesystem's current path) that matches @e wildcard. @a flags
        may be one of
        wxFILE (only files), wxDIR (only directories) or 0 (both).
        This method is only called if CanOpen() returns @true.
    */
    virtual wxString FindFirst(const wxString& wildcard,
                               int flags = 0);

    /**
        Returns next filename that matches parameters passed to wxFileSystem::FindFirst.
        This method is only called if CanOpen() returns @true and FindFirst
        returned a non-empty string.
    */
    virtual wxString FindNext();

    /**
        Returns the anchor if present in the location.
        See @ref wxFSFile::getanchor wxFSFile for details.
        Example: GetAnchor("index.htm#chapter2") == "chapter2"
        @b Note: the anchor is NOT part of the left location.
    */
    wxString GetAnchor(const wxString& location);

    /**
        Returns the left location string extracted from @e location.
        Example: GetLeftLocation("file:myzipfile.zip#zip:index.htm") ==
        "file:myzipfile.zip"
    */
    wxString GetLeftLocation(const wxString& location);

    /**
        Returns the MIME type based on @b extension of @e location. (While
        wxFSFile::GetMimeType
        returns real MIME type - either extension-based or queried from HTTP.)
        Example : GetMimeTypeFromExt("index.htm") == "text/html"
    */
    wxString GetMimeTypeFromExt(const wxString& location);

    /**
        Returns the protocol string extracted from @e location.
        Example: GetProtocol("file:myzipfile.zip#zip:index.htm") == "zip"
    */
    wxString GetProtocol(const wxString& location);

    /**
        Returns the right location string extracted from @e location.
        Example : GetRightLocation("file:myzipfile.zip#zip:index.htm") == "index.htm"
    */
    wxString GetRightLocation(const wxString& location);

    /**
        Opens the file and returns wxFSFile pointer or @NULL if failed.
        Must be overridden in derived handlers.
        
        @param fs
            Parent FS (the FS from that OpenFile was called). See ZIP handler
            for details of how to use it.
        @param location
            The absolute location of file.
    */
    virtual wxFSFile* OpenFile(wxFileSystem& fs,
                               const wxString& location);
};
