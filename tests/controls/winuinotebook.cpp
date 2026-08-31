///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuinotebook.cpp
// Purpose:     deterministic WinUI notebook lifetime/transaction tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"
#include "waitfor.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_NOTEBOOK

#include "wx/app.h"
#include "wx/bitmap.h"
#include "wx/imaglist.h"
#include "wx/log.h"
#include "wx/notebook.h"
#include "notebook-test-access.h"
#include "wx/panel.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

namespace
{

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXAP = winrt::Microsoft::UI::Xaml::Automation::Peers;
namespace MUXAPR = winrt::Microsoft::UI::Xaml::Automation::Provider;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

std::vector<MUXC::TabViewItem> wxWinUIGetExtendedNotebookItems(
    wxNotebook *notebook)
{
    std::vector<MUXC::TabViewItem> items;
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::FindSlotOwner(notebook);
    if ( !host )
        return items;

    host->FlushSync();
    wxWinUISlot * const slot = host->FindSlot(notebook);
    const MUX::UIElement root = slot ? slot->GetContent() : nullptr;
    if ( !root )
        return items;

    std::vector<MUX::DependencyObject> pending;
    pending.push_back(root);
    constexpr size_t MaxNodes = 4096;
    try
    {
        for ( size_t index = 0;
              index < pending.size() && index < MaxNodes;
              ++index )
        {
            const MUX::DependencyObject current = pending[index];
            if ( const auto item = current.try_as<MUXC::TabViewItem>() )
                items.push_back(item);

            const int count =
                MUX::Media::VisualTreeHelper::GetChildrenCount(current);
            for ( int child = 0;
                  child < count && pending.size() < MaxNodes;
                  ++child )
            {
                pending.push_back(
                    MUX::Media::VisualTreeHelper::GetChild(
                        current, child));
            }
        }

        std::sort(
            items.begin(),
            items.end(),
            [](const MUXC::TabViewItem& lhs,
               const MUXC::TabViewItem& rhs)
            {
                return MUXA::AutomationProperties::GetPositionInSet(lhs) <
                       MUXA::AutomationProperties::GetPositionInSet(rhs);
            });
    }
    catch ( const winrt::hresult_error& )
    {
        items.clear();
    }
    return items;
}

MUXAPR::ISelectionItemProvider wxWinUIGetSelectionItemProvider(
    const MUXC::TabViewItem& item)
{
    const MUXAP::AutomationPeer peer =
        item
            ? MUXAP::FrameworkElementAutomationPeer::
                  CreatePeerForElement(item)
            : nullptr;
    return peer
               ? peer.GetPattern(MUXAP::PatternInterface::SelectionItem)
                     .try_as<MUXAPR::ISelectionItemProvider>()
               : nullptr;
}

wxBitmapBundle MakeNotebookDPIBundle()
{
    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(
        wxSize(16, 16), 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(
        wxSize(16, 16), 2.0, 32));
    return wxBitmapBundle::FromBitmaps(one, two);
}

enum class NotebookBundleCallbackPoint
{
    DefaultSize,
    PreferredSize,
    Bitmap
};

struct ArmedNotebookBundleCallback
{
    bool armed = false;
    NotebookBundleCallbackPoint point =
        NotebookBundleCallbackPoint::PreferredSize;
    std::function<void ()> callback;
    unsigned calls = 0;
    unsigned bitmapCalls = 0;
    bool invalidDefaultSize = false;
    bool invalidPreferredSize = false;
};

class CallbackNotebookBitmapBundleImpl final : public wxBitmapBundleImpl
{
public:
    CallbackNotebookBitmapBundleImpl(
        const wxBitmap& bitmap,
        const std::shared_ptr<ArmedNotebookBundleCallback>& callback)
        : m_bitmap(bitmap),
          m_callback(callback)
    {
    }

    wxSize GetDefaultSize() const override
    {
        Invoke(NotebookBundleCallbackPoint::DefaultSize);
        if ( m_callback && m_callback->invalidDefaultSize )
            return wxSize();
        return m_bitmap.GetSize();
    }

    wxSize GetPreferredBitmapSizeAtScale(double) const override
    {
        Invoke(NotebookBundleCallbackPoint::PreferredSize);
        if ( m_callback && m_callback->invalidPreferredSize )
            return wxSize();
        return m_bitmap.GetSize();
    }

    wxBitmap GetBitmap(const wxSize&) override
    {
        if ( m_callback )
            ++m_callback->bitmapCalls;
        Invoke(NotebookBundleCallbackPoint::Bitmap);
        return m_bitmap;
    }

private:
    void Invoke(NotebookBundleCallbackPoint point) const
    {
        if ( !m_callback || !m_callback->armed ||
             m_callback->point != point || !m_callback->callback )
        {
            return;
        }

        m_callback->armed = false;
        ++m_callback->calls;
        m_callback->callback();
    }

    wxBitmap m_bitmap;
    std::shared_ptr<ArmedNotebookBundleCallback> m_callback;
};

wxBitmapBundle MakeCallbackNotebookBundle(
    const std::shared_ptr<ArmedNotebookBundleCallback>& callback)
{
    return wxBitmapBundle::FromImpl(
        new CallbackNotebookBitmapBundleImpl(
            wxBitmap(wxSize(11, 11), 32), callback));
}

bool wxWinUIWaitForNotebookRetirements(
    size_t retirementBaseline,
    size_t callbackBaseline,
    size_t frameworkBaseline)
{
    return WaitFor(
        "WinUI notebook peer retirements",
        [retirementBaseline, callbackBaseline, frameworkBaseline]()
        {
            return
                wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
                    retirementBaseline &&
                wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
                    callbackBaseline &&
                wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
                    frameworkBaseline;
        },
        2000);
}

void wxWinUIDrainNotebookCallbacks()
{
    const size_t retirements =
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount();
    const size_t callbacks =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    const size_t framework =
        wxWinUINotebookTestAccess::GetFrameworkRetirementCount();

    REQUIRE(callbacks >= retirements);
    REQUIRE(framework >= retirements);
    REQUIRE(wxWinUIWaitForNotebookRetirements(
        0, callbacks - retirements, framework - retirements));
}

class wxWinUINotebookRetirementTestGuard final
{
public:
    ~wxWinUINotebookRetirementTestGuard()
    {
        wxWinUINotebookTestAccess::SetPeerRetirementQueueFault(
            wxWinUINotebookTestAccess::PeerRetirementQueueFault::None);
        wxWinUINotebookTestAccess::SetFrameworkRetirementHookFault(
            wxWinUINotebookTestAccess::FrameworkRetirementHookFault::None);
        wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();
    }
};

struct ReentrantNotebookRetirementContext
{
    wxWindow *parent = nullptr;
    int calls = 0;
    bool created = false;
    bool pageAdded = false;
    bool usedExtendedSurface = false;
    size_t peerRetirementsAfterClose = 0;
    size_t frameworkEntriesAfterClose = 0;
    wxWinUINotebookTestAccess::FrameworkRetirementSnapshot phaseSnapshot;
};

void ReenterNotebookRetirementPhase(void *rawContext)
{
    auto * const context =
        static_cast<ReentrantNotebookRetirementContext *>(rawContext);
    if ( !context || !context->parent )
        return;

    ++context->calls;
    context->phaseSnapshot =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    {
        wxNotebook notebook;
        context->created = notebook.Create(
            context->parent,
            wxID_ANY,
            wxDefaultPosition,
            wxSize(220, 140),
            wxNB_RIGHT | wxNB_MULTILINE | wxNB_NOPAGETHEME);
        if ( context->created )
        {
            wxPanel * const page = new wxPanel(&notebook);
            context->pageAdded = notebook.AddPage(
                page, "reentrant retirement", true);
            if ( !context->pageAdded )
                page->Destroy();
            context->usedExtendedSurface =
                wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook);
        }
    }
    context->peerRetirementsAfterClose =
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount();
    context->frameworkEntriesAfterClose =
        wxWinUINotebookTestAccess::GetFrameworkRetirementCount();
}

struct ReentrantLedgerRetirementContext
{
    int calls = 0;
    int nestedCompletions = 0;
    std::uint64_t nestedId = 0;
    wxWinUINotebookTestAccess::FrameworkRetirementSnapshot phaseSnapshot;
};

void IncrementLedgerRetirement(void *rawContext)
{
    ++*static_cast<int *>(rawContext);
}

void ReenterLedgerRetirementAtXaml(void *rawContext)
{
    auto * const context =
        static_cast<ReentrantLedgerRetirementContext *>(rawContext);
    if ( !context )
        return;

    ++context->calls;
    context->phaseSnapshot =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    context->nestedId =
        wxWinUINotebookTestAccess::RegisterUnboundFrameworkRetirement(
            &IncrementLedgerRetirement, &context->nestedCompletions);
}

bool wxWinUIFocusIsWithin(wxWindow *page)
{
    wxWindow * const focused = wxWindow::FindFocus();
    return focused == page ||
           (focused && page && page->IsDescendant(focused));
}

class NotebookVirtualBoundary final : public wxNotebook
{
public:
    enum class Boundary
    {
        None,
        CreateChangingEvent,
        TryGetPage,
        GetPageRect,
        MakeChangedEvent
    };

    using wxNotebook::wxNotebook;

    void Arm(Boundary boundary, size_t page = wxNOT_FOUND)
    {
        m_boundary = boundary;
        m_page = page;
    }

    unsigned GetBoundaryCallCount() const
    {
        return m_boundaryCalls;
    }

protected:
    wxBookCtrlEvent *CreatePageChangingEvent() const override
    {
        wxBookCtrlEvent * const event =
            wxNotebook::CreatePageChangingEvent();
        Fire(Boundary::CreateChangingEvent);
        return event;
    }

    wxWindow *TryGetNonNullPage(size_t page) override
    {
        wxWindow * const window =
            wxNotebook::TryGetNonNullPage(page);
        if ( m_page == page )
            Fire(Boundary::TryGetPage);
        return window;
    }

    wxRect GetPageRect() const override
    {
        const wxRect rect = wxNotebook::GetPageRect();
        Fire(Boundary::GetPageRect);
        return rect;
    }

    void MakeChangedEvent(wxBookCtrlEvent& event) override
    {
        wxNotebook::MakeChangedEvent(event);
        Fire(Boundary::MakeChangedEvent);
    }

private:
    void Fire(Boundary boundary) const
    {
        if ( m_boundary != boundary )
            return;

        m_boundary = Boundary::None;
        ++m_boundaryCalls;
        const_cast<NotebookVirtualBoundary *>(this)->Destroy();
        // Destroy() can delete this synchronously. Do not access any member
        // after the call.
    }

    mutable Boundary m_boundary = Boundary::None;
    mutable size_t m_page = static_cast<size_t>(wxNOT_FOUND);
    mutable unsigned m_boundaryCalls = 0;
};

wxPoint wxWinUIFindTabGutter(const wxRect& item,
                             const wxRect& icon,
                             const wxRect& label)
{
    // Prefer the vertical centre and search outwards from the item centre.
    // The first geometrically interior pixel can be inside a rounded,
    // transparent template corner even though the enclosing wxRect contains
    // it. A central gap exercises the real TabViewItem background.
    const int centreY = item.y + item.height / 2;
    const int centreX = item.x + item.width / 2;
    for ( int distance = 0; distance < item.width; ++distance )
    {
        const int candidates[] =
        {
            centreX - distance,
            centreX + distance
        };
        for ( const int x : candidates )
        {
            const wxPoint point(x, centreY);
            if ( item.Contains(point) &&
                 !icon.Contains(point) &&
                 !label.Contains(point) )
            {
                return point;
            }
        }
    }

    // Degenerate/custom templates may leave no horizontal gap at the centre.
    // Retain a complete fallback scan while still excluding the exact border.
    for ( int y = item.GetTop() + 1; y < item.GetBottom(); ++y )
    {
        for ( int x = item.GetLeft() + 1; x < item.GetRight(); ++x )
        {
            const wxPoint point(x, y);
            if ( !icon.Contains(point) && !label.Contains(point) )
                return point;
        }
    }

    return wxDefaultPosition;
}

wxPoint wxWinUIRectCentre(const wxRect& rect)
{
    return wxPoint(rect.x + rect.width / 2,
                   rect.y + rect.height / 2);
}

struct NotebookLabelReentry
{
    wxString newest;
    unsigned calls = 0;
};

void ReplaceNotebookLabel(wxNotebook *notebook, void *context)
{
    NotebookLabelReentry * const probe =
        static_cast<NotebookLabelReentry *>(context);
    ++probe->calls;
    notebook->SetPageText(0, probe->newest);
}

struct NotebookImageReentry
{
    wxBookCtrlBase::Images newest;
    unsigned calls = 0;
};

void ReplaceNotebookImages(wxNotebook *notebook, void *context)
{
    NotebookImageReentry * const probe =
        static_cast<NotebookImageReentry *>(context);
    ++probe->calls;
    notebook->SetImages(probe->newest);
}

struct NotebookMetricsReentry
{
    wxSize padding;
    wxSize size;
    unsigned calls = 0;
};

void ReplaceNotebookMetrics(wxNotebook *notebook, void *context)
{
    NotebookMetricsReentry * const probe =
        static_cast<NotebookMetricsReentry *>(context);
    ++probe->calls;
    notebook->SetPadding(probe->padding);
    notebook->SetTabSize(probe->size);
}

struct NotebookProjectionStorm
{
    int remaining = 0;
    int mutation = 0;
    wxString newest;
};

void ContinueNotebookProjectionStorm(wxNotebook *notebook, void *context)
{
    NotebookProjectionStorm * const probe =
        static_cast<NotebookProjectionStorm *>(context);
    if ( probe->remaining <= 0 )
        return;

    --probe->remaining;
    ++probe->mutation;
    probe->newest = wxString::Format("newest-%d", probe->mutation);
    if ( probe->remaining > 0 )
    {
        wxWinUINotebookTestAccess::SetNextProjectionHook(*notebook, wxWinUINotebookTestAccess::ProjectionPoint::LabelText,
            &ContinueNotebookProjectionStorm,
            probe);
    }
    notebook->SetPageText(0, probe->newest);
}

struct NotebookDestroyProjection
{
    unsigned calls = 0;
};

void DestroyNotebookDuringProjection(wxNotebook *notebook, void *context)
{
    NotebookDestroyProjection * const probe =
        static_cast<NotebookDestroyProjection *>(context);
    ++probe->calls;
    notebook->Destroy();
}

struct NotebookDestroyInsertedPageProjection
{
    wxWindow *page = nullptr;
    unsigned calls = 0;
};

void DestroyInsertedPageDuringProjection(wxNotebook *, void *context)
{
    NotebookDestroyInsertedPageProjection * const probe =
        static_cast<NotebookDestroyInsertedPageProjection *>(context);
    ++probe->calls;
    wxWindow * const page = probe->page;
    probe->page = nullptr;
    delete page;
}

struct NotebookGeometryInsert
{
    wxRect nestedRect;
    bool inserted = false;
    unsigned calls = 0;
};

void InsertNotebookPageDuringGeometry(wxNotebook *notebook, void *context)
{
    NotebookGeometryInsert * const probe =
        static_cast<NotebookGeometryInsert *>(context);
    ++probe->calls;
    probe->nestedRect = notebook->GetTabRect(0);
    probe->inserted = notebook->InsertPage(
        0,
        new wxPanel(notebook),
        "nested inserted",
        false);
}

struct NotebookGeometryResize
{
    wxSize size;
    unsigned calls = 0;
};

void ResizeNotebookDuringGeometry(wxNotebook *notebook, void *context)
{
    NotebookGeometryResize * const probe =
        static_cast<NotebookGeometryResize *>(context);
    ++probe->calls;
    notebook->SetSize(probe->size);
}

struct NotebookStyleReentry
{
    long newestStyle = 0;
    unsigned calls = 0;
};

void ReplaceNotebookStyleDuringGeometry(wxNotebook *notebook, void *context)
{
    NotebookStyleReentry * const probe =
        static_cast<NotebookStyleReentry *>(context);
    ++probe->calls;
    notebook->SetWindowStyleFlag(probe->newestStyle);
}

void CountNotebookGeometryRealization(wxNotebook *, void *context)
{
    unsigned * const calls = static_cast<unsigned *>(context);
    ++*calls;
}

wxRect CheckNotebookPagesShareRect(wxNotebook& notebook)
{
    REQUIRE(notebook.GetPageCount() != 0);
    wxWindow * const first = notebook.GetPage(0);
    REQUIRE(first != nullptr);
    const wxRect rect = first->GetRect();
    for ( size_t page = 1; page < notebook.GetPageCount(); ++page )
    {
        wxWindow * const current = notebook.GetPage(page);
        REQUIRE(current != nullptr);
        CHECK(current->GetRect() == rect);
    }
    return rect;
}

} // anonymous namespace

