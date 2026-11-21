/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/headerprojection.cpp
// Purpose:     Retained XAML presentation of the public wxHeaderCtrl model
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_HEADERCTRL
#include "headerprojection.h"
#include "wx/headerctrl.h"
#include "wx/image.h"
#include "wx/scopeguard.h"
#include "wx/settings.h"
#include "wx/weakref.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"
#include "private.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <set>
#include <vector>

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

namespace
{
struct InvalidHeaderSnapshot {};

#if wxUSE_IMAGE
bool SameHeaderImage(const wxImage& a, const wxImage& b)
{
    if ( !a.IsOk() || !b.IsOk() ) return a.IsOk() == b.IsOk();
    if ( a.GetSize() != b.GetSize() || a.HasAlpha() != b.HasAlpha() ||
         a.HasMask() != b.HasMask() ) return false;
    if ( a.HasMask() && (a.GetMaskRed() != b.GetMaskRed() ||
         a.GetMaskGreen() != b.GetMaskGreen() || a.GetMaskBlue() != b.GetMaskBlue()) ) return false;
    const size_t pixels = static_cast<size_t>(a.GetWidth()) * a.GetHeight();
    return std::memcmp(a.GetData(), b.GetData(), pixels * 3) == 0 &&
        (!a.HasAlpha() || std::memcmp(a.GetAlpha(), b.GetAlpha(), pixels) == 0);
}
#endif

int AddHeaderWidth(int x, int width)
{
    const long long sum = static_cast<long long>(x) + width;
    return static_cast<int>(std::min(sum,
        static_cast<long long>(std::numeric_limits<int>::max())));
}

// Never make an INT_MAX-wide native button or intermediate painted surface.
// Only its visible area is interactive; the unpainted content grid retains the
// logical width so alignment and clipping still refer to the original column.
wxRect VisibleHeaderRect(int x, int width, const wxSize& viewport)
{
    const long long left = std::max(0LL, static_cast<long long>(x));
    const long long right = std::min(static_cast<long long>(viewport.x),
        static_cast<long long>(x) + width);
    if ( right <= left || viewport.y <= 0 ) return wxRect();
    return wxRect(static_cast<int>(left), 0, static_cast<int>(right - left), viewport.y);
}
}

struct wxWinUIHeaderProjection::Impl : std::enable_shared_from_this<Impl>
{
    struct Snapshot
    {
        std::uint64_t revision, topology;
        unsigned count;
        wxArrayInt order;
        wxSize viewport;
        wxFont font;
        wxColour foreground;
        bool customForeground;
        double scale;
        int scroll;
    };
    struct Column
    {
        unsigned index;
        wxRect rect;
        int start = 0, width = 0;
        wxString title;
        wxBitmapBundle bundle;
        wxSize bitmapLogicalSize;
        wxBitmap bitmap;
        wxAlignment alignment = wxALIGN_LEFT;
        bool sorted = false, ascending = false;
    };
    struct Peer
    {
        unsigned index;
        std::uint64_t topology = 0;
        C::Button button{nullptr};
        C::Grid content{nullptr};
        C::Image image{nullptr};
        C::TextBlock text{nullptr};
        C::FontIcon sort{nullptr};
        X::Media::TranslateTransform translation{nullptr};
        X::Media::RectangleGeometry clip{nullptr};
        Column painted;
#if wxUSE_IMAGE
        wxImage paintedImage;
#endif
        wxFont font;
        wxColour foreground;
        bool customForeground = false;
        double scale = 0;
    };

    explicit Impl(wxHeaderCtrl* header) : owner(header) {}
    wxWeakRef<wxHeaderCtrl> owner;
    wxWinUIControlHost host;
    C::Grid root{nullptr};
    C::Canvas canvas{nullptr};
    X::Media::RectangleGeometry rootClip{nullptr};
    C::Border ghost{nullptr}, marker{nullptr};
    std::map<unsigned, std::shared_ptr<Peer>> peers;
    std::uint64_t revision = 0, topology = 0, publishedRevision = 0;
    std::uint64_t inputSerial = 0;
    bool published = false, closed = false, dirty = true;
    bool scheduled = false, synchronizing = false, inputActive = false;

