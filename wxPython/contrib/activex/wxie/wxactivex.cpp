/*
                wxActiveX Library Licence, Version 3
                ====================================

  Copyright (C) 2003 Lindsay Mathieson [, ...]

  Everyone is permitted to copy and distribute verbatim copies
  of this licence document, but changing it is not allowed.

                       wxActiveX LIBRARY LICENCE
     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
  
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public Licence as published by
  the Free Software Foundation; either version 2 of the Licence, or (at
  your option) any later version.
  
  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
  General Public Licence for more details.

  You should have received a copy of the GNU Library General Public Licence
  along with this software, usually in a file named COPYING.LIB.  If not,
  write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA 02111-1307 USA.

  EXCEPTION NOTICE

  1. As a special exception, the copyright holders of this library give
  permission for additional uses of the text contained in this release of
  the library as licenced under the wxActiveX Library Licence, applying
  either version 3 of the Licence, or (at your option) any later version of
  the Licence as published by the copyright holders of version 3 of the
  Licence document.

  2. The exception is that you may use, copy, link, modify and distribute
  under the user's own terms, binary object code versions of works based
  on the Library.

  3. If you copy code from files distributed under the terms of the GNU
  General Public Licence or the GNU Library General Public Licence into a
  copy of this library, as this licence permits, the exception does not
  apply to the code that you add in this way.  To avoid misleading anyone as
  to the status of such modified files, you must delete this exception
  notice from such code and/or adjust the licensing conditions notice
  accordingly.

  4. If you write modifications of your own for this library, it is your
  choice whether to permit this exception to apply to your modifications. 
  If you do not wish that, you must delete the exception notice from such
  code and/or adjust the licensing conditions notice accordingly.
*/

#include "wxActiveX.h"
#include <wx/strconv.h>
#include <wx/event.h>
#include <wx/string.h>
#include <wx/datetime.h>
#include <wx/log.h>
#include <oleidl.h>
#include <winerror.h>
#include <idispids.h>
#include <olectl.h>
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

IMPLEMENT_CLASS(wxActiveX, wxWindow)
    
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

wxActiveX::wxActiveX(wxWindow * parent, const wxString& progId, wxWindowID id,
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

    // Unregister object as active
    RevokeActiveObject(m_pdwRegister, NULL);
}

