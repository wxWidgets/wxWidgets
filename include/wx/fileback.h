/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/fileback.h
// Purpose:     Back an input stream with memory or a file
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Mike Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEBACK_H__
#define _WX_FILEBACK_H__

#include "wx/defs.h"

#if wxUSE_BACKINGFILE

// ----------------------------------------------------------------------------
// Backs an input stream with memory or a file to make it seekable.
//
// One or more wxBackedInputStreams can be used to read it's data. The data is
// reference counted, so stays alive until the last wxBackingFile or
// wxBackedInputStream using it is destroyed.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxBackingFile
{
public:
    enum { DefaultBufSize = 16384 };

    // Takes ownership of stream. If the stream is smaller than bufsize, the
    // backing file is never created and the backing is done with memory.
    wxBackingFile(wxInputStream *stream,
                  size_t bufsize = DefaultBufSize,
                  const wxString& prefix = _T("wxbf"));

    wxBackingFile() : m_impl(NULL) { }
    ~wxBackingFile();

    wxBackingFile(const wxBackingFile& backer);
    wxBackingFile& operator=(const wxBackingFile& backer);

    operator bool() const { return m_impl != NULL; }

private:
    class wxBackingFileImpl *m_impl;
    friend class wxBackedInputStream;
};

// ----------------------------------------------------------------------------
// An input stream to read from a wxBackingFile.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxBackedInputStream : public wxInputStream
{
public:
    wxBackedInputStream(const wxBackingFile& backer);

    wxFileOffset GetLength() const;
    bool IsSeekable() const { return true; }

protected:
    size_t OnSysRead(void *buffer, size_t size);
    wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
    wxFileOffset OnSysTell() const;

private:
    wxBackingFile m_backer;
    wxFileOffset m_pos;

    DECLARE_NO_COPY_CLASS(wxBackedInputStream)
};

#endif // wxUSE_BACKINGFILE

#endif // _WX_FILEBACK_H__
