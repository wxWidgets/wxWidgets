// --------------------------------------------------------------------------
// Name: sndpcm.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndpcm.cpp"
#endif

#include "sndbase.h"
#include "sndpcm.h"


wxSoundFormatPcm::wxSoundFormatPcm(wxUint32 srate, wxUint8 bps, 
                                   wxUint16 nchannels, bool sign,
                                   int order) 
 : m_srate(srate), m_bps(bps), m_nchan(nchannels), m_order(order),
   m_signed(sign)
{
}

wxSoundFormatPcm::~wxSoundFormatPcm()
{
}

void wxSoundFormatPcm::SetSampleRate(wxUint32 srate)
{
  m_srate = srate;
}

void wxSoundFormatPcm::SetBPS(wxUint8 bps)
{
  m_bps = bps;
}

void wxSoundFormatPcm::SetChannels(wxUint16 nchannels)
{
  m_nchan = nchannels;
}

void wxSoundFormatPcm::SetOrder(int order)
{
  m_order = order;
}

void wxSoundFormatPcm::Signed(bool sign)
{
  m_signed = sign;
}

wxSoundFormatBase *wxSoundFormatPcm::Clone() const
{
  wxSoundFormatPcm *new_pcm;

  new_pcm = new wxSoundFormatPcm();
  new_pcm->m_srate = m_srate;
  new_pcm->m_bps   = m_bps;
  new_pcm->m_nchan = m_nchan;
  new_pcm->m_order = m_order;
  new_pcm->m_signed= m_signed;

  return new_pcm;
}

wxUint32 wxSoundFormatPcm::GetTimeFromBytes(wxUint32 bytes) const
{
  return (bytes / (m_srate * (m_bps / 8) * m_nchan));
}

wxUint32 wxSoundFormatPcm::GetBytesFromTime(wxUint32 time) const
{
  return (time * (m_srate * (m_bps / 8) * m_nchan));
}

bool wxSoundFormatPcm::operator!=(const wxSoundFormatBase& format) const
{
  wxSoundFormatPcm *format2 = (wxSoundFormatPcm *)&format;

  if (format.GetType() != wxSOUND_PCM)
    return TRUE;

  return ( (m_srate != format2->m_srate) ||
           (m_bps != format2->m_bps) ||
           (m_nchan != format2->m_nchan) ||
           (m_order != format2->m_order) ||
           (m_signed != format2->m_signed) );
}
