/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/imagwebp.cpp
// Purpose:     wxImage WebP handler
// Author:      Hermann Höhne
// Copyright:   (c) Hermann Höhne
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// based on code by Sylvain Bougnoux, Khral Steelforge and Markus Juergens
// see https://forums.wxwidgets.org/viewtopic.php?t=39212

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_IMAGE && wxUSE_LIBWEBP

#ifndef WX_PRECOMP
#include "wx/log.h"
#endif

#include "wx/imagwebp.h"

#include <webp/demux.h>
#include <webp/decode.h>
#include <webp/encode.h>

//-----------------------------------------------------------------------------
// wxWEBPHandler
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWEBPHandler, wxImageHandler);

#if wxUSE_STREAMS

#include "wx/mstream.h"
#include <memory>
#include <functional>

namespace
{
    typedef std::unique_ptr<WebPDemuxer, std::function<void(WebPDemuxer*)>> WebPDemuxerPtr;
    typedef std::unique_ptr<WebPAnimDecoder, std::function<void(WebPAnimDecoder*)>> WebPAnimDecoderPtr;

    void rgbaToImage(wxImage* image, uint8_t* rgba)
    {
        image->InitAlpha();
        unsigned char* rgb = image->GetData();
        unsigned char* alpha = image->GetAlpha();
        size_t pixel_count = (size_t)image->GetWidth() * (size_t)image->GetHeight();
        size_t rgba_index = 0, rgb_index = 0, alpha_index = 0;
        for (size_t pixel_counter = 0; pixel_counter < pixel_count; pixel_counter++)
        {
            rgb[rgb_index++] = rgba[rgba_index++]; // R
            rgb[rgb_index++] = rgba[rgba_index++]; // G
            rgb[rgb_index++] = rgba[rgba_index++]; // B
            alpha[alpha_index++] = rgba[rgba_index++]; // A
        }
    }

    void copyImageData(wxImage* to, wxImage& from)
    {
        to->Create(from.GetSize(), false);
        size_t pixel_count = (size_t)from.GetWidth() * (size_t)from.GetHeight();
        memcpy(to->GetData(), from.GetData(), pixel_count * 3);
        if (from.HasAlpha())
        {
            to->InitAlpha();
            memcpy(to->GetAlpha(), from.GetAlpha(), pixel_count);
        }
    }

    bool DecodeWebPDataIntoImage(wxImage* image, WebPData* webp_data, bool verbose)
    {
        WebPBitstreamFeatures features;
        VP8StatusCode status = WebPGetFeatures(webp_data->bytes, webp_data->size, &features);
        if (status != VP8_STATUS_OK)
        {
            if (verbose)
            {
                wxLogError("WebP: GetFeatures not OK.");
            }
            return false;
        }
        image->Create(features.width, features.height, false);
        image->SetOption(wxIMAGE_OPTION_WEBP_LOSSLESS, features.format);

        if (features.has_alpha)
        {
            // image has alpha channel. needs to be decoded, then re-ordered.
            uint8_t* rgba = WebPDecodeRGBA(webp_data->bytes, webp_data->size, &features.width, &features.height);
            if (rgba == nullptr)
            {
                if (verbose)
                {
                    wxLogError("WebP: WebPDecodeRGBA failed.");
                }
                return false;
            }
            rgbaToImage(image, rgba);
            WebPFree(rgba);
        }
        else
        {
            // image has no alpha channel. decode into target buffer directly.
            size_t buffer_size = (size_t)image->GetWidth() * (size_t)image->GetHeight() * 3;
            int stride = image->GetWidth() * 3;
            uint8_t* output_buffer = WebPDecodeRGBInto(webp_data->bytes, webp_data->size, image->GetData(), buffer_size, stride);
            if (output_buffer == nullptr)
            {
                if (verbose)
                {
                    wxLogError("WebP: WebPDecodeRGBInto failed.");
                }
                return false;
            }
        }
        return true;
    }

