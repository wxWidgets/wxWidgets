///////////////////////////////////////////////////////////////////////////////
// Name:        ole/droptgt.cpp
// Purpose:     wxDropTarget implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "droptgt.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/setup.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/log.h"

#ifdef __WIN32__
    #ifndef __GNUWIN32__
        #include <shlobj.h>            // for DROPFILES structure
    #endif
#else
    #include <shellapi.h>
#endif

#include "wx/dnd.h"

#ifndef __WIN32__
    #include <ole2.h>
    #include <olestd.h>
#endif

#include "wx/msw/ole/oleutils.h"

// ----------------------------------------------------------------------------
// IDropTarget interface: forward all interesting things to wxDropTarget
// (the name is unfortunate, but wx_I_DropTarget is not at all the same thing
//  as wxDropTarget which is 'public' class, while this one is private)
// ----------------------------------------------------------------------------

class wxIDropTarget : public IDropTarget
{
public:
    wxIDropTarget(wxDropTarget *p);
    ~wxIDropTarget();

    // accessors for wxDropTarget
    void SetHwnd(HWND hwnd) { m_hwnd = hwnd; }

    // IDropTarget methods
    STDMETHODIMP DragEnter(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
    STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD);
    STDMETHODIMP DragLeave();
    STDMETHODIMP Drop(LPDATAOBJECT, DWORD, POINTL, LPDWORD);

    DECLARE_IUNKNOWN_METHODS;

protected:
    IDataObject  *m_pIDataObject; // !NULL between DragEnter and DragLeave/Drop
    wxDropTarget *m_pTarget;      // the real target (we're just a proxy)

    HWND          m_hwnd;         // window we're associated with

    // get default drop effect for given keyboard flags
    static inline DWORD GetDropEffect(DWORD flags);
};

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxDragResult ConvertDragEffectToResult(DWORD dwEffect);
static DWORD ConvertDragResultToEffect(wxDragResult result);

// ============================================================================
// wxIDropTarget implementation
// ============================================================================

// Name    : static wxIDropTarget::GetDropEffect
// Purpose : determine the drop operation from keyboard/mouse state.
// Returns : DWORD combined from DROPEFFECT_xxx constants
// Params  : [in] DWORD flags       kbd & mouse flags as passed to
//                                  IDropTarget methods
// Notes   : We do "move" normally and "copy" if <Ctrl> is pressed,
//           which is the standard behaviour (currently there is no
//           way to redefine it)
DWORD wxIDropTarget::GetDropEffect(DWORD flags)
{
  return flags & MK_CONTROL ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
}

wxIDropTarget::wxIDropTarget(wxDropTarget *pTarget)
{
  m_cRef         = 0;
  m_pTarget      = pTarget;
  m_pIDataObject = NULL;
}

wxIDropTarget::~wxIDropTarget()
{
}

BEGIN_IID_TABLE(wxIDropTarget)
  ADD_IID(Unknown)
  ADD_IID(DropTarget)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxIDropTarget)

// Name    : wxIDropTarget::DragEnter
// Purpose : Called when the mouse enters the window (dragging something)
// Returns : S_OK
// Params  : [in] IDataObject *pIDataSource : source data
//           [in] DWORD        grfKeyState  : kbd & mouse state
//           [in] POINTL       pt           : mouse coordinates
//           [out]DWORD       *pdwEffect    : effect flag
// Notes   :
STDMETHODIMP wxIDropTarget::DragEnter(IDataObject *pIDataSource,
                                      DWORD        grfKeyState,
                                      POINTL       pt,
                                      DWORD       *pdwEffect)
{
    wxLogDebug(wxT("IDropTarget::DragEnter"));

    wxASSERT( m_pIDataObject == NULL );

    if ( !m_pTarget->IsAcceptedData(pIDataSource) ) {
        // we don't accept this kind of data
        *pdwEffect = DROPEFFECT_NONE;

        return S_OK;
    }

    // get hold of the data object
    m_pIDataObject = pIDataSource;
    m_pIDataObject->AddRef();

    // we need client coordinates to pass to wxWin functions
    if ( !ScreenToClient(m_hwnd, (POINT *)&pt) )
    {
        wxLogLastError("ScreenToClient");
    }

    // give some visual feedback
    *pdwEffect = ConvertDragResultToEffect(
                    m_pTarget->OnEnter(pt.x, pt.y,
                        ConvertDragEffectToResult(GetDropEffect(grfKeyState))
                    )
                 );

    return S_OK;
}

