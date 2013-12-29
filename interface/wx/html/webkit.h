/////////////////////////////////////////////////////////////////////////////
// Name:        wx/html/webkit.h
// Purpose:     wxWebKitCtrl - embeddable web kit control
// Author:      Jethro Grassie / Kevin Ollivier
// Modified by:
// Created:     2004-4-16
// Copyright:   (c) Jethro Grassie / Kevin Ollivier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBKIT_H
#define _WX_WEBKIT_H

/**
 @class wxWebKitCtrl

 This control is a native wrapper around the Safari web browsing engine. This wrapper
 differs from the one in wxWebView in that this version supports functionality specific to
 WebKit, such as having RunScript return a value, which is a very critical feature in many
 web embedding scenarios.
 **/

class wxWebKitCtrl : public wxControl
{
public:

    wxWebKitCtrl() {}
    wxWebKitCtrl(wxWindow *parent,
                    wxWindowID winID,
                    const wxString& strURL,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxWebKitCtrlNameStr);
    
    bool Create(wxWindow *parent,
                wxWindowID winID,
                const wxString& strURL,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxWebKitCtrlNameStr);
    virtual ~wxWebKitCtrl();

    void LoadURL(const wxString &url);

    bool CanGoBack();
    bool CanGoForward();
    bool GoBack();
    bool GoForward();
    void Reload();
    void Stop();
    bool CanGetPageSource();
    wxString GetPageSource();
    void SetPageSource(const wxString& source, const wxString& baseUrl = wxEmptyString);
    wxString GetPageURL();
    void SetPageTitle(const wxString& title);
    wxString GetPageTitle();

    // since these worked in 2.6, add wrappers
    void SetTitle(const wxString& title);
    wxString GetTitle();

    wxString GetSelection();

    bool CanIncreaseTextSize();
    void IncreaseTextSize();
    bool CanDecreaseTextSize();
    void DecreaseTextSize();

    void Print(bool showPrompt = false);

    void MakeEditable(bool enable = true);
    bool IsEditable();

    wxString RunScript(const wxString& javascript);

    void SetScrollPos(int pos);
    int GetScrollPos();
};

// ----------------------------------------------------------------------------
// Web Kit Events
// ----------------------------------------------------------------------------

enum {
    wxWEBKIT_STATE_START = 1,
    wxWEBKIT_STATE_NEGOTIATING = 2,
    wxWEBKIT_STATE_REDIRECTING = 4,
    wxWEBKIT_STATE_TRANSFERRING = 8,
    wxWEBKIT_STATE_STOP = 16,
        wxWEBKIT_STATE_FAILED = 32
};

enum {
    wxWEBKIT_NAV_LINK_CLICKED = 1,
    wxWEBKIT_NAV_BACK_NEXT = 2,
    wxWEBKIT_NAV_FORM_SUBMITTED = 4,
    wxWEBKIT_NAV_RELOAD = 8,
    wxWEBKIT_NAV_FORM_RESUBMITTED = 16,
    wxWEBKIT_NAV_OTHER = 32

};



class wxWebKitBeforeLoadEvent : public wxCommandEvent
{
public:
    bool IsCancelled() { return m_cancelled; }
    void Cancel(bool cancel = true) { m_cancelled = cancel; }
    wxString GetURL() { return m_url; }
    void SetURL(const wxString& url) { m_url = url; }
    void SetNavigationType(int navType) { m_navType = navType; }
    int GetNavigationType() { return m_navType; }

    wxWebKitBeforeLoadEvent( wxWindow* win = 0 );
};

class wxWebKitStateChangedEvent : public wxCommandEvent
{
public:
    int GetState() { return m_state; }
    void SetState(const int state) { m_state = state; }
    wxString GetURL() { return m_url; }
    void SetURL(const wxString& url) { m_url = url; }

    wxWebKitStateChangedEvent( wxWindow* win = 0 );
};


class wxWebKitNewWindowEvent : public wxCommandEvent
{
public:
    wxString GetURL() const { return m_url; }
    void SetURL(const wxString& url) { m_url = url; }
    wxString GetTargetName() const { return m_targetName; }
    void SetTargetName(const wxString& name) { m_targetName = name; }

    wxWebKitNewWindowEvent( wxWindow* win = 0 );
};

wxEventType wxEVT_WEBKIT_STATE_CHANGED;
wxEventType wxEVT_WEBKIT_BEFORE_LOAD;
wxEventType wxEVT_WEBKIT_NEW_WINDOW;


#endif
    // _WX_WEBKIT_H_
