/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/webviewie.h
// Purpose:     wxMSW IE wxWebView backend
// Author:      Marianne Gagnon
// Id:          $Id$
// Copyright:   (c) 2010 Marianne Gagnon, Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef wxWebViewIE_H
#define wxWebViewIE_H

#include "wx/setup.h"

#if wxUSE_WEBVIEW_IE

#include "wx/control.h"
#include "wx/webview.h"
#include "wx/msw/ole/automtn.h"
#include "wx/msw/ole/activex.h"
#include "wx/sharedptr.h"
#include "wx/vector.h"

struct IHTMLDocument2;

class wxFSFile;
class wxFileSystem;

class WXDLLIMPEXP_WEB wxWebViewIE : public wxWebView
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
    virtual void LoadHistoryItem(wxSharedPtr<wxWebHistoryItem> item);
    virtual wxVector<wxSharedPtr<wxWebHistoryItem> > GetBackwardHistory();
    virtual wxVector<wxSharedPtr<wxWebHistoryItem> > GetForwardHistory();

    virtual bool CanGoForward();
    virtual bool CanGoBack();
    virtual void GoBack();
    virtual void GoForward();
    virtual void ClearHistory();
    virtual void EnableHistory(bool enable = true);
    virtual void Stop();
    virtual void Reload(wxWebViewReloadFlags flags = wxWEB_VIEW_RELOAD_DEFAULT);

    virtual wxString GetPageSource();
    virtual wxString GetPageText();

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

    //Clipboard functions
    virtual bool CanCut();
    virtual bool CanCopy();
    virtual bool CanPaste();
    virtual void Cut();
    virtual void Copy();
    virtual void Paste();

    //Undo / redo functionality
    virtual bool CanUndo();
    virtual bool CanRedo();
    virtual void Undo();
    virtual void Redo();

    //Editing functions
    virtual void SetEditable(bool enable = true);
    virtual bool IsEditable();

    //Selection
    virtual void SelectAll();
    virtual bool HasSelection();
    virtual void DeleteSelection();
    virtual wxString GetSelectedText();
    virtual wxString GetSelectedSource();
    virtual void ClearSelection();

    virtual void RunScript(const wxString& javascript);

    // ---- IE-specific methods

    // FIXME: I seem to be able to access remote webpages even in offline mode...
    bool IsOfflineMode();
    void SetOfflineMode(bool offline);

    wxWebViewZoom GetIETextZoom();
    void SetIETextZoom(wxWebViewZoom level);

    wxWebViewZoom GetIEOpticalZoom();
    void SetIEOpticalZoom(wxWebViewZoom level);

    void onActiveXEvent(wxActiveXEvent& evt);
    void onEraseBg(wxEraseEvent&) {}

    DECLARE_EVENT_TABLE();

private:
    wxActiveXContainer* m_container;
    wxAutomationObject m_ie;
    IWebBrowser2* m_webBrowser;
    DWORD m_dwCookie;

    //We store the current zoom type;
    wxWebViewZoomType m_zoomType;

    /** The "Busy" property of IWebBrowser2 does not always return busy when
     *  we'd want it to; this variable may be set to true in cases where the
     *  Busy property is false but should be true.
     */
    bool m_isBusy;
    //We manage our own history, the history list contains the history items 
    //which are added as documentcomplete events arrive, unless we are loading
    //an item from the history. The position is stored as an int, and reflects
    //where we are in the history list.
    wxVector<wxSharedPtr<wxWebHistoryItem> > m_historyList;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    //Generic helper functions for IHtmlDocument commands
    bool CanExecCommand(wxString command);
    void ExecCommand(wxString command);
    IHTMLDocument2* GetDocument();

};

class VirtualProtocol : public IInternetProtocol
{
protected:
    ULONG m_refCount;
    IInternetProtocolSink* m_protocolSink;
    wxString m_html;
    VOID * fileP;

    wxFSFile* m_file;
    wxFileSystem* m_fileSys;

public:
    VirtualProtocol();
    ~VirtualProtocol();

    //IUnknown
    ULONG STDMETHODCALLTYPE AddRef();
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE Release();

    //IInternetProtocolRoot
    HRESULT STDMETHODCALLTYPE Abort(HRESULT hrReason, DWORD dwOptions)
                                   { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Continue(PROTOCOLDATA *pProtocolData)
                                       { return S_OK; }
    HRESULT STDMETHODCALLTYPE Resume() { return S_OK; }
    HRESULT STDMETHODCALLTYPE Start(LPCWSTR szUrl, 
                                    IInternetProtocolSink *pOIProtSink,
                                    IInternetBindInfo *pOIBindInfo, 
                                    DWORD grfPI, 
                                    HANDLE_PTR dwReserved);
    HRESULT STDMETHODCALLTYPE Suspend() { return S_OK; }
    HRESULT STDMETHODCALLTYPE Terminate(DWORD dwOptions) { return S_OK; }

    //IInternetProtocol
    HRESULT STDMETHODCALLTYPE LockRequest(DWORD dwOptions) { return S_OK; }
    HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, 
                                   ULARGE_INTEGER* plibNewPosition) 
                                   { return E_FAIL; }
    HRESULT STDMETHODCALLTYPE UnlockRequest() { return S_OK; }
};

class ClassFactory : public IClassFactory
{
private:
    ULONG m_refCount;
public:
    //IUnknown
    ULONG STDMETHODCALLTYPE AddRef();
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE Release();

    //IClassFactory
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, 
                                             REFIID riid, void** ppvObject);
    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock);
};

#endif // wxUSE_WEBVIEW_IE

#endif // wxWebViewIE_H
