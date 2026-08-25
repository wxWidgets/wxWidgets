/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/button.cpp
// Purpose:     wxWinUI wxButton implementation
// Author:      wxWidgets development team
// Created:     2026-05-31
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_BUTTON

#include "wx/app.h"
#include "wx/button.h"
#include "wx/bitmap.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/icon.h"
    #include "wx/log.h"
    #include "wx/stockitem.h"
#endif

#include "private.h"
#include "wx/winui/winui.h"
#include "wx/winui/private/appearance.h"
#include "wx/msw/private.h"
#include "wx/msw/wrapshl.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#if wxUSE_MARKUP
    #include "wx/private/markupparser.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Provider.h>
#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Text.h>

#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <exception>
#include <memory>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXA = winrt::Microsoft::UI::Xaml::Automation;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXD = winrt::Microsoft::UI::Xaml::Documents;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
namespace MUXMI = winrt::Microsoft::UI::Xaml::Media::Imaging;

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

namespace
{

constexpr wchar_t wxWinUIButtonBitmapTag[] =
    L"wxWinUIButtonBitmap";
constexpr wchar_t wxWinUIButtonAuthBitmapTag[] =
    L"wxWinUIButtonAuthBitmap";

// Windows currently exposes much smaller monitor scale factors, but keep a
// deliberately generous ceiling for future hardware while rejecting values
// that can overflow application-owned wxBitmapBundle implementations before
// they get a chance to return a size.
constexpr double wxWinUIButtonMinProjectionScale = 1.0 / 64.0;
constexpr double wxWinUIButtonMaxProjectionScale = 64.0;

thread_local std::vector<const wxButton *> gs_buttonBitmapGetterStack;

template <typename Function>
void wxWinUIButtonRunCallbackNoexcept(const char *operation,
                                      Function&& function) noexcept
{
    try
    {
        function();
    }
    catch ( const std::exception& e )
    {
        try
        {
            wxLogWarning(
                "wxWinUI: Button %s failed: %s",
                wxString::FromUTF8(operation),
                wxString::FromUTF8(e.what()));
        }
        catch ( ... )
        {
        }
    }
    catch ( ... )
    {
        try
        {
            wxLogWarning(
                "wxWinUI: Button %s failed with an unknown exception.",
                wxString::FromUTF8(operation));
        }
        catch ( ... )
        {
        }
    }
}

enum class wxWinUIButtonBundleState
{
    Current,
    Restart,
    Dead,
    Invalid
};

template <typename CheckState>
wxWinUIButtonBundleState wxWinUIResolveButtonBundle(
    const wxBitmapBundle& bundle,
    double scale,
    CheckState&& checkState,
    wxBitmap& bitmap,
    wxSize *defaultLogicalSize = nullptr)
{
    bitmap = wxBitmap();
    if ( defaultLogicalSize )
        *defaultLogicalSize = wxSize();
    if ( !bundle.IsOk() )
        return wxWinUIButtonBundleState::Current;

    if ( !std::isfinite(scale) ||
         scale < wxWinUIButtonMinProjectionScale ||
         scale > wxWinUIButtonMaxProjectionScale )
    {
        return wxWinUIButtonBundleState::Invalid;
    }

    // Keep the public wxBitmapBundle callback order, but split the wrapper into
    // its three virtual boundaries so destruction/reentrance can be checked
    // between them. The local bundle copy keeps its implementation alive.
    const wxSize pixelSize =
        bundle.GetPreferredBitmapSizeAtScale(scale);
    wxWinUIButtonBundleState state = checkState();
    if ( state != wxWinUIButtonBundleState::Current )
        return state;
    if ( pixelSize.x <= 0 || pixelSize.y <= 0 )
        return wxWinUIButtonBundleState::Invalid;

    const wxSize defaultSize = bundle.GetDefaultSize();
    state = checkState();
    if ( state != wxWinUIButtonBundleState::Current )
        return state;
    if ( defaultSize.x <= 0 || defaultSize.y <= 0 )
        return wxWinUIButtonBundleState::Invalid;
    if ( defaultLogicalSize )
        *defaultLogicalSize = defaultSize;

    wxBitmapBundleImpl * const bundleImpl = bundle.GetImpl();
    if ( !bundleImpl )
        return wxWinUIButtonBundleState::Invalid;

    bitmap = bundleImpl->GetBitmap(pixelSize);
    state = checkState();
    if ( state != wxWinUIButtonBundleState::Current )
        return state;

    if ( bitmap.IsOk() )
    {
        // This is the normalization performed by wxBitmapBundle::GetBitmap().
        // It preserves the bundle's default logical size even when, e.g., the
        // 1x source wins the 1.5x preferred-size tie.
        const double bitmapScale =
            static_cast<double>(pixelSize.y) / defaultSize.y;
        if ( !std::isfinite(bitmapScale) || bitmapScale <= 0.0 )
            return wxWinUIButtonBundleState::Invalid;
        bitmap.SetScaleFactor(bitmapScale);
    }

    return wxWinUIButtonBundleState::Current;
}

double wxWinUIGetFontSize(const wxFont& font)
{
    const double pt = font.GetFractionalPointSize();
    return pt > 0.0 ? pt * 96.0 / 72.0 : 14.0;
}

MUX::HorizontalAlignment wxWinUIGetHorizontalAlignment(const wxWindow *win)
{
    if ( win->HasFlag(wxBU_LEFT) )
        return MUX::HorizontalAlignment::Left;
    if ( win->HasFlag(wxBU_RIGHT) )
        return MUX::HorizontalAlignment::Right;
    return MUX::HorizontalAlignment::Center;
}

MUX::VerticalAlignment wxWinUIGetVerticalAlignment(const wxWindow *win)
{
    if ( win->HasFlag(wxBU_TOP) )
        return MUX::VerticalAlignment::Top;
    if ( win->HasFlag(wxBU_BOTTOM) )
        return MUX::VerticalAlignment::Bottom;
    return MUX::VerticalAlignment::Center;
}

MUX::FlowDirection wxWinUIGetFlowDirection(wxLayoutDirection dir)
{
    return dir == wxLayout_RightToLeft ? MUX::FlowDirection::RightToLeft
                                       : MUX::FlowDirection::LeftToRight;
}

wxBitmap wxWinUICreateDisabledBitmap(const wxBitmap& bitmap)
{
    if ( !bitmap.IsOk() )
        return wxBitmap();

    return bitmap.ConvertToDisabled();
}

#if wxUSE_MARKUP
using InlineCollection =
    winrt::Windows::Foundation::Collections::IVector<MUXD::Inline>;

void wxWinUIButtonAppendStyledText(InlineCollection const& inlines,
                                   const wxString& text,
                                   bool bold,
                                   bool italic,
                                   bool underline,
                                   const wxColour& fg)
{
    wxString remaining = text;
    bool first = true;
    for ( ;; )
    {
        const int nl = remaining.Find('\n');
        const wxString line = nl == wxNOT_FOUND ? remaining
                                                : remaining.Left(nl);

        if ( !first )
            inlines.Append(MUXD::LineBreak());
        first = false;

        if ( !line.empty() )
        {
            MUXD::Run run;
            run.Text(wxWinUIToHString(line));
            if ( bold )
                run.FontWeight(winrt::Microsoft::UI::Text::FontWeights::Bold());
            if ( italic )
                run.FontStyle(winrt::Windows::UI::Text::FontStyle::Italic);
            if ( underline )
                run.TextDecorations(winrt::Windows::UI::Text::TextDecorations::Underline);
            if ( fg.IsOk() )
                run.Foreground(wxWinUICreateColourBrush(fg));
            inlines.Append(run);
        }

        if ( nl == wxNOT_FOUND )
            break;
        remaining = remaining.Mid(nl + 1);
    }
}

class wxWinUIButtonMarkupToInlines : public wxMarkupParserOutput
{
public:
    explicit wxWinUIButtonMarkupToInlines(InlineCollection const& inlines)
        : m_inlines(inlines) {}

    void OnText(const wxString& text) override
    {
        wxWinUIButtonAppendStyledText(m_inlines, text,
                                      m_bold > 0,
                                      m_italic > 0,
                                      m_underline > 0,
                                      m_fg);
    }

    void OnBoldStart() override { ++m_bold; }
    void OnBoldEnd() override { --m_bold; }
    void OnItalicStart() override { ++m_italic; }
    void OnItalicEnd() override { --m_italic; }
    void OnUnderlinedStart() override { ++m_underline; }
    void OnUnderlinedEnd() override { --m_underline; }
    void OnStrikethroughStart() override {}
    void OnStrikethroughEnd() override {}
    void OnBigStart() override {}
    void OnBigEnd() override {}
    void OnSmallStart() override {}
    void OnSmallEnd() override {}
    void OnTeletypeStart() override {}
    void OnTeletypeEnd() override {}

    void OnSpanStart(const wxMarkupSpanAttributes& attrs) override
    {
        m_fgStack.push_back(m_fg);
        if ( !attrs.m_fgCol.empty() )
        {
            const wxColour col(attrs.m_fgCol);
            if ( col.IsOk() )
                m_fg = col;
        }
    }

