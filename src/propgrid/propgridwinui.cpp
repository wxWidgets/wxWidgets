/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgridwinui.cpp
// Purpose:     Viewport-bounded XAML presentation of wxPropertyGrid
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_PROPGRID
#include "propgridwinui.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/private.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/advprops.h"
#include "wx/private/windowlifetime.h"
#include "wx/dcmemory.h"
#include "wx/brush.h"
#include "wx/image.h"
#include "wx/scopeguard.h"
#include "wx/textctrl.h"
#include "wx/weakref.h"
#include "wx/winui/private/appearance.h"
#include "wx/winui/private/tlwhost.h"
#include "../winui/private.h"
#if wxUSE_DATEPICKCTRL
#include "wx/datectrl.h"
#include "../winui/datepresentation.h"
#endif
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#if wxUSE_SPINBTN || wxUSE_DATEPICKCTRL
#include <winrt/Windows.Globalization.NumberFormatting.h>
#endif
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <map>
#include <limits>
#include <set>
#include <typeinfo>
#include <vector>

namespace X = winrt::Microsoft::UI::Xaml;
namespace C = X::Controls;
namespace A = X::Automation;

namespace
{
struct InvalidPropertySnapshot {};

#if wxUSE_DATEPICKCTRL
// Keep the real CalendarDatePicker and its template. DateTimeFormatter cannot
// express arbitrary wx/strftime formats, while GetDisplayInfo may also return
// application-defined or unspecified text. Adapt just the exact DateText part
// after the native control formats its Date; no wx property/window is retained.
class PropertyCalendarPreview : public C::CalendarDatePickerT<PropertyCalendarPreview>
{
public:
    winrt::hstring GetRuntimeClassName() const
    {
        return winrt::hstring(winrt::name_of<C::CalendarDatePicker>());
    }
    void OnApplyTemplate()
    {
        const auto keepAlive = get_strong();
        C::CalendarDatePickerT<PropertyCalendarPreview>::OnApplyTemplate();
        PublishText(true);
    }
    void SetDisplayText(const winrt::hstring& text)
    {
        m_text = text;
        m_hasText = true;
        PublishText(false);
    }
private:
    void PublishText(bool templateApplied)
    {
        const auto text = GetTemplateChild(L"DateText").try_as<C::TextBlock>();
        if ( !text )
        {
            // Absence before template materialization is normal; a completed
            // incompatible template must not silently display a different date
            // format. This adapter is pinned to WinUI's native DateText part.
            if ( templateApplied )
                throw winrt::hresult_error(E_NOTIMPL,
                    L"PropertyGrid CalendarDatePicker requires native DateText");
            return;
        }
        if ( m_hasText && text.Text() != m_text ) text.Text(m_text);
    }
    winrt::hstring m_text;
    bool m_hasText = false;
};
#endif

#if wxUSE_SPINBTN || wxUSE_DATEPICKCTRL
// A NumberBox is only a passive presentation of a registered wx editor.
// Its double Value must never round a 64-bit property's visible text,
// reinterpret a localized display string, or become a second model authority.
// The formatter owns a value snapshot, not a property/window pointer: even a
// retained native peer cannot call back into a retired PropertyGrid.
class PropertyNumberFormatter : public winrt::implements<
    PropertyNumberFormatter,
    winrt::Windows::Globalization::NumberFormatting::INumberFormatter2,
    winrt::Windows::Globalization::NumberFormatting::INumberParser>
{
public:
    PropertyNumberFormatter(const wxString& label, double value, const wxVariant& original)
        : m_label(wxWinUIToHString(label)), m_value(value)
    {
        if ( original.IsType("long") || original.IsType("longlong") )
        {
            m_signed = original.GetLongLong().GetValue();
            m_hasSigned = true;
            if ( m_signed >= 0 )
            {
                m_unsigned = static_cast<std::uint64_t>(m_signed);
                m_hasUnsigned = true;
            }
        }
        else if ( original.IsType("ulonglong") )
        {
            m_unsigned = original.GetULongLong().GetValue();
            m_hasUnsigned = true;
            if ( m_unsigned <= static_cast<std::uint64_t>((std::numeric_limits<std::int64_t>::max)()) )
            {
                m_signed = static_cast<std::int64_t>(m_unsigned);
                m_hasSigned = true;
            }
        }
    }
    winrt::hstring FormatInt(std::int64_t) const { return m_label; }
    winrt::hstring FormatUInt(std::uint64_t) const { return m_label; }
    winrt::hstring FormatDouble(double) const { return m_label; }
    winrt::Windows::Foundation::IReference<std::int64_t>
    ParseInt(const winrt::hstring& text) const
    {
        if ( text != m_label || !m_hasSigned ) return nullptr;
        return winrt::box_value(m_signed).as<winrt::Windows::Foundation::IReference<std::int64_t>>();
    }
    winrt::Windows::Foundation::IReference<std::uint64_t>
    ParseUInt(const winrt::hstring& text) const
    {
        if ( text != m_label || !m_hasUnsigned ) return nullptr;
        return winrt::box_value(m_unsigned).as<winrt::Windows::Foundation::IReference<std::uint64_t>>();
    }
    winrt::Windows::Foundation::IReference<double>
    ParseDouble(const winrt::hstring& text) const
    {
        if ( text != m_label || !std::isfinite(m_value) ) return nullptr;
        return winrt::box_value(m_value).as<winrt::Windows::Foundation::IReference<double>>();
    }
private:
    winrt::hstring m_label;
    double m_value;
    std::int64_t m_signed = 0;
    std::uint64_t m_unsigned = 0;
    bool m_hasSigned = false, m_hasUnsigned = false;
};
#endif

// The existing property/editor epoch also covers native callbacks. In
// particular DeleteProperty() must defer reclamation across a value getter or
// custom renderer which dispatches a nested event loop.
#if wxUSE_DATEPICKCTRL
// Passive counterpart of datectrl.cpp's real wxDP_SPIN composition. The
// controls contain the same calendar components, but no handlers publish wx
// events: selecting this cell creates the sole authoritative wx editor.
struct PropertyDateSpinPreview
{
    C::ContentControl root{nullptr};
    C::Grid layout{nullptr}, fields{nullptr};
    C::NumberBox year{nullptr}, month{nullptr}, day{nullptr}, formatted{nullptr};
    C::ComboBox era{nullptr};
    C::Button clear{nullptr};
    std::vector<C::TextBlock> literals;
    winrt::hstring calendarIdentifier;
    wxDateTime minimum, maximum, shownValue;
    wxFont shownFont;
    wxString shownLabel;
    int firstEra = 0, lastEra = 0;
    bool showCentury = false, valuePrepared = false, formattedPrepared = false;

    static void Prepare(const C::Control& part, const wxString& id)
    {
        part.IsTabStop(false);
        part.IsHitTestVisible(false);
        part.VerticalAlignment(X::VerticalAlignment::Center);
        A::AutomationProperties::SetAccessibilityView(part, A::Peers::AccessibilityView::Raw);
        A::AutomationProperties::SetAutomationId(part, wxWinUIToHString(id));
    }

