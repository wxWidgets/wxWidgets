////////////////////////////////////////////////////////////////////////////////
// Name:       sndfile.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "sndfile.h"
#endif

#include "mmedia.h"
#include "sndfile.h"
#ifdef WX_PRECOMP
#include <wx/wxprec.h>
#else
#include <wx/wx.h>
#endif
#include <wx/module.h>
#include "sndfrmt.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxSndFileCodec::wxSndFileCodec()
  : wxMMediaFile(), wxSndBuffer(),
    m_fstate(wxSFILE_STOPPED)
{
}

wxSndFileCodec::wxSndFileCodec(wxOutputStream& s, bool seekable)
  : wxMMediaFile(s, seekable),
    wxSndBuffer(),
    m_fstate(wxSFILE_STOPPED)
{
}

wxSndFileCodec::wxSndFileCodec(wxInputStream& s, bool preload, bool seekable)
  : wxMMediaFile(s, preload, seekable),
    wxSndBuffer(), 
    m_fstate(wxSFILE_STOPPED)
{
}

wxSndFileCodec::wxSndFileCodec(const wxString& fname)
  : wxMMediaFile(fname), wxSndBuffer(),
    m_fstate(wxSFILE_STOPPED)
{
}

wxSndFileCodec::~wxSndFileCodec()
{
}

void wxSndFileCodec::Play(wxSound& snd)
{
  if (m_fstate != wxSFILE_STOPPED || IsSet(wxSND_BUFLOCKED))
    return;

  if (!(m_fsize = PrepareToPlay()))
    return;

  m_fpos = 0;
  m_fstate = wxSFILE_PLAYING;

  Set(wxSND_BUFREADY | wxSND_KEEPQUEUED);
  snd.QueueBuffer(*this);
}

void wxSndFileCodec::Stop(wxSound& snd)
{
  if (m_fstate == wxSFILE_STOPPED)
    return;

  snd.UnqueueBuffer(*this);
  Clear(wxSND_BUFREADY | wxSND_KEEPQUEUED);
  m_fstate = wxSFILE_STOPPED;
}

void wxSndFileCodec::Record(wxSound& snd,
                            const wxSoundDataFormat& format,
                            wxUint32 seconds)
{
  wxUint32 byterate;

  if (m_fstate != wxSFILE_STOPPED)
    return;

  m_sndformat = format;
  byterate = m_sndformat.GetCodec()->GetByteRate();

  m_fsize = seconds*byterate;
  if (!PrepareToRecord(m_fsize))
    return;
  if (IsSet(wxSND_BUFLOCKED))
    return;
 
  wxUint32 sec1 = m_fsize / byterate,
           sec2 = sec1 % 3600;

  m_sndtime.hours = sec1 / 3600;
  m_sndtime.minutes = sec2 / 60;
  m_sndtime.seconds = sec2 % 60;

  m_fdone = m_fpos = 0;
  m_fstate = wxSFILE_RECORDING;

  m_sndmode = wxSND_INPUT;

  Set(wxSND_BUFREADY | wxSND_KEEPQUEUED);
  snd.QueueBuffer(*this);
}

void wxSndFileCodec::OnNeedOutputData(char *data, wxUint32& size)
{
  wxUint32 datas_left = m_fsize-m_fpos;

  if (m_fstate != wxSFILE_PLAYING) {
    size = 0;
    return;
  }

  if (!datas_left) {
    size = 0;
    m_fpos = 0;
    m_fstate = wxSFILE_STOPPED;
    Clear(wxSND_KEEPQUEUED);
    return;
  }

  if (size > datas_left)
    size = datas_left;

  if (!OnNeedData(data, size)) {
    size = 0;
    m_fpos = 0;
    m_fstate = wxSFILE_STOPPED;
    Clear(wxSND_KEEPQUEUED);
    return;
  }

  m_fpos += size;
}

void wxSndFileCodec::OnBufferInFinished(char *iobuf, wxUint32& size)
{
  wxUint32 datas_left = m_fsize-m_fdone;

  if (m_fstate != wxSFILE_RECORDING) {
    size = 0;
    return;
  }

  if (!datas_left) {
    size = 0;
    Clear(wxSND_KEEPQUEUED); // To be sure.
    return;
  }

  if (size > datas_left)
    size = datas_left;

  OnWriteData(iobuf, size);
  m_fdone += size;
}