    WebPDemuxerPtr CreateDemuxer(wxInputStream& stream, bool verbose = false)
    {
        wxMemoryOutputStream mos;
        stream.Read(mos);
        wxStreamBuffer* mosb = mos.GetOutputStreamBuffer();
        if (mosb == nullptr)
            return nullptr;

        WebPData webp_data;
        webp_data.bytes = reinterpret_cast<uint8_t*>(mosb->GetBufferStart());
        webp_data.size = mosb->GetBufferSize();
        WebPDemuxerPtr demux
        (
            WebPDemux(&webp_data),
            [](WebPDemuxer* demux)
            {
                // delete the demuxer
                WebPDemuxDelete(demux);
            }
        );
        if (demux == nullptr)
        {
            // creating the demuxer failed, but the buffers still exist
            // and need to be cleaned up
            if (verbose)
            {
                wxLogError("WebP: WebPDemux failed.");
            }
        }
        return demux;
    }
}

bool wxWEBPHandler::LoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index)
{
    bool ok = false;
    image->Destroy();

    bool useFrameDemuxer = (index == -1);

    if (index == -1)
        index = 0;

    if (useFrameDemuxer)
    {
        // Default first frame, Use RGB(A) decoder
        // We can't use this for other frames, because the frame might be sub-frames, smaller than the
        // full image. It's size is known, but the x_offset and y_offset can not be queried, so we can't
        // reconstruct the full-size image.
        WebPDemuxerPtr demux = CreateDemuxer(stream, verbose);
        if (demux != nullptr)
        {
            WebPIterator iter;
            if (WebPDemuxGetFrame(demux.get(), index + 1, &iter))
            {
                ok = DecodeWebPDataIntoImage(image, &iter.fragment, verbose);
                WebPDemuxReleaseIterator(&iter);
            }
        }
    }
    else
    {
        // Use animation decoder, always RGBA
        std::vector<wxWebPAnimationFrame> frames;
        LoadAnimation(frames, stream, verbose);
        if (index < frames.size())
        {
            copyImageData(image, frames.at(index).image);
            ok = true;
        }
    }

    if (!ok && image->IsOk())
        image->Destroy();

    return ok;
}

bool wxWEBPHandler::LoadAnimation(std::vector<wxWebPAnimationFrame>& frames, wxInputStream& stream, bool verbose)
{
    wxMemoryOutputStream mos;
    stream.Read(mos);
    wxStreamBuffer* mosb = mos.GetOutputStreamBuffer();
    WebPData webp_data{};
    webp_data.bytes = reinterpret_cast<uint8_t*>(mosb->GetBufferStart());
    webp_data.size = mosb->GetBufferSize();

    WebPAnimDecoderOptions dec_options;
    WebPAnimDecoderOptionsInit(&dec_options);

    WebPAnimDecoderPtr decoder(
        WebPAnimDecoderNew(&webp_data, &dec_options),
        [](WebPAnimDecoder* dec) { WebPAnimDecoderDelete(dec); }
    );

    if (decoder == nullptr)
    {
        if (verbose)
        {
            wxLogError("WebP: WebPAnimDecoderNew failed.");
        }
        return false;
    }

    WebPAnimInfo anim_info;
    WebPAnimDecoderGetInfo(decoder.get(), &anim_info);
    int prevTimestamp = 0;

    while (WebPAnimDecoderHasMoreFrames(decoder.get()))
    {
        uint8_t* buf;
        int timestamp;
        WebPAnimDecoderGetNext(decoder.get(), &buf, &timestamp);
        if (buf == nullptr)
        {
            if (verbose)
            {
                wxLogError("WebP: WebPAnimDecoderGetNext failed.");
            }
            break;
        }

        wxWebPAnimationFrame frame;
        frame.image.Create(anim_info.canvas_width, anim_info.canvas_height, false);
        frame.bgColour.SetRGBA(anim_info.bgcolor);
        rgbaToImage(&frame.image, buf);
        frame.duration = timestamp - prevTimestamp;
        prevTimestamp = timestamp;
        frames.push_back(std::move(frame));
    }

    WebPAnimDecoderReset(decoder.get());

    return true;
}

