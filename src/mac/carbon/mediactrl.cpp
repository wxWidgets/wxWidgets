/////////////////////////////////////////////////////////////////////////////
// Name:        mac/carbon/mediactrl.cpp
// Purpose:     Built-in Media Backends for Mac
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) 2004-2005 Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// There are several known bugs with CreateMovieControl
// on systems > 10.2 - see main.c of QTCarbonShell sample for details
//
// Also, with either version it will overdraw anything below its TLW - so
// it's relatively useless on a notebook page (this happens in Opera too).
//
// Even though though the CreateMovieControl version is the default
// for OSX, the MovieController version is heavily tested and works
// just as well...
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/mediactrl.h"

// uma is for wxMacFSSpec
#include "wx/mac/uma.h"
#include "wx/timer.h"

#ifndef __DARWIN__
// standard QT stuff
#include <Movies.h>
#include <Gestalt.h>
#include <QuickTimeComponents.h>
#else
#include <QuickTime/QuickTimeComponents.h>
#endif

#if wxUSE_MEDIACTRL

//---------------------------------------------------------------------------
// Whether or not to use OSX 10.2's CreateMovieControl for native QuickTime
// control - i.e. native positioning and event handling etc..
//---------------------------------------------------------------------------
#ifndef wxUSE_CREATEMOVIECONTROL
#    if defined( __WXMAC_OSX__ ) && \
      ( MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_2 )
#        define wxUSE_CREATEMOVIECONTROL 1
#    else
#        define wxUSE_CREATEMOVIECONTROL 0
#    endif
#endif

//---------------------------------------------------------------------------
// Height and Width of movie controller in the movie control
//---------------------------------------------------------------------------
#define wxMCWIDTH   320
#define wxMCHEIGHT  16

//===========================================================================
//  BACKEND DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//  wxQTMediaBackend
//---------------------------------------------------------------------------


class WXDLLIMPEXP_MEDIA wxQTMediaBackend : public wxMediaBackendCommonBase
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

    virtual bool ShowPlayerControls(wxMediaCtrlPlayerControls flags);

    virtual wxLongLong GetDownloadProgress();
    virtual wxLongLong GetDownloadTotal();

    //
    //  ------  Implementation from now on  --------
    //
    bool DoPause();
    bool DoStop();

    void DoLoadBestSize();
    void DoSetControllerVisible(wxMediaCtrlPlayerControls flags);

    wxLongLong GetDataSizeFromStart(TimeValue end);

    //TODO: Last param actually long - does this work on 64bit machines?
    static pascal Boolean MCFilterProc(MovieController theController,
        short action, void *params, long refCon);

#if wxUSE_CREATEMOVIECONTROL
    void DoCreateMovieControl();
#else
    Boolean IsQuickTime4Installed();
    void DoNewMovieController();
    static pascal void PPRMProc(Movie theMovie, OSErr theErr, void* theRefCon);
#endif

    wxSize m_bestSize;          // Original movie size

#ifdef __WXMAC_OSX__
    struct MovieType** m_movie; // QT Movie handle/instance
#else
    Movie m_movie;              // Movie instance
#endif

    bool m_bPlaying;            // Whether media is playing or not
    class wxTimer* m_timer;     // Timer for streaming the movie
    MovieController m_mc;       // MovieController instance
    wxMediaCtrlPlayerControls m_interfaceflags; // Saved interface flags

#if !wxUSE_CREATEMOVIECONTROL
    EventHandlerRef m_pEventHandlerRef; // Event handler to cleanup
    MoviePrePrerollCompleteUPP  m_preprerollupp;
    EventHandlerUPP             m_eventupp;
    MCActionFilterWithRefConUPP m_mcactionupp;

    friend class wxQTMediaEvtHandler;
#endif

    DECLARE_DYNAMIC_CLASS(wxQTMediaBackend)
};

#if !wxUSE_CREATEMOVIECONTROL
// helper to hijack background erasing for the QT window
class WXDLLIMPEXP_MEDIA wxQTMediaEvtHandler : public wxEvtHandler
{
public:
    wxQTMediaEvtHandler(wxQTMediaBackend *qtb)
    {
        m_qtb = qtb;

        qtb->m_ctrl->Connect(
            qtb->m_ctrl->GetId(), wxEVT_ERASE_BACKGROUND,
            wxEraseEventHandler(wxQTMediaEvtHandler::OnEraseBackground),
            NULL, this);
    }

