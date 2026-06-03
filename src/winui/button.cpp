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

} // namespace

class wxWinUIButtonImpl
{
public:
    wxWinUIControlHost host;
    MUXC::Button button{ nullptr };
    winrt::event_token clickToken{};
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    winrt::event_token pointerPressedToken{};
    winrt::event_token pointerReleasedToken{};
    winrt::event_token pointerCaptureLostToken{};
    winrt::event_token gotFocusToken{};
    winrt::event_token lostFocusToken{};
    MUX::Input::PointerEventHandler routedPointerPressed{ nullptr };
    MUX::Input::PointerEventHandler routedPointerReleased{ nullptr };
    bool hovered = false;
    bool pressed = false;
    bool focused = false;
    bool destroying = false;
};

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

    wxWinUIDebugLog("wxButton::~wxButton enter this=%p hwnd=%p impl=%p",
                    static_cast<void *>(this),
                    reinterpret_cast<void *>(GetHWND()),
                    static_cast<void *>(m_winui.get()));

    m_winui->destroying = true;

    try
    {
        if ( m_winui->button )
        {
            if ( m_winui->clickToken.value )
                m_winui->button.Click(m_winui->clickToken);
            if ( m_winui->pointerEnteredToken.value )
                m_winui->button.PointerEntered(m_winui->pointerEnteredToken);
            if ( m_winui->pointerExitedToken.value )
                m_winui->button.PointerExited(m_winui->pointerExitedToken);
            if ( m_winui->pointerPressedToken.value )
                m_winui->button.PointerPressed(m_winui->pointerPressedToken);
            if ( m_winui->pointerReleasedToken.value )
                m_winui->button.PointerReleased(m_winui->pointerReleasedToken);
            if ( m_winui->pointerCaptureLostToken.value )
                m_winui->button.PointerCaptureLost(m_winui->pointerCaptureLostToken);
            if ( m_winui->gotFocusToken.value )
                m_winui->button.GotFocus(m_winui->gotFocusToken);
            if ( m_winui->lostFocusToken.value )
                m_winui->button.LostFocus(m_winui->lostFocusToken);

            m_winui->button.Content(
                winrt::Windows::Foundation::IInspectable{ nullptr });
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI Button event cleanup", e);
    }

    m_winui->host.Close();
    wxWinUIDebugLog("wxButton::~wxButton after host.Close this=%p",
                    static_cast<void *>(this));
    m_winui.reset();
    wxWinUIDebugLog("wxButton::~wxButton leave this=%p",
                    static_cast<void *>(this));
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
        m_winui->pointerEnteredToken = m_winui->button.PointerEntered(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::PointerRoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                m_winui->hovered = true;
                UpdateWinUIContent();
            });
        m_winui->pointerExitedToken = m_winui->button.PointerExited(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::PointerRoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                m_winui->hovered = false;
                m_winui->pressed = false;
                UpdateWinUIContent();
            });
        m_winui->pointerPressedToken = m_winui->button.PointerPressed(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::PointerRoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                m_winui->pressed = true;
                UpdateWinUIContent();
            });
        m_winui->pointerReleasedToken = m_winui->button.PointerReleased(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::PointerRoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                m_winui->pressed = false;
                UpdateWinUIContent();
            });
        m_winui->pointerCaptureLostToken = m_winui->button.PointerCaptureLost(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::Input::PointerRoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                m_winui->pressed = false;
                UpdateWinUIContent();
            });
        m_winui->routedPointerPressed =
            MUX::Input::PointerEventHandler(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       MUX::Input::PointerRoutedEventArgs const&)
                {
                    if ( !m_winui || m_winui->destroying )
                        return;

                    m_winui->pressed = true;
                    UpdateWinUIContent();
                });
        m_winui->routedPointerReleased =
            MUX::Input::PointerEventHandler(
                [this](winrt::Windows::Foundation::IInspectable const&,
                       MUX::Input::PointerRoutedEventArgs const&)
                {
                    if ( !m_winui || m_winui->destroying )
                        return;

                    m_winui->pressed = false;
                    UpdateWinUIContent();
                });
        m_winui->button.AddHandler(MUX::UIElement::PointerPressedEvent(),
                                   winrt::box_value(m_winui->routedPointerPressed),
                                   true);
        m_winui->button.AddHandler(MUX::UIElement::PointerReleasedEvent(),
                                   winrt::box_value(m_winui->routedPointerReleased),
                                   true);
        m_winui->gotFocusToken = m_winui->button.GotFocus(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                m_winui->focused = true;
                UpdateWinUIContent();
            });
        m_winui->lostFocusToken = m_winui->button.LostFocus(
            [this](winrt::Windows::Foundation::IInspectable const&,
                   MUX::RoutedEventArgs const&)
            {
                if ( !m_winui || m_winui->destroying )
                    return;

                m_winui->focused = false;
                UpdateWinUIContent();
            });

        UpdateWinUIContent();
        UpdateWinUIAppearance();
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
        UpdateWinUIContent();
    }
}

