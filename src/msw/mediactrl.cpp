/////////////////////////////////////////////////////////////////////////////
// Name:        msw/mediactrl.cpp
// Purpose:     wxMediaCtrl MSW
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by: 
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//      Pre-wx includes
//---------------------------------------------------------------------------

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "moviectrl.h"
//#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/mediactrl.h"

#if wxUSE_MEDIACTRL

//###########################################################################
//  DECLARATIONS
//###########################################################################

IMPLEMENT_CLASS(wxMediaCtrl, wxControl);
IMPLEMENT_DYNAMIC_CLASS(wxMediaEvent, wxEvent); 
DEFINE_EVENT_TYPE(wxEVT_MEDIA_FINISHED); 

//---------------------------------------------------------------------------
//  wxMediaCtrlImpl
//---------------------------------------------------------------------------

class wxMediaCtrlImpl
{
public:
    wxMediaCtrlImpl() : m_bLoaded(false)
    {                                                                   }

    virtual ~wxMediaCtrlImpl() 
    {                               }

    virtual bool Create(wxMediaCtrl* WXUNUSED(ctrl)) 
    {   return false;               }

    virtual bool Play() { return false; }
    virtual bool Pause() { return false; }
    virtual bool Stop() { return false; }

    virtual bool Load(const wxString&) { return false; }
    virtual bool Load(const wxURI&) { return false; }

    virtual wxMediaState GetState() { return wxMEDIASTATE_STOPPED; }

    virtual bool SetPosition(long) { return 0; }
    virtual long GetPosition() { return 0; }
    virtual long GetDuration() { return 0; }

    virtual void DoMoveWindow(int, int, int, int) {  }
    virtual wxSize DoGetBestSize() const { return wxSize(0,0); }

    virtual double GetPlaybackRate() { return 0; }
    virtual bool SetPlaybackRate(double) { return false; }
    
    virtual bool MSWWindowProc(WXUINT, WXWPARAM, WXLPARAM) { return false; }

    bool IsLoaded()
    {   return m_bLoaded;   }

    bool m_bLoaded;
};

//---------------------------------------------------------------------------
//  wxDXMediaCtrlImpl
//---------------------------------------------------------------------------

#if wxUSE_DIRECTSHOW

#include <dshow.h>

#define WM_GRAPHNOTIFY  WM_USER+13

#ifdef __WXDEBUG__
#define wxDSVERIFY(x) wxASSERT( SUCCEEDED ((x)) )
#else
#define wxDSVERIFY(x) (x)
#endif

class wxDXMediaCtrlImpl : public wxMediaCtrlImpl
{
public:
    wxDXMediaCtrlImpl();

    virtual ~wxDXMediaCtrlImpl();

    virtual bool Create(wxMediaCtrl* ctrl);

    virtual bool Play();
    virtual bool Pause();
    virtual bool Stop();

    virtual bool Load(const wxString& fileName);
    virtual bool Load(const wxURI& location);

    virtual wxMediaState GetState();

    virtual bool SetPosition(long where);
    virtual long GetPosition();
    virtual long GetDuration();

    virtual void DoMoveWindow(int x, int y, int w, int h);
    wxSize DoGetBestSize() const;

    virtual double GetPlaybackRate();
    virtual bool SetPlaybackRate(double);

    void Cleanup();

    bool m_bVideo;

    bool MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    wxMediaCtrl* m_ctrl;

    IGraphBuilder* m_pGB;
    IMediaControl* m_pMC;
    IMediaEventEx* m_pME;
    IVideoWindow* m_pVW;
    IBasicAudio* m_pBA;
    IBasicVideo* m_pBV;
    IMediaSeeking* m_pMS;

    wxSize m_bestSize;
};

#endif //wxUSE_DIRECTSHOW

//---------------------------------------------------------------------------
//  wxWMMEMediaCtrlImpl
//---------------------------------------------------------------------------

#include <mmsystem.h>

class wxWMMEMediaCtrlImpl : public wxMediaCtrlImpl
{
public:

    wxWMMEMediaCtrlImpl();
    ~wxWMMEMediaCtrlImpl();

    virtual bool Create(wxMediaCtrl* ctrl);

    virtual bool Play();
    virtual bool Pause();
    virtual bool Stop();

    virtual bool Load(const wxString& fileName);
    virtual bool Load(const wxURI& location);

    virtual wxMediaState GetState();

    virtual bool SetPosition(long where);
    virtual long GetPosition();
    virtual long GetDuration();

    virtual void DoMoveWindow(int x, int y, int w, int h);
    wxSize DoGetBestSize() const;

