/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/statusbar.h
// Purpose:     wxWinUI wxStatusBar declaration
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_STATUSBAR_H_
#define _WX_WINUI_STATUSBAR_H_

#if wxUSE_STATUSBAR

#include <memory>

class wxWinUIStatusBarImpl;
class wxDPIChangedEvent;
struct wxWinUIAppearanceSnapshot;
class wxStatusBar;

enum class wxWinUIStatusBarReentryPointForTesting
{
    RebuildLoaded,
    TextValue,
    AppearanceRootFont,
    DPIBorderX,
    MinHeightBeforeResize
};

using wxWinUIStatusBarReentryHookForTesting =
    void (*)(wxStatusBar *statusBar, void *context);

struct wxWinUIStatusBarSizeGripSnapshot
{
    double widthDips = 0.0;
    double heightDips = 0.0;
    int horizontalAlignment = 0;
    int cursorShape = 0;
    int nativeHitTest = 0;
    wxString automationName;
    wxString localizedControlType;
    bool overlaysFields = false;
    bool reservesFieldSpace = false;
    double fieldReservationDips = 0.0;
};

using wxWinUIStatusBarResizeActionHookForTesting =
    bool (*)(wxStatusBar *statusBar,
             void *nativeWindow,
             int nativeHitTest,
             long screenX,
             long screenY,
             void *context);

// A status bar projected into the shared WinUI island. Fields retain their wx
// pixel geometry while their XAML columns, theme borders and text live in DIPs.
class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar();
    wxStatusBar(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    ~wxStatusBar() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    // base class overrides
    void SetFieldsCount(int number = 1, const int *widths = nullptr) override;
    void SetStatusWidths(int n, const int widths[]) override;
    void SetStatusStyles(int n, const int styles[]) override;

    bool GetFieldRect(int i, wxRect& rect) const override;
    void SetMinHeight(int height) override;

    int GetBorderX() const override { return m_borderX; }
    int GetBorderY() const override { return m_borderY; }

    bool SetFont(const wxFont& font) override;
    bool SetForegroundColour(const wxColour& colour) override;
    bool SetBackgroundColour(const wxColour& colour) override;

    // Implementation-only observation of the XAML resize affordance created
    // for wxSTB_SIZEGRIP.
    bool WinUIHasSizeGripForTesting() const;
    bool WinUIGetSizeGripStateForTesting(
        wxWinUIStatusBarSizeGripSnapshot *snapshot) const;
    void WinUISetResizeActionHookForTesting(
        wxWinUIStatusBarResizeActionHookForTesting hook,
        void *context);
    bool WinUIInvokeSizeGripForTesting(
        bool isMouse,
        bool isPrimary,
        bool isLeftButtonPressed,
        const wxPoint& screenPoint);
    void WinUISetTopLevelMaximizedForTesting(bool maximized);
    bool WinUIGetFieldStateForTesting(
        int field,
        wxString *renderedText,
        int *textTrimming,
        int *fieldStyle,
        bool *hasToolTip,
        double *columnValue,
        int *columnUnitType,
        wxString *automationName,
        int *borderElementCount = nullptr,
        int *fieldGridFlowDirection = nullptr,
        int *textFlowDirection = nullptr) const;
    bool WinUIGetAppearanceForTesting(
        wxWinUIAppearanceSnapshot *snapshot) const;
    bool WinUIUsesThemeBordersForTesting() const;
    void WinUISetNextReentryHookForTesting(
        wxWinUIStatusBarReentryPointForTesting point,
        wxWinUIStatusBarReentryHookForTesting hook,
        void *context);
    unsigned long long WinUIGetModelRevisionForTesting() const;
    bool WinUIHasDeferredRebuildForTesting() const;
    bool WinUIIsRebuildQuarantinedForTesting() const;
    // Invoke the implementation handler without the outer wxEvtHandler
    // dispatcher. This is the only valid way for the lifetime seam to delete
    // the status bar synchronously: deleting an event handler while
    // ProcessWindowEvent() is still walking its tables is outside wx's event
    // contract and would test the dispatcher rather than this implementation.
    void WinUIDeliverDPIChangedForTesting(wxDPIChangedEvent& event);

protected:
    void DoUpdateStatusText(int number) override;
    wxSize DoGetBestSize() const override;

private:
    // Project the complete status-bar model as one transaction. Text,
    // trimming, UIA, tooltips and appearance deliberately share the same
    // revision so a cross-domain re-entrant mutation can never leave a
    // partially updated peer behind.
    bool RebuildContent();
    wxString GetDisplayedStatusText(int field,
                                    bool *ellipsized,
                                    int *trimming) const;
    void OnSize(wxSizeEvent& event);
    void OnDPIChanged(wxDPIChangedEvent& event);
    // absolute field widths in pixels for the current client width
    wxArrayInt GetAbsWidths() const;
    int GetSizeGripReservedWidth() const;
    int GetMinHeightPixels() const;
    void ApplyMinHeight(double heightDIPs, int heightPixels);

    std::unique_ptr<wxWinUIStatusBarImpl> m_winui;
    int m_borderX = 0;
    int m_borderY = 0;
    // Keep the caller's logical request independent of the monitor on which
    // it was made. -1 means SetMinHeight() has never imposed a minimum.
    double m_minHeightDIPs = -1.0;

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStatusBar);
};

#endif // wxUSE_STATUSBAR

#endif // _WX_WINUI_STATUSBAR_H_
