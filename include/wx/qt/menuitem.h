/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menuitem.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENUITEM_H_
#define _WX_QT_MENUITEM_H_

#include "wx/menuitem.h"
#include "wx/bitmap.h"

class QAction;
class wxQtAction;

class WXDLLIMPEXP_FWD_CORE wxMenu;

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = nullptr,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = nullptr);

    virtual void SetItemLabel(const wxString& str) override;
    virtual void SetCheckable(bool checkable) override;

    virtual void Enable(bool enable = true) override;
    virtual bool IsEnabled() const override;

    virtual void Check(bool check = true) override;
    virtual bool IsChecked() const override;

    virtual void SetBitmap(const wxBitmapBundle& bitmap) override;

    virtual QAction *GetHandle() const;

    virtual void SetFont(const wxFont& font);
private:
    // Qt is using an action instead of a menu item.
    wxQtAction *m_qtAction;

    wxDECLARE_DYNAMIC_CLASS( wxMenuItem );
};



#endif // _WX_QT_MENUITEM_H_