    virtual double GetPlaybackRate();
    virtual bool SetPlaybackRate(double);

    bool MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    MCIDEVICEID m_hDev;
    wxMediaCtrl* m_ctrl;
    bool m_bVideo;
};

//###########################################################################
//
//  IMPLEMENTATION
//
//###########################################################################

//---------------------------------------------------------------------------
//
//  wxMediaCtrl
//
//---------------------------------------------------------------------------

bool wxMediaCtrl::Create(wxWindow* parent, wxWindowID id, const wxString& fileName, 
                         const wxPoint& pos, const wxSize& size, 
                         long style, long WXUNUSED(driver), const wxString& name)
{
    //base create
    if ( !wxControl::Create(parent, id, pos, size, (style | wxNO_BORDER) | wxCLIP_CHILDREN, 
                            wxDefaultValidator, name) )
        return false;

    //Set our background color to black by default
    SetBackgroundColour(*wxBLACK);

#if wxUSE_DIRECTSHOW
    m_imp = new wxDXMediaCtrlImpl;
    if(!m_imp->Create(this))
    {
        delete m_imp;
#endif
        m_imp = new wxWMMEMediaCtrlImpl;
        if(!m_imp->Create(this))
        {
            delete m_imp;
            m_imp = NULL;
            return false;
        }
#if wxUSE_DIRECTSHOW
    }
#endif

    if(!fileName.empty())
    {
        if (!Load(fileName))
            return false;
    }

    return true;
}

bool wxMediaCtrl::Create(wxWindow* parent, wxWindowID id, const wxURI& location, 
                         const wxPoint& pos, const wxSize& size, 
                         long style, long WXUNUSED(driver), const wxString& name)
{
    //base create
    if ( !wxControl::Create(parent, id, pos, size, (style | wxNO_BORDER) | wxCLIP_CHILDREN, 
                            wxDefaultValidator, name) )
        return false;

    //Set our background color to black by default
    SetBackgroundColour(*wxBLACK);

#if wxUSE_DIRECTSHOW
    m_imp = new wxDXMediaCtrlImpl;
    if(!m_imp->Create(this))
    {
        delete m_imp;
#endif
        m_imp = new wxWMMEMediaCtrlImpl;
        if(!m_imp->Create(this))
        {
            delete m_imp;
            m_imp = NULL;
            return false;
        }
#if wxUSE_DIRECTSHOW
    }
#endif

    if (!Load(location))
        return false;

    return true;
}

bool wxMediaCtrl::Load(const wxString& fileName)
{
    if(m_imp)
        return m_imp->Load(fileName);
    return false;
}

bool wxMediaCtrl::Load(const wxURI& location)
{
    if(m_imp)
        return m_imp->Load(location);
    return false;
}

bool wxMediaCtrl::Play()
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->Play();
    return false;
}

bool wxMediaCtrl::Pause()
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->Pause();
    return false;
}

bool wxMediaCtrl::Stop()
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->Stop();
    return false;
}

double wxMediaCtrl::GetPlaybackRate()
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->GetPlaybackRate();
    return 0;
}

bool wxMediaCtrl::SetPlaybackRate(double dRate)
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->SetPlaybackRate(dRate);
    return false;
}

bool wxMediaCtrl::SetPosition(long where)
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->SetPosition(where);
    return false;
}

long wxMediaCtrl::GetPosition()
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->GetPosition();
    return 0;
}

long wxMediaCtrl::GetDuration()
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->GetDuration();
    return 0;
}

wxMediaState wxMediaCtrl::GetState()
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->GetState();
    return wxMEDIASTATE_STOPPED;
}

WXLRESULT wxMediaCtrl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if(m_imp && m_imp->IsLoaded() && m_imp->MSWWindowProc(nMsg, wParam, lParam) )
           return wxControl::MSWDefWindowProc(nMsg, wParam, lParam);
    //pass the event to our parent
    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

wxSize wxMediaCtrl::DoGetBestSize() const
{
    if(m_imp && m_imp->IsLoaded())
        return m_imp->DoGetBestSize();
    return wxSize(0,0);
}

void wxMediaCtrl::DoMoveWindow(int x, int y, int w, int h)
{
    wxControl::DoMoveWindow(x,y,w,h);

    if(m_imp && m_imp->IsLoaded())
        m_imp->DoMoveWindow(x, y, w, h);
}

wxMediaCtrl::~wxMediaCtrl()
{
    if (m_imp)
        delete m_imp;
}


