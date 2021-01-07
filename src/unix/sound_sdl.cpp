/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/sound_sdl.cpp
// Purpose:     wxSound backend using SDL
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/01/31
// Copyright:   (c) 2004, Open Source Applications Foundation
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_SOUND && wxUSE_LIBSDL

#include <SDL.h>

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/module.h"
#endif

#include "wx/thread.h"
#include "wx/sound.h"

// ----------------------------------------------------------------------------
// wxSoundBackendSDL, for Unix with libSDL
// ----------------------------------------------------------------------------

class wxSoundBackendSDLNotification : public wxEvent
{
public:
    wxDECLARE_DYNAMIC_CLASS(wxSoundBackendSDLNotification);
    wxSoundBackendSDLNotification();
    wxEvent *Clone() const wxOVERRIDE { return new wxSoundBackendSDLNotification(*this); }
};

typedef void (wxEvtHandler::*wxSoundBackendSDLNotificationFunction)
             (wxSoundBackendSDLNotification&);

wxDECLARE_EVENT(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION, wxSoundBackendSDLNotification);

#define EVT_SOUND_BACKEND_SDL_NOTIFICATON(func) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION, \
                              -1,                       \
                              -1,                       \
                              wxEVENT_HANDLER_CAST( wxSoundBackendSDLNotificationFunction, func ), \
                              NULL ),

wxIMPLEMENT_DYNAMIC_CLASS(wxSoundBackendSDLNotification, wxEvtHandler);
wxDEFINE_EVENT( wxEVT_SOUND_BACKEND_SDL_NOTIFICATION, wxSoundBackendSDLNotification );

wxSoundBackendSDLNotification::wxSoundBackendSDLNotification()
{
    SetEventType(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION);
}

class wxSoundBackendSDLEvtHandler;

class wxSoundBackendSDL : public wxSoundBackend
{
public:
    wxSoundBackendSDL()
        : m_initialized(false), m_playing(false), m_audioOpen(false),
          m_data(NULL), m_evtHandler(NULL) {}
    virtual ~wxSoundBackendSDL();

    wxString GetName() const wxOVERRIDE { return wxT("Simple DirectMedia Layer"); }
    int GetPriority() const wxOVERRIDE { return 9; }
    bool IsAvailable() const wxOVERRIDE;
    bool HasNativeAsyncPlayback() const wxOVERRIDE { return true; }
    bool Play(wxSoundData *data, unsigned flags,
              volatile wxSoundPlaybackStatus *status) wxOVERRIDE;

    void FillAudioBuffer(Uint8 *stream, int len);
    void FinishedPlayback();

    void Stop() wxOVERRIDE;
    bool IsPlaying() const wxOVERRIDE { return m_playing; }

private:
    bool OpenAudio();
    void CloseAudio();

    bool                        m_initialized;
    bool                        m_playing, m_audioOpen;
    // playback information:
    wxSoundData                 *m_data;
    unsigned                     m_pos;
    SDL_AudioSpec                m_spec;
    bool                         m_loop;

    wxSoundBackendSDLEvtHandler *m_evtHandler;
};

class wxSoundBackendSDLEvtHandler : public wxEvtHandler
{
public:
    wxSoundBackendSDLEvtHandler(wxSoundBackendSDL *bk) : m_backend(bk) {}

private:
    void OnNotify(wxSoundBackendSDLNotification& WXUNUSED(event))
    {
        wxLogTrace(wxT("sound"),
                   wxT("received playback status change notification"));
        m_backend->FinishedPlayback();
    }
    wxSoundBackendSDL *m_backend;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSoundBackendSDLEvtHandler, wxEvtHandler)
    EVT_SOUND_BACKEND_SDL_NOTIFICATON(wxSoundBackendSDLEvtHandler::OnNotify)
wxEND_EVENT_TABLE()

wxSoundBackendSDL::~wxSoundBackendSDL()
{
    Stop();
    CloseAudio();
    delete m_evtHandler;
}

bool wxSoundBackendSDL::IsAvailable() const
{
    if (m_initialized)
        return true;
    if (SDL_WasInit(SDL_INIT_AUDIO) != SDL_INIT_AUDIO)
    {
        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) == -1)
            return false;
    }
    wxConstCast(this, wxSoundBackendSDL)->m_initialized = true;
    wxLogTrace(wxT("sound"), wxT("initialized SDL audio subsystem"));
    return true;
}

extern "C" void wx_sdl_audio_callback(void *userdata, Uint8 *stream, int len)
{
    wxSoundBackendSDL *bk = (wxSoundBackendSDL*)userdata;
    bk->FillAudioBuffer(stream, len);
}

