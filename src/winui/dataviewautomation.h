/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/dataviewautomation.h
// Purpose:     Native tabular automation peers for the WinUI DataView
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_DATAVIEWAUTOMATION_H_
#define _WX_WINUI_DATAVIEWAUTOMATION_H_

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <UIAutomation.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace wxWinUIDataViewAutomation
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation::Peers;
namespace P = X::Automation::Provider;
namespace F = winrt::Windows::Foundation;

inline void Unavailable() { throw winrt::hresult_error(UIA_E_ELEMENTNOTAVAILABLE); }

struct CellBinding
{
    struct Range { double value, minimum, maximum; };
    std::function<bool()> current;
    std::function<std::pair<int, int>()> coordinates;
    std::function<winrt::hstring()> name;
    std::function<F::Rect()> bounds;
    std::function<X::FrameworkElement()> element;
    std::function<A::AutomationPeer()> grid;
    std::function<A::AutomationPeer()> header;
    std::function<void()> realize;
    std::function<bool()> enabled;
    std::function<bool()> supportsValue, supportsRange, readOnly;
    std::function<winrt::hstring()> value;
    std::function<void(const winrt::hstring&)> setValue;
    std::function<Range()> range;
    std::function<void(double)> setRange;

    void Check() const { if ( !current || !current() ) Unavailable(); }
};

