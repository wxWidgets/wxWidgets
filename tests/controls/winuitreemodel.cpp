///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/winuitreemodel.cpp
// Purpose:     deterministic WinUI TreeView model/lifetime/geometry tests
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_TREECTRL

#include "wx/app.h"
#include "wx/bmpbndl.h"
#include "wx/dcmemory.h"
#include "wx/frame.h"
#include "wx/imaglist.h"
#include "wx/log.h"
#include "wx/textctrl.h"
#include "wx/treectrl.h"
#include "wx/utils.h"
#include "wx/weakref.h"
#include "wx/winui/private/tlwhost.h"

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
    #include "wx/winui/private/dropbroker.h"
#endif

#include <array>
#include <chrono>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace
{

void DrainTreeDispatch(int rounds = 4)
{
    for ( int i = 0; i < rounds; ++i )
        wxYield();
}

wxPoint RectCentre(const wxRect& rect)
{
    return wxPoint(
        rect.x + wxMax(0, rect.width / 2),
        rect.y + wxMax(0, rect.height / 2));
}

wxBitmap MakeTreeBitmap(const wxColour& colour)
{
    wxBitmap bitmap(16, 16);
    wxMemoryDC dc(bitmap);
    dc.SetBackground(wxBrush(colour));
    dc.Clear();
    dc.SelectObject(wxNullBitmap);
    return bitmap;
}

wxBitmapBundle MakeTreeDPIBundle(const wxSize& logicalSize,
                                 const wxColour& colour)
{
    wxBitmap one;
    wxBitmap two;
    REQUIRE(one.CreateWithDIPSize(
        logicalSize, 1.0, 32));
    REQUIRE(two.CreateWithDIPSize(
        logicalSize, 2.0, 32));

    for ( wxBitmap * const bitmap : {&one, &two} )
    {
        wxMemoryDC dc(*bitmap);
        dc.SetBackground(wxBrush(colour));
        dc.Clear();
        dc.SelectObject(wxNullBitmap);
    }

    return wxBitmapBundle::FromBitmaps(one, two);
}

class CountedTreeData final : public wxTreeItemData
{
public:
    explicit CountedTreeData(int *destroyed)
        : m_destroyed(destroyed)
    {
    }

    ~CountedTreeData() override
    {
        ++*m_destroyed;
    }

private:
    int *m_destroyed;
};

struct DeepTreeChain
{
    std::vector<wxTreeItemId> items;
    std::vector<std::uintptr_t> peerIdentities;
};

DeepTreeChain AppendDeepTreeChain(wxTreeCtrl& tree,
                                  const wxTreeItemId& parent,
                                  int count,
                                  const wxString& labelPrefix,
                                  int *destroyed = nullptr)
{
    REQUIRE(parent.IsOk());
    REQUIRE(count > 0);

    DeepTreeChain chain;
    chain.items.reserve(count);
    chain.peerIdentities.reserve(count);

    wxTreeItemId currentParent = parent;
    for ( int i = 0; i < count; ++i )
    {
        std::unique_ptr<CountedTreeData> data;
        if ( destroyed )
            data = std::make_unique<CountedTreeData>(destroyed);

        const wxTreeItemId item = tree.AppendItem(
            currentParent,
            labelPrefix + wxString::Format(" %d", i),
            -1,
            -1,
            data.get());
        REQUIRE(item.IsOk());
        if ( data )
            data.release();

        const std::uintptr_t peerIdentity =
            tree.WinUIGetItemPeerIdentityForTesting(item);
        REQUIRE(peerIdentity != 0);
        chain.items.push_back(item);
        chain.peerIdentities.push_back(peerIdentity);
        currentParent = item;
    }

    return chain;
}

void CheckDeepTreeChainIntegrity(wxTreeCtrl& tree,
                                 const wxTreeItemId& parent,
                                 const DeepTreeChain& chain)
{
    REQUIRE(parent.IsOk());
    REQUIRE(chain.items.size() == chain.peerIdentities.size());
    REQUIRE_FALSE(chain.items.empty());

    wxTreeItemId expectedParent = parent;
    for ( size_t i = 0; i < chain.items.size(); ++i )
    {
        CAPTURE(i);
        const wxTreeItemId item = chain.items[i];

        CHECK(tree.GetItemParent(item) == expectedParent);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(item) ==
              chain.peerIdentities[i]);
        // This seam also checks the exact sibling index in the peer
        // collection selected from the model parent. Walking it once per
        // node therefore validates the entire deep peer topology in O(n).
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(item));

        expectedParent = item;
    }

    const size_t middle = chain.items.size() / 2;
    CHECK(tree.GetChildrenCount(chain.items.front(), false) ==
          (chain.items.size() == 1 ? 0u : 1u));
    CHECK(tree.WinUIGetPeerChildCountForTesting(
              chain.items.front()) ==
          (chain.items.size() == 1 ? 0u : 1u));
    CHECK(tree.GetChildrenCount(chain.items[middle], false) ==
          (middle + 1 < chain.items.size() ? 1u : 0u));
    CHECK(tree.WinUIGetPeerChildCountForTesting(
              chain.items[middle]) ==
          (middle + 1 < chain.items.size() ? 1u : 0u));
    CHECK(tree.GetChildrenCount(chain.items.back(), false) == 0);
    CHECK(tree.WinUIGetPeerChildCountForTesting(
              chain.items.back()) == 0);
}

enum class TreeBundleCallbackPoint
{
    DefaultSize,
    PreferredSize,
    Bitmap
};

class CallbackTreeBitmapBundleImpl final : public wxBitmapBundleImpl
{
public:
    CallbackTreeBitmapBundleImpl(
        const wxBitmap& bitmap,
        TreeBundleCallbackPoint point,
        std::function<void ()> callback,
        unsigned *bitmapCalls)
        : m_bitmap(bitmap),
          m_point(point),
          m_callback(std::move(callback)),
          m_bitmapCalls(bitmapCalls)
    {
    }

    ~CallbackTreeBitmapBundleImpl() override = default;

    wxSize GetDefaultSize() const override
    {
        Invoke(TreeBundleCallbackPoint::DefaultSize);
        return m_bitmap.GetSize();
    }

    wxSize GetPreferredBitmapSizeAtScale(double) const override
    {
        Invoke(TreeBundleCallbackPoint::PreferredSize);
        return m_bitmap.GetSize();
    }

    wxBitmap GetBitmap(const wxSize&) override
    {
        if ( m_bitmapCalls )
            ++*m_bitmapCalls;
        Invoke(TreeBundleCallbackPoint::Bitmap);
        return m_bitmap;
    }

private:
    void Invoke(TreeBundleCallbackPoint point) const
    {
        if ( point != m_point || !m_callback )
            return;

        std::function<void ()> callback =
            std::move(m_callback);
        callback();
    }

    wxBitmap m_bitmap;
    TreeBundleCallbackPoint m_point;
    mutable std::function<void ()> m_callback;
    unsigned *m_bitmapCalls;
};

wxBitmapBundle MakeCallbackTreeBundle(
    TreeBundleCallbackPoint point,
    std::function<void ()> callback,
    unsigned *bitmapCalls = nullptr)
{
    return wxBitmapBundle::FromImpl(
        new CallbackTreeBitmapBundleImpl(
            MakeTreeBitmap(*wxBLUE), point, std::move(callback),
            bitmapCalls));
}

class InvalidTreeBitmapBundleImpl final : public wxBitmapBundleImpl
{
public:
    InvalidTreeBitmapBundleImpl(TreeBundleCallbackPoint invalidPoint,
                                unsigned *bitmapCalls)
        : m_bitmap(MakeTreeBitmap(*wxBLUE)),
          m_invalidPoint(invalidPoint),
          m_bitmapCalls(bitmapCalls)
    {
    }

    wxSize GetDefaultSize() const override
    {
        return m_invalidPoint == TreeBundleCallbackPoint::DefaultSize
            ? wxSize()
            : m_bitmap.GetSize();
    }

    wxSize GetPreferredBitmapSizeAtScale(double) const override
    {
        return m_invalidPoint == TreeBundleCallbackPoint::PreferredSize
            ? wxSize()
            : m_bitmap.GetSize();
    }

    wxBitmap GetBitmap(const wxSize&) override
    {
        if ( m_bitmapCalls )
            ++*m_bitmapCalls;
        return m_bitmap;
    }

private:
    wxBitmap m_bitmap;
    TreeBundleCallbackPoint m_invalidPoint;
    unsigned *m_bitmapCalls = nullptr;
};

wxBitmapBundle MakeInvalidTreeBundle(
    TreeBundleCallbackPoint invalidPoint,
    unsigned *bitmapCalls)
{
    return wxBitmapBundle::FromImpl(
        new InvalidTreeBitmapBundleImpl(
            invalidPoint, bitmapCalls));
}

struct ArmedTreeBundleCallback
{
    bool armed = false;
    std::function<void ()> callback;
};

class ArmedTreeBitmapBundleImpl final : public wxBitmapBundleImpl
{
public:
    explicit ArmedTreeBitmapBundleImpl(
        const std::shared_ptr<ArmedTreeBundleCallback>& callback)
        : m_bitmap(MakeTreeBitmap(*wxBLUE)),
          m_callback(callback)
    {
    }

    wxSize GetDefaultSize() const override
    {
        return m_bitmap.GetSize();
    }

    wxSize GetPreferredBitmapSizeAtScale(double) const override
    {
        Invoke();
        return m_bitmap.GetSize();
    }

    wxBitmap GetBitmap(const wxSize&) override
    {
        Invoke();
        return m_bitmap;
    }

private:
    void Invoke() const
    {
        if ( !m_callback || !m_callback->armed ||
             !m_callback->callback )
        {
            return;
        }

        m_callback->armed = false;
        m_callback->callback();
    }

    wxBitmap m_bitmap;
    std::shared_ptr<ArmedTreeBundleCallback> m_callback;
};

class CallbackTreeImageList final : public wxImageList
{
public:
    enum class Point
    {
        ImageCount,
        Bitmap
    };

    CallbackTreeImageList(
        Point point,
        std::function<void ()> callback)
        : wxImageList(16, 16),
          m_point(point),
          m_callback(std::move(callback))
    {
        Add(MakeTreeBitmap(*wxBLUE));
    }

    int GetImageCount() const override
    {
        Invoke(Point::ImageCount);
        return wxImageList::GetImageCount();
    }

    wxBitmap GetBitmap(int index) const override
    {
        Invoke(Point::Bitmap);
        return wxImageList::GetBitmap(index);
    }

private:
    void Invoke(Point point) const
    {
        if ( point != m_point || !m_callback )
            return;

        std::function<void ()> callback = std::move(m_callback);
        callback();
    }

    Point m_point;
    mutable std::function<void ()> m_callback;
};

class CallbackDPITreeCtrl final : public wxTreeCtrl
{
public:
    CallbackDPITreeCtrl(wxWindow *parent, const wxSize& size)
        : wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, size)
    {
    }

    void SetDPICallback(std::function<void ()> callback)
    {
        m_callback = std::move(callback);
    }

    double GetDPIScaleFactor() const override
    {
        if ( m_callback )
        {
            std::function<void ()> callback = std::move(m_callback);
            callback();
        }
        return 1.0;
    }

private:
    mutable std::function<void ()> m_callback;
};

class CallbackLayoutTreeCtrl final : public wxTreeCtrl
{
public:
    void SetLayoutCallback(std::function<void ()> callback)
    {
        m_callback = std::move(callback);
    }

    wxLayoutDirection GetLayoutDirection() const override
    {
        if ( m_callback )
        {
            std::function<void ()> callback =
                std::move(m_callback);
            callback();
        }
        return wxLayout_LeftToRight;
    }

private:
    mutable std::function<void ()> m_callback;
};

class SortingTreeCtrl final : public wxTreeCtrl
{
public:
    using wxTreeCtrl::wxTreeCtrl;

protected:
    int OnCompareItems(const wxTreeItemId& a,
                       const wxTreeItemId& b) override
    {
        return GetItemText(a).Cmp(GetItemText(b));
    }
};

struct TreeProjectionStorm
{
    wxTreeCtrl *tree = nullptr;
    wxTreeItemId item;
    int remaining = 0;
    int mutation = 0;
    wxString newest;
};

void ContinueTreeProjectionStorm(TreeProjectionStorm *probe)
{
    if ( !probe || probe->remaining <= 0 )
        return;

    --probe->remaining;
    ++probe->mutation;
    probe->newest =
        wxString::Format("newest-%d", probe->mutation);
    probe->tree->SetItemText(probe->item, probe->newest);
    if ( probe->remaining > 0 )
    {
        wxWithImages::Images nextImages;
        nextImages.push_back(MakeCallbackTreeBundle(
            TreeBundleCallbackPoint::PreferredSize,
            [probe]()
            {
                ContinueTreeProjectionStorm(probe);
            }));
        probe->tree->SetImages(nextImages);
    }
}

void FlushTreeSlots(wxWindow *parent)
{
    if ( wxWinUITopLevelHost * const host =
             wxWinUITopLevelHost::ForWindow(parent, false) )
    {
        host->FlushSync();
    }
}

#if wxUSE_DRAG_AND_DROP
class CountingTextDropTarget final : public wxTextDropTarget
{
public:
    explicit CountingTextDropTarget(int *drops)
        : m_drops(drops)
    {
    }

    bool OnDropText(wxCoord, wxCoord, const wxString&) override
    {
        ++*m_drops;
        return true;
    }

private:
    int *m_drops;
};
#endif

} // anonymous namespace

