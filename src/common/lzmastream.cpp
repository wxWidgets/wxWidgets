///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/lzmastream.cpp
// Purpose:     Implementation of LZMA stream classes
// Author:      Vadim Zeitlin
// Created:     2018-03-29
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_LIBLZMA && wxUSE_STREAMS

#include "wx/lzmastream.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/translation.h"
#endif // WX_PRECOMP

#include <lzma.h>

namespace wxPrivate
{

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------

const size_t wxLZMA_BUF_SIZE = 4096;

// ----------------------------------------------------------------------------
// Private helpers
// ----------------------------------------------------------------------------

// Simpler wrapper around lzma_stream, taking care of initializing and
// finalizing it.
struct wxLZMAStream : lzma_stream
{
    wxLZMAStream()
    {
        memset(this, 0, sizeof(lzma_stream));
    }

    ~wxLZMAStream()
    {
        lzma_end(this);
    }
};

} // namespace wxPrivate

using namespace wxPrivate;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

wxVersionInfo wxGetLibLZMAVersionInfo()
{
    const uint32_t ver = lzma_version_number();

    // For now ignore the "stability" part of the version.
    return wxVersionInfo
           (
            "liblzma",
            ver / 10000000,
            (ver % 10000000) / 10000,
            (ver % 10000) / 10
           );
}

// ----------------------------------------------------------------------------
// wxLZMAData: common helpers for compression and decompression
// ----------------------------------------------------------------------------

wxLZMAData::wxLZMAData()
{
    m_stream = new wxLZMAStream;
    m_streamBuf = new wxUint8[wxLZMA_BUF_SIZE];
    m_pos = 0;
}

wxLZMAData::~wxLZMAData()
{
    delete [] m_streamBuf;
    delete m_stream;
}

// ----------------------------------------------------------------------------
// wxLZMAInputStream: decompression
// ----------------------------------------------------------------------------

void wxLZMAInputStream::Init()
{
    // We don't specify any memory usage limit nor any flags, not even
    // LZMA_CONCATENATED recommended by liblzma documentation, because we don't
    // foresee the need to support concatenated compressed files for now.
    const lzma_ret rc = lzma_stream_decoder(m_stream, UINT64_MAX, 0);
    switch ( rc )
    {
        case LZMA_OK:
            // Skip setting m_lasterror below.
            return;

        case LZMA_MEM_ERROR:
            wxLogError(_("Failed to allocate memory for LZMA decompression."));
            break;

        default:
            wxLogError(_("Failed to initialize LZMA decompression: "
                         "unexpected error %u."),
                       rc);
            break;
    }

    m_lasterror = wxSTREAM_READ_ERROR;
}

size_t wxLZMAInputStream::OnSysRead(void* outbuf, size_t size)
{
    m_stream->next_out = static_cast<uint8_t*>(outbuf);
    m_stream->avail_out = size;

    // Decompress input as long as we don't have any errors (including EOF, as
    // it doesn't make sense to continue after it either) and have space to
    // decompress it to.
    while ( m_lasterror == wxSTREAM_NO_ERROR && m_stream->avail_out > 0 )
    {
        // Get more input data if needed.
        if ( !m_stream->avail_in )
        {
            m_parent_i_stream->Read(m_streamBuf, wxLZMA_BUF_SIZE);
            m_stream->next_in = m_streamBuf;
            m_stream->avail_in = m_parent_i_stream->LastRead();

            if ( !m_stream->avail_in )
            {
                if ( m_parent_i_stream->GetLastError() == wxSTREAM_EOF )
                {
                    // We have reached end of the underlying stream.
                    m_lasterror = wxSTREAM_EOF;
                    break;
                }

                m_lasterror = wxSTREAM_READ_ERROR;
                return 0;
            }
        }

        // Do decompress.
        const lzma_ret rc = lzma_code(m_stream, LZMA_RUN);

        wxString err;
        switch ( rc )
        {
            case LZMA_OK:
                continue;

            case LZMA_STREAM_END:
                m_lasterror = wxSTREAM_EOF;
                continue;

            case LZMA_FORMAT_ERROR:
                err = wxTRANSLATE("input is not in XZ format");
                break;

            case LZMA_OPTIONS_ERROR:
                err = wxTRANSLATE("input compressed using unknown XZ option");
                break;

            case LZMA_DATA_ERROR:
            case LZMA_BUF_ERROR:
                err = wxTRANSLATE("input is corrupted");
                break;

            default:
                err = wxTRANSLATE("unknown decompression error");
                break;
        }

        wxLogError(_("LZMA decompression error: %s"), wxGetTranslation(err));

        m_lasterror = wxSTREAM_READ_ERROR;
        return 0;
    }

    // Return the number of bytes actually read, this may be less than the
    // requested size if we hit EOF.
    size -= m_stream->avail_out;
    m_pos += size;
    return size;
}

// ----------------------------------------------------------------------------
// wxLZMAOutputStream: compression
// ----------------------------------------------------------------------------

