///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fuzz/winuiitemmodel.cpp
// Purpose:     wxWinUIItemModel state transition fuzzing test
// Author:      wxWidgets development team
// Created:     2026-09-01
// Copyright:   (c) 2026 wxWidgets development team
///////////////////////////////////////////////////////////////////////////////

#include "wx/log.h"
#include "wx/winui/private/itemmodel.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>

#if wxDEBUG_LEVEL

static void abortAssertHandler(const wxString& file,
                               int line,
                               const wxString& func,
                               const wxString& cond,
                               const wxString& msg);

static volatile wxAssertHandler_t
    origAssertHandler = wxSetAssertHandler(abortAssertHandler);

static void abortAssertHandler(const wxString& file,
                               int line,
                               const wxString& func,
                               const wxString& cond,
                               const wxString& msg)
{
    origAssertHandler(file, line, func, cond, msg);

    std::abort();
}

#endif // wxDEBUG_LEVEL

namespace
{

using ItemId = wxWinUIItemModel::Id;
using ChangeKind = wxWinUIItemModel::Change::Kind;

constexpr std::size_t MaxLiveItems = 64;

struct ExpectedItem
{
    ItemId id;
    wxString text;
    void *clientData;
    bool selected;
    bool checked;
};

[[noreturn]] void Fail()
{
    std::abort();
}

void Require(bool condition)
{
    if ( !condition )
        Fail();
}

wxString MakeText(unsigned int selector)
{
    // Keep the text domain small enough to produce duplicates frequently, but
    // include empty, case-sensitive and dictionary-order-sensitive values.
    switch ( selector % 16 )
    {
        case 0:  return wxString();
        case 1:  return "alpha";
        case 2:  return "Alpha";
        case 3:  return "ALPHA";
        case 4:  return wxString();
        case 5:  return "a";
        case 6:  return "A";
        case 7:  return "aa";
        case 8:  return "Aaa";
        case 9:  return "aaa";
        case 10: return "two";
        case 11: return "10";
        case 12: return "2";
        case 13: return "Aaa";
        case 14: return "z";
        case 15: return "Z";
    }

    Fail();
}

std::size_t FindSortedInsertionIndex(
    const std::vector<ExpectedItem>& items,
    const wxString& text)
{
    std::size_t index = 0;
    while ( index != items.size() &&
            wxDictionaryStringSortAscending(text, items[index].text) >= 0 )
    {
        ++index;
    }

    return index;
}

std::size_t ExpectedIndexOf(const std::vector<ExpectedItem>& items,
                            ItemId id)
{
    const auto it = std::find_if(
        items.begin(), items.end(),
        [id](const ExpectedItem& item) { return item.id == id; });
    return it == items.end()
               ? wxWinUIItemModel::npos
               : static_cast<std::size_t>(it - items.begin());
}

void CheckModel(const wxWinUIItemModel& model,
                const std::vector<ExpectedItem>& expected,
                const std::vector<ItemId>& retiredIds,
                ItemId lastId,
                bool sorted,
                std::size_t probeSelector)
{
    Require(model.GetCount() == expected.size());
    Require(model.IsEmpty() == expected.empty());
    Require(model.IndexOf(0) == wxWinUIItemModel::npos);

    for ( std::size_t index = 0; index != expected.size(); ++index )
    {
        const wxWinUIItemModel::Item& actual = model.At(index);
        const ExpectedItem& wanted = expected[index];

        Require(actual.id == wanted.id);
        Require(actual.text == wanted.text);
        Require(actual.clientData == wanted.clientData);
        Require(actual.selected == wanted.selected);
        Require(actual.checked == wanted.checked);
        Require(model.IndexOf(actual.id) == index);

        for ( std::size_t previous = 0; previous != index; ++previous )
            Require(expected[previous].id != actual.id);

        if ( sorted && index != 0 )
        {
            Require(wxDictionaryStringSortAscending(
                        expected[index - 1].text, wanted.text) <= 0);
        }
    }

    if ( !retiredIds.empty() )
    {
        const ItemId retired = retiredIds[probeSelector % retiredIds.size()];
        Require(model.IndexOf(retired) == wxWinUIItemModel::npos);
    }

    if ( lastId != (std::numeric_limits<ItemId>::max)() )
        Require(model.IndexOf(lastId + 1) == wxWinUIItemModel::npos);
}

void InsertItem(wxWinUIItemModel& model,
                std::vector<ExpectedItem>& expected,
                std::vector<ItemId>& allIds,
                ItemId& lastId,
                const wxString& text,
                std::size_t requestedIndex,
                bool sorted)
{
    const std::size_t insertedIndex =
        sorted ? FindSortedInsertionIndex(expected, text) : requestedIndex;
    const wxWinUIItemModel::Change change =
        model.Insert(text, requestedIndex, sorted);

    Require(change.kind == ChangeKind::Insert);
    Require(change.oldIndex == wxWinUIItemModel::npos);
    Require(change.newIndex == insertedIndex);
    Require(change.id != 0);
    Require(change.id > lastId);
    Require(std::find(allIds.begin(), allIds.end(), change.id) == allIds.end());

    expected.insert(expected.begin() + insertedIndex,
                    {change.id, text, nullptr, false, false});
    allIds.push_back(change.id);
    lastId = change.id;
}

void RenameItem(wxWinUIItemModel& model,
                std::vector<ExpectedItem>& expected,
                std::size_t index,
                const wxString& text,
                bool sorted)
{
    const ExpectedItem oldItem = expected[index];
    std::size_t newIndex = index;
    ChangeKind kind = ChangeKind::Update;

    if ( !sorted || oldItem.text == text )
    {
        expected[index].text = text;
    }
    else
    {
        ExpectedItem renamed = oldItem;
        renamed.text = text;
        expected.erase(expected.begin() + index);
        newIndex = FindSortedInsertionIndex(expected, text);
        expected.insert(expected.begin() + newIndex, renamed);
        if ( newIndex != index )
            kind = ChangeKind::Move;
    }

    const wxWinUIItemModel::Change change = model.Rename(index, text, sorted);
    Require(change.kind == kind);
    Require(change.id == oldItem.id);
    Require(change.oldIndex == index);
    Require(change.newIndex == newIndex);
}

} // anonymous namespace

