// --------------------------------------------------------------------------
// Name: sndwin.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#include <wx/wxprec.h>

#include <wx/app.h>
#include <wx/module.h>
#include <wx/msw/private.h>
#include <string.h>
#include "sndbase.h"
#include "sndwin.h"
#include "sndpcm.h"

#include <windows.h>
#include <mmsystem.h>

typedef struct _wxSoundInternal wxSoundInternal;
typedef struct _wxSoundInfoHeader wxSoundInfoHeader;

extern const wxChar *wxCanvasClassName;

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

#define WXSOUND_MAX_QUEUE 10

wxSoundStreamWin::wxSoundStreamWin()
{
  wxSoundFormatPcm pcm;

  m_production_started = FALSE;
  m_internal = new wxSoundInternal;
  if (!m_internal) {
    m_snderror = wxSOUND_MEMERR;
    m_internal = NULL;
    return;
  }
  m_snderror = wxSOUND_NOERR;

  // Setup defaults
  CreateSndWindow();
  SetSoundFormat(pcm);

  m_internal->m_input_enabled = FALSE;
  m_internal->m_output_enabled = FALSE;

  m_waiting_for = FALSE;

  if (!OpenDevice(wxSOUND_OUTPUT))
    return;

  CloseDevice();
}

wxSoundStreamWin::~wxSoundStreamWin()
{
  if (m_internal) {
    if (m_production_started)
      StopProduction();
    DestroySndWindow();

    delete m_internal;
  }
}

LRESULT APIENTRY _EXPORT _wxSoundHandlerWndProc(HWND hWnd, UINT message,
                 WPARAM wParam, LPARAM lParam)
{
  wxSoundStreamWin *sndwin;

  sndwin = wxFindSoundFromHandle((WXHWND)hWnd);
  if (!sndwin)
    return (LRESULT)0;

  switch (message) {
  case MM_WOM_DONE:
    sndwin->NotifyDoneBuffer(wParam, wxSOUND_OUTPUT);
    break;
  case MM_WIM_DATA:
    sndwin->NotifyDoneBuffer(wParam, wxSOUND_INPUT);
    break;
  default:
    break;
  }
  return (LRESULT)0;
}

// -----------------------------------------------------------------------
// CreateSndWindow() creates an hidden window which will receive the sound
// events
// -----------------------------------------------------------------------

void wxSoundStreamWin::CreateSndWindow()
{
  FARPROC proc = MakeProcInstance((FARPROC)_wxSoundHandlerWndProc,
                                  wxGetInstance());
  int error;

  m_internal->m_sndWin = ::CreateWindow(wxCanvasClassName, NULL, 0,
					0, 0, 0, 0, NULL, (HMENU) NULL,
                                        wxGetInstance(), NULL);

  error = GetLastError();

  ::SetWindowLong(m_internal->m_sndWin, GWL_WNDPROC, (LONG)proc);

  // Add this window to the sound handle list so we'll be able to redecode
  // the "magic" number.
  wxSoundHandleList->Append((long)m_internal->m_sndWin, (wxObject *)this);
}

// -----------------------------------------------------------------------
// DestroySndWindow() destroys the hidden window
// -----------------------------------------------------------------------

void wxSoundStreamWin::DestroySndWindow()
{
  if (m_internal->m_sndWin) {
    ::DestroyWindow(m_internal->m_sndWin);
    wxSoundHandleList->DeleteObject((wxObject *)this);
  }
}