    bool Alive() const { return !closed && owner && !owner->IsBeingDeleted(); }
    void Check(const Snapshot& s) const
    {
        if ( !Alive() || revision != s.revision || topology != s.topology ||
             owner->GetColumnCount() != s.count ) throw InvalidHeaderSnapshot{};
    }
    Snapshot Capture() const
    {
        return {revision, topology, owner->GetColumnCount(), owner->m_colIndices,
                owner->GetClientSize(), owner->GetFont(), owner->GetForegroundColour(),
                owner->UseForegroundColour(), owner->GetDPIScaleFactor(), owner->m_scrollOffset};
    }
    bool IsPublished() const
    {
        if ( !Alive() || !published || !host.IsOk() || host.GetContent() != root ) return false;
        const auto* top = wxWinUITopLevelHost::FindSlotOwner(owner.get());
        const auto* slot = top ? top->FindSlot(owner.get()) : nullptr;
        return slot && slot->GetContent() == root;
    }
    void Invalidate(bool structure = false)
    {
        if ( !Alive() ) return;
        ++revision;
        if ( structure ) ++topology;
        dirty = true;
        if ( scheduled || synchronizing ) return;
        scheduled = true;
        const std::weak_ptr<Impl> weak = shared_from_this();
        owner->CallAfter([weak]()
        {
            if ( auto self = weak.lock() )
            {
                self->scheduled = false;
                self->Synchronize();
            }
        });
    }

