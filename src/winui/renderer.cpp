/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/renderer.cpp
// Purpose:     Fluent-styled wxRendererNative used by the winui toolkit
// Author:      wxWidgets development team
// Created:     2026-07-20
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// The generic controls which have no WinUI peer (wxListCtrl, wxDataViewCtrl,
// wxGrid, ...) draw their chrome through wxRendererNative.  Install a
// renderer drawing Fluent-looking headers, selections and expanders so that
// the whole generic family fits in with the real WinUI controls without
// having to reimplement each of them on a XAML peer.

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/utils.h"
    #include "wx/window.h"
#endif

#include "wx/renderer.h"
#include "wx/msw/private/metrics.h"
#include "wx/winui/private/themecolours.h"
#include "wx/winui/private/tlwhost.h"
#include "wx/winui/private/tlwhostmsw.h"

#include "private.h"
#include "hostnative.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.h>

#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <limits>

namespace
{

// Fluent-ish hover/divider tints: opaque blends of the base colour (a plain
// wxDC ignores the alpha channel of brush colours).
wxColour wxWinUISubtleFill(const wxColour& base)
{
    return base.ChangeLightness(wxWinUIIsDarkTheme() ? 115 : 95);
}

wxColour wxWinUIDividerColour(const wxColour& base)
{
    return base.ChangeLightness(wxWinUIIsDarkTheme() ? 135 : 80);
}

bool wxWinUIWindowUsesBackdrop(wxWindow *win)
{
    for ( HWND hwnd = win ? GetHwndOf(win) : nullptr; hwnd; )
    {
        if ( ::GetPropW(hwnd, L"wxWinUIBackdropTransparent") )
            return true;

        if ( !(::GetWindowLongPtr(hwnd, GWL_STYLE) & WS_CHILD) )
            break;
        hwnd = ::GetParent(hwnd);
    }

    return false;
}

wxColour wxWinUISplitterSurfaceColour(wxWindow *win)
{
    // Accessibility colours take precedence over the backdrop marker: High
    // Contrast intentionally disables decorative material assumptions.
    if ( wxMSWImpl::IsHighContrast() )
        return wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

    if ( wxWinUIWindowUsesBackdrop(win) )
    {
        // Native children use black as the composition colour key while the
        // XAML backdrop is active (see wxWinUIControlHost). Keep this named
        // protocol value here instead of treating it as a theme colour.
        static const wxColour s_backdropColourKey(0, 0, 0);
        return s_backdropColourKey;
    }

    const wxColour background = win->GetBackgroundColour();
    return background.IsOk()
        ? background
        : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
}

// The sash of a splitter: a hairline along it, a grip in its middle, and a
// fill shown while the pointer is over it. XAML draws it, in the canvas
// standing for the splitter, so that it blends with whatever is behind (the
// backdrop included) and follows the theme. It takes no input: the pointer
// goes through it to the splitter, which does the dragging.
const wchar_t wxWinUISashMarkup[] =
    L"<Grid"
    L" xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\""
    L" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\""
    L" IsHitTestVisible=\"False\">"
    L"<Border x:Name=\"Hover\" Opacity=\"0\" CornerRadius=\"3\""
    L" Background=\"{ThemeResource SubtleFillColorSecondaryBrush}\">"
    L"<Border.OpacityTransition>"
    L"<ScalarTransition Duration=\"0:0:0.083\"/>"
    L"</Border.OpacityTransition>"
    L"</Border>"
    L"<Border x:Name=\"Line\""
    L" Background=\"{ThemeResource DividerStrokeColorDefaultBrush}\"/>"
    L"<Border x:Name=\"Grip\" CornerRadius=\"1.5\""
    L" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\""
    L" Background=\"{ThemeResource ControlStrongFillColorDefaultBrush}\"/>"
    L"</Grid>";

// sash is in client pixels of the splitter. Returns false if XAML could not
// be made to show it.
bool wxWinUIShowSplitterSash(wxWindow *win,
                             const wxSize& clientSize,
                             wxRect sash,
                             bool vertical,
                             bool hot)
{
    wxWinUITopLevelHost * const host =
        wxWinUITopLevelHost::ForWindow(win, false);
    if ( !host )
        return false;

    try
    {
        namespace X = winrt::Microsoft::UI::Xaml;

        const auto sashPart =
            wxWinUITopLevelHost::WindowPartKind::SplitterSash;
        auto root =
            host->GetWindowPart(win, sashPart).try_as<X::Controls::Grid>();
        if ( !root )
        {
            root = X::Markup::XamlReader::Load(wxWinUISashMarkup)
                .as<X::Controls::Grid>();
        }

        // Also when it is already there: the splitter may have been given
        // another parent since.
        host->SetWindowPart(win, sashPart, root);
        if ( host->GetWindowPart(win, sashPart) != root )
            return false;

        const double scale = host->GetScale();
        if ( scale <= 0 )
            return false;

        // The canvas is laid out left to right whatever the direction of
        // the window, whose own coordinates are mirrored.
        if ( win->GetLayoutDirection() == wxLayout_RightToLeft )
            sash.x = clientSize.x - sash.x - sash.width;

        X::Controls::Canvas::SetLeft(root, sash.x / scale);
        X::Controls::Canvas::SetTop(root, sash.y / scale);
        root.Width(sash.width / scale);
        root.Height(sash.height / scale);

        const double stretch = std::numeric_limits<double>::quiet_NaN();
        const auto line = root.FindName(L"Line").as<X::FrameworkElement>();
        line.Width(vertical ? 1.0 : stretch);
        line.Height(vertical ? stretch : 1.0);
        line.HorizontalAlignment(vertical ? X::HorizontalAlignment::Center
                                          : X::HorizontalAlignment::Stretch);
        line.VerticalAlignment(vertical ? X::VerticalAlignment::Stretch
                                        : X::VerticalAlignment::Center);

        // The grip never takes more than a third of a short sash.
        const double length =
            (vertical ? sash.height : sash.width) / scale;
        const double grip = std::floor(std::min(32.0, length / 3));
        const auto gripElement =
            root.FindName(L"Grip").as<X::FrameworkElement>();
        gripElement.Width(vertical ? 3.0 : grip);
        gripElement.Height(vertical ? grip : 3.0);

        root.FindName(L"Hover").as<X::UIElement>().Opacity(hot ? 1.0 : 0.0);
        return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI splitter sash", e);
        return false;
    }
}

// Draws a glyph of the icon font of Windows 11 (of Windows 10 where it is
// missing) in the middle of a rectangle. The size is in points: the 12 DIPs
// of the chevrons of WinUI are 9 points.
void wxWinUIDrawIconGlyph(wxDC& dc, const wxRect& rect, const wxString& glyph,
                          const wxColour& colour, int points = 9)
{
    wxFont font(wxFontInfo(points).FaceName("Segoe Fluent Icons"));
    if ( !font.IsOk() || font.GetFaceName() != "Segoe Fluent Icons" )
        font = wxFont(wxFontInfo(points).FaceName("Segoe MDL2 Assets"));

    const wxFont previous = dc.GetFont();
    const wxColour previousColour = dc.GetTextForeground();
    const int previousMode = dc.GetBackgroundMode();
    dc.SetFont(font);
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc.SetTextForeground(colour);
    const wxSize extent = dc.GetTextExtent(glyph);
    dc.DrawText(glyph, rect.x + (rect.width - extent.x) / 2,
                rect.y + (rect.height - extent.y) / 2);
    dc.SetTextForeground(previousColour);
    dc.SetBackgroundMode(previousMode);
    dc.SetFont(previous);
}

class wxRendererWinUI : public wxDelegateRendererNative
{
public:
    // Delegate everything we don't override to the classic MSW renderer.
    wxRendererWinUI() : wxDelegateRendererNative(wxRendererNative::GetDefault())
    {
    }

