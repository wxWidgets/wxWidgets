////////////////////////////////////////////////////////////////////////////////
// Name:       snduss.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "snduss.h"
#endif

#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "wx/app.h"
#include "wx/utils.h"

#define WXMMEDIA_INTERNAL
#include "snduss.h"
#include "sndfrmt.h"

wxUssSound::wxUssSound()
  : wxSound(),
    m_srate(0), m_bps(0), m_stereo(0),
    m_mode(wxSND_OTHER_IO),
    m_stop_thrd(TRUE), m_sleeping(FALSE)
{
  m_fd = -1;
  m_ussformat.SetCodecNo(WXSOUND_PCM);
  m_ussformat.SetSign(wxSND_SAMPLE_SIGNED);
  m_ussformat.SetByteOrder(wxSND_SAMPLE_LE);

  m_sndbuf = new wxStreamBuffer(wxStreamBuffer::read_write);
  m_sndbuf->Flushable(FALSE);
  m_sndbuf->Fixed(TRUE);
}

wxUssSound::~wxUssSound()
{
  if (!m_stop_thrd) {
    m_stop_thrd = TRUE;
    if (m_sleeping) {
      m_sleep_mtx.Lock();
      m_sleep_cond.Signal();
      m_sleep_mtx.Unlock();
    }
    while (IsAlive())
      Yield();
  }

  if (m_fd != -1)
    close(m_fd);
}

bool wxUssSound::Wakeup(wxSndBuffer& WXUNUSED(buf))
{
  printf("Waking up (wxUssSound::Wakeup) ...\n");
  if (m_stop_thrd) {
    m_stop_thrd = FALSE; 
    Entry();
//    wxThread::Create();
  }

  if (m_sleeping) {
    m_sleep_mtx.Lock();
    m_sleep_cond.Signal();
    m_sleep_mtx.Unlock();
  }

  return TRUE;
}

void wxUssSound::StopBuffer(wxSndBuffer& buf)
{
  buf.HardLock();
  buf.Set(wxSND_BUFSTOP);
  buf.HardUnlock();
  while (buf.IsSet(wxSND_BUFSTOP))
    wxYield();
//    usleep(0);
}

void wxUssSound::USS_Sleep()
{
  bool ret;

  printf("Asleeping ...\n");
  m_sleeping = TRUE;
  m_sleep_mtx.Lock();
  ret = m_sleep_cond.Wait(m_sleep_mtx, 10, 0);
  m_sleep_mtx.Unlock();
  m_sleeping = FALSE;
  
  printf("Waking up ...\n");
  if (!ret)
    m_stop_thrd = TRUE;
}

bool wxUssSound::DoInput(wxSndBuffer *buf)
{
  wxUint32 bufsize;
  wxSoundCodec *codec = buf->GetCurrentCodec();

  m_sndbuf->ResetBuffer();

  bufsize = codec->Available();
  if (bufsize > m_max_bufsize)
    bufsize = m_max_bufsize;

  if (!bufsize) {
    buf->Clear(wxSND_BUFLOCKED | wxSND_BUFREADY);
    return false;
  }
  read(m_fd, m_sndbuf->GetBufferStart(), bufsize);
  codec->Encode();

  return true;
}

bool wxUssSound::DoOutput(wxSndBuffer *buf)
{
  wxSoundCodec *codec = buf->GetCurrentCodec();

  m_sndbuf->ResetBuffer();

  if (!codec->Available()) {
    buf->Clear(wxSND_BUFLOCKED | wxSND_BUFREADY);
    return FALSE;
  }
  codec->Decode();
  write(m_fd, m_sndbuf->GetBufferStart(), m_sndbuf->GetIntPosition());

  // Well ... it's not accurate ! :-|
  buf->OnBufferOutFinished();

  return TRUE;
}

bool wxUssSound::InitBuffer(wxSndBuffer *buf)
{
  wxSoundCodec *codec;

  if (!OnSetupDriver(*buf, buf->GetMode())) {
    if (buf->IsNotSet(wxSND_BUFREADY))
      return FALSE;
  }

  codec = buf->GetCurrentCodec();
  switch (m_mode) {
  case wxSND_INPUT:
    codec->SetInStream(m_sndbuf);
    codec->InitIO(m_ussformat);
    codec->InitMode(wxSoundCodec::ENCODING);
    break;
  case wxSND_OUTPUT:
    codec->SetOutStream(m_sndbuf);
    codec->InitIO(m_ussformat);
    codec->InitMode(wxSoundCodec::DECODING);
    break;
  }
  return TRUE;
}

