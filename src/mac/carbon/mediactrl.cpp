/////////////////////////////////////////////////////////////////////////////
// Name:        mac/carbon/mediactrl.cpp
// Purpose:     Built-in Media Backends for Mac
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "mediactrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "wx/mediactrl.h"

//---------------------------------------------------------------------------
// Compilation guard
//---------------------------------------------------------------------------
#if wxUSE_MEDIACTRL

//===========================================================================
//  BACKEND DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//
//  wxQTMediaBackend
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  QT Includes
//---------------------------------------------------------------------------
//uma is for wxMacFSSpec
#include "wx/mac/uma.h"
#include "wx/timer.h"
#include <Movies.h>
#include <Gestalt.h>
#include <QuickTimeComponents.h>    //Standard QT stuff

//Determines whether version 6 of QT is installed
Boolean _wxIsQuickTime4Installed (void)
{
    short error;
    long result;

    error = Gestalt (gestaltQuickTime, &result);
    return (error == noErr) && (((result >> 16) & 0xffff) >= 0x0400);
}

class WXDLLIMPEXP_MEDIA wxQTMediaBackend : public wxMediaBackend
{
public:

    wxQTMediaBackend();
    ~wxQTMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent, 
                                     wxWindowID id,
                                     const wxPoint& pos, 
                                     const wxSize& size,
                                     long style, 
                                     const wxValidator& validator,
                                     const wxString& name);

    virtual bool Play();
    virtual bool Pause();
    virtual bool Stop();

    virtual bool Load(const wxString& fileName);
    virtual bool Load(const wxURI& location);

    virtual wxMediaState GetState();

    virtual bool SetPosition(wxLongLong where);
    virtual wxLongLong GetPosition();
    virtual wxLongLong GetDuration();

    virtual void Move(int x, int y, int w, int h);
    wxSize GetVideoSize() const;

    virtual double GetPlaybackRate();
    virtual bool SetPlaybackRate(double dRate);

    void Cleanup();
    void FinishLoad();

    wxSize m_bestSize;              //Original movie size
    struct MovieType** m_movie;     //QT Movie handle/instance
    wxControl* m_ctrl;              //Parent control
    bool m_bVideo;                  //Whether or not we have video
    class _wxQTTimer* m_timer;      //Timer for streaming the movie

    DECLARE_DYNAMIC_CLASS(wxQTMediaBackend);
};


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxQTMediaBackend
// 
// TODO: Use a less cludgy way to pause/get state/set state
// TODO: Dynamically load from qtml.dll
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxQTMediaBackend, wxMediaBackend);

//Time between timer calls
#define MOVIE_DELAY 100

// --------------------------------------------------------------------------
//          wxQTTimer - Handle Asyncronous Playing
// --------------------------------------------------------------------------
class _wxQTTimer : public wxTimer
{
public:
    _wxQTTimer(Movie movie, wxQTMediaBackend* parent) :
        m_movie(movie), m_bPaused(false), m_parent(parent)
    {
    }

    ~_wxQTTimer()
    {
    }

    bool GetPaused() {return m_bPaused;}
    void SetPaused(bool bPaused) {m_bPaused = bPaused;}

    //-----------------------------------------------------------------------
    // _wxQTTimer::Notify
    //
    // 1) Checks to see if the movie is done, and if not continues
    //    streaming the movie
    // 2) Sends the wxEVT_MEDIA_STOP event if we have reached the end of
    //    the movie.
    //-----------------------------------------------------------------------
    void Notify()
    {
        if (!m_bPaused)
        {
            if(!IsMovieDone(m_movie))
                MoviesTask(m_movie, MOVIE_DELAY); 
            else
            {
                wxMediaEvent theEvent(wxEVT_MEDIA_STOP, 
                                      m_parent->m_ctrl->GetId());
                m_parent->m_ctrl->ProcessEvent(theEvent);

                if(theEvent.IsAllowed())
                {
                    Stop();
                    m_parent->Stop();
                    wxASSERT(::GetMoviesError() == noErr);

                    //send the event to our child
                    wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED, 
                                          m_parent->m_ctrl->GetId());
                    m_parent->m_ctrl->ProcessEvent(theEvent);
                }
            }
        }
    }

protected:
    Movie m_movie;                  //Our movie instance
    bool m_bPaused;                 //Whether we are paused or not
    wxQTMediaBackend* m_parent;     //Backend pointer
};

//---------------------------------------------------------------------------
// wxQTMediaBackend Constructor
//
// Sets m_timer to NULL signifying we havn't loaded anything yet
//---------------------------------------------------------------------------
wxQTMediaBackend::wxQTMediaBackend() : m_timer(NULL)
{
}

//---------------------------------------------------------------------------
// wxQTMediaBackend Destructor
//
// 1) Cleans up the QuickTime movie instance
// 2) Decrements the QuickTime reference counter - if this reaches
//    0, QuickTime shuts down
// 3) Decrements the QuickTime Windows Media Layer reference counter -
//    if this reaches 0, QuickTime shuts down the Windows Media Layer
//---------------------------------------------------------------------------
wxQTMediaBackend::~wxQTMediaBackend()
{
    if(m_timer)
        Cleanup();

    //Note that ExitMovies() is not neccessary...
    ExitMovies();
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::CreateControl
//
// 1) Intializes QuickTime
// 2) Creates the control window
//---------------------------------------------------------------------------
bool wxQTMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent, 
                                     wxWindowID id,
                                     const wxPoint& pos, 
                                     const wxSize& size,
                                     long style, 
                                     const wxValidator& validator,
                                     const wxString& name)
{
    if (!_wxIsQuickTime4Installed())
        return false;

    EnterMovies();

    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            m_ctrl->MacRemoveBordersFromStyle(style),
                            validator, name) )
        return false;

    //
    //Set our background color to black by default
    //
    ctrl->SetBackgroundColour(*wxBLACK);

    m_ctrl = ctrl;
    return true;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Load (file version)
