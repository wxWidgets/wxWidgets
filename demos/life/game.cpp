/////////////////////////////////////////////////////////////////////////////
// Name:        game.cpp
// Purpose:     Life! game logic
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// headers, declarations, constants
// ==========================================================================

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/log.h"
#include "wx/module.h"
#include "game.h"

#include <string.h>           // for memset


#define CELLSARRAYSIZE  1024       // static array for BeginFind & co.
#define ALLOCBOXES 16         // number of cellboxes to alloc at once
#define MAXDEAD    8          // tics before removing cellbox from list


// ==========================================================================
// CellBox
// ==========================================================================

#define HASH(x, y) (((x >> 3) & 0x7f) << 7) + ((y >> 3) & 0x7f)

#define HASHSIZE   16384      // hash table size (do not change!)
#define CELLBOX    8          // cells in a cellbox (do not change!)


class LifeCellBox
{
public:
    // members
    inline bool IsAlive(int dx, int dy) const;
    inline bool SetCell(int dx, int dy, bool alive);

    // attributes
    wxInt32       m_x, m_y;                     // position in universe
    wxUint32      m_live1, m_live2;             // alive cells (1 bit per cell)
    wxUint32      m_old1, m_old2;               // old values for m_live1, 2
    wxUint32      m_on[8];                      // neighbouring info
    wxUint32      m_dead;                       // been dead for n generations
    LifeCellBox  *m_up, *m_dn, *m_lf, *m_rt;    // neighbour CellBoxes
    LifeCellBox  *m_prev, *m_next;              // in linked list
    LifeCellBox  *m_hprev, *m_hnext;            // in hash table
};


// IsAlive:
//  Returns whether cell dx, dy in this box is alive
//
bool LifeCellBox::IsAlive(int dx, int dy) const
{
    if (dy > 3)
        return (m_live2 & 1 << ((dy - 4) * 8 + dx)) ? true : false ;
    else
        return (m_live1 & 1 << ((dy) * 8 + dx)) ? true : false ;
}

// SetCell:
//  Sets cell dx, dy in this box to 'alive', returns true if
//  the previous value was different, false if it was the same.
//
bool LifeCellBox::SetCell(int dx, int dy, bool alive)
{
    if (IsAlive(dx, dy) != alive)
    {
       if (dy > 3)
           m_live2 ^= 1 << ((dy - 4) * 8 + dx);
       else
           m_live1 ^= 1 << ((dy) * 8 + dx);

       // reset this here to avoid updating problems
       m_dead = 0;

       return true;
    }
    else
       return false;
}


// ==========================================================================
// Life
// ==========================================================================

// --------------------------------------------------------------------------
// Ctor and dtor
// --------------------------------------------------------------------------

Life::Life()
{
    // pattern description
    m_name        = wxEmptyString;
    m_rules       = wxEmptyString;
    m_description = wxEmptyString;

    // pattern data
    m_numcells    = 0;
    m_boxes       = new LifeCellBox *[HASHSIZE];
    m_head        = NULL;
    m_available   = NULL;
    for (int i = 0; i < HASHSIZE; i++)
        m_boxes[i] = NULL;

    // state vars for BeginFind & FindMore
    m_cells       = new LifeCell[CELLSARRAYSIZE];
    m_ncells      = 0;
    m_findmore    = false;
    m_changed     = false;
}

Life::~Life()
{
    Clear();

    delete[] m_boxes;
    delete[] m_cells;
}

// Clear:
//  Clears the board, freeing all storage.
//
void Life::Clear()
{
    LifeCellBox *c, *nc;

    // clear the hash table pointers
    for (int i = 0; i < HASHSIZE; i++)
        m_boxes[i] = NULL;

    // free used boxes
    c = m_head;
    while (c)
    {
        nc = c->m_next;
        delete c;
        c = nc;
    }
    m_head = NULL;

    // free available boxes
    c = m_available;
    while (c)
    {
        nc = c->m_next;
        delete c;
        c = nc;
    }
    m_available = NULL;

    // reset state
    m_name        = wxEmptyString;
    m_rules       = wxEmptyString;
    m_description = wxEmptyString;
    m_numcells    = 0;
}

// --------------------------------------------------------------------------
// Test and set individual cells
// --------------------------------------------------------------------------

// IsAlive:
//  Returns whether cell (x, y) is alive.
//
bool Life::IsAlive(wxInt32 x, wxInt32 y)
{
    LifeCellBox *c = LinkBox(x, y, false);

    return (c && c->IsAlive( x - c->m_x, y - c->m_y ));
}

// SetCell:
//  Sets or clears cell (x, y), according to the 'alive' param.
//
void Life::SetCell(wxInt32 x, wxInt32 y, bool alive)
{
    LifeCellBox *c  = LinkBox(x, y);
    wxUint32 dx = x - c->m_x;
    wxUint32 dy = y - c->m_y;

    if (c->SetCell(dx, dy, alive))
    {
        if (alive)
            m_numcells++;
        else
            m_numcells--;
    }
}

