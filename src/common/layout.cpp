/////////////////////////////////////////////////////////////////////////////
// Name:        layout.cpp
// Purpose:     Constraint layout system classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "layout.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if USE_CONSTRAINTS

#ifndef WX_PRECOMP
#include "wx/window.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#endif

#include "wx/layout.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxIndividualLayoutConstraint, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxLayoutConstraints, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxSizer, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxRowColSizer, wxSizer)
IMPLEMENT_DYNAMIC_CLASS(wxSpacingSizer, wxSizer)
#endif

/*
TODO:
 - Non shrink-to-fit row-col behaviour.
 - Give justification styles, so can e.g. centre
 the rows & cols, distribute the available space...
 - Shrink-to-fit: should resize outer window (e.g. dialog box)
 if directly associated with this kind of window.
 - How to deal with a rowcol that stretches in one direction
 but shrinks-to-fit in other. E.g. a horizontal toolbar: the width
 stretches to fit the frame, but the height is constant
 or wraps around contents. The algorithm currently assumes
 both dimensions have the same behaviour. Could assume a constant
 height (absolute value).
 - rowcol where each row or column is aligned (length of
   largest element determines spacing)
 - Groupbox sizer
 - Analyze aesthetic dialog boxes and implement using sizers.
 - What reuseable components can we provide? E.g. Ok/Cancel/Help
   group of buttons.
 - use wxStaticItems for aesthetic dialogs.

*/

// Find margin sizes if a sizer, or zero otherwise
int wxSizerMarginX(wxWindow *win)
{
	if ( win->IsKindOf(CLASSINFO(wxSizer)) )
	{
		wxSizer *sizer = (wxSizer *)win;
		return sizer->GetBorderX();
	}
	else
		return 0;
}

int wxSizerMarginY(wxWindow *win)
{
	if ( win->IsKindOf(CLASSINFO(wxSizer)) )
	{
		wxSizer *sizer = (wxSizer *)win;
		return sizer->GetBorderY();
	}
	else
		return 0;
}


wxIndividualLayoutConstraint::wxIndividualLayoutConstraint(void)
{
  myEdge = wxTop; relationship = wxUnconstrained; margin = 0; value = 0; percent = 0; otherEdge = wxTop;
  done = FALSE; otherWin = NULL;
}

wxIndividualLayoutConstraint::~wxIndividualLayoutConstraint(void)
{
}

void wxIndividualLayoutConstraint::Set(wxRelationship rel, wxWindow *otherW, wxEdge otherE, int val, int marg)
{
  relationship = rel; otherWin = otherW; otherEdge = otherE; value = val; margin = marg;
}

void wxIndividualLayoutConstraint::LeftOf(wxWindow *sibling, int marg)
{ Set(wxLeftOf, sibling, wxLeft, 0, marg); }

void wxIndividualLayoutConstraint::RightOf(wxWindow *sibling, int marg)
{ Set(wxRightOf, sibling, wxRight, 0, marg); }

void wxIndividualLayoutConstraint::Above(wxWindow *sibling, int marg)
{ Set(wxAbove, sibling, wxTop, 0, marg); }

void wxIndividualLayoutConstraint::Below(wxWindow *sibling, int marg)
{ Set(wxBelow, sibling, wxBottom, 0, marg); }

//
// 'Same edge' alignment
//
void wxIndividualLayoutConstraint::SameAs(wxWindow *otherW, wxEdge edge, int marg)
{ Set(wxPercentOf, otherW, edge, 0, marg); percent = 100; }

// The edge is a percentage of the other window's edge
void wxIndividualLayoutConstraint::PercentOf(wxWindow *otherW, wxEdge wh, int per)
{ otherWin = otherW; relationship = wxPercentOf; percent = per;
  otherEdge = wh;
}

//
// Edge has absolute value
//
void wxIndividualLayoutConstraint::Absolute(int val)
{ value = val; relationship = wxAbsolute; }

// Reset constraint if it mentions otherWin
bool wxIndividualLayoutConstraint::ResetIfWin(wxWindow *otherW)
{
  if (otherW == otherWin)
  {
    myEdge = wxTop; relationship = wxAsIs; margin = 0; value = 0; percent = 0; otherEdge = wxTop;
    otherWin = NULL;
    return TRUE;
  }
  else
    return FALSE;
}

