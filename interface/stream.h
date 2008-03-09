/////////////////////////////////////////////////////////////////////////////
// Name:        stream.h
// Purpose:     documentation for wxCountingOutputStream class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxCountingOutputStream
    @wxheader{stream.h}

    wxCountingOutputStream is a specialized output stream which does not write any
    data anywhere,
    instead it counts how many bytes would get written if this were a normal
    stream. This
    can sometimes be useful or required if some data gets serialized to a stream or
    compressed
    by using stream compression and thus the final size of the stream cannot be
    known other
    than pretending to write the stream. One case where the resulting size would
    have to be
    known is if the data has to be written to a piece of memory and the memory has
    to be
    allocated before writing to it (which is probably always the case when writing
    to a
    memory stream).

    @library{wxbase}
    @category{streams}
*/
class wxCountingOutputStream : public wxOutputStream
{
public:
    /**
        Creates a wxCountingOutputStream object.
    */
    wxCountingOutputStream();

    /**
        Destructor.
    */
    ~wxCountingOutputStream();

    /**
        Returns the current size of the stream.
    */
    size_t GetSize();
};


/**
    @class wxBufferedInputStream
    @wxheader{stream.h}

    This stream acts as a cache. It caches the bytes read from the specified
    input stream (See wxFilterInputStream).
    It uses wxStreamBuffer and sets the default in-buffer size to 1024 bytes.
    This class may not be used without some other stream to read the data
    from (such as a file stream or a memory stream).

    @library{wxbase}
    @category{streams}

    @seealso
    wxStreamBuffer, wxInputStream, wxBufferedOutputStream
*/
class wxBufferedInputStream : public wxFilterInputStream
{
public:

};


/**
    @class wxStreamBuffer
    @wxheader{stream.h}


    @library{wxbase}
    @category{streams}

    @seealso
    wxStreamBase
*/
class wxStreamBuffer
{
public:
    //@{
    /**
        Constructor. It initializes the stream buffer with the data of the specified
        stream buffer. The new stream buffer has the same attributes, size, position
        and they share the same buffer. This will cause problems if the stream to
        which the stream buffer belong is destroyed and the newly cloned stream
        buffer continues to be used, trying to call functions in the (destroyed)
        stream. It is advised to use this feature only in very local area of the
        program.
        
        @see @ref setbufferio() wxStreamBuffer:SetBufferIO
    */
    wxStreamBuffer(wxStreamBase& stream, BufMode mode);
    wxStreamBuffer(BufMode mode);
    wxStreamBuffer(const wxStreamBuffer& buffer);
    //@}

    /**
        Destructor. It finalizes all IO calls and frees all internal buffers if
        necessary.
    */
    wxStreamBuffer();

    /**
        Fill the IO buffer.
    */
    bool FillBuffer();

    /**
        Toggles the fixed flag. Usually this flag is toggled at the same time as
        @e flushable. This flag allows (when it has the @false value) or forbids
        (when it has the @true value) the stream buffer to resize dynamically the IO
        buffer.
        
        @see SetBufferIO()
    */
    void Fixed(bool fixed);

    /**
        Flushes the IO buffer.
    */
    bool FlushBuffer();

    /**
        Toggles the flushable flag. If @a flushable is disabled, no data are sent
        to the parent stream.
    */
    void Flushable(bool flushable);

    /**
        Returns a pointer on the end of the stream buffer.
    */
    void* GetBufferEnd();

    /**
        Returns a pointer on the current position of the stream buffer.
    */
    void* GetBufferPos();

    /**
        Returns the size of the buffer.
    */
    size_t GetBufferSize();

    /**
        Returns a pointer on the start of the stream buffer.
    */
    void* GetBufferStart();

    /**
        Gets a single char from the stream buffer. It acts like the Read call.
        
        @see Read()
    */
    char GetChar();

    /**
        Returns the amount of available data in the buffer.
    */
    size_t GetDataLeft();

    /**
        Returns the current position (counted in bytes) in the stream buffer.
    */
    off_t GetIntPosition();

    /**
        Returns the amount of bytes read during the last IO call to the parent stream.
    */
    size_t GetLastAccess();

    /**
        Puts a single char to the stream buffer.
        
        @see Read()
    */
    void PutChar(char c);

    //@{
    /**
        Copies data to @e buffer. The function returns when @a buffer is full or when
        there isn't
        any more data in the current buffer.
        
        @see Write()
    */
    size_t Read(void* buffer, size_t size);
    Return value size_t Read(wxStreamBuffer* buffer);
    //@}