    int DrawHeaderButton(wxWindow *win,
                         wxDC& dc,
                         const wxRect& rect,
                         int flags = 0,
                         wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                         wxHeaderButtonParams* params = nullptr) override
    {
        // Flat header: plain background, hover tint, hairline bottom divider.
        const wxColour bg = win->GetBackgroundColour();
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(bg));
        dc.DrawRectangle(rect);

        if ( flags & (wxCONTROL_CURRENT | wxCONTROL_PRESSED) )
        {
            dc.SetBrush(wxBrush(wxWinUISubtleFill(bg)));
            wxRect fill = rect;
            fill.Deflate(win->FromDIP(2));
            dc.DrawRoundedRectangle(fill, win->FromDIP(4));
        }

        dc.SetPen(wxPen(wxWinUIDividerColour(bg)));
        dc.DrawLine(rect.GetLeft(), rect.GetBottom(),
                    rect.GetRight() + 1, rect.GetBottom());

        return DrawHeaderButtonContents(win, dc, rect, flags, sortArrow, params);
    }

    int GetHeaderButtonHeight(wxWindow *win) override
    {
        return win->FromDIP(32);
    }

    void DrawItemSelectionRect(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0) override
    {
        if ( !(flags & (wxCONTROL_SELECTED | wxCONTROL_CURRENT)) )
            return;

        // As the item of a WinUI list: a subtle fill with rounded corners
        // and, when it is selected, a short bar in the accent colour at its
        // start. The text keeps its colour: this is not the highlight colour
        // behind white text of the classic lists.
        const wxColour ground = win->GetBackgroundColour();
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(wxWinUIBlendOver(
            wxWinUIGetThemeColour(wxWinUIThemeColour::SubtleFill), ground)));
        wxRect pill = rect;
        pill.Deflate(win->FromDIP(2), win->FromDIP(1));
        dc.DrawRoundedRectangle(pill, win->FromDIP(4));

