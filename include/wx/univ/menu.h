///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/menu.h
// Purpose:     wxMenu and wxMenuBar classes for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.05.01
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_MENU_H_
#define _WX_UNIV_MENU_H_

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

#include "wx/dynarray.h"

// fwd declarations
class WXDLLIMPEXP_FWD_CORE wxMenuInfo;
WX_DECLARE_EXPORTED_OBJARRAY(wxMenuInfo, wxMenuInfoArray);

class WXDLLIMPEXP_FWD_CORE wxMenuGeometryInfo;
class WXDLLIMPEXP_FWD_CORE wxPopupMenuWindow;
class WXDLLIMPEXP_FWD_CORE wxRenderer;

// ----------------------------------------------------------------------------
// wxMenu
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    // ctors and dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style) { Init(); }

    wxMenu(long style = 0) : wxMenuBase(style) { Init(); }

    virtual ~wxMenu();

    // called by wxMenuItem when an item of this menu changes
    void RefreshItem(wxMenuItem *item);

    // does the menu have any items?
    bool IsEmpty() const { return !GetMenuItems().GetFirst(); }

    // show this menu at the given position (in screen coords) and optionally
    // select its first item
    void Popup(const wxPoint& pos, const wxSize& size,
               bool selectFirst = true);

    // dismiss the menu
    void Dismiss();

    // override the base class methods to connect/disconnect event handlers
    virtual void Attach(wxMenuBarBase *menubar) wxOVERRIDE;
    virtual void Detach() wxOVERRIDE;

    // implementation only from here

    // do as if this item were clicked, return true if the resulting event was
    // processed, false otherwise
    bool ClickItem(wxMenuItem *item);

    // process the key event, return true if done
    bool ProcessKeyDown(int key);

#if wxUSE_ACCEL
    // find the item for the given accel and generate an event if found
    bool ProcessAccelEvent(const wxKeyEvent& event);
#endif // wxUSE_ACCEL

protected:
    // implement base class virtuals
    virtual wxMenuItem* DoAppend(wxMenuItem *item) wxOVERRIDE;
    virtual wxMenuItem* DoInsert(size_t pos, wxMenuItem *item) wxOVERRIDE;
    virtual wxMenuItem* DoRemove(wxMenuItem *item) wxOVERRIDE;

    // common part of DoAppend and DoInsert
    void OnItemAdded(wxMenuItem *item);

    // called by wxPopupMenuWindow when the window is hidden
    void OnDismiss(bool dismissParent);

    // return true if the menu is currently shown on screen
    bool IsShown() const;

    // get the menu geometry info
    const wxMenuGeometryInfo& GetGeometryInfo() const;

    // forget old menu geometry info
    void InvalidateGeometryInfo();

    // return either the menubar or the invoking window, normally never NULL
    wxWindow *GetRootWindow() const;

    // get the renderer we use for drawing: either the one of the menu bar or
    // the one of the window if we're a popup menu
    wxRenderer *GetRenderer() const;

#if wxUSE_ACCEL
    // add/remove accel for the given menu item
    void AddAccelFor(wxMenuItem *item);
    void RemoveAccelFor(wxMenuItem *item);
#endif // wxUSE_ACCEL

private:
    // common part of all ctors
    void Init();

    // terminate the current radio group, if any
    void EndRadioGroup();

    // the exact menu geometry is defined by a struct derived from this one
    // which is opaque and defined by the renderer
    wxMenuGeometryInfo *m_geometry;

    // the menu shown on screen or NULL if not currently shown
    wxPopupMenuWindow *m_popupMenu;

#if wxUSE_ACCEL
    // the accel table for this menu
    wxAcceleratorTable m_accelTable;
#endif // wxUSE_ACCEL

    // the position of the first item in the current radio group or -1
    int m_startRadioGroup;

    // it calls out OnDismiss()
    friend class wxPopupMenuWindow;
    wxDECLARE_DYNAMIC_CLASS(wxMenu);
};

