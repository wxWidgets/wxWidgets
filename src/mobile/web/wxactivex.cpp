
#ifdef __WXMSW__

#include "wx/mobile/web/wxactivex.h"
#include <wx/strconv.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/datetime.h>
#include <oleidl.h>
#include <winerror.h>
#include <idispids.h>
#include <olectl.h>
using namespace std;

#if wxCHECK_VERSION(2,5,2)
#define wxMBConv() wxConvLibc
#endif

// Depending on compilation mode, the wx headers may have undef'd
// this, but in this case we need it so the virtual method in
// wxActiveXOldFrameSite will match what is in oleidl.h.
#ifndef GetObject
    #ifdef _UNICODE
        #define GetObject GetObjectW
    #else
        #define GetObject GetObjectA
    #endif
#endif


//////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxActiveXOld, wxWindow)
    EVT_SIZE(wxActiveXOld::OnSize)
    EVT_PAINT(wxActiveXOld::OnPaint)
    EVT_MOUSE_EVENTS(wxActiveXOld::OnMouse)
    EVT_SET_FOCUS(wxActiveXOld::OnSetFocus)
    EVT_KILL_FOCUS(wxActiveXOld::OnKillFocus)
END_EVENT_TABLE()

class wxActiveXOld;

class wxActiveXOldFrameSite :
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
    DECLARE_OLE_UNKNOWN(wxActiveXOldFrameSite);

public:
    wxActiveXOldFrameSite(wxActiveXOld * win);
    virtual ~wxActiveXOldFrameSite();

    //IOleWindow
    STDMETHODIMP GetWindow(HWND*);
    STDMETHODIMP ContextSensitiveHelp(BOOL);

    //IOleInPlaceUIWindow
    STDMETHODIMP GetBorder(LPRECT);
    STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS);
    STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS);
    STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject*, LPCOLESTR);

    //IOleInPlaceFrame
    STDMETHODIMP InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS);
    STDMETHODIMP SetMenu(HMENU, HOLEMENU, HWND);
    STDMETHODIMP RemoveMenus(HMENU);
    STDMETHODIMP SetStatusText(LPCOLESTR);
    STDMETHODIMP EnableModeless(BOOL);
    STDMETHODIMP TranslateAccelerator(LPMSG, WORD);

    //IOleInPlaceSite
    STDMETHODIMP CanInPlaceActivate();
    STDMETHODIMP OnInPlaceActivate();
    STDMETHODIMP OnUIActivate();
    STDMETHODIMP GetWindowContext(IOleInPlaceFrame**, IOleInPlaceUIWindow**,
        LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO);
    STDMETHODIMP Scroll(SIZE);
    STDMETHODIMP OnUIDeactivate(BOOL);
    STDMETHODIMP OnInPlaceDeactivate();
    STDMETHODIMP DiscardUndoState();
    STDMETHODIMP DeactivateAndUndo();
    STDMETHODIMP OnPosRectChange(LPCRECT);

    //IOleInPlaceSiteEx
    STDMETHODIMP OnInPlaceActivateEx(BOOL*, DWORD);
    STDMETHODIMP OnInPlaceDeactivateEx(BOOL);
    STDMETHODIMP RequestUIActivate();

    //IOleClientSite
    STDMETHODIMP SaveObject();
    STDMETHODIMP GetMoniker(DWORD, DWORD, IMoniker**);
    STDMETHODIMP GetContainer(LPOLECONTAINER FAR*);
    STDMETHODIMP ShowObject();
    STDMETHODIMP OnShowWindow(BOOL);
    STDMETHODIMP RequestNewObjectLayout();

    //IOleControlSite
    STDMETHODIMP OnControlInfoChanged();
    STDMETHODIMP LockInPlaceActive(BOOL);
    STDMETHODIMP GetExtendedControl(IDispatch**);
    STDMETHODIMP TransformCoords(POINTL*, POINTF*, DWORD);
    STDMETHODIMP TranslateAccelerator(LPMSG, DWORD);
    STDMETHODIMP OnFocus(BOOL);
    STDMETHODIMP ShowPropertyFrame();

    //IOleCommandTarget
    STDMETHODIMP QueryStatus(const GUID*, ULONG, OLECMD[], OLECMDTEXT*);
    STDMETHODIMP Exec(const GUID*, DWORD, DWORD, VARIANTARG*, VARIANTARG*);

    //IParseDisplayName
    STDMETHODIMP ParseDisplayName(IBindCtx*, LPOLESTR, ULONG*, IMoniker**);

    //IOleContainer
    STDMETHODIMP EnumObjects(DWORD, IEnumUnknown**);
    STDMETHODIMP LockContainer(BOOL);

    //IOleItemContainer
    STDMETHODIMP GetObject(LPOLESTR, DWORD, IBindCtx*, REFIID, void**);
    STDMETHODIMP GetObjectStorage(LPOLESTR, IBindCtx*, REFIID, void**);
    STDMETHODIMP IsRunning(LPOLESTR);

    //IDispatch
    STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR**, unsigned int, LCID, DISPID*);
    STDMETHODIMP GetTypeInfo(unsigned int, LCID, ITypeInfo**);
    STDMETHODIMP GetTypeInfoCount(unsigned int*);
    STDMETHODIMP Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);

    //IAdviseSink
    void STDMETHODCALLTYPE OnDataChange(FORMATETC*, STGMEDIUM*);
    void STDMETHODCALLTYPE OnViewChange(DWORD, LONG);
    void STDMETHODCALLTYPE OnRename(IMoniker*);
    void STDMETHODCALLTYPE OnSave();
    void STDMETHODCALLTYPE OnClose();

    // IOleDocumentSite
    HRESULT STDMETHODCALLTYPE ActivateMe(IOleDocumentView __RPC_FAR *pViewToActivate);

protected:

    wxActiveXOld * m_window;

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

DEFINE_OLE_TABLE(wxActiveXOldFrameSite)
    OLE_INTERFACE(IID_IUnknown, IOleClientSite)

    OLE_IINTERFACE(IOleClientSite)

    OLE_INTERFACE(IID_IOleWindow, IOleInPlaceSite)
    OLE_IINTERFACE(IOleInPlaceSite)
    OLE_IINTERFACE(IOleInPlaceSiteEx)

    //OLE_IINTERFACE(IOleWindow)
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

END_OLE_TABLE;


wxActiveXOld::wxActiveXOld(wxWindow * parent, REFCLSID clsid, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name) :
wxWindow(parent, id, pos, size, style, name)
{
    m_bAmbientUserMode = true;
    m_docAdviseCookie = 0;
    m_frameSite = 0;
    CreateActiveX(clsid);
}

wxActiveXOld::wxActiveXOld(wxWindow * parent, wxString progId, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name) :
    wxWindow(parent, id, pos, size, style, name)
{
    m_bAmbientUserMode = true;
    m_docAdviseCookie = 0;
    m_frameSite = 0;
    CreateActiveX((LPOLESTR) (const wchar_t *) progId.wc_str(wxConvUTF8));
}

wxActiveXOld::~wxActiveXOld()
{
    // disconnect connection points
    wxOleConnectionArray::iterator it = m_connections.begin();
    while (it != m_connections.end())
    {
        wxOleConnectionPoint& cp = it->first;
        cp->Unadvise(it->second);

        it++;
    };
    m_connections.clear();

    if (m_oleInPlaceObject.Ok())
    {
        m_oleInPlaceObject->InPlaceDeactivate();
        m_oleInPlaceObject->UIDeactivate();
    }


    if (m_oleObject.Ok())
    {
        if (m_docAdviseCookie != 0)
            m_oleObject->Unadvise(m_docAdviseCookie);

        m_oleObject->DoVerb(OLEIVERB_HIDE, NULL, m_clientSite, 0, (HWND) GetHWND(), NULL);
        m_oleObject->Close(OLECLOSE_NOSAVE);
        m_oleObject->SetClientSite(NULL);
    }
}