    void ObserveInput()
    {
        const std::weak_ptr<Impl> weak = shared_from_this();
        const auto begin = [weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
            {
                ++self->inputSerial;
                self->inputActive = true;
            }
        };
        const auto end = [weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock(); self && self->Alive() )
            {
                const auto serial = self->inputSerial;
                self->owner->CallAfter([weak, serial]()
                {
                    if ( auto current = weak.lock(); current && current->inputSerial == serial )
                        current->inputActive = false;
                });
            }
        };
        // wx's existing host route owns the physical gestures. Observe even
        // handled input so Button.Click cannot emit a second public event.
        root.AddHandler(X::UIElement::PointerPressedEvent(), winrt::box_value(X::Input::PointerEventHandler(begin)), true);
        root.AddHandler(X::UIElement::PointerReleasedEvent(), winrt::box_value(X::Input::PointerEventHandler(end)), true);
        root.AddHandler(X::UIElement::PointerCanceledEvent(), winrt::box_value(X::Input::PointerEventHandler(end)), true);
        root.AddHandler(X::UIElement::PointerCaptureLostEvent(), winrt::box_value(X::Input::PointerEventHandler(end)), true);
        root.AddHandler(X::UIElement::KeyDownEvent(), winrt::box_value(X::Input::KeyEventHandler(begin)), true);
        root.AddHandler(X::UIElement::KeyUpEvent(), winrt::box_value(X::Input::KeyEventHandler(end)), true);
    }

    bool Initialize()
    {
        if ( !host.Initialize(owner.get()) || !Alive() ) return false;
        root = C::Grid();
        canvas = C::Canvas();
        rootClip = X::Media::RectangleGeometry();
        root.Clip(rootClip);
        canvas.Background(wxWinUICreateColourBrush(wxColour(0, 0, 0, 0)));
        A::AutomationProperties::SetAutomationId(root, L"wxHeader.NativeRoot");
        root.Children().Append(canvas);
        ObserveInput();
        const std::weak_ptr<Impl> weak = shared_from_this();
        owner->Bind(wxEVT_SIZE, [weak](wxSizeEvent& event)
        {
            if ( auto self = weak.lock() ) self->Invalidate();
            event.Skip();
        });
        owner->Bind(wxEVT_DPI_CHANGED, [weak](wxDPIChangedEvent& event)
        {
            if ( auto self = weak.lock() ) self->Invalidate();
            event.Skip();
        });
        owner->Bind(wxEVT_SYS_COLOUR_CHANGED, [weak](wxSysColourChangedEvent& event)
        {
            if ( auto self = weak.lock() ) self->Invalidate();
            event.Skip();
        });
        const bool mounted = host.SetContent(root);
        if ( !Alive() || !mounted ) return false;
        published = true;
        // Do not call a derived class's virtual column getter during Create().
        // Its constructor/model must first have completed publication.
        Invalidate();
        return true;
    }

    std::vector<Column> Read(const Snapshot& s)
    {
        std::vector<Column> columns;
        int x = s.scroll;
        for ( const int index : s.order )
        {
            Check(s);
            const wxHeaderColumn& source = owner->GetColumn(index);
            Check(s);
            const bool hidden = source.IsHidden(); Check(s);
            if ( hidden ) continue;
            Column col;
            col.index = static_cast<unsigned>(index);
            col.width = source.GetWidth(); Check(s);
            // Copy every extensible getter before native setters. No column
            // reference crosses a callback without checking the generation.
            col.title = source.GetTitle(); Check(s);
            col.bundle = source.GetBitmapBundle(); Check(s);
            col.alignment = source.GetAlignment(); Check(s);
            col.sorted = source.IsSortKey(); Check(s);
            if ( col.sorted ) { col.ascending = source.IsSortOrderAscending(); Check(s); }
            if ( col.width < 0 )
            {
                wxHeaderColumnSimple copy(col.title, col.width, col.alignment);
                copy.SetBitmap(col.bundle);
                col.width = owner->GetEffectiveColumnWidth(copy);
                Check(s);
            }
            col.width = std::max(0, col.width);
            col.start = x;
            col.rect = VisibleHeaderRect(x, col.width, s.viewport);
            x = AddHeaderWidth(x, col.width);
            if ( col.rect.IsEmpty() ) continue;
            if ( col.bundle.IsOk() )
            {
                // A bundle implementation may change pixels without changing
                // identity or dimensions. Resolve it for every model snapshot.
                col.bitmapLogicalSize = col.bundle.GetPreferredLogicalSizeFor(owner.get()); Check(s);
                col.bitmap = col.bundle.GetBitmapFor(owner.get()); Check(s);
            }
            columns.push_back(std::move(col));
        }
        return columns;
    }

    void Activate(const std::shared_ptr<Peer>& peer)
    {
        if ( !IsPublished() || synchronizing || inputActive || revision != publishedRevision ||
             !owner->IsEnabled() || !owner->IsShownOnScreen() || peer->topology != topology ) return;
        const auto found = peers.find(peer->index);
        if ( found == peers.end() || found->second != peer ) return;
        wxHeaderCtrlEvent event(wxEVT_HEADER_CLICK, owner->GetId());
        event.SetEventObject(owner.get());
        event.SetColumn(peer->index);
        owner->HandleWindowEvent(event);
        // No implicit sorting or post-callback access to the public header.
    }

    std::shared_ptr<Peer> MakePeer(const Column& col)
    {
        auto peer = std::make_shared<Peer>();
        peer->index = col.index;
        peer->button = C::Button();
        peer->button.IsTabStop(false); // public header excludes keyboard traversal
        peer->button.MinWidth(0); peer->button.MinHeight(0);
        peer->button.Padding({0, 0, 0, 0});
        peer->button.HorizontalContentAlignment(X::HorizontalAlignment::Left);
        peer->button.VerticalContentAlignment(X::VerticalAlignment::Stretch);
        peer->clip = X::Media::RectangleGeometry();
        peer->button.Clip(peer->clip);
        peer->content = C::Grid();
        peer->content.Padding({6, 0, 6, 0});
        for ( unsigned n = 0; n != 3; ++n )
        {
            C::ColumnDefinition definition;
            definition.Width({1, n == 1 ? X::GridUnitType::Star : X::GridUnitType::Auto});
            peer->content.ColumnDefinitions().Append(definition);
        }
        peer->translation = X::Media::TranslateTransform();
        peer->content.RenderTransform(peer->translation);
        peer->image = C::Image();
        peer->image.VerticalAlignment(X::VerticalAlignment::Center);
        peer->image.Margin({0, 0, 4, 0});
        peer->text = C::TextBlock();
        peer->text.VerticalAlignment(X::VerticalAlignment::Center);
        peer->text.TextTrimming(X::TextTrimming::CharacterEllipsis);
        C::Grid::SetColumn(peer->text, 1);
        peer->sort = C::FontIcon();
        peer->sort.FontFamily(X::Media::FontFamily(L"Segoe Fluent Icons"));
        peer->sort.FontSize(12);
        peer->sort.Margin({4, 0, 0, 0});
        C::Grid::SetColumn(peer->sort, 2);
        peer->content.Children().Append(peer->image);
        peer->content.Children().Append(peer->text);
        peer->content.Children().Append(peer->sort);
        peer->button.Content(peer->content);
        A::AutomationProperties::SetAutomationId(peer->button,
            wxWinUIToHString(wxString::Format("wxHeader.Column.%u", col.index)));
        A::AutomationProperties::SetAutomationId(peer->sort,
            wxWinUIToHString(wxString::Format("wxHeader.Sort.%u", col.index)));
        const std::weak_ptr<Impl> weak = shared_from_this();
        const std::weak_ptr<Peer> weakPeer = peer;
        peer->button.Click([weak, weakPeer](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
                if ( auto current = weakPeer.lock() ) self->Activate(current);
        });
        return peer;
    }

    void Publish(const std::vector<Column>& columns, const Snapshot& s)
    {
        rootClip.Rect({0, 0, static_cast<float>(std::max(0, s.viewport.x) / s.scale),
                            static_cast<float>(std::max(0, s.viewport.y) / s.scale)});
        Check(s);
        std::set<unsigned> wanted;
        for ( const auto& col : columns ) wanted.insert(col.index);
        for ( auto it = peers.begin(); it != peers.end(); )
        {
            const auto peer = it->second;
            if ( !wanted.count(it->first) || peer->topology != s.topology )
            {
                it = peers.erase(it); // provider is inert before native Unloaded
                uint32_t position = 0;
                if ( canvas.Children().IndexOf(peer->button, position) )
                    canvas.Children().RemoveAt(position);
                Check(s);
            }
            else ++it;
        }
        for ( const auto& col : columns )
        {
            Check(s);
            const auto found = peers.find(col.index);
            const bool added = found == peers.end();
            const auto peer = added ? MakePeer(col) : found->second;
            Check(s);
            peer->topology = s.topology;
            if ( added || peer->painted.rect != col.rect || peer->painted.width != col.width ||
                 peer->painted.start != col.start || peer->scale != s.scale )
            {
                peer->button.Width(col.rect.width / s.scale); Check(s);
                peer->button.Height(col.rect.height / s.scale); Check(s);
                C::Canvas::SetLeft(peer->button, col.rect.x / s.scale); Check(s);
                C::Canvas::SetTop(peer->button, 0); Check(s);
                peer->clip.Rect({0, 0, static_cast<float>(col.rect.width / s.scale),
                                      static_cast<float>(col.rect.height / s.scale)}); Check(s);
                peer->content.Width(col.width / s.scale); Check(s);
                peer->translation.X((col.start - static_cast<double>(col.rect.x)) / s.scale); Check(s);
            }
            if ( added || peer->painted.title != col.title )
            {
                peer->text.Text(wxWinUIToHString(col.title)); Check(s);
                A::AutomationProperties::SetName(peer->button, wxWinUIToHString(col.title)); Check(s);
            }
            if ( added || peer->font != s.font || peer->scale != s.scale )
            {
                wxWinUIApplyFont(peer->text, s.font); Check(s);
            }
            if ( s.customForeground && (added || !peer->customForeground || peer->foreground != s.foreground) )
            {
                wxWinUIApplyForeground(peer->button, s.foreground); Check(s);
            }
            else if ( !s.customForeground && peer->customForeground )
            {
                peer->button.ClearValue(C::Control::ForegroundProperty()); Check(s);
            }
            if ( added || peer->painted.alignment != col.alignment )
            {
                peer->text.TextAlignment(col.alignment == wxALIGN_RIGHT ? X::TextAlignment::Right :
                    col.alignment == wxALIGN_CENTER ? X::TextAlignment::Center : X::TextAlignment::Left); Check(s);
            }
#if wxUSE_IMAGE
            const wxImage image = col.bitmap.IsOk() ? col.bitmap.ConvertToImage() : wxImage();
            const bool imageChanged = added || !SameHeaderImage(peer->paintedImage, image);
#else
            const bool imageChanged = true;
#endif
            if ( imageChanged )
            {
                peer->image.Source(col.bitmap.IsOk() ? wxWinUIWriteableBitmapFromBitmap(col.bitmap) : nullptr); Check(s);
                peer->image.Visibility(col.bitmap.IsOk() ? X::Visibility::Visible : X::Visibility::Collapsed); Check(s);
#if wxUSE_IMAGE
                peer->paintedImage = image;
#endif
            }
            if ( col.bitmap.IsOk() && (added || peer->painted.bitmapLogicalSize != col.bitmapLogicalSize || peer->scale != s.scale) )
            {
                // Match GetColumnTitleWidth(): this size is in wx window
                // coordinates, converted exactly once into XAML DIPs.
                peer->image.Width(col.bitmapLogicalSize.x / s.scale); Check(s);
                peer->image.Height(col.bitmapLogicalSize.y / s.scale); Check(s);
            }
            if ( added || peer->painted.sorted != col.sorted || peer->painted.ascending != col.ascending )
            {
                peer->sort.Glyph(col.ascending ? L"\xE70E" : L"\xE70D"); Check(s);
                peer->sort.Visibility(col.sorted ? X::Visibility::Visible : X::Visibility::Collapsed); Check(s);
            }
            peer->painted = col;
            peer->font = s.font; peer->scale = s.scale;
            peer->foreground = s.foreground; peer->customForeground = s.customForeground;
            if ( added )
            {
                peers.emplace(col.index, peer);
                canvas.Children().Append(peer->button);
                Check(s);
            }
        }
        publishedRevision = s.revision;
    }

    void Synchronize()
    {
        if ( !IsPublished() || synchronizing ) return;
        synchronizing = true;
        const auto finish = wxMakeGuard([this]() { synchronizing = false; });
        // One repair pass handles a reentrant mutation without allowing a
        // pathological virtual getter to refill the dispatcher indefinitely.
        for ( unsigned attempt = 0; dirty && attempt != 2; ++attempt )
        {
            dirty = false;
            try
            {
                const auto s = Capture();
                const auto columns = Read(s);
                Check(s);
                Publish(columns, s);
            }
            catch ( const InvalidHeaderSnapshot& )
            {
                if ( !Alive() ) return;
                dirty = true;
            }
            catch ( const winrt::hresult_error& error )
            {
                wxWinUILogException("header native presentation", error);
                return;
            }
        }
    }

    void ClearMarkers()
    {
        const auto previousGhost = ghost;
        const auto previousMarker = marker;
        ghost = nullptr; marker = nullptr;
        if ( !Alive() || (!previousGhost && !previousMarker) ) return;
        const std::weak_ptr<Impl> weak = shared_from_this();
        // EndDragging still has to clear the public gesture and release wx
        // capture. Retire the exact native markers after that stack unwinds:
        // Unloaded handlers must not destroy the owner in its half-ended state.
        owner->CallAfter([weak, previousGhost, previousMarker]()
        {
            const auto self = weak.lock();
            if ( !self || !self->Alive() ) return;
            for ( const auto& element : {previousGhost, previousMarker} )
            {
                uint32_t position = 0;
                if ( element && self->canvas.Children().IndexOf(element, position) )
                    self->canvas.Children().RemoveAt(position);
                if ( !self->Alive() ) return;
            }
        });
    }

    void ShowMarkers(const wxRect& rect, int insertionX)
    {
        if ( !IsPublished() ) return;
        const auto s = Capture();
        const wxRect clipped = VisibleHeaderRect(rect.x, rect.width, s.viewport);
        try
        {
            if ( !ghost )
            {
                ghost = C::Border();
                ghost.IsHitTestVisible(false);
                ghost.BorderThickness({1, 1, 1, 1});
                ghost.BorderBrush(wxWinUICreateColourBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)));
                A::AutomationProperties::SetAutomationId(ghost, L"wxHeader.ReorderGhost");
                C::Canvas::SetZIndex(ghost, 1);
                canvas.Children().Append(ghost); Check(s);
            }
            ghost.Width(clipped.width / s.scale); Check(s);
            ghost.Height(clipped.height / s.scale); Check(s);
            C::Canvas::SetLeft(ghost, clipped.x / s.scale); Check(s);
            if ( insertionX >= 0 )
            {
                if ( !marker )
                {
                    marker = C::Border();
                    marker.IsHitTestVisible(false);
                    marker.Background(wxWinUICreateColourBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT)));
                    A::AutomationProperties::SetAutomationId(marker, L"wxHeader.ReorderMarker");
                    C::Canvas::SetZIndex(marker, 2);
                    canvas.Children().Append(marker); Check(s);
                }
                marker.Width(4); Check(s);
                marker.Height(std::max(0, s.viewport.y) / s.scale); Check(s);
                C::Canvas::SetLeft(marker, insertionX / s.scale - 2); Check(s);
            }
        }
        catch ( const InvalidHeaderSnapshot& ) { }
        catch ( const winrt::hresult_error& error )
        {
            wxWinUILogException("header native reorder marker", error);
        }
    }

    void Close()
    {
        if ( closed ) return;
        closed = true; published = false; ++revision;
        peers.clear();
        host.Close();
        owner = nullptr;
        ghost = nullptr; marker = nullptr;
    }
};