void Life::SetPattern(const LifePattern& pattern)
{
    wxArrayString data = pattern.m_shape;
    wxString line;
    long x = 0,
         y = 0;

    Clear();
    for (size_t n = 0; n < data.GetCount(); n++)
    {
        line = data[n];

        if ( (line.GetChar(0) != wxT('*')) &&
             (line.GetChar(0) != wxT('.')) )
        {
            // assume that it is a digit or a minus sign
            line.BeforeFirst(wxT(' ')).ToLong(&x);
            line.AfterFirst(wxT(' ')).ToLong(&y);
        }
        else
        {
            // pattern data
            for (size_t k = 0; k < line.Len(); k++)
                SetCell(x + k, y, line.GetChar(k) == wxT('*'));

            y++;
        }
    }

    m_name = pattern.m_name;
    m_rules = pattern.m_rules;
    m_description = pattern.m_description;
}

// --------------------------------------------------------------------------
// Cellbox management functions
// --------------------------------------------------------------------------

// CreateBox:
//  Creates a box in x, y, either taking it from the list
//  of available boxes, or allocating a new one.
//
LifeCellBox* Life::CreateBox(wxInt32 x, wxInt32 y, wxUint32 hv)
{
    LifeCellBox *c;

    // if there are no available boxes, alloc a few more
    if (!m_available)
        for (int i = 1; i <= ALLOCBOXES; i++)
        {
            c = new LifeCellBox();

            if (!c)
            {
                // TODO: handle memory errors. Note that right now, if we
                // couldn't allocate at least one cellbox, we will crash
                // before leaving CreateBox(). Probably we should try to
                // allocate some boxes *before* the m_available list goes
                // empty, so that we have a margin to handle errors
                // gracefully.
                wxLogFatalError(_("Out of memory! Aborting..."));

                // NOTREACHED
            }

            c->m_next = m_available;
            m_available = c;
        }

    // take a cellbox from the list of available boxes
    c = m_available;
    m_available = c->m_next;

    // reset everything
    memset((void *) c, 0, sizeof(LifeCellBox));
    c->m_x = x;
    c->m_y = y;

    // insert c in the list
    c->m_next = m_head;
    m_head = c;
    if (c->m_next) c->m_next->m_prev = c;

    // insert c in the hash table
    c->m_hnext = m_boxes[hv];
    m_boxes[hv] = c;
    if (c->m_hnext) c->m_hnext->m_hprev = c;

    return c;
}

// LinkBox:
//  Returns a pointer to the box (x, y); if it didn't exist yet,
//  it returns NULL or creates a new one, depending on the value
//  of the 'create' parameter.
//
LifeCellBox* Life::LinkBox(wxInt32 x, wxInt32 y, bool create)
{
    wxUint32 hv;
    LifeCellBox *c;

    x &= 0xfffffff8;
    y &= 0xfffffff8;
    hv = HASH(x, y);

    // search in the hash table
    for (c = m_boxes[hv]; c; c = c->m_hnext)
        if ((c->m_x == x) && (c->m_y == y)) return c;

    // if not found, and (create == true), create a new one
    return create? CreateBox(x, y, hv) : (LifeCellBox*) NULL;
}

// KillBox:
//  Removes this box from the list and the hash table and
//  puts it in the list of available boxes.
//
void Life::KillBox(LifeCellBox *c)
{
    wxUint32 hv = HASH(c->m_x, c->m_y);

    // remove from the list
    if (c != m_head)
        c->m_prev->m_next = c->m_next;
    else
        m_head = c->m_next;

    // remove from the hash table
    if (c != m_boxes[hv])
        c->m_hprev->m_hnext = c->m_hnext;
    else
        m_boxes[hv] = c->m_hnext;

    // update neighbours
    if (c->m_next) c->m_next->m_prev = c->m_prev;
    if (c->m_hnext) c->m_hnext->m_hprev = c->m_hprev;
    if (c->m_up) c->m_up->m_dn = NULL;
    if (c->m_dn) c->m_dn->m_up = NULL;
    if (c->m_lf) c->m_lf->m_rt = NULL;
    if (c->m_rt) c->m_rt->m_lf = NULL;

    // append to the list of available boxes
    c->m_next = m_available;
    m_available = c;
}

// --------------------------------------------------------------------------
// Navigation
// --------------------------------------------------------------------------

