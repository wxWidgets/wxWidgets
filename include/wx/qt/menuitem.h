/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/menuitem.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MENUITEM_H_
#define _WX_QT_MENUITEM_H_

#include "wx/menuitem.h"

#include <QtGui/QAction>
#include <QtCore/QPointer>

class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxMenu;

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
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

    void SetBitmap(const wxBitmap& bitmap);
    const wxBitmap& GetBitmap() const;

    virtual QAction *GetHandle() const;

private:
    friend class wxQtAction;

    void OnItemTriggered( bool checked );

    // Qt is using an action instead of a menu item.
    QPointer< QAction > m_qtAction;

    wxDECLARE_NO_COPY_CLASS( wxMenuItem );
};



class WXDLLIMPEXP_CORE wxQtAction : public QAction
{
    Q_OBJECT

public:
    wxQtAction( wxMenuItem *menuItem, const QString &text, QObject *parent );

private Q_SLOTS:
    void OnActionTriggered( bool checked );

private:
    wxMenuItem *m_menuItem;
};


#endif // _WX_QT_MENUITEM_H_
