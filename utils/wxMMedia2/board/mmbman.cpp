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

// Personal headers

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

#include "vidbase.h"
#ifdef __UNIX__
#include "vidxanm.h"
#endif

#ifdef __WIN32__
#include "vidwin.h"
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
  wxUint8 m_file_type;
};

class MMBoardVideoFile: public MMBoardFile {
public:
  MMBoardVideoFile(const wxString& filename);
  ~MMBoardVideoFile();

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
  wxWindow *m_output_window;
  wxInputStream *m_input_stream;
  wxVideoBaseDriver *m_video_driver;
};

// ----------------------------------------------------------------------------
// Implementation
// ----------------------------------------------------------------------------

#define MMBoard_UNKNOWNTYPE 0
#define MMBoard_WAVE 1
#define MMBoard_AIFF 2

// ----------------------------------------------------------------------------
// MMBoardSoundFile

MMBoardSoundFile::MMBoardSoundFile(const wxString& filename)
  : MMBoardFile()
{
  m_input_stream = new wxFileInputStream(filename);
  m_output_stream = MMBoardManager::OpenSoundStream();

  m_file_stream = GetDecoder();
  
  if (!m_file_stream) {
    SetError(MMBoard_UnknownFile);
    return;
  }
    
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
  if (m_file_stream)
    delete m_file_stream;
  MMBoardManager::UnrefSoundStream(m_output_stream);
  delete m_input_stream;
}

wxSoundFileStream *MMBoardSoundFile::GetDecoder()
{
  wxSoundFileStream *f_stream;

  // First, we try a Wave decoder
  f_stream = new wxSoundWave(*m_input_stream, *m_output_stream);
  m_file_type = MMBoard_WAVE;
  if (f_stream->CanRead())
    return f_stream;
  delete f_stream;

  // Then, a AIFF decoder
  f_stream = new wxSoundAiff(*m_input_stream, *m_output_stream);
  m_file_type = MMBoard_AIFF;
  if (f_stream->CanRead())
    return f_stream;
  delete f_stream;

  m_file_type = MMBoard_UNKNOWNTYPE;

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
  switch (m_file_type) {
  case MMBoard_WAVE:
    return wxString(wxT("WAVE file"));
    break;
  case MMBoard_AIFF:
    return wxString(wxT("AIFF file"));
    break;
  default:
    return wxString(wxT("Unknown file"));
    break;
  }
}

wxString MMBoardSoundFile::GetStringInformation()
{
    wxString info;
    wxSoundFormatBase *format;
    
    format = &(m_file_stream->GetSoundFormat());
    
    info = wxT("Data encoding: ");
    switch (format->GetType()) {
    case wxSOUND_PCM: {
        wxSoundFormatPcm *pcm_format = (wxSoundFormatPcm *)format;
      
	info += wxT("PCM\n");
	info += wxString::Format(wxT("Sampling rate: %d\n")
				 wxT("Bits per sample: %d\n")
				 wxT("Number of channels: %d\n"),
				 pcm_format->GetSampleRate(),
				 pcm_format->GetBPS(),
				 pcm_format->GetChannels());
	
	break;
    }
    case wxSOUND_ULAW: {
        wxSoundFormatUlaw *ulaw_format = (wxSoundFormatUlaw *)format;
	info += wxT("ULAW\n");
	info += wxString::Format(wxT("Sampling rate: %d\n"), ulaw_format->GetSampleRate());
	break;
    }
    default:
        info += wxT("Unknown");
	break;
    }
    return info;
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// MMBoardVideoFile

MMBoardVideoFile::MMBoardVideoFile(const wxString& filename)
{
    m_output_window = NULL;
  
#if defined(__UNIX__)
    m_video_driver = new wxVideoXANIM(filename);
#elif defined(__WIN32__)
    m_video_driver = new wxVideoWindows(filename);
#else
    m_video_driver = NULL;
    SetError(MMBoard_UnknownFile);
#endif
}

MMBoardVideoFile::~MMBoardVideoFile()
{
    if (m_video_driver)
        delete m_video_driver;

    delete m_input_stream;
}

bool MMBoardVideoFile::NeedWindow()
{
    return TRUE;
}

void MMBoardVideoFile::SetWindow(wxWindow *window)
{
    m_output_window = window;
    m_video_driver->AttachOutput(*window);
}

void MMBoardVideoFile::Play()
{
    m_video_driver->Play();
}

void MMBoardVideoFile::Pause()
{
    m_video_driver->Pause();
}

void MMBoardVideoFile::Resume()
{
    m_video_driver->Resume();
}

void MMBoardVideoFile::Stop()
{
    m_video_driver->Stop();
}

MMBoardTime MMBoardVideoFile::GetPosition()
{
    MMBoardTime btime;

    btime.seconds = btime.minutes = btime.hours = 0;
    return btime;
}

MMBoardTime MMBoardVideoFile::GetLength()
{
    MMBoardTime btime;

    btime.seconds = 1;
    btime.minutes = btime.hours = 0;
    return btime;
}

bool MMBoardVideoFile::IsStopped()
{
    return m_video_driver->IsStopped();
}

bool MMBoardVideoFile::IsPaused()
{
    return m_video_driver->IsPaused();
}

wxString MMBoardVideoFile::GetStringType()
{
    return wxString(wxT("Video XANIM"));
}

wxString MMBoardVideoFile::GetStringInformation()
{
    return wxString(wxT("No info"));
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

  // Test the audio codec
  file = new MMBoardSoundFile(filename);
  if (!file->GetError())
    return file;
  delete file;

  // Test the video codec
  file = new MMBoardVideoFile(filename);
  if (!file->GetError())
    return file;
  delete file;

  // Arrrgh, we just could not see what is that file ...
  return NULL;
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

