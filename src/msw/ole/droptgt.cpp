///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/ole/droptgt.cpp
// Purpose:     wxDropTarget implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#if wxUSE_OLE && wxUSE_DRAG_AND_DROP

#ifndef WX_PRECOMP
    #include "wx/msw/wrapwin.h"
    #include "wx/log.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/private/comptr.h"

#include "wx/msw/wrapshl.h"            // for DROPFILES structure

#include "wx/dnd.h"
#include "wx/except.h"

#include "wx/msw/ole/oleutils.h"

#include <initguid.h>

// Some (very) old SDKs don't define IDropTargetHelper, so define our own
// version of it here.
struct wxIDropTargetHelper : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE DragEnter(HWND hwndTarget,
                                                IDataObject *pDataObject,
                                                POINT *ppt,
                                                DWORD dwEffect) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragLeave() = 0;
    virtual HRESULT STDMETHODCALLTYPE DragOver(POINT *ppt, DWORD dwEffect) = 0;
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObject,
                                           POINT *ppt,
                                           DWORD dwEffect) = 0;
    virtual HRESULT STDMETHODCALLTYPE Show(BOOL fShow) = 0;
};

namespace
{
    DEFINE_GUID(wxCLSID_DragDropHelper,
                0x4657278A,0x411B,0x11D2,0x83,0x9A,0x00,0xC0,0x4F,0xD9,0x18,0xD0);
    DEFINE_GUID(wxIID_IDropTargetHelper,
                0x4657278B,0x411B,0x11D2,0x83,0x9A,0x00,0xC0,0x4F,0xD9,0x18,0xD0);
}

// ----------------------------------------------------------------------------
// IDropTarget interface: forward all interesting things to wxDropTarget
// (the name is unfortunate, but wx_I_DropTarget is not at all the same thing
//  as wxDropTarget which is 'public' class, while this one is private)
// ----------------------------------------------------------------------------

class wxIDropTarget : public IDropTarget
{
public:
    wxIDropTarget(wxDropTarget *p);
    virtual ~wxIDropTarget();

    // accessors for wxDropTarget
    HWND GetHWND() const { return m_hwnd; }
    void SetHwnd(HWND hwnd) { m_hwnd = hwnd; }

    // IDropTarget methods
    STDMETHODIMP DragEnter(LPDATAOBJECT, DWORD, POINTL, LPDWORD) wxOVERRIDE;
    STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD) wxOVERRIDE;
    STDMETHODIMP DragLeave() wxOVERRIDE;
    STDMETHODIMP Drop(LPDATAOBJECT, DWORD, POINTL, LPDWORD) wxOVERRIDE;

    DECLARE_IUNKNOWN_METHODS;

protected:
    // This pointer is !NULL between the calls to DragEnter and DragLeave/Drop
    wxCOMPtr<IDataObject> m_pIDataObject;

    wxDropTarget *m_pTarget;      // the real target (we're just a proxy)

    HWND          m_hwnd;         // window we're associated with

    // get default drop effect for given keyboard flags
    static DWORD GetDropEffect(DWORD flags, wxDragResult defaultAction, DWORD pdwEffect);

#if wxUSE_EXCEPTIONS
    // Helper function called if an exceptions happens in any of the
    // user-defined methods: it ensures that the exception doesn't escape and
    // also resets the data object, as drag-and-drop operation will be aborted
    // if this happens.
    HRESULT HandleException()
    {
        wxEvtHandler::WXConsumeException();

        m_pIDataObject.reset();

        return E_UNEXPECTED;
    }
