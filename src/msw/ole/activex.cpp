/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/ole/activex.cpp
// Purpose:     wxActiveXContainer implementation
// Author:      Ryan Norton <wxprojects@comcast.net>, Lindsay Mathieson <???>
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
#include "wx/private/windowlifetime.h"
#include "wx/weakref.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
#endif

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

// This is intentionally not exposed as a public wx API. ActiveX teardown
// needs the map-only half of DissociateHandle() for an ABA-recycled HWND: the
// foreign window procedure must never be restored from our stale instance.
extern void wxRemoveHandleAssociation(wxWindowMSW *win);

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

namespace
{

enum class wxActiveXLifetimePhase
{
    Creating,
    Active,
    Destroying,
    Dead
};

// Keep both sides of an event connection alive until Unadvise() has returned.
// In particular, the ActiveX callback is allowed to delete its wx container,
// and that deletion is allowed to enter this object again from Unadvise().
class wxActiveXEventConnection final
{
public:
    wxActiveXEventConnection(IConnectionPoint *point, IDispatch *sink)
        : m_point(point),
          m_sink(sink)
    {
        wxASSERT(m_point);
        wxASSERT(m_sink);
        m_point->AddRef();
        m_sink->AddRef();
    }

    ~wxActiveXEventConnection()
    {
        Disconnect();
    }

    void BeginAdvise()
    {
        m_advising = true;
    }

    void CompleteAdvise(HRESULT hr, DWORD cookie)
    {
        m_advising = false;
        if ( cookie )
            m_cookie = cookie;

        // A failed Advise() is not supposed to publish a cookie, but some
        // servers do. Once observed, it is ours to revoke exactly once.
        if ( FAILED(hr) || !cookie )
            m_disconnectRequested = true;

        if ( m_disconnectRequested )
            DisconnectNow();
    }

    void Disconnect()
    {
        if ( m_advising )
        {
            // Advise() can synchronously call the sink, whose wx handler can
            // tear down the container. The cookie isn't available until that
            // Advise() returns, so let CompleteAdvise() finish the rollback.
            m_disconnectRequested = true;
            return;
        }

        DisconnectNow();
    }

private:
    void DisconnectNow()
    {
        IConnectionPoint * const point = m_point;
        IDispatch * const sink = m_sink;
        const DWORD cookie = m_cookie;

        // Make recursive teardown a strict no-op before crossing COM again.
        m_point = nullptr;
        m_sink = nullptr;
        m_cookie = 0;
        m_disconnectRequested = false;

        if ( point && cookie )
            point->Unadvise(cookie);
        if ( sink )
            sink->Release();
        if ( point )
            point->Release();
    }

    IConnectionPoint *m_point;
    IDispatch *m_sink;
    DWORD m_cookie = 0;
    bool m_advising = false;
    bool m_disconnectRequested = false;

    wxDECLARE_NO_COPY_CLASS(wxActiveXEventConnection);
};

// IOleObject::Advise() has the same synchronous-callback window as connection
// point Advise(), but a different owner for Unadvise(). Keep it transactional
// separately so every successful cookie is revoked exactly once.
class wxActiveXDocumentConnection final
{
public:
    wxActiveXDocumentConnection(IOleObject *object, IAdviseSink *sink)
        : m_object(object),
          m_sink(sink)
    {
        wxASSERT(m_object);
        wxASSERT(m_sink);
        m_object->AddRef();
        m_sink->AddRef();
    }

    ~wxActiveXDocumentConnection()
    {
        Disconnect();
    }

    void BeginAdvise()
    {
        m_advising = true;
    }

    void CompleteAdvise(HRESULT hr, DWORD cookie)
    {
        m_advising = false;
        if ( cookie )
            m_cookie = cookie;

        if ( FAILED(hr) || !cookie )
            m_disconnectRequested = true;

        if ( m_disconnectRequested )
            DisconnectNow();
    }

    DWORD GetCookie() const
    {
        return m_cookie;
    }

    void Disconnect()
    {
        if ( m_advising )
        {
            m_disconnectRequested = true;
            return;
        }

        DisconnectNow();
    }

private:
    void DisconnectNow()
    {
        IOleObject * const object = m_object;
        IAdviseSink * const sink = m_sink;
        const DWORD cookie = m_cookie;

        m_object = nullptr;
        m_sink = nullptr;
        m_cookie = 0;
        m_disconnectRequested = false;

        if ( object && cookie )
            object->Unadvise(cookie);
        if ( sink )
            sink->Release();
        if ( object )
            object->Release();
    }

    IOleObject *m_object;
    IAdviseSink *m_sink;
    DWORD m_cookie = 0;
    bool m_advising = false;
    bool m_disconnectRequested = false;

    wxDECLARE_NO_COPY_CLASS(wxActiveXDocumentConnection);
};

class wxActiveXLifetimeState final
{
public:
    wxActiveXLifetimeState(wxActiveXContainer *container,
                           wxWindow *parent,
                           std::uint64_t generation_)
        : owner(container),
          parentWindow(parent),
          ownerThread(::GetCurrentThreadId()),
          generation(generation_)
    {
        if ( parent )
        {
            parentHwnd = GetHwndOf(parent);
#if defined(__WXWINUI__) && wxUSE_WINUI3
            parentHwndGeneration =
                wxWinUIMSWGetHwndGeneration(parent, parentHwnd);
#endif
        }
    }

    bool IsOwnerThread() const
    {
        return ::GetCurrentThreadId() == ownerThread;
    }

    wxActiveXContainer *GetSiteTarget() const
    {
        if ( !IsOwnerThread() ||
             (phase != wxActiveXLifetimePhase::Creating &&
              phase != wxActiveXLifetimePhase::Active) )
            return nullptr;

        wxActiveXContainer * const target =
            static_cast<wxActiveXContainer *>(owner.get());
        return wxWindowIsUnavailableForCallbacks(target)
            ? nullptr
            : target;
    }

    wxActiveXContainer *GetEventTarget() const
    {
        if ( !IsOwnerThread() || !acceptEvents ||
             (phase != wxActiveXLifetimePhase::Active &&
              !(phase == wxActiveXLifetimePhase::Creating &&
                creationInProgress)) )
            return nullptr;

        wxActiveXContainer * const target =
            static_cast<wxActiveXContainer *>(owner.get());
        return wxWindowIsUnavailableForCallbacks(target)
            ? nullptr
            : target;
    }

    HWND GetParentHwndIfCurrent() const
    {
        if ( !IsOwnerThread() )
            return nullptr;

        wxWindow * const parent = parentWindow.get();
        if ( !parent || wxWindowIsUnavailableForCallbacks(parent) ||
             !parentHwnd || !::IsWindow(parentHwnd) ||
             GetHwndOf(parent) != parentHwnd ||
             wxFindWinFromHandle(parentHwnd) != parent )
        {
            return nullptr;
        }

#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( !parentHwndGeneration ||
             wxWinUIMSWGetHwndGeneration(parent, parentHwnd) !=
                parentHwndGeneration )
        {
            return nullptr;
        }
#endif

        return parentHwnd;
    }

    wxWeakRef<wxWindow> owner;
    wxWeakRef<wxWindow> parentWindow;
    const DWORD ownerThread;
    const std::uint64_t generation;
    wxActiveXLifetimePhase phase = wxActiveXLifetimePhase::Creating;
    bool creationInProgress = false;
    bool acceptEvents = false;
    bool parentEventsBound = false;
    int parentEventId = wxID_ANY;
    HWND parentHwnd = nullptr;
    HWND associatedHwnd = nullptr;
    std::uint64_t hwndBindingGeneration = 0;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    unsigned long long parentHwndGeneration = 0;
    unsigned long long associatedHwndGeneration = 0;
    unsigned long long associatedNativeGeneration = 0;
#endif
    std::vector<std::shared_ptr<wxActiveXEventConnection>> eventConnections;
    std::shared_ptr<wxActiveXDocumentConnection> documentConnection;
};

struct wxActiveXStateRegistry
{
    std::mutex mutex;
    std::map<wxActiveXContainer *,
             std::shared_ptr<wxActiveXLifetimeState>> states;
    std::uint64_t nextGeneration = 0;
};

wxActiveXStateRegistry& GetActiveXStateRegistry()
{
    // wx objects may be owned by static application objects. Avoid a registry
    // destruction-order dependency during process shutdown.
    static wxActiveXStateRegistry * const registry =
        new wxActiveXStateRegistry;
    return *registry;
}

