/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/listautomation.h
// Purpose:     Native report ListView grid/table automation contracts
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WINUI_LISTAUTOMATION_H_
#define _WX_WINUI_LISTAUTOMATION_H_

#include "private.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <UIAutomation.h>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace wxWinUIListAutomation
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation::Peers;
namespace P = X::Automation::Provider;
namespace F = winrt::Windows::Foundation;

inline void Unavailable() { throw winrt::hresult_error(UIA_E_ELEMENTNOTAVAILABLE); }

struct CellBinding
{
    bool header = false;
    std::function<void()> check;
    std::function<std::pair<int, int>()> position;
    std::function<winrt::hstring()> name;
    std::function<F::Rect()> bounds;
    std::function<bool()> enabled;
    std::function<A::AutomationPeer()> grid, headerPeer;
    std::function<void()> realize, invoke, click, focus;
    std::function<bool()> readOnly;
    std::function<void(const winrt::hstring&)> setValue;
};

// Offscreen cells have an on-demand peer, not a hidden XAML control. The same
// peer is returned by the real ContentControl/Button once wx realizes it.
struct CellPeer : A::AutomationPeerT<CellPeer, P::IGridItemProvider,
    P::ITableItemProvider, P::IVirtualizedItemProvider, P::IScrollItemProvider,
    P::IInvokeProvider, P::IValueProvider>
{
    explicit CellPeer(std::shared_ptr<CellBinding> binding) : m_binding(std::move(binding)) {}
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->check();
        if ( !m_binding->header && (pattern == A::PatternInterface::GridItem ||
                                   pattern == A::PatternInterface::TableItem ||
                                   pattern == A::PatternInterface::Value) ) return *this;
        if ( pattern == A::PatternInterface::VirtualizedItem ||
             pattern == A::PatternInterface::ScrollItem ||
             (m_binding->header && pattern == A::PatternInterface::Invoke) ) return *this;
        return nullptr;
    }
    A::AutomationControlType GetAutomationControlTypeCore() const
    {
        m_binding->check();
        return m_binding->header ? A::AutomationControlType::HeaderItem : A::AutomationControlType::DataItem;
    }
    winrt::hstring GetClassNameCore() const
        { m_binding->check(); return m_binding->header ? L"wxListHeader" : L"wxListCell"; }
    winrt::hstring GetNameCore() const
    {
        m_binding->check();
        const auto name = m_binding->name();
        m_binding->check();
        return name;
    }
    bool IsControlElementCore() const { m_binding->check(); return true; }
    bool IsContentElementCore() const { m_binding->check(); return !m_binding->header; }
    bool IsEnabledCore() const { m_binding->check(); return m_binding->enabled(); }
    bool IsKeyboardFocusableCore() const { m_binding->check(); return false; }
    bool HasKeyboardFocusCore() const { m_binding->check(); return false; }
    F::Rect GetBoundingRectangleCore() const { m_binding->check(); return m_binding->bounds(); }
    bool IsOffscreenCore() const
    {
        const auto bounds = GetBoundingRectangleCore();
        return bounds.Width <= 0 || bounds.Height <= 0;
    }
    void SetFocusCore() { m_binding->check(); throw winrt::hresult_error(UIA_E_NOTSUPPORTED); }
    int Row() const { m_binding->check(); return m_binding->position().first; }
    int Column() const { m_binding->check(); return m_binding->position().second; }
    int RowSpan() const { m_binding->check(); return 1; }
    int ColumnSpan() const { m_binding->check(); return 1; }
    P::IRawElementProviderSimple ContainingGrid()
    {
        m_binding->check();
        const auto grid = m_binding->grid();
        m_binding->check();
        return ProviderFromPeer(grid);
    }
    winrt::com_array<P::IRawElementProviderSimple> GetRowHeaderItems()
        { m_binding->check(); return {}; }
    winrt::com_array<P::IRawElementProviderSimple> GetColumnHeaderItems()
    {
        m_binding->check();
        const auto header = m_binding->headerPeer();
        m_binding->check();
        return header ? winrt::com_array<P::IRawElementProviderSimple>({ProviderFromPeer(header)})
                      : winrt::com_array<P::IRawElementProviderSimple>();
    }
    void Realize() { m_binding->check(); m_binding->realize(); m_binding->check(); }
    void ScrollIntoView() { Realize(); }
    bool IsReadOnly() const { m_binding->check(); return m_binding->readOnly(); }
    winrt::hstring Value() const { return GetNameCore(); }
    void SetValue(const winrt::hstring& value)
    {
        m_binding->check();
        if ( !m_binding->enabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
        if ( IsReadOnly() ) throw winrt::hresult_error(UIA_E_INVALIDOPERATION);
        m_binding->setValue(value);
        m_binding->check();
    }
    void Invoke()
    {
        // Keep the existing native header's retained-Invoke contract inert.
        try { m_binding->check(); }
        catch ( const winrt::hresult_error& error )
        { if ( error.code() == UIA_E_ELEMENTNOTAVAILABLE ) return; throw; }
        if ( !m_binding->header ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        if ( !m_binding->enabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
        m_binding->invoke();
    }
private:
    std::shared_ptr<CellBinding> m_binding;
};

struct GridBinding
{
    std::function<void()> check;
    std::function<bool()> multiple;
    std::function<int()> rows, columns;
    std::function<A::AutomationPeer(int, int)> cell;
    std::function<A::AutomationPeer(int)> parentRow;
    std::function<std::vector<A::AutomationPeer>()> children, headers, selection;
};

struct GridPeer : A::ListViewAutomationPeerT<GridPeer, P::IGridProvider, P::ITableProvider,
                                            P::ISelectionProvider>
{
    GridPeer(const C::ListView& owner, std::shared_ptr<GridBinding> binding)
        : ListViewAutomationPeerT(owner), m_binding(std::move(binding)) {}
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->check();
        if ( pattern == A::PatternInterface::Grid || pattern == A::PatternInterface::Table ||
             pattern == A::PatternInterface::Selection ) return *this;
        return ListViewAutomationPeerT::GetPatternCore(pattern);
    }
    A::AutomationControlType GetAutomationControlTypeCore() const
        { m_binding->check(); return A::AutomationControlType::DataGrid; }
    winrt::hstring GetClassNameCore() const { m_binding->check(); return L"wxListCtrlReport"; }
    int RowCount() const { m_binding->check(); return m_binding->rows(); }
    int ColumnCount() const { m_binding->check(); return m_binding->columns(); }
    X::Automation::RowOrColumnMajor RowOrColumnMajor() const
        { m_binding->check(); return X::Automation::RowOrColumnMajor::RowMajor; }
    bool CanSelectMultiple() const { m_binding->check(); return m_binding->multiple(); }
    bool IsSelectionRequired() const { m_binding->check(); return false; }
    winrt::com_array<P::IRawElementProviderSimple> GetSelection()
    {
        m_binding->check();
        const auto rows = m_binding->selection();
        m_binding->check();
        std::vector<P::IRawElementProviderSimple> result;
        result.reserve(rows.size());
        for ( const auto& row : rows )
        {
            row.SetParent(this->try_as<A::AutomationPeer>());
            result.push_back(ProviderFromPeer(row));
            m_binding->check();
        }
        return winrt::com_array<P::IRawElementProviderSimple>(result);
    }
    P::IRawElementProviderSimple GetItem(int row, int column)
    {
        m_binding->check();
        if ( row < 0 || column < 0 || row >= RowCount() || column >= ColumnCount() )
            throw winrt::hresult_invalid_argument();
        const auto cell = m_binding->cell(row, column);
        m_binding->check();
        const auto rowPeer = m_binding->parentRow(row);
        m_binding->check();
        cell.SetParent(rowPeer ? rowPeer : this->try_as<A::AutomationPeer>());
        return ProviderFromPeer(cell);
    }
    F::Collections::IVector<A::AutomationPeer> GetChildrenCore()
    {
        m_binding->check();
        auto children = m_binding->children();
        m_binding->check();
        for ( const auto& child : children ) child.SetParent(this->try_as<A::AutomationPeer>());
        return winrt::single_threaded_vector<A::AutomationPeer>(std::move(children));
    }
    winrt::com_array<P::IRawElementProviderSimple> GetRowHeaders()
        { m_binding->check(); return {}; }
    winrt::com_array<P::IRawElementProviderSimple> GetColumnHeaders()
    {
        m_binding->check();
        std::vector<P::IRawElementProviderSimple> result;
        for ( const auto& header : m_binding->headers() )
        {
            header.SetParent(this->try_as<A::AutomationPeer>());
            result.push_back(ProviderFromPeer(header));
        }
        m_binding->check();
        return winrt::com_array<P::IRawElementProviderSimple>(result);
    }
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
struct CellElement : C::ContentControlT<CellElement>
{
    explicit CellElement(std::function<A::AutomationPeer()> peer) : m_peer(std::move(peer)) {}
    A::AutomationPeer OnCreateAutomationPeer() { return m_peer(); }
private:
    std::function<A::AutomationPeer()> m_peer;
};

struct RowBinding
{
    std::function<void()> check, realize, focus;
    std::function<bool()> selected, enabled, focused;
    std::function<int()> position, count;
    std::function<void(int)> change;
    std::function<winrt::hstring()> name;
    std::function<F::Rect()> bounds;
    std::function<A::AutomationPeer()> container;
    std::function<std::vector<A::AutomationPeer>()> children;
};

// WinUI's data peer represents a logical row without allocating an offscreen
// ListViewItem. The real container returns this same peer when wx realizes it.
struct RowPeer : A::ListViewItemDataAutomationPeerT<RowPeer,
    P::ISelectionItemProvider, P::IVirtualizedItemProvider, P::IScrollItemProvider>
{
    RowPeer(const F::IInspectable& item, const A::ListViewBaseAutomationPeer& parent,
            std::shared_ptr<RowBinding> binding)
        : ListViewItemDataAutomationPeerT(item, parent), m_binding(std::move(binding)) {}
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->check();
        if ( pattern == A::PatternInterface::SelectionItem ||
             pattern == A::PatternInterface::VirtualizedItem ||
             pattern == A::PatternInterface::ScrollItem ) return *this;
        return nullptr;
    }
    A::AutomationControlType GetAutomationControlTypeCore() const
        { m_binding->check(); return A::AutomationControlType::DataItem; }
    winrt::hstring GetClassNameCore() const { m_binding->check(); return L"wxListRow"; }
    winrt::hstring GetNameCore() const
    {
        m_binding->check();
        const auto name = m_binding->name();
        m_binding->check();
        return name;
    }
    bool IsControlElementCore() const { m_binding->check(); return true; }
    bool IsContentElementCore() const { m_binding->check(); return true; }
    bool IsEnabledCore() const { m_binding->check(); return m_binding->enabled(); }
    F::Rect GetBoundingRectangleCore() const { m_binding->check(); return m_binding->bounds(); }
    bool IsOffscreenCore() const
    {
        const auto bounds = GetBoundingRectangleCore();
        return bounds.Width <= 0 || bounds.Height <= 0;
    }
    bool IsKeyboardFocusableCore() const { return IsEnabledCore() && !IsOffscreenCore(); }
    bool HasKeyboardFocusCore() const { m_binding->check(); return m_binding->focused(); }
    int GetPositionInSetCore() const { m_binding->check(); return m_binding->position(); }
    int GetSizeOfSetCore() const { m_binding->check(); return m_binding->count(); }
    void SetFocusCore() { m_binding->check(); m_binding->focus(); m_binding->check(); }
    F::Collections::IVector<A::AutomationPeer> GetChildrenCore()
    {
        m_binding->check();
        auto children = m_binding->children();
        m_binding->check();
        for ( const auto& child : children ) child.SetParent(this->try_as<A::AutomationPeer>());
        return winrt::single_threaded_vector<A::AutomationPeer>(std::move(children));
    }
    bool IsSelected() const { m_binding->check(); return m_binding->selected(); }
    P::IRawElementProviderSimple SelectionContainer()
    {
        m_binding->check();
        const auto peer = m_binding->container();
        m_binding->check();
        return ProviderFromPeer(peer);
    }
    void Select() { Change(0); }
    void AddToSelection() { Change(1); }
    void RemoveFromSelection() { Change(2); }
    void Realize() { m_binding->check(); m_binding->realize(); m_binding->check(); }
    void ScrollIntoView() { Realize(); }
private:
    void Change(int operation)
    {
        m_binding->check();
        if ( !m_binding->enabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
        m_binding->change(operation);
        // As for the existing native item peer, a wx selection callback may
        // destroy the control: the completed notification doesn't dispatch again.
    }
    std::shared_ptr<RowBinding> m_binding;
};
struct RowElement : C::ListViewItemT<RowElement>
{
    explicit RowElement(std::function<A::AutomationPeer()> peer) : m_peer(std::move(peer)) {}
    A::AutomationPeer OnCreateAutomationPeer() { return m_peer(); }
private:
    std::function<A::AutomationPeer()> m_peer;
};
// Headers remain native Button peers: preserve focus and the native control
// contract, including when UIA obtained the header before it was mounted.
struct HeaderPeer : A::ButtonAutomationPeerT<HeaderPeer, P::IInvokeProvider,
    P::IVirtualizedItemProvider, P::IScrollItemProvider>
{
    HeaderPeer(const C::Button& owner, std::shared_ptr<CellBinding> binding)
        : ButtonAutomationPeerT(owner), m_binding(std::move(binding)) {}
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->check();
        if ( pattern == A::PatternInterface::Invoke || pattern == A::PatternInterface::VirtualizedItem ||
             pattern == A::PatternInterface::ScrollItem ) return *this;
        return ButtonAutomationPeerT::GetPatternCore(pattern);
    }
    A::AutomationControlType GetAutomationControlTypeCore() const
        { m_binding->check(); return A::AutomationControlType::HeaderItem; }
    winrt::hstring GetNameCore() const
    {
        m_binding->check();
        const auto name = m_binding->name();
        m_binding->check();
        return name;
    }
    F::Rect GetBoundingRectangleCore() const { m_binding->check(); return m_binding->bounds(); }
    bool IsOffscreenCore() const
    {
        const auto bounds = GetBoundingRectangleCore();
        return bounds.Width <= 0 || bounds.Height <= 0;
    }
    bool IsEnabledCore() const { m_binding->check(); return m_binding->enabled(); }
    bool IsKeyboardFocusableCore() const
    {
        m_binding->check();
        return !IsOffscreenCore() && ButtonAutomationPeerT::IsKeyboardFocusableCore();
    }
    bool HasKeyboardFocusCore() const
        { m_binding->check(); return ButtonAutomationPeerT::HasKeyboardFocusCore(); }
    void SetFocusCore()
    {
        m_binding->check();
        if ( !m_binding->enabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
        m_binding->focus();
        m_binding->check();
    }
    void Invoke()
    {
        try { m_binding->check(); }
        catch ( const winrt::hresult_error& error )
        { if ( error.code() == UIA_E_ELEMENTNOTAVAILABLE ) return; throw; }
        if ( !m_binding->enabled() ) throw winrt::hresult_error(UIA_E_ELEMENTNOTENABLED);
        m_binding->invoke();
    }
    void Realize() { m_binding->check(); m_binding->realize(); m_binding->check(); }
    void ScrollIntoView() { Realize(); }
private:
    std::shared_ptr<CellBinding> m_binding;
};
struct HeaderElement : C::ButtonT<HeaderElement>
{
    explicit HeaderElement(std::shared_ptr<CellBinding> binding) : m_binding(std::move(binding))
        { Click([binding = m_binding](const auto&, const auto&) { binding->click(); }); }
    A::AutomationPeer OnCreateAutomationPeer()
        { return winrt::make<HeaderPeer>(this->try_as<C::Button>(), m_binding); }
private:
    std::shared_ptr<CellBinding> m_binding;
};
}
#endif
