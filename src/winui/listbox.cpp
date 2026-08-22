/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/listbox.cpp
// Purpose:     wxWinUI wxListBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_LISTBOX

#include "wx/listbox.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/arrstr.h"
    #include "wx/bitmap.h"
    #include "wx/brush.h"
    #include "wx/colour.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
#endif

#include "private.h"
#include "wx/weakref.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"

#if wxUSE_OWNER_DRAWN
    #include "wx/msw/private/listboxitem.h"
#endif

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.Storage.Streams.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXCP = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace MUXD = winrt::Microsoft::UI::Dispatching;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace MUXMI = winrt::Microsoft::UI::Xaml::Media::Imaging;
namespace WF = winrt::Windows::Foundation;

namespace
{

// The native MSW owner-drawn ListBox adds one physical pixel above and below
// the control font. Non-owner-drawn HWND list boxes derive the same compact
// line metric from their font instead of imposing a DPI-independent minimum.
constexpr int wxWinUIListBoxExtraSpacePixels = 1;

// Different projected interfaces for the same WinRT object can have distinct
// ABI pointers. Always compare the controlling IUnknown identity instead of
// comparing whichever interface pointer happens to be held by the wrapper.
template <typename T>
WF::IUnknown wxWinUIListObjectIdentity(const T& object) noexcept
{
    return object ? object.template try_as<WF::IUnknown>() : nullptr;
}

// COM interface giving direct access to an IBuffer's bytes. This is used only
// by the deterministic presentation seam below to inspect the bitmap that is
// actually attached to the XAML Image.
struct __declspec(uuid("905a0fef-bc53-11df-8c49-001e4fc686da"))
wxWinUIListBufferByteAccess : ::IUnknown
{
    virtual HRESULT __stdcall Buffer(std::uint8_t** value) = 0;
};

class wxWinUIPeerMutationGuard final
{
public:
    explicit wxWinUIPeerMutationGuard(bool& flag)
        : m_flag(flag),
          m_oldValue(flag)
    {
        m_flag = true;
    }

    ~wxWinUIPeerMutationGuard()
    {
        m_flag = m_oldValue;
    }

private:
    bool& m_flag;
    const bool m_oldValue;
};

MUXC::ScrollViewer wxWinUIFindListScrollViewer(
    const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;

    if ( const auto scroll = root.try_as<MUXC::ScrollViewer>() )
        return scroll;

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n < count; ++n )
    {
        if ( const auto scroll = wxWinUIFindListScrollViewer(
                 MUXM::VisualTreeHelper::GetChild(root, n)) )
        {
            return scroll;
        }
    }

    return nullptr;
}

MUXC::ScrollViewer wxWinUIGetListPanelScrollViewer(
    const MUXC::ListView& listView,
    const MUXC::Panel& panel,
    bool *resolvedFromVisualAncestor = nullptr)
{
    if ( resolvedFromVisualAncestor )
        *resolvedFromVisualAncestor = false;

    if ( !listView || !panel )
        return nullptr;

    if ( const auto virtualizing =
             panel.try_as<MUXCP::IOrientedVirtualizingPanel>() )
    {
        const WF::IInspectable owner = virtualizing.ScrollOwner();
        if ( const auto scroll = owner.try_as<MUXC::ScrollViewer>() )
            return scroll;

        if ( const auto presenter =
                 owner.try_as<MUXC::ScrollContentPresenter>() )
        {
            if ( const auto scroll =
                     presenter.ScrollOwner().try_as<MUXC::ScrollViewer>() )
            {
                return scroll;
            }
        }
    }

    // ItemsStackPanel is ListView's native, pixel-virtualizing panel. Unlike
    // VirtualizingStackPanel it derives directly from Panel and deliberately
    // exposes no IOrientedVirtualizingPanel::ScrollOwner. Its nearest visual
    // ScrollContentPresenter ancestor is nevertheless the object which
    // measures this exact panel, and that presenter publishes the inner
    // ScrollViewer through ScrollOwner.
    //
    // Stop at ListView itself. This makes the lookup authoritative without a
    // DFS and prevents climbing into the unrelated shared-host ScrollViewer
    // which contains the whole control.
    const WF::IUnknown listIdentity = wxWinUIListObjectIdentity(listView);
    MUX::DependencyObject node = panel;
    for ( unsigned int depth = 0; depth < 64; ++depth )
    {
        node = MUXM::VisualTreeHelper::GetParent(node);
        if ( !node || wxWinUIListObjectIdentity(node) == listIdentity )
            break;

        if ( const auto presenter =
                 node.try_as<MUXC::ScrollContentPresenter>() )
        {
            if ( const auto scroll =
                     presenter.ScrollOwner().try_as<MUXC::ScrollViewer>() )
            {
                if ( resolvedFromVisualAncestor )
                    *resolvedFromVisualAncestor = true;
                return scroll;
            }
        }
    }

    return nullptr;
}

MUXC::ScrollViewer wxWinUIGetListScrollViewer(
    const MUXC::ListView& listView,
    bool *resolvedFromVisualAncestor = nullptr)
{
    if ( resolvedFromVisualAncestor )
        *resolvedFromVisualAncestor = false;

    if ( !listView )
        return nullptr;

    // ListView can contain more than one ScrollViewer. Resolve only the owner
    // on the realized ItemsPanelRoot chain; a DFS can select the outer shared
    // host viewer whose viewport follows the control but whose extent never
    // measures the rows.
    if ( const MUXC::Panel panel = listView.ItemsPanelRoot() )
    {
        return wxWinUIGetListPanelScrollViewer(
            listView, panel, resolvedFromVisualAncestor);
    }

    // Before ItemsPanelRoot/its presenter exists there is no authoritative
    // extent target yet. Do not mutate a DFS-selected outer viewer: Loaded and
    // SizeChanged refresh the rows once the panel can identify its owner.
    return nullptr;
}

MUXC::ItemsPresenter wxWinUIFindListItemsPresenter(
    const MUX::DependencyObject& root)
{
    if ( !root )
        return nullptr;

    if ( const auto presenter = root.try_as<MUXC::ItemsPresenter>() )
        return presenter;

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n < count; ++n )
    {
        if ( const auto presenter = wxWinUIFindListItemsPresenter(
                 MUXM::VisualTreeHelper::GetChild(root, n)) )
        {
            return presenter;
        }
    }

    return nullptr;
}

MUXC::ScrollContentPresenter wxWinUIFindListScrollContentPresenter(
    const MUX::DependencyObject& root,
    const MUXC::ScrollViewer& expectedOwner)
{
    if ( !root || !expectedOwner )
        return nullptr;

    if ( const auto presenter =
             root.try_as<MUXC::ScrollContentPresenter>() )
    {
        if ( wxWinUIListObjectIdentity(presenter.ScrollOwner()) ==
             wxWinUIListObjectIdentity(expectedOwner) )
        {
            return presenter;
        }
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n < count; ++n )
    {
        if ( const auto presenter =
                 wxWinUIFindListScrollContentPresenter(
                     MUXM::VisualTreeHelper::GetChild(root, n),
                     expectedOwner) )
        {
            return presenter;
        }
    }

    return nullptr;
}

MUX::FrameworkElement wxWinUIGetListScrollContent(
    const MUXC::ScrollViewer& scroll)
{
    if ( !scroll )
        return nullptr;

    // ScrollViewer::ExtentWidth is computed from its Content, not from an
    // arbitrary ItemsPresenter elsewhere in the ListView template. In
    // particular, a DFS rooted at ListView can resolve a presenter owned by a
    // header/template branch which never participates in this ScrollViewer's
    // measure pass.
    return scroll.Content().try_as<MUX::FrameworkElement>();
}

void wxWinUISetListExtentElement(
    const MUX::FrameworkElement& element,
    double width)
{
    if ( !element )
        return;

    element.HorizontalAlignment(MUX::HorizontalAlignment::Left);
    element.MinWidth(width);
    element.Width(width);
}

void wxWinUIResetListExtentElement(
    const MUX::FrameworkElement& element)
{
    if ( !element )
        return;

    element.ClearValue(MUX::FrameworkElement::WidthProperty());
    element.MinWidth(0.0);
    element.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
}

void wxWinUIToggleCheckBox(const MUXC::CheckBox& checkBox)
{
    if ( !checkBox )
        return;

    const auto current = checkBox.IsChecked();
    checkBox.IsChecked(!(current && current.Value()));
}

bool wxWinUIRemoveListChildFromPanel(const MUX::UIElement& child,
                                     const MUXC::Panel& panel)
{
    if ( child && panel )
    {
        const auto children = panel.Children();
        const auto childIdentity = wxWinUIListObjectIdentity(child);
        for ( uint32_t n = 0; n < children.Size(); ++n )
        {
            if ( wxWinUIListObjectIdentity(children.GetAt(n)) == childIdentity )
            {
                children.RemoveAt(n);
                return true;
            }
        }
    }
    return false;
}

void wxWinUIDetachListChild(const MUX::UIElement& child)
{
    if ( !child )
        return;

    const MUX::DependencyObject parent =
        MUXM::VisualTreeHelper::GetParent(child);
    (void)wxWinUIRemoveListChildFromPanel(
        child, parent.try_as<MUXC::Panel>());
}

double wxWinUIListScale(const wxListBox *owner)
{
    double scale = owner ? owner->GetDPIScaleFactor() : 1.0;
    if ( !std::isfinite(scale) || scale <= 0.0 )
        scale = 1.0;
    return scale;
}

void wxWinUIApplyListHorizontalExtent(
    const MUXC::ListView& listView,
    const MUXC::ListViewItem& item,
    double contentWidthDips)
{
    if ( !listView || !item )
        return;

    double width = wxMax(1.0, contentWidthDips);
    const MUXC::ScrollViewer scroll =
        wxWinUIGetListScrollViewer(listView);
    if ( scroll )
    {
        scroll.HorizontalScrollMode(MUXC::ScrollMode::Enabled);
        scroll.HorizontalScrollBarVisibility(
            MUXC::ScrollBarVisibility::Auto);
        scroll.HorizontalContentAlignment(
            MUX::HorizontalAlignment::Left);
        const double viewport = scroll.ViewportWidth();
        if ( std::isfinite(viewport) && viewport > 0.0 )
            width = wxMax(width, viewport);

        // The ScrollViewer delegates its extent calculation to the
        // ScrollContentPresenter created by its template. ListView's default
        // template sizes that presenter to its templated parent, so changing
        // only the ItemsPresenter/panel can never grow ExtentWidth. Update the
        // presenter that names this exact ScrollViewer as ScrollOwner: this
        // keeps nested/template ScrollViewers out of the transaction.
        if ( const MUXC::ScrollContentPresenter presenter =
                 wxWinUIFindListScrollContentPresenter(scroll, scroll) )
        {
            presenter.CanHorizontallyScroll(true);
            presenter.CanVerticallyScroll(true);
            presenter.SizesContentToTemplatedParent(false);
        }
    }
    else
    {
        const double actual = listView.ActualWidth();
        if ( std::isfinite(actual) && actual > 0.0 )
            width = wxMax(width, actual);
    }

    // ItemsStackPanel measures vertical ListViewItem containers against the
    // viewport when they remain stretched. An explicit left-aligned width is
    // therefore required for the retained row itself (not just its content)
    // to contribute the Windows horizontal extent to ScrollableWidth.
    item.HorizontalAlignment(MUX::HorizontalAlignment::Left);
    item.HorizontalContentAlignment(MUX::HorizontalAlignment::Left);
    item.MinWidth(width);
    item.Width(width);

    // ItemsStackPanel deliberately keeps item containers constrained for
    // virtualization and can report only the viewport as its desired width.
    // The panel is the ScrollViewer's measured child, so project the native
    // horizontal extent on it as well without replacing the pixel-virtualized
    // panel with a plain StackPanel.
    if ( const MUXC::Panel panel = listView.ItemsPanelRoot() )
    {
        // Retain support for an application theme supplying an oriented panel,
        // but ListView's native ItemsStackPanel uses the presenter ancestor
        // configured above instead of exposing these properties itself.
        if ( const auto virtualizing =
                 panel.try_as<MUXCP::IOrientedVirtualizingPanel>() )
        {
            virtualizing.CanHorizontallyScroll(true);
            virtualizing.CanVerticallyScroll(true);
        }
        wxWinUISetListExtentElement(panel, width);
    }

    // Project the extent onto the object actually measured by this
    // ScrollViewer, then onto its ItemsPresenter descendant. Keeping the
    // search rooted below ScrollViewer is essential: ListView templates can
    // contain other presenters which don't contribute to ExtentWidth.
    const MUX::FrameworkElement scrollContent =
        wxWinUIGetListScrollContent(scroll);
    wxWinUISetListExtentElement(scrollContent, width);
    const MUX::DependencyObject presenterRoot = scrollContent
        ? scrollContent.try_as<MUX::DependencyObject>()
        : scroll.try_as<MUX::DependencyObject>();
    if ( const MUXC::ItemsPresenter presenter =
             wxWinUIFindListItemsPresenter(presenterRoot) )
    {
        if ( wxWinUIListObjectIdentity(presenter) !=
             wxWinUIListObjectIdentity(scrollContent) )
        {
            wxWinUISetListExtentElement(presenter, width);
        }
    }
}

bool wxWinUIResetListHorizontalExtent(const MUXC::ListView& listView)
{
    if ( !listView )
        return false;

    try
    {
        // Width is normally Auto. Clear every explicit value on the same
        // measured chain used above instead of assigning the current
        // viewport, which would become stale on the next host resize.
        if ( const MUXC::Panel panel = listView.ItemsPanelRoot() )
            wxWinUIResetListExtentElement(panel);

        const MUXC::ScrollViewer scroll =
            wxWinUIGetListScrollViewer(listView);
        if ( !scroll )
            return false;

        const MUX::FrameworkElement scrollContent =
            wxWinUIGetListScrollContent(scroll);
        const MUX::DependencyObject presenterRoot = scrollContent
            ? scrollContent.try_as<MUX::DependencyObject>()
            : scroll.try_as<MUX::DependencyObject>();
        if ( const MUXC::ItemsPresenter presenter =
                 wxWinUIFindListItemsPresenter(presenterRoot) )
        {
            if ( wxWinUIListObjectIdentity(presenter) !=
                 wxWinUIListObjectIdentity(scrollContent) )
            {
                wxWinUIResetListExtentElement(presenter);
            }
        }
        wxWinUIResetListExtentElement(scrollContent);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox horizontal extent reset", e);
        return false;
    }
}

#if wxUSE_OWNER_DRAWN

using wxWinUIListBoxItem = wxListBoxItemBase<wxListBox>;

#endif // wxUSE_OWNER_DRAWN

} // anonymous namespace

// Delegates own this small state instead of the wx control. Close()
// invalidates it before revoking any token, so a queued callback can never
// dereference a destroyed wxListBox.
class wxWinUIListBoxCallbackState final
{
public:
    explicit wxWinUIListBoxCallbackState(wxListBox *owner)
        : m_owner(owner)
    {
    }

    wxListBox *GetOwner() const
    {
        return m_owner.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_owner.store(nullptr, std::memory_order_release);
    }

private:
    std::atomic<wxListBox *> m_owner;
};

class wxWinUIListItemCallbackState final
{
public:
    bool IsValid() const
    {
        return m_valid.load(std::memory_order_acquire);
    }

    void Invalidate()
    {
        m_valid.store(false, std::memory_order_release);
    }

private:
    std::atomic<bool> m_valid{ true };
};

class wxWinUIListBoxImpl
{
public:
    struct HitTestTicket
    {
        wxWinUIListBoxImpl *implementation = nullptr;
        std::shared_ptr<wxWinUIListBoxCallbackState> callbackState;
        std::uint64_t peerRevision = 0;
        MUXC::ListView listView{ nullptr };
        std::size_t peerItemCount = 0;
    };

    struct CheckBinding
    {
        MUXC::CheckBox checkBox{ nullptr };
        std::shared_ptr<wxWinUIListItemCallbackState> callbackState;
        winrt::event_token checkedToken{};
        winrt::event_token uncheckedToken{};
    };

    ~wxWinUIListBoxImpl()
    {
        Close();
    }

    void NotePeerItemsChanged()
    {
        if ( ++peerRevision == 0 )
            ++peerRevision;
    }

    HitTestTicket CaptureHitTestTicket() const
    {
        HitTestTicket ticket;
        ticket.implementation =
            const_cast<wxWinUIListBoxImpl *>(this);
        ticket.callbackState = callbackState;
        ticket.peerRevision = peerRevision;
        ticket.listView = listView;
        ticket.peerItemCount = peerItemIds.size();
        return ticket;
    }

