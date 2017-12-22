/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/taskbar.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TASKBAR_H_
#define _WX_QT_TASKBAR_H_

class QSystemTrayIcon;

class WXDLLIMPEXP_CORE wxTaskBarIcon : public wxTaskBarIconBase
{
public:
    wxTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE);
    virtual ~wxTaskBarIcon();

    // Accessors
    bool IsOk() const { return false; }
    bool IsIconInstalled() const { return false; }

    // Operations
    virtual bool SetIcon(const wxIcon& icon,
                         const wxString& tooltip = wxEmptyString);
    virtual bool RemoveIcon();
    virtual bool PopupMenu(wxMenu *menu);

private:
    QSystemTrayIcon *m_qtSystemTrayIcon;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTaskBarIcon);
};

#endif // _WX_QT_TASKBAR_H_
