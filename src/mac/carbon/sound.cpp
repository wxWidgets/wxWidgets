/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/sound.cpp
// Purpose:     wxSound class implementation: optional
// Author:      Ryan Norton
// Modified by: Stefan Csomor
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SOUND

#include "wx/sound.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/timer.h"
#endif

#include "wx/file.h"

// Carbon QT Implementation Details -
//
// Memory:
// 1) OpenDefaultComponent(MovieImportType, kQTFileTypeWave);
// 2) NewMovie(0);
// 3) MovieImportDataRef() //Pass Memory Location to this
// 4) PlayMovie();
// 5) IsMovieDone(), MoviesTask() //2nd param is minimum wait time to allocate to quicktime
//
// File:
// 1) Path as CFString
// 2) Call QTNewDataReferenceFromFullPathCFString
// 3) Call NewMovieFromDataRef
// 4) Call CloseMovieFile
// 4) PlayMovie();
// 5) IsMovieDone(), MoviesTask() //2nd param is minimum wait time to allocate to quicktime
//

#ifdef __WXMAC__
#include "wx/mac/uma.h"
#ifndef __DARWIN__
#include <Movies.h>
#include <Gestalt.h>
#endif
#endif

#if defined __WXMAC__ && defined __DARWIN__/*TARGET_CARBON*/
#ifdef __APPLE_CC__
#include <Carbon/Carbon.h>
#else
#include <Carbon.h>
#endif
#else
#include <Sound.h>
#endif

//quicktime media layer only required for mac emulation on pc
#ifndef __WXMAC__
#include <qtml.h>
#endif

#ifndef __DARWIN__
#include <QuickTimeComponents.h>
#else
#include <QuickTime/QuickTimeComponents.h>
#endif

//Time between timer calls
#define MOVIE_DELAY 100

static wxTimer* lastSoundTimer=NULL;
static bool lastSoundIsPlaying=false;

#if !defined(__DARWIN__) || !defined(__LP64__)
#define USE_QUICKTIME 1
#else
#define USE_QUICKTIME 0
#endif

#if USE_QUICKTIME

// ------------------------------------------------------------------
//          wxQTTimer - Handle Asyncronous Playing
// ------------------------------------------------------------------
class wxQTTimer : public wxTimer
{
public:
    wxQTTimer(Movie movie, bool bLoop, bool* playing) :
        m_movie(movie), m_bLoop(bLoop), m_pbPlaying(playing)
    {
    }

    virtual ~wxQTTimer()
    {
        if(m_pbPlaying)
            *m_pbPlaying = false;

        StopMovie(m_movie);
        DisposeMovie(m_movie);
        Stop();

        //Note that ExitMovies() is not necessary, but
        //the docs are fuzzy on whether or not TerminateQTML is
        ExitMovies();

#ifndef __WXMAC__
        TerminateQTML();
#endif
    }

    void Shutdown()
    {
        delete this;
    }

    void Notify()
    {
        if (m_pbPlaying && !*m_pbPlaying)
        {
            Shutdown();
        }

        if(IsMovieDone(m_movie))
        {
            if (!m_bLoop)
                Shutdown();
            else
            {
                StopMovie(m_movie);
                GoToBeginningOfMovie(m_movie);
                StartMovie(m_movie);
            }
        }
        else
            MoviesTask(m_movie, MOVIE_DELAY); //Give QT time to play movie
    }


    Movie& GetMovie() {return m_movie;}

protected:
    Movie m_movie;
    bool m_bLoop;

public:
    bool* m_pbPlaying;

};

// ------------------------------------------------------------------
//          wxSound
// ------------------------------------------------------------------

//Determines whether version 4 of QT is installed
Boolean wxIsQuickTime4Installed (void)
{
#ifdef __WXMAC__
    short error;
    long result;

    error = Gestalt (gestaltQuickTime, &result);
    return (error == noErr) && (((result >> 16) & 0xffff) >= 0x0400);
#else
    return true;
#endif
}

inline bool wxInitQT ()
{
    if (wxIsQuickTime4Installed())
    {
        #ifndef __WXMAC__
        int nError;
        //-2093 no dll
            if ((nError = InitializeQTML(0)) != noErr)
                wxLogSysError(wxString::Format(wxT("Couldn't Initialize Quicktime-%i"), nError));
        #endif
        EnterMovies();
        return true;
    }
    else
    {
        wxLogSysError(wxT("Quicktime is not installed, or Your Version of Quicktime is <= 4."));
        return false;
    }
}

#endif

wxSound::wxSound()
: m_hSnd(NULL), m_waveLength(0), m_pTimer(NULL), m_type(wxSound_NONE)
{
}

wxSound::wxSound(const wxString& sFileName, bool isResource)
: m_hSnd(NULL), m_waveLength(0), m_pTimer(NULL), m_type(wxSound_NONE)
{
    Create(sFileName, isResource);
}

wxSound::wxSound(int size, const wxByte* data)
: m_hSnd(NULL), m_waveLength(0), m_pTimer(NULL), m_type(wxSound_NONE)
{
   Create(size, data);
}

wxSound::~wxSound()
{
}

