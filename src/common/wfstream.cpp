/////////////////////////////////////////////////////////////////////////////
// Name:        fstream.cpp
// Purpose:     "File stream" classes
// Author:      Julian Smart
// Modified by:
// Created:     11/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "wfstream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS && wxUSE_FILE

#include <stdio.h>
#include "wx/stream.h"
#include "wx/wfstream.h"

// ----------------------------------------------------------------------------
// wxFileInputStream
// ----------------------------------------------------------------------------

wxFileInputStream::wxFileInputStream(const wxString& fileName)
  : wxInputStream()
{
    m_file = new wxFile(fileName, wxFile::read);
    m_file_destroy = TRUE;
}

wxFileInputStream::wxFileInputStream()
  : wxInputStream()
{
    m_file_destroy = FALSE;
    m_file = NULL;
}

wxFileInputStream::wxFileInputStream(wxFile& file)
{
    m_file = &file;
    m_file_destroy = FALSE;
}

wxFileInputStream::wxFileInputStream(int fd)
{
    m_file = new wxFile(fd);
    m_file_destroy = TRUE;
}

wxFileInputStream::~wxFileInputStream()
{
    if (m_file_destroy)
        delete m_file;
}

size_t wxFileInputStream::GetSize() const
{
    return m_file->Length();
}

size_t wxFileInputStream::OnSysRead(void *buffer, size_t size)
{
    off_t ret;

    ret = m_file->Read(buffer, size);

    m_lasterror = wxStream_NOERROR;
    if (m_file->Eof())
        m_lasterror = wxStream_EOF;
    if (ret == wxInvalidOffset) 
    {
        m_lasterror = wxStream_READ_ERR;
        ret = 0;
    }

    return ret;
}

off_t wxFileInputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode);
}

off_t wxFileInputStream::OnSysTell() const
{
    return m_file->Tell();
}

// ----------------------------------------------------------------------------
// wxFileOutputStream
// ----------------------------------------------------------------------------

wxFileOutputStream::wxFileOutputStream(const wxString& fileName)
{
    m_file = new wxFile(fileName, wxFile::write);
    m_file_destroy = TRUE;
}

wxFileOutputStream::wxFileOutputStream(wxFile& file)
{
    m_file = &file;
    m_file_destroy = FALSE;
}

wxFileOutputStream::wxFileOutputStream()
  : wxOutputStream()
{
    m_file_destroy = FALSE;
    m_file = NULL;
}

wxFileOutputStream::wxFileOutputStream(int fd)
{
    m_file = new wxFile(fd);
    m_file_destroy = TRUE;
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
    if (m_file->Error())
        m_lasterror = wxStream_WRITE_ERR;
    else
        m_lasterror = wxStream_NOERROR;
    return ret;
}

off_t wxFileOutputStream::OnSysTell() const
{
    return m_file->Tell();
}

off_t wxFileOutputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode);
}

void wxFileOutputStream::Sync()
{
    wxOutputStream::Sync();
    m_file->Flush();
}

size_t wxFileOutputStream::GetSize() const
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

// ----------------------------------------------------------------------------
// wxFFileInputStream
// ----------------------------------------------------------------------------

wxFFileInputStream::wxFFileInputStream(const wxString& fileName)
  : wxInputStream()
{
    m_file = new wxFFile(fileName, "r");
    m_file_destroy = TRUE;
}

wxFFileInputStream::wxFFileInputStream()
  : wxInputStream()
{
    m_file_destroy = FALSE;
    m_file = NULL;
}

wxFFileInputStream::wxFFileInputStream(wxFFile& file)
{
    m_file = &file;
    m_file_destroy = FALSE;
}

wxFFileInputStream::wxFFileInputStream(FILE *file)
{
    m_file = new wxFFile(file);
    m_file_destroy = TRUE;
}

wxFFileInputStream::~wxFFileInputStream()
{
    if (m_file_destroy)
        delete m_file;
}

size_t wxFFileInputStream::GetSize() const
{
    return m_file->Length();
}

size_t wxFFileInputStream::OnSysRead(void *buffer, size_t size)
{
    off_t ret;

    ret = m_file->Read(buffer, size);

    if (m_file->Eof())
        m_lasterror = wxStream_EOF;
    if (ret == wxInvalidOffset) 
    {
        m_lasterror = wxStream_READ_ERR;
        ret = 0;
    }

    return ret;
}

off_t wxFFileInputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode);
}

off_t wxFFileInputStream::OnSysTell() const
{
    return m_file->Tell();
}

// ----------------------------------------------------------------------------
// wxFFileOutputStream
// ----------------------------------------------------------------------------

wxFFileOutputStream::wxFFileOutputStream(const wxString& fileName)
{
    m_file = new wxFFile(fileName, "w+b");
    m_file_destroy = TRUE;
}

wxFFileOutputStream::wxFFileOutputStream(wxFFile& file)
{
    m_file = &file;
    m_file_destroy = FALSE;
}

wxFFileOutputStream::wxFFileOutputStream()
  : wxOutputStream()
{
    m_file_destroy = FALSE;
    m_file = NULL;
}

wxFFileOutputStream::wxFFileOutputStream(FILE *file)
{
    m_file = new wxFFile(file);
    m_file_destroy = TRUE;
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
    if (m_file->Error())
        m_lasterror = wxStream_WRITE_ERR;
    else
        m_lasterror = wxStream_NOERROR;
    return ret;
}

off_t wxFFileOutputStream::OnSysTell() const
{
    return m_file->Tell();
}

off_t wxFFileOutputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
    return m_file->Seek(pos, mode);
}

void wxFFileOutputStream::Sync()
{
    wxOutputStream::Sync();
    m_file->Flush();
}

size_t wxFFileOutputStream::GetSize() const
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

#endif
  // wxUSE_STREAMS && wxUSE_FILE