    void OnEraseBackground(wxEraseEvent& event);

private:
    wxQTMediaBackend *m_qtb;

    DECLARE_NO_COPY_CLASS(wxQTMediaEvtHandler)
};

// Window event handler
static pascal OSStatus wxQTMediaWindowEventHandler(
    EventHandlerCallRef inHandlerCallRef,
    EventRef inEvent, void *inUserData);

#endif

//===========================================================================
//  IMPLEMENTATION
//===========================================================================


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// wxQTMediaBackend
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IMPLEMENT_DYNAMIC_CLASS(wxQTMediaBackend, wxMediaBackend)

//Time between timer calls - this is the Apple recommondation to the TCL
//team I believe
#define MOVIE_DELAY 20

//---------------------------------------------------------------------------
//          wxQTMediaLoadTimer
//
//  QT, esp. QT for Windows is very picky about how you go about
//  async loading.  If you were to go through a Windows message loop
//  or a MoviesTask or both and then check the movie load state
//  it would still return 1000 (loading)... even (pre)prerolling doesn't
//  help.  However, making a load timer like this works
//---------------------------------------------------------------------------
class wxQTMediaLoadTimer : public wxTimer
{
public:
    wxQTMediaLoadTimer(Movie movie, wxQTMediaBackend* parent) :
      m_movie(movie), m_parent(parent) {}

    void Notify()
    {
        // Note that the CreateMovieControl variety performs
        // its own custom idling
#if !wxUSE_CREATEMOVIECONTROL
        ::MCIdle(m_parent->m_mc);
#endif

        // kMovieLoadStatePlayable is not enough on MAC:
        // it plays, but IsMovieDone might return true (!)
        // sure we need to wait until kMovieLoadStatePlaythroughOK
        if (::GetMovieLoadState(m_movie) >= 20000)
        {
            m_parent->FinishLoad();
            delete this;
        }
    }

protected:
    Movie m_movie;                  // Our movie instance
    wxQTMediaBackend *m_parent;     // Backend pointer
};

// --------------------------------------------------------------------------
//          wxQTMediaPlayTimer - Handle Asyncronous Playing
//
// 1) Checks to see if the movie is done, and if not continues
//    streaming the movie
// 2) Sends the wxEVT_MEDIA_STOP event if we have reached the end of
//    the movie.
// --------------------------------------------------------------------------
class wxQTMediaPlayTimer : public wxTimer
{
public:
    wxQTMediaPlayTimer(Movie movie, wxQTMediaBackend* parent) :
        m_movie(movie), m_parent(parent) {}

    void Notify()
    {
        //Note that CreateMovieControl performs its own idleing
#if !wxUSE_CREATEMOVIECONTROL
        //
        //  OK, a little explaining - basically originally
        //  we only called MoviesTask if the movie was actually
        //  playing (not paused or stopped)... this was before
        //  we realized MoviesTask actually handles repainting
        //  of the current frame - so if you were to resize
        //  or something it would previously not redraw that
        //  portion of the movie.
        //
        //  So now we call MoviesTask always so that it repaints
        //  correctly.
        //
        ::MCIdle(m_parent->m_mc);
#endif

        //  Handle the stop event - if the movie has reached
        //  the end, notify our handler
        if (::IsMovieDone(m_movie))
        {
            if ( m_parent->SendStopEvent() )
            {
                    m_parent->Stop();
                    wxASSERT(::GetMoviesError() == noErr);

                m_parent->QueueFinishEvent();
            }
        }
    }

protected:
    Movie m_movie;                  // Our movie instance
    wxQTMediaBackend* m_parent;     // Backend pointer
};


//---------------------------------------------------------------------------
// wxQTMediaBackend Constructor
//
// Sets m_timer to NULL signifying we havn't loaded anything yet
//---------------------------------------------------------------------------
wxQTMediaBackend::wxQTMediaBackend()
    : m_movie(NULL), m_bPlaying(false), m_timer(NULL)
      , m_mc(NULL), m_interfaceflags(wxMEDIACTRLPLAYERCONTROLS_NONE)
