/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/mediactrl.cpp
// Purpose:     Built-in Media Backends for Cocoa
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     02/03/05
// RCS-ID:      $Id: mediactrl.mm 39285 2006-05-23 11:04:37Z ABX $
// Copyright:   (c) 2004-2005 Ryan Norton, (c) 2005 David Elliot
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
//  DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
// Pre-compiled header stuff
//---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//---------------------------------------------------------------------------
// Compilation guard
//---------------------------------------------------------------------------
#if wxUSE_MEDIACTRL

#include "wx/mediactrl.h"

#ifndef WX_PRECOMP
    #include "wx/timer.h"
#endif

#include "wx/osx/private.h"

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
#include <QTKit/QTKit.h>

#include "wx/cocoa/autorelease.h"
#include "wx/cocoa/string.h"

#import <AppKit/NSMovie.h>
#import <AppKit/NSMovieView.h>


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
    virtual bool SetVolume(double dVolume);
    
    void Cleanup();
    void FinishLoad();

    virtual bool   ShowPlayerControls(wxMediaCtrlPlayerControls flags);
private:
    void DoShowPlayerControls(wxMediaCtrlPlayerControls flags);
    
    wxSize m_bestSize;              //Original movie size
    QTMovie* m_movie;               //QTMovie handle/instance
    QTMovieView* m_movieview;       //QTMovieView instance
    wxControl* m_ctrl;              //Parent control

    wxMediaCtrlPlayerControls m_interfaceflags; // Saved interface flags

    DECLARE_DYNAMIC_CLASS(wxQTMediaBackend);
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
    _wxQTTimer(QTMovie movie, wxQTMediaBackend* parent) :
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
#if 0
        if (!m_bPaused)
        {
            if(!IsMovieDone(m_movie))
                MoviesTask(m_movie, MOVIE_DELAY);
            else
            {
                wxMediaEvent theEvent(wxEVT_MEDIA_STOP,
                                      m_parent->m_ctrl->GetId());
                m_parent->m_ctrl->GetEventHandler()->ProcessEvent(theEvent);

                if(theEvent.IsAllowed())
                {
                    Stop();
                    m_parent->Stop();
                    wxASSERT(::GetMoviesError() == noErr);

                    //send the event to our child
                    wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                          m_parent->m_ctrl->GetId());
                    m_parent->m_ctrl->GetEventHandler()->ProcessEvent(theEvent);
                }
            }
        }
#endif
    }

protected:
    QTMovie m_movie;                  //Our movie instance
    bool m_bPaused;                 //Whether we are paused or not
    wxQTMediaBackend* m_parent;     //Backend pointer
};

//---------------------------------------------------------------------------
// wxQTMediaBackend Constructor
//
// Sets m_timer to NULL signifying we havn't loaded anything yet
//---------------------------------------------------------------------------
wxQTMediaBackend::wxQTMediaBackend() : 
    m_interfaceflags(wxMEDIACTRLPLAYERCONTROLS_NONE),
    m_movie(nil), m_movieview(nil), m_ctrl(NULL)
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
    [m_movie release];
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::CreateControl
//
// 1) Intializes QuickTime
// 2) Creates the control window
//---------------------------------------------------------------------------
bool wxQTMediaBackend::CreateControl(wxControl* inctrl, wxWindow* parent,
                                     wxWindowID wid,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    wxMediaCtrl* mediactrl = (wxMediaCtrl*) inctrl;

    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those
    //
    // Since we don't have a child window like most other
    // backends, we don't need wxCLIP_CHILDREN
    //
    if ( !mediactrl->wxControl::Create(
                                       parent, wid, pos, size,
                                       wxWindow::MacRemoveBordersFromStyle(style),
                                       validator, name))
    {
        return false;
    }
    
    NSRect r = wxOSXGetFrameForControl( mediactrl, pos , size ) ;
    QTMovieView* theView = [[QTMovieView alloc] initWithFrame: r];

    wxWidgetCocoaImpl* impl = new wxWidgetCocoaImpl(mediactrl,theView);
    mediactrl->SetPeer(impl);
    
    m_movieview = theView;
    // will be set up after load
    [theView setControllerVisible:NO];

    m_ctrl = mediactrl;
    return true;
}

