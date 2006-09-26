/////////////////////////////////////////////////////////////////////////////
// Name:        media.i
// Purpose:     SWIG definitions for the wxMediaCtrl
//
// Author:      Robin Dunn
//
// Created:     23-Nov-2004
// RCS-ID:      $Id$
// Copyright:   (c) 2004 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"Classes for a media player control"
%enddef

%module(package="wx", docstring=DOCSTRING) media


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"

#include <wx/mediactrl.h>
#include <wx/uri.h>
%}

//----------------------------------------------------------------------

%import core.i
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

//---------------------------------------------------------------------------

%typemap(in) wxLongLong {
    $1 = PyLong_AsLongLong($input);
}


%typemap(out) wxLongLong {
    $result = PyLong_FromLongLong($1.GetValue());
}

//---------------------------------------------------------------------------
// Make a stubbed out class for platforms that don't have wxMediaCtrl
%{
#if !wxUSE_MEDIACTRL
enum wxMediaState
{
    wxMEDIASTATE_STOPPED=0,
    wxMEDIASTATE_PAUSED=0,
    wxMEDIASTATE_PLAYING=0
};

enum wxMediaCtrlPlayerControls
{
    wxMEDIACTRLPLAYERCONTROLS_NONE,
    wxMEDIACTRLPLAYERCONTROLS_STEP,
    wxMEDIACTRLPLAYERCONTROLS_VOLUME,
    wxMEDIACTRLPLAYERCONTROLS_DEFAULT
};

static wxString wxMEDIABACKEND_DIRECTSHOW(wxEmptyString);
static wxString wxMEDIABACKEND_MCI       (wxEmptyString);
static wxString wxMEDIABACKEND_QUICKTIME (wxEmptyString);
static wxString wxMEDIABACKEND_GSTREAMER (wxEmptyString);
static wxString wxMEDIABACKEND_REALPLAYER(wxEmptyString);
static wxString wxMEDIABACKEND_WMP10     (wxEmptyString);


class wxMediaEvent : public wxNotifyEvent
{
public:
    wxMediaEvent(wxEventType, int )    { wxPyRaiseNotImplemented(); }
};

class wxMediaCtrl : public wxControl
{
public:
    wxMediaCtrl()    { wxPyRaiseNotImplemented(); }

    wxMediaCtrl(wxWindow* , wxWindowID ,
                const wxString& ,
                const wxPoint& ,
                const wxSize& ,
                long style ,
                const wxString& ,
                const wxValidator& ,
                const wxString& ) { wxPyRaiseNotImplemented(); }

    bool Create(wxWindow* , wxWindowID ,
                const wxString& ,
                const wxPoint& ,
                const wxSize& ,
                long style ,
                const wxString& ,
                const wxValidator& ,
                const wxString& ) { return false; }

    bool Play() { return false; }
    bool Pause() { return false; }
    bool Stop() { return false; }

    wxMediaState GetState() { return wxMEDIASTATE_STOPPED; }

    double GetPlaybackRate()  { return 0.0; }
    bool SetPlaybackRate(double dRate) { return false; }

    wxFileOffset Seek(wxFileOffset where, wxSeekMode mode = wxFromStart)
    { return 0; }
    
    wxFileOffset Tell()    { return 0; }
    wxFileOffset Length()    { return 0; }

    double GetVolume() { return 0.0; }
    bool   SetVolume(double dVolume) { return false; }

    bool    ShowPlayerControls(
        wxMediaCtrlPlayerControls flags = wxMEDIACTRLPLAYERCONTROLS_DEFAULT)
        { return false; }

    bool Load(const wxString& fileName) { return false; }
    bool LoadURI(const wxString& fileName) { return false; }
    bool LoadURIWithProxy(const wxString& fileName, const wxString& proxy) { return false; }

    wxFileOffset GetDownloadProgress() { return 0; }
    wxFileOffset GetDownloadTotal()    { return 0; }
    
};

const wxEventType wxEVT_MEDIA_FINISHED = 0;
const wxEventType wxEVT_MEDIA_STOP = 0;
const wxEventType wxEVT_MEDIA_LOADED = 0;
const wxEventType wxEVT_MEDIA_STATECHANGED = 0;
const wxEventType wxEVT_MEDIA_PLAY = 0;
const wxEventType wxEVT_MEDIA_PAUSE = 0;

#endif
%}



//---------------------------------------------------------------------------

enum wxMediaState
{
    wxMEDIASTATE_STOPPED,
    wxMEDIASTATE_PAUSED,
    wxMEDIASTATE_PLAYING
};


