// --------------------------------------------------------------------------
// Name: sndwin.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#include <wx/wxprec.h>

#include <wx/msw/private.h>
#include <wx/module.h>
#include <string.h>
#include "sndbase.h"
#include "sndwin.h"
#include "sndpcm.h"

#include <windows.h>
#include <mmsystem.h>

typedef struct _wxSoundInternal wxSoundInternal;
typedef struct _wxSoundInfoHeader wxSoundInfoHeader;

extern char wxCanvasClassName[];

wxList *wxSoundHandleList = NULL;

static inline wxSoundStreamWin *wxFindSoundFromHandle(WXHWND hWnd)
{
  wxNode *node = wxSoundHandleList->Find((long)hWnd);
  if (!node)
    return NULL;
  return (wxSoundStreamWin *)node->Data();
}

struct _wxSoundInternal {
  HWND m_sndWin;
  HWAVEIN m_devin;
  HWAVEOUT m_devout;
  bool m_output_enabled, m_input_enabled;
};

struct _wxSoundInfoHeader {
  HGLOBAL m_h_header, m_h_data;
  char *m_data;
  WAVEHDR *m_header;
  int m_mode;
  bool m_playing, m_recording;
  wxUint32 m_position, m_size;

  wxSoundStreamWin *m_driver;
};

#define WXSOUND_MAX_QUEUE 128

wxSoundStreamWin::wxSoundStreamWin()
{
  wxSoundFormatPcm pcm;

  m_production_started = FALSE;
  m_internal = new wxSoundInternal;
  m_snderror = wxSOUND_NOERR;

  // Setup defaults
  CreateSndWindow();
  SetSoundFormat(pcm);

  if (!OpenDevice(wxSOUND_OUTPUT))
    return;

  CloseDevice();
}

wxSoundStreamWin::~wxSoundStreamWin()
{
  if (m_production_started)
    StopProduction();
  DestroySndWindow();

  delete m_internal;
}

LRESULT APIENTRY _EXPORT _wxSoundHandlerWndProc(HWND hWnd, UINT message,
                 WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case MM_WOM_DONE: {
    wxFindSoundFromHandle((WXHWND)hWnd)->NotifyDoneBuffer(wParam);
    break;
  }
  default:
    break;
  }
  return (LRESULT)0;
}

void wxSoundStreamWin::CreateSndWindow()
{
  FARPROC proc = MakeProcInstance((FARPROC)_wxSoundHandlerWndProc,
                                  wxGetInstance());
  int error;

  m_internal->m_sndWin = ::CreateWindow(wxCanvasClassName, NULL, 0,
					0, 0, 0, 0, NULL, (HMENU) NULL,
                                        wxGetInstance(), NULL);

  error = GetLastError();
  wxPrintf("%d\n", error);

  ::SetWindowLong(m_internal->m_sndWin, GWL_WNDPROC, (LONG)proc);

  wxSoundHandleList->Append((long)m_internal->m_sndWin, (wxObject *)this);
}

void wxSoundStreamWin::DestroySndWindow()
{
  if (m_internal->m_sndWin) {
    ::DestroyWindow(m_internal->m_sndWin);
    wxSoundHandleList->DeleteObject((wxObject *)this);
  }
}

