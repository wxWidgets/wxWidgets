// /////////////////////////////////////////////////////////////////////////////
// Name:       sndau.cpp
// Purpose:    wxMMedia Sun Audio File Codec
// Author:     Guilhem Lavaux
// Created:    1998
// Updated:
// Copyright:  (C) 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sndau.h"
#endif

#include "mmriff.h"
#include "sndfile.h"
#include "sndau.h"

#define AU_ISDN_ULAW 1
#define AU_PCM_8BITS 2
#define AU_PCM_16BITS 3
#define AU_ADPCM 23

wxSndAuCodec::wxSndAuCodec()
  : wxSndFileCodec()
{
}

wxSndAuCodec::wxSndAuCodec(wxInputStream& s, bool preload, bool seekable)
  : wxSndFileCodec(s, preload, seekable)
{
}

wxSndAuCodec::wxSndAuCodec(wxOutputStream& s, bool seekable)
  : wxSndFileCodec(s, seekable)
{
}

wxSndAuCodec::wxSndAuCodec(const wxString& fname)
  : wxSndFileCodec(fname)
{
}

wxSndAuCodec::~wxSndAuCodec()
{
}

wxUint32 wxSndAuCodec::PrepareToPlay()
{
  wxString id;
  char temp_buf[5];
  int offset, srate, codec, ch_count;
  size_t len;

  m_istream->Read(temp_buf, 4);
  temp_buf[4] = 0;

  id = temp_buf;
  if (id != ".snd") {
    m_mmerror = wxMMFILE_INVALID;
    return 0;
  }

#define READ_BE_32(i) \
m_istream->Read(temp_buf, 4); \
i = (unsigned long)temp_buf[0] << 24; \
i |= (unsigned long)temp_buf[1] << 16; \
i |= (unsigned long)temp_buf[2] << 8; \
i |= (unsigned long)temp_buf[3];

  READ_BE_32(offset);
  READ_BE_32(len);
  READ_BE_32(codec);
  READ_BE_32(srate);
  READ_BE_32(ch_count);

  m_sndformat.SetSampleRate(srate);
  m_sndformat.SetChannels(ch_count);
  switch (codec) {
  case AU_ISDN_ULAW:
    ChangeCodec(WXSOUND_ULAW);
    break;
  case AU_PCM_8BITS:
    ChangeCodec(WXSOUND_PCM);
    m_sndformat.SetByteOrder(wxSND_SAMPLE_LE);
    m_sndformat.SetSign(wxSND_SAMPLE_SIGNED);
    break;
  case AU_PCM_16BITS:
    ChangeCodec(WXSOUND_PCM);
    m_sndformat.SetByteOrder(wxSND_SAMPLE_LE);
    m_sndformat.SetSign(wxSND_SAMPLE_SIGNED);
    break;
  case AU_ADPCM:
    ChangeCodec(WXSOUND_ADPCM);
    break;
  }
  return len;
}

bool wxSndAuCodec::OnNeedData(char *buf, wxUint32 size)
{
  return m_istream->Read(buf, size).LastError();
}

bool wxSndAuCodec::OnWriteData(char *buf, wxUint32 size)
{
  return m_ostream->Write(buf, size).LastError();
}

bool wxSndAuCodec::PrepareToRecord(wxUint32 file_size)
{
  return FALSE;
}
