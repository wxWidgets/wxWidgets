///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/menu.h
// Purpose:     wxMenu and wxMenuBar classes for wxUniversal
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.05.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_MENU_H_
#define _WX_UNIV_MENU_H_

#ifdef __GNUG__
    #pragma interface "univmenu.h"
#endif

#include "wx/dynarray.h"

struct WXDLLEXPORT wxMenuInfo;
WX_DECLARE_OBJARRAY(wxMenuInfo, wxMenuInfoArray);

// ----------------------------------------------------------------------------
// wxMenu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenu : public wxMenuBase
{
public:
    // ctors and dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style) { Init(); }

    wxMenu(long style = 0) : wxMenuBase(style) { Init(); }

    virtual ~wxMenu();

protected:
    // implement base class virtuals
    virtual bool DoAppend(wxMenuItem *item);
    virtual bool DoInsert(size_t pos, wxMenuItem *item);
    virtual wxMenuItem *DoRemove(wxMenuItem *item);

private:
    // common part of all ctors
    void Init();

    DECLARE_DYNAMIC_CLASS(wxMenu)
};

// ----------------------------------------------------------------------------
// wxMenuBar
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuBar : public wxMenuBarBase
{
public:
    // ctors and dtor
    wxMenuBar(long WXUNUSED(style) = 0) { Init(); }
    virtual ~wxMenuBar();

    // implement base class virtuals
    virtual bool Append( wxMenu *menu, const wxString &title );
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title);
    virtual wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title);
    virtual wxMenu *Remove(size_t pos);

    virtual void EnableTop(size_t pos, bool enable);
    virtual bool IsEnabledTop(size_t pos) const;

    virtual void SetLabelTop(size_t pos, const wxString& label);
    virtual wxString GetLabelTop(size_t pos) const;

    virtual void Attach(wxFrame *frame);
    virtual void Detach();

protected:
    // common part of all ctors
    void Init();

    // event handlers
    void OnSetFocus(wxFocusEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // draw the menubar
    virtual void DoDraw(wxControlRenderer *renderer);

    // menubar geometry
    virtual wxSize DoGetBestClientSize() const;

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

    // the array containing extra menu info we need
    wxMenuInfoArray m_menuInfos;

    // the current item (only used when menubar has focus)
    int m_current;

private:
    // the last frame to which we were attached, NULL initially
    wxFrame *m_frameLast;

    // the last window which had focus before us
    wxWindow *m_focusOld;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMenuBar)
};

#endif // _WX_UNIV_MENU_H_
