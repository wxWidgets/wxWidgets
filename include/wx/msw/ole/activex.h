///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/ole/activex.h
// Purpose:     wxActiveXContainer class
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     8/18/05
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Definitions
// ============================================================================

#ifndef _WX_MSW_OLE_ACTIVEXCONTAINER_H_
#define _WX_MSW_OLE_ACTIVEXCONTAINER_H_

#if wxUSE_ACTIVEX

//---------------------------------------------------------------------------
// wx includes
//---------------------------------------------------------------------------

#include "wx/msw/ole/oleutils.h" // wxBasicString &c
#include "wx/msw/ole/uuid.h"
#include "wx/msw/private/comptr.h"
#include "wx/window.h"
#include "wx/variant.h"

class FrameSite;

//---------------------------------------------------------------------------
// MSW COM includes
//---------------------------------------------------------------------------
#include <oleidl.h>
#include <olectl.h>

#include <exdisp.h>

#include <docobj.h>

#ifndef STDMETHOD
    #define STDMETHOD(funcname)  virtual HRESULT wxSTDCALL funcname
#endif

//
//  These defines are from another ole header - but its not in the
//  latest sdk.  Also the ifndef DISPID_READYSTATE is here because at
//  least on my machine with the latest sdk olectl.h defines these 3
//
#ifndef DISPID_READYSTATE
    #define DISPID_READYSTATE                               (-525)
    #define DISPID_READYSTATECHANGE                         (-609)
    #define DISPID_AMBIENT_TRANSFERPRIORITY                 (-728)
#endif

#define DISPID_AMBIENT_OFFLINEIFNOTCONNECTED            (-5501)
#define DISPID_AMBIENT_SILENT                           (-5502)

#ifndef DISPID_AMBIENT_CODEPAGE
    #define DISPID_AMBIENT_CODEPAGE                         (-725)
    #define DISPID_AMBIENT_CHARSET                          (-727)
#endif


//---------------------------------------------------------------------------
//
//  wxActiveXContainer
//
//---------------------------------------------------------------------------

template <class T>
class wxAutoOleInterface : public wxCOMPtr<T>
{
    typedef wxCOMPtr<T> base;
public:
    wxAutoOleInterface() {}
    wxAutoOleInterface(REFIID riid, IUnknown* pUnk)
    {
        QueryInterface(riid, pUnk);
    }

    wxAutoOleInterface& operator=(T*& ti)
    {
        Free();
        base::reset(ti);
        return *this;
    }
    void Free()
    {
        base::reset();
    }

    HRESULT QueryInterface(REFIID riid, IUnknown* pUnk)
    {
        Free();
        wxASSERT(pUnk != NULL);
        return pUnk->QueryInterface(riid, (void**)GetRef());
    }

    HRESULT CreateInstance(REFCLSID clsid, REFIID riid, DWORD dwClsContext = CLSCTX_ALL)
    {
        Free();
        return ::CoCreateInstance(
                   clsid,
                   NULL,
                   dwClsContext,
                   riid,
                   (void**)GetRef());
    }

    bool IsOk() const { return base::get() != NULL; }
    T** GetRef() { return base::operator&(); }
};

#if WXWIN_COMPATIBILITY_2_8
// this macro is kept for compatibility with older wx versions
#define WX_DECLARE_AUTOOLE(wxAutoOleInterfaceType, I) \
    typedef wxAutoOleInterface<I> wxAutoOleInterfaceType;
#endif // WXWIN_COMPATIBILITY_2_8

typedef wxAutoOleInterface<IDispatch> wxAutoIDispatch;
typedef wxAutoOleInterface<IOleClientSite> wxAutoIOleClientSite;
typedef wxAutoOleInterface<IUnknown> wxAutoIUnknown;
typedef wxAutoOleInterface<IOleObject> wxAutoIOleObject;
typedef wxAutoOleInterface<IOleInPlaceObject> wxAutoIOleInPlaceObject;
typedef wxAutoOleInterface<IOleInPlaceActiveObject> wxAutoIOleInPlaceActiveObject;
typedef wxAutoOleInterface<IOleDocumentView> wxAutoIOleDocumentView;
typedef wxAutoOleInterface<IViewObject> wxAutoIViewObject;

