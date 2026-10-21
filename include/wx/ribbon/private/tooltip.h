///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/private/tooltip.h
// Purpose:     Helper managing the rich tooltips of the ribbon controls
// Author:      Blake Madden
// Created:     2026-10-21
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_PRIVATE_TOOLTIP_H_
#define _WX_RIBBON_PRIVATE_TOOLTIP_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/longlong.h"
#include "wx/timer.h"
#include "wx/ribbon/tooltip.h"

class wxRibbonControl;
class wxRibbonToolTipPopup;

// Shows a rich tooltip for the items of a ribbon control.
// The tooltip appears below the ribbon, aligned with the item,
// and stays open while the mouse is over the item or tooltip itself,
// (so that the help link in it can be clicked).
//
// The owning control creates one helper, forwards the mouse events to it, and
// deletes it from its own destructor.
class wxRibbonToolTipHelper : public wxEvtHandler
{
public:
    explicit wxRibbonToolTipHelper(wxRibbonControl* owner);
    virtual ~wxRibbonToolTipHelper();

    // Must be called whenever the mouse moves over an item. The rect is in
    // the owner's client coordinates.
    void ItemHovered(int id,
                     const wxRect& rect,
                     const wxRibbonToolTipInfo& info,
                     const wxString& helpString);

    // Must be called when the mouse is over no item, or leaves the owner.
    void ItemLeft();

    // Must be called when an item gets the keyboard focus. Its tooltip is
    // shown until the focus moves elsewhere, regardless of the mouse.
    void ItemFocused(int id,
                     const wxRect& rect,
                     const wxRibbonToolTipInfo& info,
                     const wxString& helpString);

    // Must be called when the owner no longer has a focused item.
    void FocusCleared();

    // Hides the tooltip immediately. If suppress is true, the tooltip won't
    // be shown again for the current item until the mouse leaves it. (This is
    // used when the item is clicked.)
    void Dismiss(bool suppress = false);

    // Hides the tooltip shown by any control, if any.
    static void DismissShown();

    // Implementation only: called by the popup when its help link is clicked.
    void OnHelpLinkClicked();

private:
    bool IsShown() const;
    // Returns true if the mouse hasn't moved since the tooltip was shown for
    // the focused item.
    bool IsMouseUnmovedSinceFocus() const;
    void ShowPopup();
    void PositionPopup();
    bool IsMouseOverPopup() const;
    bool IsMouseOverItemOrPopup() const;

    void BindTopLevel();
    void UnbindTopLevel();

    void OnTimer(wxTimerEvent& event);
    void OnSwitchTimer(wxTimerEvent& event);
    void OnOwnerShow(wxShowEvent& event);
    void OnTopLevelShow(wxShowEvent& event);
    void OnTopLevelActivate(wxActivateEvent& event);
    void OnTopLevelIconize(wxIconizeEvent& event);
    void OnTopLevelCharHook(wxKeyEvent& event);

    wxRibbonControl* const m_owner;
    wxRibbonToolTipPopup* m_popup = nullptr;
    wxWindow* m_topLevel = nullptr;
    // Shows the tooltip after a delay, then checks if it must be hidden.
    wxTimer m_timer;
    // Switches the shown tooltip to another item.
    wxTimer m_switchTimer;

    // The item currently hovered, if any, and the contents of its tooltip.
    bool m_hasItem = false;
    int m_itemId = 0;
    wxRect m_itemRect;
    wxRibbonToolTipInfo m_info;
    wxString m_helpString;

    // The item whose tooltip is shown, valid only if the popup is shown.
    int m_shownId = 0;
    wxRect m_shownRect;

    // Set after a click on the item, until the mouse leaves it.
    bool m_suppressed = false;

    // Set if the item is the focused one rather than the hovered one, and
    // the mouse position at the time.
    bool m_keyboard = false;
    wxPoint m_keyboardMousePos;

    // When the mouse was last over the item or the popup.
    wxLongLong m_lastInside;

    // The helper whose tooltip is currently shown, if any, as only one
    // ribbon tooltip can be shown at any time.
    static wxRibbonToolTipHelper* ms_shown;

    wxDECLARE_NO_COPY_CLASS(wxRibbonToolTipHelper);
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PRIVATE_TOOLTIP_H_