    void RevokeCheckBinding(CheckBinding& binding)
    {
        if ( binding.callbackState )
            binding.callbackState->Invalidate();

        if ( !binding.checkBox )
        {
            binding.callbackState.reset();
            return;
        }

        if ( binding.checkedToken.value )
        {
            try
            {
                binding.checkBox.Checked(binding.checkedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CheckListBox Checked removal", e);
            }
        }
        binding.checkedToken = {};

        if ( binding.uncheckedToken.value )
        {
            try
            {
                binding.checkBox.Unchecked(binding.uncheckedToken);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI CheckListBox Unchecked removal", e);
            }
        }
        binding.uncheckedToken = {};
        binding.checkBox = nullptr;
        binding.callbackState.reset();
    }

    void RevokeCheckBinding(wxWinUIItemModel::Id id)
    {
        const auto it = checkBindings.find(id);
        if ( it == checkBindings.end() )
            return;

        RevokeCheckBinding(it->second);
        checkBindings.erase(it);
    }

    void Close()
    {
        if ( callbackState )
            callbackState->Invalidate();

        if ( listView )
        {
            if ( selectionChangedToken.value )
            {
                try
                {
                    listView.SelectionChanged(selectionChangedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox SelectionChanged removal", e);
                }
            }
            selectionChangedToken = {};

            if ( doubleTappedToken.value )
            {
                try
                {
                    listView.DoubleTapped(doubleTappedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox DoubleTapped removal", e);
                }
            }
            doubleTappedToken = {};

            if ( keyDownToken.value )
            {
                try
                {
                    listView.PreviewKeyDown(keyDownToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI CheckListBox KeyDown removal", e);
                }
            }
            keyDownToken = {};

            if ( sizeChangedToken.value )
            {
                try
                {
                    listView.SizeChanged(sizeChangedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox SizeChanged removal", e);
                }
            }
            sizeChangedToken = {};

            if ( loadedToken.value )
            {
                try
                {
                    listView.Loaded(loadedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox Loaded removal", e);
                }
            }
            loadedToken = {};

            if ( layoutUpdatedToken.value )
            {
                try
                {
                    listView.LayoutUpdated(layoutUpdatedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox LayoutUpdated removal", e);
                }
            }
            layoutUpdatedToken = {};

            if ( actualThemeChangedToken.value )
            {
                try
                {
                    listView.ActualThemeChanged(actualThemeChangedToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox ActualThemeChanged removal", e);
                }
            }
            actualThemeChangedToken = {};

            if ( gotFocusToken.value )
            {
                try
                {
                    listView.GotFocus(gotFocusToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox GotFocus removal", e);
                }
            }
            gotFocusToken = {};

            if ( lostFocusToken.value )
            {
                try
                {
                    listView.LostFocus(lostFocusToken);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox LostFocus removal", e);
                }
            }
            lostFocusToken = {};
        }

        for ( auto& entry : checkBindings )
            RevokeCheckBinding(entry.second);
        checkBindings.clear();

        host.Close();
        listView = nullptr;
        if ( !peerItemIds.empty() )
        {
            peerItemIds.clear();
            NotePeerItemsChanged();
        }
        peerItemsValid = false;
        callbackState.reset();
    }

    std::size_t FindPeerIndex(
        const winrt::Windows::Foundation::IInspectable& peer) const
    {
        if ( !listView || !peer || !peerItemsValid )
            return wxWinUIItemModel::npos;

        const auto peerIdentity = wxWinUIListObjectIdentity(peer);
        try
        {
            const auto items = listView.Items();
            const uint32_t count = items.Size();
            for ( uint32_t n = 0; n < count; ++n )
            {
                if ( wxWinUIListObjectIdentity(items.GetAt(n)) == peerIdentity )
                    return n;
            }
        }
        catch ( const winrt::hresult_error& )
        {
        }
        return wxWinUIItemModel::npos;
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIListBoxCallbackState> callbackState;
    MUXC::ListView listView{ nullptr };
    winrt::event_token selectionChangedToken{};
    winrt::event_token doubleTappedToken{};
    winrt::event_token keyDownToken{};
    winrt::event_token sizeChangedToken{};
    winrt::event_token loadedToken{};
    winrt::event_token layoutUpdatedToken{};
    winrt::event_token actualThemeChangedToken{};
    winrt::event_token gotFocusToken{};
    winrt::event_token lostFocusToken{};
    std::vector<wxWinUIItemModel::Id> peerItemIds;
    std::uint64_t peerRevision = 1;
    bool peerItemsValid = true;
    bool horizontalPresentationPending = false;
    bool horizontalMaterializationInProgress = false;
    bool horizontalMaterializationRetryQueued = false;
    unsigned int horizontalMaterializationAttempts = 0;
    unsigned int horizontalMaterializationRetriesRemaining = 1;
    bool horizontalResetPending = false;
    bool creatingOwnerItem = false;
    bool destroyingOwnerItem = false;
    std::unordered_set<wxWinUIItemModel::Id> renderingOwnerItems;
    std::unordered_map<wxWinUIItemModel::Id, CheckBinding> checkBindings;
};

namespace
{

#if wxUSE_OWNER_DRAWN

// CreateLboxItem() is an application callback and can both throw and destroy
// the control. Never retain a reference to the flag across that boundary: the
// callback state tells us whether the implementation still exists before the
// destructor restores it.
class wxWinUIOwnerItemCreationGuard final
{
public:
    wxWinUIOwnerItemCreationGuard(
        wxWinUIListBoxImpl *implementation,
        const std::shared_ptr<wxWinUIListBoxCallbackState>& callbackState)
        : m_implementation(implementation),
          m_callbackState(callbackState),
          m_oldValue(implementation
                         ? implementation->creatingOwnerItem
                         : false)
    {
        if ( m_implementation )
            m_implementation->creatingOwnerItem = true;
    }

    ~wxWinUIOwnerItemCreationGuard()
    {
        if ( m_implementation && m_callbackState &&
             m_callbackState->GetOwner() )
        {
            m_implementation->creatingOwnerItem = m_oldValue;
        }
    }

private:
    wxWinUIListBoxImpl *m_implementation;
    std::shared_ptr<wxWinUIListBoxCallbackState> m_callbackState;
    bool m_oldValue;
};

#endif // wxUSE_OWNER_DRAWN

} // anonymous namespace

void wxListBox::Init()
{
    m_selectedItemId = 0;
    m_updatingPeer = false;
    m_horizontalExtent = 0;
    m_tabStopsState = WinUITabStopsState::NeverSet;
    m_tabStops.clear();
    m_controlWidthDIPs = 0.0;
    m_controlHeightDIPs = 0.0;
    m_ownerDrawProjectionScale = 0.0;
}

wxListBox::wxListBox()
{
    Init();
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     int n, const wxString choices[],
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    Init();
    Create(parent, id, pos, size, n, choices, style, validator, name);
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,
                     const wxSize& size,
                     const wxArrayString& choices,
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    Init();
    Create(parent, id, pos, size, choices, style, validator, name);
}

wxListBox::~wxListBox()
{
    Unbind(wxEVT_DPI_CHANGED, &wxListBox::OnDPIChanged, this);

    // Peer callbacks must be gone before Clear() destroys client objects.
    if ( m_winui )
    {
        m_winui->Close();
        m_winui.reset();
    }

    Clear();
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    // The WinUI ListBox draws its own border, so suppress the native control
    // border to avoid an extra grey frame around the island.
    style = (style & ~wxBORDER_MASK) | wxBORDER_NONE;

    wxASSERT_MSG( !(style & wxLB_MULTIPLE) ||
                      !(style & wxLB_EXTENDED),
                  wxT("only one listbox selection mode may be specified") );
    wxASSERT_MSG( !(style & wxLB_ALWAYS_SB) ||
                      !(style & wxLB_NO_SB),
                  wxT("conflicting listbox scrollbar styles") );

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    // wxControl::Create() can use a default/best size before the WinUI peer
    // exists. A fully specified Create() size is the first authoritative wx
    // geometry writer, so publish it before any peer content is measured.
    const double createScale = wxWinUIListScale(this);
    if ( size.x != wxDefaultCoord && size.x >= 0 )
        m_controlWidthDIPs = size.x / createScale;
    if ( size.y != wxDefaultCoord && size.y >= 0 )
        m_controlHeightDIPs = size.y / createScale;

    m_winui.reset(new wxWinUIListBoxImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIListBoxCallbackState>(this);
    wxWinUIListBoxImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->listView = MUXC::ListView();
        if ( style & wxLB_HSCROLL )
        {
            // Keep ListView's supported pixel-virtualizing panel explicit so
            // the horizontal projection never regresses to a plain StackPanel.
            // ItemsStackPanel has no ScrollOwner API: once realized, the
            // nearest ScrollContentPresenter ancestor is configured by the
            // extent projection instead.
            static constexpr wchar_t horizontalItemsPanel[] =
                LR"(<ItemsPanelTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"><ItemsStackPanel Orientation="Vertical" /></ItemsPanelTemplate>)";
            m_winui->listView.ItemsPanel(
                MUX::Markup::XamlReader::Load(horizontalItemsPanel)
                    .as<MUXC::ItemsPanelTemplate>());
        }
        wxWinUIApplyFont(m_winui->listView, GetFont());

        WinUIApplyScrollPolicy();

        if ( style & wxLB_EXTENDED )
            m_winui->listView.SelectionMode(MUXC::ListViewSelectionMode::Extended);
        else if ( style & wxLB_MULTIPLE )
            m_winui->listView.SelectionMode(MUXC::ListViewSelectionMode::Multiple);
        else
            m_winui->listView.SelectionMode(MUXC::ListViewSelectionMode::Single);

        m_winui->selectionChangedToken = m_winui->listView.SelectionChanged(
            [callbackState](
                const winrt::Windows::Foundation::IInspectable&,
                const MUXC::SelectionChangedEventArgs& args)
            {
                wxListBox *owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_updatingPeer ||
                     !owner->m_winui->peerItemsValid )
                {
                    return;
                }

                try
                {
                    std::vector<wxWinUIItemModel::Id> deselectedIds;
                    std::vector<wxWinUIItemModel::Id> selectedIds;

                    if ( !owner->HasMultipleSelection() &&
                         args.AddedItems().Size() )
                    {
                        for ( std::size_t n = 0;
                              n < owner->m_itemModel.GetCount(); ++n )
                        {
                            auto& modelItem = owner->m_itemModel.At(n);
                            if ( modelItem.selected )
                            {
                                modelItem.selected = false;
                                deselectedIds.push_back(modelItem.id);
                            }
                        }
                    }

                    const auto applyChanges =
                        [owner, &deselectedIds, &selectedIds](
                            const auto& peers, bool selected)
                        {
                            for ( const auto& peer : peers )
                            {
                                const std::size_t peerIndex =
                                    owner->m_winui->FindPeerIndex(peer);
                                if ( peerIndex >=
                                     owner->m_winui->peerItemIds.size() )
                                {
                                    continue;
                                }

                                const wxWinUIItemModel::Id itemId =
                                    owner->m_winui->peerItemIds[peerIndex];
                                const std::size_t modelIndex =
                                    owner->m_itemModel.IndexOf(itemId);
                                if ( modelIndex != wxWinUIItemModel::npos )
                                {
                                    auto& modelItem =
                                        owner->m_itemModel.At(modelIndex);
                                    if ( modelItem.selected != selected )
                                    {
                                        modelItem.selected = selected;
                                        (selected ? selectedIds
                                                  : deselectedIds)
                                            .push_back(itemId);
                                    }
                                }
                            }
                        };

                    applyChanges(args.RemovedItems(), false);
                    applyChanges(args.AddedItems(), true);

                    owner->m_selectedItemId = 0;
                    for ( std::size_t n = 0;
                          n < owner->m_itemModel.GetCount(); ++n )
                    {
                        if ( owner->m_itemModel.At(n).selected )
                        {
                            owner->m_selectedItemId =
                                owner->m_itemModel.At(n).id;
                            break;
                        }
                    }

#if wxUSE_OWNER_DRAWN
                    if ( owner->HasFlag(wxLB_OWNERDRAW) )
                    {
                        // Owner-draw callbacks below can destroy the control.
                        wxWinUIListBoxImpl * const implementation =
                            owner->m_winui.get();

                        // Native owner-draw invalidates only rows whose
                        // selection state changed. Keep the same contract and
                        // draw removals before additions so a newly selected
                        // row is the final observable DRAWITEM state.
                        const auto refreshChanged =
                            [callbackState, implementation, &owner](
                                const auto& ids) -> bool
                            {
                                for ( const wxWinUIItemModel::Id id : ids )
                                {
                                    (void)owner->WinUIUpdatePeerItemById(id);
                                    owner = callbackState->GetOwner();
                                    if ( !owner || !owner->m_winui ||
                                         owner->m_winui.get() !=
                                             implementation ||
                                         owner->m_winui->callbackState !=
                                             callbackState )
                                    {
                                        return false;
                                    }
                                }
                                return true;
                            };

                        if ( !refreshChanged(deselectedIds) ||
                             !refreshChanged(selectedIds) )
                        {
                            return;
                        }
                    }
#endif

                    // The event can destroy owner. Nothing follows it.
                    owner->SendSelectionEvent();
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox selection callback", e);
                }
            });

        m_winui->doubleTappedToken = m_winui->listView.DoubleTapped(
            [callbackState](
                const winrt::Windows::Foundation::IInspectable&,
                const winrt::Microsoft::UI::Xaml::Input::
                    DoubleTappedRoutedEventArgs& args)
            {
                wxListBox *owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_updatingPeer )
                {
                    return;
                }

                try
                {
                    MUX::DependencyObject node =
                        args.OriginalSource().try_as<MUX::DependencyObject>();
                    wxWinUIItemModel::Id itemId = 0;
                    while ( node )
                    {
                        if ( const auto item =
                                 node.try_as<MUXC::ListViewItem>() )
                        {
                            const std::size_t peerIndex =
                                owner->m_winui->FindPeerIndex(item);
                            if ( peerIndex <
                                 owner->m_winui->peerItemIds.size() )
                            {
                                itemId =
                                    owner->m_winui->peerItemIds[peerIndex];
                            }
                            break;
                        }
                        node = MUXM::VisualTreeHelper::GetParent(node);
                    }

                    if ( !itemId )
                        return;

                    // SendEvent() can delete owner. Nothing follows it.
                    owner->WinUISendDoubleClick(itemId);
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException(
                        "WinUI ListBox double-click callback", e);
                }
            });

        if ( WinUIIsCheckable() )
        {
            // Tunnel before ListViewItem/CheckBox consume the MSW checklist
            // keys. ListView's TabNavigation=Once keeps the composite as one
            // wx focus stop while the retained checkbox remains a genuine
            // pointer-focusable UIA Toggle peer.
            m_winui->keyDownToken = m_winui->listView.PreviewKeyDown(
                [callbackState](
                    const winrt::Windows::Foundation::IInspectable&,
                    const winrt::Microsoft::UI::Xaml::Input::
                        KeyRoutedEventArgs& args)
                {
                    wxListBox * const owner = callbackState->GetOwner();
                    if ( !owner || !owner->m_winui ||
                         owner->m_winui->callbackState != callbackState ||
                         owner->m_updatingPeer )
                    {
                        return;
                    }

                    try
                    {
                        // The helper snapshots selected stable IDs and may
                        // delete owner while publishing an event. args is an
                        // independent WinRT value and remains safe to update.
                        const bool handled =
                            owner->WinUIHandleCheckKey(
                                static_cast<int>(args.Key()),
                                (::GetKeyState(VK_SHIFT) & 0x8000) != 0,
                                0,
                                true);
                        args.Handled(handled);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI CheckListBox keyboard toggle", e);
                    }
                });
        }

        const auto refreshPresentation =
            [callbackState](bool newSlotLayoutBoundary)
            {
                wxListBox *owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_updatingPeer )
                {
                    return;
                }

                if ( owner->HasFlag(wxLB_HSCROLL) )
                {
                    if ( newSlotLayoutBoundary )
                    {
                        // A genuine Loaded/SizeChanged edge grants at most two
                        // slot renders: the callback pass and one later UI-turn
                        // retry if the ItemsPanel is created asynchronously.
                        owner->m_winui->
                            horizontalMaterializationRetriesRemaining = 2;
                    }
                    // Terminal: synchronization may force the shared slot and
                    // owner-draw callbacks can destroy this control.
                    (void)owner->WinUISynchronizeHorizontalPresentation();
                    return;
                }

                bool refresh = false;
#if wxUSE_OWNER_DRAWN
                refresh = refresh || owner->HasFlag(wxLB_OWNERDRAW);
#endif
                if ( refresh )
                    owner->WinUIRefreshItems();
            };

        m_winui->sizeChangedToken = m_winui->listView.SizeChanged(
            [refreshPresentation](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::SizeChangedEventArgs&)
            {
                refreshPresentation(true);
            });
        m_winui->loadedToken = m_winui->listView.Loaded(
            [refreshPresentation](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::RoutedEventArgs&)
            {
                // The ItemsPresenter is template-created and does not exist
                // during pre-show SetHorizontalExtent(). Loaded is the first
                // deterministic point at which it can own ScrollViewer extent.
                refreshPresentation(true);
            });
        m_winui->layoutUpdatedToken = m_winui->listView.LayoutUpdated(
            [callbackState](
                const winrt::Windows::Foundation::IInspectable&,
                const winrt::Windows::Foundation::IInspectable&)
            {
                wxListBox * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     (!owner->m_winui->horizontalPresentationPending &&
                      !owner->m_winui->horizontalResetPending) ||
                     owner->m_winui->horizontalMaterializationInProgress ||
                     owner->m_updatingPeer )
                {
                    return;
                }

                try
                {
                    if ( !wxWinUIGetListScrollViewer(
                             owner->m_winui->listView) )
                    {
                        return;
                    }
                    owner->m_winui->horizontalPresentationPending = false;
                    owner->m_winui->horizontalResetPending = false;
                }
                catch ( const winrt::hresult_error& e )
                {
                    // Keep the request pending for the next real layout edge.
                    wxWinUILogException(
                        "WinUI ListBox empty extent layout retry", e);
                    return;
                }

                // Terminal: synchronization distinguishes the live peer item
                // count, then performs either the authentic empty reset or a
                // complete retained-row projection.
                (void)owner->WinUISynchronizeHorizontalPresentation();
            });
        m_winui->actualThemeChangedToken =
            m_winui->listView.ActualThemeChanged(
                [refreshPresentation](
                    const MUX::FrameworkElement&,
                    const winrt::Windows::Foundation::IInspectable&)
                {
                    refreshPresentation(false);
                });
        m_winui->gotFocusToken = m_winui->listView.GotFocus(
            [refreshPresentation](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::RoutedEventArgs&)
            {
                refreshPresentation(false);
            });
        m_winui->lostFocusToken = m_winui->listView.LostFocus(
            [refreshPresentation](
                const winrt::Windows::Foundation::IInspectable&,
                const MUX::RoutedEventArgs&)
            {
                refreshPresentation(false);
            });

        const bool contentSet =
            createImpl->host.SetContent(createImpl->listView);
        wxListBox * const owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox creation", e);
        wxListBox * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxListBox *owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    if ( n && choices )
    {
        owner->Append(n, choices);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
    }

    owner->SetInitialSize(size);
    owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    owner->Unbind(wxEVT_DPI_CHANGED, &wxListBox::OnDPIChanged, owner);
    owner->Bind(wxEVT_DPI_CHANGED, &wxListBox::OnDPIChanged, owner);
    return true;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       const wxArrayString& choices,
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, pos, size, chs.GetCount(), chs.GetStrings(),
                  style, validator, name);
}

#if wxUSE_OWNER_DRAWN

wxOwnerDrawn *wxListBox::CreateLboxItem(size_t WXUNUSED(n))
{
    return new wxWinUIListBoxItem(this);
}

void wxListBox::WinUIApplyOwnerDrawFont()
{
    if ( !HasFlag(wxLB_OWNERDRAW) )
        return;

    // wxMSW's DPI path ultimately calls SetFont(m_font), which mirrors the
    // effective control font into every wxOwnerDrawn before the next
    // DRAWITEM. Keep the retained WinUI items under exactly the same
    // contract. This operation itself invokes no application callback.
    const wxFont font = GetFont();
    const unsigned int count = m_aItems.GetCount();
    for ( unsigned int n = 0; n < count; ++n )
        m_aItems[n]->SetFont(font);
}

bool wxListBox::SetFont(const wxFont& font)
{
    const bool changed = wxListBoxBase::SetFont(font);
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;

    if ( implementation && implementation->listView )
    {
        try
        {
            // Keep the inherited ListView/ListViewItem typography in lockstep
            // with the wx font used by GetTextExtent() and owner drawing.
            wxWinUIApplyFont(implementation->listView, GetFont());
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ListBox font projection", e);
        }
    }

    wxListBox * const owner = callbackState
        ? callbackState->GetOwner()
        : this;
    if ( !owner || owner != this ||
         (implementation &&
          (!owner->m_winui || owner->m_winui.get() != implementation ||
           owner->m_winui->callbackState != callbackState)) )
    {
        return changed;
    }

    owner->WinUIApplyOwnerDrawFont();

    owner->m_horizontalExtent = 0;
    if ( owner->HasFlag(wxLB_HSCROLL) )
        owner->SetHorizontalExtent();
    else
        owner->WinUIRefreshItems();
    return changed;
}

bool wxListBox::GetItemRect(size_t n, wxRect& rect) const
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), false,
                 wxT("invalid index in wxListBox::GetItemRect") );

    rect = wxRect();
    if ( !m_winui || !m_winui->listView )
        return false;

    try
    {
        const MUX::FrameworkElement item =
            m_winui->listView.ContainerFromIndex(
                static_cast<int32_t>(n)).try_as<MUX::FrameworkElement>();
        if ( !item || item.Visibility() != MUX::Visibility::Visible ||
             item.ActualWidth() <= 0.0 || item.ActualHeight() <= 0.0 )
        {
            return false;
        }

        WF::Rect clientBounds{};
        if ( wxWinUIVisualCoordinates::ElementBoundsToClient(
                 const_cast<wxListBox *>(this),
                 item,
                 WF::Rect{ 0.0f, 0.0f,
                           static_cast<float>(item.ActualWidth()),
                           static_cast<float>(item.ActualHeight()) },
                 &clientBounds) != wxWinUICoordinateResult::Mapped )
        {
            return false;
        }

        const int left = static_cast<int>(std::floor(clientBounds.X));
        const int top = static_cast<int>(std::floor(clientBounds.Y));
        const int right = static_cast<int>(std::ceil(
            clientBounds.X + clientBounds.Width));
        const int bottom = static_cast<int>(std::ceil(
            clientBounds.Y + clientBounds.Height));
        rect = wxRect(left, top, wxMax(0, right - left),
                      wxMax(0, bottom - top));

        return rect.Intersects(
            wxRect(wxPoint(0, 0), GetClientSize()));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox item rectangle", e);
        return false;
    }
}

bool wxListBox::RefreshItem(size_t n)
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), false,
                 wxT("invalid index in wxListBox::RefreshItem") );

    const wxWinUIItemModel::Id id = m_itemModel.At(n).id;
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;

    // Keep the off-screen peer current too: unlike an HWND owner-draw item,
    // its XAML bitmap is retained between paints. The return value still
    // follows MSW and reports whether an invalidatable row is visible.
    if ( !WinUIUpdatePeerItemById(id) )
        return false;

    wxListBox * const owner = callbackState
        ? callbackState->GetOwner()
        : nullptr;
    if ( !owner || owner != this || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    const std::size_t currentIndex = owner->m_itemModel.IndexOf(id);
    if ( currentIndex == wxWinUIItemModel::npos )
        return false;

    wxRect rect;
    return owner->GetItemRect(currentIndex, rect);
}

#endif // wxUSE_OWNER_DRAWN