wxMMtime wxSndFileCodec::GetPosition()
{
  wxMMtime mm_time;
  wxUint32 sec1, sec2;
  wxUint32 byterate;

  byterate = m_sndformat.GetCodec()->GetByteRate();

  if (m_fpos && byterate) {
    sec1 = m_fpos / byterate;
    sec2 = sec1 % 3600;
    mm_time.hours = sec1 / 3600;
    mm_time.minutes = sec2 / 60;
    mm_time.seconds = sec2 % 60;
  } else {
    mm_time.hours = 0;
    mm_time.minutes = 0;
    mm_time.seconds = 0;
  }

  return mm_time;
}

wxMMtime wxSndFileCodec::GetLength()
{
  if (m_sndtime.hours == -1 && m_istream)
    PrepareToPlay();

  return m_sndtime;
}

bool wxSndFileCodec::TranslateBuffer(wxSndBuffer& buf)
{
#define TMP_BUFSIZE 10240

  wxUint32 buf_size;
  wxStreamBuffer *tmp_buf;
  wxSoundCodec *codec_in, *codec_out;
  wxSoundDataFormat std_format;

  if (!m_ostream || !buf.RestartBuffer(wxSND_OUTPUT))
    return FALSE;
 
  m_sndformat = buf.GetFormat();
  codec_in = buf.GetCurrentCodec();

  m_fdone = 0;

  if (!PrepareToRecord(m_fsize))
    return FALSE;
 
  codec_out = GetCurrentCodec();
  m_fsize = (int)(((float)buf.GetSize() / codec_in->GetByteRate()) *
             codec_out->GetByteRate());

  if (!PrepareToRecord(m_fsize))
    return FALSE;

  codec_out = GetCurrentCodec();
  codec_in->InitIO(m_sndformat);
  codec_out->InitIO(m_sndformat);

  tmp_buf = new wxStreamBuffer(wxStreamBuffer::read_write);
  tmp_buf->Fixed(TRUE);
  tmp_buf->Flushable(FALSE);
  tmp_buf->SetBufferIO(TMP_BUFSIZE);

  m_fstate = wxSFILE_RECORDING;

  while (m_fdone < m_fsize) {
    tmp_buf->ResetBuffer();
    codec_in->SetOutStream(tmp_buf);
    codec_in->Decode();

    tmp_buf->ResetBuffer();
    codec_out->SetInStream(tmp_buf);
    codec_out->Encode();

    buf.OnBufferOutFinished();
  }
  delete tmp_buf;

  m_fstate = wxSFILE_STOPPED;

  return TRUE;
}

bool wxSndFileCodec::RestartBuffer(wxSndMode mode)
{
  if (IsSet(wxSND_BUFLOCKED))
    return FALSE;

  m_fdone = 0;
  m_fpos = 0;

  if (mode == wxSND_OUTPUT && m_istream) {
    m_fsize = PrepareToPlay();
    m_fstate = wxSFILE_PLAYING;
    return TRUE;
  }
  if (mode == wxSND_INPUT && m_ostream) {
    m_fsize = 0;
    m_fstate = wxSFILE_RECORDING;
    return TRUE;
  }

  return FALSE;
}

wxUint32 wxSndFileCodec::GetSize() const
{
  return m_fsize;
}

wxUint32 wxSndFileCodec::Available() const
{
  return m_fsize-m_fpos; 
}

//
// Simple API
//
static wxSoundDevice *dev_snd = NULL;

bool wxSndFileCodec::StartPlay()
{
  if (!dev_snd)
    dev_snd = new wxSoundDevice;

  Play(*dev_snd);

  return TRUE;
}

void wxSndFileCodec::StopPlay()
{
  if (!dev_snd)
    return;

  Stop(*dev_snd);
  m_fpos = 0;
}

class wxSoundModule : public wxModule {
  DECLARE_DYNAMIC_CLASS(wxSoundModule)
public:
  virtual bool OnInit() { return TRUE; }
  virtual void OnExit() {
    if (dev_snd)
      delete dev_snd;
  }
};

IMPLEMENT_DYNAMIC_CLASS(wxSoundModule, wxModule)