void wxSoundBackendSDL::FillAudioBuffer(Uint8 *stream, int len)
{
    if (m_playing)
    {
        // finished playing the sample
        if (m_pos == m_data->m_dataBytes)
        {
            m_playing = false;
            wxSoundBackendSDLNotification event;
            m_evtHandler->AddPendingEvent(event);
        }
        // still something to play
        else
        {
            unsigned size = ((len + m_pos) < m_data->m_dataBytes) ?
                            len :
                            (m_data->m_dataBytes - m_pos);
            memcpy(stream, m_data->m_data + m_pos, size);
            m_pos += size;
            len -= size;
            stream += size;
        }
    }
    // the sample doesn't play, fill the buffer with silence and wait for
    // the main thread to shut the playback down:
    if (len > 0)
    {
        if (m_loop)
        {
            m_pos = 0;
            FillAudioBuffer(stream, len);
            return;
        }
        else
        {
            memset(stream, m_spec.silence, len);
        }
    }
}

void wxSoundBackendSDL::FinishedPlayback()
{
    if (!m_playing)
        Stop();
}

bool wxSoundBackendSDL::OpenAudio()
{
    if (!m_audioOpen)
    {
        if (!m_evtHandler)
            m_evtHandler = new wxSoundBackendSDLEvtHandler(this);

        m_spec.silence = 0;
        m_spec.samples = 4096;
        m_spec.size = 0;
        m_spec.callback = wx_sdl_audio_callback;
        m_spec.userdata = (void*)this;

        wxLogTrace(wxT("sound"), wxT("opening SDL audio..."));
        if (SDL_OpenAudio(&m_spec, NULL) >= 0)
        {
#if wxUSE_LOG_DEBUG
            char driver[256];
#if SDL_MAJOR_VERSION == 1
            SDL_AudioDriverName(driver, 256);
#elif SDL_MAJOR_VERSION > 1            
            wxStrlcpy(driver, SDL_GetCurrentAudioDriver(), 256);
#endif
            wxLogTrace(wxT("sound"), wxT("opened audio, driver '%s'"),
                       wxString(driver, wxConvLocal).c_str());
#endif
            m_audioOpen = true;
            return true;
        }
        else
        {
            wxString err(SDL_GetError(), wxConvLocal);
            wxLogError(_("Couldn't open audio: %s"), err.c_str());
            return false;
        }
    }
    return true;
}

void wxSoundBackendSDL::CloseAudio()
{
    if (m_audioOpen)
    {
        SDL_CloseAudio();
        wxLogTrace(wxT("sound"), wxT("closed audio"));
        m_audioOpen = false;
    }
}

bool wxSoundBackendSDL::Play(wxSoundData *data, unsigned flags,
                             volatile wxSoundPlaybackStatus *WXUNUSED(status))
{
    Stop();

    int format;
    if (data->m_bitsPerSample == 8)
        format = AUDIO_U8;
    else if (data->m_bitsPerSample == 16)
        format = AUDIO_S16LSB;
    else
        return false;

    bool needsOpen = true;
    if (m_audioOpen)
    {
        if (format == m_spec.format &&
            m_spec.freq == (int)data->m_samplingRate &&
            m_spec.channels == data->m_channels)
        {
            needsOpen = false;
        }
        else
        {
            CloseAudio();
        }
    }

    if (needsOpen)
    {
        m_spec.format = format;
        m_spec.freq = data->m_samplingRate;
        m_spec.channels = data->m_channels;
        if (!OpenAudio())
            return false;
    }

    SDL_LockAudio();
    wxLogTrace(wxT("sound"), wxT("playing new sound"));
    m_playing = true;
    m_pos = 0;
    m_loop = (flags & wxSOUND_LOOP);
    m_data = data;
    data->IncRef();
    SDL_UnlockAudio();

    SDL_PauseAudio(0);

    // wait until playback finishes if called in sync mode:
    if (!(flags & wxSOUND_ASYNC))
    {
        wxLogTrace(wxT("sound"), wxT("waiting for sample to finish"));
        while (m_playing && m_data == data)
        {
#if wxUSE_THREADS
            // give the playback thread a chance to add event to pending
            // events queue, release GUI lock temporarily:
            if (wxThread::IsMain())
                wxMutexGuiLeave();
#endif
            wxMilliSleep(10);
#if wxUSE_THREADS
            if (wxThread::IsMain())
                wxMutexGuiEnter();
#endif
        }
        wxLogTrace(wxT("sound"), wxT("sample finished"));
    }

    return true;
}

void wxSoundBackendSDL::Stop()
{
    SDL_LockAudio();
    SDL_PauseAudio(1);
    m_playing = false;
    if (m_data)
    {
        m_data->DecRef();
        m_data = NULL;
    }
    SDL_UnlockAudio();
}

extern "C" wxSoundBackend *wxCreateSoundBackendSDL()
{
    return new wxSoundBackendSDL();
}

#endif // wxUSE_SOUND && wxUSE_LIBSDL
