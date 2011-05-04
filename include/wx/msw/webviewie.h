/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/webviewie.h
// Purpose:     wxMSW IE wxWebView backend
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef wxWebViewIE_H
#define wxWebViewIE_H

#include "wx/setup.h"

#if wxHAVE_WEB_BACKEND_IE

#include "wx/control.h"
#include "wx/webview.h"
#include "wx/msw/ole/automtn.h"
#include "wx/msw/ole/activex.h"

// TODO: move this to wx/msw/chkconf.h
#if wxUSE_ACTIVEX != 1
#error "wxUSE_ACTIVEX must be activated for this to work"
#endif

#if wxUSE_OLE_AUTOMATION != 1
#error "wxUSE_OLE_AUTOMATION must be activated for this to work"
#endif

// FIXME: get those DLL export macros right...

class wxWebViewIE : public wxWebView
{
public:

    wxWebViewIE() {}

    wxWebViewIE(wxWindow* parent,
           wxWindowID id,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxWebViewNameStr)
   {
       Create(parent, id, url, pos, size, style, name);
   }

    bool Create(wxWindow* parent,
           wxWindowID id,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxWebViewNameStr);

    virtual void LoadUrl(const wxString& url);

    virtual bool CanGoForward() { return m_canNavigateForward; }
    virtual bool CanGoBack() { return m_canNavigateBack; }
    virtual void GoBack();
    virtual void GoForward();
    virtual void Stop();
    virtual void Reload(wxWebViewReloadFlags flags=0);

    virtual wxString GetPageSource();

    virtual bool IsBusy();
    virtual wxString GetCurrentURL();
    virtual wxString GetCurrentTitle();

    virtual void SetZoomType(wxWebViewZoomType);
    virtual wxWebViewZoomType GetZoomType() const;
    virtual bool CanSetZoomType(wxWebViewZoomType) const;

    virtual void Print();

    virtual void SetPage(const wxString& html, const wxString& baseUrl);

    virtual wxWebViewZoom GetZoom();
    virtual void SetZoom(wxWebViewZoom zoom);

    // ---- IE-specific methods

    // FIXME: I seem to be able to access remote webpages even in offline mode...
    bool IsOfflineMode();
    void SetOfflineMode(bool offline);

    /**
     * Get text zoom
     * @return text zoom from 0 to 4
     */
    int GetIETextZoom();

    /**
     *  @param level 0 to 4
     */
    void SetIETextZoom(int level);

    void SetIEOpticalZoom(float zoom);
    float GetIEOpticalZoom();

    void onActiveXEvent(wxActiveXEvent& evt);
    void onEraseBg(wxEraseEvent& evt) {}

    DECLARE_EVENT_TABLE();

private:
    wxActiveXContainer* m_container;
    wxAutomationObject m_ie;
    IWebBrowser2* m_webBrowser;
    DWORD m_dwCookie;
    bool m_canNavigateBack;
    bool m_canNavigateForward;

    /** The "Busy" property of IWebBrowser2 does not always return busy when
     *  we'd want it to; this variable may be set to true in cases where the
     *  Busy property is false but should be true.
     */
    bool m_isBusy;
};

#endif // wxHAVE_WEB_BACKEND_IE

#endif // wxWebViewIE_H