TEST_CASE("wxWinUI Notebook supports every wxMSW presentation style",
          "[winui-notebook][styles]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    {
        const wxSize requested(173, 119);
        wxNotebook notebook;
        REQUIRE(notebook.Create(
            parent, wxID_ANY, wxDefaultPosition, requested));
        CHECK((notebook.GetWindowStyleFlag() & wxBK_ALIGN_MASK) == wxNB_TOP);
        CHECK(notebook.GetSize() == requested);
    }

    {
        wxNotebook notebook;
        REQUIRE(notebook.Create(
            parent, wxID_ANY, wxDefaultPosition, wxSize(190, 130),
            wxNB_TOP));
        CHECK((notebook.GetWindowStyleFlag() & wxBK_ALIGN_MASK) == wxNB_TOP);
    }

    const std::array<long, 8> styles =
    {
        wxNB_TOP,
        wxNB_BOTTOM,
        wxNB_LEFT,
        wxNB_RIGHT,
        wxNB_TOP | wxNB_MULTILINE,
        wxNB_BOTTOM | wxNB_MULTILINE | wxNB_FIXEDWIDTH,
        wxNB_LEFT | wxNB_MULTILINE | wxNB_NOPAGETHEME,
        wxNB_RIGHT | wxNB_FIXEDWIDTH | wxNB_NOPAGETHEME
    };
    for ( const long style : styles )
    {
        CAPTURE(style);
        wxNotebook notebook;
        REQUIRE(notebook.Create(
            parent, wxID_ANY, wxDefaultPosition, wxSize(190, 130),
            style));
        CHECK((notebook.GetWindowStyleFlag() & wxBK_ALIGN_MASK) ==
              (style & wxBK_ALIGN_MASK));
        CHECK(wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook) ==
              ((style & wxBK_ALIGN_MASK) != wxNB_TOP ||
               (style & (wxNB_MULTILINE | wxNB_NOPAGETHEME)) != 0));
    }

    // Combined alignment bits are accepted and normalized deterministically
    // instead of leaving wxNotebookBase with an impossible orientation.
    {
        wxNotebook notebook;
        REQUIRE(notebook.Create(
            parent, wxID_ANY, wxDefaultPosition, wxSize(190, 130),
            wxNB_TOP | wxNB_LEFT | wxNB_RIGHT | wxNB_BOTTOM));
        CHECK((notebook.GetWindowStyleFlag() & wxBK_ALIGN_MASK) ==
              wxNB_BOTTOM);
    }
}

TEST_CASE("wxWinUI Notebook wraps and hot-mutates every tab side",
          "[winui-notebook][styles][multiline][hot]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    // Reproduce the original cross-case failure deterministically inside one
    // case: retire an attached extended tree and create/hot-retemplate the
    // next notebook before the dispatcher gets a chance to release it.
    wxWinUIDrainNotebookCallbacks();
    const size_t retirementBaseline =
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount();
    const size_t callbackBaseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    const size_t frameworkBaseline =
        wxWinUINotebookTestAccess::GetFrameworkRetirementCount();
    wxPanel sourceParent(
        parent, wxID_ANY, wxPoint(0, 0), wxSize(210, 170));
    wxPanel destinationParent(
        parent, wxID_ANY, wxPoint(220, 0), wxSize(210, 170));
    {
        wxNotebook predecessor(
            &sourceParent, wxID_ANY, wxDefaultPosition, wxSize(190, 130),
            wxNB_RIGHT | wxNB_MULTILINE | wxNB_NOPAGETHEME);
        REQUIRE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(predecessor));
        REQUIRE(predecessor.AddPage(
            new wxPanel(&predecessor), "migrating page", true));
        predecessor.SetWindowStyleFlag(wxNB_TOP | wxNB_FIXEDWIDTH);
        REQUIRE(predecessor.Reparent(&destinationParent));
        CHECK(predecessor.GetParent() == &destinationParent);
        predecessor.SetWindowStyleFlag(
            wxNB_BOTTOM | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
        REQUIRE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(predecessor));
    }
    REQUIRE(
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
        retirementBaseline + 1);
    REQUIRE(
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
        callbackBaseline + 1);

    {
        wxNotebook notebook(
            &destinationParent, wxID_ANY, wxDefaultPosition,
            wxSize(190, 150),
            wxNB_TOP | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
        notebook.SetTabSize(wxSize(80, 32));
        for ( int page = 0; page < 5; ++page )
        {
            REQUIRE(notebook.AddPage(
                new wxPanel(&notebook),
                wxString::Format("&Page %d", page),
                page == 0));
        }

        CHECK(notebook.GetRowCount() == 3);
        CHECK(notebook.GetTabRect(0).y < notebook.GetTabRect(4).y);

        notebook.SetWindowStyleFlag(
            wxNB_BOTTOM | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
        CHECK(notebook.GetRowCount() == 3);
        CHECK(notebook.GetTabRect(0).y > 0);
        CHECK(notebook.GetTabRect(4).GetBottom() <=
              notebook.GetClientSize().y);

        notebook.SetWindowStyleFlag(
            wxNB_LEFT | wxNB_MULTILINE | wxNB_FIXEDWIDTH |
            wxNB_NOPAGETHEME);
        CHECK(notebook.GetRowCount() == 2);
        CHECK(notebook.GetTabRect(0).x < notebook.GetTabRect(4).x);
        CHECK(notebook.GetTabRect(4).GetRight() <=
              notebook.GetClientSize().x);

        notebook.SetWindowStyleFlag(
            wxNB_RIGHT | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
        CHECK(notebook.GetRowCount() == 2);
        CHECK(notebook.GetTabRect(0).x > 0);

        notebook.SetWindowStyleFlag(wxNB_TOP | wxNB_FIXEDWIDTH);
        CHECK_FALSE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook));
        CHECK(notebook.GetRowCount() == 1);
    }

    REQUIRE(
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
        retirementBaseline + 2);
    REQUIRE(wxWinUIWaitForNotebookRetirements(
        retirementBaseline, callbackBaseline, frameworkBaseline));
    CHECK(
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
        retirementBaseline);
    CHECK(
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
        callbackBaseline);
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline);
}

TEST_CASE("wxWinUI Notebook enqueue rejection waits for XAML shutdown",
          "[winui-notebook][lifetime][dispatcher][shutdown]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUIDrainNotebookCallbacks();
    wxWinUINotebookRetirementTestGuard cleanup;
    const size_t retirementBaseline =
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount();
    const size_t callbackBaseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    const size_t frameworkBaseline =
        wxWinUINotebookTestAccess::GetFrameworkRetirementCount();
    const auto runtimeBaseline =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    REQUIRE(runtimeBaseline.xamlShutdownHookInstalled);
    REQUIRE(runtimeBaseline.activeStates >= 1);
    REQUIRE(runtimeBaseline.shutdownStartingHooks >= 1);
    REQUIRE(runtimeBaseline.frameworkStartingHooks >= 1);
    REQUIRE(runtimeBaseline.frameworkHooks >= 1);
    REQUIRE(runtimeBaseline.shutdownHooks >= 1);

    const auto exerciseRejectedTurn =
        [parent,
         retirementBaseline,
         callbackBaseline,
         frameworkBaseline,
         runtimeBaseline](
            wxWinUINotebookTestAccess::PeerRetirementQueueFault fault)
        {
            CAPTURE(static_cast<int>(fault));
            wxWinUINotebookTestAccess::SetPeerRetirementQueueFault(fault);
            {
                wxNotebook notebook(
                    parent, wxID_ANY, wxDefaultPosition, wxSize(220, 140),
                    wxNB_BOTTOM | wxNB_MULTILINE | wxNB_NOPAGETHEME);
                REQUIRE(notebook.AddPage(
                    new wxPanel(&notebook), "retired page", true));
                REQUIRE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook));
            }

            REQUIRE(WaitFor(
                "WinUI notebook rejected retirement turn",
                []()
                {
                    return wxWinUINotebookTestAccess::GetPeerRetirementQueueFault() ==
                           wxWinUINotebookTestAccess::PeerRetirementQueueFault::None;
                },
                2000));
            CHECK(
                wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
                retirementBaseline + 1);
            CHECK(wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
                  callbackBaseline + 1);
            CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
                  frameworkBaseline + 1);

            // Neither rejected enqueue nor any DispatcherQueue phase is a
            // release barrier once rundown starts.
            wxWinUINotebookTestAccess::SimulateShutdownStarting();
            wxWinUINotebookTestAccess::SimulateFrameworkShutdownStarting();
            const auto starting =
                wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
            CHECK(starting.rundown);
            CHECK(starting.shutdownStartingStates ==
                  runtimeBaseline.shutdownStartingStates +
                      runtimeBaseline.activeStates);
            CHECK(starting.frameworkStartingStates ==
                  runtimeBaseline.frameworkStartingStates +
                      runtimeBaseline.activeStates);
            CHECK(
                wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
                retirementBaseline + 1);
            CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
                  frameworkBaseline + 1);

            // XamlShutdownCompletedOnThread is the only terminal consumer.
            wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
            REQUIRE(wxWinUIWaitForNotebookRetirements(
                retirementBaseline, callbackBaseline, frameworkBaseline));
            const auto xamlDone =
                wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
            CHECK(xamlDone.xamlTerminal);
            CHECK(xamlDone.xamlCompletedStates ==
                  runtimeBaseline.xamlCompletedStates +
                      runtimeBaseline.queueStates);
            CHECK(xamlDone.entries == frameworkBaseline);

            wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted();
            wxWinUINotebookTestAccess::SimulateShutdownCompleted();
            const auto completed =
                wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
            CHECK(completed.frameworkDoneStates ==
                  runtimeBaseline.frameworkDoneStates +
                      runtimeBaseline.queueStates);
            CHECK(completed.shutdownDoneStates ==
                  runtimeBaseline.shutdownDoneStates +
                      runtimeBaseline.queueStates);
            CHECK(completed.phaseOrderValid);

            // Repeated terminal notifications are exact-once no-ops.
            const auto entriesAfterXaml = completed.entries;
            wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
            wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted();
            wxWinUINotebookTestAccess::SimulateShutdownCompleted();
            CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
                  entriesAfterXaml);
            wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();
            const auto reset =
                wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
            CHECK(reset.entries == runtimeBaseline.entries);
            CHECK(reset.queueStates == runtimeBaseline.queueStates);
            CHECK(reset.activeStates == runtimeBaseline.activeStates);
            CHECK(reset.shutdownStartingHooks ==
                  runtimeBaseline.shutdownStartingHooks);
            CHECK(reset.frameworkStartingHooks ==
                  runtimeBaseline.frameworkStartingHooks);
            CHECK(reset.frameworkHooks == runtimeBaseline.frameworkHooks);
            CHECK(reset.shutdownHooks == runtimeBaseline.shutdownHooks);
        };

    exerciseRejectedTurn(
        wxWinUINotebookTestAccess::PeerRetirementQueueFault::
            RejectFirstTurn);
    exerciseRejectedTurn(
        wxWinUINotebookTestAccess::PeerRetirementQueueFault::
            RejectSecondTurn);
}

TEST_CASE("wxWinUI unbound retirement freezes at rundown until XAML",
          "[winui-notebook][lifetime][dispatcher][shutdown][ledger]")
{
    wxWinUINotebookRetirementTestGuard cleanup;
    wxWinUINotebookTestAccess::SetFrameworkRetirementHookFault(
        wxWinUINotebookTestAccess::FrameworkRetirementHookFault::None);
    wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();

    const auto baseline =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    int completions = 0;
    const auto increment = [](void *context)
    {
        ++*static_cast<int *>(context);
    };
    const std::uint64_t id =
        wxWinUINotebookTestAccess::RegisterUnboundFrameworkRetirement(
            increment, &completions);
    REQUIRE(id != 0);

    auto snapshot =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    REQUIRE(snapshot.entries == baseline.entries + 1);
    REQUIRE(snapshot.unboundEntries == baseline.unboundEntries + 1);

    wxWinUINotebookTestAccess::SimulateShutdownStarting();
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownStarting();
    wxWinUINotebookTestAccess::CompleteFrameworkRetirement(id);
    CHECK(completions == 0);
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          baseline.entries + 1);

    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
    CHECK(completions == 1);
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    CHECK(snapshot.entries == baseline.entries);
    CHECK(snapshot.unboundEntries == baseline.unboundEntries);
    CHECK(snapshot.xamlTerminal);

    wxWinUINotebookTestAccess::CompleteFrameworkRetirement(id);
    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
    CHECK(completions == 1);
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted();
    wxWinUINotebookTestAccess::SimulateShutdownCompleted();
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    CHECK(snapshot.phaseOrderValid);

    // The safe XAML deferral window has ended. A later registration is kept
    // strongly quarantined and neither Complete(id) nor a duplicate XAML
    // notification may run it after queue-terminal destruction.
    int quarantinedCompletions = 0;
    const std::uint64_t quarantinedId =
        wxWinUINotebookTestAccess::RegisterUnboundFrameworkRetirement(
            increment, &quarantinedCompletions);
    REQUIRE(quarantinedId != 0);
    wxWinUINotebookTestAccess::CompleteFrameworkRetirement(quarantinedId);
    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
    CHECK(quarantinedCompletions == 0);
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    CHECK(snapshot.entries == baseline.entries + 1);
    CHECK(snapshot.unboundEntries == baseline.unboundEntries + 1);

    // Test-only reset models an explicit safe teardown and runs before the
    // stack-backed counter leaves scope.
    wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();
    CHECK(quarantinedCompletions == 1);
}

TEST_CASE("wxWinUI Notebook retirement releases only at the XAML terminal",
          "[winui-notebook][lifetime][dispatcher][shutdown][reentrant]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUIDrainNotebookCallbacks();
    wxWinUINotebookRetirementTestGuard cleanup;
    wxWinUINotebookTestAccess::SetFrameworkRetirementHookFault(
        wxWinUINotebookTestAccess::FrameworkRetirementHookFault::None);
    wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();

    const size_t retirementBaseline =
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount();
    const size_t callbackBaseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    const size_t frameworkBaseline =
        wxWinUINotebookTestAccess::GetFrameworkRetirementCount();
    const auto active =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    REQUIRE(active.activeStates == 1);
    REQUIRE(active.shutdownStartingHooks == 1);
    REQUIRE(active.frameworkStartingHooks == 1);
    REQUIRE(active.frameworkHooks == 1);
    REQUIRE(active.shutdownHooks == 1);

    wxWinUINotebookTestAccess::SetPeerRetirementQueueFault(
        wxWinUINotebookTestAccess::PeerRetirementQueueFault::
            RejectFirstTurn);
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 140),
            wxNB_BOTTOM | wxNB_MULTILINE | wxNB_NOPAGETHEME);
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "framework predecessor", true));
    }
    REQUIRE(wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
            retirementBaseline + 1);
    REQUIRE(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
            frameworkBaseline + 1);

    // Reentrant graph creation from the first rundown notification is also
    // frozen. It cannot use a normal completion turn to release either graph.
    ReentrantNotebookRetirementContext startingContext;
    startingContext.parent = parent;
    wxWinUINotebookTestAccess::SimulateShutdownStarting(
        &ReenterNotebookRetirementPhase, &startingContext);
    CHECK(startingContext.calls == 1);
    CHECK(startingContext.created);
    CHECK(startingContext.pageAdded);
    CHECK(startingContext.usedExtendedSurface);
    CHECK(startingContext.phaseSnapshot.rundown);
    CHECK(startingContext.phaseSnapshot.shutdownStartingStates == 1);
    CHECK(startingContext.peerRetirementsAfterClose ==
          retirementBaseline + 2);
    CHECK(startingContext.frameworkEntriesAfterClose ==
          frameworkBaseline + 2);

    CHECK(wxWinUINotebookTestAccess::GetPendingPeerRetirementCount() ==
          retirementBaseline + 2);
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline + 2);

    // Terminal publication precedes the hook. Re-registration is exercised
    // with a non-XAML ticket: creating a fresh Notebook after XAML has already
    // gone terminal would itself be an invalid lifetime oracle. The new ticket
    // is consumed immediately inside the modeled deferral; both pre-existing
    // Notebook graphs are then swept exactly once.
    ReentrantLedgerRetirementContext xamlContext;
    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted(
        &ReenterLedgerRetirementAtXaml, &xamlContext);
    CHECK(xamlContext.calls == 1);
    CHECK(xamlContext.nestedId != 0);
    CHECK(xamlContext.nestedCompletions == 1);
    CHECK(xamlContext.phaseSnapshot.xamlTerminal);
    CHECK(xamlContext.phaseSnapshot.xamlCompletionActive);
    CHECK(xamlContext.phaseSnapshot.xamlCompletedStates == 1);
    REQUIRE(wxWinUIWaitForNotebookRetirements(
        retirementBaseline, callbackBaseline, frameworkBaseline));

    // The XAML notification and our FrameworkShutdownStarting handler are
    // subscribed to the same framework event, so their relative order is not
    // contractual. Publishing FrameworkStarting afterwards remains valid.
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownStarting();
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted();
    wxWinUINotebookTestAccess::SimulateShutdownCompleted();
    const auto terminal =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    CHECK(terminal.queueStates == 1);
    CHECK(terminal.activeStates == 0);
    CHECK(terminal.shutdownStartingStates == 1);
    CHECK(terminal.frameworkStartingStates == 1);
    CHECK(terminal.xamlCompletedStates == 1);
    CHECK(terminal.frameworkDoneStates == 1);
    CHECK(terminal.shutdownDoneStates == 1);
    CHECK(terminal.entries == frameworkBaseline);
    CHECK(terminal.phaseOrderValid);

    // A second XAML terminal signal cannot consume anything again.
    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
    wxWinUINotebookTestAccess::CompleteFrameworkRetirement(
        xamlContext.nestedId);
    CHECK(xamlContext.nestedCompletions == 1);
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline);
    wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();
}

