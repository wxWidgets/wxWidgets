/////////////////////////////////////////////////////////////////////////////
// Name:        constrnt.cpp
// Purpose:     OGL Constraint classes
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "constrnt.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifdef PROLOGIO
#include <wx/wxexpr.h>
#endif

#include "basic.h"
#include "constrnt.h"
#include "canvas.h"

wxList *OGLConstraintTypes = NULL;

/*
 * Constraint type
 *
 */

IMPLEMENT_DYNAMIC_CLASS(OGLConstraintType, wxObject)

OGLConstraintType::OGLConstraintType(int theType, const wxString& theName, const wxString& thePhrase)
{
  m_type = theType;
  m_name = theName;
  m_phrase = thePhrase;
}

OGLConstraintType::~OGLConstraintType()
{
}

void OGLInitializeConstraintTypes()
{
    if (!OGLConstraintTypes)
        return;

    OGLConstraintTypes = new wxList(wxKEY_INTEGER);

    OGLConstraintTypes->Append(gyCONSTRAINT_CENTRED_VERTICALLY,
        new OGLConstraintType(gyCONSTRAINT_CENTRED_VERTICALLY, "Centre vertically", "centred vertically w.r.t."));

    OGLConstraintTypes->Append(gyCONSTRAINT_CENTRED_HORIZONTALLY,
        new OGLConstraintType(gyCONSTRAINT_CENTRED_HORIZONTALLY, "Centre horizontally", "centred horizontally w.r.t."));

    OGLConstraintTypes->Append(gyCONSTRAINT_CENTRED_BOTH,
        new OGLConstraintType(gyCONSTRAINT_CENTRED_BOTH, "Centre", "centred w.r.t."));

    OGLConstraintTypes->Append(gyCONSTRAINT_LEFT_OF,
        new OGLConstraintType(gyCONSTRAINT_LEFT_OF, "Left of", "left of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_RIGHT_OF,
        new OGLConstraintType(gyCONSTRAINT_RIGHT_OF, "Right of", "right of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_ABOVE,
        new OGLConstraintType(gyCONSTRAINT_ABOVE, "Above", "above"));

    OGLConstraintTypes->Append(gyCONSTRAINT_BELOW,
        new OGLConstraintType(gyCONSTRAINT_BELOW, "Below", "below"));

    // Alignment
    OGLConstraintTypes->Append(gyCONSTRAINT_ALIGNED_TOP,
        new OGLConstraintType(gyCONSTRAINT_ALIGNED_TOP, "Top-aligned", "aligned to the top of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_ALIGNED_BOTTOM,
        new OGLConstraintType(gyCONSTRAINT_ALIGNED_BOTTOM, "Bottom-aligned", "aligned to the bottom of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_ALIGNED_LEFT,
        new OGLConstraintType(gyCONSTRAINT_ALIGNED_LEFT, "Left-aligned", "aligned to the left of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_ALIGNED_RIGHT,
        new OGLConstraintType(gyCONSTRAINT_ALIGNED_RIGHT, "Right-aligned", "aligned to the right of"));

    // Mid-alignment
    OGLConstraintTypes->Append(gyCONSTRAINT_MIDALIGNED_TOP,
        new OGLConstraintType(gyCONSTRAINT_MIDALIGNED_TOP, "Top-midaligned", "centred on the top of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_MIDALIGNED_BOTTOM,
        new OGLConstraintType(gyCONSTRAINT_MIDALIGNED_BOTTOM, "Bottom-midaligned", "centred on the bottom of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_MIDALIGNED_LEFT,
        new OGLConstraintType(gyCONSTRAINT_MIDALIGNED_LEFT, "Left-midaligned", "centred on the left of"));

    OGLConstraintTypes->Append(gyCONSTRAINT_MIDALIGNED_RIGHT,
        new OGLConstraintType(gyCONSTRAINT_MIDALIGNED_RIGHT, "Right-midaligned", "centred on the right of"));
}

void OGLCleanUpConstraintTypes()
{
    if (!OGLConstraintTypes)
        return;

    wxNode* node = OGLConstraintTypes->First();
    while (node)
    {
        OGLConstraintType* ct = (OGLConstraintType*) node->Data();
        delete ct;
        node = node->Next();
    }
    delete OGLConstraintTypes;
    OGLConstraintTypes = NULL;
}

/*
 * Constraint Stuff
 *
 */

IMPLEMENT_DYNAMIC_CLASS(OGLConstraint, wxObject)

OGLConstraint::OGLConstraint(int type, wxShape *constraining, wxList& constrained)
{
  m_xSpacing = 0.0;
  m_ySpacing = 0.0;

  m_constraintType = type;
  m_constrainingObject = constraining;

  m_constraintId = 0;
  m_constraintName = "noname";

  wxNode *node = constrained.First();
  while (node)
  {
    m_constrainedObjects.Append(node->Data());
    node = node->Next();
  }
}

OGLConstraint::~OGLConstraint()
{
}

bool OGLConstraint::Equals(float a, float b)
{
  float marg = 0.5;

  bool eq = ((b <= a + marg) && (b >= a - marg));
  return eq;
}

// Return TRUE if anything changed
bool OGLConstraint::Evaluate()
{
  float maxWidth, maxHeight, minWidth, minHeight, x, y;
  m_constrainingObject->GetBoundingBoxMax(&maxWidth, &maxHeight);
  m_constrainingObject->GetBoundingBoxMin(&minWidth, &minHeight);
  x = m_constrainingObject->GetX();
  y = m_constrainingObject->GetY();

  wxClientDC dc(m_constrainingObject->GetCanvas());
  m_constrainingObject->GetCanvas()->PrepareDC(dc);

  switch (m_constraintType)
  {
    case gyCONSTRAINT_CENTRED_VERTICALLY:
    {
      int n = m_constrainedObjects.Number();
      float totalObjectHeight = 0.0;
      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);
        totalObjectHeight += height2;
        node = node->Next();
      }
      float startY;
      float spacingY;
      // Check if within the constraining object...
      if ((totalObjectHeight + (n + 1)*m_ySpacing) <= minHeight)
      {
        spacingY = (float)((minHeight - totalObjectHeight)/(n + 1));
        startY = (float)(y - (minHeight/2.0));
      }
      // Otherwise, use default spacing
      else
      {
        spacingY = m_ySpacing;
        startY = (float)(y - ((totalObjectHeight + (n+1)*spacingY)/2.0));
      }

      // Now position the objects
      bool changed = FALSE;
      node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();
        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);
        startY += (float)(spacingY + (height2/2.0));
        if (!Equals(startY, constrainedObject->GetY()))
        {
          constrainedObject->Move(dc, constrainedObject->GetX(), startY, FALSE);
          changed = TRUE;
        }
        startY += (float)(height2/2.0);
        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_CENTRED_HORIZONTALLY:
    {
      int n = m_constrainedObjects.Number();
      float totalObjectWidth = 0.0;
      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);
        totalObjectWidth += width2;
        node = node->Next();
      }
      float startX;
      float spacingX;
      // Check if within the constraining object...
      if ((totalObjectWidth + (n + 1)*m_xSpacing) <= minWidth)
      {
        spacingX = (float)((minWidth - totalObjectWidth)/(n + 1));
        startX = (float)(x - (minWidth/2.0));
      }
      // Otherwise, use default spacing
      else
      {
        spacingX = m_xSpacing;
        startX = (float)(x - ((totalObjectWidth + (n+1)*spacingX)/2.0));
      }

      // Now position the objects
      bool changed = FALSE;
      node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();
        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);
        startX += (float)(spacingX + (width2/2.0));
        if (!Equals(startX, constrainedObject->GetX()))
        {
          constrainedObject->Move(dc, startX, constrainedObject->GetY(), FALSE);
          changed = TRUE;
        }
        startX += (float)(width2/2.0);
        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_CENTRED_BOTH:
    {
      int n = m_constrainedObjects.Number();
      float totalObjectWidth = 0.0;
      float totalObjectHeight = 0.0;
      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);
        totalObjectWidth += width2;
        totalObjectHeight += height2;
        node = node->Next();
      }
      float startX;
      float spacingX;
      float startY;
      float spacingY;

      // Check if within the constraining object...
      if ((totalObjectWidth + (n + 1)*m_xSpacing) <= minWidth)
      {
        spacingX = (float)((minWidth - totalObjectWidth)/(n + 1));
        startX = (float)(x - (minWidth/2.0));
      }
      // Otherwise, use default spacing
      else
      {
        spacingX = m_xSpacing;
        startX = (float)(x - ((totalObjectWidth + (n+1)*spacingX)/2.0));
      }

      // Check if within the constraining object...
      if ((totalObjectHeight + (n + 1)*m_ySpacing) <= minHeight)
      {
        spacingY = (float)((minHeight - totalObjectHeight)/(n + 1));
        startY = (float)(y - (minHeight/2.0));
      }
      // Otherwise, use default spacing
      else
      {
        spacingY = m_ySpacing;
        startY = (float)(y - ((totalObjectHeight + (n+1)*spacingY)/2.0));
      }

      // Now position the objects
      bool changed = FALSE;
      node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();
        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);
        startX += (float)(spacingX + (width2/2.0));
        startY += (float)(spacingY + (height2/2.0));

        if ((!Equals(startX, constrainedObject->GetX())) || (!Equals(startY, constrainedObject->GetY())))
	    {
          constrainedObject->Move(dc, startX, startY, FALSE);
          changed = TRUE;
	    }

        startX += (float)(width2/2.0);
        startY += (float)(height2/2.0);

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_LEFT_OF:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float x3 = (float)(x - (minWidth/2.0) - (width2/2.0) - m_xSpacing);
        if (!Equals(x3, constrainedObject->GetX()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, x3, constrainedObject->GetY(), FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_RIGHT_OF:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float x3 = (float)(x + (minWidth/2.0) + (width2/2.0) + m_xSpacing);
        if (!Equals(x3, constrainedObject->GetX()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, x3, constrainedObject->GetY(), FALSE);
	    }

        node = node->Next();
      }
      return changed;

      return FALSE;
    }
    case gyCONSTRAINT_ABOVE:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float y3 = (float)(y - (minHeight/2.0) - (height2/2.0) - m_ySpacing);
        if (!Equals(y3, constrainedObject->GetY()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, constrainedObject->GetX(), y3, FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_BELOW:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float y3 = (float)(y + (minHeight/2.0) + (height2/2.0) + m_ySpacing);
        if (!Equals(y3, constrainedObject->GetY()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, constrainedObject->GetX(), y3, FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_ALIGNED_LEFT:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float x3 = (float)(x - (minWidth/2.0) + (width2/2.0) + m_xSpacing);
        if (!Equals(x3, constrainedObject->GetX()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, x3, constrainedObject->GetY(), FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_ALIGNED_RIGHT:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float x3 = (float)(x + (minWidth/2.0) - (width2/2.0) - m_xSpacing);
        if (!Equals(x3, constrainedObject->GetX()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, x3, constrainedObject->GetY(), FALSE);
	    }

        node = node->Next();
      }
      return changed;

      return FALSE;
    }
    case gyCONSTRAINT_ALIGNED_TOP:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float y3 = (float)(y - (minHeight/2.0) + (height2/2.0) + m_ySpacing);
        if (!Equals(y3, constrainedObject->GetY()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, constrainedObject->GetX(), y3, FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_ALIGNED_BOTTOM:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float width2, height2;
        constrainedObject->GetBoundingBoxMax(&width2, &height2);

        float y3 = (float)(y + (minHeight/2.0) - (height2/2.0) - m_ySpacing);
        if (!Equals(y3, constrainedObject->GetY()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, constrainedObject->GetX(), y3, FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_MIDALIGNED_LEFT:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float x3 = (float)(x - (minWidth/2.0));
        if (!Equals(x3, constrainedObject->GetX()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, x3, constrainedObject->GetY(), FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_MIDALIGNED_RIGHT:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float x3 = (float)(x + (minWidth/2.0));
        if (!Equals(x3, constrainedObject->GetX()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, x3, constrainedObject->GetY(), FALSE);
	    }

        node = node->Next();
      }
      return changed;

      return FALSE;
    }
    case gyCONSTRAINT_MIDALIGNED_TOP:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float y3 = (float)(y - (minHeight/2.0));
        if (!Equals(y3, constrainedObject->GetY()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, constrainedObject->GetX(), y3, FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }
    case gyCONSTRAINT_MIDALIGNED_BOTTOM:
    {
      bool changed = FALSE;

      wxNode *node = m_constrainedObjects.First();
      while (node)
      {
        wxShape *constrainedObject = (wxShape *)node->Data();

        float y3 = (float)(y + (minHeight/2.0));
        if (!Equals(y3, constrainedObject->GetY()))
	    {
          changed = TRUE;
          constrainedObject->Move(dc, constrainedObject->GetX(), y3, FALSE);
	    }

        node = node->Next();
      }
      return changed;
    }

    default:
      return FALSE;
  }
  return FALSE;
}

