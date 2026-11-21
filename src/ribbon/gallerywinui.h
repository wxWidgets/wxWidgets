/////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/gallerywinui.h
// Purpose:     Native WinUI projection of the public Ribbon Gallery model
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

class wxRibbonGalleryWinUI final
    : public std::enable_shared_from_this<wxRibbonGalleryWinUI>
{
public:
    explicit wxRibbonGalleryWinUI(wxRibbonGallery* owner) : m_owner(owner) {}
    struct Entry
    {
        std::uint64_t key;
        int id;
        wxBitmapBundle bitmap;
        MUXC::GridViewItem item{nullptr};
    };
    struct Projection
    {
        MUXC::Grid root{nullptr};
        MUXC::GridView view{nullptr};
        MUXC::ScrollViewer scroll{nullptr};
        std::array<MUXC::Button, 3> buttons{nullptr, nullptr, nullptr};
        std::array<wxRibbonGalleryButtonState, 3> states{
            wxRIBBON_GALLERY_BUTTON_DISABLED, wxRIBBON_GALLERY_BUTTON_DISABLED,
            wxRIBBON_GALLERY_BUTTON_NORMAL};
        std::vector<Entry> entries;
        std::vector<std::function<void()>> revoke;
        wxSize cell{72, 40};
        wxSize minimum{96, 72};
        wxSize best{240, 72};
        wxSize available{wxDefaultSize};
        std::uint64_t revision = 0;
        std::uint64_t generation = 0;
        double offset = 0;
        double limit = 0;
        bool pendingScroll = false;
        bool scrollRequestIssued = false;
        bool nativeScrollReady = false;
        bool staged = false;
        bool horizontal = false;
        ~Projection()
        {
            for ( auto& handler : revoke )
                try { handler(); }
                catch ( const winrt::hresult_error& e )
                    { wxWinUILogException("ribbon gallery handler removal", e); }
        }
    };

    bool Initialize() { return m_host.Initialize(m_owner.get()); }
    wxRibbonGallery* Owner() const
    {
        auto* owner = m_owner.get();
        return owner && !owner->IsBeingDeleted() && owner->m_winui.get() == this
            ? owner : nullptr;
    }
    wxRibbonGalleryItem* Find(std::uint64_t key) const
    {
        if ( auto* owner = Owner() )
            for ( auto* item : owner->m_items ) if ( item->key == key ) return item;
        return nullptr;
    }
    std::uint64_t Key(const wxRibbonGalleryItem* value) const
    {
        if ( auto* owner = Owner() )
            for ( auto* item : owner->m_items ) if ( item == value ) return item->key;
        return 0;
    }
    bool Current(std::uint64_t generation) const
    {
        return Owner() && !m_building && m_projection &&
            m_projection->generation == generation &&
            m_projection->revision == m_revision;
    }
    void Close()
    {
        m_owner = nullptr;
        m_selected = m_hovered = m_active = 0;
        m_ensureVisible = 0;
        auto retired = std::move(m_projection);
        m_host.Close();
    }
    wxRibbonGalleryItem* Append(const wxBitmapBundle& bitmap, int id,
                               void* raw, wxClientData* object, bool owned,
                               bool untyped = false)
    {
        auto self = shared_from_this();
        std::unique_ptr<wxRibbonGalleryItem> item(new wxRibbonGalleryItem);
        item->SetId(id);
        item->SetBitmap(bitmap);
        if ( owned ) item->SetClientObject(object);
        else if ( untyped ) item->SetClientData(raw);
        auto* owner = Owner();
        if ( !owner ) return nullptr;
        // Bitmap providers are extensible application code. Resolve only the
        // retained bundle and find the live model again after it returns: a
        // nested Append/Clear can change which item establishes the baseline.
        const wxSize bitmapSize = item->GetBitmapBundle().GetDefaultSize();
        owner = Owner();
        if ( !owner ) return nullptr;
        if ( owner->m_items.IsEmpty() )
            owner->m_bitmap_size = bitmapSize;
        else
        {
            const auto revision = m_revision;
            wxASSERT_MSG(bitmapSize == owner->m_bitmap_size,
                         "All ribbon gallery bitmaps must have the same default size");
            // An assertion handler can run a nested loop or destroy the bar.
            // Do not append against a different model after such a callback.
            owner = Owner();
            if ( !owner || m_revision != revision ) return nullptr;
        }
        const auto key = item->key;
        owner->m_items.Add(item.release());
        Changed();
        return Find(key);
    }
    void Clear()
    {
        auto self = shared_from_this();
        auto* owner = Owner();
        if ( !owner ) return;
        wxArrayRibbonGalleryItem retired;
        retired.swap(owner->m_items);
        m_selected = m_hovered = m_active = 0;
        m_ensureVisible = 0;
        owner->m_selected_item = owner->m_hovered_item = owner->m_active_item = nullptr;
        owner->m_mouse_active_rect = nullptr;
        Changed();
        // User-owned destructors may append, clear again, or destroy the
        // gallery. This detached batch owns only the outgoing generation.
        for ( auto* item : retired ) delete item;
    }
    wxSize Minimum() const { return m_projection ? m_projection->minimum : wxSize(20, 20); }
    wxSize Best() const { return m_projection ? m_projection->best : wxSize(20, 20); }
    wxRibbonGalleryItem* Selection() const { return Find(m_selected); }
    wxRibbonGalleryItem* Hovered() const { return Find(m_hovered); }
    wxRibbonGalleryItem* Active() const { return Find(m_active); }
    bool IsHovered() const { return m_isHovered; }

    void Send(wxEventType type, std::uint64_t key)
    {
        auto* owner = Owner();
        if ( !owner ) return;
        auto* item = Find(key);
        if ( key && !item ) return;
        wxRibbonGalleryEvent event(type, owner->GetId(), owner, item);
        event.SetEventObject(owner);
        owner->ProcessWindowEvent(event);
    }
    void Select(std::uint64_t key, bool notify)
    {
        auto self = shared_from_this();
        if ( !Owner() || (key && !Find(key)) || key == m_selected ) return;
        m_selected = key;
        const auto projection = m_projection;
        if ( projection )
        {
            MUXC::GridViewItem selected{nullptr};
            for ( const auto& entry : projection->entries )
                if ( entry.key == key ) { selected = entry.item; break; }
            const bool previous = std::exchange(m_settingSelection, true);
            const auto restore = wxMakeGuard([&]() { m_settingSelection = previous; });
            try { projection->view.SelectedItem(selected); }
            catch ( const winrt::hresult_error& e )
                { wxWinUILogException("ribbon gallery selection", e); }
        }
        if ( notify && Owner() && m_selected == key ) Send(wxEVT_RIBBONGALLERY_SELECTED, key);
    }
    std::uint64_t NativeKey(const winrt::Windows::Foundation::IInspectable& value) const
    {
        if ( m_projection )
            for ( const auto& entry : m_projection->entries )
                if ( entry.item == value ) return entry.key;
        return 0;
    }
    void SelectionChanged(std::uint64_t generation) noexcept
    {
        try
        {
            if ( m_settingSelection || !Current(generation) || !Owner()->IsEnabled() ) return;
            const auto value = m_projection->view.SelectedItem();
            const auto key = NativeKey(value);
            if ( !Current(generation) || (value && !key) ) return;
            Select(key, true);
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("ribbon gallery selection event", e); }
        catch ( ... ) { wxLogError("Exception in ribbon gallery selection handler"); }
    }
    void Click(std::uint64_t generation, const WF::IInspectable& item) noexcept
    {
        try
        {
            const auto key = NativeKey(item);
            if ( !Current(generation) || !key || !Owner()->IsEnabled() ) return;
            const auto previous = m_active;
            m_active = key;
            const auto restore = wxMakeGuard([&]() { m_active = Find(previous) ? previous : 0; });
            Select(key, true);
            // Selection handlers can clear the model, redirect selection,
            // replace the visual generation, or destroy the gallery.
            if ( Current(generation) && Find(key) && m_selected == key )
                Send(wxEVT_RIBBONGALLERY_CLICKED, key);
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("ribbon gallery click", e); }
        catch ( ... ) { wxLogError("Exception in ribbon gallery click handler"); }
    }
    void Hover(std::uint64_t generation, std::uint64_t key) noexcept
    {
        try
        {
            if ( !Current(generation) || (key && !Find(key)) || m_hovered == key ) return;
            m_hovered = key;
            Send(wxEVT_RIBBONGALLERY_HOVER_CHANGED, key);
        }
        catch ( ... ) { wxLogError("Exception in ribbon gallery hover handler"); }
    }
    void Changed()
    {
        ++m_revision;
        if ( auto* owner = Owner() ) owner->InvalidateBestSize();
        if ( m_projection && !m_building ) (void)Rebuild();
        else if ( m_building ) ScheduleRepair();
    }
    void Resized()
    {
        if ( auto* owner = Owner(); owner && owner->GetClientSize() != m_observed )
        {
            m_observed = owner->GetClientSize();
            if ( m_projection && !m_building ) (void)Rebuild();
            else if ( m_building ) ScheduleRepair();
        }
    }
    void ScheduleRepair()
    {
        if ( m_pending || m_repairRunning || !wxTheApp || !Owner() ) return;
        m_pending = true;
        const std::weak_ptr<wxRibbonGalleryWinUI> weak = shared_from_this();
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
    static MUXC::ScrollViewer FindScroll(const MUX::DependencyObject& parent)
    {
        if ( auto scroll = parent.try_as<MUXC::ScrollViewer>() ) return scroll;
        const int count = MUX::Media::VisualTreeHelper::GetChildrenCount(parent);
        for ( int i = 0; i < count; ++i )
            if ( auto scroll = FindScroll(MUX::Media::VisualTreeHelper::GetChild(parent, i)) )
                return scroll;
        return nullptr;
    }
    void UpdateScroll()
    {
        const auto projection = m_projection;
        if ( !Owner() || !projection ) return;
        if ( !projection->scroll )
        {
            projection->scroll = FindScroll(projection->view);
            if ( projection->scroll )
            {
                const auto scroll = projection->scroll;
                const std::weak_ptr<wxRibbonGalleryWinUI> weak = shared_from_this();
                const auto generation = projection->generation;
                const auto changed = scroll.ViewChanged([weak, generation](const auto&, const auto&)
                {
                    if ( auto self = weak.lock(); self && self->Current(generation) )
                    {
                        self->UpdateScroll();
                        self->ReplayScroll();
                    }
                });
                projection->revoke.push_back([scroll, changed]() { scroll.ViewChanged(changed); });
            }
        }
        const auto scroll = projection->scroll;
        if ( !Owner() || m_projection != projection ) return;
        projection->nativeScrollReady = false;
        // Before first mounting the wx model already has a bounded viewport
        // and explicit item dimensions. A zero, uninitialized ScrollViewer
        // must not erase its requested offset or replace its valid limit.
        if ( scroll && !projection->staged && projection->view.IsLoaded() &&
             scroll.ViewportWidth() > 0 && scroll.ViewportHeight() > 0 &&
             (projection->entries.empty() ||
              (projection->horizontal ? scroll.ExtentWidth() : scroll.ExtentHeight()) > 0) )
        {
            const double limit = projection->horizontal ? scroll.ScrollableWidth() : scroll.ScrollableHeight();
            const double offset = projection->horizontal ? scroll.HorizontalOffset() : scroll.VerticalOffset();
            if ( !Owner() || m_projection != projection || !std::isfinite(limit) ||
                 !std::isfinite(offset) ) return;
            projection->nativeScrollReady = true;
            projection->limit = std::max(0.0, limit);
            projection->offset = std::clamp(projection->offset, 0.0, projection->limit);
            if ( projection->pendingScroll && std::abs(offset - projection->offset) < 0.5 )
            {
                projection->pendingScroll = false;
                projection->scrollRequestIssued = false;
            }
            if ( !projection->pendingScroll ) projection->offset = offset;
        }
        for ( size_t i = 0; i < 2; ++i )
        {
            const bool enabled = i == 0 ? projection->offset > 0.5
                : projection->offset + 0.5 < projection->limit;
            projection->buttons[i].IsEnabled(enabled);
            if ( !enabled ) projection->states[i] = wxRIBBON_GALLERY_BUTTON_DISABLED;
            else if ( projection->states[i] == wxRIBBON_GALLERY_BUTTON_DISABLED )
                projection->states[i] = wxRIBBON_GALLERY_BUTTON_NORMAL;
        }
    }
    bool Scroll(double pixels)
    {
        auto self = shared_from_this();
        try
        {
            UpdateScroll();
            const auto projection = m_projection;
            auto* owner = Owner();
            if ( !owner || !projection || !std::isfinite(pixels) ) return false;
            const double delta = pixels / owner->GetDPIScaleFactor();
            const double target = std::clamp(projection->offset + delta, 0.0, projection->limit);
            if ( std::abs(target - projection->offset) < 0.01 ) return false;
            m_ensureVisible = 0;
            const double previous = projection->offset;
            projection->offset = target;
            projection->pendingScroll = true;
            projection->scrollRequestIssued = false;
            if ( !projection->nativeScrollReady )
            {
                UpdateScroll();
                return true;
            }
            const auto boxed = winrt::box_value(target).as<WF::IReference<double>>();
            const bool accepted = projection->scroll.ChangeView(
                projection->horizontal ? boxed : nullptr,
                projection->horizontal ? nullptr : boxed, nullptr, true);
            projection->scrollRequestIssued = accepted;
            if ( !accepted && Owner() && m_projection == projection )
                { projection->offset = previous; projection->pendingScroll = false; }
            UpdateScroll();
            return accepted;
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("ribbon gallery scrolling", e); return false; }
    }
    int LineSize() const
    {
        return m_projection ? (m_projection->horizontal ? m_projection->cell.x : m_projection->cell.y) : 32;
    }
    void ReplayScroll()
    {
        const auto projection = m_projection;
        if ( !Owner() || !projection || projection->revision != m_revision ||
             !projection->nativeScrollReady ||
             !projection->scroll || projection->staged || !projection->view.IsLoaded() ||
             m_replayingScroll ) return;
        const auto ensureKey = m_ensureVisible;
        if ( !ensureKey && (!projection->pendingScroll || projection->scrollRequestIssued) ) return;
        m_replayingScroll = true;
        const auto replay = wxMakeGuard([&]() { m_replayingScroll = false; });
        try
        {
            if ( ensureKey )
            {
                if ( !Find(ensureKey) ) { m_ensureVisible = 0; return; }
                const auto current = [&]()
                {
                    return Owner() && m_projection == projection &&
                        projection->revision == m_revision && m_ensureVisible == ensureKey;
                };
                for ( const auto& entry : projection->entries )
                    if ( entry.key == ensureKey )
                    {
                        projection->pendingScroll = false;
                        projection->scrollRequestIssued = false;
                        projection->view.ScrollIntoView(entry.item);
                        if ( !current() ) return;
                        m_host.ForceRender();
                        if ( !current() ) return;
                        projection->view.UpdateLayout();
                        if ( !current() ) return;
                        UpdateScroll();
                        if ( current() ) m_ensureVisible = 0;
                        return;
                    }
                return;
            }
            const auto target = winrt::box_value(projection->offset).as<WF::IReference<double>>();
            projection->scrollRequestIssued = projection->scroll.ChangeView(projection->horizontal ? target : nullptr,
                projection->horizontal ? nullptr : target, nullptr, true);
            UpdateScroll();
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("ribbon gallery pending scroll", e); }
    }
    void EnsureVisible(std::uint64_t key)
    {
        auto self = shared_from_this();
        const auto projection = m_projection;
        if ( !Owner() || !projection || !Find(key) ) return;
        // Store model identity, not a staged GridViewItem. Before mounting the
        // projection can be replaced, and ScrollIntoView on that old view has
        // no effect on its successor. Loaded/SizeChanged replay on the current
        // native viewport using the same scrolling path as public offsets.
        m_ensureVisible = key;
        projection->pendingScroll = false;
        projection->scrollRequestIssued = false;
        try
        {
            UpdateScroll();
            ReplayScroll();
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("ribbon gallery ensure visible", e); }
    }
    wxRibbonGalleryButtonState ButtonState(size_t index)
    {
        if ( !Owner() || !Owner()->IsEnabled() ) return wxRIBBON_GALLERY_BUTTON_DISABLED;
        try { UpdateScroll(); }
        catch ( const winrt::hresult_error& e ) { wxWinUILogException("ribbon gallery scroll state", e); }
        return m_projection ? m_projection->states[index] : wxRIBBON_GALLERY_BUTTON_DISABLED;
    }
    wxSize NextSize(wxOrientation direction, wxSize relative, bool larger) const
    {
        if ( !m_projection ) return relative;
        const auto& p = *m_projection;
        const int strip = p.minimum.x - p.cell.x;
        const int cols = std::max(1, (relative.x - strip) / p.cell.x);
        const int rows = std::max(1, relative.y / p.cell.y);
        if ( larger && static_cast<size_t>(cols) * rows >= p.entries.size() ) return relative;
        wxSize result = relative;
        if ( direction & wxHORIZONTAL )
            result.x = (larger ? cols + 1 : std::max(1, (relative.x - strip - 1) / p.cell.x)) * p.cell.x + strip;
        if ( direction & wxVERTICAL )
            result.y = (larger ? rows + 1 : std::max(1, (relative.y - 1) / p.cell.y)) * p.cell.y;
        if ( result.x < p.minimum.x || result.y < p.minimum.y ) return relative;
        return result;
    }

    bool Rebuild()
    {
        if ( m_building || !Owner() ) return false;
        auto self = shared_from_this();
        auto* owner = Owner();
        bool nativeComposite = false;
        bool pendingMount = false;
#if defined(WXWINUI_RIBBON_TOOLKIT)
        wxRibbonControl* compositeOwner = nullptr;
        MUXC::ContentControl mount{nullptr};
        const auto status = wxRibbonContainerWinUI::CommandMount(owner, compositeOwner, mount);
        pendingMount = status == wxRibbonContainerWinUI::MountStatus::Pending;
        if ( status == wxRibbonContainerWinUI::MountStatus::Failed ||
             (pendingMount && m_host.GetContent()) ) return false;
        nativeComposite = pendingMount || status == wxRibbonContainerWinUI::MountStatus::Ready;
        if ( !pendingMount && nativeComposite && m_projection &&
             m_projection->revision == m_revision &&
             m_projection->available == owner->GetClientSize() &&
             m_host.GetContent() == m_projection->root )
        {
            auto* host = wxWinUITopLevelHost::FindSlotOwner(owner);
            auto* slot = host ? host->FindSlot(owner) : nullptr;
            if ( slot && mount.Content() == slot->GetPresentationRoot() )
            {
                m_observed = m_projection->available = owner->GetClientSize();
                return true;
            }
        }
#endif
        if ( !nativeComposite && m_projection && m_projection->revision == m_revision &&
             m_projection->available == owner->GetClientSize() &&
             m_host.GetContent() == m_projection->root ) return true;
        m_building = true;
        const auto building = wxMakeGuard([&]()
        {
            m_building = false;
            if ( std::exchange(m_deferred, false) ) ScheduleRepair();
        });
        const auto previous = m_projection;
        const auto previousHosted = m_host.GetContent();
        const auto candidate = std::make_shared<Projection>();
        candidate->revision = m_revision;
        candidate->generation = ++m_nextGeneration;
        candidate->staged = pendingMount;
        const auto current = [&]() { return Owner() && m_revision == candidate->revision; };
        bool attempted = false;
        bool committed = false;
        const auto rollback = wxMakeGuard([&]()
        {
            if ( committed ) return;
            if ( m_projection == candidate ) m_projection = previous;
            if ( !attempted || !Owner() ) return;
            try
            {
                if ( m_host.GetContent() == candidate->root )
                {
                    if ( previousHosted )
                    {
                        if ( !Attach(previousHosted) ) { Close(); return; }
                    }
                    else m_host.ClearContent();
                }
                if ( Owner() && previousHosted && m_host.GetContent() == previousHosted )
                    m_host.ForceRender();
            }
            catch ( const winrt::hresult_error& e )
                { wxWinUILogException("ribbon gallery rollback", e); Close(); }
        });
        try
        {
            candidate->horizontal = owner->m_art &&
                (owner->m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL);
            if ( !current() ) return false;
            owner = Owner();
            for ( auto* item : owner->m_items )
                candidate->entries.push_back({item->key, item->GetId(), item->GetBitmapBundle()});
            candidate->root = MUXC::Grid();
            candidate->view = MUXC::GridView();
            candidate->root.FlowDirection(owner->GetLayoutDirection() == wxLayout_RightToLeft
                ? MUX::FlowDirection::RightToLeft : MUX::FlowDirection::LeftToRight);
            candidate->view.SelectionMode(MUXC::ListViewSelectionMode::Single);
            candidate->view.IsItemClickEnabled(true);
            candidate->view.Padding({0, 0, 0, 0});
            candidate->view.BorderThickness({0, 0, 0, 0});
            candidate->view.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
            candidate->view.VerticalAlignment(MUX::VerticalAlignment::Stretch);
            MUXC::ScrollViewer::SetHorizontalScrollMode(candidate->view, candidate->horizontal
                ? MUXC::ScrollMode::Enabled : MUXC::ScrollMode::Disabled);
            MUXC::ScrollViewer::SetVerticalScrollMode(candidate->view, candidate->horizontal
                ? MUXC::ScrollMode::Disabled : MUXC::ScrollMode::Enabled);
            MUXC::ScrollViewer::SetHorizontalScrollBarVisibility(candidate->view, MUXC::ScrollBarVisibility::Hidden);
            MUXC::ScrollViewer::SetVerticalScrollBarVisibility(candidate->view, MUXC::ScrollBarVisibility::Hidden);
            MUXC::ColumnDefinition commands;
            commands.Width(MUX::GridLengthHelper::Auto());
            MUXC::ColumnDefinition viewport;
            viewport.Width({1, MUX::GridUnitType::Star});
            candidate->root.ColumnDefinitions().Append(viewport);
            candidate->root.ColumnDefinitions().Append(commands);
            candidate->root.Children().Append(candidate->view);
            const std::weak_ptr<wxRibbonGalleryWinUI> weak = self;
            const auto generation = candidate->generation;
            wxSize nativeCell(72, 40);
            for ( auto& entry : candidate->entries )
            {
                const auto bitmap = entry.bitmap.GetBitmapFor(Owner());
                if ( !current() ) return false;
                const auto size = bitmap.GetDIPSize();
                nativeCell.x = std::max(1, size.x) + 8;
                nativeCell.y = std::max(1, size.y) + 8;
                entry.item = MUXC::GridViewItem();
                entry.item.Padding({4, 4, 4, 4});
                entry.item.Margin({0, 0, 0, 0});
                entry.item.MinWidth(0);
                entry.item.MinHeight(0);
                entry.item.Width(nativeCell.x);
                entry.item.Height(nativeCell.y);
                MUXC::Image image;
                image.Source(wxWinUIWriteableBitmapFromBitmap(bitmap));
                image.Width(std::max(1, size.x));
                image.Height(std::max(1, size.y));
                entry.item.Content(image);
                MUXA::AutomationProperties::SetName(entry.item,
                    wxWinUIToHString(wxString::Format("Gallery item %d", entry.id)));
                const auto key = entry.key;
                const auto item = entry.item;
                const auto entered = item.PointerEntered([weak, generation, key](const auto&, const auto&)
                    { if ( auto live = weak.lock() ) live->Hover(generation, key); });
                const auto exited = item.PointerExited([weak, generation, key](const auto&, const auto&)
                    { if ( auto live = weak.lock(); live && live->m_hovered == key ) live->Hover(generation, 0); });
                const MUX::Input::PointerEventHandler pressed = [weak, generation, key](const auto&, const auto&)
                    { if ( auto live = weak.lock(); live && live->Current(generation) ) live->m_active = key; };
                const MUX::Input::PointerEventHandler released = [weak, key](const auto&, const auto&)
                    { if ( auto live = weak.lock(); live && live->m_active == key ) live->m_active = 0; };
                item.AddHandler(MUX::UIElement::PointerPressedEvent(), winrt::box_value(pressed), true);
                item.AddHandler(MUX::UIElement::PointerReleasedEvent(), winrt::box_value(released), true);
                const auto canceled = item.PointerCanceled([weak, key](const auto&, const auto&)
                    { if ( auto live = weak.lock(); live && live->m_active == key ) live->m_active = 0; });
                const auto captureLost = item.PointerCaptureLost([weak, key](const auto&, const auto&)
                    { if ( auto live = weak.lock(); live && live->m_active == key ) live->m_active = 0; });
                candidate->revoke.push_back([item, entered, exited, pressed, released, canceled, captureLost]()
                {
                    item.PointerEntered(entered); item.PointerExited(exited);
                    item.RemoveHandler(MUX::UIElement::PointerPressedEvent(), winrt::box_value(pressed));
                    item.RemoveHandler(MUX::UIElement::PointerReleasedEvent(), winrt::box_value(released));
                    item.PointerCanceled(canceled); item.PointerCaptureLost(captureLost);
                });
                candidate->view.Items().Append(item);
                if ( entry.key == m_selected ) candidate->view.SelectedItem(item);
                if ( !current() ) return false;
            }
            MUXC::StackPanel buttons;
            buttons.Orientation(MUXC::Orientation::Vertical);
            MUXC::Grid::SetColumn(buttons, 1);
            candidate->root.Children().Append(buttons);
            const wchar_t* names[] = {L"Gallery scroll up", L"Gallery scroll down", L"Gallery extension"};
            const wchar_t* glyphs[] = {L"\uE70E", L"\uE70D", L"\uE712"};
            for ( size_t i = 0; i < 3; ++i )
            {
                const auto button = MUXC::Button();
                button.Padding({0, 0, 0, 0}); button.MinWidth(0); button.MinHeight(0);
                button.Width(24); button.Height(24);
                MUXC::FontIcon icon; icon.Glyph(glyphs[i]); icon.FontSize(12);
                button.Content(icon);
                MUXA::AutomationProperties::SetName(button, names[i]);
                candidate->buttons[i] = button;
                buttons.Children().Append(button);
                const auto clicked = button.Click([weak, generation, i](const auto&, const auto&)
                {
                    if ( auto live = weak.lock(); live && live->Current(generation) )
                    {
                        try
                        {
                            if ( i < 2 ) live->Scroll((i == 0 ? -1.0 : 1.0) * live->LineSize());
                            else if ( auto* owner = live->Owner(); owner && owner->IsEnabled() )
                            {
                                wxCommandEvent event(wxEVT_BUTTON, owner->GetId());
                                event.SetEventObject(owner); owner->ProcessWindowEvent(event);
                            }
                        }
                        catch ( ... ) { wxLogError("Exception in ribbon gallery button handler"); }
                    }
                });
                const auto entered = button.PointerEntered([weak, generation, i](const auto&, const auto&)
                {
                    if ( auto live = weak.lock(); live && live->Current(generation) &&
                         live->m_projection->states[i] != wxRIBBON_GALLERY_BUTTON_DISABLED )
                        live->m_projection->states[i] = wxRIBBON_GALLERY_BUTTON_HOVERED;
                });
                const auto exited = button.PointerExited([weak, generation, i](const auto&, const auto&)
                {
                    if ( auto live = weak.lock(); live && live->Current(generation) &&
                         live->m_projection->states[i] != wxRIBBON_GALLERY_BUTTON_DISABLED )
                        live->m_projection->states[i] = wxRIBBON_GALLERY_BUTTON_NORMAL;
                });
                const MUX::Input::PointerEventHandler pressed = [weak, generation, i](const auto&, const auto&)
                {
                    if ( auto live = weak.lock(); live && live->Current(generation) &&
                         live->m_projection->states[i] != wxRIBBON_GALLERY_BUTTON_DISABLED )
                        live->m_projection->states[i] = wxRIBBON_GALLERY_BUTTON_ACTIVE;
                };
                const MUX::Input::PointerEventHandler released = [weak, generation, i](const auto&, const auto&)
                {
                    if ( auto live = weak.lock(); live && live->Current(generation) &&
                         live->m_projection->states[i] != wxRIBBON_GALLERY_BUTTON_DISABLED )
                        live->m_projection->states[i] = wxRIBBON_GALLERY_BUTTON_NORMAL;
                };
                button.AddHandler(MUX::UIElement::PointerPressedEvent(), winrt::box_value(pressed), true);
                button.AddHandler(MUX::UIElement::PointerReleasedEvent(), winrt::box_value(released), true);
                button.AddHandler(MUX::UIElement::PointerCanceledEvent(), winrt::box_value(released), true);
                button.AddHandler(MUX::UIElement::PointerCaptureLostEvent(), winrt::box_value(released), true);
                candidate->revoke.push_back([button, clicked, entered, exited, pressed, released]()
                {
                    button.Click(clicked); button.PointerEntered(entered); button.PointerExited(exited);
                    button.RemoveHandler(MUX::UIElement::PointerPressedEvent(), winrt::box_value(pressed));
                    button.RemoveHandler(MUX::UIElement::PointerReleasedEvent(), winrt::box_value(released));
                    button.RemoveHandler(MUX::UIElement::PointerCanceledEvent(), winrt::box_value(released));
                    button.RemoveHandler(MUX::UIElement::PointerCaptureLostEvent(), winrt::box_value(released));
                });
            }
            const auto view = candidate->view;
            const auto selection = view.SelectionChanged([weak, generation](const auto&, const auto&)
                { if ( auto live = weak.lock() ) live->SelectionChanged(generation); });
            const auto click = view.ItemClick([weak, generation](const auto&, const MUXC::ItemClickEventArgs& event)
                { if ( auto live = weak.lock() ) live->Click(generation, event.ClickedItem()); });
            const auto loaded = view.Loaded([weak, generation](const auto&, const auto&)
            {
                if ( auto live = weak.lock(); live && live->Current(generation) )
                {
                    live->UpdateScroll();
                    live->ReplayScroll();
                }
            });
            const auto resized = view.SizeChanged([weak, generation](const auto&, const auto&)
            {
                if ( auto live = weak.lock(); live && live->Current(generation) )
                {
                    live->UpdateScroll();
                    live->ReplayScroll();
                }
            });
            candidate->revoke.push_back([view, selection, click, loaded, resized]()
                { view.SelectionChanged(selection); view.ItemClick(click); view.Loaded(loaded); view.SizeChanged(resized); });
            const auto root = candidate->root;
            const auto entered = root.PointerEntered([weak, generation](const auto&, const auto&)
                { if ( auto live = weak.lock(); live && live->Current(generation) ) live->m_isHovered = true; });
            const auto exited = root.PointerExited([weak, generation](const auto&, const auto&)
            {
                if ( auto live = weak.lock(); live && live->Current(generation) )
                    { live->m_isHovered = false; live->m_active = 0; live->Hover(generation, 0); }
            });
            candidate->revoke.push_back([root, entered, exited]()
                { root.PointerEntered(entered); root.PointerExited(exited); });
            if ( !current() ) return false;
            owner = Owner();
            candidate->cell = owner->FromDIP(nativeCell);
            candidate->minimum = owner->FromDIP(wxSize(nativeCell.x + 24, std::max(72, nativeCell.y)));
            candidate->best = owner->FromDIP(wxSize(nativeCell.x * 3 + 24, std::max(72, nativeCell.y)));
            candidate->available = m_observed = owner->GetClientSize();
            wxSize desired = candidate->available;
            if ( nativeComposite )
            {
                // Group.Content needs a bounded viewport, not a whole-model
                // desired size. A deliberate smaller shell remains meaningful
                // for Gallery overflow, unlike an uninitialized default size.
                const auto available = candidate->available;
                desired = available.x >= candidate->minimum.x && available.y >= candidate->minimum.y
                    ? available : candidate->best;
                const auto dip = owner->ToDIP(desired);
                candidate->root.Width(dip.x); candidate->root.Height(dip.y);
            }
            // These exact cell dimensions are shared by the public wx layout
            // model and the native virtualizing panel. Public scrolling must
            // work before any XamlRoot exists; it records a bounded desired
            // viewport, which is reconciled with actual extent on mounting.
            const wxString panel = wxString::Format(
                "<ItemsPanelTemplate xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\">"
                "<ItemsWrapGrid Orientation=\"%s\" ItemWidth=\"%d\" ItemHeight=\"%d\" />"
                "</ItemsPanelTemplate>", candidate->horizontal ? wxS("Vertical") : wxS("Horizontal"),
                nativeCell.x, nativeCell.y);
            candidate->view.ItemsPanel(MUX::Markup::XamlReader::Load(
                wxWinUIToHString(panel)).as<MUXC::ItemsPanelTemplate>());
            if ( !current() ) return false;
            const auto dip = Owner()->ToDIP(desired);
            const double viewportWidth = std::max(1, dip.x - 24);
            const double viewportHeight = std::max(1, dip.y);
            const double across = std::max(1.0, std::floor(candidate->horizontal
                ? viewportHeight / nativeCell.y : viewportWidth / nativeCell.x));
            const double bands = std::ceil(static_cast<double>(candidate->entries.size()) / across);
            const double extent = bands * (candidate->horizontal ? nativeCell.x : nativeCell.y);
            candidate->limit = std::max(0.0, extent -
                (candidate->horizontal ? viewportWidth : viewportHeight));
            // Prepare the native template when possible. Before first mount
            // its presenter may still have zero extent: the bounded public
            // model above remains authoritative until native layout is ready.
            candidate->view.ApplyTemplate();
            if ( !current() ) return false;
            attempted = !pendingMount;
            if ( !(pendingMount || Attach(root)) || !current() ) return false;
            m_projection = candidate;
            Owner()->InvalidateBestSize();
            m_host.ForceRender();
            if ( !current() ) return false;
            candidate->view.UpdateLayout();
            if ( !current() ) return false;
            UpdateScroll();
            if ( previous )
            {
                // Preserve the requested absolute view, not a delta added to
                // a new GridView's implicit selection/initial scroll position.
                candidate->offset = std::clamp(previous->offset, 0.0, candidate->limit);
                candidate->pendingScroll = true;
                candidate->scrollRequestIssued = false;
            }
            ReplayScroll();
            if ( !current() ) return false;
            committed = true;
            return true;
        }
        catch ( const winrt::hresult_error& e )
            { wxWinUILogException("ribbon gallery realization", e); return false; }
    }

private:
    wxWeakRef<wxRibbonGallery> m_owner;
    wxWinUIControlHost m_host;
    std::shared_ptr<Projection> m_projection;
    std::uint64_t m_revision = 0;
    std::uint64_t m_nextGeneration = 0;
    std::uint64_t m_selected = 0;
    std::uint64_t m_hovered = 0;
    std::uint64_t m_active = 0;
    std::uint64_t m_ensureVisible = 0;
    wxSize m_observed{wxDefaultSize};
    bool m_isHovered = false;
    bool m_settingSelection = false;
    bool m_building = false;
    bool m_pending = false;
    bool m_deferred = false;
    bool m_repairRunning = false;
    bool m_replayingScroll = false;
};
