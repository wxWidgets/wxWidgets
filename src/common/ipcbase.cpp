/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/ipcbase.cpp
// Purpose:     IPC base classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif

#include "wx/ipcbase.h"

IMPLEMENT_CLASS(wxServerBase, wxObject)
IMPLEMENT_CLASS(wxClientBase, wxObject)
IMPLEMENT_CLASS(wxConnectionBase, wxObject)

wxConnectionBase::wxConnectionBase(void *buffer, size_t bytes)
    : m_buffer((char *)buffer),
      m_buffersize(bytes),
      m_deletebufferwhendone(false),
      m_connected(true)
{
  if ( buffer == NULL )
  { // behave like next constructor
    m_buffersize = 0;
    m_deletebufferwhendone = true;
  }
}

wxConnectionBase::wxConnectionBase()
    : m_buffer(NULL),
      m_buffersize(0),
      m_deletebufferwhendone(true),
      m_connected(true)
{
}

wxConnectionBase::wxConnectionBase(const wxConnectionBase& copy)
    : wxObject(),
      m_buffer(copy.m_buffer),
      m_buffersize(copy.m_buffersize),
      m_deletebufferwhendone(false),
      m_connected(copy.m_connected)

{
  // copy constructor would require ref-counted pointer to buffer
  wxFAIL_MSG( _T("Copy constructor of wxConnectionBase not implemented") );
}


wxConnectionBase::~wxConnectionBase(void)
{
  if ( m_deletebufferwhendone && m_buffer )
    delete m_buffer;
}

void *wxConnectionBase::GetBufferAtLeast( size_t bytes )
{
  if ( m_buffersize >= bytes )
    return m_buffer;
  else
  {  // need to resize buffer
    if ( m_deletebufferwhendone )
    { // we're in charge of buffer, increase it
      if ( m_buffer )
        delete m_buffer;
      m_buffer = new char[bytes];
      m_buffersize = bytes;
      return m_buffer;
    } // user-supplied buffer, fail
    else
      return NULL;
  }
}
