/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Purpose:     Layout classes
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LAYOUTH__
#define _WX_LAYOUTH__

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "layout.h"
#endif

#include "wx/defs.h"

// X stupidly defines these in X.h
#ifdef Above
    #undef Above
#endif
#ifdef Below
    #undef Below
#endif

// ----------------------------------------------------------------------------
// forward declrations
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowBase;
class WXDLLEXPORT wxLayoutConstraints;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define wxLAYOUT_DEFAULT_MARGIN 0

enum wxEdge
{
    wxLeft, wxTop, wxRight, wxBottom, wxWidth, wxHeight,
    wxCentre, wxCenter = wxCentre, wxCentreX, wxCentreY
};

enum wxRelationship
{
    wxUnconstrained = 0,
    wxAsIs,
    wxPercentOf,
    wxAbove,
    wxBelow,
    wxLeftOf,
    wxRightOf,
    wxSameAs,
    wxAbsolute
};

enum wxSizerBehaviour
{
    wxSizerShrink,
    wxSizerExpand,
    wxSizerNone
};

#define wxTYPE_SIZER        90

// =============================================================================
// classes
// =============================================================================

// ----------------------------------------------------------------------------
// wxIndividualLayoutConstraint: a constraint on window position
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxIndividualLayoutConstraint : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxIndividualLayoutConstraint)

protected:
    // To be allowed to modify the internal variables
    friend class wxIndividualLayoutConstraint_Serialize;

    // 'This' window is the parent or sibling of otherWin
    wxWindowBase *otherWin;

    wxEdge myEdge;
    wxRelationship relationship;
    int margin;
    int value;
    int percent;
    wxEdge otherEdge;
    bool done;

public:
    wxIndividualLayoutConstraint();
    ~wxIndividualLayoutConstraint();

    void Set(wxRelationship rel, wxWindowBase *otherW, wxEdge otherE, int val = 0, int marg = wxLAYOUT_DEFAULT_MARGIN);

    //
    // Sibling relationships
    //
    void LeftOf(wxWindowBase *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);
    void RightOf(wxWindowBase *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);
    void Above(wxWindowBase *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);
    void Below(wxWindowBase *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);

    //
    // 'Same edge' alignment
    //
    void SameAs(wxWindowBase *otherW, wxEdge edge, int marg = wxLAYOUT_DEFAULT_MARGIN);

    // The edge is a percentage of the other window's edge
    void PercentOf(wxWindowBase *otherW, wxEdge wh, int per);

    //
    // Edge has absolute value
    //
    void Absolute(int val);

    //
    // Dimension is unconstrained
    //
    void Unconstrained() { relationship = wxUnconstrained; }

    //
    // Dimension is 'as is' (use current size settings)
    //
    void AsIs() { relationship = wxAsIs; }

    //
    // Accessors
    //
    wxWindowBase *GetOtherWindow() { return otherWin; }
    wxEdge GetMyEdge() const { return myEdge; }
    void SetEdge(wxEdge which) { myEdge = which; }
    void SetValue(int v) { value = v; }
    int GetMargin() { return margin; }
    void SetMargin(int m) { margin = m; }
    int GetValue() const { return value; }
    int GetPercent() const { return percent; }
    int GetOtherEdge() const { return otherEdge; }
    bool GetDone() const { return done; }
    void SetDone(bool d) { done = d; }
    wxRelationship GetRelationship() { return relationship; }
    void SetRelationship(wxRelationship r) { relationship = r; }

    // Reset constraint if it mentions otherWin
    bool ResetIfWin(wxWindowBase *otherW);

    // Try to satisfy constraint
    bool SatisfyConstraint(wxLayoutConstraints *constraints, wxWindowBase *win);

    // Get the value of this edge or dimension, or if this
    // is not determinable, -1.
    int GetEdge(wxEdge which, wxWindowBase *thisWin, wxWindowBase *other) const;
};

// ----------------------------------------------------------------------------
// wxLayoutConstraints: the complete set of constraints for a window
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxLayoutConstraints : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxLayoutConstraints)

public:
    // Edge constraints
    wxIndividualLayoutConstraint left;
    wxIndividualLayoutConstraint top;
    wxIndividualLayoutConstraint right;
    wxIndividualLayoutConstraint bottom;
    // Size constraints
    wxIndividualLayoutConstraint width;
    wxIndividualLayoutConstraint height;
    // Centre constraints
    wxIndividualLayoutConstraint centreX;
    wxIndividualLayoutConstraint centreY;

    wxLayoutConstraints();
    ~wxLayoutConstraints();

    bool SatisfyConstraints(wxWindowBase *win, int *noChanges);
    bool AreSatisfied() const
    {
        return left.GetDone() && top.GetDone() && right.GetDone() &&
               bottom.GetDone() && centreX.GetDone() && centreY.GetDone();
    }
};

// ----------------------------------------------------------------------------
// sizers
// ----------------------------------------------------------------------------

