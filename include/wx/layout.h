/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Purpose:     Layout classes
// Author:      Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LAYOUTH__
#define _WX_LAYOUTH__

#ifdef __GNUG__
#pragma interface "layout.h"
#endif

#include "wx/defs.h"

class WXDLLEXPORT wxWindow;

// X stupidly defines these in X.h
#ifdef Above
#undef Above
#endif
#ifdef Below
#undef Below
#endif

#define wxLAYOUT_DEFAULT_MARGIN 0

enum wxEdge { wxLeft, wxTop, wxRight, wxBottom, wxWidth, wxHeight,
  wxCentre, wxCenter = wxCentre, wxCentreX, wxCentreY };
enum wxRelationship { wxUnconstrained = 0,
                      wxAsIs,
                      wxPercentOf,
                      wxAbove,
                      wxBelow,
                      wxLeftOf,
                      wxRightOf,
                      wxSameAs,
                      wxAbsolute };

class WXDLLEXPORT wxLayoutConstraints;
class WXDLLEXPORT wxIndividualLayoutConstraint: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxIndividualLayoutConstraint)

 protected:
   // 'This' window is the parent or sibling of otherWin
   wxWindow *otherWin;

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

  void Set(wxRelationship rel, wxWindow *otherW, wxEdge otherE, int val = 0, int marg = wxLAYOUT_DEFAULT_MARGIN);

  //
  // Sibling relationships
  //
  void LeftOf(wxWindow *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);
  void RightOf(wxWindow *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);
  void Above(wxWindow *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);
  void Below(wxWindow *sibling, int marg = wxLAYOUT_DEFAULT_MARGIN);

  //
  // 'Same edge' alignment
  //
  void SameAs(wxWindow *otherW, wxEdge edge, int marg = wxLAYOUT_DEFAULT_MARGIN);

  // The edge is a percentage of the other window's edge
  void PercentOf(wxWindow *otherW, wxEdge wh, int per);

  //
  // Edge has absolute value
  //
  void Absolute(int val);

  //
  // Dimension is unconstrained
  //
  inline void Unconstrained() { relationship = wxUnconstrained; }

  //
  // Dimension is 'as is' (use current size settings)
  //
  inline void AsIs() { relationship = wxAsIs; }

  //
  // Accessors
  //
  inline wxWindow *GetOtherWindow() { return otherWin; }
  inline wxEdge GetMyEdge() const { return myEdge; }
  inline void SetEdge(wxEdge which) { myEdge = which; }
  inline void SetValue(int v) { value = v; }
  inline int GetMargin() { return margin; }
  inline void SetMargin(int m) { margin = m; }
  inline int GetValue() const { return value; }
  inline int GetPercent() const { return percent; }
  inline int GetOtherEdge() const { return otherEdge; }
  inline bool GetDone() const { return done; }
  inline void SetDone(bool d) { done = d; }
  inline wxRelationship GetRelationship() { return relationship; }
  inline void SetRelationship(wxRelationship r) { relationship = r; }

  // Reset constraint if it mentions otherWin
  bool ResetIfWin(wxWindow *otherW);

  // Try to satisfy constraint
  bool SatisfyConstraint(wxLayoutConstraints *constraints, wxWindow *win);

  // Get the value of this edge or dimension, or if this
  // is not determinable, -1.
  int GetEdge(wxEdge which, wxWindow *thisWin, wxWindow *other) const;
};

class WXDLLEXPORT wxLayoutConstraints: public wxObject
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

  bool SatisfyConstraints(wxWindow *win, int *noChanges);
  bool AreSatisfied() const
  {
    return left.GetDone() && top.GetDone() && right.GetDone() &&
           bottom.GetDone() && centreX.GetDone() && centreY.GetDone();
  }
};

bool WXDLLEXPORT wxOldDoLayout(wxWindow *win);

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

