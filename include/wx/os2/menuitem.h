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

// ----------------------------------------------------------------------------
// wxMenuItem: an item in the menu, optionally implements owner-drawn behaviour
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxMenuItem: public wxMenuItemBase
#if wxUSE_OWNER_DRAWN
                            , public wxOwnerDrawn
#endif
{
public:
    // ctor & dtor
    wxMenuItem(wxMenu *pParentMenu = NULL, int id = ID_SEPARATOR,
             const wxString& strName = "", const wxString& wxHelp = "",
             bool bCheckable = FALSE, wxMenu *pSubMenu = NULL);
    virtual ~wxMenuItem();

    // override base class virtuals
    virtual void SetText(const wxString& strName);
    virtual wxString GetLabel() const;
    virtual void SetCheckable(bool checkable);

    virtual void Enable(bool bDoEnable = TRUE);
    virtual void Check(bool bDoCheck = TRUE);
    virtual bool IsChecked() const;

#if wxUSE_ACCEL
    virtual wxAcceleratorEntry *GetAccel() const;
#endif // wxUSE_ACCEL

    // unfortunately needed to resolve ambiguity between
    // wxMenuItemBase::IsCheckable() and wxOwnerDrawn::IsCheckable()
    bool IsCheckable() const { return wxMenuItemBase::IsCheckable(); }

    // the id for a popup menu is really its menu handle (as required by
    // ::AppendMenu() API), so this function will return either the id or the
    // menu handle depending on what we're
    int GetRealId() const;

private:
    DECLARE_DYNAMIC_CLASS(wxMenuItem)
};

#endif  //_MENUITEM_H
