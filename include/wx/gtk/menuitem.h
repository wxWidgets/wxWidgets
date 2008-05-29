///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.h
// Purpose:     wxMenuItem class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKMENUITEM_H_
#define _WX_GTKMENUITEM_H_

#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// wxMenuItem
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = (wxMenu *)NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = (wxMenu *)NULL);
    virtual ~wxMenuItem();

    // implement base class virtuals
    virtual void SetItemLabel( const wxString& str );
    virtual wxString GetItemLabel() const;
    virtual void Enable( bool enable = true );
    virtual void Check( bool check = true );
    virtual bool IsChecked() const;
    virtual void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    virtual const wxBitmap& GetBitmap() const { return m_bitmap; }

#if wxUSE_ACCEL
    virtual wxAcceleratorEntry *GetAccel() const;
#endif // wxUSE_ACCEL

    // implementation
    void SetMenuItem(GtkWidget *menuItem) { m_menuItem = menuItem; }
    GtkWidget *GetMenuItem() const { return m_menuItem; }

    // splits given string in the label, doing & => _ translation, which is returned,
    // and in the hotkey which is used to set given pointer
    static wxString GTKProcessMenuItemLabel(const wxString& str, wxString *hotKey);

#if WXWIN_COMPATIBILITY_2_8
    // compatibility only, don't use in new code
    wxDEPRECATED(
    wxMenuItem(wxMenu *parentMenu,
               int id,
               const wxString& text,
               const wxString& help,
               bool isCheckable,
               wxMenu *subMenu = NULL)
    );
#endif

private:
    // common part of all ctors
    void Init(const wxString& text);

    // DoSetText() transforms the accel mnemonics in our label from MSW/wxWin
    // style to GTK+ and is called from ctor and SetText()
    void DoSetText(const wxString& text);

    wxString  m_hotKey;
    wxBitmap  m_bitmap; // Bitmap for menuitem, if any

    GtkWidget *m_menuItem;  // GtkMenuItem

    DECLARE_DYNAMIC_CLASS(wxMenuItem)
};

#endif // _WX_GTKMENUITEM_H_
