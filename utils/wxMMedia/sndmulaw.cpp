#ifdef __GNUG__
#pragma implementation "sndmulaw.h"
#endif

#include "sndsnd.h"
#include "sndfrmt.h"
#include "sndmulaw.h"
#include "adpcm/g72x.h"

wxSoundMulawCodec::wxSoundMulawCodec()
  : wxSoundCodec()
{
}

wxSoundMulawCodec::~wxSoundMulawCodec()
{
}

void wxSoundMulawCodec::Decode()
{
  int smp;
  wxSoundDataFormat pref_frmt;

  pref_frmt = GetPreferredFormat(0);
  if (m_io_format != pref_frmt)
    ChainCodecAfter(pref_frmt);

  InitMode(DECODING);

  while (!Good()) {
      smp = ulaw2linear(m_in_sound->GetChar());
#ifdef USE_BE_MACH
      m_out_sound->PutChar((smp & 0xff00) >> 8);
      m_out_sound->PutChar(smp & 0xff);
#else
      m_out_sound->PutChar(smp & 0xff);
      m_out_sound->PutChar((smp & 0xff00) >> 8);
#endif
  }
}

void wxSoundMulawCodec::Encode()
{
  int smp;
  wxSoundDataFormat pref_frmt;

  pref_frmt = GetPreferredFormat(0);
  if (m_io_format != pref_frmt)
    ChainCodecBefore(pref_frmt);

  InitMode(ENCODING);

  while (!Good()) {
#ifdef USE_BE_MACH
      smp = ((unsigned short)m_in_sound->GetChar()) << 8;
      smp |= m_in_sound->GetChar() & 0xff;
#else
      smp = m_in_sound->GetChar() & 0xff;
      smp |= ((unsigned short)m_in_sound->GetChar()) << 8;
#endif
      m_out_sound->PutChar(linear2ulaw(smp));
  }
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