//---------------------------------------------------------------------------
//
//  wxDXMediaCtrlImpl
//
//---------------------------------------------------------------------------

#if wxUSE_DIRECTSHOW 

wxDXMediaCtrlImpl::wxDXMediaCtrlImpl() : m_pGB(NULL)
{
}

bool wxDXMediaCtrlImpl::Create(wxMediaCtrl* ctrl)
{
    //create our filter graph
   HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                      IID_IGraphBuilder, (void**)&m_pGB);

   //directshow not installed?
    if ( FAILED(hr) )
        return false;

    m_ctrl = ctrl;

    return true;
}

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

bool wxDXMediaCtrlImpl::Load(const wxString& fileName)
{
    if(m_bLoaded)
        Cleanup();                        
        
    SAFE_RELEASE(m_pGB);

    CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                      IID_IGraphBuilder, (void**)&m_pGB);

    //load the graph & render 
    if( FAILED(m_pGB->RenderFile(fileName.wc_str(wxConvLocal), NULL)) )
        return false;

    //get the interfaces, all of them
    wxDSVERIFY( m_pGB->QueryInterface(IID_IMediaControl, (void**)&m_pMC) );
    wxDSVERIFY( m_pGB->QueryInterface(IID_IMediaEventEx, (void**)&m_pME) );
    wxDSVERIFY( m_pGB->QueryInterface(IID_IMediaSeeking, (void**)&m_pMS) );
    wxDSVERIFY( m_pGB->QueryInterface(IID_IVideoWindow, (void**)&m_pVW) );
    wxDSVERIFY( m_pGB->QueryInterface(IID_IBasicAudio, (void**)&m_pBA) );
    wxDSVERIFY( m_pGB->QueryInterface(IID_IBasicVideo, (void**)&m_pBV) );

    //long lVolume;
    //pBA->get_Volume(&lVolume);
    //E_NOTIMPL

    //get the _actual_ size of the movie & remember it
    long nX, nY, nSX, nSY;
    if (FAILED(m_pVW->GetWindowPosition(&nX,&nY,&nSX,&nSY)))
    {
        m_bVideo = false;

        nSX = nSY = 0;
    }
    else 
    {
        m_bVideo = true;
    }

    m_bestSize.x = nSX;
    m_bestSize.y = nSY;

    if (m_bVideo)
    {
        wxDSVERIFY( m_pVW->put_Owner((OAHWND)m_ctrl->GetHandle()) );
        wxDSVERIFY( m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS) );
        wxDSVERIFY( m_pVW->put_Visible(OATRUE) ); //OATRUE == -1
    }

    //make it so that wxEVT_MOVIE_FINISHED works
    wxDSVERIFY( m_pME->SetNotifyWindow((OAHWND)m_ctrl->GetHandle(), WM_GRAPHNOTIFY, 0) );

    //set the time format
    wxDSVERIFY( m_pMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME) );

    //so that DoGetBestSize will work :)
    m_bLoaded = true;

    //work around refresh issues
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();

    return true;
}

bool wxDXMediaCtrlImpl::Load(const wxURI& location)
{
    return Load(location.BuildUnescapedURI());
}

bool wxDXMediaCtrlImpl::Play()
{
    return SUCCEEDED( m_pMC->Run() );
}

bool wxDXMediaCtrlImpl::Pause()
{
    return SUCCEEDED( m_pMC->Pause() );
}

bool wxDXMediaCtrlImpl::Stop()
{
    bool bOK = SUCCEEDED( m_pMC->Stop() );

    //We don't care if it can't get to the beginning in directshow -
    //it could be a non-seeking filter (wince midi) in which case playing
    //starts all over again
    SetPosition(0);
    return bOK;
}

bool wxDXMediaCtrlImpl::SetPosition(long where)
{
    //DS uses 100 nanos - so we need a 10 mult
    LONGLONG pos = ((LONGLONG)where) * 10000;

    return SUCCEEDED( m_pMS->SetPositions(
                                &pos, 
                                AM_SEEKING_AbsolutePositioning,
                                NULL, 
                                AM_SEEKING_NoPositioning
                                    ) );
}

long wxDXMediaCtrlImpl::GetPosition()
{
    LONGLONG outCur, outStop;
    wxDSVERIFY( m_pMS->GetPositions(&outCur, &outStop) );

    //h,m,s,milli - outdur is in 100 nanos
    return outCur/10000;
}

long wxDXMediaCtrlImpl::GetDuration()
{
    LONGLONG outDuration;
    wxDSVERIFY( m_pMS->GetDuration(&outDuration) );

    //h,m,s,milli - outdur is in 100 nanos
    return outDuration/10000;
}