// ----------------------------------------------------------------------------
// wxMenuBar
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    // ctors and dtor
    wxMenuBar(long WXUNUSED(style) = 0) { Init(); }
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);
    virtual ~wxMenuBar();

    // implement base class virtuals
    virtual bool Append( wxMenu *menu, const wxString &title ) wxOVERRIDE;
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title) wxOVERRIDE;
    virtual wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title) wxOVERRIDE;
    virtual wxMenu *Remove(size_t pos) wxOVERRIDE;

    virtual void EnableTop(size_t pos, bool enable) wxOVERRIDE;
    virtual bool IsEnabledTop(size_t pos) const wxOVERRIDE;

    virtual void SetMenuLabel(size_t pos, const wxString& label) wxOVERRIDE;
    virtual wxString GetMenuLabel(size_t pos) const wxOVERRIDE;

    virtual void Attach(wxFrame *frame) wxOVERRIDE;
    virtual void Detach() wxOVERRIDE;

    // get the next item for the givan accel letter (used by wxFrame), return
    // -1 if none
    //
    // if unique is not NULL, filled with true if there is only one item with
    // this accel, false if two or more
    int FindNextItemForAccel(int idxStart,
                             int keycode,
                             bool *unique = NULL) const;

    // called by wxFrame to set focus to or open the given menu
    void SelectMenu(size_t pos);
    void PopupMenu(size_t pos);

#if wxUSE_ACCEL
    // find the item for the given accel and generate an event if found
    bool ProcessAccelEvent(const wxKeyEvent& event);
#endif // wxUSE_ACCEL

    // called by wxMenu when it is dismissed
    void OnDismissMenu(bool dismissMenuBar = false);

protected:
    // common part of all ctors
    void Init();

    // event handlers
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnCaptureLost(wxMouseCaptureLostEvent& event);

    // process the mouse move event, return true if we did, false to continue
    // processing as usual
    //
    // the coordinates are client coordinates of menubar, convert if necessary
    bool ProcessMouseEvent(const wxPoint& pt);

    // called when the menu bar loses mouse capture - it is not hidden unlike
    // menus, but it doesn't have modal status any longer
    void OnDismiss();

    // draw the menubar
    virtual void DoDraw(wxControlRenderer *renderer) wxOVERRIDE;

    // menubar geometry
    virtual wxSize DoGetBestClientSize() const wxOVERRIDE;

    // has the menubar been created already?
    bool IsCreated() const { return m_frameLast != NULL; }

    // "fast" version of GetMenuCount()
    size_t GetCount() const { return m_menuInfos.GetCount(); }

    // get the (total) width of the specified menu
    wxCoord GetItemWidth(size_t pos) const;

    // get the rect of the item
    wxRect GetItemRect(size_t pos) const;

    // get the menu from the given point or -1 if none
    int GetMenuFromPoint(const wxPoint& pos) const;

    // refresh the given item
    void RefreshItem(size_t pos);

    // refresh all items after this one (including it)
    void RefreshAllItemsAfter(size_t pos);

    // hide the currently shown menu and show this one
    void DoSelectMenu(size_t pos);

    // popup the currently selected menu
    void PopupCurrentMenu(bool selectFirst = true);

    // hide the currently selected menu
    void DismissMenu();

    // do we show a menu currently?
    bool IsShowingMenu() const { return m_menuShown != 0; }

    // we don't want to have focus except while selecting from menu
    void GiveAwayFocus();

    // Release the mouse capture if we have it
    bool ReleaseMouseCapture();

    // the array containing extra menu info we need
    wxMenuInfoArray m_menuInfos;

    // the current item (only used when menubar has focus)
    int m_current;

private:
    // the last frame to which we were attached, NULL initially
    wxFrame *m_frameLast;

    // the currently shown menu or NULL
    wxMenu *m_menuShown;

    // should be showing the menu? this is subtly different from m_menuShown !=
    // NULL as the menu which should be shown may be disabled in which case we
    // don't show it - but will do as soon as the focus shifts to another menu
    bool m_shouldShowMenu;

    // it calls out ProcessMouseEvent()
    friend class wxPopupMenuWindow;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxMenuBar);
};

#endif // _WX_UNIV_MENU_H_
