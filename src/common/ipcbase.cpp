/////////////////////////////////////////////////////////////////////////////
// Name:        ipcbase.cpp
// Purpose:     IPC base classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "ipcbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#include "wx/ipcbase.h"

IMPLEMENT_CLASS(wxServerBase, wxObject)
IMPLEMENT_CLASS(wxClientBase, wxObject)
IMPLEMENT_CLASS(wxConnectionBase, wxObject)

wxConnectionBase::wxConnectionBase(wxChar *buffer, int size)
  : m_buffer(buffer), m_buffersize(size),
    m_deletebufferwhendone(false), m_connected(true)
{
  if ( buffer == (wxChar *)NULL )
  { // behave like next constructor
    m_buffersize = 0;
    m_deletebufferwhendone = true;
  }
}

wxConnectionBase::wxConnectionBase()
  : m_buffersize(0), m_buffer(NULL), m_deletebufferwhendone(true),
    m_connected(true)
{
}

wxConnectionBase::wxConnectionBase(wxConnectionBase& copy)
  : m_buffer(copy.m_buffer), m_buffersize(copy.m_buffersize),
    m_deletebufferwhendone(false), m_connected(copy.m_connected)
{
  // copy constructor would require ref-counted pointer to buffer
  wxFAIL_MSG( _T("Copy constructor of wxConnectionBase not implemented") );
}


wxConnectionBase::~wxConnectionBase(void)
{
  if ( m_deletebufferwhendone && m_buffer )
   delete m_buffer;
}

wxChar *wxConnectionBase::GetBufferAtLeast( size_t bytes )
{
  if ( m_buffersize >= bytes )
    return m_buffer;
  else
  {  // need to resize buffer
    if ( m_deletebufferwhendone )
    { // we're in charge of buffer, increase it
      if ( !m_buffer )
        delete m_buffer;
      m_buffer = new wxChar[bytes];
      m_buffersize = bytes;
      return m_buffer;
    } // user-supplied buffer, fail
    else
      return NULL;
  }
}

