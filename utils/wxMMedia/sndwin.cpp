////////////////////////////////////////////////////////////////////////////////
// Name:       sndwin.cpp
// Purpose:    wxMMedia
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
////////////////////////////////////////////////////////////////////////////////
#ifdef WX_PRECOMP
#include "wx/wxprec.h"
#else
#include "wx/wx.h"
#endif
#include <wx/msw/private.h>

#define WXMMEDIA_INTERNAL
#include "sndwin.h"

#define MMD_WIN_IO_BSIZE 16384

#include <mmsystem.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

wxSndWinFragment::wxSndWinFragment(wxSound& io_drv)
  : wxFragmentBuffer(io_drv)
{
}

wxSndWinFragment::~wxSndWinFragment(void)
{
}

void wxSndWinFragment::AllocIOBuffer(void)
{
  wxWinSound *w_snd = (wxWinSound *) m_iodrv;
  wxUint8 i;

  m_maxoq = 5;
  m_maxiq = 5;

  m_lstoptrs = new wxFragBufPtr[m_maxoq];
  m_lstiptrs = new wxFragBufPtr[m_maxiq];

  for (i=0;i<m_maxoq;i++) {
    m_lstoptrs[i].buffers = new wxList();
    m_lstoptrs[i].state = wxBUFFER_FREE;

    w_snd->PrepareHeader(m_lstoptrs[i], wxSND_OUTPUT);
  }

  for (i=0;i<m_maxiq;i++) {
    m_lstiptrs[i].size = MMD_WIN_IO_BSIZE;
    m_lstiptrs[i].ptr = 0;
    m_lstiptrs[i].buffers = new wxList();
    m_lstiptrs[i].state = wxBUFFER_FREE;

    w_snd->PrepareHeader(m_lstiptrs[i], wxSND_INPUT);
  }
}

void wxSndWinFragment::FreeIOBuffer(void)
{
  wxWinSound *w_snd = (wxWinSound *)m_iodrv;
  wxUint8 i;

  if (!m_lstoptrs && !m_lstiptrs)
    return;

  for (i=0;i<m_maxoq;i++) {
    w_snd->UnprepareHeader(m_lstoptrs[i], wxSND_OUTPUT);
    delete m_lstoptrs[i].buffers;
  }

  for (i=0;i<m_maxiq;i++) {
    w_snd->UnprepareHeader(m_lstiptrs[i], wxSND_INPUT);
    delete m_lstiptrs[i].buffers;
  }

  delete[] m_lstoptrs;
  delete[] m_lstiptrs;

  m_lstoptrs = m_lstiptrs = NULL;
  m_maxoq = m_maxiq = 0;
}

void wxSndWinFragment::WaitForAll()
{
  bool buf_busy = TRUE;
  int i;

  m_dontq = TRUE;

  while (buf_busy) {
    buf_busy = FALSE;

    for (i=0;i<m_maxoq;i++) {
      if (m_lstoptrs[i].state == wxBUFFER_FFILLED) {
        buf_busy = TRUE;
        break;
      }
    }
    wxYield();
  }

  m_dontq = FALSE;
  FreeBufToFree(TRUE);
}

bool wxSndWinFragment::OnBufferFilled(wxFragBufPtr *ptr, wxSndMode mode)
{
  wxSndWinInfo *info = (wxSndWinInfo *)ptr->user_data;
  wxWinSound *w_snd = (wxWinSound *)m_iodrv;
  MMRESULT result;

  switch (mode) {
  case wxSND_INPUT:
    result = waveInAddBuffer(w_snd->internal->devin_id, info->hdr,
                             sizeof(WAVEHDR));
    break;
  case wxSND_OUTPUT:
    result = waveOutWrite(w_snd->internal->devout_id, info->hdr,
                          sizeof(WAVEHDR));
    printf("WINOUT: result=%d\n", result);
    break;
  }
  return TRUE;
}

