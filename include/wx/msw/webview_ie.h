/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/webview_ie.h
// Purpose:     wxMSW IE wxWebView backend
// Author:      Marianne Gagnon
// Copyright:   (c) 2010 Marianne Gagnon, 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef wxWebViewIE_H
#define wxWebViewIE_H

#include "wx/setup.h"

#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_IE && defined(__WXMSW__)

#include "wx/webview.h"
#include "wx/msw/ole/activex.h"

class wxWebViewIEImpl;

// Note that the highest emulation level may be used even when the
// corresponding browser version is not installed.
//
// Using FORCE options is not recommended, DEFAULT can be used to reset level
// to the system default.
//
// The value of the constants were taken from
//
//   https://msdn.microsoft.com/library/ee330730.aspx#browser_emulation
//
// and must not be changed.
enum wxWebViewIE_EmulationLevel
{
    wxWEBVIEWIE_EMU_DEFAULT =    0,
    wxWEBVIEWIE_EMU_IE7 =        7000,
    wxWEBVIEWIE_EMU_IE8 =        8000,
    wxWEBVIEWIE_EMU_IE8_FORCE =  8888,
    wxWEBVIEWIE_EMU_IE9 =        9000,
    wxWEBVIEWIE_EMU_IE9_FORCE =  9999,
    wxWEBVIEWIE_EMU_IE10 =       10000,
    wxWEBVIEWIE_EMU_IE10_FORCE = 10001,
    wxWEBVIEWIE_EMU_IE11 =       11000,
    wxWEBVIEWIE_EMU_IE11_FORCE = 11001
};

class WXDLLIMPEXP_WEBVIEW wxWebViewIE : public wxWebView
{
public:

    wxWebViewIE() {}

    wxWebViewIE(wxWindow* parent,
           wxWindowID id,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxASCII_STR(wxWebViewNameStr))
   {
       Create(parent, id, url, pos, size, style, name);
   }

    ~wxWebViewIE();

    bool Create(wxWindow* parent,
           wxWindowID id,
           const wxString& url = wxWebViewDefaultURLStr,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxASCII_STR(wxWebViewNameStr)) override;

    virtual void LoadURL(const wxString& url) override;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() override;

    virtual bool CanGoForward() const override;
    virtual bool CanGoBack() const override;
    virtual void GoBack() override;
    virtual void GoForward() override;
    virtual void ClearHistory() override;
    virtual void EnableHistory(bool enable = true) override;
    virtual void Stop() override;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) override;

    virtual wxString GetPageSource() const override;
    virtual wxString GetPageText() const override;

    virtual bool IsBusy() const override;
    virtual wxString GetCurrentURL() const override;
    virtual wxString GetCurrentTitle() const override;

    virtual void SetZoomType(wxWebViewZoomType) override;
    virtual wxWebViewZoomType GetZoomType() const override;
    virtual bool CanSetZoomType(wxWebViewZoomType) const override;

    virtual void Print() override;

    virtual wxWebViewZoom GetZoom() const override;
    virtual float GetZoomFactor() const override;
    virtual void SetZoom(wxWebViewZoom zoom) override;
    virtual void SetZoomFactor(float zoom) override;

    //Clipboard functions
    virtual bool CanCut() const override;
    virtual bool CanCopy() const override;
    virtual bool CanPaste() const override;
    virtual void Cut() override;
    virtual void Copy() override;
    virtual void Paste() override;

    //Undo / redo functionality
    virtual bool CanUndo() const override;
    virtual bool CanRedo() const override;
    virtual void Undo() override;
    virtual void Redo() override;

    //Find function
    virtual long Find(const wxString& text, int flags = wxWEBVIEW_FIND_DEFAULT) override;

    //Editing functions
    virtual void SetEditable(bool enable = true) override;
    virtual bool IsEditable() const override;

    //Selection
    virtual void SelectAll() override;
    virtual bool HasSelection() const override;
    virtual void DeleteSelection() override;
    virtual wxString GetSelectedText() const override;
    virtual wxString GetSelectedSource() const override;
    virtual void ClearSelection() override;

    virtual bool RunScript(const wxString& javascript, wxString* output = nullptr) const override;

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) override;

    virtual void* GetNativeBackend() const override;

    // ---- IE-specific methods

    // FIXME: I seem to be able to access remote webpages even in offline mode...
    bool IsOfflineMode();
    void SetOfflineMode(bool offline);

    wxWebViewZoom GetIETextZoom() const;
    void SetIETextZoom(wxWebViewZoom level);

    wxWebViewZoom GetIEOpticalZoom() const;
    int GetIEOpticalZoomFactor() const;
    void SetIEOpticalZoom(wxWebViewZoom level);
    void SetIEOpticalZoomFactor(int zoom);

    void onActiveXEvent(wxActiveXEvent& evt);
    void onEraseBg(wxEraseEvent&) {}

    // Switch to specific emulation level for the browser control to
    // ensure RunScript() and web pages work as designed and also change the
    // value of User-Agent header sent to web server.
    static bool
    MSWSetEmulationLevel(wxWebViewIE_EmulationLevel level = wxWEBVIEWIE_EMU_IE11);

    // This function is provided only for compatibility reasons, use
    // MSWSetEmulationLevel() in the new code instead.
    static bool MSWSetModernEmulationLevel(bool modernLevel = true)
    {
        return MSWSetEmulationLevel(modernLevel ? wxWEBVIEWIE_EMU_IE8
                                                : wxWEBVIEWIE_EMU_DEFAULT);
    }

    wxDECLARE_EVENT_TABLE();

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) override;

private:
    wxWebViewIEImpl* m_impl;

    wxDECLARE_DYNAMIC_CLASS(wxWebViewIE);
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryIE : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() override { return new wxWebViewIE; }
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxASCII_STR(wxWebViewNameStr)) override
    { return new wxWebViewIE(parent, id, url, pos, size, style, name); }
    virtual wxVersionInfo GetVersionInfo() override;
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_IE && defined(__WXMSW__)

#endif // wxWebViewIE_H