    void OnSpanEnd(const wxMarkupSpanAttributes& WXUNUSED(attrs)) override
    {
        m_fg = m_fgStack.back();
        m_fgStack.pop_back();
    }

private:
    InlineCollection m_inlines;
    int m_bold = 0;
    int m_italic = 0;
    int m_underline = 0;
    wxColour m_fg;
    std::vector<wxColour> m_fgStack;
};
#endif // wxUSE_MARKUP

// Build the button label element, either as plain text or from wx markup.
MUXC::TextBlock wxWinUIMakeTextBlock(const wxString& text,
                                     const wxString& markup,
                                     bool useFg,
                                     const wxColour& fg)
{
#if !wxUSE_MARKUP
    wxUnusedVar(markup);
#endif
    MUXC::TextBlock tb;
    tb.VerticalAlignment(MUX::VerticalAlignment::Center);
    tb.HorizontalAlignment(MUX::HorizontalAlignment::Center);
    if ( useFg )
        tb.Foreground(wxWinUICreateColourBrush(fg));

#if wxUSE_MARKUP
    if ( !markup.empty() )
    {
        auto inlines = tb.Inlines();
        inlines.Clear();
        wxWinUIButtonMarkupToInlines output(inlines);
        wxMarkupParser parser(output);
        parser.Parse(markup);
    }
    else
#endif // wxUSE_MARKUP
    {
        tb.Text(wxWinUIToHString(text));
    }
    return tb;
}

// Build an image element for the button bitmap, honouring the bitmap margins
// and (optionally) adding the gap between the image and the label.
MUXC::Image wxWinUIMakeImage(const wxBitmap& bmp,
                             const wxSize& margins,
                             wxDirection pos,
                             bool withGap,
                             double gap)
{
    MUXC::Image image;
    image.Tag(winrt::box_value(
        winrt::hstring(wxWinUIButtonBitmapTag)));
    image.Source(wxWinUIWriteableBitmapFromBitmap(bmp));
    const wxSize dip = bmp.GetDIPSize();
    image.Width(static_cast<double>(dip.x));
    image.Height(static_cast<double>(dip.y));
    image.VerticalAlignment(MUX::VerticalAlignment::Center);
    image.HorizontalAlignment(MUX::HorizontalAlignment::Center);

    MUX::Thickness margin{};
    margin.Left = margin.Right = margins.x;
    margin.Top = margin.Bottom = margins.y;
    if ( withGap )
    {
        switch ( pos )
        {
            case wxLEFT:   margin.Right  += gap; break;
            case wxRIGHT:  margin.Left   += gap; break;
            case wxTOP:    margin.Bottom += gap; break;
            case wxBOTTOM: margin.Top    += gap; break;
            default: break;
        }
    }
    image.Margin(margin);
    return image;
}

// Build the UAC shield image shown for SetAuthNeeded().
MUXC::Image wxWinUIMakeAuthImage(const wxBitmap& bmp, bool withGap, double gap)
{
    MUXC::Image image;
    image.Tag(winrt::box_value(
        winrt::hstring(wxWinUIButtonAuthBitmapTag)));
    image.Source(wxWinUIWriteableBitmapFromBitmap(bmp));
    const wxSize dip = bmp.GetDIPSize();
    image.Width(static_cast<double>(dip.x));
    image.Height(static_cast<double>(dip.y));
    image.VerticalAlignment(MUX::VerticalAlignment::Center);
    image.HorizontalAlignment(MUX::HorizontalAlignment::Center);
    if ( withGap )
    {
        MUX::Thickness margin{};
        margin.Right = gap;
        image.Margin(margin);
    }
    return image;
}

// Build the command-link content (a leading icon/shield plus a bold title and
// an optional dimmed note), used when the label contains an embedded newline.
MUXC::Grid wxWinUIMakeCommandLink(const wxString& title,
                                  const wxString& note,
                                  const wxBitmap& bitmap,
                                  const wxBitmap& authBitmap,
                                  double noteFontSize,
                                  bool useFg,
                                  const wxColour& fg)
{
    MUXC::Grid grid;
    grid.VerticalAlignment(MUX::VerticalAlignment::Center);
    grid.HorizontalAlignment(MUX::HorizontalAlignment::Stretch);
    grid.MinWidth(0);
    grid.MinHeight(0);

    MUXC::ColumnDefinition iconCol;
    iconCol.Width(MUX::GridLengthHelper::Auto());
    grid.ColumnDefinitions().Append(iconCol);

    MUXC::ColumnDefinition textCol;
    textCol.Width(MUX::GridLengthHelper::FromValueAndType(1, MUX::GridUnitType::Star));
    grid.ColumnDefinitions().Append(textCol);

    MUX::FrameworkElement leading{ nullptr };
    if ( authBitmap.IsOk() )
    {
        MUXC::Image authImage;
        authImage.Tag(winrt::box_value(
            winrt::hstring(wxWinUIButtonAuthBitmapTag)));
        authImage.Source(wxWinUIWriteableBitmapFromBitmap(authBitmap));
        const wxSize dip = authBitmap.GetDIPSize();
        authImage.Width(static_cast<double>(dip.x));
        authImage.Height(static_cast<double>(dip.y));
        leading = authImage;
    }
    else if ( bitmap.IsOk() )
    {
        MUXC::Image image;
        image.Tag(winrt::box_value(
            winrt::hstring(wxWinUIButtonBitmapTag)));
        image.Source(wxWinUIWriteableBitmapFromBitmap(bitmap));
        const wxSize dip = bitmap.GetDIPSize();
        image.Width(static_cast<double>(dip.x));
        image.Height(static_cast<double>(dip.y));
        leading = image;
    }
    else
    {
        MUXC::FontIcon arrow;
        arrow.Glyph(L"\xE72A");
        arrow.FontSize(13.0);
        leading = arrow;
    }

    if ( authBitmap.IsOk() || !bitmap.IsOk() )
    {
        leading.Width(16.0);
        leading.Height(16.0);
    }
    leading.VerticalAlignment(MUX::VerticalAlignment::Top);
    leading.HorizontalAlignment(MUX::HorizontalAlignment::Center);
    MUX::Thickness iconMargin{};
    iconMargin.Right = 4.0;
    // Nudge the 16-DIP icon down so it lines up with the vertical centre of the
    // (taller) main title line rather than its top.
    iconMargin.Top = 2.0;
    leading.Margin(iconMargin);
    MUXC::Grid::SetColumn(leading, 0);
    grid.Children().Append(leading);

    MUXC::StackPanel textPanel;
    textPanel.Orientation(MUXC::Orientation::Vertical);
    textPanel.VerticalAlignment(MUX::VerticalAlignment::Top);

    MUXC::TextBlock titleBlock;
    titleBlock.Text(wxWinUIToHString(title));
    titleBlock.TextWrapping(MUX::TextWrapping::NoWrap);
    titleBlock.FontWeight(winrt::Microsoft::UI::Text::FontWeights::SemiBold());
    if ( useFg )
        titleBlock.Foreground(wxWinUICreateColourBrush(fg));
    textPanel.Children().Append(titleBlock);

    if ( !note.empty() )
    {
        MUXC::TextBlock noteBlock;
        noteBlock.Text(wxWinUIToHString(note));
        noteBlock.TextWrapping(MUX::TextWrapping::NoWrap);
        noteBlock.FontSize(noteFontSize);
        noteBlock.Opacity(0.75);
        if ( useFg )
            noteBlock.Foreground(wxWinUICreateColourBrush(fg));
        textPanel.Children().Append(noteBlock);
    }

    MUXC::Grid::SetColumn(textPanel, 1);
    grid.Children().Append(textPanel);
    return grid;
}

MUXC::Image wxWinUIFindTaggedButtonImage(
    const winrt::Windows::Foundation::IInspectable& content,
    const wchar_t *tag)
{
    if ( !content || !tag )
        return nullptr;

    if ( const auto image = content.try_as<MUXC::Image>() )
    {
        const winrt::hstring actual =
            winrt::unbox_value_or<winrt::hstring>(
                image.Tag(), {});
        return actual == winrt::hstring(tag)
            ? image
            : MUXC::Image{ nullptr };
    }

    if ( const auto panel = content.try_as<MUXC::Panel>() )
    {
        const auto children = panel.Children();
        for ( std::uint32_t i = 0; i < children.Size(); ++i )
        {
            if ( const auto image =
                     wxWinUIFindTaggedButtonImage(
                         children.GetAt(i), tag) )
            {
                return image;
            }
        }
    }

    return nullptr;
}

wxSize wxWinUIGetImageSourcePixelSize(
    const MUXC::Image& image)
{
    if ( !image )
        return wxSize();

    const auto source =
        image.Source().try_as<MUXMI::WriteableBitmap>();
    return source
        ? wxSize(source.PixelWidth(), source.PixelHeight())
        : wxSize();
}

} // namespace

// ----------------------------------------------------------------------------
// wxWinUIButtonImpl
// ----------------------------------------------------------------------------

namespace
{
std::atomic<unsigned> gs_liveButtonCallbackStates{ 0 };
std::atomic<unsigned> gs_buttonPeerInvokeAttempts{ 0 };
} // anonymous namespace

class wxWinUIButtonCallbackState final
{
public:
    explicit wxWinUIButtonCallbackState(wxButton *owner)
        : m_owner(owner)
    {
        gs_liveButtonCallbackStates.fetch_add(1, std::memory_order_relaxed);
    }

    ~wxWinUIButtonCallbackState()
    {
        gs_liveButtonCallbackStates.fetch_sub(1, std::memory_order_relaxed);
    }

    wxButton *GetOwner() const { return m_owner; }
    void Invalidate() { m_owner = nullptr; }

private:
    wxButton *m_owner;
};

class wxWinUIButtonImpl
{
public:
    ~wxWinUIButtonImpl()
    {
        Close();
    }

    void Close()
    {
        // A retained AutomationPeer may outlive both the slot and the wx
        // object. Invalidate first so every delegate is already inert while
        // token removal, content detachment and host teardown run callbacks.
        destroying = true;
        if ( callbackState )
            callbackState->Invalidate();

        const MUXC::Button peer = button;
        if ( peer )
        {
            const auto revoke =
                [](winrt::event_token& token,
                   auto&& remove,
                   const char *context)
                {
                    if ( !token.value )
                        return;

                    try
                    {
                        remove(token);
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(context, e);
                    }
                    token = {};
                };

            revoke(clickToken,
                   [peer](winrt::event_token token) { peer.Click(token); },
                   "WinUI Button Click removal");
            revoke(pointerEnteredToken,
                   [peer](winrt::event_token token)
                   {
                       peer.PointerEntered(token);
                   },
                   "WinUI Button PointerEntered removal");
            revoke(pointerExitedToken,
                   [peer](winrt::event_token token)
                   {
                       peer.PointerExited(token);
                   },
                   "WinUI Button PointerExited removal");
            revoke(pointerCaptureLostToken,
                   [peer](winrt::event_token token)
                   {
                       peer.PointerCaptureLost(token);
                   },
                   "WinUI Button PointerCaptureLost removal");
            revoke(gotFocusToken,
                   [peer](winrt::event_token token) { peer.GotFocus(token); },
                   "WinUI Button GotFocus removal");
            revoke(lostFocusToken,
                   [peer](winrt::event_token token) { peer.LostFocus(token); },
                   "WinUI Button LostFocus removal");

            const auto removeRouted =
                [&peer](auto routedEvent,
                        auto& handler,
                        const char *context)
                {
                    if ( !handler )
                        return;

                    try
                    {
                        peer.RemoveHandler(
                            routedEvent, winrt::box_value(handler));
                    }
                    catch ( const winrt::hresult_error& e )
                    {
                        wxWinUILogException(context, e);
                    }
                    handler = nullptr;
                };
            removeRouted(MUX::UIElement::PointerPressedEvent(),
                         routedPointerPressed,
                         "WinUI Button PointerPressed removal");
            removeRouted(MUX::UIElement::PointerReleasedEvent(),
                         routedPointerReleased,
                         "WinUI Button PointerReleased removal");

            try
            {
                peer.Content(
                    winrt::Windows::Foundation::IInspectable{ nullptr });
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException("WinUI Button content cleanup", e);
            }
        }

        host.Close();
        button = nullptr;
        clickToken = {};
        pointerEnteredToken = {};
        pointerExitedToken = {};
        pointerCaptureLostToken = {};
        gotFocusToken = {};
        lostFocusToken = {};
        routedPointerPressed = nullptr;
        routedPointerReleased = nullptr;
        stateHandlersAttached = false;
        hovered = false;
        pressed = false;
        focused = false;
        callbackState.reset();
    }

