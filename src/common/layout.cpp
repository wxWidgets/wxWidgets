/////////////////////////////////////////////////////////////////////////////
// Name:        layout.cpp
// Purpose:     Constraint layout system classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

// =============================================================================
// declarations
// =============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "layout.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#if wxUSE_CONSTRAINTS

#ifndef WX_PRECOMP
  #include "wx/window.h"
  #include "wx/utils.h"
  #include "wx/dialog.h"
  #include "wx/msgdlg.h"
  #include "wx/intl.h"
#endif

#include "wx/layout.h"

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxIndividualLayoutConstraint, wxObject)
    IMPLEMENT_DYNAMIC_CLASS(wxLayoutConstraints, wxObject)
#endif


wxIndividualLayoutConstraint::wxIndividualLayoutConstraint()
{
    myEdge = wxTop;
    relationship = wxUnconstrained;
    margin = 0;
    value = 0;
    percent = 0;
    otherEdge = wxTop;
    done = FALSE;
    otherWin = (wxWindowBase *) NULL;
}

wxIndividualLayoutConstraint::~wxIndividualLayoutConstraint()
{
}

void wxIndividualLayoutConstraint::Set(wxRelationship rel, wxWindowBase *otherW, wxEdge otherE, int val, int marg)
{
  relationship = rel;
  otherWin = otherW;
  otherEdge = otherE;
  value = val;
  margin = marg;
}

void wxIndividualLayoutConstraint::LeftOf(wxWindowBase *sibling, int marg)
{
    Set(wxLeftOf, sibling, wxLeft, 0, marg);
}

void wxIndividualLayoutConstraint::RightOf(wxWindowBase *sibling, int marg)
{
    Set(wxRightOf, sibling, wxRight, 0, marg);
}

void wxIndividualLayoutConstraint::Above(wxWindowBase *sibling, int marg)
{
    Set(wxAbove, sibling, wxTop, 0, marg);
}

void wxIndividualLayoutConstraint::Below(wxWindowBase *sibling, int marg)
{
    Set(wxBelow, sibling, wxBottom, 0, marg);
}

//
// 'Same edge' alignment
//
void wxIndividualLayoutConstraint::SameAs(wxWindowBase *otherW, wxEdge edge, int marg)
{ 
    Set(wxPercentOf, otherW, edge, 0, marg);
    percent = 100;
}

// The edge is a percentage of the other window's edge
void wxIndividualLayoutConstraint::PercentOf(wxWindowBase *otherW, wxEdge wh, int per)
{ 
    otherWin = otherW;
    relationship = wxPercentOf;
    percent = per;

    otherEdge = wh;
}

//
// Edge has absolute value
//
void wxIndividualLayoutConstraint::Absolute(int val)
{
    value = val; relationship = wxAbsolute;
}

// Reset constraint if it mentions otherWin
bool wxIndividualLayoutConstraint::ResetIfWin(wxWindowBase *otherW)
{
    if (otherW == otherWin)
    {
        myEdge = wxTop;
        relationship = wxAsIs;
        margin = 0;
        value = 0;
        percent = 0;
        otherEdge = wxTop;
        otherWin = (wxWindowBase *) NULL;
        return TRUE;
    }
    else
        return FALSE;
}

// Try to satisfy constraint
bool wxIndividualLayoutConstraint::SatisfyConstraint(wxLayoutConstraints *constraints, wxWindowBase *win)
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
                    // We can know this edge if: otherWin is win's
                    // parent, or otherWin has a satisfied constraint,
                    // or otherWin has no constraint.
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
                    // the right-hand edge and we know the width; OR if
                    // we know the centre and the width.
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
                    // We can know this edge if: otherWin is win's
                    // parent, or otherWin has a satisfied constraint,
                    // or otherWin has no constraint.
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
                    // We know the right-hand edge position if we know the
                    // left-hand edge and we know the width, OR if we know the
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
                    // We can know this edge if: otherWin is win's
                    // parent, or otherWin has a satisfied constraint,
                    // or otherWin has no constraint.
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
                    // We know the top edge position if we know the bottom edge
                    // and we know the height; OR if we know the centre edge and
                    // the height.
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
                    // We know the bottom edge position if we know the top edge
                    // and we know the height; OR if we know the centre edge and
                    // the height.
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
                    // We can know this edge if: otherWin is win's parent, or
                    // otherWin has a satisfied constraint, or otherWin has no
                    // constraint.
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
                    // or otherWin has a satisfied constraint, or otherWin
                    // has no constraint.
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

// Get the value of this edge or dimension, or if this is not determinable, -1.
int wxIndividualLayoutConstraint::GetEdge(wxEdge which,
                                          wxWindowBase *thisWin,
                                          wxWindowBase *other) const
{
    // If the edge or dimension belongs to the parent, then we know the
    // dimension is obtainable immediately. E.g. a wxExpandSizer may contain a
    // button (but the button's true parent is a panel, not the sizer)
    if (other->GetChildren().Find(thisWin))
    {
        switch (which)
        {
            case wxLeft:
                {
                    return 0;
                }
            case wxTop:
                {
                    return 0;
                }
            case wxRight:
                {
                    int w, h;
                    other->GetClientSizeConstraint(&w, &h);
                    return w;
                }
            case wxBottom:
                {
                    int w, h;
                    other->GetClientSizeConstraint(&w, &h);
                    return h;
                }
            case wxWidth:
                {
                    int w, h;
                    other->GetClientSizeConstraint(&w, &h);
                    return w;
                }
            case wxHeight:
                {
                    int w, h;
                    other->GetClientSizeConstraint(&w, &h);
                    return h;
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

wxLayoutConstraints::wxLayoutConstraints()
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

wxLayoutConstraints::~wxLayoutConstraints()
{
}

bool wxLayoutConstraints::SatisfyConstraints(wxWindowBase *win, int *nChanges)
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

    return AreSatisfied();
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
 
#if WXWIN_COMPATIBILITY
bool wxOldDoLayout(wxWindowBase *win)
{
  // Make sure this isn't called recursively from below
  static wxList doneSoFar;

  if (doneSoFar.Member(win))
    return TRUE;

  doneSoFar.Append(win);

  wxNode *node = win->GetChildren().First();
  while (node)
  {
    wxWindowBase *child = (wxWindowBase *)node->Data();
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
    wxNode *node = win->GetChildren().First();
    while (node)
    {
      wxWindowBase *child = (wxWindowBase *)node->Data();
      wxLayoutConstraints *constr = child->GetConstraints();
      if (constr)
      {
        int tempNoChanges = 0;
        (void)constr->SatisfyConstraints(child, &tempNoChanges);
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
    wxDebugMsg(_("wxWindowBase::Layout() failed.\n"));
  }
*/
  // Now set the sizes and positions of the children, and
  // recursively call Layout().
  node = win->GetChildren().First();
  while (node)
  {
    wxWindowBase *child = (wxWindowBase *)node->Data();
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
#endif // WXWIN_COMPATIBILITY

#endif // wxUSE_CONSTRAINTS
