#include "wxActiveX.h"
#include <wx/strconv.h>
#include <wx/msw/ole/uuid.h>
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
	EVT_SIZE(OnSize)
	EVT_SET_FOCUS(OnSetFocus)
	EVT_KILL_FOCUS(OnKillFocus)
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
	~FrameSite();

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
	CreateActiveX((LPOLESTR) wxConvUTF8.cMB2WC(progId).data());
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

	// Get IOleObject interface
	hret = m_oleObject.QueryInterface(IID_IOleObject, m_ActiveX);
	wxASSERT(SUCCEEDED(hret));
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

    // document advise
    m_docAdviseCookie = 0;
    hret = m_oleObject->Advise(adviseSink, &m_docAdviseCookie);
    WXOLE_WARN(hret, "m_oleObject->Advise(adviseSink, &m_docAdviseCookie),\"Advise\")");
    m_oleObject->SetHostNames(L"wxActiveXContainer", NULL);
	OleSetContainedObject(m_oleObject, TRUE);

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
	hret = m_oleInPlaceObject->GetWindow(&m_oleObjectHWND);
	WXOLE_WARN(hret, "m_oleInPlaceObject->GetWindow(&m_oleObjectHWND)");
    if (SUCCEEDED(hret))
	    ::SetActiveWindow(m_oleObjectHWND);


    if (! (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME))
    {
	    m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);

		hret = m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, m_clientSite, 0, (HWND)GetHWND(), &posRect);
        hret = m_oleObject->DoVerb(OLEIVERB_SHOW, 0, m_clientSite, 0, (HWND)GetHWND(), &posRect);
    };

}

void wxActiveX::CreateActiveX(LPOLESTR progId)
{
    CLSID clsid;
    if (CLSIDFromProgID(progId, &clsid) != S_OK)
    	return;

    CreateActiveX(clsid);
};

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

	if (m_oleInPlaceObject)
		m_oleInPlaceObject->SetObjectRects(&posRect, &posRect);

	// extents are in HIMETRIC units
    SIZEL sz = {w, h};
	PixelsToHimetric(sz);

    SIZEL sz2;
    m_oleObject->GetExtent(DVASPECT_CONTENT, &sz2);
    if (sz2.cx !=  sz.cx || sz.cy != sz2.cy)
        m_oleObject->SetExtent(DVASPECT_CONTENT, &sz);
}

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
    	return "";

    case OLECMDERR_E_UNKNOWNGROUP:
		return "The pguidCmdGroup parameter is not NULL but does not specify a recognized command group.";

    case OLECMDERR_E_NOTSUPPORTED:
		return "The nCmdID parameter is not a valid command in the group identified by pguidCmdGroup.";

    case OLECMDERR_E_DISABLED:
		return "The command identified by nCmdID is currently disabled and cannot be executed.";

    case OLECMDERR_E_NOHELP:
		return "The caller has asked for help on the command identified by nCmdID, but no help is available.";

    case OLECMDERR_E_CANCELED:
		return "The user canceled the execution of the command.";

    case E_INVALIDARG:
        return "E_INVALIDARG";

    case E_OUTOFMEMORY:
        return "E_OUTOFMEMORY";

    case E_NOINTERFACE:
        return "E_NOINTERFACE";

    case E_UNEXPECTED:
        return "E_UNEXPECTED";

    case STG_E_INVALIDFLAG:
        return "STG_E_INVALIDFLAG";

    case E_FAIL:
        return "E_FAIL";

    case E_NOTIMPL:
        return "E_NOTIMPL";

    default:
        {
            char buf[64];
            sprintf(buf, "Unknown - 0x%X", hr);
            return wxString(buf);
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
  };

  // don't clobber preprocessor name space
  #undef ADD_KNOWN_IID

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
      return "StringFromIID() error";
}