#if !wxUSE_CREATEMOVIECONTROL
      , m_preprerollupp(NULL)
#endif
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
    if (m_movie)
        Cleanup();

#if !wxUSE_CREATEMOVIECONTROL
    // Cleanup for moviecontroller
    if (m_mc)
    {
        // destroy wxQTMediaEvtHandler we pushed on it
        m_ctrl->PopEventHandler(true);
        RemoveEventHandler((EventHandlerRef&)m_pEventHandlerRef);
        DisposeEventHandlerUPP(m_eventupp);

        // Dispose of the movie controller
        ::DisposeMovieController(m_mc);
        DisposeMCActionFilterWithRefConUPP(m_mcactionupp);
    }
#endif

    // Note that ExitMovies() is not necessary...
    ExitMovies();
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::CreateControl
//
// 1) Intializes QuickTime
// 2) Creates the control window
//---------------------------------------------------------------------------
bool wxQTMediaBackend::CreateControl(
    wxControl* ctrl,
    wxWindow* parent,
    wxWindowID id,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    // Don't bother in Native control mode
#if !wxUSE_CREATEMOVIECONTROL
    if (!IsQuickTime4Installed())
        return false;
#endif

    EnterMovies();

    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    if ( !ctrl->wxControl::Create(
        parent, id, pos, size,
        wxWindow::MacRemoveBordersFromStyle(style),
        validator, name))
    {
        return false;
    }

#if wxUSE_VALIDATORS
    ctrl->SetValidator(validator);
#endif

    m_ctrl = (wxMediaCtrl*)ctrl;
    return true;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::IsQuickTime4Installed
//
// Determines whether version 4 of QT is installed
// (Pretty much for Classic only)
//---------------------------------------------------------------------------
#if !wxUSE_CREATEMOVIECONTROL
Boolean wxQTMediaBackend::IsQuickTime4Installed()
{
    OSErr error;
    long result;

    error = Gestalt(gestaltQuickTime, &result);
    return (error == noErr) && (((result >> 16) & 0xffff) >= 0x0400);
}
#endif

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
    if (m_movie)
        Cleanup();

    OSErr err = noErr;
    short movieResFile;
    FSSpec sfFile;

    // FIXME:wxMacFilename2FSSpec crashes on empty string -
    // does it crash on other strings too and should this
    // "fix" be put in the Carbon wxSound?
    if (fileName.empty())
        return false;

    wxMacFilename2FSSpec( fileName, &sfFile );
    if (OpenMovieFile( &sfFile, &movieResFile, fsRdPerm ) != noErr)
        return false;

    short movieResID = 0;
    Str255 movieName;
    bool result;

    err = NewMovieFromFile(
        &m_movie,
        movieResFile,
        &movieResID,
        movieName,
        newMovieActive,
        NULL); // wasChanged

    // No ::GetMoviesStickyError() here because it returns -2009
    // a.k.a. invalid track on valid mpegs
    result = (err == noErr);
    if (result)
    {
        ::CloseMovieFile(movieResFile);

        // Create movie controller/control
#if wxUSE_CREATEMOVIECONTROL
        DoCreateMovieControl();
#else
        DoNewMovieController();
#endif

        FinishLoad();
    }

    return result;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::PPRMProc (static)
//
// Called when done PrePrerolling the movie.
// Note that in 99% of the cases this does nothing...
// Anyway we set up the loading timer here to tell us when the movie is done
//---------------------------------------------------------------------------
#if !wxUSE_CREATEMOVIECONTROL
pascal void wxQTMediaBackend::PPRMProc(
    Movie theMovie,
    OSErr WXUNUSED_UNLESS_DEBUG(theErr),
    void* theRefCon)
{
    wxASSERT( theMovie );
    wxASSERT( theRefCon );
    wxASSERT( theErr == noErr );

    wxQTMediaBackend* pBE = (wxQTMediaBackend*) theRefCon;

    long lTime = ::GetMovieTime(theMovie,NULL);
    Fixed rate = ::GetMoviePreferredRate(theMovie);
    ::PrerollMovie(theMovie,lTime,rate);
    pBE->m_timer = new wxQTMediaLoadTimer(pBE->m_movie, pBE);
    pBE->m_timer->Start(MOVIE_DELAY);
}
#endif

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
    if (m_movie)
        Cleanup();

    wxString theURI = location.BuildURI();

    OSErr err = noErr;
    bool result;

    // FIXME: lurking Unicode problem here
    Handle theHandle = ::NewHandleClear(theURI.length() + 1);
    wxASSERT(theHandle);

    ::BlockMoveData(theURI.mb_str(), *theHandle, theURI.length() + 1);

    // create the movie from the handle that refers to the URI
    err = ::NewMovieFromDataRef(
        &m_movie,
        newMovieActive | newMovieAsyncOK /* | newMovieIdleImportOK*/,
        NULL, theHandle,
        URLDataHandlerSubType);

    ::DisposeHandle(theHandle);

    result = (err == noErr);
    if (result)
    {
#if wxUSE_CREATEMOVIECONTROL
        // Movie control does its own "(pre)prerolling"
        // but we still need to buffer the movie for the URL
        DoCreateMovieControl();

        // Setup timer to catch load event
        m_timer = new wxQTMediaLoadTimer(m_movie, this);
        m_timer->Start(MOVIE_DELAY);
#else
        // Movie controller resets prerolling, so we must create first
        DoNewMovieController();

        long timeNow;
        Fixed playRate;

        timeNow = ::GetMovieTime(m_movie, NULL);
        wxASSERT(::GetMoviesError() == noErr);

        playRate = ::GetMoviePreferredRate(m_movie);
        wxASSERT(::GetMoviesError() == noErr);

        //  Note that the callback here is optional,
        //  but without it PrePrerollMovie can be buggy
        //  (see Apple ml).  Also, some may wonder
        //  why we need this at all - this is because
        //  Apple docs say QuickTime streamed movies
        //  require it if you don't use a Movie Controller,
        //  which we don't by default.
        //
        m_preprerollupp = NewMoviePrePrerollCompleteUPP( wxQTMediaBackend::PPRMProc );
        ::PrePrerollMovie( m_movie, timeNow, playRate, m_preprerollupp, (void*)this);
#endif
    }

    return result;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::DoCreateMovieControl
//
// Calls CreateMovieControl and performs setup related to it
//
// Note that we always hide the controller initially becuase when loading
// from a url it displays about a 40x40 box with the word loading... in it,
// but the box is outside the range of the control, which is bad (0,0
// i believe), so we need to wait until finishload to actually display
// the movie controller in this instance
//---------------------------------------------------------------------------
#if wxUSE_CREATEMOVIECONTROL
void wxQTMediaBackend::DoCreateMovieControl()
{
    // Native CreateMovieControl QT control (Thanks to Kevin Olliver's
    // wxQTMovie for some of this).
    Rect bounds = wxMacGetBoundsForControl(
        m_ctrl,
        m_ctrl->GetPosition(),
        m_ctrl->GetSize());

    // Dispose of old control for new one
    if (m_ctrl->m_peer && m_ctrl->m_peer->Ok() )
        m_ctrl->m_peer->Dispose();

    // Options:
    // kMovieControlOptionXXX
    // HideController - hide the movie controller
    // LocateTopLeft - movie is pinned to top left rather than centered in the control
    // EnableEditing - Allows programmatic editing and dragn'drop
    // HandleEditingHI- Installs event stuff for edit menu - forces EnableEditing also
    // SetKeysEnabled - Allows keyboard input
    // ManuallyIdled - app handles movie idling rather than internal timer event loop
    ::CreateMovieControl(
        (WindowRef) m_ctrl->MacGetTopLevelWindowRef(), //parent
        &bounds,                         // control bounds
        m_movie,                         // movie handle
        kMovieControlOptionHideController// flags
            // | kMovieControlOptionManuallyIdled
            | kMovieControlOptionLocateTopLeft
            | kMovieControlOptionSetKeysEnabled,
        m_ctrl->m_peer->GetControlRefAddr() );

    ::EmbedControl(
        m_ctrl->m_peer->GetControlRef(),
        (ControlRef)m_ctrl->GetParent()->GetHandle());

    // set up MovieController for the new movie
    long dataSize;

    // Get movie controller from our control
    ::GetControlData(
        m_ctrl->m_peer->GetControlRef(), 0,
        kMovieControlDataMovieController,
        sizeof(MovieController), (Ptr)&m_mc, &dataSize );

    // Setup a callback so we can tell when the user presses
    // play on the player controls
    ::MCSetActionFilterWithRefCon(m_mc,
            (MCActionFilterWithRefConUPP)wxQTMediaBackend::MCFilterProc,
            (long)this);
}
#endif

//---------------------------------------------------------------------------
// wxQTMediaBackend::DoNewMovieController
//
// Attaches movie to moviecontroller or creates moviecontroller
// if not created yet
//---------------------------------------------------------------------------
#if !wxUSE_CREATEMOVIECONTROL
void wxQTMediaBackend::DoNewMovieController()
{
    if (!m_mc)
    {
        // Get top level window ref for some mac functions
        WindowRef wrTLW = (WindowRef) m_ctrl->MacGetTopLevelWindowRef();

        // MovieController not set up yet, so we need to create a new one.
        // You have to pass a valid movie to NewMovieController, evidently
        ::SetMovieGWorld(m_movie,
                       (CGrafPtr) GetWindowPort(wrTLW),
                       NULL);
        wxASSERT(::GetMoviesError() == noErr);

        Rect bounds = wxMacGetBoundsForControl(
            m_ctrl,
            m_ctrl->GetPosition(),
            m_ctrl->GetSize());

        m_mc = ::NewMovieController(
            m_movie, &bounds,
            mcTopLeftMovie | mcNotVisible /* | mcWithFrame */ );
        wxASSERT(::GetMoviesError() == noErr);

        ::MCDoAction(m_mc, 32, (void*)true); // mcActionSetKeysEnabled
        wxASSERT(::GetMoviesError() == noErr);

        // Setup a callback so we can tell when the user presses
        // play on the player controls
        m_mcactionupp = NewMCActionFilterWithRefConUPP( wxQTMediaBackend::MCFilterProc );
        ::MCSetActionFilterWithRefCon( m_mc, m_mcactionupp, (long)this );
        wxASSERT(::GetMoviesError() == noErr);

        // Part of a suggestion from Greg Hazel to repaint movie when idle
        m_ctrl->PushEventHandler(new wxQTMediaEvtHandler(this));

        // Event types to catch from the TLW
        // for the moviecontroller
        EventTypeSpec theEventTypes[] =
        {
            { kEventClassMouse,     kEventMouseDown },
            { kEventClassMouse,     kEventMouseUp },
            { kEventClassMouse,     kEventMouseDragged },
            { kEventClassKeyboard,  kEventRawKeyDown },
            { kEventClassKeyboard,  kEventRawKeyRepeat },
            { kEventClassKeyboard,  kEventRawKeyUp },
            { kEventClassWindow,    kEventWindowUpdate },
            { kEventClassWindow,    kEventWindowActivated },
            { kEventClassWindow,    kEventWindowDeactivated }
        };

        // Catch window messages:
        // if we do not do this and if the user clicks the play
        // button on the controller, for instance, nothing will happen...
        m_eventupp = NewEventHandlerUPP( wxQTMediaWindowEventHandler );
        InstallWindowEventHandler(
            wrTLW,
            m_eventupp,
            GetEventTypeCount( theEventTypes ), theEventTypes,
            m_mc, (&(EventHandlerRef&)m_pEventHandlerRef) );
    }
    else
    {
        // MovieController already created:
        // Just change the movie in it and we're good to go
        Point thePoint;
        thePoint.h = thePoint.v = 0;
        ::MCSetMovie(m_mc, m_movie,
              (WindowRef)m_ctrl->MacGetTopLevelWindowRef(),
              thePoint);
        wxASSERT(::GetMoviesError() == noErr);
    }
}
#endif

//---------------------------------------------------------------------------
// wxQTMediaBackend::FinishLoad
//
// Performs operations after a movie ready to play/loaded.
//---------------------------------------------------------------------------
void wxQTMediaBackend::FinishLoad()
{
    // Dispose of the PrePrerollMovieUPP if we used it
#if !wxUSE_CREATEMOVIECONTROL
    DisposeMoviePrePrerollCompleteUPP(m_preprerollupp);
#endif

    // get the real size of the movie
    DoLoadBestSize();

    // show the player controls if the user wants to
    if (m_interfaceflags)
        DoSetControllerVisible(m_interfaceflags);

    // we want millisecond precision
    ::SetMovieTimeScale(m_movie, 1000);
    wxASSERT(::GetMoviesError() == noErr);

    // start movie progress timer
    m_timer = new wxQTMediaPlayTimer(m_movie, (wxQTMediaBackend*) this);
    wxASSERT(m_timer);
    m_timer->Start(MOVIE_DELAY, wxTIMER_CONTINUOUS);

    // send loaded event and refresh size
    NotifyMovieLoaded();
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::DoLoadBestSize
//
// Sets the best size of the control from the real size of the movie
//---------------------------------------------------------------------------
void wxQTMediaBackend::DoLoadBestSize()
{
    // get the real size of the movie
    Rect outRect;
    ::GetMovieNaturalBoundsRect(m_movie, &outRect);
    wxASSERT(::GetMoviesError() == noErr);

    // determine best size
    m_bestSize.x = outRect.right - outRect.left;
    m_bestSize.y = outRect.bottom - outRect.top;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Play
//
// Start the QT movie
// (Apple recommends mcActionPrerollAndPlay but that's QT 4.1+)
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Play()
{
    Fixed fixRate = (Fixed) (wxQTMediaBackend::GetPlaybackRate() * 0x10000);
    if (!fixRate)
        fixRate = ::GetMoviePreferredRate(m_movie);

    wxASSERT(fixRate != 0);

    if (!m_bPlaying)
        ::MCDoAction( m_mc, 8 /* mcActionPlay */, (void*) fixRate);

    bool result = (::GetMoviesError() == noErr);
    if (result)
    {
        m_bPlaying = true;
        QueuePlayEvent();
    }

    return result;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Pause
//
// Stop the movie
//---------------------------------------------------------------------------
bool wxQTMediaBackend::DoPause()
{
    // Stop the movie A.K.A. ::StopMovie(m_movie);
    if (m_bPlaying)
    {
        ::MCDoAction( m_mc, 8 /*mcActionPlay*/,  (void *) 0);
        m_bPlaying = false;
        return ::GetMoviesError() == noErr;
    }

    // already paused
    return true;
}

bool wxQTMediaBackend::Pause()
{
    bool bSuccess = DoPause();
    if (bSuccess)
        this->QueuePauseEvent();

    return bSuccess;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Stop
//
// 1) Stop the movie
// 2) Seek to the beginning of the movie
//---------------------------------------------------------------------------
bool wxQTMediaBackend::Stop()
{
    if (!wxQTMediaBackend::Pause())
        return false;

    ::GoToBeginningOfMovie(m_movie);
    return ::GetMoviesError() == noErr;
}

bool wxQTMediaBackend::Stop()
{
    bool bSuccess = DoStop();
    if (bSuccess)
        QueueStopEvent();

    return bSuccess;
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
    short sVolume = ::GetMovieVolume(m_movie);

    if (sVolume & (128 << 8)) //negative - no sound
        return 0.0;

    return sVolume / 256.0;
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
    ::SetMovieVolume(m_movie, (short) (dVolume * 256));
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
    // Could use
    // GetMovieActive/IsMovieDone/SetMovieActive
    // combo if implemented that way
    if (m_bPlaying)
        return wxMEDIASTATE_PLAYING;
    else if ( !m_movie || wxQTMediaBackend::GetPosition() == 0)
        return wxMEDIASTATE_STOPPED;
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
    m_bPlaying = false;
    if (m_timer)
    {
        delete m_timer;
        m_timer = NULL;
    }

    // Stop the movie:
    // Apple samples with CreateMovieControl typically
    // install a event handler and do this on the dispose
    // event, but we do it here for simplicity
    // (It might keep playing for several seconds after
    // control destruction if not)
    wxQTMediaBackend::Pause();

    // Dispose of control or remove movie from MovieController
#if wxUSE_CREATEMOVIECONTROL
    if (m_ctrl->m_peer && m_ctrl->m_peer->Ok() )
        m_ctrl->m_peer->Dispose();
#else
    Point thePoint;
    thePoint.h = thePoint.v = 0;
    ::MCSetVisible(m_mc, false);
    ::MCSetMovie(m_mc, NULL, NULL, thePoint);
#endif

    ::DisposeMovie(m_movie);
    m_movie = NULL;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::MCFilterProc (static)
//
// Callback for when the movie controller recieves a message
//---------------------------------------------------------------------------
pascal Boolean wxQTMediaBackend::MCFilterProc(
    MovieController WXUNUSED(theController),
    short action,
    void * WXUNUSED(params),
    long refCon)
{
    wxQTMediaBackend* pThis = (wxQTMediaBackend*)refCon;

    switch (action)
    {
    case 1:
        // don't process idle events
        break;

    case 8:
        // play button triggered - MC will set movie to opposite state
        // of current - playing ? paused : playing
        pThis->m_bPlaying = !(pThis->m_bPlaying);
        break;

    default:
        break;
    }

    return 0;
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
// Move the movie controller or movie control
// (we need to actually move the movie control manually...)
// Top 10 things to do with quicktime in March 93's issue
// of DEVELOP - very useful
// http:// www.mactech.com/articles/develop/issue_13/031-033_QuickTime_column.html
// OLD NOTE: Calling MCSetControllerBoundsRect without detaching
//          supposively resulted in a crash back then. Current code even
//          with CFM classic runs fine. If there is ever a problem,
//          take out the if 0 lines below
//---------------------------------------------------------------------------
void wxQTMediaBackend::Move(int x, int y, int w, int h)
{
#if !wxUSE_CREATEMOVIECONTROL
    if (m_timer)
    {
        m_ctrl->GetParent()->MacWindowToRootWindow(&x, &y);
        Rect theRect = {y, x, y + h, x + w};

#if 0
        // see note above
        ::MCSetControllerAttached(m_mc, false);
         wxASSERT(::GetMoviesError() == noErr);
#endif

        ::MCSetControllerBoundsRect(m_mc, &theRect);
        wxASSERT(::GetMoviesError() == noErr);

#if 0 // see note above
        if (m_interfaceflags)
        {
            ::MCSetVisible(m_mc, true);
            wxASSERT(::GetMoviesError() == noErr);
        }
#endif
    }
#else

    if (m_timer && m_ctrl)
    {
        m_ctrl->GetParent()->MacWindowToRootWindow( &x, &y );
        ::MoveControl( (ControlRef) m_ctrl->GetHandle(), x, y );
        m_ctrl->GetParent()->Refresh();
        m_ctrl->GetParent()->Update();
    }
#endif
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::DoSetControllerVisible
//
// Utility function that takes care of showing the moviecontroller
// and showing/hiding the particular controls on it
//---------------------------------------------------------------------------
void wxQTMediaBackend::DoSetControllerVisible(wxMediaCtrlPlayerControls flags)
{
    ::MCSetVisible(m_mc, true);

    // Take care of subcontrols
    if (::GetMoviesError() == noErr)
    {
        long mcFlags = 0;
        ::MCDoAction(m_mc, 39/*mcActionGetFlags*/, (void*)&mcFlags);

        if (::GetMoviesError() == noErr)
        {
             mcFlags |= (  //(1<<0)/*mcFlagSuppressMovieFrame*/ |
                     (1 << 3)/*mcFlagsUseWindowPalette*/
                       | ((flags & wxMEDIACTRLPLAYERCONTROLS_STEP)
                          ? 0 : (1 << 1)/*mcFlagSuppressStepButtons*/)
                       | ((flags & wxMEDIACTRLPLAYERCONTROLS_VOLUME)
                          ? 0 : (1 << 2)/*mcFlagSuppressSpeakerButton*/)
         //              | (1 << 4) /*mcFlagDontInvalidate*/ //if we take care of repainting ourselves
                          );

            ::MCDoAction(m_mc, 38/*mcActionSetFlags*/, (void*)mcFlags);
        }
    }

    // Adjust height and width of best size for movie controller
    // if the user wants it shown
    m_bestSize.x = m_bestSize.x > wxMCWIDTH ? m_bestSize.x : wxMCWIDTH;
    m_bestSize.y += wxMCHEIGHT;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::ShowPlayerControls
//
// Shows/Hides subcontrols on the media control
//---------------------------------------------------------------------------
bool wxQTMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    if (!m_mc)
        return false; // no movie controller...

    bool bSizeChanged = false;

    // if the controller is visible and we want to hide it do so
    if (m_interfaceflags && !flags)
    {
        bSizeChanged = true;
        DoLoadBestSize();
        ::MCSetVisible(m_mc, false);
    }
    else if (!m_interfaceflags && flags) // show controller if hidden
    {
        bSizeChanged = true;
        DoSetControllerVisible(flags);
    }

    // readjust parent sizers
    if (bSizeChanged)
    {
        NotifyMovieSizeChanged();

        // remember state in case of loading new media
        m_interfaceflags = flags;
    }

    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetDataSizeFromStart
//
// Calls either GetMovieDataSize or GetMovieDataSize64 with a value
// of 0 for the starting value
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDataSizeFromStart(TimeValue end)
{
#if 0 // old pre-qt4 way
    return ::GetMovieDataSize(m_movie, 0, end)
#else // qt4 way
    wide llDataSize;
    ::GetMovieDataSize64(m_movie, 0, end, &llDataSize);
    return wxLongLong(llDataSize.hi, llDataSize.lo);
#endif
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetDownloadProgress
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDownloadProgress()
{
#if 0 // hackish and slow
    Handle hMovie = NewHandle(0);
    PutMovieIntoHandle(m_movie, hMovie);
    long lSize = GetHandleSize(hMovie);
    DisposeHandle(hMovie);

    return lSize;
#else
    TimeValue tv;
    if (::GetMaxLoadedTimeInMovie(m_movie, &tv) != noErr)
    {
        wxLogDebug(wxT("GetMaxLoadedTimeInMovie failed"));
        return 0;
    }

    return wxQTMediaBackend::GetDataSizeFromStart(tv);
#endif
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::GetDownloadTotal
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDownloadTotal()
{
    return wxQTMediaBackend::GetDataSizeFromStart( ::GetMovieDuration(m_movie) );
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::OnEraseBackground
//
// Suggestion from Greg Hazel to repaint the movie when idle
// (on pause also)
//---------------------------------------------------------------------------
#if !wxUSE_CREATEMOVIECONTROL
void wxQTMediaEvtHandler::OnEraseBackground(wxEraseEvent& evt)
{
    // Work around Nasty OSX drawing bug:
    // http://lists.apple.com/archives/QuickTime-API/2002/Feb/msg00311.html
    WindowRef wrTLW = (WindowRef) m_qtb->m_ctrl->MacGetTopLevelWindowRef();

    RgnHandle region = MCGetControllerBoundsRgn(m_qtb->m_mc);
    MCInvalidate(m_qtb->m_mc, wrTLW, region);
    MCIdle(m_qtb->m_mc);
}
#endif

//---------------------------------------------------------------------------
// wxQTMediaWindowEventHandler
//
// Event callback for the top level window of our control that passes
// messages to our moviecontroller so it can receive mouse clicks etc.
//---------------------------------------------------------------------------
#if !wxUSE_CREATEMOVIECONTROL
static pascal OSStatus wxQTMediaWindowEventHandler(
    EventHandlerCallRef inHandlerCallRef,
    EventRef inEvent,
    void *inUserData)
{
    // for the overly paranoid....
#if 0
    UInt32 eventClass = GetEventClass( eventRef );
    UInt32 eventKind = GetEventKind( inEvent );

    if (eventKind != kEventMouseDown &&
       eventKind != kEventMouseUp &&
       eventKind != kEventMouseDragged &&
       eventKind != kEventRawKeyDown &&
       eventKind != kEventRawKeyRepeat &&
       eventKind != kEventRawKeyUp &&
       eventKind != kEventWindowUpdate &&
       eventKind != kEventWindowActivated &&
       eventKind != kEventWindowDeactivated)
            return eventNotHandledErr;
#endif

    EventRecord theEvent;
    ConvertEventRefToEventRecord( inEvent, &theEvent );
    OSStatus err;

    err = ::MCIsPlayerEvent( (MovieController) inUserData, &theEvent );

    // pass on to other event handlers if not handled- i.e. wx
    if (err != noErr)
        return noErr;
    else
        return eventNotHandledErr;
}
#endif

// in source file that contains stuff you don't directly use
#include "wx/html/forcelnk.h"
FORCE_LINK_ME(basewxmediabackends)

#endif // wxUSE_MEDIACTRL