TEST_CASE("wxWinUI TreeCtrl model transactions are atomic",
          "[treectrl][winui-treemodel][rollback]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree;
    REQUIRE(tree.Create(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180)));
    const wxTreeItemId root = tree.AddRoot("root");
    const wxTreeItemId first = tree.AppendItem(root, "first");
    const wxTreeItemId second = tree.AppendItem(root, "second");
    REQUIRE(root.IsOk());
    REQUIRE(first.IsOk());
    REQUIRE(second.IsOk());

    const std::uintptr_t rootPeer =
        tree.WinUIGetItemPeerIdentityForTesting(root);
    const std::uintptr_t firstPeer =
        tree.WinUIGetItemPeerIdentityForTesting(first);
    const std::uintptr_t secondPeer =
        tree.WinUIGetItemPeerIdentityForTesting(second);
    REQUIRE(rootPeer != 0);
    REQUIRE(firstPeer != 0);
    REQUIRE(secondPeer != 0);
    REQUIRE(tree.WinUIIsItemAttachedToPeerForTesting(root));
    REQUIRE(tree.WinUIIsItemAttachedToPeerForTesting(first));
    REQUIRE(tree.WinUIIsItemAttachedToPeerForTesting(second));
    REQUIRE(tree.WinUIGetPeerChildCountForTesting() == 1);
    REQUIRE(tree.WinUIGetPeerChildCountForTesting(root) == 2);

    int rejectedDataDestroyed = 0;
    CountedTreeData * const rejectedData =
        new CountedTreeData(&rejectedDataDestroyed);
    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::InsertItem);
    const wxTreeItemId rejected =
        tree.InsertItem(root, size_t{1}, "rejected", -1, -1, rejectedData);
    CHECK_FALSE(rejected.IsOk());
    CHECK(rejectedDataDestroyed == 0);
    CHECK_FALSE(rejectedData->GetId().IsOk());
    CHECK(tree.GetCount() == 3);
    wxTreeItemIdValue cookie = nullptr;
    CHECK(tree.GetFirstChild(root, cookie) == first);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(first) == firstPeer);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(second) == secondPeer);
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(first));
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(second));
    CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 2);
    delete rejectedData;
    CHECK(rejectedDataDestroyed == 1);

    int postCommitDataDestroyed = 0;
    CountedTreeData * const postCommitData =
        new CountedTreeData(&postCommitDataDestroyed);
    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::
            InsertRollbackAfterCommit);
    const wxTreeItemId postCommitRejected =
        tree.InsertItem(
            root, size_t{1}, "post-commit rejected",
            -1, -1, postCommitData);
    CHECK_FALSE(postCommitRejected.IsOk());
    CHECK(tree.GetCount() == 3);
    CHECK(postCommitDataDestroyed == 0);
    CHECK_FALSE(postCommitData->GetId().IsOk());
    CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 2);
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(first));
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(second));
    delete postCommitData;
    CHECK(postCommitDataDestroyed == 1);

    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::RemoveItem);
    tree.Delete(first);
    CHECK(tree.GetCount() == 3);
    CHECK(tree.GetItemText(first) == "first");
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(first) == firstPeer);
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(first));
    CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 2);

    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::RemoveItemAfterCommit);
    tree.Delete(first);
    CHECK(tree.GetCount() == 3);
    CHECK(tree.GetItemText(first) == "first");
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(first) == firstPeer);
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(first));
    CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 2);

    tree.Expand(root);
    REQUIRE(tree.IsExpanded(root));
    tree.SelectItem(second);
    REQUIRE(tree.GetSelection() == second);
    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::ClearItems);
    tree.DeleteChildren(root);
    CHECK(tree.GetCount() == 3);
    CHECK(tree.GetChildrenCount(root, false) == 2);
    CHECK(tree.IsExpanded(root));
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(first) == firstPeer);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(second) == secondPeer);
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(first));
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(second));
    CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 2);
    CHECK(tree.GetSelection() == second);

    // Failure injection is one-shot and peer identity survives every rollback.
    tree.Delete(first);
    CHECK(tree.GetCount() == 2);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(first) == 0);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(second) == secondPeer);
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(second));
    CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 1);

    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::ClearItems);
    tree.DeleteAllItems();
    CHECK(tree.GetCount() == 2);
    CHECK(tree.GetRootItem() == root);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) == rootPeer);
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(root));
    CHECK(tree.WinUIIsItemAttachedToPeerForTesting(second));
    CHECK(tree.WinUIGetPeerChildCountForTesting() == 1);
    CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 1);
    CHECK(tree.GetSelection() == second);

    const wxTreeItemId replacement =
        tree.AppendItem(root, "replacement");
    REQUIRE(replacement.IsOk());
    CHECK(replacement != first);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(first) == 0);

    tree.DeleteAllItems();
    CHECK(tree.GetCount() == 0);
    CHECK_FALSE(tree.GetRootItem().IsOk());

    const wxTreeItemId dataRoot = tree.AddRoot("data root");
    const wxTreeItemId dataItem = tree.AppendItem(dataRoot, "data item");
    int detachedDataDestroyed = 0;
    int attachedDataDestroyed = 0;
    CountedTreeData * const detachedData =
        new CountedTreeData(&detachedDataDestroyed);
    CountedTreeData * const attachedData =
        new CountedTreeData(&attachedDataDestroyed);
    tree.SetItemData(dataItem, detachedData);
    tree.SetItemData(dataItem, attachedData);
    CHECK(tree.GetItemData(dataItem) == attachedData);
    CHECK(detachedDataDestroyed == 0);
    CHECK(attachedDataDestroyed == 0);
    // SetItemData() follows generic/MSW ownership: replacement detaches the
    // old pointer without destroying it, while deleting the item destroys the
    // value still attached to the model.
    delete detachedData;
    CHECK(detachedDataDestroyed == 1);
    tree.Delete(dataItem);
    CHECK(attachedDataDestroyed == 1);

    wxTreeCtrl rootRollback;
    REQUIRE(rootRollback.Create(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 140)));
    int rootDataDestroyed = 0;
    CountedTreeData * const rootData =
        new CountedTreeData(&rootDataDestroyed);
    rootRollback.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::
            InsertRollbackAfterCommit);
    const wxTreeItemId rejectedRoot =
        rootRollback.AddRoot("rejected root", -1, -1, rootData);
    CHECK_FALSE(rejectedRoot.IsOk());
    CHECK(rootRollback.GetCount() == 0);
    CHECK(rootRollback.WinUIGetPeerChildCountForTesting() == 0);
    CHECK(rootDataDestroyed == 0);
    CHECK_FALSE(rootData->GetId().IsOk());
    delete rootData;
    const wxTreeItemId acceptedRoot =
        rootRollback.AddRoot("accepted root");
    REQUIRE(acceptedRoot.IsOk());
    CHECK(rootRollback.WinUIGetItemPeerIdentityForTesting(
              acceptedRoot) != 0);
    CHECK(rootRollback.WinUIIsItemAttachedToPeerForTesting(
        acceptedRoot));
    CHECK(rootRollback.WinUIGetPeerChildCountForTesting() == 1);
}

TEST_CASE("wxWinUI TreeCtrl selection expansion and key contracts",
          "[treectrl][winui-treemodel][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
    const wxTreeItemId root = tree.AddRoot("root");
    const wxTreeItemId first = tree.AppendItem(root, "first");
    const wxTreeItemId second = tree.AppendItem(root, "second");
    const wxTreeItemId third = tree.AppendItem(root, "third");

    int changing = 0;
    int changed = 0;
    bool vetoSecond = true;
    bool reenterToThird = false;
    tree.Bind(
        wxEVT_TREE_SEL_CHANGING,
        [&](wxTreeEvent& event)
        {
            ++changing;
            if ( vetoSecond && event.GetItem() == second )
                event.Veto();
            if ( reenterToThird && event.GetItem() == second )
            {
                reenterToThird = false;
                tree.SelectItem(third);
            }
        });
    tree.Bind(
        wxEVT_TREE_SEL_CHANGED,
        [&](wxTreeEvent&)
        {
            ++changed;
        });

    REQUIRE(tree.WinUISelectPeerItemForTesting(first));
    CHECK(tree.GetSelection() == first);
    CHECK(changing == 1);
    CHECK(changed == 1);

    CHECK(tree.WinUISelectPeerItemForTesting(second));
    DrainTreeDispatch();
    CHECK(tree.GetSelection() == first);
    CHECK(changing == 2);
    CHECK(changed == 1);

    vetoSecond = false;
    reenterToThird = true;
    REQUIRE(tree.WinUISelectPeerItemForTesting(second));
    DrainTreeDispatch();
    CHECK(tree.GetSelection() == third);
    CHECK(tree.IsSelected(third));
    CHECK_FALSE(tree.IsSelected(second));

    // Selecting a descendant expands its ancestors, matching the generic
    // wxTreeCtrl implementation. Start the expansion-event scenario from an
    // explicit collapsed state instead of depending on the earlier selection
    // sequence to leave the root collapsed.
    tree.Collapse(root);
    REQUIRE_FALSE(tree.IsExpanded(root));

    int expanding = 0;
    int expanded = 0;
    bool vetoExpand = true;
    tree.Bind(
        wxEVT_TREE_ITEM_EXPANDING,
        [&](wxTreeEvent& event)
        {
            ++expanding;
            if ( vetoExpand )
                event.Veto();
        });
    tree.Bind(
        wxEVT_TREE_ITEM_EXPANDED,
        [&](wxTreeEvent&)
        {
            ++expanded;
        });

    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::
            SetExpandedAfterCommit);
    CHECK_FALSE(tree.WinUISetPeerExpandedForTesting(root, true));
    CHECK_FALSE(tree.IsExpanded(root));
    CHECK_FALSE(tree.WinUIIsPeerExpandedForTesting(root));
    CHECK(expanding == 1);
    CHECK(expanded == 0);

    vetoExpand = false;
    REQUIRE(tree.WinUISetPeerExpandedForTesting(root, true));
    CHECK(tree.IsExpanded(root));
    CHECK(tree.WinUIIsPeerExpandedForTesting(root));
    CHECK(expanding == 2);
    CHECK(expanded == 1);

    tree.Collapse(root);
    REQUIRE_FALSE(tree.IsExpanded(root));
    REQUIRE_FALSE(tree.WinUIIsPeerExpandedForTesting(root));
    tree.WinUIFailNextPeerMutationForTesting(
        wxTreeCtrl::WinUIPeerMutationForTesting::
            SetExpandedAfterCommit);
    tree.Expand(root);
    CHECK_FALSE(tree.IsExpanded(root));
    CHECK_FALSE(tree.WinUIIsPeerExpandedForTesting(root));
    CHECK(expanding == 3);
    CHECK(expanded == 1);

    int keys = 0;
    bool handleKey = true;
    tree.Bind(
        wxEVT_TREE_KEY_DOWN,
        [&](wxTreeEvent& event)
        {
            ++keys;
            if ( !handleKey )
                event.Skip();
        });
    CHECK(tree.WinUIKeyDownForTesting('A'));
    CHECK(keys == 1);
    handleKey = false;
    CHECK_FALSE(tree.WinUIKeyDownForTesting('B'));
    CHECK(keys == 2);

    wxTreeCtrl multi(
        parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180),
        wxTR_HAS_BUTTONS | wxTR_MULTIPLE | wxTR_EDIT_LABELS);
    const wxTreeItemId multiRoot = multi.AddRoot("multi root");
    const wxTreeItemId multiFirst =
        multi.AppendItem(multiRoot, "multi first");
    const wxTreeItemId multiSecond =
        multi.AppendItem(multiRoot, "multi second");
    multi.SelectItem(multiRoot);

    int batchChanging = 0;
    int batchChanged = 0;
    bool vetoBatch = true;
    multi.Bind(
        wxEVT_TREE_SEL_CHANGING,
        [&](wxTreeEvent& event)
        {
            ++batchChanging;
            if ( vetoBatch )
                event.Veto();
        });
    multi.Bind(
        wxEVT_TREE_SEL_CHANGED,
        [&](wxTreeEvent&)
        {
            ++batchChanged;
        });

    multi.SelectChildren(multiRoot);
    CHECK(multi.IsSelected(multiRoot));
    CHECK_FALSE(multi.IsSelected(multiFirst));
    CHECK_FALSE(multi.IsSelected(multiSecond));
    CHECK(batchChanging == 1);
    CHECK(batchChanged == 0);

    vetoBatch = false;
    multi.SelectChildren(multiRoot);
    CHECK_FALSE(multi.IsSelected(multiRoot));
    CHECK(multi.IsSelected(multiFirst));
    CHECK(multi.IsSelected(multiSecond));
    CHECK(batchChanging == 2);
    CHECK(batchChanged == 1);

    vetoBatch = true;
    multi.UnselectAll();
    CHECK(multi.IsSelected(multiFirst));
    CHECK(multi.IsSelected(multiSecond));
    CHECK(batchChanging == 3);
    CHECK(batchChanged == 1);

    vetoBatch = false;
    multi.UnselectAll();
    CHECK_FALSE(multi.IsSelected(multiFirst));
    CHECK_FALSE(multi.IsSelected(multiSecond));
    wxArrayTreeItemIds noSelections;
    CHECK(multi.GetSelections(noSelections) == 0);
    CHECK(batchChanging == 4);
    CHECK(batchChanged == 2);

    multi.Expand(multiRoot);
    multi.SelectItem(multiFirst);
    multi.SelectItem(multiSecond);
    multi.Show();
    DrainTreeDispatch(6);
    wxArrayTreeItemIds focusedSelections;
    REQUIRE(multi.GetSelections(focusedSelections) == 2);
    REQUIRE(multi.WinUIFocusPeerItemForTesting(multiFirst));
    CHECK(multi.GetFocusedItem() == multiFirst);
    REQUIRE(multi.WinUIFocusPeerItemForTesting(multiSecond));
    CHECK(multi.GetFocusedItem() == multiSecond);
    REQUIRE(multi.WinUIFocusPeerItemForTesting(multiFirst));
    CHECK(multi.GetFocusedItem() == multiFirst);
    CHECK(multi.GetSelections(focusedSelections) == 2);
    CHECK(multi.IsSelected(multiFirst));
    CHECK(multi.IsSelected(multiSecond));

    int activated = 0;
    int menus = 0;
    multi.Bind(
        wxEVT_TREE_ITEM_ACTIVATED,
        [&](wxTreeEvent& event)
        {
            ++activated;
            CHECK(event.GetItem() == multiFirst);
        });
    multi.Bind(
        wxEVT_TREE_ITEM_MENU,
        [&](wxTreeEvent& event)
        {
            ++menus;
            CHECK(event.GetItem() == multiFirst);
            CHECK(event.GetPoint() != wxDefaultPosition);
        });
    REQUIRE(multi.WinUIKeyDownForTesting(WXK_SPACE));
    REQUIRE(multi.WinUIKeyDownForTesting(WXK_RETURN));
    CHECK_FALSE(multi.WinUIKeyDownForTesting(
        WXK_RETURN, true /* control */));
    REQUIRE(multi.WinUIKeyDownForTesting(WXK_MENU));
    CHECK(activated == 2);
    CHECK(menus == 1);

    int beginEdits = 0;
    multi.Bind(
        wxEVT_TREE_BEGIN_LABEL_EDIT,
        [&](wxTreeEvent& event)
        {
            ++beginEdits;
            CHECK(event.GetItem() == multiFirst);
        });
    REQUIRE(multi.WinUIKeyDownForTesting(WXK_F2));
    REQUIRE(multi.GetEditControl());
    CHECK(beginEdits == 1);
    multi.EndEditLabel(multiFirst, true);
}

