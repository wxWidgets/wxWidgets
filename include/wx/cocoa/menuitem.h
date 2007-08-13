///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/menuitem.h
// Purpose:     wxMenuItem class
// Author:      David Elliott
// Modified by:
// Created:     2002/12/13
// RCS-ID:      $Id:
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COCOA_MENUITEM_H_
#define _WX_COCOA_MENUITEM_H_

#include "wx/hashmap.h"
#include "wx/bitmap.h"

#include "wx/cocoa/ObjcRef.h"

// ========================================================================
// wxMenuItem
// ========================================================================

#define wxMenuItemCocoa wxMenuItem
class wxMenuItemCocoa;
WX_DECLARE_HASH_MAP(WX_NSMenuItem,wxMenuItem*,wxPointerHash,wxPointerEqual,wxMenuItemCocoaHash);

class WXDLLEXPORT wxMenuItemCocoa : public wxMenuItemBase
{
public:
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
    wxMenuItemCocoa(wxMenu *parentMenu = (wxMenu *)NULL,
               int id = wxID_SEPARATOR,
               const wxString& name = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = (wxMenu *)NULL);
    virtual ~wxMenuItemCocoa();

// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
    inline WX_NSMenuItem GetNSMenuItem() { return m_cocoaNSMenuItem; }
    static inline wxMenuItem* GetFromCocoa(WX_NSMenuItem cocoaNSMenuItem)
    {
        wxMenuItemCocoaHash::iterator iter=sm_cocoaHash.find(cocoaNSMenuItem);
        if(iter!=sm_cocoaHash.end())
            return iter->second;
        return NULL;
    }
    void CocoaItemSelected();
    bool Cocoa_validateMenuItem();
protected:
    void CocoaSetKeyEquivalent();
    WX_NSMenuItem m_cocoaNSMenuItem;
    static wxMenuItemCocoaHash sm_cocoaHash;
    static wxObjcAutoRefFromAlloc<struct objc_object *> sm_cocoaTarget;
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    // override base class virtuals to update the item appearance on screen
    virtual void SetText(const wxString& text);
    virtual void SetCheckable(bool checkable);

    virtual void Enable(bool enable = TRUE);
    virtual void Check(bool check = TRUE);

    // we add some extra functions which are also available under MSW from
    // wxOwnerDrawn class - they will be moved to wxMenuItemBase later
    // hopefully
    void SetBitmaps(const wxBitmap& bmpChecked,
                    const wxBitmap& bmpUnchecked = wxNullBitmap);
    void SetBitmap(const wxBitmap& bmp) { SetBitmaps(bmp); }
    const wxBitmap& GetBitmap(bool checked = TRUE) const
      { return checked ? m_bmpChecked : m_bmpUnchecked; }

protected:
    // notify the menu about the change in this item
    inline void NotifyMenu();

    // set the accel index and string from text
    void UpdateAccelInfo();

    // the bitmaps (may be invalid, then they're not used)
    wxBitmap m_bmpChecked,
             m_bmpUnchecked;

    // the accel string (i.e. "Ctrl-Q" or "Alt-F1")
    wxString m_strAccel;

private:
    DECLARE_DYNAMIC_CLASS(wxMenuItem)

public:

#if wxABI_VERSION >= 20805
    // return the item label including any mnemonics and accelerators.
    // This used to be called GetText.
    wxString GetItemLabel() const { return GetText(); }
#endif
};

#endif // _WX_COCOA_MENUITEM_H_