// -------------------------------------------------------------------------
// OpenDevice(int mode) initializes the windows driver for a "mode"
// operation. mode is a bit mask: if the bit "wxSOUND_OUTPUT" is set,
// the driver is opened for output operation, and if the bit "wxSOUND_INPUT"
// is set, then the driver is opened for input operation. The two modes
// aren't exclusive.
// The initialization parameters (sample rate, ...) are taken from the
// m_sndformat object.
// At the end, OpenDevice() calls AllocHeaders() to initialize the Sound IO
// queue.
// -------------------------------------------------------------------------
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
  wformat.nBlockAlign     = wformat.nChannels * pcm->GetBPS() / 8;
  wformat.nSamplesPerSec  = pcm->GetSampleRate();
  wformat.nAvgBytesPerSec = wformat.nSamplesPerSec * wformat.nBlockAlign;
  wformat.wBitsPerSample  = pcm->GetBPS();
  wformat.cbSize          = 0;

  // -----------------------------------
  // Open the driver for Output operation
  // -----------------------------------
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
  // -----------------------------------
  // Open the driver for Input operation
  // -----------------------------------
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

    m_current_frag_in = WXSOUND_MAX_QUEUE-1;
    m_input_frag_in   = 0;

    m_internal->m_input_enabled = TRUE;
  }

  if (mode & wxSOUND_OUTPUT) {
    if (!AllocHeaders(wxSOUND_OUTPUT)) {
      CloseDevice();
      return FALSE;
    }
  }
  if (mode & wxSOUND_INPUT) {
    if (!AllocHeaders(wxSOUND_INPUT)) {
      CloseDevice();
      return FALSE;
    }
  }

  return TRUE;
}

// -------------------------------------------------------------------------
// CloseDevice() closes the driver handles and frees memory allocated for
// IO queues.
// -------------------------------------------------------------------------
void wxSoundStreamWin::CloseDevice()
{
  if (m_internal->m_output_enabled) {
    FreeHeaders(wxSOUND_OUTPUT);
    m_internal->m_output_enabled = FALSE;
    waveOutClose(m_internal->m_devout);
  }

  if (m_internal->m_input_enabled) {
    FreeHeaders(wxSOUND_INPUT);
    m_internal->m_input_enabled  = FALSE;
    waveInClose(m_internal->m_devin);
  }
}

// -------------------------------------------------------------------------
// AllocHeader(int mode)
//
// mode has the same mean as in OpenDevice() except that here the two flags
// must be exclusive.
// AllocHeader() initializes an element of an operation (this can be input
// or output). It means it allocates the sound header's memory block 
// and "prepares" it (It is needed by Windows). At the same time, it sets
// private datas so we can the header's owner (See callback).
//
// It returns the new allocated block or NULL.
// -------------------------------------------------------------------------
wxSoundInfoHeader *wxSoundStreamWin::AllocHeader(int mode)
{
  wxSoundInfoHeader *info;
  WAVEHDR *header;

  // Some memory allocation
  info = new wxSoundInfoHeader;
  info->m_h_data   = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, GetBestSize());
  info->m_h_header = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(WAVEHDR));
  if (!info->m_h_data || !info->m_h_header) {
    delete info;
    m_snderror = wxSOUND_MEMERR;
    return NULL;
  }

  // Get the two pointers from the system
  info->m_data      = (char *)GlobalLock(info->m_h_data);
  info->m_header    = (WAVEHDR *)GlobalLock(info->m_h_header);
  // Set the header's mode
  info->m_mode      = mode;
  // Set the parent of the header
  info->m_driver    = this;
  // Clean it up
  ClearHeader(info);

  header            = info->m_header;
  // Initialize Windows variables
  header->lpData         = info->m_data;
  header->dwBufferLength = GetBestSize();
  header->dwUser         = (DWORD)info;
  header->dwFlags        = WHDR_DONE;

  // "Prepare" the header
  if (mode == wxSOUND_INPUT) {
    MMRESULT result;

    result  = waveInPrepareHeader(m_internal->m_devin, header,
                                  sizeof(WAVEHDR));

    if (result != MMSYSERR_NOERROR) {
      // If something goes wrong, free everything.
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
      // If something goes wrong, free everything.
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

// -------------------------------------------------------------------------
// AllocHeaders(int mode)
//
// "mode" has the same mean as for OpenDevice() except that the two flags must
// be exclusive.
// AllocHeaders() allocates WXSOUND_MAX_QUEUE (= 128) blocks for an operation
// queue. It uses AllocHeader() for each element.
//
// Once it has allocated all blocks, it returns TRUE and if an error occured
// it returns FALSE.
// -------------------------------------------------------------------------
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

// -------------------------------------------------------------------------
// FreeHeader(int mode)
//
// "mode" has the same mean as for OpenDevice() except that the two flags must
// be exclusive.
// FreeHeader() frees a memory block and "unprepares" it.
// -------------------------------------------------------------------------
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

// -------------------------------------------------------------------------
// FreeHeaders(int mode)
//
// "mode" has the same mean as for OpenDevice() except that the two flags must
// be exclusive.
// FreeHeaders() frees all an operation queue once it has checked that
// all buffers have been terminated.
// -------------------------------------------------------------------------
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
      // We wait for the end of the buffer
      WaitFor((*headers)[i]);
      // Then, we free the header
      FreeHeader((*headers)[i], mode);
    }
  }
  delete[] (*headers);
  (*headers) = NULL;
}

