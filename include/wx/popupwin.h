///////////////////////////////////////////////////////////////////////////////
// Name:        wx/popupwin.h
// Purpose:     wxPopupWindow interface declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_POPUPWIN_H_BASE_
#define _WX_POPUPWIN_H_BASE_

#include "wx/window.h"

class WXDLLEXPORT wxComboControl;

#if wxUSE_POPUPWIN

// ----------------------------------------------------------------------------
// wxPopupWindow: a special kind of top level window used for popup menus,
// combobox popups and such.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPopupWindowBase : public wxWindow
{
public:
    wxPopupWindowBase() { m_winParent = NULL; }

    // create a popup: note that the parent window in the underlying low-level
    // toolkit should be NULL regardless of the meaning of this parameter as
    // this class works with screen coordinates - the parent given here is
    // just used by Popup() for position calculations
    bool Create(wxWindow *parent);

    // move the popup window to the right position: either below or above the
    // parent window depending on its location on the screen (the idea is that
    // the popup must always be entirely visible)
    virtual void Position();

protected:
    wxWindow *m_winParent;
};

// include the real class declaration
#ifdef __WXMSW__
    #include "wx/msw/popupwin.h"
#else
    #error "wxPopupWindow is not supported under this platform."
#endif

#endif // wxUSE_POPUPWIN

// ----------------------------------------------------------------------------
// wxPopupComboWindow: a wxPopupWindow which disappears automatically
// when the user clicks mouse outside it or if it loses focus in any other way
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPopupComboWindow : public wxPopupWindow
{
public:
    wxPopupComboWindow(wxComboControl *parent);

    bool Create(wxComboControl *parent);

    // popup the window (this will show it too)
    virtual void Popup();

    // hide the window
    virtual void Dismiss();

protected:
    // event handlers
    void OnLeftDown(wxMouseEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    // dismiss and notify the combo
    void DismissAndNotify();

    // the child of this popup if any
    wxWindow *m_child;

    DECLARE_EVENT_TABLE()
};

#endif // _WX_POPUPWIN_H_BASE_

