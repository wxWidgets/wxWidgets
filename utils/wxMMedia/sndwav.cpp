////////////////////////////////////////////////////////////////////////////////
// Name:       sndwav.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    February 1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sndwav.h"
#endif

#ifdef WX_PRECOMP
#include "wx_prec.h"
#else
#include "wx/wx.h"
#endif
#include "sndwav.h"
#include "sndfrmt.h"
#include "sndpcm.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxSndWavCodec::wxSndWavCodec()
  : wxSndFileCodec(), riff_codec()
{
  m_sndtime.hours = -1;
}

wxSndWavCodec::wxSndWavCodec(wxOutputStream& s, bool seekable)
  : wxSndFileCodec(s, seekable)
{
  if (!seekable)
    CacheIO();
  riff_codec = wxRiffCodec(*m_ostream);
  m_sndtime.hours = -1;
}

wxSndWavCodec::wxSndWavCodec(wxInputStream& s, bool preload, bool seekable)
  : wxSndFileCodec(s, preload, seekable)
{
  if (!seekable)
    CacheIO();

  riff_codec = wxRiffCodec(*m_istream);
  m_sndtime.hours = -1;
}

wxSndWavCodec::wxSndWavCodec(const wxString& fname)
  : wxSndFileCodec(fname)
{
  riff_codec = wxRiffCodec(*m_istream);
  m_sndtime.hours = -1;
}

wxUint32 wxSndWavCodec::PrepareToPlay()
{
  if (!riff_codec.RiffReset(RIFF_READ))
    return 0;

  if (!riff_codec.FindChunk("RIFF", TRUE)) {
    wxSndFileCodec::m_mmerror = wxMMFILE_INVALID;
    return 0;
  }

  char tmp_buf[5];
  riff_codec.ReadData(tmp_buf, 4);
  tmp_buf[4] = 0;
  if (wxString("WAVE") != tmp_buf) {
    wxSndFileCodec::m_mmerror = wxMMFILE_INVALID;
    return 0;
  }
  if (!riff_codec.FindChunk("fmt ", TRUE))
    return 0;

  riff_codec.Read16(wav_hdr.format);
  riff_codec.Read16(wav_hdr.channels);
  riff_codec.Read32(wav_hdr.sample_fq);
  riff_codec.Read32(wav_hdr.byte_p_sec);
  riff_codec.Read16(wav_hdr.byte_p_spl);
  riff_codec.Read16(wav_hdr.bits_p_spl);

  if (!riff_codec.FindChunk("data"))
    return 0;

  m_sndmode = wxSND_OUTPUT;
  ChangeCodec(wav_hdr.format);

  m_sndformat.SetSampleRate(wav_hdr.sample_fq);
  m_sndformat.SetBps(wav_hdr.bits_p_spl);
  m_sndformat.SetChannels(wav_hdr.channels);

  if (wav_hdr.format == WXSOUND_PCM) {
    m_sndformat.SetSign(wxSND_SAMPLE_SIGNED);
    m_sndformat.SetByteOrder(wxSND_SAMPLE_LE);
  }

  wxUint32 sec1 = riff_codec.GetChunkLength() / wav_hdr.byte_p_sec,
           sec2 = sec1 % 3600;

  m_sndtime.hours = sec1 / 3600;
  m_sndtime.minutes = sec2 / 60;
  m_sndtime.seconds = sec2 % 60;

  wxSndFileCodec::m_mmerror = wxMMFILE_NOERROR;
  wxSndFileCodec::m_fstate = wxSFILE_PREPARED_TO_PLAY;

  return riff_codec.GetChunkLength();
}

wxSndWavCodec::~wxSndWavCodec()
{
}

bool wxSndWavCodec::OnNeedData(char *buf, wxUint32 size)
{
  return riff_codec.ReadData(buf, size);
}

bool wxSndWavCodec::OnWriteData(char *buf, wxUint32 size)
{
  return riff_codec.WriteData(buf, size);
}

bool wxSndWavCodec::PrepareToRecord(wxUint32 m_fsize)
{
  wxUint32 total_size;

  if (!riff_codec.RiffReset(RIFF_WRITE))
    return FALSE;

  total_size = 16 + sizeof(wav_hdr) + m_fsize;

  if (!riff_codec.CreateChunk("RIFF", total_size))
    return FALSE;
  riff_codec.WriteData("WAVE", 4);
  if (!riff_codec.CreateChunk("fmt ", sizeof(wav_hdr)))
    return FALSE;

  wav_hdr.format = 1; // PCM_WAV_FORMAT
  wav_hdr.channels = m_sndformat.GetChannels();
  wav_hdr.sample_fq = m_sndformat.GetSampleRate();
  wav_hdr.byte_p_spl = (m_sndformat.GetBps() / 8) * wav_hdr.channels;
  wav_hdr.byte_p_sec = m_sndformat.GetCodec()->GetByteRate();
  wav_hdr.bits_p_spl = m_sndformat.GetBps();
  ChangeCodec(WXSOUND_PCM);

  if (wav_hdr.format == WXSOUND_PCM) {
    m_sndformat.SetSign(wxSND_SAMPLE_SIGNED);
    m_sndformat.SetByteOrder(wxSND_SAMPLE_LE);
  }

  riff_codec.Write16(wav_hdr.format);
  riff_codec.Write16(wav_hdr.channels);
  riff_codec.Write32(wav_hdr.sample_fq);
  riff_codec.Write32(wav_hdr.byte_p_sec);
  riff_codec.Write16(wav_hdr.byte_p_spl);
  riff_codec.Write16(wav_hdr.bits_p_spl);

  if (!riff_codec.CreateChunk("data", m_fsize))
    return FALSE;
  return TRUE;
}

void wxSndWavCodec::SetFile(wxInputStream& s, bool preload, bool seekable)
{
  wxMMediaFile::SetFile(s, preload, seekable);
  if (!seekable)
    CacheIO();

  riff_codec.SetFile((seekable) ? s : *m_istream);
}

void wxSndWavCodec::SetFile(wxOutputStream& s, bool seekable)
{
  wxMMediaFile::SetFile(s, seekable);
  if (!seekable)
    CacheIO();

  riff_codec.SetFile((seekable) ? s : *m_ostream);
}