void wxActiveXOld::CreateActiveX(REFCLSID clsid)
{
#if !wxCHECK_VERSION(2,7,0)
    SetTransparent();
#endif

    HRESULT hret;

    ////////////////////////////////////////////////////////
    // wxActiveXOldFrameSite
    m_frameSite = new wxActiveXOldFrameSite(this);
    // oleClientSite
    hret = m_clientSite.QueryInterface(IID_IOleClientSite, (IDispatch *) m_frameSite);
    wxASSERT(SUCCEEDED(hret));
    // adviseSink
    wxAutoOleInterface<IAdviseSink> adviseSink(IID_IAdviseSink, (IDispatch *) m_frameSite);
    wxASSERT(adviseSink.Ok());


    // // Create Object, get IUnknown interface
    m_ActiveX.CreateInstance(clsid, IID_IUnknown);
    wxASSERT(m_ActiveX.Ok());

    // Get Dispatch interface
    hret = m_Dispatch.QueryInterface(IID_IDispatch, m_ActiveX);
    WXOLE_WARN(hret, "Unable to get dispatch interface");

    // Type Info
    GetTypeInfo();

    // Get IOleObject interface
    hret = m_oleObject.QueryInterface(IID_IOleObject, m_ActiveX);
    wxASSERT(SUCCEEDED(hret));

    // get IViewObject Interface
    hret = m_viewObject.QueryInterface(IID_IViewObject, m_ActiveX);
    wxASSERT(SUCCEEDED(hret));

    // document advise
    m_docAdviseCookie = 0;
    hret = m_oleObject->Advise(adviseSink, &m_docAdviseCookie);
    WXOLE_WARN(hret, "m_oleObject->Advise(adviseSink, &m_docAdviseCookie),\"Advise\")");
    m_oleObject->SetHostNames(L"wxActiveXOldContainer", NULL);
    OleSetContainedObject(m_oleObject, TRUE);
    OleRun(m_oleObject);


    // Get IOleInPlaceObject interface
    hret = m_oleInPlaceObject.QueryInterface(IID_IOleInPlaceObject, m_ActiveX);
    wxASSERT(SUCCEEDED(hret));

    // status
    DWORD dwMiscStatus;
    m_oleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    wxASSERT(SUCCEEDED(hret));

    // set client site first ?
    if (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
        m_oleObject->SetClientSite(m_clientSite);


    // stream init
    wxAutoOleInterface<IPersistStreamInit>
        pPersistStreamInit(IID_IPersistStreamInit, m_oleObject);

    if (pPersistStreamInit.Ok())
    {
        hret = pPersistStreamInit->InitNew();
        WXOLE_WARN(hret, "CreateActiveX::pPersistStreamInit->InitNew()");
    };

    if (! (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
        m_oleObject->SetClientSite(m_clientSite);


    int w, h;
    GetClientSize(&w, &h);
    RECT posRect;
    posRect.left = 0;
    posRect.top = 0;
    posRect.right = w;
    posRect.bottom = h;

    m_oleObjectHWND = 0;

    if (m_oleInPlaceObject.Ok())
    {
        hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
        WXOLE_WARN(hret, "m_oleInPlaceObject->GetWindow(&m_oleObjectHWND)");
        if (SUCCEEDED(hret))
            ::SetActiveWindow(m_oleObjectHWND);
    };


    if (! (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME))
    {
        if (w > 0 && h > 0 && m_oleInPlaceObject.Ok())
            m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);

        hret = m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, m_clientSite, 0, (HWND)GetHWND(), &posRect);
        hret = m_oleObject->DoVerb(OLEIVERB_SHOW, 0, m_clientSite, 0, (HWND)GetHWND(), &posRect);
    };

    if (! m_oleObjectHWND && m_oleInPlaceObject.Ok())
    {
        hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
        WXOLE_WARN(hret, "m_oleInPlaceObject->GetWindow(&m_oleObjectHWND)");
    };

    if (m_oleObjectHWND)
    {
        ::SetActiveWindow(m_oleObjectHWND);
        ::ShowWindow(m_oleObjectHWND, SW_SHOW);

        // Update by GBR to resize older controls
        wxSizeEvent szEvent;
        szEvent.m_size = wxSize(w, h) ;
        AddPendingEvent(szEvent);
    };
}

void wxActiveXOld::CreateActiveX(LPOLESTR progId)
{
    CLSID clsid;
    if (CLSIDFromProgID(progId, &clsid) != S_OK)
        return;

    CreateActiveX(clsid);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Case Insensitive Map of Event names to eventTypes
// created dynamically at run time in:
//      EVT_ACTIVEX(eventName, id, fn)
// we map the pointer to them so that:
//      const wxEventType& RegisterActiveXEvent(wxString eventName);
// can return a const reference, which is neccessary for event tables
// probably should use a wxWindows hash table here, but I'm lazy ...
typedef map<wxString, wxEventType *, NS_wxActiveXOld::less_wxStringI> ActiveXNamedEventMap;
static ActiveXNamedEventMap *sgp_NamedEventMap = 0;

const wxEventType& RegisterActiveXEvent(const wxChar *eventName)
{
    wxString ev = eventName;
    if(!sgp_NamedEventMap)
        sgp_NamedEventMap = new ActiveXNamedEventMap;

    ActiveXNamedEventMap::iterator it = sgp_NamedEventMap->find(ev);
    if (it == sgp_NamedEventMap->end())
    {
        wxEventType  *et = new wxEventType(wxNewEventType());
        (*sgp_NamedEventMap)[ev] = et;

        return *et;
    };

    return *(it->second);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Map of Event DISPID's to eventTypes
// created dynamically at run time in:
//      EVT_ACTIVEX(eventName, id, fn)
// we map the pointer to them so that:
//      const wxEventType& RegisterActiveXEvent(wxString eventName);
// can return a const reference, which is neccessary for event tables

typedef map<DISPID, wxEventType *> ActiveXDISPIDEventMap;
static ActiveXDISPIDEventMap sg_dispIdEventMap;

const wxEventType& RegisterActiveXEvent(DISPID event)
{
    ActiveXDISPIDEventMap::iterator it = sg_dispIdEventMap.find(event);
    if (it == sg_dispIdEventMap.end())
    {
        wxEventType  *et = new wxEventType(wxNewEventType());
        sg_dispIdEventMap[event] = et;

        return *et;
    };

    return *(it->second);
};

// one off class for automatic freeing of activeX eventtypes
class ActiveXEventMapFlusher
{
public:
    ~ActiveXEventMapFlusher()
    {
        if (sgp_NamedEventMap)
        {
            // Named events
            ActiveXNamedEventMap::iterator it = sgp_NamedEventMap->begin();
            while (it != sgp_NamedEventMap->end())
            {
                delete it->second;
                it++;
            };
            sgp_NamedEventMap->clear();
            delete sgp_NamedEventMap;
            sgp_NamedEventMap = 0;
        }

        // DISPID events
        ActiveXDISPIDEventMap::iterator dit = sg_dispIdEventMap.begin();
        while (dit != sg_dispIdEventMap.end())
        {
            delete dit->second;
            dit++;
        };
        sg_dispIdEventMap.clear();

    };
};

static ActiveXEventMapFlusher s_dummyActiveXEventMapFlusher;


//////////////////////////////////////////////////////
VARTYPE wxTypeToVType(const wxVariant& v)
{
    wxString type = v.GetType();
    if (type == wxT("bool"))
        return VT_BOOL;
    else if (type == wxT("char"))
        return VT_I1;
    else if (type == wxT("datetime"))
        return VT_DATE;
    else if (type == wxT("double"))
        return VT_R8;
    else if (type == wxT("list"))
        return VT_ARRAY;
    else if (type == wxT("long"))
        return VT_I4;
    else if (type == wxT("string"))
        return VT_BSTR;
    else if (type == wxT("stringlist"))
        return VT_ARRAY;
    else if (type == wxT("date"))
        return VT_DATE;
    else if (type == wxT("time"))
        return VT_DATE;
    else if (type == wxT("void*"))
        return VT_VOID | VT_BYREF;
    else
        return VT_NULL;
};

bool wxDateTimeToDATE(wxDateTime dt, DATE& d)
{
    SYSTEMTIME st;
    memset(&st, 0, sizeof(st));

    st.wYear = dt.GetYear();
    st.wMonth = dt.GetMonth() + 1;
    st.wDay = dt.GetDay();
    st.wHour = dt.GetHour();
    st.wMinute = dt.GetMinute();
    st.wSecond = dt.GetSecond();
    st.wMilliseconds = dt.GetMillisecond();
    return SystemTimeToVariantTime(&st, &d) != FALSE;
};

bool wxDateTimeToVariant(wxDateTime dt, VARIANTARG& va)
{
    return wxDateTimeToDATE(dt, va.date);
};

bool DATEToWxDateTime(DATE date, wxDateTime& dt)
{
    SYSTEMTIME st;
    if (! VariantTimeToSystemTime(date, &st))
        return false;

    dt = wxDateTime(
        st.wDay,
        wxDateTime::Month(int(wxDateTime::Jan) + st.wMonth - 1),
        st.wYear,
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    return true;
};

bool VariantToWxDateTime(VARIANTARG va, wxDateTime& dt)
{
    HRESULT hr = VariantChangeType(&va, &va, 0, VT_DATE);
    if (! SUCCEEDED(hr))
        return false;

    return DATEToWxDateTime(va.date, dt);
};

bool MSWVariantToVariant(VARIANTARG& va, wxVariant& vx)
{
    bool byRef = false;
    VARTYPE vt = va.vt;

    if (vt & VT_ARRAY)
        return false; // don't support arrays yet

    if (vt & VT_BYREF)
    {
        byRef = true;
        vt &= ~(VT_BYREF);
    };


    switch(vt)
    {
    case VT_VARIANT:
        if (byRef)
            return MSWVariantToVariant(*va.pvarVal, vx);
        else
        {
            VARIANT tmp = va;
            VariantChangeType(&tmp, &tmp, 0, wxTypeToVType(vx));
            bool rc = MSWVariantToVariant(tmp, vx);
            VariantClear(&tmp);
            return rc;
        };

    // 1 byte chars
    case VT_I1:
    case VT_UI1:
#if wxCHECK_VERSION(2,7,0)
        if (byRef)
            vx = (wxChar) (char) *va.pbVal;
        else
            vx = (wxChar) (char) va.bVal;
#else
        if (byRef)
            vx = (char) *va.pbVal;
        else
            vx = (char) va.bVal;
#endif
        return true;

    // 2 byte shorts
    case VT_I2:
    case VT_UI2:
        if (byRef)
            vx = (long) *va.puiVal;
        else
            vx = (long) va.uiVal;
        return true;

    // 4 bytes longs
    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
        if (byRef)
            vx = (long) *va.pulVal;
        else
            vx = (long) va.ulVal;
        return true;


    // 4 byte floats
    case VT_R4:
        if (byRef)
            vx = *va.pfltVal;
        else
            vx = va.fltVal;
        return true;

    // 8 byte doubles
    case VT_R8:
        if (byRef)
            vx = *va.pdblVal;
        else
            vx = va.dblVal;
        return true;

    case VT_BOOL:
        if (byRef)
            vx = (*va.pboolVal ? true : false);
        else
            vx = (va.boolVal ? true : false);
        return true;

    case VT_CY:
        vx.MakeNull();
        return false; // what the hell is a CY ?

    case VT_DECIMAL:
        {
            double d = 0;
            HRESULT hr;
            if (byRef)
                hr = VarR8FromDec(va.pdecVal, &d);
            else
                hr = VarR8FromDec(&va.decVal, &d);

            vx = d;
            return SUCCEEDED(hr);
        };

    case VT_DATE:
        {
            wxDateTime dt;
            bool rc =  false;
            if (byRef)
                rc = DATEToWxDateTime(*va.pdate, dt);
            else
                rc = VariantToWxDateTime(va, dt);
            vx = dt;
            return rc;
        };

    case VT_BSTR:
        if (byRef)
            vx = wxString(*va.pbstrVal);
        else
            vx = wxString(va.bstrVal);
        return true;

    case VT_UNKNOWN: // should do a custom wxVariantData for this
        if (byRef)
            vx = (void *) *va.ppunkVal;
        else
            vx = (void *) va.punkVal;
        return false;

    case VT_DISPATCH: // should do a custom wxVariantData for this
        if (byRef)
            vx = (void *) *va.ppdispVal;
        else
            vx = (void *) va.pdispVal;
        return false;

    default:
        vx.MakeNull();
        return false;
    };
};

bool VariantToMSWVariant(const wxVariant& vx, VARIANTARG& va)
{
    bool byRef = false;
    VARTYPE vt = va.vt;

    if (vt & VT_ARRAY)
        return false; // don't support arrays yet

    if (vt & VT_BYREF)
    {
        byRef = true;
        vt &= ~(VT_BYREF);
    };

    switch(vt)
    {
    case VT_VARIANT:
        if (byRef)
            return VariantToMSWVariant(vx, *va.pvarVal);
        else
        {
            va.vt = wxTypeToVType(vx);
            return VariantToMSWVariant(vx, va);
        };

    // 1 byte chars
    case VT_I1:
    case VT_UI1:
        if (byRef)
            *va.pbVal = (char) vx.GetChar();
        else
            va.bVal = (char) vx.GetChar();
        return true;

    // 2 byte shorts
    case VT_I2:
    case VT_UI2:
        if (byRef)
            *va.puiVal = (long) vx;
        else
            va.uiVal = (long) vx;
        return true;

    // 4 bytes longs
    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
        if (byRef)
            *va.pulVal = (long) vx;
        else
            va.ulVal = (long) vx;
        return true;


    // 4 byte floats
    case VT_R4:
        if (byRef)
            *va.pfltVal = (double) vx;
        else
            va.fltVal = (double) vx;
        return true;

    // 8 byte doubles
    case VT_R8:
        if (byRef)
            *va.pdblVal = (double) vx;
        else
            va.dblVal = (double) vx;
        return true;

    case VT_BOOL:
        if (byRef)
            *va.pboolVal = ((bool) vx) ? TRUE : FALSE;
        else
            va.boolVal = ((bool) vx) ? TRUE : FALSE;
        return true;

    case VT_CY:
        return false; // what the hell is a CY ?

    case VT_DECIMAL:
        if (byRef)
            return SUCCEEDED(VarDecFromR8(vx, va.pdecVal));
        else
            return SUCCEEDED(VarDecFromR8(vx, &va.decVal));

    case VT_DATE:
        if (byRef)
            return wxDateTimeToDATE(vx, *va.pdate);
        else
            return wxDateTimeToVariant(vx,va);

    case VT_BSTR:
        if (byRef)
            *va.pbstrVal = SysAllocString(vx.GetString().wc_str(wxMBConv()));
        else
            va.bstrVal = SysAllocString(vx.GetString().wc_str(wxMBConv()));
        return true;

    case VT_UNKNOWN: // should do a custom wxVariantData for this
        if (byRef)
            *va.ppunkVal = (IUnknown *) (void *) vx;
        else
            va.punkVal = (IUnknown *) (void *) vx;
        return false;

    case VT_DISPATCH: // should do a custom wxVariantData for this
        if (byRef)
            *va.ppdispVal = (IDispatch *) (void *) vx;
        else
            va.pdispVal = (IDispatch *) (void *) vx;
        return false;

    default:
        return false;
    };
};


class wxActiveXOldEvents : public IDispatch
{
private:
    DECLARE_OLE_UNKNOWN(wxActiveXOldEvents);


    wxActiveXOld    *m_activeX;
    IID            m_customId;
    bool        m_haveCustomId;

    friend bool wxActiveXOldEventsInterface(wxActiveXOldEvents *self, REFIID iid, void **_interface, const char *&desc);

public:
    wxActiveXOldEvents(wxActiveXOld *ax) : m_activeX(ax), m_haveCustomId(false) {}
    wxActiveXOldEvents(wxActiveXOld *ax, REFIID iid) : m_activeX(ax), m_haveCustomId(true), m_customId(iid) {}
    virtual ~wxActiveXOldEvents()
    {
    }

    //IDispatch
    STDMETHODIMP GetIDsOfNames(REFIID r, OLECHAR** o, unsigned int i, LCID l, DISPID* d)
    {
        return E_NOTIMPL;
    };

    STDMETHODIMP GetTypeInfo(unsigned int i, LCID l, ITypeInfo** t)
    {
        return E_NOTIMPL;
    };

    STDMETHODIMP GetTypeInfoCount(unsigned int* i)
    {
        return E_NOTIMPL;
    };


    void DispatchEvent(wxActiveXOld::FuncX &func, const wxEventType& eventType, DISPPARAMS * pDispParams)
    {
        wxActiveXOldEvent  event;
        event.SetId(m_activeX->GetId());
        event.SetEventType(eventType);
        event.m_params.NullList();
        event.m_params.SetName(func.name);

        // arguments
        if (pDispParams)
        {
            // cdecl call
            // sometimes the pDispParams does not match the param info for a activex control
            int nArg = wxMin(func.params.size(), pDispParams->cArgs);
            for (int i = nArg - 1; i >= 0; i--)
            {
                VARIANTARG& va = pDispParams->rgvarg[i];
                wxActiveXOld::ParamX &px = func.params[nArg - i - 1];
                wxVariant vx;

                vx.SetName(px.name);
                MSWVariantToVariant(va, vx);
                event.m_params.Append(vx);
            };
        };

        if (func.hasOut)
        {
            int nArg = wxMin(func.params.size(), pDispParams->cArgs);
            m_activeX->GetParent()->ProcessEvent(event);
            for (int i = 0; i < nArg; i++)
            {
                VARIANTARG& va = pDispParams->rgvarg[i];
                wxActiveXOld::ParamX &px = func.params[nArg - i - 1];

                if (px.IsOut())
                {
                    wxVariant& vx = event.m_params[nArg - i - 1];

                    VariantToMSWVariant(vx, va);
                };
            };
        }
        else
            m_activeX->GetParent()->AddPendingEvent(event);

    };

    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
                          WORD wFlags, DISPPARAMS * pDispParams,
                          VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
                          unsigned int * puArgErr)
    {
        if (wFlags & (DISPATCH_PROPERTYGET | DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF))
            return E_NOTIMPL;

        wxASSERT(m_activeX);

        // find event for dispid
        wxActiveXOld::MemberIdMap::iterator mit = m_activeX->m_eventMemberIds.find((MEMBERID) dispIdMember);
        if (mit == m_activeX->m_eventMemberIds.end())
            return S_OK;

        // sanity check
        int midx = mit->second;
        if (midx < 0 || midx >= m_activeX->GetEventCount())
            return S_OK;

        wxActiveXOld::FuncX &func = m_activeX->m_events[midx];


        // try to find dispid event
        ActiveXDISPIDEventMap::iterator dit = sg_dispIdEventMap.find(dispIdMember);
        if (dit != sg_dispIdEventMap.end())
        {
            // Dispatch Event
            DispatchEvent(func, *(dit->second), pDispParams);
            return S_OK;
        };

        // try named event
        if (sgp_NamedEventMap)
        {
            ActiveXNamedEventMap::iterator nit = sgp_NamedEventMap->find(func.name);
            if (nit == sgp_NamedEventMap->end())
                return S_OK;
            // Dispatch Event
            DispatchEvent(func, *(nit->second), pDispParams);
        }

        return S_OK;
    }
};

bool wxActiveXOldEventsInterface(wxActiveXOldEvents *self, REFIID iid, void **_interface, const char *&desc)
{
    if (self->m_haveCustomId && IsEqualIID(iid, self->m_customId))
    {
        WXOLE_TRACE("Found Custom Dispatch Interface");
        *_interface = (IUnknown *) (IDispatch *) self;
        desc = "Custom Dispatch Interface";
        return true;
    };

    return false;
};

DEFINE_OLE_TABLE(wxActiveXOldEvents)
    OLE_IINTERFACE(IUnknown)
    OLE_INTERFACE(IID_IDispatch, IDispatch)
    OLE_INTERFACE_CUSTOM(wxActiveXOldEventsInterface)
END_OLE_TABLE;

wxString wxActiveXOldEvent::EventName()
{
    return m_params.GetName();
};

int wxActiveXOldEvent::ParamCount() const
{
    return m_params.GetCount();
};

wxString wxActiveXOldEvent::ParamType(int idx)
{
    wxASSERT(idx >= 0 && idx < (int) m_params.GetCount());

    return m_params[idx].GetType();
};

wxString wxActiveXOldEvent::ParamName(int idx)
{
    wxASSERT(idx >= 0 && idx < (int) m_params.GetCount());

    return m_params[idx].GetName();
};

static wxVariant nullVar;

wxVariant& wxActiveXOldEvent::operator[] (int idx)
{
    wxASSERT(idx >= 0 && idx < ParamCount());

    return m_params[idx];
};

wxVariant& wxActiveXOldEvent::operator[] (wxString name)
{
    for (int i = 0; i < (int) m_params.GetCount(); i++)
    {
        if (name.CmpNoCase(m_params[i].GetName()) == 0)
            return m_params[i];
    };

    wxString err = wxT("wxActiveXOldEvent::operator[] invalid name <") + name + wxT(">");
    err += wxT("\r\nValid Names = :\r\n");
    for (i = 0; i < (int) m_params.GetCount(); i++)
    {
        err += m_params[i].GetName();
        err += wxT("\r\n");
    };

    wxASSERT_MSG(false, err);

    return nullVar;
};

void wxActiveXOld::GetTypeInfo()
{
    /*
    We are currently only interested in the IDispatch interface
    to the control. For dual interfaces (TypeKind = TKIND_INTERFACE)
    we should drill down through the inheritance
    (using TYPEATTR->cImplTypes) and GetRefTypeOfImplType(n)
    and retrieve all the func names etc that way, then generate a C++
    header    file for it.

    But we don't do this and probably never will, so if we have a DUAL
    interface then we query for the IDispatch
    via GetRefTypeOfImplType(-1).
    */

    HRESULT hret = 0;

    // get type info via class info
    wxAutoOleInterface<IProvideClassInfo> classInfo(IID_IProvideClassInfo, m_ActiveX);
    if (! classInfo.Ok())
        return;

    // type info
    wxAutoOleInterface<ITypeInfo> typeInfo;
    hret = classInfo->GetClassInfo(typeInfo.GetRef());
    if (! typeInfo.Ok())
        return;

    // TYPEATTR
    TYPEATTR *ta = NULL;
    hret = typeInfo->GetTypeAttr(&ta);
    if (! ta)
        return;

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
        wxAutoOleInterface<ITypeInfo>  ti;
        hret = typeInfo->GetRefTypeInfo(rt, ti.GetRef());
        if (! ti.Ok())
            continue;

        // check if default event sink
        bool defInterface = false;
        bool defEventSink = false;
        int impTypeFlags = 0;
        typeInfo->GetImplTypeFlags(i, &impTypeFlags);

        if (impTypeFlags & IMPLTYPEFLAG_FDEFAULT)
        {
            if (impTypeFlags & IMPLTYPEFLAG_FSOURCE)
            {
                WXOLE_TRACEOUT("Default Event Sink");
                defEventSink = true;
                if (impTypeFlags & IMPLTYPEFLAG_FDEFAULTVTABLE)
                {
                    WXOLE_TRACEOUT("*ERROR* - Default Event Sink is via vTable");
                    defEventSink = false;
                };
            }
            else
            {
                WXOLE_TRACEOUT("Default Interface");
                defInterface = true;
            }
        };


        // process
        GetTypeInfo(ti, defInterface, defEventSink);
    };


    // free
    typeInfo->ReleaseTypeAttr(ta);
};

void ElemDescToParam(const ELEMDESC& ed, wxActiveXOld::ParamX& param)
{
    param.flags = ed.idldesc.wIDLFlags;
    param.vt = ed.tdesc.vt;
    param.isPtr = (param.vt == VT_PTR);
    param.isSafeArray = (param.vt == VT_SAFEARRAY);
    if (param.isPtr || param.isSafeArray)
        param.vt = ed.tdesc.lptdesc->vt;
};

void wxActiveXOld::GetTypeInfo(ITypeInfo *ti, bool defInterface, bool defEventSink)
{
    // wxAutoOleInterface<> assumes a ref has already been added
    ti->AddRef();
    wxAutoOleInterface<ITypeInfo> typeInfo(ti);

    // TYPEATTR
    TYPEATTR *ta = NULL;
    HRESULT hret = typeInfo->GetTypeAttr(&ta);
    if (! ta)
        return;

    if (ta->typekind == TKIND_DISPATCH)
    {
        WXOLE_TRACEOUT("GUID = " << GetIIDName(ta->guid).c_str());

        if (defEventSink)
        {
            wxActiveXOldEvents *disp = new wxActiveXOldEvents(this, ta->guid);
            ConnectAdvise(ta->guid, disp);
        };


        // Get Function Names
        for (int i = 0; i < ta->cFuncs; i++)
        {
            FUNCDESC FAR *fd = NULL;

            hret = typeInfo->GetFuncDesc(i, &fd);
            if (! fd)
                continue;

            BSTR anames[1] = {NULL};
            unsigned int n = 0;

            hret = typeInfo->GetNames(fd->memid, anames, 1, &n);

            if (anames[0])
            {
                wxString name = anames[0];

                WXOLE_TRACEOUT("Name " << i << " = " << name.c_str());
                SysFreeString(anames[0]);

                if (defInterface || defEventSink)
                {
                    FuncX func;
                    func.name = name;
                    func.memid = fd->memid;
                    func.hasOut = false;

                    // get Param Names
                    unsigned int maxPNames = fd->cParams + 1;
                    unsigned int nPNames = 0;
                    BSTR *pnames = new BSTR[maxPNames];

                    hret = typeInfo->GetNames(fd->memid, pnames, maxPNames, &nPNames);

                    int pbase = 0;
                    if (fd->cParams < int(nPNames))
                    {
                        pbase++;
                        SysFreeString(pnames[0]);
                    };

                    // params
                    ElemDescToParam(fd->elemdescFunc, func.retType);
                    for (int p = 0; p < fd->cParams; p++)
                    {
                        ParamX param;

                        ElemDescToParam(fd->lprgelemdescParam[p], param);

                        param.name = pnames[pbase + p];
                        SysFreeString(pnames[pbase + p]);

                        func.hasOut |= (param.IsOut() || param.isPtr);
                        func.params.push_back(param);
                    };
                    delete [] pnames;

                    if (defEventSink)
                    {
                        m_events.push_back(func);
                        m_eventMemberIds[fd->memid] = m_events.size() - 1;
                    }
                    else
                    {
                        if (fd->invkind == INVOKE_FUNC)
                        {
                            m_methods.push_back(func);
                            m_methodNames[func.name] = m_methods.size() - 1;
                        }
                        else
                        {
                            NameMap::iterator it = m_propNames.find(func.name);
                            int idx = -1;
                            if (it == m_propNames.end())
                            {
                                m_props.push_back(PropX());
                                idx = m_props.size() - 1;
                                m_propNames[func.name] = idx;
                                m_props[idx].name = func.name;
                                m_props[idx].memid = func.memid;

                            }
                            else
                                idx = it->second;

                            if (fd->invkind == INVOKE_PROPERTYGET)
                                m_props[idx].type = func.retType;
                            else if (func.params.size() > 0)
                            {
                                m_props[idx].arg = func.params[0];
                                m_props[idx].putByRef = (fd->invkind == INVOKE_PROPERTYPUTREF);
                            };
                        };
                    };
                };
            };

            typeInfo->ReleaseFuncDesc(fd);
        };
    }

    typeInfo->ReleaseTypeAttr(ta);
};

///////////////////////////////////////////////
// Type Info exposure
const wxActiveXOld::FuncX& wxActiveXOld::GetEventDesc(int idx) const
{
    wxASSERT(idx >= 0 && idx < GetEventCount());

    return m_events[idx];
};

const wxActiveXOld::PropX& wxActiveXOld::GetPropDesc(int idx) const
{
    if (idx < 0 || idx >= GetPropCount())
        throw exception("Property index out of bounds");

    return m_props[idx];
};

const wxActiveXOld::PropX& wxActiveXOld::GetPropDesc(wxString name) const
{
    NameMap::const_iterator it = m_propNames.find(name);
    if (it == m_propNames.end())
    {
        wxString s;
        s << wxT("property <") << name << wxT("> not found");
        throw exception(s.mb_str());
    };

    return GetPropDesc(it->second);
};

const wxActiveXOld::FuncX& wxActiveXOld::GetMethodDesc(int idx) const
{
    if (idx < 0 || idx >= GetMethodCount())
        throw exception("Method index out of bounds");


    return m_methods[idx];
};


const wxActiveXOld::FuncX& wxActiveXOld::GetMethodDesc(wxString name) const
{
    NameMap::const_iterator it = m_methodNames.find(name);
    if (it == m_methodNames.end())
    {
        wxString s;
        s << wxT("method <") << name << wxT("> not found");
        throw exception(s.mb_str());
    };

    return GetMethodDesc(it->second);
};


void wxActiveXOld::SetProp(MEMBERID name, VARIANTARG& value)
{
    DISPID pids[1] = {DISPID_PROPERTYPUT};
    DISPPARAMS params = {&value, pids, 1, 1};

    EXCEPINFO x;
    memset(&x, 0, sizeof(x));
    unsigned int argErr = 0;

    HRESULT hr = m_Dispatch->Invoke(
        name,
        IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT,
        &params, NULL, &x, &argErr);

    WXOLE_WARN(hr, "Invoke Prop(...)");
};

void wxActiveXOld::SetProp(const wxString &name, const wxVariant &value)
{
    const PropX& prop = GetPropDesc(name);
    if (! prop.CanSet())
    {
        wxString s;
        s << wxT("property <") << name << wxT("> is readonly");
        throw exception(s.mb_str());
    };

    VARIANT v = {prop.arg.vt};
    VariantToMSWVariant(value, v);
    SetProp(prop.memid, v);
    VariantClear(&v); // this releases any BSTR's etc
};

VARIANT wxActiveXOld::GetPropAsVariant(MEMBERID name)
{
    VARIANT v;
    VariantInit(&v);

    DISPPARAMS params = {NULL, NULL, 0, 0};

    EXCEPINFO x;
    memset(&x, 0, sizeof(x));
    unsigned int argErr = 0;

    HRESULT hr = m_Dispatch->Invoke(
        name,
        IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET,
        &params, &v, &x, &argErr);

    WXOLE_WARN(hr, "Invoke Prop(...)");

    return v;
};

VARIANT wxActiveXOld::GetPropAsVariant(const wxString& name)
{
    const PropX& prop = GetPropDesc(name);
    if (! prop.CanGet())
    {
        wxString s;
        s << wxT("property <") << name << wxT("> is writeonly");
        throw exception(s.mb_str());
    };

    return GetPropAsVariant(prop.memid);
};

wxVariant wxActiveXOld::GetPropAsWxVariant(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BSTR);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    wxVariant wv;
    MSWVariantToVariant(v, wv);

    VariantClear(&v);

    return wv;
};

wxString wxActiveXOld::GetPropAsString(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BSTR);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    wxString s = v.bstrVal;
    VariantClear(&v);

    return s;
};