        if ( flags & wxCONTROL_SELECTED )
        {
            const int width = win->FromDIP(3);
            const int height = wxMax(width,
                wxMin(win->FromDIP(16), pill.height - 2 * win->FromDIP(4)));
            const wxColour accent =
                wxWinUIGetThemeColour(wxWinUIThemeColour::Accent);
            dc.SetBrush(wxBrush(
                wxColour(accent.Red(), accent.Green(), accent.Blue())));
            dc.DrawRoundedRectangle(
                wxRect(pill.x, pill.y + (pill.height - height) / 2,
                       width, height),
                width / 2.0);
        }
    }

    void DrawFocusRect(wxWindow *win,
                       wxDC& dc,
                       const wxRect& rect,
                       int WXUNUSED(flags) = 0) override
    {
        // Rounded outline instead of the classic dotted rectangle, in the
        // colour of the text as the focus rectangle of WinUI is.
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(wxWinUIBlendOver(
            wxWinUIGetThemeColour(wxWinUIThemeColour::TextPrimary),
            win->GetBackgroundColour())));
        dc.DrawRoundedRectangle(rect, win->FromDIP(4));
    }

    // A Fluent surface has no 3-D edges: draw nothing for the border and a
    // flat, theme-coloured sash.  Otherwise wxSP_3D splitters show the classic
    // light highlight line, which reads as a bright scar on a dark theme.
    void DrawSplitterBorder(wxWindow *WXUNUSED(win),
                            wxDC& WXUNUSED(dc),
                            const wxRect& WXUNUSED(rect),
                            int WXUNUSED(flags) = 0) override
    {
    }

    void DrawSplitterSash(wxWindow *win,
                          wxDC& dc,
                          const wxSize& size,
                          wxCoord position,
                          wxOrientation orient,
                          int flags = 0) override
    {
        // Paint explicitly: WM_PAINT is allowed without a preceding erase.
        // Leaving this rectangle untouched was the source of the persistent
        // white scar after resize. The same policy also remains meaningful
        // when High Contrast disables Mica.
        const wxCoord width = win->FromDIP(6);
        const wxRect sash = orient == wxVERTICAL
            ? wxRect(position, 0, width, size.y)
            : wxRect(0, position, size.x, width);

        // On a paint surface nothing has to be painted for the backdrop to
        // show: the rectangle is made transparent again. On a HWND, the
        // colour which stands for it is.
        if ( !wxWinUIPaintSurfaceClearRect(dc.GetHDC(), sash.x, sash.y,
                                           sash.width, sash.height) )
        {
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.SetBrush(wxBrush(wxWinUISplitterSurfaceColour(win)));
            dc.DrawRectangle(sash);
        }

        // What is seen of the sash is drawn by XAML above this cleared
        // rectangle.
        const bool shownByXaml = wxWinUIShowSplitterSash(
            win, size, sash, orient == wxVERTICAL,
            (flags & wxCONTROL_CURRENT) != 0);

        // High Contrast removes the material cue, so a surface-coloured sash
        // alone can disappear into its panes. Use system colours only and
        // retain a visible one-DIP separator in every HC palette.
        if ( !shownByXaml && wxMSWImpl::IsHighContrast() )
        {
            const wxCoord markerDIP = win->FromDIP(1);
            const wxCoord markerWidth = markerDIP > 1 ? markerDIP : 1;
            const wxRect marker = orient == wxVERTICAL
                ? wxRect(sash.x + (sash.width - markerWidth) / 2,
                         sash.y, markerWidth, sash.height)
                : wxRect(sash.x,
                         sash.y + (sash.height - markerWidth) / 2,
                         sash.width, markerWidth);
            dc.SetBrush(wxBrush(
                wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)));
            dc.DrawRectangle(marker);
        }
    }

    wxSplitterRenderParams GetSplitterParams(const wxWindow *win) override
    {
        // No 3-D border, whatever the window style asks for. The sash shows
        // that the pointer is over it.
        return wxSplitterRenderParams(win->FromDIP(6), 0, true);
    }

    void DrawTreeItemButton(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags = 0) override
    {
        // The chevron of a WinUI TreeView: towards the end of the line when
        // collapsed, down when expanded.
        const bool rtl = win->GetLayoutDirection() == wxLayout_RightToLeft;
        wxWinUIDrawIconGlyph(dc, rect,
            flags & wxCONTROL_EXPANDED ? wxString(L"\uE70D")
                                       : wxString(rtl ? L"\uE76B" : L"\uE76C"),
            wxWinUIBlendOver(
                wxWinUIGetThemeColour(wxWinUIThemeColour::TextSecondary),
                win->GetBackgroundColour()));
    }

    // The button of a collapsible header (wxCollapsibleHeaderCtrl): the same
    // chevron, in a square of 16.
    void DrawCollapseButton(wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags = 0) override
    {
        DrawTreeItemButton(win, dc, rect, flags);
    }

    wxSize GetCollapseButtonSize(wxWindow *win,
                                 wxReadOnlyDC& WXUNUSED(dc)) override
    {
        return win->FromDIP(wxSize(16, 16));
    }
};

} // anonymous namespace

