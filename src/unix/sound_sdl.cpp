/////////////////////////////////////////////////////////////////////////////
// Name:        sound_sdl.cpp
// Purpose:     wxSound backend using SDL
// Author:      Vaclav Slavik
// Modified by:
// Created:     2004/01/31
// RCS-ID:      $Id$
// Copyright:   (c) 2004, Vaclav Slavik
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/setup.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#if wxUSE_WAVE && wxUSE_LIBSDL

#include <SDL.h>

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

#include "wx/thread.h"
#include "wx/module.h"
#include "wx/sound.h"

// ----------------------------------------------------------------------------
// wxSoundBackendSDL, for Unix with libSDL
// ----------------------------------------------------------------------------

class wxSoundBackendSDLNotification : public wxEvent
{
public:
    DECLARE_DYNAMIC_CLASS(wxSoundBackendSDLNotification)
    wxSoundBackendSDLNotification();
	wxEvent *Clone() const { return new wxSoundBackendSDLNotification(*this); }
};

typedef void (wxEvtHandler::*wxSoundBackendSDLNotificationFunction)
             (wxSoundBackendSDLNotification&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION, -1)
END_DECLARE_EVENT_TYPES()

#define EVT_SOUND_BACKEND_SDL_NOTIFICATON(func) \
    DECLARE_EVENT_TABLE_ENTRY(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION, \
                              -1,                       \
                              -1,                       \
                              (wxObjectEventFunction)   \
                              (wxSoundBackendSDLNotificationFunction)& func, \
                              (wxObject *) NULL ),

IMPLEMENT_DYNAMIC_CLASS(wxSoundBackendSDLNotification, wxEvtHandler)
DEFINE_EVENT_TYPE(wxEVT_SOUND_BACKEND_SDL_NOTIFICATION)

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
          m_evtHandler(NULL) {}
    virtual ~wxSoundBackendSDL();
    
    wxString GetName() const { return _T("Simple DirectMedia Layer"); }
    int GetPriority() const { return 9; }
    bool IsAvailable() const;
    bool HasNativeAsyncPlayback() const { return true; }
    bool Play(wxSoundData *data, unsigned flags);

    void FillAudioBuffer(Uint8 *stream, int len);
    void Stop();
    bool IsPlaying() const { return m_playing; }
    
private:
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
        wxLogTrace(_T("sound"),
                   _T("received playback status change notification"));
        m_backend->Stop();
    }
    wxSoundBackendSDL *m_backend;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSoundBackendSDLEvtHandler, wxEvtHandler)
    EVT_SOUND_BACKEND_SDL_NOTIFICATON(wxSoundBackendSDLEvtHandler::OnNotify)
END_EVENT_TABLE()

wxSoundBackendSDL::~wxSoundBackendSDL()
{
    Stop();
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
    wxLogTrace(_T("sound"), _T("initialized SDL audio subsystem"));
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

bool wxSoundBackendSDL::Play(wxSoundData *data, unsigned flags)
{
    int format;
    if (data->m_bitsPerSample == 8)
        format = AUDIO_U8;
    else if (data->m_bitsPerSample == 16)
        format = AUDIO_S16LSB;
    else
        return false;
    
    SDL_LockAudio();

    if (!m_evtHandler)
        m_evtHandler = new wxSoundBackendSDLEvtHandler(this);

    data->IncRef();

    bool needsOpen = true;
    if (m_audioOpen)
    {
        wxLogTrace(_T("sound"), _T("another sound playing, will be stopped"));
        if (format == m_spec.format &&
            m_spec.freq == (int)data->m_samplingRate &&
            m_spec.channels == data->m_channels)
        {
            needsOpen = false;
        }
        else
        {
            SDL_CloseAudio();
            m_audioOpen = false;
            wxLogTrace(_T("sound"), _T("closed audio"));
        }
        m_data->DecRef();
    }
   
    m_playing = true;
    m_data = data;
    m_pos = 0;
    m_loop = (flags & wxSOUND_LOOP);
    wxLogTrace(_T("sound"), _T("prepared sound for playback"));

    bool status = true;

    if (needsOpen)
    {
        m_spec.format = format;
        m_spec.freq = data->m_samplingRate;
        m_spec.channels = data->m_channels;
        m_spec.silence = 0;
        m_spec.samples = 4096;
        m_spec.size = 0;
        m_spec.callback = wx_sdl_audio_callback;
        m_spec.userdata = (void*)this;
                
        wxLogTrace(_T("sound"), _T("opening SDL audio..."));
        status = (SDL_OpenAudio(&m_spec, NULL) >= 0);
        if (status)
        {
#if wxUSE_LOG_DEBUG
            char driver[256];
            SDL_AudioDriverName(driver, 256);                    
            wxLogTrace(_T("sound"), _T("opened audio, driver '%s'"),
                       wxString(driver, wxConvLocal).c_str());
#endif
            m_audioOpen = true;
            SDL_PauseAudio(0);
        }
        else
        {
            wxString err(SDL_GetError(), wxConvLocal);
            wxLogError(_("Couldn't open audio: %s"), err.c_str());
        }
    }

    SDL_UnlockAudio();

    if (!status)
        return false;

    // wait until playback finishes if called in sync mode:
    if (!(flags & wxSOUND_ASYNC))
    {
        wxLogTrace(_T("sound"), _T("waiting for sample to finish"));
        while (m_playing)
        {
#if wxUSE_THREADS
            // give the playback thread a chance to add event to pending
            // events queue, release GUI lock temporarily:
            if (wxThread::IsMain())
                wxMutexGuiLeave();
#endif
            wxUsleep(10);
#if wxUSE_THREADS
            if (wxThread::IsMain())
                wxMutexGuiEnter();
#endif
        }
        wxLogTrace(_T("sound"), _T("sample finished"));
    }

    return true;
}

void wxSoundBackendSDL::Stop()
{
    SDL_LockAudio();

    if (m_audioOpen)
    {
        SDL_CloseAudio();
        m_audioOpen = false;
        wxLogTrace(_T("sound"), _T("closed audio"));
        m_data->DecRef();
    }

    SDL_UnlockAudio();
}

extern "C" wxSoundBackend *wxCreateSoundBackendSDL()
{
    return new wxSoundBackendSDL();
}

#endif // wxUSE_WAVE && wxUSE_LIBSDL
