///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/backstage.h
// Purpose:     Backstage view (as used by a ribbon's "File" tab)
// Author:      Blake Madden
// Created:     2026-09-20
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_BACKSTAGE_H_
#define _WX_RIBBON_BACKSTAGE_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/artprov.h"
#include "wx/bmpbndl.h"
#include "wx/colour.h"
#include "wx/dc.h"
#include "wx/dcbuffer.h"
#include "wx/dcgraph.h"
#include "wx/scrolwin.h"
#include "wx/settings.h"
#include "wx/simplebook.h"
#include "wx/weakref.h"
#include "wx/window.h"

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_BACKSTAGE_CLICKED, wxNotifyEvent);

#define EVT_BACKSTAGE_CLICKED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_BACKSTAGE_CLICKED, winid, wxNotifyEventHandler(fn))

enum class wxBackstageHighlightStyle
{
    wxBackstageHighlightFlat,
    wxBackstageHighlightGlossy
};

class WXDLLIMPEXP_RIBBON wxBackstagePage final : public wxScrolledWindow
{
public:
    explicit wxBackstagePage(wxWindow* parent);
    wxBackstagePage() = delete;
    wxBackstagePage(const wxBackstagePage&) = delete;
    wxBackstagePage& operator=(const wxBackstagePage&) = delete;
};

