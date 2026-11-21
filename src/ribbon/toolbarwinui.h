/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/toolbarwinui.h
// Purpose:     Native projection of the public Ribbon ToolBar model
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Source-private projection, included after the ToolBar model in toolbar.cpp.
// The pre-integration checkpoint remains in toolbarwinui-integration.pending.patch
// as provenance only; the implementation below is the compiled adapter.

#include "ribboncommandwinui.h"

class wxRibbonToolBarWinUI final
    : public std::enable_shared_from_this<wxRibbonToolBarWinUI>
{
public:
    explicit wxRibbonToolBarWinUI(wxRibbonToolBar* owner) : m_owner(owner) {}

    struct Peer
    {
        std::uint64_t key;
        MUXC::Grid root{nullptr};
        std::vector<std::function<void()>> revoke;
        ~Peer()
        {
            for ( auto& handler : revoke )
                try { handler(); }
                catch ( const winrt::hresult_error& e )
                    { wxWinUILogException("ribbon toolbar handler removal", e); }
        }
    };
    struct Item
    {
        std::uint64_t key;
        int id;
        wxRibbonButtonKind kind;
        wxString help;
        wxBitmap bitmap;
        bool enabled;
        bool checked;
        wxRect rect;
        wxRect dropdown;
        long positionFlags = 0;
    };
    struct Group
    {
        std::uint64_t key;
        std::vector<Item> items;
        wxPoint position;
        wxSize size{0, 0};
        bool separator = false;
    };
    struct Projection
    {
        MUXC::Canvas root{nullptr};
        std::vector<Group> groups;
        std::vector<std::shared_ptr<Peer>> peers;
        std::vector<wxSize> sizes;
        wxSize minimum{0, 0};
        wxSize available{0, 0};
        std::uint64_t generation = 0;
        std::uint64_t revision = 0;
    };

    bool Initialize() { return m_host.Initialize(m_owner.get()); }
    bool Attach(const MUX::UIElement& root)
    {
#if defined(WXWINUI_RIBBON_TOOLKIT)
        wxRibbonControl* parent = nullptr;
        MUXC::ContentControl mount{nullptr};
        const auto status = wxRibbonContainerWinUI::CommandMount(Owner(), parent, mount);
        if ( status == wxRibbonContainerWinUI::MountStatus::Ready )
            return m_host.SetContentInParent(root, root, parent, mount);
        if ( status != wxRibbonContainerWinUI::MountStatus::Ordinary ) return false;
#endif
        return m_host.SetContent(root);
    }
    wxRibbonToolBar* Owner() const
    {
        auto* owner = m_owner.get();
        return owner && !owner->IsBeingDeleted() && owner->m_winui.get() == this
            ? owner : nullptr;
    }
    void Close()
    {
        m_owner = nullptr;
        m_active = 0;
        auto retired = std::move(m_projection);
        m_host.Close();
    }
    wxRibbonToolBarToolBase* Find(std::uint64_t key) const
    {
        if ( auto* owner = Owner() )
            for ( auto* group : owner->m_groups )
                for ( auto* tool : group->tools )
                    if ( tool->key == key ) return tool;
        return nullptr;
    }
    wxRibbonToolBarToolBase* FindSeparator(std::uint64_t key) const
    {
        if ( auto* owner = Owner() )
            for ( auto* group : owner->m_groups )
                if ( group->dummy_tool.key == key ) return &group->dummy_tool;
        return nullptr;
    }
    wxRibbonToolBarToolBase* Active() const { return Find(m_active); }
    bool HasRoot() const { return m_projection && m_projection->root; }
    bool IsBuilding() const { return m_building; }
    wxSize Minimum() const { return m_projection ? m_projection->minimum : wxSize(0, 0); }

    static int Extent(const wxSize& size, wxOrientation direction)
    {
        return direction == wxHORIZONTAL ? size.x :
               direction == wxVERTICAL ? size.y : size.x * size.y;
    }
    wxSize BestForParent(const wxSize& available) const
    {
        if ( !m_projection || m_projection->sizes.empty() ) return Minimum();
        wxSize best = m_projection->sizes.front();
        int extent = 0;
        for ( const auto& size : m_projection->sizes )
            if ( size.x <= available.x && size.y <= available.y && size.x > extent )
                { best = size; extent = size.x; }
        return best;
    }
    wxSize NextSize(wxOrientation direction, wxSize relative, bool larger) const
    {
        wxSize result = relative;
        int best = larger ? INT_MAX : 0;
        if ( !m_projection ) return result;
        for ( const wxSize& original : m_projection->sizes )
        {
            wxSize size = original;
            const auto grows = [larger](int a, int b) { return larger ? a > b : a < b; };
            if ( direction == wxHORIZONTAL )
            {
                if ( !grows(size.x, relative.x) || size.y > relative.y ) continue;
                size.y = relative.y;
            }
            else if ( direction == wxVERTICAL )
            {
                if ( size.x > relative.x || !grows(size.y, relative.y) ) continue;
                size.x = relative.x;
            }
            else if ( !grows(size.x, relative.x) || !grows(size.y, relative.y) )
                continue;
            const int extent = Extent(original, direction);
            if ( larger ? extent < best : extent > best )
                { result = size; best = extent; }
        }
        return result;
    }

    std::shared_ptr<Peer> BuildPeer(const Item& item, std::uint64_t generation)
    {
        auto peer = std::make_shared<Peer>();
        peer->key = item.key;
        const wxString name = item.help.empty()
            ? wxString::Format("Tool %d", item.id) : item.help;
        const auto visual = wxRibbonWinUIBuildCommand({item.kind, name,
            item.bitmap, item.enabled, item.checked, 16.0, 32.0, 32.0,
            MUXC::CommandBarLabelPosition::Collapsed});
        peer->root = visual.root;
        const std::weak_ptr<wxRibbonToolBarWinUI> weak = shared_from_this();
        const auto key = item.key;
        if ( auto toggle = visual.primary.try_as<MUXC::AppBarToggleButton>() )
        {
            const auto checked = toggle.Checked([weak, key, generation](const auto&, const auto&)
                { if ( auto self = weak.lock() ) self->Activate(key, generation, false, 1); });
            const auto unchecked = toggle.Unchecked([weak, key, generation](const auto&, const auto&)
                { if ( auto self = weak.lock() ) self->Activate(key, generation, false, 0); });
            peer->revoke.push_back([toggle, checked, unchecked]()
                { toggle.Checked(checked); toggle.Unchecked(unchecked); });
        }
        else if ( auto button = visual.primary.try_as<MUXC::AppBarButton>() )
        {
            const bool dropdown = item.kind == wxRIBBON_BUTTON_DROPDOWN;
            const auto click = button.Click([weak, key, generation, dropdown](const auto&, const auto&)
                { if ( auto self = weak.lock() ) self->Activate(key, generation, dropdown, -1); });
            peer->revoke.push_back([button, click]() { button.Click(click); });
        }
        if ( auto arrow = visual.arrow )
        {
            const auto click = arrow.Click([weak, key, generation](const auto&, const auto&)
                { if ( auto self = weak.lock() ) self->Activate(key, generation, true, -1); });
            peer->revoke.push_back([arrow, click]() { arrow.Click(click); });
        }
#if wxUSE_TOOLTIPS
        if ( !item.help.empty() )
            MUXC::ToolTipService::SetToolTip(peer->root,
                winrt::box_value(wxWinUIToHString(item.help)));
#endif
        return peer;
    }

    void Activate(std::uint64_t key, std::uint64_t generation,
                  bool dropdown, int checked) noexcept
    {
        try
        {
            auto* owner = Owner();
            auto* tool = Find(key);
            if ( m_building || !m_projection ||
                 generation != m_projection->generation ||
                 m_revision != m_projection->revision || !owner ||
                 !owner->IsEnabled() || !tool ||
                 (tool->state & wxRIBBON_TOOLBAR_TOOL_DISABLED) ) return;
            if ( checked >= 0 )
            {
                if ( ((tool->state & wxRIBBON_TOOLBAR_TOOL_TOGGLED) != 0) == (checked != 0) )
                    return;
                if ( checked ) tool->state |= wxRIBBON_TOOLBAR_TOOL_TOGGLED;
                else tool->state &= ~wxRIBBON_TOOLBAR_TOOL_TOGGLED;
            }
            const auto previousActive = m_active;
            m_active = key;
            wxRibbonToolBarEvent event(dropdown ? wxEVT_RIBBONTOOLBAR_DROPDOWN_CLICKED
                                               : wxEVT_RIBBONTOOLBAR_CLICKED,
                                       tool->id, owner);
            event.SetEventObject(owner);
            if ( tool->kind == wxRIBBON_BUTTON_TOGGLE )
                event.SetInt(tool->state & wxRIBBON_TOOLBAR_TOOL_TOGGLED);
            owner->ProcessWindowEvent(event);
            m_active = Find(previousActive) ? previousActive : 0;
            if ( (owner = Owner()) )
                if ( auto* panel = wxDynamicCast(owner->GetParent(), wxRibbonPanel) )
                    panel->HideIfExpanded();
        }
        catch ( const winrt::hresult_error& e )
            { m_active = 0; wxWinUILogException("ribbon toolbar command", e); }
        catch ( ... )
            { m_active = 0; wxLogError("Exception in native ribbon toolbar command handler"); }
    }

    void Changed()
    {
        ++m_revision;
        if ( auto* owner = Owner() ) owner->InvalidateBestSize();
        if ( HasRoot() && !m_building ) (void)Rebuild();
        else if ( m_building ) ScheduleRepair();
    }
    void Resized()
    {
        if ( auto* owner = Owner() )
        {
            const wxSize size = owner->GetClientSize();
            if ( size == m_observedSize ) return;
            m_observedSize = size;
            if ( HasRoot() && !m_building ) (void)Rebuild();
            else if ( m_building ) ScheduleRepair();
        }
    }
    void ScheduleRepair()
    {
        if ( m_pending || m_repairRunning || !wxTheApp || !Owner() ) return;
        m_pending = true;
        const std::weak_ptr<wxRibbonToolBarWinUI> weak = shared_from_this();
        wxTheApp->CallAfter([weak]()
        {
            if ( auto self = weak.lock() )
            {
                self->m_pending = false;
                if ( self->m_building ) { self->m_deferred = true; return; }
                auto* owner = self->Owner();
                if ( !owner || (self->m_projection &&
                     self->m_projection->revision == self->m_revision &&
                     self->m_projection->available == owner->GetClientSize()) ) return;
                self->m_repairRunning = true;
                (void)self->Rebuild();
                self->m_repairRunning = false;
            }
        });
    }

    // Common Ribbon semantics: each group is indivisible; successive groups
    // go into the shortest row. Keep all arithmetic outside native callbacks.
    static wxSize Arrange(std::vector<Group>& groups, int rows, int separation,
                          const wxSize* available = nullptr)
    {
        std::vector<wxSize> sizes(rows, wxSize(0, 0));
        std::vector<int> assignment;
        for ( auto& group : groups )
        {
            const auto shortest = std::min_element(sizes.begin(), sizes.end(),
                [](const wxSize& a, const wxSize& b) { return a.x < b.x; });
            const int row = static_cast<int>(shortest - sizes.begin());
            assignment.push_back(row);
            group.separator = shortest->x != 0 && !group.items.empty();
            group.position = wxPoint(shortest->x + (group.separator ? separation : 0), row);
            if ( !group.items.empty() )
                shortest->x = group.position.x + group.size.x;
            shortest->y = std::max(shortest->y, group.size.y);
        }
        wxSize total(0, 0);
        for ( const auto& size : sizes )
            { total.x = std::max(total.x, size.x); total.y += size.y; }
        const int gap = available ? std::max(0, (available->y - total.y) / (rows + 1)) : 0;
        std::vector<int> y(rows, gap);
        for ( int r = 1; r < rows; ++r ) y[r] = y[r - 1] + sizes[r - 1].y + gap;
        for ( size_t g = 0; g < groups.size(); ++g ) groups[g].position.y = y[assignment[g]];
        return total;
    }

    void ApplyGeometry(const std::shared_ptr<Projection>& projection)
    {
        auto* owner = Owner();
        if ( !owner || !projection ) return;
        for ( const auto& geometry : projection->groups )
        {
            for ( auto* group : owner->m_groups )
                if ( group->dummy_tool.key == geometry.key )
                {
                    group->position = geometry.position;
                    group->size = geometry.size;
                    break;
                }
            for ( const auto& item : geometry.items )
                if ( auto* tool = Find(item.key) )
                {
                    tool->position = item.rect.GetPosition();
                    tool->size = item.rect.GetSize();
                    tool->dropdown = item.dropdown;
                    tool->state = (tool->state & ~wxRIBBON_TOOLBAR_TOOL_POSITION_MASK)
                                | item.positionFlags;
                }
        }
    }

    bool Rebuild()
    {
        if ( m_building ) return false;
        auto self = shared_from_this();
        auto* owner = Owner();
        if ( !owner ) return false;
        bool nativeComposite = false;
        bool pendingMount = false;
#if defined(WXWINUI_RIBBON_TOOLKIT)
        wxRibbonControl* compositeOwner = nullptr;
        MUXC::ContentControl contentMount{nullptr};
        const auto status = wxRibbonContainerWinUI::CommandMount(
            owner, compositeOwner, contentMount);
        pendingMount = status == wxRibbonContainerWinUI::MountStatus::Pending;
        if ( pendingMount && m_host.GetContent() ) return false;
        if ( status == wxRibbonContainerWinUI::MountStatus::Failed ) return false;
        nativeComposite = pendingMount ||
            status == wxRibbonContainerWinUI::MountStatus::Ready;
        if ( !pendingMount && nativeComposite && m_projection &&
             m_projection->revision == m_revision &&
             m_host.GetContent() == m_projection->root )
        {
            // A container Loaded/migration notification must not replace an
            // unchanged command model or invalidate already acquired UIA peers.
            auto* const host = wxWinUITopLevelHost::FindSlotOwner(owner);
            const auto* const slot = host ? host->FindSlot(owner) : nullptr;
            const auto presentation = slot ? slot->GetPresentationRoot()
                                          : MUX::FrameworkElement{nullptr};
            const auto projection = m_projection;
            try
            {
                if ( presentation && slot->GetContent() == projection->root &&
                     contentMount.Content() == presentation && Owner() == owner &&
                     m_projection == projection && projection->revision == m_revision )
                {
                    m_observedSize = projection->available = owner->GetClientSize();
                    return true;
                }
            }
            catch ( const winrt::hresult_error& e )
                { wxWinUILogException("ribbon toolbar mount validation", e); return false; }
        }
#endif
        if ( !nativeComposite && m_projection &&
             m_projection->revision == m_revision &&
             m_projection->available == owner->GetClientSize() &&
             m_host.GetContent() == m_projection->root ) return true;
        m_building = true;
        struct Guard
        {
            wxRibbonToolBarWinUI& self;
            ~Guard()
            {
                self.m_building = false;
                if ( std::exchange(self.m_deferred, false) ) self.ScheduleRepair();
            }
        } guard{*this};
        const auto revision = m_revision;
        const auto current = [&]() { return Owner() && m_revision == revision; };
        const auto previous = m_projection;
        const auto previousHostedRoot = m_host.GetContent();
        const auto candidate = std::make_shared<Projection>();
        candidate->generation = ++m_nextGeneration;
        candidate->revision = revision;
        m_observedSize = owner->GetClientSize();
        bool committed = false;
        bool provisional = false;
        bool attachmentAttempted = false;
        wxScopeGuard rollback = wxMakeGuard([&]()
        {
            if ( committed ) return;
            if ( provisional ) { m_projection = previous; ApplyGeometry(previous); }
            // Failed SetContent already preserves the old host publication.
            if ( attachmentAttempted && Owner() && candidate->root &&
                 m_host.GetContent() == candidate->root )
            {
                try
                {
                    if ( previousHostedRoot )
                    {
                        if ( !Attach(previousHostedRoot) )
                        {
                            wxLogWarning("Could not restore the native ribbon toolbar peer");
                            Close();
                            return;
                        }
                    }
                    else m_host.ClearContent();
                    if ( auto* live = Owner() )
                        { live->InvalidateBestSize(); m_host.ForceRender(); }
                }
                catch ( const winrt::hresult_error& e )
                    { wxWinUILogException("ribbon toolbar rollback", e); Close(); }
            }
            else if ( attachmentAttempted && Owner() && previousHostedRoot &&
                      m_host.GetContent() == previousHostedRoot )
            {
                // A rejected content transaction restores native parentage,
                // but detaching/reinserting that same tree invalidates its
                // arrangement. Flush the surviving publication before a
                // synchronous public geometry query can observe zero offsets.
                // Never touch a newer publication or retry the failed swap.
                m_host.ForceRender();
            }
        });
        wxUnusedVar(rollback);
        try
        {
            const int firstRow = owner->m_nrows_min;
            const int lastRow = owner->m_nrows_max;
            if ( firstRow < 1 || lastRow < firstRow ) return false;
            const int separation = std::max(1, owner->m_art
                ? owner->m_art->GetMetric(wxRIBBON_ART_TOOL_GROUP_SEPARATION_SIZE)
                : owner->FromDIP(8));
            if ( !current() ) return false;
            owner = Owner();
            const auto artFlags = owner->m_art ? owner->m_art->GetFlags() : 0;
            if ( !current() ) return false;
            owner = Owner();
            wxOrientation direction = artFlags & wxRIBBON_BAR_FLOW_VERTICAL
                ? wxVERTICAL : wxHORIZONTAL;
            auto* panel = wxDynamicCast(owner->GetParent(), wxRibbonPanel);
            const bool flexible = panel && (panel->GetFlags() & wxRIBBON_PANEL_FLEXIBLE);
            if ( flexible ) direction = wxHORIZONTAL;
            if ( !current() ) return false;
            // Copy the complete model before resolving any bitmap bundle: a
            // custom provider can reenter and delete tools, groups or the bar.
            // Neither a model pointer nor a collection iterator survives it.
            std::vector<wxBitmapBundle> bitmaps;
            for ( auto* group : owner->m_groups )
            {
                Group snapshot;
                snapshot.key = group->dummy_tool.key;
                for ( auto* tool : group->tools )
                {
                    const bool enabled = !(tool->state & wxRIBBON_TOOLBAR_TOOL_DISABLED);
                    snapshot.items.push_back({tool->key, tool->id, tool->kind,
                        tool->help_string, wxBitmap(),
                        enabled, (tool->state & wxRIBBON_TOOLBAR_TOOL_TOGGLED) != 0,
                        wxRect(), wxRect(), 0});
                    bitmaps.push_back(enabled ? tool->bitmap : tool->bitmap_disabled);
                }
                candidate->groups.push_back(std::move(snapshot));
            }
            size_t bitmapIndex = 0;
            for ( auto& group : candidate->groups )
            {
                for ( size_t i = 0; i < group.items.size(); ++i )
                {
                    auto& item = group.items[i];
                    item.bitmap = bitmaps[bitmapIndex++].GetBitmapFor(Owner());
                    if ( !current() ) return false;
                    auto peer = BuildPeer(item, candidate->generation);
                    if ( !current() ) return false;
                    peer->root.Measure({std::numeric_limits<float>::infinity(),
                                        std::numeric_limits<float>::infinity()});
                    if ( !current() ) return false;
                    const auto desired = peer->root.DesiredSize();
                    if ( !current() || !std::isfinite(desired.Width) ||
                         !std::isfinite(desired.Height) || desired.Width < 0 ||
                         desired.Height < 0 || desired.Width >= INT_MAX ||
                         desired.Height >= INT_MAX ) return false;
                    const wxSize size = Owner()->FromDIP(wxSize(
                        std::max(1, static_cast<int>(std::ceil(desired.Width))),
                        std::max(1, static_cast<int>(std::ceil(desired.Height)))));
                    item.rect = wxRect(group.size.x, 0, size.x, size.y);
                    item.positionFlags = (i == 0 ? wxRIBBON_TOOLBAR_TOOL_FIRST : 0)
                        | (i + 1 == group.items.size() ? wxRIBBON_TOOLBAR_TOOL_LAST : 0);
                    if ( item.kind == wxRIBBON_BUTTON_DROPDOWN ) item.dropdown = wxRect(size);
                    else if ( item.kind == wxRIBBON_BUTTON_HYBRID )
                    {
                        const int arrow = Owner()->FromDIP(24);
                        item.dropdown = wxRect(std::max(0, size.x - arrow), 0, arrow, size.y);
                    }
                    group.size.x += size.x;
                    group.size.y = std::max(group.size.y, size.y);
                    candidate->peers.push_back(std::move(peer));
                }
                for ( auto& item : group.items ) item.rect.height = group.size.y;
            }
            int minimumExtent = INT_MAX;
            wxSize flexibleMinimum(INT_MAX, INT_MAX);
            for ( int rows = firstRow; rows <= lastRow; ++rows )
            {
                const wxSize size = Arrange(candidate->groups, rows, separation);
                candidate->sizes.push_back(size);
                if ( Extent(size, direction) < minimumExtent )
                    { minimumExtent = Extent(size, direction); candidate->minimum = size; }
                flexibleMinimum.x = std::min(flexibleMinimum.x, size.x);
                flexibleMinimum.y = std::min(flexibleMinimum.y, size.y);
            }
            if ( flexible ) candidate->minimum = flexibleMinimum;
            owner = Owner();
            if ( !current() ) return false;
            candidate->available = owner->GetClientSize();
            int selected = lastRow;
            int largest = 0;
            for ( size_t i = 0; i < candidate->sizes.size(); ++i )
            {
                const auto& size = candidate->sizes[i];
                if ( size.x <= candidate->available.x && size.y <= candidate->available.y &&
                     Extent(size, direction) > largest )
                    { selected = firstRow + static_cast<int>(i); largest = Extent(size, direction); }
            }
            if ( nativeComposite )
            {
                // Toolkit owns group measurement and collapse. Advertise the
                // preferred row layout, not a projection squeezed by a stale
                // wx shell allocation. SetRows still defines every legal size.
                selected = firstRow;
                Arrange(candidate->groups, selected, separation);
            }
            else
                Arrange(candidate->groups, selected, separation, &candidate->available);
            candidate->root = MUXC::Canvas();
            if ( nativeComposite )
            {
                const auto preferred = owner->ToDIP(candidate->sizes.front());
                candidate->root.Width(preferred.x);
                candidate->root.Height(preferred.y);
            }
            candidate->root.FlowDirection(owner->GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft : MUX::FlowDirection::LeftToRight);
            size_t index = 0;
            for ( const auto& group : candidate->groups )
            {
                if ( group.separator )
                {
                    MUXC::AppBarSeparator separator;
                    separator.Width(owner->ToDIP(separation));
                    separator.Height(owner->ToDIP(group.size.y));
                    MUXC::Canvas::SetLeft(separator, owner->ToDIP(group.position.x - separation));
                    MUXC::Canvas::SetTop(separator, owner->ToDIP(group.position.y));
                    candidate->root.Children().Append(separator);
                }
                for ( const auto& item : group.items )
                {
                    const auto& peer = candidate->peers[index++];
                    const wxPoint pos = owner->ToDIP(group.position + item.rect.GetPosition());
                    const wxSize size = owner->ToDIP(item.rect.GetSize());
                    MUXC::Canvas::SetLeft(peer->root, pos.x);
                    MUXC::Canvas::SetTop(peer->root, pos.y);
                    peer->root.Width(size.x);
                    peer->root.Height(size.y);
                    candidate->root.Children().Append(peer->root);
                    if ( !current() ) return false;
                    owner = Owner();
                }
            }
            if ( !current() ) return false;
            // A pre-show Realize stages its native model until Group.Content
            // exists. It must not create a second top-level canvas slot.
            attachmentAttempted = !pendingMount;
            if ( !(pendingMount || Attach(candidate->root)) || !current() ) return false;
            m_projection = candidate;
            ApplyGeometry(candidate);
            provisional = true;
            Owner()->InvalidateBestSize();
            m_host.ForceRender();
            if ( !current() ) return false;
            committed = true;
            if ( nativeComposite ) candidate->available = Owner()->GetClientSize();
            else if ( candidate->available != Owner()->GetClientSize() ) ScheduleRepair();
            return true;
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("ribbon toolbar realization", e); return false; }
    }

    wxRect Bounds(std::uint64_t key) const
    {
        auto* owner = Owner();
        const auto projection = m_projection;
        if ( !owner || !projection || !Find(key) ) return {};
        for ( const auto& peer : projection->peers )
            if ( peer->key == key )
            {
                const auto root = peer->root;
                WF::Rect bounds{};
                if ( wxWinUIVisualCoordinates::ElementBoundsToClient(owner, root,
                        {0, 0, static_cast<float>(root.ActualWidth()),
                               static_cast<float>(root.ActualHeight())}, &bounds)
                         != wxWinUICoordinateResult::Mapped ||
                     !Owner() || m_projection != projection ) return {};
                const int x = static_cast<int>(std::floor(bounds.X));
                const int y = static_cast<int>(std::floor(bounds.Y));
                return {x, y, static_cast<int>(std::ceil(bounds.X + bounds.Width)) - x,
                              static_cast<int>(std::ceil(bounds.Y + bounds.Height)) - y};
            }
        return {};
    }
    wxRibbonToolBarToolBase* HitTest(wxPoint point) const
    {
        const auto projection = m_projection;
        if ( projection )
            for ( const auto& peer : projection->peers )
                if ( Bounds(peer->key).Contains(point) ) return Find(peer->key);
        return nullptr;
    }

private:
    wxWeakRef<wxRibbonToolBar> m_owner;
    wxWinUIControlHost m_host;
    std::shared_ptr<Projection> m_projection;
    std::uint64_t m_revision = 0;
    std::uint64_t m_nextGeneration = 0;
    std::uint64_t m_active = 0;
    wxSize m_observedSize{wxDefaultSize};
    bool m_building = false;
    bool m_pending = false;
    bool m_repairRunning = false;
    bool m_deferred = false;
};
