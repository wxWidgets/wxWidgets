///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuiactivex.cpp
// Purpose:     deterministic WinUI wxActiveXContainer lifetime qualification
// Author:      wxWidgets development team
// Created:     2026-08-08
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_ACTIVEX && wxUSE_OLE

#include "wx/msw/ole/activex.h"

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/log.h"
#include "wx/eventfilter.h"
#include "wx/msw/private.h"
#include "wx/winui/private/tlwhostmsw.h"

#include <algorithm>
#include <cstring>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{

const IID IID_ProbeActiveXEvents =
{
    0x67e36a10, 0x5a93, 0x4c65,
    { 0xa1, 0x2d, 0x73, 0x71, 0x36, 0x5e, 0x89, 0x41 }
};

const CLSID CLSID_ProbeActiveX =
{
    0x9e296882, 0xdd35, 0x4bac,
    { 0x98, 0x69, 0x58, 0x50, 0xd4, 0xaa, 0x91, 0x75 }
};

constexpr wchar_t PROBE_NATIVE_HWND_GENERATION_PROP[] =
    L"wxWidgets.WinUI.NativeHwndGeneration."
    L"{9D33D600-8D7B-4FA2-A54C-416E0C39B8D3}";

struct ProbeResources
{
    LONG liveControls = 0;
    LONG liveConnectionPoints = 0;
    LONG liveTypeInfos = 0;
    LONG liveHwnds = 0;
    LONG liveClientSiteEdges = 0;
    unsigned typeAttrsAllocated = 0;
    unsigned typeAttrsReleased = 0;
    unsigned lateSinkFinalReleases = 0;
    unsigned lateSiteFinalReleases = 0;
    unsigned unexpectedLateReleaseCounts = 0;
};

ProbeResources gs_probeResources;

bool gs_deletedAfterControlParentStyle = false;
bool gs_controlParentStyleObserved = false;

void DeleteActiveXAfterControlParentStyle(wxWindow *window)
{
    const HWND hwnd = reinterpret_cast<HWND>(window->GetHandle());
    const HWND parentHwnd = hwnd ? ::GetParent(hwnd) : nullptr;
    gs_controlParentStyleObserved =
        parentHwnd &&
        (::GetWindowLongPtrW(parentHwnd, GWL_EXSTYLE) &
            WS_EX_CONTROLPARENT) != 0;
    gs_deletedAfterControlParentStyle = true;
    // This callback runs before the wxActiveXContainer constructor has
    // returned. Deallocating the object from inside its own new-expression is
    // undefined C++, regardless of how carefully CreateActiveX() avoids this
    // after the callback. Mark it unavailable synchronously and let the
    // fixture perform the actual delete after construction has unwound.
    if ( !wxPendingDelete.Member(window) )
        wxPendingDelete.Append(window);
}

wxActiveXContainer **gs_containerToDeleteAfterHandleDepublished = nullptr;

void DeleteActiveXAfterHandleDepublished(wxWindow *window)
{
    wxActiveXContainer ** const slot =
        gs_containerToDeleteAfterHandleDepublished;
    if ( !slot || *slot != window )
        return;

    wxActiveXContainer * const doomed = *slot;
    *slot = nullptr;
    delete doomed;
}

class ActiveXSignatureProbe : public wxActiveXContainer
{
public:
    ActiveXSignatureProbe(wxWindow *parent, REFIID iid, IUnknown *control)
        : wxActiveXContainer(parent, iid, control)
    {
    }

    using wxActiveXContainer::CreateActiveX;
    using wxActiveXContainer::TryCreateActiveX;
};

static_assert(std::is_same<
    decltype(std::declval<ActiveXSignatureProbe&>().CreateActiveX(
        IID_IUnknown, nullptr)), void>::value,
    "the legacy protected CreateActiveX entry point must remain void");
static_assert(std::is_same<
    decltype(std::declval<ActiveXSignatureProbe&>().TryCreateActiveX(
        IID_IUnknown, nullptr)), bool>::value,
    "transaction status belongs to the distinct helper");

class OleApartment final
{
public:
    OleApartment()
        : m_hr(::OleInitialize(nullptr))
    {
    }

    ~OleApartment()
    {
        if ( m_hr == S_OK || m_hr == S_FALSE )
            ::OleUninitialize();
    }

    bool IsUsable() const
    {
        // OLE controls require an STA. RPC_E_CHANGED_MODE means this thread is
        // already in an incompatible apartment and must not run the harness.
        return SUCCEEDED(m_hr);
    }

private:
    HRESULT m_hr;
};

class ProbeTypeInfo final : public ITypeInfo
{
public:
    enum class Kind
    {
        CoClass,
        Events
    };

    explicit ProbeTypeInfo(Kind kind)
        : m_kind(kind)
    {
        ++gs_probeResources.liveTypeInfos;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **out) override
    {
        if ( !out )
            return E_POINTER;
        *out = nullptr;
        if ( IsEqualIID(iid, IID_IUnknown) ||
             IsEqualIID(iid, IID_ITypeInfo) )
            *out = static_cast<ITypeInfo *>(this);
        if ( !*out )
            return E_NOINTERFACE;
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return static_cast<ULONG>(::InterlockedIncrement(&m_refs));
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        const LONG refs = ::InterlockedDecrement(&m_refs);
        if ( !refs )
            delete this;
        return static_cast<ULONG>(refs);
    }