#endif // wxUSE_EXCEPTIONS

    wxDECLARE_NO_COPY_CLASS(wxIDropTarget);
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
// Params  : [in] DWORD flags                 kbd & mouse flags as passed to
//                                            IDropTarget methods
//           [in] wxDragResult defaultAction  the default action of the drop target
//           [in] DWORD pdwEffect             the supported actions of the drop
//                                            source passed to IDropTarget methods
// Notes   : We do "move" normally and "copy" if <Ctrl> is pressed,
//           which is the standard behaviour (currently there is no
//           way to redefine it)
DWORD wxIDropTarget::GetDropEffect(DWORD flags,
                                   wxDragResult defaultAction,
                                   DWORD pdwEffect)
{
    DWORD effectiveAction;
    if ( defaultAction == wxDragCopy )
        effectiveAction = flags & MK_SHIFT ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
    else
        effectiveAction = flags & MK_CONTROL ? DROPEFFECT_COPY : DROPEFFECT_MOVE;

    if ( !(effectiveAction & pdwEffect) )
    {
        // the action is not supported by drag source, fall back to something
        // that it does support
        if ( pdwEffect & DROPEFFECT_MOVE )
            effectiveAction = DROPEFFECT_MOVE;
        else if ( pdwEffect & DROPEFFECT_COPY )
            effectiveAction = DROPEFFECT_COPY;
        else if ( pdwEffect & DROPEFFECT_LINK )
            effectiveAction = DROPEFFECT_LINK;
        else
            effectiveAction = DROPEFFECT_NONE;
    }

    return effectiveAction;
}