// Value and RangeValue have different Value() return types. This native
// pattern object shares the cell's exact lifetime/model binding, without
// manufacturing a second control or a separate edit authority.
struct CellRangeValue : winrt::implements<CellRangeValue, P::IRangeValueProvider>
{
    explicit CellRangeValue(std::shared_ptr<CellBinding> binding) : m_binding(std::move(binding)) {}
    void Check() const
    {
        m_binding->Check();
        if ( !m_binding->supportsRange() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    }
    bool IsReadOnly() const { Check(); return m_binding->readOnly(); }
    double Value() const { Check(); return m_binding->range().value; }
    double Minimum() const { Check(); return m_binding->range().minimum; }
    double Maximum() const { Check(); return m_binding->range().maximum; }
    double SmallChange() const { Check(); return 1; }
    double LargeChange() const { Check(); return 10; }
    void SetValue(double value) { Check(); m_binding->setRange(value); }
private:
    std::shared_ptr<CellBinding> m_binding;
};

inline void CollectChildren(const X::DependencyObject& root,
                            const F::Collections::IVector<A::AutomationPeer>& peers)
{
    for ( int n = 0; n < X::Media::VisualTreeHelper::GetChildrenCount(root); ++n )
    {
        const auto child = X::Media::VisualTreeHelper::GetChild(root, n);
        const auto element = child.try_as<X::FrameworkElement>();
        const auto peer = element ? A::FrameworkElementAutomationPeer::CreatePeerForElement(element) : nullptr;
        if ( peer ) peers.Append(peer);
        else CollectChildren(child, peers);
    }
}

// A cell is a logical model item, not a hidden/dummy FrameworkElement. The
// same native peer is attached by CellElement when its visual is realized.
struct CellPeer : A::AutomationPeerT<CellPeer, P::IGridItemProvider,
    P::ITableItemProvider, P::IVirtualizedItemProvider, P::IScrollItemProvider, P::IValueProvider>
{
    explicit CellPeer(std::shared_ptr<CellBinding> binding) : m_binding(std::move(binding)) {}

    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->Check();
        if ( pattern == A::PatternInterface::GridItem || pattern == A::PatternInterface::TableItem ||
             pattern == A::PatternInterface::VirtualizedItem || pattern == A::PatternInterface::ScrollItem ) return *this;
        if ( pattern == A::PatternInterface::Value && m_binding->supportsValue() ) return *this;
        if ( pattern == A::PatternInterface::RangeValue && m_binding->supportsRange() )
            return winrt::make<CellRangeValue>(m_binding);
        return nullptr;
    }
    A::AutomationControlType GetAutomationControlTypeCore() const
        { m_binding->Check(); return A::AutomationControlType::DataItem; }
    winrt::hstring GetClassNameCore() const { m_binding->Check(); return L"wxDataViewCell"; }
    winrt::hstring GetNameCore() const { m_binding->Check(); return m_binding->name(); }
    bool IsControlElementCore() const { m_binding->Check(); return true; }
    bool IsContentElementCore() const { m_binding->Check(); return true; }
    bool IsEnabledCore() const { m_binding->Check(); return m_binding->enabled(); }
    bool IsKeyboardFocusableCore() const { m_binding->Check(); return false; }
    bool HasKeyboardFocusCore() const { m_binding->Check(); return false; }
    F::Rect GetBoundingRectangleCore() const { m_binding->Check(); return m_binding->bounds(); }
    bool IsOffscreenCore() const
    {
        const auto rect = GetBoundingRectangleCore();
        return rect.Width <= 0 || rect.Height <= 0;
    }
    F::Collections::IVector<A::AutomationPeer> GetChildrenCore() const
    {
        m_binding->Check();
        const auto peers = winrt::single_threaded_vector<A::AutomationPeer>();
        if ( auto element = m_binding->element() ) CollectChildren(element, peers);
        m_binding->Check();
        return peers;
    }
    void SetFocusCore() { m_binding->Check(); throw winrt::hresult_error(UIA_E_NOTSUPPORTED); }
    int Row() const { m_binding->Check(); return m_binding->coordinates().first; }
    int Column() const { m_binding->Check(); return m_binding->coordinates().second; }
    int RowSpan() const { m_binding->Check(); return 1; }
    int ColumnSpan() const { m_binding->Check(); return 1; }
    P::IRawElementProviderSimple ContainingGrid()
    {
        m_binding->Check();
        return ProviderFromPeer(m_binding->grid());
    }
    winrt::com_array<P::IRawElementProviderSimple> GetColumnHeaderItems()
    {
        m_binding->Check();
        const auto header = m_binding->header();
        if ( !header ) return {};
        return {ProviderFromPeer(header)};
    }
    winrt::com_array<P::IRawElementProviderSimple> GetRowHeaderItems()
        { m_binding->Check(); return {}; }
    void Realize() { m_binding->Check(); m_binding->realize(); m_binding->Check(); }
    void ScrollIntoView() { Realize(); }
    bool IsReadOnly() const { CheckValue(); return m_binding->readOnly(); }
    winrt::hstring Value() const { CheckValue(); return m_binding->value(); }
    void SetValue(const winrt::hstring& value) { CheckValue(); m_binding->setValue(value); }
private:
    void CheckValue() const
    {
        m_binding->Check();
        if ( !m_binding->supportsValue() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
    }
    std::shared_ptr<CellBinding> m_binding;
};

struct CellElement : C::GridT<CellElement>
{
    explicit CellElement(std::function<A::AutomationPeer()> peer) : m_peer(std::move(peer)) {}
    A::AutomationPeer OnCreateAutomationPeer() { return m_peer ? m_peer() : nullptr; }
private:
    std::function<A::AutomationPeer()> m_peer;
};

struct RowBinding
{
    std::function<bool()> current;
    std::function<winrt::hstring()> name;
    std::function<std::vector<A::AutomationPeer>()> cells;
    std::function<void()> realize;
    void Check() const { if ( !current || !current() ) Unavailable(); }
};

struct RowPeer : A::ListViewItemDataAutomationPeerT<RowPeer,
    P::IVirtualizedItemProvider, P::IScrollItemProvider>
{
    RowPeer(const F::IInspectable& item, const A::ListViewBaseAutomationPeer& parent,
            std::shared_ptr<RowBinding> binding)
        : ListViewItemDataAutomationPeerT(item, parent), m_binding(std::move(binding)) {}
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->Check();
        if ( pattern == A::PatternInterface::VirtualizedItem || pattern == A::PatternInterface::ScrollItem ) return *this;
        return ListViewItemDataAutomationPeerT::GetPatternCore(pattern);
    }
    winrt::hstring GetNameCore() const { m_binding->Check(); return m_binding->name(); }
    F::Collections::IVector<A::AutomationPeer> GetChildrenCore()
    {
        m_binding->Check();
        auto cells = m_binding->cells();
        for ( const auto& cell : cells ) cell.SetParent(this->try_as<A::AutomationPeer>());
        m_binding->Check();
        return winrt::single_threaded_vector<A::AutomationPeer>(std::move(cells));
    }
    void Realize() { m_binding->Check(); m_binding->realize(); m_binding->Check(); }
    void ScrollIntoView() { Realize(); }
private:
    std::shared_ptr<RowBinding> m_binding;
};