    /**
        Resets to the initial state variables concerning the buffer.
    */
    void ResetBuffer();

    /**
        Changes the current position.
        @a mode may be one of the following:
        
        @b wxFromStart
        
        The position is counted from the start of the stream.
        
        @b wxFromCurrent
        
        The position is counted from the current position of the stream.
        
        @b wxFromEnd
        
        The position is counted from the end of the stream.
        
        @returns Upon successful completion, it returns the new offset as
                 measured in bytes from the beginning of the stream.
                 Otherwise, it returns wxInvalidOffset.
    */
    off_t Seek(off_t pos, wxSeekMode mode);

    //@{
    /**
        Destroys or invalidates the previous IO buffer and allocates a new one of the
        specified size.
        
        @see Fixed(), Flushable()
    */
    void SetBufferIO(char* buffer_start, char* buffer_end);
    Remarks See also
    wxStreamBuffer constructor

    wxStreamBuffer::Fixed

    wxStreamBuffer::Flushable
    void SetBufferIO(size_t bufsize);
    //@}

    /**
        Sets the current position (in bytes) in the stream buffer.
    */
    void SetIntPosition(size_t pos);

    /**
        Returns the parent stream of the stream buffer.
    */
    wxStreamBase* Stream();

    /**
        Gets the current position in the stream. This position is calculated from
        the @e real position in the stream and from the internal buffer position: so
        it gives you the position in the @e real stream counted from the start of
        the stream.
        
        @returns Returns the current position in the stream if possible,
                 wxInvalidOffset in the other case.
    */
    off_t Tell();

    /**
        Truncates the buffer to the current position.
        Note: Truncate() cannot be used to enlarge the buffer. This is
        usually not needed since the buffer expands automatically.
    */
    void Truncate();

    //@{
    /**
        See Read().
    */
    size_t Write(const void* buffer, size_t size);
    size_t Write(wxStreamBuffer* buffer);
    //@}
};


/**
    @class wxOutputStream
    @wxheader{stream.h}

    wxOutputStream is an abstract base class which may not be used directly.

    @library{wxbase}
    @category{streams}
*/
class wxOutputStream : public wxStreamBase
{
public:
    /**
        Creates a dummy wxOutputStream object.
    */
    wxOutputStream();

    /**
        Destructor.
    */
    ~wxOutputStream();

    /**
        Closes the stream, returning @false if an error occurs. The
        stream is closed implicitly in the destructor if Close() is not
        called explicitly.
        If this stream wraps another stream or some other resource such
        as a file, then the underlying resource is closed too if it is owned
        by this stream, or left open otherwise.
    */
    bool Close();

    /**
        Returns the number of bytes written during the last
        Write(). It may return 0 even if there is no
        error on the stream if it is only temporarily impossible to write to it.
    */
    size_t LastWrite();

    /**
        Puts the specified character in the output queue and increments the
        stream position.
    */
    void PutC(char c);

    /**
        Changes the stream current position.
        
        @param pos
            Offset to seek to.
        @param mode
            One of wxFromStart, wxFromEnd, wxFromCurrent.
        
        @returns The new stream position or wxInvalidOffset on error.
    */
    off_t SeekO(off_t pos, wxSeekMode mode = wxFromStart);

    /**
        Returns the current stream position.
    */
    off_t TellO();

    //@{
    /**
        Reads data from the specified input stream and stores them
        in the current stream. The data is read until an error is raised
        by one of the two streams.
    */
    wxOutputStream Write(const void* buffer, size_t size);
    wxOutputStream Write(wxInputStream& stream_in);
    //@}
};


/**
    @class wxFilterClassFactory
    @wxheader{stream.h}

    Allows the creation of filter streams to handle compression formats such
    as gzip and bzip2.

    For example, given a filename you can search for a factory that will
    handle it and create a stream to decompress it:

    @code
    factory = wxFilterClassFactory::Find(filename, wxSTREAM_FILEEXT);
        if (factory)
            stream = factory-NewStream(new wxFFileInputStream(filename));
    @endcode

    wxFilterClassFactory::Find can also search
    for a factory by MIME type, HTTP encoding or by wxFileSystem protocol.
    The available factories can be enumerated
    using @ref wxFilterClassFactory::getfirst "GetFirst() and GetNext".

    @library{wxbase}
    @category{FIXME}

    @seealso
    wxFilterInputStream, wxFilterOutputStream, wxArchiveClassFactory, @ref
    overview_wxarc "Archive formats such as zip"
*/
class wxFilterClassFactory : public wxObject
{
public:
    /**
        Returns @true if this factory can handle the given protocol, MIME type, HTTP
        encoding or file extension.
        When using wxSTREAM_FILEEXT for the second parameter, the first parameter
        can be a complete filename rather than just an extension.
    */
    bool CanHandle(const wxString& protocol,
                   wxStreamProtocolType type = wxSTREAM_PROTOCOL);