char wxActiveXOld::GetPropAsChar(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_I1);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.cVal;
};

long wxActiveXOld::GetPropAsLong(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_I4);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.iVal;
};

bool wxActiveXOld::GetPropAsBool(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BOOL);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.boolVal != 0;
};

double wxActiveXOld::GetPropAsDouble(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_R8);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.dblVal;
};

wxDateTime wxActiveXOld::GetPropAsDateTime(const wxString& name)
{
    wxDateTime dt;
    VARIANT v = GetPropAsVariant(name);

    if (! VariantToWxDateTime(v, dt))
        throw exception("Unable to convert variant to wxDateTime");

    return dt;
};

void *wxActiveXOld::GetPropAsPointer(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BYREF);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.byref;
};



// call methods
VARIANT wxActiveXOld::CallMethod(MEMBERID name, VARIANTARG args[], int argc)
{
    DISPPARAMS pargs = {args, NULL, argc, 0};
    VARIANT retVal;
    VariantInit(&retVal);

    EXCEPINFO x;
    memset(&x, 0, sizeof(x));
    unsigned int argErr = 0;

    HRESULT hr = m_Dispatch->Invoke(
        name,
        IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD,
        &pargs, &retVal, &x, &argErr);

    WXOLE_WARN(hr, "Invoke Method(...)");
    return retVal;
};

