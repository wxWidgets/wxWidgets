// --------------------------------------------------------------------------
// Name: sndbase.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndbase.cpp"
#endif

#include <wx/wxprec.h>
#include "sndbase.h"


// ---------------------------------------------------------------------------
// wxSoundFormatBase
// ---------------------------------------------------------------------------

wxSoundFormatBase::wxSoundFormatBase()
{
}

wxSoundFormatBase::~wxSoundFormatBase()
{
}

wxSoundFormatBase *wxSoundFormatBase::Clone() const
{
  return NULL;
}

bool wxSoundFormatBase::operator!=(const wxSoundFormatBase& frmt2) const
{
  return (GetType() != frmt2.GetType());
}

// ---------------------------------------------------------------------------
// wxSoundStream
// ---------------------------------------------------------------------------

wxSoundStream::wxSoundStream()
{
  int i;

  m_sndformat = NULL;
  m_handler = NULL;
  m_snderror = wxSOUND_NOERR;
  m_lastcount = 0;
  for (i=0;i<2;i++)
    m_callback[i] = NULL;
}

wxSoundStream::~wxSoundStream()
{
  if (m_sndformat)
    delete m_sndformat;
}

// SetSoundFormat returns TRUE when the format can be handled.
bool wxSoundStream::SetSoundFormat(const wxSoundFormatBase& format)
{
  if (m_sndformat)
    delete m_sndformat;

  m_sndformat = format.Clone();
  return TRUE;
}

// Register a callback for a specified async event.
void wxSoundStream::Register(int evt, wxSoundCallback cbk, char *cdata)
{
  int c;

  switch (evt) {
  case wxSOUND_INPUT:
    c = 0;
    break;
  case wxSOUND_OUTPUT:
    c = 1;
    break;
  default:
    return;
  }
  m_callback[c] = cbk;
  m_cdata[c] = cdata;
}

void wxSoundStream::OnSoundEvent(int evt)
{
  int c;

  if (m_handler) {
    m_handler->OnSoundEvent(evt);
    return;
  }

  switch (evt) {
  case wxSOUND_INPUT:
    c = 0;
    break;
  case wxSOUND_OUTPUT:
    c = 1;
    break;
  default:
    return;
  }
  if (m_callback[c])
    m_callback[c](this, evt, m_cdata[c]);
}
