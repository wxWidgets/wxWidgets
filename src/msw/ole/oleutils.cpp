///////////////////////////////////////////////////////////////////////////////
// Name:        ole/oleutils.cpp
// Purpose:     implementation of OLE helper functions
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     19.02.98
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
#pragma implementation "oleutils.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include  <wx/setup.h>
#include  <wx/log.h>

#include <windows.h>

// OLE
#include  <wx/msw/ole/uuid.h>
#include  <wx/msw/ole/oleutils.h>

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#include  <docobj.h>
#endif

// ============================================================================
// Implementation
// ============================================================================

// return TRUE if the iid is in the array
bool IsIidFromList(REFIID riid, const IID *aIids[], size_t nCount)
{
  for ( size_t i = 0; i < nCount; i++ ) {
    if ( riid == *aIids[i] )
      return TRUE;
  }

  return FALSE;
}

#if wxUSE_DRAG_AND_DROP

// ----------------------------------------------------------------------------
// Debug support
// ----------------------------------------------------------------------------

#if defined(__WXDEBUG__) && defined(_MSC_VER) && (_MSC_VER > 1000)
const char *GetIidName(REFIID riid)
{
  // an association between symbolic name and numeric value of an IID
  struct KNOWN_IID {
    const IID  *pIid;
    const char *szName;
  };

  // construct the table containing all known interfaces
  #define ADD_KNOWN_IID(name) { &IID_I##name, #name }

  static const KNOWN_IID aKnownIids[] = {
    ADD_KNOWN_IID(AdviseSink),
    ADD_KNOWN_IID(AdviseSink2),
    ADD_KNOWN_IID(BindCtx),
    ADD_KNOWN_IID(ClassFactory),
    ADD_KNOWN_IID(ContinueCallback),
    ADD_KNOWN_IID(EnumOleDocumentViews),
    ADD_KNOWN_IID(OleCommandTarget),
    ADD_KNOWN_IID(OleDocument),
    ADD_KNOWN_IID(OleDocumentSite),
    ADD_KNOWN_IID(OleDocumentView),
    ADD_KNOWN_IID(Print),
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
  for ( size_t ui = 0; ui < WXSIZEOF(aKnownIids); ui++ ) {
    if ( riid == *aKnownIids[ui].pIid ) {
      return aKnownIids[ui].szName;
    }
  }

  // unknown IID, just transform to string
  static Uuid s_uuid;
  s_uuid.Set(riid);
  return s_uuid;
}

void wxLogQueryInterface(const char *szInterface, REFIID riid)
{
  wxLogTrace("%s::QueryInterface (iid = %s)", szInterface, GetIidName(riid));
}

void wxLogAddRef(const char *szInterface, ULONG cRef)
{
  wxLogTrace("After %s::AddRef: m_cRef = %d", szInterface, cRef + 1);
}

void wxLogRelease(const char *szInterface, ULONG cRef)
{
  wxLogTrace("After %s::Release: m_cRef = %d", szInterface, cRef - 1);
}

#endif  //WXDEBUG

#endif
  // wxUSE_DRAG_AND_DROP
