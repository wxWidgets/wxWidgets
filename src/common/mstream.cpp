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
#include <stdlib.h>
#include <wx/stream.h>
#include <wx/mstream.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// ----------------------------------------------------------------------------
// wxMemoryStreamBase
// ----------------------------------------------------------------------------
wxMemoryStreamBase::wxMemoryStreamBase()
{
  m_buffer = NULL;
  m_iolimit = 0;
  m_persistent = FALSE;
  m_length = 0;
}

wxMemoryStreamBase::~wxMemoryStreamBase()
{
  if (!m_persistent && m_buffer)
   free(m_buffer);
}

bool wxMemoryStreamBase::ChangeBufferSize(size_t new_size)
{
  if (m_iolimit == 1)
    return FALSE;

  m_length = new_size;
  if (!m_buffer)
    m_buffer = (char *)malloc(m_length);
  else
    m_buffer = (char *)realloc(m_buffer, m_length);

  return (m_buffer != NULL);
}

// ----------------------------------------------------------------------------
// wxMemoryInputStream
// ----------------------------------------------------------------------------

wxMemoryInputStream::wxMemoryInputStream(const char *data, size_t len)
{
  m_persistent = TRUE;
  m_length = len;
  m_buffer = (char *)data; // It's bad.
  m_position_i = 0;
  m_lastread = 0;
  m_eof = FALSE;
  m_iolimit = 1;

  m_i_streambuf->SetBufferIO(0);
}

wxMemoryInputStream::~wxMemoryInputStream()
{
}

size_t wxMemoryInputStream::DoRead(void *buffer, size_t size)
{
  if (m_iolimit == 2) {
    m_eof = TRUE;
    return 0; 
  }
  if (m_position_i+size > m_length)
    size = m_length-m_position_i;

  memcpy((void *)((unsigned long)buffer+m_position_i), m_buffer, size);
  m_position_i += size;

  return size;
}

off_t wxMemoryInputStream::DoSeekInput(off_t pos, wxSeekMode mode)
{
  if (m_iolimit == 2)
    return 0;

  switch (mode) {
  case wxFromStart:
    if ((size_t)pos > m_length)
      return m_position_i;
    return (m_position_i = pos);

  case wxFromCurrent:
    if ((size_t)(m_position_i+pos) > m_length)
      return m_position_i;

    return (m_position_i += pos);

  case wxFromEnd:
    if ((size_t)(m_length-pos) > m_length)
      return m_position_i;

    return (m_position_i = m_length-pos);
  }

  return m_position_i;
}

// ----------------------------------------------------------------------------
// wxMemoryOutputStream
// ----------------------------------------------------------------------------

wxMemoryOutputStream::wxMemoryOutputStream(char *data, size_t len)
{
  m_persistent = FALSE;
  m_buffer = data;
  m_length = len;
  m_position_o = 0;
  m_lastwrite = 0;
  m_bad = FALSE;
  m_iolimit = 2;

  m_o_streambuf->SetBufferIO(0);
}

wxMemoryOutputStream::~wxMemoryOutputStream()
{
  Sync();
}

size_t wxMemoryOutputStream::DoWrite(const void *buffer, size_t size)
{
  if (m_iolimit == 1) {
    m_bad = TRUE;
    return 0;
  }
  
  if (m_position_o+size > m_length)
    if (!ChangeBufferSize(m_position_o+size)) {
      m_bad = TRUE;
      return 0;
    }

  memcpy(m_buffer+m_position_o, buffer, size);
  m_position_o += size;

  return size;
}

off_t wxMemoryOutputStream::DoSeekOutput(off_t pos, wxSeekMode mode)
{
  if (m_iolimit == 1)
    return 0;

  switch (mode) {
  case wxFromStart:
    if ((size_t)pos > m_length)
      return m_position_o;
    return (m_position_o = pos);

  case wxFromCurrent:
    if ((size_t)(m_position_o+pos) > m_length)
      return m_position_o;

    return (m_position_o += pos);

  case wxFromEnd:
    if ((size_t)(m_length-pos) > m_length)
      return m_position_o;

    return (m_position_o = m_length-pos);
  }

  return m_position_o;
}

// ----------------------------------------------------------------------------
// wxMemoryStream
// ----------------------------------------------------------------------------

wxMemoryStream::wxMemoryStream(char *data, size_t len)
  : wxMemoryInputStream(NULL, 0), wxMemoryOutputStream(NULL, 0)
{
  m_persistent = FALSE;
  m_buffer = data;
  m_length = len;
  m_iolimit = 0;
}

wxMemoryStream::~wxMemoryStream()
{
}