// Name    : wxIDropTarget::DragOver
// Purpose : Indicates that the mouse was moved inside the window represented
//           by this drop target.
// Returns : S_OK
// Params  : [in] DWORD   grfKeyState     kbd & mouse state
//           [in] POINTL  pt              mouse coordinates
//           [out]LPDWORD pdwEffect       effect flag
// Notes   : We're called on every WM_MOUSEMOVE, so this function should be
//           very efficient.
STDMETHODIMP wxIDropTarget::DragOver(DWORD   grfKeyState,
                                     POINTL  pt,
                                     LPDWORD pdwEffect)
{
    // there are too many of them... wxLogDebug("IDropTarget::DragOver");

    wxDragResult result;
    if ( m_pIDataObject ) {
        result = ConvertDragEffectToResult(GetDropEffect(grfKeyState));
    }
    else {
        // can't accept data anyhow normally
        result = wxDragNone;
    }

    // we need client coordinates to pass to wxWin functions
    if ( !ScreenToClient(m_hwnd, (POINT *)&pt) )
    {
        wxLogLastError("ScreenToClient");
    }

    *pdwEffect = ConvertDragResultToEffect(
                    m_pTarget->OnDragOver(pt.x, pt.y, result)
                 );

    return S_OK;
}

// Name    : wxIDropTarget::DragLeave
// Purpose : Informs the drop target that the operation has left its window.
// Returns : S_OK
// Notes   : good place to do any clean-up
STDMETHODIMP wxIDropTarget::DragLeave()
{
  wxLogDebug(wxT("IDropTarget::DragLeave"));

  // remove the UI feedback
  m_pTarget->OnLeave();

  // release the held object
  RELEASE_AND_NULL(m_pIDataObject);

  return S_OK;
}

// Name    : wxIDropTarget::Drop
// Purpose : Instructs the drop target to paste data that was just now
//           dropped on it.
// Returns : S_OK
// Params  : [in] IDataObject *pIDataSource     the data to paste
//           [in] DWORD        grfKeyState      kbd & mouse state
//           [in] POINTL       pt               where the drop occured?
//           [ouy]DWORD       *pdwEffect        operation effect
// Notes   :
STDMETHODIMP wxIDropTarget::Drop(IDataObject *pIDataSource,
                                 DWORD        grfKeyState,
                                 POINTL       pt,
                                 DWORD       *pdwEffect)
{
    wxLogDebug(wxT("IDropTarget::Drop"));

    // TODO I don't know why there is this parameter, but so far I assume
    //      that it's the same we've already got in DragEnter
    wxASSERT( m_pIDataObject == pIDataSource );

    // by default, nothing happens
    *pdwEffect = DROPEFFECT_NONE;

    // we need client coordinates to pass to wxWin functions
    if ( !ScreenToClient(m_hwnd, (POINT *)&pt) )
    {
        wxLogLastError("ScreenToClient");
    }

    // first ask the drop target if it wants data
    if ( m_pTarget->OnDrop(pt.x, pt.y) ) {
        // it does, so give it the data source
        m_pTarget->SetDataSource(pIDataSource);

        // and now it has the data
        wxDragResult rc = ConvertDragEffectToResult(GetDropEffect(grfKeyState));
        m_pTarget->OnData(pt.x, pt.y, rc);
        if ( wxIsDragResultOk(rc) ) {
            // operation succeeded
            *pdwEffect = ConvertDragResultToEffect(rc);
        }
        //else: *pdwEffect is already DROPEFFECT_NONE
    }
    //else: OnDrop() returned FALSE, no need to copy data

    // release the held object
    RELEASE_AND_NULL(m_pIDataObject);

    return S_OK;
}

// ============================================================================
// wxDropTarget implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget(wxDataObject *dataObj)
            : wxDropTargetBase(dataObj)
{
    // create an IDropTarget implementation which will notify us about d&d
    // operations.
    m_pIDropTarget = new wxIDropTarget(this);
    m_pIDropTarget->AddRef();
}

wxDropTarget::~wxDropTarget()
{
    ReleaseInterface(m_pIDropTarget);
}

// ----------------------------------------------------------------------------
// [un]register drop handler
// ----------------------------------------------------------------------------

bool wxDropTarget::Register(WXHWND hwnd)
{
    HRESULT hr = ::CoLockObjectExternal(m_pIDropTarget, TRUE, FALSE);
    if ( FAILED(hr) ) {
        wxLogApiError("CoLockObjectExternal", hr);
        return FALSE;
    }

    hr = ::RegisterDragDrop((HWND) hwnd, m_pIDropTarget);
    if ( FAILED(hr) ) {
        ::CoLockObjectExternal(m_pIDropTarget, FALSE, FALSE);

        wxLogApiError("RegisterDragDrop", hr);
        return FALSE;
    }

    // we will need the window handle for coords transformation later
    m_pIDropTarget->SetHwnd((HWND)hwnd);

    return TRUE;
}