class WXDLLIMPEXP_RIBBON wxBackstage final : public wxWindow
{
public:
    explicit wxBackstage(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~wxBackstage() override;
    wxBackstage() = delete;
    wxBackstage(const wxBackstage&) = delete;
    wxBackstage& operator=(const wxBackstage&) = delete;


    bool AddButton(wxWindowID id, const wxString& label,
                   const wxBitmapBundle& icon = wxBitmapBundle{});
    void AddSeparator();
    void AddFlexibleSpace();
    void EnableButton(wxWindowID id, bool enable = true);
    wxNODISCARD
    bool IsButtonEnabled(wxWindowID id) const;
    void SetButtonLabel(wxWindowID id, const wxString& label);


    wxBackstagePage* AddPage(wxWindowID buttonId);
    bool SetPage(wxWindowID buttonId, wxWindow* page);
    bool RemovePage(wxWindowID buttonId);
    wxNODISCARD
    wxWindow* GetPage(wxWindowID buttonId) const;
    bool ShowPage(wxWindowID buttonId);
    wxNODISCARD
    wxWindowID GetCurrentPageId() const noexcept
    {
        return m_selectedId;
    }


    wxNODISCARD
    wxColour GetNavBackgroundColour() const
    {
        if ( m_navBackgroundColour.IsOk() )
        {
            return m_navBackgroundColour;
        }
        return wxSystemSettings::GetAppearance().IsDark() ?
            wxColour{ 46, 46, 46 } : wxColour{ 43, 87, 154 };
    }
    void SetNavBackgroundColour(const wxColour& colour);
    wxNODISCARD
    const wxColour& GetHighlightColour() const noexcept
    {
        return m_highlightColour;
    }
    void SetHighlightColour(const wxColour& colour);
    wxNODISCARD
    wxBackstageHighlightStyle GetHighlightStyle() const noexcept
    {
        return m_highlightStyle;
    }
    void SetHighlightStyle(wxBackstageHighlightStyle style);
    wxNODISCARD
    wxColour GetPageBackgroundColour() const
    {
        if ( m_pageBackgroundColour.IsOk() )
        {
            return m_pageBackgroundColour;
        }
        return wxSystemSettings::GetAppearance().IsDark() ?
            wxColour{ 31, 31, 31 } : wxColour{ 255, 255, 255 };
    }
    void SetPageBackgroundColour(const wxColour& colour);
    wxNODISCARD
    wxColour GetPageForegroundColour() const
    {
        return BlackOrWhiteContrast(GetPageBackgroundColour());
    }
    void KeepWindowColours(wxWindow* window);


    wxNODISCARD
    static bool IsDark(const wxColour& colour)
    {
        wxASSERT_MSG(colour.IsOk(), "Invalid colour passed to IsDark()!");
        return (colour.IsOk() &&
            colour.Alpha() > 32 &&
            colour.GetLuminance() < 0.5);
    }
    wxNODISCARD
    static wxColour ShadeOrTint(const wxColour& colour,
        const double shadeOrTintValue = 0.2)
    {
        return (IsDark(colour) ?
            colour.ChangeLightness(100 + static_cast<int>(shadeOrTintValue * 100)) :
            colour.ChangeLightness(100 - static_cast<int>(shadeOrTintValue * 100)));
    }
    wxNODISCARD
    static wxColour BlackOrWhiteContrast(const wxColour& colour)
    {
        return (IsDark(colour) ? wxColour{ 255, 255, 255 } : wxColour{ 0, 0, 0 });
    }
    wxNODISCARD
    static wxColour Blend(const wxColour& from, const wxColour& to, const double amount)
    {
        const auto mix = [amount](const unsigned char a, const unsigned char b)
            {
                return static_cast<unsigned char>(
                    wxRound(a + (static_cast<double>(b) - a) * amount));
            };
        return wxColour{ mix(from.Red(), to.Red()),
                         mix(from.Green(), to.Green()),
                         mix(from.Blue(), to.Blue()) };
    }
    wxNODISCARD
    static const wxBackstage* FindBackstage(const wxWindow* window);
    static void GetPageColours(const wxWindow* window, wxColour& background,
                               wxColour& foreground);


    static void DrawBitmapFit(wxDC& dc, const wxWindow* window,
                              const wxBitmapBundle& bundle, const wxRect& rect);
    static void DrawGlossyRect(wxDC& dc, const wxRect& rect, const wxColour& colour);
    static void DrawFocusRect(wxDC& dc, const wxRect& rect, const wxColour& colour);
    wxNODISCARD
    static wxSize MeasureText(const wxWindow* window, const wxString& text,
                              const wxFont& font);

    static bool SkipIfShortcutKey(wxKeyEvent& event)
    {
        if ( event.HasModifiers() )
        {
            event.Skip();
            return true;
        }
        return false;
    }
    wxNODISCARD
    static bool IsActivateKey(const int keyCode) noexcept
    {
        return keyCode == WXK_SPACE || keyCode == WXK_RETURN ||
            keyCode == WXK_NUMPAD_ENTER;
    }
private:
    enum class ItemKind
    {
        Button,
        Separator,
        FlexibleSpace
    };

    struct Item
    {
        ItemKind m_kind{ ItemKind::Button };
        wxWindowID m_id{ wxNOT_FOUND };
        wxString m_label;
        wxBitmapBundle m_icon;
        wxWeakRef<wxWindow> m_page;
        bool m_enabled{ true };
        wxRect m_rect;
    };

    struct NavMetrics
    {
        wxCoord m_padX;
        wxCoord m_padY;
        wxCoord m_iconGap;
        wxCoord m_border;
        wxCoord m_separatorHeight;
        wxSize m_iconSize;
        bool m_anyIcons;
    };

    void OnPageDestroyed(wxWindowDestroyEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnPaintWindow(wxPaintEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnNavigationKey(wxNavigationKeyEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnDPIChanged(wxDPIChangedEvent& event);

    void ActivateItem(size_t index);
    void CalcLayout();
    wxNODISCARD
    NavMetrics GetNavMetrics() const;
    void UpdatePageColours();
    void MoveFocus(int direction);
    wxNODISCARD
    bool IsSelectable(const size_t index) const noexcept
    {
        return index < m_items.size() &&
            m_items[index].m_kind == ItemKind::Button &&
            m_items[index].m_enabled;
    }
    wxNODISCARD
    long FindItem(wxWindowID id) const noexcept;
    wxNODISCARD
    long HitTest(const wxPoint& pt) const noexcept;
    void RefreshItem(long index);
    void ApplyPageColours(wxWindow* page) const;
    wxNODISCARD
    bool AreColoursKept(const wxWindow* window) const;
    wxNODISCARD
    int GetChildId(size_t index) const noexcept;
    wxNODISCARD
    long GetItemFromChildId(int childId) const noexcept;
    enum class AccessibleEvent
    {
        Focus,
        Selection,
        NameChange,
        StateChange
    };
    void NotifyAccessibility(AccessibleEvent event, long index);

#if wxUSE_ACCESSIBILITY
    class Accessible;
#endif

    wxNODISCARD
    wxFont GetNavFont() const
    {
        return GetFont().Larger();
    }
    wxNODISCARD
    wxSize GetIconSize() const
    {
        return FromDIP(wxSize{ 20, 20 });
    }

    std::vector<Item> m_items;
    std::vector<wxWeakRef<wxWindow>> m_keptColourWindows;
    wxSimplebook* m_book{ nullptr };
    wxCoord m_navWidth{ 0 };
    int m_scrollOffset{ 0 };
    int m_wheelRotation{ 0 };
    int m_contentHeight{ 0 };
    long m_hoverIndex{ wxNOT_FOUND };
    long m_pressedIndex{ wxNOT_FOUND };
    long m_focusIndex{ wxNOT_FOUND };
    bool m_showFocusRect{ false };
    wxWindowID m_selectedId{ wxNOT_FOUND };
    wxColour m_navBackgroundColour;
    wxColour m_highlightColour;
    wxBackstageHighlightStyle m_highlightStyle{ wxBackstageHighlightStyle::wxBackstageHighlightFlat };
    wxColour m_pageBackgroundColour;
    wxColour m_appliedPageBackground;
    wxColour m_appliedPageForeground;
};

struct WXDLLIMPEXP_RIBBON wxBackstagePaintBuffer
{
    explicit wxBackstagePaintBuffer(wxWindow* win);

    wxAutoBufferedPaintDC m_buffer;
    wxColour m_background;
    wxColour m_foreground;
};

struct WXDLLIMPEXP_RIBBON wxBackstagePaintGraphics : public wxBackstagePaintBuffer
{
    wxBackstagePaintGraphics(wxWindow* win, const wxFont& font);

    wxGCDC m_graphics;
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_BACKSTAGE_H_