TEST_CASE("wxWinUI Notebook shutdown hook gaps retain until XAML completion",
          "[winui-notebook][lifetime][dispatcher][shutdown][fault]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUIDrainNotebookCallbacks();
    wxWinUINotebookRetirementTestGuard cleanup;
    const size_t retirementBaseline =
        wxWinUINotebookTestAccess::GetPendingPeerRetirementCount();
    const size_t callbackBaseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    const size_t frameworkBaseline =
        wxWinUINotebookTestAccess::GetFrameworkRetirementCount();

    const auto retireWithRejectedBarrier = [parent]()
    {
        wxWinUINotebookTestAccess::SetPeerRetirementQueueFault(
            wxWinUINotebookTestAccess::PeerRetirementQueueFault::
                RejectFirstTurn);
        {
            wxNotebook notebook(
                parent, wxID_ANY, wxDefaultPosition, wxSize(220, 140),
                wxNB_LEFT | wxNB_MULTILINE | wxNB_NOPAGETHEME);
            REQUIRE(notebook.AddPage(
                new wxPanel(&notebook), "hook fallback", true));
        }
        REQUIRE(
            wxWinUINotebookTestAccess::GetPeerRetirementQueueFault() ==
            wxWinUINotebookTestAccess::PeerRetirementQueueFault::None);
    };

    wxWinUINotebookTestAccess::SetFrameworkRetirementHookFault(
        wxWinUINotebookTestAccess::FrameworkRetirementHookFault::
            RejectFrameworkHook);
    wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();
    auto snapshot =
        wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    REQUIRE(snapshot.queueStates == 1);
    REQUIRE(snapshot.shutdownStartingHooks == 1);
    REQUIRE(snapshot.frameworkStartingHooks == 1);
    REQUIRE(snapshot.frameworkHooks == 0);
    REQUIRE(snapshot.shutdownHooks == 1);
    retireWithRejectedBarrier();
    REQUIRE(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
            frameworkBaseline + 1);
    wxWinUINotebookTestAccess::SimulateShutdownStarting();
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownStarting();
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted();
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline + 1);
    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
    REQUIRE(wxWinUIWaitForNotebookRetirements(
        retirementBaseline, callbackBaseline, frameworkBaseline));
    wxWinUINotebookTestAccess::SimulateShutdownCompleted();
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline);
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    CHECK(snapshot.shutdownDoneStates == 1);
    CHECK(snapshot.frameworkHooks == 0);
    CHECK(snapshot.shutdownHooks == 0);

    wxWinUINotebookTestAccess::SetFrameworkRetirementHookFault(
        wxWinUINotebookTestAccess::FrameworkRetirementHookFault::
            RejectShutdownHook);
    wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    REQUIRE(snapshot.queueStates == 1);
    REQUIRE(snapshot.frameworkHooks == 1);
    REQUIRE(snapshot.shutdownHooks == 0);
    retireWithRejectedBarrier();
    wxWinUINotebookTestAccess::SimulateShutdownStarting();
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownStarting();
    wxWinUINotebookTestAccess::SimulateShutdownCompleted();
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline + 1);
    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
    REQUIRE(wxWinUIWaitForNotebookRetirements(
        retirementBaseline, callbackBaseline, frameworkBaseline));
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted();
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline);
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    CHECK(snapshot.frameworkDoneStates == 1);
    CHECK(snapshot.frameworkHooks == 0);
    CHECK(snapshot.shutdownHooks == 0);

    // Even if both completion hooks are rejected, the starting hooks still
    // establish rundown and XAML remains the only graph consumer.
    wxWinUINotebookTestAccess::SetFrameworkRetirementHookFault(
        wxWinUINotebookTestAccess::FrameworkRetirementHookFault::
            RejectBothHooks);
    wxWinUINotebookTestAccess::ResetFrameworkRetirementRuntime();
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    REQUIRE(snapshot.xamlShutdownHookInstalled);
    REQUIRE(snapshot.queueStates == 1);
    REQUIRE(snapshot.shutdownStartingHooks == 1);
    REQUIRE(snapshot.frameworkStartingHooks == 1);
    REQUIRE(snapshot.frameworkHooks == 0);
    REQUIRE(snapshot.shutdownHooks == 0);
    retireWithRejectedBarrier();
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    REQUIRE(snapshot.entries == frameworkBaseline + 1);
    REQUIRE(snapshot.unboundEntries == 0);
    wxWinUINotebookTestAccess::SimulateShutdownStarting();
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownStarting();
    wxWinUINotebookTestAccess::SimulateFrameworkShutdownCompleted();
    wxWinUINotebookTestAccess::SimulateShutdownCompleted();
    CHECK(wxWinUINotebookTestAccess::GetFrameworkRetirementCount() ==
          frameworkBaseline + 1);
    wxWinUINotebookTestAccess::SimulateXamlShutdownCompleted();
    REQUIRE(wxWinUIWaitForNotebookRetirements(
        retirementBaseline, callbackBaseline, frameworkBaseline));
    snapshot = wxWinUINotebookTestAccess::GetFrameworkRetirementSnapshot();
    CHECK(snapshot.unboundEntries == 0);
    CHECK(snapshot.xamlTerminal);
}

TEST_CASE("wxWinUI Notebook extended RTL geometry uses wx client coordinates",
          "[winui-notebook][styles][geometry][hit-test][rtl][multiline]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const std::array<long, 3> styles =
    {
        wxNB_BOTTOM | wxNB_MULTILINE | wxNB_FIXEDWIDTH,
        wxNB_LEFT | wxNB_MULTILINE | wxNB_FIXEDWIDTH,
        wxNB_RIGHT | wxNB_MULTILINE | wxNB_FIXEDWIDTH
    };
    for ( const long style : styles )
    {
        CAPTURE(style);
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition,
            parent->FromDIP(wxSize(310, 230)), style);
        notebook.SetLayoutDirection(wxLayout_RightToLeft);
        REQUIRE(notebook.GetLayoutDirection() == wxLayout_RightToLeft);
        // Force two bands in both orientations: horizontal pages wrap on
        // width and vertical pages wrap on height.
        notebook.SetTabSize(notebook.FromDIP(wxSize(82, 58)));
        constexpr size_t PageCount = 5;
        for ( size_t page = 0; page != PageCount; ++page )
        {
            REQUIRE(notebook.AddPage(
                new wxPanel(&notebook),
                wxString::Format("extended RTL %zu", page),
                page == 0));
        }

        wxWinUIDrainNotebookCallbacks();
        REQUIRE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook));
        REQUIRE(wxWinUINotebookTestAccess::IsPeerRTL(notebook));
        REQUIRE(notebook.GetRowCount() == 2);
        wxRect previous;
        for ( size_t page = 0; page != PageCount; ++page )
        {
            const wxRect logical = notebook.GetTabRect(page);
            CAPTURE(page, logical);
            wxRect actual;
            REQUIRE(WaitFor(
                "WinUI extended tab actual-tree realization",
                [&notebook, page, &actual]()
                {
                    actual =
                        wxWinUINotebookTestAccess::GetActualTabRect(notebook, page);
                    return !actual.IsEmpty();
                },
                2000));
            CAPTURE(actual);
            REQUIRE(!logical.IsEmpty());
            CHECK(std::abs(logical.x - actual.x) <= 1);
            CHECK(std::abs(logical.y - actual.y) <= 1);
            CHECK(std::abs(logical.width - actual.width) <= 1);
            CHECK(std::abs(logical.height - actual.height) <= 1);
            if ( page == 0 &&
                 (style & wxBK_ALIGN_MASK) == wxNB_BOTTOM )
            {
                // This is deliberately a second actual-tree read, not the
                // logical fallback seam: the first connected bottom/RTL item
                // must remain arranged after the bounded realization wait.
                const wxRect repeatedActual =
                    wxWinUINotebookTestAccess::GetActualTabRect(notebook, page);
                REQUIRE(!repeatedActual.IsEmpty());
                CHECK(repeatedActual == actual);
            }

            long flags = 0;
            CHECK(notebook.HitTest(
                      wxWinUIRectCentre(logical), &flags) ==
                  static_cast<int>(page));
            CHECK((flags & wxBK_HITTEST_NOWHERE) == 0);

            if ( page == 1 )
            {
                if ( (style & wxBK_ALIGN_MASK) == wxNB_BOTTOM )
                    CHECK(previous.x < logical.x);
                else
                    CHECK(previous.y < logical.y);
            }
            previous = logical;
        }
    }
}

TEST_CASE("wxWinUI Notebook extended RTL overflow stays logical and invokable",
          "[winui-notebook][geometry][hit-test][rtl][overflow][uia]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition,
        parent->FromDIP(wxSize(240, 180)),
        wxNB_BOTTOM | wxNB_FIXEDWIDTH);
    notebook.SetLayoutDirection(wxLayout_RightToLeft);
    notebook.SetTabSize(notebook.FromDIP(wxSize(100, 40)));
    constexpr size_t PageCount = 6;
    for ( size_t page = 0; page != PageCount; ++page )
    {
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook),
            wxString::Format("RTL overflow %zu", page),
            page == 0));
    }

    wxWinUIDrainNotebookCallbacks();
    REQUIRE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook));
    REQUIRE(wxWinUINotebookTestAccess::IsPeerRTL(notebook));

    const auto getPassiveActualAfterNaturalPublication =
        [&notebook](size_t page)
        {
            wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot ready;
            const auto isNear = [](int first, int second)
            {
                return std::abs(
                           static_cast<long long>(first) -
                           static_cast<long long>(second)) <= 1;
            };
            REQUIRE(WaitFor(
                "WinUI RTL overflow natural peer publication",
                [&notebook, page, &ready, isNear]()
                {
                    ready = wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, page);
                    return !ready.pending &&
                           ready.modelRevision ==
                               ready.publishedModelRevision &&
                           ready.layoutRevision ==
                               ready.publishedLayoutRevision &&
                           !ready.publishedRect.IsEmpty() &&
                           !ready.liveActualRect.IsEmpty() &&
                           isNear(ready.publishedRect.x,
                                  ready.liveActualRect.x) &&
                           isNear(ready.publishedRect.y,
                                  ready.liveActualRect.y) &&
                           isNear(ready.publishedRect.width,
                                  ready.liveActualRect.width) &&
                           isNear(ready.publishedRect.height,
                                  ready.liveActualRect.height);
                },
                2000));

            // Once the ordinary dispatcher/layout lifecycle has published a
            // live peer, the strict seam is only an observer. Two reads with
            // no pump between them must agree without advancing any epoch,
            // revision, or continuation counter.
            const auto before =
                wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, page);
            const wxRect first =
                wxWinUINotebookTestAccess::GetActualTabRect(notebook, page);
            const auto middle =
                wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, page);
            const wxRect second =
                wxWinUINotebookTestAccess::GetActualTabRect(notebook, page);
            const auto after =
                wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, page);
            const auto samePassiveState = [](const auto& firstState,
                                             const auto& secondState)
            {
                return firstState.labelIdentity ==
                           secondState.labelIdentity &&
                       firstState.probeEpoch == secondState.probeEpoch &&
                       firstState.layoutEpoch == secondState.layoutEpoch &&
                       firstState.surfaceGeneration ==
                           secondState.surfaceGeneration &&
                       firstState.modelRevision ==
                           secondState.modelRevision &&
                       firstState.layoutRevision ==
                           secondState.layoutRevision &&
                       firstState.styleRevision ==
                           secondState.styleRevision &&
                       firstState.publishedModelRevision ==
                           secondState.publishedModelRevision &&
                       firstState.publishedLayoutRevision ==
                           secondState.publishedLayoutRevision &&
                       firstState.continuationCount ==
                           secondState.continuationCount &&
                       firstState.pending == secondState.pending &&
                       firstState.naturalSize == secondState.naturalSize &&
                       firstState.publishedRect ==
                           secondState.publishedRect &&
                       firstState.actualSize == secondState.actualSize &&
                       firstState.liveActualRect ==
                           secondState.liveActualRect;
            };
            CAPTURE(page,
                    first,
                    second,
                    before.probeEpoch,
                    before.layoutEpoch,
                    before.continuationCount);
            REQUIRE(!first.IsEmpty());
            REQUIRE(second == first);
            CHECK(samePassiveState(before, middle));
            CHECK(samePassiveState(middle, after));
            return first;
        };

    const wxRect first = notebook.GetTabRect(0);
    REQUIRE(!first.IsEmpty());
    const wxRect actualFirst =
        getPassiveActualAfterNaturalPublication(0);
    CHECK(std::abs(first.x - actualFirst.x) <= 1);
    CHECK(std::abs(first.width - actualFirst.width) <= 1);
    long flags = 0;
    CHECK(notebook.HitTest(wxWinUIRectCentre(first), &flags) == 0);

    unsigned invocations = 0;
    while ( wxWinUINotebookTestAccess::InvokeTabOverflow(notebook, true) )
        REQUIRE(++invocations < 32);
    CHECK(invocations > 0);

    const wxRect last = notebook.GetTabRect(PageCount - 1);
    REQUIRE(!last.IsEmpty());
    const wxRect actualLast =
        getPassiveActualAfterNaturalPublication(PageCount - 1);
    CHECK(std::abs(last.x - actualLast.x) <= 1);
    CHECK(std::abs(last.y - actualLast.y) <= 1);
    CHECK(std::abs(last.width - actualLast.width) <= 1);
    CHECK(std::abs(last.height - actualLast.height) <= 1);
    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(last), &flags) ==
          static_cast<int>(PageCount - 1));

    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(notebook.GetTabRect(0)), &flags) ==
          wxNOT_FOUND);
}

TEST_CASE("wxWinUI Notebook hot row-count changes resize every page",
          "[winui-notebook][multiline][geometry][hot]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxSize notebookSize =
        parent->FromDIP(wxSize(360, 250));
    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, notebookSize,
        wxNB_TOP | wxNB_MULTILINE);
    for ( int page = 0; page != 3; ++page )
    {
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "tab", page == 0));
    }

    const auto requireRowsAndResize =
        [&notebook](int previousRows, const wxRect& previousRect)
        {
            const int rows = notebook.GetRowCount();
            const wxRect current = CheckNotebookPagesShareRect(notebook);
            CAPTURE(previousRows, rows, previousRect, current);
            CHECK(rows != previousRows);
            CHECK(current != previousRect);
            return std::pair<int, wxRect>(rows, current);
        };

    int rows = notebook.GetRowCount();
    REQUIRE(rows == 1);
    wxRect pageRect = CheckNotebookPagesShareRect(notebook);

    REQUIRE(notebook.SetPageText(0, wxString('W', 200)));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows > 1);
    REQUIRE(notebook.SetPageText(0, "tab"));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 1);

    notebook.SetPadding(notebook.FromDIP(wxSize(150, 6)));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 3);
    notebook.SetPadding(notebook.FromDIP(wxSize(0, 6)));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 1);

    wxBookCtrlBase::Images images;
    images.push_back(wxBitmapBundle::FromBitmap(
        wxBitmap(wxSize(512, 16), 32)));
    notebook.SetImages(images);
    REQUIRE(notebook.SetPageImage(0, 0));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows > 1);
    REQUIRE(notebook.SetPageImage(0, wxNOT_FOUND));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 1);

    notebook.SetTabSize(notebook.FromDIP(wxSize(140, 32)));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 2);
    notebook.SetTabSize(notebook.FromDIP(wxSize(100, 32)));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 1);

    REQUIRE(notebook.InsertPage(
        notebook.GetPageCount(),
        new wxPanel(&notebook),
        "inserted",
        false));
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 2);
    wxWindow * const removed =
        notebook.GetPage(notebook.GetPageCount() - 1);
    REQUIRE(removed != nullptr);
    REQUIRE(notebook.RemovePage(notebook.GetPageCount() - 1));
    delete removed;
    std::tie(rows, pageRect) = requireRowsAndResize(rows, pageRect);
    REQUIRE(rows == 1);
}