LifeCell Life::FindCenter()
{
    double sx, sy;
    int n;
    sx = 0.0;
    sy = 0.0;
    n = 0;

    LifeCellBox *c;
    for (c = m_head; c; c = c->m_next)
        if (!c->m_dead)
        {
            sx += c->m_x;
            sy += c->m_y;
            n++;
        }

    if (n > 0)
    {
        sx = (sx / n) + CELLBOX / 2;
        sy = (sy / n) + CELLBOX / 2;
    }

    LifeCell cell;
    cell.i = (wxInt32) sx;
    cell.j = (wxInt32) sy;
    return cell;
}

LifeCell Life::FindNorth()
{
    wxInt32 x = 0, y = 0;
    bool first = true;

    LifeCellBox *c;
    for (c = m_head; c; c = c->m_next)
        if (!c->m_dead && ((first) || (c->m_y < y)))
        {
            x = c->m_x;
            y = c->m_y;
            first = false;
        }

    LifeCell cell;
    cell.i = first? 0 : x + CELLBOX / 2;
    cell.j = first? 0 : y + CELLBOX / 2;
    return cell;
}

LifeCell Life::FindSouth()
{
    wxInt32 x = 0, y = 0;
    bool first = true;

    LifeCellBox *c;
    for (c = m_head; c; c = c->m_next)
        if (!c->m_dead && ((first) || (c->m_y > y)))
        {
            x = c->m_x;
            y = c->m_y;
            first = false;
        }

    LifeCell cell;
    cell.i = first? 0 : x + CELLBOX / 2;
    cell.j = first? 0 : y + CELLBOX / 2;
    return cell;
}

LifeCell Life::FindWest()
{
    wxInt32 x = 0, y = 0;
    bool first = true;

    LifeCellBox *c;
    for (c = m_head; c; c = c->m_next)
        if (!c->m_dead && ((first) || (c->m_x < x)))
        {
            x = c->m_x;
            y = c->m_y;
            first = false;
        }

    LifeCell cell;
    cell.i = first? 0 : x + CELLBOX / 2;
    cell.j = first? 0 : y + CELLBOX / 2;
    return cell;
}

LifeCell Life::FindEast()
{
    wxInt32 x = 0, y = 0;
    bool first = true;

    LifeCellBox *c;
    for (c = m_head; c; c = c->m_next)
        if (!c->m_dead && ((first) || (c->m_x > x)))
        {
            x = c->m_x;
            y = c->m_y;
            first = false;
        }

    LifeCell cell;
    cell.i = first? 0 : x + CELLBOX / 2;
    cell.j = first? 0 : y + CELLBOX / 2;
    return cell;
}

// --------------------------------------------------------------------------
// FindMore & co.
// --------------------------------------------------------------------------

// DoLine:
//  Post eight cells to the cell arrays - leave out the fourth
//  argument (or pass 0, the default value) to post alive cells
//  only, else it will post cells which have changed.
//
void Life::DoLine(wxInt32 x, wxInt32 y, wxUint32 live, wxUint32 old)
{
    wxUint32 diff = (live ^ old) & 0xff;

    if (!diff) return;

    for (wxInt32 k = 8; k; k--, x++)
    {
        if (diff & 0x01)
        {
            m_cells[m_ncells].i = x;
            m_cells[m_ncells].j = y;
            m_ncells++;
        }
        diff >>= 1;
    }
}

void Life::BeginFind(wxInt32 x0, wxInt32 y0, wxInt32 x1, wxInt32 y1, bool changed)
{
    // TODO: optimize for the case where the maximum number of
    // cellboxes that fit in the specified viewport is smaller
    // than the current total of boxes; iterating over the list
    // should then be faster than searching in the hash table.

    m_x0 = m_x = x0 & 0xfffffff8;
    m_y0 = m_y = y0 & 0xfffffff8;
    m_x1 = (x1 + 7) & 0xfffffff8;
    m_y1 = (y1 + 7) & 0xfffffff8;

    m_findmore = true;
    m_changed = changed;
}

