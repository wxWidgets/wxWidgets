/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/taskbar.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TASKBAR_H_
#define _WX_QT_TASKBAR_H_

#include <QtWidgets/QSystemTrayIcon>

class WXDLLIMPEXP_CORE wxTaskBarIcon : public wxTaskBarIconBase
{
public:
    wxTaskBarIcon();

    virtual bool SetIcon(const wxIcon& icon,
                         const wxString& tooltip = wxEmptyString);
    virtual bool RemoveIcon();
    virtual bool PopupMenu(wxMenu *menu);

private:
    QSystemTrayIcon m_qtSystemTrayIcon;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxTaskBarIcon)
};

#endif // _WX_QT_TASKBAR_H_
