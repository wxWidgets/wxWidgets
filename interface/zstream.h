/////////////////////////////////////////////////////////////////////////////
// Name:        zstream.h
// Purpose:     documentation for wxZlibOutputStream class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxZlibOutputStream
    @wxheader{zstream.h}

    This stream compresses all data written to it. The compressed output can be
    in zlib or gzip format.
    Note that writing the gzip format requires zlib version 1.2.1 or greater
    (the builtin version does support gzip format).

    The stream is not seekable, wxOutputStream::SeekO returns
     @e wxInvalidOffset.

    @library{wxbase}
    @category{streams}

    @seealso
    wxOutputStream, wxZlibInputStream
*/
class wxZlibOutputStream : public wxFilterOutputStream
{
public:
    //@{
    /**
        Creates a new write-only compressed stream. @a level means level of
        compression. It is number between 0 and 9 (including these values) where
        0 means no compression and 9 best but slowest compression. -1 is default
        value (currently equivalent to 6).
        If the parent stream is passed as a pointer then the new filter stream
        takes ownership of it. If it is passed by reference then it does not.
        The @a flags wxZLIB_ZLIB and wxZLIB_GZIP specify whether the output data
        will be in zlib or gzip format. wxZLIB_ZLIB is the default.
        If @a flags is wxZLIB_NO_HEADER, then a raw deflate stream is output
        without either zlib or gzip headers. This is a lower level
        mode, which is not usually used directly. It can be used to embed a raw
        deflate stream in a higher level protocol.
        The following symbols can be use for the compression level and flags:
    */
    wxZlibOutputStream(wxOutputStream& stream, int level = -1,
                       int flags = wxZLIB_ZLIB);
    wxZlibOutputStream(wxOutputStream* stream, int level = -1,
                       int flags = wxZLIB_ZLIB);
    //@}

    /**
        Returns @true if zlib library in use can handle gzip compressed data.
    */
    static bool CanHandleGZip();
};


/**
    @class wxZlibInputStream
    @wxheader{zstream.h}

    This filter stream decompresses a stream that is in zlib or gzip format.
    Note that reading the gzip format requires zlib version 1.2.1 or greater,
    (the builtin version does support gzip format).

    The stream is not seekable, wxInputStream::SeekI returns
     @e wxInvalidOffset. Also wxStreamBase::GetSize is
    not supported, it always returns 0.

    @library{wxbase}
    @category{streams}

    @seealso
    wxInputStream, wxZlibOutputStream.
*/
class wxZlibInputStream : public wxFilterInputStream
{
public:
    //@{
    /**
        If the parent stream is passed as a pointer then the new filter stream
        takes ownership of it. If it is passed by reference then it does not.
        The @a flags wxZLIB_ZLIB and wxZLIB_GZIP specify whether the input data
        is in zlib or gzip format. If wxZLIB_AUTO is used, then zlib will
        autodetect the stream type, this is the default.
        If @a flags is wxZLIB_NO_HEADER, then the data is assumed to be a raw
        deflate stream without either zlib or gzip headers. This is a lower level
        mode, which is not usually used directly. It can be used to read a raw
        deflate stream embedded in a higher level protocol.
        This version is not by default compatible with the output produced by
        the version of @e wxZlibOutputStream in wxWidgets 2.4.x. However,
        there is a compatibility mode, which is switched on by passing
        wxZLIB_24COMPATIBLE for flags. Note that in when operating in compatibility
        mode error checking is very much reduced.
        The following symbols can be use for the flags:
    */
    wxZlibInputStream(wxInputStream& stream, int flags = wxZLIB_AUTO);
    wxZlibInputStream(wxInputStream* stream,
                      int flags = wxZLIB_AUTO);
    //@}

    /**
        Returns @true if zlib library in use can handle gzip compressed data.
    */
    static bool CanHandleGZip();
};