bool Life::FindMore(LifeCell *cells[], size_t *ncells)
{
    LifeCellBox *c;
    *cells = m_cells;
    m_ncells = 0;

    if (m_changed)
    {
        for ( ; m_y <= m_y1; m_y += 8, m_x = m_x0)
            for ( ; m_x <= m_x1; m_x += 8)
            {
                if ((c = LinkBox(m_x, m_y, false)) == NULL)
                    continue;

                // check whether there is enough space left in the array
                if (m_ncells > (CELLSARRAYSIZE - 64))
                {
                    *ncells = m_ncells;
                    return false;
                }

                DoLine(m_x, m_y    , c->m_live1,       c->m_old1      );
                DoLine(m_x, m_y + 1, c->m_live1 >> 8,  c->m_old1 >> 8 );
                DoLine(m_x, m_y + 2, c->m_live1 >> 16, c->m_old1 >> 16);
                DoLine(m_x, m_y + 3, c->m_live1 >> 24, c->m_old1 >> 24);
                DoLine(m_x, m_y + 4, c->m_live2,       c->m_old2      );
                DoLine(m_x, m_y + 5, c->m_live2 >> 8,  c->m_old2 >> 8 );
                DoLine(m_x, m_y + 6, c->m_live2 >> 16, c->m_old2 >> 16);
                DoLine(m_x, m_y + 7, c->m_live2 >> 24, c->m_old2 >> 24);
            }
    }
    else
    {
        for ( ; m_y <= m_y1; m_y += 8, m_x = m_x0)
            for ( ; m_x <= m_x1; m_x += 8)
            {
                if ((c = LinkBox(m_x, m_y, false)) == NULL)
                    continue;

                // check whether there is enough space left in the array
                if (m_ncells > (CELLSARRAYSIZE - 64))
                {
                    *ncells = m_ncells;
                    return false;
                }

                DoLine(m_x, m_y    , c->m_live1      );
                DoLine(m_x, m_y + 1, c->m_live1 >> 8 );
                DoLine(m_x, m_y + 2, c->m_live1 >> 16);
                DoLine(m_x, m_y + 3, c->m_live1 >> 24);
                DoLine(m_x, m_y + 4, c->m_live2      );
                DoLine(m_x, m_y + 5, c->m_live2 >> 8 );
                DoLine(m_x, m_y + 6, c->m_live2 >> 16);
                DoLine(m_x, m_y + 7, c->m_live2 >> 24);
            }
    }

    *ncells = m_ncells;
    m_findmore = false;
    return true;
}

// --------------------------------------------------------------------------
// Evolution engine
// --------------------------------------------------------------------------

extern unsigned char *g_tab;
extern int g_tab1[];
extern int g_tab2[];

