// --------------------------------------------------------------------------
// Name: sndfile.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999, 2000
// CVSID: $Id$
// --------------------------------------------------------------------------
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/defs.h>
#include <wx/stream.h>
#endif

#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndcodec.h"
#include "wx/mmedia/sndfile.h"
#include "wx/mmedia/sndcpcm.h"
#include "wx/mmedia/sndulaw.h"
#include "wx/mmedia/sndg72x.h"

// --------------------------------------------------------------------------
// Sound codec router
// A very important class: it ensures that everybody is satisfied.
// It is supposed to create as many codec as it is necessary to transform
// a signal in a specific format in an another.
// --------------------------------------------------------------------------
wxSoundRouterStream::wxSoundRouterStream(wxSoundStream& sndio)
  : wxSoundStreamCodec(sndio)
{
    m_router = NULL;
}

wxSoundRouterStream::~wxSoundRouterStream()
{
    if (m_router)
        delete m_router;
}

// --------------------------------------------------------------------------
// Read(void *buffer, wxUint32 len): It reads data synchronously. See sndbase.h
// for possible errors and behaviours ...
// --------------------------------------------------------------------------
wxSoundStream& wxSoundRouterStream::Read(void *buffer, wxUint32 len)
{
    if (m_router) {
        m_router->Read(buffer, len);
        m_snderror  = m_router->GetError();
        m_lastcount = m_router->GetLastAccess();
    } else {
        m_sndio->Read(buffer, len);
        m_snderror  = m_sndio->GetError();
        m_lastcount = m_sndio->GetLastAccess();
    }
    return *this;
}

// --------------------------------------------------------------------------
// Write(const void *buffer, wxUint32 len): It writes data synchronously
// --------------------------------------------------------------------------
wxSoundStream& wxSoundRouterStream::Write(const void *buffer, wxUint32 len)
{
    if (m_router) {
        m_router->Write(buffer, len);
        m_snderror  = m_router->GetError();
        m_lastcount = m_router->GetLastAccess();
    } else {
        m_sndio->Write(buffer, len);
        m_snderror  = m_sndio->GetError();
        m_lastcount = m_sndio->GetLastAccess();
  }
  return *this;
}

// --------------------------------------------------------------------------
// SetSoundFormat(const wxSoundFormatBase& format) first tries to setup the
// sound driver using the specified format. If this fails, it uses personnal
// codec converters: for the moment there is a PCM converter (PCM to PCM:
// with optional resampling, ...), an ULAW converter (ULAW to PCM), a G72X
// converter (G72X to PCM). If nothing works, it returns FALSE.
// --------------------------------------------------------------------------
bool wxSoundRouterStream::SetSoundFormat(const wxSoundFormatBase& format)
{
    if (m_router)
        delete m_router;
    
    // First, we try to setup the sound device
    if (m_sndio->SetSoundFormat(format)) {
        // We are lucky, it is working.
        wxSoundStream::SetSoundFormat(m_sndio->GetSoundFormat());
        return TRUE;
    }
    
    switch(format.GetType()) {
        case wxSOUND_NOFORMAT:
            return FALSE;
        case wxSOUND_PCM:
            m_router = new wxSoundStreamPcm(*m_sndio);
            m_router->SetSoundFormat(format);
            break;
        case wxSOUND_ULAW:
            m_router = new wxSoundStreamUlaw(*m_sndio);
            m_router->SetSoundFormat(format);
            break;
        case wxSOUND_G72X:
            m_router = new wxSoundStreamG72X(*m_sndio);
            m_router->SetSoundFormat(format);
            break;
        default:
            return FALSE;
        
    }
    wxSoundStream::SetSoundFormat(m_router->GetSoundFormat());
    return TRUE;
}

// --------------------------------------------------------------------------
// GetBestSize() returns the specific best buffer size a sound driver
// can manage. It means that it will be easier for it to manage the buffer
// and so it will be faster and in some case more accurate for real-time event.
// --------------------------------------------------------------------------
wxUint32 wxSoundRouterStream::GetBestSize() const
{
  if (m_router)
    return m_router->GetBestSize();
  else
    return m_sndio->GetBestSize();
}

