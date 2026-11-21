/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/gridautomation.h
// Purpose:     Real tabular automation peers for the native wxGrid projection
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef _WX_WINUI_GRIDAUTOMATION_H_
#define _WX_WINUI_GRIDAUTOMATION_H_

// Apply the Win32 macro guards before loading any WinRT projection. The
// native control subclasses also need the full ToggleButton producer.
#include "private.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <UIAutomation.h>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace wxWinUIGridAutomation
{
namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation::Peers;
namespace P = X::Automation::Provider;
namespace F = winrt::Windows::Foundation;

inline void Unavailable() { throw winrt::hresult_error(UIA_E_ELEMENTNOTAVAILABLE); }

struct ItemBinding
{
    struct Position { int row, column, rowSpan, columnSpan; };
    bool header = false;
    std::function<void()> check;
    std::function<Position()> position;
    std::function<winrt::hstring()> name;
    std::function<F::Rect()> bounds;
    std::function<A::AutomationPeer()> grid;
    std::function<std::vector<A::AutomationPeer>(bool)> headers;
    std::function<bool()> enabled, toggle, invokable;
    std::function<bool()> hasValue, readOnly;
    std::function<winrt::hstring()> value;
    std::function<void(const winrt::hstring&)> setValue;
    std::function<X::Automation::ToggleState()> toggleState;
    std::function<void()> realize, activate;
};

// A logical cell is not an invisible placeholder control. Its peer exists on
// demand and is returned by the real native Button/CheckBox when materialized.
struct ItemPeer : A::AutomationPeerT<ItemPeer, P::IGridItemProvider,
    P::ITableItemProvider, P::IVirtualizedItemProvider, P::IScrollItemProvider,
    P::IInvokeProvider, P::IToggleProvider, P::IValueProvider>
{
    explicit ItemPeer(std::shared_ptr<ItemBinding> binding) : m_binding(std::move(binding)) {}
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->check();
        if ( !m_binding->header && (pattern == A::PatternInterface::GridItem ||
                                   pattern == A::PatternInterface::TableItem) ) return *this;
        if ( pattern == A::PatternInterface::VirtualizedItem || pattern == A::PatternInterface::ScrollItem ) return *this;
        if ( pattern == A::PatternInterface::Toggle && m_binding->toggle() ) return *this;
        if ( pattern == A::PatternInterface::Invoke && m_binding->invokable() ) return *this;
        if ( pattern == A::PatternInterface::Value && m_binding->hasValue() ) return *this;
        return nullptr;
    }
    A::AutomationControlType GetAutomationControlTypeCore() const
    {
        m_binding->check();
        return m_binding->header ? A::AutomationControlType::HeaderItem : A::AutomationControlType::DataItem;
    }
    winrt::hstring GetClassNameCore() const
        { m_binding->check(); return m_binding->header ? L"wxGridHeader" : L"wxGridCell"; }
    winrt::hstring GetNameCore() const
    {
        m_binding->check();
        const auto value = m_binding->name();
        m_binding->check();
        return value;
    }
    bool IsControlElementCore() const { m_binding->check(); return true; }
    bool IsContentElementCore() const { m_binding->check(); return true; }
    bool IsEnabledCore() const { m_binding->check(); return m_binding->enabled(); }
    bool IsKeyboardFocusableCore() const { m_binding->check(); return false; }
    bool HasKeyboardFocusCore() const { m_binding->check(); return false; }
    F::Rect GetBoundingRectangleCore() const { m_binding->check(); return m_binding->bounds(); }
    bool IsOffscreenCore() const
    {
        const auto rect = GetBoundingRectangleCore();
        return rect.Width <= 0 || rect.Height <= 0;
    }
    void SetFocusCore() { m_binding->check(); throw winrt::hresult_error(UIA_E_NOTSUPPORTED); }
    int Row() const { m_binding->check(); return m_binding->position().row; }
    int Column() const { m_binding->check(); return m_binding->position().column; }
    int RowSpan() const { m_binding->check(); return m_binding->position().rowSpan; }
    int ColumnSpan() const { m_binding->check(); return m_binding->position().columnSpan; }
    P::IRawElementProviderSimple ContainingGrid()
        { m_binding->check(); return ProviderFromPeer(m_binding->grid()); }
    winrt::com_array<P::IRawElementProviderSimple> GetRowHeaderItems() { return Headers(true); }
    winrt::com_array<P::IRawElementProviderSimple> GetColumnHeaderItems() { return Headers(false); }
    void Realize() { m_binding->check(); m_binding->realize(); m_binding->check(); }
    void ScrollIntoView() { Realize(); }
    void Invoke()
    {
        // Preserve the old native Button action contract: a retained Invoke
        // is inert after retirement. Data/table queries still fail explicitly.
        try { m_binding->check(); }
        catch ( const winrt::hresult_error& error )
        {
            if ( error.code() == UIA_E_ELEMENTNOTAVAILABLE ) return;
            throw;
        }
        if ( !m_binding->invokable() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        m_binding->activate();
    }
    void Toggle()
    {
        m_binding->check();
        if ( !m_binding->toggle() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        m_binding->activate();
    }
    X::Automation::ToggleState ToggleState() const
    {
        m_binding->check();
        if ( !m_binding->toggle() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        return m_binding->toggleState();
    }
    winrt::hstring Value() const
    {
        m_binding->check();
        if ( !m_binding->hasValue() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        const auto value = m_binding->value();
        m_binding->check();
        return value;
    }
    bool IsReadOnly() const
    {
        m_binding->check();
        if ( !m_binding->hasValue() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        return m_binding->readOnly();
    }
    void SetValue(const winrt::hstring& value)
    {
        m_binding->check();
        if ( !m_binding->hasValue() ) throw winrt::hresult_error(UIA_E_NOTSUPPORTED);
        m_binding->setValue(value);
    }
private:
    winrt::com_array<P::IRawElementProviderSimple> Headers(bool rows)
    {
        m_binding->check();
        std::vector<P::IRawElementProviderSimple> result;
        for ( const auto& header : m_binding->headers(rows) ) result.push_back(ProviderFromPeer(header));
        m_binding->check();
        return winrt::com_array<P::IRawElementProviderSimple>(result);
    }
    std::shared_ptr<ItemBinding> m_binding;
};

struct GridBinding
{
    std::function<void()> check;
    std::function<int()> rows, columns;
    std::function<winrt::hstring()> name;
    std::function<bool()> enabled;
    std::function<A::AutomationPeer(int, int)> cell;
    std::function<std::vector<A::AutomationPeer>()> children;
    std::function<std::vector<A::AutomationPeer>(bool)> headers;
};

struct GridPeer : A::FrameworkElementAutomationPeerT<GridPeer, P::IGridProvider, P::ITableProvider>
{
    GridPeer(const X::FrameworkElement& owner, std::shared_ptr<GridBinding> binding)
        : FrameworkElementAutomationPeerT(owner), m_binding(std::move(binding)) {}
    F::IInspectable GetPatternCore(A::PatternInterface pattern)
    {
        m_binding->check();
        if ( pattern == A::PatternInterface::Grid || pattern == A::PatternInterface::Table ) return *this;
        return FrameworkElementAutomationPeerT::GetPatternCore(pattern);
    }
    A::AutomationControlType GetAutomationControlTypeCore() const
        { m_binding->check(); return A::AutomationControlType::DataGrid; }
    winrt::hstring GetClassNameCore() const { m_binding->check(); return L"wxGrid"; }
    winrt::hstring GetNameCore() const { m_binding->check(); return m_binding->name(); }
    bool IsEnabledCore() const { m_binding->check(); return m_binding->enabled(); }
    int RowCount() const { m_binding->check(); return m_binding->rows(); }
    int ColumnCount() const { m_binding->check(); return m_binding->columns(); }
    X::Automation::RowOrColumnMajor RowOrColumnMajor() const
        { m_binding->check(); return X::Automation::RowOrColumnMajor::RowMajor; }
    P::IRawElementProviderSimple GetItem(int row, int column)
    {
        m_binding->check();
        if ( row < 0 || column < 0 || row >= RowCount() || column >= ColumnCount() )
            throw winrt::hresult_invalid_argument();
        const auto peer = m_binding->cell(row, column);
        m_binding->check();
        peer.SetParent(this->try_as<A::AutomationPeer>());
        return ProviderFromPeer(peer);
    }
    F::Collections::IVector<A::AutomationPeer> GetChildrenCore()
    {
        m_binding->check();
        auto children = m_binding->children();
        m_binding->check();
        for ( const auto& peer : children ) peer.SetParent(this->try_as<A::AutomationPeer>());
        return winrt::single_threaded_vector<A::AutomationPeer>(std::move(children));
    }
    winrt::com_array<P::IRawElementProviderSimple> GetRowHeaders() { return Headers(true); }
    winrt::com_array<P::IRawElementProviderSimple> GetColumnHeaders() { return Headers(false); }
private:
    winrt::com_array<P::IRawElementProviderSimple> Headers(bool rows)
    {
        m_binding->check();
        std::vector<P::IRawElementProviderSimple> result;
        for ( const auto& peer : m_binding->headers(rows) )
        {
            peer.SetParent(this->try_as<A::AutomationPeer>());
            result.push_back(ProviderFromPeer(peer));
        }
        m_binding->check();
        return winrt::com_array<P::IRawElementProviderSimple>(result);
    }
    std::shared_ptr<GridBinding> m_binding;
};

struct GridElement : C::GridT<GridElement>
{
    explicit GridElement(std::shared_ptr<GridBinding> binding) : m_binding(std::move(binding)) {}
    A::AutomationPeer OnCreateAutomationPeer()
        { return winrt::make<GridPeer>(this->try_as<X::FrameworkElement>(), m_binding); }
private:
    std::shared_ptr<GridBinding> m_binding;
};

// Frozen/header viewports are parts of one logical table, not additional
// grids. Their cells are exposed once, by the primary GridPeer's children.
struct PartPeer : A::FrameworkElementAutomationPeerT<PartPeer>
{
    explicit PartPeer(const X::FrameworkElement& owner) : FrameworkElementAutomationPeerT(owner) {}
    bool IsControlElementCore() const { return false; }
    bool IsContentElementCore() const { return false; }
    F::Collections::IVector<A::AutomationPeer> GetChildrenCore()
        { return winrt::single_threaded_vector<A::AutomationPeer>(); }
};
struct PartElement : C::GridT<PartElement>
{
    A::AutomationPeer OnCreateAutomationPeer()
        { return winrt::make<PartPeer>(this->try_as<X::FrameworkElement>()); }
};

struct ButtonElement : C::ButtonT<ButtonElement>
{
    explicit ButtonElement(std::function<A::AutomationPeer()> peer) : m_peer(std::move(peer)) {}
    A::AutomationPeer OnCreateAutomationPeer() { return m_peer(); }
private:
    std::function<A::AutomationPeer()> m_peer;
};
struct ToggleElement : C::CheckBoxT<ToggleElement>
{
    explicit ToggleElement(std::function<A::AutomationPeer()> peer) : m_peer(std::move(peer)) {}
    A::AutomationPeer OnCreateAutomationPeer() { return m_peer(); }
private:
    std::function<A::AutomationPeer()> m_peer;
};
}
#endif
