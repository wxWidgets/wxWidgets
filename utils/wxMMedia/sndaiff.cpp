////////////////////////////////////////////////////////////////////////////////
// Name:       sndaiff.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    February 1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sndaiff.h"
#endif

#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h"
#endif
#include "wx/datstrm.h"
#include "sndaiff.h"
#include "sndpcm.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#define READ_DATA(n) m_istream->Read(tmp_buf,n)
#define WRITE_DATA(n) m_ostream->Write(tmp_buf,n)

#define READ_STRING(s,n) \
  READ_DATA(n); \
  tmp_buf[n] = 0; \
  s = tmp_buf;

#define WRITE_STRING(s,n) WRITE_DATA((const char *)s, n)

#define READ32(i) \
  READ_DATA(4); \
  i = (unsigned long)tmp_buf[3] | \
      ((unsigned long)tmp_buf[2] << 8) | \
      ((unsigned long)tmp_buf[1] << 16) | \
      ((unsigned long)tmp_buf[0] << 24);

#define WRITE32(i) \
  tmp_buf[3] = i & 0xFF; \
  tmp_buf[2] = (i >> 8) & 0xFF; \
  tmp_buf[1] = (i >> 16) & 0xFF; \
  tmp_buf[0] = (i >> 24) & 0xFF; \
  WRITE_DATA(4);

#define READ16(i) \
  READ_DATA(2); \
  i = (unsigned short)tmp_buf[1] | \
      ((unsigned short)tmp_buf[0] << 8);

wxSndAiffCodec::wxSndAiffCodec()
  : wxSndFileCodec()
{
  m_sndtime.hours = -1;
}

wxSndAiffCodec::wxSndAiffCodec(wxOutputStream& s, bool seekable)
  : wxSndFileCodec(s, seekable)
{
  if (!seekable)
    CacheIO();
  m_sndtime.hours = -1;
}

wxSndAiffCodec::wxSndAiffCodec(wxInputStream& s, bool preload, bool seekable)
  : wxSndFileCodec(s, preload, seekable)
{
  if (!seekable)
    CacheIO();
  m_sndtime.hours = -1;
}

wxSndAiffCodec::wxSndAiffCodec(const wxString& fname)
  : wxSndFileCodec(fname)
{
  m_sndtime.hours = -1;
}

wxUint32 wxSndAiffCodec::PrepareToPlay()
{
  char tmp_buf[5];
  wxString chunk_name;

  wxSndFileCodec::m_mmerror = wxMMFILE_INVALID;

  READ_STRING(chunk_name, 4);
  if (chunk_name != "FORM")
    return 0;
  m_istream->SeekI(4, wxFromCurrent);

  READ_STRING(chunk_name, 4);
  if (chunk_name != "AIFF" && chunk_name != "AIFC")
    return 0;

  // To check whether the file is good
  m_spos = 0;
  m_slen = 0;
  m_sndformat.SetSampleRate(0);
  while (!m_spos || !m_sndformat.GetSampleRate()) {
    READ_STRING(chunk_name, 4);
    READ32(m_chunksize);
    
    if (chunk_name == "SSND")
      ParseSSND();
    if (chunk_name == "COMM")
      ParseCOMM();
    else
      m_istream->SeekI(m_chunksize, wxFromCurrent);
  }

  m_sndmode = wxSND_OUTPUT;

  wxUint32 sec1 = m_slen / m_sndformat.GetCodec()->GetByteRate(),
           sec2 = sec1 % 3600;

  m_sndtime.hours = sec1 / 3600;
  m_sndtime.minutes = sec2 / 60;
  m_sndtime.seconds = sec2 % 60;

  wxSndFileCodec::m_mmerror = wxMMFILE_NOERROR;

  m_istream->SeekI(m_spos, wxFromStart);
  wxSndFileCodec::m_fstate = wxSFILE_PREPARED_TO_PLAY;

  return m_slen;
}

void wxSndAiffCodec::ParseCOMM()
{
  wxDataInputStream data_s(*m_istream);
  char tmp_buf[10];
  wxUint16 channels;
  wxUint32 srate, num_samples;
  wxUint16 bps;

  READ16(channels);
  READ32(num_samples);
  READ16(bps);

  srate  = (wxUint32)data_s.ReadDouble();
  m_sndformat.SetSampleRate(srate);
  m_sndformat.SetBps(bps);
  m_sndformat.SetChannels(channels);
  m_sndformat.SetByteOrder(wxSND_SAMPLE_BE);
  m_sndformat.SetSign(wxSND_SAMPLE_UNSIGNED);
  ChangeCodec(WXSOUND_PCM);

  m_istream->SeekI(m_chunksize-18, wxFromCurrent);
}

void wxSndAiffCodec::ParseSSND()
{
  wxDataInputStream data_s(*m_istream);
  char tmp_buf[10];

  READ32(m_spos);
  m_istream->SeekI(4, wxFromCurrent);

  m_slen = m_chunksize - m_spos;
  m_spos += m_istream->TellI();
}

wxSndAiffCodec::~wxSndAiffCodec()
{
}

bool wxSndAiffCodec::OnNeedData(char *buf, wxUint32 size)
{
  m_istream->Read(buf, size);
  return TRUE;
}

bool wxSndAiffCodec::OnWriteData(char *buf, wxUint32 size)
{
  return ( !(m_ostream->Write(buf, size).LastError()) );
}

void wxSndAiffCodec::WriteCOMM()
{
/*
  wxDataOutputStream data_s(*m_ostream);
  char tmp_buf[10];
  wxUint16 channels;
  wxUint32 srate, num_samples;
  wxUint16 bps;

  m_chunksize = 18;
  WRITE32(m_chunksize);
  channels = m_sndformat.GetChannels();
  srate = m_sndformat.GetSampleRate();
  bps = m_sndformat.GetBps();

  WRITE16(channels);
  WRITE32(num_samples);
  WRITE16(bps);

  data_s.WriteDouble((double)srate);

  m_sndformat.SetByteOrder(wxSND_SAMPLE_BE);
  m_sndformat.SetSign(wxSND_SAMPLE_UNSIGNED);
  ChangeCodec(WXSOUND_PCM);
*/
}

void wxSndAiffCodec::WriteSSND(wxUint32 fsize)
{
/*
  char tmp_buf[10];

  WRITE32(m_spos);
//   WRITE32(dummy ??);

  m_slen = m_chunksize - m_spos;
  m_spos += m_istream->TellI();
*/
}


bool wxSndAiffCodec::PrepareToRecord(wxUint32 m_fsize)
{
  wxUint32 total_size = m_fsize + 0;
  char tmp_buf[10];

  m_ostream->Write("FORM", 4);
  WRITE32(total_size);

  m_ostream->Write("AIFF", 4);

  WriteCOMM();
  WriteSSND(m_fsize);

  return TRUE;
}

void wxSndAiffCodec::SetFile(wxInputStream& s, bool preload, bool seekable)
{
  wxMMediaFile::SetFile(s, preload, seekable);
  if (!seekable)
    CacheIO();
}

void wxSndAiffCodec::SetFile(wxOutputStream& s, bool seekable)
{
  wxMMediaFile::SetFile(s, seekable);
  if (!seekable)
    CacheIO();
}
