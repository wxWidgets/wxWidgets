/////////////////////////////////////////////////////////////////////////////
// Name:        mac/carbon/moviectrl.cpp
// Purpose:     wxMediaCtrl MAC CARBON QT
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by: 
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "moviectrl.h"
//#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/mac/carbon/mediactrl.h"

#if wxUSE_MEDIACTRL

#include "wx/timer.h"

IMPLEMENT_CLASS(wxMediaCtrl, wxControl);
IMPLEMENT_DYNAMIC_CLASS(wxMediaEvent, wxEvent); 
DEFINE_EVENT_TYPE(wxEVT_MEDIA_FINISHED); 

//uma is for wxMacFSSpec
#ifdef __WXMAC__
#include "wx/mac/uma.h"
#include <Movies.h>
#include <Gestalt.h>
#else
//quicktime media layer for mac emulation on pc
#include <qtml.h>
#endif

#include <QuickTimeComponents.h>

#ifdef __WXMAC__
#define MSWMOVIECHECK
#else
#define MSWMOVIECHECK if(!m_bLoaded) return 0;
#endif

//Time between timer calls
#define MOVIE_DELAY 50

// ------------------------------------------------------------------
//          wxQTTimer - Handle Asyncronous Playing
// ------------------------------------------------------------------
class _wxQTTimer : public wxTimer
{
public:
    _wxQTTimer(Movie movie, wxMediaCtrl* parent) :
        m_movie(movie), m_bPaused(false), m_parent(parent)
    {
    }

    ~_wxQTTimer()
    {
    }

    bool GetPaused() {return m_bPaused;}
    void SetPaused(bool bPaused) {m_bPaused = bPaused;}

    void Notify()
    {
        if (!m_bPaused)
        {
            if(!IsMovieDone(m_movie))
                MoviesTask(m_movie, MOVIE_DELAY); //Give QT time to play movie
            else
            {
                Stop();
                m_parent->Stop();
                wxASSERT(::GetMoviesError() == noErr);
                wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED, m_parent->GetId());
                m_parent->GetParent()->ProcessEvent(theEvent);
            }
        }
    }

protected:
    Movie m_movie;
    bool m_bPaused;
    wxMediaCtrl* m_parent;
};

//Determines whether version 6 of QT is installed
Boolean _wxIsQuickTime4Installed (void)
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

bool wxMediaCtrl::InitQT ()
{
    if (_wxIsQuickTime4Installed())
    {
        #ifndef __WXMAC__
        int nError;
        //-2093 no dll
            if ((nError = InitializeQTML(0)) != noErr)
            {
                wxFAIL_MSG(wxString::Format(wxT("Couldn't Initialize Quicktime-%i"), nError));
            }
        #endif
        EnterMovies();
        return true;
    }
    else
    {
        wxFAIL_MSG(wxT("Quicktime is not installed, or Your Version of Quicktime is <= 4."));
        return false;
    }
}

bool wxMediaCtrl::Create(wxWindow* parent, wxWindowID id, const wxString& fileName, 
                         const wxPoint& pos, const wxSize& size, 
                         long style, long WXUNUSED(driver), const wxString& name)
{
    if(!DoCreate(parent, id, pos, size, style, name))
        return false;

    if(!fileName.empty())
    {
        if (!Load(fileName))
            return false;

        if(!Play())
            return false;
    }

    return true;
}

bool wxMediaCtrl::Create(wxWindow* parent, wxWindowID id, const wxURI& location, 
                         const wxPoint& pos, const wxSize& size, 
                         long style, long WXUNUSED(driver), const wxString& name)
{
    if(!DoCreate(parent, id, pos, size, style, name))
        return false;
    
    if(!location.IsReference())
    {
        if (!Load(location))
            return false;

        if(!Play())
            return false;
    }

    return true;
}

