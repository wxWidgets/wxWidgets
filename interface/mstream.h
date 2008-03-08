/////////////////////////////////////////////////////////////////////////////
// Name:        mstream.h
// Purpose:     documentation for wxMemoryOutputStream class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMemoryOutputStream
    @wxheader{mstream.h}


    @library{wxbase}
    @category{streams}

    @seealso
    wxStreamBuffer
*/
class wxMemoryOutputStream : public wxOutputStream
{
public:
    /**
        If @e data is @NULL, then it will initialize a new empty buffer which will
        grow if required.
    */
    wxMemoryOutputStream(char * data = @NULL, size_t length = 0);

    /**
        Destructor.
    */
    ~wxMemoryOutputStream();

    /**
        CopyTo allowed you to transfer data from the internal buffer of
        wxMemoryOutputStream to an external buffer. @e len specifies the size of
        the buffer.
    */
    size_t CopyTo(char * buffer, size_t len);

    /**
        Returns the pointer to the stream object used as an internal buffer
        for that stream.
    */
    wxStreamBuffer * GetOutputStreamBuffer();
};


/**
    @class wxMemoryInputStream
    @wxheader{mstream.h}


    @library{wxbase}
    @category{streams}

    @seealso
    wxStreamBuffer, wxMemoryOutputStream
*/
class wxMemoryInputStream : public wxInputStream
{
public:
    //@{
    /**
        Creates a new read-only memory stream, initializing it with the
        data from the given input stream @e stream.
        
        The @e len argument specifies the amount of data to read from
        the @e stream. Setting it to @e wxInvalidOffset means that
        the @e stream is to be read entirely (i.e. till the EOF is reached).
    */
    wxMemoryInputStream(const char * data, size_t len);
    wxMemoryInputStream(const wxMemoryOutputStream& stream);
    wxMemoryInputStream(wxInputStream& stream,
                        wxFileOffset len = wxInvalidOffset);
    //@}

    /**
        Destructor.
    */
    ~wxMemoryInputStream();

    /**
        Returns the pointer to the stream object used as an internal buffer
        for that stream.
    */
    wxStreamBuffer * GetInputStreamBuffer();
};
