// --------------------------------------------------------------------------
// Name: sndfile.cpp
// Purpose:
// Date: 08/11/1999
// Author: Guilhem Lavaux <lavaux@easynet.fr> (C) 1999
// CVSID: $Id$
// --------------------------------------------------------------------------
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/stream.h>
#endif

#include "sndbase.h"
#include "sndcodec.h"
#include "sndfile.h"
#include "sndcpcm.h"
#include "sndulaw.h"

// --------------------------------------------------------------------------
// Sound codec router
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

wxSoundStream& wxSoundRouterStream::Read(void *buffer, size_t len)
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

wxSoundStream& wxSoundRouterStream::Write(const void *buffer, size_t len)
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

bool wxSoundRouterStream::SetSoundFormat(const wxSoundFormatBase& format)
{
  if (m_router)
    delete m_router;

  if (m_sndio->SetSoundFormat(format)) {
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
  }
  wxSoundStream::SetSoundFormat(m_router->GetSoundFormat());
  return TRUE;
}

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
}

wxSoundFileStream::wxSoundFileStream(wxOutputStream& stream,
                                     wxSoundStream& io_sound)
  : m_codec(io_sound), m_sndio(&io_sound),
    m_input(NULL), m_output(&stream), m_state(wxSOUND_FILE_STOPPED)
{
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

  if (!PrepareToPlay())
    return FALSE;

  m_state = wxSOUND_FILE_PLAYING;

  if (!StartProduction(wxSOUND_OUTPUT))
    return FALSE;

  return TRUE;
}

bool wxSoundFileStream::Record(unsigned long time)
{
  if (m_state != wxSOUND_FILE_STOPPED)
    return FALSE;

  if (!PrepareToRecord(time))
    return FALSE;

  m_len = m_sndformat->GetBytesFromTime(time);

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

  if (m_state == wxSOUND_FILE_RECORDING)
    if (!FinishRecording()) {
      m_state = wxSOUND_FILE_STOPPED;
      return FALSE;
    }
 
  // TODO reset counter
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

wxSoundStream& wxSoundFileStream::Read(void *buffer, size_t len)
{
  m_lastcount = GetData(buffer, len);
  return *this;
}

wxSoundStream& wxSoundFileStream::Write(const void *buffer, size_t len)
{
  m_lastcount = PutData(buffer, len);
  return *this;
}

void wxSoundFileStream::SetDuplexMode(bool duplex)
{
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

void wxSoundFileStream::OnSoundEvent(int evt)
{
  size_t len = m_sndio->GetBestSize();
  char *buffer;

  buffer = new char[len];
  wxSoundStream::OnSoundEvent(evt);

  while (!m_sndio->QueueFilled()) {
    switch(evt) {
    case wxSOUND_INPUT:
      if (len > m_len)
        len = m_len;

      len = m_codec.Read(buffer, len).GetLastAccess();
      PutData(buffer, len);
      m_len -= len;
      if (m_len == 0) {
        Stop();
        return;
      }
      break;
    case wxSOUND_OUTPUT:
      len = GetData(buffer, len);
      if (len == 0) {
        Stop();
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
