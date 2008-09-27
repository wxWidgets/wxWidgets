/////////////////////////////////////////////////////////////////////////////
// Name:        mstream.h
// Purpose:     interface of wxMemoryOutputStream
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMemoryOutputStream


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
    */
    wxMemoryOutputStream(char* data = NULL, size_t length = 0);

    /**
        Destructor.
    */
    virtual ~wxMemoryOutputStream();

    /**
        CopyTo allowed you to transfer data from the internal buffer of
        wxMemoryOutputStream to an external buffer. @a len specifies the size of
        the buffer.
    */
    size_t CopyTo(char* buffer, size_t len) const;

    /**
        Returns the pointer to the stream object used as an internal buffer
        for that stream.
    */
    wxStreamBuffer* GetOutputStreamBuffer() const;
};



/**
    @class wxMemoryInputStream


    @library{wxbase}
    @category{streams}

    @see wxStreamBuffer, wxMemoryOutputStream
*/
class wxMemoryInputStream : public wxInputStream
{
public:
    //@{
    /**
        Creates a new read-only memory stream, initializing it with the
        data from the given input stream @e stream.
        The @a len argument specifies the amount of data to read from
        the @e stream. Setting it to @e wxInvalidOffset means that
        the @a stream is to be read entirely (i.e. till the EOF is reached).
    */
    wxMemoryInputStream(const char* data, size_t len);
    wxMemoryInputStream(const wxMemoryOutputStream& stream);
    wxMemoryInputStream(wxInputStream& stream,
                        wxFileOffset len = wxInvalidOffset);
    //@}

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

