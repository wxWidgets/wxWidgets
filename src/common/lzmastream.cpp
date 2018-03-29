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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
    // it doesn't make sense to continue after it neither) and have space to
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

#endif // wxUSE_LIBLZMA && wxUSE_STREAMS