wxListBox::WinUITabLayout
wxListBox::WinUIBuildTabLayout(const wxString& text) const
{
    WinUITabLayout layout;
    const int average = wxMax(1, GetCharWidth());
    int x = 0;
    std::size_t start = 0;

    const auto dluToPixels = [average](int dlu)
    {
        // LB_SETTABSTOPS dialog units use MulDiv rounding, not ceiling.
        // MulDiv also avoids overflowing an intermediate int multiplication.
        const int pixels = ::MulDiv(dlu, average, 4);
        if ( pixels == -1 )
            return std::numeric_limits<int>::max();
        return pixels > 0 ? pixels : 1;
    };

    const auto advanceRepeated = [](int position, int interval)
    {
        if ( position >= std::numeric_limits<int>::max() - interval )
            return std::numeric_limits<int>::max();
        return ((position / interval) + 1) * interval;
    };

    for ( ;; )
    {
        const std::size_t tab = text.find(wxT('\t'), start);
        const wxString run = tab == wxString::npos
            ? text.Mid(start)
            : text.Mid(start, tab - start);
        layout.runs.push_back(run);
        layout.runOffsets.push_back(x);

        wxCoord runWidth = 0;
        GetTextExtent(run, &runWidth, nullptr);
        if ( runWidth > std::numeric_limits<int>::max() - x )
            x = std::numeric_limits<int>::max();
        else
            x += runWidth;

        if ( tab == wxString::npos )
            break;

        if ( m_tabStopsState == WinUITabStopsState::NeverSet )
        {
            x = advanceRepeated(x, dluToPixels(32));
        }
        else if ( m_tabStopsState ==
                      WinUITabStopsState::ExplicitDefault )
        {
            x = advanceRepeated(x, dluToPixels(2));
        }
        else if ( m_tabStops.size() == 1 )
        {
            x = advanceRepeated(x, dluToPixels(m_tabStops[0]));
        }
        else
        {
            int next = -1;
            for ( const int stop : m_tabStops )
            {
                const int position = dluToPixels(stop);
                if ( position > x )
                {
                    next = position;
                    break;
                }
            }

            if ( next < 0 )
            {
                // Once an explicit array is exhausted, TabbedTextOut resumes
                // at the standard eight-character (32-DLU) grid.
                next = advanceRepeated(x, dluToPixels(32));
            }
            x = next;
        }

        start = tab + 1;
    }

    layout.width = x;
    return layout;
}

int wxListBox::WinUIMeasureTextWidth(const wxString& text) const
{
    return WinUIBuildTabLayout(text).width;
}

int wxListBox::WinUIGetHorizontalExtentPixels() const
{
    return m_horizontalExtent;
}

void wxListBox::SetHorizontalExtent(const wxString& s)
{
    if ( !HasFlag(wxLB_HSCROLL) )
        return;

    int extent = s.empty() ? 0 : m_horizontalExtent;
    const auto includeText = [this, &extent](const wxString& text,
                                             unsigned int n)
    {
        const int characterWidth = wxMax(0, GetCharWidth());
        const int measured = WinUIMeasureTextWidth(text);
        int width = measured >
                        std::numeric_limits<int>::max() - characterWidth
            ? std::numeric_limits<int>::max()
            : measured + characterWidth;
#if wxUSE_OWNER_DRAWN
        if ( HasFlag(wxLB_OWNERDRAW) && n < m_aItems.GetCount() )
        {
            const int margin = wxMax(0, m_aItems[n]->GetMarginWidth());
            width = width > std::numeric_limits<int>::max() - margin
                ? std::numeric_limits<int>::max()
                : width + margin;
        }
#else
        wxUnusedVar(n);
#endif
        extent = wxMax(extent, width);
    };

    if ( s.empty() )
    {
        for ( unsigned int n = 0; n < GetCount(); ++n )
            includeText(GetString(n), n);
    }
    else
    {
        includeText(s, 0);
    }

    // WinUI keeps the ItemsPanelRoot alive after its final item disappears.
    // Always clear the explicit panel extent for the empty model, including
    // the important old==new==0 path reached after deleting the last item.
    if ( extent == 0 && m_winui )
        (void)WinUIResetHorizontalExtent();

    if ( extent == m_horizontalExtent )
    {
        InvalidateBestSize();
        return;
    }

    m_horizontalExtent = extent;
    InvalidateBestSize();
    WinUIRefreshItems();
}

bool wxListBox::MSWSetTabStops(const wxVector<int>& tabStops)
{
    int previous = -1;
    for ( const int stop : tabStops )
    {
        if ( stop <= 0 || stop <= previous )
            return false;
        previous = stop;
    }

    m_tabStops = tabStops;
    m_tabStopsState = tabStops.empty()
        ? WinUITabStopsState::ExplicitDefault
        : WinUITabStopsState::ExplicitStops;
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    const int oldExtent = m_horizontalExtent;
    if ( HasFlag(wxLB_HSCROLL) )
    {
        m_horizontalExtent = 0;
        SetHorizontalExtent();

        wxListBox * const owner = callbackState
            ? callbackState->GetOwner()
            : nullptr;
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState )
        {
            return true;
        }

        // A changed extent already refreshed every peer.
        if ( owner->m_horizontalExtent != oldExtent )
            return true;
    }

    // Terminal: owner-draw callbacks reached here may destroy this control.
    WinUIRefreshItems();
    return true;
}

bool wxListBox::WinUIEnsureHorizontalPresentation()
{
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    if ( !HasFlag(wxLB_HSCROLL) || !implementation || !callbackState ||
         !implementation->listView )
    {
        return false;
    }

    const MUXC::ListView listView = implementation->listView;
    const auto listIdentity = wxWinUIListObjectIdentity(listView);
    wxListBox * const expectedOwner = this;
    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState->GetOwner();
        return owner && owner == expectedOwner && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState &&
                       wxWinUIListObjectIdentity(
                           owner->m_winui->listView) == listIdentity
                   ? owner
                   : nullptr;
    };

    const auto queueRetry = [&]()
    {
        wxListBox * const owner = getCurrentOwner();
        if ( !owner ||
             implementation->horizontalMaterializationRetryQueued ||
             implementation->horizontalMaterializationRetriesRemaining == 0 )
        {
            return;
        }

        try
        {
            const MUXD::DispatcherQueue queue = listView.DispatcherQueue();
            if ( !queue )
                return;

            implementation->horizontalMaterializationRetryQueued = true;
            const bool accepted = queue.TryEnqueue(
                MUXD::DispatcherQueuePriority::Low,
                [expectedOwner, callbackState, implementation,
                 listIdentity]()
                {
                    wxListBox * const liveOwner =
                        callbackState->GetOwner();
                    if ( !liveOwner || liveOwner != expectedOwner ||
                         !liveOwner->m_winui ||
                         liveOwner->m_winui.get() != implementation ||
                         liveOwner->m_winui->callbackState != callbackState ||
                         wxWinUIListObjectIdentity(
                             liveOwner->m_winui->listView) != listIdentity )
                    {
                        return;
                    }

                    implementation->horizontalMaterializationRetryQueued =
                        false;
                    if ( !implementation->horizontalPresentationPending ||
                         implementation->
                             horizontalMaterializationInProgress ||
                         liveOwner->m_updatingPeer )
                    {
                        return;
                    }

                    // Terminal: the synchronization owns the remaining retry
                    // budget and every subsequent lifetime revalidation.
                    (void)liveOwner->
                        WinUISynchronizeHorizontalPresentation();
                });
            if ( !accepted )
                implementation->horizontalMaterializationRetryQueued = false;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException(
                "WinUI ListBox horizontal presentation retry queue", e);
            wxListBox * const liveOwner = getCurrentOwner();
            if ( liveOwner )
            {
                implementation->horizontalMaterializationRetryQueued = false;
            }
        }
    };

    const auto finish = [&](bool ready) -> bool
    {
        wxListBox * const owner = getCurrentOwner();
        if ( !owner )
            return false;

        implementation->horizontalMaterializationInProgress = false;
        implementation->horizontalPresentationPending = !ready;
        if ( !ready )
            queueRetry();
        return ready;
    };

    try
    {
        if ( wxWinUIGetListScrollViewer(listView) )
        {
            implementation->horizontalPresentationPending = false;
            return true;
        }

        if ( implementation->horizontalMaterializationInProgress ||
             m_updatingPeer )
        {
            implementation->horizontalPresentationPending = true;
            return false;
        }

        if ( implementation->
                 horizontalMaterializationRetriesRemaining == 0 )
        {
            implementation->horizontalPresentationPending = true;
            return false;
        }

        implementation->horizontalMaterializationInProgress = true;
        --implementation->horizontalMaterializationRetriesRemaining;
        if ( implementation->horizontalMaterializationAttempts !=
             std::numeric_limits<unsigned int>::max() )
        {
            ++implementation->horizontalMaterializationAttempts;
        }

        // Applying the control template can synchronously run Loaded and
        // application callbacks. It creates the ListView template shell, but
        // the ItemsPresenter/ItemsPanelRoot still needs the real shared slot's
        // geometry before its ScrollOwner can be authoritative.
        listView.ApplyTemplate();
        wxListBox *owner = getCurrentOwner();
        if ( !owner )
            return false;

        if ( wxWinUIGetListScrollViewer(listView) )
            return finish(true);

        // ForceRender performs exactly one synchronous shared-slot flush and
        // is itself lifetime-safe if FlushSync destroys this control. Never
        // dereference owner/implementation until the callback ticket has been
        // revalidated below.
        implementation->host.ForceRender();
        owner = getCurrentOwner();
        if ( !owner )
            return false;

        // The slot now has current wx geometry. Bound realization to one final
        // template/layout pass; LayoutUpdated only observes readiness and does
        // not recursively force another layout cycle.
        listView.ApplyTemplate();
        owner = getCurrentOwner();
        if ( !owner )
            return false;
        listView.InvalidateMeasure();
        listView.InvalidateArrange();
        listView.UpdateLayout();
        owner = getCurrentOwner();
        if ( !owner )
            return false;

        return finish(!!wxWinUIGetListScrollViewer(listView));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ListBox horizontal presentation materialization", e);
        return finish(false);
    }
}

bool wxListBox::WinUISynchronizeHorizontalPresentation()
{
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    if ( !implementation || !callbackState || m_updatingPeer )
    {
        if ( implementation )
            implementation->horizontalPresentationPending = true;
        return false;
    }

    const MUXC::ListView listView = implementation->listView;
    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState->GetOwner();
        return owner && owner == this && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState &&
                       wxWinUIListObjectIdentity(
                           owner->m_winui->listView) ==
                           wxWinUIListObjectIdentity(listView)
                   ? owner
                   : nullptr;
    };

    (void)WinUIEnsureHorizontalPresentation();
    wxListBox *owner = getCurrentOwner();
    if ( !owner )
        return false;

    bool empty = false;
    try
    {
        if ( !wxWinUIGetListScrollViewer(listView) )
            return false;

        // Suppress recursive LayoutUpdated consumption while the actual reset
        // or row projection below mutates the realized chain. Either operation
        // republishes the appropriate pending state if it cannot complete.
        implementation->horizontalPresentationPending = false;
        implementation->horizontalResetPending = false;
        empty = listView.Items().Size() == 0;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ListBox horizontal presentation synchronization", e);
        owner = callbackState->GetOwner();
        if ( owner && owner == this && owner->m_winui &&
             owner->m_winui.get() == implementation &&
             owner->m_winui->callbackState == callbackState )
        {
            implementation->horizontalPresentationPending = true;
        }
        return false;
    }

    if ( empty )
    {
        // The reset is the real production operation. It validates this exact
        // implementation after clearing the realized panel/content chain.
        if ( !owner->WinUIResetHorizontalExtent() )
            return false;
    }
    else
    {
        // This is a real production retry, not a metric-only test projection:
        // re-render every retained row onto the newly realized authoritative
        // ScrollViewer chain. Owner drawing may destroy this control.
        owner->WinUIRefreshItems();
    }

    owner = getCurrentOwner();
    if ( !owner )
    {
        return false;
    }

    try
    {
        // Consume the row widths/reset written above in the already-realized
        // ListView. The shared slot flush was bounded to the single pass in
        // WinUIEnsureHorizontalPresentation(); this final local layout pass
        // merely publishes its resulting ExtentWidth.
        listView.InvalidateMeasure();
        listView.InvalidateArrange();
        listView.UpdateLayout();
        owner = getCurrentOwner();
        return owner && !!wxWinUIGetListScrollViewer(listView);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ListBox horizontal presentation validation", e);
        owner = getCurrentOwner();
        if ( owner )
        {
            implementation->horizontalPresentationPending = true;
            implementation->horizontalResetPending = empty;
        }
        return false;
    }
}

bool wxListBox::WinUIResetHorizontalExtent()
{
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    if ( !implementation || !callbackState || !implementation->listView )
        return false;

    const bool reset =
        wxWinUIResetListHorizontalExtent(implementation->listView);

    // Clearing XAML Width values can synchronously enter layout callbacks.
    // Publish the pending state only after revalidating the exact owner and
    // implementation; a destroyed/rebuilt control must never inherit this
    // request.
    wxListBox * const owner = callbackState->GetOwner();
    if ( !owner || owner != this || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    implementation->horizontalResetPending = !reset;
    implementation->horizontalPresentationPending = !reset;
    return reset;
}

void wxListBox::WinUIApplyScrollPolicy()
{
    if ( !m_winui || !m_winui->listView )
        return;

    const bool horizontal = HasFlag(wxLB_HSCROLL);
    MUXC::ScrollViewer::SetHorizontalScrollMode(
        m_winui->listView,
        horizontal ? MUXC::ScrollMode::Enabled
                   : MUXC::ScrollMode::Disabled);
    MUXC::ScrollViewer::SetHorizontalScrollBarVisibility(
        m_winui->listView,
        horizontal ? MUXC::ScrollBarVisibility::Auto
                   : MUXC::ScrollBarVisibility::Disabled);

    const MUXC::ScrollBarVisibility vertical =
        HasFlag(wxLB_NO_SB)
            ? MUXC::ScrollBarVisibility::Disabled
            : HasFlag(wxLB_ALWAYS_SB)
                ? MUXC::ScrollBarVisibility::Visible
                : MUXC::ScrollBarVisibility::Auto;
    MUXC::ScrollViewer::SetVerticalScrollMode(
        m_winui->listView,
        HasFlag(wxLB_NO_SB) ? MUXC::ScrollMode::Disabled
                            : MUXC::ScrollMode::Enabled);
    MUXC::ScrollViewer::SetVerticalScrollBarVisibility(
        m_winui->listView, vertical);
}

void wxListBox::OnDPIChanged(wxDPIChangedEvent& event)
{
    event.Skip();

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    const wxSize newDPI = event.GetNewDPI();
    m_ownerDrawProjectionScale = newDPI.x > 0
        ? static_cast<double>(newDPI.x) / 96.0
        : 0.0;

#if wxUSE_OWNER_DRAWN
    // The base DPI processing may have recreated the effective wxFont. MSW
    // calls SetFont(m_font) here, so update the per-item font before either
    // measuring an extent or invoking the next owner-draw callback.
    WinUIApplyOwnerDrawFont();
#endif

    m_horizontalExtent = 0;
    SetHorizontalExtent();

    wxListBox * const owner = callbackState
        ? callbackState->GetOwner()
        : nullptr;
    if ( !owner || owner != this || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }

    // Terminal: refresh uses the new bitmap scale and may run user drawing.
    WinUIRefreshItems();
}

unsigned int wxListBox::GetCount() const
{
    return static_cast<unsigned int>(m_itemModel.GetCount());
}

wxString wxListBox::GetString(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), wxString(),
                 wxT("invalid listbox index") );
    return m_itemModel.At(n).text;
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid listbox index") );
#if wxUSE_OWNER_DRAWN
    wxCHECK_RET( !m_winui ||
                     (!m_winui->creatingOwnerItem &&
                      !m_winui->destroyingOwnerItem),
                 wxT("reentrant owner-draw item mutation") );
#endif

    const wxWinUIItemModel::Id id = m_itemModel.At(n).id;
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState
            ? callbackState->GetOwner()
            : nullptr;
        return owner && owner == this && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    WinUIEnsurePeerConsistent();
    wxListBox *owner = getCurrentOwner();
    if ( !owner )
        return;
    const std::size_t currentIndex = owner->m_itemModel.IndexOf(id);
    if ( currentIndex == wxWinUIItemModel::npos )
        return;
#if wxUSE_OWNER_DRAWN
    wxOwnerDrawn * const ownerDrawItem =
        owner->HasFlag(wxLB_OWNERDRAW) &&
                currentIndex < owner->m_aItems.GetCount()
            ? owner->m_aItems[currentIndex]
            : nullptr;
#endif
    const auto change = owner->m_itemModel.Rename(
        currentIndex, s, owner->IsSorted());
    if ( change.kind == wxWinUIItemModel::Change::Kind::Move )
    {
#if wxUSE_OWNER_DRAWN
        if ( ownerDrawItem )
        {
            owner->m_aItems.RemoveAt(change.oldIndex);
            owner->m_aItems.Insert(ownerDrawItem, change.newIndex);
        }
#endif
        owner->WinUIMovePeerItem(
            static_cast<unsigned int>(change.oldIndex),
            static_cast<unsigned int>(change.newIndex));
        owner = getCurrentOwner();
        if ( !owner )
            return;
    }

    if ( !owner->WinUIUpdatePeerItemById(change.id) )
        return;

    owner->ApplySelectionToPeer();
    owner->WinUISyncOldSelections();
    owner->InvalidateBestSize();
    if ( owner->HasFlag(wxLB_HSCROLL) )
    {
        owner->m_horizontalExtent = 0;
        // Terminal: owner drawing can destroy the control.
        owner->SetHorizontalExtent();
    }
}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
    {
        if ( m_itemModel.At(i).text.IsSameAs(s, bCase) )
            return static_cast<int>(i);
    }

    return wxNOT_FOUND;
}

bool wxListBox::IsSelected(int n) const
{
    wxCHECK_MSG( n >= 0 &&
                     static_cast<std::size_t>(n) < m_itemModel.GetCount(),
                 false, wxT("invalid listbox index") );
    return m_itemModel.At(n).selected;
}

int wxListBox::GetSelection() const
{
    wxCHECK_MSG( !HasMultipleSelection(), wxNOT_FOUND,
                 wxT("GetSelection() can't be used with multiple selection listbox") );

    if ( !m_selectedItemId )
        return wxNOT_FOUND;

    const std::size_t index = m_itemModel.IndexOf(m_selectedItemId);
    return index == wxWinUIItemModel::npos
               ? wxNOT_FOUND
               : static_cast<int>(index);
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Empty();

    for ( std::size_t n = 0; n < m_itemModel.GetCount(); ++n )
    {
        if ( m_itemModel.At(n).selected )
            aSelections.Add(static_cast<int>(n));
    }
    return static_cast<int>(aSelections.GetCount());
}

