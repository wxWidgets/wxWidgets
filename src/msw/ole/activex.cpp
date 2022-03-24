/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/ole/activex.cpp
// Purpose:     wxActiveXContainer implementation
// Author:      Ryan Norton <wxprojects@comcast.net>, Lindsay Mathieson <???>
// Modified by:
// Created:     11/07/04
// Copyright:   (c) 2003 Lindsay Mathieson, (c) 2005 Ryan Norton
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#if wxUSE_ACTIVEX

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/math.h"
#endif

#include "wx/msw/dc.h"

#include "wx/msw/ole/activex.h"
#include "wx/msw/private.h" // for wxCopyRectToRECT

// autointerfaces that we only use here
typedef wxAutoOleInterface<IOleInPlaceSite> wxAutoIOleInPlaceSite;
typedef wxAutoOleInterface<IOleDocument> wxAutoIOleDocument;
typedef wxAutoOleInterface<IPersistStreamInit> wxAutoIPersistStreamInit;
typedef wxAutoOleInterface<IAdviseSink> wxAutoIAdviseSink;
typedef wxAutoOleInterface<IProvideClassInfo> wxAutoIProvideClassInfo;
typedef wxAutoOleInterface<ITypeInfo> wxAutoITypeInfo;
typedef wxAutoOleInterface<IConnectionPoint> wxAutoIConnectionPoint;
typedef wxAutoOleInterface<IConnectionPointContainer> wxAutoIConnectionPointContainer;

wxDEFINE_EVENT( wxEVT_ACTIVEX, wxActiveXEvent );

// Ole class helpers (sort of MFC-like) from wxActiveX
#define DECLARE_OLE_UNKNOWN(cls)\
    private:\
    class TAutoInitInt\
    {\
        public:\
        LONG l;\
        TAutoInitInt() : l(1) {}\
    };\
    TAutoInitInt refCount, lockCount;\
    static void _GetInterface(cls *self, REFIID iid, void **_interface, const char *&desc);\
    public:\
    LONG GetRefCount();\
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject) wxOVERRIDE;\
    ULONG STDMETHODCALLTYPE AddRef() wxOVERRIDE;\
    ULONG STDMETHODCALLTYPE Release() wxOVERRIDE;\
    ULONG STDMETHODCALLTYPE AddLock();\
    ULONG STDMETHODCALLTYPE ReleaseLock()

#define DEFINE_OLE_TABLE(cls)\
    LONG cls::GetRefCount() {return refCount.l;}\
    HRESULT STDMETHODCALLTYPE cls::QueryInterface(REFIID iid, void ** ppvObject)\
    {\
        if (! ppvObject)\
        {\
            return E_FAIL;\
        }\
        const char *desc = NULL;\
        cls::_GetInterface(this, iid, ppvObject, desc);\
        if (! *ppvObject)\
        {\
            return E_NOINTERFACE;\
        }\
        ((IUnknown * )(*ppvObject))->AddRef();\
        return S_OK;\
    }\
    ULONG STDMETHODCALLTYPE cls::AddRef()\
    {\
        InterlockedIncrement(&refCount.l);\
        return refCount.l;\
    }\
    ULONG STDMETHODCALLTYPE cls::Release()\
    {\
        if (refCount.l > 0)\
        {\
            InterlockedDecrement(&refCount.l);\
            if (refCount.l == 0)\
            {\
                delete this;\
                return 0;\
            }\
            return refCount.l;\
        }\
        else\
            return 0;\
    }\
    ULONG STDMETHODCALLTYPE cls::AddLock()\
    {\
        InterlockedIncrement(&lockCount.l);\
        return lockCount.l;\
    }\
    ULONG STDMETHODCALLTYPE cls::ReleaseLock()\
    {\
        if (lockCount.l > 0)\
        {\
            InterlockedDecrement(&lockCount.l);\
            return lockCount.l;\
        }\
        else\
            return 0;\
    }\
    DEFINE_OLE_BASE(cls)

#define DEFINE_OLE_BASE(cls)\
    void cls::_GetInterface(cls *self, REFIID iid, void **_interface, const char *&desc)\
    {\
        *_interface = NULL;\
        desc = NULL;

#define OLE_INTERFACE(_iid, _type)\
    if (IsEqualIID(iid, _iid))\
    {\
        *_interface = (IUnknown *) (_type *) self;\
        desc = # _iid;\
        return;\
    }

#define OLE_IINTERFACE(_face) OLE_INTERFACE(IID_##_face, _face)

#define OLE_INTERFACE_CUSTOM(func)\
    if (func(self, iid, _interface, desc))\
    {\
        return;\
    }

#define END_OLE_TABLE\
    }

// ============================================================================
// implementation
// ============================================================================

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PixelsToHimetric
//
// Utility to convert from pixels to the himetric values in some COM methods
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


