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

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// Personal headers

#include "wx/stream.h"
#include "wx/wfstream.h"

#include "wx/mmedia/sndbase.h"
#include "wx/mmedia/sndfile.h"
#include "wx/mmedia/sndwav.h"
#include "wx/mmedia/sndaiff.h"
#include "wx/mmedia/sndpcm.h"
#include "wx/mmedia/sndulaw.h"
#include "wx/mmedia/sndmsad.h"

#ifdef __UNIX__
#include "wx/mmedia/sndoss.h"
#include "wx/mmedia/sndesd.h"
#endif

#ifdef __WIN32__
#include "wx/mmedia/sndwin.h"
#endif

#include "wx/mmedia/vidbase.h"
#ifdef __UNIX__
#include "wx/mmedia/vidxanm.h"
#endif

#ifdef __WIN32__
#include "wx/mmedia/vidwin.h"
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
    void SetPosition(MMBoardTime btime);

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
    void SetPosition(MMBoardTime btime);

    bool IsStopped();
    bool IsPaused();

    wxString GetStringType();
    wxString GetStringInformation();

protected:
    wxWindow *m_output_window;
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

void MMBoardSoundFile::SetPosition(MMBoardTime btime)
{
    wxUint32 itime;

    itime = btime.seconds + btime.minutes * 60 + btime.hours;

    m_file_stream->SetPosition(
        m_file_stream->GetSoundFormat().GetBytesFromTime(itime)
        );
}

bool MMBoardSoundFile::NeedWindow()
{
    return false;
}

void MMBoardSoundFile::SetWindow(wxWindow *WXUNUSED(window))
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
            #if 0
            // break is not reachable after return
            break;
            #endif
        case MMBoard_AIFF:
            return wxString(wxT("AIFF file"));
            #if 0
            // break is not reachable after return
            break;
            #endif
        #if 0
        // default moved outside switch for those compilers
        // which complain about lack of return in function
        default:
            return wxString(wxT("Unknown file"));
            break;
        #endif
    }
    return wxString(wxT("Unknown file"));
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

    info += wxString::Format(wxT("PCM %s %s\n"),
                                 pcm_format->Signed() ? wxT("signed") : wxT("unsigned"),
                                 pcm_format->GetOrder() == wxLITTLE_ENDIAN ? wxT("little endian") : wxT("big endian"));
    info += wxString::Format(wxT("Sampling rate: %d\n")
                 wxT("Bits per sample: %d\n")
                 wxT("Number of channels: %d\n"),
                 pcm_format->GetSampleRate(),
                 pcm_format->GetBPS(),
                 pcm_format->GetChannels());

    break;
    }
    case wxSOUND_MSADPCM: {
        wxSoundFormatMSAdpcm *adpcm_format = (wxSoundFormatMSAdpcm *)format;

        info += wxString::Format(wxT("Microsoft ADPCM\n"));
        info += wxString::Format(wxT("Sampling Rate: %d\n")
                                 wxT("Number of channels: %d\n"),
                                 adpcm_format->GetSampleRate(),
                                 adpcm_format->GetChannels());
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

#if defined(__UNIX__) && !defined(__CYGWIN__)
    m_video_driver = new wxVideoXANIM(filename);
#elif defined(__WINDOWS__) && !defined(__MINGW32__) && !defined(__WATCOMC__) && !defined(__CYGWIN__)
    // versions of Open Watcom and MinGW tested against this source does not
    // deliver "digitalv.h" required in this feature
    m_video_driver = new wxVideoWindows(filename);
#else
    wxUnusedVar(filename);
    m_video_driver = NULL;
    SetError(MMBoard_UnknownFile);
#endif
}

MMBoardVideoFile::~MMBoardVideoFile()
{
    if (m_video_driver)
        delete m_video_driver;
}

bool MMBoardVideoFile::NeedWindow()
{
    return true;
}

void MMBoardVideoFile::SetWindow(wxWindow *window)
{
    m_output_window = window;
    m_video_driver->AttachOutput(*window);

    wxSize size;
    m_video_driver->GetSize(size);
    window->SetSize(size);
    // BAD BAD
    // and we remove
    // window->GetParent()->GetSizer()->Fit(window->GetParent());
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
    int frameTime;

    frameTime = (int)( m_video_driver->GetNbFrames() / m_video_driver->GetFrameRate());

    btime.seconds = frameTime % 60;
    btime.minutes = (frameTime / 60) % 60;
    btime.hours   = frameTime / 3600;
    return btime;
}

void MMBoardVideoFile::SetPosition(MMBoardTime WXUNUSED(btime))
{
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
    wxString info;

    info = wxT("Video codec: ");
    info += m_video_driver->GetMovieCodec() + _T("\n");
    info += wxT("Audio codec: ");
    info += m_video_driver->GetAudioCodec();
    info += wxString::Format(_T(" Sample rate: %d Channels: %d\n"), m_video_driver->GetSampleRate(),
                             m_video_driver->GetBPS());
    info += wxString::Format(_T(" Frame rate: %.01f"), m_video_driver->GetFrameRate());
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
#ifdef __WIN32__
  if ((wxGetApp().m_caps & MM_SOUND_WIN) != 0)
    return new wxSoundStreamWin();
#elif __UNIX__
  if ((wxGetApp().m_caps & MM_SOUND_ESD) != 0)
    return new wxSoundStreamESD();

  if ((wxGetApp().m_caps & MM_SOUND_OSS) != 0)
    return new wxSoundStreamOSS();
#endif

  wxMessageBox(_T("You are trying to open a multimedia but you have not devices"), _T("Error"), wxOK | wxICON_ERROR, NULL);

  return NULL;
}

void MMBoardManager::UnrefSoundStream(wxSoundStream *stream)
{
  delete stream;
}

// ----------------------------------------------------------------------------