TEST_CASE("wxWinUI TreeCtrl ancestor expansion and scrolling contracts",
          "[treectrl][winui-treemodel][events][geometry]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("selection uses vetoable expansion events")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");
        std::vector<wxString> events;
        tree.Bind(
            wxEVT_TREE_SEL_CHANGING,
            [&](wxTreeEvent&) { events.push_back("selection-changing"); });
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDING,
            [&](wxTreeEvent& event)
            {
                events.push_back(
                    event.GetItem() == branch
                        ? "expanding-branch"
                        : "expanding-root");
                if ( event.GetItem() == branch )
                    event.Veto();
            });
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDED,
            [&](wxTreeEvent& event)
            {
                events.push_back(
                    event.GetItem() == branch
                        ? "expanded-branch"
                        : "expanded-root");
            });
        tree.Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent&) { events.push_back("selection-changed"); });

        tree.SelectItem(leaf);
        CHECK(tree.GetSelection() == leaf);
        CHECK_FALSE(tree.IsExpanded(branch));
        CHECK(tree.IsExpanded(root));
        REQUIRE(events.size() == 5);
        CHECK(events[0] == "selection-changing");
        CHECK(events[1] == "expanding-branch");
        CHECK(events[2] == "expanding-root");
        CHECK(events[3] == "expanded-root");
        CHECK(events[4] == "selection-changed");
    }

    SECTION("selection reprojects an effective expanded image once")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        wxWithImages::Images images;
        images.push_back(wxBitmapBundle::FromBitmap(
            MakeTreeBitmap(*wxRED)));
        images.push_back(wxBitmapBundle::FromBitmap(
            MakeTreeBitmap(*wxBLUE)));
        tree.SetImages(images);

        const wxTreeItemId root = tree.AddRoot("root", 0);
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");

        const std::uintptr_t collapsedImage =
            tree.WinUIGetPeerItemImageIdentityForTesting(root);
        REQUIRE(collapsedImage != 0);

        int expanding = 0;
        std::uintptr_t callbackImage = 0;
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDING,
            [&](wxTreeEvent& event)
            {
                if ( event.GetItem() == root )
                {
                    ++expanding;
                    // Mutate the expanded slot from the vetoable callback.
                    // The immediate SetItemImage() projection still sees the
                    // collapsed model; the ancestor transaction must detect
                    // and reproject the effective image after committing.
                    tree.SetItemImage(
                        root, 1, wxTreeItemIcon_Expanded);
                    callbackImage =
                        tree.WinUIGetPeerItemImageIdentityForTesting(root);
                }
            });

        tree.WinUIResetModelStatsForTesting();
        tree.SelectItem(leaf);
        REQUIRE(tree.GetSelection() == leaf);
        CHECK(expanding == 1);
        REQUIRE(callbackImage != 0);
        // The callback-time projection is created while the original source
        // is still retained by the PropertySet, so these identities cannot
        // alias even if the COM allocator later recycles an old address.
        CHECK(callbackImage != collapsedImage);
        const std::uintptr_t expandedImage =
            tree.WinUIGetPeerItemImageIdentityForTesting(root);
        REQUIRE(expandedImage != 0);
        // The post-commit source is allocated before replacing the still-live
        // callback source. This proves that expansion caused a second
        // projection without relying on a historical, freed COM address.
        CHECK(expandedImage != callbackImage);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.GetItemImage(root, wxTreeItemIcon_Expanded) == 1);

        const wxTreeCtrl::WinUIModelStats stats =
            tree.WinUIGetModelStatsForTesting();
        // One callback-time collapsed projection, one post-commit expanded
        // projection, and the new selection. The image-less branch is never
        // sent through the content projector.
        CHECK(stats.peerUpdateCount == 3);
        CHECK(stats.fullRefreshCount == 0);
    }

    SECTION("a nested selection from EXPANDING is the latest writer")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");
        const wxTreeItemId sibling = tree.AppendItem(root, "sibling");
        tree.SelectItem(root);
        REQUIRE(tree.GetSelection() == root);

        bool nested = false;
        bool oldSelectionWasStable = false;
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDING,
            [&](wxTreeEvent& event)
            {
                if ( event.GetItem() == branch && !nested )
                {
                    nested = true;
                    oldSelectionWasStable =
                        tree.GetSelection() == root &&
                        tree.IsSelected(root);
                    tree.SelectItem(sibling);
                }
            });

        tree.SelectItem(leaf);
        CHECK(nested);
        CHECK(oldSelectionWasStable);
        CHECK(tree.GetSelection() == sibling);
        CHECK(tree.IsExpanded(root));
        CHECK_FALSE(tree.IsExpanded(branch));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK_FALSE(
            tree.WinUIIsPeerExpandedForTesting(branch));
    }

    SECTION("a nested selection from EXPANDED repairs deferred peers")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");
        const wxTreeItemId sibling = tree.AppendItem(root, "sibling");
        tree.SelectItem(root);
        REQUIRE(tree.GetSelection() == root);

        bool nested = false;
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDED,
            [&](wxTreeEvent& event)
            {
                if ( event.GetItem() == branch && !nested )
                {
                    nested = true;
                    tree.SelectItem(sibling);
                }
            });

        tree.SelectItem(leaf);
        CHECK(nested);
        CHECK(tree.GetSelection() == sibling);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.IsExpanded(branch));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(branch));
    }

    SECTION("a deferred peer failure reconciles before selection commits")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");
        tree.SelectItem(root);
        REQUIRE(tree.GetSelection() == root);

        int expanding = 0;
        int expanded = 0;
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDING,
            [&](wxTreeEvent&) { ++expanding; });
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDED,
            [&](wxTreeEvent&) { ++expanded; });
        tree.WinUIFailNextPeerMutationForTesting(
            wxTreeCtrl::WinUIPeerMutationForTesting::
                SetExpandedAfterCommit);

        tree.SelectItem(leaf);
        CHECK(tree.GetSelection() == leaf);
        CHECK(expanding == 2);
        CHECK(expanded == 2);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.IsExpanded(branch));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(branch));
    }

    SECTION("selection waits when both projection and reconciliation fail")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");
        tree.SelectItem(root);
        REQUIRE(tree.GetSelection() == root);

        // One failure aborts the root-to-leaf expansion write and two more
        // exhaust the bounded structural reconciliation attempts. The wx
        // model still commits, but peer selection must wait for the queued
        // full repair instead of targeting a partial hierarchy.
        tree.WinUIFailPeerMutationsForTesting(
            wxTreeCtrl::WinUIPeerMutationForTesting::
                SetExpandedAfterCommit,
            3);
        {
            wxLogNull suppressExpectedInjectedFailures;
            tree.SelectItem(leaf);
        }
        CHECK(tree.GetSelection() == leaf);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.IsExpanded(branch));
        CHECK(tree.WinUIIsPeerStructureRepairPendingForTesting());

        DrainTreeDispatch(8);
        CHECK_FALSE(
            tree.WinUIIsPeerStructureRepairPendingForTesting());
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(branch));
        CHECK(tree.GetSelection() == leaf);
    }

    SECTION("EnsureVisible expands with veto while ScrollTo never expands")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");
        const wxTreeItemId sibling = tree.AppendItem(root, "sibling");
        bool vetoBranch = false;
        bool reenterToSibling = false;
        std::vector<wxString> events;
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDING,
            [&](wxTreeEvent& event)
            {
                events.push_back(
                    event.GetItem() == branch
                        ? "expanding-branch"
                        : "expanding-root");
                if ( reenterToSibling && event.GetItem() == branch )
                {
                    reenterToSibling = false;
                    tree.EnsureVisible(sibling);
                }
                if ( vetoBranch && event.GetItem() == branch )
                    event.Veto();
            });
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDED,
            [&](wxTreeEvent& event)
            {
                events.push_back(
                    event.GetItem() == branch
                        ? "expanded-branch"
                        : "expanded-root");
            });

        tree.EnsureVisible(leaf);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.IsExpanded(branch));
        REQUIRE(events.size() == 4);
        CHECK(events[0] == "expanding-branch");
        CHECK(events[1] == "expanded-branch");
        CHECK(events[2] == "expanding-root");
        CHECK(events[3] == "expanded-root");

        tree.Collapse(branch);
        tree.Collapse(root);
        events.clear();
        vetoBranch = true;
        tree.EnsureVisible(leaf);
        CHECK(tree.IsExpanded(root));
        CHECK_FALSE(tree.IsExpanded(branch));
        REQUIRE(events.size() == 3);
        CHECK(events[0] == "expanding-branch");
        CHECK(events[1] == "expanding-root");
        CHECK(events[2] == "expanded-root");

        tree.Collapse(root);
        events.clear();
        vetoBranch = false;
        reenterToSibling = true;
        tree.EnsureVisible(leaf);
        CHECK_FALSE(reenterToSibling);
        CHECK(tree.IsExpanded(root));
        CHECK_FALSE(tree.IsExpanded(branch));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK_FALSE(
            tree.WinUIIsPeerExpandedForTesting(branch));

        tree.Collapse(root);
        events.clear();
        tree.ScrollTo(leaf);
        CHECK_FALSE(tree.IsExpanded(root));
        CHECK_FALSE(tree.IsExpanded(branch));
        CHECK(events.empty());
    }

    SECTION("ScrollTo maps a hidden root to its first child")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 140),
            wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);
        const wxTreeItemId root = tree.AddRoot("hidden root");
        const wxTreeItemId first = tree.AppendItem(root, "first");
        wxTreeItemId last = first;
        for ( int i = 0; i < 250; ++i )
        {
            last = tree.AppendItem(
                root, wxString::Format("row %d", i));
        }
        tree.Show();
        DrainTreeDispatch(6);
        tree.ScrollTo(last);
        DrainTreeDispatch(8);
        CHECK(tree.IsVisible(last));
        tree.ScrollTo(root);
        DrainTreeDispatch(8);
        CHECK(tree.IsVisible(first));
    }

    SECTION("focused item is a latest-writer transaction")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(300, 180));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch = tree.AppendItem(root, "branch");
        const wxTreeItemId leaf = tree.AppendItem(branch, "leaf");
        const wxTreeItemId sibling = tree.AppendItem(root, "sibling");
        tree.Expand(root);
        tree.Show();
        DrainTreeDispatch(6);
        REQUIRE(tree.WinUIFocusPeerItemForTesting(sibling));
        REQUIRE(tree.GetFocusedItem() == sibling);

        bool reenter = true;
        bool veto = false;
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDING,
            [&](wxTreeEvent& event)
            {
                if ( event.GetItem() != branch )
                    return;
                if ( reenter )
                    tree.SetFocusedItem(sibling);
                if ( veto )
                    event.Veto();
            });

        tree.SetFocusedItem(leaf);
        CHECK(tree.GetFocusedItem() == sibling);
        CHECK_FALSE(tree.IsExpanded(branch));

        reenter = false;
        veto = true;
        tree.SetFocusedItem(leaf);
        CHECK(tree.GetFocusedItem() == sibling);
        CHECK_FALSE(tree.IsExpanded(branch));
    }
}

TEST_CASE("wxWinUI TreeCtrl deletion selection is transactional",
          "[treectrl][winui-treemodel][delete][selection][events]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("selected Delete chooses the next sibling")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(280, 160));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId first = tree.AppendItem(root, "first");
        const wxTreeItemId second = tree.AppendItem(root, "second");
        const wxTreeItemId third = tree.AppendItem(root, "third");
        tree.SelectItem(second);

        int changing = 0;
        int changed = 0;
        int deleted = 0;
        tree.Bind(
            wxEVT_TREE_SEL_CHANGING,
            [&](wxTreeEvent& event)
            {
                ++changing;
                CHECK(event.GetItem() == third);
                CHECK(event.GetOldItem() == second);
            });
        tree.Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent& event)
            {
                ++changed;
                CHECK(event.GetItem() == third);
                CHECK(event.GetOldItem() == second);
            });
        tree.Bind(
            wxEVT_TREE_DELETE_ITEM,
            [&](wxTreeEvent& event)
            {
                if ( event.GetItem() == second )
                    ++deleted;
            });

        tree.Delete(second);
        CHECK(tree.GetSelection() == third);
        CHECK(tree.IsSelected(third));
        CHECK(tree.GetItemText(first) == "first");
        CHECK_FALSE(tree.WinUIGetItemPeerIdentityForTesting(second));
        CHECK(changing == 1);
        CHECK(changed == 1);
        CHECK(deleted == 1);
    }

    SECTION("veto keeps deletion but clears the doomed selection")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(280, 160));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId selected =
            tree.AppendItem(root, "selected");
        tree.AppendItem(root, "replacement");
        tree.SelectItem(selected);

        int changing = 0;
        int changed = 0;
        tree.Bind(
            wxEVT_TREE_SEL_CHANGING,
            [&](wxTreeEvent& event)
            {
                ++changing;
                event.Veto();
            });
        tree.Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent&) { ++changed; });

        tree.Delete(selected);
        CHECK_FALSE(tree.GetSelection().IsOk());
        CHECK(tree.GetCount() == 2);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(selected) == 0);
        CHECK(changing == 1);
        CHECK(changed == 0);
    }

    SECTION("DeleteChildren and DeleteAll pair selection events")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(280, 160));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId branch =
            tree.AppendItem(root, "branch");
        const wxTreeItemId leaf =
            tree.AppendItem(branch, "leaf");
        tree.SelectItem(leaf);

        int changing = 0;
        int changed = 0;
        tree.Bind(
            wxEVT_TREE_SEL_CHANGING,
            [&](wxTreeEvent&) { ++changing; });
        tree.Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent&) { ++changed; });

        tree.DeleteChildren(branch);
        CHECK(tree.GetSelection() == branch);
        CHECK(tree.GetChildrenCount(branch, false) == 0);
        CHECK(changing == 1);
        CHECK(changed == 1);

        tree.DeleteAllItems();
        CHECK_FALSE(tree.GetSelection().IsOk());
        CHECK(tree.GetCount() == 0);
        CHECK(changing == 2);
        CHECK(changed == 2);
    }

    SECTION("nested selection writer aborts before peer removal")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(280, 160));
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId first = tree.AppendItem(root, "first");
        const wxTreeItemId second = tree.AppendItem(root, "second");
        tree.SelectItem(first);
        wxTreeItemId nested;
        bool writeNested = true;
        tree.Bind(
            wxEVT_TREE_SEL_CHANGING,
            [&](wxTreeEvent&)
            {
                if ( writeNested )
                {
                    writeNested = false;
                    nested = tree.AppendItem(root, "nested newest");
                }
            });

        tree.Delete(first);
        REQUIRE(nested.IsOk());
        CHECK(tree.GetCount() == 4);
        CHECK(tree.GetItemText(first) == "first");
        CHECK(tree.GetSelection() == first);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(first) != 0);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(second) != 0);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(nested) != 0);
    }
}