    PropertyDateSpinPreview(const wxString& name, long style)
    {
        namespace D = wxWinUIDatePresentation;
        namespace WG = winrt::Windows::Globalization;
        namespace WF = winrt::Windows::Foundation;
        // Match the active DatePicker's default calendar and locale policy.
        // This unmounted control only resolves native defaults, never replaces
        // the visible SPIN peer or supplies a hidden model/event authority.
        const C::CalendarDatePicker defaults;
        calendarIdentifier = defaults.CalendarIdentifier();
        WF::DateTime lower{}, upper{};
        if ( !D::wxWinUIResolveCalendarEnvelope(calendarIdentifier, &lower, &upper) )
            throw winrt::hresult_error(E_INVALIDARG, L"PropertyGrid spin calendar envelope");
        minimum = D::wxWinUIFromDateTime(lower);
        maximum = D::wxWinUIFromDateTime(upper);
        showCentury = (style & wxDP_SHOWCENTURY) != 0;
        const wxString pattern = D::wxWinUIGetLocaleShortDatePattern(showCentury, {});
        WG::Calendar calendar;
        calendar.ChangeCalendarSystem(calendarIdentifier);
        firstEra = calendar.FirstEra();
        lastEra = calendar.LastEra();

        root = C::ContentControl();
        root.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
        root.VerticalContentAlignment(X::VerticalAlignment::Center);
        fields = C::Grid();
        layout = C::Grid();
        C::ColumnDefinition valueColumn, clearColumn;
        valueColumn.Width({1, X::GridUnitType::Star});
        clearColumn.Width({1, X::GridUnitType::Auto});
        layout.ColumnDefinitions().Append(valueColumn);
        layout.ColumnDefinitions().Append(clearColumn);
        const wxString prefix = "wxProperty.DateSpin." + name + ".";
        day = D::wxWinUICreateDatePart(1, 31, 64, 1, {});
        month = D::wxWinUICreateDatePart(1, 13, 64, 1, {});
        year = D::wxWinUICreateDatePart(1, 9999, 88, showCentury ? 4 : 2, {});
        Prepare(day, prefix + "Day");
        Prepare(month, prefix + "Month");
        Prepare(year, prefix + "Year");
        A::AutomationProperties::SetName(day, wxWinUIToHString(_("Day")));
        A::AutomationProperties::SetName(month, wxWinUIToHString(_("Month")));
        A::AutomationProperties::SetName(year, wxWinUIToHString(_("Year")));
        if ( !showCentury )
            year.NumberFormatter(winrt::make<D::wxWinUITwoDigitYearFormatter>(
                D::wxWinUIResolveTwoDigitYearMaximum(calendarIdentifier, {},
                    firstEra, lastEra, lower, upper), winrt::hstring()));

        if ( firstEra != lastEra || pattern.Find("{era.") != wxNOT_FOUND )
        {
            era = C::ComboBox();
            era.MinWidth(72);
            era.Margin({2, 0, 2, 0});
            Prepare(era, prefix + "Era");
            A::AutomationProperties::SetName(era, wxWinUIToHString(_("Era")));
            for ( int eraValue = firstEra; eraValue <= lastEra; ++eraValue )
            {
                WF::DateTime anchor{};
                winrt::hstring label;
                if ( D::wxWinUIFindCalendarEraAnchor(calendarIdentifier, eraValue,
                                                     lower, upper, &anchor) )
                {
                    calendar.SetDateTime(anchor);
                    label = calendar.EraAsString();
                }
                if ( label.empty() ) label = wxWinUIToHString(wxString::Format("%d", eraValue));
                era.Items().Append(winrt::box_value(label));
            }
        }

        auto tokens = D::wxWinUIParseDatePattern(pattern);
        if ( era && std::none_of(tokens.begin(), tokens.end(), [](const auto& token)
             { return token.kind == D::wxWinUIDateLayoutKind::Era; }) )
        {
            tokens.insert(tokens.begin(), {D::wxWinUIDateLayoutKind::Literal, " "});
            tokens.insert(tokens.begin(), {D::wxWinUIDateLayoutKind::Era, wxString()});
        }
        int column = 0;
        for ( const auto& token : tokens )
        {
            C::ColumnDefinition definition;
            definition.Width(X::GridLengthHelper::Auto());
            fields.ColumnDefinitions().Append(definition);
            X::FrameworkElement element{nullptr};
            switch ( token.kind )
            {
                case D::wxWinUIDateLayoutKind::Day: element = day; break;
                case D::wxWinUIDateLayoutKind::Month: element = month; break;
                case D::wxWinUIDateLayoutKind::Year: element = year; break;
                case D::wxWinUIDateLayoutKind::Era: element = era; break;
                case D::wxWinUIDateLayoutKind::Literal:
                {
                    C::TextBlock text;
                    text.Text(wxWinUIToHString(token.text));
                    text.Margin({2, 0, 2, 0});
                    text.VerticalAlignment(X::VerticalAlignment::Center);
                    A::AutomationProperties::SetAccessibilityView(text, A::Peers::AccessibilityView::Raw);
                    literals.push_back(text);
                    element = text;
                    break;
                }
            }
            if ( element )
            {
                C::Grid::SetColumn(element, column);
                fields.Children().Append(element);
            }
            ++column;
        }
        // An arbitrary wx display format may include names, time and literal
        // text which cannot be split between three native numeric fields.
        // Use another real, passive NumberBox for that exact display instead
        // of covering spin buttons with an overlay or replacing them by text.
        // Its numeric anchor is the year component, never an editable date.
        formatted = D::wxWinUICreateDatePart(1, 9999, 0, 1, {});
        Prepare(formatted, prefix + "Formatted");
        formatted.HorizontalAlignment(X::HorizontalAlignment::Stretch);
        formatted.Visibility(X::Visibility::Collapsed);
        clear = C::Button();
        clear.Content(winrt::box_value(wxWinUIToHString(_("Clear"))));
        clear.Margin({4, 0, 0, 0});
        clear.MinWidth(52);
        Prepare(clear, prefix + "Clear");
        clear.Visibility((style & wxDP_ALLOWNONE) ? X::Visibility::Visible : X::Visibility::Collapsed);
        C::Grid::SetColumn(clear, 1);
        layout.Children().Append(fields);
        layout.Children().Append(formatted);
        layout.Children().Append(clear);
        root.Content(layout);
    }

    void Present(const wxDateTime& value, const wxString& label, bool exactFormat,
                 long style, const wxFont& font, const std::function<void()>& check)
    {
        namespace D = wxWinUIDatePresentation;
        const wxDateTime civil = D::wxWinUINormalizeCivilDate(value);
        const bool century = (style & wxDP_SHOWCENTURY) != 0;
        if ( century != showCentury )
        {
            if ( century )
            {
                auto formatter = D::wxWinUICreateDateDecimalFormatter({});
                formatter.IntegerDigits(4);
                formatter.FractionDigits(0);
                year.NumberFormatter(formatter);
            }
            else
            {
                winrt::Windows::Foundation::DateTime lower{}, upper{};
                if ( !D::wxWinUIToDateTime(minimum, &lower) || !D::wxWinUIToDateTime(maximum, &upper) )
                    throw winrt::hresult_error(E_INVALIDARG, L"PropertyGrid spin year interval");
                year.NumberFormatter(winrt::make<D::wxWinUITwoDigitYearFormatter>(
                    D::wxWinUIResolveTwoDigitYearMaximum(calendarIdentifier, {},
                        firstEra, lastEra, lower, upper), winrt::hstring()));
            }
            check();
            showCentury = century;
        }
        const bool changed = !valuePrepared || !D::wxWinUISameCivilDate(shownValue, civil);
        if ( changed )
        {
            if ( !D::wxWinUIApplyDateParts(era, year, month, day, firstEra,
                     calendarIdentifier, civil, minimum, maximum, check) )
                throw winrt::hresult_error(E_INVALIDARG, L"PropertyGrid spin civil components");
            check();
            shownValue = civil;
            valuePrepared = true;
        }
        if ( exactFormat && (!formattedPrepared || changed || shownLabel != label) )
        {
            const double numericYear = year.Value();
            const wxVariant numericSnapshot = std::isfinite(numericYear)
                ? wxVariant(static_cast<long>(numericYear)) : wxVariant();
            formatted.NumberFormatter(winrt::make<PropertyNumberFormatter>(label, numericYear, numericSnapshot)); check();
            formatted.Value(numericYear); check();
            formatted.PlaceholderText(civil.IsValid() ? winrt::hstring() : wxWinUIToHString(label)); check();
            shownLabel = label;
            formattedPrepared = true;
        }
        if ( !exactFormat ) formattedPrepared = false;
        fields.Visibility(exactFormat ? X::Visibility::Collapsed : X::Visibility::Visible); check();
        formatted.Visibility(exactFormat ? X::Visibility::Visible : X::Visibility::Collapsed); check();
        clear.Visibility((style & wxDP_ALLOWNONE) ? X::Visibility::Visible : X::Visibility::Collapsed); check();
        if ( shownFont != font )
        {
            for ( const C::Control& part : {year.as<C::Control>(), month.as<C::Control>(),
                                           day.as<C::Control>(), formatted.as<C::Control>(), clear.as<C::Control>()} )
            { wxWinUIApplyFont(part, font); check(); }
            if ( era ) { wxWinUIApplyFont(era, font); check(); }
            for ( const auto& literal : literals ) { wxWinUIApplyFont(literal, font); check(); }
            shownFont = font;
        }
    }
};
#endif

class PropertyCallback final
{
public:
    explicit PropertyCallback(wxPropertyGrid* grid) : m_grid(grid), m_weak(grid)
    { ++wxPGGetPropertyGridTransientState(grid).propertyCallbackDepth; }
    ~PropertyCallback()
    {
        if ( m_weak.get() == m_grid )
            --wxPGGetPropertyGridTransientState(m_grid).propertyCallbackDepth;
    }
private:
    wxPGDeferredEditorCallbackEpoch m_epoch;
    wxPropertyGrid* m_grid;
    wxWeakRef<wxWindow> m_weak;
};

#if wxUSE_IMAGE
bool SamePropertyImage(const wxImage& a, const wxImage& b)
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
}