wxMediaState wxDXMediaCtrlImpl::GetState()
{
    //TODO: MS recommends against INFINITE here - do it in stages
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

double wxDXMediaCtrlImpl::GetPlaybackRate()
{
    double dRate;
    wxDSVERIFY( m_pMS->GetRate(&dRate) );
    return dRate;
}

bool wxDXMediaCtrlImpl::SetPlaybackRate(double dRate)
{
    return SUCCEEDED( m_pMS->SetRate(dRate) );
}

bool wxDXMediaCtrlImpl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if (nMsg == WM_GRAPHNOTIFY)
    {
        LONG evCode, evParam1, evParam2;
        HRESULT hr=S_OK;

        // Process all queued events
        while(SUCCEEDED(m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                                       (LONG_PTR *) &evParam2, 0) 
                       ) 
             )
        {
            // Free memory associated with callback, since we're not using it
            hr = m_pME->FreeEventParams(evCode, evParam1, evParam2);

            // If this is the end of the clip, notify handler
            if(EC_COMPLETE == evCode)
            {
                //Interestingly enough, DirectShow does not actually stop
                //the filters - even when it reaches the end!
#ifdef __WXDEBUG__
                wxASSERT( Stop() );
#else
                Stop();
#endif

                wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED, m_ctrl->GetId());
                m_ctrl->GetParent()->ProcessEvent(theEvent);
            }
        }    
        return true;
    }
    return false;
}

void wxDXMediaCtrlImpl::Cleanup()
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
}

wxDXMediaCtrlImpl::~wxDXMediaCtrlImpl()
{
    if (m_bLoaded)
        Cleanup();

    SAFE_RELEASE(m_pGB);
}

wxSize wxDXMediaCtrlImpl::DoGetBestSize() const
{
    return m_bestSize;
}

void wxDXMediaCtrlImpl::DoMoveWindow(int x, int y, int w, int h)
{
    if(m_bLoaded && m_bVideo)
    {
        wxDSVERIFY( m_pVW->SetWindowPosition(0, 0, w, h) );
    }
}

#endif //wxUSE_DIRECTSHOW

//---------------------------------------------------------------------------
//
//  wxWMMEMediaCtrlImpl
//
//---------------------------------------------------------------------------

//
// Cruft to simulate digitalv.h
//

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
    LPSTR   lpstrText;
} MCI_DGV_WINDOW_PARMSA;

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
    LPWSTR  lpstrText;
} MCI_DGV_WINDOW_PARMSW;
#ifdef UNICODE
typedef MCI_DGV_WINDOW_PARMSW MCI_DGV_WINDOW_PARMS;
#else
typedef MCI_DGV_WINDOW_PARMSA MCI_DGV_WINDOW_PARMS;
#endif // UNICODE

wxWMMEMediaCtrlImpl::wxWMMEMediaCtrlImpl() : m_bVideo(false)
{
/*        TCHAR sz[5000];
	 mciGetErrorString(nError, sz, 5000);
            wxMessageBox(wxString::Format(_T("Error:%s"), sz));
  */          
}

wxWMMEMediaCtrlImpl::~wxWMMEMediaCtrlImpl()
{
    mciSendCommand(m_hDev, MCI_CLOSE, 0, 0);
}

bool wxWMMEMediaCtrlImpl::Create(wxMediaCtrl* ctrl)
{    
    m_ctrl = ctrl;
    return true;
}

bool wxWMMEMediaCtrlImpl::Load(const wxString& fileName)
{
    if(m_bLoaded)
        mciSendCommand(m_hDev, MCI_CLOSE, 0, 0);

    MCI_OPEN_PARMS openParms;
    MCI_SET_PARMS setParms;

    openParms.lpstrElementName = (wxChar*) fileName.c_str();

    //Here's where the trick lies - if you don't specify MCI_OPEN_TYPE,
    //then it actually automagically finds the device for you!
    if ( mciSendCommand(0, MCI_OPEN,
         MCI_OPEN_ELEMENT,
        (DWORD)(LPVOID)&openParms) != 0)  
    return false;

    m_hDev = openParms.wDeviceID;

    setParms.dwCallback = 0;
    setParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS;

    if (mciSendCommand(m_hDev, MCI_SET, MCI_SET_TIME_FORMAT,
                         (DWORD)(LPVOID)&setParms) != 0)
        return false;

    MCI_DGV_WINDOW_PARMS windowParms;

    windowParms.hWnd = (HWND)m_ctrl->GetHandle();
    m_bVideo = (mciSendCommand(m_hDev, MCI_WINDOW,
                         0x00010000L //MCI_DGV_WINDOW_HWND
                         , 
                         (DWORD)(LPVOID)&windowParms) == 0);
    m_bLoaded = true;

    //work around refresh issues
    m_ctrl->InvalidateBestSize();
    m_ctrl->GetParent()->Layout();
    m_ctrl->GetParent()->Refresh();
    m_ctrl->GetParent()->Update();
    
    return true;
}

