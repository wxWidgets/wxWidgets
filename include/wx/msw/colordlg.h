/////////////////////////////////////////////////////////////////////////////
// Name:        colordlg.h
// Purpose:     wxColourDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __COLORDLGH__
#define __COLORDLGH__

#ifdef __GNUG__
#pragma interface "colordlg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"
#include "wx/cmndata.h"

/*
 * COLOUR DIALOG
 */

class WXDLLEXPORT wxColourDialog: public wxDialog
{
 DECLARE_DYNAMIC_CLASS(wxColourDialog)
 protected:
  wxColourData colourData;
  wxWindow *dialogParent;
 public:
  wxColourDialog(void);
  wxColourDialog(wxWindow *parent, wxColourData *data = NULL);

  bool Create(wxWindow *parent, wxColourData *data = NULL);

  int ShowModal(void);
  wxColourData& GetColourData(void) { return colourData; }
};

#endif
    // __COLORDLGH__