struct wxPGWinUIProjection::Impl : std::enable_shared_from_this<Impl>
{
    enum class PreviewKind { None, Text, Choice, EditableChoice, Check, Number, Date, DateSpin };
    struct Cell
    {
        C::Button button{nullptr};
        C::Grid content{nullptr};
        C::Image image{nullptr};
        C::TextBlock text{nullptr};
        C::Border selection{nullptr};
        C::Grid previewLayout{nullptr};
        C::Control preview{nullptr};
        C::TextBlock previewLabel{nullptr};
        C::Button previewButton{nullptr};
        PreviewKind previewKind = PreviewKind::None;
#if wxUSE_SPINBTN
        wxString numberLabel;
        double numberValue = std::numeric_limits<double>::quiet_NaN();
        bool numberPrepared = false;
#endif
#if wxUSE_DATEPICKCTRL
        winrt::com_ptr<PropertyCalendarPreview> datePreview;
        std::shared_ptr<PropertyDateSpinPreview> dateSpin;
        winrt::hstring dateLanguage, dateCalendar;
        long dateStyle = 0;
        bool datePrepared = false;
#endif
        X::Media::RectangleGeometry clip{nullptr};
        wxString label;
        wxPGCell appearance;
        wxFont font;
        wxRect rect;
        wxSize renderedSize;
        wxBitmap bitmap;
#if wxUSE_IMAGE
        wxImage pixels;
#endif
        int renderFlags = -1;
        bool custom = false;
        bool populated = false;
    };
    struct Row
    {
        wxPGProperty* property = nullptr;
        wxPropertyGridPageState* page = nullptr;
        wxString name;
        std::uint64_t key = 0;
        bool dirty = true;
        wxRect rect;
        std::vector<std::shared_ptr<Cell>> cells;
        C::Button expander{nullptr};
        C::FontIcon chevron{nullptr};
    };
    struct Snapshot
    {
        std::uint64_t revision;
        wxPropertyGridPageState* page;
        wxSize viewport;
        wxPoint scroll;
        double scale;
        int height;
        unsigned columns;
        wxFont font;
        bool rtl;
    };

    explicit Impl(wxPropertyGrid* grid) : owner(grid) {}
    wxWeakRef<wxPropertyGrid> owner;
    wxWinUIControlHost host;
    C::ContentControl root{nullptr};
    C::Canvas canvas{nullptr};
    X::Style cellStyle{nullptr}, selectedCellStyle{nullptr};
    X::Media::RectangleGeometry clip{nullptr};
    std::map<wxPGProperty*, std::shared_ptr<Row>> rows;
    std::vector<X::UIElement> retired;
    std::uint64_t revision = 0, publishedRevision = 0, nextKey = 0;
    std::uint64_t inputSerial = 0;
    unsigned geometryDepth = 0, removalDepth = 0;
    bool closed = false, published = false, dirty = true;
    bool scheduled = false, synchronizing = false, inputActive = false;

