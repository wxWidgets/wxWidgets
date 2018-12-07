///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/rowheightcache.h
// Purpose:     height cache of rows in a dataview
// Author:      Jens Goepfert (mail@jensgoepfert.de)
// Created:     2018-03-06
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_ROWHEIGHTCACHE_H_
#define _WX_PRIVATE_ROWHEIGHTCACHE_H_

#include "wx/hashmap.h"
#include "wx/vector.h"

// struct describing a range of rows which contains rows <from> .. <to-1>
struct RowRange
{
    unsigned int from;
    unsigned int to;
};

/**
@class RowRanges
A helper class that manages a set of RowRange objects.
It stores the indices that are members of a group in a memory
efficiant way.
*/
class WXDLLIMPEXP_CORE RowRanges
//class RowRanges
{
public:
    /**
    Adds a row index to this group by adding it to an existing RowRange
    or by creating a new one.
    */
    void Add(unsigned int row);

    /**
    Removes a row index and all indices after idx from this group.
    */
    void Remove(unsigned int row);

    /**
    Checks whether a row index is contained in this group.
    */
    bool Has(unsigned int row) const;

    /**
    Returns the number of row indices that are contained in this group.
    */
    unsigned int CountAll() const;

    /**
    Returns the number of rows that are in this group before the given row index.
    not including given row.
    */
    unsigned int CountTo(unsigned int row) const;

    /**
        Returns the size of the range.

        This is only used for testing and debugging.
     */
    unsigned int GetSize() const { return m_ranges.size(); }

private:
    wxVector<RowRange> m_ranges;
    /**
    If a new row index was inserted Cleanup checks if the neighbour ranges
    of idx can includes the same row indices and discards
    unnecessary RowRange objects.
    */
    void CleanUp(unsigned int idx);
};

WX_DECLARE_HASH_MAP(unsigned int, RowRanges*, wxIntegerHash, wxIntegerEqual,
    HeightToRowRangesMap);

/**
@class HeightCache

HeightCache implements a cache mechanism for the DataViewCtrl to give
fast access to:
* the height of one line (GetLineHeight)
* the y-coordinate where a row starts (GetLineStart)
* and vice versa (GetLineAt)

The layout of the cache is a hashmap where the keys are all exisiting
row heights in pixels. The values are RowRange objects that represents
all having the specified height.

{
22: RowRange([0..10], [15..17], [20..2000]),
42: RowRange([11..12], [18..18]),
62: RowRange([13..14], [19..19])
}

Examples
========

GetLineStart
------------
To retrieve the y-coordinate of item 1000 it is neccessary to look into
each key of the hashmap *m_heightToRowRange*. Get the row count of
indices lower than 1000 (RowRange::CountTo) and multiplies it wich the
according height.

RowRange([0..10], [15..17], [20..2000]).CountTo(1000)
--> 0..10 are 11 items, 15..17 are 3 items and 20..1000 are 980 items (1000-20)
= 11 + 3 + 980 = 994 items

GetLineStart(1000) --> (22 * 994) + (42 * 3) + (62 * 3) = 22180

GetLineHeight
-------------
To retrieve the line height look into each key and check if row is
contained in RowRange (RowRange::Has)

GetLineAt
---------
To retrieve the row that starts at a specific y-coordinate.
Look into each key and count all rows.
Use bisect algorithm in combination with GetLineStart() to
find the appropriate item
*/
class WXDLLIMPEXP_ADV HeightCache
{
public:
    bool GetLineStart(unsigned int row, int &start);
    bool GetLineHeight(unsigned int row, int &height);
    bool GetLineAt(int y, unsigned int &row);

    void Put(const unsigned int row, const int height);
    /**
    removes the stored height of the given row from the cache
    and invalidates all cached rows (including row)
    */
    void Remove(const unsigned int row);
    void Clear();

private:
    bool GetLineInfo(unsigned int row, int &start, int &height);
    HeightToRowRangesMap  m_heightToRowRange;
};


#endif // _WX_PRIVATE_ROWHEIGHTCACHE_H_