/*

Algorithm:

 Each sizer has a Layout function.

 wxExpandSizer::Layout                  ; E.g. for resizeable windows

   - parent size must be known (i.e. called
      from OnSize or explicitly)
   - call Layout on each child to give it a chance to resize
     (e.g. child shrinks around its own children):
     stop when all children return TRUE, or no change
   - evaluate constraints on self to set size

 wxShrinkSizer::Layout                  ; E.g. fit-to-contents windows
                                        ; Perhaps 2 rowcols, one above other.

   - call Layout on each child to give it a chance to resize
     (e.g. child shrinks around its own children):
     stop when each returns TRUE, or no change
   - fit around children
        (what if some want to be centred? E.g. OK/Cancel rowcol.
         - done by centring e.g. bottom sizer w.r.t. top sizer.
           (sibling relationship only))
   - evaluate own constraints (e.g. may be below another window)
   - IF parent is a real window (remember: a real window can
     have only one child sizer, although a sizer can have several child
     (real) windows), then resize this parent WITHOUT invoking Layout
     again.
     Frame and dialog box OnSizes can check if the sizer is a shrink
     sizer; if not, can call layout. Maybe have virtual bool AutoSizeLayout()
     to determine this.

How to relayout if a child sizer/window changes? Need to go all the way
to the top of the hierarchy and call Layout() again.

 wxRowColSizer::Layout

   - Similar to wxShrinkSizer only instead of shrinking to fit
     contents, more sophisticated layout of contents, and THEN
     shrinking (possibly).
   - Do the same parent window check/setsize as for wxShrinkSizer.

*/

class WXDLLEXPORT wxSizer : public wxWindow
{
    DECLARE_DYNAMIC_CLASS(wxSizer)

protected:
    wxSizerBehaviour sizerBehaviour;
    int borderX;
    int borderY;
    int sizerWidth;
    int sizerHeight;
    int sizerX;
    int sizerY;

public:
    wxSizer();
    wxSizer(wxWindowBase *parent, wxSizerBehaviour behav = wxSizerNone);
    ~wxSizer();

    bool Create(wxWindowBase *parent, wxSizerBehaviour behav = wxSizerNone);

    virtual void GetSize(int *w, int *h) const;

    virtual void GetClientSize(int *w, int *h) const { GetSize(w, h); }

    virtual void GetPosition(int *x, int *y) const;

    void SizerSetSize(int x, int y, int w, int h) { SetSize(x, y, w, h); }
    void SizerMove(int x, int y) { Move(x, y); }

    virtual void SetBorder(int w, int h);
    int GetBorderX() { return borderX ; }
    int GetBorderY() { return borderY ; }

    virtual void AddSizerChild(wxWindowBase *child);
    virtual void RemoveSizerChild(wxWindowBase *child);

    virtual void SetBehaviour(wxSizerBehaviour b) { sizerBehaviour = b; }
    virtual wxSizerBehaviour GetBehaviour() { return sizerBehaviour; }

    virtual bool LayoutPhase1(int *);
    virtual bool LayoutPhase2(int *);

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
};

#define wxSIZER_ROWS  TRUE
#define wxSIZER_COLS  FALSE

class WXDLLEXPORT wxRowColSizer : public wxSizer
{
    DECLARE_DYNAMIC_CLASS(wxRowColSizer)

protected:
    bool rowOrCol;
    int rowOrColSize;
    int xSpacing;
    int ySpacing;

public:
    // rowOrCol = TRUE to be laid out in rows, otherwise in columns.
    wxRowColSizer();
    wxRowColSizer(wxWindowBase *parent, bool rowOrCol = wxSIZER_ROWS,
                  int rowsOrColSize = 20, wxSizerBehaviour = wxSizerShrink);
    ~wxRowColSizer();

    bool Create(wxWindowBase *parent, bool rowOrCol = wxSIZER_ROWS,
                int rowsOrColSize = 20, wxSizerBehaviour = wxSizerShrink);

    virtual void SetRowOrCol(bool rc) { rowOrCol = rc; }
    virtual bool GetRowOrCol() { return rowOrCol; }
    virtual void SetRowOrColSize(int n) { rowOrColSize = n; }
    virtual int GetRowOrColSize() { return rowOrColSize; }
    virtual void SetSpacing(int x, int y) { xSpacing = x; ySpacing = y; }
    virtual void GetSpacing(int *x, int *y) { *x = xSpacing; *y = ySpacing; }

    bool LayoutPhase1(int *);
    bool LayoutPhase2(int *);
};

class WXDLLEXPORT wxSpacingSizer : public wxSizer
{
    DECLARE_DYNAMIC_CLASS(wxSpacingSizer)

public:
    wxSpacingSizer();
    wxSpacingSizer(wxWindowBase *parent, wxRelationship rel, wxWindowBase *other, int spacing);
    wxSpacingSizer(wxWindowBase *parent);
    ~wxSpacingSizer();

    bool Create(wxWindowBase *parent, wxRelationship rel, wxWindowBase *other, int sp);
    bool Create(wxWindowBase *parent);
};

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

#if WXWIN_COMPATIBILITY
    extern bool WXDLLEXPORT wxOldDoLayout(wxWindowBase *win);
#endif // WXWIN_COMPATIBILITY

#endif
    // _WX_LAYOUTH__
