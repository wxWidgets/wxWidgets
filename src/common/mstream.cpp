/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/mstream.cpp
// Purpose:     "Memory stream" classes
// Author:      Guilhem Lavaux
// Modified by: VZ (23.11.00): general code review
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "mstream.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_STREAMS

#include <stdlib.h>
#include "wx/stream.h"
#include "wx/mstream.h"

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMemoryInputStream
// ----------------------------------------------------------------------------

wxMemoryInputStream::wxMemoryInputStream(const void *data, size_t len)
{
    m_i_streambuf = new wxStreamBuffer(wxStreamBuffer::read);
    m_i_streambuf->SetBufferIO((void *)data, len); // const_cast
    m_i_streambuf->SetIntPosition(0); // seek to start pos
    m_i_streambuf->Fixed(TRUE);

    m_length = len;
}

wxMemoryInputStream::~wxMemoryInputStream()
{
    delete m_i_streambuf;
}

char wxMemoryInputStream::Peek()
{
    char *buf = (char *)m_i_streambuf->GetBufferStart();

    return buf[m_i_streambuf->GetIntPosition()];
}

bool wxMemoryInputStream::Eof() const
{
    return !m_i_streambuf->GetBytesLeft();
}

size_t wxMemoryInputStream::OnSysRead(void *buffer, size_t nbytes)
{
    size_t pos = m_i_streambuf->GetIntPosition();
    if ( pos == m_length )
    {
        m_lasterror = wxSTREAM_EOF;

        return 0;
    }

    m_i_streambuf->Read(buffer, nbytes);
    m_lasterror = wxSTREAM_NOERROR;

    return m_i_streambuf->GetIntPosition() - pos;
}

off_t wxMemoryInputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
    return m_i_streambuf->Seek(pos, mode);
}

off_t wxMemoryInputStream::OnSysTell() const
{
    return m_i_streambuf->Tell();
}

// ----------------------------------------------------------------------------
// wxMemoryOutputStream
// ----------------------------------------------------------------------------

wxMemoryOutputStream::wxMemoryOutputStream(void *data, size_t len)
{
    m_o_streambuf = new wxStreamBuffer(wxStreamBuffer::write);
    if ( data )
        m_o_streambuf->SetBufferIO(data, len);
    m_o_streambuf->Fixed(FALSE);
    m_o_streambuf->Flushable(FALSE);
}

wxMemoryOutputStream::~wxMemoryOutputStream()
{
    delete m_o_streambuf;
}

size_t wxMemoryOutputStream::OnSysWrite(const void *buffer, size_t nbytes)
{
    size_t oldpos = m_o_streambuf->GetIntPosition();
    m_o_streambuf->Write(buffer, nbytes);
    size_t newpos = m_o_streambuf->GetIntPosition();

    // FIXME can someone please explain what this does? (VZ)
    if ( !newpos )
        newpos = m_o_streambuf->GetBufferSize();

    return newpos - oldpos;
}

off_t wxMemoryOutputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
    return m_o_streambuf->Seek(pos, mode);
}

off_t wxMemoryOutputStream::OnSysTell() const
{
    return m_o_streambuf->Tell();
}

size_t wxMemoryOutputStream::CopyTo(void *buffer, size_t len) const
{
    wxCHECK_MSG( buffer, 0, _T("must have buffer to CopyTo") );

    if ( len > GetSize() )
        len = GetSize();

    memcpy(buffer, m_o_streambuf->GetBufferStart(), len);

    return len;
}

#endif // wxUSE_STREAMS
