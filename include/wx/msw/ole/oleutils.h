///////////////////////////////////////////////////////////////////////////////
// Name:        oleutils.h
// Purpose:     OLE helper routines, OLE debugging support &c
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.02.1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_OLEUTILS_H
#define   _WX_OLEUTILS_H

#ifdef __GNUG__
#pragma interface "oleutils.h"
#endif

#include "wx/defs.h"
#ifdef wxUSE_NORLANDER_HEADERS
#include <ole2.h>
#endif
// ============================================================================
// General purpose functions and macros
// ============================================================================

// ----------------------------------------------------------------------------
// misc helper functions/macros
// ----------------------------------------------------------------------------

// release the interface pointer (if !NULL)
inline void ReleaseInterface(IUnknown *pIUnk)
{
  if ( pIUnk != NULL )
    pIUnk->Release();
}

// release the interface pointer (if !NULL) and make it NULL
#define   RELEASE_AND_NULL(p)   if ( (p) != NULL ) { p->Release(); p = NULL; };

// return TRUE if the iid is in the array
bool IsIidFromList(REFIID riid, const IID *aIids[], size_t nCount);

// ============================================================================
// IUnknown implementation helpers
// ============================================================================

/*
   The most dumb implementation of IUnknown methods. We don't support
   aggregation nor containment, but for 99% of cases this simple
   implementation is quite enough.

   Usage is trivial: here is all you should have
   1) DECLARE_IUNKNOWN_METHOS in your (IUnknown derived!) class declaration
   2) BEGIN/END_IID_TABLE with ADD_IID in between for all interfaces you
      support (at least all for which you intent to return 'this' from QI,
      i.e. you should derive from IFoo if you have ADD_IID(Foo)) somewhere else
   3) IMPLEMENT_IUNKNOWN_METHOS somewhere also

   These macros are quite simple: AddRef and Release are trivial and QI does
   lookup in a static member array of IIDs and returns 'this' if it founds
   the requested interface in it or E_NOINTERFACE if not.
 */

// declare the methods and the member variable containing reference count
// you must also define the ms_aIids array somewhere with BEGIN_IID_TABLE
// and friends (see below)
#define   DECLARE_IUNKNOWN_METHODS                                            \
  public:                                                                     \
    STDMETHODIMP          QueryInterface(REFIID, void **);                    \
    STDMETHODIMP_(ULONG)  AddRef();                                           \
    STDMETHODIMP_(ULONG)  Release();                                          \
  private:                                                                    \
    static  const IID    *ms_aIids[];                                         \
    ULONG                 m_cRef

// macros for declaring supported interfaces
// NB: you should write ADD_INTERFACE(Foo) and not ADD_INTERFACE(IID_IFoo)!
#define BEGIN_IID_TABLE(cname)  const IID *cname::ms_aIids[] = {
#define ADD_IID(iid)                                             &IID_I##iid,
#define END_IID_TABLE                                          }

// implementation is as straightforward as possible
// Parameter:  classname - the name of the class
#define   IMPLEMENT_IUNKNOWN_METHODS(classname)                               \
  STDMETHODIMP classname::QueryInterface(REFIID riid, void **ppv)             \
  {                                                                           \
    wxLogQueryInterface(_T(#classname), riid);                                \
                                                                              \
    if ( IsIidFromList(riid, ms_aIids, WXSIZEOF(ms_aIids)) ) {                \
      *ppv = this;                                                            \
      AddRef();                                                               \
                                                                              \
      return S_OK;                                                            \
    }                                                                         \
    else {                                                                    \
      *ppv = NULL;                                                            \
                                                                              \
      return (HRESULT) E_NOINTERFACE;                                         \
    }                                                                         \
  }                                                                           \
                                                                              \
  STDMETHODIMP_(ULONG) classname::AddRef()                                    \
  {                                                                           \
    wxLogAddRef(_T(#classname), m_cRef);                                      \
                                                                              \
    return ++m_cRef;                                                          \
  }                                                                           \
                                                                              \
  STDMETHODIMP_(ULONG) classname::Release()                                   \
  {                                                                           \
    wxLogRelease(_T(#classname), m_cRef);                                     \
                                                                              \
    if ( --m_cRef == 0 ) {                                                    \
      delete this;                                                            \
      return 0;                                                               \
    }                                                                         \
    else                                                                      \
      return m_cRef;                                                          \
  }

// ============================================================================
// Debugging support
// ============================================================================

// VZ: I don't know it's not done for compilers other than VC++ but I leave it
//     as is. Please note, though, that tracing OLE interface calls may be
//     incredibly useful when debugging OLE programs.
#if defined(__WXDEBUG__) && defined(__VISUALC__) && (__VISUALC__ >= 1000)
// ----------------------------------------------------------------------------
// All OLE specific log functions have DebugTrace level (as LogTrace)
// ----------------------------------------------------------------------------

// tries to translate riid into a symbolic name, if possible
void wxLogQueryInterface(const wxChar *szInterface, REFIID riid);

// these functions print out the new value of reference counter
void wxLogAddRef (const wxChar *szInterface, ULONG cRef);
void wxLogRelease(const wxChar *szInterface, ULONG cRef);

#else   //!WXDEBUG
  #define   wxLogQueryInterface(szInterface, riid)
  #define   wxLogAddRef(szInterface, cRef)
  #define   wxLogRelease(szInterface, cRef)
#endif  //WXDEBUG

#endif  //_WX_OLEUTILS_H

