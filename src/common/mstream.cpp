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
// wxMemoryInputStream
// ----------------------------------------------------------------------------

wxMemoryInputStream::wxMemoryInputStream(const char *data, size_t len)
  : wxInputStream()
{
  m_i_streambuf->SetBufferIO((char *)data, data+len);
  m_i_streambuf->Fixed(TRUE);
}

wxMemoryInputStream::~wxMemoryInputStream()
{
}

char wxMemoryInputStream::Peek()
{
  return m_i_streambuf->GetBufferStart()[m_i_streambuf->GetIntPosition()];
}

// ----------------------------------------------------------------------------
// wxMemoryOutputStream
// ----------------------------------------------------------------------------

wxMemoryOutputStream::wxMemoryOutputStream(char *data, size_t len)
  : wxOutputStream()
{
  if (data)
    m_o_streambuf->SetBufferIO(data, data+len);
  m_o_streambuf->Fixed(TRUE);
}

wxMemoryOutputStream::~wxMemoryOutputStream()
{
}
