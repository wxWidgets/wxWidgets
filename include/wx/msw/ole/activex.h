///////////////////////////////////////////////////////////////////////////////
// Name:        wx/activex.h
// Purpose:     wxActiveXContainer class
// Author:      Ryan Norton <wxprojects@comcast.net>
// Modified by:
// Created:     8/18/05
// RCS-ID:      $Id$
// Copyright:   (c) Ryan Norton
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Definitions
// ============================================================================

#ifndef _WX_MSW_OLE_ACTIVEXCONTAINER_H_
#define _WX_MSW_OLE_ACTIVEXCONTAINER_H_

//---------------------------------------------------------------------------
//  COM includes
//---------------------------------------------------------------------------
#include "wx/msw/ole/oleutils.h" //wxBasicString, IID etc.
#include "wx/msw/ole/uuid.h" //IID etc..

//---------------------------------------------------------------------------
//  COM compatability definitions
//---------------------------------------------------------------------------
#ifndef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE __stdcall
#endif
#ifndef STDMETHOD
#define STDMETHOD(funcname)  virtual HRESULT STDMETHODCALLTYPE funcname
#endif
#ifndef PURE
#define PURE = 0
#endif
#ifndef __RPC_FAR
#define __RPC_FAR FAR
#endif

//---------------------------------------------------------------------------
//  WX includes
//---------------------------------------------------------------------------
#include "wx/window.h"

//---------------------------------------------------------------------------
// MSW COM includes
//---------------------------------------------------------------------------
#include <oleidl.h>
#include <olectl.h>

#if !defined(__WXWINCE__) || defined(__WINCE_STANDARDSDK__)
#include <exdisp.h>
#endif

#include <docobj.h>

//
//  These defines are from another ole header - but its not in the
//  latest sdk.  Also the ifndef DISPID_READYSTATE is here because at
//  least on my machine with the latest sdk olectl.h defines these 3
//
#ifndef DISPID_READYSTATE
    #define DISPID_READYSTATE                               -525
    #define DISPID_READYSTATECHANGE                         -609
    #define DISPID_AMBIENT_TRANSFERPRIORITY                 -728
#endif

#define DISPID_AMBIENT_OFFLINEIFNOTCONNECTED            -5501
#define DISPID_AMBIENT_SILENT                           -5502

#ifndef DISPID_AMBIENT_CODEPAGE
#   define DISPID_AMBIENT_CODEPAGE                         -725
#   define DISPID_AMBIENT_CHARSET                          -727
#endif


//---------------------------------------------------------------------------
//
//  wxActiveXContainer
//
//---------------------------------------------------------------------------

#define WX_DECLARE_AUTOOLE(wxAutoOleInterface, I) \
class wxAutoOleInterface \
{   \
    protected: \
    I *m_interface; \
\
    public: \
    explicit wxAutoOleInterface(I *pInterface = NULL) : m_interface(pInterface) {} \
    wxAutoOleInterface(REFIID riid, IUnknown *pUnk) : m_interface(NULL) \
    {   QueryInterface(riid, pUnk); } \
    wxAutoOleInterface(REFIID riid, IDispatch *pDispatch) : m_interface(NULL) \
    {   QueryInterface(riid, pDispatch); } \
    wxAutoOleInterface(REFCLSID clsid, REFIID riid) : m_interface(NULL)\
    {   CreateInstance(clsid, riid); }\
    wxAutoOleInterface(const wxAutoOleInterface& ti) : m_interface(NULL)\
    {   operator = (ti); }\
\
    wxAutoOleInterface& operator = (const wxAutoOleInterface& ti)\
    {\
        if (ti.m_interface)\
            ti.m_interface->AddRef();\
        Free();\
        m_interface = ti.m_interface;\
        return *this;\
    }\
\
    wxAutoOleInterface& operator = (I *&ti)\
    {\
        Free();\
        m_interface = ti;\
        return *this;\
    }\
\
    ~wxAutoOleInterface() {   Free();   }\
\
    inline void Free()\
    {\
        if (m_interface)\
            m_interface->Release();\
        m_interface = NULL;\
    }\
\
    HRESULT QueryInterface(REFIID riid, IUnknown *pUnk)\
    {\
        Free();\
        wxASSERT(pUnk != NULL);\
        return pUnk->QueryInterface(riid, (void **) &m_interface);\
    }\
\
    HRESULT CreateInstance(REFCLSID clsid, REFIID riid)\
    {\
        Free();\
        return CoCreateInstance(clsid, NULL, CLSCTX_ALL, riid, (void **) &m_interface);\
    }\
\
    inline operator I *() const {return m_interface;}\
    inline I* operator ->() {return m_interface;}\
    inline I** GetRef()    {return &m_interface;}\
    inline bool Ok() const    {return m_interface != NULL;}\
};

WX_DECLARE_AUTOOLE(wxAutoIDispatch, IDispatch)
WX_DECLARE_AUTOOLE(wxAutoIOleClientSite, IOleClientSite)
WX_DECLARE_AUTOOLE(wxAutoIUnknown, IUnknown)
WX_DECLARE_AUTOOLE(wxAutoIOleObject, IOleObject)
WX_DECLARE_AUTOOLE(wxAutoIOleInPlaceObject, IOleInPlaceObject)
WX_DECLARE_AUTOOLE(wxAutoIOleInPlaceActiveObject, IOleInPlaceActiveObject)
WX_DECLARE_AUTOOLE(wxAutoIOleDocumentView, IOleDocumentView)
WX_DECLARE_AUTOOLE(wxAutoIViewObject, IViewObject)
WX_DECLARE_AUTOOLE(wxAutoIOleInPlaceSite, IOleInPlaceSite)
WX_DECLARE_AUTOOLE(wxAutoIOleDocument, IOleDocument)
WX_DECLARE_AUTOOLE(wxAutoIPersistStreamInit, IPersistStreamInit)
WX_DECLARE_AUTOOLE(wxAutoIAdviseSink, IAdviseSink)

class wxActiveXContainer : public wxWindow
{
public:
    wxActiveXContainer(wxWindow * parent, REFIID iid, IUnknown* pUnk);
    virtual ~wxActiveXContainer();

    void OnSize(wxSizeEvent&);
    void OnPaint(wxPaintEvent&);
    void OnSetFocus(wxFocusEvent&);
    void OnKillFocus(wxFocusEvent&);

protected:
    friend class FrameSite;

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

#endif // _WX_MSW_OLE_ACTIVEXCONTAINER_H_

