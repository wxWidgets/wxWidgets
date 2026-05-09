/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/sound_sdl.cpp
// Purpose:     wxSound backend using SDL
// Author:      Vaclav Slavik
// Created:     2004/01/31
// Copyright:   (c) 2004, Open Source Applications Foundation
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_SOUND && wxUSE_LIBSDL

#if wxUSE_LIBSDL3
    #include <SDL3/SDL.h>
#else
    #include <SDL.h>
#endif

// Compatibility helpers so the rest of this file can be written once.
#if SDL_MAJOR_VERSION >= 3
    typedef SDL_AudioFormat       wxSDL_AudioFormat;
    #define wxSDL_AUDIO_U8        SDL_AUDIO_U8
    #define wxSDL_AUDIO_S16LSB    SDL_AUDIO_S16LE
    #define WX_SDL_USE_STREAM_API 1
#else
    typedef int                   wxSDL_AudioFormat;
    #define wxSDL_AUDIO_U8        AUDIO_U8
    #define wxSDL_AUDIO_S16LSB    AUDIO_S16LSB
    #define WX_SDL_USE_STREAM_API 0
#endif

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
    wxEvent *Clone() const override { return new wxSoundBackendSDLNotification(*this); }
};

typedef void (wxEvtHandler::*wxSoundBackendSDLNotificationFunction)
             (wxSoundBackendSDLNotification&);

wxDECLARE_EVENT(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION, wxSoundBackendSDLNotification);

#define EVT_SOUND_BACKEND_SDL_NOTIFICATON(func) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION, \
                              -1,                       \
                              -1,                       \
                              wxEVENT_HANDLER_CAST( wxSoundBackendSDLNotificationFunction, func ), \
                              nullptr ),

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
          m_data(nullptr),
#if WX_SDL_USE_STREAM_API
          m_stream(nullptr), m_silence(0),
#endif
          m_evtHandler(nullptr) {}
    virtual ~wxSoundBackendSDL();

    wxString GetName() const override { return wxT("Simple DirectMedia Layer"); }
    int GetPriority() const override { return 9; }
    bool IsAvailable() const override;
    bool HasNativeAsyncPlayback() const override { return true; }
    bool Play(wxSoundData *data, unsigned flags,
              volatile wxSoundPlaybackStatus *status) override;

    void FillAudioBuffer(Uint8 *stream, int len);
    void FinishedPlayback();

    void Stop() override;
    bool IsPlaying() const override { return m_playing; }

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

#if WX_SDL_USE_STREAM_API
    // SDL3 owns the device through the audio stream; silence sample is no
    // longer part of SDL_AudioSpec, so we keep our own copy.
    SDL_AudioStream             *m_stream;
    Uint8                        m_silence;
#endif

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
    if ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) != SDL_INIT_AUDIO)
    {
        SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
#if SDL_MAJOR_VERSION >= 3
        // SDL3: SDL_Init returns bool, and SDL_INIT_NOPARACHUTE is gone.
        if (!SDL_Init(SDL_INIT_AUDIO))
            return false;
#else
        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) == -1)
            return false;
#endif
    }
    wxConstCast(this, wxSoundBackendSDL)->m_initialized = true;
    wxLogTrace(wxT("sound"), wxT("initialized SDL audio subsystem"));
    return true;
}

#if WX_SDL_USE_STREAM_API
extern "C" void SDLCALL
wx_sdl_audio_callback(void *userdata, SDL_AudioStream *stream,
                      int additional_amount, int /*total_amount*/)
{
    if (additional_amount <= 0)
        return;
    wxSoundBackendSDL *bk = static_cast<wxSoundBackendSDL*>(userdata);
    Uint8 *buf = static_cast<Uint8*>(SDL_malloc(additional_amount));
    if (!buf)
        return;
    bk->FillAudioBuffer(buf, additional_amount);
    SDL_PutAudioStreamData(stream, buf, additional_amount);
    SDL_free(buf);
}
#else
extern "C" void wx_sdl_audio_callback(void *userdata, Uint8 *stream, int len)
{
    wxSoundBackendSDL *bk = (wxSoundBackendSDL*)userdata;
    bk->FillAudioBuffer(stream, len);
}
#endif

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
#if WX_SDL_USE_STREAM_API
            memset(stream, m_silence, len);
#else
            memset(stream, m_spec.silence, len);
#endif
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