bool wxWMMEMediaCtrlImpl::Load(const wxURI& WXUNUSED(location))
{
    return false;
}

bool wxWMMEMediaCtrlImpl::Play()
{
    //the directshow driver ("mpegvideo") will crash if we don't do a playParms here
    MCI_PLAY_PARMS playParms;
    playParms.dwCallback = (WORD)(HWND)m_ctrl->GetHWND();
    bool bOK = mciSendCommand(m_hDev, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&playParms) == 0;
    return (bOK) ;/*||
           (mciSendCommand(m_hDev, MCI_RESUME, 0, 0) == 0);*/
}

bool wxWMMEMediaCtrlImpl::Pause()
{
    return (mciSendCommand(m_hDev, MCI_PAUSE, MCI_WAIT, 0) == 0);
}

bool wxWMMEMediaCtrlImpl::Stop()
{
    return (mciSendCommand(m_hDev, MCI_STOP, MCI_WAIT, 0) == 0) &&
           (mciSendCommand(m_hDev, MCI_SEEK, MCI_SEEK_TO_START, 0) == 0);
}


wxMediaState wxWMMEMediaCtrlImpl::GetState()
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

bool wxWMMEMediaCtrlImpl::SetPosition(long where)
{
    MCI_SEEK_PARMS seekParms;
    seekParms.dwCallback = 0;
    seekParms.dwTo = where;

    bool bReplay = GetState() == wxMEDIASTATE_PLAYING;

    if( mciSendCommand(m_hDev, MCI_SEEK, MCI_TO, (DWORD)(LPVOID)&seekParms) != 0)
        return false;

    if (bReplay)
        return Play();
    else
        return true;
}

long wxWMMEMediaCtrlImpl::GetPosition()
{
    MCI_STATUS_PARMS statusParms;
    
    statusParms.dwItem = MCI_STATUS_POSITION; 
    if (mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM, 
        (DWORD)(LPSTR)&statusParms) != 0)
        return 0;

    return statusParms.dwReturn;
}

long wxWMMEMediaCtrlImpl::GetDuration()
{
    MCI_STATUS_PARMS statusParms;
    
    statusParms.dwItem = MCI_STATUS_LENGTH; 
    if (mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM, 
        (DWORD)(LPSTR)&statusParms) != 0)
        return 0;

    return statusParms.dwReturn;
}

void wxWMMEMediaCtrlImpl::DoMoveWindow(int, int, int, int)
{
}

wxSize wxWMMEMediaCtrlImpl::DoGetBestSize() const
{
    if(m_bVideo)
    {
        MCI_DGV_RECT_PARMS rect; 

        mciSendCommand(m_hDev, MCI_WHERE, 0x00020000L//MCI_DGV_WHERE_SOURCE
            , 
            (DWORD)(LPSTR)&rect);
        return wxSize(rect.rc.right, rect.rc.bottom);
    }
    return wxSize(0,0);
}

double wxWMMEMediaCtrlImpl::GetPlaybackRate()
{
    return 1.0;
}

bool wxWMMEMediaCtrlImpl::SetPlaybackRate(double)
{
    return false;
}

bool wxWMMEMediaCtrlImpl::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if(nMsg == MM_MCINOTIFY)
    {
        wxASSERT(lParam == (WXLPARAM) m_hDev);
        if(wParam == (WXWPARAM) MCI_NOTIFY_SUCCESSFUL && lParam == (WXLPARAM) m_hDev)
        {
#ifdef __WXDEBUG__
            wxASSERT(mciSendCommand(m_hDev, MCI_SEEK, MCI_SEEK_TO_START, 0) == 0);
#else
            mciSendCommand(m_hDev, MCI_SEEK, MCI_SEEK_TO_START, 0);
#endif

            wxMediaEvent theEvent(wxEVT_MEDIA_FINISHED, m_ctrl->GetId());
            m_ctrl->GetParent()->ProcessEvent(theEvent);
        }
        return true;
    }
    return false;
}

#endif //wxUSE_MEDIACTRL