TEST_CASE("wxWinUI Notebook has one extended tab stop",
          "[winui-notebook][keyboard][tab-stop][mutation]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180),
        wxNB_BOTTOM | wxNB_MULTILINE);
    for ( int page = 0; page != 3; ++page )
    {
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook),
            wxString::Format("page %d", page),
            page == 0));
    }

    const auto checkSingleTabStop = [&notebook]()
    {
        const int selection = notebook.GetSelection();
        REQUIRE(selection >= 0);
        unsigned tabStops = 0;
        for ( size_t page = 0; page < notebook.GetPageCount(); ++page )
        {
            const bool isTabStop =
                wxWinUINotebookTestAccess::IsPeerTabStop(notebook, page);
            CHECK(isTabStop ==
                  (page == static_cast<size_t>(selection)));
            tabStops += isTabStop ? 1u : 0u;
        }
        CHECK(tabStops == 1);
    };

    checkSingleTabStop();
    CHECK(notebook.ChangeSelection(2) == 0);
    checkSingleTabStop();

    REQUIRE(notebook.InsertPage(
        1, new wxPanel(&notebook), "inserted", false));
    checkSingleTabStop();
    wxWindow * const removed = notebook.GetPage(1);
    REQUIRE(removed != nullptr);
    REQUIRE(notebook.RemovePage(1));
    delete removed;
    checkSingleTabStop();

    const size_t selected =
        static_cast<size_t>(notebook.GetSelection());
    wxWindow * const removedSelected = notebook.GetPage(selected);
    REQUIRE(removedSelected != nullptr);
    REQUIRE(notebook.RemovePage(selected));
    delete removedSelected;
    checkSingleTabStop();
}

TEST_CASE("wxWinUI Notebook extended overflow is mouse accessible",
          "[winui-notebook][overflow][uia][hit-test]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition,
        parent->FromDIP(wxSize(240, 180)),
        wxNB_BOTTOM | wxNB_FIXEDWIDTH);
    notebook.SetTabSize(notebook.FromDIP(wxSize(100, 40)));
    constexpr size_t PageCount = 6;
    for ( size_t page = 0; page != PageCount; ++page )
    {
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook),
            wxString::Format("overflow %zu", page),
            page == 0));
    }

    CHECK_FALSE(wxWinUINotebookTestAccess::InvokeTabOverflow(notebook, false));
    REQUIRE(wxWinUINotebookTestAccess::InvokeTabOverflow(notebook, true));
    unsigned forwardInvocations = 1;
    while ( wxWinUINotebookTestAccess::InvokeTabOverflow(notebook, true) )
    {
        REQUIRE(++forwardInvocations < 32);
    }
    CHECK(forwardInvocations > 1);

    long flags = 0;
    const wxRect last = notebook.GetTabRect(PageCount - 1);
    REQUIRE(!last.IsEmpty());
    CHECK(notebook.HitTest(wxWinUIRectCentre(last), &flags) ==
          static_cast<int>(PageCount - 1));
    flags = 0;
    const wxRect firstOffscreen = notebook.GetTabRect(0);
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(firstOffscreen), &flags) == wxNOT_FOUND);

    unsigned backInvocations = 0;
    while ( wxWinUINotebookTestAccess::InvokeTabOverflow(notebook, false) )
    {
        REQUIRE(++backInvocations < 32);
    }
    CHECK(backInvocations > 1);
    flags = 0;
    const wxRect first = notebook.GetTabRect(0);
    REQUIRE(!first.IsEmpty());
    CHECK(notebook.HitTest(wxWinUIRectCentre(first), &flags) == 0);
}

TEST_CASE("wxWinUI Notebook hot style mutation is last-writer-wins",
          "[winui-notebook][styles][reentrancy][hot]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(260, 180));
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "first", true));
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "second", false));

    NotebookStyleReentry probe;
    probe.newestStyle =
        wxNB_RIGHT | wxNB_MULTILINE | wxNB_FIXEDWIDTH |
        wxNB_NOPAGETHEME;
    wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::
            TabGeometryMeasure,
        &ReplaceNotebookStyleDuringGeometry,
        &probe);

    notebook.SetWindowStyleFlag(
        wxNB_BOTTOM | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
    CHECK(probe.calls == 1);
    CHECK((notebook.GetWindowStyleFlag() &
           (wxBK_ALIGN_MASK | wxNB_MULTILINE | wxNB_FIXEDWIDTH |
            wxNB_NOPAGETHEME)) == probe.newestStyle);
    REQUIRE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook));
    // RIGHT geometry must already be committed when the nested setter
    // returns; GetTabRect() below is a consumer, not the operation that makes
    // the last writer take effect.
    CHECK(notebook.GetPage(0)->GetSize().y ==
          notebook.GetClientSize().y);
    const wxRect tab = notebook.GetTabRect(0);
    REQUIRE(!tab.IsEmpty());
    CHECK(tab.x > 0);
}

TEST_CASE("wxWinUI Notebook naturally commits extended label metrics",
          "[winui-notebook][geometry][layout][dispatcher]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 190),
        wxNB_BOTTOM | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
    // Let the automatic extended metric own this oracle. An explicit
    // SetTabSize() is exact like TCM_SETITEMSIZE and can legitimately clip a
    // natural label; exact clipped behavior is covered separately below.
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "Initial intrinsic label", true));
    REQUIRE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(notebook));

    const auto requireNaturalConvergence = [&notebook](
        wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot& snapshot)
    {
        REQUIRE(wxWinUINotebookTestAccess::HasPendingExtendedLabelMetrics(notebook));
        const auto passiveBefore =
            wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, 0);
        const std::uint64_t continuationsBefore =
            wxWinUINotebookTestAccess::GetExtendedLayoutContinuationCount(notebook);

        // The strict actual-tree seam must not make progress for its caller.
        // Before pumping any event, two consecutive reads of a pending probe
        // both fail closed and leave every publication/callback revision and
        // counter unchanged.
        const wxRect firstStrictRead =
            wxWinUINotebookTestAccess::GetActualTabLabelRect(notebook, 0);
        const auto passiveMiddle =
            wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, 0);
        const wxRect secondStrictRead =
            wxWinUINotebookTestAccess::GetActualTabLabelRect(notebook, 0);
        const auto passiveAfter =
            wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, 0);
        const auto samePassiveState = [](const auto& first,
                                         const auto& second)
        {
            return first.probeEpoch == second.probeEpoch &&
                   first.layoutEpoch == second.layoutEpoch &&
                   first.surfaceGeneration == second.surfaceGeneration &&
                   first.modelRevision == second.modelRevision &&
                   first.layoutRevision == second.layoutRevision &&
                   first.styleRevision == second.styleRevision &&
                   first.publishedModelRevision ==
                       second.publishedModelRevision &&
                   first.publishedLayoutRevision ==
                       second.publishedLayoutRevision &&
                   first.continuationCount == second.continuationCount &&
                   first.pending == second.pending &&
                   first.naturalSize == second.naturalSize &&
                   first.publishedRect == second.publishedRect;
        };
        CAPTURE(firstStrictRead,
                secondStrictRead,
                passiveBefore.probeEpoch,
                passiveBefore.layoutEpoch,
                passiveBefore.modelRevision,
                passiveBefore.layoutRevision,
                passiveBefore.continuationCount);
        CHECK(firstStrictRead.IsEmpty());
        CHECK(secondStrictRead.IsEmpty());
        CHECK(passiveBefore.pending);
        CHECK(passiveBefore.modelRevision ==
              passiveBefore.publishedModelRevision);
        CHECK(passiveBefore.layoutRevision ==
              passiveBefore.publishedLayoutRevision);
        CHECK(samePassiveState(passiveBefore, passiveMiddle));
        CHECK(samePassiveState(passiveMiddle, passiveAfter));
        CHECK(wxWinUINotebookTestAccess::GetExtendedLayoutContinuationCount(notebook) ==
              continuationsBefore);

        // These are passive state reads. In particular, do not call any tab
        // rectangle seam from the WaitFor predicate: only the continuation
        // posted by LayoutUpdated is allowed to consume the probe epoch.
        const bool converged = WaitFor(
            "WinUI extended label metric natural continuation",
            [&notebook, continuationsBefore, &snapshot]()
            {
                snapshot =
                    wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, 0);
                const std::uint64_t continuations =
                    wxWinUINotebookTestAccess::GetExtendedLayoutContinuationCount(notebook);
                return continuations - continuationsBefore >= 2 &&
                       !snapshot.pending &&
                       snapshot.probeEpoch != snapshot.layoutEpoch &&
                       snapshot.actualSize.x > 0 &&
                       snapshot.actualSize.y > 0 &&
                       snapshot.naturalSize == snapshot.actualSize &&
                       snapshot.publishedRect.GetSize() ==
                           snapshot.actualSize &&
                       snapshot.liveActualRect.GetSize() ==
                           snapshot.actualSize &&
                       std::abs(snapshot.publishedRect.x -
                                snapshot.liveActualRect.x) <= 1 &&
                       std::abs(snapshot.publishedRect.y -
                                snapshot.liveActualRect.y) <= 1;
            },
            2000);
        INFO("probe epoch=" << snapshot.probeEpoch
             << ", layout epoch=" << snapshot.layoutEpoch
             << ", surface generation=" << snapshot.surfaceGeneration
             << ", label identity=" << snapshot.labelIdentity
             << ", natural=" << snapshot.naturalSize
             << ", published=" << snapshot.publishedRect
             << ", actual size=" << snapshot.actualSize
             << ", live actual=" << snapshot.liveActualRect);
        REQUIRE(converged);
        REQUIRE_FALSE(snapshot.pending);
        REQUIRE(snapshot.probeEpoch != snapshot.layoutEpoch);
        REQUIRE(snapshot.naturalSize == snapshot.actualSize);
        REQUIRE(snapshot.publishedRect.GetSize() == snapshot.actualSize);
        REQUIRE(snapshot.liveActualRect.GetSize() == snapshot.actualSize);
    };

    wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot initialMetric;
    requireNaturalConvergence(initialMetric);

    REQUIRE(notebook.SetPageText(
        0, "A substantially longer hot label for a second natural frame"));
    wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot hotMetric;
    requireNaturalConvergence(hotMetric);

    const wxRect logical =
        wxWinUINotebookTestAccess::GetTabLabelRect(notebook, 0);
    const wxRect actual =
        wxWinUINotebookTestAccess::GetActualTabLabelRect(notebook, 0);
    CAPTURE(logical, actual);
    REQUIRE(!logical.IsEmpty());
    REQUIRE(!actual.IsEmpty());
    CHECK(std::abs(logical.x - actual.x) <= 1);
    CHECK(std::abs(logical.y - actual.y) <= 1);
    CHECK(std::abs(logical.width - actual.width) <= 1);
    CHECK(std::abs(logical.height - actual.height) <= 1);
}