//
// 1) Get an FSSpec from the Windows path name
// 2) Open the movie
// 3) Obtain the movie instance from the movie resource
// 4) 
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Load(const wxString& fileName)
{
    if(m_timer)
        Cleanup();

    OSErr err = noErr;
    short movieResFile;
    FSSpec sfFile;
    
    wxMacFilename2FSSpec( fileName , &sfFile );
    
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

    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Load
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Load(const wxURI& location)
{
    if(m_timer)
        Cleanup();

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

    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::FinishLoad
//
// TODO
//---------------------------------------------------------------------------
void wxQTMediaBackend::FinishLoad()
{
    m_timer = new _wxQTTimer(m_movie, (wxQTMediaBackend*) this);
    wxASSERT(m_timer);

    //get the real size of the movie
    Rect outRect;
    ::GetMovieNaturalBoundsRect (m_movie, &outRect);
    wxASSERT(::GetMoviesError() == noErr);

    m_bestSize.x = outRect.right - outRect.left;
    m_bestSize.y = outRect.bottom - outRect.top;
        
    //reparent movie/*AudioMediaCharacteristic*/
    if(GetMovieIndTrackType(m_movie, 1, 
                            VisualMediaCharacteristic, 
                            movieTrackCharacteristic | 
                                movieTrackEnabledOnly) != NULL)
    {        
        SetMovieGWorld(m_movie, 
                       (CGrafPtr) 
                       GetWindowPort(
                       (WindowRef)
                       m_ctrl->MacGetTopLevelWindowRef()
                       ), 
                       nil);
    }

    //we want millisecond precision
    ::SetMovieTimeScale(m_movie, 1000);
    wxASSERT(::GetMoviesError() == noErr);

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a seperate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Play
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Play()
{
    ::StartMovie(m_movie);
    m_timer->SetPaused(false);
    m_timer->Start(MOVIE_DELAY, wxTIMER_CONTINUOUS);
    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Pause
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Pause()
{
    ::StopMovie(m_movie);
    m_timer->SetPaused(true);
    m_timer->Stop();
    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Stop
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Stop()
{
    m_timer->SetPaused(false);
    m_timer->Stop();

    ::StopMovie(m_movie);
    if(::GetMoviesError() != noErr)
        return false;
    
    ::GoToBeginningOfMovie(m_movie);
    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetPlaybackRate
//
// TODO
//---------------------------------------------------------------------------
double wxQTMediaBackend::GetPlaybackRate()
{
    return ( ((double)::GetMovieRate(m_movie)) / 0x10000);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetPlaybackRate
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPlaybackRate(double dRate)
{
    ::SetMovieRate(m_movie, (Fixed) (dRate * 0x10000));
    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetPosition
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPosition(wxLongLong where)
{
    TimeRecord theTimeRecord;
    memset(&theTimeRecord, 0, sizeof(TimeRecord));
    theTimeRecord.value.lo = where.GetValue();
    theTimeRecord.scale = ::GetMovieTimeScale(m_movie);
    theTimeRecord.base = ::GetMovieTimeBase(m_movie);
    ::SetMovieTime(m_movie, &theTimeRecord);

    if (::GetMoviesError() != noErr)
        return false;

    return true;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetPosition
//
// TODO
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetPosition()
{
    return ::GetMovieTime(m_movie, NULL);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetDuration
//
// TODO
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDuration()
{
    return ::GetMovieDuration(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetState
//
// TODO
//---------------------------------------------------------------------------
wxMediaState wxQTMediaBackend::GetState()
{
    if ( !m_timer || (m_timer->IsRunning() == false && 
                      m_timer->GetPaused() == false) )
        return wxMEDIASTATE_STOPPED;

    if( m_timer->IsRunning() == true )
        return wxMEDIASTATE_PLAYING;
    else
        return wxMEDIASTATE_PAUSED;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Cleanup
//
// TODO
//---------------------------------------------------------------------------
void wxQTMediaBackend::Cleanup()
{
    delete m_timer;
    m_timer = NULL;

    StopMovie(m_movie);
    DisposeMovie(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetVideoSize
//
// TODO
//---------------------------------------------------------------------------
wxSize wxQTMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
void wxQTMediaBackend::Move(int x, int y, int w, int h)
{
    if(m_timer)
    {
        if ( m_ctrl )
        {
            m_ctrl->GetParent()->MacWindowToRootWindow(&x, &y);
        }
        
        Rect theRect = {y, x, y+h, x+w};

        ::SetMovieBox(m_movie, &theRect);
        wxASSERT(::GetMoviesError() == noErr);
    }
}


//in source file that contains stuff you don't directly use
#include <wx/html/forcelnk.h>
FORCE_LINK_ME(basewxmediabackends);

#endif //wxUSE_MEDIACTRL





