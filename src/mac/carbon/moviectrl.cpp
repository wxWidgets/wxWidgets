/////////////////////////////////////////////////////////////////////////////
// Name:        mac/carbon/moviectrl.cpp
// Purpose:     wxMovieCtrl MAC CARBON QT
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

#define wxUSE_MOVIECTRL 1

#if wxUSE_MOVIECTRL

#include "wx/moviectrl.h"
#include "wx/sound.h"
#include "wx/timer.h"

IMPLEMENT_CLASS(wxMovieCtrl, wxControl);

BEGIN_EVENT_TABLE(wxMovieCtrl, wxControl)
    EVT_SIZE(wxMovieCtrl::OnSize)
END_EVENT_TABLE()

//MESSY headers
#ifdef __WXMAC__
#include "wx/mac/uma.h"
#include <Movies.h>
#include <Gestalt.h>
#endif

//quicktime media layer only required for mac emulation on pc
#ifndef __WXMAC__
#include <qtml.h>
#endif

#include <QuickTimeComponents.h>

//Time between timer calls
#define MOVIE_DELAY 100

// ------------------------------------------------------------------
//          wxQTTimer - Handle Asyncronous Playing
// ------------------------------------------------------------------
class _wxQTTimer : public wxTimer
{
public:
    _wxQTTimer(Movie movie) :
        m_movie(movie), m_bPaused(false)
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
                Stop();
        }
    }

protected:
    Movie m_movie;
    bool m_bPaused;
};

//Determines whether version 3 of QT is installed
Boolean wxIsQuickTime3Installed (void)
{
#ifdef __WXMAC__
    short error;
    long result;

    error = Gestalt (gestaltQuickTime, &result);
    return (error == noErr) && (((result >> 16) & 0xffff) >= 0x0300);
#else
    return true;
#endif
}

bool wxMovieCtrl::InitQT ()
{
    if (wxIsQuickTime3Installed())
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

bool wxMovieCtrl::Create(wxWindow* parent, wxWindowID id, const wxString& fileName, 
                         const wxString& label, const wxPoint& pos, const wxSize& size, 
                         long WXUNUSED(style), const wxString& name)
{
    if ( !InitQT() )
        return false;

    OSErr err = noErr;
    short movieResFile;
    FSSpec sfFile;
#ifdef __WXMAC__
    wxMacFilename2FSSpec( m_sndname , &sfFile ) ;
#else
    int nError;
    if ((nError = NativePathNameToFSSpec ((char*) fileName.c_str(), &sfFile, 0)) != noErr)
    {
        wxFAIL_MSG(wxString::Format(wxT("File:%s does not exist\nError:%i"),
                        fileName.c_str(), nError));
        return false;
    }
#endif
    if (OpenMovieFile (&sfFile, &movieResFile, fsRdPerm) != noErr)
    {
        wxFAIL_MSG(wxT("Quicktime couldn't open the file"));
        return false;
    }
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
    {
        wxFAIL_MSG(wxT("Could not create movie"));
        return false;
    }

    m_timer = new _wxQTTimer(m_movie);
    wxASSERT(m_timer);

    //get the real size of the movie
    Rect outRect;
    ::GetMovieNaturalBoundsRect (m_movie, &outRect);

    m_bestSize.x = outRect.right - outRect.left;
    m_bestSize.y = outRect.bottom - outRect.top;

    //do some window stuff
    if ( !wxControl::Create(parent, id, pos, size, wxNO_BORDER, wxDefaultValidator, name) )
        return false;

    //Set our background color to black by default
    SetBackgroundColour(*wxBLACK);

    //reparent movie
#ifdef __WXMSW__
    CreatePortAssociation(this->GetHWND(), NULL, 0L);
#endif
    SetMovieGWorld(m_movie, (CGrafPtr)

#ifdef __WXMSW__
    GetNativeWindowPort(this->GetHWND())
#else
    this->GetHandle()
#endif
    , nil);

    //go!
    SetLabel(label);
    Play();

    return true;
}

void wxMovieCtrl::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
}

bool wxMovieCtrl::Play()
{
    ::StartMovie(m_movie);
    m_timer->SetPaused(false);
    m_timer->Start(MOVIE_DELAY, wxTIMER_CONTINUOUS);
    return ::GetMoviesError() == noErr;
}

bool wxMovieCtrl::Pause()
{
    ::StopMovie(m_movie);
    m_timer->SetPaused(true);
    return ::GetMoviesError() == noErr;
}

bool wxMovieCtrl::Stop()
{
    m_timer->SetPaused(false);

    ::StopMovie(m_movie);
    if(::GetMoviesError() != noErr)
        return false;
    
    ::GoToEndOfMovie(m_movie);
    return ::GetMoviesError() == noErr;
}

#if wxUSE_DATETIME

bool wxMovieCtrl::Seek(const wxTimeSpan& where)
{
    TimeRecord theTimeRecord;
    theTimeRecord.value.lo = ((size_t)where.GetMilliseconds().ToLong()) * 10;
    theTimeRecord.scale = ::GetMovieTimeScale(m_movie);
    theTimeRecord.base = ::GetMovieTimeBase(m_movie);
    ::SetMovieTime(m_movie, &theTimeRecord);

    if (::GetMoviesError() != noErr)
        return false;

    return true;
}

#endif // wxUSE_DATETIME

wxMovieCtrlState wxMovieCtrl::GetState()
{
    if( m_timer->IsRunning() == true )
        return wxMOVIECTRL_STOPPED;

    if ( m_timer->GetPaused() == false )
        return wxMOVIECTRL_PLAYING;
    else
        return wxMOVIECTRL_PAUSED;
}

wxMovieCtrl::~wxMovieCtrl()
{
    if (m_timer)
    {
        delete m_timer;

        StopMovie(m_movie);
        DisposeMovie(m_movie);
        
        //Note that ExitMovies() is not neccessary, but
        //the docs are fuzzy on whether or not TerminateQTML is
        ExitMovies();

    #ifndef __WXMAC__
        TerminateQTML();
    #endif
    }
}

wxSize wxMovieCtrl::DoGetBestSize() const
{
    return m_bestSize;
}

void wxMovieCtrl::OnSize(wxSizeEvent& evt)
{
    Rect theRect;
    theRect.left = 0;
    theRect.top = 0;
    theRect.bottom = evt.GetSize().y;
    theRect.right = evt.GetSize().x;

    ::SetMovieBox(m_movie, &theRect);

    wxASSERT(::GetMoviesError() == noErr);
    evt.Skip();
}
#endif //wxUSE_MOVIECTRL