VARIANT wxActiveXOld::CallMethod(wxString name, VARIANTARG args[], int argc)
{
    const FuncX& func = GetMethodDesc(name);
    if (argc < 0)
        argc = func.params.size();

    return CallMethod(func.memid, args, argc);
};


wxVariant wxActiveXOld::CallMethod(wxString name, wxVariant args[], int nargs)
{
    const FuncX& func = GetMethodDesc(name);

    if (args == NULL)
        nargs = 0;

    VARIANTARG *vargs = NULL;
    if (nargs < 0)
        nargs = func.params.size();

    if (nargs > 0)
        vargs = new VARIANTARG[nargs];

    if (vargs)
    {
        // init type of vargs
        for (int i = 0; i < nargs; i++)
            vargs[nargs - i - 1].vt = func.params[i].vt;

        // put data
        for (i = 0; i < nargs; i++)
            VariantToMSWVariant(args[i], vargs[nargs - i - 1]);
    };

    VARIANT rv = CallMethod(func.memid, vargs, nargs);

    // process any by ref params
    if (func.hasOut)
    {
        for (int i = 0; i < nargs; i++)
        {
            VARIANTARG& va = vargs[nargs - i - 1];
            const wxActiveXOld::ParamX &px = func.params[i];

            if (px.IsOut())
            {
                wxVariant& vx = args[i];

                MSWVariantToVariant(va, vx);
            };
        };
    }

    if (vargs)
    {
        for (int i = 0; i < nargs; i++)
            VariantClear(&vargs[i]);
        delete [] vargs;
    };

    wxVariant ret;

    MSWVariantToVariant(rv, ret);
    VariantClear(&rv);

    return ret;
};


