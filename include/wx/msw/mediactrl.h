/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/mediactrl.h
// Purpose:     DirectX7+ wxMediaCtrl MSW
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     11/07/04
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"

#if wxUSE_MEDIACTRL

#include "wx/control.h"
#include "wx/uri.h"

enum wxMediaState
{
    wxMEDIASTATE_STOPPED,
    wxMEDIASTATE_PAUSED,
    wxMEDIASTATE_PLAYING
};

class wxMediaCtrl : public wxControl
{
public:
    wxMediaCtrl() : m_imp(NULL)
    {                                                                   }

    wxMediaCtrl(wxWindow* parent, wxWindowID id, const wxString& fileName,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = 0, long driver = 0, const wxString& name = wxPanelNameStr) : m_imp(NULL)
    {   Create(parent, id, fileName, pos, size, style, driver, name);    }


    wxMediaCtrl(wxWindow* parent, wxWindowID id, const wxURI& location,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = 0, long driver = 0, const wxString& name = wxPanelNameStr) : m_imp(NULL)
    {   Create(parent, id, location, pos, size, style, driver, name);    }

    ~wxMediaCtrl();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& fileName,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = 0, long driver = 0, const wxString& name = wxPanelNameStr);

    bool Create(wxWindow* parent, wxWindowID id, const wxURI& location,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = 0, long driver = 0, const wxString& name = wxPanelNameStr);

    bool Play();
    bool Pause();
    bool Stop();

    bool Load(const wxString& fileName);
    bool Load(const wxURI& location);

    wxMediaState GetState();

    double GetPlaybackRate();
    bool SetPlaybackRate(double dRate);

    bool SetPosition(long where);
    long GetPosition();
    long GetDuration();

protected:
    virtual void DoMoveWindow(int x, int y, int w, int h);
    wxSize DoGetBestSize() const;

    //msw-specific - we need to overload the window proc
    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    class wxMediaCtrlImpl* m_imp;
    bool m_bLoaded;

    DECLARE_DYNAMIC_CLASS(wxMediaCtrl);
};

//Event stuff
class WXDLLEXPORT wxMediaEvent : public wxNotifyEvent
{
public:
    wxMediaEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxNotifyEvent(commandType, id)
    {               }

    wxMediaEvent(const wxMediaEvent &clone)
            : wxNotifyEvent(clone.GetEventType(), clone.GetId())
    {               }

    virtual wxEvent *Clone() const { return new wxMediaEvent(*this); }

    DECLARE_DYNAMIC_CLASS(wxMediaEvent)
};

#define wxMEDIA_FINISHED_ID    13000
DECLARE_EVENT_TYPE(wxEVT_MEDIA_FINISHED, wxMEDIA_FINISHED_ID)
typedef void (wxEvtHandler::*wxMediaEventFunction)(wxMediaEvent&);
#define EVT_MEDIA_FINISHED(winid, fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_MEDIA_FINISHED, winid, wxID_ANY, (wxObjectEventFunction) (wxEventFunction) (wxMediaEventFunction) & fn, (wxObject *) NULL ),

#endif // wxUSE_MEDIACTRL

