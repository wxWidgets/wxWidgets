/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/menu.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKMENUH__
#define __GTKMENUH__

#ifdef __GNUG__
    #pragma interface "menu.h"
#endif

//-----------------------------------------------------------------------------
// wxMenuBar
//-----------------------------------------------------------------------------

class wxMenuBar : public wxMenuBarBase
{
public:
    // ctors
    wxMenuBar();
    wxMenuBar(long style);
    wxMenuBar(int n, wxMenu *menus[], const wxString titles[]);
    virtual ~wxMenuBar();

    // implement base class (pure) virtuals
    virtual bool Append( wxMenu *menu, const wxString &title );
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title);
    virtual wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title);
    virtual wxMenu *Remove(size_t pos);

    virtual int FindMenuItem(const wxString& menuString,
                             const wxString& itemString) const;
    virtual wxMenuItem* FindItem( int id, wxMenu **menu = NULL ) const;

    virtual void EnableTop( size_t pos, bool flag );
    virtual void SetLabelTop( size_t pos, const wxString& label );
    virtual wxString GetLabelTop( size_t pos ) const;

    // implementation only from now on
    void SetInvokingWindow( wxWindow *win );
    void UnsetInvokingWindow( wxWindow *win );

    // common part of Append and Insert
    bool GtkAppend(wxMenu *menu, const wxString& title);

    GtkAccelGroup   *m_accel;
    GtkItemFactory  *m_factory;
    GtkWidget       *m_menubar;
    long             m_style;
    wxWindow        *m_invokingWindow;

private:
    DECLARE_DYNAMIC_CLASS(wxMenuBar)
};

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

class wxMenu : public wxMenuBase
{
public:
    // ctors & dtor
    wxMenu(const wxString& title, long style = 0)
        : wxMenuBase(title, style) { Init(); }

    wxMenu(long style = 0) : wxMenuBase(style) { Init(); }

    virtual ~wxMenu();

    // implement base class virtuals
    virtual bool DoAppend(wxMenuItem *item);
    virtual bool DoInsert(size_t pos, wxMenuItem *item);
    virtual wxMenuItem *DoRemove(wxMenuItem *item);

    // TODO: virtual void SetTitle(const wxString& title);

    wxMenu(const wxString& title, const wxFunction func)
        : wxMenuBase(title)
    {
        Callback(func);
    }

    // implementation
    int FindMenuIdByMenuItem( GtkWidget *menuItem ) const;

    // implementation GTK only
    GtkWidget       *m_menu;  // GtkMenu
    GtkWidget       *m_owner;
    GtkAccelGroup   *m_accel;
    GtkItemFactory  *m_factory;

private:
    // common code for all constructors:
    void Init();

    DECLARE_DYNAMIC_CLASS(wxMenu)
};

#endif // __GTKMENUH__