///////////////////////////////////////////////

HRESULT wxActiveXOld::ConnectAdvise(REFIID riid, IUnknown *events)
{
    wxOleConnectionPoint    cp;
    DWORD                    adviseCookie = 0;

    wxAutoOleInterface<IConnectionPointContainer> cpContainer(IID_IConnectionPointContainer, m_ActiveX);
    if (! cpContainer.Ok())
        return E_FAIL;

    HRESULT hret = cpContainer->FindConnectionPoint(riid, cp.GetRef());
    if (! SUCCEEDED(hret))
        return hret;

    hret = cp->Advise(events, &adviseCookie);

    if (SUCCEEDED(hret))
        m_connections.push_back(wxOleConnection(cp, adviseCookie));
    else
    {
        WXOLE_WARN(hret, "ConnectAdvise");
    };

    return hret;
};

HRESULT wxActiveXOld::AmbientPropertyChanged(DISPID dispid)
{
    wxAutoOleInterface<IOleControl> oleControl(IID_IOleControl, m_oleObject);

    if (oleControl.Ok())
        return oleControl->OnAmbientPropertyChange(dispid);
    else
        return S_FALSE;
};

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))

static void PixelsToHimetric(SIZEL &sz)
{
    static int logX = 0;
    static int logY = 0;

    if (logY == 0)
    {
        // initaliase
        HDC dc = GetDC(NULL);
        logX = GetDeviceCaps(dc, LOGPIXELSX);
        logY = GetDeviceCaps(dc, LOGPIXELSY);
        ReleaseDC(NULL, dc);
    };

#define HIMETRIC_INCH   2540
#define CONVERT(x, logpixels)   MulDiv(HIMETRIC_INCH, (x), (logpixels))

    sz.cx = CONVERT(sz.cx, logX);
    sz.cy = CONVERT(sz.cy, logY);

#undef CONVERT
#undef HIMETRIC_INCH
}


void wxActiveXOld::OnSize(wxSizeEvent& event)
{
    int w, h;
    GetClientSize(&w, &h);

    RECT posRect;
    posRect.left = 0;
    posRect.top = 0;
    posRect.right = w;
    posRect.bottom = h;

    if (w <= 0 && h <= 0)
        return;

    // extents are in HIMETRIC units
    if (m_oleObject.Ok())
    {
        SIZEL sz = {w, h};
        PixelsToHimetric(sz);

        SIZEL sz2;

        m_oleObject->GetExtent(DVASPECT_CONTENT, &sz2);
        if (sz2.cx !=  sz.cx || sz.cy != sz2.cy)
            m_oleObject->SetExtent(DVASPECT_CONTENT, &sz);
    };

    if (m_oleInPlaceObject.Ok())
        m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);
}

void wxActiveXOld::OnPaint(wxPaintEvent& event)
{
    wxLogTrace(wxT("paint"), wxT("repainting activex win"));
    wxPaintDC dc(this);
    dc.BeginDrawing();
    int w, h;
    GetSize(&w, &h);
    RECT posRect;
    posRect.left = 0;
    posRect.top = 0;
    posRect.right = w;
    posRect.bottom = h;

    // Draw only when control is windowless or deactivated
    if (m_viewObject)
    {
        ::RedrawWindow(m_oleObjectHWND, NULL, NULL, RDW_INTERNALPAINT);
        {
            RECTL *prcBounds = (RECTL *) &posRect;
            m_viewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL,
                (HDC)dc.GetHDC(), prcBounds, NULL, NULL, 0);
        }
    }
    else
    {
        dc.SetBrush(*wxRED_BRUSH);
        dc.DrawRectangle(0, 0, w, h);
        dc.SetBrush(wxNullBrush);
    }
    dc.EndDrawing();
}


void wxActiveXOld::OnMouse(wxMouseEvent& event)
{
    if (m_oleObjectHWND == NULL)
    {
        wxLogTrace(wxT("OnMouse"), wxT("no oleInPlaceObject"));
        event.Skip();
        return;
    }

    wxLogTrace(wxT("OnMouse"), wxT("mouse event"));
    UINT msg = 0;
    WPARAM wParam = 0;
    LPARAM lParam = 0;
    LRESULT lResult = 0;

    if (event.m_metaDown)
        wParam |= MK_CONTROL;
    if (event.m_shiftDown)
        wParam |= MK_SHIFT;
    if (event.m_leftDown)
        wParam |= MK_LBUTTON;
    if (event.m_middleDown)
        wParam |= MK_MBUTTON;
    if (event.m_rightDown)
        wParam |= MK_RBUTTON;
    lParam = event.m_x << 16;
    lParam |= event.m_y;

    if (event.LeftDown())
        msg = WM_LBUTTONDOWN;
    else if (event.LeftDClick())
        msg = WM_LBUTTONDBLCLK;
    else if (event.LeftUp())
        msg = WM_LBUTTONUP;
    else if (event.MiddleDown())
        msg = WM_MBUTTONDOWN;
    else if (event.MiddleDClick())
        msg = WM_MBUTTONDBLCLK;
    else if (event.MiddleUp())
        msg = WM_MBUTTONUP;
    else if (event.RightDown())
        msg = WM_RBUTTONDOWN;
    else if (event.RightDClick())
        msg = WM_RBUTTONDBLCLK;
    else if (event.RightUp())
        msg = WM_RBUTTONUP;
    else if (event.Moving() || event.Dragging())
        msg = WM_MOUSEMOVE;

    wxString log;
    if (msg == 0)
    {
        wxLogTrace(wxT("OnMouse"), wxT("no message"));
        event.Skip(); return;
    };

    if (!::SendMessage(m_oleObjectHWND, msg, wParam, lParam))
    {
        wxLogTrace(wxT("OnMouse"), wxT("msg not delivered"));
        event.Skip();
        return;
    };

    wxLogTrace(wxT("OnMouse"), wxT("msg sent"));
}

long wxActiveXOld::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if (m_oleObjectHWND == NULL)
        return wxWindow::MSWWindowProc(nMsg, wParam, lParam);

    switch(nMsg)
    {
    case WM_CHAR:
    case WM_DEADCHAR:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSCHAR:
    case WM_SYSDEADCHAR:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        PostMessage(m_oleObjectHWND, nMsg, wParam, lParam);

    default:
        return wxWindow::MSWWindowProc(nMsg, wParam, lParam);
    };
};

//////////////////////////////////////////////
// OK, this is ugly.  When hosting the IE control,
// the tab key was not navigating from one form
// field to the next.  This ugly little hack fixed it,
// but there was another problem.  The back-space
// key  (and others) were being processed twice.
// This method seems to fix the whole thing
static int g_dangerKeys[] =
{    VK_BACK,
    VK_LEFT,
    VK_UP,
    VK_RIGHT,
    VK_DOWN,
    0
};

bool wxActiveXOld::MSWTranslateMessage(WXMSG* pMsg)
{
    if(m_frameSite)
    {
        LPMSG msg;

        msg = (LPMSG)pMsg;

        // Trap key messages
        if(msg->message == WM_KEYDOWN ||
            msg->message == WM_KEYUP)
        {
            bool callAccelerator = true;
            int keyI = 0;

            // Check the list of keys that are "doubled"
            while(g_dangerKeys[keyI])
            {
                if(msg->wParam == (WPARAM) g_dangerKeys[keyI])
                {
                    callAccelerator = false;
                    break;
                }
                else
                    ++keyI;
            }

            // Call TranslateAccelerator for keys not in the list
            if(callAccelerator)
            {
                m_frameSite->TranslateAccelerator((LPMSG)pMsg, (WORD)0);
            }

            // Trying to make tab not do the usual wxWidgets thing
            // and tab to another control in the application.
            // TODO: this is very specific to wxIE so need to have
            // e.g. a virtual function EatsTab().
            if (msg->wParam == VK_TAB)
                return true;
        }
        else
        {
            m_frameSite->TranslateAccelerator((LPMSG)pMsg, (WORD)0);
        }
    }

    return wxWindow::MSWTranslateMessage(pMsg);
}
// End of hack
/////////////
void wxActiveXOld::OnSetFocus(wxFocusEvent& event)
{
    if (m_oleInPlaceActiveObject.Ok())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(TRUE);
}