// NextTic:
//  Advance one step in evolution :-)
//
bool Life::NextTic()
{
    LifeCellBox  *c, *up, *dn, *lf, *rt;
    wxUint32 t1, t2;
    bool     changed = false;

    m_numcells = 0;

    // Stage 1:
    // Compute neighbours of each cell
    //
    // WARNING: unrolled loops and lengthy code follows!
    //
    c = m_head;

    while (c)
    {
        if (! (c->m_live1 || c->m_live2))
        {
            c = c->m_next;
            continue;
        }
        up = c->m_up;
        dn = c->m_dn;
        lf = c->m_lf;
        rt = c->m_rt;

        // up
        t1 = c->m_live1 & 0x000000ff;
        if (t1)
        {
            if (!up)
            {
                up = LinkBox(c->m_x, c->m_y - 8);
                up->m_dn = c;
            }
            t2 = g_tab1[t1];
            up->m_on[7] += t2;
            c->m_on[1] += t2;
            c->m_on[0] += g_tab2[t1];
        }

        // down
        t1 = (c->m_live2 & 0xff000000) >> 24;
        if (t1)
        {
            if (!dn)
            {
                dn = LinkBox(c->m_x, c->m_y + 8);
                dn->m_up = c;
            }
            t2 = g_tab1[t1];
            dn->m_on[0] += t2;
            c->m_on[6] += t2;
            c->m_on[7] += g_tab2[t1];
        }

        t1 = c->m_live1;
        t2 = c->m_live2;

        // left
        if (t1 & 0x01010101)
        {
            if (!lf)
            {
                lf = LinkBox(c->m_x - 8, c->m_y);
                lf->m_rt = c;
            }
            if (t1 & 0x00000001)
            {
               if (!lf->m_up)
               {
                   lf->m_up = LinkBox(c->m_x - 8, c->m_y - 8);
                   lf->m_up->m_dn = lf;
               }
               lf->m_up->m_on[7] += 0x10000000;
               lf->m_on[0] += 0x10000000;
               lf->m_on[1] += 0x10000000;
            }
            if (t1 & 0x00000100)
            {
               lf->m_on[0] += 0x10000000;
               lf->m_on[1] += 0x10000000;
               lf->m_on[2] += 0x10000000;
            }
            if (t1 & 0x00010000)
            {
               lf->m_on[1] += 0x10000000;
               lf->m_on[2] += 0x10000000;
               lf->m_on[3] += 0x10000000;
            }
            if (t1 & 0x01000000)
            {
               lf->m_on[2] += 0x10000000;
               lf->m_on[3] += 0x10000000;
               lf->m_on[4] += 0x10000000;
            }
        }
        if (t2 & 0x01010101)
        {
            if (!lf)
            {
                lf = LinkBox(c->m_x - 8, c->m_y);
                lf->m_rt = c;
            }
            if (t2 & 0x00000001)
            {
               lf->m_on[3] += 0x10000000;
               lf->m_on[4] += 0x10000000;
               lf->m_on[5] += 0x10000000;
            }
            if (t2 & 0x00000100)
            {
               lf->m_on[4] += 0x10000000;
               lf->m_on[5] += 0x10000000;
               lf->m_on[6] += 0x10000000;
            }
            if (t2 & 0x00010000)
            {
               lf->m_on[5] += 0x10000000;
               lf->m_on[6] += 0x10000000;
               lf->m_on[7] += 0x10000000;
            }
            if (t2 & 0x01000000)
            {
               if (!lf->m_dn)
               {
                   lf->m_dn = LinkBox(c->m_x - 8, c->m_y + 8);
                   lf->m_dn->m_up = lf;
               }
               lf->m_on[6] += 0x10000000;
               lf->m_on[7] += 0x10000000;
               lf->m_dn->m_on[0] += 0x10000000;
            }
        }

        // right
        if (t1 & 0x80808080)
        {
            if (!rt)
            {
                rt = LinkBox(c->m_x + 8, c->m_y);
                rt->m_lf = c;
            }
            if (t1 & 0x00000080)
            {
               if (!rt->m_up)
               {
                   rt->m_up = LinkBox(c->m_x + 8, c->m_y - 8);
                   rt->m_up->m_dn = rt;
               }
               rt->m_up->m_on[7] += 0x00000001;
               rt->m_on[0] += 0x00000001;
               rt->m_on[1] += 0x00000001;
            }
            if (t1 & 0x00008000)
            {
               rt->m_on[0] += 0x00000001;
               rt->m_on[1] += 0x00000001;
               rt->m_on[2] += 0x00000001;
            }
            if (t1 & 0x00800000)
            {
               rt->m_on[1] += 0x00000001;
               rt->m_on[2] += 0x00000001;
               rt->m_on[3] += 0x00000001;
            }
            if (t1 & 0x80000000)
            {
               rt->m_on[2] += 0x00000001;
               rt->m_on[3] += 0x00000001;
               rt->m_on[4] += 0x00000001;
            }
        }
        if (t2 & 0x80808080)
        {
            if (!rt)
            {
                rt = LinkBox(c->m_x + 8, c->m_y);
                rt->m_lf = c;
            }
            if (t2 & 0x00000080)
            {
               rt->m_on[3] += 0x00000001;
               rt->m_on[4] += 0x00000001;
               rt->m_on[5] += 0x00000001;
            }
            if (t2 & 0x00008000)
            {
               rt->m_on[4] += 0x00000001;
               rt->m_on[5] += 0x00000001;
               rt->m_on[6] += 0x00000001;
            }
            if (t2 & 0x00800000)
            {
               rt->m_on[5] += 0x00000001;
               rt->m_on[6] += 0x00000001;
               rt->m_on[7] += 0x00000001;
            }
            if (t2 & 0x80000000)
            {
               if (!rt->m_dn)
               {
                   rt->m_dn = LinkBox(c->m_x + 8, c->m_y + 8);
                   rt->m_dn->m_up = rt;
               }
               rt->m_on[6] += 0x00000001;
               rt->m_on[7] += 0x00000001;
               rt->m_dn->m_on[0] += 0x00000001;
            }
        }

        // inner cells
        int i;
        for (i = 1; i <= 3; i++)
        {
            t1 = ((c->m_live1) >> (i * 8)) & 0x000000ff;
            if (t1)
            {
                c->m_on[i - 1] += g_tab1[t1];
                c->m_on[i    ] += g_tab2[t1];
                c->m_on[i + 1] += g_tab1[t1];
            }
        }
        for (i = 0; i <= 2; i++)
        {
            t1 = ((c->m_live2) >> (i * 8)) & 0x000000ff;
            if (t1)
            {
                c->m_on[i + 3] += g_tab1[t1];
                c->m_on[i + 4] += g_tab2[t1];
                c->m_on[i + 5] += g_tab1[t1];
            }
        }

        c->m_up = up;
        c->m_dn = dn;
        c->m_lf = lf;
        c->m_rt = rt;
        c = c->m_next;
    }

    // Stage 2:
    // Stabilize
    //
    c = m_head;

    while (c)
    {
        t1 = 0;
        t2 = 0;

        wxUint32 t3, t4;
        t3 = c->m_live1;
        c->m_old1 = t3;

        t4 = c->m_on[0];
        t1 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3      ) & 0xf) ];
        t1 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 4 ) & 0xf) ] << 4;
        t4 = c->m_on[1];
        t1 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3 >> 8 ) & 0xf) ] << 8;
        t1 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 12) & 0xf) ] << 12;
        t4 = c->m_on[2];
        t1 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3 >> 16) & 0xf) ] << 16;
        t1 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 20) & 0xf) ] << 20;
        t4 = c->m_on[3];
        t1 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3 >> 24) & 0xf) ] << 24;
        t1 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 28) & 0xf) ] << 28;

        t3 = c->m_live2;
        c->m_old2 = t3;

        t4 = c->m_on[4];
        t2 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3      ) & 0xf) ];
        t2 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 4 ) & 0xf) ] << 4;
        t4 = c->m_on[5];
        t2 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3 >> 8 ) & 0xf) ] << 8;
        t2 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 12) & 0xf) ] << 12;
        t4 = c->m_on[6];
        t2 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3 >> 16) & 0xf) ] << 16;
        t2 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 20) & 0xf) ] << 20;
        t4 = c->m_on[7];
        t2 |= g_tab[ ((t4 & 0x0000ffff) << 4 ) + ((t3 >> 24) & 0xf) ] << 24;
        t2 |= g_tab[ ((t4 & 0xffff0000) >> 12) + ((t3 >> 28) & 0xf) ] << 28;

        c->m_on[0] = c->m_on[1] = c->m_on[2] = c->m_on[3] =
        c->m_on[4] = c->m_on[5] = c->m_on[6] = c->m_on[7] = 0;
        c->m_live1 = t1;
        c->m_live2 = t2;

        // count alive cells