bool wxMediaCtrl::DoCreate(wxWindow* parent, wxWindowID id, 
                         const wxPoint& pos, const wxSize& size, 
                         long style, const wxString& name)
{
     //do some window stuff
    if ( !wxControl::Create(parent, id, pos, size, 
#ifdef __WXMAC__
                            MacRemoveBordersFromStyle(style), 
#else
                            style | wxNO_BORDER, 
#endif
                            wxDefaultValidator, name) )
        return false;

    //Set our background color to black by default
    SetBackgroundColour(*wxBLACK);

    return true;
}

bool wxMediaCtrl::Load(const wxString& fileName)
{
    if(m_bLoaded)
        Cleanup();

    if ( !InitQT() )
        return false;

    OSErr err = noErr;
    short movieResFile;
    FSSpec sfFile;
#ifdef __WXMAC__
    wxMacFilename2FSSpec( fileName , &sfFile ) ;
#else
    if (NativePathNameToFSSpec ((char*) fileName.mb_str(), &sfFile, 0) != noErr)
        return false;
#endif
    if (OpenMovieFile (&sfFile, &movieResFile, fsRdPerm) != noErr)
        return false;

    short movieResID = 0;
    Str255 movieName;

    err = NewMovieFromFile (
    &m_movie,
    movieResFile,
    &movieResID,
    movieName,
    newMovieActive,
    NULL); //wasChanged

    CloseMovieFile (movieResFile);

    if (err != noErr)
        return false;

    FinishLoad();

    return m_bLoaded;
}

bool wxMediaCtrl::Load(const wxURI& location)
{
    if(m_bLoaded)
        Cleanup();

    if ( !InitQT() )
        return false;

    wxString theURI = location.BuildURI();

    OSErr err = noErr;

    Handle theHandle = NewHandleClear(theURI.length() + 1);
    wxASSERT(theHandle);

    BlockMove(theURI.mb_str(), *theHandle, theURI.length() + 1);

    //create the movie from the handle that refers to the URI
    err = NewMovieFromDataRef(&m_movie, newMovieActive, 
                                NULL, theHandle, 
                                URLDataHandlerSubType);

    DisposeHandle(theHandle);

    if (err != noErr)
        return false;

    //preroll movie for streaming
    //TODO:Async this?
    TimeValue timeNow;
    Fixed playRate;
    timeNow = GetMovieTime(m_movie, NULL);
    playRate = GetMoviePreferredRate(m_movie);
    PrePrerollMovie(m_movie, timeNow, playRate, NULL, NULL);
    PrerollMovie(m_movie, timeNow, playRate);
    SetMovieRate(m_movie, playRate);

    FinishLoad();

    return m_bLoaded;
}

void wxMediaCtrl::FinishLoad()
{
    m_timer = new _wxQTTimer(m_movie, (wxMediaCtrl*) this);
    wxASSERT(m_timer);

    //get the real size of the movie
    Rect outRect;
    ::GetMovieNaturalBoundsRect (m_movie, &outRect);
    wxASSERT(::GetMoviesError() == noErr);

    m_bestSize.x = outRect.right - outRect.left;
    m_bestSize.y = outRect.bottom - outRect.top;
        
    //reparent movie
if(GetMovieIndTrackType(m_movie, 1, VisualMediaCharacteristic/*AudioMediaCharacteristic*/, movieTrackCharacteristic | movieTrackEnabledOnly) != NULL)
    {

#ifdef __WXMSW__
    CreatePortAssociation(this->GetHWND(), NULL, 0L);
#endif
    SetMovieGWorld(m_movie, (CGrafPtr)

#ifdef __WXMSW__
    GetNativeWindowPort(this->GetHWND())
#else
    GetWindowPort((WindowRef)this->MacGetTopLevelWindowRef())
#endif
    , nil);
    }

//    wxPrintf(wxT("%u\n"), ::GetMovieTimeScale(m_movie));
    //we want millisecond precision
    ::SetMovieTimeScale(m_movie, 1000);
    
    m_bLoaded = (::GetMoviesError() == noErr);

    //work around refresh issues
    wxSize size = GetParent()->GetSize();
    GetParent()->SetSize(wxSize(size.x+1, size.y+1));
    GetParent()->Refresh();
    GetParent()->Update();
    GetParent()->SetSize(size);
    GetParent()->Refresh();
    GetParent()->Update();
}