enum wxMediaCtrlPlayerControls
{
    wxMEDIACTRLPLAYERCONTROLS_NONE,
    wxMEDIACTRLPLAYERCONTROLS_STEP,
    wxMEDIACTRLPLAYERCONTROLS_VOLUME,
    wxMEDIACTRLPLAYERCONTROLS_DEFAULT
};


MAKE_CONST_WXSTRING(MEDIABACKEND_DIRECTSHOW);
MAKE_CONST_WXSTRING(MEDIABACKEND_MCI       );
MAKE_CONST_WXSTRING(MEDIABACKEND_QUICKTIME );
MAKE_CONST_WXSTRING(MEDIABACKEND_GSTREAMER );
MAKE_CONST_WXSTRING(MEDIABACKEND_REALPLAYER);
MAKE_CONST_WXSTRING(MEDIABACKEND_WMP10);

//---------------------------------------------------------------------------

class wxMediaEvent : public wxNotifyEvent
{
public:
    wxMediaEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
};


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING_NOSWIG(EmptyString);
MAKE_CONST_WXSTRING2(MediaCtrlNameStr, wxT("mediaCtrl"));
MustHaveApp(wxMediaCtrl);


class wxMediaCtrl : public wxControl
{
public:
    %pythonAppend wxMediaCtrl      "self._setOORInfo(self)"
    %pythonAppend wxMediaCtrl()    ""


    wxMediaCtrl(wxWindow* parent,
                wxWindowID id=-1,
                const wxString& fileName = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxPyEmptyString,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyMediaCtrlNameStr);
    
    %RenameCtor(PreMediaCtrl,  wxMediaCtrl());

    bool Create(wxWindow* parent,
                wxWindowID id=-1,
                const wxString& fileName = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& szBackend = wxPyEmptyString,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyMediaCtrlNameStr);


    bool Play();
    bool Pause();
    bool Stop();

    wxMediaState GetState();

    double GetPlaybackRate();
    bool SetPlaybackRate(double dRate);

    wxFileOffset Seek(wxFileOffset where, wxSeekMode mode = wxFromStart);    
    wxFileOffset Tell();
    wxFileOffset Length();

    double GetVolume();
    bool   SetVolume(double dVolume);

    bool    ShowPlayerControls(
        wxMediaCtrlPlayerControls flags = wxMEDIACTRLPLAYERCONTROLS_DEFAULT);

    bool Load(const wxString& fileName);
    bool LoadURI(const wxString& fileName);
    bool LoadURIWithProxy(const wxString& fileName, const wxString& proxy);
    %pythoncode { LoadFromURI = LoadURI }

    wxFileOffset GetDownloadProgress(); // DirectShow only
    wxFileOffset GetDownloadTotal();    // DirectShow only
    
    %property(DownloadProgress, GetDownloadProgress, doc="See `GetDownloadProgress`");
    %property(DownloadTotal, GetDownloadTotal, doc="See `GetDownloadTotal`");
    %property(PlaybackRate, GetPlaybackRate, SetPlaybackRate, doc="See `GetPlaybackRate` and `SetPlaybackRate`");
    %property(State, GetState, doc="See `GetState`");
    %property(Volume, GetVolume, SetVolume, doc="See `GetVolume` and `SetVolume`");
};



%constant wxEventType wxEVT_MEDIA_FINISHED;
%constant wxEventType wxEVT_MEDIA_STOP;
%constant wxEventType wxEVT_MEDIA_LOADED;
%constant wxEventType wxEVT_MEDIA_STATECHANGED;
%constant wxEventType wxEVT_MEDIA_PLAY;
%constant wxEventType wxEVT_MEDIA_PAUSE;


%pythoncode {
EVT_MEDIA_FINISHED       = wx.PyEventBinder( wxEVT_MEDIA_FINISHED, 1)
EVT_MEDIA_STOP           = wx.PyEventBinder( wxEVT_MEDIA_STOP, 1)
EVT_MEDIA_LOADED         = wx.PyEventBinder( wxEVT_MEDIA_LOADED, 1)
EVT_MEDIA_STATECHANGED   = wx.PyEventBinder( wxEVT_MEDIA_STATECHANGED, 1)
EVT_MEDIA_PLAY           = wx.PyEventBinder( wxEVT_MEDIA_PLAY, 1)
EVT_MEDIA_PAUSE          = wx.PyEventBinder( wxEVT_MEDIA_PAUSE, 1)
}    

//---------------------------------------------------------------------------

%init %{
%}

//---------------------------------------------------------------------------