TEST_CASE("wxWinUI Notebook extended geometry has exact hit flags",
          "[winui-notebook][styles][geometry][hit-test]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const std::array<long, 4> alignments =
    {
        wxNB_TOP, wxNB_BOTTOM, wxNB_LEFT, wxNB_RIGHT
    };
    for ( const long alignment : alignments )
    {
        CAPTURE(alignment);
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(280, 190),
            alignment | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
        // This oracle compares the full natural XAML label with its published
        // and actual rectangles. Give every orientation enough DPI-scaled
        // room; exact clipping of a smaller explicit size is tested below.
        notebook.SetTabSize(notebook.FromDIP(wxSize(100, 48)));
        wxBookCtrlBase::Images images;
        images.push_back(wxBitmapBundle::FromBitmap(
            wxBitmap(wxSize(16, 16), 32)));
        notebook.SetImages(images);
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "&First", true, 0));

        const wxSize gdiFallback = notebook.GetTextExtent("First");
        const std::uint64_t continuationsBefore =
            wxWinUINotebookTestAccess::GetExtendedLayoutContinuationCount(notebook);
        wxWinUINotebookTestAccess::ExtendedLabelMetricSnapshot
            naturalSnapshot;
        const bool naturallyPublished = WaitFor(
            "WinUI exact-item label metric natural publication",
            [&notebook,
             &naturalSnapshot,
             gdiFallback,
             continuationsBefore]()
            {
                naturalSnapshot =
                    wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, 0);
                const std::uint64_t continuations =
                    wxWinUINotebookTestAccess::GetExtendedLayoutContinuationCount(notebook);
                return continuations - continuationsBefore >= 2 &&
                       !naturalSnapshot.pending &&
                       naturalSnapshot.labelIdentity != 0 &&
                       naturalSnapshot.surfaceGeneration != 0 &&
                       naturalSnapshot.probeEpoch !=
                           naturalSnapshot.layoutEpoch &&
                       naturalSnapshot.naturalSize != gdiFallback &&
                       naturalSnapshot.actualSize ==
                           naturalSnapshot.naturalSize &&
                       naturalSnapshot.liveActualRect.GetSize() ==
                           naturalSnapshot.naturalSize &&
                       naturalSnapshot.publishedRect.GetSize() ==
                           naturalSnapshot.naturalSize &&
                       std::abs(naturalSnapshot.publishedRect.x -
                                naturalSnapshot.liveActualRect.x) <= 1 &&
                       std::abs(naturalSnapshot.publishedRect.y -
                                naturalSnapshot.liveActualRect.y) <= 1;
            },
            2000);
        INFO("alignment=" << alignment
             << ", GDI fallback=" << gdiFallback
             << ", continuation delta=" <<
                    wxWinUINotebookTestAccess::GetExtendedLayoutContinuationCount(notebook) -
                    continuationsBefore
             << ", probe epoch=" << naturalSnapshot.probeEpoch
             << ", layout epoch=" << naturalSnapshot.layoutEpoch
             << ", surface generation=" <<
                    naturalSnapshot.surfaceGeneration
             << ", label identity=" << naturalSnapshot.labelIdentity
             << ", natural=" << naturalSnapshot.naturalSize
             << ", published=" << naturalSnapshot.publishedRect
             << ", actual size=" << naturalSnapshot.actualSize
             << ", live actual=" << naturalSnapshot.liveActualRect);
        REQUIRE(naturallyPublished);

        const wxRect item = notebook.GetTabRect(0);
        const wxRect icon =
            wxWinUINotebookTestAccess::GetTabIconRect(notebook, 0);
        const wxRect label =
            wxWinUINotebookTestAccess::GetTabLabelRect(notebook, 0);
        REQUIRE(!item.IsEmpty());
        REQUIRE(!icon.IsEmpty());
        REQUIRE(!label.IsEmpty());
        const auto beforeActual =
            wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, 0);
        INFO("before GetActual identity=" << beforeActual.labelIdentity
             << ", natural=" << beforeActual.naturalSize
             << ", published=" << beforeActual.publishedRect
             << ", actual size=" << beforeActual.actualSize
             << ", live actual=" << beforeActual.liveActualRect);
        REQUIRE(beforeActual.labelIdentity ==
                naturalSnapshot.labelIdentity);
        REQUIRE(beforeActual.publishedRect.GetSize() ==
                beforeActual.liveActualRect.GetSize());
        wxRect actualLabel;
        REQUIRE(WaitFor(
            "WinUI extended label actual-tree realization",
            [&notebook, &actualLabel]()
            {
                actualLabel =
                    wxWinUINotebookTestAccess::GetActualTabLabelRect(notebook, 0);
                return !actualLabel.IsEmpty();
            },
            2000));
        const auto afterActual =
            wxWinUINotebookTestAccess::GetExtendedLabelMetricSnapshot(notebook, 0);
        INFO("after GetActual identity=" << afterActual.labelIdentity
             << ", natural=" << afterActual.naturalSize
             << ", published=" << afterActual.publishedRect
             << ", actual size=" << afterActual.actualSize
             << ", live actual=" << afterActual.liveActualRect);
        REQUIRE(afterActual.labelIdentity ==
                naturalSnapshot.labelIdentity);
        CAPTURE(item, icon, label, actualLabel);
        CHECK(std::abs(label.x - actualLabel.x) <= 1);
        CHECK(std::abs(label.y - actualLabel.y) <= 1);
        CHECK(std::abs(label.width - actualLabel.width) <= 1);
        CHECK(std::abs(label.height - actualLabel.height) <= 1);

        long flags = 0;
        CHECK(notebook.HitTest(wxWinUIRectCentre(icon), &flags) == 0);
        CHECK(flags == wxBK_HITTEST_ONICON);
        CHECK(notebook.HitTest(wxWinUIRectCentre(label), &flags) == 0);
        CHECK(flags == wxBK_HITTEST_ONLABEL);

        if ( alignment == wxNB_LEFT || alignment == wxNB_RIGHT )
        {
            const int itemCentreX = item.x + item.width / 2;
            INFO("vertical item=" << item
                 << ", icon=" << icon
                 << ", label=" << label);
            CHECK(icon.GetLeft() >= item.GetLeft());
            CHECK(icon.GetRight() <= item.GetRight());
            CHECK(icon.GetTop() >= item.GetTop());
            CHECK(icon.GetBottom() <= item.GetBottom());
            CHECK(label.GetLeft() >= item.GetLeft());
            CHECK(label.GetRight() <= item.GetRight());
            CHECK(label.GetTop() >= item.GetTop());
            CHECK(label.GetBottom() <= item.GetBottom());
            CHECK(std::abs(
                      itemCentreX - (icon.x + icon.width / 2)) <= 1);
            CHECK(std::abs(
                      itemCentreX - (label.x + label.width / 2)) <= 1);
            if ( alignment == wxNB_LEFT )
                CHECK(label.GetBottom() < icon.GetTop());
            else
                CHECK(icon.GetBottom() < label.GetTop());

            unsigned iconHits = 0;
            unsigned labelHits = 0;
            unsigned itemHits = 0;
            for ( int y = item.GetTop() + 1;
                  y < item.GetBottom();
                  ++y )
            {
                flags = 0;
                CHECK(notebook.HitTest(
                          wxPoint(itemCentreX, y), &flags) == 0);
                if ( flags == wxBK_HITTEST_ONICON )
                    ++iconHits;
                else if ( flags == wxBK_HITTEST_ONLABEL )
                    ++labelHits;
                else if ( flags == wxBK_HITTEST_ONITEM )
                    ++itemHits;
            }
            CHECK(iconHits > 0);
            CHECK(labelHits > 0);
            CHECK(itemHits > 0);
        }

        const wxPoint pagePoint =
            alignment == wxNB_TOP
                ? wxPoint(10, notebook.GetClientSize().y - 10)
                : alignment == wxNB_BOTTOM
                    ? wxPoint(10, 10)
                    : alignment == wxNB_LEFT
                        ? wxPoint(notebook.GetClientSize().x - 10, 10)
                        : wxPoint(10, 10);
        CHECK(notebook.HitTest(pagePoint, &flags) == wxNOT_FOUND);
        CHECK((flags & wxBK_HITTEST_ONPAGE) != 0);
    }

    // TCM_SETITEMSIZE is exact, including for LEFT/RIGHT. At high DPI the
    // natural stacked header can exceed this raw client-pixel height; both
    // visible part rectangles must then be clipped inside the unchanged item
    // while retaining their exact hit flags.
    const wxSize exactTabSize(100, 42);
    for ( const long alignment : {wxNB_LEFT, wxNB_RIGHT} )
    {
        CAPTURE(alignment, exactTabSize);
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(280, 190),
            alignment | wxNB_MULTILINE | wxNB_FIXEDWIDTH);
        notebook.SetTabSize(exactTabSize);
        wxBookCtrlBase::Images images;
        images.push_back(wxBitmapBundle::FromBitmap(
            wxBitmap(wxSize(16, 16), 32)));
        notebook.SetImages(images);
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "&Exact clipped header", true, 0));
        REQUIRE(wxWinUINotebookTestAccess::HasPendingExtendedLabelMetrics(notebook));
        REQUIRE(WaitFor(
            "WinUI exact side-tab label metric publication",
            [&notebook]()
            {
                return !wxWinUINotebookTestAccess::HasPendingExtendedLabelMetrics(notebook);
            },
            2000));

        const wxRect item = notebook.GetTabRect(0);
        const wxRect icon =
            wxWinUINotebookTestAccess::GetTabIconRect(notebook, 0);
        const wxRect label =
            wxWinUINotebookTestAccess::GetTabLabelRect(notebook, 0);
        CAPTURE(item, icon, label);
        REQUIRE(item.GetSize() == exactTabSize);
        REQUIRE(!icon.IsEmpty());
        REQUIRE(!label.IsEmpty());
        CHECK(icon.GetLeft() >= item.GetLeft());
        CHECK(icon.GetRight() <= item.GetRight());
        CHECK(icon.GetTop() >= item.GetTop());
        CHECK(icon.GetBottom() <= item.GetBottom());
        CHECK(label.GetLeft() >= item.GetLeft());
        CHECK(label.GetRight() <= item.GetRight());
        CHECK(label.GetTop() >= item.GetTop());
        CHECK(label.GetBottom() <= item.GetBottom());

        wxRect actualIcon;
        wxRect actualLabel;
        REQUIRE(WaitFor(
            "WinUI exact clipped side-tab part realization",
            [&notebook, &actualIcon, &actualLabel]()
            {
                actualIcon =
                    wxWinUINotebookTestAccess::GetActualTabIconRect(notebook, 0);
                actualLabel =
                    wxWinUINotebookTestAccess::GetActualTabLabelRect(notebook, 0);
                return !actualIcon.IsEmpty() && !actualLabel.IsEmpty();
            },
            2000));
        CHECK(std::abs(actualIcon.x - icon.x) <= 1);
        CHECK(std::abs(actualIcon.y - icon.y) <= 1);
        CHECK(std::abs(actualIcon.width - icon.width) <= 1);
        CHECK(std::abs(actualIcon.height - icon.height) <= 1);
        CHECK(std::abs(actualLabel.x - label.x) <= 1);
        CHECK(std::abs(actualLabel.y - label.y) <= 1);
        CHECK(std::abs(actualLabel.width - label.width) <= 1);
        CHECK(std::abs(actualLabel.height - label.height) <= 1);

        long flags = 0;
        CHECK(notebook.HitTest(wxWinUIRectCentre(icon), &flags) == 0);
        CHECK(flags == wxBK_HITTEST_ONICON);
        CHECK(notebook.HitTest(wxWinUIRectCentre(label), &flags) == 0);
        CHECK(flags == wxBK_HITTEST_ONLABEL);
    }

    // The shared top-level drop broker is registered before these successive
    // XAML peers, and the final stock tree raises Loaded after all six extended
    // surfaces have retired. Let that successor reach its natural layout
    // boundary: a strict observation must never nest ApplyTemplate(),
    // UpdateLayout() or ForceRender() inside Loaded.
    wxNotebook stockNotebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(280, 190), wxNB_TOP);
    REQUIRE_FALSE(wxWinUINotebookTestAccess::IsUsingExtendedSurface(stockNotebook));
    REQUIRE(stockNotebook.AddPage(
        new wxPanel(&stockNotebook), "Stock successor", true));
    const size_t stockPageCount = stockNotebook.GetPageCount();
    const int stockSelection = stockNotebook.GetSelection();
    const wxString stockText = stockNotebook.GetPageText(0);
    unsigned stockGeometryRealizations = 0;
    wxWinUINotebookTestAccess::SetNextProjectionHook(stockNotebook, wxWinUINotebookTestAccess::ProjectionPoint::TabGeometryMeasure,
        &CountNotebookGeometryRealization,
        &stockGeometryRealizations);

    // Strict actual-tree reads are passive even for the stock TabView. Two
    // reads without an intervening pump must neither consume the real geometry
    // hook nor change the logical snapshot; an unarranged tree simply reports
    // an empty rectangle until its ordinary Loaded/layout work completes.
    const wxRect firstStockStrictRead =
        wxWinUINotebookTestAccess::GetActualTabLabelRect(stockNotebook, 0);
    const wxRect secondStockStrictRead =
        wxWinUINotebookTestAccess::GetActualTabLabelRect(stockNotebook, 0);
    CHECK(firstStockStrictRead == secondStockStrictRead);
    CHECK(stockGeometryRealizations == 0);
    CHECK(stockNotebook.GetPageCount() == stockPageCount);
    CHECK(stockNotebook.GetSelection() == stockSelection);
    CHECK(stockNotebook.GetPageText(0) == stockText);

    wxRect stockLabel;
    REQUIRE(WaitFor(
        "WinUI stock notebook after extended metric continuations",
        [&stockNotebook, &stockLabel]()
        {
            stockLabel =
                wxWinUINotebookTestAccess::GetActualTabLabelRect(stockNotebook, 0);
            return !stockLabel.IsEmpty();
        },
        2000));
    CHECK(stockGeometryRealizations == 0);

    // The cache-enabled seam still performs the real stock realization and
    // consumes the armed hook, proving that the passive checks above did not
    // merely observe an inert test double.
    CHECK_FALSE(wxWinUINotebookTestAccess::GetTabLabelRect(stockNotebook, 0).IsEmpty());
    CHECK(stockGeometryRealizations == 1);
}

TEST_CASE("wxWinUI Notebook extended UIA SelectionItem selects wx pages",
          "[winui-notebook][styles][uia][selection][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUIDrainNotebookCallbacks();
    const size_t callbackBaseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    int changing = 0;
    int changed = 0;
    std::unique_ptr<wxNotebook> notebook(new wxNotebook(
        parent,
        wxID_ANY,
        wxDefaultPosition,
        wxSize(300, 190),
        wxNB_RIGHT | wxNB_MULTILINE | wxNB_FIXEDWIDTH));
    REQUIRE(notebook->AddPage(
        new wxPanel(notebook.get()), "first", true));
    REQUIRE(notebook->AddPage(
        new wxPanel(notebook.get()), "second", false));
    notebook->Bind(
        wxEVT_NOTEBOOK_PAGE_CHANGING,
        [&changing](wxBookCtrlEvent&)
        {
            ++changing;
        });
    notebook->Bind(
        wxEVT_NOTEBOOK_PAGE_CHANGED,
        [&changed](wxBookCtrlEvent&)
        {
            ++changed;
        });

    REQUIRE(!notebook->GetTabRect(1).IsEmpty());
    std::vector<MUXC::TabViewItem> oldItems =
        wxWinUIGetExtendedNotebookItems(notebook.get());
    REQUIRE(oldItems.size() == 2);
    const MUXAPR::ISelectionItemProvider firstProvider =
        wxWinUIGetSelectionItemProvider(oldItems[0]);
    const MUXAPR::ISelectionItemProvider staleSecondProvider =
        wxWinUIGetSelectionItemProvider(oldItems[1]);
    REQUIRE(firstProvider);
    REQUIRE(staleSecondProvider);
    CHECK(firstProvider.IsSelected());
    CHECK_FALSE(staleSecondProvider.IsSelected());

    // Exercise the real peer/provider path used by a UIA client. The
    // standalone TabViewItem must reconcile both wx events and both peer
    // IsSelected values exactly once.
    staleSecondProvider.Select();
    CHECK(notebook->GetSelection() == 1);
    CHECK(changing == 1);
    CHECK(changed == 1);
    CHECK_FALSE(firstProvider.IsSelected());
    CHECK(staleSecondProvider.IsSelected());

    CHECK(notebook->SetSelection(0) == 1);
    CHECK(notebook->GetSelection() == 0);
    CHECK(changing == 2);
    CHECK(changed == 2);
    CHECK(firstProvider.IsSelected());
    CHECK_FALSE(staleSecondProvider.IsSelected());

    // Adding a page rebuilds the extended peers. A retained provider for the
    // detached generation may still mutate its old item, but must be inert.
    REQUIRE(notebook->AddPage(
        new wxPanel(notebook.get()), "third", false));
    std::vector<MUXC::TabViewItem> currentItems =
        wxWinUIGetExtendedNotebookItems(notebook.get());
    REQUIRE(currentItems.size() == 3);
    CHECK(currentItems[1] != oldItems[1]);
    try
    {
        staleSecondProvider.Select();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    CHECK(notebook->GetSelection() == 0);
    CHECK(changing == 2);
    CHECK(changed == 2);

    const MUXAPR::ISelectionItemProvider currentSecondProvider =
        wxWinUIGetSelectionItemProvider(currentItems[1]);
    REQUIRE(currentSecondProvider);
    CHECK_FALSE(currentSecondProvider.IsSelected());
    currentSecondProvider.Select();
    CHECK(notebook->GetSelection() == 1);
    CHECK(changing == 3);
    CHECK(changed == 3);
    CHECK(currentSecondProvider.IsSelected());

    // Leave both retained providers unselected so Select() after teardown
    // would cross their callbacks if either token survived rebuild/Close.
    oldItems[1].IsSelected(false);
    CHECK(notebook->ChangeSelection(0) == 1);
    CHECK_FALSE(staleSecondProvider.IsSelected());
    CHECK_FALSE(currentSecondProvider.IsSelected());
    CHECK(changing == 3);
    CHECK(changed == 3);

    wxWeakRef<wxWindow> lifetime(notebook.get());
    notebook.reset();
    CHECK(lifetime.get() == nullptr);
    CHECK(
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
        callbackBaseline + 1);
    try
    {
        staleSecondProvider.Select();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    try
    {
        currentSecondProvider.Select();
    }
    catch ( const winrt::hresult_error& )
    {
    }
    CHECK(changing == 3);
    CHECK(changed == 3);
    wxWinUIDrainNotebookCallbacks();
    CHECK(
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
        callbackBaseline);
}

TEST_CASE("wxWinUI Notebook exposes realized tab geometry",
          "[winui-notebook][geometry][hit-test]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition,
        parent->FromDIP(wxSize(400, 190)));
    wxBookCtrlBase::Images images;
    images.push_back(
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(16, 16), 32)));
    notebook.SetImages(images);

    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "First measured tab", true, 0));
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "Second measured tab", false, 0));

    wxWinUIDrainNotebookCallbacks();

    const wxRect first = notebook.GetTabRect(0);
    const wxRect second = notebook.GetTabRect(1);
    INFO("first=" << first << ", second=" << second);
    REQUIRE(!first.IsEmpty());
    REQUIRE(!second.IsEmpty());
    CHECK(first.GetRight() < second.GetRight());
    CHECK(first.Intersect(second).IsEmpty());
    CHECK(first.GetTop() >= 0);
    CHECK(first.GetBottom() < notebook.GetClientSize().y);

    wxRect actualFirst;
    wxRect actualSecond;
    REQUIRE(WaitFor(
        "WinUI stock tab actual-tree realization",
        [&notebook, &actualFirst, &actualSecond]()
        {
            actualFirst =
                wxWinUINotebookTestAccess::GetActualTabRect(notebook, 0);
            actualSecond =
                wxWinUINotebookTestAccess::GetActualTabRect(notebook, 1);
            return !actualFirst.IsEmpty() &&
                   !actualSecond.IsEmpty();
        },
        2000));
    INFO("actual first=" << actualFirst
         << ", actual second=" << actualSecond);
    REQUIRE(!actualFirst.IsEmpty());
    REQUIRE(!actualSecond.IsEmpty());

    // The first GetTabRect() calls above validate the synchronous logical
    // fallback. Once the ordinary XAML dispatcher turn has realized the
    // containers, the public geometry must converge exactly to the tree.
    const wxRect refreshedFirst = notebook.GetTabRect(0);
    const wxRect refreshedSecond = notebook.GetTabRect(1);
    INFO("refreshed first=" << refreshedFirst
         << ", refreshed second=" << refreshedSecond);
    CHECK(refreshedFirst == actualFirst);
    CHECK(refreshedSecond == actualSecond);

    const wxRect icon = wxWinUINotebookTestAccess::GetTabIconRect(notebook, 0);
    const wxRect label = wxWinUINotebookTestAccess::GetTabLabelRect(notebook, 0);
    const wxRect actualIcon =
        wxWinUINotebookTestAccess::GetActualTabIconRect(notebook, 0);
    const wxRect actualLabel =
        wxWinUINotebookTestAccess::GetActualTabLabelRect(notebook, 0);
    INFO("icon=" << icon << ", label=" << label
         << ", actual icon=" << actualIcon
         << ", actual label=" << actualLabel);
    REQUIRE(!icon.IsEmpty());
    REQUIRE(!label.IsEmpty());
    REQUIRE(!actualIcon.IsEmpty());
    REQUIRE(!actualLabel.IsEmpty());
    CHECK(refreshedFirst.Contains(icon));
    CHECK(refreshedFirst.Contains(label));
    CHECK(icon.Intersect(label).IsEmpty());
    CHECK(refreshedFirst.Contains(actualIcon));
    CHECK(refreshedFirst.Contains(actualLabel));
    CHECK(actualIcon.Intersect(actualLabel).IsEmpty());

    long flags = 0;
    CHECK(notebook.HitTest(wxWinUIRectCentre(actualIcon), &flags) == 0);
    CHECK(flags == wxBK_HITTEST_ONICON);

    flags = 0;
    CHECK(notebook.HitTest(wxWinUIRectCentre(actualLabel), &flags) == 0);
    CHECK(flags == wxBK_HITTEST_ONLABEL);

    const wxPoint gutter =
        wxWinUIFindTabGutter(refreshedFirst, icon, label);
    REQUIRE(gutter != wxDefaultPosition);
    flags = 0;
    CHECK(notebook.HitTest(gutter, &flags) == 0);
    CHECK(flags == wxBK_HITTEST_ONITEM);

    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(refreshedSecond), &flags) == 1);
    CHECK((flags == wxBK_HITTEST_ONICON ||
           flags == wxBK_HITTEST_ONLABEL ||
           flags == wxBK_HITTEST_ONITEM));

    const wxPoint pagePoint(
        2,
        wxMin(notebook.GetClientSize().y - 1,
              refreshedFirst.GetBottom() + 20));
    REQUIRE(pagePoint.y > refreshedFirst.GetBottom());
    flags = 0;
    CHECK(notebook.HitTest(pagePoint, &flags) == wxNOT_FOUND);
    CHECK((flags & wxBK_HITTEST_NOWHERE) != 0);
    CHECK((flags & wxBK_HITTEST_ONPAGE) != 0);

    flags = 0;
    CHECK(notebook.HitTest(wxPoint(-1, -1), &flags) == wxNOT_FOUND);
    CHECK(flags == wxBK_HITTEST_NOWHERE);
}