    wxWinUIControlHost host;
    std::shared_ptr<wxWinUIButtonCallbackState> callbackState;
    MUXC::Button button{ nullptr };
    winrt::event_token clickToken{};

    // The handlers below are attached lazily (EnsureStateHandlers) and only
    // exist to swap per-state bitmaps for wxBitmapButton.  A plain button never
    // pays for them: the WinUI Button already renders its own visual states.
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    winrt::event_token pointerCaptureLostToken{};
    winrt::event_token gotFocusToken{};
    winrt::event_token lostFocusToken{};
    MUX::Input::PointerEventHandler routedPointerPressed{ nullptr };
    MUX::Input::PointerEventHandler routedPointerReleased{ nullptr };

    bool stateHandlersAttached = false;
    bool hovered = false;
    bool pressed = false;
    bool focused = false;
    bool destroying = false;
    wxAnyButton::State projectedBitmapState =
        wxAnyButton::State_Normal;
    std::uint64_t contentGeneration = 0;
    wxSize normalBitmapLogicalSize;
    bool normalBitmapLogicalSizeValid = false;

    // Content projection is a revisioned, latest-writer-wins transaction.
    // wxBitmapBundle implementations are user-extensible: either of their
    // virtual calls may synchronously replace the bundle or destroy the wx
    // owner. Keep nested requests out of a partially-written XAML tree and
    // bound a hostile mutation storm to eight synchronous passes plus one
    // event-loop replay.
    std::uint64_t contentRevision = 0;
    std::uint64_t contentReplayTicket = 0;
    bool contentProjectionInProgress = false;
    bool contentProjectionRequested = false;
    bool contentProjectionReplayScheduled = false;
    bool contentProjectionReplayEntryPending = false;
    bool contentProjectionDeferredUsed = false;
    bool pendingForceRender = false;
    wxAnyButton::State pendingForcedState = wxAnyButton::State_Max;
    double pendingRequestedScale = 0.0;

    // Appearance projection has its own bounded transaction because Show(),
    // SetBackgroundColour() and layout-direction changes don't necessarily
    // rebuild Content. Every synchronous XAML property boundary can therefore
    // request a newer pass without recursively entering a half-written peer.
    std::uint64_t appearanceRevision = 0;
    std::uint64_t appearanceReplayTicket = 0;
    bool appearanceProjectionInProgress = false;
    bool appearanceProjectionRequested = false;
    bool appearanceProjectionReplayScheduled = false;
    bool appearanceProjectionReplayEntryPending = false;
    bool appearanceProjectionDeferredUsed = false;
    bool pendingAppearanceForceRender = false;
};

// ----------------------------------------------------------------------------
// construction / destruction
// ----------------------------------------------------------------------------

wxButton::wxButton()
    : m_bitmapMargins(0, 0),
      m_bitmapPosition(wxLEFT),
      m_authNeeded(false)
{
}

wxButton::wxButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
    : wxButton()
{
    Create(parent, id, label, pos, size, style, validator, name);
}

wxButton::~wxButton()
{
    // Keep the guard alive until after m_bitmaps has finished releasing
    // application-owned implementations. Their destructors may reenter public
    // bitmap getters even before Create() ever produced a WinUI peer.
    m_bitmapDestroying = true;
    Unbind(wxEVT_DPI_CHANGED, &wxButton::OnDPIChanged, this);
    m_winui.reset();
}

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& labelOrig,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    wxString label;
    if ( !(style & wxBU_NOTEXT) )
    {
        label = labelOrig;
        if ( label.empty() && wxIsStockID(id) )
            label = wxGetStockLabel(id, wxSTOCK_WITH_MNEMONIC);
    }

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxControl::SetLabel(label);

    m_winui.reset(new wxWinUIButtonImpl);
    m_winui->callbackState =
        std::make_shared<wxWinUIButtonCallbackState>(this);
    wxWinUIButtonImpl * const createImpl = m_winui.get();
    const std::shared_ptr<wxWinUIButtonCallbackState> callbackState =
        createImpl->callbackState;
    if ( !m_winui->host.Initialize(this) )
    {
        m_winui.reset();
        return false;
    }

    try
    {
        createImpl->button = MUXC::Button();
        createImpl->clickToken = createImpl->button.Click(
            [callbackState](
                winrt::Windows::Foundation::IInspectable const&,
                MUX::RoutedEventArgs const&)
            {
                wxButton * const owner = callbackState->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->destroying )
                {
                    return;
                }

                // The command event can destroy owner. Nothing may follow it,
                // and an application exception must not cross the WinRT ABI.
                wxWinUIButtonRunCallbackNoexcept(
                    "click event",
                    [owner]() { owner->SendClickEvent(); });
            });

        if ( !UpdateWinUIContent(false) )
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( owner && owner->m_winui &&
                 owner->m_winui.get() == createImpl )
            {
                owner->m_winui.reset();
            }
            return false;
        }

        wxButton *owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }

        const MUXC::Button peer = createImpl->button;
        const bool contentSet =
            createImpl->host.SetContent(peer);
        owner = callbackState->GetOwner();
        if ( !owner || !owner->m_winui ||
             owner->m_winui.get() != createImpl )
        {
            return false;
        }
        if ( !contentSet )
        {
            owner->m_winui.reset();
            return false;
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Button creation", e);
        wxButton * const owner = callbackState->GetOwner();
        if ( owner && owner->m_winui &&
             owner->m_winui.get() == createImpl )
        {
            owner->m_winui.reset();
        }
        return false;
    }

    wxButton *owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }

    owner->SetInitialSize(size);
    owner = callbackState->GetOwner();
    if ( !owner || !owner->m_winui ||
         owner->m_winui.get() != createImpl )
    {
        return false;
    }
    owner->Bind(wxEVT_DPI_CHANGED, &wxButton::OnDPIChanged, owner);

    return true;
}

// ----------------------------------------------------------------------------
// public API
// ----------------------------------------------------------------------------

void wxButton::SetLabel(const wxString& label)
{
#if wxUSE_MARKUP
    m_markup.clear();
#endif // wxUSE_MARKUP

    wxControl::SetLabel(label);
    InvalidateBestSize();
    if ( !UpdateWinUIContent() )
        return;

    if ( GetParent() && GetParent()->GetSizer() )
        GetParent()->Layout();
    else
    {
        const wxSize best = GetBestSize();
        SetSize(best);
    }
}

wxWindow *wxButton::SetDefault()
{
    wxWindow * const winOldDefault = wxButtonBase::SetDefault();
    const wxWeakRef<wxButton> alive(this);

    if ( wxButton * const oldButton = wxDynamicCast(winOldDefault, wxButton) )
        oldButton->ApplyDefaultStyle(false);

    if ( wxButton * const owner = alive.get() )
        owner->ApplyDefaultStyle(true);

    return winOldDefault;
}

void wxButton::Command(wxCommandEvent& event)
{
    ProcessCommand(event);
}

bool wxButton::Show(bool show)
{
    const bool rc = wxControl::Show(show);
    UpdateWinUIAppearance();
    return rc;
}

bool wxButton::SetBackgroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetBackgroundColour(colour);
    UpdateWinUIAppearance();
    return rc;
}

bool wxButton::SetForegroundColour(const wxColour& colour)
{
    const bool rc = wxControl::SetForegroundColour(colour);
    UpdateWinUIContent();
    return rc;
}

bool wxButton::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIContent();
    return rc;
}

bool wxButton::MSWOnEffectiveLayoutDirectionChanged()
{
    return UpdateWinUIAppearance();
}

void wxButton::OnDPIChanged(wxDPIChangedEvent& event)
{
    // Bitmap bundles select their concrete bitmap for the current control
    // scale. Rebuild the peer content so a monitor transition cannot leave
    // the image cached at the previous DPI.
    event.Skip();
    InvalidateBestSize();
    wxWinUIButtonRunCallbackNoexcept(
        "DPI projection",
        [this]() { UpdateWinUIContent(); });
}

bool wxButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_BUTTON, GetId());
    event.SetEventObject(this);
    return ProcessCommand(event);
}

bool wxButton::WinUIQueueClickForTesting()
{
    if ( !m_winui || m_winui->destroying || !m_winui->button || !wxTheApp )
        return false;

    // Retain only the native peer. This deliberately does not retain the wx
    // owner or implementation, and exercises a late invoke after Close().
    const MUXC::Button peer = m_winui->button;
    wxTheApp->CallAfter(
        [peer]()
        {
            gs_buttonPeerInvokeAttempts.fetch_add(
                1, std::memory_order_release);
            try
            {
                winrt::Microsoft::UI::Xaml::Automation::Peers::
                    ButtonAutomationPeer automationPeer(peer);
                automationPeer.Invoke();
            }
            catch ( const winrt::hresult_error& e )
            {
                wxWinUILogException(
                    "WinUI retained Button test invocation", e);
            }
        });
    return true;
}

unsigned wxButton::WinUIGetLiveCallbackStateCountForTesting()
{
    return gs_liveButtonCallbackStates.load(std::memory_order_acquire);
}

unsigned wxButton::WinUIGetPeerInvokeAttemptCountForTesting()
{
    return gs_buttonPeerInvokeAttempts.load(std::memory_order_acquire);
}

bool wxButton::WinUIProjectBitmapStateForTesting(State state,
                                                 double scale)
{
    if ( state < State_Normal || state >= State_Max ||
         !std::isfinite(scale) ||
         scale < wxWinUIButtonMinProjectionScale ||
         scale > wxWinUIButtonMaxProjectionScale ||
         !m_winui || m_winui->destroying ||
         !m_winui->button )
    {
        return false;
    }

    bool projected = false;
    wxWinUIButtonRunCallbackNoexcept(
        "test bitmap-state projection",
        [&]() { projected = UpdateWinUIContent(true, state, scale); });
    return projected;
}

bool wxButton::WinUIGetPeerBitmapProjectionForTesting(
    wxSize *bitmapPixelSize,
    wxSize *authPixelSize,
    State *state,
    std::uint64_t *generation) const
{
    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return false;

    try
    {
        const auto content = m_winui->button.Content();
        if ( bitmapPixelSize )
        {
            *bitmapPixelSize = wxWinUIGetImageSourcePixelSize(
                wxWinUIFindTaggedButtonImage(
                    content, wxWinUIButtonBitmapTag));
        }
        if ( authPixelSize )
        {
            *authPixelSize = wxWinUIGetImageSourcePixelSize(
                wxWinUIFindTaggedButtonImage(
                    content, wxWinUIButtonAuthBitmapTag));
        }
        if ( state )
            *state = m_winui->projectedBitmapState;
        if ( generation )
            *generation = m_winui->contentGeneration;
        return true;
    }
    catch ( const winrt::hresult_error& )
    {
        return false;
    }
}