// --------------------------------------------------------------------------
// StartProduction(int evt). See sndbase.h 
// --------------------------------------------------------------------------
bool wxSoundRouterStream::StartProduction(int evt)
{
    if (!m_router) {
        if (m_sndio->StartProduction(evt))
            return TRUE;
        
        m_snderror = m_sndio->GetError();
        m_lastcount = m_sndio->GetLastAccess();
        return FALSE;
    }
    
    if (m_router->StartProduction(evt))
        return TRUE;
    
    m_snderror = m_router->GetError();
    m_lastcount = m_router->GetLastAccess();
    return FALSE;
} 

// --------------------------------------------------------------------------
// StopProduction(). See sndbase.h
// --------------------------------------------------------------------------
bool wxSoundRouterStream::StopProduction()
{
    if (!m_router) {
        if (m_sndio->StopProduction())
            return TRUE;
        
        m_snderror = m_sndio->GetError();
        m_lastcount = m_sndio->GetLastAccess();
        return FALSE;
    }
    
    if (m_router->StopProduction())
        return TRUE;
    
    m_snderror = m_router->GetError();
    m_lastcount = m_router->GetLastAccess();
    return FALSE;
}

// --------------------------------------------------------------------------
// wxSoundFileStream: generic reader
// --------------------------------------------------------------------------

wxSoundFileStream::wxSoundFileStream(wxInputStream& stream,
                                     wxSoundStream& io_sound)
        : m_codec(io_sound), m_sndio(&io_sound),
          m_input(&stream), m_output(NULL), m_state(wxSOUND_FILE_STOPPED)
{
    m_length = 0;
    m_bytes_left = 0;
    m_prepared = FALSE;
}

wxSoundFileStream::wxSoundFileStream(wxOutputStream& stream,
                                     wxSoundStream& io_sound)
  : m_codec(io_sound), m_sndio(&io_sound),
    m_input(NULL), m_output(&stream), m_state(wxSOUND_FILE_STOPPED)
{
  m_length = 0;
  m_bytes_left = 0;
  m_prepared = FALSE;
}

wxSoundFileStream::~wxSoundFileStream()
{
  if (m_state != wxSOUND_FILE_STOPPED)
    Stop();
}

bool wxSoundFileStream::Play()
{
  if (m_state != wxSOUND_FILE_STOPPED)
    return FALSE;

  if (!m_prepared)
    if (!PrepareToPlay())
      return FALSE;

  m_state = wxSOUND_FILE_PLAYING;

  if (!StartProduction(wxSOUND_OUTPUT))
    return FALSE;

  return TRUE;
}

bool wxSoundFileStream::Record(wxUint32 time)
{
  if (m_state != wxSOUND_FILE_STOPPED)
    return FALSE;

  if (!PrepareToRecord(time))
    return FALSE;

  FinishPreparation(m_sndformat->GetBytesFromTime(time));

  m_state = wxSOUND_FILE_RECORDING;
  if (!StartProduction(wxSOUND_INPUT))
    return FALSE;

  return TRUE;
}

bool wxSoundFileStream::Stop()
{
  if (m_state == wxSOUND_FILE_STOPPED)
    return FALSE;

  if (!StopProduction())
    return FALSE;

  m_prepared = FALSE;

  if (m_state == wxSOUND_FILE_RECORDING)
    if (!FinishRecording()) {
      m_state = wxSOUND_FILE_STOPPED;
      return FALSE;
    }

  if (m_input)
    m_input->SeekI(0, wxFromStart);

  if (m_output)
    m_output->SeekO(0, wxFromStart);
 
  m_state = wxSOUND_FILE_STOPPED;
  return TRUE;
}

bool wxSoundFileStream::Pause()
{
  if (m_state == wxSOUND_FILE_PAUSED || m_state == wxSOUND_FILE_STOPPED)
    return FALSE;

  if (!StopProduction())
    return FALSE;

  m_oldstate = m_state;
  m_state = wxSOUND_FILE_PAUSED;
  return TRUE;
}