void wxActiveX::CreateActiveX(REFCLSID clsid)
{
    HRESULT hret;

    ////////////////////////////////////////////////////////
    // FrameSite
    FrameSite *frame = new FrameSite(this);
    // oleClientSite
    hret = m_clientSite.QueryInterface(IID_IOleClientSite, (IDispatch *) frame);
    wxCHECK_RET(SUCCEEDED(hret), _T("m_clientSite.QueryInterface failed"));
    // adviseSink
    wxAutoOleInterface<IAdviseSink> adviseSink(IID_IAdviseSink, (IDispatch *) frame);
    wxCHECK_RET(adviseSink.Ok(), _T("adviseSink not Ok"));


    // Create Object, get IUnknown interface
    m_ActiveX.CreateInstance(clsid, IID_IUnknown);
    wxCHECK_RET(m_ActiveX.Ok(), _T("m_ActiveX.CreateInstance failed"));

    // Register object as active
    unsigned long pdwRegister;
    hret = RegisterActiveObject(m_ActiveX, clsid, ACTIVEOBJECT_WEAK, &m_pdwRegister);
    WXOLE_WARN(hret, "Unable to register object as active");

    // Get Dispatch interface
    hret = m_Dispatch.QueryInterface(IID_IDispatch, m_ActiveX); 
    WXOLE_WARN(hret, "Unable to get dispatch interface");

    // Type Info
    GetTypeInfo();

    // Get IOleObject interface
    hret = m_oleObject.QueryInterface(IID_IOleObject, m_ActiveX); 
    wxCHECK_RET(SUCCEEDED(hret), _("Unable to get IOleObject interface"));

    // get IViewObject Interface
    hret = m_viewObject.QueryInterface(IID_IViewObject, m_ActiveX); 
    wxCHECK_RET(SUCCEEDED(hret), _T("Unable to get IViewObject Interface"));

    // document advise
    m_docAdviseCookie = 0;
    hret = m_oleObject->Advise(adviseSink, &m_docAdviseCookie);
    WXOLE_WARN(hret, "m_oleObject->Advise(adviseSink, &m_docAdviseCookie),\"Advise\")");
    m_oleObject->SetHostNames(L"wxActiveXContainer", NULL);
    OleSetContainedObject(m_oleObject, TRUE);
    OleRun(m_oleObject);


    // Get IOleInPlaceObject interface
    hret = m_oleInPlaceObject.QueryInterface(IID_IOleInPlaceObject, m_ActiveX);
    wxCHECK_RET(SUCCEEDED(hret), _T("Unable to get IOleInPlaceObject interface"));

    // status
    DWORD dwMiscStatus;
    m_oleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    wxCHECK_RET(SUCCEEDED(hret), _T("Unable to get oleObject status"));

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
        GetEventHandler()->AddPendingEvent(szEvent);
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
typedef map<wxString, wxEventType *, NS_wxActiveX::less_wxStringI> ActiveXNamedEventMap;
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
        sg_NamedEventMap.clear();

        // DISPID events
        ActiveXDISPIDEventMap::iterator dit = sg_dispIdEventMap.end();
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
        if (byRef)
            vx = (wxChar) *va.pbVal;
        else
            vx = (wxChar) va.bVal;
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
            *va.pbVal = (wxChar) vx;
        else
            va.bVal = (wxChar) vx;
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
            *va.pbstrVal = SysAllocString(vx.GetString().wc_str(wxConvUTF8));
        else
            va.bstrVal = SysAllocString(vx.GetString().wc_str(wxConvUTF8));
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

IMPLEMENT_CLASS(wxActiveXEvent, wxCommandEvent)

class wxActiveXEvents : public IDispatch
{
private:
    DECLARE_OLE_UNKNOWN(wxActiveXEvents);


    wxActiveX   *m_activeX;
    IID         m_customId;
    bool        m_haveCustomId;

    friend bool wxActiveXEventsInterface(wxActiveXEvents *self, REFIID iid, void **_interface, const char *&desc);

public:
    wxActiveXEvents(wxActiveX *ax) : m_activeX(ax), m_haveCustomId(false) {}
    wxActiveXEvents(wxActiveX *ax, REFIID iid) : m_activeX(ax), m_haveCustomId(true), m_customId(iid) {}
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


    void DispatchEvent(wxActiveX::FuncX &func, const wxEventType& eventType, DISPPARAMS * pDispParams)
    {
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
            int nArg = wxMin(func.params.size(), pDispParams->cArgs);
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
            int nArg = wxMin(func.params.size(), pDispParams->cArgs);
            m_activeX->GetEventHandler()->ProcessEvent(event);
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
            m_activeX->GetEventHandler()->AddPendingEvent(event);

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
        wxActiveX::MemberIdMap::iterator mit = m_activeX->m_eventMemberIds.find((MEMBERID) dispIdMember);
        if (mit == m_activeX->m_eventMemberIds.end())
            return S_OK;

        // sanity check
        int midx = mit->second;
        if (midx < 0 || midx >= m_activeX->GetEventCount())
            return S_OK;

        wxActiveX::FuncX &func = m_activeX->m_events[midx];


        // try to find dispid event
        ActiveXDISPIDEventMap::iterator dit = sg_dispIdEventMap.find(dispIdMember);
        if (dit != sg_dispIdEventMap.end())
        {
            // Dispatch Event
            DispatchEvent(func, *(dit->second), pDispParams);
            return S_OK;
        };

        // try named event
        ActiveXNamedEventMap::iterator nit = sg_NamedEventMap.find(func.name);
        if (nit == sg_NamedEventMap.end())
            return S_OK;

        // Dispatch Event
        DispatchEvent(func, *(nit->second), pDispParams);
        return S_OK;
    }
};

bool wxActiveXEventsInterface(wxActiveXEvents *self, REFIID iid, void **_interface, const char *&desc)
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

DEFINE_OLE_TABLE(wxActiveXEvents)
    OLE_IINTERFACE(IUnknown)
    OLE_INTERFACE(IID_IDispatch, IDispatch)
    OLE_INTERFACE_CUSTOM(wxActiveXEventsInterface)
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

wxVariant& wxActiveXEvent::operator[] (int idx)
{
    wxASSERT(idx >= 0 && idx < ParamCount());

    return m_params[idx];
};

wxVariant& wxActiveXEvent::operator[] (wxString name)
{
    for (int i = 0; i < m_params.GetCount(); i++)
    {
        if (name.CmpNoCase(m_params[i].GetName()) == 0)
            return m_params[i];
    };

    wxString err = _T("wxActiveXEvent::operator[] invalid name <") + name + _T(">");
    err += _T("\r\nValid Names = :\r\n");
    for (i = 0; i < m_params.GetCount(); i++)
    {
        err += m_params[i].GetName();
        err += _T("\r\n");
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
    header  file for it.

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

void ElemDescToParam(const ELEMDESC& ed, wxActiveX::ParamX& param)
{
    param.flags = ed.idldesc.wIDLFlags;
    param.vt = ed.tdesc.vt;
    param.isPtr = (param.vt == VT_PTR);
    param.isSafeArray = (param.vt == VT_SAFEARRAY);
    if (param.isPtr || param.isSafeArray)
        param.vt = ed.tdesc.lptdesc->vt;
};

void wxActiveX::GetTypeInfo(ITypeInfo *ti, bool defInterface, bool defEventSink)
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
            wxActiveXEvents *disp = new wxActiveXEvents(this, ta->guid);
            ConnectAdvise(ta->guid, disp);
        };

        // Get properties
        // See bug #1280715 in the wxActiveX SF project
        int i;
        for (i = 0; i < ta->cVars; i++) {
            VARDESC FAR *vd = NULL;

            typeInfo->GetVarDesc(i, &vd) ;
            BSTR bstrProperty = NULL;
            typeInfo->GetDocumentation(vd->memid, &bstrProperty,
                                       NULL, NULL, NULL);
            wxString propName(bstrProperty);
            m_props.push_back(PropX());
            int idx = m_props.size() - 1;
            m_propNames[propName] = idx;
            m_props[idx].name = propName;
            m_props[idx].memid = vd->memid;

            ParamX param;
            param.isSafeArray = false;
            param.isPtr = false;
            param.flags = vd->elemdescVar.idldesc.wIDLFlags;
            param.vt = vd->elemdescVar.tdesc.vt;

            m_props[idx].arg = param;
            m_props[idx].type = param;
        }        

        // Get Function Names
        for (i = 0; i < ta->cFuncs; i++)
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

                        param.isOptional = (p > fd->cParams - fd->cParamsOpt);

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
const wxActiveX::FuncX& wxActiveX::GetEventDesc(int idx) const
{
    wxASSERT(idx >= 0 && idx < GetEventCount());

    return m_events[idx];
};

const wxActiveX::PropX& wxActiveX::GetPropDesc(int idx) const
{
    if (idx < 0 || idx >= GetPropCount())
        throw exception("Property index out of bounds");

    return m_props[idx];
};

const wxActiveX::PropX& wxActiveX::GetPropDesc(const wxString& name) const
{
    NameMap::const_iterator it = m_propNames.find(name);
    if (it == m_propNames.end())
    {
        wxString s;
        s << _T("property <") << name << _T("> not found");
        throw exception(s.mb_str());
    };

    return GetPropDesc(it->second);
};

const wxActiveX::FuncX& wxActiveX::GetMethodDesc(int idx) const
{
    if (idx < 0 || idx >= GetMethodCount())
        throw exception("Method index out of bounds");


    return m_methods[idx];
};


const wxActiveX::FuncX& wxActiveX::GetMethodDesc(const wxString& name) const
{
    NameMap::const_iterator it = m_methodNames.find(name);
    if (it == m_methodNames.end())
    {
        wxString s;
        s << _T("method <") << name << _T("> not found");
        throw exception(s.mb_str());
    };

    return GetMethodDesc(it->second);
};


void wxActiveX::SetProp(MEMBERID name, VARIANTARG& value)
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

void wxActiveX::SetProp(const wxString &name, const wxVariant &value)
{
    const PropX& prop = GetPropDesc(name);
    if (! prop.CanSet())
    {
        wxString s;
        s << _T("property <") << name << _T("> is readonly");
        throw exception(s.mb_str());
    };

    VARIANT v = {prop.arg.vt};
    VariantToMSWVariant(value, v);
    SetProp(prop.memid, v);
    VariantClear(&v); // this releases any BSTR's etc
};

VARIANT wxActiveX::GetPropAsVariant(MEMBERID name)
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

VARIANT wxActiveX::GetPropAsVariant(const wxString& name)
{
    const PropX& prop = GetPropDesc(name);
    if (! prop.CanGet())
    {
        wxString s;
        s << _T("property <") << name << _T("> is writeonly");
        throw exception(s.mb_str());
    };

    return GetPropAsVariant(prop.memid);
};
    
wxVariant wxActiveX::GetPropAsWxVariant(const wxString& name)
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

wxString wxActiveX::GetPropAsString(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BSTR);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    wxString s = v.bstrVal;
    VariantClear(&v);

    return s;
};

char wxActiveX::GetPropAsChar(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_I1);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.cVal;
};