TEST_CASE("wxWinUI TreeCtrl callbacks and items retire safely",
          "[treectrl][winui-treemodel][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const size_t baseline =
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();
    for ( int i = 0; i < 100; ++i )
    {
        std::unique_ptr<wxTreeCtrl> tree(
            new wxTreeCtrl(
                parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120)));
        const wxTreeItemId root = tree->AddRoot("root");
        tree->AppendItem(root, "child");
        REQUIRE(tree->WinUIQueueSelectionCorrectionForTesting());
        tree->WinUIClosePeerForTesting();
        tree->WinUIClosePeerForTesting();
        tree.reset();
        CHECK(
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline);
    }
    DrainTreeDispatch(8);
    CHECK(
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);

    int retainedDataDestroyed = 0;
    int rejectedDataDestroyed = 0;
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        const wxTreeItemId retainedRoot = tree.AddRoot(
            "old root", -1, -1,
            new CountedTreeData(&retainedDataDestroyed));
        const wxTreeItemId retainedChild =
            tree.AppendItem(retainedRoot, "old child");
        REQUIRE(retainedRoot.IsOk());
        REQUIRE(retainedChild.IsOk());
        tree.Expand(retainedRoot);
        tree.SelectItem(retainedChild);

        const std::uintptr_t rootPeer =
            tree.WinUIGetItemPeerIdentityForTesting(retainedRoot);
        const std::uintptr_t childPeer =
            tree.WinUIGetItemPeerIdentityForTesting(retainedChild);
        int deleteEvents = 0;
        tree.Bind(
            wxEVT_TREE_DELETE_ITEM,
            [&](wxTreeEvent&)
            {
                ++deleteEvents;
            });

        CountedTreeData * const rejectedData =
            new CountedTreeData(&rejectedDataDestroyed);
        const wxTreeItemId replacement = tree.AddRoot(
            "replacement", -1, -1, rejectedData);
        CHECK_FALSE(replacement.IsOk());
        CHECK(tree.GetRootItem() == retainedRoot);
        CHECK(tree.GetCount() == 2);
        CHECK(tree.GetItemText(retainedRoot) == "old root");
        CHECK(tree.GetItemText(retainedChild) == "old child");
        CHECK(tree.IsExpanded(retainedRoot));
        CHECK(tree.GetSelection() == retainedChild);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(retainedRoot) ==
              rootPeer);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(retainedChild) ==
              childPeer);
        CHECK(deleteEvents == 0);
        CHECK(retainedDataDestroyed == 0);
        CHECK(rejectedDataDestroyed == 0);
        CHECK_FALSE(rejectedData->GetId().IsOk());
        delete rejectedData;
        CHECK(rejectedDataDestroyed == 1);
    }
    CHECK(retainedDataDestroyed == 1);
    CHECK(
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);

    wxTreeCtrl *doomed = new wxTreeCtrl(
        parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
    const wxTreeItemId doomedRoot = doomed->AddRoot("root");
    const wxTreeItemId doomedChild =
        doomed->AppendItem(doomedRoot, "child");
    wxWeakRef<wxWindow> doomedLifetime(doomed);
    doomed->Bind(
        wxEVT_TREE_SEL_CHANGING,
        [doomed](wxTreeEvent&)
        {
            delete doomed;
        });
    CHECK_FALSE(
        doomed->WinUISelectPeerItemForTesting(doomedChild));
    CHECK(doomedLifetime.get() == nullptr);
    CHECK(
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);

    wxTreeCtrl *deleteDoomed = new wxTreeCtrl(
        parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
    const wxTreeItemId deleteRoot = deleteDoomed->AddRoot("root");
    const wxTreeItemId deleteChild =
        deleteDoomed->AppendItem(deleteRoot, "child");
    wxWeakRef<wxWindow> deleteLifetime(deleteDoomed);
    deleteDoomed->Bind(
        wxEVT_TREE_DELETE_ITEM,
        [deleteDoomed](wxTreeEvent&)
        {
            delete deleteDoomed;
        });
    deleteDoomed->Delete(deleteChild);
    CHECK(deleteLifetime.get() == nullptr);
    CHECK(
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);
}

TEST_CASE("wxWinUI TreeCtrl sort preserves selected node identities",
          "[treectrl][winui-treemodel][sort][selection]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SortingTreeCtrl single(
        parent, wxID_ANY, wxDefaultPosition, wxSize(260, 150));
    const wxTreeItemId root = single.AddRoot("root");
    const wxTreeItemId c = single.AppendItem(root, "c");
    const wxTreeItemId b = single.AppendItem(root, "b");
    const wxTreeItemId a = single.AppendItem(root, "a");
    single.SelectItem(b);
    single.SortChildren(root);
    CHECK(single.GetSelection() == b);
    wxTreeItemIdValue cookie = nullptr;
    CHECK(single.GetFirstChild(root, cookie) == a);
    CHECK(single.GetNextSibling(a) == b);
    CHECK(single.GetNextSibling(b) == c);

    SortingTreeCtrl multi(
        parent, wxID_ANY, wxDefaultPosition, wxSize(260, 150),
        wxTR_HAS_BUTTONS | wxTR_MULTIPLE);
    const wxTreeItemId multiRoot = multi.AddRoot("root");
    const wxTreeItemId multiC = multi.AppendItem(multiRoot, "c");
    const wxTreeItemId multiB = multi.AppendItem(multiRoot, "b");
    const wxTreeItemId multiA = multi.AppendItem(multiRoot, "a");
    multi.SelectItem(multiC);
    multi.SelectItem(multiA);
    multi.SortChildren(multiRoot);
    CHECK(multi.IsSelected(multiC));
    CHECK(multi.IsSelected(multiA));
    CHECK_FALSE(multi.IsSelected(multiB));
    cookie = nullptr;
    CHECK(multi.GetFirstChild(multiRoot, cookie) == multiA);
    CHECK(multi.GetNextSibling(multiA) == multiB);
    CHECK(multi.GetNextSibling(multiB) == multiC);
}

TEST_CASE("wxWinUI TreeCtrl after-events survive stale projection",
          "[treectrl][winui-treemodel][events][projection][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(280, 160));
    const wxTreeItemId root = tree.AddRoot("root", 0);
    const wxTreeItemId first = tree.AppendItem(root, "first", 0);
    const wxTreeItemId second = tree.AppendItem(root, "second", 0);
    const auto callback =
        std::make_shared<ArmedTreeBundleCallback>();
    wxWithImages::Images images;
    images.push_back(wxBitmapBundle::FromImpl(
        new ArmedTreeBitmapBundleImpl(callback)));
    tree.SetImages(images);
    tree.SelectItem(first);

    int changing = 0;
    int changed = 0;
    tree.Bind(
        wxEVT_TREE_SEL_CHANGING,
        [&](wxTreeEvent&) { ++changing; });
    tree.Bind(
        wxEVT_TREE_SEL_CHANGED,
        [&](wxTreeEvent&) { ++changed; });

    callback->callback =
        [&]()
        {
            tree.SetItemText(root, "selection projection nested writer");
        };
    callback->armed = true;
    tree.SelectItem(second);
    CHECK(tree.GetSelection() == second);
    CHECK(changing == 1);
    CHECK(changed == 1);
    CHECK(tree.GetItemText(root) ==
          "selection projection nested writer");

    tree.Collapse(root);
    int expanding = 0;
    int expanded = 0;
    tree.Bind(
        wxEVT_TREE_ITEM_EXPANDING,
        [&](wxTreeEvent&) { ++expanding; });
    tree.Bind(
        wxEVT_TREE_ITEM_EXPANDED,
        [&](wxTreeEvent&) { ++expanded; });
    callback->callback =
        [&]()
        {
            tree.SetItemText(first, "expansion projection nested writer");
        };
    callback->armed = true;
    tree.Expand(root);
    CHECK(tree.IsExpanded(root));
    CHECK(expanding == 1);
    CHECK(expanded == 1);
    CHECK(tree.GetItemText(first) ==
          "expansion projection nested writer");

    wxTreeCtrl multi(
        parent, wxID_ANY, wxDefaultPosition, wxSize(280, 160),
        wxTR_HAS_BUTTONS | wxTR_MULTIPLE);
    const wxTreeItemId multiRoot = multi.AddRoot("multi root");
    const wxTreeItemId multiFirst =
        multi.AppendItem(multiRoot, "multi first");
    const wxTreeItemId multiSecond =
        multi.AppendItem(multiRoot, "multi second");
    multi.SelectItem(multiFirst);

    int multiChanged = 0;
    bool installNestedWriter = true;
    multi.Bind(
        wxEVT_TREE_SEL_CHANGED,
        [&](wxTreeEvent&)
        {
            ++multiChanged;
            if ( installNestedWriter )
            {
                installNestedWriter = false;
                multi.SelectItem(multiFirst);
            }
        });

    REQUIRE(multi.WinUISelectPeerItemForTesting(multiSecond));
    DrainTreeDispatch(6);
    CHECK(multi.IsSelected(multiFirst));
    CHECK_FALSE(multi.IsSelected(multiSecond));
    CHECK(multiChanged == 2);
}

TEST_CASE("wxWinUI TreeCtrl ForceRender callbacks are terminal",
          "[treectrl][winui-treemodel][lifetime][force-render]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const size_t baseline =
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();

    SECTION("AddRoot does not form an id after destruction")
    {
        wxTreeCtrl *tree = new wxTreeCtrl(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        wxTreeCtrl * const invoking = tree;
        wxWeakRef<wxWindow> lifetime(tree);
        FlushTreeSlots(parent);
        invoking->SetSize(wxSize(223, 121));

        bool slotSynced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                CHECK(window == invoking);
                if ( window != invoking )
                    return;
                slotSynced = true;
                wxTreeCtrl * const doomed = tree;
                tree = nullptr;
                delete doomed;
            });

        const wxTreeItemId root = invoking->AddRoot("root");
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(slotSynced);
        CHECK_FALSE(root.IsOk());
        CHECK(lifetime.get() == nullptr);
        delete tree;
    }

    SECTION("selection emits no changed event after destructive flush")
    {
        wxTreeCtrl *tree = new wxTreeCtrl(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        wxTreeCtrl * const invoking = tree;
        const wxTreeItemId root = invoking->AddRoot("root");
        const wxTreeItemId child =
            invoking->AppendItem(root, "child");
        int changedEvents = 0;
        invoking->Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent&)
            {
                ++changedEvents;
            });
        wxWeakRef<wxWindow> lifetime(tree);
        FlushTreeSlots(parent);
        invoking->SetSize(wxSize(225, 123));

        bool slotSynced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                CHECK(window == invoking);
                if ( window != invoking )
                    return;
                slotSynced = true;
                wxTreeCtrl * const doomed = tree;
                tree = nullptr;
                delete doomed;
            });

        invoking->SelectItem(child);
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(slotSynced);
        CHECK(changedEvents == 0);
        CHECK(lifetime.get() == nullptr);
        delete tree;
    }

    SECTION("Delete stops after the selection flush")
    {
        wxTreeCtrl *tree = new wxTreeCtrl(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        wxTreeCtrl * const invoking = tree;
        const wxTreeItemId root = invoking->AddRoot("root");
        const wxTreeItemId child =
            invoking->AppendItem(root, "child");
        invoking->SelectItem(child);
        int deleteEvents = 0;
        invoking->Bind(
            wxEVT_TREE_DELETE_ITEM,
            [&](wxTreeEvent&)
            {
                ++deleteEvents;
            });
        wxWeakRef<wxWindow> lifetime(tree);
        FlushTreeSlots(parent);
        invoking->SetSize(wxSize(227, 125));

        bool slotSynced = false;
        wxWinUITopLevelHost::TestOnNextSlotSynced(
            [&](wxWindow *window)
            {
                CHECK(window == invoking);
                if ( window != invoking )
                    return;
                slotSynced = true;
                wxTreeCtrl * const doomed = tree;
                tree = nullptr;
                delete doomed;
            });

        invoking->Delete(child);
        wxWinUITopLevelHost::TestOnNextSlotSynced({});
        CHECK(slotSynced);
        CHECK(deleteEvents == 1);
        CHECK(lifetime.get() == nullptr);
        delete tree;
    }

    CHECK(
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
        baseline);
}

TEST_CASE("wxWinUI TreeCtrl insertion survives harmless nested writers",
          "[treectrl][winui-treemodel][transaction][reentrancy]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("AddRoot returns the exact committed item")
    {
        int destroyed = 0;
        {
            wxTreeCtrl tree(
                parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
            wxWithImages::Images images;
            images.push_back(MakeCallbackTreeBundle(
                TreeBundleCallbackPoint::PreferredSize,
                [&tree]()
                {
                    tree.SelectItem(tree.GetRootItem());
                }));
            tree.SetImages(images);

            const wxTreeItemId root = tree.AddRoot(
                "root", 0, -1, new CountedTreeData(&destroyed));
            REQUIRE(root.IsOk());
            CHECK(tree.GetRootItem() == root);
            CHECK(tree.GetCount() == 1);
            CHECK(tree.GetItemData(root) != nullptr);
            CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) != 0);
            CHECK(destroyed == 0);
        }
        CHECK(destroyed == 1);
    }

    SECTION("AppendItem returns the exact committed item")
    {
        int destroyed = 0;
        {
            wxTreeCtrl tree(
                parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
            const wxTreeItemId root = tree.AddRoot("root");
            wxWithImages::Images images;
            images.push_back(MakeCallbackTreeBundle(
                TreeBundleCallbackPoint::PreferredSize,
                [&tree, root]()
                {
                    tree.SelectItem(root);
                }));
            tree.SetImages(images);

            const wxTreeItemId child = tree.AppendItem(
                root, "child", 0, -1,
                new CountedTreeData(&destroyed));
            REQUIRE(child.IsOk());
            CHECK(tree.GetItemParent(child) == root);
            CHECK(tree.GetCount() == 2);
            CHECK(tree.GetItemData(child) != nullptr);
            CHECK(tree.WinUIGetItemPeerIdentityForTesting(child) != 0);
            CHECK(destroyed == 0);
        }
        CHECK(destroyed == 1);
    }
}