// Try to satisfy constraint
bool wxIndividualLayoutConstraint::SatisfyConstraint(wxLayoutConstraints *constraints, wxWindow *win)
{
  if (relationship == wxAbsolute)
  {
    done = TRUE;
    return TRUE;
  }
  
  switch (myEdge)
  {
    case wxLeft:
    {
      switch (relationship)
      {
        case wxLeftOf:
        {
          // We can know this edge if: otherWin is win's parent,
          // or otherWin has a satisfied constraint, or
          // otherWin has no constraint.
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos - margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxRightOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos + margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the left-hand edge position if we know
          // the right-hand edge and we know the width; OR if we know the centre and the width.
          if (constraints->right.GetDone() && constraints->width.GetDone())
          {
            value = (constraints->right.GetValue() - constraints->width.GetValue() + margin);
            done = TRUE;
            return TRUE;
          }
          else if (constraints->centreX.GetDone() && constraints->width.GetDone())
          {
            value = (int)(constraints->centreX.GetValue() - (constraints->width.GetValue()/2) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxAsIs:
        {
          int y;
          win->GetPosition(&value, &y);
          done = TRUE;
          return TRUE;
        }
        default:
          break;
      }
      break;
    }
    case wxRight:
    {
      switch (relationship)
      {
        case wxLeftOf:
        {
          // We can know this edge if: otherWin is win's parent,
          // or otherWin has a satisfied constraint, or
          // otherWin has no constraint.
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos - margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxRightOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos + margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01) - margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the right-hand edge position if we know
          // the left-hand edge and we know the width, OR if we know the
          // centre edge and the width.
          if (constraints->left.GetDone() && constraints->width.GetDone())
          {
            value = (constraints->left.GetValue() + constraints->width.GetValue() - margin);
            done = TRUE;
            return TRUE;
          }
          else if (constraints->centreX.GetDone() && constraints->width.GetDone())
          {
            value = (int)(constraints->centreX.GetValue() + (constraints->width.GetValue()/2) - margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxAsIs:
        {
          int x, y;
          int w, h;
          win->GetSize(&w, &h);
          win->GetPosition(&x, &y);
          value = x + w;
          done = TRUE;
          return TRUE;
        }
        default:
          break;
      }
      break;
    }
    case wxTop:
    {
      switch (relationship)
      {
        case wxAbove:
        {
          // We can know this edge if: otherWin is win's parent,
          // or otherWin has a satisfied constraint, or
          // otherWin has no constraint.
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos - margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxBelow:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos + margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the top edge position if we know
          // the bottom edge and we know the height; OR if we know the centre
          // edge and the height.
          if (constraints->bottom.GetDone() && constraints->height.GetDone())
          {
            value = (constraints->bottom.GetValue() - constraints->height.GetValue() + margin);
            done = TRUE;
            return TRUE;
          }
          else if (constraints->centreY.GetDone() && constraints->height.GetDone())
          {
            value = (constraints->centreY.GetValue() - (constraints->height.GetValue()/2) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxAsIs:
        {
          int x;
          win->GetPosition(&x, &value);
          done = TRUE;
          return TRUE;
        }
        default:
          break;
      }
      break;
    }
    case wxBottom:
    {
      switch (relationship)
      {
        case wxAbove:
        {
          // We can know this edge if: otherWin is win's parent,
          // or otherWin has a satisfied constraint, or
          // otherWin has no constraint.
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos + margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxBelow:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos - margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01) - margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the bottom edge position if we know
          // the top edge and we know the height; OR if we know the
          // centre edge and the height.
          if (constraints->top.GetDone() && constraints->height.GetDone())
          {
            value = (constraints->top.GetValue() + constraints->height.GetValue() - margin);
            done = TRUE;
            return TRUE;
          }
          else if (constraints->centreY.GetDone() && constraints->height.GetDone())
          {
            value = (constraints->centreY.GetValue() + (constraints->height.GetValue()/2) - margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxAsIs:
        {
          int x, y;
          int w, h;
          win->GetSize(&w, &h);
          win->GetPosition(&x, &y);
          value = h + y;
          done = TRUE;
          return TRUE;
        }
        default:
          break;
      }
      break;
    }
    case wxCentreX:
    {
      switch (relationship)
      {
        case wxLeftOf:
        {
          // We can know this edge if: otherWin is win's parent,
          // or otherWin has a satisfied constraint, or
          // otherWin has no constraint.
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos - margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxRightOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos + margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the centre position if we know
          // the left-hand edge and we know the width, OR
          // the right-hand edge and the width
          if (constraints->left.GetDone() && constraints->width.GetDone())
          {
            value = (int)(constraints->left.GetValue() + (constraints->width.GetValue()/2) + margin);
            done = TRUE;
            return TRUE;
          }
          else if (constraints->right.GetDone() && constraints->width.GetDone())
          {
            value = (int)(constraints->left.GetValue() - (constraints->width.GetValue()/2) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        default:
          break;
      }
      break;
    }
    case wxCentreY:
    {
      switch (relationship)
      {
        case wxAbove:
        {
          // We can know this edge if: otherWin is win's parent,
          // or otherWin has a satisfied constraint, or
          // otherWin has no constraint.
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos - margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxBelow:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = edgePos + margin;
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the centre position if we know
          // the top edge and we know the height, OR
          // the bottom edge and the height.
          if (constraints->bottom.GetDone() && constraints->height.GetDone())
          {
            value = (int)(constraints->bottom.GetValue() - (constraints->height.GetValue()/2) + margin);
            done = TRUE;
            return TRUE;
          }
          else if (constraints->top.GetDone() && constraints->height.GetDone())
          {
            value = (int)(constraints->top.GetValue() + (constraints->height.GetValue()/2) + margin);
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        default:
          break;
      }
      break;
    }
    case wxWidth:
    {
      switch (relationship)
      {
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01));
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxAsIs:
        {
          if (win)
          {
            int h;
            win->GetSize(&value, &h);
            done = TRUE;
            return TRUE;
          }
          else return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the width if we know the left edge and the right edge, OR
          // if we know the left edge and the centre, OR
          // if we know the right edge and the centre
          if (constraints->left.GetDone() && constraints->right.GetDone())
          {
            value = constraints->right.GetValue() - constraints->left.GetValue();
            done = TRUE;
            return TRUE;
          }
          else if (constraints->centreX.GetDone() && constraints->left.GetDone())
          {
            value = (int)(2*(constraints->centreX.GetValue() - constraints->left.GetValue()));
            done = TRUE;
            return TRUE;
          }
          else if (constraints->centreX.GetDone() && constraints->right.GetDone())
          {
            value = (int)(2*(constraints->right.GetValue() - constraints->centreX.GetValue()));
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        default:
          break;
      }
      break;
    }
    case wxHeight:
    {
      switch (relationship)
      {
        case wxPercentOf:
        {
          int edgePos = GetEdge(otherEdge, win, otherWin);
          if (edgePos != -1)
          {
            value = (int)(edgePos*(((float)percent)*0.01));
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        case wxAsIs:
        {
          if (win)
          {
            int w;
            win->GetSize(&w, &value);
            done = TRUE;
            return TRUE;
          }
          else return FALSE;
        }
        case wxUnconstrained:
        {
          // We know the height if we know the top edge and the bottom edge, OR
          // if we know the top edge and the centre, OR
          // if we know the bottom edge and the centre
          if (constraints->top.GetDone() && constraints->bottom.GetDone())
          {
            value = constraints->bottom.GetValue() - constraints->top.GetValue();
            done = TRUE;
            return TRUE;
          }
          else if (constraints->top.GetDone() && constraints->centreY.GetDone())
          {
            value = (int)(2*(constraints->centreY.GetValue() - constraints->top.GetValue()));
            done = TRUE;
            return TRUE;
          }
          else if (constraints->bottom.GetDone() && constraints->centreY.GetDone())
          {
            value = (int)(2*(constraints->bottom.GetValue() - constraints->centreY.GetValue()));
            done = TRUE;
            return TRUE;
          }
          else
            return FALSE;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
  return FALSE;
}

// Get the value of this edge or dimension, or if this
// is not determinable, -1.
int wxIndividualLayoutConstraint::GetEdge(wxEdge which, wxWindow *thisWin, wxWindow *other)
{
  // If the edge or dimension belongs to the parent, then we
  // know the dimension is obtainable immediately.
  // E.g. a wxExpandSizer may contain a button (but the button's
  // true parent is a panel, not the sizer)
  if (other->GetChildren()->Member(thisWin))
  {
    switch (which)
    {
      case wxLeft:
	  {
		return wxSizerMarginX(other);
	  }
      case wxTop:
      {
        return wxSizerMarginY(other);
      }
      case wxRight:
	  {
        int w, h;
        other->GetClientSizeConstraint(&w, &h);
        return w - wxSizerMarginX(other);
	  }
      case wxBottom:
	  {
        int w, h;
        other->GetClientSizeConstraint(&w, &h);
        return h - wxSizerMarginY(other);
	  }
      case wxWidth:
	  {
        int w, h;
        other->GetClientSizeConstraint(&w, &h);
        return w - 2*wxSizerMarginX(other);
	  }
      case wxHeight:
      {
        int w, h;
        other->GetClientSizeConstraint(&w, &h);
        return h - 2*wxSizerMarginY(other);
      }
      case wxCentreX:
      case wxCentreY:
      {
        int w, h;
        other->GetClientSizeConstraint(&w, &h);
        if (which == wxCentreX)
          return (int)(w/2);
        else
          return (int)(h/2);
      }
      default:
        return -1;
    }
  }
  switch (which)
  {
    case wxLeft:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->left.GetDone())
          return constr->left.GetValue();
        else
          return -1;
      }
      else
      {
        int x, y;
        other->GetPosition(&x, &y);
        return x;
      }
    }
    case wxTop:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->top.GetDone())
          return constr->top.GetValue();
        else
          return -1;
      }
      else
      {
        int x, y;
        other->GetPosition(&x, &y);
        return y;
      }
    }
    case wxRight:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->right.GetDone())
          return constr->right.GetValue();
        else
          return -1;
      }
      else
      {
        int x, y, w, h;
        other->GetPosition(&x, &y);
        other->GetSize(&w, &h);
        return (int)(x + w);
      }
    }
    case wxBottom:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->bottom.GetDone())
          return constr->bottom.GetValue();
        else
          return -1;
      }
      else
      {
        int x, y, w, h;
        other->GetPosition(&x, &y);
        other->GetSize(&w, &h);
        return (int)(y + h);
      }
    }
    case wxWidth:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->width.GetDone())
          return constr->width.GetValue();
        else
          return -1;
      }
      else
      {
        int w, h;
        other->GetSize(&w, &h);
        return w;
      }
    }
    case wxHeight:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->height.GetDone())
          return constr->height.GetValue();
        else
          return -1;
      }
      else
      {
        int w, h;
        other->GetSize(&w, &h);
        return h;
      }
    }
    case wxCentreX:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->centreX.GetDone())
          return constr->centreX.GetValue();
        else
          return -1;
      }
      else
      {
        int x, y, w, h;
        other->GetPosition(&x, &y);
        other->GetSize(&w, &h);
        return (int)(x + (w/2));
      }
    }
    case wxCentreY:
    {
      wxLayoutConstraints *constr = other->GetConstraints();
      // If no constraints, it means the window is not dependent
      // on anything, and therefore we know its value immediately
      if (constr)
      {
        if (constr->centreY.GetDone())
          return constr->centreY.GetValue();
        else
          return -1;
      }
      else
      {
        int x, y, w, h;
        other->GetPosition(&x, &y);
        other->GetSize(&w, &h);
        return (int)(y + (h/2));
      }
    }
    default:
      break;
  }
  return -1;
}

