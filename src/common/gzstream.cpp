/////////////////////////////////////////////////////////////////////////////
// Name:        gzstream.cpp
// Purpose:     Streams for Gzip files
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Mike Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "gzstream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS && wxUSE_GZSTREAM && wxUSE_ZLIB 

#include "wx/log.h"
#include "wx/intl.h"
#include "wx/datstrm.h"
#include "wx/txtstrm.h"
#include "wx/filename.h"
#include "wx/zstream.h"
#include "wx/gzstream.h"

#if defined(__WXMSW__) && !defined(__WX_SETUP_H__) && !defined(wxUSE_ZLIB_H_IN_PATH)
   #include "../zlib/zlib.h"
#else
   #include "zlib.h"
#endif


/////////////////////////////////////////////////////////////////////////////
// Notes:
//
// See RFC-1952 and the Gzip/Zlib sources for the details of the Gzip format
//
// Errors are displayed with wxLogError, but not errors from the compressor
// or underlying stream, since they will display their own errors.
//
// Gzip doesn't use flag 2 to indicate a header crc, so I think it's
// probably better not to use it for the moment.
//

// Flags
enum {
    GZ_ASCII_FLAG   = 0x01,
    GZ_HEAD_CRC     = 0x02,
    GZ_EXTRA_FIELD  = 0x04,
    GZ_ORIG_NAME    = 0x08,
    GZ_COMMENT      = 0x10,
    GZ_RESERVED     = 0xE0
};

// Extra flags
enum {
    GZ_SLOWEST = 2,
    GZ_FASTEST = 4
};

const wxUint16 GZ_MAGIC = 0x8b1f;


/////////////////////////////////////////////////////////////////////////////
// Input stream

wxGzipInputStream::wxGzipInputStream(wxInputStream& stream,
                                     wxMBConv& conv /*=wxConvFile*/)
  : wxFilterInputStream(stream)
{
    m_decomp = NULL;
    m_crc = crc32(0, Z_NULL, 0);

    // Try to read the Gzip magic numbers 0x1f, 0x8b. If not found then the
    // underlying stream isn't gzipped after all, so unread the bytes taken
    // so that the underlying stream can be read directly instead.
    wxUint8 magic[2];
    size_t n = m_parent_i_stream->Read(magic, sizeof(magic)).LastRead();

    if (n < sizeof(magic) || ((magic[1] << 8) | magic[0]) != GZ_MAGIC) {
        if (n)
            m_parent_i_stream->Ungetch(magic, n);
        // Set EOF rather than error to indicate no gzip data
        m_lasterror = wxSTREAM_EOF;
        return;
    }

    wxDataInputStream ds(*m_parent_i_stream);

    // read method, flags, timestamp, extra flags and OS-code
    int method = ds.Read8();
    int flags = ds.Read8();
#if wxUSE_DATETIME
    wxUint32 datetime = ds.Read32();
    if (datetime)   // zero means not set (not -1 as usual for time_t)
        m_datetime = wxLongLong(0, datetime) * 1000L;
#else
    ds.Read32();
#endif
    ds.Read8();
    ds.Read8();

    if (flags & GZ_HEAD_CRC)
        ds.Read16();

    if (flags & GZ_EXTRA_FIELD)
        for (int i = ds.Read16(); i > 0 && m_parent_i_stream->IsOk(); i--)
            m_parent_i_stream->GetC();

    // RFC-1952 specifies ISO-8859-1 for these fields
    if (flags & GZ_ORIG_NAME) {
#if wxUSE_UNICODE
        wxTextInputStream tis(*m_parent_i_stream, wxT(" \t"), conv);
#else
        wxTextInputStream tis(*m_parent_i_stream);
        (void)conv;
#endif
        wxChar c;
        while ((c = tis.GetChar()) != 0 && m_parent_i_stream->IsOk())
            m_name += c;
    }

    if (flags & GZ_COMMENT)
        while (m_parent_i_stream->GetC() != 0 && m_parent_i_stream->IsOk())
            ;   // empty loop

    m_lasterror = wxSTREAM_READ_ERROR;
    if (!*m_parent_i_stream) {
        wxLogDebug(wxT("Error reading Gzip header"));
        return;
    }

    if (flags & GZ_RESERVED)
        wxLogWarning(_("Unsupported flag in Gzip header"));

    switch (method) {
        case Z_DEFLATED:
            m_decomp = new wxZlibInputStream(*m_parent_i_stream, wxZLIB_NO_HEADER);
            break;

        default:
            wxLogError(_("unsupported compression method in Gzip stream"));
            return; 
    }

    if (m_decomp)
        m_lasterror = m_decomp->GetLastError();
}


