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

#include "wx/defs.h"

// ----------------------------------------------------------------------------
// wxTaskBarButton: define wxTaskBarButton interface.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxTaskBarButton;

/**
    State of the task bar button.
*/
enum WXDLLIMPEXP_CORE wxTaskBarButtonState
{
    wxTASKBAR_BUTTON_NO_PROGRESS   = 0,
    wxTASKBAR_BUTTON_INDETERMINATE = 1,
    wxTASKBAR_BUTTON_NORMAL        = 2,
    wxTASKBAR_BUTTON_ERROR         = 4,
    wxTASKBAR_BUTTON_PAUSED        = 8
};

class WXDLLIMPEXP_CORE wxThumbBarButton : public wxObject {
public:
    wxThumbBarButton() : m_taskBarButtonParent(NULL) { }
    wxThumbBarButton(int id,
                     const wxIcon& icon,
                     const wxString& tooltip = wxString(),
                     bool enable = true,
                     bool dismissOnClick = false,
                     bool hasBackground = true,
                     bool shown = true,
                     bool interactive = true);

    virtual ~wxThumbBarButton() {}

    bool Create(int id,
                const wxIcon& icon,
                const wxString& tooltip = wxString(),
                bool enable = true,
                bool dismissOnClick = false,
                bool hasBackground = true,
                bool shown = true,
                bool interactive = true);
    int GetID() const { return m_id; }
    const wxIcon& GetIcon() const { return m_icon; }
    const wxString& GetTooltip() const { return m_tooltip; }

    bool IsEnable() const { return m_enable; }
    void Enable(bool enable = true);
    void Disable() { Enable(false); }

    bool IsDismissOnClick() const { return m_dismissOnClick; }
    void EnableDismissOnClick(bool enable = true);
    void DisableDimissOnClick() { EnableDismissOnClick(false); }

    bool HasBackground() const { return m_hasBackground; }
    void SetHasBackground(bool has = true);

    bool IsShown() const { return m_shown; }
    void Show(bool shown = true);
    void Hide() { Show(false); }

    bool IsInteractive() const { return m_interactive; }
    void SetInteractive(bool interactive = true);

    void SetParent(wxTaskBarButton *parent) { m_taskBarButtonParent = parent; }
    wxTaskBarButton* GetParent() const { return m_taskBarButtonParent; }

private:
    bool UpdateParentTaskBarButton();

    int m_id;
    wxIcon m_icon;
    wxString m_tooltip;
    bool m_enable;
    bool m_dismissOnClick;
    bool m_hasBackground;
    bool m_shown;
    bool m_interactive;
    wxTaskBarButton *m_taskBarButtonParent;

    DECLARE_DYNAMIC_CLASS(wxThumbBarButton)
};

class WXDLLIMPEXP_CORE wxTaskBarButton
{
public:
    wxTaskBarButton() { }
    virtual ~wxTaskBarButton() { }

    // Operations:
    virtual void SetProgressRange(int range) = 0;
    virtual void SetProgressValue(int value) = 0;
    virtual void PulseProgress() = 0;
    virtual void Show(bool show = true) = 0;
    virtual void Hide() = 0;
    virtual void SetThumbnailTooltip(const wxString& tooltip) = 0;
    virtual void SetProgressState(wxTaskBarButtonState state) = 0;
    virtual void SetOverlayIcon(const wxIcon& icon,
                                const wxString& description = wxString()) = 0;
    virtual void SetThumbnailClip(const wxRect& rect) = 0;
    virtual void SetThumbnailContents(const wxWindow *child) = 0;
    virtual bool InsertThumbBarButton(size_t pos, wxThumbBarButton *button) = 0;
    virtual bool AppendThumbBarButton(wxThumbBarButton *button) = 0;
    virtual bool AppendSeparatorInThumbBar() = 0;
    virtual wxThumbBarButton* RemoveThumbBarButton(wxThumbBarButton *button) = 0;
    virtual wxThumbBarButton* RemoveThumbBarButton(int id) = 0;

private:
    wxDECLARE_NO_COPY_CLASS(wxTaskBarButton);
};


#if defined(__WXMSW__)
    #include "wx/msw/taskbarbutton.h"
#endif

#endif // wxUSE_TASKBARBUTTON

#endif  // _WX_TASKBARBUTTON_H_
