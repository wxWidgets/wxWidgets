/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_
#define _WX_MENU_H_

#ifdef __GNUG__
#pragma interface "menu.h"
#endif

#if wxUSE_ACCEL
    #include "wx/accel.h"
    #include "wx/dynarray.h"

    WX_DEFINE_EXPORTED_ARRAY(wxAcceleratorEntry *, wxAcceleratorArray);
#endif // wxUSE_ACCEL

class WXDLLEXPORT wxFrame;

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenu : public wxMenuBase
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

    virtual void Break();

    virtual void SetTitle(const wxString& title);

    // MSW-specific
    bool ProcessCommand(wxCommandEvent& event);

#if WXWIN_COMPATIBILITY
    wxMenu(const wxString& title, const wxFunction func)
        : wxMenuBase(title)
    {
        Callback(func);
    }
#endif // WXWIN_COMPATIBILITY

    // implementation only from now on
    // -------------------------------

  	bool MacMenuSelect(wxEvtHandler* handler, long when , int macMenuId, int macMenuItemNum) ;
  	int	MacGetIndexFromId( int id ) ; 
  	int	MacGetIndexFromItem( wxMenuItem *pItem ) ; 
  	void MacEnableMenu( bool bDoEnable ) ;

    // semi-private accessors
        // get the window which contains this menu
    wxWindow *GetWindow() const;
        // get the menu handle
    WXHMENU GetHMenu() const { return m_hMenu; }

    // attach/detach menu to/from wxMenuBar
    void Attach(wxMenuBar *menubar);
    void Detach();
	short MacGetMenuId() { return m_macMenuId ; }
#if wxUSE_ACCEL
    // called by wxMenuBar to build its accel table from the accels of all menus
    bool HasAccels() const { return !m_accels.IsEmpty(); }
    size_t GetAccelCount() const { return m_accels.GetCount(); }
    size_t CopyAccels(wxAcceleratorEntry *accels) const;

    // called by wxMenuItem when its accels changes
    void UpdateAccel(wxMenuItem *item);

    // helper used by wxMenu itself (returns the index in m_accels)
    int FindAccel(int id) const;
#endif // wxUSE_ACCEL

private:
    // common part of all ctors
    void Init();

    // common part of Append/Insert (behaves as Append is pos == (size_t)-1)
    bool DoInsertOrAppend(wxMenuItem *item, size_t pos = (size_t)-1);

    // if TRUE, insert a breal before appending the next item
    bool m_doBreak;

    // the menu handle of this menu
    WXHMENU m_hMenu;

  	short				m_macMenuId;

  	static short		s_macNextMenuId ;
#if wxUSE_ACCEL
    // the accelerators for our menu items
    wxAcceleratorArray m_accels;
#endif // wxUSE_ACCEL

    DECLARE_DYNAMIC_CLASS(wxMenu)
};

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuBar : public wxMenuBarBase
{
public:
    // ctors & dtor
        // default constructor
    wxMenuBar();
        // unused under MSW
    wxMenuBar(long style);
        // menubar takes ownership of the menus arrays but copies the titles
    wxMenuBar(int n, wxMenu *menus[], const wxString titles[]);
    virtual ~wxMenuBar();

    // menubar construction
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

    // compatibility: these functions are deprecated
#if WXWIN_COMPATIBILITY
    void SetEventHandler(wxEvtHandler *handler) { m_eventHandler = handler; }
    wxEvtHandler *GetEventHandler() { return m_eventHandler; }

    bool Enabled(int id) const { return IsEnabled(id); }
    bool Checked(int id) const { return IsChecked(id); }
#endif // WXWIN_COMPATIBILITY

    // implementation from now on
    WXHMENU Create();
    int  FindMenu(const wxString& title);
    void Detach();

        // returns TRUE if we're attached to a frame
    bool IsAttached() const { return m_menuBarFrame != NULL; }
        // get the frame we live in
    wxFrame *GetFrame() const { return m_menuBarFrame; }
        // attach to a frame
    void Attach(wxFrame *frame);

#if wxUSE_ACCEL
    // get the accel table for all the menus
    const wxAcceleratorTable& GetAccelTable() const { return m_accelTable; }

    // update the accel table (must be called after adding/deletign a menu)
    void RebuildAccelTable();
#endif // wxUSE_ACCEL

    // if the menubar is modified, the display is not updated automatically,
    // call this function to update it (m_menuBarFrame should be !NULL)
    void Refresh();

  void MacInstallMenuBar() ;
  void MacMenuSelect(wxEvtHandler* handler, long when , int macMenuId, int macMenuItemNum) ;
  static wxMenuBar* MacGetInstalledMenuBar() { return s_macInstalledMenuBar ; }

protected:
    // common part of all ctors
    void Init();

#if WXWIN_COMPATIBILITY
    wxEvtHandler *m_eventHandler;
#endif // WXWIN_COMPATIBILITY

    wxArrayString m_titles;

    wxFrame      *m_menuBarFrame;

#if wxUSE_ACCEL
    // the accelerator table for all accelerators in all our menus
    wxAcceleratorTable m_accelTable;
#endif // wxUSE_ACCEL

private:
  static wxMenuBar*			s_macInstalledMenuBar ;

    DECLARE_DYNAMIC_CLASS(wxMenuBar)
};

#endif // _WX_MENU_H_