std::shared_ptr<wxActiveXLifetimeState>
RegisterActiveXState(wxActiveXContainer *container, wxWindow *parent)
{
    wxActiveXStateRegistry& registry = GetActiveXStateRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    if ( ++registry.nextGeneration == 0 )
        ++registry.nextGeneration;

    const auto state = std::make_shared<wxActiveXLifetimeState>(
        container, parent, registry.nextGeneration);
    registry.states[container] = state;
    return state;
}

std::shared_ptr<wxActiveXLifetimeState>
FindActiveXState(wxActiveXContainer *container)
{
    wxActiveXStateRegistry& registry = GetActiveXStateRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    const auto it = registry.states.find(container);
    return it == registry.states.end() ? nullptr : it->second;
}

wxActiveXContainer *GetCurrentCreatingActiveXOwner(
    wxActiveXContainer *identity,
    const std::shared_ptr<wxActiveXLifetimeState>& expected,
    std::uint64_t expectedGeneration)
{
    if ( !expected || !expected->IsOwnerThread() ||
         expected->generation != expectedGeneration ||
         expected->phase != wxActiveXLifetimePhase::Creating ||
         !expected->creationInProgress ||
         expected->owner.get() != identity ||
         wxWindowIsUnavailableForCallbacks(identity) )
    {
        return nullptr;
    }

    wxActiveXStateRegistry& registry = GetActiveXStateRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    const auto it = registry.states.find(identity);
    if ( it == registry.states.end() || it->second != expected ||
         it->second->generation != expectedGeneration )
    {
        return nullptr;
    }

    return identity;
}

