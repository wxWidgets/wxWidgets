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

#include  <wx/setup.h>

#if wxUSE_DRAG_AND_DROP

#include  <wx/log.h>

#ifdef __WIN32__
#ifndef __GNUWIN32__
#include  <shlobj.h>            // for DROPFILES structure
#endif
#else
#include <shellapi.h>
#endif

#include  <wx/msw/ole/droptgt.h>

#ifndef __WIN32__
#include <ole2.h>
#include <olestd.h>
#endif

#include  <wx/msw/ole/oleutils.h>

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

  // IDropTarget methods
  STDMETHODIMP DragEnter(LPDATAOBJECT, DWORD, POINTL, LPDWORD);
  STDMETHODIMP DragOver(DWORD, POINTL, LPDWORD);
  STDMETHODIMP DragLeave(void);
  STDMETHODIMP Drop(LPDATAOBJECT, DWORD, POINTL, LPDWORD);

  // @@ we assume that if QueryGetData() returns S_OK, than we can really
  //    get data in this format, so we remember here the format for which
  //    QueryGetData() succeeded
  void SetSupportedFormat(wxDataFormat cfFormat) { m_cfFormat = cfFormat; }

  DECLARE_IUNKNOWN_METHODS;

protected:
  IDataObject   *m_pIDataObject;  // !NULL between DragEnter and DragLeave/Drop
  wxDropTarget  *m_pTarget;       // the real target (we're just a proxy)

  wxDataFormat   m_cfFormat;      // the format in which to ask for data

private:
  static inline DWORD GetDropEffect(DWORD flags);
};

// ============================================================================
// wxIDropTarget implementation
// ============================================================================

// Name    : static wxDropTarget::GetDropEffect
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
  m_cfFormat     = (wxDataFormat) 0;
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
  wxLogDebug("IDropTarget::DragEnter");

  wxASSERT( m_pIDataObject == NULL );

  if ( !m_pTarget->IsAcceptedData(pIDataSource) ) {
    // we don't accept this kind of data
    *pdwEffect = DROPEFFECT_NONE;

    return S_OK;
  }

  // @@ should check the point also?
  
  *pdwEffect = GetDropEffect(grfKeyState);

  // get hold of the data object
  m_pIDataObject = pIDataSource;
  m_pIDataObject->AddRef();

  // give some visual feedback
  m_pTarget->OnEnter();

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

  *pdwEffect = m_pIDataObject == NULL ? DROPEFFECT_NONE 
                                      : GetDropEffect(grfKeyState);
  return S_OK;
}

