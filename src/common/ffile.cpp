/////////////////////////////////////////////////////////////////////////////
// Name:        ffile.cpp
// Purpose:     wxFFile - encapsulates "FILE *" IO stream
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.07.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "ffile.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_FILE

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include "wx/ffile.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// opening the file
// ----------------------------------------------------------------------------

wxFFile::wxFFile(const wxChar *filename, const char *mode)
{
    Detach();

    (void)Open(filename, mode);
}

bool wxFFile::Open(const wxChar *filename, const char *mode)
{
    wxASSERT_MSG( !m_fp, wxT("should close or detach the old file first") );

#if wxUSE_UNICODE
    char *tmp_fname;
    size_t fname_len;

    fname_len = wxStrlen(filename)+1;
    tmp_fname = new char[fname_len];
    wxWX2MB(tmp_fname, filename, fname_len);

#ifdef __WXMAC__
  	m_fp = fopen(wxUnix2MacFilename( tmp_fname ), mode);
#else
    m_fp = fopen(tmp_fname, mode);
#endif

    delete tmp_fname;
#else
#ifdef __WXMAC__
  	m_fp = fopen(wxUnix2MacFilename( filename ), mode);
#else
    m_fp = fopen(filename, mode);
#endif
#endif


    if ( !m_fp )
    {
        wxLogSysError(_("can't open file '%s'"), filename);

        return FALSE;
    }

    m_name = filename;

    return TRUE;
}

bool wxFFile::Close()
{
    if ( IsOpened() )
    {
        if ( fclose(m_fp) != 0 )
        {
            wxLogSysError(_("can't close file '%s'"), m_name.c_str());

            return FALSE;
        }

        Detach();
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// read/write
// ----------------------------------------------------------------------------

bool wxFFile::ReadAll(wxString *str)
{
    wxCHECK_MSG( str, FALSE, wxT("invalid parameter") );
    wxCHECK_MSG( IsOpened(), FALSE, wxT("can't read from closed file") );

    clearerr(m_fp);

    str->Empty();
    str->Alloc(Length());

    wxChar buf[1024];
    static const size_t nSize = WXSIZEOF(buf) - 1; // -1 for trailing '\0'
    while ( !Eof() )
    {
        size_t nRead = fread(buf, sizeof(wxChar), nSize, m_fp);
        if ( (nRead < nSize) && Error() )
        {
            wxLogSysError(_("Read error on file '%s'"), m_name.c_str());

            return FALSE;
        }
        //else: just EOF

        buf[nRead] = 0;
        *str += buf;
    }

    return TRUE;
}

size_t wxFFile::Read(void *pBuf, size_t nCount)
{
    wxCHECK_MSG( pBuf, FALSE, wxT("invalid parameter") );
    wxCHECK_MSG( IsOpened(), FALSE, wxT("can't read from closed file") );

    size_t nRead = fread(pBuf, 1, nCount, m_fp);
    if ( (nRead < nCount) && Error() )
    {
        wxLogSysError(_("Read error on file '%s'"), m_name.c_str());
    }

    return nRead;
}

size_t wxFFile::Write(const void *pBuf, size_t nCount)
{
    wxCHECK_MSG( pBuf, FALSE, wxT("invalid parameter") );
    wxCHECK_MSG( IsOpened(), FALSE, wxT("can't write to closed file") );

    size_t nWritten = fwrite(pBuf, 1, nCount, m_fp);
    if ( nWritten < nCount )
    {
        wxLogSysError(_("Write error on file '%s'"), m_name.c_str());
    }

    return nWritten;
}

bool wxFFile::Flush()
{
    if ( IsOpened() )
    {
        // fflush returns non-zero on error
        //
        if ( fflush(m_fp) )
        {
            wxLogSysError(_("failed to flush the file '%s'"), m_name.c_str());

            return FALSE;
        }
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// seeking
// ----------------------------------------------------------------------------

bool wxFFile::Seek(long ofs, wxSeekMode mode)
{
    wxCHECK_MSG( IsOpened(), FALSE, wxT("can't seek on closed file") );

    int origin;
    switch ( mode )
    {
        default:
            wxFAIL_MSG(wxT("unknown seek mode"));
            // still fall through

        case wxFromStart:
            origin = SEEK_SET;
            break;

        case wxFromCurrent:
            origin = SEEK_CUR;
            break;

        case wxFromEnd:
            origin = SEEK_END;
            break;
    }

    if ( fseek(m_fp, ofs, origin) != 0 )
    {
        wxLogSysError(_("Seek error on file '%s'"), m_name.c_str());

        return FALSE;
    }

    return TRUE;
}

size_t wxFFile::Tell() const
{
    long rc = ftell(m_fp);
    if ( rc == -1 )
    {
        wxLogSysError(_("Can't find current position in file '%s'"),
                      m_name.c_str());
    }

    return (size_t)rc;
}

size_t wxFFile::Length() const
{
    wxFFile& self = *(wxFFile *)this;   // const_cast

    size_t posOld = Tell();
    if ( posOld != (size_t)-1 )
    {
        if ( self.SeekEnd() )
        {
            size_t len = Tell();

            (void)self.Seek(posOld);

            return len;
        }
    }

    return (size_t)-1;
}

#endif // wxUSE_FILE