void wxButton::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);
    // Appearance publishes IsEnabled and content may select State_Disabled;
    // both are committed before the single terminal host flush.
    UpdateWinUIContent();
}

// ----------------------------------------------------------------------------
// bitmaps / auth / markup
// ----------------------------------------------------------------------------

wxBitmap wxButton::DoGetBitmap(State which) const
{
    if ( m_bitmapDestroying ||
         which < State_Normal || which >= State_Max ||
         !m_bitmaps[which].IsOk() )
        return wxBitmap();

    for ( const wxButton * const active : gs_buttonBitmapGetterStack )
    {
        if ( active == this )
            return wxBitmap();
    }
    gs_buttonBitmapGetterStack.push_back(this);
    wxScopeGuard getterGuard =
        wxMakeGuard(
            []()
            {
                wxASSERT(!gs_buttonBitmapGetterStack.empty());
                gs_buttonBitmapGetterStack.pop_back();
            });
    wxUnusedVar(getterGuard);

    const std::shared_ptr<wxWinUIButtonCallbackState> callbackState =
        m_winui ? m_winui->callbackState : nullptr;
    wxWinUIButtonImpl * const impl = m_winui.get();
    const wxWeakRef<wxWindow> lifetime(
        const_cast<wxButton *>(this));

    // A bundle callback may replace the selected bundle. Retry from a fresh
    // local copy, but never recurse indefinitely on a hostile implementation.
    constexpr unsigned MaxGetterProjectionPasses = 8;
    for ( unsigned pass = 0; pass < MaxGetterProjectionPasses; ++pass )
    {
        const wxWindow * const liveWindow = lifetime.get();
        const wxButton *owner =
            liveWindow ? static_cast<const wxButton *>(liveWindow) : nullptr;
        if ( !owner )
            return wxBitmap();
        if ( callbackState )
        {
            owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return wxBitmap();
            }
        }

        const wxBitmapBundle bundle = owner->m_bitmaps[which];
        if ( !bundle.IsOk() )
            return wxBitmap();

        const std::uint64_t revision =
            impl ? impl->contentRevision : 0;
        const std::uint64_t bitmapRevision =
            owner->m_bitmapRevision;
        double scale = owner->GetDPIScaleFactor();
        if ( !std::isfinite(scale) ||
             scale < wxWinUIButtonMinProjectionScale ||
             scale > wxWinUIButtonMaxProjectionScale )
        {
            scale = 1.0;
        }

        const auto checkState =
            [lifetime, callbackState, impl, revision, bitmapRevision]()
                -> wxWinUIButtonBundleState
            {
                wxWindow * const liveWindow = lifetime.get();
                wxButton * const current =
                    liveWindow
                        ? static_cast<wxButton *>(liveWindow)
                        : nullptr;
                if ( !current || current->m_bitmapDestroying ||
                     current->m_bitmapRevision != bitmapRevision )
                {
                    return current
                        ? wxWinUIButtonBundleState::Restart
                        : wxWinUIButtonBundleState::Dead;
                }

                if ( !callbackState )
                    return wxWinUIButtonBundleState::Current;

                wxButton * const callbackOwner =
                    callbackState->GetOwner();
                if ( callbackOwner != current ||
                     !callbackOwner->m_winui ||
                     callbackOwner->m_winui.get() != impl ||
                     callbackOwner->m_winui->callbackState != callbackState ||
                     callbackOwner->m_winui->destroying )
                {
                    return wxWinUIButtonBundleState::Dead;
                }

                return callbackOwner->m_winui->contentRevision == revision
                    ? wxWinUIButtonBundleState::Current
                    : wxWinUIButtonBundleState::Restart;
            };

        wxBitmap bitmap;
        const wxWinUIButtonBundleState state =
            wxWinUIResolveButtonBundle(
                bundle, scale, checkState, bitmap);
        if ( state == wxWinUIButtonBundleState::Restart )
            continue;
        if ( state != wxWinUIButtonBundleState::Current )
            return wxBitmap();
        return bitmap;
    }

    return wxBitmap();
}

void wxButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    wxCHECK_RET( which >= State_Normal && which < State_Max,
                 wxT("invalid button bitmap state") );
    if ( m_bitmapDestroying )
        return;

    // Keep every replaced implementation alive until all owner access below
    // has completed. A user bundle destructor may reenter or destroy this
    // control, so releasing an old member in the middle of the transaction
    // would make the remaining statements unsafe.
    const wxBitmapBundle replacement = bitmap;
    std::array<wxBitmapBundle, State_Max> retired;
    for ( int state = State_Normal; state < State_Max; ++state )
        retired[state] = m_bitmaps[state];

    if ( ++m_bitmapRevision == 0 )
        ++m_bitmapRevision;

    if ( which == State_Normal && !replacement.IsOk() )
    {
        for ( wxBitmapBundle& bundle : m_bitmaps )
            bundle = wxBitmapBundle();
    }
    else
    {
        m_bitmaps[which] = replacement;

        if ( which == State_Focused && replacement.IsOk() &&
             !m_bitmaps[State_Current].IsOk() )
            m_bitmaps[State_Current] = replacement;
    }

    // Attach the pointer/focus handlers now if we just gained an interactive
    // per-state bitmap that we need to react to.
    EnsureStateHandlers();

    InvalidateBestSize();
    UpdateWinUIContent();
}

wxSize wxButton::DoGetBitmapMargins() const
{
    return m_bitmapMargins;
}

void wxButton::DoSetBitmapMargins(wxCoord x, wxCoord y)
{
    m_bitmapMargins = wxSize(x, y);
    InvalidateBestSize();
    UpdateWinUIContent();
}

void wxButton::DoSetBitmapPosition(wxDirection dir)
{
    m_bitmapPosition = dir;
    InvalidateBestSize();
    UpdateWinUIContent();
}

bool wxButton::DoGetAuthNeeded() const
{
    return m_authNeeded;
}

void wxButton::DoSetAuthNeeded(bool show)
{
    if ( m_authNeeded == show )
        return;

    m_authNeeded = show;
    InvalidateBestSize();
    UpdateWinUIContent();
}

#if wxUSE_MARKUP
bool wxButton::DoSetLabelMarkup(const wxString& markup)
{
    if ( !wxControlBase::DoSetLabelMarkup(markup) )
        return false;

    m_markup = markup;
    InvalidateBestSize();
    UpdateWinUIContent();
    return true;
}
#endif // wxUSE_MARKUP

#if wxUSE_TOOLTIPS
void wxButton::DoSetToolTipText(const wxString& tip)
{
    wxControl::DoSetToolTipText(tip);
}

void wxButton::DoSetToolTip(wxToolTip *tip)
{
    wxControl::DoSetToolTip(tip);
}
#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// sizing
// ----------------------------------------------------------------------------

wxSize wxButton::DoGetBestSize() const
{
    const wxSize defaultSize = GetDefaultSize(const_cast<wxButton *>(this));
    const wxString label = GetLabel();

    // Command-link buttons stack a bold title above a smaller note.
    if ( !DontShowLabel() && label.Find('\n') != wxNOT_FOUND )
    {
        const wxString title = wxControl::GetLabelText(label.BeforeFirst('\n'));
        const wxString note = wxControl::GetLabelText(label.AfterFirst('\n'));

        const wxSize titleSize = GetTextExtent(title);
        const wxSize noteSize = note.empty() ? wxSize(0, 0) : GetTextExtent(note);

        wxSize best;
        best.x = wxMax(titleSize.x, noteSize.x) + FromDIP(8 + 16 + 4 + 10);
        // GetTextExtent() uses the classic (smaller) GDI font, but the command
        // link renders with the larger WinUI font, so add generous vertical
        // headroom (plus the content padding) to avoid clipping the note line.
        best.y = titleSize.y + noteSize.y + FromDIP(20);

        if ( !HasFlag(wxBU_EXACTFIT) )
            best.IncTo(wxSize(defaultSize.x + FromDIP(40), FromDIP(50)));

        return best;
    }

    const wxString text = DontShowLabel()
                              ? wxString()
                              : wxControl::GetLabelText(label);
    // Best-size calculation is called by wxWindowBase, which is not allowed to
    // disappear while it caches the returned value. Never enter an
    // application-owned bundle callback from here: the content transaction
    // publishes the normal bundle's validated default logical size first.
    const bool hasBitmap =
        m_winui && m_winui->normalBitmapLogicalSizeValid;
    const wxSize bitmapLogicalSize =
        hasBitmap ? m_winui->normalBitmapLogicalSize : wxSize();

    if ( text.empty() && !hasBitmap && !m_authNeeded )
        return defaultSize;

    const wxSize textSize = text.empty()
        ? wxSize(0, 0)
        : wxWinUIMeasureText(this, text, m_hasFont ? GetFont() : wxFont());

    wxSize imageSize(0, 0);
    if ( hasBitmap )
    {
        // Best sizes are wx window coordinates (physical pixels on MSW);
        // the cached value is the bundle's validated 100%-DPI logical size.
        imageSize = FromDIP(bitmapLogicalSize);
        imageSize.x += 2 * m_bitmapMargins.x;
        imageSize.y += 2 * m_bitmapMargins.y;
    }

    const int gap = text.empty() || !hasBitmap ? 0 : FromDIP(6);
    wxSize best(0, 0);

    if ( hasBitmap && !text.empty() )
    {
        if ( m_bitmapPosition == wxTOP || m_bitmapPosition == wxBOTTOM )
        {
            best.x = wxMax(textSize.x, imageSize.x);
            best.y = textSize.y + imageSize.y + gap;
        }
        else
        {
            best.x = textSize.x + imageSize.x + gap;
            best.y = wxMax(textSize.y, imageSize.y);
        }
    }
    else if ( hasBitmap )
    {
        best = imageSize;
    }
    else
    {
        best = textSize;
    }

    if ( m_authNeeded )
    {
        const int authSize = FromDIP(16);
        if ( best.x > 0 )
            best.x += FromDIP(6);
        best.x += authSize;
        best.y = wxMax(best.y, authSize);
    }

    // WinUI Button content padding.
    best.x += FromDIP(28);
    best.y += FromDIP(14);

    if ( !HasFlag(wxBU_EXACTFIT) )
        best.IncTo(defaultSize);

    return best;
}

// ----------------------------------------------------------------------------
// per-state bitmaps
// ----------------------------------------------------------------------------