wxIDropTarget::wxIDropTarget(wxDropTarget *pTarget)
{
  m_pTarget      = pTarget;
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
// Params  : [in]    IDataObject *pIDataSource : source data
//           [in]    DWORD        grfKeyState  : kbd & mouse state
//           [in]    POINTL       pt           : mouse coordinates
//           [in/out]DWORD       *pdwEffect    : effect flag
//                                               In:  Supported effects
//                                               Out: Resulting effect
// Notes   :
STDMETHODIMP wxIDropTarget::DragEnter(IDataObject *pIDataSource,
                                      DWORD        grfKeyState,
                                      POINTL       pt,
                                      DWORD       *pdwEffect)
{
    wxTRY
    {
        wxLogTrace(wxTRACE_OleCalls, wxT("IDropTarget::DragEnter"));

        wxASSERT_MSG( !m_pIDataObject,
                      wxT("drop target can't already have a data object") );

        // show the list of formats supported by the source data object for the
        // debugging purposes, this is quite useful sometimes - please don't remove
#if 0
        IEnumFORMATETC *penumFmt;
        if ( SUCCEEDED(pIDataSource->EnumFormatEtc(DATADIR_GET, &penumFmt)) )
        {
            FORMATETC fmt;
            while ( penumFmt->Next(1, &fmt, NULL) == S_OK )
            {
                wxLogDebug(wxT("Drop source supports format %s"),
                           wxDataObject::GetFormatName(fmt.cfFormat));
            }

            penumFmt->Release();
        }
        else
        {
            wxLogLastError(wxT("IDataObject::EnumFormatEtc"));
        }
#endif // 0

        if ( !m_pTarget->MSWIsAcceptedData(pIDataSource) ) {
          // we don't accept this kind of data
          *pdwEffect = DROPEFFECT_NONE;

          // Don't do anything else if we don't support this format at all, notably
          // don't call our OnEnter() below which would show misleading cursor to
          // the user.
          return S_OK;
        }

        // for use in OnEnter and OnDrag calls
        m_pTarget->MSWSetDataSource(pIDataSource);

        // get hold of the data object
        m_pIDataObject = pIDataSource;

        // we need client coordinates to pass to wxWin functions
        if ( !ScreenToClient(m_hwnd, (POINT *)&pt) )
        {
            wxLogLastError(wxT("ScreenToClient"));
        }

        // give some visual feedback
        *pdwEffect = ConvertDragResultToEffect(
            m_pTarget->OnEnter(pt.x, pt.y, ConvertDragEffectToResult(
                GetDropEffect(grfKeyState, m_pTarget->GetDefaultAction(), *pdwEffect))
                        )
                      );

        // update drag image
        const wxDragResult res = ConvertDragEffectToResult(*pdwEffect);
        m_pTarget->MSWUpdateDragImageOnEnter(pt.x, pt.y, res);
        m_pTarget->MSWUpdateDragImageOnDragOver(pt.x, pt.y, res);

        return S_OK;
    }
    wxCATCH_ALL( return HandleException(); )
}



// Name    : wxIDropTarget::DragOver
// Purpose : Indicates that the mouse was moved inside the window represented
//           by this drop target.
// Returns : S_OK
// Params  : [in]    DWORD   grfKeyState     kbd & mouse state
//           [in]    POINTL  pt              mouse coordinates
//           [in/out]LPDWORD pdwEffect       current effect flag
// Notes   : We're called on every WM_MOUSEMOVE, so this function should be
//           very efficient.
STDMETHODIMP wxIDropTarget::DragOver(DWORD   grfKeyState,
                                     POINTL  pt,
                                     LPDWORD pdwEffect)
{
    wxTRY
    {
        // there are too many of them... wxLogDebug("IDropTarget::DragOver");

        wxDragResult result;
        if ( m_pIDataObject ) {
            result = ConvertDragEffectToResult(
                GetDropEffect(grfKeyState, m_pTarget->GetDefaultAction(), *pdwEffect));
        }
        else {
            // can't accept data anyhow normally
            result = wxDragNone;
        }

        if ( result != wxDragNone ) {
            // we need client coordinates to pass to wxWin functions
            if ( !ScreenToClient(m_hwnd, (POINT *)&pt) )
            {
                wxLogLastError(wxT("ScreenToClient"));
            }

            *pdwEffect = ConvertDragResultToEffect(
                            m_pTarget->OnDragOver(pt.x, pt.y, result)
                         );
        }
        else {
            *pdwEffect = DROPEFFECT_NONE;
        }

        // update drag image
        m_pTarget->MSWUpdateDragImageOnDragOver(pt.x, pt.y,
                                                ConvertDragEffectToResult(*pdwEffect));

        return S_OK;
    }
    wxCATCH_ALL( return HandleException(); )
}

// Name    : wxIDropTarget::DragLeave
// Purpose : Informs the drop target that the operation has left its window.
// Returns : S_OK
// Notes   : good place to do any clean-up
STDMETHODIMP wxIDropTarget::DragLeave()
{
    wxTRY
    {
        wxLogTrace(wxTRACE_OleCalls, wxT("IDropTarget::DragLeave"));

        // remove the UI feedback
        m_pTarget->OnLeave();

        // release the held object
        m_pIDataObject.reset();

        // update drag image
        m_pTarget->MSWUpdateDragImageOnLeave();

        return S_OK;
    }
    wxCATCH_ALL( return HandleException(); )
}

// Name    : wxIDropTarget::Drop
// Purpose : Instructs the drop target to paste data that was just now
//           dropped on it.
// Returns : S_OK
// Params  : [in]    IDataObject *pIDataSource     the data to paste
//           [in]    DWORD        grfKeyState      kbd & mouse state
//           [in]    POINTL       pt               where the drop occurred?
//           [in/out]DWORD       *pdwEffect        operation effect
// Notes   :
STDMETHODIMP wxIDropTarget::Drop(IDataObject *pIDataSource,
                                 DWORD        grfKeyState,
                                 POINTL       pt,
                                 DWORD       *pdwEffect)
{
    wxTRY
    {
        wxLogTrace(wxTRACE_OleCalls, wxT("IDropTarget::Drop"));

        // TODO I don't know why there is this parameter, but so far I assume
        //      that it's the same we've already got in DragEnter
        wxASSERT( m_pIDataObject == pIDataSource );

        // we need client coordinates to pass to wxWin functions
        if ( !ScreenToClient(m_hwnd, (POINT *)&pt) )
        {
            wxLogLastError(wxT("ScreenToClient"));
        }

        // first ask the drop target if it wants data
        if ( m_pTarget->OnDrop(pt.x, pt.y) ) {
            // it does, so give it the data source
            m_pTarget->MSWSetDataSource(pIDataSource);

            // and now it has the data
            wxDragResult rc = ConvertDragEffectToResult(
                GetDropEffect(grfKeyState, m_pTarget->GetDefaultAction(), *pdwEffect));
            rc = m_pTarget->OnData(pt.x, pt.y, rc);
            if ( wxIsDragResultOk(rc) ) {
                // operation succeeded
                *pdwEffect = ConvertDragResultToEffect(rc);
            }
            else {
                *pdwEffect = DROPEFFECT_NONE;
            }
        }
        else {
            // OnDrop() returned false, no need to copy data
            *pdwEffect = DROPEFFECT_NONE;
        }

        // release the held object
        m_pIDataObject.reset();

        // update drag image
        m_pTarget->MSWUpdateDragImageOnData(pt.x, pt.y,
                                            ConvertDragEffectToResult(*pdwEffect));

        return S_OK;
    }
    wxCATCH_ALL( return HandleException(); )
}

// ============================================================================
// wxDropTarget implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget(wxDataObject *dataObj)
            : wxDropTargetBase(dataObj),
              m_dropTargetHelper(NULL)
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
    HRESULT hr;

    hr = ::CoLockObjectExternal(m_pIDropTarget, TRUE, FALSE);
    if ( FAILED(hr) ) {
        wxLogApiError(wxT("CoLockObjectExternal"), hr);
        return false;
    }

    hr = ::RegisterDragDrop((HWND) hwnd, m_pIDropTarget);
    if ( FAILED(hr) ) {
        ::CoLockObjectExternal(m_pIDropTarget, FALSE, FALSE);
        wxLogApiError(wxT("RegisterDragDrop"), hr);
        return false;
    }

    // we will need the window handle for coords transformation later
    m_pIDropTarget->SetHwnd((HWND)hwnd);

    MSWInitDragImageSupport();

    return true;
}

