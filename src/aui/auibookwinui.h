/////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/auibookwinui.h
// Purpose:     Source-private native AUI tab-strip projection
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUI_BOOK_WINUI_PRIVATE_H_
#define _WX_AUI_BOOK_WINUI_PRIVATE_H_

#include <functional>
#include <memory>

class wxAuiTabCtrl;
class wxAuiTabContainer;
class wxWindow;

// The pane's wxAuiTabContainer remains the only page/order/selection model.
// The TabView contains headers only: page HWNDs and cross-pane transactions
// continue to belong to wxAuiNotebook and wxAuiManager.
class wxAuiTabCtrlWinUI final
    : public std::enable_shared_from_this<wxAuiTabCtrlWinUI>
{
public:
    using Command = std::function<void(wxAuiTabCtrl*, wxWindow*, int)>;
    static std::shared_ptr<wxAuiTabCtrlWinUI> Create(
        wxAuiTabCtrl* owner, Command command);
    ~wxAuiTabCtrlWinUI();

    void Close();
    void Invalidate();
    void Update();
    void SynchronizeGeometry();
    void ShowTab(int index);
    bool IsTabVisible(int index);
    bool IsActive() const;
    static bool CanPresent(const wxAuiTabCtrl* owner);
    static int MinimumHeight(const wxAuiTabContainer& tabs, wxWindow* owner);

private:
    struct Impl;
    explicit wxAuiTabCtrlWinUI(wxAuiTabCtrl* owner, Command command);
    bool Initialize();
    void Dispatch(wxWindow* page, int command);
    void ClearVisibilityRequest();
    void ContinueVisibilityRequest();
    std::unique_ptr<Impl> m_impl;
};

#endif
