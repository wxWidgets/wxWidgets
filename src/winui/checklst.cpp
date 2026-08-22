/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/checklst.cpp
// Purpose:     wxWinUI wxCheckListBox implementation
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"
#include "wx/renderer.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/dc.h"
#endif

#include "private.h"

#if wxUSE_OWNER_DRAWN
    #include "wx/msw/private/listboxitem.h"
#endif

namespace
{

#if wxUSE_OWNER_DRAWN

class wxWinUICheckListBoxItem final
    : public wxListBoxItemBase<wxCheckListBox>
{
public:
    explicit wxWinUICheckListBoxItem(wxCheckListBox *parent)
        : wxListBoxItemBase<wxCheckListBox>(parent)
    {
        UpdateMetrics();
    }

    bool OnMeasureItem(size_t *width, size_t *height) override
    {
        // Renderer metrics and FromDIP() both change when the list moves to
        // another monitor. Keep the text margin in the same pixel space as
        // the checkbox drawn below.
        UpdateMetrics();
        return wxListBoxItemBase<wxCheckListBox>::OnMeasureItem(
            width, height);
    }

    bool OnDrawItem(wxDC& dc,
                    const wxRect& rect,
                    wxODAction action,
                    wxODStatus status) override
    {
        UpdateMetrics();
        if ( !wxOwnerDrawn::OnDrawItem(
                 dc, rect, action, status) )
        {
            return false;
        }

        wxCheckListBox * const parent = GetParent();
        const int index = GetIndex();
        const wxSize check =
            wxRendererNative::Get().GetCheckBoxSize(parent);
        const int gap = parent->FromDIP(1);
        const bool rightToLeft =
            dc.GetLayoutDirection() == wxLayout_RightToLeft;

        // The checkbox moves to the logical leading edge in RTL, but the
        // glyph itself must retain its normal orientation, as MSW does with
        // LAYOUT_BITMAPORIENTATIONPRESERVED.
        if ( rightToLeft )
            dc.SetLayoutDirection(wxLayout_LeftToRight);
        const int x = rightToLeft
            ? rect.GetRight() - gap - check.x + 1
            : rect.x + gap;
        const int y = rect.y + (rect.height - check.y) / 2;

        int flags = wxCONTROL_FLAT;
        if ( index >= 0 && parent->IsChecked(index) )
            flags |= wxCONTROL_CHECKED;
        if ( status & wxODDisabled )
            flags |= wxCONTROL_DISABLED;
        wxRendererNative::Get().DrawCheckBox(
            parent, dc, wxRect(wxPoint(x, y), check), flags);
        if ( rightToLeft )
            dc.SetLayoutDirection(wxLayout_RightToLeft);
        return true;
    }

private:
    int MSWGetTextType() const override
    {
        // Checklist labels don't interpret ampersands as mnemonics on MSW.
        return DST_TEXT;
    }

    void UpdateMetrics()
    {
        wxCheckListBox * const parent = GetParent();
        const wxSize check =
            wxRendererNative::Get().GetCheckBoxSize(parent);
        SetMarginWidth(check.x + parent->FromDIP(4));
    }
};

#endif // wxUSE_OWNER_DRAWN

} // anonymous namespace

#if wxUSE_OWNER_DRAWN

wxOwnerDrawn *wxCheckListBox::CreateLboxItem(size_t WXUNUSED(n))
{
    return new wxWinUICheckListBoxItem(this);
}

#endif // wxUSE_OWNER_DRAWN

bool wxCheckListBox::IsChecked(unsigned int item) const
{
    wxCHECK_MSG( item < m_itemModel.GetCount(), false,
                 wxT("invalid checklistbox index") );
    return m_itemModel.At(item).checked;
}

void wxCheckListBox::Check(unsigned int item, bool check)
{
    wxCHECK_RET( item < m_itemModel.GetCount(),
                 wxT("invalid checklistbox index") );

    const std::uint64_t itemId = m_itemModel.At(item).id;
    m_itemModel.At(item).checked = check;

    // A programmatic change updates exactly one stable peer and is silent.
    // WinUIUpdatePeerItem() holds the peer-mutation guard while assigning the
    // CheckBox property, suppressing its Checked/Unchecked callback.
    // This is terminal because peer recovery or owner drawing may delete the
    // control. The helper validates the stable ID around both operations.
    WinUIUpdatePeerItemById(itemId);
}

void wxCheckListBox::Toggle(unsigned int item)
{
    wxCHECK_RET( item < m_itemModel.GetCount(),
                 wxT("invalid checklistbox index") );
    Check(item, !m_itemModel.At(item).checked);
}

void wxCheckListBox::WinUIOnItemToggled(wxWinUIItemModel::Id id,
                                        bool check)
{
    const std::size_t index = m_itemModel.IndexOf(id);
    if ( index == wxWinUIItemModel::npos )
        return;

    m_itemModel.At(index).checked = check;

    // The visible checkbox is part of the retained owner-draw bitmap. Redraw
    // it before publishing the event, while the transparent native CheckBox
    // continues to provide pointer, keyboard and UIA Toggle semantics.
    if ( !WinUIUpdatePeerItemById(id) )
        return;

    const std::size_t currentIndex = m_itemModel.IndexOf(id);
    if ( currentIndex == wxWinUIItemModel::npos )
        return;

    // wxListBoxBase::SendEvent supplies string and whichever client-data
    // flavour wxItemContainer owns. It may destroy this control.
    SendEvent(
        wxEVT_CHECKLISTBOX,
        static_cast<int>(currentIndex),
        check);
}

#endif // wxUSE_CHECKLISTBOX
