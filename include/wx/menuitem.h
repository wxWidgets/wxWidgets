///////////////////////////////////////////////////////////////////////////////
// Name:        wx/menuitem.h
// Purpose:     wxMenuItem class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.10.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MENUITEM_H_BASE_
#define _WX_MENUITEM_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/object.h"  // base class

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// id for a separator line in the menu (invalid for normal item)
#define wxID_SEPARATOR (-1)

#ifndef ID_SEPARATOR    // for compatibility only, don't use in new code
    #define ID_SEPARATOR    wxID_SEPARATOR
#endif

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxAcceleratorEntry;
class WXDLLEXPORT wxMenuItem;
class WXDLLEXPORT wxMenu;

// ----------------------------------------------------------------------------
// wxMenuItem is an item in the menu which may be either a normal item, a sub
// menu or a separator
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxMenuItemBase : public wxObject
{
public:
    // creation
    static wxMenuItem *New(wxMenu *parentMenu = (wxMenu *)NULL,
                           int id = wxID_SEPARATOR,
                           const wxString& text = wxEmptyString,
                           const wxString& help = wxEmptyString,
                           bool isCheckable = FALSE,
                           wxMenu *subMenu = (wxMenu *)NULL);

    // destruction: wxMenuItem will delete its submenu
    virtual ~wxMenuItemBase();

    // the menu we're in
    wxMenu *GetMenu() const { return m_parentMenu; }

    // get/set id
    void SetId(int id) { m_id = id; }
    int  GetId() const { return m_id; }
    bool IsSeparator() const { return m_id == wxID_SEPARATOR; }

    // the item's text (or name)
    //
    // NB: the item's text includes the accelerators and mnemonics info (if
    //     any), i.e. it may contain '&' or '_' or "\t..." and thus is
    //     different from the item's label which only contains the text shown
    //     in the menu
    virtual void SetText(const wxString& str) { m_text = str; }
    virtual wxString GetLabel() const { return m_text; }
    const wxString& GetText() const { return m_text; }

    // what kind of menu item we are
    virtual void SetCheckable(bool checkable) { m_isCheckable = checkable; }
    bool IsCheckable() const { return m_isCheckable; }

    bool IsSubMenu() const { return m_subMenu != NULL; }
    void SetSubMenu(wxMenu *menu) { m_subMenu = menu; }
    wxMenu *GetSubMenu() const { return m_subMenu; }

    // state
    virtual void Enable(bool enable = TRUE) { m_isEnabled = enable; }
    virtual bool IsEnabled() const { return m_isEnabled; }

    virtual void Check(bool check = TRUE) { m_isChecked = check; }
    virtual bool IsChecked() const { return m_isChecked; }
    void Toggle() { Check(!m_isChecked); }

    // help string (displayed in the status bar by default)
    void SetHelp(const wxString& str) { m_help = str; }
    const wxString& GetHelp() const { return m_help; }

#if wxUSE_ACCEL
    // get our accelerator or NULL (caller must delete the pointer)
    virtual wxAcceleratorEntry *GetAccel() const { return NULL; }

    // set the accel for this item - this may also be done indirectly with
    // SetText()
    virtual void SetAccel(wxAcceleratorEntry *accel);
#endif // wxUSE_ACCEL

    // compatibility only, use new functions in the new code
    void SetName(const wxString& str) { SetText(str); }
    const wxString& GetName() const { return GetText(); }

protected:
    int           m_id;             // numeric id of the item >= 0 or -1
    wxMenu       *m_parentMenu,     // the menu we belong to
                 *m_subMenu;        // our sub menu or NULL
    wxString      m_text,           // label of the item
                  m_help;           // the help string for the item
    bool          m_isCheckable;    // can be checked?
    bool          m_isChecked;      // is checked?
    bool          m_isEnabled;      // is enabled?
};

// ----------------------------------------------------------------------------
// include the real class declaration
// ----------------------------------------------------------------------------

#ifdef wxUSE_BASE_CLASSES_ONLY
    #define wxMenuItem wxMenuItemBase
#else // !wxUSE_BASE_CLASSES_ONLY
#if defined(__WXMSW__)
    #include "wx/msw/menuitem.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/menuitem.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/menuitem.h"
#elif defined(__WXQT__)
    #include "wx/qt/menuitem.h"
#elif defined(__WXMAC__)
    #include "wx/mac/menuitem.h"
#elif defined(__WXPM__)
    #include "wx/os2/menuitem.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/menuitem.h"
#endif
#endif // wxUSE_BASE_CLASSES_ONLY/!wxUSE_BASE_CLASSES_ONLY

#endif
    // _WX_MENUITEM_H_BASE_
