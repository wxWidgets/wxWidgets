/////////////////////////////////////////////////////////////////////////////
// Name:        zipstream.cpp
// Purpose:     input stream for ZIP archive access
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "zipstream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB 

#include "wx/log.h"
#include "wx/intl.h"
#include "wx/stream.h"
#include "wx/wfstream.h"
#include "wx/zipstrm.h"

/* Not the right solution (paths in makefiles) but... */
#ifdef __BORLANDC__
#include "../common/unzip.h"
#else
#include "unzip.h"
#endif


wxZipInputStream::wxZipInputStream(const wxString& archive, const wxString& file) : wxInputStream()
{
    unz_file_info zinfo;

    m_Pos = 0;
    m_Size = 0;
    m_Archive = (void*) unzOpen(archive.mb_str(wxConvFile));
    if (m_Archive == NULL) 
    {
        m_lasterror = wxStream_READ_ERR;
        return;
    }
    if (unzLocateFile((unzFile)m_Archive, file.mb_str(wxConvFile), 0) != UNZ_OK) 
    {
        m_lasterror = wxStream_READ_ERR;
        return;
    }
    
    unzGetCurrentFileInfo((unzFile)m_Archive, &zinfo, (char*) NULL, 0, (void*) NULL, 0, (char*) NULL, 0);

    if (unzOpenCurrentFile((unzFile)m_Archive) != UNZ_OK) 
    {
        m_lasterror = wxStream_READ_ERR;
        return;
    }
    m_Size = zinfo.uncompressed_size;
}



wxZipInputStream::~wxZipInputStream()
{
    if (m_Archive) 
    {
        if (m_Size != 0)
            unzCloseCurrentFile((unzFile)m_Archive);
        unzClose((unzFile)m_Archive);
    }
}



size_t wxZipInputStream::OnSysRead(void *buffer, size_t bufsize)
{
    if (m_Pos + bufsize > m_Size) bufsize = m_Size - m_Pos;
    unzReadCurrentFile((unzFile)m_Archive, buffer, bufsize);
    m_Pos += bufsize;
    return bufsize;
}



off_t wxZipInputStream::OnSysSeek(off_t seek, wxSeekMode mode)
{
    off_t nextpos;
    void *buf;

    switch (mode) 
    {
        case wxFromCurrent : nextpos = seek + m_Pos; break;
        case wxFromStart : nextpos = seek; break;
        case wxFromEnd : nextpos = m_Size - 1 + seek; break;
        default : nextpos = m_Pos; break; /* just to fool compiler, never happens */
    }

    // cheated seeking :
    if (nextpos > m_Pos) 
    {
        buf = malloc(nextpos - m_Pos);
        unzReadCurrentFile((unzFile)m_Archive, buf, nextpos -  m_Pos);
        free(buf);
    }
    else if (nextpos < m_Pos) {
        unzCloseCurrentFile((unzFile)m_Archive);
        if (unzOpenCurrentFile((unzFile)m_Archive) != UNZ_OK) 
	{
            m_lasterror = wxStream_READ_ERR;
            return m_Pos;
        }
        buf = malloc(nextpos);
        unzReadCurrentFile((unzFile)m_Archive, buf, nextpos);
        free(buf);
    }

    m_Pos = nextpos;
    return m_Pos;
}

#endif
  // wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB 