wxSize wxButton::DoGetBestSize() const
{
    const wxSize defaultSize = GetDefaultSize(const_cast<wxButton *>(this));
    const wxString label = GetLabel();

    if ( !DontShowLabel() && label.Find('\n') != wxNOT_FOUND )
    {
        const wxString title = wxControl::GetLabelText(label.BeforeFirst('\n'));
        const wxString note = wxControl::GetLabelText(label.AfterFirst('\n'));

        const wxSize titleSize = GetTextExtent(title);
        const wxSize noteSize = note.empty() ? wxSize(0, 0) : GetTextExtent(note);

        wxSize best;
        best.x = wxMax(titleSize.x, noteSize.x) + FromDIP(8 + 16 + 4 + 10);
        best.y = titleSize.y + noteSize.y + FromDIP(10);

        if ( !HasFlag(wxBU_EXACTFIT) )
            best.IncTo(wxSize(defaultSize.x + FromDIP(40), FromDIP(40)));

        return best;
    }

    const wxString text = DontShowLabel()
                              ? wxString()
                              : wxControl::GetLabelText(label);
    const wxBitmap bitmap = GetBitmapForState(State_Normal);

    if ( text.empty() && !bitmap.IsOk() && !m_authNeeded )
        return defaultSize;

    // Measure the label with the actual WinUI font (wxGetTextExtent uses the
    // classic GDI font, which is narrower and would clip the text), then add
    // the WinUI Button content padding around it.
    wxSize textSize(0, 0);
    if ( !text.empty() )
    {
        try
        {
            const float inf = std::numeric_limits<float>::infinity();
            MUXC::TextBlock probe;
            probe.Text(wxWinUIToHString(text));
            wxWinUIApplyFont(probe, GetFont());
            probe.Measure({ inf, inf });
            const auto desired = probe.DesiredSize();
            textSize.x = static_cast<int>(std::ceil(desired.Width));
            textSize.y = static_cast<int>(std::ceil(desired.Height));
        }
        catch ( const winrt::hresult_error& )
        {
            textSize = GetTextExtent(text);
        }
    }

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

    best.x += FromDIP(28);
    best.y += FromDIP(14);

    if ( !HasFlag(wxBU_EXACTFIT) )
        best.IncTo(defaultSize);

    return best;
}

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
    wxWinUIDebugLog("wxButton::DoSetToolTipText enter this=%p hwnd=%p tipLen=%lu",
                    static_cast<void *>(this),
                    reinterpret_cast<void *>(GetHWND()),
                    static_cast<unsigned long>(tip.length()));
    wxWinUIDebugLog("wxButton::DoSetToolTipText leave this=%p",
                    static_cast<void *>(this));
}

void wxButton::DoSetToolTip(wxToolTip *tip)
{
    m_tooltipText = tip ? tip->GetTip() : wxString();
    wxWinUIDebugLog("wxButton::DoSetToolTip enter this=%p hwnd=%p tip=%p tipLen=%lu",
                    static_cast<void *>(this),
                    reinterpret_cast<void *>(GetHWND()),
                    static_cast<void *>(tip),
                    static_cast<unsigned long>(m_tooltipText.length()));
    delete tip;
    wxWinUIDebugLog("wxButton::DoSetToolTip leave this=%p",
                    static_cast<void *>(this));
}
#endif // wxUSE_TOOLTIPS

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

