/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/moviectrl.h
// Purpose:     DirectX7+ wxMovieCtrl MSW 
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by: 
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#if wxUSE_MOVIECTRL

#include "wx/datetime.h"
#include "wx/control.h"

enum wxMovieCtrlState
{
    wxMOVIECTRL_STOPPED,
    wxMOVIECTRL_PAUSED,
    wxMOVIECTRL_PLAYING
};

class wxMovieCtrl : public wxControl
{
public:
    wxMovieCtrl() 
    {                                                                   }

    wxMovieCtrl(wxWindow* parent, wxWindowID id, const wxString& fileName, const wxString& label = wxT(""),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
                long style = 0, const wxString& name = wxPanelNameStr)
    {   Create(parent, id, fileName, label, pos, size, style, name);    } 

    ~wxMovieCtrl();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& fileName, const wxString& label = wxT(""),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
                long style = 0, const wxString& name = wxPanelNameStr);

    bool Play();
    bool Pause();
    bool Stop();
    
    wxMovieCtrlState GetState();

    double GetPlaybackRate();
    bool SetPlaybackRate(double dRate);

#if wxUSE_DATETIME
    bool Seek(const wxTimeSpan& where);
    wxTimeSpan Tell();
    wxTimeSpan Length();
#endif

    virtual void SetLabel(const wxString& label);

protected:
    void OnSize(wxSizeEvent& evt);
    wxSize DoGetBestSize() const;
    bool m_bVideo;

    //msw-specific - we need to overload the window proc
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    void* m_pGB;
    void* m_pMC;
    void* m_pME;
    void* m_pVW;
    void* m_pBA;
    void* m_pBV;
    void* m_pMS;

    wxSize m_bestSize;
    
    DECLARE_DYNAMIC_CLASS(wxMovieCtrl);
};

//Event stuff
class WXDLLEXPORT wxMovieEvent : public wxNotifyEvent 
{ 
public:
    wxMovieEvent(wxEventType commandType = wxEVT_NULL, int id = 0) 
        : wxNotifyEvent(commandType, id) 
    {               }
    
    wxMovieEvent(const wxMovieEvent &clone) 
            : wxNotifyEvent(clone.GetEventType(), clone.GetId()) 
    {               }

    wxEvent *Clone() { return new wxMovieEvent(*this); } 
   
    DECLARE_DYNAMIC_CLASS(wxMovieEvent) 
}; 

#define wxMOVIE_FINISHED_ID    13000 
DECLARE_EVENT_TYPE(wxEVT_MOVIE_FINISHED, wxMOVIE_FINISHED_ID) 
typedef void (wxEvtHandler::*wxMovieEventFunction)(wxMovieEvent&); 
#define EVT_MOVIE_FINISHED(winid, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_MOVIE_FINISHED, winid, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxMovieEventFunction) & fn, (wxObject *) NULL ), 

#endif // wxUSE_MOVIECTRL