/////////////////////////////////////////////////////////////////////////////
// Name:        divided.h
// Purpose:     wxDividedShape
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_DIVIDED_H_
#define _OGL_DIVIDED_H_

#ifdef __GNUG__
#pragma interface "basic.h"
#endif

/*
 * Definition of a region
 *
 */

/*
 * Box divided into horizontal regions
 *
 */

extern wxFont *g_oglNormalFont;
class wxDividedShape: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxDividedShape)

 public:
  wxDividedShape(double w = 0.0, double h = 0.0);
  ~wxDividedShape();

  void OnDraw(wxDC& dc);
  void OnDrawContents(wxDC& dc);

  void SetSize(double w, double h, bool recursive = TRUE);

  void MakeControlPoints();
  void ResetControlPoints();

  void MakeMandatoryControlPoints();
  void ResetMandatoryControlPoints();

#ifdef PROLOGIO
  void WriteAttributes(wxExpr *clause);
  void ReadAttributes(wxExpr *clause);
#endif

  void Copy(wxShape &copy);

  // Set all region sizes according to proportions and
  // this object total size
  void SetRegionSizes();

  // Edit region colours/styles
  void EditRegions();

  // Attachment points correspond to regions in the divided box
  bool GetAttachmentPosition(int attachment, double *x, double *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  bool AttachmentIsValid(int attachment);
  int GetNumberOfAttachments() const;

  // Invoke editor on CTRL-right click
  void OnRightClick(double x, double y, int keys = 0, int attachment = 0);
};

#endif
    // _OGL_DIVIDED_H_

