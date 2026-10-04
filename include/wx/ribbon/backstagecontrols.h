///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/backstagecontrols.h
// Purpose:     Controls for the pages of a wxBackstage
// Author:      Blake Madden
// Created:     2026-09-22
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_BACKSTAGECONTROLS_H_
#define _WX_RIBBON_BACKSTAGECONTROLS_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/ribbon/backstage.h"
#include "wx/control.h"
#include "wx/datetime.h"
#include "wx/panel.h"
#include "wx/scrolwin.h"

#include <functional>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_BACKSTAGE_ITEM_CLICKED, wxCommandEvent);

#define EVT_BACKSTAGE_ITEM_CLICKED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_BACKSTAGE_ITEM_CLICKED, winid, wxCommandEventHandler(fn))

enum class wxBackstageButtonStyle
{
    wxBackstageButtonTile,
    wxBackstageButtonWide,
    wxBackstageButtonCard
};

enum class wxBackstageHeadingStyle
{
    wxBackstageHeadingTitle,
    wxBackstageHeadingSection
};

class WXDLLIMPEXP_RIBBON wxBackstageButton final : public wxControl
{
public:
    wxBackstageButton(wxWindow* parent, wxWindowID id, const wxString& label,
        const wxBitmapBundle& icon = wxBitmapBundle{},
        wxBackstageButtonStyle style = wxBackstageButtonStyle::wxBackstageButtonTile,
        wxString  description = wxString{});
    wxBackstageButton() = delete;
    wxBackstageButton(const wxBackstageButton&) = delete;
    wxBackstageButton& operator=(const wxBackstageButton&) = delete;

    void SetDescription(const wxString& description);
    wxNODISCARD
    const wxString& GetDescription() const noexcept
    {
        return m_description;
    }
    void SetIcon(const wxBitmapBundle& icon);
    void SetIconSize(const wxSize& size);
    void ShowDropDownArrow(bool show = true);
    void SetCalloutColour(const wxColour& colour);

