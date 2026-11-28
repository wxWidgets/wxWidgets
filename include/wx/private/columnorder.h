///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/columnorder.h
// Purpose:     Implementation-only helpers for column order mutations
// Author:      wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_COLUMNORDER_H_
#define _WX_PRIVATE_COLUMNORDER_H_

#include "wx/dynarray.h"

#include <limits>

namespace wxPrivate
{

// Mutate arrays mapping display positions to logical column identifiers.
//
// An empty input is accepted as the natural order. Every successful mutation
// publishes an explicit permutation, except for the only possible order of
// zero columns, which remains empty. All operations provide the strong
// guarantee: an invalid request leaves the caller's array unchanged.
struct ColumnOrderMutation final
{
    static bool IsValid(const wxArrayInt& order, unsigned int count)
    {
        if ( count > MaximumCount() )
            return false;

        if ( order.empty() )
            return true;

        if ( order.size() != count )
            return false;

        wxArrayInt seen(count, 0);
        for ( unsigned int pos = 0; pos < count; ++pos )
        {
            const int value = order[pos];
            if ( value < 0 )
                return false;

            const unsigned int id = static_cast<unsigned int>(value);
            if ( id >= count || seen[id] )
                return false;

            seen[id] = 1;
        }

        return true;
    }

    static bool Move(wxArrayInt& order,
                     unsigned int count,
                     unsigned int id,
                     unsigned int newDisplayPos)
    {
        wxArrayInt updated;
        if ( !MakeExplicit(order, count, updated) ||
                id >= count ||
                newDisplayPos >= count )
        {
            return false;
        }

        const int oldDisplayPos =
            updated.Index(static_cast<int>(id));
        if ( oldDisplayPos == wxNOT_FOUND )
            return false;

        if ( static_cast<unsigned int>(oldDisplayPos) != newDisplayPos )
        {
            updated.RemoveAt(oldDisplayPos);
            updated.Insert(id, newDisplayPos);
        }

        order.swap(updated);
        return true;
    }

    static bool Insert(wxArrayInt& order,
                       unsigned int oldCount,
                       unsigned int firstId,
                       unsigned int count,
                       unsigned int displayPos)
    {
        if ( oldCount > MaximumCount() ||
                count > MaximumCount() - oldCount ||
                firstId > oldCount ||
                displayPos > oldCount ||
                firstId > MaximumCount() )
        {
            return false;
        }

        wxArrayInt updated;
        if ( !MakeExplicit(order, oldCount, updated) )
            return false;

        if ( count )
        {
            for ( unsigned int pos = 0; pos < oldCount; ++pos )
            {
                if ( static_cast<unsigned int>(updated[pos]) >= firstId )
                    updated[pos] += count;
            }

            for ( unsigned int offset = 0; offset < count; ++offset )
            {
                updated.Insert(
                    static_cast<int>(firstId + offset),
                    displayPos + offset);
            }
        }

        order.swap(updated);
        return true;
    }

    static bool Erase(wxArrayInt& order,
                      unsigned int oldCount,
                      unsigned int firstId,
                      unsigned int count)
    {
        if ( oldCount > MaximumCount() ||
                count > oldCount ||
                firstId > oldCount - count )
            return false;

        wxArrayInt current;
        if ( !MakeExplicit(order, oldCount, current) )
            return false;

        wxArrayInt updated;
        updated.reserve(oldCount - count);

        const unsigned int endId = firstId + count;
        for ( unsigned int pos = 0; pos < oldCount; ++pos )
        {
            const unsigned int id =
                static_cast<unsigned int>(current[pos]);
            if ( id >= firstId && id < endId )
                continue;

            updated.push_back(id >= endId ? id - count : id);
        }

        order.swap(updated);
        return true;
    }

private:
    static constexpr unsigned int MaximumCount()
    {
        return static_cast<unsigned int>(
            std::numeric_limits<int>::max());
    }

    static bool MakeExplicit(const wxArrayInt& order,
                             unsigned int count,
                             wxArrayInt& explicitOrder)
    {
        if ( !IsValid(order, count) )
            return false;

        if ( order.empty() )
        {
            explicitOrder.reserve(count);
            for ( unsigned int id = 0; id < count; ++id )
                explicitOrder.push_back(static_cast<int>(id));
        }
        else
        {
            explicitOrder = order;
        }

        return true;
    }
};

} // namespace wxPrivate

#endif // _WX_PRIVATE_COLUMNORDER_H_