void wxWinUIDrawComboBoxFace(wxWindow *win, wxDC& dc, const wxRect& rect,
                             const wxRect& button, int buttonFlags,
                             bool focused)
{
    // What is behind: a colour the program gave to one of the windows the
    // control is in, or else the base colour of a window of WinUI. The
    // default colour of a panel is not what is seen behind the control: the
    // fill computed over it was lighter than the one of the ComboBox beside.
    wxColour ground = wxWinUIGetThemeColour(wxWinUIThemeColour::BaseFill);
    for ( const wxWindow *w = win->GetParent(); w; w = w->GetParent() )
    {
        if ( w->UseBackgroundColour() )
        {
            ground = w->GetBackgroundColour();
            break;
        }
        if ( w->IsTopLevel() )
            break;
    }
    const auto over = [&ground](wxWinUIThemeColour which)
    {
        return wxWinUIBlendOver(wxWinUIGetThemeColour(which), ground);
    };

    // The corners outside the rounded field show what is behind.
    if ( !wxWinUIPaintSurfaceClearRect(dc.GetHDC(), rect.x, rect.y,
                                       rect.width, rect.height) )
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(ground));
        dc.DrawRectangle(rect);
    }

    const int radius = win->FromDIP(4);
    const wxColour fill = over(wxWinUIThemeColour::ControlFill);
    dc.SetPen(wxPen(over(wxWinUIThemeColour::ControlStroke)));
    dc.SetBrush(wxBrush(fill));
    dc.DrawRoundedRectangle(rect, radius);

    // With the focus, the line in the accent colour under a WinUI field.
    // Without it nothing more than the frame: the ComboBox of WinUI beside
    // has no stronger line under it.
    dc.SetPen(*wxTRANSPARENT_PEN);
    if ( focused )
    {
        const wxColour accent =
            wxWinUIGetThemeColour(wxWinUIThemeColour::Accent);
        const int lineHeight = win->FromDIP(2);
        dc.SetBrush(wxBrush(
            wxColour(accent.Red(), accent.Green(), accent.Blue())));
        dc.DrawRectangle(rect.x + radius, rect.GetBottom() - lineHeight + 1,
                         rect.width - 2 * radius, lineHeight);
    }

    if ( button.IsEmpty() )
        return;

    if ( buttonFlags & (wxCONTROL_CURRENT | wxCONTROL_PRESSED) )
    {
        wxRect hot = button;
        hot.Deflate(win->FromDIP(3));
        dc.SetBrush(wxBrush(wxWinUIBlendOver(
            wxWinUIGetThemeColour(wxWinUIThemeColour::SubtleFill), fill)));
        dc.DrawRoundedRectangle(hot, radius);
    }

    // The chevron of a WinUI ComboBox.
    wxWinUIDrawIconGlyph(dc, button, wxString(L"\uE70D"),
        wxWinUIBlendOver(
            wxWinUIGetThemeColour(wxWinUIThemeColour::TextSecondary), fill));
}

