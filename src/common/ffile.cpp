/////////////////////////////////////////////////////////////////////////////
// Name:        ffile.cpp
// Purpose:     wxFFile encapsulates "FILE *" IO stream
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.07.99
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "ffile.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_FFILE

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

wxFFile::wxFFile(const wxChar *filename, const wxChar *mode)
{
    Detach();

    (void)Open(filename, mode);
}

bool wxFFile::Open(const wxChar *filename, const wxChar *mode)
{
    wxASSERT_MSG( !m_fp, wxT("should close or detach the old file first") );

    m_fp = wxFopen(filename, mode);

    if ( !m_fp )
    {
        wxLogSysError(_("can't open file '%s'"), filename);

        return false;
    }

    m_name = filename;

    return true;
}

bool wxFFile::Close()
{
    if ( IsOpened() )
    {
        if ( fclose(m_fp) != 0 )
        {
            wxLogSysError(_("can't close file '%s'"), m_name.c_str());

            return false;
        }

        Detach();
    }

    return true;
}

// ----------------------------------------------------------------------------
// read/write
// ----------------------------------------------------------------------------

bool wxFFile::ReadAll(wxString *str)
{
    wxCHECK_MSG( str, false, wxT("invalid parameter") );
    wxCHECK_MSG( IsOpened(), false, wxT("can't read from closed file") );

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

            return false;
        }
        //else: just EOF

        buf[nRead] = 0;
        *str += buf;
    }

    return true;
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

            return false;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
// seeking
// ----------------------------------------------------------------------------

bool wxFFile::Seek(long ofs, wxSeekMode mode)
{
    wxCHECK_MSG( IsOpened(), false, wxT("can't seek on closed file") );

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

        return false;
    }

    return true;
}

size_t wxFFile::Tell() const
{
    wxCHECK_MSG( IsOpened(), (size_t)-1,
                 _T("wxFFile::Tell(): file is closed!") );

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
    wxCHECK_MSG( IsOpened(), (size_t)-1,
                 _T("wxFFile::Length(): file is closed!") );

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

#endif // wxUSE_FFILE