    bool Alive() const
    { return !closed && owner && !owner->IsBeingDeleted(); }
    bool IsPublished() const
    {
        if ( !Alive() || !published || host.GetContent() != root ) return false;
        const auto* top = wxWinUITopLevelHost::FindSlotOwner(owner.get());
        const auto* slot = top ? top->FindSlot(owner.get()) : nullptr;
        return slot && slot->GetContent() == root;
    }
    void Check(const Snapshot& s, wxPGProperty* property = nullptr) const
    {
        if ( !Alive() || revision != s.revision || removalDepth ||
             owner->GetState() != s.page || owner->GetColumnCount() != s.columns ||
             (property && owner->IsPropertyPendingRemoval(property)) )
            throw InvalidPropertySnapshot{};
    }
    Snapshot Capture() const
    {
        wxPoint scroll;
        owner->CalcUnscrolledPosition(0, 0, &scroll.x, &scroll.y);
        return {revision, owner->GetState(), owner->GetClientSize(), scroll,
                owner->GetDPIScaleFactor(), owner->GetRowHeight(),
                owner->GetColumnCount(), owner->GetFont(),
                owner->GetLayoutDirection() == wxLayout_RightToLeft};
    }
    void Schedule()
    {
        if ( !Alive() || scheduled || synchronizing || removalDepth || geometryDepth ) return;
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
    void Invalidate(const wxRect* rect = nullptr)
    {
        if ( !Alive() ) return;
        ++revision; dirty = true;
        if ( !geometryDepth )
            for ( auto& entry : rows )
                if ( !rect || entry.second->rect.Intersects(*rect) )
                    entry.second->dirty = true;
        Schedule();
    }
    void Retire(const std::shared_ptr<Row>& row)
    {
        for ( const auto& cell : row->cells ) retired.push_back(cell->button);
        if ( row->expander ) retired.push_back(row->expander);
    }
    void BeginRemoval(wxPGProperty* property)
    {
        ++removalDepth;
        ++revision; dirty = true;
        for ( auto it = rows.begin(); it != rows.end(); )
        {
            const auto row = it->second;
            if ( !property || (row->page == property->GetParentState() &&
                 (row->property == property || row->property->IsSomeParent(property))) )
            {
                // Revoke callbacks before any native Unloaded or application
                // destructor. Native detach occurs after the wx transaction.
                it = rows.erase(it);
                Retire(row);
            }
            else ++it;
        }
    }
    void ObserveInput()
    {
        const std::weak_ptr<Impl> weak = shared_from_this();
        const auto begin = [weak](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
            { ++self->inputSerial; self->inputActive = true; }
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
        // The existing wx pointer/keyboard route remains the selection and
        // splitter authority. Do not duplicate it with a second Button.Click.
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
        // Retain the actual WinUI Button template/input/UIA behavior, but use
        // a subtle row surface instead of the ordinary raised-button fill.
        // ThemeResource expressions stay attached to each native Style, so
        // light/dark/high-contrast transitions are not frozen RGB snapshots.
        const auto styles = X::Markup::XamlReader::Load(LR"xaml(
<ResourceDictionary xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">
 <Style x:Key="PropertyCell" TargetType="Button" BasedOn="{StaticResource DefaultButtonStyle}">
  <Setter Property="Background" Value="{ThemeResource SubtleFillColorTransparentBrush}" />
  <Setter Property="Foreground" Value="{ThemeResource TextFillColorPrimaryBrush}" />
  <Setter Property="BorderThickness" Value="0" />
  <Setter Property="Padding" Value="0" />
  <Setter Property="CornerRadius" Value="{ThemeResource ControlCornerRadius}" />
 </Style>
 <Style x:Key="PropertySelectedCell" TargetType="Button" BasedOn="{StaticResource PropertyCell}">
  <Setter Property="Background" Value="{ThemeResource SubtleFillColorSecondaryBrush}" />
 </Style>
</ResourceDictionary>)xaml").as<X::ResourceDictionary>();
        if ( !Alive() ) return false;
        cellStyle = styles.Lookup(winrt::box_value(L"PropertyCell")).as<X::Style>();
        selectedCellStyle = styles.Lookup(winrt::box_value(L"PropertySelectedCell")).as<X::Style>();
        if ( !Alive() ) return false;
        root = C::ContentControl(); canvas = C::Canvas();
        // The wx grid remains a focus destination when selection is cleared
        // or its last editor/row is removed. Cells are not individual tab
        // stops, so a Grid-only root would make SetFocus() silently fail and
        // prevent the editor's real focus-loss/retirement transaction.
        root.IsTabStop(true);
        root.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
        root.VerticalContentAlignment(X::VerticalAlignment::Stretch);
        clip = X::Media::RectangleGeometry(); root.Clip(clip);
        canvas.Background(wxWinUICreateColourBrush(wxColour(0, 0, 0, 0)));
        A::AutomationProperties::SetAutomationId(root, L"wxProperty.NativeRoot");
        root.Content(canvas);
        ObserveInput();
        const bool mounted = host.SetContent(root);
        if ( !Alive() || !mounted ) return false;
        host.SetPreferredFocus(root);
        published = true;
        Invalidate();
        return true;
    }
    bool Current(const std::shared_ptr<Row>& row) const
    {
        if ( !IsPublished() || synchronizing || inputActive || removalDepth ||
             revision != publishedRevision || !owner->IsEnabled() || !owner->IsShownOnScreen() ||
             owner->GetState() != row->page ) return false;
        const auto it = rows.find(row->property);
        return it != rows.end() && it->second == row &&
               !owner->IsPropertyPendingRemoval(row->property);
    }
    void Activate(const std::shared_ptr<Row>& row, bool expand)
    {
        if ( !Current(row) ) return;
        PropertyCallback callback(owner.get());
        wxPropertyGrid* const grid = owner.get();
        if ( expand )
        {
            if ( row->property->IsExpanded() ) grid->DoCollapse(row->property, true);
            else grid->DoExpand(row->property, true);
        }
        else grid->DoSelectProperty(row->property, wxPGSelectPropertyFlags::Focus);
        // User callbacks may have deleted the property, page, or entire grid.
        if ( Alive() ) Invalidate();
    }
    C::Button MakeButton(const wxString& id)
    {
        C::Button button;
        button.Style(cellStyle);
        button.IsTabStop(false);
        button.MinWidth(0); button.MinHeight(0);
        button.Padding({0, 0, 0, 0});
        button.BorderThickness({0, 0, 0, 0});
        button.HorizontalContentAlignment(X::HorizontalAlignment::Stretch);
        button.VerticalContentAlignment(X::VerticalAlignment::Stretch);
        A::AutomationProperties::SetAutomationId(button, wxWinUIToHString(id));
        return button;
    }
    std::shared_ptr<Cell> MakeCell(const std::shared_ptr<Row>& row, unsigned column)
    {
        auto cell = std::make_shared<Cell>();
        cell->appearance.SetEmptyData();
        cell->button = MakeButton(wxString::Format("wxProperty.Cell.%s.%u", row->name, column));
        cell->content = C::Grid();
        cell->content.Padding({6, 0, 6, 0});
        for ( unsigned index = 0; index != 2; ++index )
        {
            C::ColumnDefinition definition;
            definition.Width({1, index ? X::GridUnitType::Star : X::GridUnitType::Auto});
            cell->content.ColumnDefinitions().Append(definition);
        }
        cell->image = C::Image();
        cell->image.VerticalAlignment(X::VerticalAlignment::Center);
        cell->image.Margin({0, 0, 4, 0});
        cell->text = C::TextBlock();
        cell->text.VerticalAlignment(X::VerticalAlignment::Center);
        cell->text.TextTrimming(X::TextTrimming::CharacterEllipsis);
        C::Grid::SetColumn(cell->text, 1);
        cell->content.Children().Append(cell->image);
        cell->content.Children().Append(cell->text);
        if ( column == 0 )
        {
            cell->selection = X::Markup::XamlReader::Load(LR"xaml(
<Border xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
 Width="3" Height="16" CornerRadius="2" IsHitTestVisible="False"
 HorizontalAlignment="Left" VerticalAlignment="Center"
 Background="{ThemeResource AccentFillColorDefaultBrush}" />)xaml").as<C::Border>();
            cell->selection.Visibility(X::Visibility::Collapsed);
            A::AutomationProperties::SetAutomationId(cell->selection,
                wxWinUIToHString("wxProperty.Selection." + row->name));
            C::Grid::SetColumnSpan(cell->selection, 2);
            cell->content.Children().Append(cell->selection);
        }
        cell->button.Content(cell->content);
        cell->clip = X::Media::RectangleGeometry();
        cell->button.Clip(cell->clip);
        const std::weak_ptr<Impl> weak = shared_from_this();
        const std::weak_ptr<Row> weakRow = row;
        const std::weak_ptr<Cell> weakCell = cell;
        cell->button.Click([weak, weakRow, weakCell, column](const auto&, const auto&)
        {
            if ( auto self = weak.lock() )
                if ( auto current = weakRow.lock(); current && column < current->cells.size() )
                    if ( auto exact = weakCell.lock(); exact && current->cells[column] == exact )
                        self->Activate(current, false);
        });
        return cell;
    }
    void PublishImage(const std::shared_ptr<Cell>& cell, const wxBitmap& bitmap,
                      const wxSize& size, const Snapshot& s, wxPGProperty* property)
    {
#if wxUSE_IMAGE
        const wxImage pixels = bitmap.IsOk() ? bitmap.ConvertToImage() : wxImage();
        const bool changed = !cell->populated || !SamePropertyImage(cell->pixels, pixels);
#else
        const bool changed = true;
#endif
        if ( changed )
        {
            cell->image.Source(bitmap.IsOk() ? wxWinUIWriteableBitmapFromBitmap(bitmap) : nullptr); Check(s, property);
#if wxUSE_IMAGE
            cell->pixels = pixels;
#endif
        }
        cell->image.Visibility(bitmap.IsOk() ? X::Visibility::Visible : X::Visibility::Collapsed); Check(s, property);
        if ( bitmap.IsOk() )
        {
            cell->image.Width(size.x / s.scale); Check(s, property);
            cell->image.Height(size.y / s.scale); Check(s, property);
        }
        cell->bitmap = bitmap;
    }
    bool PresentEditorPreview(const std::shared_ptr<Row>& row,
                              const std::shared_ptr<Cell>& cell,
                              const Snapshot& s, bool editing)
    {
        auto* const property = row->property;
        const wxPGEditor* const editor = property->GetEditorClass(); Check(s, property);
        PreviewKind kind = PreviewKind::None;
        bool hasButton = false;
        // Test exact registered editors, never names or base classes: custom
        // editors may deliberately provide different controls and semantics.
        if ( editor && !cell->custom )
        {
            if ( editor == wxPGEditor_TextCtrl || editor == wxPGEditor_TextCtrlAndButton )
                kind = PreviewKind::Text;
            else if ( editor == wxPGEditor_Choice || editor == wxPGEditor_ChoiceAndButton )
                kind = PreviewKind::Choice;
            else if ( editor == wxPGEditor_ComboBox )
                kind = PreviewKind::EditableChoice;
#if wxPG_INCLUDE_CHECKBOX
            else if ( editor == wxPGEditor_CheckBox )
                kind = PreviewKind::Check;
#endif
#if wxUSE_SPINBTN
            else if ( editor == wxPGEditor_SpinCtrl && wxDynamicCast(property, wxNumericProperty) )
                kind = PreviewKind::Number;
#endif
#if wxUSE_DATEPICKCTRL
            else if ( editor == wxPGEditor_DatePickerCtrl )
            {
                const auto* const dateProperty = wxDynamicCast(property, wxDateProperty);
                if ( dateProperty )
                    kind = (dateProperty->GetDatePickerStyle() & wxDP_DROPDOWN)
                        ? PreviewKind::Date : PreviewKind::DateSpin;
            }
#endif
            hasButton = editor == wxPGEditor_TextCtrlAndButton || editor == wxPGEditor_ChoiceAndButton;
        }
        if ( (kind == PreviewKind::Text || kind == PreviewKind::Number ||
              kind == PreviewKind::Date || kind == PreviewKind::DateSpin) &&
             property->HasFlag(wxPGFlags::NoEditor) )
            kind = PreviewKind::None;
        if ( cell->previewKind != kind || !!cell->previewButton != hasButton )
        {
            if ( cell->previewLayout )
            {
                uint32_t index = 0;
                if ( cell->content.Children().IndexOf(cell->previewLayout, index) )
                    cell->content.Children().RemoveAt(index);
                Check(s, property);
            }
            cell->preview = nullptr; cell->previewButton = nullptr;
            cell->previewLabel = nullptr;
#if wxUSE_SPINBTN
            cell->numberPrepared = false;
#endif
#if wxUSE_DATEPICKCTRL
            cell->datePreview = nullptr;
            cell->dateSpin.reset();
            cell->datePrepared = false;
#endif
            cell->previewLayout = nullptr; cell->previewKind = kind;
            if ( kind != PreviewKind::None || hasButton )
            {
                cell->previewLayout = C::Grid();
                C::ColumnDefinition valueColumn, buttonColumn;
                valueColumn.Width({1, X::GridUnitType::Star});
                buttonColumn.Width({1, X::GridUnitType::Auto});
                cell->previewLayout.ColumnDefinitions().Append(valueColumn);
                cell->previewLayout.ColumnDefinitions().Append(buttonColumn);
                if ( kind == PreviewKind::Text )
                {
                    C::TextBox text;
                    text.IsReadOnly(true);
                    cell->preview = text;
                }
                else if ( kind == PreviewKind::Choice || kind == PreviewKind::EditableChoice )
                {
                    C::ComboBox choice;
                    choice.IsEditable(kind == PreviewKind::EditableChoice);
                    cell->preview = choice;
                }
                else if ( kind == PreviewKind::Check )
                {
                    C::CheckBox check;
                    check.IsThreeState(true);
                    cell->preview = check;
                }
#if wxUSE_SPINBTN
                else if ( kind == PreviewKind::Number )
                {
                    C::NumberBox number;
                    number.SpinButtonPlacementMode(C::NumberBoxSpinButtonPlacementMode::Inline);
                    number.AcceptsExpression(false);
                    number.ValidationMode(C::NumberBoxValidationMode::InvalidInputOverwritten);
                    cell->preview = number;
                }
#endif
#if wxUSE_DATEPICKCTRL
                else if ( kind == PreviewKind::Date )
                {
                    cell->datePreview = winrt::make_self<PropertyCalendarPreview>();
                    cell->preview = cell->datePreview.as<C::CalendarDatePicker>();
                }
                else if ( kind == PreviewKind::DateSpin )
                {
                    const auto* const dateProperty = wxDynamicCast(property, wxDateProperty);
                    if ( !dateProperty ) throw InvalidPropertySnapshot{};
                    cell->dateSpin = std::make_shared<PropertyDateSpinPreview>(
                        row->name, dateProperty->GetDatePickerStyle());
                    Check(s, property);
                    cell->preview = cell->dateSpin->root;
                }
#endif
                else if ( hasButton )
                {
                    // NoEditor removes the editable text field, not the
                    // displayed value beside a still-enabled dialog button.
                    cell->previewLabel = C::TextBlock();
                    cell->previewLabel.VerticalAlignment(X::VerticalAlignment::Center);
                    cell->previewLabel.TextTrimming(X::TextTrimming::CharacterEllipsis);
                    cell->previewLayout.Children().Append(cell->previewLabel);
                }
                const auto prepare = [&](const C::Control& control, const wxString& id)
                {
                    // These are native presentation controls. PropertyGrid
                    // retains its single active editor/validation transaction:
                    // pointer or UIA activation belongs to the enclosing cell,
                    // and its live wx editor replaces this presentation.
                    control.IsTabStop(false);
                    control.IsHitTestVisible(false);
                    control.MinWidth(0);
                    control.MinHeight(0);
                    control.HorizontalAlignment(X::HorizontalAlignment::Stretch);
                    control.VerticalAlignment(X::VerticalAlignment::Center);
                    A::AutomationProperties::SetAccessibilityView(control, A::Peers::AccessibilityView::Raw);
                    A::AutomationProperties::SetAutomationId(control, wxWinUIToHString(id));
                };
                if ( cell->preview )
                {
                    prepare(cell->preview, "wxProperty.Preview." + row->name);
                    cell->previewLayout.Children().Append(cell->preview);
                }
                if ( hasButton )
                {
                    cell->previewButton = C::Button();
                    cell->previewButton.Content(winrt::box_value(L"\u2026"));
                    cell->previewButton.Margin({4, 0, 0, 0});
                    prepare(cell->previewButton, "wxProperty.PreviewButton." + row->name);
                    C::Grid::SetColumn(cell->previewButton, 1);
                    cell->previewLayout.Children().Append(cell->previewButton);
                }
                C::Grid::SetColumn(cell->previewLayout, 1);
                cell->content.Children().Append(cell->previewLayout); Check(s, property);
            }
        }
        if ( !cell->previewLayout ) return false;
        if ( const auto text = cell->preview.try_as<C::TextBox>() )
        {
            // GetDisplayInfo already supplies the masked password and the
            // unspecified/custom display text. Never fetch an unmasked value.
            const auto label = wxWinUIToHString(cell->label);
            if ( text.Text() != label ) text.Text(label);
            Check(s, property);
        }
        else if ( const auto choice = cell->preview.try_as<C::ComboBox>() )
        {
            // The preview has no popup or mutation path. The complete choice
            // model is bound only by the real editor when editing begins.
            if ( kind == PreviewKind::EditableChoice )
            {
                const auto label = wxWinUIToHString(cell->label);
                if ( choice.Text() != label ) choice.Text(label);
            }
            else
            {
                const auto label = wxWinUIToHString(cell->label);
                if ( choice.Items().Size() == 0 ) choice.Items().Append(winrt::box_value(label));
                else if ( winrt::unbox_value<winrt::hstring>(choice.Items().GetAt(0)) != label )
                    choice.Items().SetAt(0, winrt::box_value(label));
                choice.SelectedIndex(property->IsValueUnspecified() ? -1 : 0);
            }
            Check(s, property);
        }
        else if ( const auto check = cell->preview.try_as<C::CheckBox>() )
        {
            const bool unspecified = property->IsValueUnspecified();
            const int checkedChoice = unspecified ? 0 : property->GetChoiceSelection(); Check(s, property);
            check.IsChecked(unspecified ? winrt::Windows::Foundation::IReference<bool>{nullptr}
                                       : winrt::box_value(checkedChoice != 0).as<winrt::Windows::Foundation::IReference<bool>>());
            Check(s, property);
        }
#if wxUSE_SPINBTN
        else if ( const auto number = cell->preview.try_as<C::NumberBox>() )
        {
            const wxVariant value = property->GetValue(); Check(s, property);
            const bool unspecified = property->IsValueUnspecified();
            double numericValue = std::numeric_limits<double>::quiet_NaN();
            if ( !unspecified && !value.IsNull() ) value.Convert(&numericValue);
            Check(s, property);
            // These attributes describe presentation only. wxNumericProperty
            // and the active wx editor still own stepping, wrapping and limits.
            const wxVariant lower = property->GetAttribute(wxPG_ATTR_MIN); Check(s, property);
            const wxVariant upper = property->GetAttribute(wxPG_ATTR_MAX); Check(s, property);
            const wxVariant step = property->GetAttribute(wxPG_ATTR_SPINCTRL_STEP); Check(s, property);
            const wxVariant wrap = property->GetAttribute(wxPG_ATTR_SPINCTRL_WRAP); Check(s, property);
            double minimum = -(std::numeric_limits<double>::max)();
            double maximum = (std::numeric_limits<double>::max)();
            double increment = 1;
            bool wraps = false;
            if ( !lower.IsNull() ) lower.Convert(&minimum);
            if ( !upper.IsNull() ) upper.Convert(&maximum);
            if ( !step.IsNull() ) step.Convert(&increment);
            if ( !wrap.IsNull() ) wrap.Convert(&wraps);
            Check(s, property);
            if ( !std::isfinite(minimum) ) minimum = -(std::numeric_limits<double>::max)();
            if ( !std::isfinite(maximum) || maximum < minimum ) maximum = (std::numeric_limits<double>::max)();
            if ( !std::isfinite(increment) || increment <= 0 ) increment = 1;
            // Do not coerce an existing wx value merely because it lies outside
            // newly assigned validation limits. The real editor may display it
            // until a subsequent edit is committed and validated.
            if ( std::isfinite(numericValue) )
            {
                minimum = (std::min)(minimum, numericValue);
                maximum = (std::max)(maximum, numericValue);
            }
            if ( minimum > number.Maximum() )
            { number.Maximum(maximum); Check(s, property); }
            if ( number.Minimum() != minimum )
            { number.Minimum(minimum); Check(s, property); }
            if ( number.Maximum() != maximum )
            { number.Maximum(maximum); Check(s, property); }
            if ( number.SmallChange() != increment )
            { number.SmallChange(increment); Check(s, property); }
            if ( number.IsWrapEnabled() != wraps )
            { number.IsWrapEnabled(wraps); Check(s, property); }
            const bool valueChanged = numericValue != cell->numberValue &&
                !(std::isnan(numericValue) && std::isnan(cell->numberValue));
            if ( !cell->numberPrepared || cell->numberLabel != cell->label || valueChanged )
            {
                const auto formatter = winrt::make<PropertyNumberFormatter>(cell->label, numericValue, value);
                Check(s, property);
                number.NumberFormatter(formatter); Check(s, property);
                number.Value(numericValue); Check(s, property);
                // Native NumberBox represents unspecified as NaN and renders
                // its placeholder instead of passing NaN to the formatter.
                number.PlaceholderText(unspecified ? wxWinUIToHString(cell->label) : winrt::hstring());
                Check(s, property);
                cell->numberLabel = cell->label;
                cell->numberValue = numericValue;
                cell->numberPrepared = true;
            }
        }
#endif
#if wxUSE_DATEPICKCTRL
        else if ( const auto date = cell->preview.try_as<C::CalendarDatePicker>() )
        {
            namespace D = wxWinUIDatePresentation;
            namespace WF = winrt::Windows::Foundation;
            const wxVariant value = property->GetValue(); Check(s, property);
            const auto* const dateProperty = wxDynamicCast(property, wxDateProperty);
            if ( !dateProperty ) throw InvalidPropertySnapshot{};
            const long dateStyle = dateProperty->GetDatePickerStyle();
            const wxDateTime civil = !property->IsValueUnspecified() &&
                value.IsType(wxPG_VARIANT_TYPE_DATETIME) ? value.GetDateTime() : wxDefaultDateTime;
            Check(s, property);
            const auto language = date.Language();
            const auto calendar = date.CalendarIdentifier();
            if ( !cell->datePrepared || cell->dateStyle != dateStyle ||
                 cell->dateLanguage != language || cell->dateCalendar != calendar )
            {
                const auto pattern = wxWinUIToHString(D::wxWinUIGetLocaleShortDatePattern(
                    (dateStyle & wxDP_SHOWCENTURY) != 0, language));
                Check(s, property);
                if ( date.DateFormat() != pattern )
                { date.DateFormat(pattern); Check(s, property); }
                // Native default bounds cover only a moving historical
                // interval. Preserve the complete wx civil calendar envelope,
                // including years before 1601, without coercing the model.
                WF::DateTime minimum{}, maximum{};
                if ( !D::wxWinUIResolveCalendarEnvelope(calendar, &minimum, &maximum) )
                    throw winrt::hresult_error(E_INVALIDARG, L"PropertyGrid date calendar envelope");
                Check(s, property);
                if ( date.MinDate() != minimum )
                { date.MinDate(minimum); Check(s, property); }
                if ( date.MaxDate() != maximum )
                { date.MaxDate(maximum); Check(s, property); }
                cell->dateStyle = dateStyle;
                cell->dateLanguage = language;
                cell->dateCalendar = calendar;
                cell->datePrepared = true;
            }
            WF::IReference<WF::DateTime> dateValue{nullptr};
            if ( !D::wxWinUIDateRef(civil, &dateValue) )
                throw winrt::hresult_error(E_INVALIDARG, L"PropertyGrid invalid civil date");
            const auto previous = date.Date();
            if ( !!previous != !!dateValue ||
                 (previous && dateValue && previous.Value() != dateValue.Value()) )
            { date.Date(dateValue); Check(s, property); }
            // DateChanged is raised before the native FormatDate() finishes:
            // publish the exact snapshot after Date(), not from that event.
            cell->datePreview->SetDisplayText(wxWinUIToHString(cell->label));
            Check(s, property);
        }
        else if ( kind == PreviewKind::DateSpin )
        {
            wxVariant value = property->GetValue(); Check(s, property);
            const auto* const dateProperty = wxDynamicCast(property, wxDateProperty);
            if ( !dateProperty ) throw InvalidPropertySnapshot{};
            const long style = dateProperty->GetDatePickerStyle();
            const wxDateTime spinDate = !property->IsValueUnspecified() &&
                value.IsType(wxPG_VARIANT_TYPE_DATETIME) ? value.GetDateTime() : wxDefaultDateTime;
            bool exactFormat = !dateProperty->GetFormat().empty();
            // A custom property may retain the standard editor but override
            // its display string. Preserve that snapshot too, without parsing
            // localized/literal text back into the property's date value.
            if ( !exactFormat && spinDate.IsValid() )
            {
                const wxString standard = dateProperty->wxDateProperty::ValueToString(
                    value, wxPGPropValFormatFlags::Null);
                Check(s, property);
                exactFormat = standard != cell->label;
            }
            else if ( !spinDate.IsValid() && !cell->label.empty() ) exactFormat = true;
            Check(s, property);
            const auto spin = cell->dateSpin;
            spin->Present(spinDate, cell->label, exactFormat, style, cell->font,
                          [&]() { Check(s, property); });
            Check(s, property);
        }
#endif
        if ( cell->previewLabel )
        {
            cell->previewLabel.Text(wxWinUIToHString(cell->label)); Check(s, property);
            wxWinUIApplyFont(cell->previewLabel, cell->font); Check(s, property);
        }
        if ( cell->preview )
        {
            wxWinUIApplyFont(cell->preview, cell->font); Check(s, property);
            cell->preview.IsEnabled(property->IsEnabled() && !property->HasFlag(wxPGFlags::ReadOnly)); Check(s, property);
        }
        if ( cell->previewButton )
        {
            cell->previewButton.IsEnabled(property->IsEnabled() && !property->HasFlag(wxPGFlags::ReadOnly)); Check(s, property);
        }
        cell->previewLayout.Visibility(editing ? X::Visibility::Collapsed : X::Visibility::Visible); Check(s, property);
        return true;
    }
    void PresentCell(const std::shared_ptr<Row>& row, unsigned column,
                     const wxRect& rect, const Snapshot& s, bool category, bool selected)
    {
        auto cell = row->cells[column];
        auto* const property = row->property;
        const bool enabled = property->IsEnabled();
        const int flags = (selected ? wxPGCellRenderer::Selected : 0) |
                          (!enabled ? wxPGCellRenderer::Disabled : 0);
        const bool geometryChanged = cell->rect.GetSize() != rect.GetSize();
        const bool needsRead = row->dirty || !cell->populated;
        wxPGCellRenderer* renderer = nullptr;
        if ( needsRead || (cell->custom && (geometryChanged || flags != cell->renderFlags)) )
        {
            renderer = property->GetCellRenderer(static_cast<int>(column)); Check(s, property);
            const int common = property->GetCommonValue(); Check(s, property);
            if ( column == 1 && common >= 0 ) renderer = owner->GetCommonValue(common)->GetRenderer();
            Check(s, property);
            cell->custom = renderer && typeid(*renderer) != typeid(wxPGDefaultRenderer);
        }
        if ( cell->custom && renderer )
        {
            // Custom renderers explicitly retain their wxDC contract. Only
            // their visible cells are rasterized, and only on invalidation or
            // a change of the render size/state. Standard values are XAML.
            renderer->IncRef();
            const auto release = wxMakeGuard([renderer]() { renderer->DecRef(); });
            wxBitmap bitmap;
            bitmap.CreateWithLogicalSize(std::max(1, rect.width), std::max(1, rect.height), s.scale);
            wxMemoryDC dc(bitmap);
            dc.SetFont(s.font);
            dc.SetTextForeground(selected ? owner->GetSelectionForegroundColour() : owner->GetCellTextColour());
            dc.SetBackground(wxBrush(selected ? owner->GetSelectionBackgroundColour() : owner->GetCellBackgroundColour()));
            dc.Clear();
            renderer->Render(dc, wxRect(wxPoint(), rect.GetSize()), owner.get(), property,
                             static_cast<int>(column), -1, flags);
            Check(s, property);
            dc.SelectObject(wxNullBitmap);
            PublishImage(cell, bitmap, rect.GetSize(), s, property);
            cell->text.Text(L""); Check(s, property);
            cell->content.Padding({0, 0, 0, 0}); Check(s, property);
            cell->image.Margin({0, 0, 0, 0}); Check(s, property);
            cell->renderedSize = rect.GetSize();
        }
        else if ( !cell->custom && needsRead )
        {
            cell->content.Padding({6, 0, 6, 0}); Check(s, property);
            cell->image.Margin({0, 0, 4, 0}); Check(s, property);
            wxString label;
            wxPGCell appearance;
            int choice = -1;
            if ( column == 1 && !property->IsValueUnspecified() )
            { choice = property->GetChoiceSelection(); Check(s, property); }
            property->GetDisplayInfo(column, choice, flags, &label, &appearance); Check(s, property);
            cell->text.Text(wxWinUIToHString(label)); Check(s, property);
            A::AutomationProperties::SetName(cell->button, wxWinUIToHString(label)); Check(s, property);
            const wxBitmapBundle bundle = appearance.GetBitmap();
            wxBitmap bitmap;
            wxSize imageSize;
            if ( bundle.IsOk() )
            {
                imageSize = bundle.GetPreferredLogicalSizeFor(owner.get()); Check(s, property);
                bitmap = bundle.GetBitmapFor(owner.get()); Check(s, property);
            }
            PublishImage(cell, bitmap, imageSize, s, property);
            cell->label = label; cell->appearance = appearance;
        }
        const wxFont font = cell->appearance.GetFont().IsOk() ? cell->appearance.GetFont() :
                            category ? owner->GetCaptionFont() : s.font;
        if ( !cell->populated || cell->font != font )
        { wxWinUIApplyFont(cell->text, font); Check(s, property); cell->font = font; }
        // The live wx editor owns its text while editing. Keeping the value
        // TextBlock beneath a translucent TextBox/ComboBox draws both strings
        // on top of one another. Keep the cell and preview identity intact.
        wxWindow* const valueEditor = column == 1 && owner->GetSelection() == property
            ? owner->GetEditorControl() : nullptr;
        wxWindow* const labelEditor = owner->m_labelEditorProperty == property &&
            static_cast<int>(column) == owner->m_selColumn ? owner->GetLabelEditor() : nullptr;
        const bool editing = (valueEditor && valueEditor->IsShown()) ||
                             (labelEditor && labelEditor->IsShown());
        const bool preview = column == 1 && !category &&
            PresentEditorPreview(row, cell, s, editing);
        cell->text.Visibility(editing || preview ? X::Visibility::Collapsed : X::Visibility::Visible);
        Check(s, property);
        cell->button.IsEnabled(owner->IsEnabled() && enabled); Check(s, property);
        const auto style = !cell->custom && (selected || category)
                         ? selectedCellStyle : cellStyle;
        if ( cell->button.Style() != style )
        { cell->button.Style(style); Check(s, property); }
        if ( cell->selection )
        {
            cell->selection.Visibility(selected && !cell->custom
                ? X::Visibility::Visible : X::Visibility::Collapsed); Check(s, property);
            cell->text.Margin({4, 0, 0, 0}); Check(s, property);
        }
        cell->content.FlowDirection(s.rtl ? X::FlowDirection::RightToLeft
                                         : X::FlowDirection::LeftToRight); Check(s, property);
        const auto customized = owner->m_coloursCustomized;
        if ( selected )
        {
            if ( customized & CustomColour_SelectionBg )
                wxWinUIApplyBackground(cell->button, owner->GetSelectionBackgroundColour());
            else cell->button.ClearValue(C::Control::BackgroundProperty());
            Check(s, property);
            if ( customized & CustomColour_SelectionText )
                wxWinUIApplyForeground(cell->text, owner->GetSelectionForegroundColour());
            else cell->text.ClearValue(C::TextBlock::ForegroundProperty());
            Check(s, property);
        }
        else
        {
            const auto& bg = cell->appearance.GetBgCol();
            const auto& fg = cell->appearance.GetFgCol();
            const auto defaultBg = category ? owner->GetCaptionBackgroundColour()
                                            : owner->GetCellBackgroundColour();
            const auto defaultFg = category ? owner->GetCaptionForegroundColour()
                                            : owner->GetCellTextColour();
            if ( bg.IsOk() && (bg != defaultBg ||
                 (customized & (category ? CustomColour_CaptionBg : CustomColour_CellBg))) )
                wxWinUIApplyBackground(cell->button, bg);
            else cell->button.ClearValue(C::Control::BackgroundProperty());
            Check(s, property);
            if ( !enabled && (customized & CustomColour_DisabledText) )
                wxWinUIApplyForeground(cell->text, owner->GetCellDisabledTextColour());
            else if ( fg.IsOk() && (fg != defaultFg ||
                      (customized & (category ? CustomColour_CaptionText : CustomColour_CellText))) )
                wxWinUIApplyForeground(cell->text, fg);
            else cell->text.ClearValue(C::TextBlock::ForegroundProperty());
            Check(s, property);
        }
        // The persistent native value surface must retain explicit wx cell
        // colours too. Theme defaults stay native when no override is present.
        if ( cell->preview )
        {
            const auto foreground = cell->text.ReadLocalValue(C::TextBlock::ForegroundProperty());
            if ( foreground != X::DependencyProperty::UnsetValue() )
                cell->preview.SetValue(C::Control::ForegroundProperty(), foreground);
            else cell->preview.ClearValue(C::Control::ForegroundProperty());
            Check(s, property);
            const auto background = cell->button.ReadLocalValue(C::Control::BackgroundProperty());
            if ( background != X::DependencyProperty::UnsetValue() )
                cell->preview.SetValue(C::Control::BackgroundProperty(), background);
            else cell->preview.ClearValue(C::Control::BackgroundProperty());
            Check(s, property);
        }
        if ( cell->previewLabel )
        {
            const auto foreground = cell->text.ReadLocalValue(C::TextBlock::ForegroundProperty());
            if ( foreground != X::DependencyProperty::UnsetValue() )
                cell->previewLabel.SetValue(C::TextBlock::ForegroundProperty(), foreground);
            else cell->previewLabel.ClearValue(C::TextBlock::ForegroundProperty());
            Check(s, property);
        }
        cell->button.Width(rect.width / s.scale); Check(s, property);
        cell->button.Height(rect.height / s.scale); Check(s, property);
        C::Canvas::SetLeft(cell->button, rect.x / s.scale); Check(s, property);
        C::Canvas::SetTop(cell->button, rect.y / s.scale); Check(s, property);
        cell->clip.Rect({0, 0, static_cast<float>(rect.width / s.scale),
                             static_cast<float>(rect.height / s.scale)}); Check(s, property);
        cell->rect = rect; cell->renderFlags = flags; cell->populated = true;
    }
    void PresentExpander(const std::shared_ptr<Row>& row, const Snapshot& s, int x, int y)
    {
        auto* const property = row->property;
        const bool visible = !owner->HasFlag(wxPG_HIDE_MARGIN) && property->HasVisibleChildren();
        if ( !visible )
        {
            if ( row->expander ) { row->expander.Visibility(X::Visibility::Collapsed); Check(s, property); }
            return;
        }
        if ( !row->expander )
        {
            row->expander = MakeButton("wxProperty.Expand." + row->name);
            row->chevron = C::FontIcon();
            row->chevron.FontFamily(X::Media::FontFamily(L"Segoe Fluent Icons"));
            row->chevron.FontSize(12);
            row->expander.Content(row->chevron);
            const std::weak_ptr<Impl> weak = shared_from_this();
            const std::weak_ptr<Row> weakRow = row;
            row->expander.Click([weak, weakRow](const auto&, const auto&)
            {
                if ( auto self = weak.lock() )
                    if ( auto current = weakRow.lock() ) self->Activate(current, true);
            });
            canvas.Children().Append(row->expander); Check(s, property);
        }
        const int extent = owner->GetMarginWidth();
        const int left = s.rtl ? s.viewport.x - x - extent : x;
        row->chevron.Glyph(property->IsExpanded() ? L"\xE70D" : s.rtl ? L"\xE76B" : L"\xE76C"); Check(s, property);
        row->expander.Width(extent / s.scale); Check(s, property);
        row->expander.Height(s.height / s.scale); Check(s, property);
        row->expander.IsEnabled(owner->IsEnabled()); Check(s, property);
        row->expander.Visibility(X::Visibility::Visible); Check(s, property);
        A::AutomationProperties::SetName(row->expander, wxWinUIToHString(property->GetLabel())); Check(s, property);
        C::Canvas::SetLeft(row->expander, left / s.scale); Check(s, property);
        C::Canvas::SetTop(row->expander, y / s.scale); Check(s, property);
    }
    void Publish(const Snapshot& s)
    {
        clip.Rect({0, 0, static_cast<float>(std::max(0, s.viewport.x) / s.scale),
                         static_cast<float>(std::max(0, s.viewport.y) / s.scale)}); Check(s);
        // Coordinates are mirrored exactly once below, independently of text
        // reading direction. The property/column order remains wx's order.
        canvas.FlowDirection(X::FlowDirection::LeftToRight); Check(s);
        std::vector<std::pair<wxPGProperty*, int>> visible;
        if ( s.height > 0 && s.viewport.y > 0 )
        {
            const int firstY = s.scroll.y - s.scroll.y % s.height;
            auto* first = owner->GetItemAtY(firstY);
            if ( first )
            {
                auto it = owner->GetIterator(wxPG_ITERATE_VISIBLE, first);
                for ( int y = firstY; !it.AtEnd() && y < s.scroll.y + s.viewport.y; it.Next(), y += s.height )
                {
                    auto* property = it.GetProperty();
                    if ( !owner->IsPropertyPendingRemoval(property) ) visible.emplace_back(property, y - s.scroll.y);
                }
            }
        }
        std::set<wxPGProperty*> wanted;
        for ( const auto& item : visible ) wanted.insert(item.first);
        for ( auto it = rows.begin(); it != rows.end(); )
        {
            if ( !wanted.count(it->first) || it->second->page != s.page )
            { const auto old = it->second; it = rows.erase(it); Retire(old); }
            else ++it;
        }
        auto oldElements = std::move(retired);
        retired.clear();
        for ( const auto& element : oldElements )
        {
            uint32_t index = 0;
            if ( canvas.Children().IndexOf(element, index) ) canvas.Children().RemoveAt(index);
            Check(s);
        }
        for ( const auto& item : visible )
        {
            auto* const property = item.first;
            Check(s, property);
            auto found = rows.find(property);
            std::shared_ptr<Row> row;
            if ( found == rows.end() )
            {
                row = std::make_shared<Row>(); row->property = property; row->page = s.page;
                row->key = ++nextKey; row->name = property->GetName();
                rows.emplace(property, row);
            }
            else row = found->second;
            const bool category = property->IsCategory();
            const bool selected = owner->IsPropertySelected(property);
            const unsigned count = category ? 1u : s.columns;
            while ( row->cells.size() > count )
            {
                const auto old = row->cells.back()->button;
                row->cells.pop_back();
                uint32_t index = 0;
                if ( canvas.Children().IndexOf(old, index) ) canvas.Children().RemoveAt(index);
                Check(s, property);
            }
            while ( row->cells.size() < count )
            {
                auto cell = MakeCell(row, static_cast<unsigned>(row->cells.size()));
                row->cells.push_back(cell);
                canvas.Children().Append(cell->button); Check(s, property);
            }
            row->rect = wxRect(0, item.second, s.viewport.x, s.height);
            int start = owner->GetMarginWidth() - s.scroll.x;
            const int indent = std::max(0, static_cast<int>(property->GetDepth()) - 1) * owner->m_subgroup_extramargin;
            const int expanderX = start - owner->GetMarginWidth() + indent;
            for ( unsigned column = 0; column < count; ++column )
            {
                const int width = category ? std::max(0, s.viewport.x + s.scroll.x - owner->GetMarginWidth()) :
                                             s.page->GetColumnWidth(column);
                const int padding = column == 0 ? indent : 0;
                const int x = start + padding;
                const int w = std::max(0, width - padding - 1);
                const int left = s.rtl ? s.viewport.x - x - w : x;
                // The row pitch already reserves the native control height.
                // Subtracting the legacy GDI grid-line pixel here compresses
                // a 32-DIP editor into 31 DIP and clips its native template.
                // There is no separately painted grid line in this surface.
                const wxRect logical(left, item.second, w, s.height);
                // Native roots are clipped by the viewport; no pixel bitmap is
                // allocated for standard text, regardless of column width.
                PresentCell(row, column, logical, s, category, selected);
                Check(s, property);
                start += width;
            }
            PresentExpander(row, s, expanderX, item.second);
            row->dirty = false;
        }
        publishedRevision = s.revision;
    }
    void Synchronize()
    {
        if ( !IsPublished() || synchronizing || removalDepth || geometryDepth || owner->IsFrozen() ) return;
        synchronizing = true;
        const auto finish = wxMakeGuard([this]() { synchronizing = false; });
        PropertyCallback callback(owner.get());
        for ( unsigned attempt = 0; dirty && attempt != 2; ++attempt )
        {
            dirty = false;
            try
            {
                const auto s = Capture();
                Check(s);
                Publish(s);
            }
            catch ( const InvalidPropertySnapshot& )
            { if ( !Alive() ) return; dirty = true; }
            catch ( const winrt::hresult_error& error )
            { wxWinUILogException("property grid native presentation", error); return; }
        }
    }
    void Close()
    {
        if ( closed ) return;
        closed = true; published = false; ++revision;
        rows.clear(); retired.clear();
        host.Close();
        owner = nullptr;
    }
};

wxPGWinUIProjection::wxPGWinUIProjection(std::shared_ptr<Impl> impl) : m_impl(std::move(impl)) {}
wxPGWinUIProjection::~wxPGWinUIProjection() { Close(); }
std::shared_ptr<wxPGWinUIProjection> wxPGWinUIProjection::Create(wxPropertyGrid* grid)
{
    auto impl = std::make_shared<Impl>(grid);
    auto result = std::shared_ptr<wxPGWinUIProjection>(new wxPGWinUIProjection(impl));
    try { if ( !impl->Initialize() ) return nullptr; }
    catch ( const winrt::hresult_error& error )
    { wxWinUILogException("property grid native creation", error); return nullptr; }
    return result;
}
void wxPGWinUIProjection::Invalidate(const wxRect* rect) { const auto impl = m_impl; impl->Invalidate(rect); }
void wxPGWinUIProjection::Update() { const auto impl = m_impl; impl->Synchronize(); }
bool wxPGWinUIProjection::IsPublished() const { return m_impl->IsPublished(); }
void wxPGWinUIProjection::Close() { const auto impl = m_impl; if ( impl ) impl->Close(); }
void wxPGWinUIProjection::BeginGeometry() { ++m_impl->geometryDepth; }
void wxPGWinUIProjection::EndGeometry()
{
    const auto impl = m_impl;
    wxASSERT(impl->geometryDepth);
    --impl->geometryDepth;
    if ( impl->Alive() ) { ++impl->revision; impl->dirty = true; impl->Synchronize(); }
}
void wxPGWinUIProjection::BeginRemoval(wxPGProperty* property)
{ const auto impl = m_impl; impl->BeginRemoval(property); }
void wxPGWinUIProjection::EndRemoval()
{
    const auto impl = m_impl;
    wxASSERT(impl->removalDepth);
    --impl->removalDepth;
    impl->Schedule();
}
#endif
