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

#if wxUSE_DATETIME
    bool Seek(const wxTimeSpan& where);
#endif

    virtual void SetLabel(const wxString& label);

protected:
    void OnSize(wxSizeEvent& evt);
    wxSize DoGetBestSize() const;
    bool InitQT();

    struct MovieRecord* m_movie;
    wxSize m_bestSize;
    class _wxQTTimer* m_timer;
    
    DECLARE_DYNAMIC_CLASS(wxMovieCtrl);
    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_MOVIECTRL