int wxListBox::GetTopItem() const
{
    if ( !m_winui || !m_winui->listView )
        return wxNOT_FOUND;

    try
    {
        const double viewHeight = m_winui->listView.ActualHeight();
        for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
        {
            const auto container =
                m_winui->listView.ContainerFromIndex(
                    static_cast<int32_t>(i))
                    .try_as<MUX::FrameworkElement>();
            if ( !container )
                continue;

            const auto origin = container.TransformToVisual(m_winui->listView)
                .TransformPoint(winrt::Windows::Foundation::Point{ 0, 0 });
            if ( origin.Y + container.ActualHeight() > 0 && origin.Y < viewHeight )
                return static_cast<int>(i);
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return wxNOT_FOUND;
}

int wxListBox::GetCountPerPage() const
{
    if ( !m_winui || !m_winui->listView )
        return -1;

    try
    {
        // The ListView lives under the shared WinUI host and can be taller
        // than the viewport that actually clips its items. Resolve the inner
        // ScrollViewer from ItemsPanelRoot/ScrollOwner, never an outer host
        // ScrollViewer, and use its effective viewport just like wxMSW uses
        // the list box client rect.
        const MUXC::ScrollViewer scrollViewer =
            wxWinUIGetListScrollViewer(m_winui->listView);
        if ( !scrollViewer )
            return -1;

        const double viewportHeight = scrollViewer.ViewportHeight();
        if ( !std::isfinite(viewportHeight) || viewportHeight <= 0.0 )
            return -1;

        for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
        {
            const auto container =
                m_winui->listView.ContainerFromIndex(
                    static_cast<int32_t>(i))
                    .try_as<MUX::FrameworkElement>();
            if ( container )
            {
                const double lineHeight = container.ActualHeight();
                if ( !std::isfinite(lineHeight) || lineHeight <= 0.0 )
                    continue;

                // ListBox is LBS_OWNERDRAWFIXED on wxMSW, and the WinUI
                // projection likewise assigns one explicit height to every
                // retained container. Any realized row is therefore the
                // authoritative line-height sample, including under
                // virtualization after SetFirstItem()/EnsureVisible().
                return static_cast<int>(viewportHeight / lineHeight);
            }
        }
    }
    catch ( const winrt::hresult_error& )
    {
    }

    return -1;
}

void wxListBox::EnsureVisible(int n)
{
    wxCHECK_RET( n >= 0 &&
                     static_cast<std::size_t>(n) < m_itemModel.GetCount(),
                 wxT("invalid listbox index") );

    if ( !m_winui || !m_winui->listView )
        return;

    try
    {
        const auto items = m_winui->listView.Items();
        if ( static_cast<uint32_t>(n) < items.Size() )
            m_winui->listView.ScrollIntoView(items.GetAt(n));
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox ensure visible", e);
    }
}

void wxListBox::DoSetFirstItem(int n)
{
    wxCHECK_RET( n >= 0 &&
                     static_cast<std::size_t>(n) < m_itemModel.GetCount(),
                 wxT("invalid listbox index") );

    if ( !m_winui || !m_winui->listView )
        return;

    try
    {
        const auto items = m_winui->listView.Items();
        if ( static_cast<uint32_t>(n) < items.Size() )
        {
            m_winui->listView.ScrollIntoView(items.GetAt(n));
            m_winui->listView.UpdateLayout();

            const auto scroll =
                wxWinUIGetListScrollViewer(m_winui->listView);
            const auto effectiveScroll = scroll
                ? scroll
                : wxWinUIFindListScrollViewer(m_winui->listView);
            const auto container =
                m_winui->listView.ContainerFromIndex(n)
                    .try_as<MUX::FrameworkElement>();
            if ( effectiveScroll && container )
            {
                const auto origin =
                    container.TransformToVisual(effectiveScroll).TransformPoint(
                        winrt::Windows::Foundation::Point{ 0, 0 });
                const double target =
                    wxMax(0.0,
                          wxMin(effectiveScroll.VerticalOffset() + origin.Y,
                                effectiveScroll.ScrollableHeight()));
                const auto vertical = winrt::box_value(target).
                    as<winrt::Windows::Foundation::IReference<double>>();
                effectiveScroll.ChangeView(
                    nullptr, vertical, nullptr, true);
                effectiveScroll.UpdateLayout();
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox set first item", e);
    }
}

void wxListBox::DoSetSelection(int n, bool select)
{
    if ( n == wxNOT_FOUND )
    {
        for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
            m_itemModel.At(i).selected = false;
        m_selectedItemId = 0;
    }
    else
    {
        wxCHECK_RET( n >= 0 &&
                         static_cast<std::size_t>(n) <
                             m_itemModel.GetCount(),
                     wxT("invalid listbox index") );

        if ( HasMultipleSelection() )
        {
            m_itemModel.At(n).selected = select;
            m_selectedItemId = 0;
            for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
            {
                if ( m_itemModel.At(i).selected )
                {
                    m_selectedItemId = m_itemModel.At(i).id;
                    break;
                }
            }
        }
        else if ( select )
        {
            for ( std::size_t i = 0; i < m_itemModel.GetCount(); ++i )
            {
                m_itemModel.At(i).selected =
                    static_cast<int>(i) == n;
            }
            m_selectedItemId = m_itemModel.At(n).id;
        }
        else if ( m_itemModel.At(n).selected )
        {
            m_itemModel.At(n).selected = false;
            m_selectedItemId = 0;
        }
    }

    ApplySelectionToPeer();
    WinUISyncOldSelections();
#if wxUSE_OWNER_DRAWN
    if ( HasFlag(wxLB_OWNERDRAW) )
        WinUIRefreshItems();
#endif
}

int wxListBox::DoListHitTest(const wxPoint& point) const
{
    if ( !m_winui || !m_winui->listView )
        return wxNOT_FOUND;

    try
    {
        const wxWinUIListBoxImpl::HitTestTicket ticket =
            m_winui->CaptureHitTestTicket();
        const auto getCurrentOwner =
            [&ticket]() -> wxListBox *
            {
                if ( !ticket.implementation ||
                     !ticket.callbackState ||
                     !ticket.listView )
                {
                    return nullptr;
                }

                wxListBox * const owner =
                    ticket.callbackState->GetOwner();
                if ( !owner || owner->m_updatingPeer ||
                     !owner->m_winui )
                {
                    return nullptr;
                }

                wxWinUIListBoxImpl * const implementation =
                    owner->m_winui.get();
                if ( implementation != ticket.implementation ||
                     implementation->callbackState !=
                         ticket.callbackState ||
                     implementation->peerRevision !=
                         ticket.peerRevision ||
                     !implementation->peerItemsValid ||
                     implementation->peerItemIds.size() !=
                         ticket.peerItemCount ||
                     owner->m_itemModel.GetCount() !=
                         ticket.peerItemCount ||
                     wxWinUIListObjectIdentity(implementation->listView) !=
                         wxWinUIListObjectIdentity(ticket.listView) )
                {
                    return nullptr;
                }

                return owner;
            };

        if ( !getCurrentOwner() )
            return wxNOT_FOUND;

        WF::Point listPoint{};
        const wxWinUICoordinateResult mapped =
            wxWinUIVisualCoordinates::ClientPointToElement(
                const_cast<wxListBox *>(this),
                WF::Point{
                    static_cast<float>(point.x),
                    static_cast<float>(point.y)
                },
                ticket.listView,
                &listPoint);
        if ( mapped != wxWinUICoordinateResult::Mapped ||
             !getCurrentOwner() )
        {
            return wxNOT_FOUND;
        }

        // Only realized containers can be hit. Walking the panel keeps this
        // proportional to the viewport instead of the complete item model.
        const MUXC::Panel panel =
            ticket.listView.ItemsPanelRoot();
        if ( !getCurrentOwner() || !panel )
        {
            return wxNOT_FOUND;
        }

        const MUXC::UIElementCollection children =
            panel.Children();
        if ( !getCurrentOwner() )
            return wxNOT_FOUND;
        const uint32_t count = children.Size();
        if ( !getCurrentOwner() )
            return wxNOT_FOUND;

        for ( uint32_t n = 0; n < count; ++n )
        {
            const MUX::UIElement child = children.GetAt(n);
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;

            const MUXC::ListViewItem container =
                child.try_as<MUXC::ListViewItem>();
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            if ( !container )
                continue;

            const MUX::Visibility visibility =
                container.Visibility();
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            if ( visibility != MUX::Visibility::Visible )
                continue;

            const bool isHitTestVisible =
                container.IsHitTestVisible();
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            if ( !isHitTestVisible )
                continue;

            const double width = container.ActualWidth();
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            const double height = container.ActualHeight();
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            if ( width <= 0.0 || height <= 0.0 )
                continue;

            const MUXM::GeneralTransform toList =
                container.TransformToVisual(ticket.listView);
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;
            const WF::Rect bounds = toList.TransformBounds(
                WF::Rect{
                    0.0f,
                    0.0f,
                    static_cast<float>(width),
                    static_cast<float>(height)
                });
            if ( !getCurrentOwner() )
                return wxNOT_FOUND;

            if ( listPoint.X >= bounds.X &&
                 listPoint.X < bounds.X + bounds.Width &&
                 listPoint.Y >= bounds.Y &&
                 listPoint.Y < bounds.Y + bounds.Height )
            {
                const int32_t peerIndex =
                    ticket.listView.IndexFromContainer(container);
                wxListBox * const owner =
                    getCurrentOwner();
                if ( !owner || peerIndex < 0 ||
                     static_cast<std::size_t>(peerIndex) >=
                         ticket.peerItemCount )
                {
                    return wxNOT_FOUND;
                }

                const std::size_t modelIndex =
                    static_cast<std::size_t>(peerIndex);
                if ( owner->m_winui->peerItemIds[modelIndex] !=
                     owner->m_itemModel.At(modelIndex).id )
                {
                    return wxNOT_FOUND;
                }

                return peerIndex;
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox hit test", e);
    }

    return wxNOT_FOUND;
}

void wxListBox::DoClear()
{
#if wxUSE_OWNER_DRAWN
    wxCHECK_RET( !m_winui ||
                     (!m_winui->creatingOwnerItem &&
                      !m_winui->destroyingOwnerItem),
                 wxT("reentrant owner-draw item mutation") );
#endif

    if ( m_winui && m_winui->listView )
    {
        wxWinUIPeerMutationGuard guard(m_updatingPeer);
        m_winui->peerItemsValid = false;
        try
        {
            for ( auto& entry : m_winui->checkBindings )
                m_winui->RevokeCheckBinding(entry.second);
            m_winui->checkBindings.clear();

            const auto items = m_winui->listView.Items();
            while ( items.Size() )
                items.RemoveAtEnd();
            m_winui->peerItemsValid = items.Size() == 0;
            (void)WinUIResetHorizontalExtent();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ListBox item clear", e);
            m_winui->peerItemsValid = false;
        }
        m_winui->peerItemIds.clear();
        m_winui->NotePeerItemsChanged();
    }

#if wxUSE_OWNER_DRAWN
    if ( !m_aItems.empty() )
    {
        wxWinUIListBoxImpl * const implementation = m_winui.get();
        const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
            implementation ? implementation->callbackState : nullptr;
        if ( implementation )
            implementation->destroyingOwnerItem = true;

        // Remove each pointer immediately before deleting it. If its user
        // destructor deletes this control, the nested wxListBox destructor can
        // still see and drain all remaining owned items before object storage
        // disappears. Moving the whole array to a local vector would leak the
        // tail as soon as the callback-state revalidation below fails.
        while ( !m_aItems.empty() )
        {
            wxOwnerDrawn * const item = m_aItems[0];
            m_aItems.RemoveAt(0);
            delete item;

            if ( callbackState )
            {
                wxListBox * const owner = callbackState->GetOwner();
                if ( !owner || owner != this || !owner->m_winui ||
                     owner->m_winui.get() != implementation ||
                     owner->m_winui->callbackState != callbackState )
                {
                    return;
                }
            }
        }
        if ( implementation )
            implementation->destroyingOwnerItem = false;
    }
#endif
    m_itemModel.Clear();
    m_selectedItemId = 0;
    m_oldSelections.clear();
    m_horizontalExtent = 0;
    InvalidateBestSize();
}

void wxListBox::DoDeleteOneItem(unsigned int n)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid listbox index") );
#if wxUSE_OWNER_DRAWN
    wxCHECK_RET( !m_winui ||
                     (!m_winui->creatingOwnerItem &&
                      !m_winui->destroyingOwnerItem),
                 wxT("reentrant owner-draw item mutation") );
#endif

    const wxWinUIItemModel::Id erasedId = m_itemModel.At(n).id;
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState
            ? callbackState->GetOwner()
            : nullptr;
        return owner && owner == this && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    WinUIEnsurePeerConsistent();
    wxListBox *owner = getCurrentOwner();
    if ( !owner )
        return;
    std::size_t currentIndex = owner->m_itemModel.IndexOf(erasedId);
    if ( currentIndex == wxWinUIItemModel::npos )
        return;
#if wxUSE_OWNER_DRAWN
    if ( owner->HasFlag(wxLB_OWNERDRAW) &&
         currentIndex < owner->m_aItems.GetCount() )
    {
        wxOwnerDrawn * const doomed = owner->m_aItems[currentIndex];
        owner->m_aItems.RemoveAt(currentIndex);
        owner->m_winui->destroyingOwnerItem = true;
        delete doomed;

        owner = getCurrentOwner();
        if ( !owner )
            return;
        owner->m_winui->destroyingOwnerItem = false;
    }
#endif
    currentIndex = owner->m_itemModel.IndexOf(erasedId);
    if ( currentIndex == wxWinUIItemModel::npos )
        return;
    owner->m_itemModel.Erase(currentIndex);
    if ( owner->m_selectedItemId == erasedId )
        owner->m_selectedItemId = 0;
    owner->WinUIErasePeerItem(
        static_cast<unsigned int>(currentIndex), erasedId);

    owner = getCurrentOwner();
    if ( !owner )
        return;
    owner->ApplySelectionToPeer();
    owner->WinUISyncOldSelections();
    owner->InvalidateBestSize();
    if ( owner->HasFlag(wxLB_HSCROLL) )
    {
        owner->m_horizontalExtent = 0;
        // Terminal: owner drawing can destroy the control.
        owner->SetHorizontalExtent();
    }
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter& items,
                             unsigned int pos,
                             void **clientData,
                             wxClientDataType type)
{
    wxCHECK_MSG( pos <= m_itemModel.GetCount(), wxNOT_FOUND,
                 wxT("invalid listbox index") );

#if wxUSE_OWNER_DRAWN
    wxCHECK_MSG( !m_winui ||
                     (!m_winui->creatingOwnerItem &&
                      !m_winui->destroyingOwnerItem),
                 wxNOT_FOUND,
                 wxT("reentrant owner-draw item mutation") );
#endif

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState
            ? callbackState->GetOwner()
            : nullptr;
        return owner && owner == this && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    int last = wxNOT_FOUND;
    for ( unsigned int i = 0; i < items.GetCount(); ++i )
    {
        WinUIEnsurePeerConsistent();
        wxListBox *owner = getCurrentOwner();
        if ( !owner )
            return wxNOT_FOUND;

        const auto change = owner->m_itemModel.Insert(
            items[i],
            owner->IsSorted() ? owner->m_itemModel.GetCount() : pos + i,
            owner->IsSorted());
        unsigned int insertPos =
            static_cast<unsigned int>(change.newIndex);
        const wxWinUIItemModel::Id insertedId = change.id;

#if wxUSE_OWNER_DRAWN
        if ( owner->HasFlag(wxLB_OWNERDRAW) )
        {
            const auto rollbackInserted = [&]()
            {
                wxListBox *rollbackOwner = getCurrentOwner();
                if ( !rollbackOwner )
                    return;

                const std::size_t modelIndex =
                    rollbackOwner->m_itemModel.IndexOf(insertedId);
                if ( modelIndex != wxWinUIItemModel::npos )
                    rollbackOwner->m_itemModel.Erase(modelIndex);

                auto peerIt = std::find(
                    implementation->peerItemIds.begin(),
                    implementation->peerItemIds.end(),
                    insertedId);
                if ( peerIt != implementation->peerItemIds.end() )
                {
                    const std::size_t peerIndex = static_cast<std::size_t>(
                        peerIt - implementation->peerItemIds.begin());
                    wxWinUIPeerMutationGuard guard(
                        rollbackOwner->m_updatingPeer);
                    try
                    {
                        implementation->RevokeCheckBinding(insertedId);
                        const auto peerItems =
                            implementation->listView.Items();
                        if ( peerIndex < peerItems.Size() )
                            peerItems.RemoveAt(
                                static_cast<uint32_t>(peerIndex));
                        implementation->peerItemIds.erase(peerIt);
                        implementation->NotePeerItemsChanged();
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(
                            "WinUI ListBox owner item rollback", e);
                        implementation->peerItemsValid = false;
                        return;
                    }
                }

                try
                {
                    implementation->peerItemsValid =
                        implementation->listView.Items().Size() ==
                            rollbackOwner->m_itemModel.GetCount() &&
                        implementation->peerItemIds.size() ==
                            rollbackOwner->m_itemModel.GetCount();
                }
                catch ( const winrt::hresult_error& )
                {
                    implementation->peerItemsValid = false;
                }
                implementation->renderingOwnerItems.erase(insertedId);
            };

            wxOwnerDrawn *newItem = nullptr;
            try
            {
                {
                    wxWinUIOwnerItemCreationGuard creationGuard(
                        implementation, callbackState);
                    newItem = owner->CreateLboxItem(insertPos);
                }

                wxListBox *liveOwner = getCurrentOwner();
                if ( !liveOwner )
                {
                    delete newItem;
                    return wxNOT_FOUND;
                }

                const std::size_t currentIndex =
                    liveOwner->m_itemModel.IndexOf(insertedId);
                if ( currentIndex == wxWinUIItemModel::npos ||
                     liveOwner->m_aItems.GetCount() + 1 !=
                         liveOwner->m_itemModel.GetCount() )
                {
                    delete newItem;
                    newItem = nullptr;

                    // An application destructor is another callback boundary.
                    // Roll back only after proving that this is still live.
                    if ( !getCurrentOwner() )
                        return wxNOT_FOUND;
                    rollbackInserted();
                    return wxNOT_FOUND;
                }

                if ( !newItem )
                    newItem = new wxWinUIListBoxItem(liveOwner);
                newItem->SetFont(liveOwner->GetFont());
                liveOwner->m_aItems.Insert(newItem, currentIndex);
                newItem = nullptr; // m_aItems owns it now.
                insertPos = static_cast<unsigned int>(currentIndex);
            }
            catch ( ... )
            {
                wxListBox *liveOwner = getCurrentOwner();
                if ( liveOwner && newItem )
                {
                    const int existing =
                        liveOwner->m_aItems.Index(newItem);
                    if ( existing != wxNOT_FOUND )
                        liveOwner->m_aItems.RemoveAt(existing);
                }

                delete newItem;
                newItem = nullptr;

                // Preserve the application exception, but first restore the
                // model/peer transaction if the item destructor left us live.
                if ( getCurrentOwner() )
                    rollbackInserted();
                throw;
            }
        }
#endif

        owner = getCurrentOwner();
        if ( !owner )
            return wxNOT_FOUND;
        owner->AssignNewItemClientData(insertPos, clientData, i, type);
        owner->WinUIInsertPeerItem(insertPos);

        owner = getCurrentOwner();
        if ( !owner )
            return wxNOT_FOUND;
        const std::size_t currentIndex =
            owner->m_itemModel.IndexOf(insertedId);
        if ( currentIndex == wxWinUIItemModel::npos )
            return wxNOT_FOUND;
        last = static_cast<int>(currentIndex);
    }

    wxListBox * const owner = getCurrentOwner();
    if ( !owner )
        return wxNOT_FOUND;
    owner->ApplySelectionToPeer();
    owner->WinUISyncOldSelections();
    owner->InvalidateBestSize();
    if ( owner->HasFlag(wxLB_HSCROLL) )
        owner->SetHorizontalExtent();
    return last;
}

void wxListBox::DoSetItemClientData(unsigned int n, void* clientData)
{
    wxCHECK_RET( n < m_itemModel.GetCount(), wxT("invalid listbox index") );
    m_itemModel.At(n).clientData = clientData;
}

void* wxListBox::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG( n < m_itemModel.GetCount(), nullptr,
                 wxT("invalid listbox index") );
    return m_itemModel.At(n).clientData;
}

wxSize wxListBox::DoGetBestSize() const
{
    wxCoord width = 0;
    wxCoord itemHeight = GetCharHeight();
    for ( std::size_t n = 0; n < m_itemModel.GetCount(); ++n )
    {
        wxCoord textWidth = 0;
        wxCoord textHeight = 0;
        GetTextExtent(
            m_itemModel.At(n).text, &textWidth, &textHeight);
        width = wxMax(width, textWidth);
        itemHeight = wxMax(itemHeight, textHeight);
    }

    const wxSize chrome =
        FromDIP(wxSize(WinUIIsCheckable() ? 64 : 40, 12));
    const wxSize minimum = FromDIP(wxSize(140, 90));
    const int visibleItems =
        wxMin(wxMax(static_cast<int>(m_itemModel.GetCount()), 3), 10);
    return wxSize(
        wxMax(width + chrome.x, minimum.x),
        wxMax(itemHeight * visibleItems + chrome.y, minimum.y));
}

void wxListBox::WinUIApplyControlSizeToPeer()
{
    if ( !m_winui || !m_winui->listView )
        return;

    try
    {
        if ( std::isfinite(m_controlWidthDIPs) &&
             m_controlWidthDIPs > 0.0 )
        {
            m_winui->listView.Width(m_controlWidthDIPs);
            m_winui->listView.HorizontalAlignment(
                MUX::HorizontalAlignment::Left);
        }
        if ( std::isfinite(m_controlHeightDIPs) &&
             m_controlHeightDIPs > 0.0 )
        {
            m_winui->listView.Height(m_controlHeightDIPs);
            m_winui->listView.VerticalAlignment(
                MUX::VerticalAlignment::Top);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox control size projection", e);
    }
}

void wxListBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    const double scale = wxWinUIListScale(this);
    if ( width != wxDefaultCoord && width >= 0 )
        m_controlWidthDIPs = width / scale;
    if ( height != wxDefaultCoord && height >= 0 )
        m_controlHeightDIPs = height / scale;

    // Every wx SetSize writer, including a parent sizer/layout pass, is
    // authoritative. The XAML peer lives in a shared island, but this wx
    // control still owns an ordinary child HWND and its logical geometry must
    // follow the same last-writer rule as the other ports.
    const wxWeakRef<wxWindow> self(this);
    wxControl::DoSetSize(x, y, width, height, sizeFlags);
    wxListBox * const owner = wxDynamicCast(self.get(), wxListBox);
    if ( !owner )
        return;

    owner->WinUIApplyControlSizeToPeer();
}

void wxListBox::DoSetClientSize(int width, int height)
{
    const double scale = wxWinUIListScale(this);
    if ( width >= 0 )
        m_controlWidthDIPs = width / scale;
    if ( height >= 0 )
        m_controlHeightDIPs = height / scale;

    const wxWeakRef<wxWindow> self(this);
    wxControl::DoSetClientSize(width, height);
    wxListBox * const owner = wxDynamicCast(self.get(), wxListBox);
    if ( !owner )
        return;

    owner->WinUIApplyControlSizeToPeer();
}

void wxListBox::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);

    if ( m_winui && m_winui->listView )
    {
        {
            // Suppress synchronous focus/theme/size callbacks until this
            // RAII object is gone; owner drawing runs below without it.
            wxWinUIPeerMutationGuard guard(m_updatingPeer);
            try
            {
                m_winui->listView.IsEnabled(enable);
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("WinUI ListBox enabled state", e);
            }
        }
    }

#if wxUSE_OWNER_DRAWN
    if ( HasFlag(wxLB_OWNERDRAW) )
        WinUIRefreshItems();
#endif
}

bool wxListBox::MSWOnEffectiveLayoutDirectionChanged()
{
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    if ( !implementation || !callbackState || !implementation->listView )
        return true;

    try
    {
        // FlowDirection can synchronously cause layout/focus notifications.
        // Suppress their presentation refresh until the XAML write is fully
        // committed, then render owner-drawn content without the guard.
        {
            wxWinUIPeerMutationGuard guard(m_updatingPeer);
            implementation->listView.FlowDirection(
                GetLayoutDirection() == wxLayout_RightToLeft
                    ? MUX::FlowDirection::RightToLeft
                    : MUX::FlowDirection::LeftToRight);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI ListBox layout direction", e);
        return false;
    }

    wxListBox * const owner = callbackState->GetOwner();
    if ( !owner || owner != this || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != callbackState )
    {
        return false;
    }

    // Terminal: a user owner-draw callback may destroy this control.
    owner->WinUIRefreshItems();
    return true;
}

void wxListBox::WinUIOnItemToggled(wxWinUIItemModel::Id id, bool check)
{
    const std::size_t index = m_itemModel.IndexOf(id);
    if ( index != wxWinUIItemModel::npos )
        m_itemModel.At(index).checked = check;
}

bool wxListBox::WinUIHandleCheckKey(int virtualKey,
                                    bool shiftDown,
                                    std::uintptr_t keyboardLayout,
                                    bool useLiveKeyboardState)
{
    enum class Operation
    {
        None,
        Toggle,
        Set,
        Clear
    };

    Operation operation = Operation::None;
    switch ( virtualKey )
    {
        case static_cast<int>(
            winrt::Windows::System::VirtualKey::Space):
            operation = Operation::Toggle;
            break;

        case static_cast<int>(
            winrt::Windows::System::VirtualKey::Add):
            operation = Operation::Set;
            break;

        case static_cast<int>(
            winrt::Windows::System::VirtualKey::Subtract):
            operation = Operation::Clear;
            break;

        default:
            break;
    }

    if ( operation == Operation::None )
    {
        BYTE keyboardState[256]{};
        if ( useLiveKeyboardState && !::GetKeyboardState(keyboardState) )
            return false;

        // The routed key status doesn't expose the translated character. Keep
        // the full live modifier state for AltGr layouts, but make Shift match
        // the state observed for this routed event. Tests inject a clean state.
        keyboardState[VK_SHIFT] = shiftDown ? 0x80 : 0;
        keyboardState[VK_LSHIFT] = shiftDown ? 0x80 : 0;
        keyboardState[VK_RSHIFT] = 0;

        const HKL layout = keyboardLayout
            ? reinterpret_cast<HKL>(keyboardLayout)
            : ::GetKeyboardLayout(0);
        if ( !layout )
            return false;

        const UINT scanCode = ::MapVirtualKeyExW(
            static_cast<UINT>(virtualKey), MAPVK_VK_TO_VSC_EX, layout);
        wchar_t translated[4]{};
        const int translatedCount = ::ToUnicodeEx(
            static_cast<UINT>(virtualKey),
            scanCode,
            keyboardState,
            translated,
            static_cast<int>(WXSIZEOF(translated)),
            0x4, // Don't mutate the keyboard layout's dead-key state.
            layout);

        // wxMSW dispatches the translated wxKeyEvent character, not a US OEM
        // key identity. Requiring one scalar also fails closed for dead keys.
        if ( translatedCount == 1 )
        {
            if ( translated[0] == L'+' )
                operation = Operation::Set;
            else if ( translated[0] == L'-' )
                operation = Operation::Clear;
        }
    }

    if ( operation == Operation::None )
        return false;

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    if ( !implementation || !callbackState || !WinUIIsCheckable() ||
         m_updatingPeer )
    {
        return false;
    }

    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState->GetOwner();
        return owner && owner == this && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    // MSW snapshots all selected indices in a multiple-selection control but
    // uses GetSelection() for a single-selection control. IDs preserve that
    // exact set even if an event sorts, inserts or deletes rows.
    std::vector<wxWinUIItemModel::Id> selectedIds;
    if ( HasMultipleSelection() )
    {
        selectedIds.reserve(m_itemModel.GetCount());
        for ( std::size_t n = 0; n < m_itemModel.GetCount(); ++n )
        {
            if ( m_itemModel.At(n).selected )
                selectedIds.push_back(m_itemModel.At(n).id);
        }
    }
    else
    {
        const int selection = GetSelection();
        if ( selection != wxNOT_FOUND )
        {
            selectedIds.push_back(
                m_itemModel.At(static_cast<std::size_t>(selection)).id);
        }
    }

    for ( const wxWinUIItemModel::Id id : selectedIds )
    {
        wxListBox *owner = getCurrentOwner();
        if ( !owner )
            return true;

        const std::size_t index = owner->m_itemModel.IndexOf(id);
        if ( index == wxWinUIItemModel::npos )
            continue;

        const bool checked = owner->m_itemModel.At(index).checked;
        bool newChecked = checked;
        switch ( operation )
        {
            case Operation::Toggle:
                newChecked = !checked;
                break;

            case Operation::Set:
                newChecked = true;
                break;

            case Operation::Clear:
                newChecked = false;
                break;

            case Operation::None:
                break;
        }

        // This virtual hook redraws the retained peer first and publishes the
        // exact current index. The event is an arbitrary application callback.
        owner->WinUIOnItemToggled(id, newChecked);

        // Revalidate after every event before looking at the next snapshot ID.
        if ( !getCurrentOwner() )
            return true;
    }

    // Just like MSW, supported keys are consumed even with no selection.
    return true;
}

void wxListBox::WinUISendDoubleClick(wxWinUIItemModel::Id id)
{
    const std::size_t index = m_itemModel.IndexOf(id);
    if ( index == wxWinUIItemModel::npos )
        return;

    SendEvent(wxEVT_LISTBOX_DCLICK,
              static_cast<int>(index),
              m_itemModel.At(index).selected);
}

void wxListBox::WinUIRefreshItems()
{
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation ? implementation->callbackState : nullptr;
    if ( !implementation || !callbackState )
        return;

    WinUIEnsurePeerConsistent();
    wxListBox *owner = callbackState->GetOwner();
    if ( !owner || owner != this || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }

    if ( owner->HasFlag(wxLB_HSCROLL) )
    {
        (void)owner->WinUIEnsureHorizontalPresentation();
        owner = callbackState->GetOwner();
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState )
        {
            return;
        }
    }

    const std::uint64_t revision = owner->m_winui->peerRevision;
    const unsigned int count = owner->GetCount();
    std::vector<unsigned int> refreshOrder;
    refreshOrder.reserve(count);
    for ( unsigned int n = 0; n < count; ++n )
    {
        if ( !owner->m_itemModel.At(n).selected )
            refreshOrder.push_back(n);
    }
    for ( unsigned int n = 0; n < count; ++n )
    {
        if ( owner->m_itemModel.At(n).selected )
            refreshOrder.push_back(n);
    }

    // Batch invalidations render selected rows last. Besides matching their
    // visual priority, this guarantees that a theme/size refresh leaves the
    // most relevant owner-draw status observable after the batch.
    for ( const unsigned int n : refreshOrder )
    {
        owner->WinUIUpdatePeerItem(n);

        owner = callbackState->GetOwner();
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerRevision != revision ||
             !owner->m_winui->peerItemsValid ||
             owner->GetCount() != count )
        {
            return;
        }
    }
    owner->WinUIEnsurePeerConsistent();
}

void wxListBox::WinUIEnsurePeerConsistent()
{
    if ( !m_winui || !m_winui->listView )
        return;

    bool valid = m_winui->peerItemsValid;
    try
    {
        const std::size_t count = m_itemModel.GetCount();
        valid = valid &&
                m_winui->listView.Items().Size() == count &&
                m_winui->peerItemIds.size() == count;
        for ( std::size_t n = 0; valid && n < count; ++n )
            valid = m_winui->peerItemIds[n] == m_itemModel.At(n).id;
    }
    catch ( const winrt::hresult_error& )
    {
        valid = false;
    }

    if ( !valid )
        ApplyItemsToPeer();
}

void wxListBox::ApplyItemsToPeer()
{
    if ( !m_winui || !m_winui->listView )
        return;

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation->callbackState;

    {
        wxWinUIPeerMutationGuard guard(m_updatingPeer);
        m_winui->peerItemsValid = false;
        try
        {
            for ( auto& entry : m_winui->checkBindings )
                m_winui->RevokeCheckBinding(entry.second);
            m_winui->checkBindings.clear();

            const auto items = m_winui->listView.Items();
            while ( items.Size() )
                items.RemoveAtEnd();
            m_winui->peerItemIds.clear();

            for ( unsigned int n = 0; n < GetCount(); ++n )
            {
                MUXC::ListViewItem item;
                item.HorizontalContentAlignment(
                    HasFlag(wxLB_HSCROLL)
                        ? MUX::HorizontalAlignment::Left
                        : MUX::HorizontalAlignment::Stretch);
                items.Append(item);
                m_winui->peerItemIds.push_back(m_itemModel.At(n).id);
            }
            m_winui->NotePeerItemsChanged();

            m_winui->peerItemsValid =
                items.Size() == m_itemModel.GetCount() &&
                m_winui->peerItemIds.size() == m_itemModel.GetCount();
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ListBox item recovery", e);
            m_winui->peerItemIds.clear();
            m_winui->NotePeerItemsChanged();
            m_winui->peerItemsValid = false;
        }
    }

    if ( !m_winui->peerItemsValid )
        return;

    // Owner-draw callbacks are arbitrary user code. They must run after the
    // mutation guard has unwound, otherwise deleting the wx control from a
    // callback would leave the guard referring to freed memory.
    const std::uint64_t revision = m_winui->peerRevision;
    const unsigned int count = GetCount();
    for ( unsigned int n = 0; n < count; ++n )
    {
        WinUIUpdatePeerItem(n);

        wxListBox * const owner = callbackState->GetOwner();
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState ||
             owner->m_winui->peerRevision != revision ||
             !owner->m_winui->peerItemsValid )
        {
            return;
        }
    }

    ApplySelectionToPeer();
}