static void PixelsToHimetric(SIZEL &sz)
{
    const wxSize logSz = wxGetDPIofHDC(ScreenHDC());

#define HIMETRIC_INCH   2540
#define CONVERT(x, logpixels)   wxMulDivInt32(HIMETRIC_INCH, (x), (logpixels))

    sz.cx = CONVERT(sz.cx, logSz.x);
    sz.cy = CONVERT(sz.cy, logSz.y);

#undef CONVERT
#undef HIMETRIC_INCH
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// FrameSite
//
// Handles the actual wxActiveX container implementation
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class FrameSite :
    public IOleClientSite,
    public IOleInPlaceSiteEx,
    public IOleInPlaceFrame,
    public IOleItemContainer,
    public IDispatch,
    public IOleCommandTarget,
    public IOleDocumentSite,
    public IAdviseSink,
    public IOleControlSite
{
private:
    DECLARE_OLE_UNKNOWN(FrameSite);

public:
    FrameSite(wxWindow * win, wxActiveXContainer * win2)
    {
        m_window = win2;
        m_bSupportsWindowlessActivation = true;
        m_bInPlaceLocked = false;
        m_bUIActive = false;
        m_bInPlaceActive = false;
        m_bWindowless = false;

        m_nAmbientLocale = 0;
        m_clrAmbientForeColor = ::GetSysColor(COLOR_WINDOWTEXT);
        m_clrAmbientBackColor = ::GetSysColor(COLOR_WINDOW);
        m_bAmbientShowHatching = true;
        m_bAmbientShowGrabHandles = true;
        m_bAmbientAppearance = true;

        m_hDCBuffer = NULL;
        m_hWndParent = (HWND)win->GetHWND();
    }
    virtual ~FrameSite(){}
    //***************************IDispatch*****************************
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, OLECHAR ** ,
                                            unsigned int , LCID ,
                                            DISPID * ) wxOVERRIDE
    {   return E_NOTIMPL;   }
    STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo **) wxOVERRIDE
    {   return E_NOTIMPL;   }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(unsigned int *) wxOVERRIDE
    {   return E_NOTIMPL;   }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID,
                            WORD wFlags, DISPPARAMS *,
                            VARIANT * pVarResult, EXCEPINFO *,
                            unsigned int *) wxOVERRIDE
    {
        if (!(wFlags & DISPATCH_PROPERTYGET))
            return S_OK;

        if (pVarResult == NULL)
            return E_INVALIDARG;

        //The most common case is boolean, use as an initial type
        V_VT(pVarResult) = VT_BOOL;

        switch (dispIdMember)
        {
            case DISPID_AMBIENT_MESSAGEREFLECT:
                V_BOOL(pVarResult)= VARIANT_FALSE;
                return S_OK;

            case DISPID_AMBIENT_DISPLAYASDEFAULT:
                V_BOOL(pVarResult)= VARIANT_TRUE;
                return S_OK;

            case DISPID_AMBIENT_OFFLINEIFNOTCONNECTED:
                V_BOOL(pVarResult) = VARIANT_TRUE;
                return S_OK;

            case DISPID_AMBIENT_SILENT:
                V_BOOL(pVarResult)= VARIANT_TRUE;
                return S_OK;

            case DISPID_AMBIENT_APPEARANCE:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_bAmbientAppearance ? VARIANT_TRUE : VARIANT_FALSE;
                break;

            case DISPID_AMBIENT_FORECOLOR:
                pVarResult->vt = VT_I4;
                pVarResult->lVal = (long) m_clrAmbientForeColor;
                break;

            case DISPID_AMBIENT_BACKCOLOR:
                pVarResult->vt = VT_I4;
                pVarResult->lVal = (long) m_clrAmbientBackColor;
                break;

            case DISPID_AMBIENT_LOCALEID:
                pVarResult->vt = VT_I4;
                pVarResult->lVal = (long) m_nAmbientLocale;
                break;

            case DISPID_AMBIENT_USERMODE:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_window->m_bAmbientUserMode ? VARIANT_TRUE : VARIANT_FALSE;
                break;

            case DISPID_AMBIENT_SHOWGRABHANDLES:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_bAmbientShowGrabHandles ? VARIANT_TRUE : VARIANT_FALSE;
                break;

            case DISPID_AMBIENT_SHOWHATCHING:
                pVarResult->vt = VT_BOOL;
                pVarResult->boolVal = m_bAmbientShowHatching ? VARIANT_TRUE : VARIANT_FALSE;
                break;

            default:
                return DISP_E_MEMBERNOTFOUND;
        }

        return S_OK;
    }

    //**************************IOleWindow***************************
    HRESULT STDMETHODCALLTYPE GetWindow(HWND * phwnd) wxOVERRIDE
    {
        if (phwnd == NULL)
            return E_INVALIDARG;
        (*phwnd) = m_hWndParent;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) wxOVERRIDE
    {return S_OK;}
    //**************************IOleInPlaceUIWindow*****************
    HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder) wxOVERRIDE
    {
        if (lprectBorder == NULL)
            return E_INVALIDARG;
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) wxOVERRIDE
    {
        if (pborderwidths == NULL)
            return E_INVALIDARG;
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS) wxOVERRIDE
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE SetActiveObject(
        IOleInPlaceActiveObject *pActiveObject, LPCOLESTR) wxOVERRIDE
    {
        if (pActiveObject)
            pActiveObject->AddRef();

        m_window->m_oleInPlaceActiveObject = pActiveObject;
        return S_OK;
    }

    //********************IOleInPlaceFrame************************

    STDMETHOD(InsertMenus)(HMENU, LPOLEMENUGROUPWIDTHS) wxOVERRIDE {return S_OK;}
    STDMETHOD(SetMenu)(HMENU, HOLEMENU, HWND) wxOVERRIDE {return S_OK;}
    STDMETHOD(RemoveMenus)(HMENU) wxOVERRIDE {return S_OK;}
    STDMETHOD(SetStatusText)(LPCOLESTR) wxOVERRIDE {return S_OK;}
    HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) wxOVERRIDE {return S_OK;}
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD) wxOVERRIDE
    {
        // TODO: send an event with this id
        if (m_window->m_oleInPlaceActiveObject.IsOk())
            m_window->m_oleInPlaceActiveObject->TranslateAccelerator(lpmsg);
        return S_FALSE;
    }

    //*******************IOleInPlaceSite**************************
    HRESULT STDMETHODCALLTYPE CanInPlaceActivate() wxOVERRIDE {return S_OK;}
    HRESULT STDMETHODCALLTYPE OnInPlaceActivate() wxOVERRIDE
    {   m_bInPlaceActive = true;    return S_OK;    }
    HRESULT STDMETHODCALLTYPE OnUIActivate() wxOVERRIDE
    {   m_bUIActive = true;         return S_OK;    }
    HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame,
                                        IOleInPlaceUIWindow **ppDoc,
                                        LPRECT lprcPosRect,
                                        LPRECT lprcClipRect,
                                        LPOLEINPLACEFRAMEINFO lpFrameInfo) wxOVERRIDE
    {
        if (ppFrame == NULL || ppDoc == NULL || lprcPosRect == NULL ||
            lprcClipRect == NULL || lpFrameInfo == NULL)
        {
            if (ppFrame != NULL)
                (*ppFrame) = NULL;
            if (ppDoc != NULL)
                (*ppDoc) = NULL;
            return E_INVALIDARG;
        }

        HRESULT hr = QueryInterface(IID_IOleInPlaceFrame, (void **) ppFrame);
        if (! SUCCEEDED(hr))
        {
            return E_UNEXPECTED;
        }

        hr = QueryInterface(IID_IOleInPlaceUIWindow, (void **) ppDoc);
        if (! SUCCEEDED(hr))
        {
            (*ppFrame)->Release();
            *ppFrame = NULL;
            return E_UNEXPECTED;
        }

        RECT rect;
        ::GetClientRect(m_hWndParent, &rect);
        if (lprcPosRect)
        {
            lprcPosRect->left = lprcPosRect->top = 0;
            lprcPosRect->right = rect.right;
            lprcPosRect->bottom = rect.bottom;
        }
        if (lprcClipRect)
        {
            lprcClipRect->left = lprcClipRect->top = 0;
            lprcClipRect->right = rect.right;
            lprcClipRect->bottom = rect.bottom;
        }

        memset(lpFrameInfo, 0, sizeof(OLEINPLACEFRAMEINFO));
        lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
        lpFrameInfo->hwndFrame = m_hWndParent;

        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Scroll(SIZE) wxOVERRIDE {return S_OK;}
    HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) wxOVERRIDE
    {   m_bUIActive = false;         return S_OK;    }
    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() wxOVERRIDE
    {   m_bInPlaceActive = false;    return S_OK;    }
    HRESULT STDMETHODCALLTYPE DiscardUndoState() wxOVERRIDE {return S_OK;}
    HRESULT STDMETHODCALLTYPE DeactivateAndUndo() wxOVERRIDE {return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect) wxOVERRIDE
    {
        if (m_window->m_oleInPlaceObject.IsOk() && lprcPosRect)
        {
           //
           // Result of several hours and days of bug hunting -
           // this is called by an object when it wants to resize
           // itself to something different from our parent window -
           // don't let it :)
           //
//            m_window->m_oleInPlaceObject->SetObjectRects(
//                lprcPosRect, lprcPosRect);
           RECT rcClient;
           ::GetClientRect(m_hWndParent, &rcClient);
            m_window->m_oleInPlaceObject->SetObjectRects(
                &rcClient, &rcClient);
        }
        return S_OK;
    }
    //*************************IOleInPlaceSiteEx***********************
    HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL * pfNoRedraw, DWORD) wxOVERRIDE
    {
        OleLockRunning(m_window->m_ActiveX, TRUE, FALSE);
        if (pfNoRedraw)
            (*pfNoRedraw) = FALSE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL) wxOVERRIDE
    {
        OleLockRunning(m_window->m_ActiveX, FALSE, FALSE);
        return S_OK;
    }
    STDMETHOD(RequestUIActivate)() wxOVERRIDE { return S_OK;}
    //*************************IOleClientSite**************************
    HRESULT STDMETHODCALLTYPE SaveObject() wxOVERRIDE {return S_OK;}
    const char *OleGetMonikerToStr(DWORD dwAssign)
    {
        switch (dwAssign)
        {
        case OLEGETMONIKER_ONLYIFTHERE  : return "OLEGETMONIKER_ONLYIFTHERE";
        case OLEGETMONIKER_FORCEASSIGN  : return "OLEGETMONIKER_FORCEASSIGN";
        case OLEGETMONIKER_UNASSIGN     : return "OLEGETMONIKER_UNASSIGN";
        case OLEGETMONIKER_TEMPFORUSER  : return "OLEGETMONIKER_TEMPFORUSER";
        default                         : return "Bad Enum";
        }
    }

    const char *OleGetWhicMonikerStr(DWORD dwWhichMoniker)
    {
        switch(dwWhichMoniker)
        {
        case OLEWHICHMK_CONTAINER   : return "OLEWHICHMK_CONTAINER";
        case OLEWHICHMK_OBJREL      : return "OLEWHICHMK_OBJREL";
        case OLEWHICHMK_OBJFULL     : return "OLEWHICHMK_OBJFULL";
        default                     : return "Bad Enum";
        }
    }
    STDMETHOD(GetMoniker)(DWORD, DWORD, IMoniker **) wxOVERRIDE {return E_FAIL;}
    HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER * ppContainer) wxOVERRIDE
    {
        if (ppContainer == NULL)
            return E_INVALIDARG;
        HRESULT hr = QueryInterface(
            IID_IOleContainer, (void**)(ppContainer));
        wxASSERT(SUCCEEDED(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE ShowObject() wxOVERRIDE
    {
        if (m_window->m_oleObjectHWND)
            ::ShowWindow(m_window->m_oleObjectHWND, SW_SHOW);
        return S_OK;
    }
    STDMETHOD(OnShowWindow)(BOOL) wxOVERRIDE {return S_OK;}
    STDMETHOD(RequestNewObjectLayout)() wxOVERRIDE {return E_NOTIMPL;}
    //********************IParseDisplayName***************************
    HRESULT STDMETHODCALLTYPE ParseDisplayName(
        IBindCtx *, LPOLESTR, ULONG *, IMoniker **) wxOVERRIDE {return E_NOTIMPL;}
    //********************IOleContainer*******************************
    STDMETHOD(EnumObjects)(DWORD, IEnumUnknown **) wxOVERRIDE {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE LockContainer(BOOL) wxOVERRIDE {return S_OK;}
    //********************IOleItemContainer***************************
    HRESULT STDMETHODCALLTYPE
    #if defined(_UNICODE)
    GetObjectW
    #else
    GetObjectA
    #endif
    (LPOLESTR pszItem, DWORD, IBindCtx *, REFIID, void ** ppvObject) wxOVERRIDE
    {
        if (pszItem == NULL || ppvObject == NULL)
            return E_INVALIDARG;
        *ppvObject = NULL;
        return MK_E_NOOBJECT;
    }
    HRESULT STDMETHODCALLTYPE GetObjectStorage(
        LPOLESTR pszItem, IBindCtx * , REFIID, void ** ppvStorage) wxOVERRIDE
    {
        if (pszItem == NULL || ppvStorage == NULL)
            return E_INVALIDARG;
        *ppvStorage = NULL;
        return MK_E_NOOBJECT;
    }
    HRESULT STDMETHODCALLTYPE IsRunning(LPOLESTR pszItem) wxOVERRIDE
    {
        if (pszItem == NULL)
            return E_INVALIDARG;
        return MK_E_NOOBJECT;
    }
    //***********************IOleControlSite*****************************
    HRESULT STDMETHODCALLTYPE OnControlInfoChanged() wxOVERRIDE
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE LockInPlaceActive(BOOL fLock) wxOVERRIDE
    {
        m_bInPlaceLocked = (fLock) ? true : false;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetExtendedControl(IDispatch **) wxOVERRIDE
    {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE TransformCoords(
        POINTL * pPtlHimetric, POINTF * pPtfContainer, DWORD) wxOVERRIDE
    {
        if (pPtlHimetric == NULL || pPtfContainer == NULL)
            return E_INVALIDARG;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG, DWORD) wxOVERRIDE
    {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE OnFocus(BOOL) wxOVERRIDE {return S_OK;}
    HRESULT STDMETHODCALLTYPE ShowPropertyFrame() wxOVERRIDE {return E_NOTIMPL;}
    //**************************IOleCommandTarget***********************
    HRESULT STDMETHODCALLTYPE QueryStatus(const GUID *, ULONG cCmds,
                                OLECMD prgCmds[], OLECMDTEXT *) wxOVERRIDE
    {
        if (prgCmds == NULL) return E_INVALIDARG;
        for (ULONG nCmd = 0; nCmd < cCmds; nCmd++)
        {
            // unsupported by default
            prgCmds[nCmd].cmdf = 0;
        }
        return OLECMDERR_E_UNKNOWNGROUP;
    }

    HRESULT STDMETHODCALLTYPE Exec(const GUID *, DWORD,
                            DWORD, VARIANTARG *, VARIANTARG *) wxOVERRIDE
    {return OLECMDERR_E_NOTSUPPORTED;}

    //**********************IAdviseSink************************************
    void STDMETHODCALLTYPE OnDataChange(FORMATETC *, STGMEDIUM *) wxOVERRIDE {}
    void STDMETHODCALLTYPE OnViewChange(DWORD, LONG) wxOVERRIDE {}
    void STDMETHODCALLTYPE OnRename(IMoniker *) wxOVERRIDE {}
    void STDMETHODCALLTYPE OnSave() wxOVERRIDE {}
    void STDMETHODCALLTYPE OnClose() wxOVERRIDE {}

    //**********************IOleDocumentSite***************************
    HRESULT STDMETHODCALLTYPE ActivateMe(
        IOleDocumentView __RPC_FAR *pViewToActivate) wxOVERRIDE
    {
        wxAutoIOleInPlaceSite inPlaceSite(
            IID_IOleInPlaceSite, (IDispatch *) this);
        if (!inPlaceSite.IsOk())
            return E_FAIL;

        if (pViewToActivate)
        {
            m_window->m_docView = pViewToActivate;
            m_window->m_docView->SetInPlaceSite(inPlaceSite);
        }
        else
        {
            wxAutoIOleDocument oleDoc(
                IID_IOleDocument, m_window->m_oleObject);
            if (! oleDoc.IsOk())
                return E_FAIL;

            HRESULT hr = oleDoc->CreateView(inPlaceSite, NULL,
                                    0, m_window->m_docView.GetRef());
            if (hr != S_OK)
                return E_FAIL;

            m_window->m_docView->SetInPlaceSite(inPlaceSite);
        }

        m_window->m_docView->UIActivate(TRUE);
        return S_OK;
    }

    friend bool QueryClientSiteInterface(FrameSite *self, REFIID iid, void **_interface, const char *&desc)
    {
        return self->m_window->QueryClientSiteInterface(iid,_interface,desc);
    }

protected:
    wxActiveXContainer * m_window;

    HDC m_hDCBuffer;
    HWND m_hWndParent;

    bool m_bSupportsWindowlessActivation;
    bool m_bInPlaceLocked;
    bool m_bInPlaceActive;
    bool m_bUIActive;
    bool m_bWindowless;

    LCID m_nAmbientLocale;
    COLORREF m_clrAmbientForeColor;
    COLORREF m_clrAmbientBackColor;
    bool m_bAmbientShowHatching;
    bool m_bAmbientShowGrabHandles;
    bool m_bAmbientAppearance;
};

DEFINE_OLE_TABLE(FrameSite)
    OLE_INTERFACE(IID_IUnknown, IOleClientSite)
    OLE_IINTERFACE(IOleClientSite)
    OLE_INTERFACE(IID_IOleWindow, IOleInPlaceSite)
    OLE_IINTERFACE(IOleInPlaceSite)
    OLE_IINTERFACE(IOleInPlaceSiteEx)
    OLE_IINTERFACE(IOleInPlaceUIWindow)
    OLE_IINTERFACE(IOleInPlaceFrame)
    OLE_IINTERFACE(IParseDisplayName)
    OLE_IINTERFACE(IOleContainer)
    OLE_IINTERFACE(IOleItemContainer)
    OLE_IINTERFACE(IDispatch)
    OLE_IINTERFACE(IOleCommandTarget)
    OLE_IINTERFACE(IOleDocumentSite)
    OLE_IINTERFACE(IAdviseSink)
    OLE_IINTERFACE(IOleControlSite)
    OLE_INTERFACE_CUSTOM(QueryClientSiteInterface)
END_OLE_TABLE


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxActiveXEvents
//
// Handles and sends activex events received from the ActiveX control
// to the appropriate wxEvtHandler
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class wxActiveXEvents : public IDispatch
{
private:
    DECLARE_OLE_UNKNOWN(wxActiveXEvents);


    wxActiveXContainer *m_activeX;
    IID m_customId;
    bool m_haveCustomId;

    friend bool wxActiveXEventsInterface(wxActiveXEvents *self, REFIID iid, void **_interface, const char *&desc);

public:

    // a pointer to this static variable is used as an 'invalid_entry_marker'
    // wxVariants containing a void* to this variables are 'empty' in the sense
    // that the actual ActiveX OLE parameter has not been converted and inserted
    // into m_params.
    static wxVariant ms_invalidEntryMarker;

    wxActiveXEvents(wxActiveXContainer *ax) : m_activeX(ax), m_haveCustomId(false) {}
    wxActiveXEvents(wxActiveXContainer *ax, REFIID iid) : m_activeX(ax), m_customId(iid), m_haveCustomId(true) {}
    virtual ~wxActiveXEvents()
    {
    }

    // IDispatch
    STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR**, unsigned int, LCID, DISPID*) wxOVERRIDE
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP GetTypeInfo(unsigned int, LCID, ITypeInfo**) wxOVERRIDE
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP GetTypeInfoCount(unsigned int*) wxOVERRIDE
    {
        return E_NOTIMPL;
    }


    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid,
                        LCID lcid,
                          WORD wFlags, DISPPARAMS * pDispParams,
                          VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
                          unsigned int * puArgErr) wxOVERRIDE
    {
        if (wFlags & (DISPATCH_PROPERTYGET | DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF))
            return E_NOTIMPL;

        wxASSERT(m_activeX);

        // ActiveX Event

        // Dispatch Event
        wxActiveXEvent  event;
        event.SetEventType(wxEVT_ACTIVEX);
        // Create an empty list of Variants
        // Note that the event parameters use lazy evaluation
        // They are not actually created until wxActiveXEvent::operator[] is called
        event.m_params.NullList();
        event.m_dispid = dispIdMember;

        // save the native (MSW) event parameters for event handlers that need to access them
        // this can be done on the stack since wxActiveXEvent is also allocated on the stack
        wxActiveXEventNativeMSW eventParameters(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
        event.SetClientData(&eventParameters);

        // The event parameters are not copied to event.m_params until they are actually
        // referenced in wxActiveXEvent::operator[]
        // This increases performance and avoids error messages and/or crashes
        // when the event has parameters that are not (yet or never) supported
        // by wxConvertOleToVariant

        // process the events from the activex method
        m_activeX->ProcessEvent(event);
        for (DWORD i = 0; i < pDispParams->cArgs; i++)
        {
            size_t params_index = pDispParams->cArgs - i - 1;
            if (params_index < event.m_params.GetCount()) {
                wxVariant &vx = event.m_params[params_index];
                // copy the result back to pDispParams only if the event has been accessed
                //  i.e.  if vx != ms_invalidEntryMarker
                if (!vx.IsType(wxActiveXEvents::ms_invalidEntryMarker.GetType()) || vx!=ms_invalidEntryMarker) {
                    VARIANTARG& va = pDispParams->rgvarg[i];
                    wxConvertVariantToOle(vx, va);
                }
            }
        }

        if(event.GetSkipped())
            return DISP_E_MEMBERNOTFOUND;

        return S_OK;
    }
};

namespace
{
// just a unique global variable
const int invalid_entry_marker = 0;
}

wxVariant wxActiveXEvents::ms_invalidEntryMarker(const_cast<void*>(static_cast<const void*>(&invalid_entry_marker)));

size_t wxActiveXEvent::ParamCount() const
{
    wxActiveXEventNativeMSW *native=GetNativeParameters();
    // 'native' will always be != if the event has been created
    // for an actual active X event.
    // But it may be zero if the event has been created by wx program code.
    if (native)
        return native->pDispParams ? native->pDispParams->cArgs : 0;

    return m_params.GetCount();
}

wxVariant &wxActiveXEvent::operator [](size_t idx)
{
    wxASSERT(idx < ParamCount());
    wxActiveXEventNativeMSW *native=GetNativeParameters();
    // 'native' will always be != if the event has been created
    // for an actual active X event.
    // But it may be zero if the event has been created by wx program code.
    if (native)
    {
        while ( m_params.GetCount()<=idx )
        {
            m_params.Append(wxActiveXEvents::ms_invalidEntryMarker);
        }

        wxVariant& vx = m_params[idx];
        if ( vx.IsType(wxActiveXEvents::ms_invalidEntryMarker.GetType()) &&
                vx == wxActiveXEvents::ms_invalidEntryMarker)
        {
            // copy the _real_ parameter into this one
            // NOTE: m_params stores the parameters in *reverse* order.
            // Whyever, but this was the case in the original implementation of
            // wxActiveXEvents::Invoke
            // Keep this convention.
            VARIANTARG& va = native->pDispParams->rgvarg[ native->pDispParams->cArgs - idx - 1 ];
            wxConvertOleToVariant(va, vx);
        }
        return vx;
    }
    return m_params[idx];
}

bool wxActiveXEventsInterface(wxActiveXEvents *self, REFIID iid, void **_interface, const char *&desc)
{
    if (self->m_haveCustomId && IsEqualIID(iid, self->m_customId))
    {
        *_interface = (IUnknown *) (IDispatch *) self;
        desc = "Custom Dispatch Interface";
        return true;
    }

    return false;
}

DEFINE_OLE_TABLE(wxActiveXEvents)
    OLE_IINTERFACE(IUnknown)
    OLE_INTERFACE(IID_IDispatch, IDispatch)
    OLE_INTERFACE_CUSTOM(wxActiveXEventsInterface)
END_OLE_TABLE

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// wxActiveXContainer
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// wxActiveXContainer Constructor
//
// Initializes members and creates the native ActiveX container
//---------------------------------------------------------------------------
wxActiveXContainer::wxActiveXContainer(wxWindow * parent,
                                       REFIID iid, IUnknown* pUnk)
    : m_realparent(parent)
{
    m_bAmbientUserMode = true;
    m_docAdviseCookie = 0;
    CreateActiveX(iid, pUnk);
}

//---------------------------------------------------------------------------
// wxActiveXContainer Destructor
//
// Destroys members (the FrameSite et al. are destroyed implicitly
// through COM ref counting)
//---------------------------------------------------------------------------
wxActiveXContainer::~wxActiveXContainer()
{
    // disconnect connection points
    if (m_oleInPlaceObject.IsOk())
    {
        m_oleInPlaceObject->InPlaceDeactivate();
        m_oleInPlaceObject->UIDeactivate();
    }

    if (m_oleObject.IsOk())
    {
        if (m_docAdviseCookie != 0)
            m_oleObject->Unadvise(m_docAdviseCookie);

        m_oleObject->DoVerb(
            OLEIVERB_HIDE, NULL, m_clientSite, 0, (HWND) GetHWND(), NULL);
        m_oleObject->Close(OLECLOSE_NOSAVE);
        m_oleObject->SetClientSite(NULL);
    }

    // m_clientSite uses m_frameSite so destroy it first
    m_clientSite.Free();
    delete m_frameSite;

    // our window doesn't belong to us, don't destroy it
    m_hWnd = NULL;
}

// VZ: we might want to really report an error instead of just asserting here
#if wxDEBUG_LEVEL
    #define CHECK_HR(hr) \
        wxASSERT_LEVEL_2_MSG( SUCCEEDED(hr), \
                wxString::Format("HRESULT = %X", (unsigned)(hr)) )
#else
    #define CHECK_HR(hr) wxUnusedVar(hr)
#endif

//---------------------------------------------------------------------------
// wxActiveXContainer::CreateActiveX
//
// Actually creates the ActiveX container through the FrameSite
// and sets up ActiveX events
//
// TODO: Document this more
//---------------------------------------------------------------------------
void wxActiveXContainer::CreateActiveX(REFIID iid, IUnknown* pUnk)
{
    HRESULT hret;
    hret = m_ActiveX.QueryInterface(iid, pUnk);
    CHECK_HR(hret);

    // FrameSite
    m_frameSite = new FrameSite(m_realparent, this);
    // oleClientSite
    hret = m_clientSite.QueryInterface(
        IID_IOleClientSite, (IDispatch *) m_frameSite);
    CHECK_HR(hret);
    // adviseSink
    wxAutoIAdviseSink adviseSink(IID_IAdviseSink, (IDispatch *) m_frameSite);
    wxASSERT(adviseSink.IsOk());

    // Get Dispatch interface
    hret = m_Dispatch.QueryInterface(IID_IDispatch, m_ActiveX);
    CHECK_HR(hret);

    //
    // SETUP TYPEINFO AND ACTIVEX EVENTS
    //

    // get type info via class info
    wxAutoIProvideClassInfo classInfo(IID_IProvideClassInfo, m_ActiveX);
    wxASSERT(classInfo.IsOk());

    // type info
    wxAutoITypeInfo typeInfo;
    hret = classInfo->GetClassInfo(typeInfo.GetRef());
    CHECK_HR(hret);
    wxASSERT(typeInfo.IsOk());

    // TYPEATTR
    TYPEATTR *ta = NULL;
    hret = typeInfo->GetTypeAttr(&ta);
    CHECK_HR(hret);

    // this should be a TKIND_COCLASS
    wxASSERT(ta->typekind == TKIND_COCLASS);

    // iterate contained interfaces
    for (int i = 0; i < ta->cImplTypes; i++)
    {
        HREFTYPE rt = 0;

        // get dispatch type info handle
        hret = typeInfo->GetRefTypeOfImplType(i, &rt);
        if (! SUCCEEDED(hret))
            continue;

        // get dispatch type info interface
        wxAutoITypeInfo  ti;
        hret = typeInfo->GetRefTypeInfo(rt, ti.GetRef());
        if (! ti.IsOk())
            continue;

        CHECK_HR(hret);

        // check if default event sink
        bool defEventSink = false;
        int impTypeFlags = 0;
        typeInfo->GetImplTypeFlags(i, &impTypeFlags);

        if (impTypeFlags & IMPLTYPEFLAG_FDEFAULT)
        {
            if (impTypeFlags & IMPLTYPEFLAG_FSOURCE)
            {
                // WXOLE_TRACEOUT("Default Event Sink");
                defEventSink = true;
                if (impTypeFlags & IMPLTYPEFLAG_FDEFAULTVTABLE)
                {
                    // WXOLE_TRACEOUT("*ERROR* - Default Event Sink is via vTable");
                    defEventSink = false;
                    wxFAIL_MSG(wxT("Default event sink is in vtable!"));
                }
            }
        }


        // wxAutoOleInterface<> assumes a ref has already been added
        // TYPEATTR
        TYPEATTR *ta2 = NULL;
        hret = ti->GetTypeAttr(&ta2);
        CHECK_HR(hret);

        if (ta2->typekind == TKIND_DISPATCH)
        {
            // WXOLE_TRACEOUT("GUID = " << GetIIDName(ta2->guid).c_str());
            if (defEventSink)
            {
                wxAutoIConnectionPoint    cp;
                DWORD                    adviseCookie = 0;

                wxAutoIConnectionPointContainer cpContainer(IID_IConnectionPointContainer, m_ActiveX);
                wxASSERT( cpContainer.IsOk());

                hret = cpContainer->FindConnectionPoint(ta2->guid, cp.GetRef());

                // Notice that the return value of CONNECT_E_NOCONNECTION is
                // expected if the interface doesn't support connection points.
                if ( hret != CONNECT_E_NOCONNECTION )
                {
                    CHECK_HR(hret);
                }

                if ( cp )
                {
                    wxActiveXEvents * const
                        events = new wxActiveXEvents(this, ta2->guid);
                    hret = cp->Advise(events, &adviseCookie);

                    // We don't need this object any more and cp will keep a
                    // reference to it if it needs it, i.e. if Advise()
                    // succeeded.
                    events->Release();

                    CHECK_HR(hret);
                }
            }
        }

        ti->ReleaseTypeAttr(ta2);
    }

    // free
    typeInfo->ReleaseTypeAttr(ta);

    //
    // END
    //

    // Get IOleObject interface
    hret = m_oleObject.QueryInterface(IID_IOleObject, m_ActiveX);
    CHECK_HR(hret);

    // get IViewObject Interface
    hret = m_viewObject.QueryInterface(IID_IViewObject, m_ActiveX);
    CHECK_HR(hret);

    // document advise
    m_docAdviseCookie = 0;
    hret = m_oleObject->Advise(adviseSink, &m_docAdviseCookie);
    CHECK_HR(hret);

    // TODO:Needed?
//    hret = m_viewObject->SetAdvise(DVASPECT_CONTENT, 0, adviseSink);
    m_oleObject->SetHostNames(L"wxActiveXContainer", NULL);
    OleSetContainedObject(m_oleObject, TRUE);
    OleRun(m_oleObject);


    // Get IOleInPlaceObject interface
    hret = m_oleInPlaceObject.QueryInterface(
        IID_IOleInPlaceObject, m_ActiveX);
    CHECK_HR(hret);

    // status
    DWORD dwMiscStatus;
    m_oleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    CHECK_HR(hret);

    // set client site first ?
    if (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
        m_oleObject->SetClientSite(m_clientSite);


    // stream init
    wxAutoIPersistStreamInit
        pPersistStreamInit(IID_IPersistStreamInit, m_oleObject);

    if (pPersistStreamInit.IsOk())
    {
        hret = pPersistStreamInit->InitNew();
        CHECK_HR(hret);
    }

    if (! (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
        m_oleObject->SetClientSite(m_clientSite);


    m_oleObjectHWND = 0;


    if (! (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME))
    {
        RECT posRect;
        wxCopyRectToRECT(m_realparent->GetClientSize(), posRect);

        hret = m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL,
            m_clientSite, 0, (HWND)m_realparent->GetHWND(), &posRect);
        CHECK_HR(hret);

        if (m_oleInPlaceObject.IsOk())
        {
            hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
            CHECK_HR(hret);
            ::SetActiveWindow(m_oleObjectHWND);
        }

        if (posRect.right > 0 && posRect.bottom > 0 &&
            m_oleInPlaceObject.IsOk())
        {
            m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);
        }

        hret = m_oleObject->DoVerb(OLEIVERB_SHOW, 0, m_clientSite, 0,
            (HWND)m_realparent->GetHWND(), &posRect);
        CHECK_HR(hret);
    }

    if (! m_oleObjectHWND && m_oleInPlaceObject.IsOk())
    {
        hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
        CHECK_HR(hret);
    }

    if (m_oleObjectHWND)
    {
        ::SetActiveWindow(m_oleObjectHWND);
        ::ShowWindow(m_oleObjectHWND, SW_SHOW);

        this->AssociateHandle(m_oleObjectHWND);
        this->Reparent(m_realparent);

        wxWindow* pWnd = m_realparent;
        int id = m_realparent->GetId();

        pWnd->Bind(wxEVT_SIZE, &wxActiveXContainer::OnSize, this, id);
        pWnd->Bind(wxEVT_SET_FOCUS, &wxActiveXContainer::OnSetFocus, this, id);
        pWnd->Bind(wxEVT_KILL_FOCUS, &wxActiveXContainer::OnKillFocus, this, id);
    }
}

//---------------------------------------------------------------------------
// wxActiveXContainer::OnSize
//
// Called when the parent is resized - we need to do this to actually
// move the ActiveX control to where the parent is
//---------------------------------------------------------------------------
void wxActiveXContainer::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetParent()->GetClientSize(&w, &h);

    RECT posRect;
    posRect.left = 0;
    posRect.top = 0;
    posRect.right = w;
    posRect.bottom = h;

    if (w <= 0 || h <= 0)
        return;

    // extents are in HIMETRIC units
    if (m_oleObject.IsOk())
    {

        SIZEL sz = {w, h};
        PixelsToHimetric(sz);

        SIZEL sz2;

        m_oleObject->GetExtent(DVASPECT_CONTENT, &sz2);
        if (sz2.cx !=  sz.cx || sz.cy != sz2.cy)
            m_oleObject->SetExtent(DVASPECT_CONTENT, &sz);

    }

    if (m_oleInPlaceObject.IsOk())
        m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);

    event.Skip();
}

//---------------------------------------------------------------------------
// wxActiveXContainer::OnPaint
//
// Called when the parent is resized - repaints the ActiveX control
//---------------------------------------------------------------------------
void wxActiveXContainer::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    // Draw only when control is windowless or deactivated
    if (m_viewObject)
    {
        int w, h;
        GetParent()->GetSize(&w, &h);
        RECT posRect;
        posRect.left = 0;
        posRect.top = 0;
        posRect.right = w;
        posRect.bottom = h;

        ::RedrawWindow(m_oleObjectHWND, NULL, NULL, RDW_INTERNALPAINT);
        RECTL *prcBounds = (RECTL *) &posRect;
        wxMSWDCImpl *msw = wxDynamicCast( dc.GetImpl() , wxMSWDCImpl );
        m_viewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL,
            (HDC)msw->GetHDC(), prcBounds, NULL, NULL, 0);
    }
}

//---------------------------------------------------------------------------
// wxActiveXContainer::OnSetFocus
//
// Called when the focus is set on the parent - activates the activex control
//---------------------------------------------------------------------------
void wxActiveXContainer::OnSetFocus(wxFocusEvent& event)
{
    if (m_oleInPlaceActiveObject.IsOk())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(TRUE);

    event.Skip();
}

//---------------------------------------------------------------------------
// wxActiveXContainer::OnKillFocus
//
// Called when the focus is killed on the parent -
// deactivates the activex control
//---------------------------------------------------------------------------
void wxActiveXContainer::OnKillFocus(wxFocusEvent& event)
{
    if (m_oleInPlaceActiveObject.IsOk())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(FALSE);

    event.Skip();
}

//---------------------------------------------------------------------------
// wxActiveXContainer::MSWTranslateMessage
//
// Called for every message that needs to be translated.
// Some controls might need more keyboard keys to process (CTRL-C, CTRL-A etc),
// In that case TranslateAccelerator should always be called first.
//---------------------------------------------------------------------------
bool wxActiveXContainer::MSWTranslateMessage(WXMSG* pMsg)
{
    if(m_oleInPlaceActiveObject.IsOk() && m_oleInPlaceActiveObject->TranslateAccelerator(pMsg) == S_OK)
    {
        return true;
    }
    return wxWindow::MSWTranslateMessage(pMsg);
}

//---------------------------------------------------------------------------
// wxActiveXContainer::QueryClientSiteInterface
//
// Called in the host's site's query method for other interfaces.
//---------------------------------------------------------------------------
bool wxActiveXContainer::QueryClientSiteInterface(REFIID iid, void **_interface, const char *&desc)
{
    wxUnusedVar(iid);
    wxUnusedVar(_interface);
    wxUnusedVar(desc);
    return false;
}

#endif // wxUSE_ACTIVEX
