#ifdef __GNUG__
#pragma implementation "sndfrmt.h"
#endif
#include "sndsnd.h"
#include "sndfrmt.h"
#include "sndpcm.h"

// ----------------------------------------------------------------------------
// wxSoundDataFormat
// ----------------------------------------------------------------------------

wxSoundDataFormat::wxSoundDataFormat()
{
  m_srate = 22050;
  m_bps = 8;
  m_channels = 1;
  m_codno = 1;
  m_codec = NULL;
  m_codchange = FALSE;
  m_codcreate = TRUE;
}

wxSoundDataFormat::wxSoundDataFormat(const wxSoundDataFormat& format)
{
  m_srate = format.m_srate;
  m_bps = format.m_bps;
  m_channels = format.m_channels;
  m_codno = format.m_codno;
  m_sign = format.m_sign;
  m_byteorder = format.m_byteorder;
  m_codchange = FALSE;
  m_codcreate = TRUE;
  m_codec = NULL;
}

wxSoundDataFormat::~wxSoundDataFormat()
{
  wxDELETE(m_codec);
}

void wxSoundDataFormat::SetChannels(int channels)
{
  m_channels = channels;
}

void wxSoundDataFormat::SetBps(int bps)
{
  m_bps = bps;
  CodecChange();
}

void wxSoundDataFormat::SetSign(int sign)
{
  m_sign = sign;
  CodecChange();
}

void wxSoundDataFormat::SetByteOrder(int byteorder)
{
  m_byteorder = byteorder;
  CodecChange();
}

void wxSoundDataFormat::SetCodecNo(int codno)
{
  m_codno = codno;
  m_codchange = TRUE;
  CodecChange();
}

wxSoundCodec *wxSoundDataFormat::GetCodec()
{
  if (!m_codcreate)
    return NULL;

  if (m_codchange) 
    wxDELETE(m_codec);
  
  if (m_codec)
    return m_codec;

  m_codchange = FALSE;
  m_codec = wxSoundCodec::Get(m_codno);

  return m_codec;
}

void wxSoundDataFormat::CodecChange()
{
  wxSoundCodec *codec = GetCodec();

  if (!codec)
    return;

  switch (m_codno) {
  case WXSOUND_PCM: {
      wxSoundPcmCodec *pcm_codec = (wxSoundPcmCodec *)codec;

      pcm_codec->SetBits(m_bps);
      pcm_codec->SetByteOrder(m_byteorder);
      pcm_codec->SetSign(m_sign);
      break;
    }
  default:
      break;
  }
}

wxSoundDataFormat& wxSoundDataFormat::operator =(const wxSoundDataFormat& format)
{
  wxDELETE(m_codec);

  m_srate = format.m_srate;
  m_bps = format.m_bps;
  m_channels = format.m_channels;
  m_codno = format.m_codno;
  m_sign = format.m_sign;
  m_byteorder = format.m_byteorder;

  return *this;
}

bool wxSoundDataFormat::operator ==(const wxSoundDataFormat& format) const
{
  if (m_codno != format.m_codno || m_srate != format.m_srate ||
      m_bps != format.m_bps || m_channels != format.m_channels)
    return FALSE;

  if (m_codno == WXSOUND_PCM &&
      (m_sign != format.m_sign || m_byteorder != format.m_byteorder))
    return FALSE;

  return TRUE;
}

// ----------------------------------------------------------------------------
// wxSoundCodec
// ----------------------------------------------------------------------------

#include "sndpcm.h"
//#include "sndadpcm.h"
//#include "sndalaw.h"
#include "sndmulaw.h"

static wxClassInfo *l_sound_formats[] = {
      NULL,
      CLASSINFO(wxSoundPcmCodec),
      NULL, //  CLASSINFO(wxSoundAdpcmCodec),
      NULL,
      NULL,
      NULL,
      NULL, // CLASSINFO(wxSoundAlawCodec),
      NULL  // CLASSINFO(wxSoundMulawCodec)
};

static int l_nb_formats = WXSIZEOF(l_sound_formats);

wxSoundCodec::wxSoundCodec()
{
  m_in_sound = NULL;
  m_out_sound = NULL;
  m_init = TRUE;
  m_chain_codec = NULL;
}

wxSoundCodec::~wxSoundCodec()
{
}

void wxSoundCodec::InitIO(const wxSoundDataFormat& format)
{
  m_io_format = format;
}

void wxSoundCodec::InitMode(int mode)
{
  wxStreamBuffer *buf_snd;

  m_mode = (mode == 0) ? ENCODING : DECODING;
  if (!m_chain_codec) {
    if (mode == ENCODING) {
      m_out_sound = new wxStreamBuffer(*this, wxStreamBuffer::write);
      m_out_sound->SetBufferIO(1024);
    } else {
      m_in_sound = new wxStreamBuffer(*this, wxStreamBuffer::read);
      m_in_sound->SetBufferIO(1024);
    }
  }
  if (m_chain_codec) {
    if (m_chain_before) {
      m_chain_codec->SetInStream(m_in_sound);
      buf_snd = new wxStreamBuffer(wxStreamBuffer::read_write);
      buf_snd->Fixed(FALSE);
      m_chain_codec->SetOutStream(buf_snd);
      m_chain_codec->Decode();
      buf_snd->Seek(0, wxFromStart);
      m_in_sound = buf_snd;
    } else {
      buf_snd = new wxStreamBuffer(wxStreamBuffer::read_write);
      buf_snd->Fixed(FALSE);

      m_chain_codec->SetInStream(buf_snd);
      m_chain_codec->SetOutStream(m_out_sound);
      m_out_sound = buf_snd;

      buf_snd->Seek(0, wxFromStart);
    }
  }
}

void wxSoundCodec::ExitMode()
{
  if (m_chain_codec) {
    if (m_chain_before) {
      delete m_in_sound;
      m_in_sound = m_chain_codec->GetInStream();
    } else {
      delete m_out_sound;
      m_out_sound = m_chain_codec->GetOutStream();
    }
  }
}

bool wxSoundCodec::ChainCodecBefore(wxSoundDataFormat& format)
{
  m_chain_codec = format.GetCodec();

  if (!m_chain_codec)
    return FALSE;

  m_chain_before = TRUE;
  return TRUE;
}

bool wxSoundCodec::ChainCodecAfter(wxSoundDataFormat& format)
{
  m_chain_codec = format.GetCodec();

  if (!m_chain_codec)
    return FALSE;

  m_chain_before = FALSE;
  return TRUE;
}

void wxSoundCodec::CopyToOutput()
{
  m_out_sound->Write(m_in_sound);
}

size_t wxSoundCodec::Available()
{
  return m_io_sndbuf->Available();
}

size_t wxSoundCodec::OnSysRead(void *buffer, size_t bsize)
{
  wxUint32 s = bsize;
  m_io_sndbuf->OnNeedOutputData((char *)buffer, s);
  return bsize;
}

size_t wxSoundCodec::OnSysWrite(const void *buffer, size_t bsize)
{
  wxUint32 s = bsize;
  m_io_sndbuf->OnBufferInFinished((char *)buffer, s);
  return bsize;
}

wxSoundCodec *wxSoundCodec::Get(int no)
{
  if (no < 0 || no >= l_nb_formats)
    return NULL;

  if (!l_sound_formats[no])
    return NULL;

  return (wxSoundCodec *)l_sound_formats[no]->CreateObject();
}
