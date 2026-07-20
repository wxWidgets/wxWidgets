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
#include "wx/msw/private.h"
#include "wx/msw/wrapshl.h"

#if wxUSE_MARKUP
    #include "wx/private/markupparser.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include <winrt/Microsoft.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Text.h>

#include <cmath>
#include <limits>
#include <vector>

namespace MUX = winrt::Microsoft::UI::Xaml;
namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;
namespace MUXD = winrt::Microsoft::UI::Xaml::Documents;
namespace MUXM = winrt::Microsoft::UI::Xaml::Media;

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

namespace
{

MUXM::SolidColorBrush wxWinUIBrushFromColour(const wxColour& col)
{
    return wxWinUIBrush(col.Red(), col.Green(), col.Blue(), col.Alpha());
}

double wxWinUIGetFontSize(const wxFont& font)
{
    const double pt = font.GetFractionalPointSize();
    return pt > 0.0 ? pt * 96.0 / 72.0 : 14.0;
}

auto wxWinUIGetFontWeight(const wxFont& font)
{
    return font.GetNumericWeight() >= wxFONTWEIGHT_BOLD
               ? winrt::Microsoft::UI::Text::FontWeights::Bold()
               : winrt::Microsoft::UI::Text::FontWeights::Normal();
}

auto wxWinUIGetFontStyle(const wxFont& font)
{
    return font.GetStyle() == wxFONTSTYLE_NORMAL
               ? winrt::Windows::UI::Text::FontStyle::Normal
               : winrt::Windows::UI::Text::FontStyle::Italic;
}

void wxWinUIApplyFont(const MUXC::Control& control, const wxFont& font)
{
    if ( !font.IsOk() )
        return;

    const wxString face = font.GetFaceName();
    if ( !face.empty() )
        control.FontFamily(MUXM::FontFamily(wxWinUIToHString(face)));

    control.FontSize(wxWinUIGetFontSize(font));
    control.FontWeight(wxWinUIGetFontWeight(font));
    control.FontStyle(wxWinUIGetFontStyle(font));
}

void wxWinUIApplyFont(const MUXC::TextBlock& textBlock, const wxFont& font)
{
    if ( !font.IsOk() )
        return;

    const wxString face = font.GetFaceName();
    if ( !face.empty() )
        textBlock.FontFamily(MUXM::FontFamily(wxWinUIToHString(face)));

    textBlock.FontSize(wxWinUIGetFontSize(font));
    textBlock.FontWeight(wxWinUIGetFontWeight(font));
    textBlock.FontStyle(wxWinUIGetFontStyle(font));
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

// Measure a single-line label with the real WinUI font: wxGetTextExtent() uses
// the classic GDI font which is narrower and would lead to clipped text.  When
// "font" is not Ok the probe keeps the WinUI default font/size so the measure
// matches what the button (which also keeps the WinUI default) will render.
wxSize wxWinUIMeasureText(const wxWindow *win, const wxString& text,
                          const wxFont& font)
{
    try
    {
        const float inf = std::numeric_limits<float>::infinity();
        MUXC::TextBlock probe;
        probe.Text(wxWinUIToHString(text));
        wxWinUIApplyFont(probe, font);
        probe.Measure({ inf, inf });
        const auto desired = probe.DesiredSize();
        return wxSize(static_cast<int>(std::ceil(desired.Width)),
                      static_cast<int>(std::ceil(desired.Height)));
    }
    catch ( const winrt::hresult_error& )
    {
        return win->GetTextExtent(text);
    }
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
                run.Foreground(wxWinUIBrushFromColour(fg));
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
        tb.Foreground(wxWinUIBrushFromColour(fg));

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
MUXC::Grid wxWinUIMakeCommandLink(wxWindow *win,
                                  const wxString& title,
                                  const wxString& note,
                                  const wxBitmap& authBitmap,
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
        authImage.Source(wxWinUIWriteableBitmapFromBitmap(authBitmap));
        const wxSize dip = authBitmap.GetDIPSize();
        authImage.Width(static_cast<double>(dip.x));
        authImage.Height(static_cast<double>(dip.y));
        leading = authImage;
    }
    else
    {
        MUXC::FontIcon arrow;
        arrow.Glyph(L"\xE72A");
        arrow.FontSize(win->FromDIP(13));
        leading = arrow;
    }

    leading.Width(win->FromDIP(16));
    leading.Height(win->FromDIP(16));
    leading.VerticalAlignment(MUX::VerticalAlignment::Top);
    leading.HorizontalAlignment(MUX::HorizontalAlignment::Center);
    MUX::Thickness iconMargin{};
    iconMargin.Right = win->FromDIP(4);
    // Nudge the 16px icon down so it lines up with the vertical centre of the
    // (taller) main title line rather than its top.
    iconMargin.Top = win->FromDIP(2);
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
        titleBlock.Foreground(wxWinUIBrushFromColour(fg));
    textPanel.Children().Append(titleBlock);

    if ( !note.empty() )
    {
        MUXC::TextBlock noteBlock;
        noteBlock.Text(wxWinUIToHString(note));
        noteBlock.TextWrapping(MUX::TextWrapping::NoWrap);
        noteBlock.FontSize(wxMax(1.0, wxWinUIGetFontSize(win->GetFont()) - 2.0));
        noteBlock.Opacity(0.75);
        if ( useFg )
            noteBlock.Foreground(wxWinUIBrushFromColour(fg));
        textPanel.Children().Append(noteBlock);
    }

    MUXC::Grid::SetColumn(textPanel, 1);
    grid.Children().Append(textPanel);
    return grid;
}

} // namespace

// ----------------------------------------------------------------------------
// wxWinUIButtonImpl
// ----------------------------------------------------------------------------

class wxWinUIButtonImpl
{
public:
    wxWinUIControlHost host;
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
    if ( !m_winui )
        return;