TEST_CASE("wxWinUI TreeCtrl projection is revision checked",
          "[treectrl][winui-treemodel][projection][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    SECTION("bundle mutation is last-writer-wins")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
        const wxTreeItemId root = tree.AddRoot("outer");
        tree.SetItemImage(root, 0);

        unsigned callbacks = 0;
        wxWithImages::Images images;
        images.push_back(MakeCallbackTreeBundle(
            TreeBundleCallbackPoint::PreferredSize,
            [&]()
            {
                ++callbacks;
                tree.SetItemText(root, "nested newest");
            }));
        tree.SetImages(images);

        CHECK(callbacks == 1);
        CHECK(tree.GetItemText(root) == "nested newest");
        CHECK(tree.WinUIGetPeerItemTextForTesting(root) ==
              "nested newest");
    }

    SECTION("a callback storm consumes one deferred replay wave")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
        const wxTreeItemId root = tree.AddRoot("seed");
        tree.SetItemImage(root, 0);
        tree.Show();
        tree.Update();
        DrainTreeDispatch(4);
        REQUIRE(
            tree.WinUIGetPeerAutomationNameForTesting(root) == "seed");

        TreeProjectionStorm probe;
        probe.tree = &tree;
        probe.item = root;
        probe.remaining = 24;
        wxWithImages::Images images;
        images.push_back(MakeCallbackTreeBundle(
            TreeBundleCallbackPoint::PreferredSize,
            [&probe]()
            {
                ContinueTreeProjectionStorm(&probe);
            }));
        {
            wxLogNull suppressExpectedBoundedRetryWarning;
            tree.SetImages(images);
            CHECK(probe.remaining == 16);
            for ( int i = 0; i < 4; ++i )
            {
                tree.SetItemText(
                    root, wxString::Format("external-%d", i));
                CHECK(probe.remaining == 16);
            }
            DrainTreeDispatch(8);
        }
        CHECK(probe.remaining == 8);

        tree.SetItemText(root, "external retry");
        DrainTreeDispatch(8);
        CHECK(probe.remaining == 0);
        CHECK(tree.GetItemText(root) == probe.newest);
        CHECK(tree.WinUIGetPeerItemTextForTesting(root) ==
              probe.newest);
        CHECK(tree.WinUIGetPeerAutomationNameForTesting(root) ==
              probe.newest);
    }

    SECTION("a queued replay owns only a weak lifetime ticket")
    {
        const size_t baseline =
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();
        wxTreeCtrl *tree = new wxTreeCtrl(
            parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
        const wxTreeItemId root = tree->AddRoot("seed");
        tree->SetItemImage(root, 0);

        TreeProjectionStorm probe;
        probe.tree = tree;
        probe.item = root;
        probe.remaining = 24;
        wxWithImages::Images images;
        images.push_back(MakeCallbackTreeBundle(
            TreeBundleCallbackPoint::PreferredSize,
            [&probe]()
            {
                ContinueTreeProjectionStorm(&probe);
            }));
        {
            wxLogNull suppressExpectedBoundedRetryWarning;
            tree->SetImages(images);
        }
        CHECK(probe.remaining == 16);

        delete tree;
        tree = nullptr;
        probe.tree = nullptr;
        CHECK(
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline);
        DrainTreeDispatch(8);
        CHECK(
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline);
    }

    SECTION("image-list virtual calls revalidate the owner")
    {
        for ( const CallbackTreeImageList::Point point :
              { CallbackTreeImageList::Point::ImageCount,
                CallbackTreeImageList::Point::Bitmap } )
        {
            wxTreeCtrl *tree = new wxTreeCtrl(
                parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
            wxTreeCtrl * const invoking = tree;
            const wxTreeItemId root = invoking->AddRoot("doomed");
            invoking->SetItemImage(root, 0);
            wxWeakRef<wxWindow> lifetime(tree);
            unsigned callbacks = 0;
            CallbackTreeImageList images(
                point,
                [&]()
                {
                    ++callbacks;
                    wxTreeCtrl * const doomed = tree;
                    tree = nullptr;
                    delete doomed;
                });

            invoking->SetImageList(&images);

            CHECK(callbacks == 1);
            CHECK(lifetime.get() == nullptr);
            delete tree;
        }
    }

    SECTION("DPI virtual calls revalidate the owner")
    {
        CallbackDPITreeCtrl *tree = new CallbackDPITreeCtrl(
            parent, wxSize(240, 130));
        CallbackDPITreeCtrl * const invoking = tree;
        const wxTreeItemId root = invoking->AddRoot("doomed");
        wxWeakRef<wxWindow> lifetime(tree);
        unsigned callbacks = 0;
        invoking->SetDPICallback(
            [&]()
            {
                ++callbacks;
                CallbackDPITreeCtrl * const doomed = tree;
                tree = nullptr;
                delete doomed;
            });

        invoking->SetItemText(root, "project me");

        CHECK(callbacks == 1);
        CHECK(lifetime.get() == nullptr);
        delete tree;
    }

    SECTION("layout-direction virtual calls revalidate the owner")
    {
        SECTION("Create")
        {
            CallbackLayoutTreeCtrl *tree =
                new CallbackLayoutTreeCtrl;
            CallbackLayoutTreeCtrl * const invoking = tree;
            wxWeakRef<wxWindow> lifetime(tree);
            tree->SetLayoutCallback(
                [&]()
                {
                    CallbackLayoutTreeCtrl * const doomed = tree;
                    tree = nullptr;
                    delete doomed;
                });
            CHECK_FALSE(invoking->Create(
                parent, wxID_ANY, wxDefaultPosition,
                wxSize(240, 130)));
            CHECK(lifetime.get() == nullptr);
            delete tree;
        }

        SECTION("SetIndent")
        {
            CallbackLayoutTreeCtrl *tree =
                new CallbackLayoutTreeCtrl;
            REQUIRE(tree->Create(
                parent, wxID_ANY, wxDefaultPosition,
                wxSize(240, 130)));
            CallbackLayoutTreeCtrl * const invoking = tree;
            wxWeakRef<wxWindow> lifetime(tree);
            tree->SetLayoutCallback(
                [&]()
                {
                    CallbackLayoutTreeCtrl * const doomed = tree;
                    tree = nullptr;
                    delete doomed;
                });
            invoking->SetIndent(31);
            CHECK(lifetime.get() == nullptr);
            delete tree;
        }

        SECTION("SetLayoutDirection")
        {
            CallbackLayoutTreeCtrl *tree =
                new CallbackLayoutTreeCtrl;
            REQUIRE(tree->Create(
                parent, wxID_ANY, wxDefaultPosition,
                wxSize(240, 130)));
            CallbackLayoutTreeCtrl * const invoking = tree;
            wxWeakRef<wxWindow> lifetime(tree);
            tree->SetLayoutCallback(
                [&]()
                {
                    CallbackLayoutTreeCtrl * const doomed = tree;
                    tree = nullptr;
                    delete doomed;
                });
            invoking->SetLayoutDirection(wxLayout_RightToLeft);
            CHECK(lifetime.get() == nullptr);
            delete tree;
        }

        SECTION("DPI event")
        {
            CallbackLayoutTreeCtrl *tree =
                new CallbackLayoutTreeCtrl;
            REQUIRE(tree->Create(
                parent, wxID_ANY, wxDefaultPosition,
                wxSize(240, 130)));
            CallbackLayoutTreeCtrl * const invoking = tree;
            wxWeakRef<wxWindow> lifetime(tree);
            tree->SetLayoutCallback(
                [&]()
                {
                    CallbackLayoutTreeCtrl * const doomed = tree;
                    tree = nullptr;
                    delete doomed;
                });
            wxDPIChangedEvent dpiEvent(
                wxSize(96, 96), wxSize(144, 144));
            dpiEvent.SetEventObject(invoking);
            dpiEvent.SetId(invoking->GetId());
            invoking->ProcessWindowEvent(dpiEvent);
            CHECK(lifetime.get() == nullptr);
            delete tree;
        }
    }

    SECTION("selection reentrancy guard clears on a stale projection")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130),
            wxTR_HAS_BUTTONS | wxTR_MULTIPLE);
        const wxTreeItemId root = tree.AddRoot("root");
        const wxTreeItemId first = tree.AppendItem(root, "first");
        const wxTreeItemId second = tree.AppendItem(root, "second");
        const auto callback =
            std::make_shared<ArmedTreeBundleCallback>();
        wxWithImages::Images images;
        images.push_back(wxBitmapBundle::FromImpl(
            new ArmedTreeBitmapBundleImpl(callback)));
        tree.SetImages(images);
        tree.SetItemImage(first, 0);
        tree.SelectItem(first);
        tree.SelectItem(second);
        callback->callback =
            [&tree, root]()
            {
                tree.SetItemText(root, "nested newest");
            };
        callback->armed = true;

        REQUIRE(tree.WinUISelectPeerItemForTesting(second));
        CHECK_FALSE(tree.WinUIIsPeerSelectionChangeForTesting());
        CHECK(tree.GetItemText(root) == "nested newest");
    }

    SECTION("delete and replacement invalidate the old item pass")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
        const wxTreeItemId oldRoot = tree.AddRoot("old root");
        tree.SetItemImage(oldRoot, 0);

        wxTreeItemId replacement;
        unsigned callbacks = 0;
        wxWithImages::Images images;
        images.push_back(MakeCallbackTreeBundle(
            TreeBundleCallbackPoint::PreferredSize,
            [&]()
            {
                ++callbacks;
                tree.DeleteAllItems();
                replacement = tree.AddRoot("replacement", 0);
            }));
        tree.SetImages(images);

        CHECK(callbacks == 1);
        REQUIRE(replacement.IsOk());
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(oldRoot) == 0);
        CHECK(tree.GetRootItem() == replacement);
        CHECK(tree.GetCount() == 1);
        CHECK(tree.GetItemText(replacement) == "replacement");
        CHECK(tree.WinUIGetPeerItemTextForTesting(replacement) ==
              "replacement");
    }

    SECTION("destruction from bundle resolution is terminal")
    {
        const TreeBundleCallbackPoint callbackPoint =
            GENERATE(TreeBundleCallbackPoint::DefaultSize,
                     TreeBundleCallbackPoint::Bitmap);
        CAPTURE(callbackPoint);
        const size_t baseline =
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();
        wxTreeCtrl *tree = new wxTreeCtrl(
            parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
        wxTreeCtrl * const invoking = tree;
        const wxTreeItemId root = invoking->AddRoot("doomed");
        invoking->SetItemImage(root, 0);
        wxWeakRef<wxWindow> lifetime(tree);

        unsigned callbacks = 0;
        unsigned bitmapCalls = 0;
        wxWithImages::Images images;
        images.push_back(MakeCallbackTreeBundle(
            callbackPoint,
            [&]()
            {
                ++callbacks;
                wxTreeCtrl * const doomed = tree;
                tree = nullptr;
                delete doomed;
            },
            &bitmapCalls));
        invoking->SetImages(images);

        CHECK(callbacks == 1);
        CHECK(bitmapCalls ==
              (callbackPoint == TreeBundleCallbackPoint::Bitmap
                   ? 1u
                   : 0u));
        CHECK(lifetime.get() == nullptr);
        CHECK(
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline);
        delete tree;
    }
}

TEST_CASE("wxWinUI TreeCtrl consensus stops at a dead owner",
          "[treectrl][winui-treemodel][projection][reentrancy][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const size_t baseline =
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();
    wxTreeCtrl *tree = new wxTreeCtrl(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
    wxTreeCtrl * const invoking = tree;
    const wxTreeItemId root = invoking->AddRoot("doomed");
    invoking->SetItemImage(root, 0);
    wxWeakRef<wxWindow> lifetime(tree);

    unsigned firstCalls = 0;
    unsigned secondCalls = 0;
    wxWithImages::Images images;
    images.push_back(MakeCallbackTreeBundle(
        TreeBundleCallbackPoint::PreferredSize,
        [&]()
        {
            ++firstCalls;
            wxTreeCtrl * const doomed = tree;
            tree = nullptr;
            delete doomed;
        }));
    images.push_back(MakeCallbackTreeBundle(
        TreeBundleCallbackPoint::PreferredSize,
        [&]() { ++secondCalls; }));
    invoking->SetImages(images);

    CHECK(firstCalls == 1);
    CHECK(secondCalls == 0);
    CHECK(lifetime.get() == nullptr);
    CHECK(wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
          baseline);
    delete tree;
}

TEST_CASE("wxWinUI TreeCtrl consensus stops at a stale revision",
          "[treectrl][winui-treemodel][projection][reentrancy]"
          "[transaction][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
    const wxTreeItemId root = tree.AddRoot("stale");
    tree.SetItemImage(root, 0);

    unsigned firstCalls = 0;
    unsigned secondCalls = 0;
    wxWithImages::Images newestImages{
        MakeTreeDPIBundle(wxSize(23, 23), *wxGREEN)
    };
    wxWithImages::Images staleImages;
    staleImages.push_back(MakeCallbackTreeBundle(
        TreeBundleCallbackPoint::PreferredSize,
        [&]()
        {
            ++firstCalls;
            tree.SetImages(newestImages);
        }));
    staleImages.push_back(MakeCallbackTreeBundle(
        TreeBundleCallbackPoint::PreferredSize,
        [&]() { ++secondCalls; }));
    tree.SetImages(staleImages);

    wxSize imagePixels;
    wxSize statePixels;
    wxSize imageDIPs;
    wxSize stateDIPs;
    std::uint64_t generation = 0;
    REQUIRE(tree.WinUIGetPeerItemImageProjectionForTesting(
        root, &imagePixels, &statePixels, &generation,
        &imageDIPs, &stateDIPs));
    CHECK(firstCalls == 1);
    CHECK(secondCalls == 0);
    CHECK(imagePixels ==
          newestImages[0].GetPreferredBitmapSizeAtScale(
              tree.GetDPIScaleFactor()));
    CHECK(imageDIPs == wxSize(23, 23));
}

TEST_CASE("wxWinUI TreeCtrl consensus tie uses the larger raster",
          "[treectrl][winui-treemodel][projection][images][dpi]"
          "[consensus][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
    const wxTreeItemId root = tree.AddRoot("tie");
    tree.SetItemImage(root, 0);
    const wxWithImages::Images images{
        MakeTreeDPIBundle(wxSize(11, 11), *wxBLUE),
        MakeTreeDPIBundle(wxSize(17, 17), *wxGREEN)
    };
    tree.SetImages(images);

    wxSize imagePixels;
    wxSize statePixels;
    wxSize imageDIPs;
    wxSize stateDIPs;
    std::uint64_t generation = 0;
    REQUIRE(tree.WinUIGetPeerItemImageProjectionForTesting(
        root, &imagePixels, &statePixels, &generation,
        &imageDIPs, &stateDIPs));
    CHECK(imagePixels ==
          images[1].GetPreferredBitmapSizeAtScale(
              tree.GetDPIScaleFactor()));
    CHECK(imageDIPs == wxSize(11, 11));
}

TEST_CASE("wxWinUI TreeCtrl invalid bundle sizes remove the image",
          "[treectrl][winui-treemodel][projection][images][invalid]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const TreeBundleCallbackPoint invalidPoint =
        GENERATE(TreeBundleCallbackPoint::DefaultSize,
                 TreeBundleCallbackPoint::PreferredSize);
    CAPTURE(invalidPoint);
    unsigned bitmapCalls = 0;
    wxWithImages::Images images{
        MakeInvalidTreeBundle(invalidPoint, &bitmapCalls),
        MakeTreeDPIBundle(wxSize(17, 17), *wxGREEN)
    };

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 130));
    const wxTreeItemId root = tree.AddRoot("invalid");
    tree.SetItemImage(root, 0);
    tree.SetImages(images);

    wxSize imagePixels;
    wxSize statePixels;
    wxSize imageDIPs;
    wxSize stateDIPs;
    std::uint64_t generation = 0;
    REQUIRE(tree.WinUIGetPeerItemImageProjectionForTesting(
        root, &imagePixels, &statePixels, &generation,
        &imageDIPs, &stateDIPs));
    CHECK(bitmapCalls == 0);
    CHECK(imagePixels == wxSize());
    CHECK(imageDIPs == wxSize());
    CHECK(tree.GetItemText(root) == "invalid");
    CHECK(tree.WinUIGetPeerItemTextForTesting(root) == "invalid");
    CHECK(generation != 0);
}

TEST_CASE("wxWinUI TreeCtrl coalesces projection until outer Thaw",
          "[treectrl][winui-treemodel][freeze]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 120));
    tree.WinUIResetModelStatsForTesting();

    tree.Freeze();
    tree.Freeze();
    const wxTreeItemId root = tree.AddRoot("root");
    REQUIRE(root.IsOk());
    wxTreeItemId child;
    for ( int i = 0; i < 64; ++i )
    {
        child = tree.AppendItem(
            root, wxString::Format("entry %d", i));
        REQUIRE(child.IsOk());
        tree.SetItemText(child, wxString::Format("updated %d", i));
        tree.SetItemHasChildren(child, true);
    }

    wxTreeCtrl::WinUIModelStats frozen =
        tree.WinUIGetModelStatsForTesting();
    CHECK(tree.GetCount() == 65);
    CHECK(frozen.peerUpdateCount == 0);
    CHECK(tree.WinUIGetPeerItemTextForTesting(child).empty());

    tree.Thaw();
    CHECK(tree.IsFrozen());
    frozen = tree.WinUIGetModelStatsForTesting();
    CHECK(frozen.peerUpdateCount == 0);
    CHECK(tree.WinUIGetPeerItemTextForTesting(child).empty());

    tree.Thaw();
    CHECK_FALSE(tree.IsFrozen());
    const wxTreeCtrl::WinUIModelStats published =
        tree.WinUIGetModelStatsForTesting();
    CHECK(published.peerUpdateCount <= 65);
    CHECK(tree.WinUIGetPeerItemTextForTesting(root) == "root");
    CHECK(tree.WinUIGetPeerItemTextForTesting(child) == "updated 63");
}

TEST_CASE("wxWinUI TreeCtrl destroys a frozen pending projection",
          "[treectrl][winui-treemodel][freeze][lifetime]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const size_t baseline =
        wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();
    int destroyedData = 0;
    wxTreeCtrl * const tree = new wxTreeCtrl(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 120));
    tree->Freeze();
    const wxTreeItemId root = tree->AddRoot(
        "root", -1, -1, new CountedTreeData(&destroyedData));
    REQUIRE(root.IsOk());
    for ( int i = 0; i < 32; ++i )
    {
        REQUIRE(tree->AppendItem(
            root, wxString::Format("pending %d", i)).IsOk());
    }
    CHECK(tree->WinUIGetModelStatsForTesting().peerUpdateCount == 0);

    delete tree;
    CHECK(destroyedData == 1);
    CHECK(wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
          baseline);
    DrainTreeDispatch(4);
    CHECK(wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
          baseline);
}

