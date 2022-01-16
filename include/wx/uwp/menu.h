/////////////////////////////////////////////////////////////////////////////
// Name:        wx/uwp/menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      Yann Clotioloman Yéo
// Modified by:
// Created:     02/01/2022
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UWP_MENU_H_
#define _WX_UWP_MENU_H_

#if wxUSE_ACCEL
    #include "wx/accel.h"
    #include "wx/dynarray.h"

    WX_DEFINE_EXPORTED_ARRAY_PTR(wxAcceleratorEntry *, wxAcceleratorArray);
#endif // wxUSE_ACCEL

class WXDLLIMPEXP_FWD_CORE wxFrame;

class wxMenuRadioItemsData;


#include "wx/arrstr.h"

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    // ctors & dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style) { Init(); }

    wxMenu(long style = 0) : wxMenuBase(style) { Init(); }

    virtual ~wxMenu();

    virtual void Break() wxOVERRIDE;

    virtual void SetTitle(const wxString& title) wxOVERRIDE;

protected:
    virtual wxMenuItem* DoAppend(wxMenuItem *item) wxOVERRIDE;
    virtual wxMenuItem* DoInsert(size_t pos, wxMenuItem *item) wxOVERRIDE;
    virtual wxMenuItem* DoRemove(wxMenuItem *item) wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMenu);
};

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    // ctors & dtor
        // default constructor
    wxMenuBar();
        // unused under MSW
    wxMenuBar(long style);
        // menubar takes ownership of the menus arrays but copies the titles
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);
    virtual ~wxMenuBar();

    // menubar construction
    virtual bool Append( wxMenu *menu, const wxString &title ) wxOVERRIDE;
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title) wxOVERRIDE;
    virtual wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title) wxOVERRIDE;
    virtual wxMenu *Remove(size_t pos) wxOVERRIDE;

    virtual void EnableTop( size_t pos, bool flag ) wxOVERRIDE;
    virtual bool IsEnabledTop(size_t pos) const wxOVERRIDE;
    virtual void SetMenuLabel( size_t pos, const wxString& label ) wxOVERRIDE;
    virtual wxString GetMenuLabel( size_t pos ) const wxOVERRIDE;

    // implementation from now on
    WXHMENU Create();
    virtual void Detach() wxOVERRIDE;
    virtual void Attach(wxFrame *frame) wxOVERRIDE;


protected:
    WXHMENU       m_hMenu;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMenuBar);
};

#endif // _WX_UWP_MENU_H_
