/////////////////////////////////////////////////////////////////////////////
// Name:        zipstream.h
// Purpose:     wxZipInputStream for reading files from ZIP archive
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __ZIPSTREAM_H__
#define __ZIPSTREAM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "zipstrm.h"
#endif

#include "wx/defs.h"

#if wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB

#include "wx/stream.h"

//--------------------------------------------------------------------------------
// wxZipInputStream
//                  This class is input stream from ZIP archive. The archive
//                  must be local file (accessible via FILE*)
//--------------------------------------------------------------------------------


class WXDLLIMPEXP_BASE wxZipInputStream : public wxInputStream
{
public:
    wxZipInputStream(const wxString& archive, const wxString& file);
            // archive is name of .zip archive, file is name of file to be extracted.
            // Remember that archive must be local file accesible via fopen, fread functions!
    ~wxZipInputStream();

    virtual size_t GetSize() const {return m_Size;}
    virtual bool Eof() const;

protected:
    virtual size_t OnSysRead(void *buffer, size_t bufsize);
    virtual wxFileOffset OnSysSeek(wxFileOffset seek, wxSeekMode mode);
    virtual wxFileOffset OnSysTell() const {return m_Pos;}

private:
    size_t m_Size;
    wxFileOffset m_Pos;

    // this void* is handle of archive . I'm sorry it is void and not proper
    // type but I don't want to make unzip.h header public.
    void *m_Archive;

    DECLARE_NO_COPY_CLASS(wxZipInputStream)
};


#endif
   // wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB

#endif
   // __ZIPSTREAM_H__
