///////////////////////////////////////////////////////////////////////////////
// Name:        wx/os2/menuitem.h
// Purpose:     wxMenuItem class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _MENUITEM_H
#define   _MENUITEM_H

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

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
    //
    // ctor & dtor
    //
    wxMenuItem( wxMenu*         pParentMenu = NULL
               ,int             nId = wxID_SEPARATOR
               ,const wxString& rStrName = wxEmptyString
               ,const wxString& rWxHelp = wxEmptyString
               ,wxItemKind      eKind = wxITEM_NORMAL
               ,wxMenu*         pSubMenu = NULL
              );

    //
    // Depricated, do not use in new code
    //
    wxMenuItem( wxMenu*         pParentMenu
               ,int             vId
               ,const wxString& rsText
               ,const wxString& rsHelp
               ,bool            bIsCheckable
               ,wxMenu*         pSubMenu = (wxMenu *)NULL
              );
    virtual ~wxMenuItem();

    //
    // Override base class virtuals
    //
    virtual void SetText(const wxString& rStrName);
    virtual void SetCheckable(bool bCheckable);

    virtual void Enable(bool bDoEnable = true);
    virtual void Check(bool bDoCheck = true);
    virtual bool IsChecked(void) const;

    //
    // Unfortunately needed to resolve ambiguity between
    // wxMenuItemBase::IsCheckable() and wxOwnerDrawn::IsCheckable()
    //
    bool IsCheckable(void) const { return wxMenuItemBase::IsCheckable(); }

    //
    // The id for a popup menu is really its menu handle (as required by
    // ::AppendMenu() API), so this function will return either the id or the
    // menu handle depending on what we're
    //
    int GetRealId(void) const;

    //
    // Mark item as belonging to the given radio group
    //
    void SetAsRadioGroupStart(void);
    void SetRadioGroupStart(int nStart);
    void SetRadioGroupEnd(int nEnd);

    //
    // All OS/2PM Submenus and menus have one of these
    //
    MENUITEM                        m_vMenuData;

private:
    void Init();

    //
    // The positions of the first and last items of the radio group this item
    // belongs to or -1: start is the radio group start and is valid for all
    // but first radio group items (m_isRadioGroupStart == FALSE), end is valid
    // only for the first one
    //
    union
    {
        int m_nStart;
        int m_nEnd;
    } m_vRadioGroup;

    //
    // Does this item start a radio group?
    //
    bool                            m_bIsRadioGroupStart;

    DECLARE_DYNAMIC_CLASS(wxMenuItem)

public:

#if wxABI_VERSION >= 20805
    // return the item label including any mnemonics and accelerators.
    // This used to be called GetText.
    wxString GetItemLabel() const { return GetText(); }
#endif

}; // end of CLASS wxMenuItem

#endif  //_MENUITEM_H