class WXDLLIMPEXP_CORE wxActiveXContainer : public wxWindow
{
public:
    wxActiveXContainer(wxWindow * parent, REFIID iid, IUnknown* pUnk);
    virtual ~wxActiveXContainer();

    void OnSize(wxSizeEvent&);
    void OnPaint(wxPaintEvent&);
    void OnSetFocus(wxFocusEvent&);
    void OnKillFocus(wxFocusEvent&);
    virtual bool MSWTranslateMessage(WXMSG* pMsg) wxOVERRIDE;
    virtual bool QueryClientSiteInterface(REFIID iid, void **_interface, const char *&desc);

protected:
    friend class FrameSite;
    friend class wxActiveXEvents;

    FrameSite *m_frameSite;
    wxAutoIDispatch            m_Dispatch;
    wxAutoIOleClientSite      m_clientSite;
    wxAutoIUnknown         m_ActiveX;
    wxAutoIOleObject            m_oleObject;
    wxAutoIOleInPlaceObject    m_oleInPlaceObject;
    wxAutoIOleInPlaceActiveObject m_oleInPlaceActiveObject;
    wxAutoIOleDocumentView    m_docView;
    wxAutoIViewObject            m_viewObject;
    HWND m_oleObjectHWND;
    bool m_bAmbientUserMode;
    DWORD m_docAdviseCookie;
    wxWindow* m_realparent;

    void CreateActiveX(REFIID, IUnknown*);
};

///\brief Store native event parameters.
///\detail Store OLE 'Invoke' parameters for event handlers that need to access them.
/// These are the exact values for the event as they are passed to the wxActiveXContainer.
struct wxActiveXEventNativeMSW
{
    DISPID  dispIdMember;
    REFIID  riid;
    LCID    lcid;
    WORD    wFlags;
    DISPPARAMS  *pDispParams;
    VARIANT     *pVarResult;
    EXCEPINFO   *pExcepInfo;
    unsigned int *puArgErr;

    wxActiveXEventNativeMSW
        (DISPID a_dispIdMember, REFIID a_riid, LCID a_lcid, WORD a_wFlags, DISPPARAMS  *a_pDispParams,
        VARIANT *a_pVarResult, EXCEPINFO *a_pExcepInfo, unsigned int *a_puArgErr)
        :dispIdMember(a_dispIdMember), riid(a_riid), lcid(a_lcid), wFlags(a_wFlags), pDispParams(a_pDispParams),
        pVarResult(a_pVarResult), pExcepInfo(a_pExcepInfo), puArgErr(a_puArgErr)
    { }
};

// Events
class WXDLLIMPEXP_CORE wxActiveXEvent : public wxCommandEvent
{
private:
    friend class wxActiveXEvents;
    wxVariant m_params;
    DISPID m_dispid;

public:
    virtual wxEvent *Clone() const wxOVERRIDE
    { return new wxActiveXEvent(*this); }

    size_t ParamCount() const;

    wxString ParamType(size_t idx) const
    {
        wxASSERT(idx < ParamCount());
        return m_params[idx].GetType();
    }

    wxString ParamName(size_t idx) const
    {
        wxASSERT(idx < ParamCount());
        return m_params[idx].GetName();
    }

    wxVariant& operator[] (size_t idx);

    DISPID GetDispatchId() const
    {   return m_dispid;    }

    wxActiveXEventNativeMSW *GetNativeParameters() const
    {   return (wxActiveXEventNativeMSW*)GetClientData(); }
};

// #define wxACTIVEX_ID    14001
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_ACTIVEX, wxActiveXEvent );

typedef void (wxEvtHandler::*wxActiveXEventFunction)(wxActiveXEvent&);

#define wxActiveXEventHandler(func) \
    wxEVENT_HANDLER_CAST( wxActiveXEventFunction, func )

#define EVT_ACTIVEX(id, fn)     wxDECLARE_EVENT_TABLE_ENTRY(wxEVT_ACTIVEX, id, -1, wxActiveXEventHandler( fn ), NULL ),

#endif // wxUSE_ACTIVEX

#endif // _WX_MSW_OLE_ACTIVEXCONTAINER_H_