// Name    : wxIDropTarget::DragLeave
// Purpose : Informs the drop target that the operation has left its window.
// Returns : S_OK
// Notes   : good place to do any clean-up
STDMETHODIMP wxIDropTarget::DragLeave()
{
  wxLogDebug("IDropTarget::DragLeave");

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
  wxLogDebug("IDropTarget::Drop");

  // @@ I don't know why there is this parameter, but so far I assume
  //    that it's the same we've already got in DragEnter
  wxASSERT( m_pIDataObject == pIDataSource );

  STGMEDIUM stm;
  *pdwEffect = DROPEFFECT_NONE; 
  
  // should be set by SetSupportedFormat() call
  wxASSERT( m_cfFormat != 0 );

  FORMATETC fmtMemory;
  fmtMemory.cfFormat  = m_cfFormat;
  fmtMemory.ptd       = NULL; 
  fmtMemory.dwAspect  = DVASPECT_CONTENT;
  fmtMemory.lindex    = -1;
  fmtMemory.tymed     = TYMED_HGLOBAL;  // @@@@ to add other media

  HRESULT hr = pIDataSource->GetData(&fmtMemory, &stm);
  if ( SUCCEEDED(hr) ) {
    if ( stm.hGlobal != NULL ) {
      if ( m_pTarget->OnDrop(pt.x, pt.y, GlobalLock(stm.hGlobal)) )
        *pdwEffect = GetDropEffect(grfKeyState);
      //else: DROPEFFECT_NONE

      GlobalUnlock(stm.hGlobal);
      ReleaseStgMedium(&stm);
    }
  }
  else
  {
    // wxLogApiError("GetData", hr);
  }

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

wxDropTarget::wxDropTarget()
{
  // create an IDropTarget implementation which will notify us about 
  // d&d operations.
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

  return TRUE;
}

void wxDropTarget::Revoke(WXHWND hwnd)
{
  HRESULT hr = ::RevokeDragDrop((HWND) hwnd);

  if ( FAILED(hr) ) {
    wxLogApiError("RevokeDragDrop", hr);
  }

  ::CoLockObjectExternal(m_pIDropTarget, FALSE, TRUE);
}

// ----------------------------------------------------------------------------
// determine if we accept data of this type
// ----------------------------------------------------------------------------
bool wxDropTarget::IsAcceptedData(IDataObject *pIDataSource) const
{
  // this strucutre describes a data of any type (first field will be
  // changing) being passed through global memory block.
  static FORMATETC s_fmtMemory = { 
    0,
    NULL, 
    DVASPECT_CONTENT, 
    -1, 
    TYMED_HGLOBAL 
  };

  // cycle thorugh all supported formats
  for ( size_t n = 0; n < GetFormatCount(); n++ ) {
    s_fmtMemory.cfFormat = GetFormat(n);
    // @ don't use SUCCEEDED macro here: QueryGetData returns 1 (whatever it
    //   means) for file drag and drop
    if ( pIDataSource->QueryGetData(&s_fmtMemory) == S_OK ) {
      // remember this format: we'll later ask for data in it
      m_pIDropTarget->SetSupportedFormat((wxDataFormat) s_fmtMemory.cfFormat);
      return TRUE;
    }
  }

  return FALSE;
}

// ============================================================================
// wxTextDropTarget
// ============================================================================

bool wxTextDropTarget::OnDrop(long x, long y, const void *pData)
{
  return OnDropText(x, y, (const char *)pData);
}

size_t wxTextDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxTextDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
  return wxDF_TEXT;
}

// ============================================================================
// wxFileDropTarget
// ============================================================================

bool wxFileDropTarget::OnDrop(long x, long y, const void *pData)
{
  // the documentation states that the first member of DROPFILES structure
  // is a "DWORD offset of double NUL terminated file list". What they mean by 
  // this (I wonder if you see it immediately) is that the list starts at
  // ((char *)&(pDropFiles.pFiles)) + pDropFiles.pFiles. We're also advised to
  // use DragQueryFile to work with this structure, but not told where and how
  // to get HDROP.
  HDROP hdrop = (HDROP)pData;   // @@ it works, but I'm not sure about it

  // get number of files (magic value -1)
  UINT nFiles = ::DragQueryFile(hdrop, (unsigned)-1, NULL, 0u);
  
  // for each file get the length, allocate memory and then get the name
  char **aszFiles = new char *[nFiles];
  UINT len, n;
  for ( n = 0; n < nFiles; n++ ) {
    // +1 for terminating NUL
    len = ::DragQueryFile(hdrop, n, NULL, 0) + 1;

    aszFiles[n] = new char[len];

    UINT len2 = ::DragQueryFile(hdrop, n, aszFiles[n], len);
    if ( len2 != len - 1 ) {
      wxLogDebug("In wxFileDropTarget::OnDrop DragQueryFile returned %d "
                 "characters, %d expected.", len2, len - 1);
    }
  }

  bool bResult = OnDropFiles(x, y, nFiles, (const char**) aszFiles);

  // free memory
  for ( n = 0; n < nFiles; n++ ) {
    delete [] aszFiles[n];
  }
  delete [] aszFiles;

  return bResult;
}

size_t wxFileDropTarget::GetFormatCount() const
{
  return 1;
}

wxDataFormat wxFileDropTarget::GetFormat(size_t WXUNUSED(n)) const
{
#ifdef __WIN32__
  return wxDF_FILENAME;
#else
  // TODO: how to implement this in WIN16?
  return wxDF_TEXT;
#endif
}

#endif
 // wxUSE_DRAG_AND_DROP
