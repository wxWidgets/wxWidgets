#ifdef __GNUG__
#pragma implementation "sndpcm.h"
#endif
#include "sndsnd.h"
#include "sndpcm.h"
#include <dmalloc.h>

#define WX_BIG_ENDIAN 0

wxSoundPcmCodec::wxSoundPcmCodec()
  : wxSoundCodec()
{
  m_orig_format.SetCodecCreate(FALSE);
  m_orig_format.SetCodecNo(1);
  m_char_bool = FALSE;
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
  prefFormat.SetCodecNo(WXSOUND_PCM);
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
#define OUT_ERROR() (out->LastError() == wxStream_NOERROR)
#define IN_ERROR() (in->LastError() == wxStream_NOERROR)

void wxSoundPcmCodec::InputSign8()
{
  unsigned char signer = 0;
  wxStreamBase *in = m_out_sound->Stream(), *out = m_in_sound->Stream();

  if (m_io_format.GetSign() != m_orig_format.GetSign())
    signer = 128;

  while (IN_ERROR() && OUT_ERROR())
    PUT(GET() + signer);

}

// ---------------------------------------------------------------------------
// Swap bytes and change the sign of a 16-bit sample.

void wxSoundPcmCodec::InputSwapAndSign16()
{
  unsigned short signer1 = 0, signer2 = 0;
  wxStreamBase *in = m_out_sound->Stream(), *out = m_in_sound->Stream();
  bool swap = (m_io_format.GetByteOrder() != m_orig_format.GetByteOrder());
  char temp;

  if (m_io_format.GetSign() != m_orig_format.GetSign()) {
    if (m_io_format.GetByteOrder() == wxSND_SAMPLE_LE)
      signer2 = 0x80;
    else
      signer1 = 0x80;
  }

  if (swap) {
    while (IN_ERROR() && OUT_ERROR()) {
      temp = GET() ^ signer1;
      PUT(GET() ^ signer2);
      if (OUT_ERROR()) {
        m_char_bool = TRUE;
        m_char_stack = temp;
        break;
      }
      PUT(temp);
    }
  } else {
    while (IN_ERROR() && OUT_ERROR()) {
      PUT(GET() ^ signer1);
      if (OUT_ERROR()) {
        m_char_bool = TRUE;
        m_char_stack = temp;
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
  wxStreamBase *in = m_out_sound->Stream(), *out = m_in_sound->Stream();
  unsigned char signer = 0;

  if (m_io_format.GetSign() != m_orig_format.GetSign())
    signer = 128;

  while (IN_ERROR() && OUT_ERROR())
    PUT((char)(GET() + signer));
}

// ---------------------------------------------------------------------------

void wxSoundPcmCodec::OutputSwapAndSign16()
{
  bool swap = (m_io_format.GetByteOrder() != m_orig_format.GetByteOrder());
  unsigned short signer1 = 0, signer2 = 0;
  char temp;
  wxStreamBase *in = m_out_sound->Stream(), *out = m_in_sound->Stream();

  if (m_char_bool) {
    PUT(GET());
    PUT(m_char_stack);
    m_char_bool = FALSE;
  }

  if (m_io_format.GetSign() != m_orig_format.GetSign())
    if (m_io_format.GetByteOrder() == wxSND_SAMPLE_LE)
      signer1 = 0x80;
    else
      signer2 = 0x80;

  if (swap) {
    while (IN_ERROR()) {
      temp = GET();
      PUT(GET() ^ signer1);
      if (OUT_ERROR()) {
        m_char_stack = temp ^ signer2;
        m_char_bool = TRUE;
        break;
      }
      PUT(temp ^ signer2);
    }
  } else {
    while (IN_ERROR()) {
      PUT(GET() ^ signer1);
      if (!OUT_ERROR()) {
        m_char_stack = GET() ^ signer2;
        m_char_bool = TRUE;
        break;
      }
      PUT(GET() ^ signer2);
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