void wxActiveXOld::OnKillFocus(wxFocusEvent& event)
{
    if (m_oleInPlaceActiveObject.Ok())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(FALSE);
}


wxActiveXOldFrameSite::wxActiveXOldFrameSite(wxActiveXOld * win)
{
    m_window = win;
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
    m_hWndParent = (HWND)m_window->GetHWND();
}

wxActiveXOldFrameSite::~wxActiveXOldFrameSite()
{
}


//IDispatch

HRESULT wxActiveXOldFrameSite::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, unsigned int cNames,
                                 LCID lcid, DISPID * rgDispId)
{
    WXOLE_TRACE("IDispatch::GetIDsOfNames");
    return E_NOTIMPL;
}

HRESULT wxActiveXOldFrameSite::GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
{
    WXOLE_TRACE("IDispatch::GetTypeInfo");
    return E_NOTIMPL;
}

HRESULT wxActiveXOldFrameSite::GetTypeInfoCount(unsigned int * pcTInfo)
{
    WXOLE_TRACE("IDispatch::GetTypeInfoCount");
    return E_NOTIMPL;
}

HRESULT wxActiveXOldFrameSite::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
                          WORD wFlags, DISPPARAMS * pDispParams,
                          VARIANT * pVarResult, EXCEPINFO * pExcepInfo,
                          unsigned int * puArgErr)
{
    WXOLE_TRACE("IDispatch::Invoke");

    if (!(wFlags & DISPATCH_PROPERTYGET))
        return S_OK;

    HRESULT hr;

    if (pVarResult == NULL)
        return E_INVALIDARG;

    //The most common case is boolean, use as an initial type
    V_VT(pVarResult) = VT_BOOL;

    switch (dispIdMember)
    {
        case DISPID_AMBIENT_MESSAGEREFLECT:
            WXOLE_TRACE("Invoke::DISPID_AMBIENT_MESSAGEREFLECT");
            V_BOOL(pVarResult)= FALSE;
            return S_OK;

        case DISPID_AMBIENT_DISPLAYASDEFAULT:
            WXOLE_TRACE("Invoke::DISPID_AMBIENT_DISPLAYASDEFAULT");
            V_BOOL(pVarResult)= TRUE;
            return S_OK;

        case DISPID_AMBIENT_OFFLINEIFNOTCONNECTED:
            WXOLE_TRACE("Invoke::DISPID_AMBIENT_OFFLINEIFNOTCONNECTED");
            V_BOOL(pVarResult) = TRUE;
            return S_OK;


        case DISPID_AMBIENT_SILENT:
            WXOLE_TRACE("Invoke::DISPID_AMBIENT_SILENT");
            V_BOOL(pVarResult)= TRUE;
            return S_OK;

        case DISPID_AMBIENT_APPEARANCE:
            pVarResult->vt = VT_BOOL;
            pVarResult->boolVal = m_bAmbientAppearance;
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
            pVarResult->boolVal = m_window->m_bAmbientUserMode;
            break;

        case DISPID_AMBIENT_SHOWGRABHANDLES:
            pVarResult->vt = VT_BOOL;
            pVarResult->boolVal = m_bAmbientShowGrabHandles;
            break;

        case DISPID_AMBIENT_SHOWHATCHING:
            pVarResult->vt = VT_BOOL;
            pVarResult->boolVal = m_bAmbientShowHatching;
            break;

        default:
            return DISP_E_MEMBERNOTFOUND;
    }

    return S_OK;
}

//IOleWindow

HRESULT wxActiveXOldFrameSite::GetWindow(HWND * phwnd)
{
    WXOLE_TRACE("IOleWindow::GetWindow");
    if (phwnd == NULL)
        return E_INVALIDARG;
    (*phwnd) = m_hWndParent;
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::ContextSensitiveHelp(BOOL fEnterMode)
{
    WXOLE_TRACE("IOleWindow::ContextSensitiveHelp");
    return S_OK;
}

//IOleInPlaceUIWindow

HRESULT wxActiveXOldFrameSite::GetBorder(LPRECT lprectBorder)
{
    WXOLE_TRACE("IOleInPlaceUIWindow::GetBorder");
    if (lprectBorder == NULL)
        return E_INVALIDARG;
    return INPLACE_E_NOTOOLSPACE;
}

HRESULT wxActiveXOldFrameSite::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
    WXOLE_TRACE("IOleInPlaceUIWindow::RequestBorderSpace");
    if (pborderwidths == NULL)
        return E_INVALIDARG;
    return INPLACE_E_NOTOOLSPACE;
}

HRESULT wxActiveXOldFrameSite::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
    WXOLE_TRACE("IOleInPlaceUIWindow::SetBorderSpace");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
    WXOLE_TRACE("IOleInPlaceUIWindow::SetActiveObject");

    if (pActiveObject)
        pActiveObject->AddRef();

    m_window->m_oleInPlaceActiveObject = pActiveObject;
    return S_OK;
}

//IOleInPlaceFrame