bool wxSoundStreamWin::OpenDevice(int mode)
{
  wxSoundFormatPcm *pcm;
  WAVEFORMATEX wformat;

  if (!m_sndformat) {
    m_snderror = wxSOUND_INVFRMT;
    return FALSE;
  }
    
  pcm = (wxSoundFormatPcm *)m_sndformat;

  wformat.wFormatTag      = WAVE_FORMAT_PCM;
  wformat.nChannels       = pcm->GetChannels();
  wformat.nBlockAlign     = pcm->GetBPS() / 8 * wformat.nChannels;
  wformat.nAvgBytesPerSec = pcm->GetBytesFromTime(1);
  wformat.nSamplesPerSec  = pcm->GetSampleRate();
  wformat.wBitsPerSample  = pcm->GetBPS();
  wformat.cbSize          = 0;

  if (mode & wxSOUND_OUTPUT) {
    MMRESULT result;

    result = waveOutOpen(&m_internal->m_devout,
                         WAVE_MAPPER, &wformat,
                         (DWORD)m_internal->m_sndWin, 0,
                         CALLBACK_WINDOW);

    if (result != MMSYSERR_NOERROR) {
      m_snderror = wxSOUND_INVDEV;
      return FALSE;
    }

    m_output_frag_out  = WXSOUND_MAX_QUEUE-1;
    m_current_frag_out = 0;

    m_internal->m_output_enabled = TRUE;
  }
  if (mode & wxSOUND_INPUT) {
    MMRESULT result;

    result = waveInOpen(&m_internal->m_devin,
                        WAVE_MAPPER, &wformat,
                        (DWORD)m_internal->m_sndWin, 0,
                        CALLBACK_WINDOW);

    if (result != MMSYSERR_NOERROR) {
      m_snderror = wxSOUND_INVDEV;
      return FALSE;
    }

    m_input_frag_in   = WXSOUND_MAX_QUEUE-1;
    m_current_frag_in = 0;

    m_internal->m_input_enabled = TRUE;
  }

  if (!AllocHeaders(mode)) {
    CloseDevice();
    return FALSE;
  }
  return TRUE;
}


void wxSoundStreamWin::CloseDevice()
{
  if (m_internal->m_output_enabled) {
    FreeHeaders(wxSOUND_OUTPUT);
    waveOutReset(m_internal->m_devout);
    waveOutClose(m_internal->m_devout);
  }

  if (m_internal->m_input_enabled) {
    FreeHeaders(wxSOUND_INPUT);
    waveInReset(m_internal->m_devin);
    waveInClose(m_internal->m_devin);
  }

  m_internal->m_output_enabled = FALSE;
  m_internal->m_input_enabled  = FALSE;
}

wxSoundInfoHeader *wxSoundStreamWin::AllocHeader(int mode)
{
  wxSoundInfoHeader *info;
  WAVEHDR *header;

  info = new wxSoundInfoHeader;
  info->m_h_data   = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, GetBestSize());
  info->m_h_header = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(WAVEHDR));
  if (!info->m_h_data || !info->m_h_header) {
    delete info;
    m_snderror = wxSOUND_MEMERR;
    return NULL;
  }

  info->m_data      = (char *)GlobalLock(info->m_h_data);
  info->m_header    = (WAVEHDR *)GlobalLock(info->m_h_header);
  info->m_mode      = mode;
  info->m_driver    = this;
  ClearHeader(info);

  header            = info->m_header;

  header->lpData         = info->m_data;
  header->dwBufferLength = GetBestSize();
  header->dwUser         = (DWORD)info;
  header->dwFlags        = WHDR_DONE;

  if (mode == wxSOUND_INPUT) {
    MMRESULT result;

    result  = waveInPrepareHeader(m_internal->m_devin, header,
                                  sizeof(WAVEHDR));

    if (result != MMSYSERR_NOERROR) {
      GlobalUnlock(info->m_data);
      GlobalUnlock(info->m_header);
      GlobalFree(info->m_h_data);
      GlobalFree(info->m_h_header);
      delete info;

      m_snderror = wxSOUND_IOERR;
      return NULL;
    }
  } else if (mode == wxSOUND_OUTPUT) {
    MMRESULT result;

    result  = waveOutPrepareHeader(m_internal->m_devout, header,
                                   sizeof(WAVEHDR));

    if (result != MMSYSERR_NOERROR) {
      GlobalUnlock(info->m_data);
      GlobalUnlock(info->m_header);
      GlobalFree(info->m_h_data);
      GlobalFree(info->m_h_header);
      delete info;

      m_snderror = wxSOUND_IOERR;
      return NULL;
    }
  }
  return info;
}

