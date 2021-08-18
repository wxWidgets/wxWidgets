/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/ffile.cpp
// Purpose:     wxFFile encapsulates "FILE *" IO stream
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.07.99
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_FFILE

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/crt.h"
#endif

#include "wx/filename.h"
#include "wx/ffile.h"

// ============================================================================
// implementation of wxFFile
// ============================================================================

// ----------------------------------------------------------------------------
// opening the file
// ----------------------------------------------------------------------------

wxFFile::wxFFile(const wxString& filename, const wxString& mode)
{
    m_fp = NULL;

    (void)Open(filename, mode);
}

bool wxFFile::Open(const wxString& filename, const wxString& mode)
{
    wxASSERT_MSG( !m_fp, wxT("should close or detach the old file first") );

    FILE* const fp = wxFopen(filename, mode);

    if ( !fp )
    {
        wxLogSysError(_("can't open file '%s'"), filename);

        return false;
    }

    Attach(fp, filename);

    return true;
}

bool wxFFile::Close()
{
    if ( IsOpened() )
    {
        if ( fclose(m_fp) != 0 )
        {
            wxLogSysError(_("can't close file '%s'"), m_name);

            return false;
        }

        m_fp = NULL;
    }

    return true;
}

// ----------------------------------------------------------------------------
// read/write
// ----------------------------------------------------------------------------

bool wxFFile::ReadAll(wxString *str, const wxMBConv& conv)
{
    wxCHECK_MSG( str, false, wxT("invalid parameter") );
    wxCHECK_MSG( IsOpened(), false, wxT("can't read from closed file") );
    wxCHECK_MSG( Length() >= 0, false, wxT("invalid length") );
    size_t length = wx_truncate_cast(size_t, Length());
    wxCHECK_MSG( (wxFileOffset)length == Length(), false, wxT("huge file not supported") );

    clearerr(m_fp);

    wxCharBuffer buf(length);

    // note that real length may be less than file length for text files with DOS EOLs
    // ('\r's get dropped by CRT when reading which means that we have
    // realLen = fileLen - numOfLinesInTheFile)
    length = fread(buf.data(), 1, length, m_fp);

    if ( Error() )
    {
        wxLogSysError(_("Read error on file '%s'"), m_name);

        return false;
    }

    // shrink the buffer to possibly shorter data as explained above:
    buf.shrink(length);

    wxString strTmp(buf, conv);
    str->swap(strTmp);

    return true;
}

size_t wxFFile::Read(void *pBuf, size_t nCount)
{
    if ( !nCount )
        return 0;

    wxCHECK_MSG( pBuf, 0, wxT("invalid parameter") );
    wxCHECK_MSG( IsOpened(), 0, wxT("can't read from closed file") );

    size_t nRead = fread(pBuf, 1, nCount, m_fp);
    if ( (nRead < nCount) && Error() )
    {
        wxLogSysError(_("Read error on file '%s'"), m_name);
    }

    return nRead;
}

size_t wxFFile::Write(const void *pBuf, size_t nCount)
{
    if ( !nCount )
        return 0;

    wxCHECK_MSG( pBuf, 0, wxT("invalid parameter") );
    wxCHECK_MSG( IsOpened(), 0, wxT("can't write to closed file") );

    size_t nWritten = fwrite(pBuf, 1, nCount, m_fp);
    if ( nWritten < nCount )
    {
        wxLogSysError(_("Write error on file '%s'"), m_name);
    }

    return nWritten;
}

bool wxFFile::Write(const wxString& s, const wxMBConv& conv)
{
    // Writing nothing always succeeds -- and simplifies the check for
    // conversion failure below.
    if ( s.empty() )
        return true;

    const wxWX2MBbuf buf = s.mb_str(conv);

#if wxUSE_UNICODE
    const size_t size = buf.length();

    if ( !size )
    {
        // This means that the conversion failed as the original string wasn't
        // empty (we explicitly checked for this above) and in this case we
        // must fail too to indicate that we can't save the data.
        return false;
    }
#else
    const size_t size = s.length();
#endif

    return Write(buf, size) == size;
}