    m_winui->destroying = true;

    try
    {
        if ( m_winui->button )
        {
            if ( m_winui->clickToken.value )
                m_winui->button.Click(m_winui->clickToken);

            if ( m_winui->stateHandlersAttached )
            {
                if ( m_winui->pointerEnteredToken.value )
                    m_winui->button.PointerEntered(m_winui->pointerEnteredToken);
                if ( m_winui->pointerExitedToken.value )
                    m_winui->button.PointerExited(m_winui->pointerExitedToken);
                if ( m_winui->pointerCaptureLostToken.value )
                    m_winui->button.PointerCaptureLost(m_winui->pointerCaptureLostToken);
                if ( m_winui->gotFocusToken.value )
                    m_winui->button.GotFocus(m_winui->gotFocusToken);
                if ( m_winui->lostFocusToken.value )
                    m_winui->button.LostFocus(m_winui->lostFocusToken);
                if ( m_winui->routedPointerPressed )
                    m_winui->button.RemoveHandler(
                        MUX::UIElement::PointerPressedEvent(),
                        winrt::box_value(m_winui->routedPointerPressed));
                if ( m_winui->routedPointerReleased )
                    m_winui->button.RemoveHandler(
                        MUX::UIElement::PointerReleasedEvent(),
                        winrt::box_value(m_winui->routedPointerReleased));
            }

            m_winui->button.Content(
                winrt::Windows::Foundation::IInspectable{ nullptr });
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Button cleanup", e);
    }

    m_winui->host.Close();
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
    if ( !m_winui->host.Initialize(this) )
        return false;

    try
    {
        m_winui->button = MUXC::Button();
        m_winui->clickToken = m_winui->button.Click(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                SendClickEvent();
            });

        UpdateWinUIContent();
        UpdateWinUIAppearance();
        ApplyToolTip();
        m_winui->host.SetContent(m_winui->button);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Button creation", e);
        return false;
    }

