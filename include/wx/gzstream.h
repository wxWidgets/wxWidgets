/////////////////////////////////////////////////////////////////////////////
// Name:        gzstream.h
// Purpose:     Streams for Gzip files
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Mike Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GZSTREAM_H__
#define _WX_GZSTREAM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "gzstream.h"
#endif

#include "wx/defs.h"

#if wxUSE_STREAMS && wxUSE_GZSTREAM && wxUSE_ZLIB 

#include "wx/datetime.h"


/////////////////////////////////////////////////////////////////////////////
// wxGzipInputStream 

class WXDLLIMPEXP_BASE wxGzipInputStream : public wxFilterInputStream
{
public:
    wxGzipInputStream(wxInputStream& stream, wxMBConv& conv = wxConvFile);
    virtual ~wxGzipInputStream();

    wxString GetName() const { return m_name; }

#if wxUSE_DATETIME
    wxDateTime GetDateTime() const { return m_datetime; }
#endif

    virtual char Peek() { return wxInputStream::Peek(); }
    virtual size_t GetSize() const { return m_decomp ? m_decomp->GetSize() : 0; }

protected:
    virtual size_t OnSysRead(void *buffer, size_t size);
    virtual off_t OnSysTell() const { return m_decomp ? m_decomp->TellI() : 0; }

private:
    wxInputStream *m_decomp;
    wxUint32 m_crc;
    wxString m_name;

#if wxUSE_DATETIME
    wxDateTime m_datetime;
#endif

    DECLARE_NO_COPY_CLASS(wxGzipInputStream)
};


/////////////////////////////////////////////////////////////////////////////
// wxGzipOutputStream 

class WXDLLIMPEXP_BASE wxGzipOutputStream : public wxFilterOutputStream
{
public:
    wxGzipOutputStream(wxOutputStream& stream,
                       const wxString& originalName = wxEmptyString,
                       int level = -1,
                       wxMBConv& conv = wxConvFile);
    virtual ~wxGzipOutputStream();

    virtual void Sync();

protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size);
    virtual off_t OnSysTell() const { return m_comp ? m_comp->TellO() : 0; }

private:
    wxOutputStream *m_comp;
    wxUint32 m_crc;

    DECLARE_NO_COPY_CLASS(wxGzipOutputStream)
};


#endif // wxUSE_STREAMS && wxUSE_GZSTREAM && wxUSE_ZLIB 

#endif // _WX_GZSTREAM_H__