    void SetLabel(const wxString& label) override;
    bool AcceptsFocus() const override
    {
        return IsShown() && IsEnabled();
    }
protected:
    wxSize DoGetBestSize() const override;
private:
    void OnPaint(wxPaintEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnFocusChange(wxFocusEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void Activate();
    void ResetBestSize();
#if wxUSE_ACCESSIBILITY
    class Accessible;
#endif
    wxNODISCARD
    wxString GetDisplayLabel() const;

    wxBitmapBundle m_icon;
    wxSize m_iconSizeDIP;
    wxString m_description;
    wxBackstageButtonStyle m_style;
    wxColour m_calloutColour;
    bool m_dropDownArrow{ false };
    bool m_hover{ false };
    bool m_pressed{ false };
    bool m_showFocusRect{ false };
};

class WXDLLIMPEXP_RIBBON wxBackstageHeading final : public wxControl
{
public:
    wxBackstageHeading(wxWindow* parent, wxWindowID id, const wxString& label,
        wxBackstageHeadingStyle style = wxBackstageHeadingStyle::wxBackstageHeadingTitle);
    wxBackstageHeading() = delete;
    wxBackstageHeading(const wxBackstageHeading&) = delete;
    wxBackstageHeading& operator=(const wxBackstageHeading&) = delete;

    void SetLabel(const wxString& label) override;
    bool AcceptsFocus() const override
    {
        return false;
    }
protected:
    wxSize DoGetBestSize() const override;
private:
    void OnPaint(wxPaintEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    wxNODISCARD
    wxFont GetHeadingFont() const;
#if wxUSE_ACCESSIBILITY
    class Accessible;
#endif

    wxBackstageHeadingStyle m_style;
};

class WXDLLIMPEXP_RIBBON wxBackstageCallout final : public wxPanel
{
public:
    wxBackstageCallout(wxWindow* parent, wxWindowID id, const wxString& title,
                       const wxString& message = wxString{});
    wxBackstageCallout() = delete;
    wxBackstageCallout(const wxBackstageCallout&) = delete;
    wxBackstageCallout& operator=(const wxBackstageCallout&) = delete;

    void SetTile(wxBackstageButton* tile);
    void SetAction(wxBackstageButton* action);
protected:
    wxSize DoGetBestSize() const override;
private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void PositionChildren();
    void ApplyButtonColours();
    wxNODISCARD
    wxPoint GetTextOrigin() const;
    wxNODISCARD
    wxSize GetTextSize() const;
    wxNODISCARD
    wxFont GetTitleFont() const;

    wxString m_title;
    wxString m_message;
    wxWeakRef<wxBackstageButton> m_tile;
    wxWeakRef<wxBackstageButton> m_action;
};

class WXDLLIMPEXP_RIBBON wxBackstageItemList : public wxScrolledCanvas
{
public:
    explicit wxBackstageItemList(wxWindow* parent, wxWindowID id = wxID_ANY);
    wxBackstageItemList() = delete;
    wxBackstageItemList(const wxBackstageItemList&) = delete;
    wxBackstageItemList& operator=(const wxBackstageItemList&) = delete;

    void AddHeader(const wxString& text);
    size_t AddItem(const wxString& title, const wxString& subtitle = wxString{},
        const wxString& rightText = wxString{},
        const wxBitmapBundle& icon = wxBitmapBundle{},
        const wxString& userString = wxString{});
    void Clear();
    wxNODISCARD
    size_t GetItemCount() const noexcept
    {
        return m_itemCount;
    }
    wxNODISCARD
    wxString GetItemString(size_t index) const;
    bool SetFont(const wxFont& font) override;
    void SetEmptyText(const wxString& text);

protected:
    wxSize DoGetBestSize() const override;
private:
    struct Row
    {
        bool m_isHeader{ false };
        wxString m_title;
        wxString m_subtitle;
        wxString m_rightText;
        wxBitmapBundle m_icon;
        wxString m_userString;
        size_t m_itemIndex{ 0 };
    };

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnFocusChange(wxFocusEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    void UpdateVirtualSize();
    void RebuildRowLayout();
    wxNODISCARD
    size_t FirstRowAtOrBelow(wxCoord y) const;
    void Activate(size_t row);
    void MoveSelection(int direction);
    void EnsureRowVisible(size_t row);
    void NotifySelectionChanged(long row);
#if wxUSE_ACCESSIBILITY
    class Accessible;
#endif
    wxNODISCARD
    long RowAt(const wxPoint& clientPt) const;
    wxNODISCARD
    wxCoord GetRowHeight(const Row& row) const;
    wxNODISCARD
    wxCoord GetRowTop(size_t row) const;
    wxNODISCARD
    wxCoord GetIconSize() const
    {
        return FromDIP(32);
    }
    wxNODISCARD
    wxCoord GetPadding() const
    {
        return FromDIP(8);
    }

    std::vector<Row> m_rows;
    std::vector<wxCoord> m_rowTops{ 0 };
    wxCoord m_charHeight{ 0 };
    bool m_anyIcons{ false };
    wxString m_emptyText;
    size_t m_itemCount{ 0 };
    long m_hoverRow{ wxNOT_FOUND };
    long m_selectedRow{ wxNOT_FOUND };
    long m_pressedRow{ wxNOT_FOUND };
    bool m_focusFromMouse{ false };
};

class WXDLLIMPEXP_RIBBON wxBackstageMRUList final : public wxBackstageItemList
{
public:
    explicit wxBackstageMRUList(wxWindow* parent, wxWindowID id = wxID_ANY);
    wxBackstageMRUList() = delete;
    wxBackstageMRUList(const wxBackstageMRUList&) = delete;
    wxBackstageMRUList& operator=(const wxBackstageMRUList&) = delete;

    void SetFiles(const wxArrayString& paths);
    void AddFile(const wxString& path, const wxDateTime& modified);
    void ClearFiles();
    wxNODISCARD
    size_t GetFileCount() const noexcept
    {
        return m_entries.size();
    }
    void SetIconProvider(std::function<wxBitmapBundle(const wxString&)> provider);
    void ShowSectionHeaders(bool show = true);
    void SetMaxFiles(size_t maxFiles);
    static constexpr size_t UNLIMITED_MRU_LIST = static_cast<size_t>(-1);
    wxNODISCARD
    static wxString FormatModifiedDate(const wxDateTime& modified,
                                       const wxDateTime& now = wxDateTime::Now());

private:
    struct Entry
    {
        wxString m_path;
        wxDateTime m_modified;
    };

    enum class Section
    {
        Today,
        Yesterday,
        ThisWeek,
        LastWeek,
        ThisMonth,
        Older
    };

    void Rebuild();
    wxNODISCARD
    static Section GetSection(const wxDateTime& modified, const wxDateTime& now);
    wxNODISCARD
    static wxString GetSectionLabel(Section section);
    wxNODISCARD
    static wxString FormatTimeOfDay(const wxDateTime& time);
    wxNODISCARD
    static wxString SimplifyFolderPath(wxString path);

    std::vector<Entry> m_entries;
    std::function<wxBitmapBundle(const wxString&)> m_iconProvider;
    size_t m_maxFiles{ 10 };
    bool m_showSectionHeaders{ true };
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_BACKSTAGECONTROLS_H_
