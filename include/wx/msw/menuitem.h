///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.h
// Purpose:     wxMenuItem class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _MENUITEM_H
#define   _MENUITEM_H

#ifdef __GNUG__
    #pragma interface "menuitem.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/setup.h"

// an exception to the general rule that a normal header doesn't include other
// headers - only because ownerdrw.h is not always included and I don't want
// to write #ifdef's everywhere...
#if wxUSE_OWNER_DRAWN
    #include  "wx/ownerdrw.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// id for a separator line in the menu (invalid for normal item)
#define   ID_SEPARATOR    (-1)

// ----------------------------------------------------------------------------
// wxMenuItem: an item in the menu, optionally implements owner-drawn behaviour
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxMenuItem: public wxObject
#if wxUSE_OWNER_DRAWN
                            , public wxOwnerDrawn
#endif
{
DECLARE_DYNAMIC_CLASS(wxMenuItem)

public:
  // ctor & dtor
  wxMenuItem(wxMenu *pParentMenu = NULL, int id = ID_SEPARATOR,
             const wxString& strName = "", const wxString& wxHelp = "",
             bool bCheckable = FALSE, wxMenu *pSubMenu = NULL);
  virtual ~wxMenuItem();

  // accessors (some more are inherited from wxOwnerDrawn or are below)
  bool              IsSeparator() const { return m_idItem == ID_SEPARATOR;  }
  bool              IsEnabled()   const { return m_bEnabled;                }
  bool              IsChecked()   const { return m_bChecked;                }
  bool              IsSubMenu()   const { return GetSubMenu() != NULL;      }

  int               GetId()       const { return m_idItem;    }
  const wxString&   GetHelp()     const { return m_strHelp;   }
  wxMenu           *GetSubMenu()  const { return m_pSubMenu;  }

  // the id for a popup menu is really its menu handle (as required by
  // ::AppendMenu() API)
  int               GetRealId()   const;

  // operations
  void SetName(const wxString& strName);
  void SetHelp(const wxString& strHelp) { m_strHelp = strHelp; }

  void Enable(bool bDoEnable = TRUE);
  void Check(bool bDoCheck = TRUE);

  void DeleteSubMenu();

private:
  int         m_idItem;         // numeric id of the item
  wxString    m_strHelp;        // associated help string
  wxMenu     *m_pSubMenu,       // may be NULL
             *m_pParentMenu;    // menu this item is contained in
  bool        m_bEnabled,       // enabled or greyed?
              m_bChecked;       // checked? (only if checkable)

#if wxUSE_OWNER_DRAWN
  // wxOwnerDrawn base class already has these variables - nothing to do

#else   //!owner drawn
  bool        m_bCheckable;     // can be checked?
  wxString    m_strName;        // name or label of the item

public:
  const wxString&   GetName()     const { return m_strName;    }
  bool              IsCheckable() const { return m_bCheckable; }
#endif  //owner drawn
};

#endif  //_MENUITEM_H