void wxDropTarget::Revoke(WXHWND hwnd)
{
    HRESULT hr = ::RevokeDragDrop((HWND) hwnd);

    if ( FAILED(hr) ) {
        wxLogApiError(wxT("RevokeDragDrop"), hr);
    }

    ::CoLockObjectExternal(m_pIDropTarget, FALSE, TRUE);

    MSWEndDragImageSupport();

    // remove window reference
    m_pIDropTarget->SetHwnd(0);
}

// ----------------------------------------------------------------------------
// base class pure virtuals
// ----------------------------------------------------------------------------

// OnDrop() is called only if we previously returned true from
// IsAcceptedData(), so no need to check anything here
bool wxDropTarget::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    return true;
}

// copy the data from the data source to the target data object
bool wxDropTarget::GetData()
{
    wxDataFormat format = MSWGetSupportedFormat(m_pIDataSource);
    if ( format == wxDF_INVALID ) {
        return false;
    }

    STGMEDIUM stm;
    FORMATETC fmtMemory;
    fmtMemory.cfFormat  = format;
    fmtMemory.ptd       = NULL;
    fmtMemory.dwAspect  = DVASPECT_CONTENT;
    fmtMemory.lindex    = -1;
    fmtMemory.tymed     = TYMED_HGLOBAL;  // TODO to add other media

    bool rc = false;

    HRESULT hr = m_pIDataSource->GetData(&fmtMemory, &stm);
    if ( SUCCEEDED(hr) ) {
        IDataObject *dataObject = m_dataObject->GetInterface();

        hr = dataObject->SetData(&fmtMemory, &stm, TRUE);
        if ( SUCCEEDED(hr) ) {
            rc = true;
        }
        else {
            wxLogApiError(wxT("IDataObject::SetData()"), hr);
        }
    }
    else {
        wxLogApiError(wxT("IDataObject::GetData()"), hr);
    }

    return rc;
}

