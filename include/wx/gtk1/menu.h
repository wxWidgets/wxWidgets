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

    GtkAccelGroup   *m_accel;
    GtkItemFactory  *m_factory;
    GtkWidget       *m_menubar;
    long             m_style;
    wxWindow        *m_invokingWindow;

#if 0 // seems to be unused (VZ)
    wxMenuList& GetMenus() { return m_menus; }
#endif // 0

private:
    DECLARE_DYNAMIC_CLASS(wxMenuBar)
};

//-----------------------------------------------------------------------------
// wxMenu
//-----------------------------------------------------------------------------

class wxMenu : public wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(wxMenu)

public:
    wxMenu( const wxString& title, const wxFunction func)
    {
        Init(title, 0, func);
    }
    wxMenu( long style )
    {
        Init( wxEmptyString, style );
    }
    wxMenu( const wxString& title = wxEmptyString, long style = 0 )
    {
        Init(title, style);
    }

    ~wxMenu();

    // title
    void SetTitle(const wxString& label);
    const wxString GetTitle() const;

    // menu creation
    void AppendSeparator();
    void Append(int id, const wxString &item,
                const wxString &helpStr = "", bool checkable = FALSE);
    void Append(int id, const wxString &item,
                wxMenu *subMenu, const wxString &helpStr = "" );
    void Append(wxMenuItem *pItem);
    void Break() { }

    // delete item. don't delete the wxMenu if it's a submenu
    void Delete( int id );

    // find item by name/id
    int FindItem( const wxString itemString ) const;
    wxMenuItem *FindItem( int id ) const;

    // get/set item's state
    void Enable( int id, bool enable );
    bool IsEnabled( int id ) const;
    void Check( int id, bool check );
    bool IsChecked( int id ) const;

    void SetLabel( int id, const wxString &label );
    wxString GetLabel( int id ) const;

        // helpstring
    virtual void SetHelpString(int id, const wxString& helpString);
    virtual wxString GetHelpString(int id) const ;

        // accessors
    wxList& GetItems() { return m_items; }

    void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
    wxEvtHandler *GetEventHandler() { return m_eventHandler; }

    void SetClientData( void* clientData ) { m_clientData = clientData; }
    void* GetClientData() const { return m_clientData; }

    // Updates the UI for a menu and all submenus recursively.
    // source is the object that has the update event handlers
    // defined for it. If NULL, the menu or associated window
    // will be used.
    void UpdateUI(wxEvtHandler* source = (wxEvtHandler*) NULL);

    wxMenuItem *FindItemForId( int id ) const { return FindItem( id ); }

    wxFunction GetCallback() const { return m_callback; }
    void Callback(const wxFunction func) { m_callback = func; }
    wxFunction m_callback;

#ifdef WXWIN_COMPATIBILITY

    // compatibility: these functions are deprecated
    bool Enabled(int id) const { return IsEnabled(id); }
    bool Checked(int id) const { return IsChecked(id); }

#endif // WXWIN_COMPATIBILITY

    // implementation
    int FindMenuIdByMenuItem( GtkWidget *menuItem ) const;
    void SetInvokingWindow( wxWindow *win );
    wxWindow *GetInvokingWindow();

    // implementation GTK only
    GtkWidget       *m_menu;  // GtkMenu
    GtkWidget       *m_owner;
    GtkAccelGroup   *m_accel;
    GtkItemFactory  *m_factory;

    // used by wxMenuBar
    long GetStyle(void) const { return m_style; }

private:
    // common code for both constructors:
    void Init( const wxString& title,
               long style,
               const wxFunction func = (wxFunction) NULL );

    wxString       m_title;
    wxList         m_items;
    wxWindow      *m_invokingWindow;
    wxEvtHandler  *m_eventHandler;
    void          *m_clientData;
    long           m_style;
};

#endif // __GTKMENUH__