TEST_CASE("wxWinUI Notebook separates logical overflow geometry from hits",
          "[winui-notebook][geometry][overflow]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(220, 170));
    notebook.SetTabSize(wxSize(120, 44));
    constexpr size_t PageCount = 30;
    for ( size_t page = 0; page < PageCount; ++page )
    {
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "Overflow tab", page == 0));
    }

    const wxRect first = notebook.GetTabRect(0);
    REQUIRE(!first.IsEmpty());
    const wxRect logicalLast = notebook.GetTabRect(PageCount - 1);
    REQUIRE(!logicalLast.IsEmpty());
    CHECK(std::abs(logicalLast.width - 120) <= 1);
    CHECK(std::abs(logicalLast.height - 44) <= 1);

    long flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(logicalLast), &flags) == wxNOT_FOUND);
    CHECK(flags == wxBK_HITTEST_NOWHERE);

    const wxPoint overflowChrome(
        notebook.GetClientSize().x - 2,
        wxWinUIRectCentre(first).y);
    flags = 0;
    CHECK(notebook.HitTest(overflowChrome, &flags) == wxNOT_FOUND);

    // Selecting the distant item asks TabView to scroll it into view. Its
    // public logical rectangle remains non-empty. A realized item becomes
    // hit-testable; a virtualized/private-desktop item stays fail-closed.
    CHECK(notebook.ChangeSelection(PageCount - 1) == 0);
    const wxRect last = notebook.GetTabRect(PageCount - 1);
    const wxRect actualLast =
        wxWinUINotebookTestAccess::GetActualTabRect(notebook, PageCount - 1);
    INFO("first=" << first << ", realized last=" << last
         << ", actual last=" << actualLast);
    REQUIRE(!last.IsEmpty());
    CHECK(std::abs(last.width - 120) <= 1);
    CHECK(std::abs(last.height - 44) <= 1);
    flags = 0;
    const int lastHit =
        notebook.HitTest(wxWinUIRectCentre(last), &flags);
    if ( actualLast.IsEmpty() )
    {
        // On an isolated/non-input desktop the virtualized TabView can have
        // no realized viewport. The bridge must fail closed rather than
        // guessing the width of its overflow buttons.
        CHECK(lastHit == wxNOT_FOUND);
        CHECK(flags == wxBK_HITTEST_NOWHERE);
    }
    else
    {
        CHECK(lastHit == static_cast<int>(PageCount - 1));
    }

    const wxRect logicalFirstAfterScroll = notebook.GetTabRect(0);
    REQUIRE(!logicalFirstAfterScroll.IsEmpty());
    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(logicalFirstAfterScroll), &flags) ==
          wxNOT_FOUND);
}

TEST_CASE("wxWinUI Notebook logical geometry mirrors in RTL",
          "[winui-notebook][geometry][rtl][dpi]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition,
        parent->FromDIP(wxSize(270, 190)));
    const wxSize tabSize = notebook.FromDIP(wxSize(101, 43));
    notebook.SetTabSize(tabSize);
    wxBookCtrlBase::Images images;
    images.push_back(
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(16, 16), 32)));
    notebook.SetImages(images);
    constexpr size_t PageCount = 12;
    for ( size_t page = 0; page < PageCount; ++page )
    {
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook),
            wxString::Format("logical tab %zu", page),
            page == 0,
            0));
    }

    wxWinUIDrainNotebookCallbacks();

    std::vector<wxRect> ltr;
    for ( size_t page = 0; page < PageCount; ++page )
    {
        const wxRect rect = notebook.GetTabRect(page);
        REQUIRE(!rect.IsEmpty());
        CHECK(std::abs(rect.width - tabSize.x) <= 1);
        CHECK(std::abs(rect.height - tabSize.y) <= 1);
        if ( page != 0 )
        {
            CHECK(rect.x > ltr.back().x);
            CHECK(rect.x >= ltr.back().GetRight());
            CHECK(rect.x <= ltr.back().GetRight() + 1);
        }
        ltr.push_back(rect);
    }

    notebook.SetLayoutDirection(wxLayout_RightToLeft);
    wxWinUIDrainNotebookCallbacks();
    REQUIRE(notebook.GetLayoutDirection() == wxLayout_RightToLeft);
    REQUIRE((::GetWindowLongPtr(
                 static_cast<HWND>(notebook.GetHWND()),
                 GWL_EXSTYLE) &
             WS_EX_LAYOUTRTL) != 0);
    REQUIRE(wxWinUINotebookTestAccess::IsPeerRTL(notebook));
    std::vector<wxRect> rtl;
    for ( size_t page = 0; page < PageCount; ++page )
    {
        const wxRect rect = notebook.GetTabRect(page);
        REQUIRE(!rect.IsEmpty());
        CHECK(std::abs(rect.width - tabSize.x) <= 1);
        CHECK(std::abs(rect.height - tabSize.y) <= 1);
        if ( page != 0 )
        {
            CAPTURE(page, rect, rtl.back());
            CHECK(rect.x > rtl.back().x);
            CHECK(rect.x >= rtl.back().GetRight());
            CHECK(rect.x <= rtl.back().GetRight() + 1);
        }
        rtl.push_back(rect);
    }

    // GetTabRect() is public wx client geometry. WS_EX_LAYOUTRTL mirrors the
    // physical XAML strip into this logical space, so page order remains
    // increasing just as it does for native wx controls.
    CHECK(rtl.front().x < rtl.back().x);

    const wxRect actualRTLFirst =
        wxWinUINotebookTestAccess::GetActualTabRect(notebook, 0);
    const wxRect actualRTLSecond =
        wxWinUINotebookTestAccess::GetActualTabRect(notebook, 1);
    const wxRect actualRTLIcon =
        wxWinUINotebookTestAccess::GetActualTabIconRect(notebook, 0);
    const wxRect actualRTLLabel =
        wxWinUINotebookTestAccess::GetActualTabLabelRect(notebook, 0);
    const wxRect logicalRTLIcon =
        wxWinUINotebookTestAccess::GetTabIconRect(notebook, 0);
    const wxRect logicalRTLLabel =
        wxWinUINotebookTestAccess::GetTabLabelRect(notebook, 0);
    INFO("actual RTL first=" << actualRTLFirst
         << ", second=" << actualRTLSecond
         << ", icon=" << actualRTLIcon
         << ", label=" << actualRTLLabel
         << ", logical icon=" << logicalRTLIcon
         << ", logical label=" << logicalRTLLabel);
    REQUIRE(!actualRTLFirst.IsEmpty());
    REQUIRE(!actualRTLSecond.IsEmpty());
    REQUIRE(!actualRTLIcon.IsEmpty());
    REQUIRE(!actualRTLLabel.IsEmpty());
    CHECK(actualRTLFirst.x < actualRTLSecond.x);
    CHECK(actualRTLFirst.Contains(actualRTLIcon));
    CHECK(actualRTLFirst.Contains(actualRTLLabel));
    CHECK(actualRTLIcon.Intersect(actualRTLLabel).IsEmpty());
    CHECK(actualRTLIcon.x < actualRTLLabel.x);
    REQUIRE(!logicalRTLIcon.IsEmpty());
    REQUIRE(!logicalRTLLabel.IsEmpty());
    CHECK(logicalRTLIcon.Contains(wxWinUIRectCentre(actualRTLIcon)));
    CHECK(logicalRTLLabel.Contains(wxWinUIRectCentre(actualRTLLabel)));

    long flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(actualRTLIcon), &flags) == 0);
    CHECK(flags == wxBK_HITTEST_ONICON);
    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(actualRTLLabel), &flags) == 0);
    CHECK(flags == wxBK_HITTEST_ONLABEL);
    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(rtl.back()), &flags) == wxNOT_FOUND);

    CHECK(notebook.ChangeSelection(PageCount - 1) == 0);
    wxWinUIDrainNotebookCallbacks();
    const wxRect visibleLast = notebook.GetTabRect(PageCount - 1);
    const wxRect actualVisibleLast =
        wxWinUINotebookTestAccess::GetActualTabRect(notebook, PageCount - 1);
    REQUIRE(!visibleLast.IsEmpty());
    REQUIRE(!actualVisibleLast.IsEmpty());
    flags = 0;
    const int visibleLastHit =
        notebook.HitTest(
            wxWinUIRectCentre(actualVisibleLast), &flags);
    CAPTURE(visibleLast, actualVisibleLast, visibleLastHit, flags);
    CHECK(visibleLastHit == static_cast<int>(PageCount - 1));

    const wxRect offscreenFirst = notebook.GetTabRect(0);
    REQUIRE(!offscreenFirst.IsEmpty());
    flags = 0;
    const int offscreenFirstHit =
        notebook.HitTest(wxWinUIRectCentre(offscreenFirst), &flags);
    CAPTURE(offscreenFirst, offscreenFirstHit, flags);
    CHECK(offscreenFirstHit == wxNOT_FOUND);

    notebook.SetLayoutDirection(wxLayout_LeftToRight);
    wxWinUIDrainNotebookCallbacks();
    REQUIRE(notebook.GetLayoutDirection() == wxLayout_LeftToRight);
    REQUIRE_FALSE(wxWinUINotebookTestAccess::IsPeerRTL(notebook));
    wxRect previous;
    for ( size_t page = 0; page < PageCount; ++page )
    {
        const wxRect rect = notebook.GetTabRect(page);
        REQUIRE(!rect.IsEmpty());
        if ( page != 0 )
            CHECK(rect.x > previous.x);
        previous = rect;
    }

    const wxRect actualLTRLast =
        wxWinUINotebookTestAccess::GetActualTabRect(notebook, PageCount - 1);
    REQUIRE(!actualLTRLast.IsEmpty());
    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(actualLTRLast), &flags) ==
          static_cast<int>(PageCount - 1));

    // A second hot transition must re-scroll the still-selected distant tab;
    // visibility cannot rely on the direction used by the previous layout.
    notebook.SetLayoutDirection(wxLayout_RightToLeft);
    wxWinUIDrainNotebookCallbacks();
    REQUIRE(wxWinUINotebookTestAccess::IsPeerRTL(notebook));
    const wxRect actualRTLLastAgain =
        wxWinUINotebookTestAccess::GetActualTabRect(notebook, PageCount - 1);
    REQUIRE(!actualRTLLastAgain.IsEmpty());
    flags = 0;
    CHECK(notebook.HitTest(
              wxWinUIRectCentre(actualRTLLastAgain), &flags) ==
          static_cast<int>(PageCount - 1));
}

TEST_CASE("wxWinUI Notebook geometry queries are reentrancy-safe",
          "[winui-notebook][geometry][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("nested insertion supersedes the outer target")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(420, 180));
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "original", true));

        NotebookGeometryInsert probe;
        wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::
                TabGeometryMeasure,
            &InsertNotebookPageDuringGeometry,
            &probe);

        CHECK(notebook.GetTabRect(0).IsEmpty());
        CHECK(probe.calls == 1);
        CHECK(probe.nestedRect.IsEmpty());
        REQUIRE(probe.inserted);
        REQUIRE(notebook.GetPageCount() == 2);
        CHECK(notebook.GetPageText(0) == "nested inserted");
        CHECK(notebook.GetPageText(1) == "original");

        const wxRect inserted = notebook.GetTabRect(0);
        const wxRect original = notebook.GetTabRect(1);
        REQUIRE(!inserted.IsEmpty());
        REQUIRE(!original.IsEmpty());
        CHECK(inserted.Intersect(original).IsEmpty());
    }

    SECTION("resize invalidates the in-flight client snapshot")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(340, 170));
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "first", true));
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "second", false));

        NotebookGeometryResize probe;
        probe.size = wxSize(520, 210);
        wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::
                TabGeometryMeasure,
            &ResizeNotebookDuringGeometry,
            &probe);

        CHECK(notebook.GetTabRect(1).IsEmpty());
        CHECK(probe.calls == 1);
        CHECK(notebook.GetSize() == probe.size);
        CHECK_FALSE(notebook.GetTabRect(1).IsEmpty());
    }

    SECTION("destruction is terminal")
    {
        wxWinUIDrainNotebookCallbacks();
        const size_t baseline =
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
        wxNotebook * const doomed = new wxNotebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(340, 170));
        REQUIRE(doomed->AddPage(
            new wxPanel(doomed), "doomed", true));
        wxWeakRef<wxWindow> lifetime(doomed);

        NotebookDestroyProjection probe;
        wxWinUINotebookTestAccess::SetNextProjectionHook(*doomed, wxWinUINotebookTestAccess::ProjectionPoint::
                TabGeometryMeasure,
            &DestroyNotebookDuringProjection,
            &probe);
        CHECK(doomed->GetTabRect(0).IsEmpty());
        CHECK(probe.calls == 1);
        CHECK(lifetime.get() == nullptr);
        wxWinUIDrainNotebookCallbacks();
        CHECK(
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
            baseline);
    }
}

TEST_CASE("wxWinUI Notebook CalcSizeFromPage uses the native tab band",
          "[winui-notebook][geometry][metrics][dpi]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(320, 170));
    const wxSize pageSize(211, 127);
    const int margin = notebook.FromDIP(8);
    const int fallbackBand = notebook.FromDIP(40);

    // With no realized page, CalcSizeFromPage uses the DPI-scaled native
    // fallback band and margin.
    CHECK(notebook.CalcSizeFromPage(pageSize) ==
          wxSize(pageSize.x + margin,
                 pageSize.y + fallbackBand + margin));

    // SetTabSize() takes wx client pixels. Before a native measurement exists,
    // its 96-pixel height is the band oracle and must not be DPI-scaled again.
    notebook.SetTabSize(wxSize(120, 96));
    CHECK(notebook.CalcSizeFromPage(pageSize) ==
          wxSize(pageSize.x + margin,
                 pageSize.y + 96 + margin));
}

