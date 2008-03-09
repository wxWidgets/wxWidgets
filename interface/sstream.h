/////////////////////////////////////////////////////////////////////////////
// Name:        sstream.h
// Purpose:     documentation for wxStringInputStream class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxStringInputStream
    @wxheader{sstream.h}

    This class implements an input stream which reads data from a string. It
    supports seeking.

    @library{wxbase}
    @category{streams}
*/
class wxStringInputStream : public wxInputStream
{
public:
    /**
        Creates a new read-only stream using the specified string. Note that the string
        is copied by the stream so if the original string is modified after using this
        constructor, changes to it are not reflected when reading from stream.
    */
    wxStringInputStream(const wxString& s);
};


/**
    @class wxStringOutputStream
    @wxheader{sstream.h}

    This class implements an output stream which writes data either to a
    user-provided or internally allocated string. Note that currently this stream
    does not support seeking but can tell its current position.

    @library{wxbase}
    @category{streams}
*/
class wxStringOutputStream : public wxOutputStream
{
public:
    /**
        If the provided pointer is non-@NULL, data will be written to it.
        Otherwise, an internal string is used for the data written to this stream, use
        GetString() to get access to it.
        If @a str is used, data written to the stream is appended to the current
        contents of it, i.e. the string is not cleared here. However if it is not
        empty, the positions returned by wxOutputStream::TellO will be
        offset by the initial string length, i.e. initial stream position will be the
        initial length of the string and not 0.
    */
    wxStringOutputStream(wxString str = NULL);

    /**
        Returns the string containing all the data written to the stream so far.
    */
    const wxString GetString() const;
};