void wxListBox::ApplySelectionToPeer()
{
    if ( !m_winui || !m_winui->listView )
        return;
    if ( !m_winui->peerItemsValid )
        return;

    wxWinUIPeerMutationGuard guard(m_updatingPeer);
    try
    {
        const MUXC::ListView listView = m_winui->listView;
        const auto items = m_winui->listView.Items();
        const uint32_t count = items.Size();
        if ( HasMultipleSelection() )
        {
            // Selection belongs to the Selector model, not to a detached
            // ListViewItem DP. Mutating IsSelected on an object stored in
            // Items can leave SelectedItems (and UIA ISelectionProvider)
            // empty even though the item appears selected.
            const auto selectedItems = listView.SelectedItems();
            selectedItems.Clear();
            for ( uint32_t n = 0; n < count; ++n )
            {
                if ( n >= m_winui->peerItemIds.size() )
                    continue;
                const std::size_t modelIndex =
                    m_itemModel.IndexOf(m_winui->peerItemIds[n]);
                if ( modelIndex != wxWinUIItemModel::npos &&
                     m_itemModel.At(modelIndex).selected )
                {
                    selectedItems.Append(items.GetAt(n));
                }
            }
        }
        else
        {
            winrt::Windows::Foundation::IInspectable selected{ nullptr };
            for ( uint32_t n = 0; n < count; ++n )
            {
                if ( n >= m_winui->peerItemIds.size() )
                    continue;
                const std::size_t modelIndex =
                    m_itemModel.IndexOf(m_winui->peerItemIds[n]);
                if ( modelIndex != wxWinUIItemModel::npos &&
                     m_itemModel.At(modelIndex).selected )
                {
                    selected = items.GetAt(n);
                    break;
                }
            }
            listView.SelectedItem(selected);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox selection update", e);
    }
}

void wxListBox::WinUIInsertPeerItem(unsigned int n)
{
    if ( !m_winui || !m_winui->listView ||
         n >= m_itemModel.GetCount() )
    {
        return;
    }

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() + 1 ==
            m_itemModel.GetCount();
    try
    {
        peerReady = peerReady &&
                    m_winui->listView.Items().Size() ==
                        m_winui->peerItemIds.size();
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    bool failed = false;
    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation->callbackState;
    const wxWinUIItemModel::Id id = m_itemModel.At(n).id;
    {
        wxWinUIPeerMutationGuard guard(m_updatingPeer);
        try
        {
            MUXC::ListViewItem item;
            item.HorizontalContentAlignment(
                HasFlag(wxLB_HSCROLL)
                    ? MUX::HorizontalAlignment::Left
                    : MUX::HorizontalAlignment::Stretch);
            const auto items = m_winui->listView.Items();
            items.InsertAt(n, item);
            m_winui->peerItemIds.insert(
                m_winui->peerItemIds.begin() + n,
                m_itemModel.At(n).id);
            m_winui->NotePeerItemsChanged();
            m_winui->peerItemsValid =
                items.Size() == m_itemModel.GetCount() &&
                m_winui->peerItemIds.size() ==
                    m_itemModel.GetCount();
            failed = !m_winui->peerItemsValid;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ListBox item insertion", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

    }

    if ( failed )
    {
        ApplyItemsToPeer();
        return;
    }

    const std::uint64_t revision = m_winui->peerRevision;
    WinUIUpdatePeerItem(n);

    wxListBox * const owner = callbackState->GetOwner();
    if ( !owner || owner != this || !owner->m_winui ||
         owner->m_winui.get() != implementation ||
         owner->m_winui->callbackState != callbackState )
    {
        return;
    }
    if ( owner->m_winui->peerRevision != revision ||
         n >= owner->m_itemModel.GetCount() ||
         owner->m_itemModel.At(n).id != id ||
         !owner->m_winui->peerItemsValid )
    {
        owner->WinUIEnsurePeerConsistent();
    }
}

void wxListBox::WinUIErasePeerItem(unsigned int n,
                                   wxWinUIItemModel::Id id)
{
    if ( !m_winui || !m_winui->listView )
        return;

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() ==
            m_itemModel.GetCount() + 1;
    try
    {
        peerReady = peerReady &&
                    m_winui->listView.Items().Size() ==
                        m_winui->peerItemIds.size();
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_updatingPeer);
        try
        {
            m_winui->RevokeCheckBinding(id);
            const auto items = m_winui->listView.Items();
            if ( n >= items.Size() ||
                 n >= m_winui->peerItemIds.size() )
            {
                m_winui->peerItemsValid = false;
                failed = true;
            }
            else
            {
                items.RemoveAt(n);
                m_winui->peerItemIds.erase(
                    m_winui->peerItemIds.begin() + n);
                m_winui->NotePeerItemsChanged();
                m_winui->peerItemsValid =
                    items.Size() == m_itemModel.GetCount() &&
                    m_winui->peerItemIds.size() ==
                        m_itemModel.GetCount();
                failed = !m_winui->peerItemsValid;
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ListBox item removal", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxListBox::WinUIMovePeerItem(unsigned int oldIndex,
                                  unsigned int newIndex)
{
    if ( !m_winui || !m_winui->listView ||
         oldIndex == newIndex )
    {
        return;
    }

    bool peerReady =
        m_winui->peerItemsValid &&
        m_winui->peerItemIds.size() == m_itemModel.GetCount();
    try
    {
        peerReady = peerReady &&
                    m_winui->listView.Items().Size() ==
                        m_itemModel.GetCount() &&
                    oldIndex < m_winui->peerItemIds.size() &&
                    newIndex < m_itemModel.GetCount() &&
                    m_winui->peerItemIds[oldIndex] ==
                        m_itemModel.At(newIndex).id;
    }
    catch ( const winrt::hresult_error& )
    {
        peerReady = false;
    }
    if ( !peerReady )
    {
        ApplyItemsToPeer();
        return;
    }

    {
        bool failed = false;
        wxWinUIPeerMutationGuard guard(m_updatingPeer);
        try
        {
            const auto items = m_winui->listView.Items();
            const auto item = items.GetAt(oldIndex);
            const auto itemControl = item.try_as<MUXC::Control>();
            const MUX::FocusState focusState =
                itemControl ? itemControl.FocusState()
                            : MUX::FocusState::Unfocused;
            items.RemoveAt(oldIndex);
            items.InsertAt(newIndex, item);

            const wxWinUIItemModel::Id id =
                m_winui->peerItemIds.at(oldIndex);
            m_winui->peerItemIds.erase(
                m_winui->peerItemIds.begin() + oldIndex);
            m_winui->peerItemIds.insert(
                m_winui->peerItemIds.begin() + newIndex, id);
            m_winui->NotePeerItemsChanged();
            m_winui->peerItemsValid =
                items.Size() == m_itemModel.GetCount() &&
                m_winui->peerItemIds.size() ==
                    m_itemModel.GetCount();
            if ( m_winui->peerItemsValid && itemControl &&
                 focusState != MUX::FocusState::Unfocused )
            {
                itemControl.Focus(focusState);
            }
            failed = !m_winui->peerItemsValid;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI ListBox item move", e);
            m_winui->peerItemsValid = false;
            failed = true;
        }

        if ( !failed )
            return;
    }

    ApplyItemsToPeer();
}

void wxListBox::WinUIUpdatePeerItem(unsigned int n)
{
    if ( !m_winui || !m_winui->listView ||
         n >= m_itemModel.GetCount() )
    {
        return;
    }

    if ( !m_winui->peerItemsValid ||
         n >= m_winui->peerItemIds.size() ||
         m_winui->peerItemIds[n] != m_itemModel.At(n).id )
    {
        return;
    }

    wxWinUIListBoxImpl * const entryImplementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> entryCallbackState =
        entryImplementation->callbackState;
    const wxWinUIItemModel::Id entryId = m_itemModel.At(n).id;
    const std::uint64_t entryRevision = entryImplementation->peerRevision;

    try
    {
        const auto items = m_winui->listView.Items();
        if ( n >= items.Size() )
        {
            m_winui->peerItemsValid = false;
            return;
        }

        const MUXC::ListViewItem item =
            items.GetAt(n).as<MUXC::ListViewItem>();
        const wxWinUIItemModel::Id id = m_itemModel.At(n).id;
        const auto entryPeerIdentity = wxWinUIListObjectIdentity(item);
        const double initialScale = wxWinUIListScale(this);
        double itemHeightDips = wxMax(
            1.0,
            static_cast<double>(wxMax(
                1,
                GetCharHeight() +
                    2 * wxWinUIListBoxExtraSpacePixels)) / initialScale);

#if wxUSE_OWNER_DRAWN
        wxBitmap ownerDrawBitmap;
        wxSize ownerDrawSize;
        bool hasOwnerDrawProjection = false;
        if ( HasFlag(wxLB_OWNERDRAW) && n < m_aItems.GetCount() )
        {
            if ( m_winui->renderingOwnerItems.find(id) !=
                 m_winui->renderingOwnerItems.end() )
            {
                return;
            }

            wxWinUIListBoxImpl * const implementation = m_winui.get();
            const std::shared_ptr<wxWinUIListBoxCallbackState>
                callbackState = implementation->callbackState;
            const std::uint64_t revision = implementation->peerRevision;
            wxOwnerDrawn * const ownerDrawItem = m_aItems[n];
            const auto peerIdentity = wxWinUIListObjectIdentity(item);
            implementation->renderingOwnerItems.insert(id);

            const auto getCurrentOwner = [&]() -> wxListBox *
            {
                wxListBox * const owner = callbackState->GetOwner();
                if ( !owner || owner != this || !owner->m_winui ||
                     owner->m_winui.get() != implementation ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->peerRevision != revision ||
                     !owner->m_winui->peerItemsValid ||
                     n >= owner->m_itemModel.GetCount() ||
                     n >= owner->m_aItems.GetCount() ||
                     owner->m_itemModel.At(n).id != id ||
                     owner->m_aItems[n] != ownerDrawItem )
                {
                    return nullptr;
                }

                try
                {
                    const auto currentItems =
                        owner->m_winui->listView.Items();
                    if ( n >= currentItems.Size() ||
                         wxWinUIListObjectIdentity(currentItems.GetAt(n)) !=
                             peerIdentity )
                    {
                        return nullptr;
                    }
                }
                catch ( const winrt::hresult_error& )
                {
                    return nullptr;
                }
                return owner;
            };

            const auto releaseRenderSlot = [&]()
            {
                wxListBox * const owner = callbackState->GetOwner();
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == implementation &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->m_winui->renderingOwnerItems.erase(id);
                }
            };

            size_t measuredWidth = 0;
            size_t ignoredMeasuredHeight = 0;
            ownerDrawItem->OnMeasureItem(
                &measuredWidth, &ignoredMeasuredHeight);
            wxListBox *liveOwner = getCurrentOwner();
            if ( !liveOwner )
            {
                releaseRenderSlot();
                return;
            }

            const double currentScale = wxWinUIListScale(liveOwner);
            const double projectionScale =
                std::isfinite(liveOwner->m_ownerDrawProjectionScale) &&
                    liveOwner->m_ownerDrawProjectionScale > 0.0
                    ? liveOwner->m_ownerDrawProjectionScale
                    : currentScale;

            const int extra = 2 * wxWinUIListBoxExtraSpacePixels;
            const auto pixelsToDips = [currentScale](double value)
            {
                return wxMax(1.0, value / currentScale);
            };

            double widthDips = pixelsToDips(
                static_cast<double>(measuredWidth) + extra);
            if ( liveOwner->HasFlag(wxLB_HSCROLL) )
            {
                widthDips = wxMax(
                    widthDips,
                    pixelsToDips(
                        liveOwner->WinUIGetHorizontalExtentPixels()));
            }
            else
            {
                // Draw the retained row to the current logical wx control
                // width. DoSetSize()/DoSetClientSize() record every genuine
                // user or parent-layout writer in DIPs, while the XAML
                // ListView's ActualWidth remains a presentation observation.
                const double controlWidthDips =
                    liveOwner->m_controlWidthDIPs;
                widthDips = wxMax(
                    widthDips,
                    std::isfinite(controlWidthDips) &&
                            controlWidthDips > 0.0
                        ? controlWidthDips
                        : widthDips);
            }

            // wxMSW creates ListBox with LBS_OWNERDRAWFIXED and sets the
            // common line height to GetCharHeight() + 2 physical pixels. A
            // per-item OnMeasureItem() height is not allowed to turn this
            // into the unsupported LBS_OWNERDRAWVARIABLE contract. The
            // callback remains useful for retained bitmap width only.
            const double heightDips = pixelsToDips(
                liveOwner->GetCharHeight() + extra);
            itemHeightDips = heightDips;
            const auto dipsToPixels = [projectionScale](double value)
            {
                const double scaled = value * projectionScale;
                if ( !std::isfinite(scaled) ||
                     scaled >= std::numeric_limits<int>::max() )
                {
                    return std::numeric_limits<int>::max();
                }
                return wxMax(1, static_cast<int>(std::lround(scaled)));
            };
            const int width = dipsToPixels(widthDips);
            const int height = dipsToPixels(heightDips);

            ownerDrawBitmap = wxBitmap(width, height, 32);
            if ( ownerDrawBitmap.IsOk() )
            {
                // wxMemoryDC scales fonts to the PPI carried by its selected
                // bitmap. The pixels above are device pixels, but the default
                // wxBitmap metadata is 1x; leaving it unchanged makes
                // dc.GetFont() a 96-PPI-adjusted copy at a 2x XAML root. Mark
                // the actual control scale before selecting the wx font, just
                // as the HWND DRAWITEM DC uses the control's current DPI.
                ownerDrawBitmap.SetScaleFactor(currentScale);
                wxMemoryDC dc(ownerDrawBitmap);
                // MSW selects the control font into the owner-draw DC before
                // invoking DRAWITEM. Do the same independently of whether a
                // custom item mirrored SetFont() into its own state.
                dc.SetFont(liveOwner->GetFont());
                dc.SetBackground(wxBrush(
                    wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX)));
                dc.Clear();
                dc.SetLayoutDirection(liveOwner->GetLayoutDirection());

                int status = wxOwnerDrawn::wxODHidePrefix;
                if ( liveOwner->m_itemModel.At(n).selected )
                    status |= wxOwnerDrawn::wxODSelected;
                if ( !liveOwner->IsEnabled() )
                    status |= wxOwnerDrawn::wxODDisabled;
                if ( item.FocusState() != MUX::FocusState::Unfocused ||
                     (liveOwner->m_itemModel.At(n).selected &&
                      liveOwner->m_winui->listView.FocusState() !=
                          MUX::FocusState::Unfocused) )
                {
                    status |= wxOwnerDrawn::wxODHasFocus;
                }

                hasOwnerDrawProjection = ownerDrawItem->OnDrawItem(
                    dc,
                    wxRect(0, 0, width, height),
                    wxOwnerDrawn::wxODDrawAll,
                    static_cast<wxOwnerDrawn::wxODStatus>(status));
                dc.SelectObject(wxNullBitmap);
                ownerDrawSize = wxSize(width, height);
            }

            liveOwner = getCurrentOwner();
            if ( !liveOwner )
            {
                releaseRenderSlot();
                return;
            }
            releaseRenderSlot();
        }
#endif // wxUSE_OWNER_DRAWN

        MUXC::CheckBox pendingFocusTarget{ nullptr };
        MUX::FocusState pendingFocusState = MUX::FocusState::Unfocused;
        winrt::Windows::Foundation::IUnknown
            pendingFocusContentIdentity{ nullptr };
        winrt::Windows::Foundation::IUnknown
            pendingFocusParentIdentity{ nullptr };
        bool pendingFocusDirectContent = false;
        std::optional<wxWinUIPeerMutationGuard> peerMutationGuard;
        peerMutationGuard.emplace(m_updatingPeer);
        // A ListViewItem theme supplies a roughly 32-DIP MinHeight, which is
        // 64 physical pixels on a 2x XamlRoot and is unrelated to the compact
        // LB_GETITEMHEIGHT contract. Override both dimensions on every peer
        // refresh: create/insert, SetFont(), owner measurement and DPI changes
        // all converge through this single guarded projection point.
        item.MinHeight(0.0);
        item.Height(itemHeightDips);
        item.VerticalContentAlignment(MUX::VerticalAlignment::Center);
        MUXA::AutomationProperties::SetName(
            item, wxWinUIToHString(m_itemModel.At(n).text));

        MUXC::CheckBox checkBox{ nullptr };
        if ( WinUIIsCheckable() )
        {
            auto bindingIt = m_winui->checkBindings.find(id);
            if ( bindingIt == m_winui->checkBindings.end() )
            {
                wxWinUIListBoxImpl::CheckBinding binding;
                binding.checkBox = MUXC::CheckBox();
                binding.checkBox.IsTabStop(true);
                binding.checkBox.AllowFocusOnInteraction(true);
                binding.checkBox.Content(winrt::box_value(
                    wxWinUIToHString(m_itemModel.At(n).text)));
                binding.checkBox.IsChecked(m_itemModel.At(n).checked);
                binding.callbackState =
                    std::make_shared<wxWinUIListItemCallbackState>();

                const std::shared_ptr<wxWinUIListBoxCallbackState>
                    callbackState = m_winui->callbackState;
                const std::shared_ptr<wxWinUIListItemCallbackState>
                    itemCallbackState = binding.callbackState;
                const MUX::RoutedEventHandler onToggle{
                    [callbackState, itemCallbackState, id](
                        const winrt::Windows::Foundation::IInspectable&
                            sender,
                        const MUX::RoutedEventArgs&)
                    {
                        if ( !itemCallbackState->IsValid() )
                            return;

                        wxListBox * const owner =
                            callbackState->GetOwner();
                        if ( !owner || !owner->m_winui ||
                             owner->m_winui->callbackState !=
                                 callbackState ||
                             owner->m_updatingPeer )
                        {
                            return;
                        }

                        bool checked = false;
                        try
                        {
                            if ( const auto checkBox =
                                     sender.try_as<MUXC::CheckBox>() )
                            {
                                if ( const auto state =
                                         checkBox.IsChecked() )
                                {
                                    checked = state.Value();
                                }
                            }
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "WinUI CheckListBox toggle callback", e);
                            return;
                        }

                        // The event can destroy owner. Nothing follows this.
                        owner->WinUIOnItemToggled(id, checked);
                    }};
                binding.checkedToken =
                    binding.checkBox.Checked(onToggle);
                binding.uncheckedToken =
                    binding.checkBox.Unchecked(onToggle);
                bindingIt = m_winui->checkBindings.emplace(
                    id, std::move(binding)).first;
            }
            else
            {
                bindingIt->second.checkBox.Content(
                    winrt::box_value(
                        wxWinUIToHString(m_itemModel.At(n).text)));
                bindingIt->second.checkBox.IsChecked(
                    m_itemModel.At(n).checked);
            }

            checkBox = bindingIt->second.checkBox;
            wxWinUIApplyFont(checkBox, GetFont());
        }

        winrt::Windows::Foundation::IInspectable content{ nullptr };

#if wxUSE_OWNER_DRAWN
        if ( hasOwnerDrawProjection && ownerDrawBitmap.IsOk() )
        {
            const double scale =
                std::isfinite(m_ownerDrawProjectionScale) &&
                    m_ownerDrawProjectionScale > 0.0
                    ? m_ownerDrawProjectionScale
                    : wxWinUIListScale(this);
            // Retain the existing owner-draw root whenever possible. Moving a
            // focused CheckBox to a replacement Grid necessarily clears XAML
            // focus; replacing only the bitmap preserves both peer identity
            // and the authentic pointer focus state across redraws.
            MUXC::Grid grid = item.Content().try_as<MUXC::Grid>();
            if ( !grid )
                grid = MUXC::Grid();
            grid.Tag(winrt::box_value(
                winrt::hstring(L"wxWinUIListOwnerDrawGrid")));
            grid.Width(ownerDrawSize.x / scale);
            grid.Height(ownerDrawSize.y / scale);
            if ( HasFlag(wxLB_HSCROLL) )
            {
                wxWinUIApplyListHorizontalExtent(
                    m_winui->listView, item, ownerDrawSize.x / scale);
            }

            MUXC::Image image;
            image.Tag(winrt::box_value(
                winrt::hstring(L"wxWinUIListOwnerDrawBitmap")));
            image.Source(wxWinUIWriteableBitmapFromBitmap(
                ownerDrawBitmap));
            image.Stretch(MUXM::Stretch::Fill);
            image.Width(ownerDrawSize.x / scale);
            image.Height(ownerDrawSize.y / scale);
            image.IsHitTestVisible(false);
            const MUXC::UIElementCollection gridChildren = grid.Children();
            for ( uint32_t childIndex = gridChildren.Size();
                  childIndex > 0;
                  --childIndex )
            {
                if ( gridChildren.GetAt(childIndex - 1)
                         .try_as<MUXC::Image>() )
                {
                    gridChildren.RemoveAt(childIndex - 1);
                }
            }
            gridChildren.InsertAt(0, image);

            if ( checkBox )
            {
                int overlayWidth = GetCharHeight() + FromDIP(8);
                if ( n < m_aItems.GetCount() )
                {
                    overlayWidth = wxMax(
                        overlayWidth,
                        m_aItems[n]->GetMarginWidth());
                }
                checkBox.Content(nullptr);
                checkBox.Opacity(0.0);
                checkBox.Width(overlayWidth / scale);
                checkBox.Height(ownerDrawSize.y / scale);
                checkBox.HorizontalAlignment(
                    GetLayoutDirection() == wxLayout_RightToLeft
                        ? MUX::HorizontalAlignment::Right
                        : MUX::HorizontalAlignment::Left);
                MUXA::AutomationProperties::SetName(
                    checkBox,
                    wxWinUIToHString(m_itemModel.At(n).text));
                bool checkAlreadyAttached = false;
                const auto checkIdentity =
                    wxWinUIListObjectIdentity(checkBox);
                for ( const MUX::UIElement& child : gridChildren )
                {
                    if ( wxWinUIListObjectIdentity(child) == checkIdentity )
                    {
                        checkAlreadyAttached = true;
                        break;
                    }
                }

                if ( !checkAlreadyAttached )
                {
                    const MUX::FocusState checkFocusState =
                        checkBox.FocusState();
                    if ( wxWinUIListObjectIdentity(item.Content()) ==
                         checkIdentity )
                    {
                        // A direct Content child is parented through the
                        // ContentPresenter, not a Panel, so detach it through
                        // the owning ContentControl before adding it to Grid.
                        item.Content(nullptr);
                    }
                    else
                    {
                        wxWinUIDetachListChild(checkBox);
                    }
                    gridChildren.Append(checkBox);
                    if ( checkFocusState != MUX::FocusState::Unfocused )
                    {
                        pendingFocusTarget = checkBox;
                        pendingFocusState = checkFocusState;
                        pendingFocusContentIdentity =
                            wxWinUIListObjectIdentity(grid);
                        pendingFocusParentIdentity =
                            wxWinUIListObjectIdentity(grid);
                    }
                }
            }

            item.Padding(MUX::Thickness{});
            content = grid;
        }

        if ( !content && checkBox && HasFlag(wxLB_OWNERDRAW) )
        {
            // A CheckListBox keeps one stable visual parent for its semantic
            // CheckBox even when an application owner-draw callback declines
            // to provide a bitmap. Reparenting the focused CheckBox between a
            // direct ContentPresenter child and the owner-draw Grid lets
            // ListView's final layout move focus back to its container after
            // even a low-priority Focus() continuation. Removing only the
            // bitmap preserves the authentic XAML focus/UIA peer instead.
            MUXC::Grid grid = item.Content().try_as<MUXC::Grid>();
            if ( !grid )
                grid = MUXC::Grid();
            grid.Tag(winrt::box_value(
                winrt::hstring(L"wxWinUIListOwnerDrawGrid")));
            grid.ClearValue(MUX::FrameworkElement::WidthProperty());
            grid.ClearValue(MUX::FrameworkElement::HeightProperty());
            grid.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);

            const MUXC::UIElementCollection gridChildren = grid.Children();
            for ( uint32_t childIndex = gridChildren.Size();
                  childIndex > 0;
                  --childIndex )
            {
                if ( gridChildren.GetAt(childIndex - 1)
                         .try_as<MUXC::Image>() )
                {
                    gridChildren.RemoveAt(childIndex - 1);
                }
            }

            const auto checkIdentity =
                wxWinUIListObjectIdentity(checkBox);
            bool checkAlreadyAttached = false;
            for ( const MUX::UIElement& child : gridChildren )
            {
                if ( wxWinUIListObjectIdentity(child) == checkIdentity )
                {
                    checkAlreadyAttached = true;
                    break;
                }
            }
            if ( !checkAlreadyAttached )
            {
                const MUX::FocusState checkFocusState =
                    checkBox.FocusState();
                // This also handles a peer created by an older projection
                // path before its first refresh. New peers start in this Grid
                // and never cross a visual-parent boundary afterwards.
                if ( wxWinUIListObjectIdentity(item.Content()) ==
                     checkIdentity )
                {
                    item.Content(nullptr);
                }
                else
                {
                    wxWinUIDetachListChild(checkBox);
                }
                gridChildren.Append(checkBox);
                if ( checkFocusState != MUX::FocusState::Unfocused )
                {
                    pendingFocusTarget = checkBox;
                    pendingFocusState = checkFocusState;
                    pendingFocusContentIdentity =
                        wxWinUIListObjectIdentity(grid);
                    pendingFocusParentIdentity =
                        wxWinUIListObjectIdentity(grid);
                }
            }

            checkBox.Opacity(1.0);
            checkBox.ClearValue(MUX::FrameworkElement::WidthProperty());
            checkBox.ClearValue(MUX::FrameworkElement::HeightProperty());
            checkBox.HorizontalAlignment(
                MUX::HorizontalAlignment::Stretch);
            item.ClearValue(MUXC::Control::PaddingProperty());
            content = grid;
        }
#endif // wxUSE_OWNER_DRAWN

        if ( !content &&
             (HasFlag(wxLB_HSCROLL) ||
              m_itemModel.At(n).text.Find(wxT('\t')) != wxNOT_FOUND) )
        {
            const double scale = wxWinUIListScale(this);
            const wxString text = m_itemModel.At(n).text;
            const int average = wxMax(1, GetCharWidth());
            const WinUITabLayout tabLayout =
                WinUIBuildTabLayout(text);
            const int paddedWidth =
                tabLayout.width >
                    std::numeric_limits<int>::max() - average
                    ? std::numeric_limits<int>::max()
                    : tabLayout.width + average;
            const int width = wxMax(
                paddedWidth,
                m_horizontalExtent);
            const double widthDips = wxMax(1, width) / scale;
            MUXC::Canvas canvas;
            canvas.Tag(winrt::box_value(
                winrt::hstring(L"wxWinUIListTabCanvas")));
            canvas.Width(widthDips);
            canvas.Height(itemHeightDips);

            // ListView's vertical ItemsStackPanel otherwise constrains its
            // item containers to the viewport even when their content has an
            // explicit wider Width. Put the Windows horizontal extent on the
            // real container too so it participates in ScrollViewer measure.
            wxWinUIApplyListHorizontalExtent(
                m_winui->listView, item, widthDips);

            for ( std::size_t runIndex = 0;
                  runIndex < tabLayout.runs.size();
                  ++runIndex )
            {
                MUXC::TextBlock block;
                block.Text(wxWinUIToHString(
                    tabLayout.runs[runIndex]));
                block.TextWrapping(MUX::TextWrapping::NoWrap);
                wxWinUIApplyFont(block, GetFont());
                MUXC::Canvas::SetLeft(
                    block,
                    tabLayout.runOffsets[runIndex] / scale);
                canvas.Children().Append(block);
            }
            content = canvas;
        }

        if ( !content )
        {
            if ( checkBox )
            {
                const auto checkIdentity =
                    wxWinUIListObjectIdentity(checkBox);
                if ( wxWinUIListObjectIdentity(item.Content()) !=
                     checkIdentity )
                {
                    // Inverse of the direct-to-owner-draw transition above:
                    // remove the retained overlay from its Grid before making
                    // it direct Content. Assigning it while still parented is
                    // rejected by XAML with E_INVALIDARG.
                    const MUX::FocusState checkFocusState =
                        checkBox.FocusState();
                    wxWinUIDetachListChild(checkBox);
                    if ( checkFocusState != MUX::FocusState::Unfocused )
                    {
                        pendingFocusTarget = checkBox;
                        pendingFocusState = checkFocusState;
                        pendingFocusContentIdentity = checkIdentity;
                        pendingFocusParentIdentity = nullptr;
                        pendingFocusDirectContent = true;
                    }
                }
                content = checkBox.as<
                    winrt::Windows::Foundation::IInspectable>();
            }
            else
            {
                content = winrt::box_value(
                    wxWinUIToHString(m_itemModel.At(n).text));
            }
        }

        if ( wxWinUIListObjectIdentity(item.Content()) !=
             wxWinUIListObjectIdentity(content) )
        {
            item.Content(content);
        }
        peerMutationGuard.reset();

        if ( pendingFocusTarget &&
             pendingFocusState != MUX::FocusState::Unfocused )
        {
            const auto getContentOwner = [&]() -> wxListBox *
            {
                wxListBox * const owner = entryCallbackState->GetOwner();
                if ( !owner || owner != this || !owner->m_winui ||
                     owner->m_winui.get() != entryImplementation ||
                     owner->m_winui->callbackState != entryCallbackState ||
                     owner->m_winui->peerRevision != entryRevision ||
                     !owner->m_winui->peerItemsValid ||
                     n >= owner->m_itemModel.GetCount() ||
                     n >= owner->m_winui->peerItemIds.size() ||
                     owner->m_itemModel.At(n).id != entryId ||
                     owner->m_winui->peerItemIds[n] != entryId )
                {
                    return nullptr;
                }

                try
                {
                    const auto currentItems =
                        owner->m_winui->listView.Items();
                    if ( n >= currentItems.Size() ||
                         wxWinUIListObjectIdentity(currentItems.GetAt(n)) !=
                             entryPeerIdentity )
                    {
                        return nullptr;
                    }
                    const MUXC::ListViewItem currentItem =
                        currentItems.GetAt(n).try_as<MUXC::ListViewItem>();
                    if ( !currentItem ||
                         wxWinUIListObjectIdentity(currentItem.Content()) !=
                             pendingFocusContentIdentity )
                    {
                        return nullptr;
                    }
                }
                catch ( const winrt::hresult_error& )
                {
                    return nullptr;
                }
                return owner;
            };

            const auto getAttachedOwner = [&]() -> wxListBox *
            {
                wxListBox * const owner = entryCallbackState->GetOwner();
                if ( !owner || owner != this || !owner->m_winui ||
                     owner->m_winui.get() != entryImplementation ||
                     owner->m_winui->callbackState != entryCallbackState ||
                     owner->m_winui->peerRevision != entryRevision ||
                     !owner->m_winui->peerItemsValid ||
                     n >= owner->m_itemModel.GetCount() ||
                     n >= owner->m_winui->peerItemIds.size() ||
                     owner->m_itemModel.At(n).id != entryId ||
                     owner->m_winui->peerItemIds[n] != entryId )
                {
                    return nullptr;
                }

                try
                {
                    const auto currentItems =
                        owner->m_winui->listView.Items();
                    if ( n >= currentItems.Size() ||
                         wxWinUIListObjectIdentity(currentItems.GetAt(n)) !=
                             entryPeerIdentity )
                    {
                        return nullptr;
                    }

                    const MUXC::ListViewItem currentItem =
                        currentItems.GetAt(n).try_as<MUXC::ListViewItem>();
                    const MUX::DependencyObject parent =
                        MUXM::VisualTreeHelper::GetParent(
                            pendingFocusTarget);
                    const auto parentIdentity =
                        wxWinUIListObjectIdentity(parent);
                    if ( !currentItem ||
                         wxWinUIListObjectIdentity(currentItem.Content()) !=
                             pendingFocusContentIdentity ||
                         !parentIdentity )
                    {
                        return nullptr;
                    }

                    if ( pendingFocusParentIdentity )
                    {
                        if ( parentIdentity != pendingFocusParentIdentity )
                            return nullptr;
                    }
                    else
                    {
                        // The ContentPresenter parent does not exist until the
                        // direct Content is attached. Capture its controlling
                        // IUnknown now and require it to remain stable across
                        // every focus/reentrancy boundary below.
                        pendingFocusParentIdentity = parentIdentity;
                    }

                    if ( pendingFocusDirectContent )
                    {
                        bool attachedToItem = false;
                        MUX::DependencyObject ancestor = parent;
                        for ( unsigned int depth = 0;
                              ancestor && depth < 64;
                              ++depth )
                        {
                            if ( wxWinUIListObjectIdentity(ancestor) ==
                                 entryPeerIdentity )
                            {
                                attachedToItem = true;
                                break;
                            }
                            ancestor =
                                MUXM::VisualTreeHelper::GetParent(ancestor);
                        }
                        if ( !attachedToItem )
                            return nullptr;
                    }
                }
                catch ( const winrt::hresult_error& )
                {
                    return nullptr;
                }
                return owner;
            };

            wxListBox *owner = getContentOwner();
            if ( !owner )
                return;

            if ( pendingFocusDirectContent )
            {
                // Force the already-live ContentPresenter to consume its new
                // direct Content before requiring a visual parent. Layout can
                // raise application callbacks, so validate immediately after.
                item.UpdateLayout();
                owner = getContentOwner();
                if ( !owner )
                    return;
            }

            owner = getAttachedOwner();
            if ( !owner && pendingFocusDirectContent )
            {
                // Slot synchronization can complete a layout deferred by the
                // Content change. Bound this to one retry and validate around
                // both reentrancy boundaries.
                owner = getContentOwner();
                if ( !owner )
                    return;
                owner->m_winui->host.SynchronizeForFocus();
                owner = getContentOwner();
                if ( !owner )
                    return;
                item.UpdateLayout();
                owner = getContentOwner();
                if ( !owner )
                    return;
                owner = getAttachedOwner();
            }
            if ( !owner )
                return;

            bool restored = pendingFocusTarget.Focus(pendingFocusState);
            owner = getAttachedOwner();
            if ( !owner )
                return;

            if ( !restored ||
                 pendingFocusTarget.FocusState() ==
                     MUX::FocusState::Unfocused )
            {
                // The item can have been attached before its island completed
                // a layout/focus synchronization. Retry once only after the
                // real host has synchronized, and revalidate again because
                // focus notifications are arbitrary application code.
                owner->m_winui->host.SynchronizeForFocus();
                owner = getAttachedOwner();
                if ( !owner )
                    return;
                restored = pendingFocusTarget.Focus(pendingFocusState);
                owner = getAttachedOwner();
                if ( !owner )
                    return;
            }

            // Reparenting is finalized by ListView/ContentPresenter layout
            // after this stack unwinds. That phase can supersede a successful
            // synchronous Focus() and focus the container instead. Reassert
            // the retained target once on the next UI turn, with the complete
            // identity ticket captured above; never post an unbounded retry.
            bool queuedFocusRestore = false;
            try
            {
                const auto queue = pendingFocusTarget.DispatcherQueue();
                const wxListBox * const expectedOwner = this;
                const auto callbackState = entryCallbackState;
                wxWinUIListBoxImpl * const expectedImplementation =
                    entryImplementation;
                const wxWinUIItemModel::Id expectedId = entryId;
                const std::uint64_t expectedRevision = entryRevision;
                const unsigned int expectedIndex = n;
                const auto expectedPeerIdentity = entryPeerIdentity;
                const auto expectedContentIdentity =
                    pendingFocusContentIdentity;
                const auto expectedParentIdentity =
                    pendingFocusParentIdentity;
                const bool directContent = pendingFocusDirectContent;
                const MUXC::CheckBox focusTarget = pendingFocusTarget;
                const MUX::FocusState focusState = pendingFocusState;

                queuedFocusRestore = queue && queue.TryEnqueue(
                    MUXD::DispatcherQueuePriority::Low,
                    [expectedOwner, callbackState, expectedImplementation,
                     expectedId, expectedRevision, expectedIndex,
                     expectedPeerIdentity, expectedContentIdentity,
                     expectedParentIdentity, directContent, focusTarget,
                     focusState]()
                    {
                        wxListBox * const owner =
                            callbackState->GetOwner();
                        if ( !owner || owner != expectedOwner ||
                             !owner->m_winui ||
                             owner->m_winui.get() !=
                                 expectedImplementation ||
                             owner->m_winui->callbackState != callbackState ||
                             owner->m_winui->peerRevision !=
                                 expectedRevision ||
                             !owner->m_winui->peerItemsValid ||
                             expectedIndex >= owner->m_itemModel.GetCount() ||
                             expectedIndex >=
                                 owner->m_winui->peerItemIds.size() ||
                             owner->m_itemModel.At(expectedIndex).id !=
                                 expectedId ||
                             owner->m_winui->peerItemIds[expectedIndex] !=
                                 expectedId )
                        {
                            return;
                        }

                        try
                        {
                            const auto binding =
                                owner->m_winui->checkBindings.find(
                                    expectedId);
                            if ( binding ==
                                     owner->m_winui->checkBindings.end() ||
                                 wxWinUIListObjectIdentity(
                                     binding->second.checkBox) !=
                                     wxWinUIListObjectIdentity(focusTarget) )
                            {
                                return;
                            }

                            const auto currentItems =
                                owner->m_winui->listView.Items();
                            if ( expectedIndex >= currentItems.Size() ||
                                 wxWinUIListObjectIdentity(
                                     currentItems.GetAt(expectedIndex)) !=
                                     expectedPeerIdentity )
                            {
                                return;
                            }

                            const MUXC::ListViewItem currentItem =
                                currentItems.GetAt(expectedIndex)
                                    .try_as<MUXC::ListViewItem>();
                            const MUX::DependencyObject parent =
                                MUXM::VisualTreeHelper::GetParent(
                                    focusTarget);
                            if ( !currentItem ||
                                 wxWinUIListObjectIdentity(
                                     currentItem.Content()) !=
                                     expectedContentIdentity ||
                                 wxWinUIListObjectIdentity(parent) !=
                                     expectedParentIdentity )
                            {
                                return;
                            }

                            if ( directContent )
                            {
                                bool attachedToItem = false;
                                MUX::DependencyObject ancestor = parent;
                                for ( unsigned int depth = 0;
                                      ancestor && depth < 64;
                                      ++depth )
                                {
                                    if ( wxWinUIListObjectIdentity(ancestor) ==
                                         expectedPeerIdentity )
                                    {
                                        attachedToItem = true;
                                        break;
                                    }
                                    ancestor =
                                        MUXM::VisualTreeHelper::GetParent(
                                            ancestor);
                                }
                                if ( !attachedToItem )
                                    return;
                            }

                            if ( !focusTarget.Focus(focusState) )
                            {
                                wxLogTrace(
                                    wxT("winui"),
                                    wxT("queued WinUI CheckListBox focus "
                                        "restore was rejected"));
                            }
                        }
                        catch ( const winrt::hresult_error& e )
                        {
                            wxWinUILogException(
                                "queued WinUI CheckListBox focus restore", e);
                        }
                    });
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "queue WinUI CheckListBox focus restore", e);
            }

            if ( !queuedFocusRestore &&
                 (!restored ||
                  pendingFocusTarget.FocusState() ==
                      MUX::FocusState::Unfocused) )
            {
                wxLogTrace(wxT("winui"),
                           wxT("WinUI CheckListBox focus restore was rejected"));
            }
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox item content update", e);
        wxListBox * const owner = entryCallbackState
            ? entryCallbackState->GetOwner()
            : nullptr;
        if ( owner && owner == this && owner->m_winui &&
             owner->m_winui.get() == entryImplementation &&
             owner->m_winui->callbackState == entryCallbackState )
        {
            owner->m_winui->renderingOwnerItems.erase(entryId);
            owner->m_winui->peerItemsValid = false;
        }
    }
    catch ( ... )
    {
        // Preserve application exception semantics, but never leave a live
        // control permanently marked as rendering after a user callback.
        wxListBox * const owner = entryCallbackState
            ? entryCallbackState->GetOwner()
            : nullptr;
        if ( owner && owner == this && owner->m_winui &&
             owner->m_winui.get() == entryImplementation &&
             owner->m_winui->callbackState == entryCallbackState )
        {
            owner->m_winui->renderingOwnerItems.erase(entryId);
            owner->m_winui->peerItemsValid = false;
        }
        throw;
    }
}

bool wxListBox::WinUIUpdatePeerItemById(wxWinUIItemModel::Id id,
                                        bool ensureConsistent)
{
    if ( !m_winui || !id )
        return false;

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation->callbackState;
    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState->GetOwner();
        return owner && owner == this && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    if ( ensureConsistent )
    {
        WinUIEnsurePeerConsistent();
        if ( !getCurrentOwner() )
            return false;
    }

    wxListBox *owner = getCurrentOwner();
    if ( !owner )
        return false;
    const std::size_t index = owner->m_itemModel.IndexOf(id);
    if ( index == wxWinUIItemModel::npos )
        return false;

    owner->WinUIUpdatePeerItem(static_cast<unsigned int>(index));
    owner = getCurrentOwner();
    if ( !owner )
        return false;

    if ( ensureConsistent )
    {
        owner->WinUIEnsurePeerConsistent();
        owner = getCurrentOwner();
        if ( !owner )
            return false;
    }

    const std::size_t currentIndex = owner->m_itemModel.IndexOf(id);
    return currentIndex != wxWinUIItemModel::npos &&
           owner->m_winui->peerItemsValid &&
           currentIndex < owner->m_winui->peerItemIds.size() &&
           owner->m_winui->peerItemIds[currentIndex] == id;
}

void wxListBox::WinUISyncOldSelections()
{
    m_oldSelections.clear();
    GetSelections(m_oldSelections);
}

void wxListBox::SendSelectionEvent()
{
    CalcAndSendEvent();
}

std::uint64_t wxListBox::WinUIGetItemIdForTesting(unsigned int n) const
{
    return n < m_itemModel.GetCount() ? m_itemModel.At(n).id : 0;
}

std::uintptr_t
wxListBox::WinUIGetItemPeerIdentityForTesting(unsigned int n) const
{
    if ( !m_winui || !m_winui->listView )
        return 0;

    try
    {
        const auto items = m_winui->listView.Items();
        if ( n >= items.Size() )
            return 0;
        return reinterpret_cast<std::uintptr_t>(
            winrt::get_abi(wxWinUIListObjectIdentity(items.GetAt(n))));
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}

unsigned int wxListBox::WinUIGetPeerCountForTesting() const
{
    if ( !m_winui || !m_winui->listView )
        return 0;

    try
    {
        return m_winui->listView.Items().Size();
    }
    catch ( const winrt::hresult_error& )
    {
        return 0;
    }
}

bool wxListBox::WinUIPoisonPeerForTesting()
{
    if ( !m_winui || !m_winui->listView )
        return false;

    try
    {
        // Model a partial XAML mutation: the peer gains an item while the
        // stable-ID vector remains unchanged and still claims validity. The
        // next public mutation must detect all three counts disagreeing and
        // rebuild before applying its delta.
        m_winui->listView.Items().Append(MUXC::ListViewItem());
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox peer poison test", e);
        return false;
    }
}

bool wxListBox::WinUISetPeerSelectionForTesting(unsigned int n, bool select)
{
    if ( !m_winui || !m_winui->listView )
        return false;

    WinUIEnsurePeerConsistent();
    if ( !m_winui || !m_winui->peerItemsValid )
        return false;

    try
    {
        const MUXC::ListView listView = m_winui->listView;
        const auto items = listView.Items();
        if ( n >= items.Size() )
            return false;
        const auto item = items.GetAt(n);

        // Mutate the ListView selection model, not just the container DP:
        // detached ListViewItem.IsSelected changes don't raise the parent's
        // SelectionChanged event. This is the same collection/property path
        // used by pointer, keyboard and UIA selection.
        if ( HasMultipleSelection() )
        {
            const auto selected = listView.SelectedItems();
            uint32_t found = selected.Size();
            for ( uint32_t i = 0; i < selected.Size(); ++i )
            {
                if ( wxWinUIListObjectIdentity(selected.GetAt(i)) ==
                     wxWinUIListObjectIdentity(item) )
                {
                    found = i;
                    break;
                }
            }

            if ( select && found == selected.Size() )
                selected.Append(item);
            else if ( !select && found < selected.Size() )
                selected.RemoveAt(found);
        }
        else
        {
            listView.SelectedItem(
                select ? item
                       : winrt::Windows::Foundation::IInspectable{ nullptr });
        }
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox test selection", e);
        return false;
    }
}

bool wxListBox::WinUIFocusPeerItemForTesting(unsigned int n)
{
    if ( !m_winui || !m_winui->listView ||
         !m_winui->callbackState )
    {
        return false;
    }

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation->callbackState;
    try
    {
        const auto items = implementation->listView.Items();
        if ( n >= items.Size() )
            return false;

        const MUXC::Control item =
            items.GetAt(n).try_as<MUXC::Control>();
        if ( !item )
            return false;

        const bool focused = item.Focus(MUX::FocusState::Programmatic);
        wxListBox * const owner = callbackState->GetOwner();
        return focused && owner && owner == this && owner->m_winui &&
               owner->m_winui.get() == implementation &&
               owner->m_winui->callbackState == callbackState;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox test focus", e);
        return false;
    }
}

bool wxListBox::WinUIDoubleTapPeerForTesting(unsigned int n)
{
    if ( n >= m_itemModel.GetCount() )
        return false;

    const wxWinUIItemModel::Id id = m_itemModel.At(n).id;
    WinUISendDoubleClick(id);
    return true;
}

bool wxListBox::WinUISetPeerCheckForTesting(unsigned int n, bool check)
{
    if ( !m_winui || n >= m_itemModel.GetCount() )
        return false;

    const wxWinUIItemModel::Id id = m_itemModel.At(n).id;
    const auto it = m_winui->checkBindings.find(id);
    if ( it == m_winui->checkBindings.end() || !it->second.checkBox )
        return false;

    try
    {
        // Checked/Unchecked is the common native path used by pointer,
        // keyboard Space and UIA Toggle. Its callback can destroy this.
        const MUXC::CheckBox checkBox = it->second.checkBox;
        checkBox.IsChecked(check);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI CheckListBox test toggle", e);
        return false;
    }
}

bool wxListBox::WinUIActivatePeerCheckForTesting(unsigned int n,
                                                  bool *focused,
                                                  bool *pointerTarget)
{
    if ( focused )
        *focused = false;
    if ( pointerTarget )
        *pointerTarget = false;
    if ( !m_winui || !m_winui->callbackState ||
         n >= m_itemModel.GetCount() )
    {
        return false;
    }

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation->callbackState;
    const auto it =
        implementation->checkBindings.find(m_itemModel.At(n).id);
    if ( it == implementation->checkBindings.end() ||
         !it->second.checkBox )
    {
        return false;
    }

    try
    {
        // Prove that the real retained CheckBox is a non-empty, enabled XAML
        // hit target at its centre. This uses the same visual-tree hit test as
        // island pointer routing without relying on physical SendInput.
        const MUXC::CheckBox checkBox = it->second.checkBox;
        bool isPointerTarget = false;
        if ( checkBox.Visibility() == MUX::Visibility::Visible &&
             checkBox.IsHitTestVisible() && checkBox.IsEnabled() &&
             checkBox.ActualWidth() > 0.0 &&
             checkBox.ActualHeight() > 0.0 )
        {
            const MUX::XamlRoot xamlRoot = checkBox.XamlRoot();
            const MUX::UIElement root = xamlRoot
                ? xamlRoot.Content()
                : nullptr;
            if ( root )
            {
                const MUXM::GeneralTransform toRoot =
                    checkBox.TransformToVisual(root);
                const WF::Point centre = toRoot.TransformPoint(
                    WF::Point{
                        static_cast<float>(checkBox.ActualWidth() / 2.0),
                        static_cast<float>(checkBox.ActualHeight() / 2.0)
                    });
                const auto hits =
                    MUXM::VisualTreeHelper::FindElementsInHostCoordinates(
                        centre, root);
                const auto checkIdentity = wxWinUIListObjectIdentity(checkBox);
                for ( const MUX::UIElement& hit : hits )
                {
                    MUX::DependencyObject node = hit;
                    while ( node )
                    {
                        if ( wxWinUIListObjectIdentity(node) == checkIdentity )
                        {
                            isPointerTarget = true;
                            break;
                        }
                        if ( node == root )
                            break;
                        node = MUXM::VisualTreeHelper::GetParent(node);
                    }
                    if ( isPointerTarget )
                        break;
                }
            }
        }
        if ( pointerTarget )
            *pointerTarget = isPointerTarget;
        if ( !isPointerTarget )
            return false;

        // A pointer entering a shared island first focuses its wx slot, then
        // the hit CheckBox. Reproduce that native ordering without SendInput;
        // focusing a child in a non-activated slot directly is rejected by
        // XAML even though it is a valid hit target.
        wxListBox *owner = callbackState->GetOwner();
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }
        owner->SetFocus();
        owner = callbackState->GetOwner();
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }
        owner->m_winui->host.SynchronizeForFocus();
        owner = callbackState->GetOwner();
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }

        const bool gotFocus =
            checkBox.Focus(MUX::FocusState::Pointer) &&
            checkBox.FocusState() != MUX::FocusState::Unfocused;
        if ( focused )
            *focused = gotFocus;

        owner = callbackState->GetOwner();
        if ( !owner || owner != this || !owner->m_winui ||
             owner->m_winui.get() != implementation ||
             owner->m_winui->callbackState != callbackState )
        {
            return false;
        }

        // Checked/Unchecked can delete owner. Nothing touches it afterwards.
        wxWinUIToggleCheckBox(checkBox);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CheckListBox peer activation test", e);
        return false;
    }
}

