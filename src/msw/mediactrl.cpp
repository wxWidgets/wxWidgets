/////////////////////////////////////////////////////////////////////////////
// Name:        msw/mediactrl.cpp
// Purpose:     Built-in Media Backends for Windows
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

//---------------------------------------------------------------------------
// Externals (somewhere in src/msw/app.cpp)
//---------------------------------------------------------------------------
extern "C" WXDLLIMPEXP_BASE HINSTANCE wxGetInstance(void);
#ifdef __WXWINCE__
extern WXDLLIMPEXP_CORE      wxChar *wxCanvasClassName;
#else
extern WXDLLIMPEXP_CORE const wxChar *wxCanvasClassName;
#endif

//===========================================================================
//  BACKEND DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//
//  wxAMMediaBackend
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  Compilation guard for DirectShow
//---------------------------------------------------------------------------
#if wxUSE_DIRECTSHOW

//---------------------------------------------------------------------------
//  DirectShow includes
//---------------------------------------------------------------------------
#include <dshow.h>

class WXDLLIMPEXP_MEDIA wxAMMediaBackend : public wxMediaBackend
{
public:
    wxAMMediaBackend();

    virtual ~wxAMMediaBackend();

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
    virtual bool SetPlaybackRate(double);

    void Cleanup();

    bool m_bVideo;

    static LRESULT CALLBACK NotifyWndProc(HWND hWnd, UINT nMsg,
                                          WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                          WPARAM wParam, LPARAM lParam);

    wxControl* m_ctrl;

    IGraphBuilder* m_pGB;
    IMediaControl* m_pMC;
    IMediaEventEx* m_pME;
    IVideoWindow* m_pVW;
    IBasicAudio* m_pBA;
    IBasicVideo* m_pBV;
    IMediaSeeking* m_pMS;

    HWND m_hNotifyWnd;
    wxSize m_bestSize;

    DECLARE_DYNAMIC_CLASS(wxAMMediaBackend);
};

#endif //wxUSE_DIRECTSHOW

//---------------------------------------------------------------------------
//
//  wxMCIMediaBackend
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MCI Includes
//---------------------------------------------------------------------------
#include <mmsystem.h>

class WXDLLIMPEXP_MEDIA wxMCIMediaBackend : public wxMediaBackend
{
public:

    wxMCIMediaBackend();
    ~wxMCIMediaBackend();

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

    static LRESULT CALLBACK NotifyWndProc(HWND hWnd, UINT nMsg,
                                             WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                     WPARAM wParam, LPARAM lParam);

    MCIDEVICEID m_hDev;     //Our MCI Device ID/Handler
    wxControl* m_ctrl;      //Parent control
    HWND m_hNotifyWnd;      //Window to use for MCI events
    bool m_bVideo;          //Whether or not we have video

    DECLARE_DYNAMIC_CLASS(wxMCIMediaBackend);
};

//---------------------------------------------------------------------------
//
//  wxQTMediaBackend
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  QT Compilation Guard
//---------------------------------------------------------------------------
#if wxUSE_QUICKTIME

//---------------------------------------------------------------------------
//  QT Includes
//---------------------------------------------------------------------------
#include <qtml.h>                   //Windoze QT include
#include <QuickTimeComponents.h>    //Standard QT stuff

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
    struct MovieRecord* m_movie;    //QT Movie handle/instance
    wxControl* m_ctrl;              //Parent control
    bool m_bVideo;                  //Whether or not we have video
    class _wxQTTimer* m_timer;      //Timer for streaming the movie

    DECLARE_DYNAMIC_CLASS(wxQTMediaBackend);
};

//---------------------------------------------------------------------------
//  End QT Compilation Guard
//---------------------------------------------------------------------------
#endif //wxUSE_QUICKTIME

//===========================================================================
//  IMPLEMENTATION
//===========================================================================

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxAMMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Only use if user wants it -
//---------------------------------------------------------------------------
#if wxUSE_DIRECTSHOW

IMPLEMENT_DYNAMIC_CLASS(wxAMMediaBackend, wxMediaBackend);

// Numerical value for when the graph reaches the stop position
#define WM_GRAPHNOTIFY  WM_USER+13

