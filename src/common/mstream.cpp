/////////////////////////////////////////////////////////////////////////////
// Name:        mstream.cpp
// Purpose:     "Memory stream" classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

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

// ----------------------------------------------------------------------------
// wxMemoryInputStream
// ----------------------------------------------------------------------------

wxMemoryInputStream::wxMemoryInputStream(const char *data, size_t len)
  : wxInputStream()
{
  m_i_streambuf = new wxStreamBuffer(wxStreamBuffer::read);
  m_i_streambuf->SetBufferIO((char*) data, (char*) (data+len));
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
  return m_i_streambuf->GetBufferStart()[m_i_streambuf->GetIntPosition()];
}

size_t wxMemoryInputStream::OnSysRead(void *buffer, size_t nbytes)
{
  m_lastcount = 0;
  return m_i_streambuf->Read(buffer, nbytes);
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

wxMemoryOutputStream::wxMemoryOutputStream(char *data, size_t len)
  : wxOutputStream()
{
  m_o_streambuf = new wxStreamBuffer(wxStreamBuffer::write);
  if (data)
    m_o_streambuf->SetBufferIO(data, data+len);
  m_o_streambuf->Fixed(TRUE);
}

wxMemoryOutputStream::~wxMemoryOutputStream()
{
  delete m_o_streambuf;
}

size_t wxMemoryOutputStream::OnSysWrite(const void *buffer, size_t nbytes)
{
  m_lastcount = 0;
  return m_o_streambuf->Write(buffer, nbytes);
}

off_t wxMemoryOutputStream::OnSysSeek(off_t pos, wxSeekMode mode)
{
  return m_o_streambuf->Seek(pos, mode);
}

off_t wxMemoryOutputStream::OnSysTell() const
{
  return m_o_streambuf->Tell();
}

size_t wxMemoryOutputStream::CopyTo(char *buffer, size_t len) const
{
  if (!buffer)
    return 0;

  if (len > GetSize())
    len = GetSize();

  memcpy(buffer, m_o_streambuf->GetBufferStart(), len);
  return len;
}

#endif
