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

#include "wx/taskbarbutton.h"

#include <memory>
#include <vector>

class WXDLLIMPEXP_FWD_CORE wxMSWTaskBarButtonNativeBackend;
struct wxMSWTaskBarButtonSnapshot;
struct wxMSWTaskBarThumbButtonNative;
struct wxTaskBarButtonStateData;

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
    // The native command id identifies one of the seven immutable shell slots.
    // It resolves to the button currently occupying that visual slot.
    wxThumbBarButton* GetThumbBarButtonByIndex(size_t index);
    bool InitOrUpdateThumbBarButtons();
    virtual void Realize() override;

    bool IsAvailable() const;
    bool GetSnapshotForTesting(wxMSWTaskBarButtonSnapshot* snapshot) const;

private:
    // This ctor is only used by wxTaskBarButton::New()
    wxTaskBarButtonImpl(
        const std::shared_ptr<wxMSWTaskBarButtonNativeBackend>& backend,
        wxWindow* parent,
        WXHWND hwnd,
        unsigned long long hwndGeneration);

    std::vector<wxMSWTaskBarThumbButtonNative>
        BuildNativeThumbButtons() const;
    bool Rebind();
    bool GetExactIdentity(WXHWND* hwnd,
                          unsigned long long* hwndGeneration) const;

    typedef std::vector<wxThumbBarButton*> wxThumbBarButtons;
    wxThumbBarButtons m_thumbBarButtons;

    std::shared_ptr<wxTaskBarButtonStateData> m_state;

    friend wxTaskBarButton* wxTaskBarButton::New(wxWindow*);
    friend class wxFrame;

    wxDECLARE_NO_COPY_CLASS(wxTaskBarButtonImpl);
};

#endif // wxUSE_TASKBARBUTTON

#endif  // _WX_MSW_TASKBARBUTTON_H_