TEST_CASE("wxWinUI TreeCtrl model deltas stay linear at 10k",
          "[treectrl][winui-treemodel][performance]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(240, 120));
    const wxTreeItemId root = tree.AddRoot("root");
    tree.WinUIResetModelStatsForTesting();

    constexpr int ItemCount = 10000;
    wxTreeItemId last;
    const auto started = std::chrono::steady_clock::now();
    for ( int i = 0; i < ItemCount; ++i )
    {
        last = tree.AppendItem(
            root, wxString::Format("item %d", i));
        REQUIRE(last.IsOk());
    }
    const auto elapsed = std::chrono::duration_cast<
        std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - started);
    INFO("10k insertion time: " << elapsed.count() << " ms");
    CHECK(elapsed < std::chrono::seconds(120));

    const wxTreeCtrl::WinUIModelStats inserted =
        tree.WinUIGetModelStatsForTesting();
    CHECK(tree.GetCount() == ItemCount + 1);
    CHECK(inserted.itemCount == ItemCount + 1);
    CHECK(inserted.expandableItemCount == 1);
    CHECK(inserted.fullRefreshCount == 1);
    CHECK(inserted.nodeLookupCount == 0);
    CHECK(inserted.peerUpdateCount <= 2 * ItemCount + 4);
    CHECK(inserted.modelGrowthCount < 32);

    int siblingCount = 0;
    const auto iterationStarted = std::chrono::steady_clock::now();
    wxTreeItemIdValue cookie = nullptr;
    for ( wxTreeItemId current = tree.GetFirstChild(root, cookie);
          current.IsOk();
          current = tree.GetNextSibling(current) )
    {
        ++siblingCount;
    }
    const auto iterationElapsed = std::chrono::duration_cast<
        std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - iterationStarted);
    INFO("10k sibling iteration time: "
         << iterationElapsed.count() << " ms");
    CHECK(iterationElapsed < std::chrono::seconds(5));
    CHECK(siblingCount == ItemCount);

    tree.WinUIResetModelStatsForTesting();
    const std::uintptr_t lastPeer =
        tree.WinUIGetItemPeerIdentityForTesting(last);
    tree.SetItemText(last, "last renamed");
    const wxTreeCtrl::WinUIModelStats renamed =
        tree.WinUIGetModelStatsForTesting();
    CHECK(renamed.peerUpdateCount == 1);
    CHECK(renamed.fullRefreshCount == 0);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(last) == lastPeer);

    tree.Delete(last);
    CHECK(tree.GetCount() == ItemCount);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(last) == 0);

    tree.WinUIResetModelStatsForTesting();
    const auto subtreeStarted = std::chrono::steady_clock::now();
    tree.DeleteChildren(root);
    const auto subtreeElapsed = std::chrono::duration_cast<
        std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - subtreeStarted);
    INFO("10k subtree deletion time: "
         << subtreeElapsed.count() << " ms");
    CHECK(subtreeElapsed < std::chrono::seconds(30));
    const wxTreeCtrl::WinUIModelStats subtree =
        tree.WinUIGetModelStatsForTesting();
    CHECK(tree.GetCount() == 1);
    CHECK(subtree.itemCount == 1);
    CHECK(subtree.expandableItemCount == 0);
    CHECK(subtree.fullRefreshCount == 1);
    CHECK(subtree.nodeLookupCount == 0);
    CHECK(subtree.peerUpdateCount <= 3);
}

TEST_CASE("wxWinUI TreeCtrl uses realized geometry and image parts",
          "[treectrl][winui-treemodel][geometry]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxPoint(0, 0), wxSize(420, 240));
    wxImageList * const images = new wxImageList(16, 16);
    images->Add(MakeTreeBitmap(*wxBLUE));
    tree.AssignImageList(images);
    wxImageList * const states = new wxImageList(16, 16);
    states->Add(MakeTreeBitmap(*wxRED));
    tree.AssignStateImageList(states);

    const wxTreeItemId root = tree.AddRoot("measured root", 0);
    tree.SetItemState(root, 0);
    const wxTreeItemId child = tree.AppendItem(root, "child");
    tree.Expand(root);
    tree.EnsureVisible(root);
    tree.Show();
    tree.Update();
    DrainTreeDispatch(6);
    CHECK(tree.IsVisible(root));
    CHECK(tree.IsVisible(child));

    wxTreeCtrl::WinUIMeasuredItemParts parts;
    REQUIRE(tree.WinUIGetMeasuredItemPartsForTesting(root, &parts));
    CHECK_FALSE(parts.item.IsEmpty());
    CHECK_FALSE(parts.expander.IsEmpty());
    CHECK_FALSE(parts.stateImage.IsEmpty());
    CHECK_FALSE(parts.image.IsEmpty());
    CHECK_FALSE(parts.label.IsEmpty());

    wxRect itemRect;
    wxRect labelRect;
    REQUIRE(tree.GetBoundingRect(root, itemRect, false));
    REQUIRE(tree.GetBoundingRect(root, labelRect, true));
    CHECK(itemRect == parts.item);
    CHECK(labelRect == parts.label);

    int flags = 0;
    CHECK(tree.HitTest(RectCentre(parts.expander), flags) == root);
    CHECK(flags == wxTREE_HITTEST_ONITEMBUTTON);
    CHECK(tree.HitTest(RectCentre(parts.stateImage), flags) == root);
    CHECK(flags == wxTREE_HITTEST_ONITEMSTATEICON);
    CHECK(tree.HitTest(RectCentre(parts.image), flags) == root);
    CHECK(flags == wxTREE_HITTEST_ONITEMICON);
    CHECK(tree.HitTest(RectCentre(parts.label), flags) == root);
    CHECK(flags == wxTREE_HITTEST_ONITEMLABEL);

    int stateImageClicks = 0;
    tree.Bind(
        wxEVT_TREE_STATE_IMAGE_CLICK,
        [&](wxTreeEvent& event)
        {
            ++stateImageClicks;
            CHECK(event.GetItem() == root);
        });
    const wxPoint statePoint = RectCentre(parts.stateImage);
    tree.WinUIPointerPressedForTesting(statePoint);
    tree.WinUIPointerMovedForTesting(
        statePoint +
        wxPoint(::GetSystemMetrics(SM_CXDRAG) + 2,
                ::GetSystemMetrics(SM_CYDRAG) + 2));
    tree.WinUIPointerReleasedForTesting(statePoint);
    CHECK(stateImageClicks == 0);
    tree.WinUIPointerPressedForTesting(statePoint);
    tree.WinUIPointerReleasedForTesting(statePoint);
    CHECK(stateImageClicks == 1);

    if ( parts.item.GetRight() > parts.label.GetRight() + 2 )
    {
        const wxPoint right(
            parts.label.GetRight() + 2,
            parts.label.y + parts.label.height / 2);
        CHECK(tree.HitTest(right, flags) == root);
        CHECK(flags == wxTREE_HITTEST_ONITEMRIGHT);
    }

    tree.SetItemDropHighlight(root, true);
    DrainTreeDispatch();
    CHECK(tree.WinUIIsPeerDropHighlightedForTesting(root));
    tree.SetItemDropHighlight(root, false);
    DrainTreeDispatch();
    CHECK_FALSE(tree.WinUIIsPeerDropHighlightedForTesting(root));

    tree.SetItemText(root, "renamed for UIA");
    DrainTreeDispatch();
    CHECK(
        tree.WinUIGetPeerAutomationNameForTesting(root) ==
        "renamed for UIA");

    const std::uintptr_t rootPeer =
        tree.WinUIGetItemPeerIdentityForTesting(root);
    REQUIRE(child.IsOk());
    const std::uintptr_t childPeer =
        tree.WinUIGetItemPeerIdentityForTesting(child);
    REQUIRE(childPeer != 0);

    tree.SetIndent(28);
    DrainTreeDispatch(4);
    double rootLeading = -1.0;
    double rootTrailing = -1.0;
    double childLeading = -1.0;
    double childTrailing = -1.0;
    REQUIRE(tree.WinUIGetPeerIndentForTesting(
        root, &rootLeading, &rootTrailing));
    REQUIRE(tree.WinUIGetPeerIndentForTesting(
        child, &childLeading, &childTrailing));
    CHECK(rootLeading == Approx(0.0));
    CHECK(rootTrailing == Approx(0.0));
    CHECK(childLeading == Approx(28.0));
    CHECK(childTrailing == Approx(0.0));

    tree.SetLayoutDirection(wxLayout_RightToLeft);
    DrainTreeDispatch(4);
    REQUIRE(tree.WinUIGetPeerIndentForTesting(
        child, &childLeading, &childTrailing));
    CHECK(childLeading == Approx(28.0));
    CHECK(childTrailing == Approx(0.0));
    wxTreeCtrl::WinUIMeasuredItemParts rtlParts;
    REQUIRE(tree.WinUIGetMeasuredItemPartsForTesting(child, &rtlParts));
    REQUIRE(rtlParts.item.x + 1 < rtlParts.label.x);
    REQUIRE(rtlParts.item.GetRight() >
            rtlParts.label.GetRight() + 1);
    CHECK(tree.HitTest(
              wxPoint(rtlParts.item.x + 1,
                      rtlParts.item.y + rtlParts.item.height / 2),
              flags) == child);
    CHECK(flags == wxTREE_HITTEST_ONITEMRIGHT);
    CHECK(tree.HitTest(
              wxPoint(rtlParts.item.GetRight() - 1,
                      rtlParts.item.y + rtlParts.item.height / 2),
              flags) == child);
    CHECK(flags == wxTREE_HITTEST_ONITEMINDENT);

    const long visibleRootStyle = tree.GetWindowStyleFlag();
    tree.SetWindowStyleFlag(visibleRootStyle | wxTR_HIDE_ROOT);
    DrainTreeDispatch(4);
    CHECK(tree.GetCount() >= 1);
    CHECK_FALSE(tree.IsVisible(root));
    REQUIRE(tree.WinUIGetPeerIndentForTesting(
        child, &childLeading, &childTrailing));
    CHECK(childLeading == Approx(0.0));
    tree.SetWindowStyleFlag(visibleRootStyle);
    DrainTreeDispatch(4);
    CHECK(tree.IsExpanded(root));
    REQUIRE(tree.WinUIGetPeerIndentForTesting(
        child, &childLeading, &childTrailing));
    CHECK(childLeading == Approx(28.0));

    tree.SelectItem(root);
    tree.WinUIResetModelStatsForTesting();
    wxDPIChangedEvent dpiEvent(wxSize(96, 96), wxSize(144, 144));
    dpiEvent.SetEventObject(&tree);
    dpiEvent.SetId(tree.GetId());
    tree.ProcessWindowEvent(dpiEvent);
    DrainTreeDispatch();
    const wxTreeCtrl::WinUIModelStats dpiRefresh =
        tree.WinUIGetModelStatsForTesting();
    CHECK(dpiRefresh.peerUpdateCount == tree.GetCount());
    CHECK(dpiRefresh.fullRefreshCount == 1);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) == rootPeer);
    CHECK(tree.WinUIGetItemPeerIdentityForTesting(child) == childPeer);
    CHECK(tree.IsExpanded(root));
    CHECK(tree.IsSelected(root));
    wxTreeCtrl::WinUIMeasuredItemParts dpiParts;
    REQUIRE(tree.WinUIGetMeasuredItemPartsForTesting(root, &dpiParts));
    CHECK_FALSE(dpiParts.stateImage.IsEmpty());
    CHECK_FALSE(dpiParts.image.IsEmpty());

    wxTreeItemId virtualized;
    for ( int i = 0; i < 500; ++i )
        virtualized = tree.AppendItem(root, wxString::Format("row %d", i));
    DrainTreeDispatch();
    wxRect unavailable;
    CHECK_FALSE(tree.GetBoundingRect(virtualized, unavailable, true));
    CHECK_FALSE(tree.IsVisible(virtualized));
    tree.ScrollTo(virtualized);
    DrainTreeDispatch(8);
    CHECK(tree.IsVisible(virtualized));
    CHECK(tree.GetFirstVisibleItem().IsOk());
}

TEST_CASE("wxWinUI TreeCtrl image PropertySets follow a full DPI cycle",
          "[treectrl][winui-treemodel][images][dpi][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxBitmapBundle normal =
        MakeTreeDPIBundle(wxSize(16, 16), *wxBLUE);
    const wxBitmapBundle state =
        MakeTreeDPIBundle(wxSize(12, 12), *wxRED);
    wxWithImages::Images images;
    images.push_back(normal);
    wxVector<wxBitmapBundle> stateImages;
    stateImages.push_back(state);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxDefaultPosition, wxSize(340, 220));
    tree.SetImages(images);
    tree.SetStateImages(stateImages);
    const wxTreeItemId root = tree.AddRoot("root", 0);
    tree.SetItemState(root, 0);
    const wxTreeItemId child =
        tree.AppendItem(root, "selected child", 0);
    tree.SetItemState(child, 0);
    tree.Expand(root);
    tree.SelectItem(child);
    REQUIRE(tree.GetSelection() == child);
    REQUIRE(tree.IsExpanded(root));

    const std::uintptr_t rootIdentity =
        tree.WinUIGetItemPeerIdentityForTesting(root);
    const std::uintptr_t childIdentity =
        tree.WinUIGetItemPeerIdentityForTesting(child);
    REQUIRE(rootIdentity != 0);
    REQUIRE(childIdentity != 0);

    constexpr std::array<double, 4> Scales{
        1.0, 1.5, 2.0, 1.0
    };
    std::uint64_t lastGeneration = 0;
    for ( const double scale : Scales )
    {
        CAPTURE(scale);
        REQUIRE(tree.WinUIRefreshForScaleForTesting(scale));

        wxSize imagePixels;
        wxSize statePixels;
        wxSize imageDIPs;
        wxSize stateDIPs;
        std::uint64_t generation = 0;
        REQUIRE(tree.WinUIGetPeerItemImageProjectionForTesting(
            root, &imagePixels, &statePixels, &generation,
            &imageDIPs, &stateDIPs));
        CHECK(imagePixels ==
              normal.GetPreferredBitmapSizeAtScale(scale));
        CHECK(statePixels ==
              state.GetPreferredBitmapSizeAtScale(scale));
        CHECK(imageDIPs == wxSize(16, 16));
        CHECK(stateDIPs == wxSize(12, 12));
        CHECK(generation > lastGeneration);

        wxSize childImagePixels;
        wxSize childStatePixels;
        std::uint64_t childGeneration = 0;
        REQUIRE(tree.WinUIGetPeerItemImageProjectionForTesting(
            child, &childImagePixels, &childStatePixels,
            &childGeneration));
        CHECK(childImagePixels == imagePixels);
        CHECK(childStatePixels == statePixels);
        CHECK(childGeneration == generation);

        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) ==
              rootIdentity);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(child) ==
              childIdentity);
        CHECK(tree.GetSelection() == child);
        CHECK(tree.IsSelected(child));
        CHECK(tree.IsExpanded(root));
        lastGeneration = generation;
    }
}

TEST_CASE("wxWinUI TreeCtrl keeps state images set before Create",
          "[treectrl][winui-treemodel][images][default-create][winui-009]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    const wxBitmapBundle state =
        MakeTreeDPIBundle(wxSize(12, 12), *wxRED);
    wxVector<wxBitmapBundle> stateImages{state};

    wxTreeCtrl tree;
    tree.SetStateImages(stateImages);
    REQUIRE(tree.Create(
        parent, wxID_ANY, wxDefaultPosition, wxSize(340, 220)));
    const wxTreeItemId root = tree.AddRoot("root");
    REQUIRE(root.IsOk());
    tree.SetItemState(root, 0);
    REQUIRE(tree.WinUIRefreshForScaleForTesting(1.5));

    wxSize imagePixels;
    wxSize statePixels;
    wxSize imageDIPs;
    wxSize stateDIPs;
    std::uint64_t generation = 0;
    REQUIRE(tree.WinUIGetPeerItemImageProjectionForTesting(
        root, &imagePixels, &statePixels, &generation,
        &imageDIPs, &stateDIPs));
    CHECK(imagePixels == wxSize());
    CHECK(statePixels ==
          state.GetPreferredBitmapSizeAtScale(1.5));
    CHECK(imageDIPs == wxSize());
    CHECK(stateDIPs == wxSize(12, 12));
    CHECK(generation != 0);
}