bool wxListBox::WinUITogglePeerViaAutomationForTesting(unsigned int n)
{
    if ( !m_winui || n >= m_itemModel.GetCount() )
        return false;

    const auto it =
        m_winui->checkBindings.find(m_itemModel.At(n).id);
    if ( it == m_winui->checkBindings.end() || !it->second.checkBox )
        return false;

    try
    {
        const MUXC::CheckBox checkBox = it->second.checkBox;
        winrt::Microsoft::UI::Xaml::Automation::Peers::
            CheckBoxAutomationPeer peer(checkBox);
        peer.Toggle();
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException(
            "WinUI CheckListBox UIA test toggle", e);
        return false;
    }
}

bool wxListBox::WinUITogglePeerWithKeyboardForTesting(unsigned int n)
{
    if ( n >= m_itemModel.GetCount() || !m_itemModel.At(n).selected )
        return false;

    bool handled = false;
    return WinUIDispatchCheckKeyForTesting(
               static_cast<int>(
                   winrt::Windows::System::VirtualKey::Space),
               &handled) &&
           handled;
}

bool wxListBox::WinUIDispatchCheckKeyForTesting(int virtualKey,
                                                 bool *handled,
                                                 bool shiftDown,
                                                 std::uintptr_t keyboardLayout)
{
    if ( handled )
        *handled = false;
    if ( !m_winui || !m_winui->callbackState || !WinUIIsCheckable() )
        return false;

    // This is the same decoder and dispatcher used by PreviewKeyDown. The
    // callback may destroy this object, so only caller-owned output follows.
    const bool wasHandled = WinUIHandleCheckKey(
        virtualKey, shiftDown, keyboardLayout, false);
    if ( handled )
        *handled = wasHandled;
    return true;
}

