/////////////////////////////////////////////////////////////////////////////
// Name:        imagpng.h
// Purpose:     interface of wxPNGHandler
// Author:      Samuel Dunn
// Licence:     wxWindows licence
////////////////////////////////////////////////////////////////////////////

/**
Key used for setting an image option that, upon saving to a PNG file, will be
converted to an iTXt chunk with the key: "Description", and stored in the file.

Uncompressed tEXt and iTXt chunks with the key: "Description" are automatically
retrieved upon loading of a PNG file, and stored in this image option. If
multiple chunks with this key are present, only the first is retrieved.

@since 3.3.1
*/
#define wxIMAGE_OPTION_PNG_DESCRIPTION              wxT("PngDescription")

#define wxIMAGE_OPTION_PNG_FORMAT                   wxT("PngFormat")
#define wxIMAGE_OPTION_PNG_BITDEPTH                 wxT("PngBitDepth")
#define wxIMAGE_OPTION_PNG_FILTER                   wxT("PngF")
#define wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL        wxT("PngZL")
#define wxIMAGE_OPTION_PNG_COMPRESSION_MEM_LEVEL    wxT("PngZM")
#define wxIMAGE_OPTION_PNG_COMPRESSION_STRATEGY     wxT("PngZS")
#define wxIMAGE_OPTION_PNG_COMPRESSION_BUFFER_SIZE  wxT("PngZB")

/* These are already in interface/wx/image.h
    They were likely put there as a stopgap, but they've been there long enough
    that that location trumps moving them here.
enum
{
    wxPNG_TYPE_COLOUR = 0,
    wxPNG_TYPE_GREY = 2,
    wxPNG_TYPE_GREY_RED = 3,
    wxPNG_TYPE_PALETTE = 4
};
*/

/**
    @class wxPNGHandler

    This is the image handler for the PNG format.

    @library{wxcore}
    @category{gdi}

    @see wxImage, wxImageHandler, wxInitAllImageHandlers()
*/
class wxPNGHandler : public wxImageHandler
{
public:
    /**
    Default constructor for wxPNGHandler
    */
    wxPNGHandler();

    // let parent class's documentation through.
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );

protected:
    virtual bool DoCanRead( wxInputStream& stream );
};