void EraseActiveXState(
    wxActiveXContainer *container,
    const std::shared_ptr<wxActiveXLifetimeState>& expected)
{
    wxActiveXStateRegistry& registry = GetActiveXStateRegistry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    const auto it = registry.states.find(container);
    if ( it != registry.states.end() && it->second == expected )
        registry.states.erase(it);
}

} // anonymous namespace

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
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject) override;\
    ULONG STDMETHODCALLTYPE AddRef() override;\
    ULONG STDMETHODCALLTYPE Release() override;\
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
        const char *desc = nullptr;\
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
        return static_cast<ULONG>(InterlockedIncrement(&refCount.l));\
    }\
    ULONG STDMETHODCALLTYPE cls::Release()\
    {\
        const LONG refs = InterlockedDecrement(&refCount.l);\
        wxASSERT_MSG(refs >= 0, "unbalanced COM Release()");\
        if (refs == 0)\
        {\
            delete this;\
            return 0;\
        }\
        return refs > 0 ? static_cast<ULONG>(refs) : 0;\
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
        *_interface = nullptr;\
        desc = nullptr;

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
    FrameSite(wxWindow * win,
              const std::shared_ptr<wxActiveXLifetimeState>& state)
        : m_state(state)
    {
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

        m_hDCBuffer = nullptr;
        wxUnusedVar(win);
    }
    virtual ~FrameSite(){}
    //***************************IDispatch*****************************
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, OLECHAR ** ,
                                            unsigned int , LCID ,
                                            DISPID * ) override
    {   return E_NOTIMPL;   }
    STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo **) override
    {   return E_NOTIMPL;   }
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(unsigned int *) override
    {   return E_NOTIMPL;   }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID, LCID,
                            WORD wFlags, DISPPARAMS *,
                            VARIANT * pVarResult, EXCEPINFO *,
                            unsigned int *) override
    {
        const auto state = m_state.lock();
        wxActiveXContainer * const window =
            state ? state->GetSiteTarget() : nullptr;
        if ( !window )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        if (!(wFlags & DISPATCH_PROPERTYGET))
            return S_OK;

        if (pVarResult == nullptr)
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
                pVarResult->boolVal = window->m_bAmbientUserMode ? VARIANT_TRUE : VARIANT_FALSE;
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
    HRESULT STDMETHODCALLTYPE GetWindow(HWND * phwnd) override
    {
        if (phwnd == nullptr)
            return E_INVALIDARG;

        const auto state = m_state.lock();
        const HWND parent = state ? state->GetParentHwndIfCurrent() : nullptr;
        if ( !parent )
        {
            *phwnd = nullptr;
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        (*phwnd) = parent;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override
    {return S_OK;}
    //**************************IOleInPlaceUIWindow*****************
    HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder) override
    {
        if (lprectBorder == nullptr)
            return E_INVALIDARG;
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths) override
    {
        if (pborderwidths == nullptr)
            return E_INVALIDARG;
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS) override
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE SetActiveObject(
        IOleInPlaceActiveObject *pActiveObject, LPCOLESTR) override
    {
        const auto state = m_state.lock();
        wxActiveXContainer *window =
            state ? state->GetSiteTarget() : nullptr;
        if ( !window )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        if (pActiveObject)
            pActiveObject->AddRef();

        // AddRef() is server code and may re-enter. Publish only after the
        // weak owner and phase have been revalidated, then release the old
        // value after the member is already self-contained. No owner access is
        // allowed after that Release().
        window = state->GetSiteTarget();
        if ( !window )
        {
            if ( pActiveObject )
                pActiveObject->Release();
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        IOleInPlaceActiveObject * const oldActiveObject =
            window->m_oleInPlaceActiveObject.Detach();
        window->m_oleInPlaceActiveObject.Attach(pActiveObject);
        if ( oldActiveObject )
            oldActiveObject->Release();
        return S_OK;
    }

    //********************IOleInPlaceFrame************************

    STDMETHOD(InsertMenus)(HMENU, LPOLEMENUGROUPWIDTHS) override {return S_OK;}
    STDMETHOD(SetMenu)(HMENU, HOLEMENU, HWND) override {return S_OK;}
    STDMETHOD(RemoveMenus)(HMENU) override {return S_OK;}
    STDMETHOD(SetStatusText)(LPCOLESTR) override {return S_OK;}
    HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) override {return S_OK;}
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD) override
    {
        // TODO: send an event with this id
        const auto state = m_state.lock();
        wxActiveXContainer *window =
            state ? state->GetSiteTarget() : nullptr;
        IOleInPlaceActiveObject *activeObjectRaw = window
            ? static_cast<IOleInPlaceActiveObject *>(
                  window->m_oleInPlaceActiveObject)
            : nullptr;
        if ( activeObjectRaw )
            activeObjectRaw->AddRef();
        wxAutoIOleInPlaceActiveObject activeObject(activeObjectRaw);
        window = state ? state->GetSiteTarget() : nullptr;
        if ( window && activeObject.IsOk() )
            activeObject->TranslateAccelerator(lpmsg);
        return S_FALSE;
    }

    //*******************IOleInPlaceSite**************************
    HRESULT STDMETHODCALLTYPE CanInPlaceActivate() override {return S_OK;}
    HRESULT STDMETHODCALLTYPE OnInPlaceActivate() override
    {   m_bInPlaceActive = true;    return S_OK;    }
    HRESULT STDMETHODCALLTYPE OnUIActivate() override
    {   m_bUIActive = true;         return S_OK;    }
    HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame,
                                        IOleInPlaceUIWindow **ppDoc,
                                        LPRECT lprcPosRect,
                                        LPRECT lprcClipRect,
                                        LPOLEINPLACEFRAMEINFO lpFrameInfo) override
    {
        if (ppFrame == nullptr || ppDoc == nullptr || lprcPosRect == nullptr ||
            lprcClipRect == nullptr || lpFrameInfo == nullptr)
        {
            if (ppFrame != nullptr)
                (*ppFrame) = nullptr;
            if (ppDoc != nullptr)
                (*ppDoc) = nullptr;
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
            *ppFrame = nullptr;
            return E_UNEXPECTED;
        }

        const auto state = m_state.lock();
        const HWND parent = state ? state->GetParentHwndIfCurrent() : nullptr;
        if ( !parent )
        {
            (*ppDoc)->Release();
            *ppDoc = nullptr;
            (*ppFrame)->Release();
            *ppFrame = nullptr;
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        RECT rect;
        ::GetClientRect(parent, &rect);
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
        lpFrameInfo->hwndFrame = parent;

        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Scroll(SIZE) override {return S_OK;}
    HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL) override
    {   m_bUIActive = false;         return S_OK;    }
    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate() override
    {   m_bInPlaceActive = false;    return S_OK;    }
    HRESULT STDMETHODCALLTYPE DiscardUndoState() override {return S_OK;}
    HRESULT STDMETHODCALLTYPE DeactivateAndUndo() override {return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect) override
    {
        const auto state = m_state.lock();
        wxActiveXContainer *window =
            state ? state->GetSiteTarget() : nullptr;
        IOleInPlaceObject *inPlaceObjectRaw = window
            ? static_cast<IOleInPlaceObject *>(window->m_oleInPlaceObject)
            : nullptr;
        if ( inPlaceObjectRaw )
            inPlaceObjectRaw->AddRef();
        wxAutoIOleInPlaceObject inPlaceObject(inPlaceObjectRaw);
        window = state ? state->GetSiteTarget() : nullptr;
        const HWND parent = state ? state->GetParentHwndIfCurrent() : nullptr;
        if ( window && parent && inPlaceObject.IsOk() && lprcPosRect )
        {
           //
           // Result of several hours and days of bug hunting -
           // this is called by an object when it wants to resize
           // itself to something different from our parent window -
           // don't let it :)
           //
//            inPlaceObject->SetObjectRects(
//                lprcPosRect, lprcPosRect);
           RECT rcClient;
           ::GetClientRect(parent, &rcClient);
            inPlaceObject->SetObjectRects(&rcClient, &rcClient);
        }
        return S_OK;
    }
    //*************************IOleInPlaceSiteEx***********************
    HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(BOOL * pfNoRedraw, DWORD) override
    {
        const auto state = m_state.lock();
        wxActiveXContainer *window =
            state ? state->GetSiteTarget() : nullptr;
        if ( !window )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        IUnknown *activeXRaw = window->m_ActiveX;
        if ( activeXRaw )
            activeXRaw->AddRef();
        wxAutoIUnknown activeX(activeXRaw);
        window = state->GetSiteTarget();
        if ( !window || !activeX.IsOk() )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        OleLockRunning(activeX, TRUE, FALSE);
        if (pfNoRedraw)
            (*pfNoRedraw) = FALSE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(BOOL) override
    {
        const auto state = m_state.lock();
        wxActiveXContainer *window =
            state ? state->GetSiteTarget() : nullptr;
        if ( !window )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        IUnknown *activeXRaw = window->m_ActiveX;
        if ( activeXRaw )
            activeXRaw->AddRef();
        wxAutoIUnknown activeX(activeXRaw);
        window = state->GetSiteTarget();
        if ( !window || !activeX.IsOk() )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        OleLockRunning(activeX, FALSE, FALSE);
        return S_OK;
    }
    STDMETHOD(RequestUIActivate)() override { return S_OK;}
    //*************************IOleClientSite**************************
    HRESULT STDMETHODCALLTYPE SaveObject() override {return S_OK;}
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
    STDMETHOD(GetMoniker)(DWORD, DWORD, IMoniker **) override {return E_FAIL;}
    HRESULT STDMETHODCALLTYPE GetContainer(LPOLECONTAINER * ppContainer) override
    {
        if (ppContainer == nullptr)
            return E_INVALIDARG;
        HRESULT hr = QueryInterface(
            IID_IOleContainer, (void**)(ppContainer));
        wxASSERT(SUCCEEDED(hr));
        return hr;
    }
    HRESULT STDMETHODCALLTYPE ShowObject() override
    {
        const auto state = m_state.lock();
        wxActiveXContainer * const window =
            state ? state->GetSiteTarget() : nullptr;
        if (window && window->m_oleObjectHWND)
            ::ShowWindow(window->m_oleObjectHWND, SW_SHOW);
        return S_OK;
    }
    STDMETHOD(OnShowWindow)(BOOL) override {return S_OK;}
    STDMETHOD(RequestNewObjectLayout)() override {return E_NOTIMPL;}
    //********************IParseDisplayName***************************
    HRESULT STDMETHODCALLTYPE ParseDisplayName(
        IBindCtx *, LPOLESTR, ULONG *, IMoniker **) override {return E_NOTIMPL;}
    //********************IOleContainer*******************************
    STDMETHOD(EnumObjects)(DWORD, IEnumUnknown **) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE LockContainer(BOOL) override {return S_OK;}
    //********************IOleItemContainer***************************
    HRESULT STDMETHODCALLTYPE
    GetObjectW
    (LPOLESTR pszItem, DWORD, IBindCtx *, REFIID, void ** ppvObject) override
    {
        if (pszItem == nullptr || ppvObject == nullptr)
            return E_INVALIDARG;
        *ppvObject = nullptr;
        return MK_E_NOOBJECT;
    }
    HRESULT STDMETHODCALLTYPE GetObjectStorage(
        LPOLESTR pszItem, IBindCtx * , REFIID, void ** ppvStorage) override
    {
        if (pszItem == nullptr || ppvStorage == nullptr)
            return E_INVALIDARG;
        *ppvStorage = nullptr;
        return MK_E_NOOBJECT;
    }
    HRESULT STDMETHODCALLTYPE IsRunning(LPOLESTR pszItem) override
    {
        if (pszItem == nullptr)
            return E_INVALIDARG;
        return MK_E_NOOBJECT;
    }
    //***********************IOleControlSite*****************************
    HRESULT STDMETHODCALLTYPE OnControlInfoChanged() override
    {return S_OK;}
    HRESULT STDMETHODCALLTYPE LockInPlaceActive(BOOL fLock) override
    {
        m_bInPlaceLocked = (fLock) ? true : false;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetExtendedControl(IDispatch **) override
    {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE TransformCoords(
        POINTL * pPtlHimetric, POINTF * pPtfContainer, DWORD) override
    {
        if (pPtlHimetric == nullptr || pPtfContainer == nullptr)
            return E_INVALIDARG;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG, DWORD) override
    {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE OnFocus(BOOL) override {return S_OK;}
    HRESULT STDMETHODCALLTYPE ShowPropertyFrame() override {return E_NOTIMPL;}
    //**************************IOleCommandTarget***********************
    HRESULT STDMETHODCALLTYPE QueryStatus(const GUID *, ULONG cCmds,
                                OLECMD prgCmds[], OLECMDTEXT *) override
    {
        if (prgCmds == nullptr) return E_INVALIDARG;
        for (ULONG nCmd = 0; nCmd < cCmds; nCmd++)
        {
            // unsupported by default
            prgCmds[nCmd].cmdf = 0;
        }
        return OLECMDERR_E_UNKNOWNGROUP;
    }

    HRESULT STDMETHODCALLTYPE Exec(const GUID *, DWORD,
                            DWORD, VARIANTARG *, VARIANTARG *) override
    {return OLECMDERR_E_NOTSUPPORTED;}

    //**********************IAdviseSink************************************
    void STDMETHODCALLTYPE OnDataChange(FORMATETC *, STGMEDIUM *) override {}
    void STDMETHODCALLTYPE OnViewChange(DWORD, LONG) override {}
    void STDMETHODCALLTYPE OnRename(IMoniker *) override {}
    void STDMETHODCALLTYPE OnSave() override {}
    void STDMETHODCALLTYPE OnClose() override {}

    //**********************IOleDocumentSite***************************
    HRESULT STDMETHODCALLTYPE ActivateMe(
        IOleDocumentView __RPC_FAR *pViewToActivate) override
    {
        const auto state = m_state.lock();
        wxActiveXContainer *window =
            state ? state->GetSiteTarget() : nullptr;
        if ( !window )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        wxAutoIOleInPlaceSite inPlaceSite(
            IID_IOleInPlaceSite, (IDispatch *) this);
        if (!inPlaceSite.IsOk())
            return E_FAIL;

        wxAutoIOleDocumentView documentView;
        if (pViewToActivate)
        {
            pViewToActivate->AddRef();
            documentView.Attach(pViewToActivate);
        }
        else
        {
            IOleObject * const oleObjectRaw = window->m_oleObject;
            if ( oleObjectRaw )
                oleObjectRaw->AddRef();
            wxAutoIOleObject oleObject(oleObjectRaw);
            window = state->GetSiteTarget();
            if ( !window || !oleObject.IsOk() )
                return CO_E_OBJNOTCONNECTED;

            wxAutoIOleDocument oleDoc(IID_IOleDocument, oleObject);
            window = state->GetSiteTarget();
            if (! oleDoc.IsOk())
                return E_FAIL;
            if ( !window )
                return CO_E_OBJNOTCONNECTED;

            HRESULT hr = oleDoc->CreateView(inPlaceSite, nullptr,
                                             0, documentView.GetRef());
            if (hr != S_OK)
                return E_FAIL;
        }

        if ( !documentView.IsOk() || state->GetSiteTarget() != window )
            return CO_E_OBJNOTCONNECTED;

        IOleDocumentView * const publishedViewRaw = documentView;
        publishedViewRaw->AddRef();
        wxAutoIOleDocumentView publishedView(publishedViewRaw);
        window = state->GetSiteTarget();
        if ( !window )
        {
            return state && !state->IsOwnerThread()
                ? RPC_E_WRONG_THREAD
                : CO_E_OBJNOTCONNECTED;
        }

        // Fully depublish and deactivate the previous view before publishing
        // the replacement. Every call is server code; retain the old member
        // reference in a local pin and reacquire the exact owner afterwards.
        IOleDocumentView * const oldViewRaw =
            window->m_docView.Detach();
        wxAutoIOleDocumentView oldView(oldViewRaw);
        if ( oldView.IsOk() )
        {
            HRESULT oldResult = oldView->UIActivate(FALSE);
            window = state->GetSiteTarget();
            if ( !window )
                return CO_E_OBJNOTCONNECTED;
            if ( FAILED(oldResult) )
            {
                window->m_docView.Attach(oldView.Detach());
                return oldResult;
            }

            oldResult = oldView->SetInPlaceSite(nullptr);
            window = state->GetSiteTarget();
            if ( !window )
                return CO_E_OBJNOTCONNECTED;
            if ( FAILED(oldResult) )
            {
                window->m_docView.Attach(oldView.Detach());
                return oldResult;
            }

            oldView.Free();
            window = state->GetSiteTarget();
            if ( !window )
                return CO_E_OBJNOTCONNECTED;
        }

        window->m_docView.Attach(publishedView.Detach());

        const HRESULT siteResult =
            documentView->SetInPlaceSite(inPlaceSite);
        if ( state->GetSiteTarget() != window )
            return CO_E_OBJNOTCONNECTED;
        if ( FAILED(siteResult) )
            return siteResult;

        return documentView->UIActivate(TRUE);
    }

    friend bool QueryClientSiteInterface(FrameSite *self, REFIID iid, void **_interface, const char *&desc)
    {
        const auto state = self->m_state.lock();
        wxActiveXContainer * const window =
            state ? state->GetSiteTarget() : nullptr;
        return window &&
               window->QueryClientSiteInterface(iid, _interface, desc);
    }

protected:
    std::weak_ptr<wxActiveXLifetimeState> m_state;

    HDC m_hDCBuffer;
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


    std::weak_ptr<wxActiveXLifetimeState> m_state;
    IID m_customId;
    bool m_haveCustomId;

    friend bool wxActiveXEventsInterface(wxActiveXEvents *self, REFIID iid, void **_interface, const char *&desc);

public:

    // a pointer to this static variable is used as an 'invalid_entry_marker'
    // wxVariants containing a void* to this variables are 'empty' in the sense
    // that the actual ActiveX OLE parameter has not been converted and inserted
    // into m_params.
    static wxVariant ms_invalidEntryMarker;

    explicit wxActiveXEvents(
        const std::shared_ptr<wxActiveXLifetimeState>& state)
        : m_state(state), m_haveCustomId(false) {}
    wxActiveXEvents(
        const std::shared_ptr<wxActiveXLifetimeState>& state,
        REFIID iid)
        : m_state(state), m_customId(iid), m_haveCustomId(true) {}
    virtual ~wxActiveXEvents()
    {
    }

    // IDispatch
    STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR**, unsigned int, LCID, DISPID*) override
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP GetTypeInfo(unsigned int, LCID, ITypeInfo**) override
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP GetTypeInfoCount(unsigned int*) override
    {
        return E_NOTIMPL;
    }


    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid,
                        LCID lcid,
                          WORD wFlags, DISPPARAMS * pDispParams,
                          VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
                          unsigned int * puArgErr) override
    {
        // The wx event handler is allowed to synchronously delete its
        // container. Unadvise() can then drop the connection point's final
        // reference to this sink while Invoke() is still on the stack.
        AddRef();
        const HRESULT hr = InvokePinned(
            dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult,
            pExcepInfo, puArgErr);
        Release();
        return hr;
    }

private:
    HRESULT InvokePinned(DISPID dispIdMember, REFIID riid,
                         LCID lcid,
                         WORD wFlags, DISPPARAMS * pDispParams,
                         VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
                         unsigned int * puArgErr)
    {
        if (wFlags & (DISPATCH_PROPERTYGET | DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF))
            return E_NOTIMPL;

        if ( !pDispParams )
            return E_INVALIDARG;

        const auto state = m_state.lock();
        if ( state && !state->IsOwnerThread() )
            return RPC_E_WRONG_THREAD;

        wxActiveXContainer * const activeX =
            state ? state->GetEventTarget() : nullptr;
        if ( !activeX )
            return RPC_E_DISCONNECTED;

        // ActiveX Event

        // Dispatch Event
        wxActiveXEvent  event;
        event.SetEventType(wxEVT_ACTIVEX);
        event.SetEventObject(activeX);
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

        // Keep a weak lifetime across application code. During construction a
        // wx child can't be deallocated from inside its own new-expression
        // without invoking undefined C++, but it can be marked for pending
        // deletion. Make that request tear down the native transaction before
        // returning to the server; in particular this preserves the pending-
        // Advise rollback path without dereferencing an object actually deleted
        // by an already-constructed container's handler.
        const wxWeakRef<wxWindow> activeXLifetime(activeX);

        // process the events from the activex method
        activeX->ProcessEvent(event);

        wxWindow * const liveActiveX = activeXLifetime.get();
        if ( liveActiveX == activeX &&
             wxWindowIsUnavailableForCallbacks(liveActiveX) )
        {
            static_cast<wxActiveXContainer *>(liveActiveX)->TearDownActiveX();
        }

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
            // Whatever, but this was the case in the original implementation of
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
    : m_frameSite(nullptr),
      m_oleObjectHWND(nullptr),
      m_bAmbientUserMode(true),
      m_docAdviseCookie(0),
      m_realparent(parent)
{
    RegisterActiveXState(this, parent);
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
    TearDownActiveX();
}

namespace
{

void LogActiveXRequiredCallFailure(HRESULT hr, const char *operation)
{
    wxLogDebug("wxActiveXContainer: %s failed with HRESULT 0x%08lX",
               wxString::FromAscii(operation),
               static_cast<unsigned long>(hr));
}

} // anonymous namespace

void wxActiveXContainer::TearDownActiveX()
{
    wxActiveXContainer * const identity = this;
    const auto state = FindActiveXState(identity);
    if ( state )
    {
        if ( state->phase == wxActiveXLifetimePhase::Destroying ||
             state->phase == wxActiveXLifetimePhase::Dead )
        {
            return;
        }

        state->phase = wxActiveXLifetimePhase::Destroying;
        state->creationInProgress = false;
        state->acceptEvents = false;
    }

    // Decide the native detach while the exact logical/native generations can
    // still be inspected, but don't cross into the host until every member
    // and callback route below has been made inert.
    const HWND associatedHwnd = GetHwnd();
    bool isExactAssociation = false;
    bool hasStaleMapEntry = false;
    if ( state && associatedHwnd )
    {
        isExactAssociation =
            associatedHwnd == state->associatedHwnd &&
            wxFindWinFromHandle(associatedHwnd) == identity;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( isExactAssociation && state->associatedHwndGeneration )
        {
            isExactAssociation =
                wxWinUIMSWGetHwndGeneration(identity, associatedHwnd) ==
                    state->associatedHwndGeneration;
        }
        if ( isExactAssociation && state->associatedNativeGeneration )
        {
            isExactAssociation =
                wxWinUIMSWGetNativeHwndGeneration(associatedHwnd) ==
                    state->associatedNativeGeneration;
        }
#endif
        hasStaleMapEntry =
            !isExactAssociation &&
            wxFindWinFromHandle(associatedHwnd) == identity;

        state->associatedHwnd = nullptr;
        ++state->hwndBindingGeneration;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        state->associatedHwndGeneration = 0;
        state->associatedNativeGeneration = 0;
#endif
    }

    const bool parentEventsBound = state && state->parentEventsBound;
    const int parentEventId = state ? state->parentEventId : wxID_ANY;
    if ( state )
        state->parentEventsBound = false;

    // Move every resource out without AddRef()/Release(). From the native
    // detach boundary onward this function uses only locals and the sidecar;
    // the address in identity is used solely as an opaque registry/handler key.
    const auto documentConnection = state
        ? std::move(state->documentConnection)
        : std::shared_ptr<wxActiveXDocumentConnection>();
    std::vector<std::shared_ptr<wxActiveXEventConnection>> eventConnections;
    if ( state )
        eventConnections = std::move(state->eventConnections);

    IOleInPlaceActiveObject * const activeObject =
        m_oleInPlaceActiveObject.Detach();
    IOleDocumentView * const documentView = m_docView.Detach();
    IViewObject * const viewObject = m_viewObject.Detach();
    IOleInPlaceObject * const inPlaceObject = m_oleInPlaceObject.Detach();
    IDispatch * const dispatch = m_Dispatch.Detach();
    IOleObject * const oleObject = m_oleObject.Detach();
    IUnknown * const activeX = m_ActiveX.Detach();
    IOleClientSite * const clientSite = m_clientSite.Detach();
    FrameSite * const frameSite = m_frameSite;
    m_frameSite = nullptr;

    const HWND parentHwnd = state
        ? state->GetParentHwndIfCurrent()
        : nullptr;

    m_oleObjectHWND = nullptr;
    m_docAdviseCookie = 0;
    m_realparent = nullptr;

    if ( state )
        state->owner.Release();

    if ( associatedHwnd )
    {
        if ( isExactAssociation )
        {
            // DoDetachHWND() may traverse DnD, UIA and the WinUI host. The
            // object is already unpublished and no access to it is made after
            // this qualified call returns.
            identity->wxWindow::DissociateHandle();
        }
        else
        {
            // ABA: remove only our stale wx map edge. Never restore the saved
            // WNDPROC onto a foreign native generation.
            identity->MSWSetOldWndProc(nullptr);
            if ( hasStaleMapEntry )
                wxRemoveHandleAssociation(identity);
            identity->SetHWND(nullptr);
        }
    }
    else
    {
        identity->MSWSetOldWndProc(nullptr);
    }

    // No object access is permitted below this line. The expected sidecar
    // comparison also makes registry removal safe against address reuse.
    if ( state )
    {
        state->phase = wxActiveXLifetimePhase::Dead;
        EraseActiveXState(identity, state);
    }

    // Unbind with the raw identity only after native detach. Each lookup is
    // repeated because removing a handler may release application functors
    // and destroy the parent.
    if ( parentEventsBound && state )
    {
        if ( wxWindow * const parent = state->parentWindow.get() )
        {
            parent->Unbind(wxEVT_SIZE,
                           &wxActiveXContainer::OnSize,
                           identity, parentEventId);
        }
        if ( wxWindow * const parent = state->parentWindow.get() )
        {
            parent->Unbind(wxEVT_SET_FOCUS,
                           &wxActiveXContainer::OnSetFocus,
                           identity, parentEventId);
        }
        if ( wxWindow * const parent = state->parentWindow.get() )
        {
            parent->Unbind(wxEVT_KILL_FOCUS,
                           &wxActiveXContainer::OnKillFocus,
                           identity, parentEventId);
        }
        state->parentWindow.Release();
    }

    // The document advise was established last, so unwind it before the
    // connection points. Each record clears its cookie before Unadvise(),
    // making recursive teardown and error returns exactly-once.
    if ( documentConnection )
        documentConnection->Disconnect();
    for ( auto it = eventConnections.rbegin();
          it != eventConnections.rend(); ++it )
    {
        (*it)->Disconnect();
    }

    if ( documentView )
    {
        documentView->UIActivate(FALSE);
        documentView->SetInPlaceSite(nullptr);
    }

    if ( inPlaceObject )
    {
        // UI activation is nested inside in-place activation.
        inPlaceObject->UIDeactivate();
        inPlaceObject->InPlaceDeactivate();
    }

    if ( oleObject )
    {
        oleObject->DoVerb(OLEIVERB_HIDE, nullptr, clientSite, 0,
                          parentHwnd, nullptr);
        oleObject->Close(OLECLOSE_NOSAVE);
        oleObject->SetClientSite(nullptr);
    }

    if ( activeX )
        OleLockRunning(activeX, FALSE, FALSE);

    // Drop server interfaces before the client site and its creator reference.
    if ( activeObject )
        activeObject->Release();
    if ( documentView )
        documentView->Release();
    if ( viewObject )
        viewObject->Release();
    if ( inPlaceObject )
        inPlaceObject->Release();
    if ( dispatch )
        dispatch->Release();
    if ( oleObject )
        oleObject->Release();
    if ( activeX )
        activeX->Release();

    // clientSite holds a FrameSite reference. Release it before releasing the
    // creator reference; never delete the COM object behind late holders.
    if ( clientSite )
        clientSite->Release();
    if ( frameSite )
        frameSite->Release();
}

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
    (void)TryCreateActiveX(iid, pUnk);
}

bool wxActiveXContainer::TryCreateActiveX(REFIID iid, IUnknown* pUnk)
{
    const auto state = FindActiveXState(this);
    if ( state &&
         (state->phase != wxActiveXLifetimePhase::Creating ||
          state->creationInProgress) )
    {
        wxLogDebug("wxActiveXContainer: recursive CreateActiveX() rejected");
        return false;
    }
    if ( state )
        state->creationInProgress = true;
    wxWindow * const realParent =
        state ? state->parentWindow.get() : nullptr;
    const HWND realParentHwnd =
        state ? state->GetParentHwndIfCurrent() : nullptr;
    if ( !state || !realParent || !realParentHwnd || !pUnk )
    {
        TearDownActiveX();
        return false;
    }
    const std::uint64_t stateGeneration = state->generation;
    wxActiveXContainer * const identity = this;
    const auto getCurrentOwner = [identity, &state, stateGeneration]()
    {
        return GetCurrentCreatingActiveXOwner(
            identity, state, stateGeneration);
    };
    const auto rollbackRequiredFailure =
        [&getCurrentOwner](HRESULT hr, const char *operation)
    {
        if ( SUCCEEDED(hr) )
            return false;

        // Logging is application code: a custom target is allowed to destroy
        // the container. Never use an owner captured before this call.
        LogActiveXRequiredCallFailure(hr, operation);
        if ( wxActiveXContainer * const liveOwner = getCurrentOwner() )
            liveOwner->TearDownActiveX();
        return true;
    };
    const wxSize initialParentSize = realParent->GetClientSize();

    wxActiveXContainer *owner = getCurrentOwner();
    if ( !owner )
        return false;

    // Every out-parameter belongs to a stack RAII object until the COM call
    // has returned and the sidecar identity has been revalidated. A reentrant
    // server must never be given storage inside a possibly deleted owner.
    wxAutoIUnknown acquiredActiveX;
    HRESULT hret = acquiredActiveX.QueryInterface(iid, pUnk);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( rollbackRequiredFailure(hret, "QueryInterface(control)") )
        return false;

    owner->m_ActiveX.Attach(acquiredActiveX.Detach());
    IUnknown * const activeXRaw = owner->m_ActiveX;
    activeXRaw->AddRef();
    wxAutoIUnknown activeX(activeXRaw);
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    // FrameSite
    FrameSite * const frameSite = new FrameSite(realParent, state);
    wxAutoIAdviseSink frameSiteCreator(
        static_cast<IAdviseSink *>(frameSite));

    wxAutoIOleClientSite acquiredClientSite;
    hret = acquiredClientSite.QueryInterface(
        IID_IOleClientSite, (IDispatch *) frameSite);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( rollbackRequiredFailure(hret, "QueryInterface(IOleClientSite)") )
        return false;

    owner->m_frameSite = frameSite;
    (void)frameSiteCreator.Detach();
    owner->m_clientSite.Attach(acquiredClientSite.Detach());
    IOleClientSite * const clientSiteRaw = owner->m_clientSite;
    clientSiteRaw->AddRef();
    wxAutoIOleClientSite clientSite(clientSiteRaw);
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    // adviseSink
    wxAutoIAdviseSink adviseSink(
        IID_IAdviseSink, (IDispatch *) frameSite);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( !adviseSink.IsOk() )
    {
        owner->TearDownActiveX();
        return false;
    }

    // IDispatch and type information are optional for controls without a
    // dispatch event source. Hosting itself only requires the OLE interfaces.
    wxAutoIDispatch acquiredDispatch;
    hret = acquiredDispatch.QueryInterface(IID_IDispatch, activeX);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( SUCCEEDED(hret) && acquiredDispatch.IsOk() )
        owner->m_Dispatch.Attach(acquiredDispatch.Detach());

    //
    // SETUP TYPEINFO AND ACTIVEX EVENTS
    //

    // get type info via class info
    wxAutoIProvideClassInfo classInfo(IID_IProvideClassInfo, activeX);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    wxAutoITypeInfo typeInfo;
    TYPEATTR *ta = nullptr;
    bool creationCancelled = false;
    if ( classInfo.IsOk() &&
         SUCCEEDED(classInfo->GetClassInfo(typeInfo.GetRef())) &&
         getCurrentOwner() && typeInfo.IsOk() &&
         SUCCEEDED(typeInfo->GetTypeAttr(&ta)) &&
         getCurrentOwner() && ta )
    {
        if ( ta->typekind == TKIND_COCLASS )
        {
            // iterate contained interfaces
            for (int i = 0; i < ta->cImplTypes; i++)
            {
                if ( !getCurrentOwner() )
                {
                    creationCancelled = true;
                    break;
                }

                HREFTYPE rt = 0;
                if ( FAILED(typeInfo->GetRefTypeOfImplType(i, &rt)) ||
                     !getCurrentOwner() )
                    continue;

                wxAutoITypeInfo ti;
                if ( FAILED(typeInfo->GetRefTypeInfo(rt, ti.GetRef())) ||
                     !getCurrentOwner() || !ti.IsOk() )
                {
                    continue;
                }

                int impTypeFlags = 0;
                if ( FAILED(typeInfo->GetImplTypeFlags(i, &impTypeFlags)) ||
                     !getCurrentOwner() ||
                     !(impTypeFlags & IMPLTYPEFLAG_FDEFAULT) ||
                     !(impTypeFlags & IMPLTYPEFLAG_FSOURCE) ||
                     (impTypeFlags & IMPLTYPEFLAG_FDEFAULTVTABLE) )
                {
                    continue;
                }

                TYPEATTR *ta2 = nullptr;
                const HRESULT typeAttrResult = ti->GetTypeAttr(&ta2);
                if ( FAILED(typeAttrResult) || !ta2 )
                    continue;
                if ( !getCurrentOwner() )
                {
                    ti->ReleaseTypeAttr(ta2);
                    creationCancelled = true;
                    break;
                }

                if ( ta2->typekind == TKIND_DISPATCH )
                {
                    wxAutoIConnectionPointContainer cpContainer(
                        IID_IConnectionPointContainer, activeX);
                    wxAutoIConnectionPoint cp;
                    if ( getCurrentOwner() && cpContainer.IsOk() &&
                         SUCCEEDED(cpContainer->FindConnectionPoint(
                             ta2->guid, cp.GetRef())) &&
                         getCurrentOwner() && cp.IsOk() )
                    {
                        wxActiveXEvents * const events =
                            new wxActiveXEvents(state, ta2->guid);
                        const auto connection =
                            std::make_shared<wxActiveXEventConnection>(
                                cp, events);
                        if ( !getCurrentOwner() )
                        {
                            events->Release();
                            ti->ReleaseTypeAttr(ta2);
                            creationCancelled = true;
                            break;
                        }
                        state->eventConnections.push_back(connection);

                        DWORD adviseCookie = 0;
                        // Preserve the historical synchronous-event contract
                        // during creation. The sidecar and pending connection
                        // record make destruction from this callback safe.
                        state->acceptEvents = true;
                        connection->BeginAdvise();
                        hret = cp->Advise(events, &adviseCookie);
                        if ( FAILED(hret) )
                            state->acceptEvents = false;
                        connection->CompleteAdvise(hret, adviseCookie);
                        events->Release();

                        // Advise() is a synchronous COM boundary. Teardown
                        // may have cleared the sidecar and deleted the C++
                        // owner while the connection record was pending. The
                        // local record is sufficient to finish Unadvise; do
                        // not touch the former owner until all three identity
                        // dimensions have been revalidated.
                        if ( !getCurrentOwner() )
                            creationCancelled = true;
                    }
                }

                if ( !getCurrentOwner() )
                    creationCancelled = true;
                ti->ReleaseTypeAttr(ta2);
                if ( creationCancelled )
                    break;
            }
        }

    }

    if ( ta && typeInfo.IsOk() )
        typeInfo->ReleaseTypeAttr(ta);

    // ReleaseTypeAttr() is a COM boundary too. This check also makes the
    // cancellation path above leave without dereferencing a deleted owner.
    if ( creationCancelled || !getCurrentOwner() )
        return false;

    //
    // END
    //

    // Get IOleObject interface
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    wxAutoIOleObject acquiredOleObject;
    hret = acquiredOleObject.QueryInterface(IID_IOleObject, activeX);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( rollbackRequiredFailure(hret, "QueryInterface(IOleObject)") )
        return false;

    owner->m_oleObject.Attach(acquiredOleObject.Detach());
    IOleObject * const oleObjectRaw = owner->m_oleObject;
    oleObjectRaw->AddRef();
    wxAutoIOleObject oleObject(oleObjectRaw);
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    // Windowed controls don't require IViewObject; keep it when available for
    // the existing inactive/windowless paint path.
    wxAutoIViewObject acquiredViewObject;
    hret = acquiredViewObject.QueryInterface(IID_IViewObject, activeX);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( SUCCEEDED(hret) && acquiredViewObject.IsOk() )
        owner->m_viewObject.Attach(acquiredViewObject.Detach());

    // document advise
    auto documentConnection =
        std::make_shared<wxActiveXDocumentConnection>(oleObject, adviseSink);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    state->documentConnection = documentConnection;
    DWORD documentCookie = 0;
    documentConnection->BeginAdvise();
    hret = oleObject->Advise(adviseSink, &documentCookie);
    documentConnection->CompleteAdvise(hret, documentCookie);
    const DWORD committedDocumentCookie = documentConnection->GetCookie();
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    owner->m_docAdviseCookie = committedDocumentCookie;
    if ( !committedDocumentCookie )
        state->documentConnection.reset();

    // TODO:Needed?
//    hret = m_viewObject->SetAdvise(DVASPECT_CONTENT, 0, adviseSink);
    oleObject->SetHostNames(L"wxActiveXContainer", nullptr);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    hret = OleSetContainedObject(oleObject, TRUE);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( rollbackRequiredFailure(hret, "OleSetContainedObject") )
        return false;
    hret = OleRun(oleObject);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( rollbackRequiredFailure(hret, "OleRun") )
        return false;


    // Get IOleInPlaceObject interface
    wxAutoIOleInPlaceObject acquiredInPlaceObject;
    hret = acquiredInPlaceObject.QueryInterface(
        IID_IOleInPlaceObject, activeX);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( rollbackRequiredFailure(
             hret, "QueryInterface(IOleInPlaceObject)") )
        return false;
    owner->m_oleInPlaceObject.Attach(acquiredInPlaceObject.Detach());
    IOleInPlaceObject * const inPlaceObjectRaw =
        owner->m_oleInPlaceObject;
    inPlaceObjectRaw->AddRef();
    wxAutoIOleInPlaceObject inPlaceObject(inPlaceObjectRaw);
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    // status
    DWORD dwMiscStatus = 0;
    hret = oleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( rollbackRequiredFailure(hret, "IOleObject::GetMiscStatus") )
        return false;

    // set client site first ?
    if (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
    {
        hret = oleObject->SetClientSite(clientSite);
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( rollbackRequiredFailure(
                 hret, "IOleObject::SetClientSite") )
            return false;
    }


    // stream init
    wxAutoIPersistStreamInit
        pPersistStreamInit(IID_IPersistStreamInit, oleObject);
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    if (pPersistStreamInit.IsOk())
    {
        hret = pPersistStreamInit->InitNew();
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( rollbackRequiredFailure(
                 hret, "IPersistStreamInit::InitNew") )
            return false;
    }

    if (! (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
    {
        hret = oleObject->SetClientSite(clientSite);
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( rollbackRequiredFailure(
                 hret, "IOleObject::SetClientSite") )
            return false;
    }

    HWND oleObjectHwnd = nullptr;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    unsigned long long oleObjectNativeGeneration = 0;
#endif
    const auto queryHostedWindow = [&]()
    {
        HWND queriedHwnd = nullptr;
        const HRESULT result = inPlaceObject->GetWindow(&queriedHwnd);
        if ( !getCurrentOwner() || FAILED(result) || !queriedHwnd ||
             !::IsWindow(queriedHwnd) )
        {
            return false;
        }

#if defined(__WXWINUI__) && wxUSE_WINUI3
        const unsigned long long queriedGeneration =
            wxWinUIMSWGetNativeHwndGeneration(queriedHwnd);
        if ( !getCurrentOwner() || !queriedGeneration ||
             !::IsWindow(queriedHwnd) ||
             wxWinUIMSWGetNativeHwndGeneration(queriedHwnd) !=
                 queriedGeneration )
        {
            return false;
        }
        oleObjectNativeGeneration = queriedGeneration;
#endif
        oleObjectHwnd = queriedHwnd;
        return true;
    };
    const auto hostedWindowIsCurrent = [&]()
    {
        if ( !getCurrentOwner() || !oleObjectHwnd ||
             !::IsWindow(oleObjectHwnd) )
        {
            return false;
        }
#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( !oleObjectNativeGeneration ||
             wxWinUIMSWGetNativeHwndGeneration(oleObjectHwnd) !=
                 oleObjectNativeGeneration )
        {
            return false;
        }
#endif

        HWND queriedHwnd = nullptr;
        const HRESULT result = inPlaceObject->GetWindow(&queriedHwnd);
        if ( !getCurrentOwner() || FAILED(result) ||
             queriedHwnd != oleObjectHwnd ||
             !::IsWindow(queriedHwnd) )
        {
            return false;
        }
#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( wxWinUIMSWGetNativeHwndGeneration(queriedHwnd) !=
                 oleObjectNativeGeneration )
        {
            return false;
        }
#endif
        return getCurrentOwner() != nullptr;
    };
    const auto rollbackInvalidHostedWindow = [&]()
    {
        if ( wxActiveXContainer * const liveOwner = getCurrentOwner() )
            liveOwner->TearDownActiveX();
        return false;
    };

    if (! (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME))
    {
        if ( state->GetParentHwndIfCurrent() != realParentHwnd )
        {
            owner->TearDownActiveX();
            return false;
        }

        RECT posRect;
        wxCopyRectToRECT(initialParentSize, posRect);

        hret = oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, nullptr,
            clientSite, 0, realParentHwnd, &posRect);
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( rollbackRequiredFailure(
                 hret, "IOleObject::DoVerb(INPLACEACTIVATE)") )
            return false;

        if ( !queryHostedWindow() )
            return rollbackInvalidHostedWindow();

        ::SetActiveWindow(oleObjectHwnd);
        if ( !hostedWindowIsCurrent() )
            return rollbackInvalidHostedWindow();

        if (posRect.right > 0 && posRect.bottom > 0)
        {
            hret = inPlaceObject->SetObjectRects(&posRect, &posRect);
            owner = getCurrentOwner();
            if ( !owner )
                return false;
            if ( rollbackRequiredFailure(
                     hret, "IOleInPlaceObject::SetObjectRects") )
                return false;
            if ( !hostedWindowIsCurrent() )
                return rollbackInvalidHostedWindow();
        }

        if ( state->GetParentHwndIfCurrent() != realParentHwnd )
        {
            owner->TearDownActiveX();
            return false;
        }
        hret = oleObject->DoVerb(OLEIVERB_SHOW, 0, clientSite, 0,
            realParentHwnd, &posRect);
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( rollbackRequiredFailure(
                 hret, "IOleObject::DoVerb(SHOW)") )
            return false;

        // This initial transaction deliberately requires one stable native
        // lifetime. A server replacing its HWND during SHOW is rejected
        // fail-closed instead of silently publishing a different generation.
        if ( !hostedWindowIsCurrent() )
            return rollbackInvalidHostedWindow();
    }

    if ( !oleObjectHwnd && inPlaceObject.IsOk() )
    {
        if ( !queryHostedWindow() )
        {
            owner = getCurrentOwner();
            if ( !owner )
                return false;
            // An invisible-at-runtime control is allowed not to expose a
            // window. A visible control was rejected above.
            oleObjectHwnd = nullptr;
#if defined(__WXWINUI__) && wxUSE_WINUI3
            oleObjectNativeGeneration = 0;
#endif
        }
    }

    if ( oleObjectHwnd )
    {
        ::SetActiveWindow(oleObjectHwnd);
        if ( !hostedWindowIsCurrent() )
            return rollbackInvalidHostedWindow();

        ::ShowWindow(oleObjectHwnd, SW_SHOW);
        if ( !hostedWindowIsCurrent() )
            return rollbackInvalidHostedWindow();

        // Re-query once more immediately before publishing the wx binding.
        if ( !hostedWindowIsCurrent() )
            return rollbackInvalidHostedWindow();
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        owner->m_oleObjectHWND = oleObjectHwnd;

        // Publish the pending binding before SubclassWin() crosses USER32.
        // A reentrant control is allowed to destroy its HWND from that
        // call; WM_DESTROY must already know which logical binding to retire.
        state->associatedHwnd = oleObjectHwnd;
        if ( ++state->hwndBindingGeneration == 0 )
            ++state->hwndBindingGeneration;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        state->associatedNativeGeneration =
            oleObjectNativeGeneration;
#endif
        // m_hWnd is known empty here. Bypass AssociateHandle()'s unrelated
        // DestroyWindow/logging branch and adopt directly; SubclassWin()
        // guards destruction from its synchronous wxEVT_CREATE dispatch.
        owner->SubclassWin(oleObjectHwnd);

        owner = getCurrentOwner();
        if ( !owner )
            return false;

#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( owner->GetHWND() == oleObjectHwnd )
        {
            state->associatedHwndGeneration =
                wxWinUIMSWGetHwndGeneration(owner, oleObjectHwnd);
        }
#endif
        owner = getCurrentOwner();
        if ( !owner )
            return false;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        if ( !state->associatedHwndGeneration ||
             !state->associatedNativeGeneration ||
             wxWinUIMSWGetNativeHwndGeneration(oleObjectHwnd) !=
                 oleObjectNativeGeneration )
        {
            owner->TearDownActiveX();
            return false;
        }
#endif
        if ( owner->GetHWND() != oleObjectHwnd ||
             wxFindWinFromHandle(oleObjectHwnd) != owner ||
             state->GetParentHwndIfCurrent() != realParentHwnd )
        {
            owner->TearDownActiveX();
            return false;
        }

        const bool reparented = owner->Reparent(realParent);
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( !reparented || owner->GetHWND() != oleObjectHwnd ||
             wxFindWinFromHandle(oleObjectHwnd) != owner ||
             state->GetParentHwndIfCurrent() != realParentHwnd )
        {
            owner->TearDownActiveX();
            return false;
        }

        wxWindow* pWnd = realParent;
        int id = realParent->GetId();

        state->parentEventId = id;
        state->parentEventsBound = true;
        pWnd->Bind(wxEVT_SIZE, &wxActiveXContainer::OnSize, owner, id);
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( state->parentWindow.get() != pWnd )
        {
            owner->TearDownActiveX();
            return false;
        }
        pWnd->Bind(wxEVT_SET_FOCUS, &wxActiveXContainer::OnSetFocus, owner, id);
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( state->parentWindow.get() != pWnd )
        {
            owner->TearDownActiveX();
            return false;
        }
        pWnd->Bind(wxEVT_KILL_FOCUS, &wxActiveXContainer::OnKillFocus, owner, id);

        owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( state->parentWindow.get() != pWnd )
        {
            owner->TearDownActiveX();
            return false;
        }
    }

    // Drop every creation-local COM pin before committing Active. These
    // releases may execute server code, so perform them as one owner-free
    // region and validate the sidecar identity again only after all locals are
    // inert. The member references and committed connection records remain.
    pPersistStreamInit.Free();
    inPlaceObject.Free();
    oleObject.Free();
    documentConnection.reset();
    adviseSink.Free();
    clientSite.Free();
    activeX.Free();
    typeInfo.Free();
    classInfo.Free();

    owner = getCurrentOwner();
    if ( !owner )
        return false;

    bool nativeEdgesCurrent =
        state->GetParentHwndIfCurrent() == realParentHwnd;
    if ( nativeEdgesCurrent && state->associatedHwnd )
    {
        nativeEdgesCurrent =
            owner->GetHWND() == state->associatedHwnd &&
            wxFindWinFromHandle(state->associatedHwnd) == owner;
#if defined(__WXWINUI__) && wxUSE_WINUI3
        nativeEdgesCurrent = nativeEdgesCurrent &&
            state->associatedHwndGeneration != 0 &&
            wxWinUIMSWGetHwndGeneration(
                owner, state->associatedHwnd) ==
                state->associatedHwndGeneration &&
            state->associatedNativeGeneration != 0 &&
            wxWinUIMSWGetNativeHwndGeneration(state->associatedHwnd) ==
                state->associatedNativeGeneration;
#endif
    }

    owner = getCurrentOwner();
    if ( !owner )
        return false;
    if ( !nativeEdgesCurrent )
    {
        owner->TearDownActiveX();
        return false;
    }

    state->creationInProgress = false;
    state->phase = wxActiveXLifetimePhase::Active;
    state->acceptEvents = true;
    return true;
}

WXLRESULT wxActiveXContainer::MSWWindowProc(WXUINT nMsg,
                                             WXWPARAM wParam,
                                             WXLPARAM lParam)
{
    if ( nMsg != WM_DESTROY )
        return wxWindow::MSWWindowProc(nMsg, wParam, lParam);

    // The ActiveX server owns the HWND and may destroy it before releasing the
    // container. Retire the exact wx/HWND association while WM_DESTROY still
    // identifies this native lifetime. Calling the base chain can re-enter wx
    // code and delete this object, so never retain an unchecked raw owner.
    const auto state = FindActiveXState(this);
    const HWND hwnd = GetHwnd();
    const std::uint64_t bindingGeneration =
        state ? state->hwndBindingGeneration : 0;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    const unsigned long long associationGeneration =
        state ? state->associatedHwndGeneration : 0;
    const unsigned long long nativeGeneration =
        state ? state->associatedNativeGeneration : 0;
#endif
    const wxWeakRef<wxWindow> lifetime(this);
    const WXLRESULT result =
        wxWindow::MSWWindowProc(nMsg, wParam, lParam);

    wxWindow * const live = lifetime.get();
    if ( live != this || !state || !hwnd || live->GetHWND() != hwnd ||
         wxFindWinFromHandle(hwnd) != live ||
         state->associatedHwnd != hwnd ||
         state->hwndBindingGeneration != bindingGeneration )
    {
        return result;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( (associationGeneration &&
          wxWinUIMSWGetHwndGeneration(live, hwnd) !=
              associationGeneration) ||
         (nativeGeneration &&
          wxWinUIMSWGetNativeHwndGeneration(hwnd) != nativeGeneration) )
    {
        // The numeric HWND now denotes a different association/native
        // generation. Remove only our stale map entry and forget our saved
        // procedure; restoring it would subclass the foreign lifetime with a
        // dead wx object.
        wxActiveXContainer * const container =
            static_cast<wxActiveXContainer *>(live);
        if ( (HWND)container->GetHWND() == hwnd &&
             wxFindWinFromHandle(hwnd) == container )
        {
            wxRemoveHandleAssociation(container);
        }
        state->associatedHwnd = nullptr;
        ++state->hwndBindingGeneration;
        state->associatedHwndGeneration = 0;
        state->associatedNativeGeneration = 0;
        container->m_oleObjectHWND = nullptr;
        container->SetHWND(nullptr);
        container->MSWSetOldWndProc(nullptr);
        return result;
    }
#endif

    wxActiveXContainer * const container =
        static_cast<wxActiveXContainer *>(live);
    state->associatedHwnd = nullptr;
    ++state->hwndBindingGeneration;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    state->associatedHwndGeneration = 0;
    state->associatedNativeGeneration = 0;
#endif
    container->m_oleObjectHWND = nullptr;
    container->wxWindow::DissociateHandle();
    return result;
}

//---------------------------------------------------------------------------
// wxActiveXContainer::OnSize
//
// Called when the parent is resized - we need to do this to actually
// move the ActiveX control to where the parent is
//---------------------------------------------------------------------------
void wxActiveXContainer::OnSize(wxSizeEvent& event)
{
    const auto state = FindActiveXState(this);
    wxWindow * const parent = state ? state->parentWindow.get() : nullptr;
    if ( !state || state->GetEventTarget() != this || !parent )
    {
        event.Skip();
        return;
    }

    int w, h;
    parent->GetClientSize(&w, &h);

    RECT posRect;
    posRect.left = 0;
    posRect.top = 0;
    posRect.right = w;
    posRect.bottom = h;

    if (w <= 0 || h <= 0)
    {
        event.Skip();
        return;
    }

    // Pin both server interfaces independently of the wx object. Any COM call
    // below may synchronously dispatch application code which destroys this
    // container and releases its member interfaces.
    IOleObject *oleObjectRaw = m_oleObject;
    if ( oleObjectRaw )
        oleObjectRaw->AddRef();
    wxAutoIOleObject oleObject(oleObjectRaw);
    if ( state->GetEventTarget() != this )
    {
        event.Skip();
        return;
    }

    IOleInPlaceObject *oleInPlaceObjectRaw = m_oleInPlaceObject;
    if ( oleInPlaceObjectRaw )
        oleInPlaceObjectRaw->AddRef();
    wxAutoIOleInPlaceObject oleInPlaceObject(oleInPlaceObjectRaw);
    if ( state->GetEventTarget() != this )
    {
        event.Skip();
        return;
    }

    // extents are in HIMETRIC units
    if (oleObject.IsOk())
    {
        SIZEL sz = {w, h};
        PixelsToHimetric(sz);

        SIZEL sz2 = { 0, 0 };
        const HRESULT getExtentResult =
            oleObject->GetExtent(DVASPECT_CONTENT, &sz2);
        if ( state->GetEventTarget() != this || FAILED(getExtentResult) )
        {
            event.Skip();
            return;
        }
        if (sz2.cx != sz.cx || sz.cy != sz2.cy)
        {
            const HRESULT setExtentResult =
                oleObject->SetExtent(DVASPECT_CONTENT, &sz);
            if ( state->GetEventTarget() != this ||
                 FAILED(setExtentResult) )
            {
                event.Skip();
                return;
            }
        }
    }

    if ( state->GetEventTarget() == this && oleInPlaceObject.IsOk() )
    {
        const HRESULT rectResult =
            oleInPlaceObject->SetObjectRects(&posRect, &posRect);
        if ( state->GetEventTarget() != this || FAILED(rectResult) )
        {
            event.Skip();
            return;
        }
    }

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
    const auto state = FindActiveXState(this);
    wxWindow *parent = state ? state->parentWindow.get() : nullptr;
    const HWND oleObjectHwnd = m_oleObjectHWND;
    IViewObject *viewObjectRaw = m_viewObject;
    if ( viewObjectRaw )
        viewObjectRaw->AddRef();
    wxAutoIViewObject viewObject(viewObjectRaw);
    parent = state ? state->parentWindow.get() : nullptr;
    // Draw only when control is windowless or deactivated
    if ( state && state->GetEventTarget() == this && parent && viewObject )
    {
        int w, h;
        parent->GetSize(&w, &h);
        RECT posRect;
        posRect.left = 0;
        posRect.top = 0;
        posRect.right = w;
        posRect.bottom = h;

        if ( oleObjectHwnd )
        {
            ::RedrawWindow(
                oleObjectHwnd, nullptr, nullptr, RDW_INTERNALPAINT);
        }
        RECTL *prcBounds = (RECTL *) &posRect;
        wxMSWDCImpl *msw = wxDynamicCast( dc.GetImpl() , wxMSWDCImpl );
        viewObject->Draw(DVASPECT_CONTENT, -1, nullptr, nullptr, nullptr,
                         (HDC)msw->GetHDC(), prcBounds, nullptr, nullptr, 0);
    }
}

//---------------------------------------------------------------------------
// wxActiveXContainer::OnSetFocus
//
// Called when the focus is set on the parent - activates the activex control
//---------------------------------------------------------------------------
void wxActiveXContainer::OnSetFocus(wxFocusEvent& event)
{
    const auto state = FindActiveXState(this);
    IOleInPlaceActiveObject *activeObjectRaw = m_oleInPlaceActiveObject;
    if ( activeObjectRaw )
        activeObjectRaw->AddRef();
    wxAutoIOleInPlaceActiveObject activeObject(activeObjectRaw);
    if ( state && state->GetEventTarget() == this && activeObject.IsOk() )
        activeObject->OnFrameWindowActivate(TRUE);

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
    const auto state = FindActiveXState(this);
    IOleInPlaceActiveObject *activeObjectRaw = m_oleInPlaceActiveObject;
    if ( activeObjectRaw )
        activeObjectRaw->AddRef();
    wxAutoIOleInPlaceActiveObject activeObject(activeObjectRaw);
    if ( state && state->GetEventTarget() == this && activeObject.IsOk() )
        activeObject->OnFrameWindowActivate(FALSE);

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
    const auto state = FindActiveXState(this);
    IOleInPlaceActiveObject *activeObjectRaw = m_oleInPlaceActiveObject;
    if ( activeObjectRaw )
        activeObjectRaw->AddRef();
    wxAutoIOleInPlaceActiveObject activeObject(activeObjectRaw);
    if ( state && state->GetEventTarget() == this && activeObject.IsOk() &&
         activeObject->TranslateAccelerator(pMsg) == S_OK )
    {
        return true;
    }

    if ( !state || state->GetEventTarget() != this )
        return false;

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
