#include "wxActiveX.h"
#include <wx/strconv.h>
#include <wx/event.h>
#include <oleidl.h>
#include <winerror.h>
#include <idispids.h>
#include <exdispid.h>
#include <olectl.h>
#include <Mshtml.h>
#include <sstream>
using namespace std;

// Depending on compilation mode, the wx headers may have undef'd
// this, but in this case we need it so the virtual method in
// FrameSite will match what is in oleidl.h.
#ifndef GetObject
    #ifdef _UNICODE
        #define GetObject GetObjectW
    #else
        #define GetObject GetObjectA
    #endif
#endif


//////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxActiveX, wxWindow)
    EVT_SIZE(wxActiveX::OnSize)
    EVT_PAINT(wxActiveX::OnPaint)
    EVT_MOUSE_EVENTS(wxActiveX::OnMouse)
	EVT_SET_FOCUS(wxActiveX::OnSetFocus)
	EVT_KILL_FOCUS(wxActiveX::OnKillFocus)
END_EVENT_TABLE()

class wxActiveX;

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
	FrameSite(wxActiveX * win);
	virtual ~FrameSite();

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

	wxActiveX * m_window;

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


wxActiveX::wxActiveX(wxWindow * parent, REFCLSID clsid, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name) :
wxWindow(parent, id, pos, size, style, name)
{
    m_bAmbientUserMode = true;
    m_docAdviseCookie = 0;
    CreateActiveX(clsid);
}

wxActiveX::wxActiveX(wxWindow * parent, wxString progId, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name) :
    wxWindow(parent, id, pos, size, style, name)
{
    m_bAmbientUserMode = true;
    m_docAdviseCookie = 0;
    CreateActiveX((LPOLESTR) (const wchar_t *) progId.wc_str(wxConvUTF8));
}

wxActiveX::~wxActiveX()
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

void wxActiveX::CreateActiveX(REFCLSID clsid)
{
	SetTransparent();

	HRESULT hret;

    ////////////////////////////////////////////////////////
	// FrameSite
    FrameSite *frame = new FrameSite(this);
    // oleClientSite
    hret = m_clientSite.QueryInterface(IID_IOleClientSite, (IDispatch *) frame);
    wxASSERT(SUCCEEDED(hret));
    // adviseSink
    wxAutoOleInterface<IAdviseSink> adviseSink(IID_IAdviseSink, (IDispatch *) frame);
    wxASSERT(adviseSink.Ok());


	// // Create Object, get IUnknown interface
    m_ActiveX.CreateInstance(clsid, IID_IUnknown);
	wxASSERT(m_ActiveX.Ok());

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
    m_oleObject->SetHostNames(L"wxActiveXContainer", NULL);
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
	};
}

void wxActiveX::CreateActiveX(LPOLESTR progId)
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
struct less_wxStringI
{
    bool operator()(const wxString& x, const wxString& y) const
    {
        return x.CmpNoCase(y) < 0;
    };
};

typedef map<wxString, wxEventType *, less_wxStringI> ActiveXNamedEventMap;
static ActiveXNamedEventMap sg_NamedEventMap;

const wxEventType& RegisterActiveXEvent(const wxChar *eventName)
{
    wxString ev = eventName;
    ActiveXNamedEventMap::iterator it = sg_NamedEventMap.find(ev);
    if (it == sg_NamedEventMap.end())
    {
        wxEventType  *et = new wxEventType(wxNewEventType());
        sg_NamedEventMap[ev] = et;

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
        // Named events
        ActiveXNamedEventMap::iterator it = sg_NamedEventMap.end();
        while (it != sg_NamedEventMap.end())
        {
            delete it->second;
            it++;
        };

        // DISPID events
        ActiveXDISPIDEventMap::iterator dit = sg_dispIdEventMap.end();
        while (dit != sg_dispIdEventMap.end())
        {
            delete dit->second;
            dit++;
        };
    };
};

static ActiveXEventMapFlusher s_dummyActiveXEventMapFlusher;