extern "C" int LLVMFuzzerTestOneInput(const wxUint8 *data, size_t size)
{
    wxLogNull noLog;

    if ( size == 0 )
        return 0;

    const bool sorted = (data[0] & 1) != 0;
    wxWinUIItemModel model;
    std::vector<ExpectedItem> expected;
    std::vector<ItemId> allIds;
    std::vector<ItemId> retiredIds;
    std::array<unsigned char, 32> clientTokens{};
    ItemId lastId = 0;

    CheckModel(model, expected, retiredIds, lastId, sorted, 0);

    for ( std::size_t position = 1; position != size; ++position )
    {
        const unsigned int command = data[position];
        const unsigned int value = command >> 3;
        const unsigned int operation = command & 7;
        const wxString text = MakeText(value);

        if ( (operation == 0 || operation == 1) &&
             expected.size() < MaxLiveItems )
        {
            const std::size_t requestedIndex =
                (value + position) % (expected.size() + 1);
            InsertItem(model, expected, allIds, lastId,
                       text, requestedIndex, sorted);
        }
        else if ( (operation == 2 || operation == 7) && !expected.empty() )
        {
            const std::size_t index =
                (value + position) % expected.size();
            RenameItem(model, expected, index, text, sorted);
        }
        else if ( operation == 3 && !expected.empty() )
        {
            const std::size_t index =
                (value + position) % expected.size();
            const ItemId erasedId = expected[index].id;
            const wxWinUIItemModel::Change change = model.Erase(index);

            Require(change.kind == ChangeKind::Erase);
            Require(change.id == erasedId);
            Require(change.oldIndex == index);
            Require(change.newIndex == wxWinUIItemModel::npos);

            expected.erase(expected.begin() + index);
            retiredIds.push_back(erasedId);
            Require(model.IndexOf(erasedId) == wxWinUIItemModel::npos);
        }
        else if ( operation == 4 && !expected.empty() )
        {
            const std::size_t index =
                (value + position) % expected.size();
            void * const clientData = (command & 0x40)
                                         ? &clientTokens[value % clientTokens.size()]
                                         : nullptr;
            const bool selected = (command & 0x10) != 0;
            const bool checked = (command & 0x20) != 0;

            wxWinUIItemModel::Item& actual = model.At(index);
            actual.clientData = clientData;
            actual.selected = selected;
            actual.checked = checked;

            expected[index].clientData = clientData;
            expected[index].selected = selected;
            expected[index].checked = checked;
        }
        else if ( operation == 5 )
        {
            ItemId id = lastId + 1;
            if ( value % 3 == 0 && !expected.empty() )
                id = expected[(value + position) % expected.size()].id;
            else if ( value % 3 == 1 && !retiredIds.empty() )
                id = retiredIds[(value + position) % retiredIds.size()];

            Require(model.IndexOf(id) == ExpectedIndexOf(expected, id));
        }
        else if ( operation == 6 )
        {
            for ( const ExpectedItem& item : expected )
                retiredIds.push_back(item.id);

            model.Clear();
            expected.clear();

            for ( const ItemId id : retiredIds )
                Require(model.IndexOf(id) == wxWinUIItemModel::npos);
        }
        else if ( expected.empty() )
        {
            InsertItem(model, expected, allIds, lastId,
                       text, 0, sorted);
        }

        CheckModel(model, expected, retiredIds, lastId,
                   sorted, command + position);
    }

    for ( const ItemId id : retiredIds )
        Require(model.IndexOf(id) == wxWinUIItemModel::npos);

    return 0;
}