void wxDropTarget::Revoke(WXHWND hwnd)
{
    HRESULT hr = ::RevokeDragDrop((HWND) hwnd);

    if ( FAILED(hr) ) {
        wxLogApiError("RevokeDragDrop", hr);
    }

    ::CoLockObjectExternal(m_pIDropTarget, FALSE, TRUE);

    m_pIDropTarget->SetHwnd(0);
}

// ----------------------------------------------------------------------------
// base class pure virtuals
// ----------------------------------------------------------------------------

// OnDrop() is called only if we previously returned TRUE from
// IsAcceptedData(), so no need to check anything here
bool wxDropTarget::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    return TRUE;
}

// copy the data from the data source to the target data object
bool wxDropTarget::GetData()
{
    wxDataFormat format = GetSupportedFormat(m_pIDataSource);
    if ( format == wxDF_INVALID ) {
        // this is strange because IsAcceptedData() succeeded previously!
        wxFAIL_MSG(wxT("strange - did supported formats list change?"));

        return FALSE;
    }

    STGMEDIUM stm;
    FORMATETC fmtMemory;
    fmtMemory.cfFormat  = format;
    fmtMemory.ptd       = NULL;
    fmtMemory.dwAspect  = DVASPECT_CONTENT;
    fmtMemory.lindex    = -1;
    fmtMemory.tymed     = TYMED_HGLOBAL;  // TODO to add other media

    bool rc = FALSE;

    HRESULT hr = m_pIDataSource->GetData(&fmtMemory, &stm);
    if ( SUCCEEDED(hr) ) {
        IDataObject *dataObject = m_dataObject->GetInterface();

        hr = dataObject->SetData(&fmtMemory, &stm, TRUE);
        if ( SUCCEEDED(hr) ) {
            rc = TRUE;
        }
        else {
            wxLogLastError("IDataObject::SetData()");
        }
    }
    else {
        wxLogLastError("IDataObject::GetData()");
    }

    return rc;
}

// ----------------------------------------------------------------------------
// callbacks used by wxIDropTarget
// ----------------------------------------------------------------------------

// we need a data source, so wxIDropTarget gives it to us using this function
void wxDropTarget::SetDataSource(IDataObject *pIDataSource)
{
    m_pIDataSource = pIDataSource;
}

// determine if we accept data of this type
bool wxDropTarget::IsAcceptedData(IDataObject *pIDataSource) const
{
    return GetSupportedFormat(pIDataSource) != wxDF_INVALID;
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

wxDataFormat wxDropTarget::GetSupportedFormat(IDataObject *pIDataSource) const
{
    // this strucutre describes a data of any type (first field will be
    // changing) being passed through global memory block.
    static FORMATETC s_fmtMemory = {
        0,
        NULL,
        DVASPECT_CONTENT,
        -1,
        TYMED_HGLOBAL       // TODO is it worth supporting other tymeds here?
    };

    // get the list of supported formats
    size_t nFormats = m_dataObject->GetFormatCount(wxDataObject::Set);
    wxDataFormat format, *formats;
    formats = nFormats == 1 ? &format :  new wxDataFormat[nFormats];

    m_dataObject->GetAllFormats(formats, wxDataObject::Set);

    // cycle through all supported formats
    size_t n;
    for ( n = 0; n < nFormats; n++ ) {
        s_fmtMemory.cfFormat = formats[n];

        // NB: don't use SUCCEEDED macro here: QueryGetData returns S_FALSE
        //     for file drag and drop (format == CF_HDROP)
        if ( pIDataSource->QueryGetData(&s_fmtMemory) == S_OK ) {
            format = formats[n];

            break;
        }
    }

    if ( formats != &format ) {
        // free memory if we allocated it
        delete [] formats;
    }

    return n < nFormats ? format : wxFormatInvalid;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxDragResult ConvertDragEffectToResult(DWORD dwEffect)
{
    switch ( dwEffect ) {
        case DROPEFFECT_COPY:
            return wxDragCopy;

        case DROPEFFECT_MOVE:
            return wxDragMove;

        default:
            wxFAIL_MSG(wxT("invalid value in ConvertDragEffectToResult"));
            // fall through

        case DROPEFFECT_NONE:
            return wxDragNone;
    }
}

static DWORD ConvertDragResultToEffect(wxDragResult result)
{
    switch ( result ) {
        case wxDragCopy:
            return DROPEFFECT_COPY;

        case wxDragMove:
            return DROPEFFECT_MOVE;

        default:
            wxFAIL_MSG(wxT("invalid value in ConvertDragResultToEffect"));
            // fall through

        case wxDragNone:
            return DROPEFFECT_NONE;
    }
}

#endif
 // wxUSE_DRAG_AND_DROP
