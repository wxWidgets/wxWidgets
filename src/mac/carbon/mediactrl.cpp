/////////////////////////////////////////////////////////////////////////////
// Name:        mac/carbon/mediactrl.cpp
// Purpose:     Built-in Media Backends for Mac
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) 2004-2005 Ryan Norton, portions (c) 2004 Kevin Olliver
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

//---------------------------------------------------------------------------
// Whether or not to use OSX 10.2's CreateMovieControl for native QuickTime
// control - i.e. native positioning and event handling etc..
//---------------------------------------------------------------------------
#ifndef wxUSE_CREATEMOVIECONTROL
#    if defined( __WXMAC_OSX__ ) && ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )
#        define wxUSE_CREATEMOVIECONTROL 1
#    else
#        define wxUSE_CREATEMOVIECONTROL 0
#    endif
#endif

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
#ifndef __DARWIN__
#include <Movies.h>
#include <Gestalt.h>
#include <QuickTimeComponents.h>    //Standard QT stuff
#else
#include <QuickTime/QuickTimeComponents.h>
#endif

//Determines whether version 4 of QT is installed (Pretty much for classic only)
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

    virtual double GetVolume();
    virtual bool SetVolume(double);

    void Cleanup();
    void FinishLoad();

    wxSize m_bestSize;              //Original movie size
#ifdef __WXMAC_OSX__
    struct MovieType** m_movie;     //QT Movie handle/instance
#else
    Movie m_movie ;
#endif
    wxControl* m_ctrl;              //Parent control
    bool m_bVideo;                  //Whether or not we have video
    class _wxQTTimer* m_timer;      //Timer for streaming the movie

    DECLARE_DYNAMIC_CLASS(wxQTMediaBackend)
};


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxQTMediaBackend
//
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

    //Note that ExitMovies() is not necessary...
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
    //Don't bother in Native control mode
#if defined( __WXMAC__ ) && TARGET_API_MAC_OSX && ( MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_2 )
    if (!_wxIsQuickTime4Installed())
        return false;
#endif

    EnterMovies();

    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    //
    if ( !

#if wxUSE_CREATEMOVIECONTROL
          ctrl->wxWindow::Create(parent, id, pos, size,
                                 wxWindow::MacRemoveBordersFromStyle(style),
                                 name)
#else
          ctrl->wxControl::Create(parent, id, pos, size,
                                  wxWindow::MacRemoveBordersFromStyle(style),
                                  validator, name)
#endif
        )
        return false;

#if wxUSE_VALIDATORS
    ctrl->SetValidator(validator);
#endif

    m_ctrl = ctrl;
    return true;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Load (file version)
//
// 1) Get an FSSpec from the Windows path name
// 2) Open the movie
// 3) Obtain the movie instance from the movie resource
// 4) Close the movie resource
// 5) Finish loading
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Load(const wxString& fileName)
{
    if(m_timer)
        Cleanup();

    OSErr err = noErr;
    short movieResFile;
    FSSpec sfFile;

    //FIXME:wxMacFilename2FSSpec crashes on empty string -
    //does it crash on other strings too and should this
    //"fix" be put in the carbon wxSound?
    if (fileName.empty())
        return false;

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
// wxQTMediaBackend::Load (URL Version)
//
// 1) Build an escaped URI from location
// 2) Create a handle to store the URI string
// 3) Put the URI string inside the handle
// 4) Make a QuickTime URL data ref from the handle with the URI in it
// 5) Clean up the URI string handle
// 6) Do some prerolling
// 7) Finish Loading
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
    //TODO:Async this using threads?
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
// 1) Create the movie timer
// 2) Get real size of movie for GetBestSize/sizers
// 3) See if there is video in the movie, and if so then either
//    SetMovieGWorld if < 10.2 or use Native CreateMovieControl
// 4) Set the movie time scale to something usable so that seeking
//    etc.  will work correctly
// 5) Refresh parent window
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
#if wxUSE_CREATEMOVIECONTROL
        //
        //Native CreateMovieControl QT control (Thanks to Kevin Olliver's
        //wxQTMovie for some of this).
        //
        #define GetControlPeer(whatever) ctrl->m_peer
        wxMediaCtrl* ctrl = (wxMediaCtrl*) m_ctrl;
            Rect bounds = wxMacGetBoundsForControl(m_ctrl,
                                                   m_ctrl->GetPosition(),
                                                   m_ctrl->GetSize());

        //Dispose of old control for new one
        if (GetControlPeer(m_ctrl) && GetControlPeer(m_ctrl)->Ok() )
            GetControlPeer(m_ctrl)->Dispose();

        //Options-
        //kMovieControlOptionXXX
        //HideController - hide the movie controller
        //LocateTopLeft - movie is pinned to top left rather than centered in the control
        //EnableEditing - Allows programmatic editing and dragn'drop
        //HandleEditingHI- Installs event stuff for edit menu - forces EnableEditing also
        //SetKeysEnabled - Allows keyboard input
        //ManuallyIdled - app handles movie idling rather than internal timer event loop
        ::CreateMovieControl(
                    (WindowRef)
                       ctrl->MacGetTopLevelWindowRef(), //parent
                       &bounds,                         //control bounds
                       m_movie,                         //movie handle
                       kMovieControlOptionHideController
                       | kMovieControlOptionLocateTopLeft
                       | kMovieControlOptionSetKeysEnabled
//                       | kMovieControlOptionManuallyIdled
                       ,                                //flags
                       ctrl->m_peer->GetControlRefAddr() );

        ::EmbedControl(ctrl->m_peer->GetControlRef(), (ControlRef)ctrl->GetParent()->GetHandle());
#else
        //
        //"Emulation"
        //
        SetMovieGWorld(m_movie,
                       (CGrafPtr)
                       GetWindowPort(
                       (WindowRef)
                       m_ctrl->MacGetTopLevelWindowRef()
                       ),
                       nil);
#endif
    }

    //we want millisecond precision
    ::SetMovieTimeScale(m_movie, 1000);
    wxASSERT(::GetMoviesError() == noErr);

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a separate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();

    //send loaded event
    wxMediaEvent theEvent(wxEVT_MEDIA_LOADED,
                            m_ctrl->GetId());
    m_ctrl->AddPendingEvent(theEvent);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Play
