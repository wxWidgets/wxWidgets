/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/srchctrl.h
// Purpose:     mac carbon wxSearchCtrl class
// Author:      Vince Harron
// Created:     2006-02-19
// RCS-ID:      $Id$
// Copyright:   Vince Harron
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SEARCHCTRL_H_
#define _WX_SEARCHCTRL_H_

#if wxUSE_SEARCHCTRL

class wxMacSearchFieldControl;

class WXDLLEXPORT wxSearchCtrl : public wxSearchCtrlBase
{
public:
    // creation
    // --------

    wxSearchCtrl();
    wxSearchCtrl(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxSearchCtrlNameStr);

    virtual ~wxSearchCtrl();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxSearchCtrlNameStr);

    // get/set search button menu
    // --------------------------
    virtual void SetMenu( wxMenu* menu );
    virtual wxMenu* GetMenu();

    // get/set search options
    // ----------------------
    virtual void ShowSearchButton( bool show );
    virtual bool IsSearchButtonVisible() const;

    virtual void ShowCancelButton( bool show );
    virtual bool IsCancelButtonVisible() const;

#if wxABI_VERSION >= 20802
    // TODO: In 2.9 these should probably be virtual, and declared in the base class...
    void SetDescriptiveText(const wxString& text);
    wxString GetDescriptiveText() const;
#endif

    virtual wxInt32     MacSearchFieldSearchHit( WXEVENTHANDLERREF handler , WXEVENTREF event ) ;
    virtual wxInt32     MacSearchFieldCancelHit( WXEVENTHANDLERREF handler , WXEVENTREF event ) ;

    wxMacSearchFieldControl * GetPeer() const
    { return (wxMacSearchFieldControl*) m_peer; }

    virtual void SetFocus();

protected:

    wxSize DoGetBestSize() const;

    void Init();

    virtual void CreatePeer(
           const wxString& str,
           const wxPoint& pos,
           const wxSize& size, long style );

    wxMenu *m_menu;

private:
    DECLARE_DYNAMIC_CLASS(wxSearchCtrl)

    DECLARE_EVENT_TABLE()
};

#endif // wxUSE_SEARCHCTRL

#endif // _WX_SEARCHCTRL_H_