    /**
        A static member that finds a factory that can handle a given protocol, MIME
        type, HTTP encoding or file extension.  Returns a pointer to the class
        factory if found, or @NULL otherwise. It does not give away ownership of the
        factory.
        When using wxSTREAM_FILEEXT for the second parameter, the first parameter
        can be a complete filename rather than just an extension.
    */
    static const wxFilterClassFactory* Find(const wxString& protocol,
                                            wxStreamProtocolType type = wxSTREAM_PROTOCOL);

    //@{
    /**
        GetFirst and GetNext can be used to enumerate the available factories.
        For example, to list them:
        
        GetFirst()/GetNext() return a pointer to a factory or @NULL if no more
        are available. They do not give away ownership of the factory.
    */
    static const wxFilterClassFactory* GetFirst();
    const wxFilterClassFactory* GetNext();
    //@}

    /**
        Returns the wxFileSystem protocol supported by this factory. Equivalent
        to wxString(*GetProtcols()).
    */
    wxString GetProtocol();

    /**
        Returns the protocols, MIME types, HTTP encodings or file extensions
        supported by this factory, as an array of null terminated strings. It does
        not give away ownership of the array or strings.
        For example, to list the file extensions a factory supports:
    */
    const wxChar* const* GetProtocols(wxStreamProtocolType type = wxSTREAM_PROTOCOL);

    //@{
    /**
        Create a new input or output stream to decompress or compress a given stream.
        If the parent stream is passed as a pointer then the new filter stream
        takes ownership of it. If it is passed by reference then it does not.
    */
    wxFilterInputStream* NewStream(wxInputStream& stream);
    wxFilterOutputStream* NewStream(wxOutputStream& stream);
    wxFilterInputStream* NewStream(wxInputStream* stream);
    wxFilterOutputStream* NewStream(wxOutputStream* stream);
    //@}

    /**
        Remove the file extension of @a location if it is one of the file
        extensions handled by this factory.
    */
    wxString PopExtension(const wxString& location);

    /**
        Adds this class factory to the list returned
        by @ref getfirst() GetFirst()/GetNext.
        It is not necessary to do this to use the filter streams. It is usually
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
    @class wxFilterOutputStream
    @wxheader{stream.h}

    A filter stream has the capability of a normal
    stream but it can be placed on top of another stream. So, for example, it
    can compress, encrypt the data which are passed to it and write them to another
    stream.

    @library{wxbase}
    @category{streams}

    @seealso
    wxFilterClassFactory, wxFilterInputStream
*/
class wxFilterOutputStream : public wxOutputStream
{
public:
    //@{
    /**
        Initializes a "filter" stream.
        If the parent stream is passed as a pointer then the new filter stream
        takes ownership of it. If it is passed by reference then it does not.
    */
    wxFilterOutputStream(wxOutputStream& stream);
    wxFilterOutputStream(wxOutputStream* stream);
    //@}
};


/**
    @class wxFilterInputStream
    @wxheader{stream.h}

    A filter stream has the capability of a normal stream but it can be placed on
    top
    of another stream. So, for example, it can uncompress or decrypt the data which
    are read
    from another stream and pass it to the requester.

    @library{wxbase}
    @category{streams}

    @seealso
    wxFilterClassFactory, wxFilterOutputStream
*/
class wxFilterInputStream : public wxInputStream
{
public:
    //@{
    /**
        Initializes a "filter" stream.
        If the parent stream is passed as a pointer then the new filter stream
        takes ownership of it. If it is passed by reference then it does not.
    */
    wxFilterInputStream(wxInputStream& stream);
    wxFilterInputStream(wxInputStream* stream);
    //@}
};


/**
    @class wxBufferedOutputStream
    @wxheader{stream.h}

    This stream acts as a cache. It caches the bytes to be written to the specified
    output stream (See wxFilterOutputStream). The
    data is only written when the cache is full, when the buffered stream is
    destroyed or when calling SeekO().

    This class may not be used without some other stream to write the data
    to (such as a file stream or a memory stream).

    @library{wxbase}
    @category{streams}

    @seealso
    wxStreamBuffer, wxOutputStream
*/
class wxBufferedOutputStream : public wxFilterOutputStream
{
public:
    /**
        Creates a buffered stream using a buffer of a default size of 1024 bytes for
        cashing
        the stream @e parent.
    */
    wxBufferedOutputStream(const wxOutputStream& parent);