//
// 1) Start the QT movie
// 2) Start the movie loading timer
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
// 1) Stop the movie
// 2) Stop the movie timer
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
// 1) Stop the movie
// 2) Stop the movie timer
// 3) Seek to the beginning of the movie
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
// 1) Get the movie playback rate from ::GetMovieRate
//---------------------------------------------------------------------------
double wxQTMediaBackend::GetPlaybackRate()
{
    return ( ((double)::GetMovieRate(m_movie)) / 0x10000);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetPlaybackRate
//
// 1) Convert dRate to Fixed and Set the movie rate through SetMovieRate
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPlaybackRate(double dRate)
{
    ::SetMovieRate(m_movie, (Fixed) (dRate * 0x10000));
    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetPosition
//
// 1) Create a time record struct (TimeRecord) with appropriate values
// 2) Pass struct to SetMovieTime
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
// Calls GetMovieTime
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetPosition()
{
    return ::GetMovieTime(m_movie, NULL);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetVolume
//
// Gets the volume through GetMovieVolume - which returns a 16 bit short -
//
// +--------+--------+
// +   (1)  +   (2)  +
// +--------+--------+
//
// (1) first 8 bits are value before decimal
// (2) second 8 bits are value after decimal
//
// Volume ranges from -1.0 (gain but no sound), 0 (no sound and no gain) to
// 1 (full gain and sound)
//---------------------------------------------------------------------------
double wxQTMediaBackend::GetVolume()
{
    short sVolume = GetMovieVolume(m_movie);

    if(sVolume & (128 << 8)) //negative - no sound
        return 0.0;

    return (sVolume & (127 << 8)) ? 1.0 : ((double)(sVolume & 255)) / 255.0;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::SetVolume
//
// Sets the volume through SetMovieVolume - which takes a 16 bit short -
//
// +--------+--------+
// +   (1)  +   (2)  +
// +--------+--------+
//
// (1) first 8 bits are value before decimal
// (2) second 8 bits are value after decimal
//
// Volume ranges from -1.0 (gain but no sound), 0 (no sound and no gain) to
// 1 (full gain and sound)
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetVolume(double dVolume)
{
    short sVolume = (short) (dVolume >= .9999 ? 1 << 8 : (dVolume * 255) );
    SetMovieVolume(m_movie, sVolume);
    return true;
}
 
 //---------------------------------------------------------------------------
// wxQTMediaBackend::GetDuration
//
// Calls GetMovieDuration
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDuration()
{
    return ::GetMovieDuration(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetState
//
// Determines the current state - the timer keeps track of whether or not
// we are paused or stopped (if the timer is running we are playing)
//---------------------------------------------------------------------------
wxMediaState wxQTMediaBackend::GetState()
{
    if ( !m_timer || (m_timer->IsRunning() == false &&
                      m_timer->GetPaused() == false) )
        return wxMEDIASTATE_STOPPED;

    if( m_timer->IsRunning() )
        return wxMEDIASTATE_PLAYING;
    else
        return wxMEDIASTATE_PAUSED;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Cleanup
//
// Diposes of the movie timer, Control if native, and stops and disposes
// of the QT movie
//---------------------------------------------------------------------------
void wxQTMediaBackend::Cleanup()
{
    delete m_timer;
    m_timer = NULL;

#if wxUSE_CREATEMOVIECONTROL
    DisposeControl(((wxMediaCtrl*)m_ctrl)->m_peer->GetControlRef());
#endif

    StopMovie(m_movie);
    DisposeMovie(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetVideoSize
//
// Returns the actual size of the QT movie
//---------------------------------------------------------------------------
wxSize wxQTMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// We need to do this even when using native qt control because
// CreateMovieControl is broken in this regard...
//---------------------------------------------------------------------------
void wxQTMediaBackend::Move(int x, int y, int w, int h)
{
#if !wxUSE_CREATEMOVIECONTROL
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
#else
    if(m_timer && m_ctrl)
    {
        m_ctrl->GetParent()->MacWindowToRootWindow(&x, &y);

        ::MoveControl( (ControlRef) m_ctrl->GetHandle(), x, y );
        m_ctrl->GetParent()->Refresh();
        m_ctrl->GetParent()->Update();
    }
#endif
}


//in source file that contains stuff you don't directly use
#include "wx/html/forcelnk.h"
FORCE_LINK_ME(basewxmediabackends);

#endif //wxUSE_MEDIACTRL