wxWinUIHeaderProjection::wxWinUIHeaderProjection(std::shared_ptr<Impl> impl) : m_impl(std::move(impl)) {}
wxWinUIHeaderProjection::~wxWinUIHeaderProjection() { Close(); }
std::shared_ptr<wxWinUIHeaderProjection> wxWinUIHeaderProjection::Create(wxHeaderCtrl* owner)
{
    const auto impl = std::make_shared<Impl>(owner);
    auto projection = std::shared_ptr<wxWinUIHeaderProjection>(new wxWinUIHeaderProjection(impl));
    try
    {
        if ( impl->Initialize() ) return projection;
    }
    catch ( const winrt::hresult_error& error )
    {
        wxWinUILogException("header native initialization", error);
    }
    return nullptr;
}
void wxWinUIHeaderProjection::Invalidate(bool topologyChanged) { m_impl->Invalidate(topologyChanged); }
void wxWinUIHeaderProjection::Update() { const auto impl = m_impl; impl->Synchronize(); }
void wxWinUIHeaderProjection::Close() { m_impl->Close(); }
bool wxWinUIHeaderProjection::IsPublished() const { return m_impl->IsPublished(); }
void wxWinUIHeaderProjection::ShowMarkers(const wxRect& ghost, int insertionX)
{
    const auto impl = m_impl;
    impl->ShowMarkers(ghost, insertionX);
}
void wxWinUIHeaderProjection::ClearMarkers() { const auto impl = m_impl; impl->ClearMarkers(); }
#endif
