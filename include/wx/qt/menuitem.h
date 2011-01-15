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
#include "wx/qt/winevent_qt.h"
#include <QtGui/QAction>
#include "wx/qt/pointer_qt.h"

class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxMenu;

class WXDLLIMPEXP_FWD_CORE wxQtAction;

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
    // Qt is using an action instead of a menu item.
    wxQtPointer< wxQtAction > m_qtAction;

    wxDECLARE_NO_COPY_CLASS( wxMenuItem );
};



class WXDLLIMPEXP_CORE wxQtAction : public QAction, public wxQtSignalHandler< wxMenuItem >
{
    Q_OBJECT

public:
    wxQtAction( wxMenu *parent, int id, const wxString &text, const wxString &help,
        wxItemKind kind, wxMenu *subMenu, wxMenuItem *handler );

    void SetItemLabel( const wxString &label );

    void Enable( bool enable );
    bool IsEnabled() const;

    void SetCheckable( bool checkable );
    void Check( bool checked );
    bool IsChecked() const;

    void SetBitmap( const wxBitmap &bitmap );
    const wxBitmap &GetBitmap() const;

private Q_SLOTS:
    void OnActionTriggered( bool checked );
};


#endif // _WX_QT_MENUITEM_H_