bool wxMediaCtrl::Play()
{
    MSWMOVIECHECK
    ::StartMovie(m_movie);
    m_timer->SetPaused(false);
    m_timer->Start(MOVIE_DELAY, wxTIMER_CONTINUOUS);
    return ::GetMoviesError() == noErr;
}

bool wxMediaCtrl::Pause()
{
    MSWMOVIECHECK
    ::StopMovie(m_movie);
    m_timer->SetPaused(true);
    m_timer->Stop();
    return ::GetMoviesError() == noErr;
}

bool wxMediaCtrl::Stop()
{
    MSWMOVIECHECK
    m_timer->SetPaused(false);
    m_timer->Stop();

    ::StopMovie(m_movie);
    if(::GetMoviesError() != noErr)
        return false;
    
    ::GoToBeginningOfMovie(m_movie);
    return ::GetMoviesError() == noErr;
}

double wxMediaCtrl::GetPlaybackRate()
{
    MSWMOVIECHECK
    return ( ((double)::GetMovieRate(m_movie)) / 0x10000);
}

bool wxMediaCtrl::SetPlaybackRate(double dRate)
{
    MSWMOVIECHECK
    ::SetMovieRate(m_movie, (Fixed) (dRate * 0x10000));
    return ::GetMoviesError() == noErr;
}

bool wxMediaCtrl::SetPosition(long where)
{
    MSWMOVIECHECK
    TimeRecord theTimeRecord;
    memset(&theTimeRecord, 0, sizeof(TimeRecord));
    theTimeRecord.value.lo = where;
    theTimeRecord.scale = ::GetMovieTimeScale(m_movie);
    theTimeRecord.base = ::GetMovieTimeBase(m_movie);
    ::SetMovieTime(m_movie, &theTimeRecord);

    if (::GetMoviesError() != noErr)
        return false;

    return true;
}

long wxMediaCtrl::GetPosition()
{
    MSWMOVIECHECK
    return ::GetMovieTime(m_movie, NULL);
}

long wxMediaCtrl::GetDuration()
{
    MSWMOVIECHECK
    return ::GetMovieDuration(m_movie);
}

wxMediaState wxMediaCtrl::GetState()
{
    if ( !m_bLoaded || (m_timer->IsRunning() == false && m_timer->GetPaused() == false) )
        return wxMEDIASTATE_STOPPED;

    if( m_timer->IsRunning() == true )
        return wxMEDIASTATE_PLAYING;
    else
        return wxMEDIASTATE_PAUSED;
}

void wxMediaCtrl::Cleanup()
{
    delete m_timer;
    m_timer = NULL;

    StopMovie(m_movie);
    DisposeMovie(m_movie);
    
    //Note that ExitMovies() is not neccessary, but
    //the docs are fuzzy on whether or not TerminateQTML is
    ExitMovies();

#ifndef __WXMAC__
    TerminateQTML();
#endif
}

wxMediaCtrl::~wxMediaCtrl()
{
    if(m_bLoaded)
        Cleanup();
}

wxSize wxMediaCtrl::DoGetBestSize() const
{
    return m_bestSize;
}

void wxMediaCtrl::DoMoveWindow(int x, int y, int w, int h)
{
    wxControl::DoMoveWindow(x,y,w,h);
    
    if(m_bLoaded)
    {
#ifdef __WXMAC__
        Rect theRect = {y, x, y+h, x+w};
#else
        Rect theRect = {0, 0, h, w};
#endif
        ::SetMovieBox(m_movie, &theRect);
        wxASSERT(::GetMoviesError() == noErr);
    }
}

#endif //wxUSE_MOVIECTRL