wxLayoutConstraints::wxLayoutConstraints(void)
{
  left.SetEdge(wxLeft);
  top.SetEdge(wxTop);
  right.SetEdge(wxRight);
  bottom.SetEdge(wxBottom);
  centreX.SetEdge(wxCentreX);
  centreY.SetEdge(wxCentreY);
  width.SetEdge(wxWidth);
  height.SetEdge(wxHeight);
}

wxLayoutConstraints::~wxLayoutConstraints(void)
{
}

bool wxLayoutConstraints::SatisfyConstraints(wxWindow *win, int *nChanges)
{
  int noChanges = 0;
  
  bool done = width.GetDone();
  bool newDone = (done ? TRUE : width.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;

  done = height.GetDone();
  newDone = (done ? TRUE : height.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;

  done = left.GetDone();
  newDone = (done ? TRUE : left.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;

  done = top.GetDone();
  newDone = (done ? TRUE : top.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;

  done = right.GetDone();
  newDone = (done ? TRUE : right.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;
          
  done = bottom.GetDone();
  newDone = (done ? TRUE : bottom.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;

  done = centreX.GetDone();
  newDone = (done ? TRUE : centreX.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;

  done = centreY.GetDone();
  newDone = (done ? TRUE : centreY.SatisfyConstraint(this, win));
  if (newDone != done)
    noChanges ++;

  *nChanges = noChanges;

  return (left.GetDone() && top.GetDone() && right.GetDone() && bottom.GetDone() &&
    centreX.GetDone() && centreY.GetDone());
}

/*
 * Main constrained layout algorithm. Look at all the child
 * windows, and their constraints (if any).
 * The idea is to keep iterating through the constraints
 * until all left, right, bottom and top edges, and widths and heights,
 * are known (or no change occurs and we've failed to resolve all
 * constraints).
 *
 * If the user has not specified a dimension or edge, it will be
 * be calculated from the other known values. E.g. If we know
 * the right hand edge and the left hand edge, we now know the width.
 * The snag here is that this means we must specify absolute dimensions
 * twice (in constructor and in constraint), if we wish to use the
 * constraint notation to just set the position, for example.
 * Otherwise, if we only set ONE edge and no dimension, it would never
 * find the other edge.
 *
 * Algorithm:

  Mark all constraints as not done.

  iterations = 0;
  until no change or iterations >= max iterations
    For each child:
    {
      Calculate all constraints
    }
    iterations ++;

  For each child
    Set each calculated position and size

 */
 
bool wxOldDoLayout(wxWindow *win)
{
  // Make sure this isn't called recursively from below
  static wxList doneSoFar;

  if (doneSoFar.Member(win))
    return TRUE;

  doneSoFar.Append(win);

  wxNode *node = win->GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    wxLayoutConstraints *constr = child->GetConstraints();
    if (constr)
    {
      constr->left.SetDone(FALSE);
      constr->top.SetDone(FALSE);
      constr->right.SetDone(FALSE);
      constr->bottom.SetDone(FALSE);
      constr->width.SetDone(FALSE);
      constr->height.SetDone(FALSE);
      constr->centreX.SetDone(FALSE);
      constr->centreY.SetDone(FALSE);
    }
    node = node->Next();
  }
  int noIterations = 0;
  int maxIterations = 500;
  int noChanges = 1;

  while ((noChanges > 0) && (noIterations < maxIterations))
  {
    noChanges = 0;
    wxNode *node = win->GetChildren()->First();
    while (node)
    {
      wxWindow *child = (wxWindow *)node->Data();
      wxLayoutConstraints *constr = child->GetConstraints();
      if (constr)
      {
        int tempNoChanges = 0;
        (void) constr->SatisfyConstraints(child, &tempNoChanges);
        noChanges += tempNoChanges;
      }
      node = node->Next();
    }
    noIterations ++;
  }
/*
  // Would be nice to have a test here to see _which_ constraint(s)
  // failed, so we can print a specific diagnostic message.
  if (noFailures > 0)
  {
    wxDebugMsg("wxWindow::Layout() failed.\n");
  }
*/
  // Now set the sizes and positions of the children, and
  // recursively call Layout().
  node = win->GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    wxLayoutConstraints *constr = child->GetConstraints();
    if (constr && constr->left.GetDone() && constr->right.GetDone() &&
                  constr->width.GetDone() && constr->height.GetDone())
    {
      int x = constr->left.GetValue();
      int y = constr->top.GetValue();
      int w = constr->width.GetValue();
      int h = constr->height.GetValue();

      // If we don't want to resize this window, just move it...
      if ((constr->width.GetRelationship() != wxAsIs) ||
          (constr->height.GetRelationship() != wxAsIs))
      {
        // _Should_ call Layout() recursively.
        child->SetSize(x, y, w, h);
      }
      else
      {
        child->Move(x, y);
      }
    }
    else
      child->Layout();
    node = node->Next();
  }
  doneSoFar.DeleteObject(win);

  return TRUE;
}

wxSizer::wxSizer(void)
{
  sizerBehaviour = wxSizerNone;
  borderX = 2;
  borderY = 2;
  sizerX = 0;
  sizerY = 0;
  sizerWidth = 0;
  sizerHeight = 0;
}

wxSizer::wxSizer(wxWindow *parent, wxSizerBehaviour behav)
{
  Create(parent, behav);
}

bool wxSizer::Create(wxWindow *parent, wxSizerBehaviour behav)
{
  sizerBehaviour = behav;
  borderX = 2;
  borderY = 2;
  m_sizerParent = parent;
  sizerX = 0;
  sizerY = 0;
  sizerWidth = 0;
  sizerHeight = 0;

  // A normal window can have just one top-level sizer
  // associated with it.
  if (!parent->IsKindOf(CLASSINFO(wxSizer)))
  {
    parent->SetSizer(this);
  }
  else
    ((wxSizer *)parent)->AddSizerChild(this);

  switch (sizerBehaviour)
  {
    case wxSizerExpand:
    {
      // Defines a set of constraints
      // to expand the sizer to fit the parent window
      wxLayoutConstraints *c = new wxLayoutConstraints;

      c->left.SameAs(parent, wxLeft, 0);
      c->top.SameAs(parent, wxTop, 0);
      c->right.SameAs(parent, wxRight, 0);
      c->bottom.SameAs(parent, wxBottom, 0);

      SetConstraints(c);
      break;
    }
    case wxSizerShrink:
    case wxSizerNone:
    default:
    {
    }
  }
  return TRUE;
}

wxSizer::~wxSizer(void)
{
  // Remove all children without deleting them,
  // or ~wxbWindow will delete proper windows _twice_
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxNode *next = node->Next();
    wxWindow *win = (wxWindow *)node->Data();
    if (!win->IsKindOf(CLASSINFO(wxSizer)))
	{
      	delete node;
  		win->SetSizerParent(NULL);
	}
    else
    {
      RemoveSizerChild(win);
      delete win;
    }
    node = next;
  }

  if (m_sizerParent) // && !m_sizerParent->IsKindOf(CLASSINFO(wxSizer)))
  {
    m_sizerParent->SetSizer(NULL);
	m_sizerParent = NULL;
  }

}

void wxSizer::SetBorder(int x, int y)
{
	borderX = x;
	borderY = y;
/* No: the margin is for inside, not outside (expansion)

	if ( GetConstraints() )
	{
		GetConstraints()->left.SetMargin(x);
		GetConstraints()->right.SetMargin(x);
		GetConstraints()->top.SetMargin(y);
		GetConstraints()->bottom.SetMargin(y);
	}
*/

}

void wxSizer::AddSizerChild(wxWindow *child)
{
  child->SetSizerParent(this);
  GetChildren()->Append(child);

  // Add some constraints for the purpose of storing
  // the relative position of the window/sizer
  // during layout calculations.
  if (!child->GetConstraints())
  {
    wxLayoutConstraints *c = new wxLayoutConstraints;
    c->left.AsIs();
    c->top.AsIs();
    c->width.AsIs();
    c->height.AsIs();
    int w, h;
    child->GetSize(&w, &h);
    c->width.SetValue(w);
    c->height.SetValue(h);
    
    child->SetConstraints(c);
  }
}

void wxSizer::RemoveSizerChild(wxWindow *child)
{
  GetChildren()->DeleteObject(child);
}

void wxSizer::SetSize(const int x, const int y, const int w, const int h, const int WXUNUSED(flags))
{
  wxLayoutConstraints *constr = GetConstraints();
  if (x != -1)
  {
    sizerX = x;
    if (constr)
      constr->left.SetValue(x);
  }
  if (y != -1)
  {
    sizerY = y;
    if (constr)
      constr->top.SetValue(y);
  }
  if (w != -1)
  {
    sizerWidth = w;
    if (constr)
      constr->width.SetValue(w);
  }
  if (h != -1)
  {
    sizerHeight = h;
    if (constr)
      constr->height.SetValue(h);
  }
}

void wxSizer::Move(const int x, const int y)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (x != -1)
  {
    sizerX = x;
    if (constr)
      constr->left.SetValue(x);
  }
  if (y != -1)
  {
    sizerY = y;
    if (constr)
      constr->top.SetValue(y);
  }
}

void wxSizer::GetSize(int *w, int *h) const
{
  *w = sizerWidth;
  *h = sizerHeight;
}

void wxSizer::GetPosition(int *x, int *y) const
{
  *x = sizerX;
  *y = sizerY;
}

bool wxSizer::LayoutPhase1(int *noChanges)
{
  *noChanges = 0;
  switch (sizerBehaviour)
  {
    case wxSizerExpand:
    {
      if (!m_sizerParent)
      {
        wxMessageBox("wxExpandSizer has no parent!", "Sizer error", wxOK);
        return TRUE;
      }

      // Set the size to fill the parent client area
      int pw, ph;
      m_sizerParent->GetClientSize(&pw, &ph);
      SetSize(GetBorderX(), GetBorderY(), pw - 2*GetBorderX(), ph - 2*GetBorderY());
      wxLayoutConstraints *constr = GetConstraints();

      // Fill in the constraints
      if (constr)
      {
        constr->left.SetValue(0); constr->left.SetDone(TRUE);
        constr->top.SetValue(0); constr->right.SetDone(TRUE);
        constr->width.SetValue(pw); constr->width.SetDone(TRUE);
        constr->height.SetValue(ph); constr->height.SetDone(TRUE);
      }
  
      return TRUE;
      break;
    }
    case wxSizerShrink:
    {
      wxLayoutConstraints *constr = GetConstraints();

      if (constr)
      {
        // Force the constraint to have as-is width and height
        // if we're in shrink-to-fit mode, because if left unconstrained,
        // SatisfyConstraints will fail. The shrink-to-fit option
        // essentially specifies the width and height as 'whatever I calculate'.
        constr->width.AsIs();
        constr->height.AsIs();
      }
      DoPhase(1);
      DoPhase(2);
      // Find the bounding box and set own size
      int maxX = 0;
      int maxY = 0;
      wxNode *node = GetChildren()->First();
      while (node)
      {
        int x, y, width, height;
        wxWindow *win = (wxWindow *)node->Data();
        win->GetSizeConstraint(&width, &height);
        win->GetPositionConstraint(&x, &y);
        if ((x+width) > maxX)
          maxX = (x + width);
        if ((y+height) > maxY)
          maxY = (y + height);
        node = node->Next();
      }
      SetSize(GetBorderX(), GetBorderY(), maxX, maxY);

	  // If this is the only sizer for the parent, size the parent to this sizer.
	  if ( m_sizerParent && (m_sizerParent->GetSizer() == this) )
		m_sizerParent->SetClientSize(maxX + 2*GetBorderX(), maxY + 2*GetBorderY());

      return TRUE;
      break;
    }
    case wxSizerNone:
    {
      wxLayoutConstraints *constr = GetConstraints();
      if (constr)
      {
        bool success = constr->SatisfyConstraints(this, noChanges);
        if (success)
        {
          int x = constr->left.GetValue();
          int y = constr->top.GetValue();
          int w = constr->width.GetValue();
          int h = constr->height.GetValue();
          SetSize(x, y, w, h);
        }
        return success;
      }
      else
        return TRUE;
      break;
    }
  }
  return TRUE;

}

bool wxSizer::LayoutPhase2(int *noChanges)
{
  *noChanges = 0;

  switch (sizerBehaviour)
  {
    case wxSizerExpand:
    {
      // Layout children
      DoPhase(1);
      DoPhase(2);
      return TRUE;
    }
    case wxSizerShrink:
    {
      wxLayoutConstraints *constr = GetConstraints();
      if (constr)
      {
        bool success = constr->SatisfyConstraints(this, noChanges);
        if (success)
        {
          int x = constr->left.GetValue();
          int y = constr->top.GetValue();
          Move(x, y);
        }
        return success;
      }
      break;
    }
    case wxSizerNone:
    {
      // Layout children
      DoPhase(1);
      DoPhase(2);

	  // Is this a dumb fix for lack of constraint evaluation?
      wxLayoutConstraints *constr = GetConstraints();
      if (constr)
      {
        bool success = constr->SatisfyConstraints(this, noChanges);
        if (success)
        {
          int x = constr->left.GetValue();
          int y = constr->top.GetValue();
          int w = constr->width.GetValue();
          int h = constr->height.GetValue();
          SetSize(x, y, w, h);
        }
        return success;
      }
      else
        return TRUE;
    }
  }
  return TRUE;
}

/*
 * wxRowColSizer
 */
 
wxRowColSizer::wxRowColSizer(void)
{
  rowOrCol = TRUE;
  rowOrColSize = 20;
  xSpacing = 2;
  ySpacing = 2;
}

wxRowColSizer::wxRowColSizer(wxWindow *parent, bool rc, int n, wxSizerBehaviour behav)
{
  Create(parent, rc, n, behav);
}

bool wxRowColSizer::Create(wxWindow *parent, bool rc, int n, wxSizerBehaviour behav)
{
  wxSizer::Create(parent, behav);
  
  rowOrCol = rc;
  rowOrColSize = n;
  xSpacing = 2;
  ySpacing = 2;

  return TRUE;
}

wxRowColSizer::~wxRowColSizer(void)
{
}

void wxRowColSizer::SetSize(const int x, const int y, const int w, const int h, const int flags)
{
  wxSizer::SetSize(x, y, w, h, flags);
}

bool wxRowColSizer::LayoutPhase1(int *noChanges)
{
  *noChanges = 0;
  wxLayoutConstraints *constr = GetConstraints();

  if (constr)
  {
    // Force the constraint to have as-is width and height
    // if we're in shrink-to-fit mode, because if left unconstrained,
    // SatisfyConstraints will fail. The shrink-to-fit option
    // essentially specifies the width and height as 'whatever I calculate'.
    if (sizerBehaviour == wxSizerShrink)
    {
      constr->width.AsIs();
      constr->height.AsIs();
    }

    // Only evaluate the constraints FIRST if we're NOT
    // in shrink-to-fit mode, i.e. we want to size the rowcol
    // first, then lay the children out in the space we've calculated.
    if (sizerBehaviour != wxSizerShrink)
    {
      bool success = constr->SatisfyConstraints(this, noChanges);
      if (success)
      {
        int x = constr->left.GetValue();
        int y = constr->top.GetValue();
        int w = constr->width.GetValue();
        int h = constr->height.GetValue();
        SetSize(x, y, w, h);
      }
      else
        return FALSE;

      // Continue to do the rest of the phase when the constraints have been
      // satisfied, i.e. we're on the last iteration of phase 1 and
      // can now do the actual rowcol laying out.
    }
  }

  // If we ARE in shrink-to-fit mode, we must now
  // calculate the child sizes BEFORE laying out in rows or columns.
  if (sizerBehaviour == wxSizerShrink)
  {
    DoPhase(1);
    DoPhase(2);

    // WILL THE WINDOW BE SIZED CORRECTLY AT THIS POINT?
    // CHECK CONSTRAINTS IF ANY...
    int noRows = 0;
    int noCols = 0;
    int currentX = borderX;
    int currentY = borderY;
    int maxX = currentX;
    int maxY = currentY;
    
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      int childWidth, childHeight;
      if (win->GetConstraints() &&
          win->GetConstraints()->width.GetDone() &&
          win->GetConstraints()->height.GetDone())
      {
        childWidth = win->GetConstraints()->width.GetValue();
        childHeight = win->GetConstraints()->height.GetValue();
      }
      else
        win->GetSize(&childWidth, &childHeight);

      win->MoveConstraint(currentX, currentY);

      if ((currentX + childWidth) > maxX)
        maxX = (currentX + childWidth);
      if ((currentY + childHeight) > maxY)
        maxY = (currentY + childHeight);

      if (rowOrCol)
      {
        currentX += childWidth + xSpacing;
        noCols ++;

        // Reset to start of row
        if (noCols == rowOrColSize)
        {
          currentX = borderX;
          currentY += childHeight + ySpacing;
          noCols = 0;
        }
      }
      else
      {
        currentY += childHeight + ySpacing;
        noRows ++;

        // Reset to start of col
        if (noRows == rowOrColSize)
        {
          currentY = borderY;
          currentX += childWidth + xSpacing;
          noRows = 0;
        }
      }
      
      node = node->Next();
    }
    maxX += borderX;
    maxY += borderY;

    SetSize(-1, -1, maxX, maxY);
  }
  return TRUE;
}

bool wxRowColSizer::LayoutPhase2(int *noChanges)
{
  *noChanges = 0;
  
  // If shrink-to-fit, it's only at Phase 2 that we know the size of
  // the wxRowColSizer, and now we can evaluate the
  // constraints and pass result back up to parent.
  // This implements a depth-first strategy
  if (sizerBehaviour == wxSizerShrink)
  {
    wxLayoutConstraints *constr = GetConstraints();
    if (constr)
    {
      bool success = constr->SatisfyConstraints(this, noChanges);
      if (success)
      {
        int x = constr->left.GetValue();
        int y = constr->top.GetValue();
        Move(x, y);
      }
      return success;
    }
    else return TRUE;
  }
  else
  {
    // Lay out the children: breadth-first strategy.
    DoPhase(1);
    DoPhase(2);

    // Space them
  }
  return TRUE;
}


/*
 * wxSpacingSizer
 */
 
wxSpacingSizer::wxSpacingSizer(void)
{
}

wxSpacingSizer::wxSpacingSizer(wxWindow *parent)
{
  Create(parent);
}

wxSpacingSizer::wxSpacingSizer(wxWindow *parent, wxRelationship rel, wxWindow *other, int spacing)
{
  Create(parent, rel, other, spacing);
}

bool wxSpacingSizer::Create(wxWindow *parent)
{
  wxSizer::Create(parent);
  return TRUE;
}

bool wxSpacingSizer::Create(wxWindow *parent, wxRelationship rel, wxWindow *other, int spacing)
{
  wxLayoutConstraints *c = new wxLayoutConstraints;

  wxSizer::Create(parent);

  switch ( rel )
  {
	case wxLeftOf :
  		c->width.Absolute	(spacing);
		c->top.SameAs		(other, wxTop);
		c->bottom.SameAs	(other, wxBottom);
		c->right.LeftOf		(other);
		break;
	case wxRightOf :
  		c->width.Absolute	(spacing);
		c->top.SameAs		(other, wxTop);
		c->bottom.SameAs	(other, wxBottom);
		c->left.RightOf		(other);
		break;
	case wxBelow :
  		c->height.Absolute	(spacing);
		c->left.SameAs		(other, wxLeft);
		c->right.SameAs		(other, wxRight);
		c->top.Below		(other);
		break;
	case wxAbove :
  		c->height.Absolute	(spacing);
		c->left.SameAs		(other, wxLeft);
		c->right.SameAs		(other, wxRight);
		c->bottom.Above		(other);
		break;

	default :
		break;
  }
  SetConstraints(c);

  return TRUE;
}

wxSpacingSizer::~wxSpacingSizer(void)
{
}



#endif
