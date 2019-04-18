///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/menuitem.h
// Purpose:     wxMenuItem class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.11.97
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _MENUITEM_H
#define   _MENUITEM_H

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"
#include "wx/bitmap.h"

// ----------------------------------------------------------------------------
// wxMenuItem: an item in the menu, optionally implements owner-drawn behaviour
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxMenuItemImpl ;

class WXDLLIMPEXP_CORE wxMenuItem: public wxMenuItemBase
{
public:
    // ctor & dtor
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& name = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);
    virtual ~wxMenuItem();

    // override base class virtuals
    virtual void SetItemLabel(const wxString& strName) wxOVERRIDE;

    virtual void Enable(bool bDoEnable = true) wxOVERRIDE;
    virtual void Check(bool bDoCheck = true) wxOVERRIDE;

    virtual void SetBitmap(const wxBitmap& bitmap) ;
    virtual const wxBitmap& GetBitmap() const { return m_bitmap; }


    // Implementation only from now on.

    // update the os specific representation
    void UpdateItemBitmap() ;
    void UpdateItemText() ;
    void UpdateItemStatus() ;

    wxMenuItemImpl* GetPeer() { return m_peer; }
private:
    void UncheckRadio() ;

    wxBitmap  m_bitmap; // Bitmap for menuitem, if any

    wxMenuItemImpl* m_peer;

    wxDECLARE_DYNAMIC_CLASS(wxMenuItem);
};

#endif  //_MENUITEM_H
