/////////////////////////////////////////////////////////////////////////////
// Name:        mstream.h
// Purpose:     interface of wxMemoryOutputStream
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMemoryOutputStream

    @todo describe me.

    @library{wxbase}
    @category{streams}

    @see wxStreamBuffer
*/
class wxMemoryOutputStream : public wxOutputStream
{
public:
    /**
        If @a data is @NULL, then it will initialize a new empty buffer which will
        grow if required.

        @warning
        If the buffer is created, it will be destroyed at the destruction of the stream.
    */
    wxMemoryOutputStream(void* data = NULL, size_t length = 0);

    /**
        Destructor.
    */
    virtual ~wxMemoryOutputStream();

    /**
        Allows you to transfer data from the internal buffer of wxMemoryOutputStream
        to an external buffer. @a len specifies the size of the buffer.
    */
    size_t CopyTo(void* buffer, size_t len) const;

    /**
        Returns the pointer to the stream object used as an internal buffer
        for that stream.
    */
    wxStreamBuffer* GetOutputStreamBuffer() const;
};



/**
    @class wxMemoryInputStream

    @todo describe me.

    @library{wxbase}
    @category{streams}

    @see wxStreamBuffer, wxMemoryOutputStream
*/
class wxMemoryInputStream : public wxInputStream
{
public:
    /**
        Initializes a new read-only memory stream which will use the specified
        buffer data of length len. The stream does not take ownership of the buffer,
        i.e. the buffer will not be deleted in its destructor.
    */
    wxMemoryInputStream(const void* data, size_t len);

    /**
        Creates a new read-only memory stream, initializing it with the data from
        the given output stream @a stream.
    */
    wxMemoryInputStream(const wxMemoryOutputStream& stream);

    /**
        Creates a new read-only memory stream, initializing it with the
        data from the given input stream @a stream.

        The @a len argument specifies the amount of data to read from the
        @a stream. Setting it to @e wxInvalidOffset means that the @a stream
        is to be read entirely (i.e. till the EOF is reached).
    */
    wxMemoryInputStream(wxInputStream& stream,
                        wxFileOffset len = wxInvalidOffset);

    /**
        Destructor.
    */
    virtual ~wxMemoryInputStream();

    /**
        Returns the pointer to the stream object used as an internal buffer
        for that stream.
    */
    wxStreamBuffer* GetInputStreamBuffer() const;
};

