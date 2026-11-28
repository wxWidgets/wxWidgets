///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/windowlifetime.h
// Purpose:     Internal callback-boundary window lifetime predicates
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WINDOWLIFETIME_H_
#define _WX_PRIVATE_WINDOWLIFETIME_H_

#include "wx/app.h"
#include "wx/weakref.h"
#include "wx/window.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/tlwhostmsw.h"
#endif

// Test only the object's own deletion flag. IsBeingDeleted() cannot be used
// here because for non-TLWs it deliberately propagates the state of every
// ancestor. Destruction observers sometimes need to inspect or close a still
// live child while its owner is already dispatching wxEVT_DESTROY. Ordinary
// callback transactions should use wxWindowIsUnavailableForCallbacks() below.
inline bool wxWindowItselfIsUnavailableForCallbacks(
    const wxWindowBase* window)
{
    if ( !window )
        return true;

    wxWindowBase* const mutableWindow = const_cast<wxWindowBase*>(window);
    if ( mutableWindow->m_isBeingDeleted ||
         (wxTheApp &&
          wxTheApp->IsScheduledForDestruction(mutableWindow)) )
    {
        return true;
    }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    // The ordinary host predicate also consults IsBeingDeleted(), whose
    // result is ancestor-propagating for child windows. Use the exact private
    // queue predicate here so this function keeps its own-object contract.
    if ( wxWinUITLWHostHasDeferredDestroyExact(mutableWindow->AsWindow()) )
        return true;
#endif

    return false;
}

// A live C++ object is not necessarily available to a callback transaction:
// Destroy() can retain it in wxPendingDelete, and wxWinUI can retain a TLW in
// its private callback queue before it reaches wxPendingDelete. A child is also
// unavailable when any ancestor is in one of these states: WinUI schedules the
// TLW first while all of its controls remain weak-live. Callers must stop at
// every such state, even while weak references and HWNDs still exist.
inline bool wxWindowIsUnavailableForCallbacks(const wxWindowBase* window)
{
    if ( !window )
        return true;

    for ( wxWindowBase* current = const_cast<wxWindowBase*>(window);
          current;
          current = current->GetParent() )
    {
        if ( wxWindowItselfIsUnavailableForCallbacks(current) )
            return true;
    }

    return false;
}

template <typename T, typename U>
inline bool wxWeakWindowIsAvailableForCallbacks(
    const wxWeakRef<T>& weakWindow,
    const U* expected)
{
    T* const window = weakWindow.get();
    return window == expected &&
           !wxWindowIsUnavailableForCallbacks(window);
}

#endif // _WX_PRIVATE_WINDOWLIFETIME_H_
