/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/taskbar.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TASKBAR_H_
#define _WX_QT_TASKBAR_H_

class QSystemTrayIcon;

class WXDLLIMPEXP_ADV wxTaskBarIcon : public wxTaskBarIconBase
{
public:
    wxTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE);
    virtual ~wxTaskBarIcon();

    // Accessors
    bool IsOk() const { return false; }
    bool IsIconInstalled() const { return false; }

    // Operations
    virtual bool SetIcon(const wxBitmapBundle& icon,
                         const wxString& tooltip = wxEmptyString) wxOVERRIDE;
    virtual bool RemoveIcon() wxOVERRIDE;
    virtual bool PopupMenu(wxMenu *menu) wxOVERRIDE;

private:
    QSystemTrayIcon *m_qtSystemTrayIcon;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTaskBarIcon);
};

#endif // _WX_QT_TASKBAR_H_