wxWinSound::wxWinSound(void)
  : wxSound(),
    fragments(*this)
{
  internal = new wxWinSoundInternal;
  internal->devout_id = 0;
  internal->devin_id = 0;
  internal->sndWin = 0;

  wout_opened = FALSE;
  win_opened = FALSE;
  curr_o_srate = (wxUint32)-1;
  curr_o_bps = (wxUint8)-1;
  curr_o_stereo = (bool)-1;
  curr_i_srate = (wxUint32)-1;
  curr_i_bps = (wxUint8)-1;
  curr_i_stereo = (bool)-1;
}

wxWinSound::~wxWinSound(void)
{
  int i;

  fragments.WaitForAll();

  if (wout_opened)
    waveOutReset(internal->devout_id);
  if (win_opened)
    waveInReset(internal->devout_id);

  fragments.FreeIOBuffer();

  if (wout_opened)
    waveOutClose(internal->devout_id);
  if (win_opened)
    waveInClose(internal->devin_id);

  if (internal->sndWin)
    ::DestroyWindow(internal->sndWin);

  delete internal;
}

bool wxWinSound::Wakeup(wxSndBuffer& buf)
{
  if (!Reopen(buf, FALSE)) {
    buf.Clear(wxSND_BUFLOCKED);
    return FALSE;
  }

  fragments.OnBufferFinished(NULL);
  return TRUE;
}

void wxWinSound::PrepareHeader(wxFragmentBuffer::wxFragBufPtr& frag,
                               wxSndMode mode)
{
  wxSndWinInfo *info;
  WAVEHDR *hdr;

  if ((mode == wxSND_INPUT && !win_opened) ||
      (mode == wxSND_OUTPUT && !wout_opened))
    return;

  info = new wxSndWinInfo;

  info->h_data = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, MMD_WIN_IO_BSIZE);
  info->h_hdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(WAVEHDR));

  info->data = (char *)GlobalLock(info->h_data);
  hdr = info->hdr = (WAVEHDR *)GlobalLock(info->h_hdr);

  memset(hdr, 0, sizeof(*hdr));
  hdr->lpData = info->data;
  hdr->dwBufferLength = frag.size;
  hdr->dwUser = (DWORD)&frag;
  hdr->dwFlags = WHDR_DONE;

  if (mode == wxSND_INPUT) {
    MMRESULT result = waveInPrepareHeader(internal->devin_id, hdr,
                                          sizeof(WAVEHDR));

    if (result != MMSYSERR_NOERROR)
      wxExit();
  } else {
    MMRESULT result = waveOutPrepareHeader(internal->devout_id, hdr,
                                           sizeof(WAVEHDR));
    if (result != MMSYSERR_NOERROR)
      wxExit();
  }

  frag.sndbuf = new wxStreamBuffer();
  frag.sndbuf->SetBufferIO(info->data, info->data + MMD_WIN_IO_BSIZE);
  frag.user_data = (char *)info;
}

void wxWinSound::UnprepareHeader(wxFragmentBuffer::wxFragBufPtr& frag,
                                 wxSndMode mode)
{
  wxSndWinInfo *info = (wxSndWinInfo *)frag.user_data;

  if ((mode == wxSND_INPUT && !win_opened) ||
      (mode == wxSND_OUTPUT && !wout_opened))
    return;

  MMRESULT result;

  if (mode == wxSND_INPUT) {
    result = waveInUnprepareHeader(internal->devin_id, info->hdr, sizeof(*info->hdr));
  } else {
    result = waveOutUnprepareHeader(internal->devout_id, info->hdr, sizeof(*info->hdr));
  }

  delete frag.sndbuf;

  printf("unprepare = %d\n", result);

  GlobalUnlock(info->h_hdr);
  GlobalUnlock(info->h_data);

  GlobalFree(info->h_hdr);
  GlobalFree(info->h_data);

  delete info;
}

extern char wxCanvasClassName[];