bool wxButton::HasInteractiveStateBitmap() const
{
    return m_bitmaps[State_Current].IsOk()
        || m_bitmaps[State_Pressed].IsOk()
        || m_bitmaps[State_Focused].IsOk();
}

wxBitmap wxButton::GetBitmapForState(State which,
                                     double requestedScale) const
{
    wxCHECK_MSG( which >= State_Normal && which < State_Max, wxBitmap(),
                 wxT("invalid button bitmap state") );

    const std::shared_ptr<wxWinUIButtonCallbackState> callbackState =
        m_winui ? m_winui->callbackState : nullptr;
    wxWinUIButtonImpl * const impl = m_winui.get();

    constexpr unsigned MaxGetterProjectionPasses = 8;
    for ( unsigned pass = 0; pass < MaxGetterProjectionPasses; ++pass )
    {
        const wxButton *owner = this;
        if ( callbackState )
        {
            owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui.get() != impl ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return wxBitmap();
            }
        }

        const wxBitmapBundle selectedBundle =
            owner->m_bitmaps[which];
        const wxBitmapBundle normalBundle =
            owner->m_bitmaps[State_Normal];
        const std::uint64_t revision =
            impl ? impl->contentRevision : 0;

        double scale = requestedScale;
        if ( !std::isfinite(scale) || scale <= 0.0 )
            scale = owner->GetDPIScaleFactor();
        if ( !std::isfinite(scale) ||
             scale < wxWinUIButtonMinProjectionScale ||
             scale > wxWinUIButtonMaxProjectionScale )
        {
            scale = 1.0;
        }

        const auto checkState =
            [callbackState, impl, revision]() -> wxWinUIButtonBundleState
            {
                if ( !callbackState )
                    return wxWinUIButtonBundleState::Current;

                wxButton * const current = callbackState->GetOwner();
                if ( !current || !current->m_winui ||
                     current->m_winui.get() != impl ||
                     current->m_winui->callbackState != callbackState ||
                     current->m_winui->destroying )
                {
                    return wxWinUIButtonBundleState::Dead;
                }

                return current->m_winui->contentRevision == revision
                    ? wxWinUIButtonBundleState::Current
                    : wxWinUIButtonBundleState::Restart;
            };

        const wxBitmapBundle bundle = selectedBundle.IsOk()
                                          ? selectedBundle
                                          : normalBundle;
        wxBitmap bitmap;
        const wxWinUIButtonBundleState state =
            wxWinUIResolveButtonBundle(
                bundle, scale, checkState, bitmap);
        if ( state == wxWinUIButtonBundleState::Restart )
            continue;
        if ( state != wxWinUIButtonBundleState::Current )
            return wxBitmap();

        if ( which == State_Disabled && !selectedBundle.IsOk() )
            return wxWinUICreateDisabledBitmap(bitmap);
        return bitmap;
    }

    return wxBitmap();
}

wxAnyButton::State wxButton::GetCurrentBitmapState() const
{
    if ( !IsEnabled() )
        return State_Disabled;

    if ( m_winui )
    {
        if ( m_winui->pressed && m_bitmaps[State_Pressed].IsOk() )
            return State_Pressed;

        if ( m_winui->hovered && m_bitmaps[State_Current].IsOk() )
            return State_Current;

        if ( m_winui->focused && m_bitmaps[State_Focused].IsOk() )
            return State_Focused;
    }

    return State_Normal;
}

void wxButton::EnsureStateHandlers()
{
    if ( !m_winui || m_winui->destroying || !m_winui->button ||
         m_winui->stateHandlersAttached || !HasInteractiveStateBitmap() )
        return;

    auto& impl = *m_winui;
    const std::shared_ptr<wxWinUIButtonCallbackState> callbackState =
        impl.callbackState;
    const auto updateContentNoexcept =
        [callbackState]()
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }

            wxWinUIButtonRunCallbackNoexcept(
                "interactive-state projection",
                [owner]() { owner->UpdateWinUIContent(); });
        };

    impl.pointerEnteredToken = impl.button.PointerEntered(
        [callbackState, updateContentNoexcept](
            winrt::Windows::Foundation::IInspectable const&,
            MUX::Input::PointerRoutedEventArgs const&)
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }
            owner->m_winui->hovered = true;
            updateContentNoexcept();
        });
    impl.pointerExitedToken = impl.button.PointerExited(
        [callbackState, updateContentNoexcept](
            winrt::Windows::Foundation::IInspectable const&,
            MUX::Input::PointerRoutedEventArgs const&)
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }
            owner->m_winui->hovered = false;
            owner->m_winui->pressed = false;
            updateContentNoexcept();
        });
    impl.pointerCaptureLostToken = impl.button.PointerCaptureLost(
        [callbackState, updateContentNoexcept](
            winrt::Windows::Foundation::IInspectable const&,
            MUX::Input::PointerRoutedEventArgs const&)
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }
            owner->m_winui->pressed = false;
            updateContentNoexcept();
        });
    impl.gotFocusToken = impl.button.GotFocus(
        [callbackState, updateContentNoexcept](
            winrt::Windows::Foundation::IInspectable const&,
            MUX::RoutedEventArgs const&)
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }
            owner->m_winui->focused = true;
            updateContentNoexcept();
        });
    impl.lostFocusToken = impl.button.LostFocus(
        [callbackState, updateContentNoexcept](
            winrt::Windows::Foundation::IInspectable const&,
            MUX::RoutedEventArgs const&)
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }
            owner->m_winui->focused = false;
            updateContentNoexcept();
        });

    // The Button marks PointerPressed/Released as handled, so the normal
    // events would not fire; register with handledEventsToo = true.
    impl.routedPointerPressed = MUX::Input::PointerEventHandler(
        [callbackState, updateContentNoexcept](
            winrt::Windows::Foundation::IInspectable const&,
            MUX::Input::PointerRoutedEventArgs const&)
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }
            owner->m_winui->pressed = true;
            updateContentNoexcept();
        });
    impl.routedPointerReleased = MUX::Input::PointerEventHandler(
        [callbackState, updateContentNoexcept](
            winrt::Windows::Foundation::IInspectable const&,
            MUX::Input::PointerRoutedEventArgs const&)
        {
            wxButton * const owner = callbackState->GetOwner();
            if ( !owner || !owner->m_winui ||
                 owner->m_winui->callbackState != callbackState ||
                 owner->m_winui->destroying )
            {
                return;
            }
            owner->m_winui->pressed = false;
            updateContentNoexcept();
        });
    impl.button.AddHandler(MUX::UIElement::PointerPressedEvent(),
                           winrt::box_value(impl.routedPointerPressed), true);
    impl.button.AddHandler(MUX::UIElement::PointerReleasedEvent(),
                           winrt::box_value(impl.routedPointerReleased), true);

    impl.stateHandlersAttached = true;
}

// ----------------------------------------------------------------------------
// content
// ----------------------------------------------------------------------------

