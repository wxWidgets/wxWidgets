// --------------------------------------------------------------------------
// Name: sndulaw.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "sndulaw.cpp"
#endif

#include <wx/wxprec.h>
#include "sndbase.h"
#include "sndfile.h"
#include "sndpcm.h"
#include "sndulaw.h"
#include "sndg72x.h"
#include "g72x.h"

// --------------------------------------------------------------------------
// wxSoundFormatG72X
// --------------------------------------------------------------------------

wxSoundFormatG72X::wxSoundFormatG72X()
  : m_srate(22050)
{
}

wxSoundFormatG72X::~wxSoundFormatG72X()
{
}

void wxSoundFormatG72X::SetSampleRate(wxUint32 srate)
{
  m_srate = srate;
}

wxUint32 wxSoundFormatG72X::GetSampleRate() const
{
  return m_srate;
}

void wxSoundFormatG72X::SetG72XType(wxSoundG72XType type)
{
  m_g72x_type = type;
}

wxSoundFormatBase *wxSoundFormatG72X::Clone() const
{
  wxSoundFormatG72X *g72x = new wxSoundFormatG72X();

  g72x->m_srate = m_srate;
  g72x->m_g72x_type = m_g72x_type;
  return g72x;
}

wxUint32 wxSoundFormatG72X::GetTimeFromBytes(wxUint32 bytes) const
{
  int n_bits;

  switch (m_g72x_type) {
  case wxSOUND_G721:
    n_bits = 4;
    break;
  case wxSOUND_G723_24:
    n_bits = 3;
    break;
  case wxSOUND_G723_40:
    n_bits = 5;
    break;
  default:
    n_bits = 0;
    break;
  }
  return (wxUint32)((bytes / m_srate) * ((float)n_bits / 8));
}

wxUint32 wxSoundFormatG72X::GetBytesFromTime(wxUint32 time) const
{
  int n_bits;

  switch (m_g72x_type) {
  case wxSOUND_G721:
    n_bits = 4;
    break;
  case wxSOUND_G723_24:
    n_bits = 3;
    break;
  case wxSOUND_G723_40:
    n_bits = 5;
    break;
  default:
    n_bits = 0;
  }
  return (wxUint32)(time * (m_srate * ((float)n_bits / 8)));
}

bool wxSoundFormatG72X::operator !=(const wxSoundFormatBase& frmt2) const
{
  wxSoundFormatG72X *g72x = (wxSoundFormatG72X *)&frmt2;

  if (frmt2.GetType() != wxSOUND_G72X)
    return TRUE;

  return (g72x->m_srate != m_srate || g72x->m_g72x_type != m_g72x_type);
}

// --------------------------------------------------------------------------
// wxSoundStreamG72X
// --------------------------------------------------------------------------
wxSoundStreamG72X::wxSoundStreamG72X(wxSoundStream& sndio)
 : wxSoundStreamCodec(sndio)
{
  // PCM converter
  m_router = new wxSoundRouterStream(sndio);
  m_state  = new struct g72x_state;
  g72x_init_state(m_state);
}

wxSoundStreamG72X::~wxSoundStreamG72X()
{
  delete m_router;
}

wxSoundStream& wxSoundStreamG72X::Read(void *buffer, wxUint32 len)
{
  return *this;
}

wxSoundStream& wxSoundStreamG72X::Write(const void *buffer, wxUint32 len)
{
  wxUint16 *old_linear;
  register wxUint16 *linear_buffer;
  register wxUint32 countdown = len;
  register wxUint32 real_len;

  real_len = (wxUint32)(len * ((float)m_n_bits / 8));

  old_linear = linear_buffer = new wxUint16[real_len];

  // Bad, we override the const
  m_io_buffer = (wxUint8 *)buffer;
  m_current_b_pos = 0;

  while (countdown != 0) {
    *linear_buffer++ = m_decoder(GetBits(), AUDIO_ENCODING_LINEAR, m_state);
    countdown--;
  }
  m_lastcount = len;

  m_router->Write(old_linear, real_len);

  delete[] old_linear;

  return *m_router;
}

bool wxSoundStreamG72X::SetSoundFormat(const wxSoundFormatBase& format)
{
  if (format.GetType() != wxSOUND_G72X) {
    m_snderror = wxSOUND_INVFRMT;
    return FALSE;
  }

  wxSoundFormatPcm pcm;
  wxSoundFormatG72X *g72x;

  wxSoundStreamCodec::SetSoundFormat(format);

  g72x = (wxSoundFormatG72X *)m_sndformat;

  pcm.SetSampleRate(g72x->GetSampleRate());
  pcm.SetBPS(16);
  pcm.SetChannels(1);
  pcm.Signed(TRUE);
  pcm.SetOrder(wxBYTE_ORDER);

  switch (g72x->GetG72XType()) {
  case wxSOUND_G721:
    m_n_bits  = 4;
    m_coder   = g721_encoder;
    m_decoder = g721_decoder;
    break;
  case wxSOUND_G723_24:
    m_n_bits  = 3;
    m_coder   = g723_24_encoder;
    m_decoder = g723_24_decoder;
    break;
  case wxSOUND_G723_40:
    m_n_bits  = 5;
    m_coder   = g723_40_encoder;
    m_decoder = g723_40_decoder;
    break;
  }

  m_router->SetSoundFormat(pcm);

  return TRUE;
}

#define BYTE_SIZE 8

wxUint8 wxSoundStreamG72X::GetBits()
{
  register wxUint8 bits;

  if (m_current_b_pos < m_n_bits) {
    register wxUint8 b_left;

    // TRANSLATE the mask
    m_current_mask >>= m_current_b_pos;

    // GET the last bits: 0001..1
    bits = (m_current_byte & m_current_mask) << (m_n_bits - m_current_b_pos);
    
    // GEN: 1. n times .1000
    b_left = BYTE_SIZE-m_n_bits;
    m_current_mask = ((1 << m_n_bits) - 1) << b_left;
 
    // GET the next byte
    m_current_byte = *m_io_buffer++;
 
    register wxUint8 tmp_mask;

    // COMPUTE a new temporary mask to get the last bits
    b_left = m_n_bits - b_left;
    tmp_mask = (1 << b_left) - 1;
    // TRANSLATE the old mask to get ready for the next time
    m_current_mask >>= b_left;

    // COMPUTE the new bit position
    b_left = BYTE_SIZE - b_left; 
    m_current_b_pos = b_left;
    tmp_mask <<= b_left;
 
    // GET the last bits
    bits |= (m_current_byte & tmp_mask) >> b_left;
  } else {
    m_current_mask >>= m_n_bits;
    m_current_b_pos -= m_n_bits;
    bits = (m_current_byte & m_current_mask) >> m_current_b_pos;
  }
  return bits;
}

void wxSoundStreamG72X::PutBits(wxUint8 bits)
{
  if (m_current_b_pos < m_n_bits) {
    register wxUint8 tmp_mask;
    register wxUint8 diff;

    diff = m_n_bits - m_current_b_pos;
    // Pack bits and put the byte in the buffer
    m_current_byte |= bits >> diff;
    *m_io_buffer++ = m_current_byte;

    // Gen a mask
    tmp_mask = ~((1 << diff) - 1);

    m_current_b_pos = BYTE_SIZE - (m_n_bits - m_current_b_pos);

    m_current_byte = (bits & (tmp_mask)) << m_current_b_pos;
  } else {
    m_current_b_pos -= m_n_bits;
    bits           <<= m_current_b_pos;
    m_current_byte |= bits;
  }
}