    HRESULT STDMETHODCALLTYPE GetTypeAttr(TYPEATTR **out) override
    {
        if ( !out )
            return E_POINTER;
        *out = static_cast<TYPEATTR *>(::CoTaskMemAlloc(sizeof(TYPEATTR)));
        if ( !*out )
            return E_OUTOFMEMORY;
        std::memset(*out, 0, sizeof(TYPEATTR));
        ++gs_probeResources.typeAttrsAllocated;
        (*out)->guid = m_kind == Kind::CoClass
            ? CLSID_ProbeActiveX
            : IID_ProbeActiveXEvents;
        (*out)->typekind = m_kind == Kind::CoClass
            ? TKIND_COCLASS
            : TKIND_DISPATCH;
        (*out)->cImplTypes = m_kind == Kind::CoClass ? 1 : 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTypeComp(ITypeComp **out) override
    {
        if ( out )
            *out = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetFuncDesc(UINT, FUNCDESC **out) override
    {
        if ( out )
            *out = nullptr;
        return TYPE_E_ELEMENTNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetVarDesc(UINT, VARDESC **out) override
    {
        if ( out )
            *out = nullptr;
        return TYPE_E_ELEMENTNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetNames(MEMBERID, BSTR *, UINT,
                                       UINT *count) override
    {
        if ( count )
            *count = 0;
        return TYPE_E_ELEMENTNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetRefTypeOfImplType(UINT index,
                                                   HREFTYPE *ref) override
    {
        if ( !ref )
            return E_POINTER;
        if ( m_kind != Kind::CoClass || index != 0 )
            return TYPE_E_ELEMENTNOTFOUND;
        *ref = 1;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetImplTypeFlags(UINT index,
                                               INT *flags) override
    {
        if ( !flags )
            return E_POINTER;
        if ( m_kind != Kind::CoClass || index != 0 )
            return TYPE_E_ELEMENTNOTFOUND;
        *flags = IMPLTYPEFLAG_FDEFAULT | IMPLTYPEFLAG_FSOURCE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(LPOLESTR *, UINT,
                                            MEMBERID *) override
    {
        return DISP_E_UNKNOWNNAME;
    }

    HRESULT STDMETHODCALLTYPE Invoke(PVOID, MEMBERID, WORD, DISPPARAMS *,
                                     VARIANT *, EXCEPINFO *, UINT *) override
    {
        return DISP_E_MEMBERNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetDocumentation(MEMBERID, BSTR *name,
                                               BSTR *doc, DWORD *context,
                                               BSTR *help) override
    {
        if ( name )
            *name = nullptr;
        if ( doc )
            *doc = nullptr;
        if ( context )
            *context = 0;
        if ( help )
            *help = nullptr;
        return TYPE_E_ELEMENTNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetDllEntry(MEMBERID, INVOKEKIND,
                                          BSTR *dll, BSTR *name,
                                          WORD *ordinal) override
    {
        if ( dll )
            *dll = nullptr;
        if ( name )
            *name = nullptr;
        if ( ordinal )
            *ordinal = 0;
        return TYPE_E_ELEMENTNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetRefTypeInfo(HREFTYPE ref,
                                             ITypeInfo **out) override
    {
        if ( !out )
            return E_POINTER;
        *out = nullptr;
        if ( m_kind != Kind::CoClass || ref != 1 )
            return TYPE_E_ELEMENTNOTFOUND;
        *out = new ProbeTypeInfo(Kind::Events);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE AddressOfMember(MEMBERID, INVOKEKIND,
                                              PVOID *address) override
    {
        if ( address )
            *address = nullptr;
        return TYPE_E_ELEMENTNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *, REFIID,
                                             PVOID *out) override
    {
        if ( out )
            *out = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetMops(MEMBERID, BSTR *mops) override
    {
        if ( mops )
            *mops = nullptr;
        return TYPE_E_ELEMENTNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetContainingTypeLib(ITypeLib **typeLib,
                                                   UINT *index) override
    {
        if ( typeLib )
            *typeLib = nullptr;
        if ( index )
            *index = 0;
        return E_NOTIMPL;
    }

    void STDMETHODCALLTYPE ReleaseTypeAttr(TYPEATTR *attr) override
    {
        if ( attr )
            ++gs_probeResources.typeAttrsReleased;
        ::CoTaskMemFree(attr);
    }

    void STDMETHODCALLTYPE ReleaseFuncDesc(FUNCDESC *desc) override
    {
        ::CoTaskMemFree(desc);
    }

    void STDMETHODCALLTYPE ReleaseVarDesc(VARDESC *desc) override
    {
        ::CoTaskMemFree(desc);
    }

private:
    ~ProbeTypeInfo()
    {
        --gs_probeResources.liveTypeInfos;
    }

    LONG m_refs = 1;
    Kind m_kind;
};

class ProbeActiveX;

class ProbeConnectionPoint final : public IConnectionPoint
{
public:
    explicit ProbeConnectionPoint(ProbeActiveX *owner)
        : m_owner(owner)
    {
        ++gs_probeResources.liveConnectionPoints;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **out) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE GetConnectionInterface(IID *iid) override;
    HRESULT STDMETHODCALLTYPE GetConnectionPointContainer(
        IConnectionPointContainer **container) override;
    HRESULT STDMETHODCALLTYPE Advise(IUnknown *sink, DWORD *cookie) override;
    HRESULT STDMETHODCALLTYPE Unadvise(DWORD cookie) override;
    HRESULT STDMETHODCALLTYPE EnumConnections(IEnumConnections **out) override;

    HRESULT Fire();
    HRESULT FireLate();

private:
    ~ProbeConnectionPoint();

    LONG m_refs = 1;
    ProbeActiveX *m_owner;
    IDispatch *m_sink = nullptr;
    IDispatch *m_lateSink = nullptr;
    DWORD m_cookie = 0;
};

class ProbeActiveX final :
    public IOleObject,
    public IOleInPlaceObject,
    public IOleInPlaceActiveObject,
    public IDispatch,
    public IProvideClassInfo,
    public IConnectionPointContainer,
    public IRunnableObject,
    public IPersistStreamInit
{
public:
    enum class Failure
    {
        None,
        OleObjectInterface,
        EventAdvise,
        DocumentAdvise,
        EventUnadvise,
        DocumentUnadvise,
        SetClientSite,
        ContainedObject,
        Run,
        InPlaceObjectInterface,
        MiscStatus,
        InitNew,
        InPlaceActivate,
        Show,
        RecycleHostedWindowBeforeAssociation,
        ReentrantControlParentStyle,
        ResizeGetExtent,
        ResizeSetExtent,
        ResizeSetObjectRects,
        ReentrantResizeGetExtent,
        ReentrantQueryInterface,
        ReentrantAddRef,
        ReentrantReleaseAfterReparent
    };

    explicit ProbeActiveX(Failure failure = Failure::None,
                          bool retainLateCallbacks = false)
        : m_failure(failure),
          m_retainLateCallbacks(retainLateCallbacks),
          m_connectionPoint(new ProbeConnectionPoint(this))
    {
        ++gs_probeResources.liveControls;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **out) override
    {
        if ( !out )
            return E_POINTER;
        *out = nullptr;

        if ( IsEqualIID(iid, IID_IUnknown) )
            *out = static_cast<IOleObject *>(this);
        else if ( IsEqualIID(iid, IID_IOleObject) &&
                  m_failure != Failure::OleObjectInterface )
            *out = static_cast<IOleObject *>(this);
        else if ( IsEqualIID(iid, IID_IOleInPlaceObject) ||
                  IsEqualIID(iid, IID_IOleWindow) )
        {
            if ( m_failure != Failure::InPlaceObjectInterface )
                *out = static_cast<IOleInPlaceObject *>(this);
        }
        else if ( IsEqualIID(iid, IID_IOleInPlaceActiveObject) )
            *out = static_cast<IOleInPlaceActiveObject *>(this);
        else if ( IsEqualIID(iid, IID_IDispatch) )
            *out = static_cast<IDispatch *>(this);
        else if ( IsEqualIID(iid, IID_IProvideClassInfo) )
            *out = static_cast<IProvideClassInfo *>(this);
        else if ( IsEqualIID(iid, IID_IConnectionPointContainer) )
            *out = static_cast<IConnectionPointContainer *>(this);
        else if ( IsEqualIID(iid, IID_IRunnableObject) )
            *out = static_cast<IRunnableObject *>(this);
        else if ( IsEqualIID(iid, IID_IPersist) ||
                  IsEqualIID(iid, IID_IPersistStream) ||
                  IsEqualIID(iid, IID_IPersistStreamInit) )
            *out = static_cast<IPersistStreamInit *>(this);

        if ( !*out )
            return E_NOINTERFACE;

        m_insideQueryInterface = true;
        AddRef();
        m_insideQueryInterface = false;

        if ( IsEqualIID(iid, IID_IOleObject) &&
             m_failure == Failure::ReentrantQueryInterface &&
             eventAdvises && !reentrantCallbackFired )
        {
            reentrantCallbackFired = true;
            reentrantCallbackResult = FireEvent();
        }
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        const LONG refs = ::InterlockedIncrement(&m_refs);
        if ( !m_insideQueryInterface &&
             m_failure == Failure::ReentrantAddRef &&
             eventAdvises && !reentrantCallbackFired )
        {
            reentrantCallbackFired = true;
            reentrantCallbackResult = FireEvent();
        }
        return static_cast<ULONG>(refs);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        const LONG refs = ::InterlockedDecrement(&m_refs);
        wxWindow * const mapped = m_hwnd
            ? wxFindWinFromHandle(m_hwnd)
            : nullptr;
        if ( refs > 0 &&
             m_failure == Failure::ReentrantReleaseAfterReparent &&
             m_showCompleted && mapped && mapped->GetParent() &&
             !reentrantCallbackFired )
        {
            reentrantCallbackFired = true;
            reentrantCallbackResult = FireEvent();
        }
        if ( !refs )
        {
            delete this;
            return 0;
        }
        return static_cast<ULONG>(
            ::InterlockedCompareExchange(&m_refs, 0, 0));
    }

    ULONG RefCountForTest() const
    {
        return static_cast<ULONG>(m_refs);
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *count) override
    {
        if ( !count )
            return E_POINTER;
        *count = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT, LCID, ITypeInfo **out) override
    {
        if ( out )
            *out = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID, LPOLESTR *, UINT, LCID,
                                            DISPID *) override
    {
        return DISP_E_UNKNOWNNAME;
    }

    HRESULT STDMETHODCALLTYPE Invoke(DISPID, REFIID, LCID, WORD,
                                     DISPPARAMS *, VARIANT *, EXCEPINFO *,
                                     UINT *) override
    {
        return DISP_E_MEMBERNOTFOUND;
    }

    HRESULT STDMETHODCALLTYPE GetClassInfo(ITypeInfo **out) override
    {
        if ( !out )
            return E_POINTER;
        *out = new ProbeTypeInfo(ProbeTypeInfo::Kind::CoClass);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnumConnectionPoints(
        IEnumConnectionPoints **out) override
    {
        if ( out )
            *out = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE FindConnectionPoint(REFIID iid,
                                                  IConnectionPoint **out) override
    {
        if ( !out )
            return E_POINTER;
        *out = nullptr;
        if ( !IsEqualIID(iid, IID_ProbeActiveXEvents) )
            return CONNECT_E_NOCONNECTION;
        *out = m_connectionPoint;
        m_connectionPoint->AddRef();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetClientSite(IOleClientSite *site) override
    {
        if ( site && m_failure == Failure::SetClientSite )
        {
            Log("client-site-set-fail");
            return E_FAIL;
        }

        IOleClientSite * const old = m_clientSite;
        m_clientSite = site;
        if ( m_clientSite )
            m_clientSite->AddRef();
        if ( !old && m_clientSite )
            ++gs_probeResources.liveClientSiteEdges;
        else if ( old && !m_clientSite )
            --gs_probeResources.liveClientSiteEdges;
        if ( old )
            old->Release();

        if ( site )
        {
            Log("client-site-set");
            if ( m_retainLateCallbacks && !m_lateSite )
            {
                m_lateSite = site;
                m_lateSite->AddRef();
            }
        }
        else
        {
            Log("client-site-null");
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetClientSite(IOleClientSite **site) override
    {
        if ( !site )
            return E_POINTER;
        *site = m_clientSite;
        if ( *site )
            (*site)->AddRef();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetHostNames(LPCOLESTR, LPCOLESTR) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Close(DWORD) override
    {
        Log("close");
        DestroyHostedWindow();
        m_running = false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetMoniker(DWORD, IMoniker *) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetMoniker(DWORD, DWORD, IMoniker **out) override
    {
        if ( out )
            *out = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE InitFromData(IDataObject *, BOOL, DWORD) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetClipboardData(DWORD, IDataObject **out) override
    {
        if ( out )
            *out = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE DoVerb(LONG verb, LPMSG,
                                     IOleClientSite *clientSite, LONG,
                                     HWND parent, LPCRECT rect) override
    {
        if ( verb == OLEIVERB_INPLACEACTIVATE )
        {
            Log("inplace-activate");
            if ( m_failure == Failure::InPlaceActivate )
                return E_FAIL;
            if ( !m_hwnd )
            {
                const int width = rect ? rect->right - rect->left : 120;
                const int height = rect ? rect->bottom - rect->top : 40;
                const DWORD extendedStyle =
                    m_failure == Failure::ReentrantControlParentStyle
                        ? WS_EX_CONTROLPARENT
                        : 0;
                m_hwnd = ::CreateWindowExW(
                    extendedStyle,
                    L"STATIC", L"wx ActiveX lifetime probe",
                    WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                    0, 0, width, height, parent, nullptr,
                    wxGetInstance(), nullptr);
                if ( !m_hwnd )
                    return HRESULT_FROM_WIN32(::GetLastError());
                m_lastHwnd = m_hwnd;
                ++gs_probeResources.liveHwnds;
                m_originalWndProc = reinterpret_cast<WNDPROC>(
                    ::GetWindowLongPtrW(m_hwnd, GWLP_WNDPROC));
            }

            IOleInPlaceFrame *frame = nullptr;
            if ( clientSite &&
                 SUCCEEDED(clientSite->QueryInterface(
                     IID_IOleInPlaceFrame,
                     reinterpret_cast<void **>(&frame))) )
            {
                frame->SetActiveObject(
                    static_cast<IOleInPlaceActiveObject *>(this), nullptr);
                frame->Release();
            }
            return S_OK;
        }

        if ( verb == OLEIVERB_SHOW )
        {
            Log("show");
            if ( m_failure == Failure::Show )
                return E_FAIL;

            if ( m_failure ==
                    Failure::RecycleHostedWindowBeforeAssociation )
            {
                const unsigned long long previousGeneration =
                    wxWinUIMSWGetNativeHwndGeneration(m_hwnd);
                if ( previousGeneration &&
                     ::RemovePropW(
                         m_hwnd, PROBE_NATIVE_HWND_GENERATION_PROP) )
                {
                    const unsigned long long replacementGeneration =
                        wxWinUIMSWGetNativeHwndGeneration(m_hwnd);
                    recycledBeforeAssociation =
                        replacementGeneration &&
                        replacementGeneration != previousGeneration;
                }
            }

            m_showCompleted = true;
            return S_OK;
        }

        if ( verb == OLEIVERB_HIDE )
        {
            Log("hide");
            if ( m_hwnd )
                ::ShowWindow(m_hwnd, SW_HIDE);
            return S_OK;
        }

        return OLEOBJ_E_NOVERBS;
    }

    HRESULT STDMETHODCALLTYPE EnumVerbs(IEnumOLEVERB **out) override
    {
        if ( out )
            *out = nullptr;
        return OLEOBJ_E_NOVERBS;
    }

    HRESULT STDMETHODCALLTYPE Update() override { return S_OK; }
    HRESULT STDMETHODCALLTYPE IsUpToDate() override { return S_OK; }

    HRESULT STDMETHODCALLTYPE GetUserClassID(CLSID *clsid) override
    {
        if ( !clsid )
            return E_POINTER;
        *clsid = CLSID_ProbeActiveX;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetUserType(DWORD, LPOLESTR *type) override
    {
        if ( type )
            *type = nullptr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SetExtent(DWORD, SIZEL *size) override
    {
        if ( !size )
            return E_POINTER;
        ++setExtentCalls;
        if ( m_failure == Failure::ResizeSetExtent )
            return E_FAIL;
        m_extent = *size;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetExtent(DWORD, SIZEL *size) override
    {
        if ( !size )
            return E_POINTER;
        ++getExtentCalls;
        if ( m_failure == Failure::ResizeGetExtent )
            return E_FAIL;
        if ( m_failure == Failure::ReentrantResizeGetExtent &&
             !reentrantCallbackFired )
        {
            reentrantCallbackFired = true;
            reentrantCallbackResult = FireEvent();
        }
        *size = m_extent;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Advise(IAdviseSink *sink, DWORD *cookie) override
    {
        if ( !sink || !cookie )
            return E_POINTER;
        if ( m_docSink )
            return OLE_E_ADVISENOTSUPPORTED;
        ++docAdviseAttempts;
        m_docSink = sink;
        m_docSink->AddRef();
        *cookie = m_docCookie = 0xD015;
        ++docAdvises;
        Log("doc-advise");

        // Exercise the pending window before Advise() returns its cookie.
        ++docAdviseCallbacks;
        sink->OnViewChange(DVASPECT_CONTENT, -1);
        return m_failure == Failure::DocumentAdvise ? E_FAIL : S_OK;
    }

    HRESULT STDMETHODCALLTYPE Unadvise(DWORD cookie) override
    {
        if ( !m_docSink || cookie != m_docCookie )
            return OLE_E_NOCONNECTION;
        IAdviseSink * const sink = m_docSink;
        m_docSink = nullptr;
        m_docCookie = 0;
        ++docUnadvises;
        Log("doc-unadvise");
        ++docUnadviseCallbacks;
        sink->OnViewChange(DVASPECT_CONTENT, -1);
        sink->Release();
        return m_failure == Failure::DocumentUnadvise ? E_FAIL : S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnumAdvise(IEnumSTATDATA **out) override
    {
        if ( out )
            *out = nullptr;
        return OLE_E_ADVISENOTSUPPORTED;
    }

    HRESULT STDMETHODCALLTYPE GetMiscStatus(DWORD, DWORD *status) override
    {
        if ( !status )
            return E_POINTER;
        if ( m_failure == Failure::MiscStatus )
            return E_FAIL;
        *status = OLEMISC_SETCLIENTSITEFIRST;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetColorScheme(LOGPALETTE *) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetWindow(HWND *hwnd) override
    {
        if ( !hwnd )
            return E_POINTER;
        *hwnd = m_hwnd;
        return m_hwnd && ::IsWindow(m_hwnd) ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE InPlaceDeactivate() override
    {
        Log("inplace-deactivate");
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE UIDeactivate() override
    {
        Log("ui-deactivate");
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetObjectRects(LPCRECT, LPCRECT) override
    {
        ++setObjectRectsCalls;
        if ( m_failure == Failure::ResizeSetObjectRects &&
             m_showCompleted )
        {
            return E_FAIL;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ReactivateAndUndo() override
    {
        return E_NOTIMPL;
    }

    // IOleInPlaceActiveObject
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG) override
    {
        ++translateAcceleratorCalls;
        return S_FALSE;
    }

    HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(BOOL active) override
    {
        ++frameActivationCalls;
        lastFrameActivation = active != FALSE;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDocWindowActivate(BOOL) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ResizeBorder(
        LPCRECT, IOleInPlaceUIWindow *, BOOL) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetRunningClass(LPCLSID clsid) override
    {
        if ( !clsid )
            return E_POINTER;
        *clsid = CLSID_ProbeActiveX;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Run(LPBINDCTX) override
    {
        if ( m_failure == Failure::Run )
            return E_FAIL;
        m_running = true;
        return S_OK;
    }

    BOOL STDMETHODCALLTYPE IsRunning() override
    {
        return m_running ? TRUE : FALSE;
    }

    HRESULT STDMETHODCALLTYPE LockRunning(BOOL, BOOL) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetContainedObject(BOOL) override
    {
        return m_failure == Failure::ContainedObject ? E_FAIL : S_OK;
    }

    // IPersistStreamInit
    HRESULT STDMETHODCALLTYPE GetClassID(CLSID *clsid) override
    {
        if ( !clsid )
            return E_POINTER;
        *clsid = CLSID_ProbeActiveX;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE IsDirty() override { return S_FALSE; }
    HRESULT STDMETHODCALLTYPE Load(IStream *) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Save(IStream *, BOOL) override
    {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetSizeMax(ULARGE_INTEGER *size) override
    {
        if ( !size )
            return E_POINTER;
        size->QuadPart = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE InitNew() override
    {
        return m_failure == Failure::InitNew ? E_FAIL : S_OK;
    }

    HRESULT FireEvent()
    {
        return m_connectionPoint->Fire();
    }

    HRESULT ActivateDocumentView(IOleDocumentView *view)
    {
        if ( !m_clientSite || !view )
            return E_INVALIDARG;

        IOleDocumentSite *site = nullptr;
        HRESULT hr = m_clientSite->QueryInterface(
            IID_IOleDocumentSite,
            reinterpret_cast<void **>(&site));
        if ( SUCCEEDED(hr) )
        {
            hr = site->ActivateMe(view);
            site->Release();
        }
        return hr;
    }

    HRESULT FireLateEvent()
    {
        return m_connectionPoint->FireLate();
    }

    HRESULT CallLateSite() const
    {
        if ( !m_lateSite )
            return E_UNEXPECTED;
        IOleWindow *window = nullptr;
        HRESULT hr = m_lateSite->QueryInterface(
            IID_IOleWindow, reinterpret_cast<void **>(&window));
        if ( SUCCEEDED(hr) )
        {
            HWND hwnd = reinterpret_cast<HWND>(1);
            hr = window->GetWindow(&hwnd);
            if ( SUCCEEDED(hr) && hwnd )
                hr = E_UNEXPECTED;
            window->Release();
        }
        return hr;
    }

    void DestroyHostedWindow()
    {
        const HWND hwnd = m_hwnd;
        if ( !hwnd || !::IsWindow(hwnd) )
            return;

        if ( m_expectForeignWndProc )
        {
            foreignWndProcPreserved =
                reinterpret_cast<WNDPROC>(
                    ::GetWindowLongPtrW(hwnd, GWLP_WNDPROC)) ==
                m_originalWndProc;
        }

        if ( ::DestroyWindow(hwnd) )
        {
            m_hwnd = nullptr;
            --gs_probeResources.liveHwnds;
        }
    }

    bool PrepareForeignNativeGeneration(
        unsigned long long previousGeneration)
    {
        if ( !m_hwnd || !::IsWindow(m_hwnd) || !m_originalWndProc )
            return false;

        ::SetLastError(ERROR_SUCCESS);
        const LONG_PTR previousProc = ::SetWindowLongPtrW(
            m_hwnd, GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(m_originalWndProc));
        if ( !previousProc && ::GetLastError() != ERROR_SUCCESS )
            return false;

        if ( !::RemovePropW(m_hwnd, PROBE_NATIVE_HWND_GENERATION_PROP) )
            return false;

        const unsigned long long replacementGeneration =
            wxWinUIMSWGetNativeHwndGeneration(m_hwnd);
        m_expectForeignWndProc = replacementGeneration != 0 &&
            replacementGeneration != previousGeneration;
        return m_expectForeignWndProc;
    }

    HWND GetHostedWindow() const { return m_hwnd; }
    HWND GetLastHostedWindow() const { return m_lastHwnd; }
    void ResetExtentForTest() { m_extent = { 0, 0 }; }

    void Log(const wxString& value)
    {
        calls.push_back(value);
    }

    bool RetainLateCallbacks() const { return m_retainLateCallbacks; }

    std::vector<wxString> calls;
    unsigned eventAdvises = 0;
    unsigned eventAdviseAttempts = 0;
    unsigned eventUnadvises = 0;
    HRESULT eventAdviseCallbackResult = E_UNEXPECTED;
    HRESULT eventUnadviseCallbackResult = E_UNEXPECTED;
    unsigned docAdvises = 0;
    unsigned docAdviseAttempts = 0;
    unsigned docAdviseCallbacks = 0;
    unsigned docUnadvises = 0;
    unsigned docUnadviseCallbacks = 0;
    unsigned getExtentCalls = 0;
    unsigned setExtentCalls = 0;
    unsigned setObjectRectsCalls = 0;
    unsigned frameActivationCalls = 0;
    unsigned translateAcceleratorCalls = 0;
    bool lastFrameActivation = false;
    bool foreignWndProcPreserved = false;
    bool recycledBeforeAssociation = false;
    bool reentrantCallbackFired = false;
    HRESULT reentrantCallbackResult = E_UNEXPECTED;

private:
    ~ProbeActiveX()
    {
        DestroyHostedWindow();
        if ( m_docSink )
            m_docSink->Release();
        if ( m_clientSite )
        {
            --gs_probeResources.liveClientSiteEdges;
            m_clientSite->Release();
        }
        if ( m_lateSite )
        {
            const ULONG refs = m_lateSite->Release();
            if ( refs == 0 )
                ++gs_probeResources.lateSiteFinalReleases;
            else
                ++gs_probeResources.unexpectedLateReleaseCounts;
        }
        m_connectionPoint->Release();
        --gs_probeResources.liveControls;
    }

    LONG m_refs = 1;
    Failure m_failure;
    bool m_retainLateCallbacks;
    ProbeConnectionPoint *m_connectionPoint;
    IOleClientSite *m_clientSite = nullptr;
    IOleClientSite *m_lateSite = nullptr;
    IAdviseSink *m_docSink = nullptr;
    DWORD m_docCookie = 0;
    HWND m_hwnd = nullptr;
    HWND m_lastHwnd = nullptr;
    WNDPROC m_originalWndProc = nullptr;
    SIZEL m_extent = { 0, 0 };
    bool m_running = false;
    bool m_expectForeignWndProc = false;
    bool m_insideQueryInterface = false;
    bool m_showCompleted = false;

    friend class ProbeConnectionPoint;
};

class ProbeDocumentView final : public IOleDocumentView
{
public:
    explicit ProbeDocumentView(ProbeActiveX *owner)
        : m_owner(owner)
    {
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **out) override
    {
        if ( !out )
            return E_POINTER;
        *out = nullptr;
        if ( IsEqualIID(iid, IID_IUnknown) ||
             IsEqualIID(iid, IID_IOleDocumentView) )
        {
            *out = static_cast<IOleDocumentView *>(this);
        }
        if ( !*out )
            return E_NOINTERFACE;
        AddRef();
        return S_OK;
    }

    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return static_cast<ULONG>(::InterlockedIncrement(&m_refs));
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        const LONG refs = ::InterlockedDecrement(&m_refs);
        if ( refs > 0 && m_fireOnRelease && !releaseCallbackFired )
        {
            releaseCallbackFired = true;
            releaseCallbackResult = m_owner->FireEvent();
        }
        if ( !refs )
        {
            delete this;
            return 0;
        }
        return static_cast<ULONG>(
            ::InterlockedCompareExchange(&m_refs, 0, 0));
    }

    HRESULT STDMETHODCALLTYPE SetInPlaceSite(IOleInPlaceSite *site) override
    {
        if ( site )
            ++setInPlaceSiteCalls;
        else
            ++clearInPlaceSiteCalls;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetInPlaceSite(IOleInPlaceSite **site) override
    {
        if ( !site )
            return E_POINTER;
        *site = nullptr;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetDocument(IUnknown **document) override
    {
        if ( document )
            *document = nullptr;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE SetRect(LPRECT) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE GetRect(LPRECT rect) override
    {
        if ( !rect )
            return E_POINTER;
        ::SetRectEmpty(rect);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetRectComplex(LPRECT, LPRECT, LPRECT,
                                              LPRECT) override
    {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Show(BOOL) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE UIActivate(BOOL active) override
    {
        if ( active )
            ++uiActivateCalls;
        else
            ++uiDeactivateCalls;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Open() override { return S_OK; }
    HRESULT STDMETHODCALLTYPE CloseView(DWORD) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE SaveViewState(IStream *) override
    {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ApplyViewState(IStream *) override
    {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Clone(IOleInPlaceSite *,
                                    IOleDocumentView **view) override
    {
        if ( view )
            *view = nullptr;
        return E_NOTIMPL;
    }

    void ArmReleaseCallback() { m_fireOnRelease = true; }

    unsigned setInPlaceSiteCalls = 0;
    unsigned clearInPlaceSiteCalls = 0;
    unsigned uiActivateCalls = 0;
    unsigned uiDeactivateCalls = 0;
    bool releaseCallbackFired = false;
    HRESULT releaseCallbackResult = E_UNEXPECTED;

private:
    ~ProbeDocumentView() = default;

    LONG m_refs = 1;
    ProbeActiveX *m_owner;
    bool m_fireOnRelease = false;
};

HRESULT STDMETHODCALLTYPE
ProbeConnectionPoint::QueryInterface(REFIID iid, void **out)
{
    if ( !out )
        return E_POINTER;
    *out = nullptr;
    if ( IsEqualIID(iid, IID_IUnknown) ||
         IsEqualIID(iid, IID_IConnectionPoint) )
        *out = static_cast<IConnectionPoint *>(this);
    if ( !*out )
        return E_NOINTERFACE;
    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE ProbeConnectionPoint::AddRef()
{
    return static_cast<ULONG>(::InterlockedIncrement(&m_refs));
}

ULONG STDMETHODCALLTYPE ProbeConnectionPoint::Release()
{
    const LONG refs = ::InterlockedDecrement(&m_refs);
    if ( !refs )
        delete this;
    return static_cast<ULONG>(refs);
}

HRESULT STDMETHODCALLTYPE
ProbeConnectionPoint::GetConnectionInterface(IID *iid)
{
    if ( !iid )
        return E_POINTER;
    *iid = IID_ProbeActiveXEvents;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE ProbeConnectionPoint::GetConnectionPointContainer(
    IConnectionPointContainer **container)
{
    return m_owner->QueryInterface(
        IID_IConnectionPointContainer,
        reinterpret_cast<void **>(container));
}

HRESULT STDMETHODCALLTYPE
ProbeConnectionPoint::Advise(IUnknown *sink, DWORD *cookie)
{
    if ( !sink || !cookie )
        return E_POINTER;
    if ( m_sink )
        return CONNECT_E_ADVISELIMIT;

    ++m_owner->eventAdviseAttempts;
    IDispatch *candidate = nullptr;
    HRESULT hr = sink->QueryInterface(
        IID_ProbeActiveXEvents, reinterpret_cast<void **>(&candidate));
    if ( FAILED(hr) )
        return CONNECT_E_CANNOTCONNECT;

    // Invoke before publishing/returning the cookie: synchronous delivery is
    // permitted while Creating, and teardown must defer until this pending
    // Advise transaction has received its cookie.
    DISPPARAMS pendingParams = {};
    m_owner->eventAdviseCallbackResult = candidate->Invoke(
        70, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD,
        &pendingParams, nullptr, nullptr, nullptr);

    m_sink = candidate;
    m_cookie = 0xE015;
    *cookie = m_cookie;
    if ( m_owner->RetainLateCallbacks() )
    {
        m_lateSink = m_sink;
        m_lateSink->AddRef();
    }
    ++m_owner->eventAdvises;
    m_owner->Log("event-advise");
    return m_owner->m_failure == ProbeActiveX::Failure::EventAdvise
        ? E_FAIL
        : S_OK;
}

HRESULT STDMETHODCALLTYPE ProbeConnectionPoint::Unadvise(DWORD cookie)
{
    if ( !m_sink || cookie != m_cookie )
        return CONNECT_E_NOCONNECTION;

    IDispatch * const sink = m_sink;
    m_sink = nullptr;
    m_cookie = 0;
    ++m_owner->eventUnadvises;
    m_owner->Log("event-unadvise");

    DISPPARAMS reentrantParams = {};
    m_owner->eventUnadviseCallbackResult = sink->Invoke(
        73, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD,
        &reentrantParams, nullptr, nullptr, nullptr);
    sink->Release();
    return m_owner->m_failure == ProbeActiveX::Failure::EventUnadvise
        ? E_FAIL
        : S_OK;
}

HRESULT STDMETHODCALLTYPE
ProbeConnectionPoint::EnumConnections(IEnumConnections **out)
{
    if ( out )
        *out = nullptr;
    return E_NOTIMPL;
}

HRESULT ProbeConnectionPoint::Fire()
{
    if ( !m_sink )
        return CONNECT_E_NOCONNECTION;
    DISPPARAMS params = {};
    return m_sink->Invoke(71, IID_NULL, LOCALE_USER_DEFAULT,
                          DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
}

HRESULT ProbeConnectionPoint::FireLate()
{
    if ( !m_lateSink )
        return CONNECT_E_NOCONNECTION;
    DISPPARAMS params = {};
    return m_lateSink->Invoke(72, IID_NULL, LOCALE_USER_DEFAULT,
                              DISPATCH_METHOD, &params,
                              nullptr, nullptr, nullptr);
}

ProbeConnectionPoint::~ProbeConnectionPoint()
{
    if ( m_sink )
        m_sink->Release();
    if ( m_lateSink )
    {
        const ULONG refs = m_lateSink->Release();
        if ( refs == 0 )
            ++gs_probeResources.lateSinkFinalReleases;
        else
            ++gs_probeResources.unexpectedLateReleaseCounts;
    }
    --gs_probeResources.liveConnectionPoints;
}

class LifecycleActiveXEventFilter final : public wxEventFilter
{
public:
    enum class Action
    {
        CaptureOnly,
        ReenterCreation,
        DeletePendingAdvise,
        DeleteServerCallback,
        DeleteCreate,
        ScheduleCreate
    };

    explicit LifecycleActiveXEventFilter(Action action)
        : m_action(action)
    {
        wxEvtHandler::AddFilter(this);
    }

    ~LifecycleActiveXEventFilter() override
    {
        wxEvtHandler::RemoveFilter(this);
    }

    int FilterEvent(wxEvent& event) override
    {
        if ( event.GetEventType() == wxEVT_ACTIVEX )
        {
            wxActiveXEvent& activeXEvent =
                static_cast<wxActiveXEvent&>(event);
            m_target = static_cast<wxActiveXContainer *>(
                event.GetEventObject());
            const bool deleteNow =
                (m_action == Action::DeletePendingAdvise &&
                 activeXEvent.GetDispatchId() == 70) ||
                (m_action == Action::DeleteServerCallback &&
                 activeXEvent.GetDispatchId() == 71);
            if ( deleteNow && DestroyCaptured() )
                return Event_Processed;
            if ( m_action == Action::ReenterCreation &&
                 activeXEvent.GetDispatchId() == 70 &&
                 m_control && !m_didReenter )
            {
                m_didReenter = true;
                m_reentryResult =
                    static_cast<ActiveXSignatureProbe *>(m_target)
                        ->TryCreateActiveX(IID_IUnknown, m_control);
            }
        }
        else if ( event.GetEventType() == wxEVT_CREATE &&
                  event.GetEventObject() == m_target )
        {
            if ( m_action == Action::DeleteCreate )
            {
                if ( DestroyCaptured() )
                    return Event_Processed;
            }
            else if ( m_action == Action::ScheduleCreate && !m_scheduled )
            {
                wxPendingDelete.Append(m_target);
                m_scheduled = true;
                return Event_Processed;
            }
        }

        return Event_Skip;
    }

    bool DestroyCaptured()
    {
        wxActiveXContainer * const doomed = m_target;
        if ( !doomed || m_deleted )
            return false;

        m_target = nullptr;
        m_deleted = true;
        // All destructive actions exercised by this filter occur while the
        // wxActiveXContainer constructor is still active. Pending deletion is
        // observable immediately by wxWindowIsUnavailableForCallbacks(), but
        // avoids ending and deallocating the C++ object before its new-
        // expression has returned.
        if ( !wxPendingDelete.Member(doomed) )
            wxPendingDelete.Append(doomed);
        return true;
    }

    bool DidDelete() const { return m_deleted; }
    void SetControl(IUnknown *control) { m_control = control; }
    bool NeedsSignatureProbe() const
    {
        return m_action == Action::ReenterCreation;
    }
    bool DidReenter() const { return m_didReenter; }
    bool ReentryResult() const { return m_reentryResult; }
    bool DidSchedule() const { return m_scheduled; }
    wxActiveXContainer *GetCaptured() const { return m_target; }

private:
    Action m_action;
    wxActiveXContainer *m_target = nullptr;
    IUnknown *m_control = nullptr;
    bool m_deleted = false;
    bool m_scheduled = false;
    bool m_didReenter = false;
    bool m_reentryResult = true;
};

class ReentrantActiveXLogTarget final : public wxLog
{
public:
    explicit ReentrantActiveXLogTarget(
        LifecycleActiveXEventFilter& filter)
        : m_filter(filter),
          m_previous(wxLog::SetActiveTarget(this))
    {
    }

    ~ReentrantActiveXLogTarget() override
    {
        wxLog::SetActiveTarget(m_previous);
    }

    bool DidDelete() const { return m_deleted; }

protected:
    void DoLogTextAtLevel(wxLogLevel, const wxString&) override
    {
        // wxLog's base implementation sends Debug/Trace directly to
        // wxMessageOutputDebug instead of calling DoLogText(). Override this
        // level-aware seam so the expected Release-build debug record remains
        // a deterministic re-entrancy boundary.
        if ( !m_deleted )
            m_deleted = m_filter.DestroyCaptured();
    }

private:
    LifecycleActiveXEventFilter& m_filter;
    wxLog *m_previous;
    bool m_deleted = false;
};

class ActiveXFixture final
{
public:
    ActiveXFixture(ProbeActiveX::Failure failure = ProbeActiveX::Failure::None,
                   bool retainLateCallbacks = false,
                   LifecycleActiveXEventFilter *lifecycleFilter = nullptr)
    {
        frame = new wxFrame(nullptr, wxID_ANY, "activex-lifetime-probe",
                            wxPoint(-24000, -24000), wxSize(320, 180));
        parent = new wxPanel(frame, wxID_ANY,
                             wxPoint(0, 0), wxSize(300, 150));
        control = new ProbeActiveX(failure, retainLateCallbacks);
        if ( lifecycleFilter )
            lifecycleFilter->SetControl(static_cast<IOleObject *>(control));
        const bool armControlParentCallback =
            failure == ProbeActiveX::Failure::ReentrantControlParentStyle;
        if ( armControlParentCallback )
        {
            wxWinUIMSWSetAfterEnsureControlParentStyleForTest(
                DeleteActiveXAfterControlParentStyle);
        }
        if ( lifecycleFilter && lifecycleFilter->NeedsSignatureProbe() )
        {
            container = new ActiveXSignatureProbe(
                parent, IID_IUnknown, static_cast<IOleObject *>(control));
        }
        else
        {
            container = new wxActiveXContainer(
                parent, IID_IUnknown, static_cast<IOleObject *>(control));
        }
        if ( armControlParentCallback )
            wxWinUIMSWSetAfterEnsureControlParentStyleForTest(nullptr);
        if ( (lifecycleFilter && lifecycleFilter->DidDelete()) ||
             gs_deletedAfterControlParentStyle )
        {
            // The callback made the object unavailable while construction was
            // active. Complete the requested destruction only now, after the
            // new-expression has returned and the C++ lifetime can end safely.
            DeleteContainer();
        }
    }

    ~ActiveXFixture()
    {
        DeleteContainer();
        if ( control )
            control->Release();
        delete frame;
    }

    void DeleteContainer()
    {
        wxActiveXContainer * const doomed = container;
        container = nullptr;
        delete doomed;
    }

    wxFrame *frame = nullptr;
    wxPanel *parent = nullptr;
    ProbeActiveX *control = nullptr;
    wxActiveXContainer *container = nullptr;
};

size_t CallIndex(const ProbeActiveX& control, const wxString& call)
{
    const auto it = std::find(control.calls.begin(), control.calls.end(), call);
    return it == control.calls.end()
        ? control.calls.size()
        : static_cast<size_t>(it - control.calls.begin());
}

} // anonymous namespace

TEST_CASE("WinUI ActiveX rolls back every acquired lifetime edge",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    wxLogNull suppressExpectedFailures;

    const LONG controlsBefore = gs_probeResources.liveControls;
    const LONG pointsBefore = gs_probeResources.liveConnectionPoints;
    const LONG typeInfosBefore = gs_probeResources.liveTypeInfos;
    const LONG hwndsBefore = gs_probeResources.liveHwnds;
    const LONG siteEdgesBefore = gs_probeResources.liveClientSiteEdges;
    const unsigned attrsAllocatedBefore =
        gs_probeResources.typeAttrsAllocated;
    const unsigned attrsReleasedBefore =
        gs_probeResources.typeAttrsReleased;

    const ProbeActiveX::Failure failures[] =
    {
        ProbeActiveX::Failure::OleObjectInterface,
        ProbeActiveX::Failure::ContainedObject,
        ProbeActiveX::Failure::Run,
        ProbeActiveX::Failure::InPlaceObjectInterface,
        ProbeActiveX::Failure::MiscStatus,
        ProbeActiveX::Failure::SetClientSite,
        ProbeActiveX::Failure::InitNew,
        ProbeActiveX::Failure::InPlaceActivate,
        ProbeActiveX::Failure::Show
    };

    for ( const ProbeActiveX::Failure failure : failures )
    {
        INFO("failure stage " << static_cast<int>(failure));
        ActiveXFixture fixture(failure);
        CHECK(fixture.container->GetHWND() == nullptr);
        fixture.DeleteContainer();

        CHECK(fixture.control->eventAdvises == 1);
        CHECK(fixture.control->eventUnadvises == 1);
        if ( failure == ProbeActiveX::Failure::OleObjectInterface )
        {
            CHECK(fixture.control->docAdvises == 0);
            CHECK(fixture.control->docUnadvises == 0);
        }
        else
        {
            CHECK(fixture.control->docAdvises == 1);
            CHECK(fixture.control->docUnadvises == 1);
        }
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(fixture.control->GetHostedWindow() == nullptr);
        if ( fixture.control->GetLastHostedWindow() )
        {
            CHECK(wxFindWinFromHandle(
                      fixture.control->GetLastHostedWindow()) == nullptr);
        }
    }

    CHECK(gs_probeResources.liveControls == controlsBefore);
    CHECK(gs_probeResources.liveConnectionPoints == pointsBefore);
    CHECK(gs_probeResources.liveTypeInfos == typeInfosBefore);
    CHECK(gs_probeResources.liveHwnds == hwndsBefore);
    CHECK(gs_probeResources.liveClientSiteEdges == siteEdgesBefore);
    CHECK(gs_probeResources.typeAttrsAllocated - attrsAllocatedBefore ==
          gs_probeResources.typeAttrsReleased - attrsReleasedBefore);
}

TEST_CASE("WinUI ActiveX advise transactions survive pending callbacks and errors",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    wxLogNull suppressExpectedFailures;

    const ProbeActiveX::Failure failures[] =
    {
        ProbeActiveX::Failure::EventAdvise,
        ProbeActiveX::Failure::DocumentAdvise,
        ProbeActiveX::Failure::EventUnadvise,
        ProbeActiveX::Failure::DocumentUnadvise
    };

    for ( const ProbeActiveX::Failure failure : failures )
    {
        INFO("advise failure stage " << static_cast<int>(failure));
        ActiveXFixture fixture(failure);
        REQUIRE(fixture.container->GetHWND() != nullptr);

        CHECK(fixture.control->eventAdviseAttempts == 1);
        CHECK(fixture.control->eventAdviseCallbackResult == S_OK);
        CHECK(fixture.control->docAdviseAttempts == 1);
        CHECK(fixture.control->docAdviseCallbacks == 1);

        fixture.DeleteContainer();

        CHECK(fixture.control->eventAdvises == 1);
        CHECK(fixture.control->eventUnadvises == 1);
        CHECK(fixture.control->docAdvises == 1);
        CHECK(fixture.control->docUnadvises == 1);
        CHECK(fixture.control->eventUnadviseCallbackResult ==
              RPC_E_DISCONNECTED);
        CHECK(fixture.control->docUnadviseCallbacks == 1);
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(fixture.control->GetHostedWindow() == nullptr);
    }
}

TEST_CASE("WinUI ActiveX cancels while connection-point Advise is pending",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    LifecycleActiveXEventFilter filter(
        LifecycleActiveXEventFilter::Action::DeletePendingAdvise);
    ActiveXFixture fixture(
        ProbeActiveX::Failure::None, false, &filter);

    CHECK(filter.DidDelete());
    CHECK(fixture.container == nullptr);
    CHECK(fixture.control->eventAdviseAttempts == 1);
    CHECK(fixture.control->eventAdvises == 1);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->eventAdviseCallbackResult == S_OK);
    CHECK(fixture.control->docAdviseAttempts == 0);
    CHECK(fixture.control->docAdvises == 0);
    CHECK(fixture.control->docUnadvises == 0);
    CHECK(fixture.control->RefCountForTest() == 1);
    CHECK(fixture.control->GetHostedWindow() == nullptr);
    CHECK(gs_probeResources.liveClientSiteEdges == 0);
}

TEST_CASE("WinUI ActiveX rejects recursive creation from pending Advise",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    LifecycleActiveXEventFilter filter(
        LifecycleActiveXEventFilter::Action::ReenterCreation);
    ActiveXFixture fixture(
        ProbeActiveX::Failure::None, false, &filter);

    REQUIRE(fixture.container != nullptr);
    CHECK(filter.DidReenter());
    CHECK_FALSE(filter.ReentryResult());
    CHECK(fixture.container->GetHWND() != nullptr);
    fixture.DeleteContainer();
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
}

TEST_CASE("WinUI ActiveX publishes no interface after reentrant QI or AddRef",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    const ProbeActiveX::Failure callbacks[] =
    {
        ProbeActiveX::Failure::ReentrantQueryInterface,
        ProbeActiveX::Failure::ReentrantAddRef
    };

    for ( const ProbeActiveX::Failure callback : callbacks )
    {
        INFO("reentrant acquisition " << static_cast<int>(callback));
        LifecycleActiveXEventFilter filter(
            LifecycleActiveXEventFilter::Action::DeleteServerCallback);
        ActiveXFixture fixture(callback, false, &filter);

        CHECK(filter.DidDelete());
        CHECK(fixture.container == nullptr);
        CHECK(fixture.control->reentrantCallbackFired);
        CHECK(fixture.control->reentrantCallbackResult == S_OK);
        CHECK(fixture.control->eventAdvises == 1);
        CHECK(fixture.control->eventUnadvises == 1);
        CHECK(fixture.control->docAdvises == 0);
        CHECK(fixture.control->docUnadvises == 0);
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(fixture.control->GetHostedWindow() == nullptr);
        CHECK(gs_probeResources.liveClientSiteEdges == 0);
    }
}

TEST_CASE("WinUI ActiveX survives destruction from create and post-reparent callbacks",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    {
        gs_deletedAfterControlParentStyle = false;
        gs_controlParentStyleObserved = false;
        ActiveXFixture fixture(
            ProbeActiveX::Failure::ReentrantControlParentStyle);
        const bool deletedAfterStyle =
            gs_deletedAfterControlParentStyle;
        const bool observedControlParentStyle =
            gs_controlParentStyleObserved;
        gs_deletedAfterControlParentStyle = false;
        gs_controlParentStyleObserved = false;

        CHECK(deletedAfterStyle);
        CHECK(observedControlParentStyle);
        CHECK(fixture.container == nullptr);
        CHECK(fixture.control->eventUnadvises == 1);
        CHECK(fixture.control->docUnadvises == 1);
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(fixture.control->GetHostedWindow() == nullptr);
        CHECK(wxFindWinFromHandle(
                  fixture.control->GetLastHostedWindow()) == nullptr);
    }

    {
        LifecycleActiveXEventFilter filter(
            LifecycleActiveXEventFilter::Action::DeleteCreate);
        ActiveXFixture fixture(
            ProbeActiveX::Failure::None, false, &filter);
        CHECK(filter.DidDelete());
        CHECK(fixture.container == nullptr);
        CHECK(fixture.control->eventUnadvises == 1);
        CHECK(fixture.control->docUnadvises == 1);
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(fixture.control->GetHostedWindow() == nullptr);
        CHECK(wxFindWinFromHandle(
                  fixture.control->GetLastHostedWindow()) == nullptr);
    }

    {
        LifecycleActiveXEventFilter filter(
            LifecycleActiveXEventFilter::Action::ScheduleCreate);
        ActiveXFixture fixture(
            ProbeActiveX::Failure::None, false, &filter);
        REQUIRE(filter.DidSchedule());
        REQUIRE(fixture.container != nullptr);
        CHECK(wxPendingDelete.Member(fixture.container));
        CHECK(fixture.control->FireEvent() == RPC_E_DISCONNECTED);

        fixture.DeleteContainer();
        CHECK(fixture.control->eventUnadvises == 1);
        CHECK(fixture.control->docUnadvises == 1);
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(fixture.control->GetHostedWindow() == nullptr);
        CHECK(wxFindWinFromHandle(
                  fixture.control->GetLastHostedWindow()) == nullptr);
    }

    {
        LifecycleActiveXEventFilter filter(
            LifecycleActiveXEventFilter::Action::DeleteServerCallback);
        ActiveXFixture fixture(
            ProbeActiveX::Failure::ReentrantReleaseAfterReparent,
            false, &filter);
        CHECK(filter.DidDelete());
        CHECK(fixture.container == nullptr);
        CHECK(fixture.control->reentrantCallbackFired);
        CHECK(fixture.control->reentrantCallbackResult == S_OK);
        CHECK(fixture.control->eventUnadvises == 1);
        CHECK(fixture.control->docUnadvises == 1);
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(fixture.control->GetHostedWindow() == nullptr);
        CHECK(wxFindWinFromHandle(
                  fixture.control->GetLastHostedWindow()) == nullptr);
    }
}

TEST_CASE("WinUI ActiveX rejects a recycled HWND before association",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    ActiveXFixture fixture(
        ProbeActiveX::Failure::RecycleHostedWindowBeforeAssociation);

    CHECK(fixture.control->recycledBeforeAssociation);
    CHECK(fixture.container->GetHWND() == nullptr);
    CHECK(fixture.control->GetHostedWindow() == nullptr);
    CHECK(wxFindWinFromHandle(
              fixture.control->GetLastHostedWindow()) == nullptr);

    fixture.DeleteContainer();
    CHECK(fixture.control->eventAdvises == 1);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docAdvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
    CHECK(gs_probeResources.liveClientSiteEdges == 0);
}

TEST_CASE("WinUI ActiveX revalidates after replacing a document view",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    ActiveXFixture fixture;
    REQUIRE(fixture.container->GetHWND() != nullptr);

    ProbeDocumentView * const firstProbe =
        new ProbeDocumentView(fixture.control);
    wxAutoIOleDocumentView first(firstProbe);
    ProbeDocumentView * const secondProbe =
        new ProbeDocumentView(fixture.control);
    wxAutoIOleDocumentView second(secondProbe);

    REQUIRE(fixture.control->ActivateDocumentView(first) == S_OK);
    REQUIRE(firstProbe->setInPlaceSiteCalls == 1);
    REQUIRE(firstProbe->uiActivateCalls == 1);

    unsigned events = 0;
    fixture.container->Bind(
        wxEVT_ACTIVEX,
        [&](wxActiveXEvent&)
        {
            ++events;
            fixture.DeleteContainer();
        });
    firstProbe->ArmReleaseCallback();

    CHECK(fixture.control->ActivateDocumentView(second) ==
          CO_E_OBJNOTCONNECTED);
    CHECK(firstProbe->releaseCallbackFired);
    CHECK(firstProbe->releaseCallbackResult == S_OK);
    CHECK(firstProbe->clearInPlaceSiteCalls == 1);
    CHECK(firstProbe->uiDeactivateCalls == 1);
    CHECK(events == 1);
    CHECK(fixture.container == nullptr);
    CHECK(secondProbe->setInPlaceSiteCalls == 0);
    CHECK(secondProbe->clearInPlaceSiteCalls == 0);
    CHECK(secondProbe->uiActivateCalls == 0);
    CHECK(secondProbe->uiDeactivateCalls == 0);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
    CHECK(gs_probeResources.liveClientSiteEdges == 0);

    IOleDocumentView * const firstFinal = first.Detach();
    IOleDocumentView * const secondFinal = second.Detach();
    CHECK(firstFinal->Release() == 0);
    CHECK(secondFinal->Release() == 0);
}

TEST_CASE("WinUI ActiveX detach survives destruction after HWND depublish",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    ActiveXFixture fixture;
    wxActiveXContainer * const container = fixture.container;
    const HWND hwnd = fixture.control->GetHostedWindow();
    REQUIRE(container != nullptr);
    REQUIRE(hwnd != nullptr);
    REQUIRE(wxFindWinFromHandle(hwnd) == container);

    gs_containerToDeleteAfterHandleDepublished = &fixture.container;
    wxWinUIMSWSetAfterHandleDepublishedForTest(
        DeleteActiveXAfterHandleDepublished);
    container->DissociateHandle();
    wxWinUIMSWSetAfterHandleDepublishedForTest(nullptr);
    gs_containerToDeleteAfterHandleDepublished = nullptr;

    CHECK(fixture.container == nullptr);
    CHECK(wxFindWinFromHandle(hwnd) == nullptr);
    CHECK(fixture.control->GetHostedWindow() == nullptr);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
    CHECK(gs_probeResources.liveClientSiteEdges == 0);
}

#if wxDEBUG_LEVEL
TEST_CASE("WinUI ActiveX reacquires its owner after failure logging",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    LifecycleActiveXEventFilter filter(
        LifecycleActiveXEventFilter::Action::CaptureOnly);
    ReentrantActiveXLogTarget logTarget(filter);
    ActiveXFixture fixture(
        ProbeActiveX::Failure::Run, false, &filter);

    CHECK(logTarget.DidDelete());
    CHECK(filter.DidDelete());
    CHECK(fixture.container == nullptr);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
    CHECK(fixture.control->GetHostedWindow() == nullptr);
    CHECK(gs_probeResources.liveClientSiteEdges == 0);
}
#endif // wxDEBUG_LEVEL

TEST_CASE("WinUI ActiveX rejects direct callbacks from a foreign thread",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    ActiveXFixture fixture(
        ProbeActiveX::Failure::None, true);
    REQUIRE(fixture.container->GetHWND() != nullptr);

    HRESULT eventResult = E_UNEXPECTED;
    HRESULT siteResult = E_UNEXPECTED;
    std::thread callbackThread(
        [&]
        {
            eventResult = fixture.control->FireEvent();
            siteResult = fixture.control->CallLateSite();
        });
    callbackThread.join();

    CHECK(eventResult == RPC_E_WRONG_THREAD);
    CHECK(siteResult == RPC_E_WRONG_THREAD);
    fixture.DeleteContainer();
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
}

TEST_CASE("WinUI ActiveX resize stops after every failed OLE step",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    const ProbeActiveX::Failure failures[] =
    {
        ProbeActiveX::Failure::ResizeGetExtent,
        ProbeActiveX::Failure::ResizeSetExtent,
        ProbeActiveX::Failure::ResizeSetObjectRects
    };

    for ( const ProbeActiveX::Failure failure : failures )
    {
        INFO("resize failure stage " << static_cast<int>(failure));
        ActiveXFixture fixture(failure);
        REQUIRE(fixture.container->GetHWND() != nullptr);
        fixture.control->ResetExtentForTest();

        const unsigned getExtentBefore =
            fixture.control->getExtentCalls;
        const unsigned setExtentBefore =
            fixture.control->setExtentCalls;
        const unsigned rectsBefore =
            fixture.control->setObjectRectsCalls;
        wxSizeEvent sizeEvent(
            fixture.parent->GetClientSize(), fixture.parent->GetId());
        sizeEvent.SetEventObject(fixture.parent);
        fixture.parent->ProcessWindowEvent(sizeEvent);

        CHECK(fixture.control->getExtentCalls == getExtentBefore + 1);
        if ( failure == ProbeActiveX::Failure::ResizeGetExtent )
        {
            CHECK(fixture.control->setExtentCalls == setExtentBefore);
            CHECK(fixture.control->setObjectRectsCalls == rectsBefore);
        }
        else if ( failure == ProbeActiveX::Failure::ResizeSetExtent )
        {
            CHECK(fixture.control->setExtentCalls == setExtentBefore + 1);
            CHECK(fixture.control->setObjectRectsCalls == rectsBefore);
        }
        else
        {
            CHECK(fixture.control->setExtentCalls == setExtentBefore + 1);
            CHECK(fixture.control->setObjectRectsCalls == rectsBefore + 1);
        }

        fixture.DeleteContainer();
        CHECK(fixture.control->eventUnadvises == 1);
        CHECK(fixture.control->docUnadvises == 1);
        CHECK(fixture.control->RefCountForTest() == 1);
        CHECK(gs_probeResources.liveClientSiteEdges == 0);
    }
}

TEST_CASE("WinUI ActiveX resize revalidates after reentrant GetExtent",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());

    ActiveXFixture fixture(
        ProbeActiveX::Failure::ReentrantResizeGetExtent);
    REQUIRE(fixture.container->GetHWND() != nullptr);
    fixture.control->ResetExtentForTest();

    unsigned events = 0;
    fixture.container->Bind(
        wxEVT_ACTIVEX,
        [&](wxActiveXEvent&)
        {
            ++events;
            fixture.DeleteContainer();
        });

    const unsigned getExtentBefore = fixture.control->getExtentCalls;
    const unsigned setExtentBefore = fixture.control->setExtentCalls;
    const unsigned rectsBefore = fixture.control->setObjectRectsCalls;
    wxSizeEvent sizeEvent(
        fixture.parent->GetClientSize(), fixture.parent->GetId());
    sizeEvent.SetEventObject(fixture.parent);
    fixture.parent->ProcessWindowEvent(sizeEvent);

    CHECK(events == 1);
    CHECK(fixture.container == nullptr);
    CHECK(fixture.control->reentrantCallbackFired);
    CHECK(fixture.control->reentrantCallbackResult == S_OK);
    CHECK(fixture.control->getExtentCalls == getExtentBefore + 1);
    CHECK(fixture.control->setExtentCalls == setExtentBefore);
    CHECK(fixture.control->setObjectRectsCalls == rectsBefore);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
    CHECK(gs_probeResources.liveClientSiteEdges == 0);
}

TEST_CASE("WinUI ActiveX revokes callbacks before COM and parent teardown",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    ActiveXFixture fixture(ProbeActiveX::Failure::None, true);
    REQUIRE(fixture.container->GetHWND() != nullptr);

    const unsigned rectCallsBeforeResize =
        fixture.control->setObjectRectsCalls;
    fixture.parent->SetSize(wxSize(260, 120));
    wxSizeEvent firstSizeEvent(
        fixture.parent->GetClientSize(), fixture.parent->GetId());
    firstSizeEvent.SetEventObject(fixture.parent);
    fixture.parent->ProcessWindowEvent(firstSizeEvent);
    const unsigned rectCalls = fixture.control->setObjectRectsCalls;
    REQUIRE(rectCalls > rectCallsBeforeResize);

    const unsigned activationCallsBeforeFocus =
        fixture.control->frameActivationCalls;
    wxFocusEvent firstSetFocus(wxEVT_SET_FOCUS, fixture.parent->GetId());
    firstSetFocus.SetEventObject(fixture.parent);
    fixture.parent->ProcessWindowEvent(firstSetFocus);
    wxFocusEvent firstKillFocus(wxEVT_KILL_FOCUS, fixture.parent->GetId());
    firstKillFocus.SetEventObject(fixture.parent);
    fixture.parent->ProcessWindowEvent(firstKillFocus);
    REQUIRE(fixture.control->frameActivationCalls ==
            activationCallsBeforeFocus + 2);
    CHECK_FALSE(fixture.control->lastFrameActivation);

    wxActiveXContainer * const staleHandlerIdentity = fixture.container;
    const int parentEventId = fixture.parent->GetId();

    fixture.DeleteContainer();
    CHECK(fixture.control->eventAdvises == 1);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docAdvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);

    CHECK(CallIndex(*fixture.control, "doc-unadvise") <
          CallIndex(*fixture.control, "event-unadvise"));
    CHECK(CallIndex(*fixture.control, "event-unadvise") <
          CallIndex(*fixture.control, "ui-deactivate"));
    CHECK(CallIndex(*fixture.control, "ui-deactivate") <
          CallIndex(*fixture.control, "inplace-deactivate"));
    CHECK(CallIndex(*fixture.control, "inplace-deactivate") <
          CallIndex(*fixture.control, "hide"));
    CHECK(CallIndex(*fixture.control, "hide") <
          CallIndex(*fixture.control, "close"));
    CHECK(CallIndex(*fixture.control, "close") <
          CallIndex(*fixture.control, "client-site-null"));

    // Unbind() compares the handler identity without dereferencing it. A false
    // result proves all three raw parent bindings were removed by teardown.
    CHECK_FALSE(fixture.parent->Unbind(
        wxEVT_SIZE, &wxActiveXContainer::OnSize,
        staleHandlerIdentity, parentEventId));
    CHECK_FALSE(fixture.parent->Unbind(
        wxEVT_SET_FOCUS, &wxActiveXContainer::OnSetFocus,
        staleHandlerIdentity, parentEventId));
    CHECK_FALSE(fixture.parent->Unbind(
        wxEVT_KILL_FOCUS, &wxActiveXContainer::OnKillFocus,
        staleHandlerIdentity, parentEventId));

    fixture.parent->SetSize(wxSize(240, 110));
    wxSizeEvent lateSizeEvent(
        fixture.parent->GetClientSize(), fixture.parent->GetId());
    lateSizeEvent.SetEventObject(fixture.parent);
    fixture.parent->ProcessWindowEvent(lateSizeEvent);
    wxFocusEvent lateSetFocus(wxEVT_SET_FOCUS, fixture.parent->GetId());
    lateSetFocus.SetEventObject(fixture.parent);
    fixture.parent->ProcessWindowEvent(lateSetFocus);
    wxFocusEvent lateKillFocus(wxEVT_KILL_FOCUS, fixture.parent->GetId());
    lateKillFocus.SetEventObject(fixture.parent);
    fixture.parent->ProcessWindowEvent(lateKillFocus);
    CHECK(fixture.control->setObjectRectsCalls == rectCalls);
    CHECK(fixture.control->FireLateEvent() == RPC_E_DISCONNECTED);
    CHECK(fixture.control->CallLateSite() == CO_E_OBJNOTCONNECTED);
}

TEST_CASE("WinUI ActiveX pins its event sink across reentrant deletion",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    ActiveXFixture fixture(ProbeActiveX::Failure::None, true);
    REQUIRE(fixture.container->GetHWND() != nullptr);

    unsigned events = 0;
    fixture.container->Bind(
        wxEVT_ACTIVEX,
        [&](wxActiveXEvent&)
        {
            ++events;
            fixture.DeleteContainer();
        });

    CHECK(fixture.control->FireEvent() == S_OK);
    CHECK(events == 1);
    CHECK(fixture.container == nullptr);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
    CHECK(fixture.control->FireLateEvent() == RPC_E_DISCONNECTED);
}

TEST_CASE("WinUI ActiveX retires an externally destroyed HWND generation",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    ActiveXFixture fixture;
    const HWND hwnd = fixture.control->GetHostedWindow();
    REQUIRE(hwnd != nullptr);
    REQUIRE(fixture.container->GetHWND() == hwnd);
    const unsigned long long nativeGeneration =
        wxWinUIMSWGetNativeHwndGeneration(hwnd);
    REQUIRE(nativeGeneration != 0);

    fixture.control->DestroyHostedWindow();
    CHECK(fixture.container->GetHWND() == nullptr);
    CHECK(wxFindWinFromHandle(hwnd) == nullptr);
    CHECK(fixture.control->GetHostedWindow() == nullptr);

    fixture.DeleteContainer();
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
}

TEST_CASE("WinUI ActiveX ABA rollback removes only its exact wx map entry",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    ActiveXFixture fixture;
    const HWND hwnd = fixture.control->GetHostedWindow();
    REQUIRE(hwnd != nullptr);
    REQUIRE(fixture.container->GetHWND() == hwnd);
    REQUIRE(wxFindWinFromHandle(hwnd) == fixture.container);

    const unsigned long long originalGeneration =
        wxWinUIMSWGetNativeHwndGeneration(hwnd);
    REQUIRE(originalGeneration != 0);
    REQUIRE(fixture.control->PrepareForeignNativeGeneration(
        originalGeneration));
    REQUIRE(wxWinUIMSWGetNativeHwndGeneration(hwnd) !=
            originalGeneration);

    // The simulated foreign generation owns the original STATIC WNDPROC. The
    // host must remove only its stale wx map entry and must not restore its
    // saved subclass procedure onto that generation.
    fixture.DeleteContainer();
    CHECK(wxFindWinFromHandle(hwnd) == nullptr);
    CHECK(fixture.control->foreignWndProcPreserved);
    CHECK(fixture.control->GetHostedWindow() == nullptr);
    CHECK(fixture.control->eventUnadvises == 1);
    CHECK(fixture.control->docUnadvises == 1);
    CHECK(fixture.control->RefCountForTest() == 1);
}

TEST_CASE("WinUI ActiveX lifetime transaction is stable for 100 cycles",
          "[winui-015][winui-activex][lifetime]")
{
    OleApartment apartment;
    REQUIRE(apartment.IsUsable());
    wxFrame * const frame =
        new wxFrame(nullptr, wxID_ANY, "activex-cycle-probe",
                    wxPoint(-24000, -24000), wxSize(300, 160));
    wxPanel * const parent =
        new wxPanel(frame, wxID_ANY, wxPoint(0, 0), wxSize(280, 130));

    const LONG controlsBefore = gs_probeResources.liveControls;
    const LONG pointsBefore = gs_probeResources.liveConnectionPoints;
    const LONG typeInfosBefore = gs_probeResources.liveTypeInfos;
    const LONG hwndsBefore = gs_probeResources.liveHwnds;
    const LONG cycleSiteEdgesBefore =
        gs_probeResources.liveClientSiteEdges;
    const unsigned attrsAllocatedBefore =
        gs_probeResources.typeAttrsAllocated;
    const unsigned attrsReleasedBefore =
        gs_probeResources.typeAttrsReleased;
    const unsigned lateSinkReleasesBefore =
        gs_probeResources.lateSinkFinalReleases;
    const unsigned lateSiteReleasesBefore =
        gs_probeResources.lateSiteFinalReleases;
    const unsigned unexpectedReleasesBefore =
        gs_probeResources.unexpectedLateReleaseCounts;

    for ( unsigned i = 0; i < 100; ++i )
    {
        ProbeActiveX * const control = new ProbeActiveX(
            ProbeActiveX::Failure::None, true);
        wxActiveXContainer * const container =
            new wxActiveXContainer(
                parent, IID_IUnknown, static_cast<IOleObject *>(control));
        const bool created = container->GetHWND() != nullptr;
        delete container;
        const bool lateCallbacksRevoked =
            control->FireLateEvent() == RPC_E_DISCONNECTED &&
            control->CallLateSite() == CO_E_OBJNOTCONNECTED;
        const bool balanced =
            control->eventAdvises == 1 &&
            control->eventUnadvises == 1 &&
            control->docAdvises == 1 &&
            control->docUnadvises == 1 &&
            control->RefCountForTest() == 1 &&
            control->GetHostedWindow() == nullptr;
        const ULONG finalControlRefs = control->Release();

        INFO("cycle " << i);
        CHECK(created);
        CHECK(lateCallbacksRevoked);
        CHECK(balanced);
        CHECK(finalControlRefs == 0);
    }

    CHECK(gs_probeResources.liveControls == controlsBefore);
    CHECK(gs_probeResources.liveConnectionPoints == pointsBefore);
    CHECK(gs_probeResources.liveTypeInfos == typeInfosBefore);
    CHECK(gs_probeResources.liveHwnds == hwndsBefore);
    CHECK(gs_probeResources.liveClientSiteEdges == cycleSiteEdgesBefore);
    CHECK(gs_probeResources.typeAttrsAllocated - attrsAllocatedBefore ==
          gs_probeResources.typeAttrsReleased - attrsReleasedBefore);
    CHECK(gs_probeResources.lateSinkFinalReleases -
              lateSinkReleasesBefore == 100);
    CHECK(gs_probeResources.lateSiteFinalReleases -
              lateSiteReleasesBefore == 100);
    CHECK(gs_probeResources.unexpectedLateReleaseCounts ==
          unexpectedReleasesBefore);

    delete frame;
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_ACTIVEX && wxUSE_OLE