#if 1
        wxUint32 t1_, t2_;

        t1_ = (t1  & 0x55555555) + (t1  >> 1 & 0x55555555);
        t1_ = (t1_ & 0x33333333) + (t1_ >> 2 & 0x33333333);

        t2_ = (t2  & 0x55555555) + (t2  >> 1 & 0x55555555);
        t2_ = (t2_ & 0x33333333) + (t2_ >> 2 & 0x33333333) + t1_;
        t2_ = (t2_ & 0x0F0F0F0F) + (t2_ >> 4 & 0x0F0F0F0F);
        t2_ = (t2_ & 0x00FF00FF) + (t2_ >> 8 & 0x00FF00FF);

        m_numcells += (t2_ & 0xFF) + (t2_ >> 16 & 0xFF);
#else
        // Original, slower code
        for (int i = 0; i < 32; i++)
        {
            if (t1 & (1 << i)) m_numcells++;
            if (t2 & (1 << i)) m_numcells++;
        }
#endif

        changed |= ((t1 ^ c->m_old1) || (t2 ^ c->m_old2));

        // mark, and discard if necessary, dead boxes
        if (t1 || t2)
        {
            c->m_dead = 0;
            c = c->m_next;
        }
        else
        {
            LifeCellBox *aux = c->m_next;
            if (c->m_dead++ > MAXDEAD)
               KillBox(c);

            c = aux;
        }
    }

    return changed;
}

// ==========================================================================
// LifeModule
// ==========================================================================

// A module to pregenerate lookup tables without having to do it
// from the application.

class LifeModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(LifeModule);

public:
    LifeModule() {}
    bool OnInit() wxOVERRIDE;
    void OnExit() wxOVERRIDE;
};

wxIMPLEMENT_DYNAMIC_CLASS(LifeModule, wxModule);

bool LifeModule::OnInit()
{
    // see below
    g_tab = new unsigned char [0xfffff];

    if (!g_tab) return false;

    for (wxUint32 i = 0; i < 0xfffff; i++)
    {
        wxUint32 val  = i >> 4;
        wxUint32 old  = i & 0x0000f;
        wxUint32 live = 0;

        for (int j = 0; j < 4; j++)
        {
            live >>= 1;

            if (((val & 0xf) == 3) || (((val & 0xf) == 2) && (old & 0x1)))
                live |= 0x8;

            old >>= 1;
            val >>= 4;
        }

        g_tab[i] = (unsigned char) live;
    }

    return true;
}

void LifeModule::OnExit()
{
    delete [] g_tab;
}


// This table converts from number of neighbors (like in on[]) to
// bits, for a set of four cells. It takes as index a five-digit
// hexadecimal value (0xNNNNB) where Ns hold number of neighbors
// for each cell and B holds their previous state.
//
unsigned char *g_tab;

