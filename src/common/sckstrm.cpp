/////////////////////////////////////////////////////////////////////////////
// Name:        sckstrm.h
// Purpose:     wxSocket*Stream
// Author:      Guilhem Lavaux
// Modified by:
// Created:     17/07/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sckstrm.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#if wxUSE_SOCKETS && wxUSE_STREAMS

#include "wx/stream.h"
#include "wx/socket.h"
#include "wx/sckstrm.h"

// ---------------------------------------------------------------------------
// wxSocketOutputStream
// ---------------------------------------------------------------------------

wxSocketOutputStream::wxSocketOutputStream(wxSocketBase& s)
  : m_o_socket(&s)
{
}

wxSocketOutputStream::~wxSocketOutputStream()
{
}

wxOutputStream& wxSocketOutputStream::Write(const void *buffer, size_t size)
{
  m_lastcount = m_o_socket->Write((const char *)buffer, size).LastCount();
  return *this;
}

size_t wxSocketOutputStream::OnSysWrite(const void *buffer, size_t size)
{
  return m_o_socket->Write((const char *)buffer, size).LastCount();
}

// ---------------------------------------------------------------------------
// wxSocketInputStream
// ---------------------------------------------------------------------------

wxSocketInputStream::wxSocketInputStream(wxSocketBase& s)
 : m_i_socket(&s)
{
}

wxSocketInputStream::~wxSocketInputStream()
{
}

wxInputStream& wxSocketInputStream::Read(void *buffer, size_t size)
{
  m_lastcount = m_i_socket->Read((char *)buffer, size).LastCount();
  return *this;
}

size_t wxSocketInputStream::OnSysRead(void *buffer, size_t size)
{
  return m_i_socket->Read((char *)buffer, size).LastCount();
}

// ---------------------------------------------------------------------------
// wxSocketStream
// ---------------------------------------------------------------------------

wxSocketStream::wxSocketStream(wxSocketBase& s)
  : wxSocketInputStream(s), wxSocketOutputStream(s)
{
}

wxSocketStream::~wxSocketStream()
{
}

#endif
  // wxUSE_STREAMS && wxUSE_SOCKETS
