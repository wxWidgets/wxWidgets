/////////////////////////////////////////////////////////////////////////////
// Name:        constrnt.h
// Purpose:     OGL constraint definitions
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_CONSTRNT_H_
#define _OGL_CONSTRNT_H_

#ifdef __GNUG__
#pragma interface "constrnt.h"
#endif

/*
 * OGL Constraints
 *
 */

class wxOGLConstraintType: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxOGLConstraintType)
public:
  wxOGLConstraintType(int type = 0, const wxString& name = "", const wxString& phrase = "");
  ~wxOGLConstraintType();

public:
  int           m_type;     // E.g. gyCONSTRAINT_CENTRED_VERTICALLY
  wxString      m_name;     // E.g. "Centre vertically"
  wxString      m_phrase;   // E.g. "centred vertically with respect to", "left of"

};

extern wxList* wxOGLConstraintTypes;

#define gyCONSTRAINT_CENTRED_VERTICALLY   1
#define gyCONSTRAINT_CENTRED_HORIZONTALLY 2
#define gyCONSTRAINT_CENTRED_BOTH         3
#define gyCONSTRAINT_LEFT_OF              4
#define gyCONSTRAINT_RIGHT_OF             5
#define gyCONSTRAINT_ABOVE                6
#define gyCONSTRAINT_BELOW                7
#define gyCONSTRAINT_ALIGNED_TOP          8
#define gyCONSTRAINT_ALIGNED_BOTTOM       9
#define gyCONSTRAINT_ALIGNED_LEFT         10
#define gyCONSTRAINT_ALIGNED_RIGHT        11

// Like aligned, but with the objects centred on the respective edge
// of the reference object.
#define gyCONSTRAINT_MIDALIGNED_TOP       12
#define gyCONSTRAINT_MIDALIGNED_BOTTOM    13
#define gyCONSTRAINT_MIDALIGNED_LEFT      14
#define gyCONSTRAINT_MIDALIGNED_RIGHT     15

class wxOGLConstraint: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxOGLConstraint)
 public:
  wxOGLConstraint() { m_xSpacing = 0.0; m_ySpacing = 0.0; m_constraintType = 0; m_constraintName = ""; m_constraintId = 0;
                        m_constrainingObject = NULL; }
  wxOGLConstraint(int type, wxShape *constraining, wxList& constrained);
  ~wxOGLConstraint();

  // Returns TRUE if anything changed
  bool Evaluate();
  inline void SetSpacing(double x, double y) { m_xSpacing = x; m_ySpacing = y; };
  bool Equals(double a, double b);

  double         m_xSpacing;
  double         m_ySpacing;
  int           m_constraintType;
  wxString      m_constraintName;
  long          m_constraintId;
  wxShape*      m_constrainingObject;
  wxList        m_constrainedObjects;

};

void OGLInitializeConstraintTypes();
void OGLCleanUpConstraintTypes();

#endif
 // _OGL_CONSTRNT_H_