bool wxQTMediaBackend::Load(const wxString& fileName)
{
    return Load(
                wxURI(
                    wxString( wxT("file://") ) + fileName
                     )
               );
}

bool wxQTMediaBackend::Load(const wxURI& location)
{
    wxString theURI = location.BuildURI();

    QTMovie* movie = [[QTMovie alloc] initWithURL: [NSURL URLWithString: wxNSStringWithWxString(theURI)] error: nil ];

    [m_movie release];
    m_movie = movie;
    
    [m_movieview setMovie:movie];
    
    DoShowPlayerControls(m_interfaceflags);

    FinishLoad();

    return movie != nil;
}

void wxQTMediaBackend::FinishLoad()
{
    NSRect r =[m_movieview movieBounds];
    m_bestSize.x = r.size.width;
    m_bestSize.y = r.size.height;

    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
}

bool wxQTMediaBackend::Play()
{
    [m_movieview play:nil];
    return true;
}

bool wxQTMediaBackend::Pause()
{
    [m_movieview pause:nil];
    return true;
}

bool wxQTMediaBackend::Stop()
{
    [m_movieview pause:nil];
    [m_movieview gotoBeginning:nil];
    return true;
}

double wxQTMediaBackend::GetVolume()
{
    return [m_movie volume];
}

bool wxQTMediaBackend::SetVolume(double dVolume)
{
    [m_movie setVolume:dVolume];
    return true;
}
double wxQTMediaBackend::GetPlaybackRate()
{
    return [m_movie rate];
}

bool wxQTMediaBackend::SetPlaybackRate(double dRate)
{
    [m_movie setRate:dRate];
    return true;
}

bool wxQTMediaBackend::SetPosition(wxLongLong where)
{
    QTTime position;
    position = [m_movie currentTime];
    position.timeValue = (where.GetValue() / 1000.0) * position.timeScale;
    [m_movie setCurrentTime:position];
    return true;
}

wxLongLong wxQTMediaBackend::GetPosition()
{
    QTTime position = [m_movie currentTime];
    return ((double) position.timeValue) / position.timeScale * 1000;
}

wxLongLong wxQTMediaBackend::GetDuration()
{
    QTTime duration = [m_movie duration];
    return ((double) duration.timeValue) / duration.timeScale * 1000;
}

wxMediaState wxQTMediaBackend::GetState()
{
    /*
    if ( !m_timer || (m_timer->IsRunning() == false &&
                      m_timer->GetPaused() == false) )
        return wxMEDIASTATE_STOPPED;

    if( m_timer->IsRunning() == true )
        return wxMEDIASTATE_PLAYING;
    else
     */
        return wxMEDIASTATE_PAUSED;
}

void wxQTMediaBackend::Cleanup()
{
    [[m_movieview movie] release];
    [m_movieview setMovie:NULL];
}

wxSize wxQTMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

void wxQTMediaBackend::Move(int x, int y, int w, int h)
{
}

bool wxQTMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    if ( m_interfaceflags != flags )
        DoShowPlayerControls(flags);
    
    m_interfaceflags = flags;    
    return true;
}

void wxQTMediaBackend::DoShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    if (flags == wxMEDIACTRLPLAYERCONTROLS_NONE )
    {
        [m_movieview setControllerVisible:NO];
    }
    else 
    {
        [m_movieview setStepButtonsVisible:(flags & wxMEDIACTRLPLAYERCONTROLS_STEP) ? YES:NO];
        [m_movieview setVolumeButtonVisible:(flags & wxMEDIACTRLPLAYERCONTROLS_VOLUME) ? YES:NO];
    }
}

//in source file that contains stuff you don't directly use
#include "wx/html/forcelnk.h"
FORCE_LINK_ME(basewxmediabackends);

#endif //wxUSE_MEDIACTRL
