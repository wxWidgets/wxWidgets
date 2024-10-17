/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/menuradio.h
// Purpose:     wxMenuRadioItemsData implementation
// Author:      Vadim Zeitlin
// Modified:    Artur Wieczorek: added UpdateOnInsertNonRadio()
// Created:     2017-08-12
// Copyright:   (c) 2011 Vadim Zeitlin et al
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_PRIVATE_MENURADIO_H_
#define WX_PRIVATE_MENURADIO_H_

#include "wx/vector.h"

// Contains the data about the radio items groups in the given menu.
class wxMenuRadioItemsData
{
public:
    wxMenuRadioItemsData() = default;

    // Default copy ctor, assignment operator and dtor are all ok.

    // Find the start and end of the group containing the given position or
    // return false if it's not inside any range.
    bool GetGroupRange(int pos, int *start, int *end) const
    {
        // We use a simple linear search here because there are not that many
        // items in a menu and hence even fewer radio items ranges anyhow, so
        // normally there is no need to do anything fancy (like keeping the
        // array sorted and using binary search).
        for ( Ranges::const_iterator it = m_ranges.begin();
            it != m_ranges.end();
            ++it )
        {
            const Range& r = *it;

            if ( r.start <= pos && pos <= r.end )
            {
                if ( start )
                    *start = r.start;
                if ( end )
                    *end = r.end;

                return true;
            }
        }

        return false;
    }

    // Take into account the new radio item about to be added at the given
    // position. The are two cases to handle:
    // - If item precedes the range, the range indices have to be updated.
    // - If item falls inside the range, this range is extended to include
    //   the item.
    // Returns true if this item starts a new radio group, false if it extends
    // an existing one.
    bool UpdateOnInsertRadio(int pos)
    {
        bool inExistingGroup = false;

        for ( Ranges::iterator it = m_ranges.begin();
            it != m_ranges.end();
            ++it )
        {
            Range& r = *it;

            if ( pos < r.start )
            {
                // Item is inserted before this range, update its indices.
                r.start++;
                r.end++;
            }
            else if ( pos <= r.end + 1 )
            {
                wxASSERT_MSG(!inExistingGroup,
                    wxS("Item already inserted inside another range"));
                // Item is inserted in the middle of this range or immediately
                // after it in which case it extends this range so make it span
                // one more item in any case.
                r.end++;

                inExistingGroup = true;
            }
            //else: Item is inserted after this range, nothing to do for it.
        }

        if ( inExistingGroup )
            return false;

        // Make a new range for the group this item will belong to.
        Range r;
        r.start = pos;
        r.end = pos;
        m_ranges.push_back(r);

        return true;
    }

    // Take into account the new non-radio item about to be added at the given
    // position. The are two cases to handle:
    // - If item precedes the range, the range indices have to be updated.
    // - If item falls inside the range, this range has to be split into
    //    two new ranges.
    // Returns true if existing group has been split into two subgroups.
    bool UpdateOnInsertNonRadio(int pos)
    {
        bool wasSplit = false;
        Range newRange;

        for ( Ranges::iterator it = m_ranges.begin();
            it != m_ranges.end();
            ++it )
        {
            Range& r = *it;

            if ( pos <= r.start )
            {
                // Item is inserted before this range or just at its start,
                // update its indices.
                r.start++;
                r.end++;
            }
            else if ( pos <= r.end )
            {
                wxASSERT_MSG(!wasSplit,
                    wxS("Item already inserted inside another range"));
                // Item is inserted inside this range in which case
                // it breaks the range into two parts: one ending before
                // the item and one started after it.

                // The new range after the item has to be stored and added to the list
                // after finishing the iteration through the ranges.
                newRange.start = pos + 1; // start after the item
                newRange.end = r.end + 1; // inherits current end "moved up" by one item
                wasSplit = true;
                // Current range ends just before the item position.
                r.end = pos - 1;
            }
            //else: Item is inserted after this range, nothing to do for it.
        }

        if ( !wasSplit )
            return false;

        // Add a split range to the list.
        m_ranges.push_back(newRange);

        return true;
    }

    // Update the ranges of the existing radio groups after removing the menu
    // item at the given position.
    //
    // The item being removed can be the item of any kind, not only the radio
    // button belonging to the radio group, and this function checks for it
    // and, as a side effect, returns true if this item was found inside an
    // existing radio group.
    bool UpdateOnRemoveItem(int pos)
    {
        bool inExistingGroup = false;

        // Pointer to (necessarily unique) empty group which could be left
        // after removing the last radio button from it.
        Ranges::iterator itEmptyGroup = m_ranges.end();

        for ( Ranges::iterator it = m_ranges.begin();
            it != m_ranges.end();
            ++it )
        {
            Range& r = *it;

            if ( pos < r.start )
            {
                // Removed item was positioned before this range, update its
                // indices.
                r.start--;
                r.end--;
            }
            else if ( pos <= r.end )
            {
                // Removed item belongs to this radio group (it is a radio
                // button), update index of its end.
                r.end--;

                // Check if empty group left after removal.
                // If so, it will be deleted later on.
                if ( r.end < r.start )
                    itEmptyGroup = it;

                inExistingGroup = true;
            }
            //else: Removed item was after this range, nothing to do for it.
        }

        // Remove empty group from the list.
        if ( itEmptyGroup != m_ranges.end() )
            m_ranges.erase(itEmptyGroup);

        return inExistingGroup;
    }

private:
    // Contains the inclusive positions of the range start and end.
    struct Range
    {
        int start;
        int end;
    };

    typedef wxVector<Range> Ranges;
    Ranges m_ranges;
};

#endif // WX_PRIVATE_MENURADIO_H_