bool wxButton::UpdateWinUIContent(bool forceRender,
                                  State forcedState,
                                  double requestedScale)
{
    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return true;

    wxWinUIButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIButtonCallbackState> callbackState =
        impl->callbackState;
    const MUXC::Button peer = impl->button;

    const bool replayEntry =
        impl->contentProjectionReplayEntryPending &&
        !impl->contentProjectionInProgress;
    if ( replayEntry )
    {
        // This is the single continuation which owns the remaining bounded
        // budget. Its pending request already names the latest revision.
        impl->contentProjectionReplayEntryPending = false;
        impl->contentProjectionRequested = true;
    }
    else
    {
        const bool externalRequest = !impl->contentProjectionInProgress;
        if ( externalRequest )
        {
            // A real caller supersedes any queued continuation and rearms the
            // one-replay budget. The stale callback remains harmless because
            // its ticket will no longer match.
            impl->contentProjectionDeferredUsed = false;
            impl->contentProjectionReplayScheduled = false;
            ++impl->contentReplayTicket;
            if ( impl->contentReplayTicket == 0 )
                ++impl->contentReplayTicket;
            impl->pendingForceRender = forceRender;
        }
        else
        {
            // A nested request cannot recurse into a partially published
            // content tree. Preserve a render requested by either writer.
            impl->pendingForceRender =
                impl->pendingForceRender || forceRender;
        }

        impl->pendingForcedState = forcedState;
        impl->pendingRequestedScale = requestedScale;
        ++impl->contentRevision;
        if ( impl->contentRevision == 0 )
            ++impl->contentRevision;
        impl->contentProjectionRequested = true;
    }

    if ( impl->contentProjectionInProgress )
        return true;

    impl->contentProjectionInProgress = true;
    wxScopeGuard projectionFlagGuard =
        wxMakeGuard(
            [callbackState, impl]()
            {
                wxButton * const owner = callbackState
                                            ? callbackState->GetOwner()
                                            : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->m_winui->contentProjectionInProgress = false;
                }
            });
    wxUnusedVar(projectionFlagGuard);

    const auto getCurrentOwner = [callbackState, impl]() -> wxButton *
    {
        wxButton * const owner = callbackState
                                    ? callbackState->GetOwner()
                                    : nullptr;
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       !owner->m_winui->destroying
                   ? owner
                   : nullptr;
    };

    enum class ProjectionState
    {
        Applied,
        Restart,
        Dead,
        Failed
    };

    constexpr unsigned MaxSynchronousProjectionPasses = 8;
    for ( unsigned pass = 0;
          pass < MaxSynchronousProjectionPasses;
          ++pass )
    {
        wxButton * const live = getCurrentOwner();
        if ( !live )
            return false;

        impl->contentProjectionRequested = false;
        const std::uint64_t revision = impl->contentRevision;
        const bool passForceRender = impl->pendingForceRender;
        const State passForcedState = impl->pendingForcedState;
        const double passRequestedScale = impl->pendingRequestedScale;

        // Snapshot the complete wx model before the first user-extensible
        // wxBitmapBundle callback. Only retained values and the native peer are
        // used until checkState() proves that the same revision is still live.
        const wxString label = live->GetLabel();
        const bool showLabel = !live->DontShowLabel();
        const bool commandLink =
            showLabel && label.Find('\n') != wxNOT_FOUND;
        const State bitmapState =
            passForcedState >= State_Normal && passForcedState < State_Max
                ? passForcedState
                : live->GetCurrentBitmapState();
        const wxBitmapBundle selectedBundle = live->m_bitmaps[bitmapState];
        const wxBitmapBundle normalBundle =
            live->m_bitmaps[State_Normal];
        const bool authNeeded = live->m_authNeeded;
        const wxSize bitmapMarginsDIP =
            live->ToDIP(live->m_bitmapMargins);
        const wxDirection bitmapPosition = live->m_bitmapPosition;
        const bool useForeground = live->UseForegroundColour();
        const wxColour foreground = live->GetForegroundColour();
        const double noteFontSize =
            wxMax(1.0, wxWinUIGetFontSize(live->GetFont()) - 2.0);
#if wxUSE_MARKUP
        const wxString markup = live->m_markup;
#else
        const wxString markup;
#endif
        double scale = passRequestedScale;
        if ( !std::isfinite(scale) || scale <= 0.0 )
            scale = live->GetDPIScaleFactor();
        if ( !std::isfinite(scale) ||
             scale < wxWinUIButtonMinProjectionScale ||
             scale > wxWinUIButtonMaxProjectionScale )
        {
            scale = 1.0;
        }

        const auto checkState =
            [callbackState, impl, revision]() -> wxWinUIButtonBundleState
            {
                wxButton * const owner = callbackState
                                            ? callbackState->GetOwner()
                                            : nullptr;
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->destroying )
                {
                    return wxWinUIButtonBundleState::Dead;
                }

                return owner->m_winui->contentRevision == revision
                    ? wxWinUIButtonBundleState::Current
                    : wxWinUIButtonBundleState::Restart;
            };

        ProjectionState projection = ProjectionState::Failed;
        try
        {
            wxBitmap bitmap;
            wxSize normalBitmapLogicalSize;
            bool normalBitmapLogicalSizeValid = false;
            wxWinUIButtonBundleState state =
                wxWinUIButtonBundleState::Current;
            const bool selectedIsNormal =
                selectedBundle.IsOk() && normalBundle.IsOk() &&
                selectedBundle.IsSameAs(normalBundle);
            if ( selectedBundle.IsOk() )
            {
                state = wxWinUIResolveButtonBundle(
                    selectedBundle, scale, checkState, bitmap,
                    selectedIsNormal
                        ? &normalBitmapLogicalSize
                        : nullptr);
                normalBitmapLogicalSizeValid =
                    selectedIsNormal &&
                    normalBitmapLogicalSize.x > 0 &&
                    normalBitmapLogicalSize.y > 0;
            }
            else if ( normalBundle.IsOk() )
            {
                state = wxWinUIResolveButtonBundle(
                    normalBundle, scale, checkState, bitmap,
                    &normalBitmapLogicalSize);
                normalBitmapLogicalSizeValid =
                    normalBitmapLogicalSize.x > 0 &&
                    normalBitmapLogicalSize.y > 0;
                if ( state == wxWinUIButtonBundleState::Current &&
                     bitmapState == State_Disabled )
                {
                    bitmap = wxWinUICreateDisabledBitmap(bitmap);
                }
            }

            // A non-normal selected state still needs a callback-free
            // best-size cache for the normal bundle.
            if ( state == wxWinUIButtonBundleState::Current &&
                 normalBundle.IsOk() &&
                 !normalBitmapLogicalSizeValid )
            {
                normalBitmapLogicalSize =
                    normalBundle.GetDefaultSize();
                state = checkState();
                normalBitmapLogicalSizeValid =
                    state == wxWinUIButtonBundleState::Current &&
                    normalBitmapLogicalSize.x > 0 &&
                    normalBitmapLogicalSize.y > 0;
                if ( state == wxWinUIButtonBundleState::Current &&
                     !normalBitmapLogicalSizeValid )
                {
                    state = wxWinUIButtonBundleState::Invalid;
                }
            }

            if ( state == wxWinUIButtonBundleState::Invalid )
            {
                projection = ProjectionState::Failed;
            }
            else if ( state == wxWinUIButtonBundleState::Dead )
            {
                projection = ProjectionState::Dead;
            }
            else if ( state == wxWinUIButtonBundleState::Restart )
            {
                projection = ProjectionState::Restart;
            }
            else
            {
                wxBitmap authBitmap;
                if ( authNeeded )
                {
                    wxButton * const owner = callbackState->GetOwner();
                    if ( !owner )
                    {
                        projection = ProjectionState::Dead;
                    }
                    else
                    {
                        authBitmap = owner->GetAuthBitmap(scale);
                        state = checkState();
                        if ( state == wxWinUIButtonBundleState::Dead )
                            projection = ProjectionState::Dead;
                        else if ( state == wxWinUIButtonBundleState::Restart )
                            projection = ProjectionState::Restart;
                    }
                }

                if ( projection != ProjectionState::Dead &&
                     projection != ProjectionState::Restart )
                {
                    // Command-link buttons use a dedicated two-line layout.
                    if ( commandLink )
                    {
                        const wxString title =
                            wxControl::GetLabelText(
                                label.BeforeFirst('\n'));
                        const wxString note =
                            wxControl::GetLabelText(
                                label.AfterFirst('\n'));
                        peer.Content(
                            wxWinUIMakeCommandLink(
                                title, note, bitmap, authBitmap, noteFontSize,
                                useForeground, foreground));
                    }
                    else
                    {
                        const wxString text =
                            showLabel
                                ? wxControl::GetLabelText(label)
                                : wxString();
                        const bool hasText = !text.empty();
                        const bool hasBitmap = bitmap.IsOk();
                        const bool hasAuth = authBitmap.IsOk();

                        // Fast path: a plain text label with no bitmap, auth
                        // icon or markup.
                        if ( hasText && !hasBitmap && !hasAuth &&
                             markup.empty() )
                        {
                            peer.Content(winrt::box_value(
                                wxWinUIToHString(text)));
                        }
                        else
                        {
                            constexpr double gap = 6.0;
                            const int count =
                                (hasAuth ? 1 : 0) +
                                (hasBitmap ? 1 : 0) +
                                (hasText ? 1 : 0);
                            winrt::Windows::Foundation::IInspectable
                                content{ nullptr };

                            if ( count == 1 )
                            {
                                if ( hasAuth )
                                {
                                    content = wxWinUIMakeAuthImage(
                                        authBitmap, false, gap);
                                }
                                else if ( hasBitmap )
                                {
                                    content = wxWinUIMakeImage(
                                        bitmap, bitmapMarginsDIP,
                                        bitmapPosition, false, gap);
                                }
                                else
                                {
                                    content = wxWinUIMakeTextBlock(
                                        text, markup, useForeground,
                                        foreground);
                                }
                            }
                            else if ( count > 1 )
                            {
                                MUXC::StackPanel panel;
                                panel.Orientation(
                                    bitmapPosition == wxTOP ||
                                    bitmapPosition == wxBOTTOM
                                        ? MUXC::Orientation::Vertical
                                        : MUXC::Orientation::Horizontal);
                                panel.VerticalAlignment(
                                    MUX::VerticalAlignment::Center);
                                panel.HorizontalAlignment(
                                    MUX::HorizontalAlignment::Center);
                                auto children = panel.Children();

                                if ( hasAuth )
                                {
                                    children.Append(
                                        wxWinUIMakeAuthImage(
                                            authBitmap,
                                            hasText || hasBitmap,
                                            gap));
                                }

                                const MUXC::Image image =
                                    hasBitmap
                                        ? wxWinUIMakeImage(
                                              bitmap,
                                              bitmapMarginsDIP,
                                              bitmapPosition,
                                              hasText,
                                              gap)
                                        : MUXC::Image{ nullptr };
                                const MUXC::TextBlock textBlock =
                                    hasText
                                        ? wxWinUIMakeTextBlock(
                                              text, markup,
                                              useForeground,
                                              foreground)
                                        : MUXC::TextBlock{ nullptr };

                                if ( bitmapPosition == wxRIGHT ||
                                     bitmapPosition == wxBOTTOM )
                                {
                                    if ( hasText )
                                        children.Append(textBlock);
                                    if ( hasBitmap )
                                        children.Append(image);
                                }
                                else
                                {
                                    if ( hasBitmap )
                                        children.Append(image);
                                    if ( hasText )
                                        children.Append(textBlock);
                                }

                                content = panel;
                            }

                            peer.Content(content);
                        }
                    }

                    state = checkState();
                    if ( state == wxWinUIButtonBundleState::Dead )
                    {
                        projection = ProjectionState::Dead;
                    }
                    else if ( state == wxWinUIButtonBundleState::Restart )
                    {
                        projection = ProjectionState::Restart;
                    }
                    else
                    {
                        wxButton *owner =
                            callbackState->GetOwner();
                        if ( !owner )
                        {
                            projection = ProjectionState::Dead;
                        }
                        else if ( !owner->UpdateWinUIAppearance(false) )
                        {
                            state = checkState();
                            projection =
                                state == wxWinUIButtonBundleState::Dead
                                    ? ProjectionState::Dead
                                    : state == wxWinUIButtonBundleState::Restart
                                        ? ProjectionState::Restart
                                        : ProjectionState::Failed;
                        }
                        else
                        {
                            state = checkState();
                            if ( state == wxWinUIButtonBundleState::Dead )
                            {
                                projection = ProjectionState::Dead;
                            }
                            else if ( state == wxWinUIButtonBundleState::Restart )
                            {
                                projection =
                                    ProjectionState::Restart;
                            }
                            else
                            {
                                if ( passForceRender )
                                {
                                    owner =
                                        callbackState->GetOwner();
                                    if ( !owner )
                                    {
                                        projection =
                                            ProjectionState::Dead;
                                    }
                                    else
                                    {
                                        owner->m_winui->host
                                            .ForceRender();
                                        state = checkState();
                                    }
                                }

                                if ( projection !=
                                         ProjectionState::Dead )
                                {
                                    if ( state ==
                                         wxWinUIButtonBundleState::Dead )
                                    {
                                        projection =
                                            ProjectionState::Dead;
                                    }
                                    else if ( state ==
                                              wxWinUIButtonBundleState::Restart )
                                    {
                                        projection =
                                            ProjectionState::Restart;
                                    }
                                    else
                                    {
                                        owner =
                                            callbackState->GetOwner();
                                        owner->m_winui
                                            ->projectedBitmapState =
                                                bitmapState;
                                        const bool bestSizeChanged =
                                            owner->m_winui
                                                    ->normalBitmapLogicalSizeValid !=
                                                normalBitmapLogicalSizeValid ||
                                            (normalBitmapLogicalSizeValid &&
                                             owner->m_winui
                                                     ->normalBitmapLogicalSize !=
                                                 normalBitmapLogicalSize);
                                        owner->m_winui
                                            ->normalBitmapLogicalSize =
                                                normalBitmapLogicalSize;
                                        owner->m_winui
                                            ->normalBitmapLogicalSizeValid =
                                                normalBitmapLogicalSizeValid;
                                        ++owner->m_winui
                                              ->contentGeneration;
                                        if ( owner->m_winui
                                                 ->contentGeneration == 0 )
                                        {
                                            ++owner->m_winui
                                                  ->contentGeneration;
                                        }
                                        if ( bestSizeChanged )
                                            owner->InvalidateBestSize();
                                        projection =
                                            ProjectionState::Applied;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI Button content", e);
            const wxWinUIButtonBundleState state = checkState();
            projection =
                state == wxWinUIButtonBundleState::Dead
                    ? ProjectionState::Dead
                    : state == wxWinUIButtonBundleState::Restart
                        ? ProjectionState::Restart
                        : ProjectionState::Failed;
        }

        if ( projection == ProjectionState::Dead )
            return false;

        if ( projection == ProjectionState::Restart )
            continue;

        wxButton * const owner = getCurrentOwner();
        if ( !owner )
            return false;

        if ( projection == ProjectionState::Failed )
        {
            owner->m_winui->contentProjectionInProgress = false;
            return false;
        }

        if ( owner->m_winui->contentProjectionRequested ||
             owner->m_winui->contentRevision != revision )
        {
            continue;
        }

        owner->m_winui->contentProjectionInProgress = false;
        owner->m_winui->pendingForceRender = false;
        return true;
    }

    wxButton * const owner = getCurrentOwner();
    if ( !owner )
        return false;

    owner->m_winui->contentProjectionInProgress = false;
    owner->m_winui->contentProjectionRequested = true;

    if ( !owner->m_winui->contentProjectionDeferredUsed && wxTheApp )
    {
        owner->m_winui->contentProjectionDeferredUsed = true;
        owner->m_winui->contentProjectionReplayScheduled = true;
        ++owner->m_winui->contentReplayTicket;
        if ( owner->m_winui->contentReplayTicket == 0 )
            ++owner->m_winui->contentReplayTicket;
        const std::uint64_t ticket =
            owner->m_winui->contentReplayTicket;
        const std::weak_ptr<wxWinUIButtonCallbackState> weakState(
            callbackState);

        wxTheApp->CallAfter(
            [weakState, impl, ticket]()
            {
                const std::shared_ptr<wxWinUIButtonCallbackState>
                    state = weakState.lock();
                if ( !state )
                    return;

                wxButton * const owner = state->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->destroying ||
                     !owner->m_winui
                          ->contentProjectionReplayScheduled ||
                     owner->m_winui->contentReplayTicket != ticket )
                {
                    return;
                }

                owner->m_winui->contentProjectionReplayScheduled =
                    false;
                owner->m_winui->contentProjectionReplayEntryPending =
                    true;
                const bool force =
                    owner->m_winui->pendingForceRender;
                const State stateToProject =
                    owner->m_winui->pendingForcedState;
                const double scaleToProject =
                    owner->m_winui->pendingRequestedScale;
                wxWinUIButtonRunCallbackNoexcept(
                    "deferred content projection",
                    [owner, force, stateToProject, scaleToProject]()
                    {
                        owner->UpdateWinUIContent(
                            force, stateToProject, scaleToProject);
                    });

                wxButton * const current = state->GetOwner();
                if ( current && current->m_winui &&
                     current->m_winui.get() == impl &&
                     current->m_winui->callbackState == state &&
                     !current->m_winui->destroying )
                {
                    current->m_winui
                        ->contentProjectionReplayEntryPending = false;
                }
            });
        return true;
    }

    // The replay also failed to quiesce. Quarantine this request rather than
    // manufacturing an unbounded CallAfter chain; the next explicit mutation
    // is an external request and receives a fresh bounded budget.
    owner->m_winui->contentProjectionRequested = false;
    owner->m_winui->pendingForceRender = false;
    return false;
}

bool wxButton::UpdateWinUIAppearance(bool forceRender)
{
    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return true;

    wxWinUIButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIButtonCallbackState> callbackState =
        impl->callbackState;
    const MUXC::Button peer = impl->button;

    const bool replayEntry =
        impl->appearanceProjectionReplayEntryPending &&
        !impl->appearanceProjectionInProgress;
    if ( replayEntry )
    {
        impl->appearanceProjectionReplayEntryPending = false;
        impl->appearanceProjectionRequested = true;
    }
    else
    {
        const bool externalRequest =
            !impl->appearanceProjectionInProgress;
        if ( externalRequest )
        {
            impl->appearanceProjectionDeferredUsed = false;
            impl->appearanceProjectionReplayScheduled = false;
            ++impl->appearanceReplayTicket;
            if ( impl->appearanceReplayTicket == 0 )
                ++impl->appearanceReplayTicket;
            impl->pendingAppearanceForceRender = forceRender;
        }
        else
        {
            impl->pendingAppearanceForceRender =
                impl->pendingAppearanceForceRender || forceRender;
        }

        ++impl->appearanceRevision;
        if ( impl->appearanceRevision == 0 )
            ++impl->appearanceRevision;
        impl->appearanceProjectionRequested = true;
    }

    if ( impl->appearanceProjectionInProgress )
        return true;

    impl->appearanceProjectionInProgress = true;
    wxScopeGuard projectionFlagGuard =
        wxMakeGuard(
            [callbackState, impl]()
            {
                wxButton * const owner = callbackState
                                            ? callbackState->GetOwner()
                                            : nullptr;
                if ( owner && owner->m_winui &&
                     owner->m_winui.get() == impl &&
                     owner->m_winui->callbackState == callbackState )
                {
                    owner->m_winui->appearanceProjectionInProgress = false;
                }
            });
    wxUnusedVar(projectionFlagGuard);

    const auto getCurrentOwner = [callbackState, impl]() -> wxButton *
    {
        wxButton * const owner = callbackState
                                    ? callbackState->GetOwner()
                                    : nullptr;
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       !owner->m_winui->destroying
                   ? owner
                   : nullptr;
    };

    enum class AppearanceProjectionState
    {
        Applied,
        Restart,
        Dead,
        Failed
    };

    constexpr unsigned MaxSynchronousAppearancePasses = 8;
    for ( unsigned pass = 0;
          pass < MaxSynchronousAppearancePasses;
          ++pass )
    {
        wxButton * const live = getCurrentOwner();
        if ( !live )
            return false;

        impl->appearanceProjectionRequested = false;
        const std::uint64_t revision = impl->appearanceRevision;
        const std::uint64_t contentRevision = impl->contentRevision;
        const bool passForceRender =
            impl->pendingAppearanceForceRender;

        // Snapshot the complete wx model before the first XAML setter. No wx
        // member is read again until checkState() proves this exact pass live.
        const bool enabled = live->IsEnabled();
        const bool shown = live->IsShown();
        const wxString label = live->GetLabel();
        const bool commandLinkLabel =
            label.Find('\n') != wxNOT_FOUND;
        const MUX::HorizontalAlignment horizontal =
            commandLinkLabel
                ? MUX::HorizontalAlignment::Left
                : wxWinUIGetHorizontalAlignment(live);
        const MUX::VerticalAlignment vertical =
            commandLinkLabel
                ? MUX::VerticalAlignment::Center
                : wxWinUIGetVerticalAlignment(live);
        const MUX::FlowDirection flow =
            wxWinUIGetFlowDirection(live->GetLayoutDirection());
        const wxFont font =
            live->m_hasFont ? live->GetFont() : wxNullFont;
        const wxColour foreground =
            live->UseForegroundColour()
                ? live->GetForegroundColour()
                : wxNullColour;
        const wxColour background =
            live->UseBackgroundColour()
                ? live->GetBackgroundColour()
                : wxNullColour;
        const long border =
            live->GetWindowStyleFlag() & wxBORDER_MASK;
        const wxWinUILabelPresentation labelPresentation =
            wxWinUIParseLabel(label);
        const winrt::hstring accessKey =
            wxWinUIToHString(labelPresentation.accessKey);

        const auto checkState =
            [callbackState, impl, revision,
             contentRevision]() -> wxWinUIButtonBundleState
            {
                wxButton * const owner = callbackState
                                            ? callbackState->GetOwner()
                                            : nullptr;
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != callbackState ||
                     owner->m_winui->destroying )
                {
                    return wxWinUIButtonBundleState::Dead;
                }

                return owner->m_winui->appearanceRevision == revision &&
                               owner->m_winui->contentRevision ==
                                   contentRevision
                    ? wxWinUIButtonBundleState::Current
                    : wxWinUIButtonBundleState::Restart;
            };

        AppearanceProjectionState projection =
            AppearanceProjectionState::Failed;
        wxWinUIButtonBundleState state =
            wxWinUIButtonBundleState::Current;

        try
        {
            // Every call below is exactly one synchronous XAML boundary. A
            // nested mutation only records a newer revision, and a destructive
            // callback prevents every later write to the retained peer.
            const auto boundary =
                [&state, &checkState](auto&& operation)
                {
                    if ( state != wxWinUIButtonBundleState::Current )
                        return;
                    operation();
                    state = checkState();
                };

            boundary([&]() { peer.IsEnabled(enabled); });
            boundary(
                [&]()
                {
                    peer.Visibility(
                        shown ? MUX::Visibility::Visible
                              : MUX::Visibility::Collapsed);
                });
            boundary(
                [&]() { peer.HorizontalContentAlignment(horizontal); });
            boundary(
                [&]() { peer.VerticalContentAlignment(vertical); });
            boundary([&]() { peer.FlowDirection(flow); });
            boundary([&]() { peer.MinWidth(0.0); });
            boundary([&]() { peer.MinHeight(0.0); });

            const auto familyProperty =
                MUXC::Control::FontFamilyProperty();
            const auto sizeProperty =
                MUXC::Control::FontSizeProperty();
            const auto weightProperty =
                MUXC::Control::FontWeightProperty();
            const auto styleProperty =
                MUXC::Control::FontStyleProperty();
            if ( !font.IsOk() )
            {
                boundary(
                    [&]() { peer.ClearValue(familyProperty); });
                boundary(
                    [&]() { peer.ClearValue(sizeProperty); });
                boundary(
                    [&]() { peer.ClearValue(weightProperty); });
                boundary(
                    [&]() { peer.ClearValue(styleProperty); });
            }
            else
            {
                const wxString face = font.GetFaceName();
                if ( face.empty() )
                {
                    boundary(
                        [&]() { peer.ClearValue(familyProperty); });
                }
                else
                {
                    boundary(
                        [&]()
                        {
                            peer.FontFamily(
                                MUXM::FontFamily(
                                    wxWinUIToHString(face)));
                        });
                }

                const double pointSize =
                    font.GetFractionalPointSize();
                if ( pointSize > 0.0 )
                {
                    boundary(
                        [&]()
                        {
                            peer.FontSize(
                                pointSize * 96.0 / 72.0);
                        });
                }
                else
                {
                    boundary(
                        [&]() { peer.ClearValue(sizeProperty); });
                }

                winrt::Windows::UI::Text::FontWeight fontWeight{};
                fontWeight.Weight =
                    static_cast<std::uint16_t>(
                        font.GetNumericWeight());
                boundary(
                    [&]() { peer.FontWeight(fontWeight); });

                winrt::Windows::UI::Text::FontStyle fontStyle =
                    winrt::Windows::UI::Text::FontStyle::Normal;
                if ( font.GetStyle() == wxFONTSTYLE_ITALIC )
                {
                    fontStyle =
                        winrt::Windows::UI::Text::FontStyle::Italic;
                }
                else if ( font.GetStyle() == wxFONTSTYLE_SLANT )
                {
                    fontStyle =
                        winrt::Windows::UI::Text::FontStyle::Oblique;
                }
                boundary(
                    [&]() { peer.FontStyle(fontStyle); });
            }

            if ( commandLinkLabel )
            {
                MUX::Thickness padding{};
                padding.Left = 8.0;
                padding.Top = 7.0;
                padding.Right = 10.0;
                padding.Bottom = 8.0;
                boundary([&]() { peer.Padding(padding); });
            }
            else
            {
                boundary(
                    [&]()
                    {
                        peer.ClearValue(
                            MUXC::Control::PaddingProperty());
                    });
            }

            boundary(
                [&]()
                {
                    if ( foreground.IsOk() )
                    {
                        peer.Foreground(
                            wxWinUICreateColourBrush(foreground));
                    }
                    else
                    {
                        peer.ClearValue(
                            MUXC::Control::ForegroundProperty());
                    }
                });
            boundary(
                [&]()
                {
                    if ( background.IsOk() )
                    {
                        peer.Background(
                            wxWinUICreateColourBrush(background));
                    }
                    else
                    {
                        peer.ClearValue(
                            MUXC::Control::BackgroundProperty());
                    }
                });

            boundary(
                [&]()
                {
                    MUXA::AutomationProperties::SetAccessKey(
                        peer, accessKey);
                });
            boundary([&]() { peer.AccessKey(accessKey); });

            if ( border == wxBORDER_NONE )
            {
                MUX::Thickness thickness{};
                boundary(
                    [&]() { peer.BorderThickness(thickness); });
            }
            else if ( border )
            {
                MUX::Thickness thickness{};
                thickness.Left = thickness.Top =
                    thickness.Right = thickness.Bottom = 1;
                boundary(
                    [&]() { peer.BorderThickness(thickness); });
            }
            else
            {
                boundary(
                    [&]()
                    {
                        peer.ClearValue(
                            MUXC::Control::BorderThicknessProperty());
                    });
            }

            boundary([&]() { peer.UpdateLayout(); });
            if ( passForceRender )
            {
                boundary(
                    [&]()
                    {
                        wxButton * const owner =
                            callbackState->GetOwner();
                        if ( owner )
                            owner->m_winui->host.ForceRender();
                    });
            }

            projection =
                state == wxWinUIButtonBundleState::Dead
                    ? AppearanceProjectionState::Dead
                    : state == wxWinUIButtonBundleState::Restart
                        ? AppearanceProjectionState::Restart
                        : AppearanceProjectionState::Applied;
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI Button appearance", e);
            state = checkState();
            projection =
                state == wxWinUIButtonBundleState::Dead
                    ? AppearanceProjectionState::Dead
                    : state == wxWinUIButtonBundleState::Restart
                        ? AppearanceProjectionState::Restart
                        : AppearanceProjectionState::Failed;
        }

        if ( projection == AppearanceProjectionState::Dead )
            return false;
        if ( projection == AppearanceProjectionState::Restart )
            continue;

        wxButton * const owner = getCurrentOwner();
        if ( !owner )
            return false;
        if ( projection == AppearanceProjectionState::Failed )
        {
            owner->m_winui->appearanceProjectionInProgress = false;
            return false;
        }
        if ( owner->m_winui->appearanceProjectionRequested ||
             owner->m_winui->appearanceRevision != revision ||
             owner->m_winui->contentRevision != contentRevision )
        {
            continue;
        }

        owner->m_winui->appearanceProjectionInProgress = false;
        owner->m_winui->pendingAppearanceForceRender = false;
        return true;
    }

    wxButton * const owner = getCurrentOwner();
    if ( !owner )
        return false;

    owner->m_winui->appearanceProjectionInProgress = false;
    owner->m_winui->appearanceProjectionRequested = true;

    if ( !owner->m_winui->appearanceProjectionDeferredUsed && wxTheApp )
    {
        owner->m_winui->appearanceProjectionDeferredUsed = true;
        owner->m_winui->appearanceProjectionReplayScheduled = true;
        ++owner->m_winui->appearanceReplayTicket;
        if ( owner->m_winui->appearanceReplayTicket == 0 )
            ++owner->m_winui->appearanceReplayTicket;
        const std::uint64_t ticket =
            owner->m_winui->appearanceReplayTicket;
        const std::weak_ptr<wxWinUIButtonCallbackState> weakState(
            callbackState);

        wxTheApp->CallAfter(
            [weakState, impl, ticket]()
            {
                const std::shared_ptr<wxWinUIButtonCallbackState>
                    state = weakState.lock();
                if ( !state )
                    return;

                wxButton * const owner = state->GetOwner();
                if ( !owner || !owner->m_winui ||
                     owner->m_winui.get() != impl ||
                     owner->m_winui->callbackState != state ||
                     owner->m_winui->destroying ||
                     !owner->m_winui
                          ->appearanceProjectionReplayScheduled ||
                     owner->m_winui->appearanceReplayTicket != ticket )
                {
                    return;
                }

                owner->m_winui->appearanceProjectionReplayScheduled =
                    false;
                owner->m_winui
                    ->appearanceProjectionReplayEntryPending = true;
                const bool force =
                    owner->m_winui->pendingAppearanceForceRender;
                wxWinUIButtonRunCallbackNoexcept(
                    "deferred appearance projection",
                    [owner, force]()
                    {
                        owner->UpdateWinUIAppearance(force);
                    });

                wxButton * const current = state->GetOwner();
                if ( current && current->m_winui &&
                     current->m_winui.get() == impl &&
                     current->m_winui->callbackState == state &&
                     !current->m_winui->destroying )
                {
                    current->m_winui
                        ->appearanceProjectionReplayEntryPending = false;
                }
            });
        return true;
    }

    owner->m_winui->appearanceProjectionRequested = false;
    owner->m_winui->pendingAppearanceForceRender = false;
    return false;
}

// ----------------------------------------------------------------------------
// default button (accent) / tooltip / auth shield
// ----------------------------------------------------------------------------

bool wxButton::ApplyDefaultStyle(bool on)
{
    m_isDefault = on;

    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return true;

    wxWinUIButtonImpl * const impl = m_winui.get();
    const std::shared_ptr<wxWinUIButtonCallbackState> callbackState =
        impl->callbackState;
    const MUXC::Button peer = impl->button;
    const auto getCurrentOwner = [callbackState, impl]() -> wxButton *
    {
        wxButton * const owner = callbackState
                                    ? callbackState->GetOwner()
                                    : nullptr;
        return owner && owner->m_winui &&
                       owner->m_winui.get() == impl &&
                       owner->m_winui->callbackState == callbackState &&
                       !owner->m_winui->destroying
                   ? owner
                   : nullptr;
    };

    if ( on )
    {
        // Use the standard WinUI accent style for the default button.  The
        // resource lives in the merged XamlControlsResources dictionary, so we
        // must use Lookup() (which traverses merged dictionaries) rather than
        // HasKey() (which does not).  Lookup() throws if the key is genuinely
        // absent, which we treat as "no accent style available".
        MUX::Style style{ nullptr };
        try
        {
            auto app = MUX::Application::Current();
            auto resources = app ? app.Resources()
                                 : MUX::ResourceDictionary{ nullptr };
            if ( resources )
                style = resources
                    .Lookup(winrt::box_value(winrt::hstring(L"AccentButtonStyle")))
                    .try_as<MUX::Style>();
        }
        catch ( const winrt::hresult_error& )
        {
            // AccentButtonStyle not present; leave the button with its default
            // (non-accent) style.
        }

        if ( style )
        {
            try
            {
                peer.Style(style);
            }
            catch ( const winrt::hresult_error& e )
            {
                // Applications may shadow the framework resource with a
                // Style whose TargetType is not Button. Treat this exactly
                // like a missing accent resource: preserve the logical
                // default-button state and keep the peer usable.
                wxWinUILogException("WinUI Button default style", e);
            }
        }
    }
    else
    {
        try
        {
            peer.ClearValue(MUX::FrameworkElement::StyleProperty());
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI Button default style", e);
        }
    }

    wxButton * const owner = getCurrentOwner();
    if ( !owner )
        return false;
    owner->m_winui->host.ForceRender();
    return getCurrentOwner() != nullptr;
}

wxBitmap wxButton::GetAuthBitmap(double requestedScale) const
{
#ifdef SHGSI_ICON
    // SHGSI_SMALLICON is always a 16-pixel asset and therefore becomes blurry
    // (and physically undersized) after a monitor transition. Start from the
    // large stock asset above 100%, resample to the exact physical size, and
    // retain 16 DIPs by recording the matching bitmap scale factor.
    double scale = requestedScale;
    if ( scale <= 0.0 )
    {
        const HWND hwnd =
            static_cast<HWND>(GetHWND());
        const UINT dpi = hwnd ? ::GetDpiForWindow(hwnd) : 0;
        scale = dpi ? static_cast<double>(dpi) / 96.0 : 1.0;
    }
    if ( !std::isfinite(scale) ||
         scale < wxWinUIButtonMinProjectionScale ||
         scale > wxWinUIButtonMaxProjectionScale )
    {
        scale = 1.0;
    }

    const int targetSize =
        wxMax(1, static_cast<int>(std::lround(16.0 * scale)));
    WinStruct<SHSTOCKICONINFO> sii;
    const HRESULT hr = ::SHGetStockIconInfo(SIID_SHIELD,
                                            SHGSI_ICON |
                                                (targetSize > 16
                                                     ? SHGSI_LARGEICON
                                                     : SHGSI_SMALLICON),
                                            &sii);
    if ( hr == S_OK && sii.hIcon )
    {
        wxIcon icon;
        if ( icon.CreateFromHICON(reinterpret_cast<WXHICON>(sii.hIcon)) )
        {
            wxBitmap bitmap(icon);
            if ( bitmap.GetSize() != wxSize(targetSize, targetSize) )
            {
                wxBitmap::Rescale(
                    bitmap, wxSize(targetSize, targetSize));
            }
            bitmap.SetScaleFactor(scale);
            return bitmap;
        }
        ::DestroyIcon(sii.hIcon);
    }
#endif // SHGSI_ICON

    return wxBitmap();
}

wxSize wxButtonBase::GetDefaultSize(wxWindow *win)
{
    return wxWindow::FromDIP(wxSize(90, 32), win);
}

#endif // wxUSE_BUTTON
