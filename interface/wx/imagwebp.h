/////////////////////////////////////////////////////////////////////////////
// Name:        imagwebp.h
// Purpose:     interface of wxWEBPHandler
// Author:      Hermann Höhne
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxWEBPHandler

    This is the image handler for the WebP format.

    See @ref page_build_libwebp for information about libwebp, required in
    order to use this class.

    @library{wxcore}
    @category{gdi}

    @see wxImage, wxImageHandler, wxInitAllImageHandlers()
*/
class wxWEBPHandler : public wxImageHandler
{
public:
    virtual bool LoadFile(wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1);

    /** @copybrief wxImageHandler::SaveFile
     *
     * By default, WebP operates in lossy mode. You may want to set @c wxIMAGE_OPTION_QUALITY to control the quality. @n
     * 100 means "best quality, but large file size". 0 means "small file size, but worst quality". Default is 90.
     *
     * @copydetails wxImageHandler::SaveFile
     */
    virtual bool SaveFile(wxImage *image, wxOutputStream& stream, bool verbose=true);
};