//////////////////////////////////////////////////////
bool MSWVariantToVariant(VARIANTARG& va, wxVariant& vx)
{
    switch(va.vt)
    {
	case VT_VARIANT | VT_BYREF:
		return MSWVariantToVariant(*va.pvarVal, vx);

    case VT_I2:
    case VT_I4:
        vx = (long) va.iVal;
        return true;

    case VT_I2 | VT_BYREF:
    case VT_I4 | VT_BYREF:
        vx = (long) *va.piVal;
        return true;

    case VT_BSTR:
        vx = wxString(va.bstrVal);
        return true;

    case VT_BSTR | VT_BYREF:
        vx = wxString(*va.pbstrVal);
        return true;

	case VT_BOOL:
		vx = (va.boolVal != FALSE);
		return true;

	case VT_BOOL | VT_BYREF:
		vx = (*va.pboolVal != FALSE);
		return true;

    default:
        vx.MakeNull();
        return false;
    };
};

bool VariantToMSWVariant(wxVariant& vx, VARIANTARG& va)
{
    switch(va.vt)
    {
	case VT_VARIANT | VT_BYREF:
		return VariantToMSWVariant(vx, va);

    case VT_I2:
    case VT_I4:
        va.iVal = (long) vx;
        return true;

    case VT_I2 | VT_BYREF:
    case VT_I4 | VT_BYREF:
         *va.piVal = (long) vx;
        return true;

	case VT_BOOL:
		va.boolVal = ((bool) vx) ? TRUE : FALSE;
		return true;

	case VT_BOOL | VT_BYREF:
		*va.pboolVal = ((bool) vx) ? TRUE : FALSE;
		return true;

    default:
        return false;
    };
};

class wxActiveXEvents : public IDispatch
{
private:
    DECLARE_OLE_UNKNOWN(wxActiveXEvents);


    wxActiveX *m_activeX;

public:
    wxActiveXEvents(wxActiveX *ax) : m_activeX(ax) {}
	virtual ~wxActiveXEvents()
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