bool wxSoundFileStream::Resume()
{
  if (m_state == wxSOUND_FILE_PLAYING || m_state == wxSOUND_FILE_RECORDING ||
      m_state == wxSOUND_FILE_STOPPED)
    return FALSE;

  if (!StartProduction( (m_oldstate == wxSOUND_FILE_PLAYING) ?
                             wxSOUND_OUTPUT : wxSOUND_INPUT))
    return FALSE;

  m_state = m_oldstate;

  return TRUE;
}

wxSoundStream& wxSoundFileStream::Read(void *buffer, wxUint32 len)
{
  if (!m_prepared || m_state != wxSOUND_FILE_PLAYING) {
      m_snderror = wxSOUND_NOTSTARTED;
      m_lastcount = 0;
      return *this;
  }
  m_lastcount = GetData(buffer, len);
  return *this;
}

wxSoundStream& wxSoundFileStream::Write(const void *buffer, wxUint32 len)
{
  if (!m_prepared || m_state != wxSOUND_FILE_RECORDING) {
      m_snderror = wxSOUND_NOTSTARTED;
      m_lastcount = 0;
      return *this;
  }
  m_lastcount = PutData(buffer, len);
  return *this;
}

bool wxSoundFileStream::StartProduction(int evt)
{
  m_sndio->SetEventHandler(this);

  if (!m_codec.StartProduction(evt))
    return FALSE;

  return TRUE;
}

bool wxSoundFileStream::StopProduction()
{
  return m_codec.StopProduction();
}

void wxSoundFileStream::FinishPreparation(wxUint32 len)
{
  m_bytes_left = m_length = len;
  m_prepared = TRUE;
}

wxString wxSoundFileStream::GetCodecName() const
{
    return wxString(wxT("wxSoundFileStream base codec"));
}

wxUint32 wxSoundFileStream::GetLength()
{
  if (m_input && !m_prepared && GetError() == wxSOUND_NOERROR)
    return (PrepareToPlay()) ? m_length : 0;

  return m_length;
}

wxUint32 wxSoundFileStream::GetPosition()
{
  if (!m_prepared && m_input != NULL && GetError() == wxSOUND_NOERROR)
    PrepareToPlay();

  return m_length-m_bytes_left;
}

wxUint32 wxSoundFileStream::SetPosition(wxUint32 new_position)
{
  if (!m_prepared && m_input != NULL && GetError() == wxSOUND_NOERROR)
    PrepareToPlay();

  if (!m_prepared)
    return 0;

  if (!RepositionStream(new_position))
      return m_length-m_bytes_left;
  
  if (new_position >= m_length) {
    m_bytes_left = 0;
    return m_length;
  }

  m_bytes_left = m_length-new_position;
  return new_position;
}

void wxSoundFileStream::OnSoundEvent(int evt)
{
  wxUint32 len = m_codec.GetBestSize();
  char *buffer;

  buffer = new char[len];
  wxSoundStream::OnSoundEvent(evt);

  while (!m_sndio->QueueFilled()) {
    switch(evt) {
    case wxSOUND_INPUT:
      if (len > m_bytes_left)
        len = m_bytes_left;

      len = m_codec.Read(buffer, len).GetLastAccess();
      PutData(buffer, len);
      m_bytes_left -= len;
      if (m_bytes_left == 0) {
        Stop();
        delete[] buffer;
        return;
      }
      break;
    case wxSOUND_OUTPUT:
      if (len > m_bytes_left)
        len = m_bytes_left;

      len = GetData(buffer, len);
      m_bytes_left -= len;
      if (m_bytes_left == 0) {
        Stop();
        delete[] buffer;
        return;
      }
      m_codec.Write(buffer, len);
      break;
    }
  }
  delete[] buffer;
}

bool wxSoundFileStream::SetSoundFormat(const wxSoundFormatBase& format)
{
  wxSoundStream::SetSoundFormat(format);
  return m_codec.SetSoundFormat(format);
}