// This table converts from bits (like in live1, live2) to number
// of neighbors for each cell in the upper or lower row.
//
int g_tab1[]=
{
    0x00000000,
    0x00000011,
    0x00000111,
    0x00000122,
    0x00001110,
    0x00001121,
    0x00001221,
    0x00001232,
    0x00011100,
    0x00011111,
    0x00011211,
    0x00011222,
    0x00012210,
    0x00012221,
    0x00012321,
    0x00012332,
    0x00111000,
    0x00111011,
    0x00111111,
    0x00111122,
    0x00112110,
    0x00112121,
    0x00112221,
    0x00112232,
    0x00122100,
    0x00122111,
    0x00122211,
    0x00122222,
    0x00123210,
    0x00123221,
    0x00123321,
    0x00123332,
    0x01110000,
    0x01110011,
    0x01110111,
    0x01110122,
    0x01111110,
    0x01111121,
    0x01111221,
    0x01111232,
    0x01121100,
    0x01121111,
    0x01121211,
    0x01121222,
    0x01122210,
    0x01122221,
    0x01122321,
    0x01122332,
    0x01221000,
    0x01221011,
    0x01221111,
    0x01221122,
    0x01222110,
    0x01222121,
    0x01222221,
    0x01222232,
    0x01232100,
    0x01232111,
    0x01232211,
    0x01232222,
    0x01233210,
    0x01233221,
    0x01233321,
    0x01233332,
    0x11100000,
    0x11100011,
    0x11100111,
    0x11100122,
    0x11101110,
    0x11101121,
    0x11101221,
    0x11101232,
    0x11111100,
    0x11111111,
    0x11111211,
    0x11111222,
    0x11112210,
    0x11112221,
    0x11112321,
    0x11112332,
    0x11211000,
    0x11211011,
    0x11211111,
    0x11211122,
    0x11212110,
    0x11212121,
    0x11212221,
    0x11212232,
    0x11222100,
    0x11222111,
    0x11222211,
    0x11222222,
    0x11223210,
    0x11223221,
    0x11223321,
    0x11223332,
    0x12210000,
    0x12210011,
    0x12210111,
    0x12210122,
    0x12211110,
    0x12211121,
    0x12211221,
    0x12211232,
    0x12221100,
    0x12221111,
    0x12221211,
    0x12221222,
    0x12222210,
    0x12222221,
    0x12222321,
    0x12222332,
    0x12321000,
    0x12321011,
    0x12321111,
    0x12321122,
    0x12322110,
    0x12322121,
    0x12322221,
    0x12322232,
    0x12332100,
    0x12332111,
    0x12332211,
    0x12332222,
    0x12333210,
    0x12333221,
    0x12333321,
    0x12333332,
    0x11000000,
    0x11000011,
    0x11000111,
    0x11000122,
    0x11001110,
    0x11001121,
    0x11001221,
    0x11001232,
    0x11011100,
    0x11011111,
    0x11011211,
    0x11011222,
    0x11012210,
    0x11012221,
    0x11012321,
    0x11012332,
    0x11111000,
    0x11111011,
    0x11111111,
    0x11111122,
    0x11112110,
    0x11112121,
    0x11112221,
    0x11112232,
    0x11122100,
    0x11122111,
    0x11122211,
    0x11122222,
    0x11123210,
    0x11123221,
    0x11123321,
    0x11123332,
    0x12110000,
    0x12110011,
    0x12110111,
    0x12110122,
    0x12111110,
    0x12111121,
    0x12111221,
    0x12111232,
    0x12121100,
    0x12121111,
    0x12121211,
    0x12121222,
    0x12122210,
    0x12122221,
    0x12122321,
    0x12122332,
    0x12221000,
    0x12221011,
    0x12221111,
    0x12221122,
    0x12222110,
    0x12222121,
    0x12222221,
    0x12222232,
    0x12232100,
    0x12232111,
    0x12232211,
    0x12232222,
    0x12233210,
    0x12233221,
    0x12233321,
    0x12233332,
    0x22100000,
    0x22100011,
    0x22100111,
    0x22100122,
    0x22101110,
    0x22101121,
    0x22101221,
    0x22101232,
    0x22111100,
    0x22111111,
    0x22111211,
    0x22111222,
    0x22112210,
    0x22112221,
    0x22112321,
    0x22112332,
    0x22211000,
    0x22211011,
    0x22211111,
    0x22211122,
    0x22212110,
    0x22212121,
    0x22212221,
    0x22212232,
    0x22222100,
    0x22222111,
    0x22222211,
    0x22222222,
    0x22223210,
    0x22223221,
    0x22223321,
    0x22223332,
    0x23210000,
    0x23210011,
    0x23210111,
    0x23210122,
    0x23211110,
    0x23211121,
    0x23211221,
    0x23211232,
    0x23221100,
    0x23221111,
    0x23221211,
    0x23221222,
    0x23222210,
    0x23222221,
    0x23222321,
    0x23222332,
    0x23321000,
    0x23321011,
    0x23321111,
    0x23321122,
    0x23322110,
    0x23322121,
    0x23322221,
    0x23322232,
    0x23332100,
    0x23332111,
    0x23332211,
    0x23332222,
    0x23333210,
    0x23333221,
    0x23333321,
    0x23333332
};

