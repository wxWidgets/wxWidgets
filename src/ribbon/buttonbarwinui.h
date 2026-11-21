/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/buttonbarwinui.h
// Purpose:     Native wxWinUI ribbon command projection
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
//
// Private native projection of wxRibbonButtonBar's existing command model.
// Included after the model/layout definitions in buttonbar.cpp only.
/////////////////////////////////////////////////////////////////////////////

class wxRibbonButtonBarWinUI final
    : public std::enable_shared_from_this<wxRibbonButtonBarWinUI>
{
public:
    explicit wxRibbonButtonBarWinUI(wxRibbonButtonBar* owner) : m_owner(owner) {}

    struct Peer
    {
        std::uint64_t key = 0;
        MUXC::Grid root{nullptr};
        std::vector<std::function<void()>> revoke;

        ~Peer()
        {
            for ( auto& handler : revoke )
            {
                try { handler(); }
                catch ( const winrt::hresult_error& e )
                    { wxWinUILogException("ribbon handler removal", e); }
            }
        }
    };

    struct Item
    {
        std::uint64_t key;
        int id;
        wxRibbonButtonKind kind;
        wxString label;
        wxString help;
        wxBitmap bitmap;
        bool disabled;
        bool checked;
        bool disabledHelp;
        int textMinWidth;
    };

    bool Initialize() { return m_host.Initialize(m_owner.get()); }

    bool Attach(const MUX::UIElement& root)
    {
#if defined(WXWINUI_RIBBON_TOOLKIT)
        wxRibbonControl* parent = nullptr;
        MUXC::ContentControl mount{nullptr};
        const auto status = wxRibbonContainerWinUI::CommandMount(
            Owner(), parent, mount);
        if ( status == wxRibbonContainerWinUI::MountStatus::Ready )
            return m_host.SetContentInParent(root, root, parent, mount);
        if ( status != wxRibbonContainerWinUI::MountStatus::Ordinary )
            return false;
#endif
        return m_host.SetContent(root);
    }

    void Close()
    {
        m_owner = nullptr;
        ++m_generation;
        m_active = m_hovered = 0;
        auto retired = std::move(m_peers);
        m_root = nullptr;
        m_host.Close();
    }

    wxRibbonButtonBar* Owner() const
    {
        wxRibbonButtonBar* owner = m_owner.get();
        return owner && !owner->IsBeingDeleted() &&
               owner->m_winui.get() == this ? owner : nullptr;
    }

    wxRibbonButtonBarButtonBase* Find(std::uint64_t key) const
    {
        if ( wxRibbonButtonBar* owner = Owner() )
        {
            for ( auto* item : owner->m_buttons )
                if ( item->key == key )
                    return item;
        }
        return nullptr;
    }

    wxRibbonButtonBarButtonBase* Active() const { return Find(m_active); }
    wxRibbonButtonBarButtonBase* Hovered() const { return Find(m_hovered); }

    Item Snapshot(wxRibbonButtonBarButtonBase& model, unsigned size) const
    {
        wxRibbonButtonBar* owner = Owner();
        Item item{model.key, model.id, model.kind, model.label, model.help_string,
                  wxBitmap(),
                  (model.state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) != 0,
                  (model.state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED) != 0,
                  owner->m_show_tooltips_for_disabled,
                  owner->ToDIP(model.text_min_width[size])};
        const bool large = size == wxRIBBON_BUTTONBAR_BUTTON_LARGE;
        const auto& bundles = large
            ? (item.disabled ? owner->m_bundlesLargeDisabled : owner->m_bundlesLarge)
            : (item.disabled ? owner->m_bundlesSmallDisabled : owner->m_bundlesSmall);
        const int index = large
            ? (item.disabled ? model.imageIndexLargeDisabled : model.imageIndexLarge)
            : (item.disabled ? model.imageIndexSmallDisabled : model.imageIndexSmall);
        if ( index >= 0 && static_cast<size_t>(index) < bundles.size() )
            item.bitmap = bundles[index].GetBitmapFor(owner);
        return item;
    }

    std::shared_ptr<Peer> BuildPeer(const Item& item, unsigned size,
                                  std::uint64_t generation)
    {
        auto peer = std::make_shared<Peer>();
        peer->key = item.key;
        const auto labelPosition = size == wxRIBBON_BUTTONBAR_BUTTON_SMALL
            ? MUXC::CommandBarLabelPosition::Collapsed
            : MUXC::CommandBarLabelPosition::Default;
        const auto visual = wxRibbonWinUIBuildCommand({
            item.kind, item.label, item.bitmap, !item.disabled, item.checked,
            size == wxRIBBON_BUTTONBAR_BUTTON_LARGE ? 32.0 : 16.0,
            size == wxRIBBON_BUTTONBAR_BUTTON_LARGE ? 68.0 :
                size == wxRIBBON_BUTTONBAR_BUTTON_MEDIUM ? 48.0 : 32.0,
            static_cast<double>(std::max(32, item.textMinWidth)), labelPosition});
        peer->root = visual.root;
        const std::weak_ptr<wxRibbonButtonBarWinUI> weak = shared_from_this();
        const std::uint64_t key = item.key;
        if ( item.kind == wxRIBBON_BUTTON_TOGGLE )
        {
            const auto button = visual.primary.as<MUXC::AppBarToggleButton>();
            if ( generation )
            {
                const auto checked = button.Checked(
                    [weak, key, generation](const auto&, const auto&)
                    { if ( auto self = weak.lock() )
                          self->Activate(key, generation, false, 1); });
                const auto unchecked = button.Unchecked(
                    [weak, key, generation](const auto&, const auto&)
                    { if ( auto self = weak.lock() )
                          self->Activate(key, generation, false, 0); });
                peer->revoke.push_back([button, checked, unchecked]()
                    { button.Checked(checked); button.Unchecked(unchecked); });
            }
        }
        else
        {
            const auto button = visual.primary.as<MUXC::AppBarButton>();
            if ( generation )
            {
                const bool dropdown = item.kind == wxRIBBON_BUTTON_DROPDOWN;
                const auto click = button.Click(
                    [weak, key, generation, dropdown](const auto&, const auto&)
                    { if ( auto self = weak.lock() )
                          self->Activate(key, generation, dropdown, -1); });
                peer->revoke.push_back([button, click]() { button.Click(click); });
            }
        }
        if ( visual.arrow )
        {
            const auto arrow = visual.arrow;
            if ( generation )
            {
                const auto click = arrow.Click(
                    [weak, key, generation](const auto&, const auto&)
                    { if ( auto self = weak.lock() )
                          self->Activate(key, generation, true, -1); });
                peer->revoke.push_back([arrow, click]() { arrow.Click(click); });
            }
        }
        // The wrapper remains enabled: a disabled command can still expose
        // its help when requested, without making its command peers invokable.
        if ( !item.help.empty() && (!item.disabled || item.disabledHelp) )
            MUXC::ToolTipService::SetToolTip(peer->root,
                winrt::box_value(wxWinUIToHString(item.help)));
        if ( generation )
        {
            const auto enter = peer->root.PointerEntered(
                [weak, key, generation](const auto&, const auto&)
                {
                    if ( auto self = weak.lock(); self &&
                         self->CanActivate(key, generation) )
                        self->m_hovered = key;
                });
            const auto leave = peer->root.PointerExited(
                [weak, key](const auto&, const auto&)
                { if ( auto self = weak.lock(); self && self->m_hovered == key )
                      self->m_hovered = 0; });
            const auto element = peer->root;
            peer->revoke.push_back([element, enter, leave]()
                { element.PointerEntered(enter); element.PointerExited(leave); });
        }
        return peer;
    }

    bool CanActivate(std::uint64_t key, std::uint64_t generation) const
    {
        return !m_building && m_generation == generation &&
               m_publishedRevision == m_revision && Find(key);
    }

    void Activate(std::uint64_t key, std::uint64_t generation,
                  bool dropdown, int checked) noexcept
    {
        try
        {
            if ( !CanActivate(key, generation) )
                return;
            wxRibbonButtonBar* owner = Owner();
            auto* button = Find(key);
            if ( !owner || !owner->IsEnabled() || !owner->IsShownOnScreen() || !button ||
                 (button->state & wxRIBBON_BUTTONBAR_BUTTON_DISABLED) )
                return;
            if ( checked >= 0 )
            {
                const bool old =
                    (button->state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED) != 0;
                if ( old == (checked != 0) )
                    return;
                if ( checked ) button->state |= wxRIBBON_BUTTONBAR_BUTTON_TOGGLED;
                else button->state &= ~wxRIBBON_BUTTONBAR_BUTTON_TOGGLED;
            }
            const auto previousActive = m_active;
            m_active = key;
            wxRibbonButtonBarEvent event(dropdown
                ? wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED
                : wxEVT_RIBBONBUTTONBAR_CLICKED, button->id, owner, button);
            event.SetEventObject(owner);
            if ( button->kind == wxRIBBON_BUTTON_TOGGLE )
                event.SetInt(button->state & wxRIBBON_BUTTONBAR_BUTTON_TOGGLED);
            owner->ProcessWindowEvent(event);
            m_active = Find(previousActive) ? previousActive : 0;
            // The handler can remove this item, rebuild, open a nested menu,
            // or destroy the bar/TLW. Never carry a model pointer past it.
            owner = Owner();
            if ( owner )
            {
                if ( auto* panel = wxDynamicCast(owner->GetParent(), wxRibbonPanel) )
                    panel->HideIfExpanded();
            }
        }
        catch ( const winrt::hresult_error& e )
            { m_active = 0; wxWinUILogException("ribbon command", e); }
        catch ( ... )
            { m_active = 0; wxLogError("Exception in native ribbon command handler"); }
    }

    void Changed()
    {
        ++m_revision;
        if ( auto* owner = Owner() )
            owner->InvalidateBestSize();
        if ( m_root && !m_building )
            (void)Rebuild();
        else if ( m_building )
            ScheduleRepair();
    }

    void Resized()
    {
        if ( auto* owner = Owner() )
        {
            const wxSize size = owner->GetClientSize();
            // Content attachment and host migration can emit WM_SIZE without
            // changing the available space. Do not retire the candidate for
            // those notifications, but still repair a real nested resize.
            if ( size != m_observedClientSize )
            {
                m_observedClientSize = size;
                // A viewport change does not retire command identities or
                // measurements. Keep model transactions valid and arrange
                // their published projection for the new space afterwards.
                if ( m_root && !m_building )
                    (void)Rebuild();
                else if ( m_building )
                    ScheduleRepair();
            }
        }
    }

    void ScheduleRepair()
    {
        if ( m_repairPending || m_repairRunning || !wxTheApp || !Owner() ) return;
        m_repairPending = true;
        const std::weak_ptr<wxRibbonButtonBarWinUI> weak = shared_from_this();
        wxTheApp->CallAfter([weak]()
        {
            if ( auto self = weak.lock() )
            {
                self->m_repairPending = false;
                if ( self->m_building )
                {
                    // A nested event loop consumed our queued turn before
                    // the transaction ended. Its guard rearms this same
                    // repair after rollback/commit, without another attempt.
                    self->m_repairDeferred = true;
                    return;
                }
                auto* owner = self->Owner();
                if ( !owner ||
                     (self->m_publishedRevision == self->m_revision &&
                      self->m_publishedClientSize == owner->GetClientSize()) )
                    return;
                // One deferred attempt for a nested writer, not an unbounded
                // rebuild loop if application callbacks keep mutating.
                self->m_repairRunning = true;
                (void)self->Rebuild();
                self->m_repairRunning = false;
            }
        });
    }

    bool Rebuild()
    {
        if ( m_building )
            return false;
        auto self = shared_from_this();
        wxRibbonButtonBar* owner = Owner();
        if ( !owner )
            return false;
        bool nativeComposite = false;
        bool pendingMount = false;
#if defined(WXWINUI_RIBBON_TOOLKIT)
        wxRibbonControl* compositeOwner = nullptr;
        MUXC::ContentControl contentMount{nullptr};
        const auto mountStatus = wxRibbonContainerWinUI::CommandMount(
            owner, compositeOwner, contentMount);
        pendingMount = mountStatus == wxRibbonContainerWinUI::MountStatus::Pending;
        if ( pendingMount && m_host.GetContent() )
            return false; // Never replace an already hosted generation off-tree.
        if ( mountStatus == wxRibbonContainerWinUI::MountStatus::Failed )
            return false;
        nativeComposite = pendingMount ||
            mountStatus == wxRibbonContainerWinUI::MountStatus::Ready;
        if ( !pendingMount && nativeComposite && m_root &&
             owner->m_layouts_valid && m_publishedRevision == m_revision &&
             m_host.GetContent() == m_root )
        {
            // Loaded is raised again when the complete Ribbon tree moves to
            // another XamlRoot. Its container asks children to Realize so an
            // initially staged command can publish, but an already-published
            // current model must retain its command peers and UIA providers.
            // Toolkit owns arrangement: a different wx shell allocation does
            // not require a new command projection here.
            auto* const host = wxWinUITopLevelHost::FindSlotOwner(owner);
            const auto* const slot = host ? host->FindSlot(owner) : nullptr;
            const auto presentation = slot
                ? slot->GetPresentationRoot() : MUX::FrameworkElement{nullptr};
            const auto root = m_root;
            const auto revision = m_revision;
            try
            {
                if ( presentation && slot->GetContent() == root &&
                     contentMount.Content() == presentation &&
                     Owner() == owner && m_root == root &&
                     m_publishedRevision == revision && m_revision == revision )
                {
                    m_observedClientSize = owner->GetClientSize();
                    m_publishedClientSize = m_observedClientSize;
                    return true;
                }
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("ribbon command mount validation", e);
                return false;
            }
        }
#endif
        m_observedClientSize = owner->GetClientSize();
        m_building = true;
        struct Guard
        {
            wxRibbonButtonBarWinUI& self;
            ~Guard()
            {
                self.m_building = false;
                if ( std::exchange(self.m_repairDeferred, false) )
                    self.ScheduleRepair();
            }
        } guard{*this};
        const auto revision = m_revision;
        const auto generation = ++m_nextGeneration;
        const auto current = [&]()
            { return Owner() && m_revision == revision; };
        MUXC::Canvas candidate{nullptr};
        std::vector<std::shared_ptr<Peer>> peers;
        struct LayoutStorage
        {
            wxArrayRibbonButtonBarLayout values;
            ~LayoutStorage() { for ( auto* layout : values ) delete layout; }
        } previousLayouts, candidateLayouts;
        struct Sizes
        {
            std::uint64_t key;
            std::array<wxRibbonButtonBarButtonSizeInfo, 3> previous;
            std::array<wxRibbonButtonBarButtonSizeInfo, 3> candidate;
        };
        std::vector<Sizes> sizes;
        const auto previousRoot = m_root;
        const auto previousHostedRoot = m_host.GetContent();
        const auto previousGeneration = m_generation;
        const auto previousRevision = m_publishedRevision;
        const auto previousBest = m_best;
        const auto previousMinimum = m_minimum;
        const auto previousClientSize = m_publishedClientSize;
        const int previousLayout = owner->m_current_layout;
        const wxPoint previousOffset = owner->m_layout_offset;
        const bool previousValid = owner->m_layouts_valid;
        bool staging = false;
        bool provisional = false;
        bool attachmentAttempted = false;
        bool committed = false;
        const auto applySizes = [&](bool candidateSizes)
        {
            for ( const auto& item : sizes )
                if ( auto* model = Find(item.key) )
                    std::copy_n((candidateSizes ? item.candidate : item.previous).begin(),
                                3, model->sizes);
        };
        const auto restoreLayouts = [&]()
        {
            if ( auto* live = Owner() )
            {
                candidateLayouts.values.swap(live->m_layouts);
                previousLayouts.values.swap(live->m_layouts);
                live->m_current_layout = previousLayout;
                live->m_layout_offset = previousOffset;
                live->m_layouts_valid = previousValid;
                applySizes(false);
            }
        };
        wxScopeGuard rollback = wxMakeGuard([&]()
        {
            if ( committed ) return;
            if ( staging || provisional ) restoreLayouts();
            if ( provisional )
            {
                m_root = previousRoot;
                m_peers.swap(peers);
                m_generation = previousGeneration;
                m_publishedRevision = previousRevision;
                m_best = previousBest;
                m_minimum = previousMinimum;
                m_publishedClientSize = previousClientSize;
            }
            // SetContent(false) is already transactional: the host keeps its
            // previous publication. Only undo a candidate actually published
            // by this operation, never retry a rejected swap or overwrite a
            // newer host publication. In particular, a temporary refusal
            // during reparenting must not close a still-valid command host.
            if ( attachmentAttempted && Owner() &&
                 m_host.GetContent() == candidate )
            {
                try
                {
                    if ( previousHostedRoot )
                    {
                        if ( !Attach(previousHostedRoot) )
                        {
                            wxLogWarning("Could not restore the previous native ribbon peer");
                            Close();
                            return;
                        }
                    }
                    else
                        m_host.ClearContent();
                    if ( auto* live = Owner() )
                    {
                        live->InvalidateBestSize();
                        m_host.ForceRender();
                    }
                }
                catch ( const winrt::hresult_error& e )
                {
                    wxWinUILogException("ribbon peer rollback", e);
                    Close();
                }
            }
        });
        wxUnusedVar(rollback);
        try
        {
            // Native templates determine all three size classes; the common
            // ribbon layout algorithm still owns adaptive grouping/collapse.
            std::vector<std::uint64_t> keys;
            for ( auto* item : owner->m_buttons ) keys.push_back(item->key);
            for ( auto key : keys )
            {
                auto* original = Find(key);
                if ( !current() || !original ) return false;
                Sizes measuredSizes;
                measuredSizes.key = key;
                std::copy_n(original->sizes, 3, measuredSizes.previous.begin());
                for ( unsigned size = 0; size != 3; ++size )
                {
                    auto* model = Find(key);
                    if ( !current() || !model ) return false;
                    const auto measured = BuildPeer(Snapshot(*model, size), size, 0);
                    measured->root.Measure({
                        std::numeric_limits<float>::infinity(),
                        std::numeric_limits<float>::infinity()});
                    if ( !current() || !(model = Find(key)) ) return false;
                    const auto desired = measured->root.DesiredSize();
                    auto& info = measuredSizes.candidate[size];
                    info.is_supported = true;
                    info.size = Owner()->FromDIP(wxSize(
                        std::max(1, static_cast<int>(std::ceil(desired.Width))),
                        std::max(1, static_cast<int>(std::ceil(desired.Height)))));
                    info.normal_region = wxRect(info.size);
                    info.dropdown_region = wxRect();
                    if ( model->kind == wxRIBBON_BUTTON_DROPDOWN )
                    {
                        info.dropdown_region = info.normal_region;
                        info.normal_region = wxRect();
                    }
                    else if ( model->kind == wxRIBBON_BUTTON_HYBRID )
                    {
                        const int arrow = Owner()->FromDIP(24);
                        info.normal_region.width = std::max(0, info.size.x - arrow);
                        info.dropdown_region = wxRect(info.normal_region.width, 0,
                                                     arrow, info.size.y);
                    }
                }
                sizes.push_back(std::move(measuredSizes));
            }
            owner = Owner();
            if ( !current() ) return false;
            // The common layout builder has no callback boundary. Lend it the
            // measured sizes, capture its result, and restore the published
            // projection before constructing/attaching any native candidate.
            previousLayouts.values.swap(owner->m_layouts);
            staging = true;
            applySizes(true);
            owner->m_layouts_valid = false;
            owner->MakeLayouts();
            owner->m_layouts_valid = true;
            wxSize available = owner->GetClientSize();
            size_t selected = owner->m_layouts.size() - 1;
            for ( size_t i = 0; i < owner->m_layouts.size(); ++i )
            {
                const wxSize size = owner->m_layouts[i]->overall_size;
                if ( size.x <= available.x && size.y <= available.y )
                    { selected = i; break; }
            }
            if ( nativeComposite )
            {
                // The Toolkit measures and collapses the group. A stale wx
                // shell size must not choose the smallest command projection
                // or introduce canvas padding inside the native group.
                selected = 0;
                available = owner->m_layouts[0]->overall_size;
            }
            owner->m_current_layout = static_cast<int>(selected);
            const auto* layout = owner->m_layouts[selected];
            const wxSize layoutSize = layout->overall_size;
            owner->m_layout_offset = wxPoint(
                std::max(0, (available.x - layoutSize.x) / 2),
                std::max(0, (available.y - layoutSize.y) / 2));
            const wxSize best = owner->m_layouts[0]->overall_size;
            const wxSize minimum = owner->m_layouts.Last()->overall_size;
            const wxPoint candidateOffset = owner->m_layout_offset;
            // Copy only placement/keys. No layout-instance pointer survives
            // construction of a WinRT object or any subsequent callback.
            struct Placement { std::uint64_t key; unsigned size; wxRect rect; };
            std::vector<Placement> placements;
            for ( const auto& instance : layout->buttons )
                placements.push_back({instance.base->key,
                    static_cast<unsigned>(instance.size),
                    wxRect(instance.position + owner->m_layout_offset,
                           instance.base->sizes[instance.size].size)});
            restoreLayouts();
            staging = false;
            candidate = MUXC::Canvas();
            if ( nativeComposite )
            {
                const wxSize nativeSize = owner->ToDIP(layoutSize);
                candidate.Width(nativeSize.x);
                candidate.Height(nativeSize.y);
            }
            candidate.FlowDirection(owner->GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft : MUX::FlowDirection::LeftToRight);
            for ( const auto& placement : placements )
            {
                auto* model = Find(placement.key);
                if ( !current() || !model ) return false;
                auto peer = BuildPeer(Snapshot(*model, placement.size),
                                      placement.size, generation);
                if ( !current() ) return false;
                owner = Owner();
                const wxPoint pos = owner->ToDIP(placement.rect.GetPosition());
                const wxSize size = owner->ToDIP(placement.rect.GetSize());
                MUXC::Canvas::SetLeft(peer->root, pos.x);
                MUXC::Canvas::SetTop(peer->root, pos.y);
                peer->root.Width(size.x);
                peer->root.Height(size.y);
                candidate.Children().Append(peer->root);
                peers.push_back(std::move(peer));
            }
            if ( !current() ) return false;
            // Realize() is valid before the parent window is shown. Build and
            // publish the measured command model now, but do not manufacture a
            // top-level slot while Group.Content is still awaiting its native
            // template. Group.Loaded performs a fresh, current-model rebuild.
            attachmentAttempted = !pendingMount;
            const bool attached = pendingMount || Attach(candidate);
            if ( !current() || !attached )
                return false;
            owner = Owner();
            previousLayouts.values.swap(owner->m_layouts);
            candidateLayouts.values.swap(owner->m_layouts);
            owner->m_current_layout = static_cast<int>(selected);
            owner->m_layout_offset = candidateOffset;
            owner->m_layouts_valid = true;
            applySizes(true);
            m_root = candidate;
            m_peers.swap(peers);
            m_generation = generation;
            m_publishedRevision = revision;
            m_best = best;
            m_minimum = minimum;
            m_publishedClientSize = nativeComposite ? owner->GetClientSize() : available;
            provisional = true;
            Owner()->InvalidateBestSize();
            m_host.ForceRender();
            if ( !current() ) return false;
            committed = true;
            if ( !nativeComposite && m_publishedClientSize != Owner()->GetClientSize() )
                ScheduleRepair();
            return true;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("ribbon peer realization", e);
            return false;
        }
    }

    wxRect Bounds(std::uint64_t key) const
    {
        wxRibbonButtonBar* owner = Owner();
        if ( !owner || !Find(key) ) return {};
        const auto generation = m_generation;
        for ( const auto& peer : m_peers )
        {
            if ( peer->key != key ) continue;
            const auto element = peer->root;
            WF::Rect bounds{};
            if ( wxWinUIVisualCoordinates::ElementBoundsToClient(owner, element,
                    {0, 0, static_cast<float>(element.ActualWidth()),
                           static_cast<float>(element.ActualHeight())}, &bounds)
                    != wxWinUICoordinateResult::Mapped ||
                 !Owner() || m_generation != generation ) return {};
            const int x = static_cast<int>(std::floor(bounds.X));
            const int y = static_cast<int>(std::floor(bounds.Y));
            return {x, y, static_cast<int>(std::ceil(bounds.X + bounds.Width)) - x,
                          static_cast<int>(std::ceil(bounds.Y + bounds.Height)) - y};
        }
        return {};
    }

    wxSize Best() const { return m_best; }
    wxSize Minimum() const { return m_minimum; }
    bool HasRoot() const { return m_root != nullptr; }
    bool IsBuilding() const { return m_building; }
    bool HasPeer(std::uint64_t key) const
    {
        return std::any_of(m_peers.begin(), m_peers.end(),
                          [key](const auto& peer) { return peer->key == key; });
    }

private:
    wxWeakRef<wxRibbonButtonBar> m_owner;
    wxWinUIControlHost m_host;
    MUXC::Canvas m_root{nullptr};
    std::vector<std::shared_ptr<Peer>> m_peers;
    std::uint64_t m_generation = 0;
    std::uint64_t m_nextGeneration = 0;
    std::uint64_t m_revision = 0;
    std::uint64_t m_publishedRevision = 0;
    std::uint64_t m_active = 0;
    std::uint64_t m_hovered = 0;
    bool m_building = false;
    bool m_repairPending = false;
    bool m_repairRunning = false;
    bool m_repairDeferred = false;
    wxSize m_observedClientSize{wxDefaultSize};
    wxSize m_publishedClientSize{wxDefaultSize};
    wxSize m_best{20, 20};
    wxSize m_minimum{20, 20};
};