    void DispatchEvent(int eventIdx, const wxEventType& eventType, DISPPARAMS * pDispParams)
    {
        wxASSERT(eventIdx >= 0 && eventIdx < int(m_activeX->m_events.size()));
        wxActiveX::FuncX &func = m_activeX->m_events[eventIdx];

		wxActiveXEvent  event;
    	event.SetId(m_activeX->GetId());
	    event.SetEventType(eventType);
        event.m_params.NullList();
        event.m_params.SetName(func.name);

        // arguments
        if (pDispParams)
        {
			// cdecl call
            // sometimes the pDispParams does not match the param info for a activex control
            int nArg = min(func.params.size(), pDispParams->cArgs);
            for (int i = nArg - 1; i >= 0; i--)
            {
                VARIANTARG& va = pDispParams->rgvarg[i];
				wxActiveX::ParamX &px = func.params[nArg - i - 1];
                wxVariant vx;

                vx.SetName(px.name);
                MSWVariantToVariant(va, vx);
                event.m_params.Append(vx);
            };
        };

		if (func.hasOut)
		{
            int nArg = min(func.params.size(), pDispParams->cArgs);
    		m_activeX->GetParent()->ProcessEvent(event);
            for (int i = 0; i < nArg; i++)
            {
                VARIANTARG& va = pDispParams->rgvarg[i];
				wxActiveX::ParamX &px = func.params[nArg - i - 1];

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

        // map dispid to m_eventsIdx
        wxActiveX::MemberIdList::iterator mid = m_activeX->m_eventsIdx.find((MEMBERID) dispIdMember);
        if (mid == m_activeX->m_eventsIdx.end())
            return S_OK;

        int funcIdx = mid->second;
        wxActiveX::FuncX &func = m_activeX->m_events[funcIdx];


        // try to find dispid event
        ActiveXDISPIDEventMap::iterator dit = sg_dispIdEventMap.find(dispIdMember);
        if (dit != sg_dispIdEventMap.end())
        {
            // Dispatch Event
            DispatchEvent(funcIdx, *(dit->second), pDispParams);
        	return S_OK;
        };

        // try named event
        ActiveXNamedEventMap::iterator nit = sg_NamedEventMap.find(func.name);
        if (nit == sg_NamedEventMap.end())
            return S_OK;

        // Dispatch Event
        DispatchEvent(funcIdx, *(nit->second), pDispParams);
    	return S_OK;
    }
};


DEFINE_OLE_TABLE(wxActiveXEvents)
	OLE_IINTERFACE(IUnknown)
	OLE_INTERFACE(IID_IDispatch, IDispatch)
END_OLE_TABLE;

wxString wxActiveXEvent::EventName()
{
    return m_params.GetName();
};

int wxActiveXEvent::ParamCount() const
{
    return m_params.GetCount();
};

wxString wxActiveXEvent::ParamType(int idx)
{
    wxASSERT(idx >= 0 && idx < m_params.GetCount());

    return m_params[idx].GetType();
};

wxString wxActiveXEvent::ParamName(int idx)
{
    wxASSERT(idx >= 0 && idx < m_params.GetCount());

    return m_params[idx].GetName();
};

static wxVariant nullVar;

wxVariant wxActiveXEvent::operator[] (int idx) const
{
    return (wxVariant&) operator[] (idx);
};

wxVariant& wxActiveXEvent::operator[] (int idx)
{
    wxASSERT(idx >= 0 && idx < ParamCount());

    return m_params[idx];
};

wxVariant wxActiveXEvent::operator[] (wxString name) const
{
    return (wxVariant&) operator[] (name);
};

wxVariant& wxActiveXEvent::operator[] (wxString name)
{
    for (int i = 0; i < m_params.GetCount(); i++)
    {
        if (name.CmpNoCase(m_params[i].GetName()) == 0)
            return m_params[i];
    };

    wxString err = wxT("wxActiveXEvent::operator[] invalid name <") + name + wxT(">");
    err += wxT("\r\nValid Names = :\r\n");
    for (i = 0; i < m_params.GetCount(); i++)
    {
        err += m_params[i].GetName();
        err += wxT("\r\n");
    };

    wxASSERT_MSG(false, err);

    return nullVar;
};

void wxActiveX::GetTypeInfo()
{
	/*
	We are currently only interested in the IDispatch interface
	to the control. For dual interfaces (TypeKind = TKIND_INTERFACE)
	we should drill down through the inheritance
	(using TYPEATTR->cImplTypes) and GetRefTypeOfImplType(n)
	and retrieve all the func names etc that way, then generate a C++
	header	file for it.

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
        bool defEventSink = false;
        int impTypeFlags = 0;
        typeInfo->GetImplTypeFlags(i, &impTypeFlags);

        if (impTypeFlags & IMPLTYPEFLAG_FDEFAULT)
        {
            if (impTypeFlags & IMPLTYPEFLAG_FSOURCE)
            {
                WXOLE_TRACEOUT(wxT("Default Event Sink"));
                defEventSink = true;
            }
            else
            {
                WXOLE_TRACEOUT(wxT("Default Interface"));
            }
        };


		// process
		GetTypeInfo(ti, defEventSink);
	};


    // free
    typeInfo->ReleaseTypeAttr(ta);
};

void wxActiveX::GetTypeInfo(ITypeInfo *ti, bool defEventSink)
{
	ti->AddRef();
	wxAutoOleInterface<ITypeInfo> typeInfo(ti);

	// TYPEATTR
	TYPEATTR *ta = NULL;
	HRESULT hret = typeInfo->GetTypeAttr(&ta);
	if (! ta)
		return;

	if (ta->typekind == TKIND_DISPATCH)
	{
        WXOLE_TRACEOUT(wxT("GUID = ") << GetIIDName(ta->guid).c_str());

        if (defEventSink)
        {
            wxActiveXEvents *disp = new wxActiveXEvents(this);
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

				WXOLE_TRACEOUT(wxT("Name ") << i << wxT(" = ") << name.c_str());
				SysFreeString(anames[0]);

                if (defEventSink)
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
                    wxASSERT(int(nPNames) >= fd->cParams + 1);

                    SysFreeString(pnames[0]);
					// params
					for (int p = 0; p < fd->cParams; p++)
					{
						ParamX param;

						param.flags = fd->lprgelemdescParam[p].idldesc.wIDLFlags;
						param.vt = fd->lprgelemdescParam[p].tdesc.vt;
                        param.isPtr = (param.vt == VT_PTR);
                        param.isSafeArray = (param.vt == VT_SAFEARRAY);
                        if (param.isPtr || param.isSafeArray)
                            param.vt = fd->lprgelemdescParam[p].tdesc.lptdesc->vt;

                        param.name = pnames[p + 1];
                        SysFreeString(pnames[p + 1]);

						func.hasOut |= (param.IsOut() || param.isPtr);
						func.params.push_back(param);
					};
                    delete [] pnames;

                    m_events.push_back(func);
                    m_eventsIdx[fd->memid] = m_events.size() - 1;
                };
			};

			typeInfo->ReleaseFuncDesc(fd);
		};
	}

	typeInfo->ReleaseTypeAttr(ta);
};

///////////////////////////////////////////////
// Type Info exposure
const wxActiveX::FuncX& wxActiveX::GetEvent(int idx) const
{
    wxASSERT(idx >= 0 && idx < GetEventCount());

    return m_events[idx];
};

///////////////////////////////////////////////

HRESULT wxActiveX::ConnectAdvise(REFIID riid, IUnknown *events)
{
	wxOleConnectionPoint	cp;
	DWORD					adviseCookie = 0;

	wxAutoOleInterface<IConnectionPointContainer> cpContainer(IID_IConnectionPointContainer, m_ActiveX);
	if (! cpContainer.Ok())
		return E_FAIL;

	HRESULT hret = cpContainer->FindConnectionPoint(riid, cp.GetRef());
	if (! SUCCEEDED(hret))
		return hret;

	hret = cp->Advise(events, &adviseCookie);

	if (SUCCEEDED(hret))
		m_connections.push_back(wxOleConnection(cp, adviseCookie));

	return hret;
};

HRESULT wxActiveX::AmbientPropertyChanged(DISPID dispid)
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


void wxActiveX::OnSize(wxSizeEvent& event)
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

void wxActiveX::OnPaint(wxPaintEvent& event)
{
//	wxLogTrace(wxT("repainting activex win"));
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


void wxActiveX::OnMouse(wxMouseEvent& event)
{
	if (m_oleObjectHWND == NULL)
    {
//        wxLogTrace(wxT("no oleInPlaceObject"));
        event.Skip();
        return;
    }

//	wxLogTrace(wxT("mouse event"));
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
//        wxLogTrace(wxT("no message"));
        event.Skip(); return;
    };

	if (!::SendMessage(m_oleObjectHWND, msg, wParam, lParam))
    {
//        wxLogTrace(wxT("msg not delivered"));
        event.Skip();
        return;
    };

//	wxLogTrace(wxT("msg sent"));
}

WXLRESULT wxActiveX::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
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

void wxActiveX::OnSetFocus(wxFocusEvent& event)
{
	if (m_oleInPlaceActiveObject.Ok())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(TRUE);
}

void wxActiveX::OnKillFocus(wxFocusEvent& event)
{
	if (m_oleInPlaceActiveObject.Ok())
        m_oleInPlaceActiveObject->OnFrameWindowActivate(FALSE);
}


FrameSite::FrameSite(wxActiveX * win)
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

FrameSite::~FrameSite()
{
}


//IDispatch

HRESULT FrameSite::GetIDsOfNames(REFIID riid, OLECHAR ** rgszNames, unsigned int cNames,
								 LCID lcid, DISPID * rgDispId)
{
	WXOLE_TRACE("IDispatch::GetIDsOfNames");
	return E_NOTIMPL;
}

HRESULT FrameSite::GetTypeInfo(unsigned int iTInfo, LCID lcid, ITypeInfo ** ppTInfo)
{
	WXOLE_TRACE("IDispatch::GetTypeInfo");
	return E_NOTIMPL;
}

HRESULT FrameSite::GetTypeInfoCount(unsigned int * pcTInfo)
{
	WXOLE_TRACE("IDispatch::GetTypeInfoCount");
	return E_NOTIMPL;
}

HRESULT FrameSite::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
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

HRESULT FrameSite::GetWindow(HWND * phwnd)
{
	WXOLE_TRACE("IOleWindow::GetWindow");
	if (phwnd == NULL)
        return E_INVALIDARG;
	(*phwnd) = m_hWndParent;
	return S_OK;
}

HRESULT FrameSite::ContextSensitiveHelp(BOOL fEnterMode)
{
	WXOLE_TRACE("IOleWindow::ContextSensitiveHelp");
	return S_OK;
}

//IOleInPlaceUIWindow

HRESULT FrameSite::GetBorder(LPRECT lprectBorder)
{
	WXOLE_TRACE("IOleInPlaceUIWindow::GetBorder");
	if (lprectBorder == NULL)
        return E_INVALIDARG;
	return INPLACE_E_NOTOOLSPACE;
}

HRESULT FrameSite::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	WXOLE_TRACE("IOleInPlaceUIWindow::RequestBorderSpace");
	if (pborderwidths == NULL)
        return E_INVALIDARG;
	return INPLACE_E_NOTOOLSPACE;
}

HRESULT FrameSite::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
	WXOLE_TRACE("IOleInPlaceUIWindow::SetBorderSpace");
	return S_OK;
}

HRESULT FrameSite::SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
	WXOLE_TRACE("IOleInPlaceUIWindow::SetActiveObject");

    if (pActiveObject)
        pActiveObject->AddRef();

    m_window->m_oleInPlaceActiveObject = pActiveObject;
	return S_OK;
}

//IOleInPlaceFrame

HRESULT FrameSite::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	WXOLE_TRACE("IOleInPlaceFrame::InsertMenus");
	return S_OK;
}

HRESULT FrameSite::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	WXOLE_TRACE("IOleInPlaceFrame::SetMenu");
	return S_OK;
}

HRESULT FrameSite::RemoveMenus(HMENU hmenuShared)
{
	WXOLE_TRACE("IOleInPlaceFrame::RemoveMenus");
	return S_OK;
}

HRESULT FrameSite::SetStatusText(LPCOLESTR pszStatusText)
{
	WXOLE_TRACE("IOleInPlaceFrame::SetStatusText");
	//((wxFrame*)wxGetApp().GetTopWindow())->GetStatusBar()->SetStatusText(pszStatusText);
	return S_OK;
}

HRESULT FrameSite::EnableModeless(BOOL fEnable)
{
	WXOLE_TRACE("IOleInPlaceFrame::EnableModeless");
	return S_OK;
}

HRESULT FrameSite::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	WXOLE_TRACE("IOleInPlaceFrame::TranslateAccelerator");
	// TODO: send an event with this id
    if (m_window->m_oleInPlaceActiveObject.Ok())
    	m_window->m_oleInPlaceActiveObject->TranslateAccelerator(lpmsg);

	return S_FALSE;
}

//IOleInPlaceSite

HRESULT FrameSite::CanInPlaceActivate()
{
	WXOLE_TRACE("IOleInPlaceSite::CanInPlaceActivate");
	return S_OK;
}

HRESULT FrameSite::OnInPlaceActivate()
{
	WXOLE_TRACE("IOleInPlaceSite::OnInPlaceActivate");
	m_bInPlaceActive = true;
	return S_OK;
}

HRESULT FrameSite::OnUIActivate()
{
	WXOLE_TRACE("IOleInPlaceSite::OnUIActivate");
	m_bUIActive = true;
	return S_OK;
}

HRESULT FrameSite::GetWindowContext(IOleInPlaceFrame **ppFrame,
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

HRESULT FrameSite::Scroll(SIZE scrollExtent)
{
	WXOLE_TRACE("IOleInPlaceSite::Scroll");
	return S_OK;
}

HRESULT FrameSite::OnUIDeactivate(BOOL fUndoable)
{
	WXOLE_TRACE("IOleInPlaceSite::OnUIDeactivate");
	m_bUIActive = false;
	return S_OK;
}

HRESULT FrameSite::OnInPlaceDeactivate()
{
	WXOLE_TRACE("IOleInPlaceSite::OnInPlaceDeactivate");
	m_bInPlaceActive = false;
	return S_OK;
}

HRESULT FrameSite::DiscardUndoState()
{
	WXOLE_TRACE("IOleInPlaceSite::DiscardUndoState");
	return S_OK;
}

HRESULT FrameSite::DeactivateAndUndo()
{
	WXOLE_TRACE("IOleInPlaceSite::DeactivateAndUndo");
	return S_OK;
}

HRESULT FrameSite::OnPosRectChange(LPCRECT lprcPosRect)
{
	WXOLE_TRACE("IOleInPlaceSite::OnPosRectChange");
    if (m_window->m_oleInPlaceObject.Ok() && lprcPosRect)
        m_window->m_oleInPlaceObject->SetObjectRects(lprcPosRect, lprcPosRect);

	return S_OK;
}

//IOleInPlaceSiteEx

HRESULT FrameSite::OnInPlaceActivateEx(BOOL * pfNoRedraw, DWORD dwFlags)
{
	WXOLE_TRACE("IOleInPlaceSiteEx::OnInPlaceActivateEx");
	OleLockRunning(m_window->m_ActiveX, TRUE, FALSE);
    if (pfNoRedraw)
        (*pfNoRedraw) = FALSE;
	return S_OK;
}

HRESULT FrameSite::OnInPlaceDeactivateEx(BOOL fNoRedraw)
{
	WXOLE_TRACE("IOleInPlaceSiteEx::OnInPlaceDeactivateEx");
    OleLockRunning(m_window->m_ActiveX, FALSE, FALSE);
	return S_OK;
}

HRESULT FrameSite::RequestUIActivate()
{
	WXOLE_TRACE("IOleInPlaceSiteEx::RequestUIActivate");
	return S_OK;
}


//IOleClientSite

HRESULT FrameSite::SaveObject()
{
	WXOLE_TRACE("IOleClientSite::SaveObject");
	return S_OK;
}

HRESULT FrameSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
							  IMoniker ** ppmk)
{
	WXOLE_TRACE("IOleClientSite::GetMoniker");
	return E_NOTIMPL;
}

HRESULT FrameSite::GetContainer(LPOLECONTAINER * ppContainer)
{
	WXOLE_TRACE("IOleClientSite::GetContainer");
	if (ppContainer == NULL)
        return E_INVALIDARG;

    HRESULT hr = QueryInterface(IID_IOleContainer, (void**)(ppContainer));
    wxASSERT(SUCCEEDED(hr));

	return hr;
}

HRESULT FrameSite::ShowObject()
{
	WXOLE_TRACE("IOleClientSite::ShowObject");
	if (m_window->m_oleObjectHWND)
		::ShowWindow(m_window->m_oleObjectHWND, SW_SHOW);
	return S_OK;
}

HRESULT FrameSite::OnShowWindow(BOOL fShow)
{
	WXOLE_TRACE("IOleClientSite::OnShowWindow");
	return S_OK;
}

HRESULT FrameSite::RequestNewObjectLayout()
{
	WXOLE_TRACE("IOleClientSite::RequestNewObjectLayout");
	return E_NOTIMPL;
}

// IParseDisplayName

HRESULT FrameSite::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName,
									ULONG *pchEaten, IMoniker **ppmkOut)
{
	WXOLE_TRACE("IParseDisplayName::ParseDisplayName");
	return E_NOTIMPL;
}

//IOleContainer

HRESULT FrameSite::EnumObjects(DWORD grfFlags, IEnumUnknown **ppenum)
{
	WXOLE_TRACE("IOleContainer::EnumObjects");
	return E_NOTIMPL;
}

HRESULT FrameSite::LockContainer(BOOL fLock)
{
	WXOLE_TRACE("IOleContainer::LockContainer");
	// TODO
	return S_OK;
}

//IOleItemContainer

HRESULT FrameSite::GetObject(LPOLESTR pszItem, DWORD dwSpeedNeeded,
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

HRESULT FrameSite::GetObjectStorage(LPOLESTR pszItem, IBindCtx * pbc,
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

HRESULT FrameSite::IsRunning(LPOLESTR pszItem)
{
	WXOLE_TRACE("IOleItemContainer::IsRunning");
	if (pszItem == NULL)
        return E_INVALIDARG;

	return MK_E_NOOBJECT;
}



//IOleControlSite

HRESULT FrameSite::OnControlInfoChanged()
{
	WXOLE_TRACE("IOleControlSite::OnControlInfoChanged");
	return S_OK;
}

HRESULT FrameSite::LockInPlaceActive(BOOL fLock)
{
	WXOLE_TRACE("IOleControlSite::LockInPlaceActive");
	m_bInPlaceLocked = (fLock) ? true : false;
	return S_OK;
}

HRESULT FrameSite::GetExtendedControl(IDispatch ** ppDisp)
{
	WXOLE_TRACE("IOleControlSite::GetExtendedControl");
	return E_NOTIMPL;
}

HRESULT FrameSite::TransformCoords(POINTL * pPtlHimetric, POINTF * pPtfContainer, DWORD dwFlags)
{
	WXOLE_TRACE("IOleControlSite::TransformCoords");
	HRESULT hr = S_OK;

	if (pPtlHimetric == NULL)
		return E_INVALIDARG;

	if (pPtfContainer == NULL)
		return E_INVALIDARG;

	return E_NOTIMPL;

}

HRESULT FrameSite::TranslateAccelerator(LPMSG pMsg, DWORD grfModifiers)
{
	WXOLE_TRACE("IOleControlSite::TranslateAccelerator");
	// TODO: send an event with this id
	return E_NOTIMPL;
}

HRESULT FrameSite::OnFocus(BOOL fGotFocus)
{
	WXOLE_TRACE("IOleControlSite::OnFocus");
	return S_OK;
}

HRESULT FrameSite::ShowPropertyFrame()
{
	WXOLE_TRACE("IOleControlSite::ShowPropertyFrame");
	return E_NOTIMPL;
}

//IOleCommandTarget

HRESULT FrameSite::QueryStatus(const GUID * pguidCmdGroup, ULONG cCmds,
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

HRESULT FrameSite::Exec(const GUID * pguidCmdGroup, DWORD nCmdID,
						DWORD nCmdExecOpt, VARIANTARG * pVaIn,
						VARIANTARG * pVaOut)
{
	WXOLE_TRACE("IOleCommandTarget::Exec");
	bool bCmdGroupFound = false;

	if (!bCmdGroupFound) { OLECMDERR_E_UNKNOWNGROUP; }
	return OLECMDERR_E_NOTSUPPORTED;
}

//IAdviseSink

void STDMETHODCALLTYPE FrameSite::OnDataChange(FORMATETC * pFormatEtc, STGMEDIUM * pgStgMed)
{
	WXOLE_TRACE("IAdviseSink::OnDataChange");
}

void STDMETHODCALLTYPE FrameSite::OnViewChange(DWORD dwAspect, LONG lIndex)
{
	WXOLE_TRACE("IAdviseSink::OnViewChange");
	// redraw the control
}

void STDMETHODCALLTYPE FrameSite::OnRename(IMoniker * pmk)
{
	WXOLE_TRACE("IAdviseSink::OnRename");
}

void STDMETHODCALLTYPE FrameSite::OnSave()
{
	WXOLE_TRACE("IAdviseSink::OnSave");
}

void STDMETHODCALLTYPE FrameSite::OnClose()
{
	WXOLE_TRACE("IAdviseSink::OnClose");
}

/////////////////////////////////////////////
// IOleDocumentSite
HRESULT STDMETHODCALLTYPE FrameSite::ActivateMe(
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

wxString OLEHResultToString(HRESULT hr)
{
    switch (hr)
    {
    case S_OK:
    	return wxEmptyString;

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
            wxString buf;
            buf.Printf(wxT("Unknown - 0x%X"), hr);
            return buf;
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