typedef enum {
    wxSizerShrink,
    wxSizerExpand,
    wxSizerNone
} wxSizerBehaviour;

#define wxTYPE_SIZER        90

class WXDLLEXPORT wxSizer: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxSizer)

 private:
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
  wxSizer(wxWindow *parent, wxSizerBehaviour behav = wxSizerNone);
  ~wxSizer();

  bool Create(wxWindow *parent, wxSizerBehaviour behav = wxSizerNone);
  virtual void SetSize(int x, int y, int w, int h, int flags = wxSIZE_AUTO);
  // Avoid compiler warning
  void SetSize(int w, int h) { wxWindow::SetSize(w, h); }
  virtual void Move(int x, int y);
  virtual void GetSize(int *w, int *h) const;
  inline virtual void GetClientSize(int *w, int *h) const { GetSize(w, h); }
  virtual void GetPosition(int *x, int *y) const;

  inline void SizerSetSize(int x, int y, int w, int h)
    { SetSize(x, y, w, h); }
  inline void SizerMove(int x, int y)
    { Move(x, y); }

  virtual void SetBorder(int w, int h);
  inline int GetBorderX() { return borderX ; }
  inline int GetBorderY() { return borderY ; }

  virtual void AddSizerChild(wxWindow *child);
  virtual void RemoveSizerChild(wxWindow *child);

  inline virtual void SetBehaviour(wxSizerBehaviour b) { sizerBehaviour = b; }
  inline virtual wxSizerBehaviour GetBehaviour() { return sizerBehaviour; }

  virtual bool LayoutPhase1(int *);
  virtual bool LayoutPhase2(int *);
};

#define wxSIZER_ROWS  TRUE
#define wxSIZER_COLS  FALSE

class WXDLLEXPORT wxRowColSizer: public wxSizer
{
  DECLARE_DYNAMIC_CLASS(wxRowColSizer)

 private:
 protected:
  bool rowOrCol;
  int rowOrColSize;
  int xSpacing;
  int ySpacing;
 public:
  // rowOrCol = TRUE to be laid out in rows, otherwise in columns.
  wxRowColSizer();
  wxRowColSizer(wxWindow *parent, bool rowOrCol = wxSIZER_ROWS, int rowsOrColSize = 20, wxSizerBehaviour = wxSizerShrink);
  ~wxRowColSizer();

  bool Create(wxWindow *parent, bool rowOrCol = wxSIZER_ROWS, int rowsOrColSize = 20, wxSizerBehaviour = wxSizerShrink);
  void SetSize(int x, int y, int w, int h, int flags = wxSIZE_AUTO);
  // Avoid compiler warning
  void SetSize(int w, int h) { wxSizer::SetSize(w, h); }

  inline virtual void SetRowOrCol(bool rc) { rowOrCol = rc; }
  inline virtual bool GetRowOrCol() { return rowOrCol; }
  inline virtual void SetRowOrColSize(int n) { rowOrColSize = n; }
  inline virtual int GetRowOrColSize() { return rowOrColSize; }
  inline virtual void SetSpacing(int x, int y) { xSpacing = x; ySpacing = y; }
  inline virtual void GetSpacing(int *x, int *y) { *x = xSpacing; *y = ySpacing; }

  bool LayoutPhase1(int *);
  bool LayoutPhase2(int *);
};

class WXDLLEXPORT wxSpacingSizer: public wxSizer
{
  DECLARE_DYNAMIC_CLASS(wxSpacingSizer)

 private:
 protected:
 public:
  wxSpacingSizer();
  wxSpacingSizer(wxWindow *parent, wxRelationship rel, wxWindow *other, int spacing);
  wxSpacingSizer(wxWindow *parent);
  ~wxSpacingSizer();

  bool Create(wxWindow *parent, wxRelationship rel, wxWindow *other, int sp);
  bool Create(wxWindow *parent);
};

#endif
    // _WX_LAYOUTH__
