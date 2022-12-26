/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/private/webview_ie.h
// Purpose:     wxMSW IE wxWebView backend private classes
// Author:      Marianne Gagnon
// Copyright:   (c) 2010 Marianne Gagnon, 2011 Steven Lamerton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef wxWebViewIE_PRIVATE_H
#define wxWebViewIE_PRIVATE_H

#include "wx/msw/ole/automtn.h"
#include "wx/msw/private/comptr.h"
#include "wx/msw/private/webview_missing.h"

class ClassFactory;
class wxIEContainer;
class DocHostUIHandler;
class wxFindPointers;

class wxWebViewIEImpl
{
public:
    explicit wxWebViewIEImpl(wxWebViewIE* webview);
    ~wxWebViewIEImpl();

    bool Create();

    wxWebViewIE* m_webview;

    wxIEContainer* m_container;
    wxAutomationObject m_ie;
    IWebBrowser2* m_webBrowser;
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
    long Find(const wxString& text, int flags = wxWEBVIEW_FIND_DEFAULT);
    void FindInternal(const wxString& text, int flags, int internal_flag);
    long FindNext(int direction = 1);
    void FindClear();
    //Toggles control features see INTERNETFEATURELIST for values.
    bool EnableControlFeature(long flag, bool enable = true);

    wxDECLARE_NO_COPY_CLASS(wxWebViewIEImpl);
};

class VirtualProtocol : public wxIInternetProtocol, public wxIInternetProtocolInfo
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
                                    DWORD WXUNUSED(dwOptions)) override
                                   { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Continue(wxPROTOCOLDATA *WXUNUSED(pProtocolData)) override
                                       { return S_OK; }
    HRESULT STDMETHODCALLTYPE Resume() override { return S_OK; }
    HRESULT STDMETHODCALLTYPE Start(LPCWSTR szUrl,
                                    wxIInternetProtocolSink *pOIProtSink,
                                    wxIInternetBindInfo *pOIBindInfo,
                                    DWORD grfPI,
                                    HANDLE_PTR dwReserved) override;
    HRESULT STDMETHODCALLTYPE Suspend() override { return S_OK; }
    HRESULT STDMETHODCALLTYPE Terminate(DWORD WXUNUSED(dwOptions)) override { return S_OK; }

    //IInternetProtocol
    HRESULT STDMETHODCALLTYPE LockRequest(DWORD WXUNUSED(dwOptions)) override
                                          { return S_OK; }
    HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead) override;
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER WXUNUSED(dlibMove),
                                   DWORD WXUNUSED(dwOrigin),
                                   ULARGE_INTEGER* WXUNUSED(plibNewPosition)) override
                                   { return E_FAIL; }
    HRESULT STDMETHODCALLTYPE UnlockRequest() override { return S_OK; }

    //IInternetProtocolInfo
    HRESULT STDMETHODCALLTYPE CombineUrl(
            LPCWSTR pwzBaseUrl, LPCWSTR pwzRelativeUrl,
            DWORD dwCombineFlags, LPWSTR pwzResult,
            DWORD cchResult, DWORD *pcchResult,
            DWORD dwReserved) override;
    HRESULT STDMETHODCALLTYPE ParseUrl(
            LPCWSTR pwzUrl, wxPARSEACTION ParseAction,
            DWORD dwParseFlags, LPWSTR pwzResult,
            DWORD cchResult, DWORD *pcchResult,
            DWORD dwReserved) override;
    HRESULT STDMETHODCALLTYPE CompareUrl(
            LPCWSTR pwzUrl1,
            LPCWSTR pwzUrl2,
            DWORD dwCompareFlags) override;
    HRESULT STDMETHODCALLTYPE QueryInfo(
            LPCWSTR pwzUrl, wxQUERYOPTION OueryOption,
            DWORD dwQueryFlags, LPVOID pBuffer,
            DWORD cbBuffer, DWORD *pcbBuf,
            DWORD dwReserved) override;
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
                                             REFIID riid, void** ppvObject) override;
    HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) override;

    //IUnknown
    DECLARE_IUNKNOWN_METHODS;

private:
    wxSharedPtr<wxWebViewHandler> m_handler;
};

class wxIEContainer : public wxActiveXContainer
{
public:
    wxIEContainer(wxWindow *parent, REFIID iid, IUnknown *pUnk, DocHostUIHandler* uiHandler = nullptr);
    virtual ~wxIEContainer();
    virtual bool QueryClientSiteInterface(REFIID iid, void **_interface, const char *&desc) override;
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
                                              IDispatch *pdispReserved) override;

    virtual HRESULT wxSTDCALL GetHostInfo(DOCHOSTUIINFO *pInfo) override;

    virtual HRESULT wxSTDCALL ShowUI(DWORD dwID,
                                     IOleInPlaceActiveObject *pActiveObject,
                                     IOleCommandTarget *pCommandTarget,
                                     IOleInPlaceFrame *pFrame,
                                     IOleInPlaceUIWindow *pDoc) override;

    virtual HRESULT wxSTDCALL HideUI(void) override;

    virtual HRESULT wxSTDCALL UpdateUI(void) override;

    virtual HRESULT wxSTDCALL EnableModeless(BOOL fEnable) override;

    virtual HRESULT wxSTDCALL OnDocWindowActivate(BOOL fActivate) override;

    virtual HRESULT wxSTDCALL OnFrameWindowActivate(BOOL fActivate) override;

    virtual HRESULT wxSTDCALL ResizeBorder(LPCRECT prcBorder,
                                           IOleInPlaceUIWindow *pUIWindow,
                                           BOOL fRameWindow) override;

    virtual HRESULT wxSTDCALL TranslateAccelerator(LPMSG lpMsg,
                                                   const GUID *pguidCmdGroup,
                                                   DWORD nCmdID) override;

    virtual HRESULT wxSTDCALL GetOptionKeyPath(LPOLESTR *pchKey,
                                               DWORD dw) override;

    virtual HRESULT wxSTDCALL GetDropTarget(IDropTarget *pDropTarget,
                                            IDropTarget **ppDropTarget) override;

    virtual HRESULT wxSTDCALL GetExternal(IDispatch **ppDispatch) override;

    virtual HRESULT wxSTDCALL TranslateUrl(DWORD dwTranslate,
                                           OLECHAR *pchURLIn,
                                           OLECHAR **ppchURLOut) override;

    virtual HRESULT wxSTDCALL FilterDataObject(IDataObject *pDO,
                                               IDataObject **ppDORet) override;
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

#endif // wxWebViewIE_PRIVATE_H
