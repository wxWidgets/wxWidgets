#ifdef __GNUG__
#pragma implementation "sndmulaw.h"
#endif

#include "sndsnd.h"
#include "sndfrmt.h"
#include "sndadpcm.h"
#include "adpcm/g72x.h"

wxSoundAdpcmCodec::wxSoundAdpcmCodec()
  : wxSoundCodec()
{
  // TODO: For the moment, only 1 channel is supported.
  m_codec_state = new g72x_state;
  g72x_init_state(m_codec_state);
}

wxSoundAdpcmCodec::~wxSoundAdpcmCodec()
{
}

void wxSoundAdpcmCodec::InitWith(const wxSoundDataFormat& format)
{
  m_srate = format.GetSampleRate();
}

int wxSoundAdpcmCodec::GetBits(int nbits)
{
  unsigned int mask;
  int bits;

  if (m_bits_waiting == 0)
    m_current_byte = m_in_sound->GetChar();

  mask = (1 << nbits) - 1;
  bits = m_current_byte & mask;
  m_current_byte >>= nbits;
  m_bits_waiting -= nbits;
  return bits;
}

void wxSoundAdpcmCodec::Decode()
{
  int smp, bits;
  wxSoundDataFormat pref_frmt;

  pref_frmt = GetPreferredFormat(0);
  if (!(m_io_format == pref_frmt))
    ChainCodecAfter(pref_frmt);

  bits = GetBits(4);
  if (m_io_format.GetByteOrder() == wxSND_SAMPLE_LE) {
    while (!StreamOk()) {
      smp = g721_decoder(bits, AUDIO_ENCODING_LINEAR, m_codec_state);
      m_out_sound->PutChar(smp & 0x00ff);
      m_out_sound->PutChar((smp & 0xff00) >> 8);
      bits = GetBits(4);
    }
  } else {
    while (!StreamOk()) {
      smp = g721_decoder(bits, AUDIO_ENCODING_LINEAR, m_codec_state);
      m_out_sound->PutChar((smp & 0xff00) >> 8);
      m_out_sound->PutChar(smp & 0x00ff);
      bits = GetBits(4);
    }
  }
}

void wxSoundAdpcmCodec::Encode()
{
/*
  int smp;
  wxSoundDataFormat pref_frmt;

  pref_frmt = GetPreferredFormat(0);
  if (!(m_io_format == pref_frmt))
    ChainCodecAfter(pref_frmt);

  bits = GetBits(4);
  if (m_io_format.GetByteOrder() == wxSND_SAMPLE_LE) {
    while (!StreamOk()) {
      smp = g721_decoder(bits, AUDIO_ENCODING_LINEAR, codec_state);
      m_out_sound->PutChar(smp & 0x00ff);
      m_out_sound->PutChar((smp & 0xff00) >> 8);
      bits = GetBits(4);
    }
  } else {
    while (!StreamOk()) {
      smp = g721_decoder(bits, AUDIO_ENCODING_LINEAR, codec_state);
      m_out_sound->PutChar((smp & 0xff00) >> 8);
      m_out_sound->PutChar(smp & 0x00ff);
      bits = GetBits(4);
    }
  }
*/
}

size_t wxSoundAdpcmCodec::GetByteRate() const
{
  return (m_io_format.GetSampleRate() * m_io_format.GetChannels()) / 2;
}

wxSoundDataFormat wxSoundAdpcmCodec::GetPreferredFormat(int WXUNUSED(no)) const
{
  wxSoundDataFormat format;

  format.SetCodecNo(WXSOUND_PCM);
  format.SetSampleRate(m_srate);
  format.SetBps(16);
  format.SetChannels(1);
  format.SetSign(wxSND_SAMPLE_SIGNED);
#ifdef USE_BE_MACH
  format.SetByteOrder(wxSND_SAMPLE_BE);
#else
  format.SetByteOrder(wxSND_SAMPLE_LE);
#endif
  return format;
}
