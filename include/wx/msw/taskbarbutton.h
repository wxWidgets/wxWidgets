/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/taskbarbutton.h
// Purpose:     Defines wxTaskBarButtonImpl class.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-06-01
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef  _WX_MSW_TASKBARBUTTON_H_
#define  _WX_MSW_TASKBARBUTTON_H_

#if wxUSE_TASKBARBUTTON

#include "wx/defs.h"

struct ITaskbarList3;

class wxTaskBarButtonImpl : public wxTaskBarButton {
public:
    virtual ~wxTaskBarButtonImpl();

    virtual void SetProgressValue(int value) wxOVERRIDE;
    virtual void Show() wxOVERRIDE;
    virtual void Hide() wxOVERRIDE;
    virtual void SetThumbnailTooltip(const wxString& tooltip) wxOVERRIDE;
    virtual void SetProgressState(wxTaskBarButtonState state) wxOVERRIDE;
    virtual void SetOverlayIcon(const wxIcon& icon) wxOVERRIDE;
    virtual void SetThumbnailClip(const wxRect& rect) wxOVERRIDE;

private:
    friend class wxTopLevelWindowMSW;
    wxTaskBarButtonImpl(WXWidget parent);

    WXWidget m_hwnd;
    ITaskbarList3 *m_taskbarList;
};

#endif // wxUSE_TASKBARBUTTON

#endif  // _WX_MSW_TASKBARBUTTON_H_
