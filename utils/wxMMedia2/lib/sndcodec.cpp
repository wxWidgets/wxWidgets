// --------------------------------------------------------------------------
// Name: sndcodec.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndcodec.cpp"
#endif

#include <wx/wxprec.h>
#include "sndbase.h"
#include "sndcodec.h"

wxSoundStreamCodec::wxSoundStreamCodec(wxSoundStream& snd_io)
 : m_sndio(&snd_io)
{
}

wxSoundStreamCodec::~wxSoundStreamCodec()
{
}

bool wxSoundStreamCodec::StartProduction(int evt)
{
  return m_sndio->StartProduction(evt);
}

bool wxSoundStreamCodec::StopProduction()
{
  return m_sndio->StopProduction();
}

void wxSoundStreamCodec::SetDuplexMode(bool duplex)
{
  m_sndio->SetDuplexMode(duplex);
}
