/////////////////////////////////////////////////////////////////////////////
// Name:        mmbman.cpp
// Purpose:     Multimedia Board manager
// Author:      Guilhem Lavaux, <guilhem.lavaux@libertysurf.fr>
// Modified by:
// Created:     13/02/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
  #pragma implementation "mmbman.cpp"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Personnal headers

#include "wx/stream.h"
#include "wx/wfstream.h"

#include "sndbase.h"
#include "sndfile.h"
#include "sndwav.h"
#include "sndaiff.h"
#include "sndpcm.h"
#include "sndulaw.h"

#ifdef __UNIX__
#include "sndoss.h"
#include "sndesd.h"
#endif

#ifdef __WIN32__
#include "sndwin.h"
#endif

#include "mmboard.h"
#include "mmbman.h"

// ----------------------------------------------------------------------------
// Private class definitions
// ----------------------------------------------------------------------------

class MMBoardSoundFile: public MMBoardFile {
public:
  MMBoardSoundFile(const wxString& filename);
  ~MMBoardSoundFile();

  bool NeedWindow();

  void SetWindow(wxWindow *window);
  
  void Play();
  void Pause();
  void Resume();
  void Stop();

  MMBoardTime GetPosition();
  MMBoardTime GetLength();

  bool IsStopped();
  bool IsPaused();

  wxString GetStringType();
  wxString GetStringInformation();

protected:
  wxSoundFileStream *GetDecoder();

  wxSoundStream *m_output_stream;
  wxInputStream *m_input_stream;
  wxSoundFileStream *m_file_stream;

  MMBoardTime m_length;
};

// ----------------------------------------------------------------------------
// Implementation
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// MMBoardSoundFile

MMBoardSoundFile::MMBoardSoundFile(const wxString& filename)
  : MMBoardFile()
{
  m_input_stream = new wxFileInputStream(filename);
  m_output_stream = MMBoardManager::OpenSoundStream();

  m_file_stream = GetDecoder();
  
  if (!m_file_stream)
    SetError(MMBoard_UnknownFile);

  // Compute length
  wxUint32 length, seconds;

  length = m_file_stream->GetLength();
  seconds = m_file_stream->GetSoundFormat().GetTimeFromBytes(length);
  m_length.seconds = seconds % 60;
  m_length.minutes = (seconds / 60) % 60;
  m_length.hours   = seconds / 3600;
}

MMBoardSoundFile::~MMBoardSoundFile()
{
  delete m_file_stream;
  MMBoardManager::UnrefSoundStream(m_output_stream);
  delete m_input_stream;
}

wxSoundFileStream *MMBoardSoundFile::GetDecoder()
{
  wxSoundFileStream *f_stream;

  // First, we try a Wave decoder
  f_stream = new wxSoundWave(*m_input_stream, *m_output_stream);
  if (f_stream->CanRead())
    return f_stream;
  delete f_stream;

  // Then, a AIFF decoder
  f_stream = new wxSoundAiff(*m_input_stream, *m_output_stream);
  if (f_stream->CanRead())
    return f_stream;
  delete f_stream;

  // TODO: automate

  return NULL;
}

MMBoardTime MMBoardSoundFile::GetLength()
{
  return m_length;
}

bool MMBoardSoundFile::IsStopped()
{
  return m_file_stream->IsStopped();
}

bool MMBoardSoundFile::IsPaused()
{
  return m_file_stream->IsPaused();
}

MMBoardTime MMBoardSoundFile::GetPosition()
{
  wxUint32 length, seconds;
  MMBoardTime file_time;

  file_time.seconds = file_time.minutes = file_time.hours = 0;
  if (m_file_stream->IsStopped())
    return file_time;

  length = m_file_stream->GetPosition();
  seconds = m_file_stream->GetSoundFormat().GetTimeFromBytes(length);
  file_time.seconds = seconds % 60;
  file_time.minutes = (seconds / 60) % 60;
  file_time.hours   = seconds / 3600;

  return file_time;
}

bool MMBoardSoundFile::NeedWindow()
{
  return FALSE;
}

void MMBoardSoundFile::SetWindow(wxWindow *window)
{
}

void MMBoardSoundFile::Play()
{
  m_file_stream->Play();
}

void MMBoardSoundFile::Pause()
{
  m_file_stream->Pause();
}

void MMBoardSoundFile::Resume()
{
  m_file_stream->Resume();
}

void MMBoardSoundFile::Stop()
{
  m_file_stream->Stop();
}

wxString MMBoardSoundFile::GetStringType()
{
  return wxString("WAVE file");
}

wxString MMBoardSoundFile::GetStringInformation()
{
  wxString info;
  wxSoundFormatBase *format;

  format = &(m_file_stream->GetSoundFormat());

  info = _T("Data encoding: ");
  switch (format->GetType()) {
  case wxSOUND_PCM: {
    wxSoundFormatPcm *pcm_format = (wxSoundFormatPcm *)format;

    info += _T("PCM\n");
    info += wxString::Format(_T("Sampling rate: %d\n")
			     _T("Bits per sample: %d\n")
			     _T("Number of channels: %d\n"),
			     pcm_format->GetSampleRate(),
			     pcm_format->GetBPS(),
			     pcm_format->GetChannels());

    break;
  }
  case wxSOUND_ULAW: {
    wxSoundFormatUlaw *ulaw_format = (wxSoundFormatUlaw *)format;
    info += _T("ULAW\n");
    info += wxString::Format(_T("Sampling rate: %d\n"), ulaw_format->GetSampleRate());
    break;
  }
  default:
    info += _T("Unknown");
    break;
  }
  return info;
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// MMBoardFile

MMBoardFile::MMBoardFile()
{
  m_error = 0;
}

MMBoardFile::~MMBoardFile()
{
}

//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// MMBoardManager

MMBoardFile *MMBoardManager::Open(const wxString& filename)
{
  MMBoardFile *file;

  file = new MMBoardSoundFile(filename);
  if (file->GetError()) {
    delete file;
    return NULL;
  }
  return file;
}

DECLARE_APP(MMBoardApp)

wxSoundStream *MMBoardManager::OpenSoundStream()
{
#ifdef __UNIX__
  if ((wxGetApp().m_caps & MM_SOUND_ESD) != 0)
    return new wxSoundStreamESD();

  if ((wxGetApp().m_caps & MM_SOUND_OSS) != 0)
    return new wxSoundStreamOSS();
#endif

#ifdef __WIN32__
  if ((wxGetApp().m_caps & MM_SOUND_WIN) != 0)
    return new wxSoundStreamWin();
#endif

  wxMessageBox("You are trying to open a multimedia but you have not devices", "Error", wxOK | wxICON_ERROR, NULL);

  return NULL;
}

void MMBoardManager::UnrefSoundStream(wxSoundStream *stream)
{
  delete stream;
}

// ----------------------------------------------------------------------------