//---------------------------------------------------------------------------
// Usual debugging macros
//---------------------------------------------------------------------------
#ifdef __WXDEBUG__
#define wxAMVERIFY(x) \
{ \
    HRESULT hrdsv = (x); \
    if ( FAILED(hrdsv) ) \
    { \
        /*TCHAR szError[MAX_ERROR_TEXT_LEN];*/ \
        /*if( AMGetErrorText(hrdsv, szError, MAX_ERROR_TEXT_LEN) == 0)*/ \
        /*{*/ \
            /*wxFAIL_MSG( wxString::Format(wxT("DirectShow error \"%s\" ")*/\
                                         /*wxT("occured at line %i in ")*/ \
                                         /*wxT("mediactrl.cpp"),*/ \
                                            /*szError, __LINE__) );*/ \
        /*}*/ \
        /*else*/ \
            wxFAIL_MSG( wxString::Format(wxT("Unknown error (%i) ") \
                                         wxT("occured at") \
                                         wxT(" line %i in mediactrl.cpp."), \
                                         (int)hrdsv, __LINE__) ); \
    } \
}
#define wxVERIFY(x) wxASSERT((x))
#else
#define wxAMVERIFY(x) (x)
#define wxVERIFY(x) (x)
#endif

//---------------------------------------------------------------------------
// Standard macros for ease of use
//---------------------------------------------------------------------------
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

//---------------------------------------------------------------------------
// wxAMMediaBackend Constructor
//
// Sets m_hNotifyWnd to NULL to signify that we haven't loaded anything yet
//---------------------------------------------------------------------------
wxAMMediaBackend::wxAMMediaBackend() : m_hNotifyWnd(NULL)
{
}

//---------------------------------------------------------------------------
// wxAMMediaBackend Destructor
//
// Cleans up everything
//---------------------------------------------------------------------------
wxAMMediaBackend::~wxAMMediaBackend()
{
    if (m_hNotifyWnd)
        Cleanup();
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::CreateControl
//
// ActiveMovie does not really have any native control to speak of,
// so we just create a normal control with a black background.
//
// We also check to see if ActiveMovie is installed
//---------------------------------------------------------------------------
bool wxAMMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    //create our filter graph
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                      IID_IGraphBuilder, (void**)&m_pGB);

   //directshow not installed?
    if ( FAILED(hr) )
        return false;

    //release the filter graph - we don't need it yet
    m_pGB->Release();
    m_pGB = NULL;

    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those, and create with
    // wxCLIP_CHILDREN, so that if the driver/backend
    // is a child window, it refereshes properly
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            (style | wxNO_BORDER) | wxCLIP_CHILDREN,
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
// wxAMMediaBackend::Load (file version)
//
// Creates an Active Movie filter graph from a file or url
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxString& fileName)
{
    if(m_hNotifyWnd)
        Cleanup();

    CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                      IID_IGraphBuilder, (void**)&m_pGB);

    //load the graph & render
    if( FAILED(m_pGB->RenderFile(fileName.wc_str(wxConvLocal), NULL)) )
        return false;

    //get the interfaces, all of them
    wxAMVERIFY( m_pGB->QueryInterface(IID_IMediaControl, (void**)&m_pMC) );
    wxAMVERIFY( m_pGB->QueryInterface(IID_IMediaEventEx, (void**)&m_pME) );
    wxAMVERIFY( m_pGB->QueryInterface(IID_IMediaSeeking, (void**)&m_pMS) );
    wxAMVERIFY( m_pGB->QueryInterface(IID_IVideoWindow, (void**)&m_pVW) );
    wxAMVERIFY( m_pGB->QueryInterface(IID_IBasicAudio, (void**)&m_pBA) );
    wxAMVERIFY( m_pGB->QueryInterface(IID_IBasicVideo, (void**)&m_pBV) );

    //We could tell if the media has audio or not by
    //something like
    //-----
    //long lVolume;
    //pBA->get_Volume(&lVolume) == E_NOTIMPL
    //-----
    //here...

    //
    //Obtain the _actual_ size of the movie & remember it
    //
    long    nX,
            nY;

    m_bestSize.x = m_bestSize.y = 0;

    m_bVideo = SUCCEEDED( m_pVW->GetWindowPosition( &nX,
                                                    &nY,
                                                    (long*)&m_bestSize.x,
                                                    (long*)&m_bestSize.y) );

    //
    //If we have video in the media - set it up so that
    //its a child window of the control, its visible,
    //and that the control is the owner of the video window
    //
    if (m_bVideo)
    {
        wxAMVERIFY( m_pVW->put_Owner((OAHWND)m_ctrl->GetHandle()) );
        wxAMVERIFY( m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS) );
        wxAMVERIFY( m_pVW->put_Visible(OATRUE) ); //OATRUE == -1
    }

    //
    // Create a hidden window and register to handle
    // directshow events for this graph
    // Note that wxCanvasClassName is already registered
    // and used by all wxWindows and normal wxControls
    //
    m_hNotifyWnd = ::CreateWindow
                    (
                        wxCanvasClassName,
                        NULL,
                        0, 0, 0, 0,
                        0,
                        (HWND) NULL,
                        (HMENU)NULL,
                        wxGetInstance(),
                        (LPVOID) NULL
                    );

    if(!m_hNotifyWnd)
    {
        wxLogSysError( wxT("Could not create hidden needed for ")
                       wxT("registering for DirectShow events!")  );

        return false;
    }

    ::SetWindowLongPtr(m_hNotifyWnd, GWLP_WNDPROC,
                       (LONG_PTR)wxAMMediaBackend::NotifyWndProc);

    ::SetWindowLong(m_hNotifyWnd, GWL_USERDATA,
                       (LONG) this);

    wxAMVERIFY( m_pME->SetNotifyWindow((OAHWND)m_hNotifyWnd,
                                       WM_GRAPHNOTIFY, 0) );

    //
    //set the time format
    //
    wxAMVERIFY( m_pMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) );

    //
    // Force the parent window of this control to recalculate
    // the size of this if sizers are being used
    // and render the results immediately
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();

    return true;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Load (URL Version)
