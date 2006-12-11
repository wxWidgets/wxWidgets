/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/srchctrl.h
// Purpose:     mac carbon wxSearchCtrl class
// Author:      Vince Harron
// Modified by:
// Created:     2006-02-19
// RCS-ID:      
// Copyright:   Vince Harron
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SEARCHCTRL_H_
#define _WX_SEARCHCTRL_H_

#if wxUSE_SEARCHCTRL

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "srchctrl.h"
#endif

class WXDLLEXPORT wxSearchCtrlBase : public wxTextCtrl
{
public:
    wxSearchCtrlBase() {}
    virtual ~wxSearchCtrlBase() {}

    // search control 
    virtual void SetMenu( wxMenu* menu ) = 0;
    virtual wxMenu* GetMenu() = 0;


    // get/set options
    virtual void SetSearchButtonVisible( bool show ) = 0;
    virtual bool GetSearchButtonVisible() const = 0;

    virtual void SetCancelButtonVisible( bool show ) = 0;
    virtual bool GetCancelButtonVisible() const = 0;
protected:
};

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

    ~wxSearchCtrl();

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
    virtual void SetSearchButtonVisible( bool show );
    virtual bool GetSearchButtonVisible() const;

    virtual void SetCancelButtonVisible( bool show );
    virtual bool GetCancelButtonVisible() const;

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