// ----------------------------------------------------------------------------
// callbacks used by wxIDropTarget
// ----------------------------------------------------------------------------

// we need a data source, so wxIDropTarget gives it to us using this function
void wxDropTarget::MSWSetDataSource(IDataObject *pIDataSource)
{
    m_pIDataSource = pIDataSource;
}

// determine if we accept data of this type
bool wxDropTarget::MSWIsAcceptedData(IDataObject *pIDataSource) const
{
    return MSWGetSupportedFormat(pIDataSource) != wxDF_INVALID;
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

wxDataFormat wxDropTarget::GetMatchingPair()
{
    return MSWGetSupportedFormat( m_pIDataSource );
}

wxDataFormat wxDropTarget::MSWGetSupportedFormat(IDataObject *pIDataSource) const
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
    wxDataFormat format;
    wxDataFormat *formats;
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
// drag image functions
// ----------------------------------------------------------------------------

void
wxDropTarget::MSWEndDragImageSupport()
{
    // release drop target helper
    if ( m_dropTargetHelper != NULL )
    {
        m_dropTargetHelper->Release();
        m_dropTargetHelper = NULL;
    }
}

void
wxDropTarget::MSWInitDragImageSupport()
{
    // Use the default drop target helper to show shell drag images
    CoCreateInstance(wxCLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER,
                     wxIID_IDropTargetHelper, (LPVOID*)&m_dropTargetHelper);
}

void
wxDropTarget::MSWUpdateDragImageOnData(wxCoord x,
                                       wxCoord y,
                                       wxDragResult dragResult)
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != NULL )
    {
        POINT pt = {x, y};
        DWORD dwEffect = ConvertDragResultToEffect(dragResult);
        m_dropTargetHelper->Drop(m_pIDataSource, &pt, dwEffect);
    }
}

void
wxDropTarget::MSWUpdateDragImageOnDragOver(wxCoord x,
                                           wxCoord y,
                                           wxDragResult dragResult)
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != NULL )
    {
        POINT pt = {x, y};
        DWORD dwEffect = ConvertDragResultToEffect(dragResult);
        m_dropTargetHelper->DragOver(&pt, dwEffect);
    }
}

void
wxDropTarget::MSWUpdateDragImageOnEnter(wxCoord x,
                                        wxCoord y,
                                        wxDragResult dragResult)
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != NULL )
    {
        POINT pt = {x, y};
        DWORD dwEffect = ConvertDragResultToEffect(dragResult);
        m_dropTargetHelper->DragEnter(m_pIDropTarget->GetHWND(), m_pIDataSource, &pt, dwEffect);
    }
}

void
wxDropTarget::MSWUpdateDragImageOnLeave()
{
    // call corresponding event on drop target helper
    if ( m_dropTargetHelper != NULL )
    {
        m_dropTargetHelper->DragLeave();
    }
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static wxDragResult ConvertDragEffectToResult(DWORD dwEffect)
{
    switch ( dwEffect ) {
        case DROPEFFECT_COPY:
            return wxDragCopy;

        case DROPEFFECT_LINK:
            return wxDragLink;

        case DROPEFFECT_MOVE:
            return wxDragMove;

        default:
            wxFAIL_MSG(wxT("invalid value in ConvertDragEffectToResult"));
            wxFALLTHROUGH;

        case DROPEFFECT_NONE:
            return wxDragNone;
    }
}

static DWORD ConvertDragResultToEffect(wxDragResult result)
{
    switch ( result ) {
        case wxDragCopy:
            return DROPEFFECT_COPY;

        case wxDragLink:
            return DROPEFFECT_LINK;

        case wxDragMove:
            return DROPEFFECT_MOVE;

        default:
            wxFAIL_MSG(wxT("invalid value in ConvertDragResultToEffect"));
            wxFALLTHROUGH;

        case wxDragNone:
            return DROPEFFECT_NONE;
    }
}

#endif // wxUSE_OLE && wxUSE_DRAG_AND_DROP
