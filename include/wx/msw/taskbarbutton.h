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

#include "wx/defs.h"

#if wxUSE_TASKBARBUTTON

#include "wx/vector.h"
#include "wx/taskbarbutton.h"

class WXDLLIMPEXP_FWD_CORE wxITaskbarList3;

class WXDLLIMPEXP_CORE wxTaskBarButtonImpl : public wxTaskBarButton
{
public:
    virtual ~wxTaskBarButtonImpl();

    virtual void SetProgressRange(int range) override;
    virtual void SetProgressValue(int value) override;
    virtual void PulseProgress() override;
    virtual void Show(bool show = true) override;
    virtual void Hide() override;
    virtual void SetThumbnailTooltip(const wxString& tooltip) override;
    virtual void SetProgressState(wxTaskBarButtonState state) override;
    virtual void SetOverlayIcon(const wxIcon& icon,
        const wxString& description = wxString()) override;
    virtual void SetThumbnailClip(const wxRect& rect) override;
    virtual void SetThumbnailContents(const wxWindow *child) override;
    virtual bool InsertThumbBarButton(size_t pos,
                                      wxThumbBarButton *button) override;
    virtual bool AppendThumbBarButton(wxThumbBarButton *button) override;
    virtual bool AppendSeparatorInThumbBar() override;
    virtual wxThumbBarButton* RemoveThumbBarButton(
        wxThumbBarButton *button) override;
    virtual wxThumbBarButton* RemoveThumbBarButton(int id) override;
    wxThumbBarButton* GetThumbBarButtonByIndex(size_t index);
    bool InitOrUpdateThumbBarButtons();
    virtual void Realize() override;

private:
    // This ctor is only used by wxTaskBarButton::New()
    wxTaskBarButtonImpl(wxITaskbarList3* taskbarList, wxWindow* parent);

    wxWindow* m_parent;
    wxITaskbarList3 *m_taskbarList;

    typedef wxVector<wxThumbBarButton*> wxThumbBarButtons;
    wxThumbBarButtons m_thumbBarButtons;

    int m_progressRange;
    int m_progressValue;
    wxTaskBarButtonState m_progressState;
    wxString m_thumbnailTooltip;
    wxIcon m_overlayIcon;
    wxString m_overlayIconDescription;
    wxRect m_thumbnailClipRect;
    bool m_hasInitThumbnailToolbar;

    friend wxTaskBarButton* wxTaskBarButton::New(wxWindow*);

    wxDECLARE_NO_COPY_CLASS(wxTaskBarButtonImpl);
};

#endif // wxUSE_TASKBARBUTTON

#endif  // _WX_MSW_TASKBARBUTTON_H_
