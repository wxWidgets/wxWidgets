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
  g72x_init_state(codec_state);
}

wxSoundAdpcmCodec::~wxSoundAdpcmCodec()
{
}

int wxSoundAdpcmCodec::GetBits(int nbits)
{
  unsigned int mask;
  int bits;

  if (bits_waiting == 0)
    current_byte = m_in_sound->GetChar();

  mask = (1 << nbits) - 1;
  bits = current_byte & mask;
  current_byte >>= nbits;
  return bits;
}


void wxSoundAdpcmCodec::Decode()
{
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
}

void wxSoundMulawCodec::Encode()
{
}

size_t wxSoundMulawCodec::GetByteRate() const
{
  return m_srate;
}

wxSoundDataFormat wxSoundMulawCodec::GetPreferredFormat(int WXUNUSED(no)) const
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