TEST_CASE("wxWinUI Notebook refreshes every image generation",
          "[winui-notebook][images][dpi]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    const wxBitmapBundle bundle16 =
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(16, 16), 32));
    wxBookCtrlBase::Images images16{bundle16};
    notebook.SetImages(images16);
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "first", true, 0));
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "second", false, 0));
    const wxSize expected16 =
        bundle16.GetPreferredBitmapSizeAtScale(
            notebook.GetDPIScaleFactor());
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) ==
          expected16);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 1) ==
          expected16);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) ==
          wxSize(16, 16));
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 1) ==
          wxSize(16, 16));

    const std::uint64_t oldFirst =
        wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0);
    const std::uint64_t oldSecond =
        wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 1);
    REQUIRE(oldFirst != 0);
    REQUIRE(oldSecond != 0);

    const wxBitmapBundle bundle24 =
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(24, 24), 32));
    wxBookCtrlBase::Images images24{bundle24};
    notebook.SetImages(images24);
    const std::uint64_t refreshed =
        wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0);
    CHECK(refreshed > oldFirst);
    CHECK(refreshed > oldSecond);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 1) == refreshed);
    const wxSize expected24 =
        bundle24.GetPreferredBitmapSizeAtScale(
            notebook.GetDPIScaleFactor());
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) ==
          expected24);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 1) ==
          expected24);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) ==
          wxSize(24, 24));
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 1) ==
          wxSize(24, 24));

    notebook.SetImages(wxBookCtrlBase::Images{});
    const std::uint64_t cleared =
        wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0);
    CHECK(cleared > refreshed);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 1) == cleared);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) == wxSize());
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 1) == wxSize());
    CHECK(wxWinUINotebookTestAccess::GetTabIconRect(notebook, 0).IsEmpty());
    CHECK(wxWinUINotebookTestAccess::GetTabIconRect(notebook, 1).IsEmpty());
}

TEST_CASE("wxWinUI Notebook image projection follows a full DPI cycle",
          "[winui-notebook][images][dpi][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    const wxBitmapBundle bundle = MakeNotebookDPIBundle();
    wxBookCtrlBase::Images images;
    images.push_back(bundle);
    notebook.SetImages(images);

    std::array<wxPanel *, 3> pages{
        new wxPanel(&notebook),
        new wxPanel(&notebook),
        new wxPanel(&notebook)
    };
    REQUIRE(notebook.AddPage(pages[0], "first", false, 0));
    REQUIRE(notebook.AddPage(pages[1], "second", true, 0));
    REQUIRE(notebook.AddPage(pages[2], "third", false, 0));
    REQUIRE(notebook.GetSelection() == 1);

    constexpr std::array<double, 4> Scales{
        1.0, 1.5, 2.0, 1.0
    };
    std::uint64_t lastGeneration =
        wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0);
    REQUIRE(lastGeneration != 0);

    for ( const double scale : Scales )
    {
        CAPTURE(scale);
        REQUIRE(wxWinUINotebookTestAccess::RefreshForScale(notebook, scale));

        const wxSize expected =
            bundle.GetPreferredBitmapSizeAtScale(scale);
        const std::uint64_t generation =
            wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0);
        CHECK(generation > lastGeneration);
        for ( size_t page = 0; page < pages.size(); ++page )
        {
            CHECK(notebook.GetPage(page) == pages[page]);
            CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, page) ==
                  expected);
            CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, page) ==
                  wxSize(16, 16));
            CHECK(wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, page) ==
                  generation);
        }
        CHECK(notebook.GetPageCount() == pages.size());
        CHECK(notebook.GetSelection() == 1);
        lastGeneration = generation;
    }
}

TEST_CASE("wxWinUI Notebook DPI event projects exactly once with or without images",
          "[winui-notebook][images][dpi][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const bool withImages = GENERATE(false, true);
    CAPTURE(withImages);
    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    if ( withImages )
    {
        notebook.SetImages(
            wxBookCtrlBase::Images{MakeNotebookDPIBundle()});
    }
    notebook.SetPadding(parent->FromDIP(wxSize(9, 5)));
    notebook.SetTabSize(parent->FromDIP(wxSize(120, 42)));
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "page", true,
        withImages ? 0 : wxBookCtrlBase::NO_IMAGE));

    const std::uint64_t before =
        wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0);
    REQUIRE(before != 0);
    wxDPIChangedEvent dpiEvent(
        wxSize(96, 96), wxSize(144, 144));
    dpiEvent.SetEventObject(&notebook);
    dpiEvent.SetId(notebook.GetId());
    notebook.ProcessWindowEvent(dpiEvent);

    CHECK(wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0) ==
          before + 1);
    CHECK(wxWinUINotebookTestAccess::GetPeerTabPadding(notebook, 0) ==
          parent->FromDIP(wxSize(9, 5)));
    CHECK(wxWinUINotebookTestAccess::GetPeerTabSize(notebook, 0) ==
          parent->FromDIP(wxSize(120, 42)));
}

TEST_CASE("wxWinUI Notebook keeps wxImageList compatibility projection",
          "[winui-notebook][images][imagelist][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxImageList images(19, 19, true);
    REQUIRE(images.Add(wxBitmap(wxSize(19, 19), 32)) != -1);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    notebook.SetImageList(&images);
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "image-list page", true, 0));

    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) ==
          wxSize(19, 19));
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) ==
          wxSize(19, 19));
    CHECK(notebook.GetSelection() == 0);
}

TEST_CASE("wxWinUI Notebook bitmap callbacks are transactional",
          "[winui-notebook][images][reentrant][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const NotebookBundleCallbackPoint callbackPoint =
        GENERATE(NotebookBundleCallbackPoint::DefaultSize,
                 NotebookBundleCallbackPoint::PreferredSize,
                 NotebookBundleCallbackPoint::Bitmap);
    CAPTURE(callbackPoint);

    auto callback = std::make_shared<ArmedNotebookBundleCallback>();
    callback->point = callbackPoint;
    const wxBitmapBundle reentrant =
        MakeCallbackNotebookBundle(callback);
    wxBookCtrlBase::Images initialImages{reentrant};

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    notebook.SetImages(initialImages);
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "page", true, 0));

    const wxBitmapBundle newest =
        wxBitmapBundle::FromBitmap(
            wxBitmap(wxSize(23, 23), 32));
    callback->callback =
        [&notebook, newest]()
        {
            notebook.SetImages(
                wxBookCtrlBase::Images{newest});
        };
    callback->armed = true;

    // This is the ordinary production path: no DPI seam/override is active.
    notebook.SetImages(initialImages);

    CHECK(callback->calls == 1);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) ==
          newest.GetPreferredBitmapSizeAtScale(
              notebook.GetDPIScaleFactor()));
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) ==
          wxSize(23, 23));
    CHECK(notebook.GetPageCount() == 1);
    CHECK(notebook.GetSelection() == 0);
}

TEST_CASE("wxWinUI Notebook destruction inside bitmap callback is safe",
          "[winui-notebook][images][reentrant][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxWinUIDrainNotebookCallbacks();
    const size_t baseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    auto callback = std::make_shared<ArmedNotebookBundleCallback>();
    const NotebookBundleCallbackPoint callbackPoint =
        GENERATE(NotebookBundleCallbackPoint::DefaultSize,
                 NotebookBundleCallbackPoint::Bitmap);
    CAPTURE(callbackPoint);
    callback->point = callbackPoint;
    const wxBitmapBundle bundle =
        MakeCallbackNotebookBundle(callback);
    wxBookCtrlBase::Images images{bundle};

    wxNotebook * const doomed = new wxNotebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    doomed->SetImages(images);
    REQUIRE(doomed->AddPage(
        new wxPanel(doomed), "page", true, 0));
    wxWeakRef<wxWindow> lifetime(doomed);

    callback->callback =
        [doomed]()
        {
            doomed->Destroy();
        };
    const unsigned bitmapCallsBefore = callback->bitmapCalls;
    callback->armed = true;
    doomed->SetImages(images);

    CHECK(callback->calls == 1);
    CHECK(callback->bitmapCalls ==
          bitmapCallsBefore +
              (callbackPoint == NotebookBundleCallbackPoint::Bitmap
                   ? 1u
                   : 0u));
    CHECK(lifetime.get() == nullptr);
    CHECK(wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
          baseline + 1);
    wxWinUIDrainNotebookCallbacks();
    CHECK(wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
          baseline);
}

TEST_CASE("wxWinUI Notebook consensus stops at a dead owner",
          "[winui-notebook][images][reentrant][lifetime][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const size_t baseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    auto first = std::make_shared<ArmedNotebookBundleCallback>();
    auto second = std::make_shared<ArmedNotebookBundleCallback>();
    first->point = NotebookBundleCallbackPoint::PreferredSize;
    second->point = NotebookBundleCallbackPoint::PreferredSize;
    const wxBookCtrlBase::Images images{
        MakeCallbackNotebookBundle(first),
        MakeCallbackNotebookBundle(second)
    };

    wxNotebook * const doomed = new wxNotebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    doomed->SetImages(images);
    REQUIRE(doomed->AddPage(
        new wxPanel(doomed), "page", true, 0));
    wxWeakRef<wxWindow> lifetime(doomed);

    first->callback = [doomed]() { doomed->Destroy(); };
    second->callback = []() {};
    first->armed = true;
    second->armed = true;
    doomed->SetImages(images);

    CHECK(first->calls == 1);
    CHECK(second->calls == 0);
    CHECK(lifetime.get() == nullptr);
    wxWinUIDrainNotebookCallbacks();
    CHECK(wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
          baseline);
}

TEST_CASE("wxWinUI Notebook consensus stops at a stale revision",
          "[winui-notebook][images][reentrant][transaction][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    auto first = std::make_shared<ArmedNotebookBundleCallback>();
    auto second = std::make_shared<ArmedNotebookBundleCallback>();
    first->point = NotebookBundleCallbackPoint::PreferredSize;
    second->point = NotebookBundleCallbackPoint::PreferredSize;
    const wxBookCtrlBase::Images staleImages{
        MakeCallbackNotebookBundle(first),
        MakeCallbackNotebookBundle(second)
    };
    const wxBitmapBundle newest =
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(23, 23), 32));

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    notebook.SetImages(staleImages);
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "page", true, 0));

    first->callback =
        [&notebook, newest]()
        {
            notebook.SetImages(wxBookCtrlBase::Images{newest});
        };
    second->callback = []() {};
    first->armed = true;
    second->armed = true;
    notebook.SetImages(staleImages);

    CHECK(first->calls == 1);
    CHECK(second->calls == 0);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) ==
          newest.GetPreferredBitmapSizeAtScale(
              notebook.GetDPIScaleFactor()));
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) ==
          wxSize(23, 23));
}

TEST_CASE("wxWinUI Notebook consensus tie uses the larger raster",
          "[winui-notebook][images][dpi][consensus][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxBitmapBundle smallRaster =
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(11, 11), 32));
    const wxBitmapBundle largeRaster =
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(17, 17), 32));
    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    notebook.SetImages(
        wxBookCtrlBase::Images{smallRaster, largeRaster});
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "tie", true, 0));

    // Consensus is over preferred raster pixels at the current scale: the
    // literal 17x17 value is correct only at 1x, while the selected small
    // bundle deliberately keeps its own 11x11 DIP size at every scale.
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) ==
          largeRaster.GetPreferredBitmapSizeAtScale(
              notebook.GetDPIScaleFactor()));
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) ==
          wxSize(11, 11));
}

TEST_CASE("wxWinUI Notebook invalid bundle sizes remove the icon",
          "[winui-notebook][images][invalid][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const bool invalidPreferred = GENERATE(false, true);
    CAPTURE(invalidPreferred);
    auto invalid = std::make_shared<ArmedNotebookBundleCallback>();
    invalid->invalidDefaultSize = !invalidPreferred;
    invalid->invalidPreferredSize = invalidPreferred;
    const wxBookCtrlBase::Images images{
        MakeCallbackNotebookBundle(invalid),
        wxBitmapBundle::FromBitmap(wxBitmap(wxSize(17, 17), 32))
    };

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(430, 180));
    notebook.SetImages(images);
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "invalid image", true, 0));

    CHECK(invalid->bitmapCalls == 0);
    CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) == wxSize());
    CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) == wxSize());
    CHECK(notebook.GetSelection() == 0);
}

TEST_CASE("wxWinUI Notebook persists tab metrics across Create and insertion",
          "[winui-notebook][metrics][default-create]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxSize initialPadding = parent->FromDIP(wxSize(8, 5));
    const wxSize initialSize = parent->FromDIP(wxSize(124, 44));
    wxNotebook notebook;
    notebook.SetPadding(initialPadding);
    notebook.SetTabSize(initialSize);
    REQUIRE(notebook.Create(
        parent, wxID_ANY, wxDefaultPosition, wxSize(470, 190),
        wxNB_TOP | wxNB_FIXEDWIDTH));
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "first", true));
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "second", false));

    for ( size_t page = 0; page < notebook.GetPageCount(); ++page )
    {
        CHECK(wxWinUINotebookTestAccess::GetPeerTabPadding(notebook, page) ==
              initialPadding);
        CHECK(wxWinUINotebookTestAccess::GetPeerTabSize(notebook, page) ==
              initialSize);
        const wxRect rect = notebook.GetTabRect(page);
        REQUIRE(!rect.IsEmpty());
        CHECK(std::abs(rect.width - initialSize.x) <= 1);
        CHECK(std::abs(rect.height - initialSize.y) <= 1);
    }

    const wxSize changedPadding = parent->FromDIP(wxSize(12, 7));
    const wxSize changedSize = parent->FromDIP(wxSize(142, 48));
    notebook.SetPadding(changedPadding);
    notebook.SetTabSize(changedSize);
    REQUIRE(notebook.AddPage(
        new wxPanel(&notebook), "third", false));

    for ( size_t page = 0; page < notebook.GetPageCount(); ++page )
    {
        CHECK(wxWinUINotebookTestAccess::GetPeerTabPadding(notebook, page) ==
              changedPadding);
        CHECK(wxWinUINotebookTestAccess::GetPeerTabSize(notebook, page) ==
              changedSize);
        const wxRect rect = notebook.GetTabRect(page);
        REQUIRE(!rect.IsEmpty());
        CHECK(std::abs(rect.width - changedSize.x) <= 1);
        CHECK(std::abs(rect.height - changedSize.y) <= 1);
    }

    // TabView is allowed to reset Width/Height during layout, but the
    // Min==Max constraints remain authoritative. Repeated size/layout/query
    // cycles must neither relax the peer nor let a const query mutate it.
    for ( int pass = 0; pass < 3; ++pass )
    {
        notebook.SetSize(wxSize(470 + pass * 17, 190 + pass * 9));
        for ( size_t page = 0; page < notebook.GetPageCount(); ++page )
        {
            CHECK(wxWinUINotebookTestAccess::GetPeerTabSize(notebook, page) ==
                  changedSize);
            const wxRect rect = notebook.GetTabRect(page);
            REQUIRE(!rect.IsEmpty());
            CHECK(std::abs(rect.width - changedSize.x) <= 1);
            CHECK(std::abs(rect.height - changedSize.y) <= 1);
        }
    }
}

