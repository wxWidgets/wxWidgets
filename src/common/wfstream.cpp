/////////////////////////////////////////////////////////////////////////////
// Name:        fstream.cpp
// Purpose:     "File stream" classes
// Author:      Julian Smart
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wfstream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include <stdio.h>
#include "wx/stream.h"
#include "wx/wfstream.h"

#if wxUSE_FILE

// ----------------------------------------------------------------------------
// wxFileInputStream
// ----------------------------------------------------------------------------

wxFileInputStream::wxFileInputStream(const wxString& fileName)
  : wxInputStream()
{
    m_file = new wxFile(fileName, wxFile::read);
    m_file_destroy = true;
}

wxFileInputStream::wxFileInputStream()
  : wxInputStream()
{
    m_file_destroy = false;
    m_file = NULL;
}

wxFileInputStream::wxFileInputStream(wxFile& file)
{
    m_file = &file;
    m_file_destroy = false;
}

wxFileInputStream::wxFileInputStream(int fd)
{
    m_file = new wxFile(fd);
    m_file_destroy = true;
}

wxFileInputStream::~wxFileInputStream()
{
    if (m_file_destroy)
        delete m_file;
}

wxFileOffset wxFileInputStream::GetLength() const
{
    return m_file->Length();
}

size_t wxFileInputStream::OnSysRead(void *buffer, size_t size)
{
    ssize_t ret = m_file->Read(buffer, size);

    // NB: we can't use a switch here because HP-UX CC doesn't allow
    //     switching over long long (which size_t is in 64bit mode)

    if ( !ret )
    {
        // nothing read, so nothing more to read
        m_lasterror = wxSTREAM_EOF;
    }
    else if ( ret == wxInvalidOffset )
    {
        m_lasterror = wxSTREAM_READ_ERROR;
        ret = 0;
    }
    else
    {
        // normal case
        m_lasterror = wxSTREAM_NO_ERROR;
    }

    return ret;
}

wxFileOffset wxFileInputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode);
}

wxFileOffset wxFileInputStream::OnSysTell() const
{
    return m_file->Tell();
}

// ----------------------------------------------------------------------------
// wxFileOutputStream
// ----------------------------------------------------------------------------

wxFileOutputStream::wxFileOutputStream(const wxString& fileName)
{
    m_file = new wxFile(fileName, wxFile::write);
    m_file_destroy = true;

    if (!m_file->IsOpened())
    {
        m_lasterror = wxSTREAM_WRITE_ERROR;
    }
    else
    {
        if (m_file->Error())
            m_lasterror = wxSTREAM_WRITE_ERROR;
    }
}

wxFileOutputStream::wxFileOutputStream(wxFile& file)
{
    m_file = &file;
    m_file_destroy = false;
}

wxFileOutputStream::wxFileOutputStream()
                  : wxOutputStream()
{
    m_file_destroy = false;
    m_file = NULL;
}

wxFileOutputStream::wxFileOutputStream(int fd)
{
    m_file = new wxFile(fd);
    m_file_destroy = true;
}

wxFileOutputStream::~wxFileOutputStream()
{
    if (m_file_destroy)
    {
        Sync();
        delete m_file;
    }
}

size_t wxFileOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    size_t ret = m_file->Write(buffer, size);

    m_lasterror = m_file->Error() ? wxSTREAM_WRITE_ERROR : wxSTREAM_NO_ERROR;

    return ret;
}

wxFileOffset wxFileOutputStream::OnSysTell() const
{
    return m_file->Tell();
}

wxFileOffset wxFileOutputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode);
}

void wxFileOutputStream::Sync()
{
    wxOutputStream::Sync();
    m_file->Flush();
}

wxFileOffset wxFileOutputStream::GetLength() const
{
    return m_file->Length();
}

// ----------------------------------------------------------------------------
// wxFileStream
// ----------------------------------------------------------------------------

wxFileStream::wxFileStream(const wxString& fileName)
            : wxFileInputStream(fileName)
{
    wxFileOutputStream::m_file = wxFileInputStream::m_file;
}