bool wxSoundStreamWin::AllocHeaders(int mode)
{
  int i;
  wxSoundInfoHeader **headers;

  if (mode == wxSOUND_OUTPUT)
    headers = m_headers_play = new wxSoundInfoHeader *[WXSOUND_MAX_QUEUE];
  else
    headers = m_headers_rec = new wxSoundInfoHeader *[WXSOUND_MAX_QUEUE];

  memset(headers, 0, WXSOUND_MAX_QUEUE*sizeof(wxSoundInfoHeader *));

  for (i=0;i<WXSOUND_MAX_QUEUE;i++) {
    headers[i] = AllocHeader(mode);
    if (!headers[i]) {
      FreeHeaders(mode);
      return FALSE;
    }
  }
  return TRUE;
}

void wxSoundStreamWin::FreeHeader(wxSoundInfoHeader *header, int mode)
{
  if (mode == wxSOUND_OUTPUT)
    waveOutUnprepareHeader(m_internal->m_devout, header->m_header, sizeof(WAVEHDR));
  else
    waveInUnprepareHeader(m_internal->m_devin, header->m_header, sizeof(WAVEHDR));

  GlobalUnlock(header->m_data);
  GlobalUnlock(header->m_header);
  GlobalFree(header->m_h_header);
  GlobalFree(header->m_h_data);
  delete header;
}

void wxSoundStreamWin::FreeHeaders(int mode)
{
  int i;
  wxSoundInfoHeader ***headers;

  if (mode == wxSOUND_OUTPUT)
    headers = &m_headers_play;
  else
    headers = &m_headers_rec;

  for (i=0;i<WXSOUND_MAX_QUEUE;i++) {
    if ((*headers)[i]) {
      WaitFor((*headers)[i]);
      FreeHeader((*headers)[i], mode);
    }
  }
  delete[] (*headers);
  (*headers) = NULL;
}

void wxSoundStreamWin::WaitFor(wxSoundInfoHeader *info)
{
  if (info->m_position != 0) {
    memset(info->m_data + info->m_position, 0, info->m_size);
    AddToQueue(info);
  }

  if (!info->m_playing && !info->m_recording)
    return;

  while (info->m_playing || info->m_recording)
    wxYield();
}

bool wxSoundStreamWin::AddToQueue(wxSoundInfoHeader *info)
{
  MMRESULT result;

  if (info->m_mode == wxSOUND_INPUT) {
    m_current_frag_in = (m_current_frag_in + 1) % WXSOUND_MAX_QUEUE;
    result = waveInAddBuffer(m_internal->m_devin,
                             info->m_header, sizeof(WAVEHDR));
    if (result == MMSYSERR_NOERROR)
      info->m_recording = TRUE;
    else
      return FALSE;
  } else if (info->m_mode == wxSOUND_OUTPUT) {
    result = waveOutWrite(m_internal->m_devout,
                          info->m_header, sizeof(WAVEHDR));
    if (result == MMSYSERR_NOERROR)
      info->m_playing = TRUE;
    else
      return FALSE;
  }
  return TRUE;
}

void wxSoundStreamWin::ClearHeader(wxSoundInfoHeader *info)
{
  info->m_playing   = FALSE;
  info->m_recording = FALSE;
  info->m_position  = 0;
  info->m_size      = GetBestSize();
}

wxSoundInfoHeader *wxSoundStreamWin::NextFragmentOutput()
{
  if (m_headers_play[m_current_frag_out]->m_playing) {
    m_current_frag_out = (m_current_frag_out + 1) % WXSOUND_MAX_QUEUE;

    if (m_headers_play[m_current_frag_out]->m_playing)
      WaitFor(m_headers_play[m_current_frag_out]);
  }
  if (m_current_frag_out == m_output_frag_out)
    m_queue_filled = TRUE;
  return m_headers_play[m_current_frag_out];
}

wxSoundStream& wxSoundStreamWin::Write(const void *buffer, size_t len)
{
  m_lastcount = 0;
  if (!m_internal->m_output_enabled)
    return *this;

  while (len > 0) {
    wxSoundInfoHeader *header;
    size_t to_copy;

    header              = NextFragmentOutput();

    to_copy             = (len > header->m_size) ? header->m_size : len;
    memcpy(header->m_data + header->m_position, buffer, to_copy);

    header->m_position += to_copy;
    header->m_size     -= to_copy;
    buffer              = (((const char *)buffer) + to_copy);
    len                -= to_copy;
    m_lastcount        += to_copy;
    
    if (header->m_size == 0)
      if (!AddToQueue(header)) {
        m_snderror = wxSOUND_IOERR;
        return *this;
      }
  }
  return *this;
}