LRESULT APIENTRY _EXPORT wxSoundHandlerWndProc(HWND hWnd, UINT message,
                 WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case MM_WOM_DONE: {
    wxWinSound *snd_drv = (wxWinSound *)GetWindowLong(hWnd, GWL_USERDATA);
    WAVEHDR *hdr = (WAVEHDR *)lParam;
    wxFragmentBuffer::wxFragBufPtr *buf =
            (wxFragmentBuffer::wxFragBufPtr *)hdr->dwUser;

    // To be sure ...
    hdr->dwFlags |= WHDR_DONE;

    snd_drv->fragments.OnBufferFinished(buf);
    break;
  }
  case MM_WOM_OPEN:
    printf("wave Open ack\n");
    break;
  case MM_WOM_CLOSE:
    printf("wave Close ack\n");
    break;
  default:
    // TODO: Useful ?
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return (LRESULT)0;
}

void wxWinSound::StopBuffer(wxSndBuffer& buf)
{
  buf.HardLock();
  buf.Set(wxSND_BUFSTOP);
  fragments.AbortBuffer(buf);
  buf.HardUnlock();

  while (buf.IsSet(wxSND_BUFSTOP))
    wxYield();
}

bool wxWinSound::Reopen(wxSndBuffer& buf, bool force)
{
  WAVEFORMATEX wformat;

  if ((buf.GetSampleRate() != curr_o_srate) ||
      (buf.GetBps() != curr_o_bps) ||
      (buf.GetStereo() != curr_o_stereo) ||
      (buf.GetMode() != curr_mode))
    force = TRUE;

  if (force) {
    wxUint32 *curr_srate =
       (buf.GetMode() == wxSND_OUTPUT) ? &curr_o_srate : &curr_i_srate;
    wxUint8 *curr_bps =
       (buf.GetMode() == wxSND_OUTPUT) ? &curr_o_bps : &curr_i_bps;
    bool *curr_stereo =
       (buf.GetMode() == wxSND_OUTPUT) ? &curr_o_stereo : &curr_i_stereo;

    fragments.WaitForAll();
    fragments.FreeIOBuffer();
 
    if (!internal->sndWin) {
        FARPROC proc = MakeProcInstance((FARPROC)wxSoundHandlerWndProc, wxGetInstance());

        internal->sndWin = ::CreateWindow(wxCanvasClassName, NULL, 0,
		                0, 0, 0, 0, NULL, (HMENU) NULL,
                                wxGetInstance(), 0);

        ::SetWindowLong(internal->sndWin, GWL_WNDPROC,
                        (LONG)proc);
	::SetWindowLong(internal->sndWin, GWL_USERDATA, (LONG) this);
    }

    if (wout_opened) {
      waveOutClose(internal->devout_id);
      wout_opened = FALSE;
    }
    if (win_opened) {
      waveInClose(internal->devin_id);
      win_opened = FALSE;
    }

    *curr_srate = buf.GetSampleRate();
    *curr_bps = buf.GetBps();
    *curr_stereo = buf.GetStereo();
    wformat.wFormatTag = WAVE_FORMAT_PCM;
    wformat.nChannels = curr_o_stereo+1;

    wformat.nSamplesPerSec = curr_o_srate;
    wformat.nBlockAlign = curr_o_bps / 8 * wformat.nChannels;
    wformat.nAvgBytesPerSec =
                wformat.nSamplesPerSec * wformat.nBlockAlign;
    wformat.wBitsPerSample = curr_o_bps;
    wformat.cbSize = 0;

    if (buf.GetMode() == wxSND_OUTPUT) {
      MMRESULT result = waveOutOpen(&internal->devout_id,
                                    WAVE_MAPPER, &wformat,
                                    (DWORD)internal->sndWin, (DWORD)this,
                                    CALLBACK_WINDOW);
      if (result != MMSYSERR_NOERROR)
        return FALSE;
      internal->devin_id = 0;
      wout_opened = TRUE;
      curr_mode = wxSND_OUTPUT;

      fragments.AllocIOBuffer();
    }
    else {
      MMRESULT result = waveInOpen(&internal->devin_id,
                                   WAVE_MAPPER, &wformat,
                                   (DWORD)internal->sndWin, (DWORD)this,
                                   CALLBACK_FUNCTION);
      if (result != MMSYSERR_NOERROR)
        return FALSE;
      internal->devout_id = 0;
      win_opened = TRUE;
      curr_mode = wxSND_INPUT;

      fragments.AllocIOBuffer();
    }
  }
  return TRUE;
}