#endif //wxUSE_FILE

#if wxUSE_FFILE

// ----------------------------------------------------------------------------
// wxFFileInputStream
// ----------------------------------------------------------------------------

wxFFileInputStream::wxFFileInputStream(const wxString& fileName,
                                       const wxChar *mode)
                  : wxInputStream()
{
    m_file = new wxFFile(fileName, mode);
    m_file_destroy = true;
}

wxFFileInputStream::wxFFileInputStream()
                  : wxInputStream()
{
    m_file = NULL;
    m_file_destroy = false;
}

wxFFileInputStream::wxFFileInputStream(wxFFile& file)
{
    m_file = &file;
    m_file_destroy = false;
}

wxFFileInputStream::wxFFileInputStream(FILE *file)
{
    m_file = new wxFFile(file);
    m_file_destroy = true;
}

wxFFileInputStream::~wxFFileInputStream()
{
    if (m_file_destroy)
        delete m_file;
}

wxFileOffset wxFFileInputStream::GetLength() const
{
    return m_file->Length();
}

size_t wxFFileInputStream::OnSysRead(void *buffer, size_t size)
{
    ssize_t ret = m_file->Read(buffer, size);

    // It is not safe to call Eof() if the file is not opened.
    if (!m_file->IsOpened() || m_file->Eof())
        m_lasterror = wxSTREAM_EOF;
    if (ret == wxInvalidOffset)
    {
        m_lasterror = wxSTREAM_READ_ERROR;
        ret = 0;
    }

    return ret;
}

wxFileOffset wxFFileInputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode) ? m_file->Tell() : wxInvalidOffset;
}

wxFileOffset wxFFileInputStream::OnSysTell() const
{
    return m_file->Tell();
}

// ----------------------------------------------------------------------------
// wxFFileOutputStream
// ----------------------------------------------------------------------------

wxFFileOutputStream::wxFFileOutputStream(const wxString& fileName,
                                         const wxChar *mode)
{
    m_file = new wxFFile(fileName, mode);
    m_file_destroy = true;

    if (!m_file->IsOpened())
    {
        m_lasterror = wxSTREAM_WRITE_ERROR;
    }
    else
    {
        if (m_file->Error())
            m_lasterror = wxSTREAM_WRITE_ERROR;
    }
}

wxFFileOutputStream::wxFFileOutputStream(wxFFile& file)
{
    m_file = &file;
    m_file_destroy = false;
}

wxFFileOutputStream::wxFFileOutputStream()
                   : wxOutputStream()
{
    m_file = NULL;
    m_file_destroy = false;
}

wxFFileOutputStream::wxFFileOutputStream(FILE *file)
{
    m_file = new wxFFile(file);
    m_file_destroy = true;
}

wxFFileOutputStream::~wxFFileOutputStream()
{
    if (m_file_destroy)
    {
        Sync();
        delete m_file;
    }
}

size_t wxFFileOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    size_t ret = m_file->Write(buffer, size);
    // It is not safe to call Error() if the file is not opened.
    if (!m_file->IsOpened() || m_file->Error())
        m_lasterror = wxSTREAM_WRITE_ERROR;
    else
        m_lasterror = wxSTREAM_NO_ERROR;
    return ret;
}

wxFileOffset wxFFileOutputStream::OnSysTell() const
{
    return m_file->Tell();
}

wxFileOffset wxFFileOutputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode) ? m_file->Tell() : wxInvalidOffset;
}

void wxFFileOutputStream::Sync()
{
    wxOutputStream::Sync();
    m_file->Flush();
}

wxFileOffset wxFFileOutputStream::GetLength() const
{
    return m_file->Length();
}

// ----------------------------------------------------------------------------
// wxFFileStream
// ----------------------------------------------------------------------------

wxFFileStream::wxFFileStream(const wxString& fileName)
             : wxFFileInputStream(fileName)
{
    wxFFileOutputStream::m_file = wxFFileInputStream::m_file;
}

#endif //wxUSE_FFILE

#endif // wxUSE_STREAMS