    SetInitialSize(size);

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
    UpdateWinUIContent();

    if ( GetParent() && GetParent()->GetSizer() )
        GetParent()->Layout();
    else
        SetSize(GetBestSize());
}

wxWindow *wxButton::SetDefault()
{
    wxWindow * const winOldDefault = wxButtonBase::SetDefault();

    if ( wxButton * const oldButton = wxDynamicCast(winOldDefault, wxButton) )
        oldButton->ApplyDefaultStyle(false);

    ApplyDefaultStyle(true);

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
    UpdateWinUIAppearance();
    UpdateWinUIContent();
    return rc;
}

bool wxButton::SetFont(const wxFont& font)
{
    const bool rc = wxControl::SetFont(font);
    InvalidateBestSize();
    UpdateWinUIAppearance();
    UpdateWinUIContent();
    return rc;
}

void wxButton::SetLayoutDirection(wxLayoutDirection dir)
{
    wxControl::SetLayoutDirection(dir);
    UpdateWinUIAppearance();
}

bool wxButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_BUTTON, GetId());
    event.SetEventObject(this);
    return ProcessCommand(event);
}

void wxButton::DoEnable(bool enable)
{
    wxControl::DoEnable(enable);

    if ( m_winui && m_winui->button )
    {
        m_winui->button.IsEnabled(enable);
        // The current bitmap may depend on the enabled state (State_Disabled).
        UpdateWinUIContent();
    }
}

// ----------------------------------------------------------------------------
// bitmaps / auth / markup
// ----------------------------------------------------------------------------

wxBitmap wxButton::DoGetBitmap(State which) const
{
    if ( which < State_Normal || which >= State_Max || !m_bitmaps[which].IsOk() )
        return wxBitmap();

    return m_bitmaps[which].GetBitmapFor(this);
}

void wxButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    wxCHECK_RET( which >= State_Normal && which < State_Max,
                 wxT("invalid button bitmap state") );

    if ( which == State_Normal && !bitmap.IsOk() )
    {
        for ( wxBitmapBundle& bundle : m_bitmaps )
            bundle = wxBitmapBundle();
    }
    else
    {
        m_bitmaps[which] = bitmap;

        if ( which == State_Focused && bitmap.IsOk() &&
             !m_bitmaps[State_Current].IsOk() )
            m_bitmaps[State_Current] = bitmap;
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
    m_tooltipText = tip;
    ApplyToolTip();
}