// This table converts from bits (like in live1, live2) to number
// of neighbors for each cell in the same row (excluding ourselves)
//
int g_tab2[]=
{
    0x00000000,
    0x00000010,
    0x00000101,
    0x00000111,
    0x00001010,
    0x00001020,
    0x00001111,
    0x00001121,
    0x00010100,
    0x00010110,
    0x00010201,
    0x00010211,
    0x00011110,
    0x00011120,
    0x00011211,
    0x00011221,
    0x00101000,
    0x00101010,
    0x00101101,
    0x00101111,
    0x00102010,
    0x00102020,
    0x00102111,
    0x00102121,
    0x00111100,
    0x00111110,
    0x00111201,
    0x00111211,
    0x00112110,
    0x00112120,
    0x00112211,
    0x00112221,
    0x01010000,
    0x01010010,
    0x01010101,
    0x01010111,
    0x01011010,
    0x01011020,
    0x01011111,
    0x01011121,
    0x01020100,
    0x01020110,
    0x01020201,
    0x01020211,
    0x01021110,
    0x01021120,
    0x01021211,
    0x01021221,
    0x01111000,
    0x01111010,
    0x01111101,
    0x01111111,
    0x01112010,
    0x01112020,
    0x01112111,
    0x01112121,
    0x01121100,
    0x01121110,
    0x01121201,
    0x01121211,
    0x01122110,
    0x01122120,
    0x01122211,
    0x01122221,
    0x10100000,
    0x10100010,
    0x10100101,
    0x10100111,
    0x10101010,
    0x10101020,
    0x10101111,
    0x10101121,
    0x10110100,
    0x10110110,
    0x10110201,
    0x10110211,
    0x10111110,
    0x10111120,
    0x10111211,
    0x10111221,
    0x10201000,
    0x10201010,
    0x10201101,
    0x10201111,
    0x10202010,
    0x10202020,
    0x10202111,
    0x10202121,
    0x10211100,
    0x10211110,
    0x10211201,
    0x10211211,
    0x10212110,
    0x10212120,
    0x10212211,
    0x10212221,
    0x11110000,
    0x11110010,
    0x11110101,
    0x11110111,
    0x11111010,
    0x11111020,
    0x11111111,
    0x11111121,
    0x11120100,
    0x11120110,
    0x11120201,
    0x11120211,
    0x11121110,
    0x11121120,
    0x11121211,
    0x11121221,
    0x11211000,
    0x11211010,
    0x11211101,
    0x11211111,
    0x11212010,
    0x11212020,
    0x11212111,
    0x11212121,
    0x11221100,
    0x11221110,
    0x11221201,
    0x11221211,
    0x11222110,
    0x11222120,
    0x11222211,
    0x11222221,
    0x01000000,
    0x01000010,
    0x01000101,
    0x01000111,
    0x01001010,
    0x01001020,
    0x01001111,
    0x01001121,
    0x01010100,
    0x01010110,
    0x01010201,
    0x01010211,
    0x01011110,
    0x01011120,
    0x01011211,
    0x01011221,
    0x01101000,
    0x01101010,
    0x01101101,
    0x01101111,
    0x01102010,
    0x01102020,
    0x01102111,
    0x01102121,
    0x01111100,
    0x01111110,
    0x01111201,
    0x01111211,
    0x01112110,
    0x01112120,
    0x01112211,
    0x01112221,
    0x02010000,
    0x02010010,
    0x02010101,
    0x02010111,
    0x02011010,
    0x02011020,
    0x02011111,
    0x02011121,
    0x02020100,
    0x02020110,
    0x02020201,
    0x02020211,
    0x02021110,
    0x02021120,
    0x02021211,
    0x02021221,
    0x02111000,
    0x02111010,
    0x02111101,
    0x02111111,
    0x02112010,
    0x02112020,
    0x02112111,
    0x02112121,
    0x02121100,
    0x02121110,
    0x02121201,
    0x02121211,
    0x02122110,
    0x02122120,
    0x02122211,
    0x02122221,
    0x11100000,
    0x11100010,
    0x11100101,
    0x11100111,
    0x11101010,
    0x11101020,
    0x11101111,
    0x11101121,
    0x11110100,
    0x11110110,
    0x11110201,
    0x11110211,
    0x11111110,
    0x11111120,
    0x11111211,
    0x11111221,
    0x11201000,
    0x11201010,
    0x11201101,
    0x11201111,
    0x11202010,
    0x11202020,
    0x11202111,
    0x11202121,
    0x11211100,
    0x11211110,
    0x11211201,
    0x11211211,
    0x11212110,
    0x11212120,
    0x11212211,
    0x11212221,
    0x12110000,
    0x12110010,
    0x12110101,
    0x12110111,
    0x12111010,
    0x12111020,
    0x12111111,
    0x12111121,
    0x12120100,
    0x12120110,
    0x12120201,
    0x12120211,
    0x12121110,
    0x12121120,
    0x12121211,
    0x12121221,
    0x12211000,
    0x12211010,
    0x12211101,
    0x12211111,
    0x12212010,
    0x12212020,
    0x12212111,
    0x12212121,
    0x12221100,
    0x12221110,
    0x12221201,
    0x12221211,
    0x12222110,
    0x12222120,
    0x12222211,
    0x12222221
};
