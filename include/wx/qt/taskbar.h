/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/taskbar.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TASKBAR_H_
#define _WX_QT_TASKBAR_H_

#include <QtGui/QSystemTrayIcon>

class WXDLLIMPEXP_CORE wxTaskBarIcon : public wxTaskBarIconBase
{
public:
    wxTaskBarIcon();

    virtual bool SetIcon(const wxIcon& icon,
                         const wxString& tooltip = wxEmptyString);
    virtual bool RemoveIcon();
    virtual bool PopupMenu(wxMenu *menu);

protected:
    DECLARE_DYNAMIC_CLASS(wxTaskBarIcon)

private:
    QSystemTrayIcon m_qtSystemTrayIcon;
};

#endif // _WX_QT_TASKBAR_H_
