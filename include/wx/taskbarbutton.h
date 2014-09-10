/////////////////////////////////////////////////////////////////////////////
// Name:        include/taskbarbutton.h
// Purpose:     Defines wxTaskBarButton class for manipulating buttons on the
//              windows taskbar.
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-04-30
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TASKBARBUTTON_H_
#define _WX_TASKBARBUTTON_H_

#if wxUSE_TASKBARBUTTON

// ----------------------------------------------------------------------------
// wxTaskBarButton: define wxTaskBarButton interface.
// ----------------------------------------------------------------------------

/**
    State of the task bar button.
*/
enum wxTaskBarButtonState
{
    wxTASKBAR_BUTTON_NO_PROGRESS   = 0,
    wxTASKBAR_BUTTON_INDETERMINATE = 1,
    wxTASKBAR_BUTTON_NORMAL        = 2,
    wxTASKBAR_BUTTON_ERROR         = 4,
    wxTASKBAR_BUTTON_PAUSED        = 8
};

class WXDLLIMPEXP_ADV wxThumbBarButton {
public:
    wxThumbBarButton(int id,
                     const wxIcon& icon,
                     const wxString& tooltip = wxString());

    virtual ~wxThumbBarButton() {}

    int GetID() const { return m_id; }
    const wxIcon& GetIcon() const { return m_icon; }
    const wxString& GetTooltip() const { return m_tooltip; }

private:
    int m_id;
    wxIcon m_icon;
    wxString m_tooltip;
};

class WXDLLIMPEXP_ADV wxTaskBarButton
{
public:
    wxTaskBarButton() { }
    virtual ~wxTaskBarButton() { }

    // Operations:
    virtual void SetProgressValue(int value) = 0;
    virtual void Show(bool show = true) = 0;
    virtual void Hide() = 0;
    virtual void SetThumbnailTooltip(const wxString& tooltip) = 0;
    virtual void SetProgressState(wxTaskBarButtonState state) = 0;
    virtual void SetOverlayIcon(const wxIcon& icon,
                                const wxString& description = wxString()) = 0;
    virtual void SetThumbnailClip(const wxRect& rect) = 0;
    virtual void SetThumbnailContents(const wxWindow* child) = 0;

    /**
        Adds a thumbnail toolbar button to the thumbnail image of a window in
        the taskbar button flyout. Note that a wxTaskbarButton can only have no
        more than seven wxThumbBarButtons, and ShowThumbnailToolbar should be
        called to show them, then these buttons cannot be added or removed until
        the window is re-created.
    */
    virtual bool AddThumbBarButton(wxThumbBarButton *button) = 0;
    virtual void ShowThumbnailToolbar() = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxTaskBarButton);
};


#if defined(__WXMSW__)
    #include "wx/msw/taskbarbutton.h"
#endif

#endif // wxUSE_TASKBARBUTTON

#endif  // _WX_TASKBARBUTTON_H_