void wxLZMAOutputStream::Init(int level)
{
    if ( level == -1 )
        level = LZMA_PRESET_DEFAULT;

    // Use the check type recommended by liblzma documentation.
    const lzma_ret rc = lzma_easy_encoder(m_stream, level, LZMA_CHECK_CRC64);
    switch ( rc )
    {
        case LZMA_OK:
            // Prepare for the first call to OnSysWrite().
            m_stream->next_out = m_streamBuf;
            m_stream->avail_out = wxLZMA_BUF_SIZE;

            // Skip setting m_lasterror below.
            return;

        case LZMA_MEM_ERROR:
            wxLogError(_("Failed to allocate memory for LZMA compression."));
            break;

        default:
            wxLogError(_("Failed to initialize LZMA compression: "
                         "unexpected error %u."),
                       rc);
            break;
    }

    m_lasterror = wxSTREAM_WRITE_ERROR;
}

size_t wxLZMAOutputStream::OnSysWrite(const void *inbuf, size_t size)
{
    m_stream->next_in = static_cast<const uint8_t*>(inbuf);
    m_stream->avail_in = size;

    // Compress as long as we have any input data, but stop at first error as
    // it's useless to try to continue after it (or even starting if the stream
    // had already been in an error state).
    while ( m_lasterror == wxSTREAM_NO_ERROR && m_stream->avail_in > 0 )
    {
        // Flush the output buffer if necessary.
        if ( !UpdateOutputIfNecessary() )
            return 0;

        const lzma_ret rc = lzma_code(m_stream, LZMA_RUN);

        wxString err;
        switch ( rc )
        {
            case LZMA_OK:
                continue;

            case LZMA_MEM_ERROR:
                err = wxTRANSLATE("out of memory");
                break;

            case LZMA_STREAM_END:
                // This is unexpected as we don't use LZMA_FINISH here.
                wxFAIL_MSG( "Unexpected LZMA stream end" );
                wxFALLTHROUGH;

            default:
                err = wxTRANSLATE("unknown compression error");
                break;
        }

        wxLogError(_("LZMA compression error: %s"), wxGetTranslation(err));

        m_lasterror = wxSTREAM_WRITE_ERROR;
        return 0;
    }

    m_pos += size;
    return size;
}

bool wxLZMAOutputStream::UpdateOutput()
{
    // Write the buffer contents to the real output, taking care only to write
    // as much of it as we actually have, as the buffer can (and very likely
    // will) be incomplete.
    const size_t numOut = wxLZMA_BUF_SIZE - m_stream->avail_out;
    m_parent_o_stream->Write(m_streamBuf, numOut);
    if ( m_parent_o_stream->LastWrite() != numOut )
    {
        m_lasterror = wxSTREAM_WRITE_ERROR;
        return false;
    }

    return true;
}

bool wxLZMAOutputStream::UpdateOutputIfNecessary()
{
    if ( !m_stream->avail_out )
    {
        if ( !UpdateOutput() )
            return false;

        m_stream->next_out = m_streamBuf;
        m_stream->avail_out = wxLZMA_BUF_SIZE;
    }

    return true;
}

bool wxLZMAOutputStream::DoFlush(bool finish)
{
    const lzma_action action = finish ? LZMA_FINISH : LZMA_FULL_FLUSH;

    while ( m_lasterror == wxSTREAM_NO_ERROR )
    {
        if ( !UpdateOutputIfNecessary() )
            break;

        const lzma_ret rc = lzma_code(m_stream, action);

        wxString err;
        switch ( rc )
        {
            case LZMA_OK:
                continue;

            case LZMA_STREAM_END:
                // Don't forget to output the last part of the data.
                return UpdateOutput();

            case LZMA_MEM_ERROR:
                err = wxTRANSLATE("out of memory");
                break;

            default:
                err = wxTRANSLATE("unknown compression error");
                break;
        }

        wxLogError(_("LZMA compression error when flushing output: %s"),
                   wxGetTranslation(err));

        m_lasterror = wxSTREAM_WRITE_ERROR;
    }

    return false;
}

bool wxLZMAOutputStream::Close()
{
    if ( !DoFlush(true) )
        return false;

    m_stream->next_out = m_streamBuf;
    m_stream->avail_out = wxLZMA_BUF_SIZE;

    return wxFilterOutputStream::Close() && IsOk();
}

// ----------------------------------------------------------------------------
// wxLZMAClassFactory: allow creating streams from extension/MIME type
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxLZMAClassFactory, wxFilterClassFactory);

static wxLZMAClassFactory g_wxLZMAClassFactory;

wxLZMAClassFactory::wxLZMAClassFactory()
{
    if ( this == &g_wxLZMAClassFactory )
        PushFront();
}

const wxChar * const *
wxLZMAClassFactory::GetProtocols(wxStreamProtocolType type) const
{
    static const wxChar *mime[] = { wxT("application/xz"), NULL };
    static const wxChar *encs[] = { wxT("xz"), NULL };
    static const wxChar *exts[] = { wxT(".xz"), NULL };

    const wxChar* const* ret = NULL;
    switch ( type )
    {
        case wxSTREAM_PROTOCOL: ret = encs; break;
        case wxSTREAM_MIMETYPE: ret = mime; break;
        case wxSTREAM_ENCODING: ret = encs; break;
        case wxSTREAM_FILEEXT:  ret = exts; break;
    }

    return ret;
}

#endif // wxUSE_LIBLZMA && wxUSE_STREAMS
