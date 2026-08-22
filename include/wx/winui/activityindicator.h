/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/activityindicator.h
// Purpose:     wxWinUI wxActivityIndicator declaration (WinUI ProgressRing)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_ACTIVITYINDICATOR_H_
#define _WX_WINUI_ACTIVITYINDICATOR_H_

#include <memory>

class wxWinUIActivityIndicatorImpl;
struct wxWinUIAppearanceSnapshot;

using wxWinUIActivityPeerWriteHookForTesting = void (*)(void *);

class WXDLLIMPEXP_CORE wxActivityIndicator : public wxActivityIndicatorBase
{
public:
    wxActivityIndicator();
    explicit
    wxActivityIndicator(wxWindow* parent,
                        wxWindowID winid = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxActivityIndicatorNameStr);
    ~wxActivityIndicator() override;

    bool Create(wxWindow* parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxActivityIndicatorNameStr);

    void Start() override;
    void Stop() override;
    bool IsRunning() const override;

    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;

    bool WinUIGetStateForTesting(
        bool *peerActive,
        bool *isTabStop,
        wxString *itemStatus,
        wxWinUIAppearanceSnapshot *appearance) const;
    // One-shot deterministic seam invoked after the next peer write.
    void WinUISetNextPeerWriteHookForTesting(
        wxWinUIActivityPeerWriteHookForTesting hook,
        void *context);
    bool WinUIHasDeferredPeerWriteForTesting() const;
    bool WinUIIsPeerProjectionQuarantinedForTesting() const;
    unsigned long long WinUIGetModelRevisionForTesting() const;

protected:
    wxSize DoGetBestSize() const override;
    bool ApplyWinUIModel();

    std::unique_ptr<wxWinUIActivityIndicatorImpl> m_winui;
    bool m_running = false;

private:
    void BumpWinUIModelRevision();
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxActivityIndicator);
};

#endif // _WX_WINUI_ACTIVITYINDICATOR_H_
