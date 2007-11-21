/////////////////////////////////////////////////////////////////////////////
// Name:        game.h
// Purpose:     Life! game logic
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _LIFE_GAME_H_
#define _LIFE_GAME_H_

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// --------------------------------------------------------------------------
// LifePattern
// --------------------------------------------------------------------------

// A class which holds a pattern
class LifePattern
{
public:
    // This ctor is used by the LifeReader class
    LifePattern(wxString      name,
                wxString      description,
                wxString      rules,
                wxArrayString shape)
    {
        m_name        = name;
        m_description = description;
        m_rules       = rules;
        m_shape       = shape;
    };

    // A more convenient ctor for the built-in samples
    LifePattern(wxString      name,
                wxString      description,
                int           width,
                int           height,
                const char   *shape)
    {
        m_name        = name;
        m_description = description;
        m_rules       = wxEmptyString;
        // TODO: add the positions later, since the formatting command
        // causes a crash due to conversion objects not being available
        // during initialisation.
#ifndef __WXMAC__
        m_shape.Add( wxString::Format(_T("%i %i"), -width/2, -height/2) );
#endif
        for(int j = 0; j < height; j++)
        {
            wxString tmp;

            for(int i = 0; i < width; i++)
            {
                tmp += wxChar(shape[j * width + i]);
            }

            m_shape.Add( tmp );
        }
    };

    wxString      m_name;
    wxString      m_description;
    wxString      m_rules;
    wxArrayString m_shape;
};


// --------------------------------------------------------------------------
// Life
// --------------------------------------------------------------------------

// A struct used to pass cell coordinates around
struct LifeCell
{
    wxInt32 i;
    wxInt32 j;
};

// A private class that contains data about a block of cells
class LifeCellBox;

// A class that models a Life game instance
class Life
{
public:
    // ctor and dtor
    Life();
    ~Life();

    // accessors
    inline wxUint32 GetNumCells() const    { return m_numcells; };
    inline wxString GetRules() const       { return m_rules; };
    inline wxString GetDescription() const { return m_description; };
    bool IsAlive(wxInt32 x, wxInt32 y);
    void SetCell(wxInt32 x, wxInt32 y, bool alive = true);
    void SetPattern(const LifePattern &pattern);

    // game control
    void Clear();
    bool NextTic();

    // navigation
    LifeCell FindNorth();
    LifeCell FindSouth();
    LifeCell FindWest();
    LifeCell FindEast();
    LifeCell FindCenter();

    // The following functions find cells within a given viewport; either
    // all alive cells, or only those cells which have changed since last
    // generation. You first call BeginFind() to specify the viewport,
    // then keep calling FindMore() until it returns true.
    //
    // BeginFind:
    //  Specify the viewport and whether to look for alive cells or for
    //  cells which have changed since the last generation and thus need
    //  to be repainted. In this latter case, there is no distinction
    //  between newborn or just-dead cells.
    //
    // FindMore:
    //  Fills an array with cells that match the specification given with
    //  BeginFind(). The array itself belongs to the Life object and must
    //  not be modified or freed by the caller. If this function returns
    //  false, then the operation is not complete: just process all cells
    //  and call FillMore() again.
    //
    void BeginFind(wxInt32 x0, wxInt32 y0,
                   wxInt32 x1, wxInt32 y1,
                   bool changed);
    bool FindMore(LifeCell *cells[], size_t *ncells);

private:
    // cellbox-related
    LifeCellBox *CreateBox(wxInt32 x, wxInt32 y, wxUint32 hv);
    LifeCellBox *LinkBox(wxInt32 x, wxInt32 y, bool create = true);
    void KillBox(LifeCellBox *c);

    // helper for BeginFind & FindMore
    void DoLine(wxInt32 x, wxInt32 y, wxUint32 alive, wxUint32 old = 0);


    // pattern description
    wxString   m_name;          // name (currently unused)
    wxString   m_rules;         // rules (currently unused)
    wxString   m_description;   // description

    // pattern data
    LifeCellBox   *m_head;          // list of alive boxes
    LifeCellBox   *m_available;     // list of reusable dead boxes
    LifeCellBox  **m_boxes;         // hash table of alive boxes
    wxUint32   m_numcells;      // population (number of alive cells)

    // state vars for BeginFind & FindMore
    LifeCell  *m_cells;         // array of cells
    size_t     m_ncells;        // number of valid entries in m_cells
    wxInt32    m_x, m_y,        // counters and search mode
               m_x0, m_y0,
               m_x1, m_y1;
    bool       m_changed;
    bool       m_findmore;
};

#endif  // _LIFE_GAME_H_