TEST_CASE("wxWinUI TreeCtrl deep chains tear down iteratively",
          "[treectrl][winui-treemodel][lifetime][performance][deep]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);
    constexpr int Depth = 10000;

    SECTION("DeleteAllItems")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        const wxTreeItemId root = tree.AddRoot("0");
        REQUIRE(root.IsOk());
        wxTreeItemId current = root;
        wxTreeItemId middle;
        for ( int i = 1; i < Depth; ++i )
        {
            current = tree.AppendItem(
                current, wxString::Format("%d", i));
            REQUIRE(current.IsOk());
            if ( i == Depth / 2 )
                middle = current;
        }
        REQUIRE(middle.IsOk());
        CHECK(tree.GetCount() == Depth);

        // Exercise the full deep rollback as well as the commit. Both peer
        // directions must remain iterative: deepest-first while detaching and
        // shallowest-first while rebuilding the XAML graph.
        unsigned deleteEvents = 0;
        unsigned selectionEvents = 0;
        tree.Bind(
            wxEVT_TREE_DELETE_ITEM,
            [&](wxTreeEvent&) { ++deleteEvents; });
        tree.Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent&) { ++selectionEvents; });
        tree.SelectItem(root);
        REQUIRE(tree.GetSelection() == root);
        deleteEvents = 0;
        selectionEvents = 0;

        const std::uintptr_t rootPeer =
            tree.WinUIGetItemPeerIdentityForTesting(root);
        const std::uintptr_t middlePeer =
            tree.WinUIGetItemPeerIdentityForTesting(middle);
        const std::uintptr_t leafPeer =
            tree.WinUIGetItemPeerIdentityForTesting(current);
        tree.WinUIFailNextPeerMutationForTesting(
            wxTreeCtrl::WinUIPeerMutationForTesting::ClearItems);
        tree.DeleteAllItems();
        CHECK(tree.GetCount() == Depth);
        CHECK(tree.GetRootItem() == root);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) == rootPeer);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(middle) == middlePeer);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(current) == leafPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(root));
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(middle));
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(current));
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(middle) == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(current) == 0);
        CHECK(tree.GetSelection() == root);
        CHECK(deleteEvents == 0);
        CHECK(selectionEvents == 0);

        tree.DeleteAllItems();
        CHECK(tree.GetCount() == 0);
    }

    SECTION("Delete a non-root deep subtree rolls back and commits")
    {
        int destroyed = 0;
        unsigned deleteEvents = 0;
        unsigned selectionEvents = 0;
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        const wxTreeItemId root = tree.AddRoot("root");
        REQUIRE(root.IsOk());
        // A warm shared island projects mutations immediately. Keep this
        // scenario explicit: an unloaded peer can hide recursive work in
        // TreeViewNode until a later control activates the island.
        tree.Show();
        tree.Update();
        DrainTreeDispatch(4);

        const DeepTreeChain chain =
            AppendDeepTreeChain(
                tree, root, Depth, "delete", &destroyed);
        REQUIRE(tree.GetCount() == Depth + 1);

        const std::uintptr_t rootPeer =
            tree.WinUIGetItemPeerIdentityForTesting(root);
        REQUIRE(rootPeer != 0);
        // The transaction only needs a selection inside the doomed subtree.
        // Keep the independent deep-selection/layout stress bounded below.
        tree.SelectItem(chain.items.front());
        REQUIRE(tree.GetSelection() == chain.items.front());

        tree.Bind(
            wxEVT_TREE_DELETE_ITEM,
            [&](wxTreeEvent&) { ++deleteEvents; });
        tree.Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent&) { ++selectionEvents; });

        // This injection occurs after the top peer node was removed. The
        // rollback must therefore rebuild every severed edge shallow-first
        // without ever recursively releasing the 10k-node COM graph.
        tree.WinUIFailNextPeerMutationForTesting(
            wxTreeCtrl::WinUIPeerMutationForTesting::
                RemoveItemAfterCommit);
        tree.Delete(chain.items.front());

        CHECK(tree.GetCount() == Depth + 1);
        CHECK(tree.GetRootItem() == root);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) ==
              rootPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(root));
        CHECK(tree.WinUIGetPeerChildCountForTesting() == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 1);
        CHECK(tree.GetSelection() == chain.items.front());
        CHECK(destroyed == 0);
        CHECK(deleteEvents == 0);
        CHECK(selectionEvents == 0);
        CheckDeepTreeChainIntegrity(tree, root, chain);

        tree.Delete(chain.items.front());
        CHECK(tree.GetCount() == 1);
        CHECK(tree.GetRootItem() == root);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) ==
              rootPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(root));
        CHECK(tree.WinUIGetPeerChildCountForTesting() == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 0);
        CHECK(tree.GetSelection() == root);
        CHECK(destroyed == Depth);
        CHECK(deleteEvents == Depth);
        CHECK(selectionEvents == 1);
        for ( size_t i = 0; i < chain.items.size(); ++i )
        {
            CAPTURE(i);
            CHECK(tree.WinUIGetItemPeerIdentityForTesting(
                      chain.items[i]) == 0);
        }
    }

    SECTION("A loaded deep selection preserves wx event order")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        const wxTreeItemId root = tree.AddRoot("root");
        REQUIRE(root.IsOk());
        tree.SelectItem(root);
        REQUIRE(tree.GetSelection() == root);

        const DeepTreeChain chain =
            AppendDeepTreeChain(tree, root, Depth, "selection");
        REQUIRE(chain.items.size() == Depth);
        tree.Show();
        tree.Update();
        DrainTreeDispatch(4);

        size_t expanding = 0;
        size_t expanded = 0;
        bool orderIsGeneric = true;
        bool eventsArePaired = true;
        bool oldSelectionIsStable = true;
        const auto expectedAncestor =
            [&](size_t index)
            {
                return index + 1 == static_cast<size_t>(Depth)
                    ? root
                    : chain.items[
                          static_cast<size_t>(Depth) - 2 - index];
            };
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDING,
            [&](wxTreeEvent& event)
            {
                if ( expanding >= static_cast<size_t>(Depth) ||
                     event.GetItem() != expectedAncestor(expanding) )
                {
                    orderIsGeneric = false;
                }
                eventsArePaired = eventsArePaired &&
                                  expanded == expanding;
                oldSelectionIsStable =
                    oldSelectionIsStable &&
                    tree.GetSelection() == root &&
                    tree.IsSelected(root);
                ++expanding;
            });
        tree.Bind(
            wxEVT_TREE_ITEM_EXPANDED,
            [&](wxTreeEvent& event)
            {
                if ( expanded >= static_cast<size_t>(Depth) ||
                     event.GetItem() != expectedAncestor(expanded) )
                {
                    orderIsGeneric = false;
                }
                eventsArePaired = eventsArePaired &&
                                  expanding == expanded + 1;
                oldSelectionIsStable =
                    oldSelectionIsStable &&
                    tree.GetSelection() == root &&
                    tree.IsSelected(root);
                ++expanded;
            });

        tree.WinUIResetModelStatsForTesting();
        const auto started = std::chrono::steady_clock::now();
        tree.SelectItem(chain.items.back());
        const auto elapsed = std::chrono::duration_cast<
            std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - started);
        INFO("loaded 10k selection time: " << elapsed.count() << " ms");

        CHECK(elapsed < std::chrono::seconds(60));
        CHECK(tree.GetSelection() == chain.items.back());
        CHECK_FALSE(tree.IsSelected(root));
        CHECK(expanding == static_cast<size_t>(Depth));
        CHECK(expanded == static_cast<size_t>(Depth));
        CHECK(orderIsGeneric);
        CHECK(eventsArePaired);
        CHECK(oldSelectionIsStable);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.IsExpanded(chain.items[Depth - 2]));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(
            chain.items[Depth - 2]));

        const wxTreeCtrl::WinUIModelStats stats =
            tree.WinUIGetModelStatsForTesting();
        CHECK(stats.nodeLookupCount == 0);
        CHECK(stats.peerUpdateCount <= 3);
        CHECK(stats.fullRefreshCount == 0);
    }

    SECTION("DeleteChildren rolls back and commits a deep chain")
    {
        int destroyed = 0;
        unsigned deleteEvents = 0;
        unsigned selectionEvents = 0;
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        const wxTreeItemId root = tree.AddRoot("root");
        REQUIRE(root.IsOk());

        const DeepTreeChain chain =
            AppendDeepTreeChain(
                tree, root, Depth, "children", &destroyed);
        REQUIRE(tree.GetCount() == Depth + 1);

        const std::uintptr_t rootPeer =
            tree.WinUIGetItemPeerIdentityForTesting(root);
        REQUIRE(rootPeer != 0);
        tree.SelectItem(chain.items.front());
        REQUIRE(tree.GetSelection() == chain.items.front());

        tree.Bind(
            wxEVT_TREE_DELETE_ITEM,
            [&](wxTreeEvent&) { ++deleteEvents; });
        tree.Bind(
            wxEVT_TREE_SEL_CHANGED,
            [&](wxTreeEvent&) { ++selectionEvents; });

        tree.WinUIFailNextPeerMutationForTesting(
            wxTreeCtrl::WinUIPeerMutationForTesting::ClearItems);
        tree.DeleteChildren(root);

        CHECK(tree.GetCount() == Depth + 1);
        CHECK(tree.GetRootItem() == root);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) ==
              rootPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(root));
        CHECK(tree.WinUIGetPeerChildCountForTesting() == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 1);
        CHECK(tree.GetSelection() == chain.items.front());
        CHECK(destroyed == 0);
        CHECK(deleteEvents == 0);
        CHECK(selectionEvents == 0);
        CheckDeepTreeChainIntegrity(tree, root, chain);

        tree.DeleteChildren(root);
        CHECK(tree.GetCount() == 1);
        CHECK(tree.GetRootItem() == root);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) ==
              rootPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(root));
        CHECK(tree.WinUIGetPeerChildCountForTesting() == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 0);
        CHECK(tree.GetSelection() == root);
        CHECK(destroyed == Depth);
        CHECK(deleteEvents == Depth);
        CHECK(selectionEvents == 1);
        for ( size_t i = 0; i < chain.items.size(); ++i )
        {
            CAPTURE(i);
            CHECK(tree.WinUIGetItemPeerIdentityForTesting(
                      chain.items[i]) == 0);
        }
    }

    SECTION("SortChildren preserves a deep branch on rollback and commit")
    {
        SortingTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        const wxTreeItemId root = tree.AddRoot("root");
        REQUIRE(root.IsOk());

        const DeepTreeChain deep =
            AppendDeepTreeChain(tree, root, Depth, "z-deep");
        const wxTreeItemId shallow =
            tree.AppendItem(root, "a-shallow");
        REQUIRE(shallow.IsOk());
        const std::uintptr_t shallowPeer =
            tree.WinUIGetItemPeerIdentityForTesting(shallow);
        REQUIRE(shallowPeer != 0);
        REQUIRE(tree.GetCount() == Depth + 2);
        REQUIRE(tree.WinUIGetPeerChildCountForTesting(root) == 2);
        REQUIRE(tree.WinUIIsItemAttachedToPeerForTesting(shallow));

        tree.SelectItem(deep.items.front());
        REQUIRE(tree.GetSelection() == deep.items.front());

        // ClearItems is injected after the sorted peer order was installed.
        // Recovery must restore both sibling order and every edge below the
        // deep branch while keeping all TreeViewNode identities stable.
        tree.WinUIFailNextPeerMutationForTesting(
            wxTreeCtrl::WinUIPeerMutationForTesting::ClearItems);
        tree.SortChildren(root);

        wxTreeItemIdValue cookie = nullptr;
        CHECK(tree.GetFirstChild(root, cookie) ==
              deep.items.front());
        CHECK(tree.GetNextSibling(deep.items.front()) == shallow);
        CHECK_FALSE(tree.GetNextSibling(shallow).IsOk());
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(shallow) ==
              shallowPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(shallow));
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 2);
        CHECK(tree.GetSelection() == deep.items.front());
        CheckDeepTreeChainIntegrity(tree, root, deep);

        tree.SortChildren(root);
        cookie = nullptr;
        CHECK(tree.GetFirstChild(root, cookie) == shallow);
        CHECK(tree.GetNextSibling(shallow) == deep.items.front());
        CHECK_FALSE(
            tree.GetNextSibling(deep.items.front()).IsOk());
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(shallow) ==
              shallowPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(shallow));
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 2);
        CHECK(tree.GetSelection() == deep.items.front());
        CheckDeepTreeChainIntegrity(tree, root, deep);
    }

    SECTION("wxTR_HIDE_ROOT moves a deep chain in both directions")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120),
            wxTR_HAS_BUTTONS);
        const wxTreeItemId root = tree.AddRoot("root");
        REQUIRE(root.IsOk());
        const DeepTreeChain chain =
            AppendDeepTreeChain(tree, root, Depth, "style");
        REQUIRE(tree.GetCount() == Depth + 1);

        const std::uintptr_t rootPeer =
            tree.WinUIGetItemPeerIdentityForTesting(root);
        REQUIRE(rootPeer != 0);
        tree.Expand(root);
        REQUIRE(tree.IsExpanded(root));
        REQUIRE(tree.WinUIIsPeerExpandedForTesting(root));
        tree.SelectItem(chain.items.front());
        REQUIRE(tree.GetSelection() == chain.items.front());

        const long visibleRootStyle = tree.GetWindowStyleFlag();
        tree.SetWindowStyleFlag(
            visibleRootStyle | wxTR_HIDE_ROOT);
        CHECK((tree.GetWindowStyleFlag() & wxTR_HIDE_ROOT) != 0);
        // Match wxGenericTreeCtrl::GetCount(): the invisible anchor is not an
        // item reported by the public count.
        CHECK(tree.GetCount() == Depth);
        CHECK(tree.GetRootItem() == root);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) ==
              rootPeer);
        CHECK_FALSE(
            tree.WinUIIsItemAttachedToPeerForTesting(root));
        CHECK(tree.WinUIGetPeerChildCountForTesting() == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 1);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK(tree.GetSelection() == chain.items.front());
        CheckDeepTreeChainIntegrity(tree, root, chain);

        tree.SetWindowStyleFlag(visibleRootStyle);
        CHECK((tree.GetWindowStyleFlag() & wxTR_HIDE_ROOT) == 0);
        CHECK(tree.GetCount() == Depth + 1);
        CHECK(tree.GetRootItem() == root);
        CHECK(tree.WinUIGetItemPeerIdentityForTesting(root) ==
              rootPeer);
        CHECK(tree.WinUIIsItemAttachedToPeerForTesting(root));
        CHECK(tree.WinUIGetPeerChildCountForTesting() == 1);
        CHECK(tree.WinUIGetPeerChildCountForTesting(root) == 1);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
        CHECK(tree.GetSelection() == chain.items.front());
        CheckDeepTreeChainIntegrity(tree, root, chain);
    }

    SECTION("An initially hidden root remains an expanded anchor")
    {
        wxTreeCtrl tree(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120),
            wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);
        const wxTreeItemId root = tree.AddRoot("hidden root");
        REQUIRE(root.IsOk());
        CHECK(tree.GetCount() == 0);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));

        const wxTreeItemId child = tree.AppendItem(root, "child");
        REQUIRE(child.IsOk());
        CHECK(tree.GetCount() == 1);
        tree.Delete(child);
        CHECK(tree.GetCount() == 0);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));

        REQUIRE(tree.AppendItem(root, "replacement").IsOk());
        tree.DeleteChildren(root);
        CHECK(tree.GetCount() == 0);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));

        tree.SetWindowStyleFlag(
            tree.GetWindowStyleFlag() & ~wxTR_HIDE_ROOT);
        CHECK(tree.GetCount() == 1);
        CHECK(tree.IsExpanded(root));
        CHECK(tree.WinUIIsPeerExpandedForTesting(root));
    }

    SECTION("A virtualized teardown cannot poison a deep successor")
    {
        {
            wxTreeCtrl virtualizedTree(
                parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
            const wxTreeItemId root =
                virtualizedTree.AddRoot("virtualized root");
            REQUIRE(root.IsOk());

            wxTreeItemId last;
            for ( int i = 0; i < 500; ++i )
            {
                last = virtualizedTree.AppendItem(
                    root, wxString::Format("row %d", i));
                REQUIRE(last.IsOk());
            }
            virtualizedTree.Expand(root);
            virtualizedTree.Show();
            virtualizedTree.Update();
            DrainTreeDispatch(4);
            virtualizedTree.ScrollTo(last);
            DrainTreeDispatch(8);
            REQUIRE(virtualizedTree.IsVisible(last));
        }

        // Do not yield between the two controls: Close() itself must leave no
        // populated TreeView cleanup for the next host render to inherit.
        wxTreeCtrl successor(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        const wxTreeItemId root = successor.AddRoot("successor root");
        REQUIRE(root.IsOk());
        const DeepTreeChain chain =
            AppendDeepTreeChain(successor, root, Depth, "successor");
        CHECK(successor.GetCount() == Depth + 1);
        CheckDeepTreeChainIntegrity(successor, root, chain);
        successor.DeleteAllItems();
        CHECK(successor.GetCount() == 0);
    }

    SECTION("Close and destructor")
    {
        const size_t baseline =
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();
        wxTreeCtrl *tree = new wxTreeCtrl(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        wxTreeItemId current = tree->AddRoot("0");
        REQUIRE(current.IsOk());
        for ( int i = 1; i < Depth; ++i )
        {
            current = tree->AppendItem(
                current, wxString::Format("%d", i));
            REQUIRE(current.IsOk());
        }
        tree->WinUIClosePeerForTesting();
        delete tree;
        CHECK(
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline);
    }

    SECTION("Close remains iterative when a peer edge cannot be cleared")
    {
        const size_t baseline =
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting();
        wxTreeCtrl *tree = new wxTreeCtrl(
            parent, wxID_ANY, wxDefaultPosition, wxSize(220, 120));
        wxTreeItemId current = tree->AddRoot("0");
        REQUIRE(current.IsOk());
        for ( int i = 1; i < Depth; ++i )
        {
            current = tree->AppendItem(
                current, wxString::Format("%d", i));
            REQUIRE(current.IsOk());
        }
        tree->WinUIFailNextPeerMutationForTesting(
            wxTreeCtrl::WinUIPeerMutationForTesting::ClearItems);
        tree->WinUIClosePeerForTesting();
        delete tree;
        CHECK(
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting() ==
            baseline);
    }
}

TEST_CASE("wxWinUI TreeCtrl edit sessions are transactional",
          "[treectrl][winui-treemodel][edit]")
{
    wxWindow * const parent = wxTheApp->GetTopWindow();
    REQUIRE(parent);

    wxTreeCtrl tree(
        parent, wxID_ANY, wxPoint(0, 0), wxSize(320, 180),
        wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS);
    const wxTreeItemId root = tree.AddRoot("old");
    tree.AppendItem(root, "child");
    tree.Expand(root);
    tree.EnsureVisible(root);
    tree.Show();
    DrainTreeDispatch(6);

    bool vetoBegin = true;
    int beginEvents = 0;
    bool vetoEnd = false;
    bool deleteAtEnd = false;
    bool overrideAtEnd = false;
    tree.Bind(
        wxEVT_TREE_BEGIN_LABEL_EDIT,
        [&](wxTreeEvent& event)
        {
            ++beginEvents;
            if ( vetoBegin )
                event.Veto();
        });
    tree.Bind(
        wxEVT_TREE_END_LABEL_EDIT,
        [&](wxTreeEvent& event)
        {
            if ( vetoEnd )
                event.Veto();
            if ( overrideAtEnd )
                tree.SetItemText(event.GetItem(), "handler value");
            if ( deleteAtEnd )
                tree.Delete(event.GetItem());
        });

    CHECK(tree.EditLabel(root) == nullptr);
    CHECK(beginEvents == 1);
    tree.SelectItem(root);
    CHECK_FALSE(tree.WinUIKeyDownForTesting(WXK_F2));
    CHECK(beginEvents == 2);
    vetoBegin = false;

    int activated = 0;
    tree.Bind(
        wxEVT_TREE_ITEM_ACTIVATED,
        [&](wxTreeEvent& event)
        {
            ++activated;
            CHECK(event.GetItem() == root);
        });
    REQUIRE(tree.WinUIInvokeItemForTesting(root));
    CHECK(activated == 0);
    REQUIRE(tree.WinUIDoubleClickItemForTesting(root));
    CHECK(activated == 1);
    REQUIRE(tree.WinUIKeyDownForTesting(WXK_RETURN));
    CHECK(activated == 2);

    REQUIRE(tree.WinUIScheduleLabelEditForTesting(root));
    REQUIRE(tree.WinUIFireLabelEditDelayForTesting());
    REQUIRE(tree.GetEditControl());
    tree.EndEditLabel(root, true);
    DrainTreeDispatch();

    REQUIRE(tree.WinUIKeyDownForTesting(WXK_F2));
    REQUIRE(tree.GetEditControl());
    tree.EndEditLabel(root, true);
    DrainTreeDispatch();

    wxTextCtrl *editor = tree.EditLabel(root);
    REQUIRE(editor);
    editor->ChangeValue("committed");
    tree.EndEditLabel(root);
    CHECK(tree.GetItemText(root) == "committed");
    DrainTreeDispatch();

    overrideAtEnd = true;
    editor = tree.EditLabel(root);
    REQUIRE(editor);
    editor->ChangeValue("editor value");
    tree.EndEditLabel(root);
    CHECK(tree.GetItemText(root) == "handler value");
    DrainTreeDispatch();
    overrideAtEnd = false;

    vetoEnd = true;
    editor = tree.EditLabel(root);
    REQUIRE(editor);
    editor->ChangeValue("rejected");
    tree.EndEditLabel(root);
    CHECK(tree.GetItemText(root) == "handler value");
    DrainTreeDispatch();

    vetoEnd = false;
    deleteAtEnd = true;
    editor = tree.EditLabel(root);
    REQUIRE(editor);
    editor->ChangeValue("must not touch a retired item");
    tree.EndEditLabel(root);
    CHECK(tree.GetCount() == 0);
    DrainTreeDispatch();

    wxTreeCtrl queued(
        parent, wxID_ANY, wxPoint(0, 0), wxSize(320, 180),
        wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS);
    const wxTreeItemId queuedRoot = queued.AddRoot("queued");
    queued.AppendItem(queuedRoot, "child");
    queued.Expand(queuedRoot);
    queued.EnsureVisible(queuedRoot);
    DrainTreeDispatch(6);
    wxTextCtrl * const queuedEditor = queued.EditLabel(queuedRoot);
    REQUIRE(queuedEditor);
    int lateEndEvents = 0;
    queued.Bind(
        wxEVT_TREE_END_LABEL_EDIT,
        [&](wxTreeEvent&)
        {
            ++lateEndEvents;
        });
    queuedEditor->ChangeValue("late");
    wxCommandEvent * const enter =
        new wxCommandEvent(wxEVT_TEXT_ENTER, queuedEditor->GetId());
    enter->SetEventObject(queuedEditor);
    wxQueueEvent(queuedEditor, enter);
    queued.WinUIClosePeerForTesting();
    DrainTreeDispatch();
    CHECK(lateEndEvents == 0);
    CHECK(queued.GetCount() == 0);
}

TEST_CASE("wxWinUI TreeCtrl internal drag is isolated from OLE",
          "[treectrl][winui-treemodel][drag]")
{
    wxFrame firstOwner(
        nullptr, wxID_ANY, "tree drag owner one",
        wxPoint(-24000, -24000), wxSize(360, 220));
    wxFrame secondOwner(
        nullptr, wxID_ANY, "tree drag owner two",
        wxPoint(-23500, -24000), wxSize(360, 220));
    wxTreeCtrl first(
        &firstOwner, wxID_ANY, wxDefaultPosition, wxSize(240, 140));
    wxTreeCtrl second(
        &secondOwner, wxID_ANY, wxDefaultPosition, wxSize(240, 140));
    const wxTreeItemId firstRoot = first.AddRoot("first root");
    const wxTreeItemId firstChild =
        first.AppendItem(firstRoot, "first child");
    const wxTreeItemId secondRoot = second.AddRoot("second root");
    const wxTreeItemId secondChild =
        second.AppendItem(secondRoot, "second child");

    int firstBegin = 0;
    int firstEnd = 0;
    int secondBegin = 0;
    int secondEnd = 0;
    first.Bind(
        wxEVT_TREE_BEGIN_DRAG,
        [&](wxTreeEvent& event)
        {
            ++firstBegin;
            event.Allow();
        });
    first.Bind(
        wxEVT_TREE_END_DRAG,
        [&](wxTreeEvent& event)
        {
            ++firstEnd;
            CHECK(event.GetItem() == firstChild);
        });
    second.Bind(
        wxEVT_TREE_BEGIN_DRAG,
        [&](wxTreeEvent& event)
        {
            ++secondBegin;
            event.Allow();
        });
    second.Bind(
        wxEVT_TREE_END_DRAG,
        [&](wxTreeEvent&)
        {
            ++secondEnd;
        });

#if wxUSE_DRAG_AND_DROP
    int oleDrops = 0;
    first.SetDropTarget(new CountingTextDropTarget(&oleDrops));
    firstOwner.ShowWithoutActivating();
    secondOwner.ShowWithoutActivating();
    DrainTreeDispatch(6);
    wxWinUITopLevelHost * const initialHost =
        wxWinUITopLevelHost::FindSlotOwner(&first);
    REQUIRE(initialHost);
    initialHost->FlushSync();
    DrainTreeDispatch(4);
#endif

    REQUIRE(first.WinUIBeginInternalDragForTesting(firstChild));
    REQUIRE(first.WinUICompleteInternalDragForTesting(
        firstChild,
        wxTreeCtrl::WinUIDragCompletionForTesting::Cancel));
    CHECK(firstBegin == 1);
    CHECK(firstEnd == 0);

    REQUIRE(first.WinUIBeginInternalDragForTesting(firstChild));
    REQUIRE(first.Reparent(&secondOwner));
#if wxUSE_DRAG_AND_DROP
    DrainTreeDispatch(4);
    wxWinUITopLevelHost * const migratedHost =
        wxWinUITopLevelHost::FindSlotOwner(&first);
    REQUIRE(migratedHost);
    REQUIRE(migratedHost ==
            wxWinUITopLevelHost::ForWindow(&secondOwner, false));
    REQUIRE(migratedHost != initialHost);
    migratedHost->FlushSync();
    DrainTreeDispatch(4);
    wxWinUIDropBroker * const migratedBroker =
        migratedHost->GetDropBrokerForTest();
    REQUIRE(migratedBroker);
    const wxWinUIDropBrokerSnapshot brokerBefore =
        migratedBroker->GetSnapshotForTest();
    CHECK(brokerBefore.active);
#endif
    REQUIRE(first.WinUICompleteInternalDragForTesting(
        firstChild,
        wxTreeCtrl::WinUIDragCompletionForTesting::Drop));
    CHECK(firstBegin == 2);
    CHECK(firstEnd == 1);

    // This drives the same DropResult mapping as the XAML completion
    // callback. With native reordering disabled, None over an exact target is
    // still a completed wx drag, while None without a target is cancellation.
    REQUIRE(first.WinUIBeginInternalDragForTesting(firstChild));
    REQUIRE(first.WinUICompletePeerDragForTesting(firstChild, true));
    CHECK(firstBegin == 3);
    CHECK(firstEnd == 2);
    REQUIRE(first.WinUIBeginInternalDragForTesting(firstChild));
    REQUIRE(first.WinUICompletePeerDragForTesting(wxTreeItemId(), true));
    CHECK(firstBegin == 4);
    CHECK(firstEnd == 2);

    REQUIRE(second.WinUIBeginInternalDragForTesting(secondChild));
    REQUIRE(second.WinUICompleteInternalDragForTesting(
        secondRoot,
        wxTreeCtrl::WinUIDragCompletionForTesting::Drop));
    CHECK(secondBegin == 1);
    CHECK(secondEnd == 1);
    CHECK(firstBegin == 4);
    CHECK(firstEnd == 2);

#if wxUSE_DRAG_AND_DROP
    wxWinUISlot * const migratedSlot = migratedHost->FindSlot(&first);
    REQUIRE(migratedSlot);
    const wxRect slotRect = migratedSlot->GetRectInTLW();
    REQUIRE_FALSE(slotRect.IsEmpty());
    const RECT clientScreen = migratedHost->GetClientScreenRect();
    const POINTL point =
    {
        clientScreen.left + slotRect.x + slotRect.width / 2,
        clientScreen.top + slotRect.y + slotRect.height / 2
    };
    IDropTarget * const comTarget =
        migratedBroker->GetCOMTargetForTest();
    REQUIRE(comTarget);
    wxTextDataObject source("tree-ole-drop");
    const int beginBeforeOle = firstBegin;
    const int endBeforeOle = firstEnd;
    DWORD effect = DROPEFFECT_COPY;
    REQUIRE(comTarget->DragEnter(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    effect = DROPEFFECT_COPY;
    REQUIRE(comTarget->Drop(
        source.GetInterface(), 0, point, &effect) == S_OK);
    CHECK(effect == DROPEFFECT_COPY);
    CHECK(oleDrops == 1);
    CHECK(firstBegin == beginBeforeOle);
    CHECK(firstEnd == endBeforeOle);

    const wxWinUIDropBrokerSnapshot brokerAfter =
        migratedBroker->GetSnapshotForTest();
    CHECK(brokerAfter.active);
    CHECK(brokerAfter.registerCalls == brokerBefore.registerCalls);
    CHECK(brokerAfter.revokeCalls == brokerBefore.revokeCalls);
    CHECK(brokerAfter.lockCalls == brokerBefore.lockCalls);

    REQUIRE(first.WinUIBeginInternalDragForTesting(firstChild));
    REQUIRE(first.WinUICompleteInternalDragForTesting(
        firstChild,
        wxTreeCtrl::WinUIDragCompletionForTesting::Drop));
    CHECK(firstBegin == beginBeforeOle + 1);
    CHECK(firstEnd == endBeforeOle + 1);
    CHECK(oleDrops == 1);
#endif
}

#endif // __WXWINUI__ && wxUSE_WINUI3 && wxUSE_TREECTRL
