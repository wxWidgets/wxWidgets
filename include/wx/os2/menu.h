/////////////////////////////////////////////////////////////////////////////
// Name:        menu.h
// Purpose:     wxMenu, wxMenuBar classes
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENU_H_
#define _WX_MENU_H_

#if wxUSE_ACCEL
    #include "wx/accel.h"
    #include "wx/dynarray.h"

    WX_DEFINE_EXPORTED_ARRAY(wxAcceleratorEntry *, wxAcceleratorArray);
#endif // wxUSE_ACCEL

class WXDLLEXPORT wxFrame;

void wxSetShortCutKey(wxChar* zText);

// ----------------------------------------------------------------------------
// Menu
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenu : public wxMenuBase
{
public:
    //
    // Ctors & dtor
    //
    wxMenu( const wxString& rTitle
           ,long            lStyle = 0
          )
          : wxMenuBase( rTitle
                       ,lStyle
                      )
    {
        Init();
    }

    wxMenu(long lStyle = 0)
          : wxMenuBase(lStyle)
    {
        Init();
    }

    virtual ~wxMenu();

    //
    // Implement base class virtuals
    //
    virtual bool        DoAppend(wxMenuItem* pItem);
    virtual bool        DoInsert( size_t      nPos
                                 ,wxMenuItem* pItem
                                );
    virtual wxMenuItem* DoRemove(wxMenuItem* pItem);
    virtual void        Break(void);
    virtual void        SetTitle(const wxString& rTitle);

#if wxUSE_MENU_CALLBACK
    wxMenu( const wxString&  rTitle
           ,const wxFunction fnFunc
          )
          : wxMenuBase(rTitle)
    {
        Init();
        Callback(fnFunc);
    }
#endif // wxUSE_MENU_CALLBACK

    //
    // OS2-specific
    //
    bool ProcessCommand(wxCommandEvent& rEvent);


    //
    // Implementation only from now on
    // -------------------------------
    //
    bool OS2Command( WXUINT uParam
                    ,WXWORD wId
                   );

    //
    // Semi-private accessors
    //

    //
    // Get the window which contains this menu
    //
    wxWindow* GetWindow(void) const;

    //
    // Get the menu handle
    //
    WXHMENU GetHMenu() const { return m_hMenu; }

    //
    // Attach/detach menu to/from wxMenuBar
    //
    void Attach(wxMenuBar* pMenubar);
    void Detach(void);

#if wxUSE_ACCEL
    //
    // Called by wxMenuBar to build its accel table from the accels of all menus
    //
    bool   HasAccels(void) const { return !m_vAccels.IsEmpty(); }
    size_t GetAccelCount(void) const { return m_vAccels.GetCount(); }
    size_t CopyAccels(wxAcceleratorEntry* pAccels) const;

    //
    // Called by wxMenuItem when its accels changes
    //
    void UpdateAccel(wxMenuItem* pItem);

    //
    // Helper used by wxMenu itself (returns the index in m_accels)
    //
    int FindAccel(int nId) const;
#endif // wxUSE_ACCEL

    //
    // All OS/2PM Menu's have one of these
    //
    MENUITEM                        m_vMenuData;

private:
    //
    // Common part of all ctors
    //
    void Init();

    //
    // Common part of Append/Insert (behaves as Append is pos == (size_t)-1)
    //
    bool DoInsertOrAppend( wxMenuItem* pItem
                          ,size_t      nPos = (size_t)-1
                         );

    //
    // If TRUE, insert a breal before appending the next item
    //
    bool                            m_bDoBreak;

    //
    // The menu handle of this menu
    //
    WXHMENU                         m_hMenu;

    //
    // The helper variable for creating unique IDs.
    //
    static USHORT		    m_nextMenuId;

#if wxUSE_ACCEL
    //
    // The accelerators for our menu items
    //
    wxAcceleratorArray              m_vAccels;
#endif // wxUSE_ACCEL

    DECLARE_DYNAMIC_CLASS(wxMenu)
}; // end of wxMenu