bool wxFFile::Flush()
{
    if ( IsOpened() )
    {
        if ( fflush(m_fp) != 0 )
        {
            wxLogSysError(_("failed to flush the file '%s'"), m_name);

            return false;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
// seeking
// ----------------------------------------------------------------------------

bool wxFFile::Seek(wxFileOffset ofs, wxSeekMode mode)
{
    wxCHECK_MSG( IsOpened(), false, wxT("can't seek on closed file") );

    int origin;
    switch ( mode )
    {
        default:
            wxFAIL_MSG(wxT("unknown seek mode"));
            wxFALLTHROUGH;

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

#ifndef wxHAS_LARGE_FFILES
    if ((long)ofs != ofs)
    {
        wxLogError(_("Seek error on file '%s' (large files not supported by stdio)"), m_name);

        return false;
    }

    if ( wxFseek(m_fp, (long)ofs, origin) != 0 )
#else
    if ( wxFseek(m_fp, ofs, origin) != 0 )
#endif
    {
        wxLogSysError(_("Seek error on file '%s'"), m_name);

        return false;
    }

    return true;
}

wxFileOffset wxFFile::Tell() const
{
    wxCHECK_MSG( IsOpened(), wxInvalidOffset,
                 wxT("wxFFile::Tell(): file is closed!") );

    wxFileOffset rc = wxFtell(m_fp);
    if ( rc == wxInvalidOffset )
    {
        wxLogSysError(_("Can't find current position in file '%s'"), m_name);
    }

    return rc;
}

wxFileOffset wxFFile::Length() const
{
    wxCHECK_MSG( IsOpened(), wxInvalidOffset,
                 wxT("wxFFile::Length(): file is closed!") );

    wxFileOffset posOld = Tell();
    if ( posOld != wxInvalidOffset )
    {
        wxFFile& self = *const_cast<wxFFile*>(this);

        if ( self.SeekEnd() )
        {
            wxFileOffset len = Tell();

            (void)self.Seek(posOld);

            return len;
        }
    }

    return wxInvalidOffset;
}

bool wxFFile::Eof() const
{
    wxCHECK_MSG( IsOpened(), false,
                 wxT("wxFFile::Eof(): file is closed!") );
    return feof(m_fp) != 0;
}

bool wxFFile::Error() const
{
    wxCHECK_MSG( IsOpened(), false,
                 wxT("wxFFile::Error(): file is closed!") );
    return ferror(m_fp) != 0;
}

// ============================================================================
// implementation of wxTempFFile
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

wxTempFFile::wxTempFFile(const wxString& strName)
{
    Open(strName);
}

bool wxTempFFile::Open(const wxString& strName)
{
    // we must have an absolute filename because otherwise CreateTempFileName()
    // would create the temp file in $TMP (i.e. the system standard location
    // for the temp files) which might be on another volume/drive/mount and
    // wxRename()ing it later to m_strName from Commit() would then fail
    //
    // with the absolute filename, the temp file is created in the same
    // directory as this one which ensures that wxRename() may work later
    wxFileName fn(strName);
    if ( !fn.IsAbsolute() )
    {
        fn.Normalize(wxPATH_NORM_ABSOLUTE);
    }

    m_strName = fn.GetFullPath();

    m_strTemp = wxFileName::CreateTempFileName(m_strName, &m_file);

    if ( m_strTemp.empty() )
    {
        // CreateTempFileName() failed
        return false;
    }

#ifdef __UNIX__
    // the temp file should have the same permissions as the original one
    mode_t mode;

    wxStructStat st;
    if ( stat( (const char*) m_strName.fn_str(), &st) == 0 )
    {
        mode = st.st_mode;
    }
    else
    {
        // file probably didn't exist, just give it the default mode _using_
        // user's umask (new files creation should respect umask)
        mode_t mask = umask(0777);
        mode = 0666 & ~mask;
        umask(mask);
    }

    if ( chmod( (const char*) m_strTemp.fn_str(), mode) == -1 )
    {
        wxLogSysError(_("Failed to set temporary file permissions"));
    }
#endif // Unix

    return true;
}

// ----------------------------------------------------------------------------
// destruction
// ----------------------------------------------------------------------------

wxTempFFile::~wxTempFFile()
{
    if ( IsOpened() )
        Discard();
}

bool wxTempFFile::Commit()
{
    m_file.Close();

    if ( wxFile::Exists(m_strName) && wxRemove(m_strName) != 0 ) {
        wxLogSysError(_("can't remove file '%s'"), m_strName);
        return false;
    }

    if ( !wxRenameFile(m_strTemp, m_strName)  ) {
        wxLogSysError(_("can't commit changes to file '%s'"), m_strName);
        return false;
    }

    return true;
}

void wxTempFFile::Discard()
{
    m_file.Close();
    if ( wxRemove(m_strTemp) != 0 )
    {
        wxLogSysError(_("can't remove temporary file '%s'"), m_strTemp);
    }
}

#endif // wxUSE_FFILE
