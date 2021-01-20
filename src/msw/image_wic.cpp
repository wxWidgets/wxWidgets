/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/image_wic.cpp
// Purpose:     Implementation of wxImageHandler using
//              Windows Imaging Component
// Author:      Tobias Taschner
// Created:     2021-01-15
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/msw/image_wic.h"

#if wxUSE_IMAGE && wxUSE_WIC

// For MSVC we can link in the required library explicitly, for the other
// compilers (e.g. MinGW) this needs to be done at makefiles level.
#ifdef __VISUALC__
#pragma comment(lib, "windowscodecs")
#endif

#include "wx/msw/private/comptr.h"
#include "wx/msw/private/comstream.h"

#include <wincodec.h>

// ----------------------------------------------------------------------------
// local functions
// ----------------------------------------------------------------------------

#define wxCOM_CHECK(name, params) hr = name params; \
    if (FAILED(hr)) \
    {\
        wxLogApiError(wxS("Windows Imaging Component: "#name), hr); \
        return false; \
    }   

static
bool wxWICCopyPixels(IWICBitmapSource* bmpSource, wxMemoryBuffer& buffer,
    wxUint32 width, wxUint32 height, wxUint32 bitsPerPixel)
{
    HRESULT hr;

    size_t memSize = width * height * (bitsPerPixel / 8);
    wxCOM_CHECK(bmpSource->CopyPixels,
        (NULL,
            width * (bitsPerPixel / 8), memSize,
            (BYTE*)buffer.GetWriteBuf(memSize)
            )
    );
    buffer.SetDataLen(memSize);

    return true;
}

static
bool wxWICConvertAndCopyPixels(IWICBitmapSource* bmpSource, REFWICPixelFormatGUID pixelFormat,
    wxMemoryBuffer& buffer, wxUint32 width, wxUint32 height, wxUint32 bitsPerPixel)
{
    HRESULT hr;
    wxCOMPtr<IWICBitmapSource> dstSource;
    wxCOM_CHECK(WICConvertBitmapSource, (pixelFormat, bmpSource, &dstSource));
    return wxWICCopyPixels(dstSource, buffer, width, height, bitsPerPixel);
}

// copy palette
static bool wxWICCreateImagePalette(IWICPalette* palette, wxImage* image)
{
    HRESULT hr;
    UINT colorCount;
    wxCOM_CHECK(palette->GetColorCount, (&colorCount));
    if (!colorCount)
        return false;

    BOOL hasAlpha;
    wxCOM_CHECK(palette->HasAlpha, (&hasAlpha));

    WICColor* colors = new WICColor[colorCount];
    UINT actualColorCount;
    wxCOM_CHECK(palette->GetColors, (colorCount, colors, &actualColorCount));

    unsigned char* r = new unsigned char[colorCount];
    unsigned char* g = new unsigned char[colorCount];
    unsigned char* b = new unsigned char[colorCount];

    for (UINT j = 0; j < colorCount; j++)
    {
        r[j] = colors[j] & 0xff;
        g[j] = (colors[j] >> 8) & 0xff;
        b[j] = (colors[j] >> 16) & 0xff;
    }

    image->SetPalette(wxPalette(colorCount, r, g, b));
    delete[] r;
    delete[] g;
    delete[] b;

    delete[] colors;

    return true;
}

// ============================================================================
// wxImageHandlerWIC implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxImageHandlerWIC, wxImageHandler);

wxImageHandlerWIC::wxImageHandlerWIC(wxBitmapType bitmapType)
{
    m_type = bitmapType;

    switch (m_type)
    {
    case wxBITMAP_TYPE_PNG:
        m_name = wxT("PNG file");
        m_extension = wxT("png");
        m_mime = wxT("image/png");
        m_containerFormat = &GUID_ContainerFormatPng;
        break;
    case wxBITMAP_TYPE_JPEG:
        m_name = wxT("JPEG file");
        m_extension = wxT("jpg");
        m_altExtensions.Add(wxT("jpeg"));
        m_altExtensions.Add(wxT("jpe"));
        m_mime = wxT("image/jpeg");
        m_containerFormat = &GUID_ContainerFormatJpeg;
        break;
    case wxBITMAP_TYPE_TIFF:
        m_name = wxT("TIFF file");
        m_extension = wxT("tif");
        m_altExtensions.Add(wxT("tiff"));
        m_mime = wxT("image/tiff");
        m_containerFormat = &GUID_ContainerFormatTiff;
        break;
    default:
        wxASSERT_MSG(FALSE, "Unsupported bitmap type");
        break;
    }
}

void wxImageHandlerWIC::InitAllSupportedHandlers()
{
    wxImage::AddHandler(new wxImageHandlerWIC(wxBITMAP_TYPE_PNG));
    wxImage::AddHandler(new wxImageHandlerWIC(wxBITMAP_TYPE_JPEG));
    wxImage::AddHandler(new wxImageHandlerWIC(wxBITMAP_TYPE_TIFF));
}

bool wxImageHandlerWIC::LoadFile(wxImage *image, wxInputStream& stream,
    bool WXUNUSED(verbose), int index)
{
    UINT frameIndex = index < 0 ? 0 : index;

    HRESULT hr;

    // Create the COM imaging factory
    wxCOMPtr<IWICImagingFactory> imgFactory;
    wxCOM_CHECK(::CoCreateInstance, (
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        wxIID_PPV_ARGS(IWICImagingFactory, &imgFactory)
    ));

    // Create decoder
    wxCOMPtr<IWICBitmapDecoder> decoder;
    wxCOM_CHECK(imgFactory->CreateDecoder,
        (*m_containerFormat, NULL, &decoder));
    wxCOM_CHECK(decoder->Initialize, (new wxCOMInputStreamAdapter(&stream),
        WICDecodeMetadataCacheOnDemand));

    // Get frame bitmap
    UINT frameCount = 0;
    wxCOM_CHECK(decoder->GetFrameCount, (&frameCount));
    if (frameCount < frameIndex)
        return false;
    wxCOMPtr<IWICBitmapFrameDecode> frameBitmap;
    wxCOM_CHECK(decoder->GetFrame, (frameIndex, &frameBitmap));

    // Get bitmap information
    UINT imgWidth, imgHeight;
    wxCOM_CHECK(frameBitmap->GetSize, (&imgWidth, &imgHeight));
    double resX, resY;
    wxCOM_CHECK(frameBitmap->GetResolution, (&resX, &resY));

    // Get format information
    WICPixelFormatGUID formatGUID;
    wxCOM_CHECK(frameBitmap->GetPixelFormat, (&formatGUID));
    wxCOMPtr<IWICComponentInfo> compInfo;
    wxCOM_CHECK(imgFactory->CreateComponentInfo, (formatGUID, &compInfo));
    wxCOMPtr<IWICPixelFormatInfo> formatInfo;
    wxCOM_CHECK(compInfo->QueryInterface,
        (wxIID_PPV_ARGS(IWICPixelFormatInfo, &formatInfo)));
    UINT bitsPerPixel = 0;
    wxCOM_CHECK(formatInfo->GetBitsPerPixel, (&bitsPerPixel));

    // Set metadata
    image->SetOption(wxIMAGE_OPTION_RESOLUTIONX,
        wxString::FromCDouble(resX * imgWidth, 2));
    image->SetOption(wxIMAGE_OPTION_RESOLUTIONY,
        wxString::FromCDouble(resY * imgHeight, 2));
    image->SetOption(wxIMAGE_OPTION_RESOLUTIONUNIT, wxIMAGE_RESOLUTION_INCHES);

    // Extract and or convert pixel data
    wxCOMPtr<IWICBitmapSource> bmpSrc;
    if (IsEqualGUID(formatGUID, GUID_WICPixelFormat32bppCMYK))
    {
        // CMYK images require this copy to another bitmap in order to the data
        wxCOMPtr<IWICBitmap> bmp;
        wxCOM_CHECK(imgFactory->CreateBitmapFromSource,
            (frameBitmap, WICBitmapCacheOnDemand, &bmp));
        bmp->QueryInterface(wxIID_PPV_ARGS(IWICBitmapSource, &bmpSrc));
    }
    else
        bmpSrc = frameBitmap;

    wxMemoryBuffer imgData;
    wxMemoryBuffer alphaData;
    if (!wxWICConvertAndCopyPixels(bmpSrc, GUID_WICPixelFormat24bppRGB,
        imgData, imgWidth, imgHeight, 24))
        return false;

    if (!wxWICConvertAndCopyPixels(bmpSrc, GUID_WICPixelFormat8bppAlpha,
        alphaData, imgWidth, imgHeight, 8))
        alphaData.Clear();

    // Init wxImage from pixel data
    bool success = image->Create(imgWidth, imgHeight,
        (unsigned char*)imgData.release(), false);
    if (success && !alphaData.IsEmpty())
        image->SetAlpha((unsigned char*)alphaData.release());

    if (success)
    {
        // Copy palette if available
        wxCOMPtr<IWICPalette> palette;
        wxCOM_CHECK(imgFactory->CreatePalette, (&palette));
        hr = frameBitmap->CopyPalette(palette);
        if (SUCCEEDED(hr))
            wxWICCreateImagePalette(palette, image);
    }

    return success;
}

bool wxImageHandlerWIC::SaveFile(wxImage *image, wxOutputStream& stream,
    bool WXUNUSED(verbose))
{
    HRESULT hr;

    // Create the COM imaging factory
    wxCOMPtr<IWICImagingFactory> imgFactory;
    wxCOM_CHECK(::CoCreateInstance, (
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        wxIID_PPV_ARGS(IWICImagingFactory, &imgFactory)
        ));

    // Create and initalize encoder
    wxCOMPtr<IWICBitmapEncoder> encoder;
    wxCOM_CHECK(imgFactory->CreateEncoder,
        (*m_containerFormat, NULL, &encoder));
    wxCOM_CHECK(encoder->Initialize,
        (new wxCOMOutputStreamAdapter(&stream), WICBitmapEncoderNoCache));

    // Create frame
    wxCOMPtr<IPropertyBag2> encodeOptions;
    wxCOMPtr<IWICBitmapFrameEncode> frameEncode;
    wxCOM_CHECK(encoder->CreateNewFrame, (&frameEncode, &encodeOptions));

    // Create bitmap from data
    wxCOMPtr<IWICBitmap> srcBitmap;
    int bitsPerPixel = 24;
    size_t memSize = image->GetWidth() * image->GetHeight() *
        (bitsPerPixel / 8);
    wxCOM_CHECK(imgFactory->CreateBitmapFromMemory, (
        image->GetWidth(), image->GetHeight(),
        GUID_WICPixelFormat24bppRGB,
        image->GetWidth() * (bitsPerPixel / 8), memSize, image->GetData(),
        &srcBitmap));

    // Write bitmap to frame
    wxCOM_CHECK(frameEncode->Initialize, (encodeOptions));
    wxCOM_CHECK(frameEncode->WriteSource, (srcBitmap, NULL));

    // Commit data to file
    wxCOM_CHECK(frameEncode->Commit, ());
    wxCOM_CHECK(encoder->Commit, ());

    return true;
}

bool wxImageHandlerWIC::DoCanRead(wxInputStream& stream)
{
    unsigned char hdr[4];

    if (!stream.Read(hdr, WXSIZEOF(hdr)))     // it's ok to modify the stream position here
        return false;

    switch (m_type)
    {
        case wxBITMAP_TYPE_PNG:
            return memcmp(hdr, "\211PNG", WXSIZEOF(hdr)) == 0;
        case wxBITMAP_TYPE_JPEG:
            return hdr[0] == 0xFF && hdr[1] == 0xD8;
        case wxBITMAP_TYPE_TIFF:
            return (hdr[0] == 'I' && hdr[1] == 'I') ||
                (hdr[0] == 'M' && hdr[1] == 'M');
        default:
            return false;
    }
}

#endif // wxUSE_WIC