// ----------------------------------------------------------------------------
// Menu Bar (a la Windows)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuBar : public wxMenuBarBase
{
public:
    //
    // Ctors & dtor
    //

    //
    // Default constructor
    //
    wxMenuBar();

    //
    // Unused under OS2
    wxMenuBar(long lStyle);

    //
    // Menubar takes ownership of the menus arrays but copies the titles
    //
    wxMenuBar( int            n
              ,wxMenu*        vMenus[]
              ,const wxString sTitles[]
             );
    virtual ~wxMenuBar();

    //
    // Menubar construction
    //
    virtual bool        Append( wxMenu*         pMenu
                               ,const wxString& rTitle
                              );
    virtual bool        Insert( size_t          nPos
                               ,wxMenu*         pMenu
                               ,const wxString& rTitle
                              );
    virtual wxMenu*     Replace( size_t          nPos
                                ,wxMenu*         pMenu
                                ,const wxString& rTitle
                               );
    virtual wxMenu*     Remove(size_t nPos);
    virtual int         FindMenuItem( const wxString& rMenuString
                                     ,const wxString& rItemString
                                    ) const;
    virtual wxMenuItem* FindItem( int      nId
                                 ,wxMenu** ppMenu = NULL
                                ) const;

    virtual void        EnableTop( size_t nPos
                                  ,bool   bFlag
                                 );
    virtual void        SetLabelTop( size_t          nPos
                                    ,const wxString& rLabel
                                   );
    virtual wxString    GetLabelTop(size_t nPos) const;

    //
    // Compatibility: these functions are deprecated
    //
#if WXWIN_COMPATIBILITY
    void          SetEventHandler(wxEvtHandler* pHandler) { m_pEventHandler = pHandler; }
    wxEvtHandler* GetEventHandler(void) { return m_pEventHandler; }
    bool          Enabled(int nId) const { return IsEnabled(nId); }
    bool          Checked(int nId) const { return IsChecked(nId); }
#endif // WXWIN_COMPATIBILITY

    //
    // Implementation from now on
    //
    WXHMENU                   Create(void);
    void                      Detach(void);

    //
    // Returns TRUE if we're attached to a frame
    //
    bool                      IsAttached(void) const { return m_pMenuBarFrame != NULL; }

    //
    // Get the frame we live in
    //
    wxFrame *                 GetFrame(void) const { return m_pMenuBarFrame; }

    //
    // Attach to a frame
    //
    void                      Attach(wxFrame* pFrame);

#if wxUSE_ACCEL
    //
    // Get the accel table for all the menus
    //
    const wxAcceleratorTable& GetAccelTable(void) const { return m_vAccelTable; }

    //
    // Update the accel table (must be called after adding/deletign a menu)
    //
    void                      RebuildAccelTable(void);
#endif // wxUSE_ACCEL

    //
    // Get the menu handle
    WXHMENU                   GetHMenu(void) const { return m_hMenu; }

    //
    // If the menubar is modified, the display is not updated automatically,
    // call this function to update it (m_menuBarFrame should be !NULL)
    //
    void                      Refresh(void);

protected:
    //
    // Common part of all ctors
    //
    void                      Init(void);

#if WXWIN_COMPATIBILITY
    wxEvtHandler*                   m_pEventHandler;
#endif // WXWIN_COMPATIBILITY

    wxArrayString m_titles;

    wxFrame*                        m_pMenuBarFrame;
    WXHMENU                         m_hMenu;

#if wxUSE_ACCEL
    //
    // The accelerator table for all accelerators in all our menus
    //
    wxAcceleratorTable              m_vAccelTable;
#endif // wxUSE_ACCEL

private:
    //
    // Virtual function hiding suppression
    //
    void                      Refresh( bool          bErase
                                      ,const wxRect* pRect
                                     )
    { wxWindow::Refresh(bErase, pRect); }

    DECLARE_DYNAMIC_CLASS(wxMenuBar)
};

#endif // _WX_MENU_H_
