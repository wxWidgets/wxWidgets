#ifdef __GNUG__
#pragma implementation "sndpcm.h"
#endif
#include "sndsnd.h"
#include "sndpcm.h"

#define WX_BIG_ENDIAN 0

wxSoundPcmCodec::wxSoundPcmCodec()
  : wxSoundCodec()
{
  m_orig_format.SetCodecCreate(FALSE);
  m_orig_format.SetCodecNo(WXSOUND_PCM);
}

wxSoundPcmCodec::~wxSoundPcmCodec()
{
}

size_t wxSoundPcmCodec::GetByteRate() const
{
  return (m_orig_format.GetBps()/8)*
          m_orig_format.GetSampleRate()*
          m_orig_format.GetChannels();
}

wxSoundDataFormat wxSoundPcmCodec::GetPreferredFormat(int codec) const
{
  wxSoundDataFormat prefFormat;

  prefFormat = m_orig_format;
  return prefFormat;
}

// ---------------------------------------------------------------------------
// Main part of the decoder
// ---------------------------------------------------------------------------

void wxSoundPcmCodec::Decode()
{
  InitMode(DECODING);
  if (m_io_format == m_orig_format) {
    CopyToOutput();
    ExitMode();
    return;
  }

  // Swap bytes
  switch (m_io_format.GetBps()) {
  case 8:
    InputSign8();
    break;
  case 16:
    InputSwapAndSign16();
    break;
  case 32:
  case 64:
  default:
    break;
  }
  ExitMode();
}

// ---------------------------------------------------------------------------
// Change the sign of a 8-bit sample.

#define GET() (m_in_sound->GetChar())
#define PUT(c) (m_out_sound->PutChar(c))

void wxSoundPcmCodec::InputSign8()
{
  unsigned char signer = 0;

  if (m_io_format.GetSign() != m_orig_format.GetSign())
    signer = 128;

  while (StreamOk())
    PUT(GET() + signer);
}

// ---------------------------------------------------------------------------
// Swap bytes and change the sign of a 16-bit sample.

void wxSoundPcmCodec::InputSwapAndSign16()
{
  unsigned short signer1 = 0, signer2 = 0;
  bool swap = (m_io_format.GetByteOrder() != m_orig_format.GetByteOrder());
  register char temp, temp2;

  if (m_io_format.GetSign() != m_orig_format.GetSign()) {
    if (m_io_format.GetByteOrder() == wxSND_SAMPLE_LE)
      signer2 = 0x80;
    else
      signer1 = 0x80;
  }

  if (swap) {
    while (StreamOk()) {
      temp = GET();
      temp2 = GET();
      PUT(temp2 ^ signer2);
      if (!StreamOk()) {
        m_in_sound->WriteBack(temp);
        m_in_sound->WriteBack(temp2);
        break;
      }
      PUT(temp ^ signer1);
    }
  } else {
    while (StreamOk()) {
      temp = GET();
      PUT(temp ^ signer1);
      if (!StreamOk()) {
        m_in_sound->WriteBack(temp);
        break;
      }
      PUT(GET() ^ signer2);
    }
  }
}

// ---------------------------------------------------------------------------
// Encoder part.
// ---------------------------------------------------------------------------

void wxSoundPcmCodec::OutputSign8()
{
  unsigned char signer = 0;

  if (m_io_format.GetSign() != m_orig_format.GetSign())
    signer = 128;

  while (StreamOk())
    PUT((char)(GET() + signer));
}

// ---------------------------------------------------------------------------

void wxSoundPcmCodec::OutputSwapAndSign16()
{
  bool swap = (m_io_format.GetByteOrder() != m_orig_format.GetByteOrder());
  unsigned short signer1 = 0, signer2 = 0;
  register char temp, temp2;

  if (m_io_format.GetSign() != m_orig_format.GetSign())
    if (m_io_format.GetByteOrder() == wxSND_SAMPLE_LE)
      signer1 = 0x80;
    else
      signer2 = 0x80;

  if (swap) {
    while (StreamOk()) {
      temp = GET();
      temp2 = GET();
      PUT(temp2 ^ signer1);
      if (!StreamOk()) {
        m_in_sound->WriteBack(temp);
        m_in_sound->WriteBack(temp2);
        break;
      }
      PUT(temp ^ signer2);
    }
  } else {
    while (StreamOk()) {
      temp = GET();
      temp2 = GET();
      PUT(temp ^ signer1);
      if (!StreamOk()) {
        m_in_sound->WriteBack(temp);
        break;
      }
      PUT(temp2 ^ signer2);
    }
  }

}

// ---------------------------------------------------------------------------

void wxSoundPcmCodec::Encode()
{
  InitMode(ENCODING);
  if (m_io_format == m_orig_format) {
    CopyToOutput();
    ExitMode();
    return;
  }

  // Swap bytes
  switch (m_io_format.GetBps()) {
  case 8:
    OutputSign8();
    break;
  case 16:
    OutputSwapAndSign16();
    break;
  case 32:
  case 64:
  default:
    break;
  }
  ExitMode();
}