void *wxUssSound::Entry()
{
  wxNode *node;
  wxSndBuffer *buf;

  node = m_buffers.First();
  if (!node) {
    m_stop_thrd = FALSE;
    return NULL;
  }

  buf = (wxSndBuffer *)node->Data();
  InitBuffer(buf);

  while (!m_stop_thrd) {
    buf->HardLock();
    if (buf->IsSet(wxSND_BUFSTOP)) {
      buf->HardUnlock();
      goto sound_clean_buffer;
    }
    switch(m_mode) {
    case wxSND_INPUT:
      if (!DoInput(buf))
        goto sound_clean_buffer;
      break;
    case wxSND_OUTPUT:
      if (!DoOutput(buf))
        goto sound_clean_buffer;
      break;
    case wxSND_DUPLEX:
    case wxSND_OTHER_IO:
      goto sound_clean_buffer;
      break;
    }
    buf->HardUnlock();
    continue;
  sound_clean_buffer:
    buf->GetCurrentCodec()->ExitMode();
    delete node;
    node = m_buffers.First();
    if (!node)
      USS_Sleep();
    if (node)
      buf = (wxSndBuffer *)node->Data();
  }
  return NULL;
}

bool wxUssSound::OnSetupDriver(wxSndBuffer& buf, wxSndMode WXUNUSED(mode))
{
  wxSoundDataFormat format;
  wxSoundCodec *codec;

  codec = buf.GetCurrentCodec();
  format = codec->GetPreferredFormat(WXSOUND_PCM);

  if ((format.GetSampleRate() != m_srate) ||
      (format.GetBps() != m_bps) ||
      (format.GetStereo() != m_stereo)) {

    if (!SetupSound(format.GetSampleRate(), format.GetBps(),
		    format.GetStereo())) {
      m_buffers.DeleteObject(&buf);
      buf.Clear(wxSND_BUFLOCKED | wxSND_BUFREADY);
      buf.SetError(wxSND_CANTSET);
      return FALSE;
    }
    m_mode = wxSND_OTHER_IO;
  }

  if (buf.GetMode() != m_mode) {
    m_mode = buf.GetMode();
    return FALSE;
  }

  return TRUE;
}

wxUint32 wxUssSound::GetNbFragments()
{
  struct audio_buf_info frag_info;

  ioctl(m_fd, SNDCTL_DSP_GETOSPACE, &frag_info);

  return frag_info.fragstotal;
}

wxUint32 wxUssSound::GetFragmentSize()
{
  return m_max_bufsize;
}

bool wxUssSound::SetupSound(wxUint16 srate, wxUint8 bps, bool stereo)
{
  int tmp;
  unsigned long tmp_ul;

  if (m_fd != -1) {
    delete m_sndbuf;
    fsync(m_fd);
    close(m_fd);
  }

  m_fd = open("/dev/dsp", O_RDWR);

  tmp = stereo;
  if (ioctl(m_fd, SNDCTL_DSP_STEREO, &tmp) < 0)
    return FALSE;
  m_stereo = tmp;
 
  tmp_ul = srate;
  if (ioctl(m_fd, SNDCTL_DSP_SPEED, &tmp_ul) < 0)
    return FALSE;
  m_srate = tmp_ul;

  tmp = bps;
  if (ioctl(m_fd, SNDCTL_DSP_SAMPLESIZE, &tmp) < 0)
    return FALSE;
  m_bps = tmp;

  ioctl(m_fd, SNDCTL_DSP_GETBLKSIZE, &tmp);
  m_max_bufsize = tmp;
  m_sndbuf->SetBufferIO(m_max_bufsize);

  m_ussformat.SetBps(m_bps);
  m_ussformat.SetChannels((m_stereo) ? 2 : 1);
  m_ussformat.SetSampleRate(m_srate);

  return TRUE;
}