long wxActiveX::GetPropAsLong(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_I4);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.iVal;
};

bool wxActiveX::GetPropAsBool(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BOOL);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.boolVal != 0;
};

double wxActiveX::GetPropAsDouble(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_R8);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.dblVal;
};

wxDateTime wxActiveX::GetPropAsDateTime(const wxString& name)
{
    wxDateTime dt;
    VARIANT v = GetPropAsVariant(name);

    if (! VariantToWxDateTime(v, dt))
        throw exception("Unable to convert variant to wxDateTime");

    return dt;
};

void *wxActiveX::GetPropAsPointer(const wxString& name)
{
    VARIANT v = GetPropAsVariant(name);
    HRESULT hr = VariantChangeType(&v, &v, 0, VT_BYREF);
    if (! SUCCEEDED(hr))
        throw exception("Unable to convert variant");

    return v.byref;
};



// call methods
VARIANT wxActiveX::CallMethod(MEMBERID name, VARIANTARG args[], int argc)
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

VARIANT wxActiveX::CallMethod(const wxString& name, VARIANTARG args[], int argc)
{
    const FuncX& func = GetMethodDesc(name);
    if (argc < 0)
        argc = func.params.size();

    return CallMethod(func.memid, args, argc);
};


wxVariant wxActiveX::CallMethod(const wxString& name, wxVariant args[], int nargs)
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
            const wxActiveX::ParamX &px = func.params[i];

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

