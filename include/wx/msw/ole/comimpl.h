///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/ole/comimpl.h
// Purpose:     COM helper routines, COM debugging support &c
// Author:      Vadim Zeitlin
// Modified by:
// Created:     19.02.1998
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_COMIMPL_H
#define WX_COMIMPL_H

#include "wx/defs.h"

#include "wx/msw/wrapwin.h"
// get IUnknown, REFIID &c
#include <objbase.h>

// ============================================================================
// General purpose functions and macros
// ============================================================================

// release the interface pointer (if non-null)
inline void ReleaseInterface(IUnknown *pIUnk)
{
  if ( pIUnk != nullptr )
    pIUnk->Release();
}

// release the interface pointer (if non-null) and set it to nullptr
#define   RELEASE_AND_NULL(p)   if ( (p) != nullptr ) { p->Release(); p = nullptr; };

// return true if the iid is in the array
extern WXDLLIMPEXP_CORE bool IsIidFromList(REFIID riid, const IID *aIids[], size_t nCount);

// ============================================================================
// IUnknown implementation helpers
// ============================================================================

/*
   WARNING: This does NOT work with multiple inheritance, so multiple
   interfaces can only be supported when they inherit from each other.

   The most dumb implementation of IUnknown methods. We don't support
   aggregation nor containment, but for 99% of cases this simple
   implementation is quite enough.

   Usage is trivial: here is all you should have
   1) DECLARE_IUNKNOWN_METHODS in your (IUnknown derived!) class declaration
   2) BEGIN/END_IID_TABLE with ADD_IID in between for all interfaces you
      support (at least all for which you intent to return 'this' from QI,
      i.e. you should derive from IFoo if you have ADD_IID(Foo)) somewhere else
   3) IMPLEMENT_IUNKNOWN_METHODS somewhere also

   These macros are quite simple: AddRef and Release are trivial and QI does
   lookup in a static member array of IIDs and returns 'this' if it founds
   the requested interface in it or E_NOINTERFACE if not.
 */

/*
  wxAutoULong: this class is used for automatically initialising m_cRef to 0
*/
class wxAutoULong
{
public:
    wxAutoULong(ULONG value = 0) : m_Value(value) { }

    operator ULONG&() { return m_Value; }
    ULONG& operator=(ULONG value) { m_Value = value; return m_Value;  }

    wxAutoULong& operator++() { ++m_Value; return *this; }
    const wxAutoULong operator++( int ) { wxAutoULong temp = *this; ++m_Value; return temp; }

    wxAutoULong& operator--() { --m_Value; return *this; }
    const wxAutoULong operator--( int ) { wxAutoULong temp = *this; --m_Value; return temp; }

private:
    ULONG m_Value;
};

// declare the methods and the member variable containing reference count
// you must also define the ms_aIids array somewhere with BEGIN_IID_TABLE
// and friends (see below)

#define   DECLARE_IUNKNOWN_METHODS                                            \
  public:                                                                     \
    STDMETHODIMP          QueryInterface(REFIID, void **) override;         \
    STDMETHODIMP_(ULONG)  AddRef() override;                                \
    STDMETHODIMP_(ULONG)  Release() override;                               \
  private:                                                                    \
    static  const IID    *ms_aIids[];                                         \
    wxAutoULong           m_cRef

// macros for declaring supported interfaces
// NB: ADD_IID prepends IID_I whereas ADD_RAW_IID does not
#define BEGIN_IID_TABLE(cname)  const IID *cname::ms_aIids[] = {
#define ADD_IID(iid)                                             &IID_I##iid,
#define ADD_RAW_IID(iid)                                         &iid,
#define END_IID_TABLE                                          }

// implementation is as straightforward as possible
// Parameter:  classname - the name of the class
#define   IMPLEMENT_IUNKNOWN_METHODS(classname)                               \
  STDMETHODIMP classname::QueryInterface(REFIID riid, void **ppv)             \
  {                                                                           \
    wxLogQueryInterface(wxT(#classname), riid);                               \
                                                                              \
    if ( IsIidFromList(riid, ms_aIids, WXSIZEOF(ms_aIids)) ) {                \
      *ppv = this;                                                            \
      AddRef();                                                               \
                                                                              \
      return S_OK;                                                            \
    }                                                                         \
    else {                                                                    \
      *ppv = nullptr;                                                            \
                                                                              \
      return (HRESULT) E_NOINTERFACE;                                         \
    }                                                                         \
  }                                                                           \
                                                                              \
  STDMETHODIMP_(ULONG) classname::AddRef()                                    \
  {                                                                           \
    wxLogAddRef(wxT(#classname), m_cRef);                                     \
                                                                              \
    return ++m_cRef;                                                          \
  }                                                                           \
                                                                              \
  STDMETHODIMP_(ULONG) classname::Release()                                   \
  {                                                                           \
    wxLogRelease(wxT(#classname), m_cRef);                                    \
                                                                              \
    if ( --m_cRef == wxAutoULong(0) ) {                                       \
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
//     as is. Please note, though, that tracing COM interface calls may be
//     incredibly useful when debugging COM programs.
#if defined(__WXDEBUG__) && defined(__VISUALC__)
// ----------------------------------------------------------------------------
// All COM specific log functions have DebugTrace level (as LogTrace)
// ----------------------------------------------------------------------------

// tries to translate riid into a symbolic name, if possible
WXDLLIMPEXP_CORE void wxLogQueryInterface(const wxChar *szInterface, REFIID riid);

// these functions print out the new value of reference counter
WXDLLIMPEXP_CORE void wxLogAddRef (const wxChar *szInterface, ULONG cRef);
WXDLLIMPEXP_CORE void wxLogRelease(const wxChar *szInterface, ULONG cRef);

#else   //!__WXDEBUG__
  #define   wxLogQueryInterface(szInterface, riid)
  #define   wxLogAddRef(szInterface, cRef)
  #define   wxLogRelease(szInterface, cRef)
#endif  //__WXDEBUG__

#endif // WX_COMIMPL_H