    /**
        Destructor. Calls Sync() and destroys the internal buffer.
    */
    ~wxBufferedOutputStream();

    /**
        Calls Sync() and changes the stream position.
    */
    off_t SeekO(off_t pos, wxSeekMode mode);

    /**
        Flushes the buffer and calls Sync() on the parent stream.
    */
    void Sync();
};


/**
    @class wxInputStream
    @wxheader{stream.h}

    wxInputStream is an abstract base class which may not be used directly.

    @library{wxbase}
    @category{streams}
*/
class wxInputStream : public wxStreamBase
{
public:
    /**
        Creates a dummy input stream.
    */
    wxInputStream();

    /**
        Destructor.
    */
    ~wxInputStream();

    /**
        Returns @true if some data is available in the stream right now, so that
        calling Read() wouldn't block.
    */
    bool CanRead();

    /**
        Returns @true after an attempt has been made to read past the end of the
        stream.
    */
    bool Eof();

    /**
        Returns the first character in the input queue and removes it,
        blocking until it appears if necessary.
    */
    char GetC();

    /**
        Returns the last number of bytes read.
    */
    size_t LastRead();

    /**
        Returns the first character in the input queue without removing it.
    */
    char Peek();

    //@{
    /**
        Reads data from the input queue and stores it in the specified output stream.
        The data is read until an error is raised by one of the two streams.
        
        @returns This function returns a reference on the current object, so the
                 user can test any states of the stream right away.
    */
    wxInputStream Read(void* buffer, size_t size);
    Warning Return value
    This function returns a reference on the current object, so the user can test
    any states of the stream right away.
    wxInputStream&  Read(wxOutputStream& stream_out);
    //@}

    /**
        Changes the stream current position.
        
        @param pos
            Offset to seek to.
        @param mode
            One of wxFromStart, wxFromEnd, wxFromCurrent.
        
        @returns The new stream position or wxInvalidOffset on error.
    */
    off_t SeekI(off_t pos, wxSeekMode mode = wxFromStart);

    /**
        Returns the current stream position.
    */
    off_t TellI();

    //@{
    /**
        This function acts like the previous one except that it takes only one
        character: it is sometimes shorter to use than the generic function.
    */
    size_t Ungetch(const char* buffer, size_t size);
    Return value bool Ungetch(char c);
    //@}
};


/**
    @class wxStreamBase
    @wxheader{stream.h}

    This class is the base class of most stream related classes in wxWidgets. It
    must
    not be used directly.

    @library{wxbase}
    @category{streams}

    @seealso
    wxStreamBuffer
*/
class wxStreamBase
{
public:
    /**
        Creates a dummy stream object. It doesn't do anything.
    */
    wxStreamBase();

    /**
        Destructor.
    */
    ~wxStreamBase();

    /**
        This function returns the last error.
        
        @b wxSTREAM_NO_ERROR
        
        No error occurred.
        
        @b wxSTREAM_EOF
        
        An End-Of-File occurred.
        
        @b wxSTREAM_WRITE_ERROR
        
        A generic error occurred on the last write call.
        
        @b wxSTREAM_READ_ERROR
        
        A generic error occurred on the last read call.
    */
    wxStreamError GetLastError();

    /**
        Returns the length of the stream in bytes. If the length cannot be determined
        (this is always the case for socket streams for example), returns
        @c wxInvalidOffset.
        This function is new since wxWidgets version 2.5.4
    */
    wxFileOffset GetLength();

    /**
        GetLength()
        This function returns the size of the stream. For example, for a file it is the
        size of the file.
    */
    size_t GetSize();

    /**
        Returns @true if no error occurred on the stream.
        
        @see GetLastError()
    */
    virtual bool IsOk();

    /**
        Returns @true if the streams supports seeking to arbitrary offsets.
    */
    bool IsSeekable();

    /**
        Internal function. It is called when the stream wants to read data of the
        specified size. It should return the size that was actually read.
    */
    size_t OnSysRead(void* buffer, size_t bufsize);

    /**
        Internal function. It is called when the stream needs to change the
        current position.
    */
    off_t OnSysSeek(off_t pos, wxSeekMode mode);

    /**
        Internal function. Is is called when the stream needs to know the
        real position.
    */
    off_t OnSysTell();

    /**
        See OnSysRead().
    */
    size_t OnSysWrite(const void* buffer, size_t bufsize);
};
