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

#include <stdlib.h>
#include <wx/stream.h>
#include <wx/mstream.h>

wxMemoryStreamBase::wxMemoryStreamBase(char *data, size_t length, int iolimit)
{
  m_buffer = data;
  m_iolimit = iolimit;
  m_persistent = FALSE;
  m_length = length;
  m_position_i = m_position_o = 0;
}

wxMemoryStreamBase::~wxMemoryStreamBase()
{
  free(m_buffer);
}

wxInputStream& wxMemoryStreamBase::Read(void *buffer, size_t size)
{
  if (m_iolimit == 2) {
    m_eof = TRUE;
    return *this;
  }
  if (m_position_i+size > m_length)
    size = m_length-m_position_i;

  memcpy((void *)((unsigned long)buffer+m_position_i), m_buffer, size);
  m_position_i += size;
  m_lastread = size;

  return *this;
}

size_t wxMemoryStreamBase::SeekI(int pos, wxWhenceType whence)
{
  if (m_iolimit == 2)
    return 0;

  switch (whence) {
  case wxBeginPosition:
    if ((size_t)pos > m_length)
      return m_position_i;
    return (m_position_i = pos);

  case wxCurrentPosition:
    if ((size_t)(m_position_i+pos) > m_length)
      return m_position_i;

    return (m_position_i += pos);

  case wxEndPosition:
    if ((size_t)(m_length-pos) > m_length)
      return m_position_i;

    return (m_position_i = m_length-pos);
  }

  return m_position_i;
}

wxOutputStream& wxMemoryStreamBase::Write(const void *buffer, size_t size)
{
  if (m_iolimit == 1) {
    m_bad = TRUE;
    return *this;
  }
  
  if (m_position_o+size > m_length)
    if (!ChangeBufferSize(m_position_o+size)) {
      m_bad = TRUE;
      return *this;
    }

  memcpy(m_buffer+m_position_o, buffer, size);
  m_position_o += size;
  m_lastwrite = size;

  return *this;
}

size_t wxMemoryStreamBase::SeekO(int pos, wxWhenceType whence)
{
  if (m_iolimit == 2)
    return 0;

  switch (whence) {
  case wxBeginPosition:
    if ((size_t)pos > m_length)
      return m_position_o;
    return (m_position_o = pos);

  case wxCurrentPosition:
    if ((size_t)(m_position_o+pos) > m_length)
      return m_position_o;

    return (m_position_o += pos);

  case wxEndPosition:
    if ((size_t)(m_length-pos) > m_length)
      return m_position_o;

    return (m_position_o = m_length-pos);
  }

  return m_position_o;
}

bool wxMemoryStreamBase::ChangeBufferSize(size_t new_size)
{
  m_length = new_size;
  if (!m_buffer)
    m_buffer = (char *)malloc(m_length);
  else
    m_buffer = (char *)realloc(m_buffer, m_length);

  return (m_buffer != NULL);
}