#if WX_SDL_USE_STREAM_API
        // SDL3: silence and buffering are handled by the stream itself, and
        // SDL_AudioSpec only carries format/channels/freq.
        m_silence = (m_spec.format == wxSDL_AUDIO_U8) ? 128 : 0;

        wxLogTrace(wxT("sound"), wxT("opening SDL audio..."));
        m_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                             &m_spec,
                                             wx_sdl_audio_callback,
                                             this);
        if (m_stream)
        {
#if wxUSE_LOG_DEBUG
            const char *drv = SDL_GetCurrentAudioDriver();
            wxLogTrace(wxT("sound"), wxT("opened audio, driver '%s'"),
                       wxString(drv ? drv : "", wxConvLocal));
#endif
            m_audioOpen = true;
            return true;
        }
        else
        {
            wxString err(SDL_GetError(), wxConvLocal);
            wxLogError(_("Couldn't open audio: %s"), err);
            return false;
        }
#else
        m_spec.silence = 0;
        m_spec.samples = 4096;
        m_spec.size = 0;
        m_spec.callback = wx_sdl_audio_callback;
        m_spec.userdata = (void*)this;

        wxLogTrace(wxT("sound"), wxT("opening SDL audio..."));
        if (SDL_OpenAudio(&m_spec, nullptr) >= 0)
        {
#if wxUSE_LOG_DEBUG
            char driver[256];
#if SDL_MAJOR_VERSION == 1
            SDL_AudioDriverName(driver, 256);
#elif SDL_MAJOR_VERSION > 1
            wxStrlcpy(driver, SDL_GetCurrentAudioDriver(), 256);
#endif
            wxLogTrace(wxT("sound"), wxT("opened audio, driver '%s'"),
                       wxString(driver, wxConvLocal));
#endif
            m_audioOpen = true;
            return true;
        }
        else
        {
            wxString err(SDL_GetError(), wxConvLocal);
            wxLogError(_("Couldn't open audio: %s"), err);
            return false;
        }
#endif
    }
    return true;
}

void wxSoundBackendSDL::CloseAudio()
{
    if (m_audioOpen)
    {
#if WX_SDL_USE_STREAM_API
        SDL_DestroyAudioStream(m_stream);
        m_stream = nullptr;
#else
        SDL_CloseAudio();
#endif
        wxLogTrace(wxT("sound"), wxT("closed audio"));
        m_audioOpen = false;
    }
}

bool wxSoundBackendSDL::Play(wxSoundData *data, unsigned flags,
                             volatile wxSoundPlaybackStatus *WXUNUSED(status))
{
    Stop();

    wxSDL_AudioFormat format;
    if (data->m_bitsPerSample == 8)
        format = wxSDL_AUDIO_U8;
    else if (data->m_bitsPerSample == 16)
        format = wxSDL_AUDIO_S16LSB;
    else
        return false;

    bool needsOpen = true;
    if (m_audioOpen)
    {
        if (format == m_spec.format &&
            m_spec.freq == (int)data->m_samplingRate &&
            (unsigned)m_spec.channels == data->m_channels)
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

#if WX_SDL_USE_STREAM_API
    SDL_LockAudioStream(m_stream);
#else
    SDL_LockAudio();
#endif
    wxLogTrace(wxT("sound"), wxT("playing new sound"));
    m_playing = true;
    m_pos = 0;
    m_loop = (flags & wxSOUND_LOOP);
    m_data = data;
    data->IncRef();
#if WX_SDL_USE_STREAM_API
    SDL_UnlockAudioStream(m_stream);
    SDL_ResumeAudioStreamDevice(m_stream);
#else
    SDL_UnlockAudio();
    SDL_PauseAudio(0);
#endif

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
#if WX_SDL_USE_STREAM_API
    if (m_stream)
    {
        SDL_LockAudioStream(m_stream);
        SDL_PauseAudioStreamDevice(m_stream);
    }
    m_playing = false;
    if (m_data)
    {
        m_data->DecRef();
        m_data = nullptr;
    }
    if (m_stream)
        SDL_UnlockAudioStream(m_stream);
#else
    SDL_LockAudio();
    SDL_PauseAudio(1);
    m_playing = false;
    if (m_data)
    {
        m_data->DecRef();
        m_data = nullptr;
    }
    SDL_UnlockAudio();
#endif
}

extern "C" wxSoundBackend *wxCreateSoundBackendSDL()
{
    return new wxSoundBackendSDL();
}

#endif // wxUSE_SOUND && wxUSE_LIBSDL