//
// Loads media from a URL.  Interestingly enough DirectShow
// appears (?) to escape the URL for us, at least on normal
// files
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Load(const wxURI& location)
{
    return Load(location.BuildUnescapedURI());
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Play
//
// Plays the stream.  If it is non-seekable, it will restart it.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Play()
{
    return SUCCEEDED( m_pMC->Run() );
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Pause
//
// Pauses the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Pause()
{
    return SUCCEEDED( m_pMC->Pause() );
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Stop
//
// Stops the stream.
//---------------------------------------------------------------------------
bool wxAMMediaBackend::Stop()
{
    bool bOK = SUCCEEDED( m_pMC->Stop() );

    //We don't care if it can't get to the beginning in directshow -
    //it could be a non-seeking filter (wince midi) in which case playing
    //starts all over again
    SetPosition(0);
    return bOK;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::SetPosition
//
// 1) Translates the current position's time to directshow time,
//    which is in a scale of 100 nanoseconds
// 2) Sets the play position of the IMediaSeeking interface -
//    passing NULL as the stop position means to keep the old
//    stop position
//---------------------------------------------------------------------------
bool wxAMMediaBackend::SetPosition(wxLongLong where)
{
    LONGLONG pos = ((LONGLONG)where.GetValue()) * 10000;

    return SUCCEEDED( m_pMS->SetPositions(
                                &pos,
                                AM_SEEKING_AbsolutePositioning,
                                NULL,
                                AM_SEEKING_NoPositioning
                                    ) );
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetPosition
//
// 1) Obtains the current play and stop positions from IMediaSeeking
// 2) Returns the play position translated to our time base
//---------------------------------------------------------------------------
wxLongLong wxAMMediaBackend::GetPosition()
{
    LONGLONG outCur, outStop;
    wxAMVERIFY( m_pMS->GetPositions(&outCur, &outStop) );

    //h,m,s,milli - outdur is in 100 nanos
    return outCur/10000;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetDuration
//
// 1) Obtains the duration of the media from the IMediaSeeking interface
// 2) Converts that value to our time base, and returns it
//---------------------------------------------------------------------------
wxLongLong wxAMMediaBackend::GetDuration()
{
    LONGLONG outDuration;
    wxAMVERIFY( m_pMS->GetDuration(&outDuration) );

    //h,m,s,milli - outdur is in 100 nanos
    return outDuration/10000;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetState
//
// Obtains the state from the IMediaControl interface.
// Note that it's enumeration values for stopping/playing
// etc. are the same as ours, so we just do a straight cast.
// TODO: MS recommends against INFINITE here for
//       IMediaControl::GetState- do it in stages
//---------------------------------------------------------------------------
wxMediaState wxAMMediaBackend::GetState()
{
    HRESULT hr;
    OAFilterState theState;
    hr = m_pMC->GetState(INFINITE, &theState);

    wxASSERT( SUCCEEDED(hr) );

    //MSW state is the same as ours
    //State_Stopped   = 0,
    //State_Paused    = State_Stopped + 1,
    //State_Running   = State_Paused + 1

    return (wxMediaState) theState;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::GetPlaybackRate
//
// Pretty simple way of obtaining the playback rate from
// the IMediaSeeking interface
//---------------------------------------------------------------------------
double wxAMMediaBackend::GetPlaybackRate()
{
    double dRate;
    wxAMVERIFY( m_pMS->GetRate(&dRate) );
    return dRate;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::SetPlaybackRate
//
// Sets the playback rate of the media - DirectShow is pretty good
// about this, actually
//---------------------------------------------------------------------------
bool wxAMMediaBackend::SetPlaybackRate(double dRate)
{
    return SUCCEEDED( m_pMS->SetRate(dRate) );
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::NotifyWndProc
//
// Here we check to see if DirectShow tells us we've reached the stop
// position in our stream - if it has, it may not actually stop
// the stream - which we need to do...
//---------------------------------------------------------------------------
LRESULT CALLBACK wxAMMediaBackend::NotifyWndProc(HWND hWnd, UINT nMsg,
                                                    WPARAM wParam,
                                                    LPARAM lParam)
{
    wxAMMediaBackend* backend = (wxAMMediaBackend*)
        ::GetWindowLong(hWnd, GWL_USERDATA);

    return backend->OnNotifyWndProc(hWnd, nMsg, wParam, lParam);
}

LRESULT CALLBACK wxAMMediaBackend::OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                                    WPARAM wParam,
                                                    LPARAM lParam)
{
    if (nMsg == WM_GRAPHNOTIFY)
    {
        LONG    evCode,
                evParam1,
                evParam2;

        //
        // DirectShow keeps a list of queued events, and we need
        // to go through them one by one, stopping at (Hopefully only one)
        // EC_COMPLETE message
        //
        while(SUCCEEDED(m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                                       (LONG_PTR *) &evParam2, 0)
                       )
             )
        {
            // Cleanup memory that GetEvent allocated
            wxAMVERIFY( m_pME->FreeEventParams(evCode, evParam1, evParam2) );

            // If this is the end of the clip, notify handler
            if(EC_COMPLETE == evCode)
            {
                //send the event to our child
                wxMediaEvent theEvent(wxEVT_MEDIA_STOP, m_ctrl->GetId());
                m_ctrl->ProcessEvent(theEvent);

                //if the user didn't veto it, stop the stream
                if (theEvent.IsAllowed())
                {
                    //Interestingly enough, DirectShow does not actually stop
                    //the filters - even when it reaches the end!
                    wxVERIFY( Stop() );

                    //send the event to our child
                    wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                          m_ctrl->GetId());
                    m_ctrl->ProcessEvent(theEvent);
                }
            }
        }
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Cleanup
//
// 1) Hide/disowns the video window (MS says bad things will happen if
//    you don't)
// 2) Releases all the directshow interfaces we use
// TODO: Maybe there's a way to redirect the IGraphBuilder each time
//       we load, rather then creating and destroying the interfaces
//       each time?
//---------------------------------------------------------------------------
void wxAMMediaBackend::Cleanup()
{
    // Hide then disown the window
    if(m_pVW)
    {
        m_pVW->put_Visible(OAFALSE); //OSFALSE == 0
        m_pVW->put_Owner(NULL);
    }

    // Release and zero DirectShow interfaces
    SAFE_RELEASE(m_pME);
    SAFE_RELEASE(m_pMS);
    SAFE_RELEASE(m_pMC);
    SAFE_RELEASE(m_pBA);
    SAFE_RELEASE(m_pBV);
    SAFE_RELEASE(m_pVW);
    SAFE_RELEASE(m_pGB);

    // Get rid of our hidden Window
    DestroyWindow(m_hNotifyWnd);
    m_hNotifyWnd = NULL;
}


//---------------------------------------------------------------------------
// wxAMMediaBackend::GetVideoSize
//
// Obtains the cached original video size
//---------------------------------------------------------------------------
wxSize wxAMMediaBackend::GetVideoSize() const
{
    return m_bestSize;
}

//---------------------------------------------------------------------------
// wxAMMediaBackend::Move
//
// Resizes the IVideoWindow to the size of the control window
//---------------------------------------------------------------------------
void wxAMMediaBackend::Move(int x, int y, int w, int h)
{
    if(m_hNotifyWnd && m_bVideo)
    {
        wxAMVERIFY( m_pVW->SetWindowPosition(0, 0, w, h) );
    }
}

//---------------------------------------------------------------------------
// End of wxAMMediaBackend
//---------------------------------------------------------------------------
#endif //wxUSE_DIRECTSHOW

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxMCIMediaBackend
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


IMPLEMENT_DYNAMIC_CLASS(wxMCIMediaBackend, wxMediaBackend);

//---------------------------------------------------------------------------
// Usual debugging macros for MCI returns
//---------------------------------------------------------------------------

#ifdef __WXDEBUG__
#define wxMCIVERIFY(arg) \
{ \
    DWORD nRet; \
    if ( (nRet = (arg)) != 0) \
    { \
        TCHAR sz[5000]; \
        mciGetErrorString(nRet, sz, 5000); \
        wxFAIL_MSG(wxString::Format(_T("MCI Error:%s"), sz)); \
    } \
}
#else
#define wxMCIVERIFY(arg) (arg);
#endif

//---------------------------------------------------------------------------
// Simulation for <digitalv.h>
//
// Mingw and possibly other compilers don't have the digitalv.h header
// that is needed to have some essential features of mci work with
// windows - so we provide the declarations for the types we use here
//---------------------------------------------------------------------------

typedef struct {
    DWORD_PTR   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else
    RECT    rc;
#endif
} MCI_DGV_RECT_PARMS;

typedef struct {
    DWORD_PTR   dwCallback;
    HWND    hWnd;
#ifndef _WIN32
    WORD    wReserved1;
#endif
    UINT    nCmdShow;
#ifndef _WIN32
    WORD    wReserved2;
#endif
    wxChar*   lpstrText;
} MCI_DGV_WINDOW_PARMS;

typedef struct {
    DWORD_PTR dwCallback;
    DWORD     dwTimeFormat;
    DWORD     dwAudio;
    DWORD     dwFileFormat;
    DWORD     dwSpeed;
} MCI_DGV_SET_PARMS;

//---------------------------------------------------------------------------
// wxMCIMediaBackend Constructor
//
// Here we don't need to do much except say we don't have any video :)
//---------------------------------------------------------------------------
wxMCIMediaBackend::wxMCIMediaBackend() : m_hNotifyWnd(NULL), m_bVideo(false)
{
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend Destructor
//
// We close the mci device - note that there may not be an mci device here,
// or it may fail - but we don't really care, since we're destructing
//---------------------------------------------------------------------------
wxMCIMediaBackend::~wxMCIMediaBackend()
{
    if(m_hNotifyWnd)
    {
        mciSendCommand(m_hDev, MCI_CLOSE, 0, 0);
        DestroyWindow(m_hNotifyWnd);
        m_hNotifyWnd = NULL;
    }
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Create
//
// Here we just tell wxMediaCtrl that mci does exist (which it does, on all
// msw systems, at least in some form dating back to win16 days)
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                                     wxWindowID id,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    //
    // Create window
    // By default wxWindow(s) is created with a border -
    // so we need to get rid of those, and create with
    // wxCLIP_CHILDREN, so that if the driver/backend
    // is a child window, it refereshes properly
    //
    if ( !ctrl->wxControl::Create(parent, id, pos, size,
                            (style & ~wxBORDER_MASK) | wxCLIP_CHILDREN,
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
// wxMCIMediaBackend::Load (file version)
//
// Here we have MCI load a file and device, set the time format to our
// default (milliseconds), and set the video (if any) to play in the control
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Load(const wxString& fileName)
{
    //
    //if the user already called load close the previous MCI device
    //
    if(m_hNotifyWnd)
    {
        mciSendCommand(m_hDev, MCI_CLOSE, 0, 0);
        DestroyWindow(m_hNotifyWnd);
        m_hNotifyWnd = NULL;
    }

    //
    //Opens a file and has MCI select a device.  Normally you'd put
    //MCI_OPEN_TYPE in addition to MCI_OPEN_ELEMENT - however if you
    //omit this it tells MCI to select the device instead.  This is
    //good because we have no reliable way of "enumerating" the devices
    //in MCI
    //
    MCI_OPEN_PARMS openParms;
    openParms.lpstrElementName = (wxChar*) fileName.c_str();

    if ( mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT,
                        (DWORD)(LPVOID)&openParms) != 0)
        return false;

    m_hDev = openParms.wDeviceID;

    //
    //Now set the time format for the device to milliseconds
    //
    MCI_SET_PARMS setParms;
    setParms.dwCallback = 0;
    setParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS;

    if (mciSendCommand(m_hDev, MCI_SET, MCI_SET_TIME_FORMAT,
                         (DWORD)(LPVOID)&setParms) != 0)
        return false;

    //
    //Now tell the MCI device to display the video in our wxMediaCtrl
    //
    MCI_DGV_WINDOW_PARMS windowParms;
    windowParms.hWnd = (HWND)m_ctrl->GetHandle();

    m_bVideo = (mciSendCommand(m_hDev, MCI_WINDOW,
                               0x00010000L, //MCI_DGV_WINDOW_HWND
                               (DWORD)(LPVOID)&windowParms) == 0);

    //
    // Create a hidden window and register to handle
    // MCI events
    // Note that wxCanvasClassName is already registered
    // and used by all wxWindows and normal wxControls
    //
    m_hNotifyWnd = ::CreateWindow
                    (
                        wxCanvasClassName,
                        NULL,
                        0, 0, 0, 0,
                        0,
                        (HWND) NULL,
                        (HMENU)NULL,
                        wxGetInstance(),
                        (LPVOID) NULL
                    );

    if(!m_hNotifyWnd)
    {
        wxLogSysError( wxT("Could not create hidden needed for ")
                       wxT("registering for DirectShow events!")  );

        return false;
    }

    ::SetWindowLong(m_hNotifyWnd, GWL_WNDPROC,
                       (LONG)wxMCIMediaBackend::NotifyWndProc);

    ::SetWindowLong(m_hNotifyWnd, GWL_USERDATA,
                       (LONG) this);

    //
    //Here, if the parent of the control has a sizer - we
    //tell it to recalculate the size of this control since
    //the user opened a seperate media file
    //
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();

    return true;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Load (URL version)
//
// MCI doesn't support URLs directly (?)
//
// TODO:  Use wxURL/wxFileSystem and mmioInstallProc
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Load(const wxURI& WXUNUSED(location))
{
    return false;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Play
//
// Plays/Resumes the MCI device... a couple notes:
// 1) Certain drivers will crash and burn if we don't pass them an
//    MCI_PLAY_PARMS, despite the documentation that says otherwise...
// 2) There is a MCI_RESUME command, but MCI_PLAY does the same thing
//    and will resume from a stopped state also, so there's no need to
//    call both, for example
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Play()
{
    MCI_PLAY_PARMS playParms;
    playParms.dwCallback = (DWORD)m_hNotifyWnd;

    return ( mciSendCommand(m_hDev, MCI_PLAY, MCI_NOTIFY,
                            (DWORD)(LPVOID)&playParms) == 0 );
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Pause
//
// Pauses the MCI device - nothing special
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Pause()
{
    return (mciSendCommand(m_hDev, MCI_PAUSE, MCI_WAIT, 0) == 0);
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Stop
//
// Stops the MCI device & seeks to the beginning as wxMediaCtrl docs outline
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::Stop()
{
    return (mciSendCommand(m_hDev, MCI_STOP, MCI_WAIT, 0) == 0) &&
           (mciSendCommand(m_hDev, MCI_SEEK, MCI_SEEK_TO_START, 0) == 0);
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetState
//
// Here we get the state and convert it to a wxMediaState -
// since we use direct comparisons with MCI_MODE_PLAY and
// MCI_MODE_PAUSE, we don't care if the MCI_STATUS call
// fails or not
//---------------------------------------------------------------------------
wxMediaState wxMCIMediaBackend::GetState()
{
    MCI_STATUS_PARMS statusParms;
    statusParms.dwItem = MCI_STATUS_MODE;

    mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM,
                         (DWORD)(LPVOID)&statusParms);

    if(statusParms.dwReturn == MCI_MODE_PAUSE)
        return wxMEDIASTATE_PAUSED;
    else if(statusParms.dwReturn == MCI_MODE_PLAY)
        return wxMEDIASTATE_PLAYING;
    else
        return wxMEDIASTATE_STOPPED;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::SetPosition
//
// Here we set the position of the device in the stream.
// Note that MCI actually stops the device after you seek it if the
// device is playing/paused, so we need to play the file after
// MCI seeks like normal APIs would
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::SetPosition(wxLongLong where)
{
    MCI_SEEK_PARMS seekParms;
    seekParms.dwCallback = 0;
    seekParms.dwTo = (DWORD)where.GetValue();

    //device was playing?
    bool bReplay = GetState() == wxMEDIASTATE_PLAYING;

    if( mciSendCommand(m_hDev, MCI_SEEK, MCI_TO,
                       (DWORD)(LPVOID)&seekParms) != 0)
        return false;

    //If the device was playing, resume it
    if (bReplay)
        return Play();
    else
        return true;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetPosition
//
// Gets the position of the device in the stream using the current
// time format... nothing special here...
//---------------------------------------------------------------------------
wxLongLong wxMCIMediaBackend::GetPosition()
{
    MCI_STATUS_PARMS statusParms;
    statusParms.dwItem = MCI_STATUS_POSITION;

    if (mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM,
                       (DWORD)(LPSTR)&statusParms) != 0)
        return 0;

    return statusParms.dwReturn;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetDuration
//
// Gets the duration of the stream... nothing special
//---------------------------------------------------------------------------
wxLongLong wxMCIMediaBackend::GetDuration()
{
    MCI_STATUS_PARMS statusParms;
    statusParms.dwItem = MCI_STATUS_LENGTH;

    if (mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM,
                        (DWORD)(LPSTR)&statusParms) != 0)
        return 0;

    return statusParms.dwReturn;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::Move
//
// Moves the window to a location
//---------------------------------------------------------------------------
void wxMCIMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y),
                                       int w,           int h)
{
    if (m_hNotifyWnd && m_bVideo)
    {
        MCI_DGV_RECT_PARMS putParms; //ifdefed MCI_DGV_PUT_PARMS
        putParms.rc.top = 0;
        putParms.rc.bottom = 0;
        putParms.rc.right = w;
        putParms.rc.bottom = h;

        wxMCIVERIFY( mciSendCommand(m_hDev, MCI_PUT,
                                   0x00040000L, //MCI_DGV_PUT_DESTINATION
                                   (DWORD)(LPSTR)&putParms) );
    }
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetVideoSize
//
// Gets the original size of the movie for sizers
//---------------------------------------------------------------------------
wxSize wxMCIMediaBackend::GetVideoSize() const
{
    if(m_bVideo)
    {
        MCI_DGV_RECT_PARMS whereParms; //ifdefed MCI_DGV_WHERE_PARMS

        wxMCIVERIFY( mciSendCommand(m_hDev, MCI_WHERE,
                       0x00020000L, //MCI_DGV_WHERE_SOURCE
                       (DWORD)(LPSTR)&whereParms) );

        return wxSize(whereParms.rc.right, whereParms.rc.bottom);
    }
    return wxSize(0,0);
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::GetPlaybackRate
//
// TODO
//---------------------------------------------------------------------------
double wxMCIMediaBackend::GetPlaybackRate()
{
    return 1.0;
}

//---------------------------------------------------------------------------
// wxMCIMediaBackend::SetPlaybackRate
//
// TODO
//---------------------------------------------------------------------------
bool wxMCIMediaBackend::SetPlaybackRate(double WXUNUSED(dRate))
{
/*
    MCI_WAVE_SET_SAMPLESPERSEC
    MCI_DGV_SET_PARMS setParms;
    setParms.dwSpeed = (DWORD) (dRate * 1000.0);

    return (mciSendCommand(m_hDev, MCI_SET,
                       0x00020000L, //MCI_DGV_SET_SPEED
                       (DWORD)(LPSTR)&setParms) == 0);
*/
    return false;
}

//---------------------------------------------------------------------------
// [static] wxMCIMediaBackend::MSWWindowProc
//
// Here we process a message when MCI reaches the stopping point
// in the stream
//---------------------------------------------------------------------------
LRESULT CALLBACK wxMCIMediaBackend::NotifyWndProc(HWND hWnd, UINT nMsg,
                                                  WPARAM wParam,
                                                  LPARAM lParam)
{
    wxMCIMediaBackend* backend = (wxMCIMediaBackend*)
        ::GetWindowLong(hWnd, GWL_USERDATA);
    wxASSERT(backend);

    return backend->OnNotifyWndProc(hWnd, nMsg, wParam, lParam);
}

LRESULT CALLBACK wxMCIMediaBackend::OnNotifyWndProc(HWND hWnd, UINT nMsg,
                                                  WPARAM wParam,
                                                  LPARAM lParam)
{
    if(nMsg == MM_MCINOTIFY)
    {
        wxASSERT(lParam == (LPARAM) m_hDev);
        if(wParam == MCI_NOTIFY_SUCCESSFUL && lParam == (LPARAM)m_hDev)
        {
            wxMediaEvent theEvent(wxEVT_MEDIA_STOP, m_ctrl->GetId());
            m_ctrl->ProcessEvent(theEvent);

            if(theEvent.IsAllowed())
            {
                wxMCIVERIFY( mciSendCommand(m_hDev, MCI_SEEK,
                                            MCI_SEEK_TO_START, 0) );

                //send the event to our child
                wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED,
                                      m_ctrl->GetId());
                m_ctrl->ProcessEvent(theEvent);
            }
        }
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxQTMediaBackend
//
// TODO: Use a less cludgy way to pause/get state/set state
// TODO: Dynamically load from qtml.dll
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if wxUSE_QUICKTIME

IMPLEMENT_DYNAMIC_CLASS(wxQTMediaBackend, wxMediaBackend);

//Time between timer calls
#define MOVIE_DELAY 100

#include "wx/timer.h"

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
// wxQTMediaBackend Destructor
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

    //Note that ExitMovies() is not neccessary, but
    //the docs are fuzzy on whether or not TerminateQTML is
    ExitMovies();
    TerminateQTML();
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
    int nError;
    if ((nError = InitializeQTML(0)) != noErr)    //-2093 no dll
    {
        wxFAIL_MSG(wxString::Format(wxT("Couldn't Initialize Quicktime-%i"), nError));
        return false;
    }
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
                            (style & ~wxBORDER_MASK),
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

    if (NativePathNameToFSSpec ((char*) (const char*) fileName.mb_str(),
                                &sfFile, 0) != noErr)
        return false;

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
// wxQTMediaBackend::Move
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
// wxQTMediaBackend::Move
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
        CreatePortAssociation(m_ctrl->GetHWND(), NULL, 0L);

        SetMovieGWorld(m_movie,
                       (CGrafPtr) GetNativeWindowPort(m_ctrl->GetHWND()),
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
// wxQTMediaBackend::Move
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
// wxQTMediaBackend::Move
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
// wxQTMediaBackend::Move
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
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
double wxQTMediaBackend::GetPlaybackRate()
{
    return ( ((double)::GetMovieRate(m_movie)) / 0x10000);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
bool wxQTMediaBackend::SetPlaybackRate(double dRate)
{
    ::SetMovieRate(m_movie, (Fixed) (dRate * 0x10000));
    return ::GetMoviesError() == noErr;
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
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
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetPosition()
{
    return ::GetMovieTime(m_movie, NULL);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
//
// TODO
//---------------------------------------------------------------------------
wxLongLong wxQTMediaBackend::GetDuration()
{
    return ::GetMovieDuration(m_movie);
}

//---------------------------------------------------------------------------
// wxQTMediaBackend::Move
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
// wxQTMediaBackend::Move
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
// wxQTMediaBackend::Move
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
        Rect theRect = {0, 0, h, w};

        ::SetMovieBox(m_movie, &theRect);
        wxASSERT(::GetMoviesError() == noErr);
    }
}

//---------------------------------------------------------------------------
//  End QT Compilation Guard
//---------------------------------------------------------------------------
#endif //wxUSE_QUICKTIME

//in source file that contains stuff you don't directly use
#include <wx/html/forcelnk.h>
FORCE_LINK_ME(basewxmediabackends);

//---------------------------------------------------------------------------
//  End wxMediaCtrl Compilation Guard and this file
//---------------------------------------------------------------------------
#endif //wxUSE_MEDIACTRL


