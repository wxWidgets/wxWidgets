///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/tlwdrag.h
// Purpose:     wxTLWDragSession: drag a TLW using the system drag machinery
// Author:      Vadim Zeitlin
// Created:     2026-08-25
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_TLWDRAG_H_
#define _WX_PRIVATE_TLWDRAG_H_

#include "wx/defs.h"
#include "wx/gdicmn.h"

#include <memory>

class WXDLLIMPEXP_FWD_CORE wxWindow;

// Only wxGTK provides an implementation of this class for now and even there
// it may not be available, see wxTLWDragSession::Create().
#ifdef __WXGTK3__
    #define wxHAS_TLW_DRAG_SESSION
#endif

#ifdef wxHAS_TLW_DRAG_SESSION

// ----------------------------------------------------------------------------
// wxTLWDragHandler: receives the notifications during wxTLWDragSession
// ----------------------------------------------------------------------------

class wxTLWDragHandler
{
public:
    virtual ~wxTLWDragHandler() = default;

    // Called whenever the pointer moves during the drag.
    //
    // "win" is the target window passed to wxTLWDragSession::Create() if the
    // pointer is currently over it or null if it is not, and "pt" is the
    // pointer position in screen coordinates. Note that we can only detect
    // being over the target window itself, drags over the other windows of
    // this application, or of the other applications, are indistinguishable
    // from being outside of them.
    //
    // Note that under Wayland the screen coordinates are not really global,
    // but they are still consistent with the coordinates used by the windows
    // inside "win", which is all we need here.
    virtual void OnDragOver(wxWindow* win, const wxPoint& pt) = 0;

    // Called when the drag ends with a drop.
    //
    // If this function is not called, the drag was cancelled and OnCancel()
    // is called instead.
    //
    // The arguments are the same as for OnDragOver().
    //
    // The handler, and the session owning it, may be destroyed from here.
    virtual void OnDragDrop(wxWindow* win, const wxPoint& pt) = 0;

    // Called when the drag was cancelled.
    virtual void OnDragCancel() = 0;
};

// ----------------------------------------------------------------------------
// wxTLWDragSession: represents a drag moving a TLW with the pointer
// ----------------------------------------------------------------------------

// This class encapsulates a drag operation during which a TLW follows the
// mouse pointer until the drag ends.
//
// Currently it's only implemented in wxGTK3 when using Wayland and the
// appropriate build-time support is available, but we still use a base class
// for it to avoid exposing the implementation details in this header and to
// allow for future implementations for other ports.
class WXDLLIMPEXP_CORE wxTLWDragSession
{
public:
    // Return true if drag sessions can be used at all, i.e. if we're running
    // under a system supporting them.
    //
    // IOW, if this returns true, Create() may still fail, but if this returns
    // false, Create() will never succeed.
    static bool IsAvailable();

    // Creating a drag session starts a drag originating from the given window,
    // which must be shown and must be the window currently having the pointer
    // grab, i.e. this can only be called while handling a mouse event for it.
    //
    // The drag position is reported to the handler in terms of "target", which
    // is often, but not necessarily, the same as "origin": e.g. when dragging
    // a floating window over the main one, the drag originates from the former
    // but we're interested in the position over the latter.
    //
    // Returns null if starting the drag is not supported, in which case the
    // caller has to fall back to moving the window on its own.
    static std::unique_ptr<wxTLWDragSession>
    Create(wxWindow* origin,
           wxWindow* target,
           std::unique_ptr<wxTLWDragHandler> handler);

    // Ask the system to move the given TLW with the pointer for the rest of
    // the drag. The window must be already shown and "offset" is the position
    // of the pointer inside it, used as a hint for its initial position.
    //
    // This can only be called once for any given session.
    virtual bool AttachWindow(wxWindow* tlw, const wxPoint& offset) = 0;

    // Virtual dtor for any base class used polymorphically.
    virtual ~wxTLWDragSession() = default;
};

#else // !wxHAS_TLW_DRAG_SESSION

// Define a stub just to allow to refer to it in the headers without checking
// for wxHAS_TLW_DRAG_SESSION first.
class wxTLWDragSession
{
public:
    static bool IsAvailable() { return false; }
};

#endif // wxHAS_TLW_DRAG_SESSION/!wxHAS_TLW_DRAG_SESSION

#endif // _WX_PRIVATE_TLWDRAG_H_