HRESULT wxActiveXOldFrameSite::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
    WXOLE_TRACE("IOleInPlaceFrame::InsertMenus");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
    WXOLE_TRACE("IOleInPlaceFrame::SetMenu");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::RemoveMenus(HMENU hmenuShared)
{
    WXOLE_TRACE("IOleInPlaceFrame::RemoveMenus");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::SetStatusText(LPCOLESTR pszStatusText)
{
    WXOLE_TRACE("IOleInPlaceFrame::SetStatusText");
    //((wxFrame*)wxGetApp().GetTopWindow())->GetStatusBar()->SetStatusText(pszStatusText);
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::EnableModeless(BOOL fEnable)
{
    WXOLE_TRACE("IOleInPlaceFrame::EnableModeless");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
    static MSG oldMsg;

    // Avoid infinite recursion in some circumstances
    if (lpmsg->message == oldMsg.message && lpmsg->lParam == oldMsg.lParam)
    {
        oldMsg.lParam = 0;
        oldMsg.message = 0;
        return S_FALSE;
    }

    oldMsg.lParam = lpmsg->lParam;
    oldMsg.message = lpmsg->message;
    HRESULT res = S_FALSE;

    WXOLE_TRACE("IOleInPlaceFrame::TranslateAccelerator");
    // TODO: send an event with this id
    if (m_window->m_oleInPlaceActiveObject.Ok())
    {
        HRESULT res = m_window->m_oleInPlaceActiveObject->TranslateAccelerator(lpmsg);
    }

    oldMsg.lParam = 0;
    oldMsg.message = 0;

    return res;
}

//IOleInPlaceSite

HRESULT wxActiveXOldFrameSite::CanInPlaceActivate()
{
    WXOLE_TRACE("IOleInPlaceSite::CanInPlaceActivate");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::OnInPlaceActivate()
{
    WXOLE_TRACE("IOleInPlaceSite::OnInPlaceActivate");
    m_bInPlaceActive = true;
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::OnUIActivate()
{
    WXOLE_TRACE("IOleInPlaceSite::OnUIActivate");
    m_bUIActive = true;
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::GetWindowContext(IOleInPlaceFrame **ppFrame,
                                    IOleInPlaceUIWindow **ppDoc,
                                    LPRECT lprcPosRect,
                                    LPRECT lprcClipRect,
                                    LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    WXOLE_TRACE("IOleInPlaceSite::GetWindowContext");
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
        WXOLE_TRACE("IOleInPlaceSite::IOleInPlaceFrame Error !");
        return E_UNEXPECTED;
    };

    hr = QueryInterface(IID_IOleInPlaceUIWindow, (void **) ppDoc);
    if (! SUCCEEDED(hr))
    {
        WXOLE_TRACE("IOleInPlaceSite::IOleInPlaceUIWindow Error !");
        (*ppFrame)->Release();
        *ppFrame = NULL;
        return E_UNEXPECTED;
    };

    int w, h;
    m_window->GetClientSize(&w, &h);
    if (lprcPosRect)
    {
        lprcPosRect->left = lprcPosRect->top = 0;
        lprcPosRect->right = w;
        lprcPosRect->bottom = h;
    };
    if (lprcClipRect)
    {
        lprcClipRect->left = lprcClipRect->top = 0;
        lprcClipRect->right = w;
        lprcClipRect->bottom = h;
    };

    memset(lpFrameInfo, 0, sizeof(OLEINPLACEFRAMEINFO));
    lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->hwndFrame = m_hWndParent;

    return S_OK;
}

HRESULT wxActiveXOldFrameSite::Scroll(SIZE scrollExtent)
{
    WXOLE_TRACE("IOleInPlaceSite::Scroll");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::OnUIDeactivate(BOOL fUndoable)
{
    WXOLE_TRACE("IOleInPlaceSite::OnUIDeactivate");
    m_bUIActive = false;
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::OnInPlaceDeactivate()
{
    WXOLE_TRACE("IOleInPlaceSite::OnInPlaceDeactivate");
    m_bInPlaceActive = false;
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::DiscardUndoState()
{
    WXOLE_TRACE("IOleInPlaceSite::DiscardUndoState");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::DeactivateAndUndo()
{
    WXOLE_TRACE("IOleInPlaceSite::DeactivateAndUndo");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::OnPosRectChange(LPCRECT lprcPosRect)
{
    WXOLE_TRACE("IOleInPlaceSite::OnPosRectChange");
    if (m_window->m_oleInPlaceObject.Ok() && lprcPosRect)
        m_window->m_oleInPlaceObject->SetObjectRects(lprcPosRect, lprcPosRect);

    return S_OK;
}

//IOleInPlaceSiteEx

HRESULT wxActiveXOldFrameSite::OnInPlaceActivateEx(BOOL * pfNoRedraw, DWORD dwFlags)
{
    WXOLE_TRACE("IOleInPlaceSiteEx::OnInPlaceActivateEx");
    OleLockRunning(m_window->m_ActiveX, TRUE, FALSE);
    if (pfNoRedraw)
        (*pfNoRedraw) = FALSE;
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::OnInPlaceDeactivateEx(BOOL fNoRedraw)
{
    WXOLE_TRACE("IOleInPlaceSiteEx::OnInPlaceDeactivateEx");
    OleLockRunning(m_window->m_ActiveX, FALSE, FALSE);
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::RequestUIActivate()
{
    WXOLE_TRACE("IOleInPlaceSiteEx::RequestUIActivate");
    return S_OK;
}


//IOleClientSite

HRESULT wxActiveXOldFrameSite::SaveObject()
{
    WXOLE_TRACE("IOleClientSite::SaveObject");
    return S_OK;
}

const char *OleGetMonikerToStr(DWORD dwAssign)
{
    switch (dwAssign)
    {
    case OLEGETMONIKER_ONLYIFTHERE  : return "OLEGETMONIKER_ONLYIFTHERE";
    case OLEGETMONIKER_FORCEASSIGN  : return "OLEGETMONIKER_FORCEASSIGN";
    case OLEGETMONIKER_UNASSIGN     : return "OLEGETMONIKER_UNASSIGN";
    case OLEGETMONIKER_TEMPFORUSER  : return "OLEGETMONIKER_TEMPFORUSER";
    default                         : return "Bad Enum";
    };
};

const char *OleGetWhicMonikerStr(DWORD dwWhichMoniker)
{
    switch(dwWhichMoniker)
    {
    case OLEWHICHMK_CONTAINER   : return "OLEWHICHMK_CONTAINER";
    case OLEWHICHMK_OBJREL      : return "OLEWHICHMK_OBJREL";
    case OLEWHICHMK_OBJFULL     : return "OLEWHICHMK_OBJFULL";
    default                     : return "Bad Enum";
    };
};

HRESULT wxActiveXOldFrameSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
                              IMoniker ** ppmk)
{
    WXOLE_TRACEOUT("IOleClientSite::GetMoniker(" << OleGetMonikerToStr(dwAssign) << ", " << OleGetWhicMonikerStr(dwWhichMoniker) << ")");


    if (! ppmk)
        return E_FAIL;

    /*
    HRESULT hr = CreateFileMoniker(L"e:\\dev\\wxie\\bug-zap.swf", ppmk);
    if (SUCCEEDED(hr))
        return S_OK;
    */
    *ppmk = NULL;

    return E_FAIL ;
}

HRESULT wxActiveXOldFrameSite::GetContainer(LPOLECONTAINER * ppContainer)
{
    WXOLE_TRACE("IOleClientSite::GetContainer");
    if (ppContainer == NULL)
        return E_INVALIDARG;

    HRESULT hr = QueryInterface(IID_IOleContainer, (void**)(ppContainer));
    wxASSERT(SUCCEEDED(hr));

    return hr;
}

HRESULT wxActiveXOldFrameSite::ShowObject()
{
    WXOLE_TRACE("IOleClientSite::ShowObject");
    if (m_window->m_oleObjectHWND)
        ::ShowWindow(m_window->m_oleObjectHWND, SW_SHOW);
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::OnShowWindow(BOOL fShow)
{
    WXOLE_TRACE("IOleClientSite::OnShowWindow");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::RequestNewObjectLayout()
{
    WXOLE_TRACE("IOleClientSite::RequestNewObjectLayout");
    return E_NOTIMPL;
}

// IParseDisplayName

HRESULT wxActiveXOldFrameSite::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName,
                                    ULONG *pchEaten, IMoniker **ppmkOut)
{
    WXOLE_TRACE("IParseDisplayName::ParseDisplayName");
    return E_NOTIMPL;
}

//IOleContainer

HRESULT wxActiveXOldFrameSite::EnumObjects(DWORD grfFlags, IEnumUnknown **ppenum)
{
    WXOLE_TRACE("IOleContainer::EnumObjects");
    return E_NOTIMPL;
}

HRESULT wxActiveXOldFrameSite::LockContainer(BOOL fLock)
{
    WXOLE_TRACE("IOleContainer::LockContainer");
    // TODO
    return S_OK;
}

//IOleItemContainer

HRESULT wxActiveXOldFrameSite::GetObject(LPOLESTR pszItem, DWORD dwSpeedNeeded,
                             IBindCtx * pbc, REFIID riid, void ** ppvObject)
{
    WXOLE_TRACE("IOleItemContainer::GetObject");
    if (pszItem == NULL)
        return E_INVALIDARG;
    if (ppvObject == NULL)
        return E_INVALIDARG;

    *ppvObject = NULL;
    return MK_E_NOOBJECT;
}

HRESULT wxActiveXOldFrameSite::GetObjectStorage(LPOLESTR pszItem, IBindCtx * pbc,
                                    REFIID riid, void ** ppvStorage)
{
    WXOLE_TRACE("IOleItemContainer::GetObjectStorage");
    if (pszItem == NULL)
        return E_INVALIDARG;
    if (ppvStorage == NULL)
        return E_INVALIDARG;

    *ppvStorage = NULL;
    return MK_E_NOOBJECT;
}

HRESULT wxActiveXOldFrameSite::IsRunning(LPOLESTR pszItem)
{
    WXOLE_TRACE("IOleItemContainer::IsRunning");
    if (pszItem == NULL)
        return E_INVALIDARG;

    return MK_E_NOOBJECT;
}



//IOleControlSite

HRESULT wxActiveXOldFrameSite::OnControlInfoChanged()
{
    WXOLE_TRACE("IOleControlSite::OnControlInfoChanged");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::LockInPlaceActive(BOOL fLock)
{
    WXOLE_TRACE("IOleControlSite::LockInPlaceActive");
    m_bInPlaceLocked = (fLock) ? true : false;
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::GetExtendedControl(IDispatch ** ppDisp)
{
    WXOLE_TRACE("IOleControlSite::GetExtendedControl");
    return E_NOTIMPL;
}

HRESULT wxActiveXOldFrameSite::TransformCoords(POINTL * pPtlHimetric, POINTF * pPtfContainer, DWORD dwFlags)
{
    WXOLE_TRACE("IOleControlSite::TransformCoords");
    HRESULT hr = S_OK;

    if (pPtlHimetric == NULL)
        return E_INVALIDARG;

    if (pPtfContainer == NULL)
        return E_INVALIDARG;

    return E_NOTIMPL;

}

HRESULT wxActiveXOldFrameSite::TranslateAccelerator(LPMSG pMsg, DWORD grfModifiers)
{
    WXOLE_TRACE("IOleControlSite::TranslateAccelerator");
    // TODO: send an event with this id
    return E_NOTIMPL;
}

HRESULT wxActiveXOldFrameSite::OnFocus(BOOL fGotFocus)
{
    WXOLE_TRACE("IOleControlSite::OnFocus");
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::ShowPropertyFrame()
{
    WXOLE_TRACE("IOleControlSite::ShowPropertyFrame");
    return E_NOTIMPL;
}

//IOleCommandTarget

HRESULT wxActiveXOldFrameSite::QueryStatus(const GUID * pguidCmdGroup, ULONG cCmds,
                               OLECMD * prgCmds, OLECMDTEXT * pCmdTet)
{
    WXOLE_TRACE("IOleCommandTarget::QueryStatus");
    if (prgCmds == NULL) return E_INVALIDARG;
    bool bCmdGroupFound = false;

    for (ULONG nCmd = 0; nCmd < cCmds; nCmd++)
    {
        // unsupported by default
        prgCmds[nCmd].cmdf = 0;

        // TODO
    }

    if (!bCmdGroupFound) { OLECMDERR_E_UNKNOWNGROUP; }
    return S_OK;
}

HRESULT wxActiveXOldFrameSite::Exec(const GUID * pguidCmdGroup, DWORD nCmdID,
                        DWORD nCmdExecOpt, VARIANTARG * pVaIn,
                        VARIANTARG * pVaOut)
{
    WXOLE_TRACE("IOleCommandTarget::Exec");
    bool bCmdGroupFound = false;

    if (!bCmdGroupFound) { OLECMDERR_E_UNKNOWNGROUP; }
    return OLECMDERR_E_NOTSUPPORTED;
}

//IAdviseSink

void STDMETHODCALLTYPE wxActiveXOldFrameSite::OnDataChange(FORMATETC * pFormatEtc, STGMEDIUM * pgStgMed)
{
    WXOLE_TRACE("IAdviseSink::OnDataChange");
}

void STDMETHODCALLTYPE wxActiveXOldFrameSite::OnViewChange(DWORD dwAspect, LONG lIndex)
{
    WXOLE_TRACE("IAdviseSink::OnViewChange");
    // redraw the control
}

void STDMETHODCALLTYPE wxActiveXOldFrameSite::OnRename(IMoniker * pmk)
{
    WXOLE_TRACE("IAdviseSink::OnRename");
}

void STDMETHODCALLTYPE wxActiveXOldFrameSite::OnSave()
{
    WXOLE_TRACE("IAdviseSink::OnSave");
}

void STDMETHODCALLTYPE wxActiveXOldFrameSite::OnClose()
{
    WXOLE_TRACE("IAdviseSink::OnClose");
}

/////////////////////////////////////////////
// IOleDocumentSite
HRESULT STDMETHODCALLTYPE wxActiveXOldFrameSite::ActivateMe(
        /* [in] */ IOleDocumentView __RPC_FAR *pViewToActivate)
{
    wxAutoOleInterface<IOleInPlaceSite> inPlaceSite(IID_IOleInPlaceSite, (IDispatch *) this);
    if (!inPlaceSite.Ok())
        return E_FAIL;

    if (pViewToActivate)
    {
        m_window->m_docView = pViewToActivate;
        m_window->m_docView->SetInPlaceSite(inPlaceSite);
    }
    else
    {
        wxAutoOleInterface<IOleDocument> oleDoc(IID_IOleDocument, m_window->m_oleObject);
        if (! oleDoc.Ok())
            return E_FAIL;

        HRESULT hr = oleDoc->CreateView(inPlaceSite, NULL, 0, m_window->m_docView.GetRef());
        if (hr != S_OK)
            return E_FAIL;

        m_window->m_docView->SetInPlaceSite(inPlaceSite);
    };

    m_window->m_docView->UIActivate(TRUE);

    return S_OK;
};


static IMalloc *iMalloc = NULL;

IMalloc *wxOleInit::GetIMalloc()
{
    assert(iMalloc);
    return iMalloc;
};

wxOleInit::wxOleInit()
{
    if (OleInitialize(NULL) == S_OK && iMalloc == NULL)
        CoGetMalloc(1, &iMalloc);
    else if (iMalloc)
        iMalloc->AddRef();
};

wxOleInit::~wxOleInit()
{
    if (iMalloc)
    {
        if (iMalloc->Release() == 0)
            iMalloc = NULL;
    };

    OleUninitialize();
}

bool GetSysErrMessage(int err, wxString& s)
{
    wxChar buf[256];
    if (FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM, NULL,
        err,0, buf, sizeof(buf), NULL) == 0)
        return false;

    buf[sizeof(buf) - 1] = 0;
    s = buf;
    return true;
};

wxString OLEHResultToString(HRESULT hr)
{
    // try formatmessage
    wxString err;
    if (GetSysErrMessage(hr, err))
        return err;

    switch (hr)
    {
    case S_OK:
        return wxT("");

    case CONNECT_E_CANNOTCONNECT:
        return wxT("Cannot connect to event interface (maybe not there ?) - see MSDN");

    case DISP_E_MEMBERNOTFOUND:
        return wxT("The requested member does not exist, or the call to Invoke tried to set the value of a read-only property.");

    case DISP_E_BADVARTYPE:
        return wxT("One of the parameters in rgvarg is not a valid variant type.");

    case DISP_E_BADPARAMCOUNT:
        return wxT("The number of elements provided to DISPPARAMS is different from the number of parameters accepted by the method or property");

    case DISP_E_EXCEPTION:
        return wxT("The application needs to raise an exception. In this case, the structure passed in pExcepInfo should be filled in.");

    case DISP_E_TYPEMISMATCH:
        return wxT("One or more of the parameters could not be coerced. The index within rgvarg of the first parameter with the incorrect type is returned in the puArgErr parameter.");

    case DISP_E_PARAMNOTOPTIONAL:
        return wxT("A required parameter was omitted.");

    case DISP_E_PARAMNOTFOUND:
        return wxT("One of the parameter DISPIDs does not correspond to a parameter on the method. In this case, puArgErr should be set to the first parameter that contains the error.");

    case OLECMDERR_E_UNKNOWNGROUP:
        return wxT("The pguidCmdGroup parameter is not NULL but does not specify a recognized command group.");

    case OLECMDERR_E_NOTSUPPORTED:
        return wxT("The nCmdID parameter is not a valid command in the group identified by pguidCmdGroup.");

    case OLECMDERR_E_DISABLED:
        return wxT("The command identified by nCmdID is currently disabled and cannot be executed.");

    case OLECMDERR_E_NOHELP:
        return wxT("The caller has asked for help on the command identified by nCmdID, but no help is available.");

    case OLECMDERR_E_CANCELED:
        return wxT("The user canceled the execution of the command.");

    case E_INVALIDARG:
        return wxT("E_INVALIDARG");

    case E_OUTOFMEMORY:
        return wxT("E_OUTOFMEMORY");

    case E_NOINTERFACE:
        return wxT("E_NOINTERFACE");

    case E_UNEXPECTED:
        return wxT("E_UNEXPECTED");

    case STG_E_INVALIDFLAG:
        return wxT("STG_E_INVALIDFLAG");

    case E_FAIL:
        return wxT("E_FAIL");

    case E_NOTIMPL:
        return wxT("E_NOTIMPL");

    default:
        {
            return wxString::Format(wxT("Unknown - 0x%X"), hr);
        }
    };
};

// borrowed from src/msw/ole/oleutils.cpp
wxString GetIIDName(REFIID riid)
{
  // an association between symbolic name and numeric value of an IID
  struct KNOWN_IID
  {
    const IID  *pIid;
    const wxChar *szName;
  };

  // construct the table containing all known interfaces
  #define ADD_KNOWN_IID(name) { &IID_I##name, _T(#name) }
  #define ADD_KNOWN_GUID(name) { &name, _T(#name) }

  static const KNOWN_IID aKnownIids[] =
  {
    ADD_KNOWN_IID(ServiceProvider),
    ADD_KNOWN_IID(AdviseSink),
    ADD_KNOWN_IID(AdviseSink2),
    ADD_KNOWN_IID(BindCtx),
    ADD_KNOWN_IID(ClassFactory),
#if ( !defined( __VISUALC__) || (__VISUALC__!=1010) ) && !defined(__MWERKS__)
    ADD_KNOWN_IID(ContinueCallback),
    ADD_KNOWN_IID(EnumOleDocumentViews),
    ADD_KNOWN_IID(OleCommandTarget),
    ADD_KNOWN_IID(OleDocument),
    ADD_KNOWN_IID(OleDocumentSite),
    ADD_KNOWN_IID(OleDocumentView),
    ADD_KNOWN_IID(Print),
#endif
    ADD_KNOWN_IID(DataAdviseHolder),
    ADD_KNOWN_IID(DataObject),
    ADD_KNOWN_IID(Debug),
    ADD_KNOWN_IID(DebugStream),
    ADD_KNOWN_IID(DfReserved1),
    ADD_KNOWN_IID(DfReserved2),
    ADD_KNOWN_IID(DfReserved3),
    ADD_KNOWN_IID(Dispatch),
    ADD_KNOWN_IID(DropSource),
    ADD_KNOWN_IID(DropTarget),
    ADD_KNOWN_IID(EnumCallback),
    ADD_KNOWN_IID(EnumFORMATETC),
    ADD_KNOWN_IID(EnumGeneric),
    ADD_KNOWN_IID(EnumHolder),
    ADD_KNOWN_IID(EnumMoniker),
    ADD_KNOWN_IID(EnumOLEVERB),
    ADD_KNOWN_IID(EnumSTATDATA),
    ADD_KNOWN_IID(EnumSTATSTG),
    ADD_KNOWN_IID(EnumString),
    ADD_KNOWN_IID(EnumUnknown),
    ADD_KNOWN_IID(EnumVARIANT),
    ADD_KNOWN_IID(ExternalConnection),
    ADD_KNOWN_IID(InternalMoniker),
    ADD_KNOWN_IID(LockBytes),
    ADD_KNOWN_IID(Malloc),
    ADD_KNOWN_IID(Marshal),
    ADD_KNOWN_IID(MessageFilter),
    ADD_KNOWN_IID(Moniker),
    ADD_KNOWN_IID(OleAdviseHolder),
    ADD_KNOWN_IID(OleCache),
    ADD_KNOWN_IID(OleCache2),
    ADD_KNOWN_IID(OleCacheControl),
    ADD_KNOWN_IID(OleClientSite),
    ADD_KNOWN_IID(OleContainer),
    ADD_KNOWN_IID(OleInPlaceActiveObject),
    ADD_KNOWN_IID(OleInPlaceFrame),
    ADD_KNOWN_IID(OleInPlaceObject),
    ADD_KNOWN_IID(OleInPlaceSite),
    ADD_KNOWN_IID(OleInPlaceUIWindow),
    ADD_KNOWN_IID(OleItemContainer),
    ADD_KNOWN_IID(OleLink),
    ADD_KNOWN_IID(OleManager),
    ADD_KNOWN_IID(OleObject),
    ADD_KNOWN_IID(OlePresObj),
    ADD_KNOWN_IID(OleWindow),
    ADD_KNOWN_IID(PSFactory),
    ADD_KNOWN_IID(ParseDisplayName),
    ADD_KNOWN_IID(Persist),
    ADD_KNOWN_IID(PersistFile),
    ADD_KNOWN_IID(PersistStorage),
    ADD_KNOWN_IID(PersistStream),
    ADD_KNOWN_IID(ProxyManager),
    ADD_KNOWN_IID(RootStorage),
    ADD_KNOWN_IID(RpcChannel),
    ADD_KNOWN_IID(RpcProxy),
    ADD_KNOWN_IID(RpcStub),
    ADD_KNOWN_IID(RunnableObject),
    ADD_KNOWN_IID(RunningObjectTable),
    ADD_KNOWN_IID(StdMarshalInfo),
    ADD_KNOWN_IID(Storage),
    ADD_KNOWN_IID(Stream),
    ADD_KNOWN_IID(StubManager),
    ADD_KNOWN_IID(Unknown),
    ADD_KNOWN_IID(ViewObject),
    ADD_KNOWN_IID(ViewObject2),
    ADD_KNOWN_GUID(IID_IDispatch),
    ADD_KNOWN_GUID(IID_IWebBrowser),
    ADD_KNOWN_GUID(IID_IWebBrowserApp),
    ADD_KNOWN_GUID(IID_IWebBrowser2),
    ADD_KNOWN_GUID(IID_IWebBrowser),
    ADD_KNOWN_GUID(DIID_DWebBrowserEvents2),
    ADD_KNOWN_GUID(DIID_DWebBrowserEvents),
  };

  // don't clobber preprocessor name space
  #undef ADD_KNOWN_IID
  #undef ADD_KNOWN_GUID

  // try to find the interface in the table
  for ( size_t ui = 0; ui < WXSIZEOF(aKnownIids); ui++ )
  {
    if ( riid == *aKnownIids[ui].pIid )
    {
      return aKnownIids[ui].szName;
    }
  }

  // unknown IID, just transform to string
  LPOLESTR str = NULL;
  StringFromIID(riid, &str);
  if (str)
  {
      wxString s = str;
      CoTaskMemFree(str);
      return s;
  }
  else
      return wxT("StringFromIID() error");
}

#endif
