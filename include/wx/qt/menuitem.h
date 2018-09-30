/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menuitem.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENUITEM_H_
#define _WX_QT_MENUITEM_H_

#include "wx/menuitem.h"

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

class QAction;

class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxMenu;

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
#if wxUSE_OWNER_DRAWN
    , public wxOwnerDrawnBase
#endif
{
public:
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);

    virtual void SetItemLabel(const wxString& str);
    virtual void SetCheckable(bool checkable);

    virtual void Enable(bool enable = true);
    virtual bool IsEnabled() const;

    virtual void Check(bool check = true);
    virtual bool IsChecked() const;

    void SetBitmaps(const wxBitmap& bmpChecked,
                    const wxBitmap& bmpUnchecked = wxNullBitmap);
    void SetBitmap(const wxBitmap& bitmap);
    const wxBitmap& GetBitmap() const;

    virtual QAction *GetHandle() const;

#if wxUSE_OWNER_DRAWN
    void SetDisabledBitmap(const wxBitmap& bmpDisabled);
    const wxBitmap& GetDisabledBitmap() const;

    // override wxOwnerDrawn base class virtuals
    virtual wxString GetName() const wxOVERRIDE;
    virtual bool OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus stat) wxOVERRIDE;
#endif // wxUSE_OWNER_DRAWN

private:
    // Qt is using an action instead of a menu item.
    QAction *m_qtAction;

#if wxUSE_OWNER_DRAWN
    wxBitmap m_bmpDisabled;
#endif // wxUSE_OWNER_DRAWN

    wxDECLARE_DYNAMIC_CLASS( wxMenuItem );
};



#endif // _WX_QT_MENUITEM_H_