HRESULT wxActiveX::ConnectAdvise(REFIID riid, IUnknown *events)
{
    wxOleConnectionPoint    cp;
    DWORD                   adviseCookie = 0;

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
    wxLogTrace(wxT(""),wxT("repainting activex win"));
    wxPaintDC dc(this);
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
}


void wxActiveX::OnMouse(wxMouseEvent& event)
{
    if (m_oleObjectHWND == NULL) 
    { 
        wxLogTrace(wxT(""),wxT("no oleInPlaceObject")); 
        event.Skip(); 
        return; 
    }

    wxLogTrace(wxT(""),wxT("mouse event"));
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
        wxLogTrace(wxT(""),wxT("no message"));
        event.Skip(); return; 
    };

    if (!::SendMessage(m_oleObjectHWND, msg, wParam, lParam)) 
    { 
        wxLogTrace(wxT(""),wxT("msg not delivered"));
        event.Skip(); 
        return; 
    };

    wxLogTrace(wxT(""),wxT("msg sent"));
}

bool wxActiveX::MSWTranslateMessage(WXMSG *msg){
	
	if (msg->message == WM_KEYDOWN){		
		HRESULT result = m_oleInPlaceActiveObject->TranslateAccelerator(msg);
		return (result == S_OK);
	}
	
	return wxWindow::MSWTranslateMessage(msg);
}

long wxActiveX::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
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

HRESULT FrameSite::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
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
        return wxEmptyString;

    case CONNECT_E_CANNOTCONNECT:
        return _T("Cannot connect to event interface (maybe not there ?) - see MSDN");

    case DISP_E_MEMBERNOTFOUND:
        return _T("The requested member does not exist, or the call to Invoke tried to set the value of a read-only property.");

    case DISP_E_BADVARTYPE:
        return _T("One of the parameters in rgvarg is not a valid variant type.");

    case DISP_E_BADPARAMCOUNT:
        return _T("The number of elements provided to DISPPARAMS is different from the number of parameters accepted by the method or property");

    case DISP_E_EXCEPTION:
        return _T("The application needs to raise an exception. In this case, the structure passed in pExcepInfo should be filled in.");

    case DISP_E_TYPEMISMATCH:
        return _T("One or more of the parameters could not be coerced. The index within rgvarg of the first parameter with the incorrect type is returned in the puArgErr parameter.");

    case DISP_E_PARAMNOTOPTIONAL:
        return _T("A required parameter was omitted.");

    case DISP_E_PARAMNOTFOUND:
        return _T("One of the parameter DISPIDs does not correspond to a parameter on the method. In this case, puArgErr should be set to the first parameter that contains the error.");

    case OLECMDERR_E_UNKNOWNGROUP:
        return _T("The pguidCmdGroup parameter is not NULL but does not specify a recognized command group.");

    case OLECMDERR_E_NOTSUPPORTED:
        return _T("The nCmdID parameter is not a valid command in the group identified by pguidCmdGroup.");

    case OLECMDERR_E_DISABLED:
        return _T("The command identified by nCmdID is currently disabled and cannot be executed.");

    case OLECMDERR_E_NOHELP:
        return _T("The caller has asked for help on the command identified by nCmdID, but no help is available.");

    case OLECMDERR_E_CANCELED:
        return _T("The user canceled the execution of the command.");

    case E_INVALIDARG:
        return _T("E_INVALIDARG");

    case E_OUTOFMEMORY:
        return _T("E_OUTOFMEMORY");

    case E_NOINTERFACE:
        return _T("E_NOINTERFACE");

    case E_UNEXPECTED:
        return _T("E_UNEXPECTED");

    case STG_E_INVALIDFLAG:
        return _T("STG_E_INVALIDFLAG");

    case E_FAIL:
        return _T("E_FAIL");

    case E_NOTIMPL:
        return _T("E_NOTIMPL");

    default:
        {
            wxString buf;
            buf.Printf(_T("Unknown - 0x%X"), hr);
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
      return _T("StringFromIID() error");
}