TEST_CASE("wxWinUI Notebook header projection is last-writer-wins",
          "[winui-notebook][transaction][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("label and automation name converge to the nested model")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(360, 170));
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "initial", true));

        NotebookLabelReentry probe;
        probe.newest = "nested newest";
        wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::LabelText,
            &ReplaceNotebookLabel,
            &probe);

        REQUIRE(notebook.SetPageText(0, "stale outer"));
        CHECK(probe.calls == 1);
        CHECK(notebook.GetPageText(0) == probe.newest);
        CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 0) ==
              probe.newest);
        CHECK(wxWinUINotebookTestAccess::GetPeerAutomationName(notebook, 0) ==
              probe.newest);
    }

    SECTION("image source and cache converge to the nested generation")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(360, 170));
        wxBookCtrlBase::Images initial;
        initial.push_back(wxBitmapBundle::FromBitmap(
            wxBitmap(wxSize(16, 16), 32)));
        notebook.SetImages(initial);
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "image", true, 0));
        const std::uint64_t before =
            wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0);

        wxBookCtrlBase::Images outer;
        outer.push_back(wxBitmapBundle::FromBitmap(
            wxBitmap(wxSize(20, 20), 32)));
        NotebookImageReentry probe;
        probe.newest.push_back(wxBitmapBundle::FromBitmap(
            wxBitmap(wxSize(28, 28), 32)));
        wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::IconSource,
            &ReplaceNotebookImages,
            &probe);

        notebook.SetImages(outer);
        CHECK(probe.calls == 1);
        CHECK(wxWinUINotebookTestAccess::GetPeerIconGeneration(notebook, 0) >
              before);
        CHECK(wxWinUINotebookTestAccess::GetPeerIconPixelSize(notebook, 0) ==
              probe.newest[0].GetPreferredBitmapSizeAtScale(
                  notebook.GetDPIScaleFactor()));
        CHECK(wxWinUINotebookTestAccess::GetPeerIconDIPSize(notebook, 0) ==
              wxSize(28, 28));
    }

    SECTION("padding and fixed size converge as one projection")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(430, 190));
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "first", true));
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "second", false));
        notebook.SetPadding(parent->FromDIP(wxSize(3, 2)));
        notebook.SetTabSize(parent->FromDIP(wxSize(100, 38)));

        NotebookMetricsReentry probe;
        probe.padding = parent->FromDIP(wxSize(11, 7));
        probe.size = parent->FromDIP(wxSize(146, 50));
        wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::TabPadding,
            &ReplaceNotebookMetrics,
            &probe);

        notebook.SetPadding(parent->FromDIP(wxSize(5, 4)));
        CHECK(probe.calls == 1);
        for ( size_t page = 0; page < notebook.GetPageCount(); ++page )
        {
            CHECK(wxWinUINotebookTestAccess::GetPeerTabPadding(notebook, page) ==
                  probe.padding);
            CHECK(wxWinUINotebookTestAccess::GetPeerTabSize(notebook, page) ==
                  probe.size);
        }
    }

    SECTION("a storm consumes exactly one deferred wave")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(360, 170));
        REQUIRE(notebook.AddPage(
            new wxPanel(&notebook), "initial", true));

        NotebookProjectionStorm probe;
        // Eight synchronous passes plus one deferred wave can consume sixteen
        // mutations. The remaining eight prove that nested model revisions do
        // not silently rearm the CallAfter budget.
        probe.remaining = 24;
        wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::LabelText,
            &ContinueNotebookProjectionStorm,
            &probe);

        {
            wxLogNull suppressExpectedBoundedRetryWarning;
            REQUIRE(notebook.SetPageText(0, "seed"));
            wxWinUIDrainNotebookCallbacks();
        }

        CHECK(probe.remaining == 8);

        // Once the bounded transaction is quiescent, a genuinely external
        // model change starts a fresh budget and drains the rest of the storm.
        REQUIRE(notebook.SetPageText(0, "external-retry"));
        wxWinUIDrainNotebookCallbacks();
        CHECK(probe.remaining == 0);
        CHECK(notebook.GetPageText(0) == probe.newest);
        CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 0) ==
              probe.newest);
        CHECK(wxWinUINotebookTestAccess::GetPeerAutomationName(notebook, 0) ==
              probe.newest);
    }

    SECTION("destruction from a dependency-property callback is terminal")
    {
        // A preceding notebook may still own its deliberately quarantined
        // detached graph. Establish this section's baseline only after the
        // two dispatcher retirement turns have completed.
        wxWinUIDrainNotebookCallbacks();
        const size_t baseline =
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
        wxNotebook * const doomed = new wxNotebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(360, 170));
        REQUIRE(doomed->AddPage(
            new wxPanel(doomed), "initial", true));
        wxWeakRef<wxWindow> lifetime(doomed);

        NotebookDestroyProjection probe;
        wxWinUINotebookTestAccess::SetNextProjectionHook(*doomed, wxWinUINotebookTestAccess::ProjectionPoint::AutomationName,
            &DestroyNotebookDuringProjection,
            &probe);
        CHECK(doomed->SetPageText(0, "destroy"));

        CHECK(probe.calls == 1);
        CHECK(lifetime.get() == nullptr);
        CHECK(
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
            baseline + 1);
        wxWinUIDrainNotebookCallbacks();
        CHECK(
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
            baseline);
    }
}

TEST_CASE("wxWinUI Notebook page mutations are atomic",
          "[winui-notebook][rollback]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(260, 160));
    wxPanel * const first = new wxPanel(&notebook);
    wxPanel * const second = new wxPanel(&notebook);
    REQUIRE(notebook.AddPage(first, "first", true, wxNOT_FOUND));
    REQUIRE(notebook.AddPage(second, "second", false, wxNOT_FOUND));
    REQUIRE(notebook.GetSelection() == 0);
    REQUIRE(notebook.GetPageCount() == 2);
    REQUIRE(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 2);

    wxPanel * const rejected = new wxPanel(&notebook);
    wxWinUINotebookTestAccess::FailNextPeerMutation(notebook, wxWinUINotebookTestAccess::PeerMutation::InsertPage);
    CHECK_FALSE(notebook.InsertPage(
        1, rejected, "rejected", false, wxNOT_FOUND));
    CHECK(notebook.GetPageCount() == 2);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 2);
    CHECK(notebook.GetPage(0) == first);
    CHECK(notebook.GetPage(1) == second);
    CHECK(notebook.GetPageText(0) == "first");
    CHECK(notebook.GetPageText(1) == "second");
    CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 0) == "first");
    CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 1) == "second");
    CHECK(notebook.GetSelection() == 0);
    delete rejected;

    wxWinUINotebookTestAccess::FailNextPeerMutation(notebook, wxWinUINotebookTestAccess::PeerMutation::RemovePage);
    CHECK_FALSE(notebook.RemovePage(0));
    CHECK(notebook.GetPageCount() == 2);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 2);
    CHECK(notebook.GetPage(0) == first);
    CHECK(notebook.GetPage(1) == second);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 0) == "first");
    CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 1) == "second");
    CHECK(notebook.GetSelection() == 0);

    wxWinUINotebookTestAccess::FailNextPeerMutation(notebook, wxWinUINotebookTestAccess::PeerMutation::ClearPages);
    CHECK_FALSE(notebook.DeleteAllPages());
    CHECK(notebook.GetPageCount() == 2);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 2);
    CHECK(notebook.GetPage(0) == first);
    CHECK(notebook.GetPage(1) == second);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 0) == "first");
    CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 1) == "second");
    CHECK(notebook.GetSelection() == 0);

    // Each injected failure is one-shot: the same operations must remain
    // usable after the rollback.
    wxPanel * const inserted = new wxPanel(&notebook);
    REQUIRE(notebook.InsertPage(
        0, inserted, "inserted", false, wxNOT_FOUND));
    CHECK(notebook.GetPageCount() == 3);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 3);
    CHECK(notebook.GetSelection() == 1);
    CHECK(notebook.GetCurrentPage() == first);
    REQUIRE(notebook.RemovePage(0));
    CHECK(notebook.GetPageCount() == 2);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 2);
    CHECK(notebook.GetSelection() == 0);
    CHECK(notebook.GetCurrentPage() == first);
    delete inserted;

    REQUIRE(notebook.DeleteAllPages());
    CHECK(notebook.GetPageCount() == 0);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 0);
    CHECK(notebook.GetSelection() == wxNOT_FOUND);
}

TEST_CASE("wxWinUI Notebook insertion removes a page destroyed by projection",
          "[winui-notebook][rollback][lifetime][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxNotebook notebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(260, 160));
    wxPanel * const existing = new wxPanel(&notebook);
    REQUIRE(notebook.AddPage(existing, "existing", true));

    wxPanel * const candidate = new wxPanel(&notebook);
    const wxWeakRef<wxWindow> candidateLifetime(candidate);
    NotebookDestroyInsertedPageProjection probe;
    probe.page = candidate;
    wxWinUINotebookTestAccess::SetNextProjectionHook(notebook, wxWinUINotebookTestAccess::ProjectionPoint::LabelText,
        &DestroyInsertedPageDuringProjection,
        &probe);

    // Publication briefly owns the candidate, so its synchronous destruction
    // is a successful ownership-consuming outcome. The exact pre-insertion
    // model and all peer mirrors must nevertheless be restored.
    CHECK(notebook.AddPage(candidate, "doomed", false));
    CHECK(probe.calls == 1);
    CHECK_FALSE(candidateLifetime);
    REQUIRE(notebook.GetPageCount() == 1);
    CHECK(notebook.GetPage(0) == existing);
    CHECK(notebook.GetPageText(0) == "existing");
    CHECK(notebook.GetSelection() == 0);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(notebook) == 1);
    CHECK(wxWinUINotebookTestAccess::GetPeerPageText(notebook, 0) == "existing");
}

TEST_CASE("wxWinUI Notebook callbacks do not outlive their owner",
          "[winui-notebook][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    // ForceRender() can leave a revoked LayoutUpdated delegate queued from a
    // notebook destroyed by the preceding test. Drain it before establishing
    // this test's own lifetime baseline.
    wxWinUIDrainNotebookCallbacks();
    const size_t baseline =
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
    int changed = 0;

    {
        std::unique_ptr<wxNotebook> notebook(
            new wxNotebook(
                parent, wxID_ANY, wxDefaultPosition, wxSize(240, 150)));
        REQUIRE(notebook->AddPage(
            new wxPanel(notebook.get()), "first", true));
        REQUIRE(notebook->AddPage(
            new wxPanel(notebook.get()), "second", false));
        notebook->Bind(
            wxEVT_NOTEBOOK_PAGE_CHANGED,
            [&changed](wxBookCtrlEvent&)
            {
                ++changed;
            });

        REQUIRE(
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
            baseline + 1);
        REQUIRE(wxWinUINotebookTestAccess::QueueSelectionCallback(*notebook, 1));
        REQUIRE(!notebook->GetTabRect(0).IsEmpty());

        // Close is deliberately invoked twice before the destructor. The
        // queued callback still owns the state, but its generation is stale.
        wxWinUINotebookTestAccess::ClosePeer(*notebook);
        wxWinUINotebookTestAccess::ClosePeer(*notebook);
        CHECK(wxWinUINotebookTestAccess::GetPeerPageCount(*notebook) == 0);
        CHECK(notebook->GetTabRect(0).IsEmpty());
        long flags = 0;
        CHECK(notebook->HitTest(wxPoint(1, 1), &flags) == wxNOT_FOUND);
        CHECK(flags == wxBK_HITTEST_NOWHERE);
        notebook.reset();
        CHECK(
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
            baseline + 1);
    }

    wxWinUIDrainNotebookCallbacks();
    CHECK(changed == 0);
    CHECK(
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
        baseline);

    // A wx event handler is also allowed to destroy the control synchronously.
    // Selection must return without touching the retired owner.
    wxNotebook *doomed = new wxNotebook(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 150));
    REQUIRE(doomed->AddPage(new wxPanel(doomed), "first", true));
    REQUIRE(doomed->AddPage(new wxPanel(doomed), "second", false));
    wxWeakRef<wxWindow> lifetime(doomed);
    doomed->Bind(
        wxEVT_NOTEBOOK_PAGE_CHANGING,
        [doomed](wxBookCtrlEvent&)
        {
            doomed->Destroy();
        });

    CHECK(doomed->SetSelection(1) == 0);
    CHECK(lifetime.get() == nullptr);
    CHECK(
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
        baseline + 1);
    wxWinUIDrainNotebookCallbacks();
    CHECK(
        wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
        baseline);
}

TEST_CASE("wxWinUI Notebook selection revalidates virtual boundaries",
          "[winui-notebook][selection][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const auto exerciseSelectionBoundary =
        [parent](NotebookVirtualBoundary::Boundary boundary,
                 size_t page = wxNOT_FOUND)
        {
            const size_t baseline =
                wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
            NotebookVirtualBoundary * const notebook =
                new NotebookVirtualBoundary(
                    parent,
                    wxID_ANY,
                    wxDefaultPosition,
                    wxSize(240, 150));
            REQUIRE(notebook->AddPage(
                new wxPanel(notebook), "first", true));
            REQUIRE(notebook->AddPage(
                new wxPanel(notebook), "second", false));
            wxWeakRef<wxWindow> lifetime(notebook);
            notebook->Arm(boundary, page);

            CHECK(notebook->SetSelection(1) == 0);
            CHECK(lifetime.get() == nullptr);

            wxWinUIDrainNotebookCallbacks();
            CHECK(
                wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
                baseline);
        };

    SECTION("CreatePageChangingEvent")
    {
        exerciseSelectionBoundary(
            NotebookVirtualBoundary::Boundary::CreateChangingEvent);
    }

    SECTION("old TryGetNonNullPage")
    {
        exerciseSelectionBoundary(
            NotebookVirtualBoundary::Boundary::TryGetPage, 0);
    }

    SECTION("new TryGetNonNullPage")
    {
        exerciseSelectionBoundary(
            NotebookVirtualBoundary::Boundary::TryGetPage, 1);
    }

    SECTION("selection GetPageRect")
    {
        exerciseSelectionBoundary(
            NotebookVirtualBoundary::Boundary::GetPageRect);
    }

    SECTION("MakeChangedEvent")
    {
        exerciseSelectionBoundary(
            NotebookVirtualBoundary::Boundary::MakeChangedEvent);
    }

    SECTION("insertion GetPageRect")
    {
        const size_t baseline =
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount();
        NotebookVirtualBoundary * const notebook =
            new NotebookVirtualBoundary(
                parent,
                wxID_ANY,
                wxDefaultPosition,
                wxSize(240, 150));
        REQUIRE(notebook->AddPage(
            new wxPanel(notebook), "first", true));
        wxWeakRef<wxWindow> lifetime(notebook);
        notebook->Arm(
            NotebookVirtualBoundary::Boundary::GetPageRect);

        CHECK(notebook->AddPage(
            new wxPanel(notebook), "second", false));
        CHECK(lifetime.get() == nullptr);

        wxWinUIDrainNotebookCallbacks();
        CHECK(
            wxWinUINotebookTestAccess::GetLiveCallbackStateCount() ==
            baseline);
    }
}

TEST_CASE("wxWinUI Notebook reentrant selection preserves focus ownership",
          "[winui-notebook][focus]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    const wxWeakRef<wxWindow> originalFocus(wxWindow::FindFocus());

    SECTION("newest internal selection wins")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(260, 160));
        wxPanel * const first = new wxPanel(&notebook);
        wxPanel * const second = new wxPanel(&notebook);
        wxPanel * const third = new wxPanel(&notebook);
        wxWindow * const firstChild =
            new wxWindow(first, wxID_ANY, wxPoint(4, 4), wxSize(20, 20));
        REQUIRE(notebook.AddPage(first, "first", true));
        REQUIRE(notebook.AddPage(second, "second", false));
        REQUIRE(notebook.AddPage(third, "third", false));

        int changing = 0;
        int changed = 0;
        notebook.Bind(
            wxEVT_NOTEBOOK_PAGE_CHANGING,
            [&notebook, &changing](wxBookCtrlEvent& event)
            {
                if ( event.GetSelection() == 1 )
                {
                    ++changing;
                    notebook.ChangeSelection(2);
                }
            });
        notebook.Bind(
            wxEVT_NOTEBOOK_PAGE_CHANGED,
            [&changed](wxBookCtrlEvent&)
            {
                ++changed;
            });

        firstChild->SetFocus();
        REQUIRE(wxWindow::FindFocus() == firstChild);
        CHECK(notebook.SetSelection(1) == 0);
        CHECK(notebook.GetSelection() == 2);
        CHECK(changing == 1);
        CHECK(changed == 0);
        CHECK(wxWinUIFocusIsWithin(third));
    }

    SECTION("focus on the extended tab surface stays on the notebook")
    {
        wxNotebook notebook(
            parent, wxID_ANY, wxDefaultPosition, wxSize(260, 160),
            wxNB_BOTTOM | wxNB_MULTILINE);
        wxPanel * const first = new wxPanel(&notebook);
        wxPanel * const second = new wxPanel(&notebook);
        REQUIRE(notebook.AddPage(first, "first", true));
        REQUIRE(notebook.AddPage(second, "second", false));

        notebook.SetFocus();
        REQUIRE(wxWindow::FindFocus() == &notebook);
        CHECK(notebook.SetSelection(1) == 0);
        CHECK(notebook.GetSelection() == 1);
        CHECK(wxWindow::FindFocus() == &notebook);
        CHECK_FALSE(wxWinUIFocusIsWithin(second));
    }

    SECTION("explicit external focus is not stolen")
    {
        wxWindow external(
            parent, wxID_ANY, wxPoint(4, 4), wxSize(20, 20));
        wxNotebook notebook(
            parent, wxID_ANY, wxPoint(30, 4), wxSize(260, 160));
        wxPanel * const first = new wxPanel(&notebook);
        wxPanel * const second = new wxPanel(&notebook);
        wxPanel * const third = new wxPanel(&notebook);
        wxWindow * const firstChild =
            new wxWindow(first, wxID_ANY, wxPoint(4, 4), wxSize(20, 20));
        REQUIRE(notebook.AddPage(first, "first", true));
        REQUIRE(notebook.AddPage(second, "second", false));
        REQUIRE(notebook.AddPage(third, "third", false));

        notebook.Bind(
            wxEVT_NOTEBOOK_PAGE_CHANGING,
            [&notebook, &external](wxBookCtrlEvent& event)
            {
                if ( event.GetSelection() == 1 )
                {
                    external.SetFocus();
                    notebook.ChangeSelection(2);
                }
            });

        firstChild->SetFocus();
        REQUIRE(wxWindow::FindFocus() == firstChild);
        CHECK(notebook.SetSelection(1) == 0);
        CHECK(notebook.GetSelection() == 2);
        CHECK(wxWindow::FindFocus() == &external);
    }

    if ( wxWindow * const focus = originalFocus.get() )
        focus->SetFocus();
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_NOTEBOOK
