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

#include "wx/control.h"
#include "wx/webview.h"
#include "wx/msw/ole/automtn.h"
#include "wx/msw/ole/activex.h"
#include "wx/msw/ole/oleutils.h"
#include "wx/msw/private/comptr.h"
#include "wx/msw/wrapwin.h"
#include "wx/msw/missing.h"
#include "wx/msw/webview_missing.h"
#include "wx/sharedptr.h"
#include "wx/vector.h"
#include "wx/msw/private.h"

struct IHTMLDocument2;
struct IHTMLElement;
struct IMarkupPointer;
class wxFSFile;
class ClassFactory;
class wxIEContainer;
class DocHostUIHandler;
class wxFindPointers;
class wxIInternetProtocol;

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
           const wxString& name = wxWebViewNameStr)
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
           const wxString& name = wxWebViewNameStr) wxOVERRIDE;

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
    virtual void SetZoom(wxWebViewZoom zoom) wxOVERRIDE;

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

    virtual void* GetNativeBackend() const wxOVERRIDE { return m_webBrowser; }

    // ---- IE-specific methods

    // FIXME: I seem to be able to access remote webpages even in offline mode...
    bool IsOfflineMode();
    void SetOfflineMode(bool offline);

    wxWebViewZoom GetIETextZoom() const;
    void SetIETextZoom(wxWebViewZoom level);

    wxWebViewZoom GetIEOpticalZoom() const;
    void SetIEOpticalZoom(wxWebViewZoom level);

    void onActiveXEvent(wxActiveXEvent& evt);
    void onEraseBg(wxEraseEvent&) {}

    // Establish sufficiently modern emulation level for the browser control to
    // allow RunScript() to return any kind of values.
    static bool MSWSetModernEmulationLevel(bool modernLevel = true);

    wxDECLARE_EVENT_TABLE();

protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) wxOVERRIDE;

private:
    wxIEContainer* m_container;
    wxAutomationObject m_ie;
    IWebBrowser2* m_webBrowser;
    DWORD m_dwCookie;
    wxCOMPtr<DocHostUIHandler> m_uiHandler;

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
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    wxVector<ClassFactory*> m_factories;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    //We store find flag, results and position.
    wxVector<wxFindPointers> m_findPointers;
    int m_findFlags;
    wxString m_findText;
    int m_findPosition;

    //Generic helper functions
    bool CanExecCommand(wxString command) const;
    void ExecCommand(wxString command);
    wxCOMPtr<IHTMLDocument2> GetDocument() const;
    bool IsElementVisible(wxCOMPtr<IHTMLElement> elm);
    //Find helper functions.
    void FindInternal(const wxString& text, int flags, int internal_flag);
    long FindNext(int direction = 1);
    void FindClear();
    //Toggles control features see INTERNETFEATURELIST for values.
    bool EnableControlFeature(long flag, bool enable = true);

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
                              const wxString& name = wxWebViewNameStr) wxOVERRIDE
    { return new wxWebViewIE(parent, id, url, pos, size, style, name); }
};

class VirtualProtocol : public wxIInternetProtocol
{
protected:
    wxIInternetProtocolSink* m_protocolSink;
    wxString m_html;
    VOID * fileP;

    wxFSFile* m_file;
    wxSharedPtr<wxWebViewHandler> m_handler;

public:
    VirtualProtocol(wxSharedPtr<wxWebViewHandler> handler);
    virtual ~VirtualProtocol() {}

    //IUnknown
    DECLARE_IUNKNOWN_METHODS;

    //IInternetProtocolRoot
    HRESULT STDMETHODCALLTYPE Abort(HRESULT WXUNUSED(hrReason),
                                    DWORD WXUNUSED(dwOptions)) wxOVERRIDE
                                   { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Continue(wxPROTOCOLDATA *WXUNUSED(pProtocolData)) wxOVERRIDE
                                       { return S_OK; }
    HRESULT STDMETHODCALLTYPE Resume() wxOVERRIDE { return S_OK; }
    HRESULT STDMETHODCALLTYPE Start(LPCWSTR szUrl,
                                    wxIInternetProtocolSink *pOIProtSink,
                                    wxIInternetBindInfo *pOIBindInfo,
                                    DWORD grfPI,
                                    HANDLE_PTR dwReserved) wxOVERRIDE;
    HRESULT STDMETHODCALLTYPE Suspend() wxOVERRIDE { return S_OK; }
    HRESULT STDMETHODCALLTYPE Terminate(DWORD WXUNUSED(dwOptions)) wxOVERRIDE { return S_OK; }

    //IInternetProtocol
    HRESULT STDMETHODCALLTYPE LockRequest(DWORD WXUNUSED(dwOptions)) wxOVERRIDE
                                          { return S_OK; }
    HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead) wxOVERRIDE;
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER WXUNUSED(dlibMove),
                                   DWORD WXUNUSED(dwOrigin),
                                   ULARGE_INTEGER* WXUNUSED(plibNewPosition)) wxOVERRIDE
                                   { return E_FAIL; }
    HRESULT STDMETHODCALLTYPE UnlockRequest() wxOVERRIDE { return S_OK; }
};