bool wxSound::Create(const wxString& fileName, bool isResource)
{
    Stop();

    m_sndname.Empty();

    if (isResource)
    {
#ifdef __WXMAC__
        CFURLRef url;
        CFStringRef path;

        url = CFBundleCopyResourceURL(CFBundleGetMainBundle(),
                                      wxMacCFStringHolder(fileName,wxLocale::GetSystemEncoding()),
                                      NULL,
                                      NULL);
        if (url)
        {
            path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
            CFRelease(url);
            m_type = wxSound_FILE;
            m_sndname = wxMacCFStringHolder(path).AsString(wxLocale::GetSystemEncoding());
        }
#endif
    }
    else
    {
        m_type = wxSound_FILE;
        m_sndname = fileName;
    }

    return !m_sndname.IsEmpty();
}

bool wxSound::Create(int size, const wxByte* data)
{
   m_hSnd = (char *)data;
   m_waveLength = size;
   m_type = wxSound_MEMORY;

    return true;
}

bool wxSound::DoPlay(unsigned flags) const
{
    Stop();

#if USE_QUICKTIME

    Movie movie;
    Handle dataRef = nil;
    OSType dataRefType;
    OSErr err = noErr;
    
    if (!wxInitQT())
        return false;

    switch(m_type)
    {
    case wxSound_MEMORY:
        {
            Handle myHandle = nil;
            unsigned long type;
            unsigned long atoms[5];

            if (memcmp(&m_hSnd[8], "WAVE", 4) == 0)
                type = kQTFileTypeWave;
            else if (memcmp(&m_hSnd[8], "AIFF", 4) == 0)
                type = kQTFileTypeAIFF;
            else if (memcmp(&m_hSnd[8], "AIFC", 4) == 0)
                type = kQTFileTypeAIFC;
            else
            {
                wxLogSysError(wxT("wxSound - Location in memory does not contain valid data"));
                return false;
            }

            atoms[0] = EndianU32_NtoB(sizeof(long) * 3);
            atoms[1] = EndianU32_NtoB(kDataRefExtensionMacOSFileType);
            atoms[2] = EndianU32_NtoB(type);
            atoms[3] = EndianU32_NtoB(sizeof(long) * 2 + m_waveLength);
            atoms[4] = EndianU32_NtoB(kDataRefExtensionInitializationData);

            err = 0;
            err |= PtrToHand(&myHandle, &dataRef, sizeof(Handle));
            err |= PtrAndHand("\p", dataRef, 1);
            err |= PtrAndHand(atoms, dataRef, sizeof(long) * 5);
            err |= PtrAndHand(m_hSnd, dataRef, m_waveLength);

            dataRefType = HandleDataHandlerSubType;
        }
        break;
    case wxSound_FILE:
            {
            err = QTNewDataReferenceFromFullPathCFString(wxMacCFStringHolder(m_sndname,wxLocale::GetSystemEncoding()),
                                                         (UInt32)kQTNativeDefaultPathStyle,
                                                         0,
                                                         &dataRef,
                                                         &dataRefType);
        }
        break;
    default:
                return false;
    }//end switch(m_type)

            wxASSERT(err == noErr);

            if (NULL != dataRef || err != noErr)
            {
        err = NewMovieFromDataRef(&movie,
                                  0,
                                  nil,
                                  dataRef,
                                  dataRefType);
                wxASSERT(err == noErr);
                DisposeHandle(dataRef);
            }

            if (err != noErr)
            {
                wxLogSysError(
            wxString::Format(wxT("wxSound - Could not create movie\nError:%i"), err)
                    );
                return false;
            }

    SetMovieVolume(movie, kFullVolume);
    GoToBeginningOfMovie(movie);
    StartMovie(movie);

    if (flags & wxSOUND_ASYNC)
    {
        //Start timer and play movie asyncronously
        lastSoundTimer = ((wxQTTimer*&)m_pTimer) =
            new wxQTTimer(movie, flags & wxSOUND_LOOP ? 1 : 0,
                          &lastSoundIsPlaying);
        lastSoundIsPlaying = true;
        ((wxQTTimer*)m_pTimer)->Start(MOVIE_DELAY, wxTIMER_CONTINUOUS);
    }
    else
    {
        wxASSERT_MSG(!(flags & wxSOUND_LOOP), wxT("Can't loop and play syncronously at the same time"));

        //Play movie until it ends, then exit
        //Note that due to quicktime caching this may not always
        //work 100% correctly
        while (!IsMovieDone(movie))
            MoviesTask(movie, 1);

        DisposeMovie(movie);
    }
#endif

    return true;
}

bool wxSound::IsPlaying()
{
    return lastSoundIsPlaying;
}

void wxSound::Stop()
{
    if (lastSoundIsPlaying)
    {
        delete (wxTimer*&) lastSoundTimer;
        lastSoundIsPlaying = false;
        lastSoundTimer = NULL;
    }
}

void* wxSound::GetHandle()
{
#if USE_QUICKTIME
    return (void*) ((wxQTTimer*) m_pTimer)->GetMovie();
#endif
    return NULL;
}
#endif //wxUSE_SOUND
