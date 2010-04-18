/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menuitem.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENUITEM_H_
#define _WX_QT_MENUITEM_H_

#include <QtGui/QAction>
#include <QtCore/QPointer>

class WXDLLIMPEXP_FWD_CORE wxBitmap;

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);

    void SetBitmap(const wxBitmap& bitmap);
    const wxBitmap& GetBitmap() const;

    virtual QAction *GetHandle() const;

//private slot:
//    void OnTriggered();

private:
    // Qt doesn't have something like a menu item.
    QPointer< QAction > m_qtAction;

    wxDECLARE_NO_COPY_CLASS( wxMenuItem );
};

#endif // _WX_QT_MENUITEM_H_
