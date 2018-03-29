///////////////////////////////////////////////////////////////////////////////
// Name:        wx/lzmastream.h
// Purpose:     LZMA [de]compression classes documentation
// Author:      Vadim Zeitlin
// Created:     2018-03-29
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxLZMAInputStream

    This filter stream decompresses data in XZ format.

    XZ format uses LZMA2 algorithm for compression used for .xz files and is
    similar to GZip or BZip2 format. Notice that it is different from, and
    incompatible with, 7z archive format even although it uses the same
    compression algorithm.

    To decompress contents of standard input to standard output, the following
    (not optimally efficient) code could be used:
    @code
    wxFFileInputStream fin(stdin);
    wxLZMAInputStream zin(fin);
    wxFFileOutputStream fout(stdout);
    zin.Read(fout);

    if ( zin.GetLastError() != wxSTREAM_EOF ) {
        ... handle error ...
    }
    @endcode

    @library{wxbase}
    @category{archive,streams}

    @see wxInputStream, wxZlibInputStream

    @since 3.1.2
*/
class wxLZMAInputStream : public wxFilterInputStream
{
public:
    /**
        Create decompressing stream associated with the given underlying
        stream.

        This overload does not take ownership of the @a stream.
    */
    wxLZMAInputStream(wxInputStream& stream);

    /**
        Create decompressing stream associated with the given underlying
        stream and takes ownership of it.

        As with the base wxFilterInputStream class, passing @a stream by
        pointer indicates that this object takes ownership of it and will
        delete it when it is itself destroyed.
     */
    wxLZMAInputStream(wxInputStream* stream);
};
