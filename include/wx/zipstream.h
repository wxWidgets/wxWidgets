/////////////////////////////////////////////////////////////////////////////
// Name:        zipstream.h
// Purpose:     wxZipInputStream for reading files from ZIP archive
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __ZIPSTREAM_H__
#define __ZIPSTREAM_H__

#ifdef __GNUG__
#pragma interface
#endif

#if wxUSE_ZLIB && wxUSE_STREAMS

#include <wx/stream.h>

//--------------------------------------------------------------------------------
// wxZipInputStream
//                  This class is input stream from ZIP archive. The archive
//                  must be local file (accessible via FILE*)
//--------------------------------------------------------------------------------


class WXDLLEXPORT wxZipInputStream : public wxInputStream
{
    private:
        size_t m_Size;
        off_t m_Pos;
        void *m_Archive;
                // this void* is handle of archive .
                // I'm sorry it is void and not proper type but I don't want
                // to make unzip.h header public.

    public:
        wxZipInputStream(const wxString& archive, const wxString& file);
                // archive is name of .zip archive, file is name of file to be extracted.
                // Remember that archive must be local file accesible via fopen, fread functions!
        ~wxZipInputStream();

    protected:
        virtual size_t StreamSize() const {return m_Size;}
        virtual size_t OnSysRead(void *buffer, size_t bufsize);
        virtual off_t OnSysSeek(off_t seek, wxSeekMode mode);
        virtual off_t OnSysTell() const {return m_Pos;}
};


#endif // if use_zlib && use_streams

#endif // __ZIPSTREAM_H__

