bool wxWEBPHandler::SaveFile(wxImage *image, wxOutputStream& stream, bool)
{
    float quality_factor = 90; // if you change this, update the documentation, too
    if (image->HasOption(wxIMAGE_OPTION_WEBP_QUALITY))
        quality_factor = image->GetOptionInt(wxIMAGE_OPTION_WEBP_QUALITY);
    bool lossless = image->GetOptionInt(wxIMAGE_OPTION_WEBP_LOSSLESS) == 2;

    size_t output_size = 0;
    uint8_t * output = nullptr;
    unsigned char * rgb = image->GetData();
    if (image->HasAlpha())
    {
        unsigned char* alpha = image->GetAlpha();
        size_t pixel_count = (size_t)image->GetWidth() * (size_t)image->GetHeight();
        std::vector<unsigned char> rgba(pixel_count * 4);
        size_t rgba_index = 0, rgb_index = 0, alpha_index = 0;
        for (size_t pixel_counter = 0; pixel_counter < pixel_count; pixel_counter++)
        {
            rgba[rgba_index++] = rgb[rgb_index++]; // R
            rgba[rgba_index++] = rgb[rgb_index++]; // G
            rgba[rgba_index++] = rgb[rgb_index++]; // B
            rgba[rgba_index++] = alpha[alpha_index++]; // A
        }
        int stride = image->GetWidth() * 4; // stride is the "width" of a "line" in bytes
        if (lossless)
            output_size = WebPEncodeLosslessRGBA(rgba.data(), image->GetWidth(), image->GetHeight(), stride, &output);
        else
            output_size = WebPEncodeRGBA(rgba.data(), image->GetWidth(), image->GetHeight(), stride, quality_factor, &output);
    }
    else
    {
        int stride = image->GetWidth() * 3; // stride is the "width" of a "line" in bytes
        if (lossless)
            output_size = WebPEncodeLosslessRGB(rgb, image->GetWidth(), image->GetHeight(), stride, &output);
        else
            output_size = WebPEncodeRGB(rgb, image->GetWidth(), image->GetHeight(), stride, quality_factor, &output);
    }

    stream.WriteAll(output, output_size);
    WebPFree(output);
    return (output_size > 0 && stream.LastWrite() == output_size);
}

int wxWEBPHandler::DoGetImageCount(wxInputStream& stream)
{
    int frame_count = -1;
    WebPDemuxerPtr demux = CreateDemuxer(stream);
    if (demux != nullptr)
    {
        frame_count = WebPDemuxGetI(demux.get(), WEBP_FF_FRAME_COUNT);
    }
    return frame_count;
}

bool wxWEBPHandler::DoCanRead(wxInputStream& stream)
{
    // check header according to https://developers.google.com/speed/webp/docs/riff_container
    const std::string riff = "RIFF";
    const std::string webp = "WEBP";
    const int buffer_size = 12;
    char buffer[buffer_size];
    // it's ok to modify the stream position here
    stream.Read(buffer, buffer_size);
    if (stream.LastRead() != buffer_size)
    {
        return false;
    }
    return std::string(buffer, 4) == riff && std::string(&buffer[8], 4) == webp;
}

#endif // wxUSE_STREAMS

/*static*/ wxVersionInfo wxWEBPHandler::GetLibraryVersionInfo()
{
    int webpVersion = WebPGetDecoderVersion();
    return wxVersionInfo("libwebp",
        webpVersion >> 16 & 0xff,
        webpVersion >> 8 & 0xff,
        webpVersion & 0xff);
}

#endif // wxUSE_IMAGE && wxUSE_LIBWEBP