namespace
{

// The face of a generic combo: a Grid which holds what a text field of
// WinUI looks like with the focus, hidden without it, and a ComboBox over
// it.
struct wxWinUIComboFace
{
    winrt::Microsoft::UI::Xaml::Controls::Grid grid{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::Border focus{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::ComboBox combo{ nullptr };
};

wxWinUIComboFace wxWinUIGetComboFace(wxWinUITopLevelHost *host, wxWindow *win)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxWinUIComboFace face;
    face.grid = host->GetWindowPart(
        win, wxWinUITopLevelHost::WindowPartKind::Ground).try_as<MUXC::Grid>();
    if ( face.grid && face.grid.Children().Size() == 2 )
    {
        face.focus = face.grid.Children().GetAt(0).try_as<MUXC::Border>();
        face.combo = face.grid.Children().GetAt(1).try_as<MUXC::ComboBox>();
    }
    return face;
}

// The field of an editable ComboBox, a TextBox of its template.
winrt::Microsoft::UI::Xaml::Controls::TextBox
wxWinUIFindComboFaceText(
    const winrt::Microsoft::UI::Xaml::DependencyObject& root)
{
    namespace MUXM = winrt::Microsoft::UI::Xaml::Media;
    using winrt::Microsoft::UI::Xaml::Controls::TextBox;

    if ( !root )
        return nullptr;

    if ( const TextBox text = root.try_as<TextBox>() )
    {
        if ( text.Name() == L"EditableText" )
            return text;
    }

    const int count = MUXM::VisualTreeHelper::GetChildrenCount(root);
    for ( int n = 0; n != count; ++n )
    {
        if ( const TextBox text = wxWinUIFindComboFaceText(
                 MUXM::VisualTreeHelper::GetChild(root, n)) )
        {
            return text;
        }
    }
    return nullptr;
}

// The list of the face of a combo while it is open.
struct wxWinUIComboListSession
{
    winrt::Microsoft::UI::Xaml::Controls::ComboBox combo{ nullptr };
    winrt::event_token closedToken{};
    winrt::event_token changedToken{};
    int initial = -1;
    int picked = -1;            // the item the user chose in the list
    bool accepted = false;      // closed by Enter: the selected item is taken
    bool programmatic = false;  // the selection is being set from here
    bool finished = false;
    std::function<void (int)> closed;
};

std::map<const wxWindow *, std::shared_ptr<wxWinUIComboListSession>>
    gs_comboLists;

void wxWinUIFinishComboFaceList(
    const std::shared_ptr<wxWinUIComboListSession>& session, wxWindow *win)
{
    if ( session->finished )
        return;
    session->finished = true;

    int chosen = -1;
    try
    {
        const auto combo = session->combo;
        combo.DropDownClosed(session->closedToken);
        combo.SelectionChanged(session->changedToken);
        chosen = session->accepted ? combo.SelectedIndex() : session->picked;

        // The face shows nothing of its own: the control of wx shows the
        // value.
        session->programmatic = true;
        combo.SelectedIndex(-1);
        combo.Items().Clear();
        if ( combo.IsEditable() )
            combo.Text(L"");
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI combo list closing", e);
    }

    for ( auto it = gs_comboLists.begin(); it != gs_comboLists.end(); ++it )
    {
        if ( it->second == session )
        {
            gs_comboLists.erase(it);
            break;
        }
    }

    if ( win && session->closed )
        session->closed(chosen != session->initial ? chosen : -1);
}

} // anonymous namespace

bool wxWinUIOpenComboFaceList(wxWindow *win, const wxArrayString& items,
                              int selection, std::function<void (int)> closed)
{
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxWinUITopLevelHost * const host =
        win ? wxWinUITopLevelHost::ForWindow(win, false) : nullptr;
    if ( !host || items.empty() || gs_comboLists.count(win) )
        return false;

    const auto session = std::make_shared<wxWinUIComboListSession>();
    try
    {
        const MUXC::ComboBox combo = wxWinUIGetComboFace(host, win).combo;
        if ( !combo || !combo.IsLoaded() )
            return false;

        session->combo = combo;
        session->initial =
            selection >= 0 && selection < static_cast<int>(items.size())
                ? selection : -1;
        session->closed = std::move(closed);

        session->programmatic = true;
        combo.Items().Clear();
        for ( const wxString& item : items )
            combo.Items().Append(winrt::box_value(wxWinUIToHString(item)));
        combo.SelectedIndex(session->initial);
        session->programmatic = false;

        session->changedToken = combo.SelectionChanged(
            [session](winrt::Windows::Foundation::IInspectable const&,
                      MUXC::SelectionChangedEventArgs const&)
            {
                if ( !session->programmatic )
                    session->picked = session->combo.SelectedIndex();
            });

        // Its items are taken away once it has closed: not from inside its
        // own callback.
        const wxWeakRef<wxWindow> weak(win);
        session->closedToken = combo.DropDownClosed(
            [session, weak](winrt::Windows::Foundation::IInspectable const&,
                            winrt::Windows::Foundation::IInspectable const&)
            {
                const auto finish = [session, weak]()
                {
                    wxWinUIFinishComboFaceList(session, weak.get());
                };
                if ( !session->combo.DispatcherQueue().TryEnqueue(finish) )
                    finish();
            });

        gs_comboLists[win] = session;
        combo.IsDropDownOpen(true);
        if ( combo.IsDropDownOpen() )
            return true;
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI combo list opening", e);
    }

    // Not opened: nothing of it is left.
    session->closed = nullptr;
    if ( session->combo )
        wxWinUIFinishComboFaceList(session, nullptr);
    gs_comboLists.erase(win);
    return false;
}

void wxWinUIMoveComboFaceListSelection(wxWindow *win, int by)
{
    const auto found = gs_comboLists.find(win);
    if ( found == gs_comboLists.end() )
        return;

    const auto session = found->second;
    try
    {
        const int count = static_cast<int>(session->combo.Items().Size());
        if ( !count )
            return;

        const int now = session->combo.SelectedIndex();
        const int next = now < 0 ? (by > 0 ? 0 : count - 1)
                                 : wxClip(now + by, 0, count - 1);
        session->programmatic = true;
        session->combo.SelectedIndex(next);
        session->programmatic = false;
    }
    catch ( const winrt::hresult_error& e )
    {
        session->programmatic = false;
        wxWinUILogException("WinUI combo list selection", e);
    }
}

void wxWinUICloseComboFaceList(wxWindow *win, bool accept)
{
    const auto found = gs_comboLists.find(win);
    if ( found == gs_comboLists.end() )
        return;

    const auto session = found->second;
    session->accepted = accept;
    try
    {
        session->combo.IsDropDownOpen(false);
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI combo list dismissal", e);
    }
}

bool wxWinUIIsComboFaceListOpen(const wxWindow *win)
{
    return gs_comboLists.count(win) != 0;
}

void wxWinUIForgetComboFaceList(wxWindow *win)
{
    const auto found = gs_comboLists.find(win);
    if ( found == gs_comboLists.end() )
        return;

    const auto session = found->second;
    session->closed = nullptr;
    gs_comboLists.erase(found);
    try
    {
        session->combo.IsDropDownOpen(false);
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxWinUISetComboBoxFace(wxWindow *win, int flags)
{
    namespace MUX = winrt::Microsoft::UI::Xaml;
    namespace MUXC = winrt::Microsoft::UI::Xaml::Controls;

    wxWinUITopLevelHost * const host =
        win ? wxWinUITopLevelHost::ForWindow(win, false) : nullptr;
    if ( !host )
        return;

    const double scale = host->GetScale();
    if ( scale <= 0 )
        return;

    try
    {
        wxWinUIComboFace face = wxWinUIGetComboFace(host, win);
        if ( !face.combo )
        {
            // The control of wx handles the mouse and the keys, and is what
            // a screen reader sees: this is its face only. The text field
            // of an editable ComboBox of WinUI is a TextBox with no frame of
            // its own until it has the focus, when it shows the ground and
            // the line in the accent colour of a text field: the brushes
            // and the thickness of that state, behind the ComboBox.
            face.grid = MUX::Markup::XamlReader::Load(
                L"<Grid"
                L" xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
                L" IsHitTestVisible='False'>"
                L"<Border Opacity='0'"
                L" CornerRadius='{ThemeResource ControlCornerRadius}'"
                L" Background='{ThemeResource TextControlBackgroundFocused}'"
                L" BorderBrush='{ThemeResource TextControlBorderBrushFocused}'"
                L" BorderThickness='{ThemeResource TextControlBorderThemeThicknessFocused}'/>"
                L"<ComboBox IsTabStop='False'"
                L" HorizontalAlignment='Stretch' VerticalAlignment='Stretch'"
                L" AutomationProperties.AccessibilityView='Raw'/>"
                L"</Grid>").as<MUXC::Grid>();
            face.focus = face.grid.Children().GetAt(0).as<MUXC::Border>();
            face.combo = face.grid.Children().GetAt(1).as<MUXC::ComboBox>();

            // Its states are set once it has its template: painted again
            // then.
            const wxWeakRef<wxWindow> weak(win);
            face.combo.Loaded(
                [weak](winrt::Windows::Foundation::IInspectable const&,
                       MUX::RoutedEventArgs const&)
                {
                    if ( wxWindow * const live = weak.get() )
                        live->Refresh();
                });
            host->SetWindowPart(
                win, wxWinUITopLevelHost::WindowPartKind::Ground, face.grid);
        }

        const MUXC::ComboBox combo = face.combo;
        const bool disabled = (flags & wxWINUI_COMBO_FACE_DISABLED) != 0;
        const bool hover = (flags & wxWINUI_COMBO_FACE_HOVER) != 0;
        const bool pressed = (flags & wxWINUI_COMBO_FACE_PRESSED) != 0;
        const bool textFocus =
            !disabled && (flags & wxWINUI_COMBO_FACE_TEXT_FOCUS) != 0;
        if ( wxWinUIInputLogEnabled() )
            wxWinUIInputLog("combo FACE win=%p flags=%02x", win, flags);

        if ( combo.IsEnabled() == disabled )
            combo.IsEnabled(!disabled);

        // The list of an editable ComboBox opens under it, the one of a
        // ComboBox which is not over it: the face is of the kind of the
        // control. Its own text field, which the one of wx stands for,
        // shows nothing and takes neither the pointer nor the focus.
        const bool editable = (flags & wxWINUI_COMBO_FACE_EDITABLE) != 0;
        if ( combo.IsEditable() != editable &&
             !gs_comboLists.count(win) )
        {
            combo.IsEditable(editable);
        }
        if ( editable && !face.grid.Tag() )
        {
            if ( const MUXC::TextBox text = wxWinUIFindComboFaceText(combo) )
            {
                text.Opacity(0);
                text.IsTabStop(false);
                text.IsHitTestVisible(false);
                face.grid.Tag(winrt::box_value(true));
            }
        }

        const wxSize client = win->GetClientSize();
        face.grid.Width(client.x / scale);
        face.grid.Height(client.y / scale);
        MUXC::Canvas::SetLeft(face.grid, 0);
        MUXC::Canvas::SetTop(face.grid, 0);

        // With the focus in its text field, the field is what shows: the
        // ComboBox over it keeps its chevron only.
        const bool showsFocus = face.focus.Opacity() > 0;
        if ( textFocus != showsFocus )
        {
            face.focus.Opacity(textFocus ? 1 : 0);
            if ( textFocus )
            {
                const MUX::Media::SolidColorBrush none(
                    winrt::Windows::UI::Color{ 0, 0, 0, 0 });
                combo.Background(none);
                combo.BorderBrush(none);
            }
            else
            {
                combo.ClearValue(MUXC::Control::BackgroundProperty());
                combo.ClearValue(MUXC::Control::BorderBrushProperty());
            }
        }

        // The ComboBox sets its states itself when the pointer or the focus
        // reaches it, which they never do here.
        if ( !disabled )
        {
            MUX::VisualStateManager::GoToState(
                combo,
                textFocus ? L"Normal"
                          : pressed ? L"Pressed"
                                    : hover ? L"PointerOver" : L"Normal",
                true);

            bool keyFocus = false;
            if ( const auto tag = combo.Tag() )
                keyFocus = winrt::unbox_value_or<bool>(tag, false);
            MUX::VisualStateManager::GoToState(
                combo,
                keyFocus && !(flags & wxWINUI_COMBO_FACE_EDITABLE)
                    ? (pressed ? L"FocusedPressed" : L"Focused")
                    : L"Unfocused",
                true);
        }
    }
    catch ( const winrt::hresult_error& e )
    {
        wxWinUILogException("WinUI combo face", e);
    }
}

void wxWinUISetComboBoxFaceKeyFocus(wxWindow *win, bool on)
{
    wxWinUITopLevelHost * const host =
        win ? wxWinUITopLevelHost::ForWindow(win, false) : nullptr;
    if ( !host )
        return;

    try
    {
        if ( const auto combo = wxWinUIGetComboFace(host, win).combo )
            combo.Tag(winrt::box_value(on));
    }
    catch ( const winrt::hresult_error& )
    {
    }
}

void wxWinUIHideSplitterSash(wxWindow *splitter)
{
    wxWinUITopLevelHost * const host =
        splitter ? wxWinUITopLevelHost::ForWindow(splitter, false) : nullptr;
    if ( host )
    {
        host->SetWindowPart(
            splitter, wxWinUITopLevelHost::WindowPartKind::SplitterSash,
            nullptr);
    }
}

// Choose the toolkit renderer once, independently of XAML runtime epochs.
void wxWinUIInstallRenderer()
{
    static bool configured = false;
    if ( configured )
        return;

    // Resolve a traits-provided renderer, or one explicitly installed before
    // initialization. An application renderer always takes precedence.
    if ( &wxRendererNative::Get() == &wxRendererNative::GetDefault() )
    {
        // Get() returned the unowned default: there is no displaced renderer
        // to delete. The global renderer holder owns the new instance, using
        // the ordinary wxRendererNative::Set() ownership contract.
        wxRendererNative::Set(new wxRendererWinUI);
    }

    // Even Set(nullptr) is an application choice after this point. Repeated
    // runtime initialization must never overwrite it or a newer renderer.
    configured = true;
}

#endif // wxUSE_WINUI3