struct GridBinding
{
    std::function<bool()> current;
    std::function<std::uint64_t()> version;
    std::function<int()> rowCount, columnCount;
    std::function<F::IInspectable(int)> item;
    std::function<A::AutomationPeer(int, int)> cell;
    std::function<std::shared_ptr<RowBinding>(const F::IInspectable&)> row;
    std::function<std::vector<A::AutomationPeer>(bool)> headers;
    void Check() const { if ( !current || !current() ) Unavailable(); }
};

struct GridPeer : A::ListViewAutomationPeerT<GridPeer, P::IGridProvider, P::ITableProvider>
{
    GridPeer(const C::ListView& owner, std::shared_ptr<GridBinding> binding)
        : ListViewAutomationPeerT(owner), m_binding(std::move(binding)) {}
    A::AutomationControlType GetAutomationControlTypeCore() const
        { m_binding->Check(); return A::AutomationControlType::DataGrid; }
    winrt::hstring GetClassNameCore() const { m_binding->Check(); return L"wxDataViewCtrl"; }
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->Check();
        if ( pattern == A::PatternInterface::Grid || pattern == A::PatternInterface::Table ) return *this;
        return ListViewAutomationPeerT::GetPatternCore(pattern);
    }
    int RowCount() const { m_binding->Check(); return m_binding->rowCount(); }
    int ColumnCount() const { m_binding->Check(); return m_binding->columnCount(); }
    P::IRawElementProviderSimple GetItem(int row, int column)
    {
        m_binding->Check();
        const auto version = m_binding->version();
        if ( row < 0 || column < 0 || row >= RowCount() || column >= ColumnCount() )
            throw winrt::hresult_invalid_argument();
        if ( version != m_binding->version() ) Unavailable();
        const auto item = m_binding->item(row);
        const auto peer = m_binding->cell(row, column);
        if ( version != m_binding->version() ) Unavailable();
        const auto parent = CreateItemAutomationPeer(item);
        m_binding->Check();
        if ( version != m_binding->version() ) Unavailable();
        peer.SetParent(parent);
        return ProviderFromPeer(peer);
    }
    A::ItemAutomationPeer OnCreateItemAutomationPeer(const F::IInspectable& item)
    {
        m_binding->Check();
        return winrt::make<RowPeer>(item, this->try_as<A::ListViewBaseAutomationPeer>(), m_binding->row(item));
    }
    F::Collections::IVector<A::AutomationPeer> GetChildrenCore()
    {
        m_binding->Check();
        auto children = winrt::single_threaded_vector<A::AutomationPeer>();
        for ( const auto& header : m_binding->headers(false) )
        {
            header.SetParent(this->try_as<A::AutomationPeer>());
            children.Append(header);
        }
        const auto rows = ListViewAutomationPeerT::GetChildrenCore();
        if ( rows ) for ( const auto& row : rows ) children.Append(row);
        m_binding->Check();
        return children;
    }
    X::Automation::RowOrColumnMajor RowOrColumnMajor() const
        { m_binding->Check(); return X::Automation::RowOrColumnMajor::RowMajor; }
    winrt::com_array<P::IRawElementProviderSimple> GetColumnHeaders()
    {
        m_binding->Check();
        std::vector<P::IRawElementProviderSimple> result;
        for ( const auto& header : m_binding->headers(true) )
        {
            header.SetParent(this->try_as<A::AutomationPeer>());
            result.push_back(ProviderFromPeer(header));
        }
        m_binding->Check();
        return winrt::com_array<P::IRawElementProviderSimple>(result);
    }
    winrt::com_array<P::IRawElementProviderSimple> GetRowHeaders()
        { m_binding->Check(); return {}; }
private:
    std::shared_ptr<GridBinding> m_binding;
};

struct GridElement : C::ListViewT<GridElement>
{
    explicit GridElement(std::shared_ptr<GridBinding> binding) : m_binding(std::move(binding)) {}
    A::AutomationPeer OnCreateAutomationPeer()
        { return winrt::make<GridPeer>(this->try_as<C::ListView>(), m_binding); }
private:
    std::shared_ptr<GridBinding> m_binding;
};
}

#endif // _WX_WINUI_DATAVIEWAUTOMATION_H_