wxGzipInputStream::~wxGzipInputStream()
{
    delete m_decomp;
}


size_t wxGzipInputStream::OnSysRead(void *buffer, size_t size)
{
    wxASSERT_MSG(m_decomp, wxT("Gzip not open"));

    if (!m_decomp)
        m_lasterror = wxSTREAM_READ_ERROR;
    if (!IsOk() || !size)
        return 0;

    m_decomp->Read(buffer, size);
    m_crc = crc32(m_crc, (Byte*)buffer, m_decomp->LastRead());

    if (m_decomp->Eof()) {
        wxDataInputStream ds(*m_parent_i_stream);
        m_lasterror = wxSTREAM_READ_ERROR;

        if (m_parent_i_stream->IsOk() && ds.Read32() != m_crc)
            wxLogError(_("reading Gzip stream: bad crc"));
        else if (m_parent_i_stream->IsOk() && ds.Read32() != (wxUint32)TellI())
            wxLogError(_("reading Gzip stream: incorrect length"));
        else if (m_parent_i_stream->IsOk())
            m_lasterror = wxSTREAM_EOF;
    } 
    else if (!*m_decomp) {
        m_lasterror = wxSTREAM_READ_ERROR;
    }

    return m_decomp->LastRead();
}


/////////////////////////////////////////////////////////////////////////////
// Output stream

wxGzipOutputStream::wxGzipOutputStream(
                            wxOutputStream& stream,
                            const wxString& originalName /*=wxEmptyString*/,
                            int level /*=-1*/,
                            wxMBConv& conv /*=wxConvFile*/)
  : wxFilterOutputStream(stream)
{
    m_comp = NULL;
    m_crc = crc32(0, Z_NULL, 0);

    wxFileName filename(originalName);

#if wxUSE_DATETIME
    wxDateTime datetime;

    if (filename.FileExists())
        datetime = filename.GetModificationTime();
    else
        datetime = wxDateTime::Now();

    wxUint32 timestamp = (datetime.GetValue() / 1000L).GetLo();
#else
    wxUint32 timestamp = 0;
#endif

    // RFC-1952 specifies ISO-8859-1 for the name. Also it should be just the
    // name part, no directory, folded to lowercase if case insensitive
    wxString name = filename.GetFullName();
    const wxWX2MBbuf mbName = conv.cWX2MB(name);
    
    wxDataOutputStream ds(*m_parent_o_stream);

    // write signature, method, flags, timestamp, extra flags and OS-code
    ds.Write16(GZ_MAGIC);
    ds.Write8(Z_DEFLATED);
    ds.Write8(mbName && *mbName ? GZ_ORIG_NAME : 0);
    ds.Write32(timestamp);
    ds.Write8(level == 1 ? GZ_FASTEST : level == 9 ? GZ_SLOWEST : 0);
    ds.Write8(255);

    if (mbName && *mbName)
        m_parent_o_stream->Write(mbName, strlen(mbName) + 1);

    m_lasterror = wxSTREAM_WRITE_ERROR;
    if (!*m_parent_o_stream) {
        wxLogDebug(wxT("Error writing Gzip header"));
        return;
    }

    m_comp = new wxZlibOutputStream(*m_parent_o_stream, level, wxZLIB_NO_HEADER);

    if (m_comp)
        m_lasterror = m_comp->GetLastError();
}


wxGzipOutputStream::~wxGzipOutputStream()
{
    if (m_comp && m_comp->IsOk()) {
        wxUint32 len = (wxUint32)m_comp->TellO();
        delete m_comp;
        if (m_parent_o_stream->IsOk()) {
            wxDataOutputStream ds(*m_parent_o_stream);
            ds.Write32(m_crc);
            ds.Write32(len);    // underlying stream will report errors
        }
    } else {
        delete m_comp;
    }
}


void wxGzipOutputStream::Sync()
{
    wxASSERT_MSG(m_comp, wxT("Gzip not open"));

    if (!m_comp)
        m_lasterror = wxSTREAM_WRITE_ERROR;
    if (IsOk())
        m_comp->Sync();
}


size_t wxGzipOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    wxASSERT_MSG(m_comp, wxT("Gzip not open"));

    if (!m_comp)
        m_lasterror = wxSTREAM_WRITE_ERROR;
    if (!IsOk() || !size)
        return 0;

    if (m_comp->Write(buffer, size).LastWrite() != size)
        m_lasterror = wxSTREAM_WRITE_ERROR;
    m_crc = crc32(m_crc, (Byte*)buffer, size);

    return m_comp->LastWrite();
}


#endif // wxUSE_STREAMS && wxUSE_GZSTREAM && wxUSE_ZLIB 