void wxButton::UpdateWinUIContent()
{
    if ( !m_winui || m_winui->destroying || !m_winui->button )
        return;

    try
    {
        const wxString label = GetLabel();
        const wxString text = DontShowLabel()
                                  ? wxString()
                                  : wxControl::GetLabelText(label);
        const bool hasText = !text.empty();
        const bool commandLinkLabel = label.Find('\n') != wxNOT_FOUND;
        const wxBitmap bitmap = m_authNeeded && commandLinkLabel
                                    ? wxBitmap()
                                    : GetBitmapForState(GetCurrentBitmapState());
        const wxBitmap authBitmap = m_authNeeded ? GetAuthBitmap() : wxBitmap();
        const bool hasBitmap = bitmap.IsOk();
        const bool hasAuthBitmap = authBitmap.IsOk();

        if ( commandLinkLabel )
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
            textCol.Width(MUX::GridLengthHelper::FromValueAndType(
                1, MUX::GridUnitType::Star));
            grid.ColumnDefinitions().Append(textCol);

            const wxString title = wxControl::GetLabelText(label.BeforeFirst('\n'));
            const wxString note = wxControl::GetLabelText(label.AfterFirst('\n'));

            MUX::FrameworkElement leading{ nullptr };
            if ( hasAuthBitmap )
            {
                MUXC::Image authImage;
                authImage.Source(wxWinUIWriteableBitmapFromBitmap(authBitmap));
                authImage.Width(static_cast<double>(authBitmap.GetDIPSize().x));
                authImage.Height(static_cast<double>(authBitmap.GetDIPSize().y));
                leading = authImage;
            }
            else
            {
                MUXC::FontIcon arrow;
                arrow.Glyph(L"\xE72A");
                arrow.FontSize(FromDIP(13));
                leading = arrow;
            }

            leading.Width(FromDIP(16));
            leading.Height(FromDIP(16));
            leading.VerticalAlignment(MUX::VerticalAlignment::Top);
            leading.HorizontalAlignment(MUX::HorizontalAlignment::Center);
            MUX::Thickness iconMargin{};
            iconMargin.Top = FromDIP(0);
            iconMargin.Right = FromDIP(4);
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
            if ( UseForegroundColour() )
                titleBlock.Foreground(wxWinUIBrushFromColour(GetForegroundColour()));
            textPanel.Children().Append(titleBlock);

            if ( !note.empty() )
            {
                MUXC::TextBlock noteBlock;
                noteBlock.Text(wxWinUIToHString(note));
                noteBlock.TextWrapping(MUX::TextWrapping::NoWrap);
                noteBlock.FontSize(wxMax(1.0, wxWinUIGetFontSize(GetFont()) - 2.0));
                noteBlock.Opacity(0.75);
                if ( UseForegroundColour() )
                    noteBlock.Foreground(wxWinUIBrushFromColour(GetForegroundColour()));
                textPanel.Children().Append(noteBlock);
            }

            MUXC::Grid::SetColumn(textPanel, 1);
            grid.Children().Append(textPanel);

            m_winui->button.Content(grid);
            UpdateWinUIAppearance();
            m_winui->host.ForceRender();
            return;
        }

        if ( hasText && !hasBitmap && !m_authNeeded
#if wxUSE_MARKUP
             && m_markup.empty()
#endif // wxUSE_MARKUP
             && !commandLinkLabel )
        {
            m_winui->button.Content(winrt::box_value(wxWinUIToHString(text)));
            UpdateWinUIAppearance();
            m_winui->host.ForceRender();
            return;
        }

        MUXC::TextBlock textBlock{ nullptr };
        if ( hasText )
        {
            textBlock = MUXC::TextBlock();
            textBlock.VerticalAlignment(MUX::VerticalAlignment::Center);
            textBlock.HorizontalAlignment(MUX::HorizontalAlignment::Center);
            if ( commandLinkLabel )
                textBlock.TextWrapping(MUX::TextWrapping::NoWrap);

            if ( UseForegroundColour() )
                textBlock.Foreground(wxWinUIBrushFromColour(GetForegroundColour()));

#if wxUSE_MARKUP
            if ( !m_markup.empty() )
            {
                auto inlines = textBlock.Inlines();
                inlines.Clear();

                wxWinUIButtonMarkupToInlines output(inlines);
                wxMarkupParser parser(output);
                parser.Parse(m_markup);
            }
            else
#endif // wxUSE_MARKUP
            {
                textBlock.Text(wxWinUIToHString(text));
            }
        }

        MUXC::Image image{ nullptr };
        if ( hasBitmap )
        {
            image = MUXC::Image();
            image.Source(wxWinUIWriteableBitmapFromBitmap(bitmap));
            image.Width(static_cast<double>(bitmap.GetDIPSize().x));
            image.Height(static_cast<double>(bitmap.GetDIPSize().y));
            image.VerticalAlignment(MUX::VerticalAlignment::Center);
            image.HorizontalAlignment(MUX::HorizontalAlignment::Center);

            MUX::Thickness margin{};
            margin.Left = m_bitmapMargins.x;
            margin.Top = m_bitmapMargins.y;
            margin.Right = m_bitmapMargins.x;
            margin.Bottom = m_bitmapMargins.y;

            if ( hasText )
            {
                const double gap = FromDIP(6);
                switch ( m_bitmapPosition )
                {
                    case wxLEFT:
                        margin.Right += gap;
                        break;
                    case wxRIGHT:
                        margin.Left += gap;
                        break;
                    case wxTOP:
                        margin.Bottom += gap;
                        break;
                    case wxBOTTOM:
                        margin.Top += gap;
                        break;
                    default:
                        break;
                }
            }

            image.Margin(margin);
        }

        MUXC::Image authImage{ nullptr };
        if ( m_authNeeded && hasAuthBitmap )
        {
            authImage = MUXC::Image();
            authImage.Source(wxWinUIWriteableBitmapFromBitmap(authBitmap));
            authImage.Width(static_cast<double>(authBitmap.GetDIPSize().x));
            authImage.Height(static_cast<double>(authBitmap.GetDIPSize().y));
            authImage.VerticalAlignment(MUX::VerticalAlignment::Center);
            authImage.HorizontalAlignment(MUX::HorizontalAlignment::Center);

            if ( hasText || hasBitmap )
            {
                MUX::Thickness margin{};
                margin.Right = FromDIP(6);
                authImage.Margin(margin);
            }
        }

        const int elementCount = (hasAuthBitmap ? 1 : 0) +
                                 (hasBitmap ? 1 : 0) +
                                 (hasText ? 1 : 0);

        if ( elementCount == 0 )
        {
            m_winui->button.Content(
                winrt::Windows::Foundation::IInspectable{ nullptr });
        }
        else if ( elementCount == 1 )
        {
            if ( hasAuthBitmap )
                m_winui->button.Content(authImage);
            else if ( hasBitmap )
                m_winui->button.Content(image);
            else
                m_winui->button.Content(textBlock);
        }
        else
        {
            MUXC::StackPanel panel;
            panel.Orientation(m_bitmapPosition == wxTOP || m_bitmapPosition == wxBOTTOM
                                  ? MUXC::Orientation::Vertical
                                  : MUXC::Orientation::Horizontal);
            panel.VerticalAlignment(wxWinUIGetVerticalAlignment(this));
            panel.HorizontalAlignment(wxWinUIGetHorizontalAlignment(this));

            auto children = panel.Children();
            if ( hasAuthBitmap )
                children.Append(authImage);

            if ( m_bitmapPosition == wxRIGHT || m_bitmapPosition == wxBOTTOM )
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

            m_winui->button.Content(panel);
        }

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
        wxWinUIApplyFont(m_winui->button, GetFont());

        if ( commandLinkLabel )
        {
            MUX::Thickness padding{};
            padding.Left = FromDIP(8);
            padding.Top = FromDIP(5);
            padding.Right = FromDIP(10);
            padding.Bottom = FromDIP(5);
            m_winui->button.Padding(padding);
        }
        else
        {
            m_winui->button.ClearValue(MUXC::Control::PaddingProperty());
        }

        if ( UseForegroundColour() )
        {
            m_winui->button.Foreground(wxWinUIBrushFromColour(GetForegroundColour()));
        }
        else
        {
            m_winui->button.ClearValue(MUXC::Control::ForegroundProperty());
        }

        if ( UseBackgroundColour() )
        {
            m_winui->button.Background(wxWinUIBrushFromColour(GetBackgroundColour()));
        }
        else
        {
            m_winui->button.ClearValue(MUXC::Control::BackgroundProperty());
        }

        const long border = GetWindowStyleFlag() & wxBORDER_MASK;
        if ( border == wxBORDER_NONE )
        {
            MUX::Thickness thickness{};
            m_winui->button.BorderThickness(thickness);
        }
        else if ( border )
        {
            MUX::Thickness thickness{};
            thickness.Left = 1;
            thickness.Top = 1;
            thickness.Right = 1;
            thickness.Bottom = 1;
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

wxSize wxButtonBase::GetDefaultSize(wxWindow *win)
{
    return wxWindow::FromDIP(wxSize(90, 32), win);
}

#endif // wxUSE_BUTTON
