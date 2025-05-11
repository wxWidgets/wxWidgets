/////////////////////////////////////////////////////////////////////////////
// Name:        imagwebp.h
// Purpose:     interface of wxWEBPHandler
// Author:      Hermann Höhne
// Created:     2024-03-08
// Copyright:   (c) Hermann Höhne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#define wxIMAGE_OPTION_WEBP_QUALITY wxString("WebPQuality")
#define wxIMAGE_OPTION_WEBP_FORMAT  wxString("WebPLossless")

/**
    Format of the WebP image.

    These are the same values as the WebPBitstreamFeatures format from libwebp
    (see https://developers.google.com/speed/webp/docs/api#simple_decoding_api).

    @since 3.3.0
*/
enum wxWebPImageFormat
{
    Undefined = 0,
    Lossy = 1,
    Lossless = 2

};

/**
    Simple struct containing WebP animated frame information.

    @since 3.3.0
*/
struct wxWebPAnimationFrame
{
    /// Image of the frame.
    wxImage image;

    /// Background colour of the frame.
    wxColour bgColour;

    /// Duration of the frame.
    int duration = 0;
};

/**
    @class wxWEBPHandler

    This is the image handler for the WebP format.

    See @ref page_build_libwebp for information about libwebp, required in
    order to use this class.

    @library{wxcore}
    @category{gdi}

    @since 3.3.0

    @see wxImage, wxImageHandler, wxInitAllImageHandlers()
*/
class wxWEBPHandler : public wxImageHandler
{
public:
    /**
    Default constructor for wxWEBPHandler
    */
    wxWEBPHandler();

    /**
        Retrieve the version information about the WebP library used by this
        handler.
    */
    static wxVersionInfo GetLibraryVersionInfo();

    virtual bool LoadFile(wxImage *image, wxInputStream& stream,
                          bool verbose = true, int index = -1);
    virtual bool SaveFile(wxImage *image, wxOutputStream& stream,
                          bool verbose = true);

    /**
        Load an animated WebP image.

        @param frames
            Vector that will be filled with all wxWebPAnimationFrame of the
            image.
        @param stream
            Opened input stream for reading the data.
        @param verbose
            If set to @true, errors reported by the image handler will produce
            wxLogMessages

        @return @true if the operation succeeded, @false otherwise.

    */
    virtual bool LoadAnimation(std::vector<wxWebPAnimationFrame>& frames,
                               wxInputStream& stream, bool verbose = true);

protected:
    virtual bool DoCanRead(wxInputStream& stream);
    virtual int DoGetImageCount(wxInputStream& stream);
};