// -------------------------------------------------------------------------
// WaitFor(wxSoundInfoHeader *info)
//
// "info" is one element of an IO queue
// WaitFor() checks whether the specified block has been terminated.
// If it hasn't been terminated, it waits for its termination.
//
// NB: if it's a partially filled buffer it adds it to the Windows queue
// -------------------------------------------------------------------------
void wxSoundStreamWin::WaitFor(wxSoundInfoHeader *info)
{
  // If the buffer is finished, we return immediately
  if (!info->m_playing) {

    // We begun filling it: we must send it to the Windows queue
    if (info->m_position != 0) {
      memset(info->m_data + info->m_position, 0, info->m_size);
      AddToQueue(info);
    }
  }

  if (m_waiting_for) {
    // PROBLEM //
    return;
  }
  m_waiting_for = TRUE;
  // Else, we wait for its termination
  while (info->m_playing || info->m_recording)
    wxYield();
  m_waiting_for = FALSE;
}

// -------------------------------------------------------------------------
// AddToQueue(wxSoundInfoHeader *info)
//
// For "info", see WaitFor()
// AddToQueue() sends the IO queue element to the Windows queue.
//
// Warning: in the current implementation, it partially assume we send the
// element in the right order. This is true in that implementation but if
// you use it elsewhere, be careful: it may shuffle all your sound datas.
// -------------------------------------------------------------------------
bool wxSoundStreamWin::AddToQueue(wxSoundInfoHeader *info)
{
  MMRESULT result;

  if (info->m_mode == wxSOUND_INPUT) {
    // Increment the input fragment pointer
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

// -------------------------------------------------------------------------
// ClearHeader(wxSoundInfoHeader *info)
//
// ClearHeader() reinitializes the parameters of "info" to their default
// value.
// -------------------------------------------------------------------------
void wxSoundStreamWin::ClearHeader(wxSoundInfoHeader *info)
{
  info->m_playing   = FALSE;
  info->m_recording = FALSE;
  info->m_position  = 0;
  info->m_size      = GetBestSize();
}

// -------------------------------------------------------------------------
// wxSoundInfoHeader *NextFragmentOutput()
//
// NextFragmentOutput() looks for a free output block. It will always
// return you a non-NULL pointer but it may waits for an empty buffer a long
// time.
// -------------------------------------------------------------------------
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

// -------------------------------------------------------------------------
// The behaviour of Write is documented in the global documentation.
// -------------------------------------------------------------------------
wxSoundStream& wxSoundStreamWin::Write(const void *buffer, wxUint32 len)
{
  m_lastcount = 0;
  if (!m_internal->m_output_enabled)
    return *this;

  while (len > 0) {
    wxSoundInfoHeader *header;
    wxUint32 to_copy;

    // Get a new output fragment
    header              = NextFragmentOutput();

    to_copy             = (len > header->m_size) ? header->m_size : len;
    memcpy(header->m_data + header->m_position, buffer, to_copy);

    header->m_position += to_copy;
    header->m_size     -= to_copy;
    buffer              = (((const char *)buffer) + to_copy);
    len                -= to_copy;
    m_lastcount        += to_copy;
    
    // If the fragment is full, we send it to the Windows queue.
    if (header->m_size == 0)
      if (!AddToQueue(header)) {
        m_snderror = wxSOUND_IOERR;
        return *this;
      }
  }
  return *this;
}

// -------------------------------------------------------------------------
// NextFragmentInput is not functional.
// -------------------------------------------------------------------------
wxSoundInfoHeader *wxSoundStreamWin::NextFragmentInput()
{
  wxSoundInfoHeader *header;

  m_current_frag_in = (m_current_frag_in + 1) % WXSOUND_MAX_QUEUE;

  header = m_headers_rec[m_current_frag_in];
  if (header->m_recording)
    WaitFor(header);

  if (m_current_frag_in == m_input_frag_in)
    m_queue_filled = TRUE;

  return header;
}

// -------------------------------------------------------------------------
// The behaviour of Read is documented in the global documentation.
// -------------------------------------------------------------------------
wxSoundStream& wxSoundStreamWin::Read(void *buffer, wxUint32 len)
{
  wxSoundInfoHeader *header;
  wxUint32 to_copy;

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

// -------------------------------------------------------------------------
// NotifyDoneBuffer(wxUint32 dev_handle)
//
// NotifyDoneBuffer() is called by wxSoundHandlerProc each time a sound
// fragment finished. It reinitializes the parameters of the fragment and
// sends an event to the clients.
// -------------------------------------------------------------------------
void wxSoundStreamWin::NotifyDoneBuffer(wxUint32 dev_handle, int flag)
{
  wxSoundInfoHeader *info;

  if (flag == wxSOUND_OUTPUT) {
    if (!m_internal->m_output_enabled)
      return;

    m_output_frag_out = (m_output_frag_out + 1) % WXSOUND_MAX_QUEUE;
    info = m_headers_play[m_output_frag_out];
    ClearHeader(info);
    m_queue_filled = FALSE;
    if (!m_waiting_for)
      OnSoundEvent(wxSOUND_OUTPUT);
  } else {
    if (!m_internal->m_input_enabled)
      return;

    m_headers_rec[m_input_frag_in]->m_recording = FALSE;
    m_input_frag_in = (m_input_frag_in + 1) % WXSOUND_MAX_QUEUE;
    if (!m_waiting_for)
      OnSoundEvent(wxSOUND_INPUT);
    m_queue_filled = FALSE;
  }
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
bool wxSoundStreamWin::SetSoundFormat(wxSoundFormatBase& base)
{
  return wxSoundStream::SetSoundFormat(base);
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
bool wxSoundStreamWin::StartProduction(int evt)
{
  if (!m_internal)
    return FALSE;

  if ((m_internal->m_output_enabled && (evt & wxSOUND_OUTPUT)) ||
      (m_internal->m_input_enabled && (evt & wxSOUND_INPUT)))
    CloseDevice();

  if (!OpenDevice(evt))
    return FALSE;

  m_production_started = TRUE;
  m_queue_filled = FALSE;
  // Send a dummy event to start.
  if (evt & wxSOUND_OUTPUT)
    OnSoundEvent(wxSOUND_OUTPUT);

  if (evt & wxSOUND_INPUT) {
    int i;
    for (i=0;i<WXSOUND_MAX_QUEUE;i++)
      AddToQueue(m_headers_rec[i]);

    waveInStart(m_internal->m_devin);
  }

  return TRUE;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
bool wxSoundStreamWin::StopProduction()
{
  if (!m_production_started)
    return FALSE;

  m_production_started = FALSE;
  CloseDevice();
  return TRUE;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
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
