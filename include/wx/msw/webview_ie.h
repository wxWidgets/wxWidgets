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
           const wxString& name = wxASCII_STR(wxWebViewNameStr)) wxOVERRIDE;

    virtual void LoadURL(const wxString& url) wxOVERRIDE;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) wxOVERRIDE;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() wxOVERRIDE;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() wxOVERRIDE;

    virtual bool CanGoForward() const wxOVERRIDE;
    virtual bool CanGoBack() const wxOVERRIDE;
    virtual void GoBack() wxOVERRIDE;
    virtual void GoForward() wxOVERRIDE;
    virtual void ClearHistory() wxOVERRIDE;
    virtual void EnableHistory(bool enable = true) wxOVERRIDE;
    virtual void Stop() wxOVERRIDE;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) wxOVERRIDE;

    virtual wxString GetPageSource() const wxOVERRIDE;
    virtual wxString GetPageText() const wxOVERRIDE;

    virtual bool IsBusy() const wxOVERRIDE;
    virtual wxString GetCurrentURL() const wxOVERRIDE;
    virtual wxString GetCurrentTitle() const wxOVERRIDE;

    virtual void SetZoomType(wxWebViewZoomType) wxOVERRIDE;
    virtual wxWebViewZoomType GetZoomType() const wxOVERRIDE;
    virtual bool CanSetZoomType(wxWebViewZoomType) const wxOVERRIDE;

    virtual void Print() wxOVERRIDE;

    virtual wxWebViewZoom GetZoom() const wxOVERRIDE;
    virtual float GetZoomFactor() const wxOVERRIDE;
    virtual void SetZoom(wxWebViewZoom zoom) wxOVERRIDE;
    virtual void SetZoomFactor(float zoom) wxOVERRIDE;

    //Clipboard functions
    virtual bool CanCut() const wxOVERRIDE;
    virtual bool CanCopy() const wxOVERRIDE;
    virtual bool CanPaste() const wxOVERRIDE;
    virtual void Cut() wxOVERRIDE;
    virtual void Copy() wxOVERRIDE;
    virtual void Paste() wxOVERRIDE;

    //Undo / redo functionality
    virtual bool CanUndo() const wxOVERRIDE;
    virtual bool CanRedo() const wxOVERRIDE;
    virtual void Undo() wxOVERRIDE;
    virtual void Redo() wxOVERRIDE;

    //Find function
    virtual long Find(const wxString& text, int flags = wxWEBVIEW_FIND_DEFAULT) wxOVERRIDE;

    //Editing functions
    virtual void SetEditable(bool enable = true) wxOVERRIDE;
    virtual bool IsEditable() const wxOVERRIDE;

    //Selection
    virtual void SelectAll() wxOVERRIDE;
    virtual bool HasSelection() const wxOVERRIDE;
    virtual void DeleteSelection() wxOVERRIDE;
    virtual wxString GetSelectedText() const wxOVERRIDE;
    virtual wxString GetSelectedSource() const wxOVERRIDE;
    virtual void ClearSelection() wxOVERRIDE;

    virtual bool RunScript(const wxString& javascript, wxString* output = NULL) wxOVERRIDE;

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) wxOVERRIDE;

    virtual void* GetNativeBackend() const wxOVERRIDE;

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
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) wxOVERRIDE;

private:
    wxWebViewIEImpl* m_impl;

    wxDECLARE_DYNAMIC_CLASS(wxWebViewIE);
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryIE : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() wxOVERRIDE { return new wxWebViewIE; }
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxWebViewDefaultURLStr,
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxASCII_STR(wxWebViewNameStr)) wxOVERRIDE
    { return new wxWebViewIE(parent, id, url, pos, size, style, name); }
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_IE && defined(__WXMSW__)

#endif // wxWebViewIE_H