bool wxListBox::WinUIGetPeerCheckForTesting(unsigned int n) const
{
    if ( !m_winui || n >= m_itemModel.GetCount() )
        return false;

    const auto it =
        m_winui->checkBindings.find(m_itemModel.At(n).id);
    if ( it == m_winui->checkBindings.end() || !it->second.checkBox )
        return false;

    try
    {
        const auto checked = it->second.checkBox.IsChecked();
        return checked && checked.Value();
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxListBox::WinUIGetScrollPresentationForTesting(
    int *horizontalVisibility,
    int *verticalVisibility,
    double *horizontalScrollableWidth,
    WinUIScrollMetricsForTesting *metrics) const
{
    if ( horizontalScrollableWidth )
        *horizontalScrollableWidth = 0.0;
    if ( metrics )
        *metrics = WinUIScrollMetricsForTesting{};

    if ( !m_winui || !m_winui->listView )
        return false;

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const MUXC::ListView listView = implementation->listView;

    try
    {
        if ( horizontalVisibility )
        {
            *horizontalVisibility = static_cast<int>(
                MUXC::ScrollViewer::
                    GetHorizontalScrollBarVisibility(
                        listView));
        }
        if ( verticalVisibility )
        {
            *verticalVisibility = static_cast<int>(
                MUXC::ScrollViewer::
                    GetVerticalScrollBarVisibility(
                        listView));
        }
        if ( horizontalScrollableWidth || metrics )
        {
            const MUXC::Panel panel = listView.ItemsPanelRoot();
            bool scrollResolvedFromPanelAncestor = false;
            const MUXC::ScrollViewer scrollViewer =
                wxWinUIGetListScrollViewer(
                    listView, &scrollResolvedFromPanelAncestor);

            if ( metrics )
            {
                metrics->horizontalPresentationPending =
                    implementation->horizontalPresentationPending;
                metrics->horizontalResetPending =
                    implementation->horizontalResetPending;
                metrics->materializationRetryQueued =
                    implementation->horizontalMaterializationRetryQueued;
                metrics->materializationAttempts =
                    implementation->horizontalMaterializationAttempts;
                metrics->materializationRetriesRemaining =
                    implementation->
                        horizontalMaterializationRetriesRemaining;
                metrics->listVisualChildCount =
                    MUXM::VisualTreeHelper::GetChildrenCount(listView);
                metrics->hasItemsPanelRoot = !!panel;
                metrics->hasAuthoritativeScroll = !!scrollViewer;
                if ( panel )
                {
                    metrics->hasItemsStackPanel =
                        !!panel.try_as<MUXC::ItemsStackPanel>();
                    metrics->hasVirtualizingStackPanel =
                        !!panel.try_as<MUXC::VirtualizingStackPanel>();
                    metrics->panelScrollOwnerResolvedFromAncestor =
                        scrollResolvedFromPanelAncestor;
                    metrics->panelActualWidth = panel.ActualWidth();
                    metrics->panelDesiredWidth =
                        panel.DesiredSize().Width;
                    if ( const auto virtualizing =
                             panel.try_as<
                                 MUXCP::IOrientedVirtualizingPanel>() )
                    {
                        metrics->hasOrientedPanel = true;
                        metrics->panelCanHorizontallyScroll =
                            virtualizing.CanHorizontallyScroll();
                        const WF::IInspectable panelOwner =
                            virtualizing.ScrollOwner();
                        metrics->panelScrollOwnerMatches =
                            wxWinUIListObjectIdentity(panelOwner) ==
                            wxWinUIListObjectIdentity(scrollViewer);
                        if ( !metrics->panelScrollOwnerMatches )
                        {
                            if ( const auto presenter = panelOwner.try_as<
                                     MUXC::ScrollContentPresenter>() )
                            {
                                metrics->panelScrollOwnerMatches =
                                    wxWinUIListObjectIdentity(
                                        presenter.ScrollOwner()) ==
                                    wxWinUIListObjectIdentity(
                                        scrollViewer);
                            }
                        }
                    }
                    else
                    {
                        // For ItemsStackPanel the nearest visual
                        // ScrollContentPresenter is the only public owner
                        // relationship. The resolver is rooted at this panel
                        // and bounded by ListView, so equality here cannot be
                        // satisfied by the outer shared-host ScrollViewer.
                        metrics->panelScrollOwnerMatches =
                            scrollResolvedFromPanelAncestor && !!scrollViewer;
                    }
                }
            }

            if ( !scrollViewer )
            {
                // Strictly observational: diagnostics above expose why the
                // production Loaded/SizeChanged/retry path has not converged,
                // but a getter must never materialize or repair that path.
                return false;
            }

            if ( horizontalScrollableWidth )
            {
                *horizontalScrollableWidth =
                    scrollViewer.ScrollableWidth();
            }
            if ( metrics )
            {
                metrics->scrollExtentWidth =
                    scrollViewer.ExtentWidth();
                metrics->scrollViewportWidth =
                    scrollViewer.ViewportWidth();
                metrics->scrollExtentHeight =
                    scrollViewer.ExtentHeight();
                metrics->scrollViewportHeight =
                    scrollViewer.ViewportHeight();
                metrics->scrollScrollableHeight =
                    scrollViewer.ScrollableHeight();

                if ( const MUXC::ScrollContentPresenter presenter =
                         wxWinUIFindListScrollContentPresenter(
                             scrollViewer, scrollViewer) )
                {
                    metrics->hasScrollContentPresenter = true;
                    metrics->contentPresenterCanHorizontallyScroll =
                        presenter.CanHorizontallyScroll();
                    metrics->contentPresenterSizesContentToTemplatedParent =
                        presenter.SizesContentToTemplatedParent();
                    metrics->contentPresenterExtentWidth =
                        presenter.ExtentWidth();
                    metrics->contentPresenterViewportWidth =
                        presenter.ViewportWidth();
                }

                if ( const MUX::FrameworkElement content =
                         wxWinUIGetListScrollContent(scrollViewer) )
                {
                    metrics->contentActualWidth = content.ActualWidth();
                    metrics->contentDesiredWidth =
                        content.DesiredSize().Width;
                }

                const auto items = listView.Items();
                if ( items.Size() )
                {
                    if ( const MUXC::ListViewItem item =
                             items.GetAt(0)
                                 .try_as<MUXC::ListViewItem>() )
                    {
                        metrics->itemActualWidth = item.ActualWidth();
                        metrics->itemDesiredWidth =
                            item.DesiredSize().Width;
                        metrics->itemExplicitWidth = item.Width();
                    }
                }
            }
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxListBox::WinUIGetPeerLayoutDirectionForTesting(
    bool *rightToLeft) const
{
    if ( rightToLeft )
        *rightToLeft = false;
    if ( !m_winui || !m_winui->listView )
        return false;

    try
    {
        if ( rightToLeft )
        {
            *rightToLeft =
                m_winui->listView.FlowDirection() ==
                    MUX::FlowDirection::RightToLeft;
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxListBox::WinUIGetItemPresentationForTesting(
    unsigned int n,
    bool *ownerDrawBitmap,
    wxSize *bitmapPixelSize,
    unsigned int *tabRunCount,
    double *contentWidthDips,
    bool *checkOverlay,
    wxRealPoint *bitmapDIPSize,
    wxVector<double> *tabRunOffsetsDips,
    bool *checkOverlayFocused,
    unsigned int *contentFontWeight,
    double *contentFontSizeDips,
    wxString *contentFontFamily,
    double *containerHeightDips,
    double *containerMinHeightDips,
    double *containerActualHeightDips,
    double *xamlRasterizationScale) const
{
    if ( ownerDrawBitmap )
        *ownerDrawBitmap = false;
    if ( bitmapPixelSize )
        *bitmapPixelSize = wxSize();
    if ( tabRunCount )
        *tabRunCount = 0;
    if ( contentWidthDips )
        *contentWidthDips = 0.0;
    if ( checkOverlay )
        *checkOverlay = false;
    if ( bitmapDIPSize )
        *bitmapDIPSize = wxRealPoint();
    if ( tabRunOffsetsDips )
        tabRunOffsetsDips->clear();
    if ( checkOverlayFocused )
        *checkOverlayFocused = false;
    if ( contentFontWeight )
        *contentFontWeight = 0;
    if ( contentFontSizeDips )
        *contentFontSizeDips = 0.0;
    if ( contentFontFamily )
        contentFontFamily->clear();
    if ( containerHeightDips )
        *containerHeightDips = 0.0;
    if ( containerMinHeightDips )
        *containerMinHeightDips = 0.0;
    if ( containerActualHeightDips )
        *containerActualHeightDips = 0.0;
    if ( xamlRasterizationScale )
        *xamlRasterizationScale = 0.0;

    if ( !m_winui || !m_winui->listView )
        return false;

    try
    {
        if ( xamlRasterizationScale )
        {
            const MUX::XamlRoot xamlRoot = m_winui->listView.XamlRoot();
            if ( xamlRoot )
                *xamlRasterizationScale = xamlRoot.RasterizationScale();
        }

        if ( contentFontWeight )
        {
            *contentFontWeight =
                m_winui->listView.FontWeight().Weight;
        }
        if ( contentFontSizeDips )
            *contentFontSizeDips = m_winui->listView.FontSize();
        if ( contentFontFamily && m_winui->listView.FontFamily() )
        {
            *contentFontFamily = wxWinUIFromHString(
                m_winui->listView.FontFamily().Source());
        }

        const auto items = m_winui->listView.Items();
        if ( n >= items.Size() )
            return false;
        const MUXC::ListViewItem item =
            items.GetAt(n).try_as<MUXC::ListViewItem>();
        if ( !item )
            return false;
        if ( containerHeightDips )
            *containerHeightDips = item.Height();
        if ( containerMinHeightDips )
            *containerMinHeightDips = item.MinHeight();
        if ( containerActualHeightDips )
            *containerActualHeightDips = item.ActualHeight();

        const auto inspect = [&](const MUX::DependencyObject& object,
                                 const auto& self) -> void
        {
            if ( !object )
                return;

            if ( const auto image = object.try_as<MUXC::Image>() )
            {
                const MUXMI::WriteableBitmap source =
                    image.Source().try_as<MUXMI::WriteableBitmap>();
                if ( source )
                {
                    if ( ownerDrawBitmap )
                        *ownerDrawBitmap = true;
                    if ( bitmapPixelSize )
                    {
                        *bitmapPixelSize = wxSize(
                            source.PixelWidth(), source.PixelHeight());
                    }
                    if ( bitmapDIPSize )
                    {
                        const double width = image.Width();
                        const double height = image.Height();
                        *bitmapDIPSize = wxRealPoint(
                            std::isfinite(width)
                                ? width
                                : image.ActualWidth(),
                            std::isfinite(height)
                                ? height
                                : image.ActualHeight());
                    }
                }
            }
            if ( const auto canvas = object.try_as<MUXC::Canvas>() )
            {
                if ( tabRunCount )
                    *tabRunCount = canvas.Children().Size();
                if ( tabRunOffsetsDips )
                {
                    tabRunOffsetsDips->clear();
                    for ( const MUX::UIElement& child :
                          canvas.Children() )
                    {
                        tabRunOffsetsDips->push_back(
                            MUXC::Canvas::GetLeft(child));
                    }
                }
            }
            if ( const auto text = object.try_as<MUXC::TextBlock>() )
            {
                if ( contentFontWeight )
                    *contentFontWeight = text.FontWeight().Weight;
                if ( contentFontSizeDips )
                    *contentFontSizeDips = text.FontSize();
                if ( contentFontFamily && text.FontFamily() )
                {
                    *contentFontFamily = wxWinUIFromHString(
                        text.FontFamily().Source());
                }
            }
            if ( const auto checkBox = object.try_as<MUXC::CheckBox>() )
            {
                if ( checkOverlay )
                    *checkOverlay = true;
                if ( checkOverlayFocused )
                {
                    const auto xamlRoot = checkBox.XamlRoot();
                    const MUX::DependencyObject focused = xamlRoot
                        ? MUX::Input::FocusManager::GetFocusedElement(
                              xamlRoot).try_as<MUX::DependencyObject>()
                        : nullptr;
                    *checkOverlayFocused =
                        checkBox.FocusState() !=
                            MUX::FocusState::Unfocused &&
                        wxWinUIListObjectIdentity(focused) ==
                            wxWinUIListObjectIdentity(checkBox);
                }
                if ( contentFontWeight )
                    *contentFontWeight = checkBox.FontWeight().Weight;
                if ( contentFontSizeDips )
                    *contentFontSizeDips = checkBox.FontSize();
                if ( contentFontFamily && checkBox.FontFamily() )
                {
                    *contentFontFamily = wxWinUIFromHString(
                        checkBox.FontFamily().Source());
                }
            }

            const int count =
                MUXM::VisualTreeHelper::GetChildrenCount(object);
            for ( int i = 0; i < count; ++i )
            {
                self(MUXM::VisualTreeHelper::GetChild(object, i), self);
            }
        };

        const auto content =
            item.Content().try_as<MUX::DependencyObject>();
        inspect(content, inspect);
        if ( contentWidthDips )
        {
            if ( const auto framework =
                     content.try_as<MUX::FrameworkElement>() )
            {
                const double explicitWidth = framework.Width();
                *contentWidthDips =
                    std::isfinite(explicitWidth)
                        ? explicitWidth
                        : framework.ActualWidth();
            }
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxListBox::WinUIGetThemePresentationForTesting(
    unsigned int n,
    int *actualTheme,
    std::uint32_t *foregroundARGB,
    std::uint32_t *bitmapARGB) const
{
    if ( actualTheme )
        *actualTheme = -1;
    if ( foregroundARGB )
        *foregroundARGB = 0;
    if ( bitmapARGB )
        *bitmapARGB = 0;
    if ( !m_winui || !m_winui->listView )
        return false;

    const auto packColour = [](const auto& colour)
    {
        return (static_cast<std::uint32_t>(colour.A) << 24) |
               (static_cast<std::uint32_t>(colour.R) << 16) |
               (static_cast<std::uint32_t>(colour.G) << 8) |
               static_cast<std::uint32_t>(colour.B);
    };

    try
    {
        if ( actualTheme )
        {
            *actualTheme = static_cast<int>(
                m_winui->listView.ActualTheme());
        }

        MUXM::SolidColorBrush foreground =
            m_winui->listView.Foreground()
                .try_as<MUXM::SolidColorBrush>();
        if ( !foreground )
        {
            const auto items = m_winui->listView.Items();
            if ( n < items.Size() )
            {
                if ( const auto item =
                         items.GetAt(n).try_as<MUXC::Control>() )
                {
                    foreground = item.Foreground()
                        .try_as<MUXM::SolidColorBrush>();
                }
            }
        }
        if ( !foreground && n < m_itemModel.GetCount() )
        {
            const auto binding = m_winui->checkBindings.find(
                m_itemModel.At(n).id);
            if ( binding != m_winui->checkBindings.end() &&
                 binding->second.checkBox )
            {
                foreground = binding->second.checkBox.Foreground()
                    .try_as<MUXM::SolidColorBrush>();
            }
        }
        if ( foregroundARGB && foreground )
            *foregroundARGB = packColour(foreground.Color());

        if ( bitmapARGB )
        {
            const auto items = m_winui->listView.Items();
            if ( n >= items.Size() )
                return false;
            const MUXC::ListViewItem item =
                items.GetAt(n).try_as<MUXC::ListViewItem>();
            if ( !item )
                return false;

            const auto inspect = [&](const MUX::DependencyObject& object,
                                     const auto& self) -> bool
            {
                if ( !object )
                    return false;
                if ( const auto image = object.try_as<MUXC::Image>() )
                {
                    const MUXMI::WriteableBitmap source =
                        image.Source().try_as<MUXMI::WriteableBitmap>();
                    if ( source && source.PixelBuffer().Length() >= 4 )
                    {
                        std::uint8_t *bytes = nullptr;
                        const auto access = source.PixelBuffer()
                            .as<wxWinUIListBufferByteAccess>();
                        if ( SUCCEEDED(access->Buffer(&bytes)) && bytes )
                        {
                            *bitmapARGB =
                                (static_cast<std::uint32_t>(bytes[3]) << 24) |
                                (static_cast<std::uint32_t>(bytes[2]) << 16) |
                                (static_cast<std::uint32_t>(bytes[1]) << 8) |
                                static_cast<std::uint32_t>(bytes[0]);
                            return true;
                        }
                    }
                }

                const int count =
                    MUXM::VisualTreeHelper::GetChildrenCount(object);
                for ( int i = 0; i < count; ++i )
                {
                    if ( self(MUXM::VisualTreeHelper::GetChild(object, i),
                              self) )
                    {
                        return true;
                    }
                }
                return false;
            };

            (void)inspect(
                item.Content().try_as<MUX::DependencyObject>(), inspect);
        }
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

bool wxListBox::WinUISetPeerThemeForTesting(bool dark)
{
    if ( !m_winui || !m_winui->listView || !m_winui->callbackState )
        return false;

    wxWinUIListBoxImpl * const implementation = m_winui.get();
    const std::shared_ptr<wxWinUIListBoxCallbackState> callbackState =
        implementation->callbackState;
    const auto getCurrentOwner = [&]() -> wxListBox *
    {
        wxListBox * const owner = callbackState->GetOwner();
        return owner && owner == this && owner->m_winui &&
                       owner->m_winui.get() == implementation &&
                       owner->m_winui->callbackState == callbackState
                   ? owner
                   : nullptr;
    };

    try
    {
        // This is deliberately a real XAML theme transition: tests use the
        // resulting ActualThemeChanged path, not an implementation flag.
        const MUX::ElementTheme theme =
            dark ? MUX::ElementTheme::Dark : MUX::ElementTheme::Light;
        implementation->host.ApplyTheme(theme);
        wxListBox *owner = getCurrentOwner();
        if ( !owner )
            return false;

        owner->m_winui->host.ForceRender();
        owner = getCurrentOwner();
        if ( !owner )
            return false;

#if wxUSE_OWNER_DRAWN
        // RequestedTheme can already equal ActualTheme (for example when the
        // process starts in dark mode), in which case XAML correctly raises no
        // ActualThemeChanged event. An explicit theme application must still
        // refresh retained owner-draw pixels for deterministic parity.
        if ( owner->HasFlag(wxLB_OWNERDRAW) )
        {
            owner->WinUIRefreshItems();
            owner = getCurrentOwner();
            if ( !owner )
                return false;
        }
#endif
        return owner->m_winui->listView.ActualTheme() == theme;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI ListBox theme test seam", e);
        return false;
    }
}

#endif // wxUSE_LISTBOX