void wxButton::DoSetToolTip(wxToolTip *tip)
{
    // wxWindowBase::SetToolTip(wxToolTip*) forwards ownership to us without
    // storing it, so we are responsible for deleting the object.
    m_tooltipText = tip ? tip->GetTip() : wxString();
    delete tip;
    ApplyToolTip();
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
    const wxBitmap bitmap = GetBitmapForState(State_Normal);

    if ( text.empty() && !bitmap.IsOk() && !m_authNeeded )
        return defaultSize;

    const wxSize textSize = text.empty()
        ? wxSize(0, 0)
        : wxWinUIMeasureText(this, text, m_hasFont ? GetFont() : wxFont());

    wxSize imageSize(0, 0);
    if ( bitmap.IsOk() )
    {
        imageSize = bitmap.GetDIPSize();
        imageSize.x += 2 * m_bitmapMargins.x;
        imageSize.y += 2 * m_bitmapMargins.y;
    }

    const int gap = text.empty() || !bitmap.IsOk() ? 0 : FromDIP(6);
    wxSize best(0, 0);

    if ( bitmap.IsOk() && !text.empty() )
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
    else if ( bitmap.IsOk() )
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

wxBitmap wxButton::GetBitmapForState(State which) const
{
    wxCHECK_MSG( which >= State_Normal && which < State_Max, wxBitmap(),
                 wxT("invalid button bitmap state") );

    if ( m_bitmaps[which].IsOk() )
        return m_bitmaps[which].GetBitmapFor(this);

    const wxBitmap normal = m_bitmaps[State_Normal].IsOk()
                                ? m_bitmaps[State_Normal].GetBitmapFor(this)
                                : wxBitmap();

    if ( which == State_Disabled )
        return wxWinUICreateDisabledBitmap(normal);

    return normal;
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

    impl.pointerEnteredToken = impl.button.PointerEntered(
        [this](winrt::Windows::Foundation::IInspectable const&,
               MUX::Input::PointerRoutedEventArgs const&)
        {
            if ( !m_winui || m_winui->destroying )
                return;
            m_winui->hovered = true;
            UpdateWinUIContent();
        });
    impl.pointerExitedToken = impl.button.PointerExited(
        [this](winrt::Windows::Foundation::IInspectable const&,
               MUX::Input::PointerRoutedEventArgs const&)
        {
            if ( !m_winui || m_winui->destroying )
                return;
            m_winui->hovered = false;
            m_winui->pressed = false;
            UpdateWinUIContent();
        });
    impl.pointerCaptureLostToken = impl.button.PointerCaptureLost(
        [this](winrt::Windows::Foundation::IInspectable const&,
               MUX::Input::PointerRoutedEventArgs const&)
        {
            if ( !m_winui || m_winui->destroying )
                return;
            m_winui->pressed = false;
            UpdateWinUIContent();
        });
    impl.gotFocusToken = impl.button.GotFocus(
        [this](winrt::Windows::Foundation::IInspectable const&,
               MUX::RoutedEventArgs const&)
        {
            if ( !m_winui || m_winui->destroying )
                return;
            m_winui->focused = true;
            UpdateWinUIContent();
        });
    impl.lostFocusToken = impl.button.LostFocus(
        [this](winrt::Windows::Foundation::IInspectable const&,
               MUX::RoutedEventArgs const&)
        {
            if ( !m_winui || m_winui->destroying )
                return;
            m_winui->focused = false;
            UpdateWinUIContent();
        });

    // The Button marks PointerPressed/Released as handled, so the normal
    // events would not fire; register with handledEventsToo = true.
    impl.routedPointerPressed = MUX::Input::PointerEventHandler(
        [this](winrt::Windows::Foundation::IInspectable const&,
               MUX::Input::PointerRoutedEventArgs const&)
        {
            if ( !m_winui || m_winui->destroying )
                return;
            m_winui->pressed = true;
            UpdateWinUIContent();
        });
    impl.routedPointerReleased = MUX::Input::PointerEventHandler(
        [this](winrt::Windows::Foundation::IInspectable const&,
               MUX::Input::PointerRoutedEventArgs const&)
        {
            if ( !m_winui || m_winui->destroying )
                return;
            m_winui->pressed = false;
            UpdateWinUIContent();
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

void wxButton::UpdateWinUIContent()
{
    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return;

#if wxUSE_MARKUP
    const wxString markup = m_markup;
#else
    const wxString markup;
#endif

    try
    {
        const wxString label = GetLabel();
        const bool showLabel = !DontShowLabel();

        // Command-link buttons use a dedicated two-line layout.
        if ( showLabel && label.Find('\n') != wxNOT_FOUND )
        {
            const wxString title = wxControl::GetLabelText(label.BeforeFirst('\n'));
            const wxString note = wxControl::GetLabelText(label.AfterFirst('\n'));
            const wxBitmap authBitmap = m_authNeeded ? GetAuthBitmap() : wxBitmap();

            m_winui->button.Content(
                wxWinUIMakeCommandLink(this, title, note, authBitmap,
                                       UseForegroundColour(),
                                       GetForegroundColour()));
            UpdateWinUIAppearance();
            m_winui->host.ForceRender();
            return;
        }

        const wxString text = showLabel ? wxControl::GetLabelText(label)
                                        : wxString();
        const bool hasText = !text.empty();
        const wxBitmap bitmap = GetBitmapForState(GetCurrentBitmapState());
        const bool hasBitmap = bitmap.IsOk();
        const wxBitmap authBitmap = m_authNeeded ? GetAuthBitmap() : wxBitmap();
        const bool hasAuth = authBitmap.IsOk();

        // Fast path: a plain text label with no bitmap, auth icon or markup.
        if ( hasText && !hasBitmap && !hasAuth && markup.empty() )
        {
            m_winui->button.Content(winrt::box_value(wxWinUIToHString(text)));
            UpdateWinUIAppearance();
            m_winui->host.ForceRender();
            return;
        }

        const double gap = FromDIP(6);
        const int count = (hasAuth ? 1 : 0) + (hasBitmap ? 1 : 0) +
                          (hasText ? 1 : 0);

        winrt::Windows::Foundation::IInspectable content{ nullptr };

        if ( count == 1 )
        {
            if ( hasAuth )
                content = wxWinUIMakeAuthImage(authBitmap, false, gap);
            else if ( hasBitmap )
                content = wxWinUIMakeImage(bitmap, m_bitmapMargins,
                                           m_bitmapPosition, false, gap);
            else
                content = wxWinUIMakeTextBlock(text, markup,
                                               UseForegroundColour(),
                                               GetForegroundColour());
        }
        else if ( count > 1 )
        {
            MUXC::StackPanel panel;
            panel.Orientation(m_bitmapPosition == wxTOP ||
                              m_bitmapPosition == wxBOTTOM
                                  ? MUXC::Orientation::Vertical
                                  : MUXC::Orientation::Horizontal);
            panel.VerticalAlignment(MUX::VerticalAlignment::Center);
            panel.HorizontalAlignment(MUX::HorizontalAlignment::Center);
            auto children = panel.Children();

            if ( hasAuth )
                children.Append(
                    wxWinUIMakeAuthImage(authBitmap, hasText || hasBitmap, gap));

            const MUXC::Image image = hasBitmap
                ? wxWinUIMakeImage(bitmap, m_bitmapMargins, m_bitmapPosition,
                                   hasText, gap)
                : MUXC::Image{ nullptr };
            const MUXC::TextBlock textBlock = hasText
                ? wxWinUIMakeTextBlock(text, markup, UseForegroundColour(),
                                       GetForegroundColour())
                : MUXC::TextBlock{ nullptr };

            if ( m_bitmapPosition == wxRIGHT || m_bitmapPosition == wxBOTTOM )
            {
                if ( hasText ) children.Append(textBlock);
                if ( hasBitmap ) children.Append(image);
            }
            else
            {
                if ( hasBitmap ) children.Append(image);
                if ( hasText ) children.Append(textBlock);
            }

            content = panel;
        }

        m_winui->button.Content(content);
        UpdateWinUIAppearance();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Button content", e);
    }

    m_winui->host.ForceRender();
}

void wxButton::UpdateWinUIAppearance()
{
    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return;

    try
    {
        m_winui->button.IsEnabled(IsEnabled());
        m_winui->button.Visibility(IsShown() ? MUX::Visibility::Visible
                                             : MUX::Visibility::Collapsed);
        const bool commandLinkLabel = GetLabel().Find('\n') != wxNOT_FOUND;

        m_winui->button.HorizontalContentAlignment(commandLinkLabel
            ? MUX::HorizontalAlignment::Left
            : wxWinUIGetHorizontalAlignment(this));
        m_winui->button.VerticalContentAlignment(commandLinkLabel
            ? MUX::VerticalAlignment::Center
            : wxWinUIGetVerticalAlignment(this));
        m_winui->button.FlowDirection(wxWinUIGetFlowDirection(GetLayoutDirection()));
        m_winui->button.MinWidth(0.0);
        m_winui->button.MinHeight(0.0);

        // Only override the font when the user explicitly set one: otherwise
        // keep the WinUI default content font (14px) so our buttons match the
        // size of stock XAML buttons instead of shrinking to the wx GUI font.
        if ( m_hasFont )
        {
            wxWinUIApplyFont(m_winui->button, GetFont());
        }
        else
        {
            m_winui->button.ClearValue(MUXC::Control::FontSizeProperty());
            m_winui->button.ClearValue(MUXC::Control::FontFamilyProperty());
            m_winui->button.ClearValue(MUXC::Control::FontWeightProperty());
            m_winui->button.ClearValue(MUXC::Control::FontStyleProperty());
        }

        if ( commandLinkLabel )
        {
            MUX::Thickness padding{};
            padding.Left = FromDIP(8);
            padding.Top = FromDIP(7);
            padding.Right = FromDIP(10);
            padding.Bottom = FromDIP(8);
            m_winui->button.Padding(padding);
        }
        else
        {
            m_winui->button.ClearValue(MUXC::Control::PaddingProperty());
        }

        if ( UseForegroundColour() )
            m_winui->button.Foreground(wxWinUIBrushFromColour(GetForegroundColour()));
        else
            m_winui->button.ClearValue(MUXC::Control::ForegroundProperty());

        if ( UseBackgroundColour() )
            m_winui->button.Background(wxWinUIBrushFromColour(GetBackgroundColour()));
        else
            m_winui->button.ClearValue(MUXC::Control::BackgroundProperty());

        const long border = GetWindowStyleFlag() & wxBORDER_MASK;
        if ( border == wxBORDER_NONE )
        {
            MUX::Thickness thickness{};
            m_winui->button.BorderThickness(thickness);
        }
        else if ( border )
        {
            MUX::Thickness thickness{};
            thickness.Left = thickness.Top = thickness.Right = thickness.Bottom = 1;
            m_winui->button.BorderThickness(thickness);
        }
        else
        {
            m_winui->button.ClearValue(MUXC::Control::BorderThicknessProperty());
        }

        m_winui->button.UpdateLayout();
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Button appearance", e);
    }

    m_winui->host.ForceRender();
}

// ----------------------------------------------------------------------------
// default button (accent) / tooltip / auth shield
// ----------------------------------------------------------------------------

void wxButton::ApplyDefaultStyle(bool on)
{
    m_isDefault = on;

    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return;

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
            m_winui->button.Style(style);
    }
    else
    {
        try
        {
            m_winui->button.ClearValue(MUX::FrameworkElement::StyleProperty());
        }
        catch ( const winrt::hresult_error& e )
        {
            wxWinUILogException("WinUI Button default style", e);
        }
    }

    m_winui->host.ForceRender();
}

void wxButton::ApplyToolTip()
{
#if wxUSE_TOOLTIPS
    if ( m_winui && !m_winui->destroying && m_winui->button )
        wxWinUISetToolTip(m_winui->button, m_tooltipText);
#endif // wxUSE_TOOLTIPS
}

wxBitmap wxButton::GetAuthBitmap() const
{
#ifdef SHGSI_ICON
    WinStruct<SHSTOCKICONINFO> sii;
    const HRESULT hr = ::SHGetStockIconInfo(SIID_SHIELD,
                                            SHGSI_ICON | SHGSI_SMALLICON,
                                            &sii);
    if ( hr == S_OK && sii.hIcon )
    {
        wxIcon icon;
        if ( icon.CreateFromHICON(reinterpret_cast<WXHICON>(sii.hIcon)) )
            return wxBitmap(icon);
    }
#endif // SHGSI_ICON

    return wxBitmap();
}

wxSize wxButtonBase::GetDefaultSize(wxWindow *win)
{
    return wxWindow::FromDIP(wxSize(90, 32), win);
}

#endif // wxUSE_BUTTON
