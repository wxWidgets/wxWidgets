/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/ribboncontainerswinui.h
// Purpose:     Private wx model / native Toolkit container boundary
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_CONTAINERS_WINUI_H_
#define _WX_RIBBON_CONTAINERS_WINUI_H_

#if defined(WXWINUI_RIBBON_TOOLKIT)

#include "ribbontoolkitruntime.h"
#include "ribbonflyoutwinui.h"
#include "wx/weakref.h"
#include "wx/bmpbndl.h"
#include "wx/font.h"
#include <array>
#include <vector>

class wxRibbonControl;
class wxRibbonBar;
class wxRibbonPage;
class wxRibbonPanel;
class wxEventFilter;
struct wxWinUIDirectCompositeMount;
struct wxRibbonCommandLayout;

// This adapter is deliberately absent from installed headers. The wx model
// owns selection and command semantics; the native tree owns arrangement.
// Groups must be direct Ribbon.Items entries: wrapping them breaks the
// Toolkit panel's adaptive layout (which queries RibbonCollapsibleGroup).
class wxRibbonContainerWinUI final
    : public std::enable_shared_from_this<wxRibbonContainerWinUI>
{
public:
    static bool Create(wxRibbonControl* owner);
    static WXDLLIMPEXP_RIBBON bool Native(const wxRibbonControl* owner);
    static bool Realize(wxRibbonControl* owner);
    static bool Layout(wxRibbonPanel* owner);
    static void Sync(wxRibbonControl* owner);
    static void SetTabMargins(wxRibbonBar* owner, int left, int right);
    static void NotifyFocusChange(wxRibbonControl* owner);
    static void Close(wxRibbonControl* owner);
    static wxSize BestSize(wxRibbonControl* owner);
    static bool ScrollPage(wxRibbonPage* owner, double pixels);
    static bool ScrollPageSections(wxRibbonPage* owner, int sections);
    static bool IsMinimised(const wxRibbonPanel* owner);
    static bool IsExtensionHovered(const wxRibbonPanel* owner);
    static bool ShowExpanded(wxRibbonPanel* owner);
    static bool HideExpanded(wxRibbonPanel* owner);

    // A pending native template is not permission to create an independent
    // canvas slot. The caller retries Realize after the group's Loaded event.
    enum class MountStatus { Ordinary, Pending, Ready, Failed };
    static WXDLLIMPEXP_RIBBON MountStatus CommandMount(
        wxRibbonControl* command, wxRibbonControl*& compositeOwner,
        winrt::Microsoft::UI::Xaml::Controls::ContentControl& mount);

private:
    explicit wxRibbonContainerWinUI(wxRibbonControl* owner);
    bool Initialize();
    bool Synchronize();
    bool LayoutCommands();
    bool SynchronizePageLayout();
    bool InstallPageTemplate(const winrt::Microsoft::UI::Xaml::Controls::ControlTemplate& target);
    void QueuePageLayout();
    void NotifyOwnerPageLayout();
    bool AdaptVerticalPage();
    bool PreparePageScroll();
    bool PageScrollIsCurrent() const;
    void OnPageViewChanged(bool intermediate);
    void OnCommandsArranged(const winrt::Windows::Foundation::Size& size);
    void Retire();
    void OnLoaded();
    void OnSelectionChanged();
    bool OnTabPressed(wxRibbonPage* page,
                      const winrt::Microsoft::UI::Xaml::Controls::TabViewItem& item);
    void QueueBarFocusCheck();
    void CheckBarFocus();
    void OnBarButton(bool toggle);
    void OnPanelButton(bool popup);
    bool PreparePanelButtons();
    bool PrepareFlyout();
    bool PrepareMinimisedIcon(const wxRibbonFlyoutParts& parts);
    bool BeginExpansion();
    bool RestoreExpansion();
    void OnFlyoutOpening();
    void OnFlyoutClosed();
    void OnNativePanelState();
    void RequestFlyoutClose();
    wxRibbonControl* Owner() const;

    struct Icon
    {
        wxBitmapBundle bundle;
        wxSize size;
        winrt::Microsoft::UI::Xaml::Controls::ImageIconSource source{nullptr};
    };
    static bool UpdateIcon(Icon& icon, const wxBitmapBundle& bundle,
                           const wxSize& size);
    struct Tab
    {
        wxWeakRef<wxRibbonPage> page;
        winrt::Microsoft::UI::Xaml::Controls::TabViewItem item{nullptr};
        wxString label;
        Icon icon;
        wxFont labelFont;
    };
    struct Command
    {
        wxWeakRef<wxRibbonControl> owner;
        winrt::Microsoft::UI::Xaml::Controls::ContentControl mount{nullptr};
    };
    struct Footer
    {
        winrt::Microsoft::UI::Xaml::Controls::ControlTemplate controlTemplate{nullptr};
        winrt::Microsoft::UI::Xaml::Controls::Panel parent{nullptr};
        winrt::Microsoft::UI::Xaml::Controls::Grid root{nullptr};
        winrt::Microsoft::UI::Xaml::Controls::TextBlock label{nullptr};
        winrt::Microsoft::UI::Xaml::Controls::ContentControl state{nullptr};
        winrt::Microsoft::UI::Xaml::Controls::Button button{nullptr};
        winrt::event_token click{};
    };
    bool FooterIsCurrent(const Footer& footer, bool popup) const;
    std::array<Footer, 2> m_footers;

    wxWeakRef<wxRibbonControl> m_owner;
    wxWinUIControlHost m_host;
    std::shared_ptr<wxRibbonToolkitRuntime> m_runtime;
    std::shared_ptr<wxWinUIDirectCompositeMount> m_mount;
    winrt::Microsoft::UI::Xaml::FrameworkElement m_root{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::TabView m_tabs{nullptr};
    wxSize m_tabMargins{0, 0}; // wx pixel units; convert on every synchronization
    winrt::Microsoft::UI::Xaml::Controls::Button m_toggleButton{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::Button m_helpButton{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::FontIcon m_toggleIcon{nullptr};
    winrt::event_token m_toggleToken{};
    winrt::event_token m_helpToken{};
    winrt::event_token m_focusGotToken{};
    winrt::event_token m_focusLostToken{};
    std::shared_ptr<wxEventFilter> m_focusFilter;
    bool m_barFocusCheckQueued = false;
    winrt::Microsoft::UI::Xaml::Controls::Grid m_pages{nullptr};
    winrt::XamlToolkit::Labs::WinUI::Ribbon m_ribbon{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ScrollViewer m_pageScroll{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ControlTemplate m_pageScrollTemplate{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ControlTemplate m_horizontalPageTemplate{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ControlTemplate m_verticalPageTemplate{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ControlTemplate m_emptyPageTemplate{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::Panel m_pagePanel{nullptr};
    winrt::event_token m_pageScrollToken{};
    winrt::event_token m_pageSizeToken{};
    winrt::event_token m_pagePanelSizeToken{};
    winrt::event_token m_pageItemsToken{};
    bool m_pageVertical = false;
    bool m_pageAxisKnown = false;
    bool m_pageLayoutQueued = false;
    bool m_pageLayoutTransaction = false;
    bool m_pageLayoutDeferred = false;
    double m_pageScrollOffset = 0;
    unsigned long long m_pageScrollRevision = 0;
    bool m_pageScrollPending = false;
    bool m_pageScrollIssuing = false;
    winrt::XamlToolkit::Labs::WinUI::RibbonGroup m_group{nullptr};
    Icon m_minimisedIcon;
    winrt::Microsoft::UI::Xaml::Controls::IconElement m_minimisedImage{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::ImageIconSource m_minimisedImageSource{nullptr};
    winrt::Microsoft::UI::Xaml::Controls::Grid m_commands{nullptr};
    std::shared_ptr<wxRibbonCommandLayout> m_commandLayout;
    std::vector<Tab> m_tabModels;
    std::vector<Command> m_commandModels;
    // An expanded panel is a real wx child of the dummy. Its native content
    // remains in the dummy's exact Toolkit template, so command slots retain
    // their original composite owner, mounts, generations and providers.
    std::weak_ptr<wxRibbonContainerWinUI> m_proxyFor;
    wxWeakRef<wxRibbonPanel> m_expanded;
    std::shared_ptr<wxRibbonFlyoutParts> m_flyoutParts;
    std::shared_ptr<wxWinUIPhysicalDisconnectGate> m_panelGate;
    std::shared_ptr<wxWinUIFlyoutPopupRetirementState> m_flyoutSession;
    winrt::event_token m_openingToken{};
    winrt::event_token m_closedToken{};
    int64_t m_nativeStateToken = 0;
    bool m_nativeStateHooked = false;
    bool m_expansionTransaction = false;
    bool m_restoringExpansion = false;
    bool m_nativeStateWorkQueued = false;
    bool m_requestingFlyoutClose = false;
    winrt::event_token m_loadedToken{};
    winrt::event_token m_selectionToken{};
    bool m_synchronizing = false;
    bool m_syncDeferred = false;
    bool m_syncQueued = false;
    bool m_panelExtensionsKnown = false;
    bool m_panelExtensionsShown = false;
    bool m_hasPanelExtension = false;
    bool m_arrangingCommands = false;
    bool m_commandLayoutQueued = false;
    bool m_commandLayoutDeferred = false;
    bool m_published = false;
    bool m_loadedWorkQueued = false;
    bool m_retired = false;
};

#endif // WXWINUI_RIBBON_TOOLKIT
#endif // _WX_RIBBON_CONTAINERS_WINUI_H_
