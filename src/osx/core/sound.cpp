/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/sound.cpp
// Purpose:     wxSound class implementation using AudioToolbox
// Author:      Stefan Csomor
// Modified by: Stefan Csomor
// Created:     2009-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOUND

#include "wx/sound.h"

#if wxOSX_USE_AUDIOTOOLBOX

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/timer.h"
#endif

#include "wx/file.h"

#include "wx/osx/private.h"

#include <AudioToolbox/AudioToolbox.h>

class wxOSXAudioToolboxSoundData : public wxSoundData
{
public:
    wxOSXAudioToolboxSoundData(const wxString& fileName);

    ~wxOSXAudioToolboxSoundData();

    virtual bool Play(unsigned flags) wxOVERRIDE;

    virtual void DoStop() wxOVERRIDE;
protected:
    static void CompletionCallback(SystemSoundID  mySSID, void * soundRef);
    void SoundCompleted();

    SystemSoundID m_soundID;
    wxString m_sndname; //file path
    bool m_playing;
};

wxOSXAudioToolboxSoundData::wxOSXAudioToolboxSoundData(const wxString& fileName) :
    m_soundID(NULL)
{
    m_sndname = fileName;
    m_playing = false;
}

wxOSXAudioToolboxSoundData::~wxOSXAudioToolboxSoundData()
{
    DoStop();
}

void
wxOSXAudioToolboxSoundData::CompletionCallback(SystemSoundID WXUNUSED(mySSID),
                                               void * soundRef)
{
    wxOSXAudioToolboxSoundData* data = (wxOSXAudioToolboxSoundData*) soundRef;

    data->SoundCompleted();

    if (data->IsMarkedForDeletion())
        delete data;
}

void wxOSXAudioToolboxSoundData::SoundCompleted()
{
    if ( m_flags & wxSOUND_ASYNC )
    {
        if (m_flags & wxSOUND_LOOP)
            AudioServicesPlaySystemSound(m_soundID);
        else
            Stop();
    }
    else
    {
        Stop();
        CFRunLoopStop(CFRunLoopGetCurrent());
    }

}

void wxOSXAudioToolboxSoundData::DoStop()
{
    if ( m_playing )
    {
        m_playing = false;
        AudioServicesDisposeSystemSoundID (m_soundID);

        wxSound::SoundStopped(this);
    }
}

bool wxOSXAudioToolboxSoundData::Play(unsigned flags)
{
    Stop();

    m_flags = flags;

    wxCFRef<CFMutableStringRef> cfMutableString(CFStringCreateMutableCopy(NULL, 0, wxCFStringRef(m_sndname)));
    CFStringNormalize(cfMutableString,kCFStringNormalizationFormD);
    wxCFRef<CFURLRef> url(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfMutableString , kCFURLPOSIXPathStyle, false));

    OSStatus err = AudioServicesCreateSystemSoundID(url, &m_soundID);
    if ( err != 0 )
    {
        wxLogError(_("Failed to load sound from \"%s\" (error %d)."), m_sndname, err);
        return false;
    }

    AudioServicesAddSystemSoundCompletion( m_soundID, CFRunLoopGetCurrent(), NULL, wxOSXAudioToolboxSoundData::CompletionCallback, (void *) this );

    bool sync = !(flags & wxSOUND_ASYNC);

    m_playing = true;

    AudioServicesPlaySystemSound(m_soundID);

    if ( sync )
    {
        while ( m_playing )
        {
            CFRunLoopRun();
        }
    }

    return true;
}

bool wxSound::Create(size_t WXUNUSED(size), const void* WXUNUSED(data))
{
    wxFAIL_MSG( "not implemented" );

    return false;
}

bool wxSound::Create(const wxString& fileName, bool isResource)
{
    wxCHECK_MSG( !isResource, false, "not implemented" );

    m_data = new wxOSXAudioToolboxSoundData(fileName);
    return true;
}

#endif // wxOSX_USE_AUDIOTOOLBOX

#endif //wxUSE_SOUND