class ClassFactory : public IClassFactory
{
public:
    ClassFactory(wxSharedPtr<wxWebViewHandler> handler) : m_handler(handler) 
        { AddRef(); }
    virtual ~ClassFactory() {}

    wxString GetName() { return m_handler->GetName(); }

    //IClassFactory
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter,
                                             REFIID riid, void** ppvObject) wxOVERRIDE;
    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) wxOVERRIDE;

    //IUnknown
    DECLARE_IUNKNOWN_METHODS;

private:
    wxSharedPtr<wxWebViewHandler> m_handler;
};

class wxIEContainer : public wxActiveXContainer
{
public:
    wxIEContainer(wxWindow *parent, REFIID iid, IUnknown *pUnk, DocHostUIHandler* uiHandler = NULL);
    virtual ~wxIEContainer();
    virtual bool QueryClientSiteInterface(REFIID iid, void **_interface, const char *&desc) wxOVERRIDE;
private:
    DocHostUIHandler* m_uiHandler;
};

class DocHostUIHandler : public wxIDocHostUIHandler
{
public:
    DocHostUIHandler(wxWebView* browser) { m_browser = browser; }
    virtual ~DocHostUIHandler() {}

    virtual HRESULT wxSTDCALL ShowContextMenu(DWORD dwID, POINT *ppt,
                                              IUnknown *pcmdtReserved,
                                              IDispatch *pdispReserved) wxOVERRIDE;

    virtual HRESULT wxSTDCALL GetHostInfo(DOCHOSTUIINFO *pInfo) wxOVERRIDE;

    virtual HRESULT wxSTDCALL ShowUI(DWORD dwID,
                                     IOleInPlaceActiveObject *pActiveObject,
                                     IOleCommandTarget *pCommandTarget,
                                     IOleInPlaceFrame *pFrame,
                                     IOleInPlaceUIWindow *pDoc) wxOVERRIDE;

    virtual HRESULT wxSTDCALL HideUI(void) wxOVERRIDE;

    virtual HRESULT wxSTDCALL UpdateUI(void) wxOVERRIDE;

    virtual HRESULT wxSTDCALL EnableModeless(BOOL fEnable) wxOVERRIDE;

    virtual HRESULT wxSTDCALL OnDocWindowActivate(BOOL fActivate) wxOVERRIDE;

    virtual HRESULT wxSTDCALL OnFrameWindowActivate(BOOL fActivate) wxOVERRIDE;

    virtual HRESULT wxSTDCALL ResizeBorder(LPCRECT prcBorder,
                                           IOleInPlaceUIWindow *pUIWindow,
                                           BOOL fRameWindow) wxOVERRIDE;

    virtual HRESULT wxSTDCALL TranslateAccelerator(LPMSG lpMsg,
                                                   const GUID *pguidCmdGroup,
                                                   DWORD nCmdID) wxOVERRIDE;

    virtual HRESULT wxSTDCALL GetOptionKeyPath(LPOLESTR *pchKey,
                                               DWORD dw) wxOVERRIDE;

    virtual HRESULT wxSTDCALL GetDropTarget(IDropTarget *pDropTarget,
                                            IDropTarget **ppDropTarget) wxOVERRIDE;

    virtual HRESULT wxSTDCALL GetExternal(IDispatch **ppDispatch) wxOVERRIDE;

    virtual HRESULT wxSTDCALL TranslateUrl(DWORD dwTranslate,
                                           OLECHAR *pchURLIn,
                                           OLECHAR **ppchURLOut) wxOVERRIDE;

    virtual HRESULT wxSTDCALL FilterDataObject(IDataObject *pDO,
                                               IDataObject **ppDORet) wxOVERRIDE;
    //IUnknown
    DECLARE_IUNKNOWN_METHODS;

private:
    wxWebView* m_browser;
};

class wxFindPointers
{
public:
    wxFindPointers(wxIMarkupPointer *ptrBegin, wxIMarkupPointer *ptrEnd)
    {
        begin = ptrBegin;
        end = ptrEnd;
    }
    //The two markup pointers.
    wxIMarkupPointer *begin, *end;
};

#endif // wxUSE_WEBVIEW && wxUSE_WEBVIEW_IE && defined(__WXMSW__)

#endif // wxWebViewIE_H