wxSoundInfoHeader *wxSoundStreamWin::NextFragmentInput()
{
  wxSoundInfoHeader *header;

  // TODO //
  header = m_headers_rec[m_current_frag_in];
  WaitFor(header);

  if (m_current_frag_in == m_input_frag_in)
    m_queue_filled = TRUE;

  return header;
}

wxSoundStream& wxSoundStreamWin::Read(void *buffer, size_t len)
{
  wxSoundInfoHeader *header;
  size_t to_copy;

  m_lastcount = 0;
  if (!m_internal->m_input_enabled)
    return *this;

  while (len > 0) {
    header = NextFragmentInput();

    to_copy             = (len > header->m_size) ? header->m_size : len;
    memcpy(buffer, header->m_data + header->m_position, to_copy);

    header->m_position += to_copy;
    header->m_size     -= to_copy;
    buffer              = (((char *)buffer) + to_copy);
    len                -= to_copy;
    m_lastcount        += to_copy;

    if (header->m_size == 0) {
      ClearHeader(header);
      if (!AddToQueue(header)) {
        m_snderror = wxSOUND_IOERR;
        return *this;
      }
    }
  }
  return *this;
}

void wxSoundStreamWin::NotifyDoneBuffer(wxUint32 dev_handle)
{
  wxSoundInfoHeader *info;

  if (dev_handle == (wxUint32)m_internal->m_devout) {
    m_output_frag_out = (m_output_frag_out + 1) % WXSOUND_MAX_QUEUE;
    info = m_headers_play[m_output_frag_out];
    ClearHeader(info);
    m_queue_filled = FALSE;
    OnSoundEvent(wxSOUND_OUTPUT);
  } else {
    m_input_frag_in = (m_input_frag_in + 1) % WXSOUND_MAX_QUEUE;
    OnSoundEvent(wxSOUND_INPUT);
    m_queue_filled = FALSE;
  }
}

bool wxSoundStreamWin::SetSoundFormat(wxSoundFormatBase& base)
{
  return wxSoundStream::SetSoundFormat(base);
}

bool wxSoundStreamWin::StartProduction(int evt)
{
  if ((m_internal->m_output_enabled && (evt & wxSOUND_OUTPUT)) ||
      (m_internal->m_input_enabled && (evt & wxSOUND_INPUT)))
    CloseDevice();

  if (!OpenDevice(evt))
    return FALSE;

  m_production_started = TRUE;
  m_queue_filled = FALSE;
  // Send a dummy event to start.
  if (evt & wxSOUND_OUTPUT)
    OnSoundEvent(evt);

  if (evt & wxSOUND_INPUT) {
    int i;
    for (i=0;i<WXSOUND_MAX_QUEUE;i++)
      AddToQueue(m_headers_rec[i]);
  }

  return TRUE;
}

bool wxSoundStreamWin::StopProduction()
{
  m_production_started = FALSE;
  CloseDevice();
  return TRUE;
}

bool wxSoundStreamWin::QueueFilled() const
{
  return (!m_production_started || m_queue_filled);
}


// --------------------------------------------------------------------------
// wxSoundWinModule
// --------------------------------------------------------------------------

class WXDLLEXPORT wxSoundWinModule : public wxModule {
   DECLARE_DYNAMIC_CLASS(wxSoundWinModule)
 public:
   bool OnInit();
   void OnExit();
};

IMPLEMENT_DYNAMIC_CLASS(wxSoundWinModule, wxModule)

bool wxSoundWinModule::OnInit() {
  wxSoundHandleList = new wxList(wxKEY_INTEGER);
  return TRUE;
}

void wxSoundWinModule::OnExit() {
  delete wxSoundHandleList;
}
