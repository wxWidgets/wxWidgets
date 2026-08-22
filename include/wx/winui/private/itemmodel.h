/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/itemmodel.h
// Purpose:     Stable wx-side model for WinUI item controls
// Author:      wxWidgets development team
// Created:     2026-07-25
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_ITEMMODEL_H_
#define _WX_WINUI_PRIVATE_ITEMMODEL_H_

#include "wx/arrstr.h"
#include "wx/defs.h"
#include "wx/string.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

enum class wxWinUIComboPopupResolutionOriginForTesting : unsigned
{
    None,
    DropDownOpenedNewSession,
    DropDownOpenedExistingSession,
    AfterNativeSetter
};

enum class wxWinUIComboPopupResolutionResultForTesting : unsigned
{
    None,
    Resolved,
    TemplatePending,
    PublicationPending,
    Closing,
    Failed
};

enum class wxWinUIComboPopupFailCloseReasonForTesting : unsigned
{
    None,
    HostShutdownSealed,
    PublicationPoisoned,
    ReopenBeforeRetirement,
    BeginSessionRejected,
    ObserveOpenedException,
    StrictResolutionRejected,
    RequestCloseException
};

// Passive snapshot of the last exact-popup resolution attempt. Production
// code records only values it already had to read for the retirement gate;
// retrieving this structure never calls into XAML or changes peer state.
struct wxWinUIComboPopupResolutionDiagnosticForTesting
{
    wxWinUIComboPopupResolutionOriginForTesting origin =
        wxWinUIComboPopupResolutionOriginForTesting::None;
    wxWinUIComboPopupResolutionResultForTesting result =
        wxWinUIComboPopupResolutionResultForTesting::None;
    wxWinUIComboPopupFailCloseReasonForTesting failCloseReason =
        wxWinUIComboPopupFailCloseReasonForTesting::None;
    unsigned failCloseCount = 0;
    bool propertyOpen = false;
    bool rootAvailable = false;
    bool queueAvailable = false;
    bool sessionActive = false;
    bool completionArmed = false;
    bool rootMatches = false;
    bool controlAvailable = false;
    bool popupAvailable = false;
    bool childAvailable = false;
    bool hooksArmed = false;
    bool popupIsOpen = false;
    bool exactOpenList = false;
    bool duplicateOpenList = false;
};

WXDLLIMPEXP_CORE void
wxWinUIResetComboPopupResolutionDiagnosticForTesting() noexcept;
WXDLLIMPEXP_CORE wxWinUIComboPopupResolutionDiagnosticForTesting
wxWinUIGetComboPopupResolutionDiagnosticForTesting() noexcept;

// This model deliberately does not own clientData: wxItemContainer remains
// its sole owner and destroys wxClientData objects before asking the backend
// to erase an item. Stable IDs let XAML delegates refer to an item without
// retaining either its mutable index or the wx control itself.
class wxWinUIItemModel final
{
public:
    using Id = std::uint64_t;

    static constexpr std::size_t npos =
        (std::numeric_limits<std::size_t>::max)();

    struct Item
    {
        Id id = 0;
        wxString text;
        void *clientData = nullptr;
        bool selected = false;
        bool checked = false;
    };

    struct Change
    {
        enum class Kind
        {
            Insert,
            Erase,
            Update,
            Move
        };

        Kind kind;
        Id id;
        std::size_t oldIndex;
        std::size_t newIndex;
    };

    std::size_t GetCount() const { return m_items.size(); }
    bool IsEmpty() const { return m_items.empty(); }

    Item& At(std::size_t index)
    {
        wxASSERT_MSG(index < m_items.size(), "invalid WinUI item index");
        return m_items[index];
    }

    const Item& At(std::size_t index) const
    {
        wxASSERT_MSG(index < m_items.size(), "invalid WinUI item index");
        return m_items[index];
    }

    std::size_t IndexOf(Id id) const
    {
        const auto it = std::find_if(
            m_items.begin(), m_items.end(),
            [id](const Item& item) { return item.id == id; });
        return it == m_items.end()
                   ? npos
                   : static_cast<std::size_t>(it - m_items.begin());
    }

    Change Insert(const wxString& text,
                  std::size_t requestedIndex,
                  bool sorted)
    {
        wxASSERT_MSG(requestedIndex <= m_items.size(),
                     "invalid WinUI item insertion index");

        const std::size_t index =
            sorted ? FindSortedInsertionIndex(text) : requestedIndex;

        wxASSERT_MSG(m_nextId != 0,
                     "WinUI item identity space exhausted");
        const Id id = m_nextId++;
        Item item;
        item.id = id;
        item.text = text;
        m_items.insert(m_items.begin() + index, std::move(item));
        return {Change::Kind::Insert, id, npos, index};
    }

    Change Rename(std::size_t index,
                  const wxString& text,
                  bool sorted)
    {
        wxASSERT_MSG(index < m_items.size(), "invalid WinUI item index");

        if ( !sorted || m_items[index].text == text )
        {
            m_items[index].text = text;
            return {Change::Kind::Update, m_items[index].id, index, index};
        }

        Item item = std::move(m_items[index]);
        item.text = text;
        const Id id = item.id;
        m_items.erase(m_items.begin() + index);

        const std::size_t newIndex = FindSortedInsertionIndex(text);
        m_items.insert(m_items.begin() + newIndex, std::move(item));

        return {
            newIndex == index ? Change::Kind::Update : Change::Kind::Move,
            id,
            index,
            newIndex
        };
    }

    Change Erase(std::size_t index)
    {
        wxASSERT_MSG(index < m_items.size(), "invalid WinUI item index");
        const Id id = m_items[index].id;
        m_items.erase(m_items.begin() + index);
        return {Change::Kind::Erase, id, index, npos};
    }

    void Clear()
    {
        // Do not reset m_nextId: a queued delegate holding an ID from before
        // Clear() must never accidentally resolve to a newly inserted item.
        m_items.clear();
    }

private:
    std::size_t FindSortedInsertionIndex(const wxString& text) const
    {
        const auto it = std::upper_bound(
            m_items.begin(), m_items.end(), text,
            [](const wxString& value, const Item& item)
            {
                return wxDictionaryStringSortAscending(value, item.text) < 0;
            });
        return static_cast<std::size_t>(it - m_items.begin());
    }

    std::vector<Item> m_items;
    Id m_nextId = 1;
};

#endif // _WX_WINUI_PRIVATE_ITEMMODEL_H_
