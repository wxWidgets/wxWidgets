///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/dropsrc.cpp
// Purpose:     implementation of wxIDropSource and wxDropSource
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     10.05.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "dropsrc.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
  #pragma hdrstop
#endif

#include  <wx/setup.h>

#if wxUSE_DRAG_AND_DROP

#include  <wx/log.h>
#include  <wx/msw/ole/dataobj.h>
#include  <wx/msw/ole/dropsrc.h>

#include <windows.h>

#ifndef __WIN32__
  #include <ole2.h>
  #include <olestd.h>
#endif

#include <oleauto.h>

#include <wx/msw/ole/oleutils.h>

// ----------------------------------------------------------------------------
// wxIDropSource implementation of IDropSource interface
// ----------------------------------------------------------------------------

class wxIDropSource : public IDropSource
{
public:
  wxIDropSource(wxDropSource *pDropSource);

  DECLARE_IUNKNOWN_METHODS;

  // IDropSource
  STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
  STDMETHODIMP GiveFeedback(DWORD dwEffect);

private:
  DWORD         m_grfInitKeyState;  // button which started the d&d operation
  wxDropSource *m_pDropSource;      // pointer to C++ class we belong to
};

// ============================================================================
// Implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIDropSource implementation
// ----------------------------------------------------------------------------
BEGIN_IID_TABLE(wxIDropSource)
  ADD_IID(Unknown)
  ADD_IID(DropSource)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxIDropSource)

wxIDropSource::wxIDropSource(wxDropSource *pDropSource)
{
  wxASSERT( pDropSource != NULL );

  m_pDropSource = pDropSource;
  m_grfInitKeyState = 0;
  m_cRef = 0;
}

// Name    : wxIDropSource::QueryContinueDrag
// Purpose : decide if drag operation must be continued or not
// Returns : HRESULT: S_OK              if we should continue
//                    DRAGDROP_S_DROP   to drop right now
//                    DRAGDROP_S_CANCEL to cancel everything
// Params  : [in] BOOL  fEscapePressed  <Esc> pressed since last call?
//           [in] DWORD grfKeyState     mask containing state of kbd keys
// Notes   : as there is no reasonably simple portable way to implement this
//           function, we currently don't give the possibility to override the
//           default behaviour implemented here
STDMETHODIMP wxIDropSource::QueryContinueDrag(BOOL fEscapePressed,
                                              DWORD grfKeyState)
{
  if ( fEscapePressed )
    return DRAGDROP_S_CANCEL;

  // initialize ourself with the drag begin button
  if ( m_grfInitKeyState == 0 ) {
    m_grfInitKeyState = grfKeyState & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON);
  }

  if ( !(grfKeyState & m_grfInitKeyState) ) {
    // button which started d&d released, go!
    return DRAGDROP_S_DROP;
  }

  return S_OK;
}

// Name    : wxIDropSource::GiveFeedback
// Purpose : give UI feedback according to current state of operation
// Returns : STDMETHODIMP 
// Params  : [in] DWORD dwEffect - what would happen if we dropped now
// Notes   : default implementation is ok in more than 99% of cases
STDMETHODIMP wxIDropSource::GiveFeedback(DWORD dwEffect)
{
  wxDragResult effect;
  if ( dwEffect & DROPEFFECT_COPY )
    effect = wxDragCopy;
  else if ( dwEffect & DROPEFFECT_MOVE )
    effect = wxDragMove;
  else
    effect = wxDragNone;

  if ( m_pDropSource->GiveFeedback(effect,
                                   (dwEffect & DROPEFFECT_SCROLL) != 0 ) )
    return S_OK;

  return DRAGDROP_S_USEDEFAULTCURSORS;
}

// ----------------------------------------------------------------------------
// wxDropSource implementation
// ----------------------------------------------------------------------------

// ctors

// common part of all ctors
void wxDropSource::Init()
{
  m_pIDropSource = new wxIDropSource(this);
  m_pIDropSource->AddRef();
}

wxDropSource::wxDropSource(wxWindow* WXUNUSED(win))
{
  Init();
  m_pData = NULL;
}

wxDropSource::wxDropSource(wxDataObject& data, wxWindow* WXUNUSED(win))
{
  Init();
  SetData(data);
}

void wxDropSource::SetData(wxDataObject& data)
{
  m_pData = &data;
}

wxDropSource::~wxDropSource()
{
  m_pIDropSource->Release();
}

// Name    : DoDragDrop
// Purpose : start drag and drop operation
// Returns : wxDragResult - the code of performed operation
// Params  : [in] bool bAllowMove: if false, only copy is allowed
// Notes   : you must call SetData() before if you had used def ctor
wxDragResult wxDropSource::DoDragDrop(bool bAllowMove)
{
  wxCHECK_MSG( m_pData != NULL, wxDragNone, "No data in wxDropSource!" );

  DWORD dwEffect;
  HRESULT hr = ::DoDragDrop(m_pData->GetInterface(), 
                            m_pIDropSource, 
                            bAllowMove ? DROPEFFECT_COPY | DROPEFFECT_MOVE
                                       : DROPEFFECT_COPY,
                            &dwEffect);

  if ( hr == DRAGDROP_S_CANCEL ) {
    return wxDragCancel;
  }
  else if ( hr == DRAGDROP_S_DROP ) {
    if ( dwEffect & DROPEFFECT_COPY ) {
      return wxDragCopy;
    }
    else if ( dwEffect & DROPEFFECT_MOVE ) {
      // consistency check: normally, we shouldn't get "move" at all
      // here if !bAllowMove, but in practice it does happen quite often
      if ( bAllowMove )
        return wxDragMove;
      else
        return wxDragCopy;
    }
    else {
      // not copy or move
      return wxDragNone;
    }
  }
  else {
    if ( FAILED(hr) ) {
      wxLogApiError("DoDragDrop", hr);
      wxLogError("Drag & drop operation failed.");
    }
    else {
      wxLogDebug("Unexpected success return code %08lx from DoDragDrop.", hr);
    }

    return wxDragError;
  }
}

// Name    : wxDropSource::GiveFeedback
// Purpose : visually inform the user about d&d operation state
// Returns : bool: true if we do all ourselves or false for default feedback
// Params  : [in] DragResult effect - what would happen if we dropped now
//           [in] bool bScrolling   - true if target is scrolling    
// Notes   : here we just leave this stuff for default implementation
bool wxDropSource::GiveFeedback(wxDragResult effect, bool bScrolling)
{
  return FALSE;
}

#endif  //USE_DRAG_AND_DROP
